/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include "rolling-gif-image-cache.h"

// EXTERNAL HEADERS

// INTERNAL HEADERS
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h> // For ImageAtlasManagerPtr
#include <dali/integration-api/debug.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gAnimImgLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_ANIMATED_IMAGE");

#define LOG_CACHE                                                       \
  {                                                                     \
    std::ostringstream oss;                                             \
    oss<<"Size:"<<mQueue.Count()<<" [ ";                                \
    for(std::size_t _i=0; _i<mQueue.Count(); ++_i)                      \
    {                                                                   \
      oss<<_i<<                                                         \
        "={ frm#: " << mQueue[_i].mFrameNumber <<                        \
           " tex: " << mImageUrls[mQueue[_i].mFrameNumber].mTextureId<<"}, ";  \
    }                                                                   \
    oss<<" ]"<<std::endl;                                               \
    DALI_LOG_INFO(gAnimImgLogFilter,Debug::Concise,"%s",oss.str().c_str()); \
  }

#else
  #define LOG_CACHE
#endif

const bool ENABLE_ORIENTATION_CORRECTION( true );

}

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

RollingGifImageCache::RollingGifImageCache(
  TextureManager& textureManager, GifLoading& gifLoading, uint32_t frameCount, ImageCache::FrameReadyObserver& observer,
  uint16_t cacheSize, uint16_t batchSize )
: ImageCache( textureManager, observer, batchSize ),
  mGifLoading( gifLoading ),
  mFrameCount( frameCount ),
  mFrameIndex( 0 ),
  mCacheSize( cacheSize ),
  mQueue( cacheSize )
{
  mImageUrls.resize( mFrameCount );
  LoadBatch();
}

RollingGifImageCache::~RollingGifImageCache()
{
  if( mTextureManagerAlive )
  {
    while( !mQueue.IsEmpty() )
    {
      ImageFrame imageFrame = mQueue.PopFront();
      Dali::Toolkit::TextureManager::RemoveTexture( mImageUrls[ imageFrame.mFrameNumber ].mUrl );
    }
  }
}


TextureSet RollingGifImageCache::FirstFrame()
{
  return GetFrontTextureSet();
}

TextureSet RollingGifImageCache::NextFrame()
{
  TextureSet textureSet;

  ImageFrame imageFrame = mQueue.PopFront();
  Dali::Toolkit::TextureManager::RemoveTexture( mImageUrls[ imageFrame.mFrameNumber ].mUrl );
  mImageUrls[ imageFrame.mFrameNumber ].mTextureId = TextureManager::INVALID_TEXTURE_ID;

  LoadBatch();

  return GetFrontTextureSet();
}

bool RollingGifImageCache::IsFrontReady() const
{
  return ( !mQueue.IsEmpty() );
}

void RollingGifImageCache::LoadBatch()
{
  // Try and load up to mBatchSize images, until the cache is filled.
  // Once the cache is filled, as frames progress, the old frame is
  // removed, and another frame is loaded

  std::vector<Dali::PixelData> pixelDataList;

  // Get the smallest number of frames we need to load
  int batchSize = std::min( std::size_t(mBatchSize), mCacheSize - mQueue.Count() );
  DALI_LOG_INFO( gAnimImgLogFilter, Debug::Concise, "RollingGifImageCache::LoadBatch() mFrameIndex:%d  batchSize:%d\n", mFrameIndex, batchSize );

  if( mGifLoading.LoadNextNFrames(  mFrameIndex, batchSize, pixelDataList) )
  {
    unsigned int pixelDataListCount = pixelDataList.size();

    for( unsigned int i = 0; i < pixelDataListCount && !mQueue.IsFull(); ++i )
    {
      ImageFrame imageFrame;

      // create the texture for uploading the pixel data
      Texture texture = Texture::New( Dali::TextureType::TEXTURE_2D,
                                      pixelDataList[i].GetPixelFormat(),
                                      pixelDataList[i].GetWidth(),
                                      pixelDataList[i].GetHeight() );

      texture.Upload( pixelDataList[i] );

      mImageUrls[ mUrlIndex ].mUrl = Dali::Toolkit::TextureManager::AddTexture(texture);
      imageFrame.mFrameNumber = mUrlIndex;

      ++mUrlIndex;
      mUrlIndex %= mImageUrls.size();

      mQueue.PushBack( imageFrame );

      bool synchronousLoading = false;
      bool atlasingStatus = false;
      bool loadingStatus = false;
      TextureManager::MaskingDataPointer maskInfo = nullptr;
      AtlasUploadObserver* atlasObserver = nullptr;
      ImageAtlasManagerPtr imageAtlasManager = nullptr;
      Vector4 textureRect;
      auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

      mTextureManager.LoadTexture(
        mImageUrls[ imageFrame.mFrameNumber ].mUrl, ImageDimensions(), FittingMode::SCALE_TO_FILL,
        SamplingMode::BOX_THEN_LINEAR, maskInfo,
        synchronousLoading, mImageUrls[ imageFrame.mFrameNumber ].mTextureId, textureRect,
        atlasingStatus, loadingStatus, Dali::WrapMode::Type::DEFAULT,
        Dali::WrapMode::Type::DEFAULT, NULL,
        atlasObserver, imageAtlasManager, ENABLE_ORIENTATION_CORRECTION, TextureManager::ReloadPolicy::CACHED, preMultiply );
    }

    mFrameIndex += batchSize;
    mFrameIndex %= mFrameCount;
  }

  LOG_CACHE;
}

TextureSet RollingGifImageCache::GetFrontTextureSet() const
{
  DALI_LOG_INFO( gAnimImgLogFilter, Debug::Concise, "RollingGifImageCache::GetFrontTextureSet() FrameNumber:%d\n", mQueue[ 0 ].mFrameNumber );

  TextureManager::TextureId textureId = GetCachedTextureId( 0 );
  return mTextureManager.GetTextureSet( textureId );
}

TextureManager::TextureId RollingGifImageCache::GetCachedTextureId( int index ) const
{
  return mImageUrls[ mQueue[ index ].mFrameNumber ].mTextureId;
}

} //namespace Internal
} //namespace Toolkit
} //namespace Dali
