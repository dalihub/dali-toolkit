/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/animated-image/fixed-image-cache.h>

// INTERNAL HEADERS
#include <dali-toolkit/internal/visuals/image/image-atlas-manager.h> // For ImageAtlasManagerPtr

// EXTERNAL HEADERS
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
constexpr bool     ENABLE_ORIENTATION_CORRECTION(true);
constexpr uint32_t FIRST_FRAME_INDEX = 0u;
} // namespace

FixedImageCache::FixedImageCache(TextureManager&                     textureManager,
                                 ImageDimensions                     size,
                                 Dali::FittingMode::Type             fittingMode,
                                 Dali::SamplingMode::Type            samplingMode,
                                 UrlList&                            urlList,
                                 TextureManager::MaskingDataPointer& maskingData,
                                 ImageCache::FrameReadyObserver&     observer,
                                 uint32_t                            batchSize,
                                 uint32_t                            interval,
                                 bool                                preMultiplyOnLoad)
: ImageCache(textureManager, size, fittingMode, samplingMode, maskingData, observer, batchSize, interval, preMultiplyOnLoad),
  mImageUrls(urlList),
  mCurrentFrameIndex(FIRST_FRAME_INDEX)
{
  mReadyFlags.reserve(mImageUrls.size());
}

FixedImageCache::~FixedImageCache()
{
  ClearCache();
}

TextureSet FixedImageCache::Frame(uint32_t frameIndex)
{
  TextureSet textureSet;
  if(frameIndex >= mImageUrls.size())
  {
    DALI_LOG_ERROR("Wrong frameIndex requested.\n");
    return textureSet;
  }

  mCurrentFrameIndex = frameIndex;

  bool batchRequested = false;

  // Make ensure that current frameIndex load requested.
  while(mReadyFlags.size() <= frameIndex)
  {
    batchRequested = true;
    LoadBatch();
  }

  // Request batch only 1 times for this function.
  if(!batchRequested && mReadyFlags.size() < mImageUrls.size())
  {
    LoadBatch();
  }

  if(IsFrameReady(mCurrentFrameIndex) && mLoadState != TextureManager::LoadState::LOAD_FAILED)
  {
    textureSet = GetTextureSet(mCurrentFrameIndex);
  }

  return textureSet;
}

TextureSet FixedImageCache::FirstFrame()
{
  TextureSet textureSet = Frame(FIRST_FRAME_INDEX);

  return textureSet;
}

uint32_t FixedImageCache::GetFrameInterval(uint32_t frameIndex) const
{
  return mInterval;
}

int32_t FixedImageCache::GetCurrentFrameIndex() const
{
  return static_cast<int32_t>(mCurrentFrameIndex);
}

int32_t FixedImageCache::GetTotalFrameCount() const
{
  return mImageUrls.size();
}

bool FixedImageCache::IsFrameReady(uint32_t frameIndex) const
{
  return ((mReadyFlags.size() > 0) && (mReadyFlags[frameIndex] == true));
}

void FixedImageCache::LoadBatch()
{
  // Try and load up to mBatchSize images, until the cache is filled.
  // Once the cache is filled, no more images are loaded.
  for(unsigned int i = 0; i < mBatchSize && mReadyFlags.size() < mImageUrls.size(); ++i)
  {
    uint32_t   frameIndex = mReadyFlags.size();
    VisualUrl& url        = mImageUrls[frameIndex].mUrl;

    mReadyFlags.push_back(false);

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

    auto preMultiplyOnLoading = mPreMultiplyOnLoad ? TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD
                                                   : TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

    TextureManager::TextureId loadTextureId = TextureManager::INVALID_TEXTURE_ID;
    mTextureManager.LoadTexture(url, mDesiredSize, mFittingMode, mSamplingMode, mMaskingData, synchronousLoading, loadTextureId, textureRect, textureRectSize, atlasingStatus, loadingStatus, this, atlasObserver, imageAtlasManager, ENABLE_ORIENTATION_CORRECTION, TextureManager::ReloadPolicy::CACHED, preMultiplyOnLoading);
    mImageUrls[frameIndex].mTextureId = loadTextureId;
    mRequestingLoad                   = false;
  }
}

TextureSet FixedImageCache::GetTextureSet(uint32_t frameIndex) const
{
  TextureSet textureSet = mTextureManager.GetTextureSet(mImageUrls[frameIndex].mTextureId);
  return textureSet;
}

void FixedImageCache::MakeReady(bool wasReady, uint32_t frameIndex, bool preMultiplied)
{
  if(wasReady == false && IsFrameReady(frameIndex))
  {
    mObserver.FrameReady(GetTextureSet(frameIndex), mInterval, preMultiplied);
  }
}

void FixedImageCache::ClearCache()
{
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
  {
    for(std::size_t i = 0u; i < mImageUrls.size(); ++i)
    {
      mTextureManager.RequestRemove(mImageUrls[i].mTextureId, this);
      mImageUrls[i].mTextureId = TextureManager::INVALID_TEXTURE_ID;
    }
  }
  mReadyFlags.clear();
  mLoadState = TextureManager::LoadState::NOT_STARTED;
  if(mMaskingData)
  {
    mMaskingData->mAlphaMaskId = TextureManager::INVALID_TEXTURE_ID;
  }
}

void FixedImageCache::LoadComplete(bool loadSuccess, TextureInformation textureInformation)
{
  if(loadSuccess)
  {
    mLoadState                = TextureManager::LoadState::LOAD_FINISHED;
    bool wasCurrentFrameReady = IsFrameReady(mCurrentFrameIndex);
    if(!mRequestingLoad)
    {
      for(std::size_t i = 0; i < mImageUrls.size(); ++i)
      {
        if(mImageUrls[i].mTextureId == textureInformation.textureId)
        {
          mReadyFlags[i] = true;
          break;
        }
      }
    }
    else
    {
      DALI_ASSERT_ALWAYS(mReadyFlags.size() > 0u && "Some FixedImageCache::LoadBatch() called mismatched!");
      size_t i = mReadyFlags.size() - 1u;

      // texture id might not setup yet. Update it now.
      mImageUrls[i].mTextureId = textureInformation.textureId;
      mReadyFlags[i]           = true;
    }
    MakeReady(wasCurrentFrameReady, mCurrentFrameIndex, textureInformation.preMultiplied);
  }
  else
  {
    mLoadState = TextureManager::LoadState::LOAD_FAILED;
    // preMultiplied should be false because broken image don't premultiply alpha on load
    mObserver.FrameReady(TextureSet(), 0, false);
  }
}

} //namespace Internal
} //namespace Toolkit
} //namespace Dali