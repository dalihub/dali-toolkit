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
#include "rolling-animated-image-cache.h"

// EXTERNAL HEADERS
#include <dali/devel-api/rendering/texture-devel.h>

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

RollingAnimatedImageCache::RollingAnimatedImageCache(
  TextureManager& textureManager, AnimatedImageLoading& animatedImageLoading, uint32_t frameCount, ImageCache::FrameReadyObserver& observer,
  uint16_t cacheSize, uint16_t batchSize, bool isSynchronousLoading )
: ImageCache( textureManager, observer, batchSize ),
  mAnimatedImageLoading( animatedImageLoading ),
  mFrameCount( frameCount ),
  mFrameIndex( 0 ),
  mCacheSize( cacheSize ),
  mQueue( cacheSize ),
  mIsSynchronousLoading( isSynchronousLoading ),
  mOnLoading( false )
{
  mImageUrls.resize( mFrameCount );
  mIntervals.assign( mFrameCount, 0 );
  LoadBatch();
}

RollingAnimatedImageCache::~RollingAnimatedImageCache()
{
  if( mTextureManagerAlive )
  {
    while( !mQueue.IsEmpty() )
    {
      ImageFrame imageFrame = mQueue.PopFront();
      mTextureManager.Remove( mImageUrls[ imageFrame.mFrameNumber ].mTextureId, this );
    }
  }
}

TextureSet RollingAnimatedImageCache::Frame( uint32_t frameIndex )
{
  if(mQueue.IsFull() && IsFrontReady() == true)
  {
    TextureSet textureSet = GetFrontTextureSet();
    if(!Dali::DevelTexture::IsUploaded(textureSet.GetTexture(0)))
    {
      return textureSet;
    }
  }

  bool popExist = false;
  while( !mQueue.IsEmpty() && mQueue.Front().mFrameNumber != frameIndex )
  {
    ImageFrame imageFrame = mQueue.PopFront();
    mTextureManager.Remove( mImageUrls[ imageFrame.mFrameNumber ].mTextureId, this );
    mImageUrls[ imageFrame.mFrameNumber ].mTextureId = TextureManager::INVALID_TEXTURE_ID;
    popExist = true;
  }

  TextureSet textureSet;
  // If we need to load new frame that are not stored in queue.
  // Load the frame synchronously.
  if( mIsSynchronousLoading && mQueue.IsEmpty() )
  {
    bool synchronousLoading = true;
    textureSet = mTextureManager.LoadAnimatedImageTexture( mAnimatedImageLoading, frameIndex, SamplingMode::BOX_THEN_LINEAR,
                                                           synchronousLoading, mImageUrls[ frameIndex ].mTextureId, Dali::WrapMode::Type::DEFAULT,
                                                           Dali::WrapMode::Type::DEFAULT, this );
    mFrameIndex = ( frameIndex + 1 ) % mFrameCount;
  }

  if( popExist || mQueue.IsEmpty() )
  {
    // If the frame of frameIndex was already loaded, load batch from the last frame of queue
    if( !mQueue.IsEmpty() )
    {
      if(!mLoadWaitingQueue.empty())
      {
        mFrameIndex = ( mLoadWaitingQueue.back() + 1 ) % mFrameCount;
      }
      else
      {
        mFrameIndex = ( mQueue.Back().mFrameNumber + 1 ) % mFrameCount;
      }
    }
    else
    {
      mOnLoading = false;
      // If the request is for the first frame or a jumped frame(JUMP_TO) remove current waiting queue.
      mLoadWaitingQueue.clear();
      // If the queue is empty, and the frame of frameIndex is not loaded synchronously. load batch from the frame of frameIndex
      if( !textureSet )
      {
        mFrameIndex = frameIndex;
      }
    }
    LoadBatch();
  }

  if( !textureSet )
  {
    if( IsFrontReady() == true )
    {
      textureSet = GetFrontTextureSet();
    }
    else
    {
      mWaitingForReadyFrame = true;
    }
  }

  return textureSet;
}

TextureSet RollingAnimatedImageCache::FirstFrame()
{
  return Frame( 0u );
}

TextureSet RollingAnimatedImageCache::NextFrame()
{
  TextureSet textureSet;
  if(!mQueue.IsEmpty())
  {
    uint32_t frameIndex = mQueue.Front().mFrameNumber;
    if(IsFrontReady())
    {
      frameIndex = (frameIndex + 1) % mFrameCount;
    }
    textureSet = Frame(frameIndex);
  }
  else
  {
    DALI_LOG_ERROR("Cache is empty.");
  }
  
  return textureSet;
}

uint32_t RollingAnimatedImageCache::GetFrameInterval( uint32_t frameIndex )
{
  return mAnimatedImageLoading.GetFrameInterval( frameIndex );
}

int32_t RollingAnimatedImageCache::GetCurrentFrameIndex()
{
  if(mQueue.IsEmpty())
  {
    return -1;
  }
  return mQueue.Front().mFrameNumber;
}

bool RollingAnimatedImageCache::IsFrontReady() const
{
  return ( !mQueue.IsEmpty() && mQueue.Front().mReady );
}

void RollingAnimatedImageCache::RequestFrameLoading( uint32_t frameIndex )
{
  ImageFrame imageFrame;
  imageFrame.mFrameNumber = frameIndex;
  imageFrame.mReady       = false;

  mQueue.PushBack(imageFrame);

  mRequestingLoad = true;

  bool synchronousLoading = false;
  mTextureManager.LoadAnimatedImageTexture( mAnimatedImageLoading, frameIndex, SamplingMode::BOX_THEN_LINEAR,
                                            synchronousLoading, mImageUrls[ frameIndex ].mTextureId, Dali::WrapMode::Type::DEFAULT,
                                            Dali::WrapMode::Type::DEFAULT, this );

  mRequestingLoad = false;
}

void RollingAnimatedImageCache::LoadBatch()
{
  // Try and load up to mBatchSize images, until the cache is filled.
  // Once the cache is filled, as frames progress, the old frame is
  // removed, and another frame is loaded

  bool frontFrameReady = IsFrontReady();
  for( unsigned int i=0; i< mBatchSize && mQueue.Count() + mLoadWaitingQueue.size() < static_cast<uint32_t>(mCacheSize) && !mQueue.IsFull(); ++i )
  {
    if( !mOnLoading )
    {
      mOnLoading = true;
      RequestFrameLoading( mFrameIndex );
    }
    else
    {
      mLoadWaitingQueue.push_back( mFrameIndex );
    }

    mFrameIndex++;
    mFrameIndex %= mFrameCount;
  }

  CheckFrontFrame( frontFrameReady );

  LOG_CACHE;
}

void RollingAnimatedImageCache::SetImageFrameReady( TextureManager::TextureId textureId )
{
  for( std::size_t i = 0; i < mQueue.Count() ; ++i )
  {
    if( GetCachedTextureId( i ) == textureId )
    {
      mQueue[i].mReady = true;
      break;
    }
  }
}

TextureSet RollingAnimatedImageCache::GetFrontTextureSet() const
{
  DALI_LOG_INFO( gAnimImgLogFilter, Debug::Concise, "RollingAnimatedImageCache::GetFrontTextureSet() FrameNumber:%d\n", mQueue[ 0 ].mFrameNumber );

  TextureManager::TextureId textureId = GetCachedTextureId( 0 );
  return mTextureManager.GetTextureSet( textureId );
}

TextureManager::TextureId RollingAnimatedImageCache::GetCachedTextureId( int index ) const
{
  return mImageUrls[ mQueue[ index ].mFrameNumber ].mTextureId;
}

void RollingAnimatedImageCache::CheckFrontFrame( bool wasReady )
{
  if( mWaitingForReadyFrame && wasReady == false && IsFrontReady() )
  {
    mWaitingForReadyFrame = false;
    mObserver.FrameReady( GetFrontTextureSet() );
  }
}

void RollingAnimatedImageCache::UploadComplete(
  bool           loadSuccess,
  int32_t        textureId,
  TextureSet     textureSet,
  bool           useAtlasing,
  const Vector4& atlasRect,
  bool           preMultiplied )
{
  DALI_LOG_INFO(gAnimImgLogFilter,Debug::Concise,"AnimatedImageVisual::UploadComplete(textureId:%d) start\n", textureId);
  LOG_CACHE;

  bool frontFrameReady = IsFrontReady();

  if( !mRequestingLoad )
  {
    SetImageFrameReady( textureId );

    CheckFrontFrame( frontFrameReady );
  }
  else
  {
    // UploadComplete has been called from within RequestLoad. TextureManager must
    // therefore already have the texture cached, so make the texture ready.
    // (Use the last texture, as the texture id hasn't been assigned yet)
    mQueue.Back().mReady = true;
  }

  mOnLoading = false;
  // The frames of a single animated image can not be loaded parallelly.
  // Therefore, a frame is now loading, other orders are waiting.
  // And, after the frame is loaded, requests load of next order.
  if( !mLoadWaitingQueue.empty() )
  {
    uint32_t loadingIndex = mLoadWaitingQueue.front();
    mLoadWaitingQueue.erase( mLoadWaitingQueue.begin() );
    mOnLoading = true;
    RequestFrameLoading( loadingIndex );
  }

  LOG_CACHE;
}

void RollingAnimatedImageCache::LoadComplete(
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
