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
#include "npatch-renderer.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/platform-abstraction.h>
#include <dali/public-api/images/buffer-image.h>
#include <dali/public-api/images/resource-image.h>

// INTERNAL IINCLUDES
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
const char * const RENDERER_TYPE_VALUE("n-patch-renderer");

const char * const IMAGE_URL_NAME("image-url");
const char * const BORDER_ONLY("border-only");

std::string TEXTURE_UNIFORM_NAME = "sTexture";

const char* VERTEX_SHADER_3X3 = DALI_COMPOSE_SHADER(
    attribute mediump vec2 aPosition;\n
    varying mediump vec2 vTexCoord;\n
    uniform mediump mat4 uModelMatrix;\n
    uniform mediump mat4 uMvpMatrix;\n
    uniform mediump vec3 uSize;\n
    uniform mediump vec2 uFixed[ 3 ];\n
    uniform mediump vec2 uStretchTotal;\n
    \n
    void main()\n
    {\n
      mediump vec2 scale        = vec2( length( uModelMatrix[ 0 ].xyz ), length( uModelMatrix[ 1 ].xyz ) );\n
      mediump vec2 size         = uSize.xy * scale;\n
      \n
      mediump vec2 fixedFactor  = vec2( uFixed[ int( ( aPosition.x + 1.0 ) * 0.5 ) ].x, uFixed[ int( ( aPosition.y  + 1.0 ) * 0.5 ) ].y );\n
      mediump vec2 stretch      = floor( aPosition * 0.5 );\n
      mediump vec2 fixedTotal   = uFixed[ 2 ];\n
      \n
      mediump vec4 vertexPosition = vec4( fixedFactor + ( size - fixedTotal ) * stretch, 0.0, 1.0 );\n
      vertexPosition.xy -= size * vec2( 0.5, 0.5 );\n
      vertexPosition.xy =  vertexPosition.xy / scale;\n
      \n
      vertexPosition = uMvpMatrix * vertexPosition;\n
      \n
      vTexCoord = ( fixedFactor + stretch * uStretchTotal ) / ( fixedTotal + uStretchTotal );\n
      \n
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

/**
 * @brief Creates the geometry formed from the vertices and indices
 *
 * @param[in]  vertices             The vertices to generate the geometry from
 * @param[in]  indices              The indices to generate the geometry from
 * @return The geometry formed from the vertices and indices
 */
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

/**
 * @brief Adds the indices to form a quad composed off two triangles where the indices are organised in a grid
 *
 * @param[out] indices     The indices to add to
 * @param[in]  rowIdx      The row index to start the quad
 * @param[in]  nextRowIdx  The index to the next row
 */
void AddQuadIndices( Vector< unsigned int >& indices, unsigned int rowIdx, unsigned int nextRowIdx )
{
  indices.PushBack( rowIdx );
  indices.PushBack( nextRowIdx + 1 );
  indices.PushBack( rowIdx + 1 );

  indices.PushBack( rowIdx );
  indices.PushBack( nextRowIdx );
  indices.PushBack( nextRowIdx + 1 );
}

void AddVertex( Vector< Vector2 >& vertices, unsigned int x, unsigned int y )
{
  vertices.PushBack( Vector2( x, y ) );
}

} //unnamed namespace

/////////////////NPatchRenderer////////////////

NPatchRenderer::NPatchRenderer()
: ControlRenderer(),
  mBorderOnly( false )
{
}

NPatchRenderer::~NPatchRenderer()
{
}

void NPatchRenderer::Initialize( RendererFactoryCache& factoryCache, const Property::Map& propertyMap )
{
  Initialize(factoryCache);

  Property::Value* imageURLValue = propertyMap.Find( IMAGE_URL_NAME );
  if( imageURLValue )
  {
    //Read the border-only property first since InitialiseFromImage relies on mBorderOnly to be properly set
    Property::Value* borderOnlyValue = propertyMap.Find( BORDER_ONLY );
    if( borderOnlyValue )
    {
      borderOnlyValue->Get( mBorderOnly );
    }

    if( imageURLValue->Get( mImageUrl ) )
    {
      NinePatchImage nPatch = NinePatchImage::New( mImageUrl );
      InitialiseFromImage( nPatch );
    }
    else
    {
      CreateErrorImage();
      DALI_LOG_ERROR( "The property '%s' is not a string\n", IMAGE_URL_NAME );
    }
  }
}

void NPatchRenderer::SetClipRect( const Rect<int>& clipRect )
{
  ControlRenderer::SetClipRect( clipRect );
  //ToDo: renderer responds to the clipRect change
}

void NPatchRenderer::SetOffset( const Vector2& offset )
{
  //ToDo: renderer applies the offset
}

void NPatchRenderer::DoSetOnStage( Actor& actor )
{
  if( !mCroppedImage )
  {
    if( !mImageUrl.empty() )
    {
      NinePatchImage nPatch = NinePatchImage::New( mImageUrl );
      InitialiseFromImage( nPatch );
    }
    else if( mImage )
    {
      InitialiseFromImage( mImage );
    }
  }

  if( mCroppedImage )
  {
    ApplyImageToSampler();
  }
}

void NPatchRenderer::DoSetOffStage( Actor& actor )
{
  mCroppedImage.Reset();
}

void NPatchRenderer::CreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( RENDERER_TYPE, RENDERER_TYPE_VALUE );
  if( !mImageUrl.empty() )
  {
    map.Insert( IMAGE_URL_NAME, mImageUrl );
  }
  else if( mImage )
  {
    map.Insert( IMAGE_URL_NAME, mImage.GetUrl() );
  }
  map.Insert( BORDER_ONLY, mBorderOnly );
}

