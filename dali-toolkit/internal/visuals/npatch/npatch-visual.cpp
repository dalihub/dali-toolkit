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
#include "npatch-visual.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/platform-abstraction.h>
#include <dali/public-api/images/buffer-image.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/devel-api/images/texture-set-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/devel-api/visual-factory/devel-visual-properties.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>


namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
const char * const BORDER_ONLY("borderOnly");

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  varying mediump vec2 vTexCoord;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  uniform mediump vec2 uNinePatchFactorsX[ FACTOR_SIZE_X ];\n
  uniform mediump vec2 uNinePatchFactorsY[ FACTOR_SIZE_Y ];\n
  \n

  //Visual size and offset
  uniform mediump vec2 offset;\n
  uniform mediump vec2 size;\n
  uniform mediump vec4 offsetSizeMode;\n
  uniform mediump vec2 origin;\n
  uniform mediump vec2 anchorPoint;\n

  void main()\n
  {\n
    mediump vec2 fixedFactor  = vec2( uNinePatchFactorsX[ int( ( aPosition.x + 1.0 ) * 0.5 ) ].x, uNinePatchFactorsY[ int( ( aPosition.y + 1.0 ) * 0.5 ) ].x );\n
    mediump vec2 stretch      = vec2( uNinePatchFactorsX[ int( ( aPosition.x       ) * 0.5 ) ].y, uNinePatchFactorsY[ int( ( aPosition.y       ) * 0.5 ) ].y );\n
    \n
    mediump vec2 fixedTotal   = vec2( uNinePatchFactorsX[ FACTOR_SIZE_X - 1 ].x, uNinePatchFactorsY[ FACTOR_SIZE_Y - 1 ].x );\n
    mediump vec2 stretchTotal = vec2( uNinePatchFactorsX[ FACTOR_SIZE_X - 1 ].y, uNinePatchFactorsY[ FACTOR_SIZE_Y - 1 ].y );\n
    \n

    vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );\n
    vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);\n

    mediump vec4 vertexPosition = vec4( ( fixedFactor + ( visualSize.xy - fixedTotal ) * stretch / stretchTotal ) +  anchorPoint*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );\n
    vertexPosition.xy -= visualSize.xy * vec2( 0.5, 0.5 );\n

    vertexPosition = uMvpMatrix * vertexPosition;\n
    \n
    vTexCoord = ( fixedFactor + stretch ) / ( fixedTotal + stretchTotal );\n
    \n
    gl_Position = vertexPosition;\n
  }\n
);

const char* VERTEX_SHADER_3X3 = DALI_COMPOSE_SHADER(
    attribute mediump vec2 aPosition;\n
    varying mediump vec2 vTexCoord;\n
    uniform mediump mat4 uModelMatrix;\n
    uniform mediump mat4 uMvpMatrix;\n
    uniform mediump vec3 uSize;\n
    uniform mediump vec2 uFixed[ 3 ];\n
    uniform mediump vec2 uStretchTotal;\n
    \n

    //Visual size and offset
    uniform mediump vec2 offset;\n
    uniform mediump vec2 size;\n
    uniform mediump vec4 offsetSizeMode;\n
    uniform mediump vec2 origin;\n
    uniform mediump vec2 anchorPoint;\n

    void main()\n
    {\n
      vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );\n
      vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);\n

      mediump vec2 scale        = vec2( length( uModelMatrix[ 0 ].xyz ), length( uModelMatrix[ 1 ].xyz ) );\n
      mediump vec2 size         = visualSize.xy * scale;\n
      \n
      mediump vec2 fixedFactor  = vec2( uFixed[ int( ( aPosition.x + 1.0 ) * 0.5 ) ].x, uFixed[ int( ( aPosition.y  + 1.0 ) * 0.5 ) ].y );\n
      mediump vec2 stretch      = floor( aPosition * 0.5 );\n
      mediump vec2 fixedTotal   = uFixed[ 2 ];\n
      \n
      mediump vec4 vertexPosition = vec4( fixedFactor + ( size - fixedTotal ) * stretch, 0.0, 1.0 );
      vertexPosition.xy -= size * vec2( 0.5, 0.5 );\n
      vertexPosition.xy =  vertexPosition.xy / scale + anchorPoint*size + (visualOffset + origin)*uSize.xy;\
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
Geometry GenerateGeometry( const Vector< Vector2 >& vertices, const Vector< unsigned short >& indices )
{
  Property::Map vertexFormat;
  vertexFormat[ "aPosition" ] = Property::VECTOR2;
  PropertyBuffer vertexPropertyBuffer = PropertyBuffer::New( vertexFormat );
  if( vertices.Size() > 0 )
  {
    vertexPropertyBuffer.SetData( &vertices[ 0 ], vertices.Size() );
  }

  // Create the geometry object
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexPropertyBuffer );
  if( indices.Size() > 0 )
  {
    geometry.SetIndexBuffer( &indices[ 0 ], indices.Size() );
  }


  return geometry;
}

