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
 *
 */

// CLASS HEADER
#include "animated-image-visual.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/gif-loading.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/animated-image/fixed-image-cache.h>
#include <dali-toolkit/internal/visuals/animated-image/rolling-image-cache.h>
#include <dali-toolkit/internal/visuals/image/image-visual.h>
#include <dali-toolkit/devel-api/image-loader/image-atlas.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
// wrap modes
DALI_ENUM_TO_STRING_TABLE_BEGIN( WRAP_MODE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::WrapMode, DEFAULT )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::WrapMode, CLAMP_TO_EDGE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::WrapMode, REPEAT )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::WrapMode, MIRRORED_REPEAT )
DALI_ENUM_TO_STRING_TABLE_END( WRAP_MODE )

const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

#if defined(DEBUG_ENABLED)
Debug::Filter* gAnimImgLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_ANIMATED_IMAGE");
#endif
}


/**
 * Multi-image  Flow of execution
 *
 *   | DoSetProperties()
 *   |   LoadFirstBatch()
 *   |     cache->LoadBatch()
 *   |
 *   | DoSetOnStage()
 *   |   CreateRenderer()    (Doesn't become ready until first frame loads)
 *   |
 *   | FrameReady(textureSet)
 *   |   start first frame:
 *   |     actor.AddRenderer
 *   |     start timer
 *   |   mRenderer.SetTextures(textureSet)
 *   |
 *   | Timer ticks
 *   |   DisplayNextFrame()
 *   |     if front frame is ready,
 *   |       mRenderer.SetTextures( front frame's texture )
 *   |     else
 *   |       mWaitingForTexture=true
 *   |     cache->LoadBatch()
 *   |
 *   | FrameReady(textureSet)
 *   |   mRenderer.SetTextures(textureSet)
 *   V
 *  Time
 */

AnimatedImageVisualPtr AnimatedImageVisual::New( VisualFactoryCache& factoryCache, const VisualUrl& imageUrl, const Property::Map& properties )
{
  AnimatedImageVisual* visual = new AnimatedImageVisual( factoryCache );
  visual->mImageUrl = imageUrl;
  visual->SetProperties( properties );

  return visual;
}

AnimatedImageVisualPtr AnimatedImageVisual::New( VisualFactoryCache& factoryCache, const Property::Array& imageUrls, const Property::Map& properties )
{
  AnimatedImageVisual* visual = new AnimatedImageVisual( factoryCache );
  visual->mImageUrls = new ImageCache::UrlList();
  visual->mImageUrls->reserve( imageUrls.Count() );

  for( unsigned int i=0; i < imageUrls.Count(); ++i)
  {
    ImageCache::UrlStore urlStore;
    urlStore.mTextureId = TextureManager::INVALID_TEXTURE_ID;
    urlStore.mUrl = imageUrls[i].Get<std::string>();
    visual->mImageUrls->push_back( urlStore );
  }

  visual->SetProperties( properties );
  // starts loading immediately
  return visual;
}

AnimatedImageVisualPtr AnimatedImageVisual::New( VisualFactoryCache& factoryCache, const VisualUrl& imageUrl )
{
  AnimatedImageVisual* visual = new AnimatedImageVisual( factoryCache );
  visual->mImageUrl = imageUrl;

  return visual;
}

AnimatedImageVisual::AnimatedImageVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache ),
  mFrameDelayTimer(),
  mPlacementActor(),
  mPixelArea( FULL_TEXTURE_RECT ),
  mImageUrl(),
  mCurrentFrameIndex( 0 ),
  mImageUrls( NULL ),
  mImageCache( NULL ),
  mCacheSize( 1 ),
  mBatchSize( 1 ),
  mFrameDelay( 100 ),
  mUrlIndex( 0 ),
  mImageSize(),
  mWrapModeU( WrapMode::DEFAULT ),
  mWrapModeV( WrapMode::DEFAULT ),
  mStartFirstFrame(false)
{}

AnimatedImageVisual::~AnimatedImageVisual()
{
  delete mImageCache;
  delete mImageUrls;
}

