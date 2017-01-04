/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/image/image-visual.h>
#include <dali-toolkit/devel-api/image-loader/image-atlas.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

AnimatedImageVisualPtr AnimatedImageVisual::New( VisualFactoryCache& factoryCache, const std::string& imageUrl, const Property::Map& properties )
{
  AnimatedImageVisual* visual = new AnimatedImageVisual( factoryCache );
  visual->mImageUrl = imageUrl;
  visual->SetProperties( properties );

  return visual;
}

AnimatedImageVisualPtr AnimatedImageVisual::New( VisualFactoryCache& factoryCache, const std::string& imageUrl )
{
  AnimatedImageVisual* visual = new AnimatedImageVisual( factoryCache );
  visual->mImageUrl = imageUrl;

  return visual;
}

AnimatedImageVisual::AnimatedImageVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache ),
  mFrameDelayTimer(),
  mImageUrl(),
  mImageSize(),
  mCurrentFrameIndex( 0 )
{}

AnimatedImageVisual::~AnimatedImageVisual()
{
}

void AnimatedImageVisual::GetNaturalSize( Vector2& naturalSize )
{
  if( mImageSize.GetWidth() == 0 &&  mImageSize.GetHeight() == 0)
  {
    mImageSize = Dali::GetGifImageSize( mImageUrl );
  }

  naturalSize.width = mImageSize.GetWidth();
  naturalSize.height = mImageSize.GetHeight();
}

void AnimatedImageVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::DevelVisual::Property::TYPE, Toolkit::Visual::IMAGE );
  if( !mImageUrl.empty() )
  {
    map.Insert( Toolkit::ImageVisual::Property::URL, mImageUrl );
  }
}

void AnimatedImageVisual::DoSetProperties( const Property::Map& propertyMap )
{
  // url already passed in from constructor
}

void AnimatedImageVisual::DoSetOnStage( Actor& actor )
{
  Texture texture = PrepareAnimatedImage();
  if( texture ) // if the image loading is successful
  {
    Shader shader = ImageVisual::GetImageShader( mFactoryCache, true, true );
    Geometry geometry = mFactoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );
    TextureSet textureSet = TextureSet::New();
    mImpl->mRenderer = Renderer::New( geometry, shader );
    mImpl->mRenderer.SetTextures( textureSet );
    textureSet.SetTexture( 0u, PrepareAnimatedImage() );
    mImpl->mRenderer.RegisterProperty( ATLAS_RECT_UNIFORM_NAME, mTextureRectContainer[0] );

    // Register transform properties
    mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );

    mCurrentFrameIndex = 0;
    if( mFrameDelayContainer.Count() > 1 )
    {
      mFrameDelayTimer = Timer::New( mFrameDelayContainer[0] );
      mFrameDelayTimer.TickSignal().Connect( this, &AnimatedImageVisual::DisplayNextFrame );
      mFrameDelayTimer.Start();
    }

    actor.AddRenderer( mImpl->mRenderer );
  }
}

void AnimatedImageVisual::DoSetOffStage( Actor& actor )
{
  if( !mImpl->mRenderer )
  {
    return;
  }

  if( mFrameDelayTimer )
  {
    mFrameDelayTimer.Stop();
    mFrameDelayTimer.Reset();
  }

  mTextureRectContainer.Clear();
  mFrameDelayContainer.Clear();

  actor.RemoveRenderer( mImpl->mRenderer );
  mImpl->mRenderer.Reset();
}

void AnimatedImageVisual::OnSetTransform()
{
  if( mImpl->mRenderer )
  {
    mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
  }
}

Texture AnimatedImageVisual::PrepareAnimatedImage()
{
  // load from image file
  std::vector<Dali::PixelData> pixelDataList;
  if( Dali::LoadAnimatedGifFromFile( mImageUrl.c_str() , pixelDataList, mFrameDelayContainer ) )
  {
    mImageSize.SetWidth( pixelDataList[0].GetWidth() );
    mImageSize.SetHeight( pixelDataList[0].GetHeight() );

    return Toolkit::ImageAtlas::PackToAtlas( pixelDataList, mTextureRectContainer );
  }

  return Texture();
}

bool AnimatedImageVisual::DisplayNextFrame()
{
  mCurrentFrameIndex = (mCurrentFrameIndex+1) % mFrameDelayContainer.Count();
  mImpl->mRenderer.RegisterProperty( ATLAS_RECT_UNIFORM_NAME, mTextureRectContainer[mCurrentFrameIndex] );
  if( mFrameDelayTimer.GetInterval() != mFrameDelayContainer[mCurrentFrameIndex] )
  {
    mFrameDelayTimer.SetInterval( mFrameDelayContainer[mCurrentFrameIndex] );
  }

  return true;
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali
