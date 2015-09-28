/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include "image-renderer.h"

// EXTERNAL HEADER
#include <dali/public-api/images/resource-image.h>
#include <dali/integration-api/debug.h>

// INTERNAL HEADER
#include <dali-toolkit/internal/controls/renderers/renderer-factory-impl.h>
#include <dali-toolkit/internal/controls/renderers/renderer-factory-cache.h>
#include <dali-toolkit/internal/controls/renderers/control-renderer-impl.h>
#include <dali-toolkit/internal/controls/renderers/control-renderer-data-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
const char * const RENDERER_TYPE("renderer-type");
const char * const RENDERER_TYPE_VALUE("image-renderer");

// property names
const char * const IMAGE_URL_NAME( "image-url" );
const char * const IMAGE_FITTING_MODE( "image-fitting-mode" );
const char * const IMAGE_SAMPLING_MODE( "image-sampling-mode" );
const char * const IMAGE_DESIRED_WIDTH( "image-desired-width" );
const char * const IMAGE_DESIRED_HEIGHT( "image-desired-height" );

// fitting modes
const char * const SHRINK_TO_FIT("shrink-to-fit");
const char * const SCALE_TO_FILL("scale-to-fill");
const char * const FIT_WIDTH("fit-width");
const char * const FIT_HEIGHT("fit-height");
const char * const DEFAULT("default");

// sampling modes
const char * const BOX("box");
const char * const NEAREST("nearest");
const char * const LINEAR("linear");
const char * const BOX_THEN_NEAREST("box-then-nearest");
const char * const BOX_THEN_LINEAR("box-then-linear");
const char * const NO_FILTER("no-filter");
const char * const DONT_CARE("dont-care");

std::string TEXTURE_UNIFORM_NAME = "sTexture";

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  varying mediump vec2 vTexCoord;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  \n
  void main()\n
  {\n
    mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
    vertexPosition.xyz *= uSize;\n
    vertexPosition = uMvpMatrix * vertexPosition;\n
    \n
    vTexCoord = aPosition + vec2(0.5);\n
    gl_Position = vertexPosition;\n
  }\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
  }\n
);

void AddQuadIndices( Vector< unsigned int >& indices, unsigned int rowIdx, unsigned int nextRowIdx )
{
  indices.PushBack( rowIdx );
  indices.PushBack( nextRowIdx + 1 );
  indices.PushBack( rowIdx + 1 );

  indices.PushBack( rowIdx );
  indices.PushBack( nextRowIdx );
  indices.PushBack( nextRowIdx + 1 );
}

Geometry GenerateGeometry( const Vector< Vector2 >& vertices, const Vector< unsigned int >& indices )
{
  Property::Map vertexFormat;
  vertexFormat[ "aPosition" ] = Property::VECTOR2;
  PropertyBuffer vertexPropertyBuffer = PropertyBuffer::New( vertexFormat, vertices.Size() );
  if( vertices.Size() > 0 )
  {
    vertexPropertyBuffer.SetData( &vertices[ 0 ] );
  }

  Property::Map indexFormat;
  indexFormat[ "indices" ] = Property::INTEGER;
  PropertyBuffer indexPropertyBuffer = PropertyBuffer::New( indexFormat, indices.Size() );
  if( indices.Size() > 0 )
  {
    indexPropertyBuffer.SetData( &indices[ 0 ] );
  }

  // Create the geometry object
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexPropertyBuffer );
  geometry.SetIndexBuffer( indexPropertyBuffer );

  return geometry;
}

