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
#include "svg-visual.h"

// EXTERNAL INCLUDES
#include <dali/public-api/images/buffer-image.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/math/vector4.h>
#include <dali/devel-api/images/atlas.h>
#include <dali/devel-api/images/texture-set-image.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/third-party/nanosvg/nanosvg.h>
#include <dali-toolkit/internal/visuals/svg/svg-rasterize-thread.h>
#include <dali-toolkit/internal/visuals/image/image-visual.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>


namespace
{
const char * const UNITS("px");

const Dali::Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);
}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

SvgVisual::SvgVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache ),
  mAtlasRect( FULL_TEXTURE_RECT ),
  mParsedImage( NULL )
{
  // the rasterized image is with pre-multiplied alpha format
  mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;
}

SvgVisual::~SvgVisual()
{
  if( mParsedImage )
  {
    nsvgDelete( mParsedImage );
  }
}

void SvgVisual::DoInitialize( Actor& actor, const Property::Map& propertyMap )
{
  Property::Value* imageURLValue = propertyMap.Find( Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME );
  if( imageURLValue )
  {
    std::string imageUrl;
    if( imageURLValue->Get( imageUrl ) )
    {
      SetImage( imageUrl );
    }
    else
    {
      DALI_LOG_ERROR( "The property '%s' is not a string\n", IMAGE_URL_NAME );
    }
  }
}

void SvgVisual::DoSetOnStage( Actor& actor )
{
  Shader shader = ImageVisual::GetImageShader( mFactoryCache, true, true );
  Geometry geometry = mFactoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );
  if( !geometry )
  {
    geometry =  mFactoryCache.CreateQuadGeometry();
    mFactoryCache.SaveGeometry( VisualFactoryCache::QUAD_GEOMETRY, geometry );
  }
  TextureSet textureSet = TextureSet::New();
  mImpl->mRenderer = Renderer::New( geometry, shader );
  mImpl->mRenderer.SetTextures( textureSet );

  if( mImpl->mSize != Vector2::ZERO && mParsedImage )
  {
    AddRasterizationTask( mImpl->mSize );
  }

  // Hold the weak handle of the placement actor and delay the adding of renderer until the svg rasterization is finished.
  mPlacementActor = actor;
}

void SvgVisual::DoSetOffStage( Actor& actor )
{
  mFactoryCache.GetSVGRasterizationThread()->RemoveTask( this );

  actor.RemoveRenderer( mImpl->mRenderer );
  mImpl->mRenderer.Reset();
  mPlacementActor.Reset();
}

void SvgVisual::GetNaturalSize( Vector2& naturalSize ) const
{
  if( mParsedImage )
  {
    naturalSize.x = mParsedImage->width;
    naturalSize.y = mParsedImage->height;
  }
  else
  {
    naturalSize = Vector2::ZERO;
  }
}

void SvgVisual::SetSize( const Vector2& size )
{
  if(mImpl->mSize != size && mParsedImage && GetIsOnStage() )
  {
    AddRasterizationTask( size );
  }
  mImpl->mSize = size;
}

void SvgVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE );
  if( !mImageUrl.empty() )
  {
    map.Insert( Toolkit::ImageVisual::Property::URL, mImageUrl );
  }
}

void SvgVisual::SetImage( const std::string& imageUrl, ImageDimensions size )
{
  if( mImageUrl != imageUrl )
  {
    mImageUrl = imageUrl;

    NSVGimage* parsedImageOld = mParsedImage;

    Vector2 dpi = Stage::GetCurrent().GetDpi();
    float meanDpi = (dpi.height + dpi.width) * 0.5f;
    mParsedImage = nsvgParseFromFile(mImageUrl.c_str(), UNITS, meanDpi);

    if( size.GetWidth() != 0u && size.GetHeight() != 0u)
    {
      mImpl->mSize.x = size.GetWidth();
      mImpl->mSize.y = size.GetHeight();
    }

    if( mImpl->mSize != Vector2::ZERO && GetIsOnStage() )
    {
      AddRasterizationTask( mImpl->mSize );
    }

    mFactoryCache.GetSVGRasterizationThread()->DeleteImage( parsedImageOld );
  }
}

void SvgVisual::AddRasterizationTask( const Vector2& size )
{
  if( mImpl->mRenderer && mParsedImage )
  {
    unsigned int width = static_cast<unsigned int>(size.width);
    unsigned int height = static_cast<unsigned int>( size.height );
    BufferImage image = BufferImage::New( width, height, Pixel::RGBA8888);

    RasterizingTaskPtr newTask = new RasterizingTask( this, mParsedImage, width, height );
    mFactoryCache.GetSVGRasterizationThread()->AddTask( newTask );
  }
}

void SvgVisual::ApplyRasterizedImage( PixelData rasterizedPixelData )
{
  if( GetIsOnStage()  )
  {
    TextureSet currentTextureSet = mImpl->mRenderer.GetTextures();
    if( mAtlasRect != FULL_TEXTURE_RECT )
    {
      mFactoryCache.GetAtlasManager()->Remove( currentTextureSet, mAtlasRect );
    }

    Vector4 atlasRect;
    TextureSet textureSet = mFactoryCache.GetAtlasManager()->Add(atlasRect, rasterizedPixelData );
    if( textureSet ) // atlasing
    {
      if( textureSet != currentTextureSet )
      {
        mImpl->mRenderer.SetTextures( textureSet );
      }
      mImpl->mRenderer.RegisterProperty( ATLAS_RECT_UNIFORM_NAME, atlasRect );
      mAtlasRect = atlasRect;
      mImpl->mFlags |= Impl::IS_ATLASING_APPLIED;
    }
    else // no atlasing
    {
      Atlas texture = Atlas::New( rasterizedPixelData.GetWidth(), rasterizedPixelData.GetHeight() );
      texture.Upload( rasterizedPixelData, 0, 0 );
      mImpl->mFlags &= ~Impl::IS_ATLASING_APPLIED;

      if( mAtlasRect == FULL_TEXTURE_RECT )
      {
        textureSet = currentTextureSet;
      }
      else
      {
        textureSet = TextureSet::New();
        mImpl->mRenderer.SetTextures( textureSet );

        mImpl->mRenderer.RegisterProperty( ATLAS_RECT_UNIFORM_NAME, FULL_TEXTURE_RECT );
        mAtlasRect = FULL_TEXTURE_RECT;
      }

      if( textureSet )
      {
        TextureSetImage( textureSet, 0u, texture );
      }
    }

    // Rasterized pixels are uploaded to texture. If weak handle is holding a placement actor, it is the time to add the renderer to actor.
    Actor actor = mPlacementActor.GetHandle();
    if( actor )
    {
      actor.AddRenderer( mImpl->mRenderer );
      // reset the weak handle so that the renderer only get added to actor once
      mPlacementActor.Reset();
    }
  }
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali
