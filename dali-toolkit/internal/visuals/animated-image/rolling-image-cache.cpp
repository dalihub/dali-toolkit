/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/animated-image/rolling-image-cache.h>

// INTERNAL HEADERS
#include <dali-toolkit/internal/visuals/image-atlas-manager.h> // For ImageAtlasManagerPtr
#include <dali/integration-api/debug.h>

// EXTERNAL HEADERS

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gAnimImgLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_ANIMATED_IMAGE");

#define LOG_CACHE                                                                                                                                                        \
  {                                                                                                                                                                      \
    std::ostringstream oss;                                                                                                                                              \
    oss << "Size:" << mQueue.Count() << " [ ";                                                                                                                           \
    for(std::size_t _i = 0; _i < mQueue.Count(); ++_i)                                                                                                                   \
    {                                                                                                                                                                    \
      oss << _i << "={ tex:" << mImageUrls[mQueue[_i].mUrlIndex].mTextureId << " urlId:" << mQueue[_i].mUrlIndex << " rdy:" << (mQueue[_i].mReady ? "T" : "F") << "}, "; \
    }                                                                                                                                                                    \
    oss << " ]" << std::endl;                                                                                                                                            \
    DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "%s", oss.str().c_str());                                                                                           \
  }

#else
#define LOG_CACHE
#endif

static constexpr bool ENABLE_ORIENTATION_CORRECTION(true);

static constexpr uint32_t FIRST_FRAME_INDEX = 0u;

} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
RollingImageCache::RollingImageCache(
  TextureManager& textureManager, UrlList& urlList, ImageCache::FrameReadyObserver& observer, uint16_t cacheSize, uint16_t batchSize, uint32_t interval)
: ImageCache(textureManager, observer, batchSize, interval),
  mImageUrls(urlList),
  mQueue(cacheSize)
{
  mLoadStates.assign(mImageUrls.size(), TextureManager::LoadState::NOT_STARTED);
}

RollingImageCache::~RollingImageCache()
{
  ClearCache();
}

TextureSet RollingImageCache::Frame(uint32_t frameIndex)
{
  // Pop frames until the frame of frameIndex become front frame.
  bool popExist = false;
  while(!mQueue.IsEmpty() && mQueue.Front().mUrlIndex != frameIndex)
  {
    ImageFrame imageFrame = mQueue.PopFront();
    mTextureManager.Remove(mImageUrls[imageFrame.mUrlIndex].mTextureId, this);
    mImageUrls[imageFrame.mUrlIndex].mTextureId = TextureManager::INVALID_TEXTURE_ID;
    popExist                                    = true;
  }
  if(popExist || mQueue.IsEmpty())
  {
    uint32_t batchFrameIndex = frameIndex;
    // If the frame of frameIndex was already loaded, load batch from the last frame of queue
    if(!mQueue.IsEmpty())
    {
      batchFrameIndex = (mQueue.Back().mUrlIndex + 1) % mImageUrls.size();
    }
    LoadBatch(batchFrameIndex);
  }

  TextureSet textureSet;
  if(IsFrontReady() == true)
  {
    textureSet = GetFrontTextureSet();
  }

  return textureSet;
}

TextureSet RollingImageCache::FirstFrame()
{
  TextureSet textureSet = Frame(FIRST_FRAME_INDEX);
  return textureSet;
}

uint32_t RollingImageCache::GetFrameInterval(uint32_t frameIndex) const
{
  return mInterval;
}

int32_t RollingImageCache::GetCurrentFrameIndex() const
{
  if(mQueue.IsEmpty())
  {
    return -1;
  }
  return mQueue.Front().mUrlIndex;
}

int32_t RollingImageCache::GetTotalFrameCount() const
{
  return mImageUrls.size();
}

TextureManager::LoadState RollingImageCache::GetLoadState() const
{
  return mLoadStates[mQueue.Front().mUrlIndex];
}

bool RollingImageCache::IsFrontReady() const
{
  return (!mQueue.IsEmpty() && mQueue.Front().mReady);
}