/**
 * @brief Adds the indices to form a quad composed off two triangles where the indices are organised in a grid
 *
 * @param[out] indices     The indices to add to
 * @param[in]  rowIdx      The row index to start the quad
 * @param[in]  nextRowIdx  The index to the next row
 */
void AddQuadIndices( Vector< unsigned short >& indices, unsigned int rowIdx, unsigned int nextRowIdx )
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

void RegisterStretchProperties( Renderer& renderer, const char * uniformName, const NinePatchImage::StretchRanges& stretchPixels, uint16_t imageExtent)
{
  uint16_t prevEnd = 0;
  uint16_t prevFix = 0;
  uint16_t prevStretch = 0;
  unsigned int i = 1;
  for( NinePatchImage::StretchRanges::ConstIterator it = stretchPixels.Begin(); it != stretchPixels.End(); ++it, ++i )
  {
    uint16_t start = it->GetX();
    uint16_t end = it->GetY();

    uint16_t fix = prevFix + start - prevEnd;
    uint16_t stretch = prevStretch + end - start;

    std::stringstream uniform;
    uniform << uniformName << "[" << i << "]";
    renderer.RegisterProperty( uniform.str(), Vector2( fix, stretch ) );

    prevEnd = end;
    prevFix = fix;
    prevStretch = stretch;
  }

  {
    prevFix += imageExtent - prevEnd;
    std::stringstream uniform;
    uniform << uniformName << "[" << i << "]";
    renderer.RegisterProperty( uniform.str(), Vector2( prevFix, prevStretch ) );
  }
}

} //unnamed namespace

/////////////////NPatchVisual////////////////

NPatchVisualPtr NPatchVisual::New( VisualFactoryCache& factoryCache )
{
  return new NPatchVisual( factoryCache );
}

NPatchVisualPtr NPatchVisual::New( VisualFactoryCache& factoryCache, const std::string& imageUrl, bool borderOnly )
{
  NPatchVisual* nPatchVisual = new NPatchVisual( factoryCache, borderOnly );
  nPatchVisual->mImageUrl = imageUrl;

  NinePatchImage image = NinePatchImage::New( imageUrl );
  nPatchVisual->InitializeFromImage( image );

  return nPatchVisual;
}

NPatchVisualPtr NPatchVisual::New( VisualFactoryCache& factoryCache, NinePatchImage image, bool borderOnly )
{
  NPatchVisual* nPatchVisual = new NPatchVisual( factoryCache, borderOnly );
  nPatchVisual->mImage = image;

  nPatchVisual->InitializeFromImage( image );

  return nPatchVisual;
}

NPatchVisual::NPatchVisual( VisualFactoryCache& factoryCache, bool borderOnly )
: Visual::Base( factoryCache ),
  mImage(),
  mCroppedImage(),
  mImageUrl(),
  mStretchPixelsX(),
  mStretchPixelsY(),
  mImageSize(),
  mBorderOnly( borderOnly )
{
}

