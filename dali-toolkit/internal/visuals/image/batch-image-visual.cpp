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
#include "batch-image-visual.h"

// EXTERNAL HEADER
#include <cstring> // for strncasecmp
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/images/native-image.h>
#include <dali/integration-api/debug.h>
#include <dali/devel-api/adaptor-framework/bitmap-loader.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/public-api/rendering/texture-set.h>

// INTERNAL HEADER
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
const char HTTP_URL[] = "http://";
const char HTTPS_URL[] = "https://";

// Properties:
const char * const DESIRED_WIDTH( "desiredWidth" );
const char * const DESIRED_HEIGHT( "desiredHeight" );

const Vector4 FULL_TEXTURE_RECT( 0.f, 0.f, 1.f, 1.f );

// The shader used for batched rendering. It uses interleaved data for
// attributes. Limitation is that all batched renderers will share same set of uniforms.
const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  attribute mediump vec2 aTexCoord;\n
  uniform mediump mat4 uMvpMatrix;\n
  varying mediump vec2 vTexCoord;\n
  \n
  void main()\n
  {\n
    vTexCoord = aTexCoord;\n
    gl_Position = uMvpMatrix * vec4( aPosition, 0.0, 1.0 );\n
  }\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  uniform lowp float uAlphaBlending; // Set to 1.0 for conventional alpha blending; if pre-multiplied alpha blending, set to 0.0
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vTexCoord ) * vec4( uColor.rgb*max( uAlphaBlending, uColor.a ), uColor.a );\n
  }\n
);

} //unnamed namespace

BatchImageVisual::BatchImageVisual( VisualFactoryCache& factoryCache )
  : Visual::Base( factoryCache ),
    mDesiredSize()
{
}

BatchImageVisual::~BatchImageVisual()
{
}

void BatchImageVisual::DoInitialize( Actor& actor, const Property::Map& propertyMap )
{
  std::string oldImageUrl = mImageUrl;
  Property::Value* imageURLValue = propertyMap.Find( Dali::Toolkit::ImageVisual::Property::URL, Dali::Toolkit::Internal::IMAGE_URL_NAME );

  if( imageURLValue )
  {
    imageURLValue->Get( mImageUrl );

    int desiredWidth = 0;
    Property::Value* desiredWidthValue = propertyMap.Find( Dali::Toolkit::ImageVisual::Property::DESIRED_WIDTH, DESIRED_WIDTH );
    if( desiredWidthValue )
    {
      desiredWidthValue->Get( desiredWidth );
    }

    int desiredHeight = 0;
    Property::Value* desiredHeightValue = propertyMap.Find( Dali::Toolkit::ImageVisual::Property::DESIRED_HEIGHT, DESIRED_HEIGHT );
    if( desiredHeightValue )
    {
      desiredHeightValue->Get( desiredHeight );
    }

    mDesiredSize = ImageDimensions( desiredWidth, desiredHeight );
  }

  // Remove old renderer if exit.
  if( mImpl->mRenderer )
  {
    if( actor ) // Remove old renderer from actor.
    {
      actor.RemoveRenderer( mImpl->mRenderer );
    }
    if( !oldImageUrl.empty() ) // Clean old renderer from cache.
    {
      CleanCache( oldImageUrl );
    }
  }

  // If actor is on stage, create new renderer and apply to actor.
  if( actor && actor.OnStage() )
  {
    SetOnStage( actor );
  }
}

void BatchImageVisual::SetSize( const Vector2& size )
{
  Visual::Base::SetSize( size );
}

void BatchImageVisual::GetNaturalSize( Vector2& naturalSize ) const
{
  if( mDesiredSize.GetWidth() > 0 && mDesiredSize.GetHeight() > 0 )
  {
    naturalSize.x = mDesiredSize.GetWidth();
    naturalSize.y = mDesiredSize.GetHeight();
    return;
  }
  else if( !mImageUrl.empty() )
  {
    ImageDimensions dimentions = ResourceImage::GetImageSize( mImageUrl );
    naturalSize.x = dimentions.GetWidth();
    naturalSize.y = dimentions.GetHeight();
    return;
  }

  naturalSize = Vector2::ZERO;
}