Geometry CreateGeometry( RendererFactoryCache& factoryCache, ImageDimensions gridSize )
{
  Geometry geometry;

  if( gridSize == ImageDimensions( 1, 1 ) )
  {
    geometry = factoryCache.GetGeometry( RendererFactoryCache::QUAD_GEOMETRY );
    if( !geometry )
    {
      geometry =  factoryCache.CreateQuadGeometry();
      factoryCache.SaveGeometry( RendererFactoryCache::QUAD_GEOMETRY, geometry );
    }
  }
  else
  {
    uint16_t gridWidth = gridSize.GetWidth();
    uint16_t gridHeight = gridSize.GetHeight();

    // Create vertices
    Vector< Vector2 > vertices;
    vertices.Reserve( ( gridWidth + 1 ) * ( gridHeight + 1 ) );

    for( int y = 0; y < gridHeight + 1; ++y )
    {
      for( int x = 0; x < gridWidth + 1; ++x )
      {
        vertices.PushBack( Vector2( (float)x/gridWidth - 0.5f, (float)y/gridHeight  - 0.5f) );
      }
    }

    // Create indices
    Vector< unsigned int > indices;
    indices.Reserve( gridWidth * gridHeight * 6 );

    unsigned int rowIdx     = 0;
    unsigned int nextRowIdx = gridWidth + 1;
    for( int y = 0; y < gridHeight; ++y, ++nextRowIdx, ++rowIdx )
    {
      for( int x = 0; x < gridWidth; ++x, ++nextRowIdx, ++rowIdx )
      {
        AddQuadIndices( indices, rowIdx, nextRowIdx );
      }
    }

    return GenerateGeometry( vertices, indices );
  }

  return geometry;
}

} //unnamed namespace

ImageRenderer::ImageRenderer()
: ControlRenderer(),
  mDesiredSize(),
  mFittingMode( FittingMode::DEFAULT ),
  mSamplingMode( SamplingMode::DEFAULT )
{
}

ImageRenderer::~ImageRenderer()
{
}

void ImageRenderer::DoInitialize( RendererFactoryCache& factoryCache, const Property::Map& propertyMap )
{
  Initialize(factoryCache);

  Property::Value* imageURLValue = propertyMap.Find( IMAGE_URL_NAME );
  if( imageURLValue )
  {
    imageURLValue->Get( mImageUrl );
    if( !mImageUrl.empty() )
    {
      mImage.Reset();
    }

    Property::Value* fittingValue = propertyMap.Find( IMAGE_FITTING_MODE );
    if( fittingValue )
    {
      std::string fitting;
      fittingValue->Get( fitting );

      mFittingMode = FittingMode::DEFAULT;
      if( fitting == SHRINK_TO_FIT )
      {
        mFittingMode = FittingMode::SHRINK_TO_FIT;
      }
      else if( fitting == SCALE_TO_FILL )
      {
        mFittingMode = FittingMode::SCALE_TO_FILL;
      }
      else if( fitting == FIT_WIDTH )
      {
        mFittingMode = FittingMode::FIT_WIDTH;
      }
      else if( fitting == FIT_HEIGHT )
      {
        mFittingMode = FittingMode::FIT_HEIGHT;
      }
      else if( fitting == DEFAULT )
      {
        mFittingMode = FittingMode::DEFAULT;
      }
      else
      {
        DALI_ASSERT_ALWAYS("Unknown fitting mode");
      }
    }

    Property::Value* samplingValue = propertyMap.Find( IMAGE_SAMPLING_MODE );
    if( samplingValue )
    {
      std::string sampling;
      samplingValue->Get( sampling );

      mSamplingMode = SamplingMode::DEFAULT;
      if( sampling == BOX )
      {
        mSamplingMode = SamplingMode::BOX;
      }
      else if( sampling == NEAREST )
      {
        mSamplingMode = SamplingMode::NEAREST;
      }
      else if( sampling == LINEAR )
      {
        mSamplingMode = SamplingMode::LINEAR;
      }
      else if( sampling == BOX_THEN_NEAREST )
      {
        mSamplingMode = SamplingMode::BOX_THEN_NEAREST;
      }
      else if( sampling == BOX_THEN_LINEAR )
      {
        mSamplingMode = SamplingMode::BOX_THEN_LINEAR;
      }
      else if( sampling == NO_FILTER )
      {
        mSamplingMode = SamplingMode::NO_FILTER;
      }
      else if( sampling == DONT_CARE )
      {
        mSamplingMode = SamplingMode::DONT_CARE;
      }
      else if( sampling == DEFAULT )
      {
        mSamplingMode = SamplingMode::DEFAULT;
      }
      else
      {
        DALI_ASSERT_ALWAYS("Unknown sampling mode");
      }
    }

    int desiredWidth = 0;
    Property::Value* desiredWidthValue = propertyMap.Find( IMAGE_DESIRED_WIDTH );
    if( desiredWidthValue )
    {
      desiredWidthValue->Get( desiredWidth );
    }

    int desiredHeight = 0;
    Property::Value* desiredHeightValue = propertyMap.Find( IMAGE_DESIRED_HEIGHT );
    if( desiredHeightValue )
    {
      desiredHeightValue->Get( desiredHeight );
    }

    mDesiredSize = ImageDimensions( desiredWidth, desiredHeight );
  }
}