void NPatchRenderer::Initialize( RendererFactoryCache& factoryCache )
{
  mNinePatchGeometry = factoryCache.GetGeometry( RendererFactoryCache::NINE_PATCH_GEOMETRY );
  if( !(mNinePatchGeometry) )
  {
    mNinePatchGeometry = CreateGeometry( Uint16Pair( 3, 3 ) );
    factoryCache.SaveGeometry( RendererFactoryCache::NINE_PATCH_GEOMETRY, mNinePatchGeometry );
  }

  mNinePatchBorderGeometry = factoryCache.GetGeometry( RendererFactoryCache::NINE_PATCH_BORDER_GEOMETRY );
  if( !(mNinePatchBorderGeometry) )
  {
    mNinePatchBorderGeometry = CreateGeometryBorder( Uint16Pair( 3, 3 ) );
    factoryCache.SaveGeometry( RendererFactoryCache::NINE_PATCH_BORDER_GEOMETRY, mNinePatchBorderGeometry );
  }

  mNinePatchShader = factoryCache.GetShader( RendererFactoryCache::NINE_PATCH_SHADER );
  if( !mNinePatchShader )
  {
    mNinePatchShader = Shader::New( VERTEX_SHADER_3X3, FRAGMENT_SHADER );
    factoryCache.SaveShader( RendererFactoryCache::NINE_PATCH_SHADER, mNinePatchShader );
  }

  mImpl->mGeometry = mNinePatchGeometry;
  mImpl->mShader = mNinePatchShader;

  mImageUrl.clear();
}

void NPatchRenderer::SetImage( const std::string& imageUrl, bool borderOnly )
{
  mBorderOnly = borderOnly;
  mImage.Reset();
  if( mImageUrl == imageUrl )
  {
    return;
  }

  mImageUrl = imageUrl;
  NinePatchImage nPatch = NinePatchImage::New( mImageUrl );
  InitialiseFromImage( nPatch );

  if( mCroppedImage && mImpl->mIsOnStage )
  {
    ApplyImageToSampler();
  }
}

void NPatchRenderer::SetImage( NinePatchImage image, bool borderOnly )
{
  mBorderOnly = borderOnly;
  mImageUrl.empty();
  if( mImage == image )
  {
    return;
  }

  mImage = image;
  InitialiseFromImage( mImage );

  if( mCroppedImage && mImpl->mIsOnStage )
  {
    ApplyImageToSampler();
  }
}

void NPatchRenderer::InitialiseFromImage( NinePatchImage nPatch )
{
  mCroppedImage = nPatch.CreateCroppedBufferImage();
  if( !mCroppedImage )
  {
    DALI_LOG_ERROR("'%s' specify a valid 9 patch image\n", mImageUrl.c_str() );
    CreateErrorImage();
    return;
  }

  mImageSize = ImageDimensions( mCroppedImage.GetWidth(), mCroppedImage.GetHeight() );

  mStretchPixelsX = nPatch.GetStretchPixelsX();
  mStretchPixelsY = nPatch.GetStretchPixelsY();

  if( mStretchPixelsX.Size() > 0 && mStretchPixelsY.Size() > 0 )
  {
    //only 9 patch supported for now
    mImpl->mGeometry = !mBorderOnly ? mNinePatchGeometry : mNinePatchBorderGeometry;
    mImpl->mShader = mNinePatchShader;
  }
}

void NPatchRenderer::CreateErrorImage()
{
  mImageSize = ImageDimensions( 1, 1 );

  BufferImage bufferImage = BufferImage::New( mImageSize.GetWidth(), mImageSize.GetHeight(), Pixel::RGBA8888 );
  mCroppedImage = bufferImage;
  PixelBuffer* pixbuf = bufferImage.GetBuffer();

  for( size_t i = 0; i < mImageSize.GetWidth() * mImageSize.GetHeight() * 4u; )
  {
    pixbuf[ i++ ] = 0;
    pixbuf[ i++ ] = 0;
    pixbuf[ i++ ] = 0;
    pixbuf[ i++ ] = 255;
  }

  mStretchPixelsX.Clear();
  mStretchPixelsX.PushBack( Uint16Pair( 0, mImageSize.GetWidth() ) );
  mStretchPixelsY.Clear();
  mStretchPixelsY.PushBack( Uint16Pair( 0, mImageSize.GetHeight() ) );

  mImpl->mGeometry = mNinePatchGeometry;
  mImpl->mShader = mNinePatchShader;
}

