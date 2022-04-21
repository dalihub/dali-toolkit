/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// CLASS HEADER
#include "rolling-animated-image-cache.h"

// INTERNAL HEADERS
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h> // For ImageAtlasManagerPtr
#include <dali/integration-api/debug.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gAnimImgLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_ANIMATED_IMAGE");

#define LOG_CACHE                                                                                                                 \
  {                                                                                                                               \
    std::ostringstream oss;                                                                                                       \
    oss << "Size:" << mQueue.Count() << " [ ";                                                                                    \
    for(std::size_t _i = 0; _i < mQueue.Count(); ++_i)                                                                            \
    {                                                                                                                             \
      oss << _i << "={ frm#: " << mQueue[_i].mFrameNumber << " tex: " << mImageUrls[mQueue[_i].mFrameNumber].mTextureId << "}, "; \
    }                                                                                                                             \
    oss << " ]" << std::endl;                                                                                                     \
    DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "%s", oss.str().c_str());                                                    \
  }

#else
#define LOG_CACHE
#endif

static constexpr bool ENABLE_ORIENTATION_CORRECTION(true);

} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
static constexpr uint32_t SINGLE_IMAGE_COUNT = 1u;
static constexpr uint32_t FIRST_FRAME_INDEX  = 0u;
} // namespace

RollingAnimatedImageCache::RollingAnimatedImageCache(TextureManager&                     textureManager,
                                                     AnimatedImageLoading&               animatedImageLoading,
                                                     TextureManager::MaskingDataPointer& maskingData,
                                                     ImageCache::FrameReadyObserver&     observer,
                                                     uint16_t                            cacheSize,
                                                     uint16_t                            batchSize,
                                                     bool                                isSynchronousLoading)
: ImageCache(textureManager, maskingData, observer, batchSize, 0u),
  mAnimatedImageLoading(animatedImageLoading),
  mFrameCount(SINGLE_IMAGE_COUNT),
  mFrameIndex(FIRST_FRAME_INDEX),
  mCacheSize(cacheSize),
  mQueue(cacheSize),
  mIsSynchronousLoading(isSynchronousLoading)
{
  mImageUrls.resize(mFrameCount);
  mIntervals.assign(mFrameCount, 0);
}

RollingAnimatedImageCache::~RollingAnimatedImageCache()
{
  ClearCache();
  mAnimatedImageLoading.Reset();
}

TextureSet RollingAnimatedImageCache::Frame(uint32_t frameIndex)
{
  bool popExist = false;
  while(!mQueue.IsEmpty() && mQueue.Front().mFrameNumber != frameIndex)
  {
    PopFrontCache();
    popExist = true;
  }

  TextureSet textureSet;
  uint32_t   batchFrameIndex = frameIndex;
  // If we need to load new frame that are not stored in queue.
  // Load the frame synchronously.
  bool synchronouslyLoaded = false;
  if(mIsSynchronousLoading && mQueue.IsEmpty())
  {
    textureSet        = RequestFrameLoading(frameIndex, true);
    batchFrameIndex   = (frameIndex + 1) % mFrameCount;
    uint32_t interval = 0u;
    if(textureSet)
    {
      synchronouslyLoaded = true;
      interval            = mAnimatedImageLoading.GetFrameInterval(mQueue.Back().mFrameNumber);
    }
    MakeFrameReady(synchronouslyLoaded, textureSet, interval);
  }

  if(popExist || mQueue.IsEmpty() || synchronouslyLoaded)
  {
    // If the frame of frameIndex was already loaded, load batch from the last frame of queue
    if(!mQueue.IsEmpty())
    {
      if(!mLoadWaitingQueue.empty())
      {
        batchFrameIndex = (mLoadWaitingQueue.back() + 1) % mFrameCount;
      }
      else
      {
        batchFrameIndex = (mQueue.Back().mFrameNumber + 1) % mFrameCount;
      }
    }
    else
    {
      // If the request is for the first frame or a jumped frame(JUMP_TO) remove current waiting queue.
      mLoadWaitingQueue.clear();
      // If the queue is empty, and the frame of frameIndex is not loaded synchronously. load batch from the frame of frameIndex
      if(!textureSet)
      {
        batchFrameIndex = frameIndex;
      }
    }
    LoadBatch(batchFrameIndex);
  }

  if(!textureSet && mLoadState != TextureManager::LoadState::LOAD_FAILED && IsFrontReady() == true)
  {
    textureSet = GetFrontTextureSet();
  }

  return textureSet;
}

TextureSet RollingAnimatedImageCache::FirstFrame()
{
  TextureSet textureSet = Frame(FIRST_FRAME_INDEX);
  return textureSet;
}

uint32_t RollingAnimatedImageCache::GetFrameInterval(uint32_t frameIndex) const
{
  if(frameIndex >= mIntervals.size())
  {
    return 0u;
  }
  return mIntervals[frameIndex];
}

int32_t RollingAnimatedImageCache::GetCurrentFrameIndex() const
{
  if(mQueue.IsEmpty())
  {
    return -1;
  }
  return mQueue.Front().mFrameNumber;
}

int32_t RollingAnimatedImageCache::GetTotalFrameCount() const
{
  return mFrameCount;
}

bool RollingAnimatedImageCache::IsFrontReady() const
{
  return (!mQueue.IsEmpty() && mQueue.Front().mReady);
}

TextureSet RollingAnimatedImageCache::RequestFrameLoading(uint32_t frameIndex, bool synchronousLoading)
{
  ImageFrame imageFrame;
  imageFrame.mFrameNumber = frameIndex;
  imageFrame.mReady       = false;

  mQueue.PushBack(imageFrame);

  mLoadState = TextureManager::LoadState::LOADING;

  TextureManager::TextureId loadTextureId = TextureManager::INVALID_TEXTURE_ID;
  TextureSet                textureSet    = mTextureManager.LoadAnimatedImageTexture(mAnimatedImageLoading,
                                                                   frameIndex,
                                                                   loadTextureId,
                                                                   mMaskingData,
                                                                   SamplingMode::BOX_THEN_LINEAR,
                                                                   Dali::WrapMode::Type::DEFAULT,
                                                                   Dali::WrapMode::Type::DEFAULT,
                                                                   synchronousLoading,
                                                                   this);

  mImageUrls[frameIndex].mTextureId = loadTextureId;

  return textureSet;
}

void RollingAnimatedImageCache::LoadBatch(uint32_t frameIndex)
{
  // Try and load up to mBatchSize images, until the cache is filled.
  // Once the cache is filled, as frames progress, the old frame is
  // removed, and another frame is loaded
  uint32_t minimumSize = std::min(mCacheSize, mFrameCount);
  for(uint32_t i = 0; i < mBatchSize && (mQueue.Count() + mLoadWaitingQueue.size()) < minimumSize; ++i)
  {
    if(mLoadState != TextureManager::LoadState::LOADING)
    {
      RequestFrameLoading(frameIndex, false);
    }
    else
    {
      mLoadWaitingQueue.push_back(frameIndex);
    }

    frameIndex++;
    frameIndex %= mFrameCount;
  }

  LOG_CACHE;
}

void RollingAnimatedImageCache::SetImageFrameReady(TextureManager::TextureId textureId)
{
  for(std::size_t i = 0; i < mQueue.Count(); ++i)
  {
    if(GetCachedTextureId(i) == textureId)
    {
      mQueue[i].mReady = true;
      break;
    }
  }
}

TextureSet RollingAnimatedImageCache::GetFrontTextureSet() const
{
  DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "RollingAnimatedImageCache::GetFrontTextureSet() FrameNumber:%d\n", mQueue[0].mFrameNumber);

  TextureManager::TextureId textureId = GetCachedTextureId(0);
  return mTextureManager.GetTextureSet(textureId);
}

TextureManager::TextureId RollingAnimatedImageCache::GetCachedTextureId(int index) const
{
  return mImageUrls[mQueue[index].mFrameNumber].mTextureId;
}

void RollingAnimatedImageCache::PopFrontCache()
{
  ImageFrame imageFrame = mQueue.PopFront();
  mTextureManager.Remove(mImageUrls[imageFrame.mFrameNumber].mTextureId, this);
  mImageUrls[imageFrame.mFrameNumber].mTextureId = TextureManager::INVALID_TEXTURE_ID;

  if(mMaskingData && mMaskingData->mAlphaMaskId != TextureManager::INVALID_TEXTURE_ID)
  {
    mTextureManager.Remove(mMaskingData->mAlphaMaskId, this);
    if(mQueue.IsEmpty())
    {
      mMaskingData->mAlphaMaskId = TextureManager::INVALID_TEXTURE_ID;
    }
  }
}

void RollingAnimatedImageCache::ClearCache()
{
  while(mTextureManagerAlive && !mQueue.IsEmpty())
  {
    PopFrontCache();
  }
  mLoadWaitingQueue.clear();
  mLoadState = TextureManager::LoadState::NOT_STARTED;
}

void RollingAnimatedImageCache::MakeFrameReady(bool loadSuccess, TextureSet textureSet, uint32_t interval)
{
  if(!loadSuccess)
  {
    mLoadState = TextureManager::LoadState::LOAD_FAILED;
    mObserver.FrameReady(TextureSet(), 0);
  }
  else
  {
    mLoadState = TextureManager::LoadState::LOAD_FINISHED;

    // Reset size of Queue according to the real frame count.
    if(mFrameCount != mAnimatedImageLoading.GetImageCount())
    {
      mFrameCount = mAnimatedImageLoading.GetImageCount();
      mImageUrls.resize(mFrameCount);
      mIntervals.assign(mFrameCount, 0u);
    }

    bool frontFrameReady = IsFrontReady();
    // Because only one frame is on loading and the others are in mLoadWaitingQueue,
    // mQueue.Back() is always the frame currently loaded.
    mQueue.Back().mReady                   = true;
    mIntervals[mQueue.Back().mFrameNumber] = interval;
    // Check whether currently loaded frame is front of queue or not.
    // If it is, notify frame ready to observer.
    if(frontFrameReady == false && IsFrontReady())
    {
      mObserver.FrameReady(textureSet, interval);
    }
  }
}

void RollingAnimatedImageCache::LoadComplete(bool loadSuccess, TextureInformation textureInformation)
{
  DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "AnimatedImageVisual::LoadComplete(textureId:%d) start\n", textureInformation.textureId);
  LOG_CACHE;

  MakeFrameReady(loadSuccess, mTextureManager.GetTextureSet(textureInformation.textureId), textureInformation.interval);

  if(loadSuccess)
  {
    // The frames of a single animated image can not be loaded parallelly.
    // Therefore, a frame is now loading, other orders are waiting.
    // And, after the frame is loaded, requests load of next order.
    if(!mLoadWaitingQueue.empty())
    {
      uint32_t loadingIndex = mLoadWaitingQueue.front();
      mLoadWaitingQueue.erase(mLoadWaitingQueue.begin());
      RequestFrameLoading(loadingIndex, false);
    }
    else if(mQueue.Count() == 1u && textureInformation.frameCount > SINGLE_IMAGE_COUNT)
    {
      // There is only an image in queue and no waiting queue.
      // Request to load batch once again.
      uint32_t batchFrameIndex = 0u;
      if(!mLoadWaitingQueue.empty())
      {
        batchFrameIndex = (mLoadWaitingQueue.back() + 1) % mFrameCount;
      }
      else
      {
        batchFrameIndex = (mQueue.Back().mFrameNumber + 1) % mFrameCount;
      }
      LoadBatch(batchFrameIndex);
    }
  }

  LOG_CACHE;
}

} //namespace Internal
} //namespace Toolkit
} //namespace Dali