void AnimatedImageVisual::GetNaturalSize( Vector2& naturalSize )
{
  if( mImageSize.GetWidth() == 0 &&  mImageSize.GetHeight() == 0)
  {
    if( mImageUrl.IsValid() )
    {
      mImageSize = Dali::GetGifImageSize( mImageUrl.GetUrl() );
    }
    else if( mImageUrls && mImageUrls->size() > 0 )
    {
      mImageSize = Dali::GetClosestImageSize( (*mImageUrls)[0].mUrl );
    }
  }

  naturalSize.width = mImageSize.GetWidth();
  naturalSize.height = mImageSize.GetHeight();
}

void AnimatedImageVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();

  map.Insert( Toolkit::DevelVisual::Property::TYPE, Toolkit::DevelVisual::ANIMATED_IMAGE );

  if( mImageUrl.IsValid() )
  {
    map.Insert( Toolkit::ImageVisual::Property::URL, mImageUrl.GetUrl() );
  }
  if( mImageUrls != NULL && ! mImageUrls->empty() )
  {
    Property::Array urls;
    for( unsigned int i=0; i<mImageUrls->size(); ++i)
    {
      urls.Add( (*mImageUrls)[i].mUrl );
    }
    Property::Value value( const_cast<Property::Array&>(urls) );
    map.Insert( Toolkit::ImageVisual::Property::URL, value );
  }

  map.Insert( Toolkit::ImageVisual::Property::PIXEL_AREA, mPixelArea );
  map.Insert( Toolkit::ImageVisual::Property::WRAP_MODE_U, mWrapModeU );
  map.Insert( Toolkit::ImageVisual::Property::WRAP_MODE_V, mWrapModeV );

  map.Insert( Toolkit::DevelImageVisual::Property::BATCH_SIZE, static_cast<int>(mBatchSize) );
  map.Insert( Toolkit::DevelImageVisual::Property::CACHE_SIZE, static_cast<int>(mCacheSize) );
  map.Insert( Toolkit::DevelImageVisual::Property::FRAME_DELAY, static_cast<int>(mFrameDelay) );
}

void AnimatedImageVisual::DoCreateInstancePropertyMap( Property::Map& map ) const
{
  // Do nothing
}

void AnimatedImageVisual::DoSetProperties( const Property::Map& propertyMap )
{
  // url[s] already passed in from constructor

  for( Property::Map::SizeType iter = 0; iter < propertyMap.Count(); ++iter )
  {
    KeyValuePair keyValue = propertyMap.GetKeyValue( iter );
    if( keyValue.first.type == Property::Key::INDEX )
    {
      DoSetProperty( keyValue.first.indexKey, keyValue.second );
    }
    else
    {
      if( keyValue.first == PIXEL_AREA_UNIFORM_NAME )
      {
        DoSetProperty( Toolkit::ImageVisual::Property::PIXEL_AREA, keyValue.second );
      }
      else if( keyValue.first == IMAGE_WRAP_MODE_U )
      {
        DoSetProperty( Toolkit::ImageVisual::Property::WRAP_MODE_U, keyValue.second );
      }
      else if( keyValue.first == IMAGE_WRAP_MODE_V )
      {
        DoSetProperty( Toolkit::ImageVisual::Property::WRAP_MODE_V, keyValue.second );
      }
      else if( keyValue.first == BATCH_SIZE_NAME )
      {
        DoSetProperty( Toolkit::DevelImageVisual::Property::BATCH_SIZE, keyValue.second );
      }
      else if( keyValue.first == CACHE_SIZE_NAME )
      {
        DoSetProperty( Toolkit::DevelImageVisual::Property::CACHE_SIZE, keyValue.second );
      }
      else if( keyValue.first == FRAME_DELAY_NAME )
      {
        DoSetProperty( Toolkit::DevelImageVisual::Property::FRAME_DELAY, keyValue.second );
      }
    }
  }

  if( mImageUrls && mImageUrls->size() > 0 )
  {
    LoadFirstBatch();
  }
}