NPatchVisual::~NPatchVisual()
{
}

void NPatchVisual::DoSetProperties( const Property::Map& propertyMap )
{
  Property::Value* imageURLValue = propertyMap.Find( Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME );
  if( imageURLValue )
  {
    //Read the borderOnly property first since InitialiseFromImage relies on mBorderOnly to be properly set
    Property::Value* borderOnlyValue = propertyMap.Find( Toolkit::ImageVisual::Property::BORDER_ONLY, BORDER_ONLY );
    if( borderOnlyValue )
    {
      borderOnlyValue->Get( mBorderOnly );
    }

    if( imageURLValue->Get( mImageUrl ) )
    {
      NinePatchImage nPatch = NinePatchImage::New( mImageUrl );
      InitializeFromImage( nPatch );
    }
    else
    {
      InitializeFromBrokenImage();
      DALI_LOG_ERROR( "The property '%s' is not a string\n", IMAGE_URL_NAME );
    }
  }
}

void NPatchVisual::GetNaturalSize( Vector2& naturalSize ) const
{
  if( mImage )
  {
    naturalSize.x = mImage.GetWidth();
    naturalSize.y = mImage.GetHeight();
  }
  else if( !mImageUrl.empty() )
  {
    ImageDimensions dimentions = ResourceImage::GetImageSize( mImageUrl );
    naturalSize.x = dimentions.GetWidth();
    naturalSize.y = dimentions.GetHeight();
  }
  else
  {
    naturalSize = Vector2::ZERO;
  }
}

Geometry NPatchVisual::CreateGeometry()
{
  Geometry geometry;
  if( mStretchPixelsX.Size() == 1 && mStretchPixelsY.Size() == 1 )
  {
    if( !mBorderOnly )
    {
      geometry = mFactoryCache.GetGeometry( VisualFactoryCache::NINE_PATCH_GEOMETRY );
      if( !geometry )
      {
        geometry = CreateGeometry( Uint16Pair( 3, 3 ) );
        mFactoryCache.SaveGeometry( VisualFactoryCache::NINE_PATCH_GEOMETRY, geometry );
      }
    }
    else
    {
      geometry = mFactoryCache.GetGeometry( VisualFactoryCache::NINE_PATCH_BORDER_GEOMETRY );
      if( !geometry )
      {
        geometry = CreateGeometryBorder( Uint16Pair( 3, 3 ) );
        mFactoryCache.SaveGeometry( VisualFactoryCache::NINE_PATCH_BORDER_GEOMETRY, geometry );
      }
    }
  }
  else if( mStretchPixelsX.Size() > 0 || mStretchPixelsY.Size() > 0)
  {
    Uint16Pair gridSize( 2 * mStretchPixelsX.Size() + 1,  2 * mStretchPixelsY.Size() + 1 );
    geometry = !mBorderOnly ? CreateGeometry( gridSize ) : CreateGeometryBorder( gridSize );
  }

  return geometry;
}

Shader NPatchVisual::CreateShader()
{
  Shader shader;
  if( !mImpl->mCustomShader )
  {
    if( mStretchPixelsX.Size() == 1 && mStretchPixelsY.Size() == 1 )
    {
      shader = mFactoryCache.GetShader( VisualFactoryCache::NINE_PATCH_SHADER );
      if( !shader )
      {
        shader = Shader::New( VERTEX_SHADER_3X3, FRAGMENT_SHADER );
        mFactoryCache.SaveShader( VisualFactoryCache::NINE_PATCH_SHADER, shader );
      }
    }
    else if( mStretchPixelsX.Size() > 0 || mStretchPixelsY.Size() > 0)
    {
      std::stringstream vertexShader;
      vertexShader << "#define FACTOR_SIZE_X " << mStretchPixelsX.Size() + 2 << "\n"
                   << "#define FACTOR_SIZE_Y " << mStretchPixelsY.Size() + 2 << "\n"
                   << VERTEX_SHADER;

      shader = Shader::New( vertexShader.str(), FRAGMENT_SHADER );
    }
  }
  else
  {
    const char* fragmentShader = FRAGMENT_SHADER;
    Dali::Shader::Hint::Value hints = Dali::Shader::Hint::NONE;

    if( !mImpl->mCustomShader->mFragmentShader.empty() )
    {
      fragmentShader = mImpl->mCustomShader->mFragmentShader.c_str();
    }
    hints = mImpl->mCustomShader->mHints;

    if( mStretchPixelsX.Size() == 1 && mStretchPixelsY.Size() == 1 )
    {
      shader = Shader::New( VERTEX_SHADER_3X3, fragmentShader, hints );
    }
    else if( mStretchPixelsX.Size() > 0 || mStretchPixelsY.Size() > 0)
    {
      std::stringstream vertexShader;
      vertexShader << "#define FACTOR_SIZE_X " << mStretchPixelsX.Size() + 2 << "\n"
                   << "#define FACTOR_SIZE_Y " << mStretchPixelsY.Size() + 2 << "\n"
                   << VERTEX_SHADER;

      shader = Shader::New( vertexShader.str(), fragmentShader, hints );
    }
  }

  return shader;
}

void NPatchVisual::InitializeRenderer()
{
  Geometry geometry = CreateGeometry();
  Shader shader = CreateShader();

  if( !geometry || !shader )
  {
    DALI_LOG_ERROR("The 9 patch image '%s' doesn't have any valid stretch borders and so is not a valid 9 patch image\n", mImageUrl.c_str() );
    InitializeFromBrokenImage();
  }

  TextureSet textureSet = TextureSet::New();
  mImpl->mRenderer = Renderer::New( geometry, shader );
  mImpl->mRenderer.SetTextures( textureSet );

  //Register transform properties
  mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
}


void NPatchVisual::DoSetOnStage( Actor& actor )
{
  if( !mCroppedImage )
  {
    if( !mImageUrl.empty() )
    {
      NinePatchImage nPatch = NinePatchImage::New( mImageUrl );
      InitializeFromImage( nPatch );
    }
    else if( mImage )
    {
      InitializeFromImage( mImage );
    }
  }

  //initialize the renderer after initializing from the image since we need to know the grid size from the image before creating the geometry
  InitializeRenderer();

  if( mCroppedImage )
  {
    ApplyImageToSampler();
  }

  actor.AddRenderer( mImpl->mRenderer );
}

void NPatchVisual::DoSetOffStage( Actor& actor )
{
  mCroppedImage.Reset();
  actor.RemoveRenderer( mImpl->mRenderer );
  mImpl->mRenderer.Reset();
}

void NPatchVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::VisualProperty::TYPE, Toolkit::Visual::IMAGE );
  if( !mImageUrl.empty() )
  {
    map.Insert( Toolkit::ImageVisual::Property::URL, mImageUrl );
  }
  else if( mImage )
  {
    map.Insert( Toolkit::ImageVisual::Property::URL, mImage.GetUrl() );
  }
  map.Insert( Toolkit::ImageVisual::Property::BORDER_ONLY, mBorderOnly );
}

void NPatchVisual::DoSetProperty( Dali::Property::Index index, const Dali::Property::Value& propertyValue )
{
  // TODO
}

Dali::Property::Value NPatchVisual::DoGetProperty( Dali::Property::Index index )
{
  // TODO
  return Dali::Property::Value();
}

void NPatchVisual::OnSetTransform()
{
  if( mImpl->mRenderer )
  {
    mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
  }
}


