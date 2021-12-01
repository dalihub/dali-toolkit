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
#include <dali-toolkit/internal/visuals/animated-image/fixed-image-cache.h>

// INTERNAL HEADERS
#include <dali-toolkit/internal/visuals/image-atlas-manager.h> // For ImageAtlasManagerPtr

// EXTERNAL HEADERS
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
static constexpr bool ENABLE_ORIENTATION_CORRECTION(true);

static constexpr uint32_t FIRST_FRAME_INDEX = 0u;
} // namespace

FixedImageCache::FixedImageCache(
  TextureManager& textureManager, UrlList& urlList, ImageCache::FrameReadyObserver& observer, uint32_t batchSize, uint32_t interval)
: ImageCache(textureManager, observer, batchSize, interval),
  mImageUrls(urlList),
  mFront(FIRST_FRAME_INDEX)
{
  mLoadStates.assign(mImageUrls.size(), TextureManager::LoadState::NOT_STARTED);
  mReadyFlags.reserve(mImageUrls.size());
}

FixedImageCache::~FixedImageCache()
{
  ClearCache();
}

TextureSet FixedImageCache::Frame(uint32_t frameIndex)
{
  while(frameIndex > mFront || mReadyFlags.empty() ||
        (frameIndex == FIRST_FRAME_INDEX && mFront != FIRST_FRAME_INDEX))
  {
    ++mFront;
    if(mFront >= mImageUrls.size())
    {
      mFront = 0;
    }
    LoadBatch();
  }

  mFront = frameIndex;

  TextureSet textureSet;
  if(IsFrontReady())
  {
    textureSet = GetFrontTextureSet();
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
  return static_cast<int32_t>(mFront);
}

int32_t FixedImageCache::GetTotalFrameCount() const
{
  return mImageUrls.size();
}

TextureManager::LoadState FixedImageCache::GetLoadState() const
{
  return mLoadStates[mFront];
}

bool FixedImageCache::IsFrontReady() const
{
  return (mReadyFlags.size() > 0 && mReadyFlags[mFront] == true);
}

void FixedImageCache::LoadBatch()
{
  // Try and load up to mBatchSize images, until the cache is filled.
  // Once the cache is filled, no more images are loaded.
  for(unsigned int i = 0; i < mBatchSize && mReadyFlags.size() < mImageUrls.size(); ++i)
  {
    uint32_t frameIndex = mReadyFlags.size();
    std::string& url = mImageUrls[frameIndex].mUrl;

    mReadyFlags.push_back(false);

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
      url, ImageDimensions(), FittingMode::SCALE_TO_FILL, SamplingMode::BOX_THEN_LINEAR, maskInfo, synchronousLoading, mImageUrls[frameIndex].mTextureId, textureRect, textureRectSize, atlasingStatus, loadingStatus, Dali::WrapMode::Type::DEFAULT, Dali::WrapMode::Type::DEFAULT, this, atlasObserver, imageAtlasManager, ENABLE_ORIENTATION_CORRECTION, TextureManager::ReloadPolicy::CACHED, preMultiply);

    // If textureSet is returned but loadingState is false than load state is LOAD_FINISHED. (Notification is not comming yet.)
    // If textureSet is null and the request is synchronous, load state is LOAD_FAILED.
    // If textureSet is null but the request is asynchronous, the frame is still loading so load state is LOADING.
    mLoadStates[frameIndex] = TextureManager::LoadState::LOADING;
    if(textureSet)
    {
      if(!loadingStatus)
      {
        SetImageFrameReady(mImageUrls[frameIndex].mTextureId, true);
      }
    }
    else if(synchronousLoading)
    {
      // Synchronous loading is failed
      mLoadStates[frameIndex] = TextureManager::LoadState::LOAD_FAILED;
    }

    mRequestingLoad = false;
  }
}

void FixedImageCache::SetImageFrameReady(TextureManager::TextureId textureId, bool loadSuccess)
{
  for(std::size_t i = 0; i < mImageUrls.size(); ++i)
  {
    if(mImageUrls[i].mTextureId == textureId)
    {
      if(loadSuccess)
      {
        mLoadStates[i] = TextureManager::LoadState::LOAD_FINISHED;
      }
      else
      {
        mLoadStates[i] = TextureManager::LoadState::LOAD_FAILED;
      }
      mReadyFlags[i] = true;
      break;
    }
  }
}

TextureSet FixedImageCache::GetFrontTextureSet() const
{
  return mTextureManager.GetTextureSet(mImageUrls[mFront].mTextureId);
}

void FixedImageCache::CheckFrontFrame(bool wasReady)
{
  if(wasReady == false && IsFrontReady())
  {
    mObserver.FrameReady(GetFrontTextureSet(), mInterval);
  }
}

void FixedImageCache::ClearCache()
{
  if(mTextureManagerAlive)
  {
    for(std::size_t i = 0; i < mImageUrls.size(); ++i)
    {
      mTextureManager.Remove(mImageUrls[i].mTextureId, this);
      mImageUrls[i].mTextureId = TextureManager::INVALID_TEXTURE_ID;
    }
  }
  mReadyFlags.clear();
  mLoadStates.assign(mImageUrls.size(), TextureManager::LoadState::NOT_STARTED);
}

void FixedImageCache::UploadComplete(
  bool           loadSuccess,
  int32_t        textureId,
  TextureSet     textureSet,
  bool           useAtlasing,
  const Vector4& atlasRect,
  bool           preMultiplied)
{
  bool frontFrameReady = IsFrontReady();
  SetImageFrameReady(textureId, loadSuccess);
  if(!mRequestingLoad)
  {
    CheckFrontFrame(frontFrameReady);
  }
}

} //namespace Internal
} //namespace Toolkit
} //namespace Dali