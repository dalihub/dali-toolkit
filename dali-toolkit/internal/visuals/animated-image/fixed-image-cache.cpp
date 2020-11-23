/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

namespace
{
const bool ENABLE_ORIENTATION_CORRECTION( true );
} // namespace

FixedImageCache::FixedImageCache(
  TextureManager& textureManager, UrlList& urlList, ImageCache::FrameReadyObserver& observer,
  unsigned int batchSize )
: ImageCache( textureManager, observer, batchSize ),
  mImageUrls( urlList ),
  mFront(0u)
{
  mReadyFlags.reserve( mImageUrls.size() );
  LoadBatch();
}

FixedImageCache::~FixedImageCache()
{
  if( mTextureManagerAlive )
  {
    for( std::size_t i = 0; i < mImageUrls.size() ; ++i )
    {
      mTextureManager.Remove( mImageUrls[i].mTextureId, this );
    }
  }
}

TextureSet FixedImageCache::Frame( uint32_t frameIndex )
{
  while( frameIndex > mFront )
  {
    ++mFront;
    if( mFront >= mImageUrls.size() )
    {
      mFront = 0;
    }
    LoadBatch();
  }

  mFront = frameIndex;

  TextureSet textureSet;
  if( IsFrontReady() == true )
  {
    textureSet = GetFrontTextureSet();
  }
  else
  {
    mWaitingForReadyFrame = true;
  }

  return textureSet;
}

TextureSet FixedImageCache::FirstFrame()
{
  TextureSet textureSet = GetFrontTextureSet();

  if( ! textureSet )
  {
    mWaitingForReadyFrame = true;
  }

  return textureSet;
}

TextureSet FixedImageCache::NextFrame()
{
  TextureSet textureSet = Frame((mFront + 1) % mImageUrls.size());

  return textureSet;
}

uint32_t FixedImageCache::GetFrameInterval( uint32_t frameIndex )
{
  return 0u;
}

int32_t FixedImageCache::GetCurrentFrameIndex()
{
  return static_cast<int32_t>(mFront);
}

bool FixedImageCache::IsFrontReady() const
{
  return ( mReadyFlags.size() > 0 && mReadyFlags[mFront] == true );
}

void FixedImageCache::LoadBatch()
{
  // Try and load up to mBatchSize images, until the cache is filled.
  // Once the cache is filled, mUrlIndex exceeds mImageUrls size and
  // no more images are loaded.
  bool frontFrameReady = IsFrontReady();

  for( unsigned int i=0; i< mBatchSize && mUrlIndex < mImageUrls.size(); ++i )
  {
    std::string& url = mImageUrls[ mUrlIndex ].mUrl;

    mReadyFlags.push_back(false);

    // Note, if the image is already loaded, then UploadComplete will get called
    // from within this method. This means it won't yet have a texture id, so we
    // need to account for this inside the UploadComplete method using mRequestingLoad.
    mRequestingLoad = true;

    bool synchronousLoading = false;
    bool atlasingStatus = false;
    bool loadingStatus = false;
    TextureManager::MaskingDataPointer maskInfo = nullptr;
    AtlasUploadObserver* atlasObserver = nullptr;
    ImageAtlasManagerPtr imageAtlasManager = nullptr;
    Vector4 textureRect;
    Dali::ImageDimensions textureRectSize;
    auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

    mTextureManager.LoadTexture(
      url, ImageDimensions(), FittingMode::SCALE_TO_FILL,
      SamplingMode::BOX_THEN_LINEAR, maskInfo,
      synchronousLoading, mImageUrls[ mUrlIndex ].mTextureId, textureRect, textureRectSize,
      atlasingStatus, loadingStatus, Dali::WrapMode::Type::DEFAULT,
      Dali::WrapMode::Type::DEFAULT, this,
      atlasObserver, imageAtlasManager, ENABLE_ORIENTATION_CORRECTION, TextureManager::ReloadPolicy::CACHED,
      preMultiply );

    if( loadingStatus == false )  // not loading, means it's already ready.
    {
      SetImageFrameReady( mImageUrls[ mUrlIndex ].mTextureId );
    }
    mRequestingLoad = false;
    ++mUrlIndex;
  }

  CheckFrontFrame( frontFrameReady );
}

void FixedImageCache::SetImageFrameReady( TextureManager::TextureId textureId )
{
  for( std::size_t i = 0; i < mImageUrls.size() ; ++i )
  {
    if( mImageUrls[i].mTextureId == textureId )
    {
      mReadyFlags[i] = true;
      break;
    }
  }
}

TextureSet FixedImageCache::GetFrontTextureSet() const
{
  return mTextureManager.GetTextureSet( mImageUrls[mFront].mTextureId );
}

void FixedImageCache::CheckFrontFrame( bool wasReady )
{
  if( mWaitingForReadyFrame && wasReady == false && IsFrontReady() )
  {
    mWaitingForReadyFrame = false;
    mObserver.FrameReady( GetFrontTextureSet() );
  }
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

  if( ! mRequestingLoad )
  {
    SetImageFrameReady( textureId );

    CheckFrontFrame( frontFrameReady );
  }
  else
  {
    // UploadComplete has been called from within RequestLoad. TextureManager must
    // therefore already have the texture cached, so make the texture ready.
    // (Use the last texture, as the texture id hasn't been assigned yet)
    mReadyFlags.back() = true;
  }
}

void FixedImageCache::LoadComplete(
  bool loadSuccess,
  Devel::PixelBuffer pixelBuffer,
  const VisualUrl& url,
  bool preMultiplied )
{
  // LoadComplete is called if this TextureUploadObserver requested to load
  // an image that will be returned as a type of PixelBuffer by using a method
  // TextureManager::LoadPixelBuffer.
}

} //namespace Internal
} //namespace Toolkit
} //namespace Dali