void NPatchRenderer::ApplyImageToSampler()
{
  Material material = mImpl->mRenderer.GetMaterial();
  if( material )
  {
    Sampler sampler;
    for( std::size_t i = 0; i < material.GetNumberOfSamplers(); ++i )
    {
      sampler = material.GetSamplerAt( i );
      if( sampler.GetUniformName() == TEXTURE_UNIFORM_NAME )
      {
        sampler.SetImage( mCroppedImage );
        break;
      }
    }
    if( !sampler )
    {
      sampler = Sampler::New( mCroppedImage, TEXTURE_UNIFORM_NAME );
      material.AddSampler( sampler );
    }

    if( mStretchPixelsX.Size() > 0 && mStretchPixelsY.Size() > 0 )
    {
      //only 9 patch supported for now
      Uint16Pair stretchX = mStretchPixelsX[ 0 ];
      Uint16Pair stretchY = mStretchPixelsY[ 0 ];

      uint16_t stretchWidth = stretchX.GetY() - stretchX.GetX();
      uint16_t stretchHeight = stretchY.GetY() - stretchY.GetX();

      sampler.RegisterProperty( "uFixed[0]", Vector2::ZERO );
      sampler.RegisterProperty( "uFixed[1]", Vector2( stretchX.GetX(), stretchY.GetX()) );
      sampler.RegisterProperty( "uFixed[2]", Vector2( mImageSize.GetWidth() - stretchWidth, mImageSize.GetHeight() - stretchHeight ) );
      sampler.RegisterProperty( "uStretchTotal", Vector2( stretchWidth, stretchHeight ) );
    }
  }
}

Geometry NPatchRenderer::CreateGeometry( Uint16Pair gridSize )
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
      AddVertex( vertices, x, y );
    }
  }

  // Create indices
  //TODO: compare performance with triangle strip when Geometry supports it
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

Geometry NPatchRenderer::CreateGeometryBorder( Uint16Pair gridSize )
{
  uint16_t gridWidth = gridSize.GetWidth();
  uint16_t gridHeight = gridSize.GetHeight();

  // Create vertices
  Vector< Vector2 > vertices;
  vertices.Reserve( ( gridWidth + 1 ) * ( gridHeight + 1 ) );

  //top
  int y = 0;
  for(; y < 2; ++y)
  {
    for( int x = 0; x < gridWidth + 1; ++x )
    {
      AddVertex( vertices, x, y );
    }
  }

  for(; y < gridHeight - 1; ++y)
  {
    //left
    AddVertex( vertices, 0, y );
    AddVertex( vertices, 1, y );

    //right
    AddVertex( vertices, gridWidth - 1, y );
    AddVertex( vertices, gridWidth, y );
  }

  //bottom
  for(; y < gridHeight + 1; ++y)
  {
    for( int x = 0; x < gridWidth + 1; ++x )
    {
      AddVertex( vertices, x, y );
    }
  }

  // Create indices
  //TODO: compare performance with triangle strip when Geometry supports it
  Vector< unsigned int > indices;
  indices.Reserve( gridWidth * gridHeight * 6 );

  //top
  unsigned int rowIdx     = 0 ;
  unsigned int nextRowIdx = gridWidth + 1;
  for( int x = 0; x < gridWidth; ++x, ++nextRowIdx, ++rowIdx )
  {
    AddQuadIndices( indices, rowIdx, nextRowIdx );
  }

  if(gridHeight > 2)
  {
    rowIdx     = gridWidth + 1;
    nextRowIdx = ( gridWidth + 1 ) * 2;

    unsigned increment = gridWidth - 1;
    if(gridHeight > 3)
    {
      increment = 2;
      //second row left
      AddQuadIndices( indices, rowIdx, nextRowIdx );

      rowIdx     = gridWidth * 2;
      nextRowIdx = ( gridWidth + 1 ) * 2 + 2;
      //second row right
      AddQuadIndices( indices, rowIdx, nextRowIdx );

      //left and right
      rowIdx     = nextRowIdx - 2;
      nextRowIdx = rowIdx + 4;
      for(int y = 2; y < 2*(gridHeight - 3); ++y, rowIdx += 2, nextRowIdx += 2)
      {
        AddQuadIndices( indices, rowIdx, nextRowIdx );
      }
    }

    //second row left
    AddQuadIndices( indices, rowIdx, nextRowIdx );

    rowIdx     += increment;
    nextRowIdx += gridWidth - 1;
    //second row right
    AddQuadIndices( indices, rowIdx, nextRowIdx );
  }

  //bottom
  rowIdx     = nextRowIdx - gridWidth + 1;
  nextRowIdx = rowIdx + gridWidth + 1;
  for( int x = 0; x < gridWidth; ++x, ++nextRowIdx, ++rowIdx )
  {
    AddQuadIndices( indices, rowIdx, nextRowIdx );
  }

  return GenerateGeometry( vertices, indices );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
