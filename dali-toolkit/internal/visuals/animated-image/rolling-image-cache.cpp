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
  if(gAnimImgLogFilter->IsEnabledFor(Debug::Concise))                                                                                                                    \
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
RollingImageCache::RollingImageCache(TextureManager&                     textureManager,
                                     ImageDimensions                     size,
                                     Dali::FittingMode::Type             fittingMode,
                                     Dali::SamplingMode::Type            samplingMode,
                                     UrlList&                            urlList,
                                     TextureManager::MaskingDataPointer& maskingData,
                                     ImageCache::FrameReadyObserver&     observer,
                                     uint16_t                            cacheSize,
                                     uint16_t                            batchSize,
                                     uint32_t                            interval)
: ImageCache(textureManager, size, fittingMode, samplingMode, maskingData, observer, batchSize, interval),
  mImageUrls(urlList),
  mQueue(cacheSize)
{
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
    PopFrontCache();
    popExist = true;
  }

  // TODO: synchronous loading of first frame.
  if(popExist || mQueue.IsEmpty())
  {
    uint32_t batchFrameIndex = frameIndex;
    // If the frame of frameIndex was already loaded, load batch from the last frame of queue
    if(!mQueue.IsEmpty())
    {
      batchFrameIndex = (mQueue.Back().mUrlIndex + 1) % static_cast<uint32_t>(mImageUrls.size());
    }
    LoadBatch(batchFrameIndex);
  }

  TextureSet textureSet;
  if(IsFrontReady() == true && mLoadState != TextureManager::LoadState::LOAD_FAILED)
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

    VisualUrl& url       = mImageUrls[frameIndex].mUrl;
    imageFrame.mUrlIndex = frameIndex;
    imageFrame.mReady    = false;

    mQueue.PushBack(imageFrame);

    // Note, if the image is already loaded, then LoadComplete will get called
    // from within this method. This means it won't yet have a texture id, so we
    // need to account for this inside the LoadComplete method using mRequestingLoad.
    mRequestingLoad = true;
    mLoadState      = TextureManager::LoadState::LOADING;

    bool                  synchronousLoading = false;
    bool                  atlasingStatus     = false;
    bool                  loadingStatus      = false;
    AtlasUploadObserver*  atlasObserver      = nullptr;
    ImageAtlasManagerPtr  imageAtlasManager  = nullptr;
    Vector4               textureRect;
    Dali::ImageDimensions textureRectSize;
    auto                  preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

    TextureManager::TextureId loadTextureId = TextureManager::INVALID_TEXTURE_ID;
    TextureSet                textureSet    = mTextureManager.LoadTexture(
      url, mDesiredSize, mFittingMode, mSamplingMode, mMaskingData, synchronousLoading, loadTextureId, textureRect, textureRectSize, atlasingStatus, loadingStatus, this, atlasObserver, imageAtlasManager, ENABLE_ORIENTATION_CORRECTION, TextureManager::ReloadPolicy::CACHED, preMultiply);
    mImageUrls[imageFrame.mUrlIndex].mTextureId = loadTextureId;

    mRequestingLoad = false;

    ++frameIndex;
    frameIndex %= mImageUrls.size();
  }
}

TextureSet RollingImageCache::GetFrontTextureSet() const
{
  TextureManager::TextureId textureId  = GetCachedTextureId(0);
  TextureSet                textureSet = mTextureManager.GetTextureSet(textureId);
  if(textureSet)
  {
    Sampler sampler = Sampler::New();
    sampler.SetWrapMode(Dali::WrapMode::Type::DEFAULT, Dali::WrapMode::Type::DEFAULT);
    textureSet.SetSampler(0u, sampler);
  }
  return textureSet;
}

TextureManager::TextureId RollingImageCache::GetCachedTextureId(int index) const
{
  return mImageUrls[mQueue[index].mUrlIndex].mTextureId;
}

void RollingImageCache::PopFrontCache()
{
  ImageFrame imageFrame = mQueue.PopFront();
  mTextureManager.Remove(mImageUrls[imageFrame.mUrlIndex].mTextureId, this);
  mImageUrls[imageFrame.mUrlIndex].mTextureId = TextureManager::INVALID_TEXTURE_ID;

  if(mMaskingData && mMaskingData->mAlphaMaskId != TextureManager::INVALID_TEXTURE_ID)
  {
    if(mQueue.IsEmpty())
    {
      mMaskingData->mAlphaMaskId = TextureManager::INVALID_TEXTURE_ID;
    }
  }
}

void RollingImageCache::ClearCache()
{
  while(mTextureManagerAlive && !mQueue.IsEmpty())
  {
    PopFrontCache();
  }
  mLoadState = TextureManager::LoadState::NOT_STARTED;
}

void RollingImageCache::LoadComplete(bool loadSuccess, TextureInformation textureInformation)
{
  DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "AnimatedImageVisual::LoadComplete(textureId:%d) start\n", textureInformation.textureId);
  LOG_CACHE;

  if(loadSuccess)
  {
    mLoadState           = TextureManager::LoadState::LOAD_FINISHED;
    bool frontFrameReady = IsFrontReady();
    if(!mRequestingLoad)
    {
      for(std::size_t i = 0; i < mQueue.Count(); ++i)
      {
        if(GetCachedTextureId(i) == textureInformation.textureId)
        {
          mQueue[i].mReady = true;
          break;
        }
      }
    }
    else
    {
      // LoadComplete has been called from within RequestLoad. TextureManager must
      // therefore already have the texture cached, so make the texture ready.
      // (Use the last texture, as the texture id hasn't been assigned yet)
      mQueue.Back().mReady = true;
    }

    if(!frontFrameReady && IsFrontReady())
    {
      if(textureInformation.textureSet)
      {
        Sampler sampler = Sampler::New();
        sampler.SetWrapMode(Dali::WrapMode::Type::DEFAULT, Dali::WrapMode::Type::DEFAULT);
        textureInformation.textureSet.SetSampler(0u, sampler);
      }
      mObserver.FrameReady(textureInformation.textureSet, mInterval);
    }
  }
  else
  {
    mLoadState = TextureManager::LoadState::LOAD_FAILED;
    mObserver.FrameReady(TextureSet(), 0);
  }

  LOG_CACHE;
}

} //namespace Internal
} //namespace Toolkit
} //namespace Dali