void BatchImageVisual::InitializeRenderer( const std::string& imageUrl )
{
  if( imageUrl.empty() )
  {
    return;
  }

  mImageUrl = imageUrl;
  mImpl->mRenderer.Reset();
  mAtlasRect = FULL_TEXTURE_RECT;

  if( !mImpl->mCustomShader &&
      ( strncasecmp( imageUrl.c_str(),HTTP_URL,  sizeof( HTTP_URL )  -1 ) != 0 ) && // Ignore remote images
      ( strncasecmp( imageUrl.c_str(), HTTPS_URL, sizeof( HTTPS_URL ) -1 ) != 0 ) )
  {
    if( !mImpl->mRenderer )
    {
      TextureSet textureSet = mFactoryCache.GetAtlasManager()->Add(
            mAtlasRect,
            imageUrl,
            mDesiredSize );

      // If image doesn't fit the atlas, create new texture set with texture that
      // is used as whole.
      if( !textureSet )
      {
        BitmapLoader loader = BitmapLoader::New( imageUrl, mDesiredSize );
        loader.Load();
        Dali::PixelData pixelData = loader.GetPixelData();
        Texture texture = Texture::New( TextureType::TEXTURE_2D,
                                        pixelData.GetPixelFormat(),
                                        pixelData.GetWidth(),
                                        pixelData.GetHeight() );
        texture.Upload( pixelData );
        textureSet = TextureSet::New();
        textureSet.SetTexture( 0, texture );
        mAtlasRect = FULL_TEXTURE_RECT;
      }

      Geometry geometry = mFactoryCache.CreateBatchQuadGeometry( mAtlasRect );
      Shader shader( GetBatchShader( mFactoryCache ) );
      mImpl->mRenderer = Renderer::New( geometry, shader );
      mImpl->mRenderer.SetTextures( textureSet );

      // Turn batching on, to send message it must be on stage.
      mImpl->mRenderer.SetProperty( Dali::Renderer::Property::BATCHING_ENABLED, true );
    }
    mImpl->mFlags |= Impl::IS_FROM_CACHE;
  }
}

void BatchImageVisual::DoSetOnStage( Actor& actor )
{
  if( !mImageUrl.empty() )
  {
    InitializeRenderer( mImageUrl );
  }
  // Turn batching on, to send message it must be on stage
  mImpl->mRenderer.SetProperty( Dali::Renderer::Property::BATCHING_ENABLED, true );
}

void BatchImageVisual::DoSetOffStage( Actor& actor )
{
  actor.RemoveRenderer( mImpl->mRenderer );

  // If we own the image then make sure we release it when we go off stage
  if( !mImageUrl.empty() )
  {
    CleanCache( mImageUrl );
  }
  else
  {
    mImpl->mRenderer.Reset();
  }
}

void BatchImageVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE );

  if( !mImageUrl.empty() )
  {
    map.Insert( Toolkit::ImageVisual::Property::URL, mImageUrl );
    map.Insert( Toolkit::ImageVisual::Property::BATCHING_ENABLED, true );
    map.Insert( Toolkit::ImageVisual::Property::DESIRED_WIDTH, mDesiredSize.GetWidth() );
    map.Insert( Toolkit::ImageVisual::Property::DESIRED_HEIGHT, mDesiredSize.GetHeight() );
  }
}

Shader BatchImageVisual::GetBatchShader( VisualFactoryCache& factoryCache )
{
  Shader shader = factoryCache.GetShader( VisualFactoryCache::BATCH_IMAGE_SHADER );
  if( !shader )
  {
    shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
    factoryCache.SaveShader( VisualFactoryCache::BATCH_IMAGE_SHADER, shader );
  }
  return shader;
}

void BatchImageVisual::CleanCache(const std::string& url)
{
  TextureSet textureSet = mImpl->mRenderer.GetTextures();
  mImpl->mRenderer.Reset();
  if( mFactoryCache.CleanRendererCache( url ) )
  {
    mFactoryCache.GetAtlasManager()->Remove( textureSet, mAtlasRect );
  }
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali
