/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/images/texture-set-image.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/third-party/nanosvg/nanosvg.h>
#include <dali-toolkit/internal/visuals/svg/svg-rasterize-thread.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/image-visual-shader-factory.h>

namespace
{
const char * const UNITS("px");

// property name
const char * const IMAGE_ATLASING( "atlasing" );

const Dali::Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);
}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

SvgVisualPtr SvgVisual::New( VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, const Property::Map& properties )
{
  SvgVisualPtr svgVisual( new SvgVisual( factoryCache, shaderFactory ) );
  svgVisual->ParseFromUrl( imageUrl );
  svgVisual->SetProperties( properties );

  return svgVisual;
}

SvgVisualPtr SvgVisual::New( VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl )
{
  SvgVisualPtr svgVisual( new SvgVisual( factoryCache, shaderFactory ) );
  svgVisual->ParseFromUrl( imageUrl );

  return svgVisual;
}

SvgVisual::SvgVisual( VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory )
: Visual::Base( factoryCache, Visual::FittingMode::FILL ),
  mImageVisualShaderFactory( shaderFactory ),
  mAtlasRect( FULL_TEXTURE_RECT ),
  mImageUrl( ),
  mParsedImage( NULL ),
  mPlacementActor(),
  mVisualSize(Vector2::ZERO),
  mAttemptAtlasing( false )
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

void SvgVisual::DoSetProperties( const Property::Map& propertyMap )
{
  // url already passed in from constructor
  for( Property::Map::SizeType iter = 0; iter < propertyMap.Count(); ++iter )
  {
    KeyValuePair keyValue = propertyMap.GetKeyValue( iter );
    if( keyValue.first.type == Property::Key::INDEX )
    {
      DoSetProperty( keyValue.first.indexKey, keyValue.second );
    }
    else if( keyValue.first == IMAGE_ATLASING )
    {
      DoSetProperty( Toolkit::ImageVisual::Property::ATLASING, keyValue.second );
    }
  }
}

void SvgVisual::DoSetProperty( Property::Index index, const Property::Value& value )
{
  switch( index )
  {
    case Toolkit::ImageVisual::Property::ATLASING:
    {
      value.Get( mAttemptAtlasing );
      break;
    }
  }
}

void SvgVisual::DoSetOnStage( Actor& actor )
{
  Shader shader;
  if( !mImpl->mCustomShader )
  {
    shader = mImageVisualShaderFactory.GetShader( mFactoryCache, mAttemptAtlasing, true );
  }
  else
  {
    shader = Shader::New( mImpl->mCustomShader->mVertexShader.empty() ? mImageVisualShaderFactory.GetVertexShaderSource() : mImpl->mCustomShader->mVertexShader,
                          mImpl->mCustomShader->mFragmentShader.empty() ? mImageVisualShaderFactory.GetFragmentShaderSource() : mImpl->mCustomShader->mFragmentShader,
                          mImpl->mCustomShader->mHints );

    shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
  }

  Geometry geometry = mFactoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );
  TextureSet textureSet = TextureSet::New();
  mImpl->mRenderer = Renderer::New( geometry, shader );
  mImpl->mRenderer.SetTextures( textureSet );

  // Register transform properties
  mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );

  // Defer the rasterisation task until we get given a size (by Size Negotiation algorithm)

  // Hold the weak handle of the placement actor and delay the adding of renderer until the svg rasterization is finished.
  mPlacementActor = actor;

  // SVG visual needs it's size set before it can be rasterized hence set ResourceReady once on stage
  ResourceReady( Toolkit::Visual::ResourceStatus::READY );
}

void SvgVisual::DoSetOffStage( Actor& actor )
{
  mFactoryCache.GetSVGRasterizationThread()->RemoveTask( this );

  actor.RemoveRenderer( mImpl->mRenderer );
  mImpl->mRenderer.Reset();
  mPlacementActor.Reset();

  // Reset the visual size to zero so that when adding the actor back to stage the SVG rasterization is forced
  mVisualSize = Vector2::ZERO;
}

void SvgVisual::GetNaturalSize( Vector2& naturalSize )
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

void SvgVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::Visual::SVG );
  if( mImageUrl.IsValid() )
  {
    map.Insert( Toolkit::ImageVisual::Property::URL, mImageUrl.GetUrl() );
    map.Insert( Toolkit::ImageVisual::Property::ATLASING, mAttemptAtlasing );
  }
}

void SvgVisual::DoCreateInstancePropertyMap( Property::Map& map ) const
{
  // Do nothing
}

void SvgVisual::ParseFromUrl( const VisualUrl& imageUrl )
{
  mImageUrl = imageUrl;
  if( mImageUrl.IsLocalResource() )
  {
    Vector2 dpi = Stage::GetCurrent().GetDpi();
    float meanDpi = (dpi.height + dpi.width) * 0.5f;
    mParsedImage = nsvgParseFromFile( mImageUrl.GetUrl().c_str(), UNITS, meanDpi );
  }
}

void SvgVisual::AddRasterizationTask( const Vector2& size )
{
  if( mImpl->mRenderer && mParsedImage )
  {
    unsigned int width = static_cast<unsigned int>(size.width);
    unsigned int height = static_cast<unsigned int>( size.height );

    RasterizingTaskPtr newTask = new RasterizingTask( this, mParsedImage, width, height );
    mFactoryCache.GetSVGRasterizationThread()->AddTask( newTask );
  }
}

void SvgVisual::ApplyRasterizedImage( PixelData rasterizedPixelData )
{
  if( IsOnStage()  )
  {
    TextureSet currentTextureSet = mImpl->mRenderer.GetTextures();
    if( mImpl->mFlags |= Impl::IS_ATLASING_APPLIED )
    {
      mFactoryCache.GetAtlasManager()->Remove( currentTextureSet, mAtlasRect );
    }

    TextureSet textureSet;

    if( mAttemptAtlasing && !mImpl->mCustomShader )
    {
      Vector4 atlasRect;
      textureSet = mFactoryCache.GetAtlasManager()->Add(atlasRect, rasterizedPixelData );
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
    }

    if( !textureSet ) // no atlasing - mAttemptAtlasing is false or adding to atlas is failed
    {
      Texture texture = Texture::New( Dali::TextureType::TEXTURE_2D, Pixel::RGBA8888,
                                      rasterizedPixelData.GetWidth(), rasterizedPixelData.GetHeight() );
      texture.Upload( rasterizedPixelData );
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
        textureSet.SetTexture( 0, texture );
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

   // Svg loaded and ready to display
   ResourceReady( Toolkit::Visual::ResourceStatus::READY );
  }
}

void SvgVisual::OnSetTransform()
{
  Vector2 visualSize = mImpl->mTransform.GetVisualSize( mImpl->mControlSize );

  if( mParsedImage && IsOnStage() )
  {
    if( visualSize != mVisualSize )
    {
      AddRasterizationTask( visualSize );
      mVisualSize = visualSize;
    }
  }

  if(mImpl->mRenderer)
  {
    mImpl->mTransform.RegisterUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