void RollingImageCache::LoadBatch(uint32_t frameIndex)
{
  // Try and load up to mBatchSize images, until the cache is filled.
  // Once the cache is filled, as frames progress, the old frame is
  // cleared, but not erased, and another image is loaded
  for(unsigned int i = 0; i < mBatchSize && !mQueue.IsFull(); ++i)
  {
    ImageFrame imageFrame;

    std::string& url     = mImageUrls[frameIndex].mUrl;
    imageFrame.mUrlIndex = frameIndex;
    imageFrame.mReady    = false;

    mQueue.PushBack(imageFrame);

    // Note, if the image is already loaded, then UploadComplete will get called
    // from within this method. This means it won't yet have a texture id, so we
    // need to account for this inside the UploadComplete method using mRequestingLoad.
    mRequestingLoad = true;

    bool                               synchronousLoading = false;
    bool                               atlasingStatus     = false;
    bool                               loadingStatus      = false;
    TextureManager::MaskingDataPointer maskInfo           = nullptr;
    AtlasUploadObserver*               atlasObserver      = nullptr;
    ImageAtlasManagerPtr               imageAtlasManager  = nullptr;
    Vector4                            textureRect;
    Dali::ImageDimensions              textureRectSize;
    auto                               preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

    TextureSet textureSet = mTextureManager.LoadTexture(
      url, ImageDimensions(), FittingMode::SCALE_TO_FILL, SamplingMode::BOX_THEN_LINEAR, maskInfo, synchronousLoading, mImageUrls[imageFrame.mUrlIndex].mTextureId, textureRect, textureRectSize, atlasingStatus, loadingStatus, Dali::WrapMode::Type::DEFAULT, Dali::WrapMode::Type::DEFAULT, this, atlasObserver, imageAtlasManager, ENABLE_ORIENTATION_CORRECTION, TextureManager::ReloadPolicy::CACHED, preMultiply);

    // If textureSet is returned but loadingState is false than load state is LOAD_FINISHED. (Notification is not comming yet.)
    // If textureSet is null and the request is synchronous, load state is LOAD_FAILED.
    // If textureSet is null but the request is asynchronous, the frame is still loading so load state is LOADING.
    mLoadStates[frameIndex] = TextureManager::LoadState::LOADING;
    if(textureSet)
    {
      if(!loadingStatus)
      {
        mLoadStates[frameIndex] = TextureManager::LoadState::LOAD_FINISHED;
      }
    }
    else if(synchronousLoading)
    {
      // Synchronous loading is failed
      mLoadStates[frameIndex] = TextureManager::LoadState::LOAD_FAILED;
    }

    mRequestingLoad = false;

    ++frameIndex;
    frameIndex %= mImageUrls.size();
  }
}

void RollingImageCache::SetImageFrameReady(TextureManager::TextureId textureId, bool loadSuccess)
{
  for(std::size_t i = 0; i < mQueue.Count(); ++i)
  {
    if(GetCachedTextureId(i) == textureId)
    {
      if(loadSuccess)
      {
        mLoadStates[i] = TextureManager::LoadState::LOAD_FINISHED;
      }
      else
      {
        mLoadStates[i] = TextureManager::LoadState::LOAD_FAILED;
      }
      mQueue[i].mReady = true;
      break;
    }
  }
}

TextureSet RollingImageCache::GetFrontTextureSet() const
{
  TextureManager::TextureId textureId = GetCachedTextureId(0);
  return mTextureManager.GetTextureSet(textureId);
}

TextureManager::TextureId RollingImageCache::GetCachedTextureId(int index) const
{
  return mImageUrls[mQueue[index].mUrlIndex].mTextureId;
}

void RollingImageCache::CheckFrontFrame(bool wasReady)
{
  if(wasReady == false && IsFrontReady())
  {
    mObserver.FrameReady(GetFrontTextureSet(), mInterval);
  }
}

void RollingImageCache::ClearCache()
{
  while(mTextureManagerAlive && !mQueue.IsEmpty())
  {
    ImageFrame imageFrame = mQueue.PopFront();
    mTextureManager.Remove(mImageUrls[imageFrame.mUrlIndex].mTextureId, this);
    mImageUrls[imageFrame.mUrlIndex].mTextureId = TextureManager::INVALID_TEXTURE_ID;
  }
  mLoadStates.assign(mImageUrls.size(), TextureManager::LoadState::NOT_STARTED);
}

void RollingImageCache::UploadComplete(
  bool           loadSuccess,
  int32_t        textureId,
  TextureSet     textureSet,
  bool           useAtlasing,
  const Vector4& atlasRect,
  bool           preMultiplied)
{
  DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "AnimatedImageVisual::UploadComplete(textureId:%d) start\n", textureId);
  LOG_CACHE;

  bool frontFrameReady = IsFrontReady();
  SetImageFrameReady(textureId, loadSuccess);
  if(!mRequestingLoad)
  {
    CheckFrontFrame(frontFrameReady);
  }

  LOG_CACHE;
}

} //namespace Internal
} //namespace Toolkit
} //namespace Dali