void ImageRenderer::SetSize( const Vector2& size )
{
  ControlRenderer::SetSize( size );
}

void ImageRenderer::GetNaturalSize( Vector2& naturalSize ) const
{
  if(mImage)
  {
    naturalSize.x = mImage.GetWidth();
    naturalSize.y = mImage.GetHeight();
    return;
  }
  else if( mDesiredSize.GetWidth()>0 && mDesiredSize.GetHeight()>0)
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

void ImageRenderer::SetClipRect( const Rect<int>& clipRect )
{
  ControlRenderer::SetClipRect( clipRect );
}

void ImageRenderer::SetOffset( const Vector2& offset )
{
}

void ImageRenderer::DoSetOnStage( Actor& actor )
{
  if( !mImageUrl.empty() && !mImage )
  {
    mImage = Dali::ResourceImage::New( mImageUrl, mDesiredSize, mFittingMode, mSamplingMode );
  }

  ApplyImageToSampler();
}

void ImageRenderer::DoSetOffStage( Actor& actor )
{
  //If we own the image then make sure we release it when we go off stage
  if( !mImageUrl.empty() )
  {
    mImage.Reset();
  }
}

void ImageRenderer::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( RENDERER_TYPE, RENDERER_TYPE_VALUE );
  if( !mImageUrl.empty() )
  {
    map.Insert( IMAGE_URL_NAME, mImageUrl );
    map.Insert( IMAGE_DESIRED_WIDTH, mDesiredSize.GetWidth() );
    map.Insert( IMAGE_DESIRED_HEIGHT, mDesiredSize.GetHeight() );
  }
  else if( mImage )
  {
    map.Insert( IMAGE_DESIRED_WIDTH, static_cast<int>(mImage.GetWidth()) );
    map.Insert( IMAGE_DESIRED_HEIGHT, static_cast<int>(mImage.GetHeight()) );

    ResourceImage resourceImage = ResourceImage::DownCast(mImage);
    if( resourceImage )
    {
      map.Insert( IMAGE_URL_NAME, resourceImage.GetUrl() );
    }
  }

  switch( mFittingMode )
  {
    case Dali::FittingMode::FIT_HEIGHT:
    {
      map.Insert( IMAGE_FITTING_MODE, FIT_HEIGHT );
      break;
    }
    case Dali::FittingMode::FIT_WIDTH:
    {
      map.Insert( IMAGE_FITTING_MODE, FIT_WIDTH );
      break;
    }
    case Dali::FittingMode::SCALE_TO_FILL:
    {
      map.Insert( IMAGE_FITTING_MODE, SCALE_TO_FILL );
      break;
    }
    case Dali::FittingMode::SHRINK_TO_FIT:
    {
      map.Insert( IMAGE_FITTING_MODE, SHRINK_TO_FIT );
      break;
    }
    default:
    {
      map.Insert( IMAGE_FITTING_MODE, DEFAULT );
      break;
    }
  }

  switch( mSamplingMode )
  {
    case Dali::SamplingMode::BOX:
    {
      map.Insert( IMAGE_SAMPLING_MODE, BOX );
      break;
    }
    case Dali::SamplingMode::NEAREST:
    {
      map.Insert( IMAGE_SAMPLING_MODE, NEAREST );
      break;
    }
    case Dali::SamplingMode::LINEAR:
    {
      map.Insert( IMAGE_SAMPLING_MODE, LINEAR );
      break;
    }
    case Dali::SamplingMode::BOX_THEN_LINEAR:
    {
      map.Insert( IMAGE_SAMPLING_MODE, BOX_THEN_LINEAR );
      break;
    }
    case Dali::SamplingMode::BOX_THEN_NEAREST:
    {
      map.Insert( IMAGE_SAMPLING_MODE, BOX_THEN_NEAREST );
      break;
    }
    case Dali::SamplingMode::NO_FILTER:
    {
      map.Insert( IMAGE_SAMPLING_MODE, NO_FILTER );
      break;
    }
    case Dali::SamplingMode::DONT_CARE:
    {
      map.Insert( IMAGE_SAMPLING_MODE, DONT_CARE );
      break;
    }
    default:
    {
      map.Insert( IMAGE_SAMPLING_MODE, DEFAULT );
      break;
    }
  }
}

void ImageRenderer::Initialize( RendererFactoryCache& factoryCache )
{
  if( !mImpl->mCustomShader )
  {
    mImpl->mGeometry = CreateGeometry( factoryCache, ImageDimensions( 1, 1 ) );

    mImpl->mShader = factoryCache.GetShader( RendererFactoryCache::IMAGE_SHADER );

    if( !mImpl->mShader )
    {
      mImpl->mShader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
      factoryCache.SaveShader( RendererFactoryCache::IMAGE_SHADER, mImpl->mShader );
    }
  }
  else
  {
    mImpl->mGeometry = CreateGeometry( factoryCache, mImpl->mCustomShader->mGridSize );

    if( mImpl->mCustomShader->mVertexShader.empty() && mImpl->mCustomShader->mFragmentShader.empty() )
    {
      mImpl->mShader = factoryCache.GetShader( RendererFactoryCache::IMAGE_SHADER );

      if( !mImpl->mShader )
      {
        mImpl->mShader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
        factoryCache.SaveShader( RendererFactoryCache::IMAGE_SHADER, mImpl->mShader );
      }
    }
    else
    {
      mImpl->mShader = Shader::New( mImpl->mCustomShader->mVertexShader.empty() ? VERTEX_SHADER : mImpl->mCustomShader->mVertexShader,
                                    mImpl->mCustomShader->mFragmentShader.empty() ? FRAGMENT_SHADER : mImpl->mCustomShader->mFragmentShader,
                                    mImpl->mCustomShader->mHints );
    }
  }

  if( mImpl->mRenderer )
  {
    mImpl->mRenderer.SetGeometry( mImpl->mGeometry );
    Material material = mImpl->mRenderer.GetMaterial();
    if( material )
    {
      material.SetShader( mImpl->mShader );
    }
  }
}

void ImageRenderer::SetImage( const std::string& imageUrl )
{
  SetImage( imageUrl, 0, 0, Dali::FittingMode::DEFAULT, Dali::SamplingMode::DEFAULT );
}

void ImageRenderer::SetImage( const std::string& imageUrl, int desiredWidth, int desiredHeight, Dali::FittingMode::Type fittingMode, Dali::SamplingMode::Type samplingMode )
{
  if( mImageUrl != imageUrl )
  {
    mImageUrl = imageUrl;
    mDesiredSize = ImageDimensions( desiredWidth, desiredHeight );
    mFittingMode = fittingMode;
    mSamplingMode = samplingMode;

    if( !mImageUrl.empty() && mImpl->mIsOnStage )
    {
      mImage = Dali::ResourceImage::New( mImageUrl, mDesiredSize, mFittingMode, mSamplingMode );
      ApplyImageToSampler();
    }
    else
    {
      mImage.Reset();
    }
  }
}

void ImageRenderer::SetImage( Image image )
{
  if( mImage != image )
  {
    mImageUrl.clear();
    mDesiredSize = ImageDimensions();
    mFittingMode = FittingMode::DEFAULT;
    mSamplingMode = SamplingMode::DEFAULT;
    mImage = image;

    if( mImage && mImpl->mIsOnStage )
    {
      ApplyImageToSampler();
    }
  }
}

Image ImageRenderer::GetImage() const
{
  return mImage;
}

void ImageRenderer::ApplyImageToSampler()
{
  if( mImage )
  {
    Material material = mImpl->mRenderer.GetMaterial();
    if( material )
    {
      for( std::size_t i = 0; i < material.GetNumberOfSamplers(); ++i )
      {
        Sampler sampler = material.GetSamplerAt( i );
        if( sampler.GetUniformName() == TEXTURE_UNIFORM_NAME )
        {
          sampler.SetImage( mImage );
          return;
        }
      }

      Sampler sampler = Sampler::New( mImage, TEXTURE_UNIFORM_NAME );
      material.AddSampler( sampler );
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