void AnimatedImageVisual::DoSetProperty( Property::Index index,
                                         const Property::Value& value )
{
  switch(index)
  {
    case Toolkit::ImageVisual::Property::PIXEL_AREA:
    {
      value.Get( mPixelArea );
      break;
    }
    case Toolkit::ImageVisual::Property::WRAP_MODE_U:
    {
      int wrapMode;
      Scripting::GetEnumerationProperty( value, WRAP_MODE_TABLE, WRAP_MODE_TABLE_COUNT, wrapMode );
      mWrapModeU = Dali::WrapMode::Type( wrapMode );
      break;
    }
    case Toolkit::ImageVisual::Property::WRAP_MODE_V:
    {
      int wrapMode;
      Scripting::GetEnumerationProperty( value, WRAP_MODE_TABLE, WRAP_MODE_TABLE_COUNT, wrapMode );
      mWrapModeV = Dali::WrapMode::Type( wrapMode );
      break;
    }

    case Toolkit::DevelImageVisual::Property::BATCH_SIZE:
    {
      int batchSize;
      if( value.Get( batchSize ) )
      {
        mBatchSize = batchSize;
      }
      break;
    }

    case Toolkit::DevelImageVisual::Property::CACHE_SIZE:
    {
      int cacheSize;
      if( value.Get( cacheSize ) )
      {
        mCacheSize = cacheSize;
      }
      break;
    }

    case Toolkit::DevelImageVisual::Property::FRAME_DELAY:
    {
      int frameDelay;
      if( value.Get( frameDelay ) )
      {
        mFrameDelay = frameDelay;
      }
      break;
    }
  }
}

void AnimatedImageVisual::DoSetOnStage( Actor& actor )
{
  mPlacementActor = actor;
  TextureSet textureSet = PrepareTextureSet();
  CreateRenderer(); // Always create a renderer when on stage

  if( textureSet ) // if the image loading is successful
  {
    StartFirstFrame( textureSet );
  }
  else
  {
    mStartFirstFrame = true;
  }
}

void AnimatedImageVisual::DoSetOffStage( Actor& actor )
{
  DALI_ASSERT_DEBUG( (bool)mImpl->mRenderer && "There should always be a renderer whilst on stage");

  if( mFrameDelayTimer )
  {
    mFrameDelayTimer.Stop();
    mFrameDelayTimer.Reset();
  }

  mTextureRectContainer.Clear();
  mFrameDelayContainer.Clear();

  actor.RemoveRenderer( mImpl->mRenderer );
  mImpl->mRenderer.Reset();
  mPlacementActor.Reset();
}

void AnimatedImageVisual::OnSetTransform()
{
  if( mImpl->mRenderer )
  {
    mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
  }
}

void AnimatedImageVisual::CreateRenderer()
{
  bool defaultWrapMode = mWrapModeU <= WrapMode::CLAMP_TO_EDGE && mWrapModeV <= WrapMode::CLAMP_TO_EDGE;
  bool atlasing = (mTextureRectContainer.Count() > 0) ;
  Shader shader = ImageVisual::GetImageShader( mFactoryCache, atlasing, defaultWrapMode );

  Geometry geometry = mFactoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );

  mImpl->mRenderer = Renderer::New( geometry, shader );

  // Register transform properties
  mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );

  if( !defaultWrapMode ) // custom wrap mode
  {
    Vector2 wrapMode(mWrapModeU-WrapMode::CLAMP_TO_EDGE, mWrapModeV-WrapMode::CLAMP_TO_EDGE);
    wrapMode.Clamp( Vector2::ZERO, Vector2( 2.f, 2.f ) );
    mImpl->mRenderer.RegisterProperty( WRAP_MODE_UNIFORM_NAME, wrapMode );
  }

  if( mPixelArea != FULL_TEXTURE_RECT )
  {
    mImpl->mRenderer.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, mPixelArea );
  }

  mCurrentFrameIndex = 0;

  if( mTextureRectContainer.Count() > 0 )
  {
    mImpl->mRenderer.RegisterProperty( ATLAS_RECT_UNIFORM_NAME, mTextureRectContainer[mCurrentFrameIndex] );
  }
}

void AnimatedImageVisual::LoadFirstBatch()
{
  DALI_LOG_INFO(gAnimImgLogFilter,Debug::Concise,"AnimatedImageVisual::LoadFirstBatch()\n");

  // Ensure the batch size and cache size are no bigger than the number of URLs,
  // and that the cache is at least as big as the batch size.
  uint16_t numUrls = mImageUrls->size();
  uint16_t batchSize = std::min( mBatchSize, numUrls );
  uint16_t cacheSize = std::min( std::max( batchSize, mCacheSize ), numUrls );

  mUrlIndex = 0;
  TextureManager& textureManager = mFactoryCache.GetTextureManager();

  if( batchSize > 0 && cacheSize > 0 )
  {
    if( cacheSize < numUrls )
    {
      mImageCache = new RollingImageCache( textureManager, *mImageUrls, *this, cacheSize, batchSize );
    }
    else
    {
      mImageCache = new FixedImageCache( textureManager, *mImageUrls, *this, batchSize );
    }
  }
  else
  {
    mImageCache = new RollingImageCache( textureManager, *mImageUrls, *this, 1, 1 );
  }
}