void NPatchVisual::ChangeRenderer( bool oldBorderOnly, size_t oldGridX, size_t oldGridY )
{
  //check to see if the border style has changed

  bool borderOnlyChanged = oldBorderOnly != mBorderOnly;
  bool gridChanged = oldGridX != mStretchPixelsX.Size() || oldGridY != mStretchPixelsY.Size();

  if( borderOnlyChanged || gridChanged )
  {
    Geometry geometry = CreateGeometry();
    if( geometry )
    {
      mImpl->mRenderer.SetGeometry( geometry );
    }
    else
    {
      InitializeFromBrokenImage();
    }
  }

  if( gridChanged )
  {
    Shader shader = CreateShader();
    TextureSet textureSet;
    if( shader )
    {
      textureSet = mImpl->mRenderer.GetTextures();
      if( !textureSet )
      {
        InitializeFromBrokenImage();
      }
      mImpl->mRenderer.SetShader( shader );
    }
  }
}

void NPatchVisual::InitializeFromImage( NinePatchImage nPatch )
{
  mCroppedImage = nPatch.CreateCroppedBufferImage();
  if( !mCroppedImage )
  {
    DALI_LOG_ERROR("'%s' specify a valid 9 patch image\n", mImageUrl.c_str() );
    InitializeFromBrokenImage();
    return;
  }

  mImageSize = ImageDimensions( mCroppedImage.GetWidth(), mCroppedImage.GetHeight() );

  mStretchPixelsX = nPatch.GetStretchPixelsX();
  mStretchPixelsY = nPatch.GetStretchPixelsY();
}

void NPatchVisual::InitializeFromBrokenImage()
{
  mCroppedImage = VisualFactoryCache::GetBrokenVisualImage();
  mImageSize = ImageDimensions( mCroppedImage.GetWidth(), mCroppedImage.GetHeight() );

  mStretchPixelsX.Clear();
  mStretchPixelsX.PushBack( Uint16Pair( 0, mImageSize.GetWidth() ) );
  mStretchPixelsY.Clear();
  mStretchPixelsY.PushBack( Uint16Pair( 0, mImageSize.GetHeight() ) );
}

void NPatchVisual::ApplyImageToSampler()
{
  TextureSet textureSet = mImpl->mRenderer.GetTextures();
  if( textureSet )
  {
    TextureSetImage( textureSet, 0u, mCroppedImage );

    if( mStretchPixelsX.Size() == 1 && mStretchPixelsY.Size() == 1 )
    {
      //special case for 9 patch
      Uint16Pair stretchX = mStretchPixelsX[ 0 ];
      Uint16Pair stretchY = mStretchPixelsY[ 0 ];

      uint16_t stretchWidth = stretchX.GetY() - stretchX.GetX();
      uint16_t stretchHeight = stretchY.GetY() - stretchY.GetX();

      mImpl->mRenderer.RegisterProperty( "uFixed[0]", Vector2::ZERO );
      mImpl->mRenderer.RegisterProperty( "uFixed[1]", Vector2( stretchX.GetX(), stretchY.GetX()) );
      mImpl->mRenderer.RegisterProperty( "uFixed[2]", Vector2( mImageSize.GetWidth() - stretchWidth, mImageSize.GetHeight() - stretchHeight ) );
      mImpl->mRenderer.RegisterProperty( "uStretchTotal", Vector2( stretchWidth, stretchHeight ) );
    }
    else
    {
      mImpl->mRenderer.RegisterProperty( "uNinePatchFactorsX[0]", Vector2::ZERO );
      mImpl->mRenderer.RegisterProperty( "uNinePatchFactorsY[0]", Vector2::ZERO );

      RegisterStretchProperties( mImpl->mRenderer, "uNinePatchFactorsX", mStretchPixelsX, mImageSize.GetWidth() );
      RegisterStretchProperties( mImpl->mRenderer, "uNinePatchFactorsY", mStretchPixelsY, mImageSize.GetHeight() );
    }
  }
}

Geometry NPatchVisual::CreateGeometry( Uint16Pair gridSize )
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
  Vector< unsigned short > indices;
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

Geometry NPatchVisual::CreateGeometryBorder( Uint16Pair gridSize )
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
  Vector< unsigned short > indices;
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
