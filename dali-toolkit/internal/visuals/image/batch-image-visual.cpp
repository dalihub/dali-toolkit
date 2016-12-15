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
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/rendering/texture-set.h>

// INTERNAL HEADER
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
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
  uniform lowp vec4 mixColor;\n
  uniform lowp float preMultipliedAlpha;\n
  lowp vec4 visualMixColor()\n
  {\n
    return vec4( mixColor.rgb * mix( 1.0, mixColor.a, preMultipliedAlpha ), mixColor.a );\n
  }\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor * visualMixColor();
  }\n
);

} // unnamed namespace

BatchImageVisualPtr BatchImageVisual::New( VisualFactoryCache& factoryCache, const std::string& url, const Property::Map& properties )
{
  BatchImageVisualPtr visual = new BatchImageVisual( factoryCache );
  visual->mImageUrl = url;
  visual->SetProperties( properties );

  return visual;
}

BatchImageVisual::BatchImageVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache ),
  mImageUrl(""),
  mDesiredSize()
{
}

BatchImageVisual::~BatchImageVisual()
{
}

void BatchImageVisual::DoSetProperties( const Property::Map& propertyMap )
{
  // url already passed in constructor

  for( Property::Map::SizeType iter = 0; iter < propertyMap.Count(); ++iter )
  {
    KeyValuePair keyValue = propertyMap.GetKeyValue( iter );
    if( keyValue.first.type == Property::Key::INDEX )
    {
      DoSetProperty( keyValue.first.indexKey, keyValue.second );
    }
    else
    {
      if( keyValue.first == DESIRED_WIDTH )
      {
        DoSetProperty( Toolkit::ImageVisual::Property::DESIRED_WIDTH, keyValue.second );
      }
      else if( keyValue.first == DESIRED_HEIGHT )
      {
        DoSetProperty( Toolkit::ImageVisual::Property::DESIRED_HEIGHT, keyValue.second );
      }
    }
  }
}

void BatchImageVisual::DoSetProperty( Property::Index index, const Property::Value& value )
{
  switch( index )
  {
    case Dali::Toolkit::ImageVisual::Property::DESIRED_WIDTH:
    {
      int width;
      if( value.Get( width ) )
      {
        mDesiredSize.SetWidth( width );
      }
      else
      {
        DALI_LOG_ERROR("BatchImageVisual: width property has incorrect type\n");
      }
      break;
    }
    case Dali::Toolkit::ImageVisual::Property::DESIRED_HEIGHT:
    {
      int height;
      if( value.Get( height ) )
      {
        mDesiredSize.SetHeight( height );
      }
      else
      {
        DALI_LOG_ERROR("BatchImageVisual: height property has incorrect type\n");
      }
      break;
    }
  }
}

void BatchImageVisual::GetNaturalSize( Vector2& naturalSize )
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
      mImpl->mRenderer.SetProperty( Dali::DevelRenderer::Property::BATCHING_ENABLED, true );
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
  mImpl->mRenderer.SetProperty( Dali::DevelRenderer::Property::BATCHING_ENABLED, true );

  actor.AddRenderer( mImpl->mRenderer );
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
  map.Insert( Toolkit::DevelVisual::Property::TYPE, Toolkit::Visual::IMAGE );

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

void BatchImageVisual::OnSetTransform()
{
  if( mImpl->mRenderer )
  {
    //Register transform properties
    mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