void AnimatedImageVisual::StartFirstFrame( TextureSet& textureSet )
{
  DALI_LOG_INFO(gAnimImgLogFilter,Debug::Concise,"AnimatedImageVisual::StartFirstFrame()\n");

  mStartFirstFrame = false;
  mImpl->mRenderer.SetTextures( textureSet );
  Actor actor = mPlacementActor.GetHandle();
  if( actor )
  {
    actor.AddRenderer( mImpl->mRenderer );
    mPlacementActor.Reset();
  }

  int frameDelay = mFrameDelay;
  if( mFrameDelayContainer.Count() > 1 )
  {
    frameDelay = mFrameDelayContainer[0];
  }
  mFrameDelayTimer = Timer::New( frameDelay );
  mFrameDelayTimer.TickSignal().Connect( this, &AnimatedImageVisual::DisplayNextFrame );
  mFrameDelayTimer.Start();

  DALI_LOG_INFO(gAnimImgLogFilter,Debug::Concise,"ResourceReady()\n");
  ResourceReady();
}

TextureSet AnimatedImageVisual::PrepareTextureSet()
{
  TextureSet textureSet;
  if( mImageUrl.IsValid() )
  {
    textureSet = PrepareAnimatedGifImage();
  }
  else
  {
    textureSet = mImageCache->FirstFrame();
    if( textureSet )
    {
      SetImageSize( textureSet );
    }
  }
  return textureSet;
}

TextureSet AnimatedImageVisual::PrepareAnimatedGifImage()
{
  TextureSet textureSet;

  // load from image file
  std::vector<Dali::PixelData> pixelDataList;

  if( mImageUrl.IsLocal() )
  {
    if( Dali::LoadAnimatedGifFromFile( mImageUrl.GetUrl().c_str() , pixelDataList, mFrameDelayContainer ) )
    {
      mImageSize.SetWidth( pixelDataList[0].GetWidth() );
      mImageSize.SetHeight( pixelDataList[0].GetHeight() );

      Texture texture = Toolkit::ImageAtlas::PackToAtlas( pixelDataList, mTextureRectContainer );
      textureSet = TextureSet::New();
      textureSet.SetTexture( 0, texture );
    }
  }

  return textureSet;
}

void AnimatedImageVisual::SetImageSize( TextureSet& textureSet )
{
  if( textureSet )
  {
    Texture texture = textureSet.GetTexture( 0 );
    if( texture )
    {
      mImageSize.SetWidth( texture.GetWidth() );
      mImageSize.SetHeight( texture.GetHeight() );
    }
  }
}

void AnimatedImageVisual::FrameReady( TextureSet textureSet )
{
  SetImageSize( textureSet );

  if( mStartFirstFrame )
  {
    StartFirstFrame( textureSet );
  }
  else
  {
    mImpl->mRenderer.SetTextures( textureSet );
  }
}

bool AnimatedImageVisual::DisplayNextFrame()
{
  DALI_LOG_INFO(gAnimImgLogFilter,Debug::Concise,"AnimatedImageVisual::DisplayNextFrame() start\n");

  if( mFrameDelayContainer.Count() > 0 )
  {
    // Wrap the frame index
    ++mCurrentFrameIndex;
    mCurrentFrameIndex %= mFrameDelayContainer.Count();

    mImpl->mRenderer.RegisterProperty( ATLAS_RECT_UNIFORM_NAME, mTextureRectContainer[mCurrentFrameIndex] );

    if( mFrameDelayTimer.GetInterval() != mFrameDelayContainer[mCurrentFrameIndex] )
    {
      mFrameDelayTimer.SetInterval( mFrameDelayContainer[mCurrentFrameIndex] );
    }
  }
  else if( mImageCache )
  {
    TextureSet textureSet = mImageCache->NextFrame();
    if( textureSet )
    {
      SetImageSize( textureSet );
      mImpl->mRenderer.SetTextures( textureSet );
    }
  }

  // Keep timer ticking
  return true;
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali
