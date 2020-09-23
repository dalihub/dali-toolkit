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
 *
 */

// CLASS HEADER
#include "npatch-visual.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/internal/visuals/npatch-loader.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/rendering-addon.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  varying mediump vec2 vTexCoord;\n
  varying mediump vec2 vMaskTexCoord;\n
  uniform highp   mat4 uMvpMatrix;\n
  uniform highp   vec3 uSize;\n
  uniform mediump vec2 uNinePatchFactorsX[ FACTOR_SIZE_X ];\n
  uniform mediump vec2 uNinePatchFactorsY[ FACTOR_SIZE_Y ];\n
  \n

  // Visual size and offset
  uniform mediump vec2 offset;\n
  uniform highp   vec2 size;\n
  uniform mediump vec4 offsetSizeMode;\n
  uniform mediump vec2 origin;\n
  uniform mediump vec2 anchorPoint;\n
  uniform mediump vec2 extraSize;\n

  void main()\n
  {\n
    mediump vec2 fixedFactor  = vec2( uNinePatchFactorsX[ int( ( aPosition.x + 1.0 ) * 0.5 ) ].x, uNinePatchFactorsY[ int( ( aPosition.y + 1.0 ) * 0.5 ) ].x );\n
    mediump vec2 stretch      = vec2( uNinePatchFactorsX[ int( ( aPosition.x       ) * 0.5 ) ].y, uNinePatchFactorsY[ int( ( aPosition.y       ) * 0.5 ) ].y );\n
    \n
    mediump vec2 fixedTotal   = vec2( uNinePatchFactorsX[ FACTOR_SIZE_X - 1 ].x, uNinePatchFactorsY[ FACTOR_SIZE_Y - 1 ].x );\n
    mediump vec2 stretchTotal = vec2( uNinePatchFactorsX[ FACTOR_SIZE_X - 1 ].y, uNinePatchFactorsY[ FACTOR_SIZE_Y - 1 ].y );\n
    \n
    vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw ) + extraSize;\n
    vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);\n
    \n
    mediump vec4 gridPosition = vec4( fixedFactor + ( visualSize.xy - fixedTotal ) * stretch / stretchTotal, 0.0, 1.0 );\n
    mediump vec4 vertexPosition = gridPosition;\n
    vertexPosition.xy -= visualSize.xy * vec2( 0.5, 0.5 );\n
    vertexPosition.xy += anchorPoint*visualSize + (visualOffset + origin)*uSize.xy;\n
    vertexPosition = uMvpMatrix * vertexPosition;\n
    \n
    vTexCoord = ( fixedFactor + stretch ) / ( fixedTotal + stretchTotal );\n
    vMaskTexCoord = gridPosition.xy / visualSize;\n
    \n
    gl_Position = vertexPosition;\n
  }\n
);

const char* VERTEX_SHADER_3X3 = DALI_COMPOSE_SHADER(
    attribute mediump vec2 aPosition;\n
    varying mediump vec2 vTexCoord;\n
    varying mediump vec2 vMaskTexCoord;\n
    uniform highp   mat4 uMvpMatrix;\n
    uniform highp   vec3 uSize;\n
    uniform mediump vec2 uFixed[ 3 ];\n
    uniform mediump vec2 uStretchTotal;\n
    \n
    //Visual size and offset
    uniform mediump vec2 offset;\n
    uniform highp   vec2 size;\n
    uniform mediump vec4 offsetSizeMode;\n
    uniform mediump vec2 origin;\n
    uniform mediump vec2 anchorPoint;\n
    uniform mediump vec2 extraSize;\n
    \n
    void main()\n
    {\n
      vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw ) + extraSize;\n
      vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);\n
      \n
      mediump vec2 size         = visualSize.xy;\n
      \n
      mediump vec2 fixedFactor  = vec2( uFixed[ int( ( aPosition.x + 1.0 ) * 0.5 ) ].x, uFixed[ int( ( aPosition.y  + 1.0 ) * 0.5 ) ].y );\n
      mediump vec2 stretch      = floor( aPosition * 0.5 );\n
      mediump vec2 fixedTotal   = uFixed[ 2 ];\n
      \n
      mediump vec4 gridPosition = vec4( fixedFactor + ( size - fixedTotal ) * stretch, 0.0, 1.0 );\n
      mediump vec4 vertexPosition = gridPosition;\n
      vertexPosition.xy -= size * vec2( 0.5, 0.5 );\n
      vertexPosition.xy += anchorPoint*size + (visualOffset + origin)*uSize.xy;\n
      \n
      vertexPosition = uMvpMatrix * vertexPosition;\n
      \n
      vTexCoord = ( fixedFactor + stretch * uStretchTotal ) / ( fixedTotal + uStretchTotal );\n
      \n
      vMaskTexCoord = gridPosition.xy / size;\n
      gl_Position = vertexPosition;\n
    }\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  uniform lowp vec3 mixColor;\n
  uniform lowp float preMultipliedAlpha;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor * vec4( mixColor, 1.0 );\n
  }\n
);

const char* FRAGMENT_MASK_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  varying mediump vec2 vMaskTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform sampler2D sMask;\n
  uniform lowp vec4 uColor;\n
  uniform lowp vec3 mixColor;\n
  uniform lowp float preMultipliedAlpha;\n
  uniform mediump float auxiliaryImageAlpha;\n
  \n
  void main()\n
  {\n
      // Where mask image is transparent, all of background image must show through.
      // where mask image is opaque, only mask should be shown
      // where mask is translucent, less of background should be shown.
      // auxiliaryImageAlpha controls how much of mask is visible

      mediump vec4 color = texture2D( sTexture, vTexCoord );\n
      mediump vec4 mask  = texture2D( sMask, vMaskTexCoord );\n

      mediump vec3 mixedColor = color.rgb * mix( 1.0-mask.a, 1.0, 1.0-auxiliaryImageAlpha)
                                + mask.rgb*mask.a * auxiliaryImageAlpha;\n
      gl_FragColor = vec4(mixedColor,1.0) * uColor * vec4( mixColor, 1.0 );\n
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
  VertexBuffer vertexBuffer = VertexBuffer::New( vertexFormat );
  if( vertices.Size() > 0 )
  {
    vertexBuffer.SetData( &vertices[ 0 ], vertices.Size() );
  }

  // Create the geometry object
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer );
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

void RegisterStretchProperties( Renderer& renderer, const char * uniformName, const NPatchUtility::StretchRanges& stretchPixels, uint16_t imageExtent)
{
  uint16_t prevEnd = 0;
  uint16_t prevFix = 0;
  uint16_t prevStretch = 0;
  unsigned int i = 1;
  for( NPatchUtility::StretchRanges::ConstIterator it = stretchPixels.Begin(); it != stretchPixels.End(); ++it, ++i )
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

NPatchVisualPtr NPatchVisual::New( VisualFactoryCache& factoryCache, const VisualUrl& imageUrl, const Property::Map& properties )
{
  NPatchVisualPtr nPatchVisual( new NPatchVisual( factoryCache ) );
  nPatchVisual->mImageUrl = imageUrl;
  nPatchVisual->SetProperties( properties );

  return nPatchVisual;
}

NPatchVisualPtr NPatchVisual::New( VisualFactoryCache& factoryCache, const VisualUrl& imageUrl )
{
  NPatchVisualPtr nPatchVisual( new NPatchVisual( factoryCache ) );
  nPatchVisual->mImageUrl = imageUrl;

  return nPatchVisual;
}

void NPatchVisual::LoadImages()
{
  TextureManager& textureManager = mFactoryCache.GetTextureManager();
  bool synchronousLoading = mImpl->mFlags & Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;

  if( NPatchLoader::UNINITIALIZED_ID == mId && mImageUrl.IsLocalResource() )
  {
    bool preMultiplyOnLoad = IsPreMultipliedAlphaEnabled() && !mImpl->mCustomShader ? true : false;
    mId = mLoader.Load( textureManager, this, mImageUrl.GetUrl(), mBorder, preMultiplyOnLoad, synchronousLoading );

    const NPatchLoader::Data* data;
    if( mLoader.GetNPatchData( mId, data ) && data->loadCompleted )
    {
      EnablePreMultipliedAlpha( preMultiplyOnLoad );
    }
  }

  if( !mAuxiliaryPixelBuffer && mAuxiliaryUrl.IsValid() && mAuxiliaryUrl.IsLocalResource() )
  {
    // Load the auxiliary image
    auto preMultiplyOnLoading = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
    mAuxiliaryPixelBuffer = textureManager.LoadPixelBuffer( mAuxiliaryUrl, Dali::ImageDimensions(), FittingMode::DEFAULT,
                                                            SamplingMode::BOX_THEN_LINEAR, synchronousLoading,
                                                            this, true, preMultiplyOnLoading );
  }
}

void NPatchVisual::GetNaturalSize( Vector2& naturalSize )
{
  naturalSize.x = 0u;
  naturalSize.y = 0u;

  // load now if not already loaded
  const NPatchLoader::Data* data;
  if( mLoader.GetNPatchData( mId, data ) && data->loadCompleted )
  {
    naturalSize.x = data->croppedWidth;
    naturalSize.y = data->croppedHeight;
  }
  else
  {
    if( mImageUrl.IsValid() )
    {
      ImageDimensions dimensions = Dali::GetOriginalImageSize( mImageUrl.GetUrl() );
      if( dimensions != ImageDimensions( 0, 0 ) )
      {
        naturalSize.x = dimensions.GetWidth();
        naturalSize.y = dimensions.GetHeight();
      }
    }
  }

  if( mAuxiliaryPixelBuffer )
  {
    naturalSize.x = std::max( naturalSize.x, float(mAuxiliaryPixelBuffer.GetWidth()) );
    naturalSize.y = std::max( naturalSize.y, float(mAuxiliaryPixelBuffer.GetHeight()) );
  }
}

void NPatchVisual::DoSetProperties( const Property::Map& propertyMap )
{
  // URL is already passed in via constructor

  Property::Value* borderOnlyValue = propertyMap.Find( Toolkit::ImageVisual::Property::BORDER_ONLY, BORDER_ONLY );
  if( borderOnlyValue )
  {
    borderOnlyValue->Get( mBorderOnly );
  }

  Property::Value* borderValue = propertyMap.Find( Toolkit::ImageVisual::Property::BORDER, BORDER );
  if( borderValue && ! borderValue->Get( mBorder ) ) // If value exists and is rect, just set mBorder
  {
    // Not a rect so try vector4
    Vector4 border;
    if( borderValue->Get( border ) )
    {
      mBorder.left = static_cast< int >( border.x );
      mBorder.right = static_cast< int >( border.y );
      mBorder.bottom = static_cast< int >( border.z );
      mBorder.top = static_cast< int >( border.w );
    }
  }

  Property::Value* auxImage = propertyMap.Find( Toolkit::DevelImageVisual::Property::AUXILIARY_IMAGE, AUXILIARY_IMAGE_NAME );
  if( auxImage )
  {
    std::string url;
    if( auxImage->Get( url ) )
    {
      mAuxiliaryUrl = url;
    }
  }

  Property::Value* auxImageAlpha = propertyMap.Find( Toolkit::DevelImageVisual::Property::AUXILIARY_IMAGE_ALPHA, AUXILIARY_IMAGE_ALPHA_NAME );
  if( auxImageAlpha )
  {
    auxImageAlpha->Get( mAuxiliaryImageAlpha );
  }

  Property::Value* synchronousLoading = propertyMap.Find( Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, SYNCHRONOUS_LOADING );
  if( synchronousLoading )
  {
    bool sync = false;
    synchronousLoading->Get( sync );
    if( sync )
    {
      mImpl->mFlags |= Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
    }
    else
    {
      mImpl->mFlags &= ~Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
    }
  }
}

void NPatchVisual::DoSetOnScene( Actor& actor )
{
  // load when first go on stage
  LoadImages();

  const NPatchLoader::Data* data;
  if( mLoader.GetNPatchData( mId, data ) )
  {
    Geometry geometry = CreateGeometry();
    Shader shader = CreateShader();

    mImpl->mRenderer = Renderer::New( geometry, shader );

    mPlacementActor = actor;
    if( data->loadCompleted )
    {
      if( RenderingAddOn::Get().IsValid() )
      {
        RenderingAddOn::Get().SubmitRenderTask( mImpl->mRenderer, data->renderingMap );
      }

      ApplyTextureAndUniforms();
      actor.AddRenderer( mImpl->mRenderer );
      mPlacementActor.Reset();

      // npatch loaded and ready to display
      ResourceReady( Toolkit::Visual::ResourceStatus::READY );
    }
  }
}

void NPatchVisual::DoSetOffScene( Actor& actor )
{
  actor.RemoveRenderer( mImpl->mRenderer );
  mImpl->mRenderer.Reset();
  mPlacementActor.Reset();
}

void NPatchVisual::OnSetTransform()
{
  if( mImpl->mRenderer )
  {
    mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
  }
}

void NPatchVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  bool sync = IsSynchronousLoadingRequired();
  map.Insert( Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, sync );
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::Visual::N_PATCH );
  map.Insert( Toolkit::ImageVisual::Property::URL, mImageUrl.GetUrl() );
  map.Insert( Toolkit::ImageVisual::Property::BORDER_ONLY, mBorderOnly );
  map.Insert( Toolkit::ImageVisual::Property::BORDER, mBorder );

  if( mAuxiliaryUrl.IsValid() )
  {
    map.Insert( Toolkit::DevelImageVisual::Property::AUXILIARY_IMAGE, mAuxiliaryUrl.GetUrl() );
    map.Insert( Toolkit::DevelImageVisual::Property::AUXILIARY_IMAGE_ALPHA, mAuxiliaryImageAlpha );
  }
}

void NPatchVisual::DoCreateInstancePropertyMap( Property::Map& map ) const
{
  if( mAuxiliaryUrl.IsValid() )
  {
    map.Insert( Toolkit::DevelImageVisual::Property::AUXILIARY_IMAGE, mAuxiliaryUrl.GetUrl() );
    map.Insert( Toolkit::DevelImageVisual::Property::AUXILIARY_IMAGE_ALPHA, mAuxiliaryImageAlpha );
  }
}

NPatchVisual::NPatchVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache, Visual::FittingMode::FILL, Toolkit::Visual::N_PATCH ),
  mPlacementActor(),
  mLoader( factoryCache.GetNPatchLoader() ),
  mImageUrl(),
  mAuxiliaryUrl(),
  mId( NPatchLoader::UNINITIALIZED_ID ),
  mBorderOnly( false ),
  mBorder(),
  mAuxiliaryImageAlpha( 0.0f )
{
  EnablePreMultipliedAlpha( mFactoryCache.GetPreMultiplyOnLoad() );
}

NPatchVisual::~NPatchVisual()
{
}

Geometry NPatchVisual::CreateGeometry()
{
  Geometry geometry;
  const NPatchLoader::Data* data;
  if( mLoader.GetNPatchData( mId, data ) && data->loadCompleted )
  {
    if( data->stretchPixelsX.Size() == 1 && data->stretchPixelsY.Size() == 1 )
    {
      if( DALI_UNLIKELY( mBorderOnly ) )
      {
        geometry = GetNinePatchGeometry( VisualFactoryCache::NINE_PATCH_BORDER_GEOMETRY );
      }
      else
      {
        if( data->renderingMap )
        {
          uint32_t elementCount[2];
          geometry = RenderingAddOn::Get().CreateGeometryGrid(data->renderingMap, Uint16Pair(3, 3), elementCount );
          if( mImpl->mRenderer )
          {
            RenderingAddOn::Get().SubmitRenderTask(mImpl->mRenderer, data->renderingMap);
          }
        }
        else
        {
          geometry = GetNinePatchGeometry( VisualFactoryCache::NINE_PATCH_GEOMETRY );
        }
      }
    }
    else if( data->stretchPixelsX.Size() > 0 || data->stretchPixelsY.Size() > 0)
    {
      Uint16Pair gridSize( 2 * data->stretchPixelsX.Size() + 1,  2 * data->stretchPixelsY.Size() + 1 );
      if( !data->renderingMap )
      {
        geometry = !mBorderOnly ? CreateGridGeometry( gridSize ) : CreateBorderGeometry( gridSize );
      }
      else
      {
        uint32_t elementCount[2];
        geometry = !mBorderOnly ?
                   RenderingAddOn::Get().CreateGeometryGrid(data->renderingMap, gridSize, elementCount ) : CreateBorderGeometry(gridSize );
        if( mImpl->mRenderer )
        {
          RenderingAddOn::Get().SubmitRenderTask(mImpl->mRenderer, data->renderingMap);
        }
      }
    }
  }
  else
  {
    // no N patch data so use default geometry
    geometry = GetNinePatchGeometry( VisualFactoryCache::NINE_PATCH_GEOMETRY );
  }
  return geometry;
}

Shader NPatchVisual::CreateShader()
{
  Shader shader;
  const NPatchLoader::Data* data;
  // 0 is either no data (load failed?) or no stretch regions on image
  // for both cases we use the default shader
  NPatchUtility::StretchRanges::SizeType xStretchCount = 0;
  NPatchUtility::StretchRanges::SizeType yStretchCount = 0;

  auto fragmentShader = mAuxiliaryPixelBuffer ? FRAGMENT_MASK_SHADER
                                              : FRAGMENT_SHADER;
  auto shaderType = mAuxiliaryPixelBuffer ? VisualFactoryCache::NINE_PATCH_MASK_SHADER
                                          : VisualFactoryCache::NINE_PATCH_SHADER;

  // ask loader for the regions
  if( mLoader.GetNPatchData( mId, data ) )
  {
    xStretchCount = data->stretchPixelsX.Count();
    yStretchCount = data->stretchPixelsY.Count();
  }

  if( DALI_LIKELY( !mImpl->mCustomShader ) )
  {
    if( DALI_LIKELY( ( xStretchCount == 1 && yStretchCount == 1 ) ||
                     ( xStretchCount == 0 && yStretchCount == 0 ) ) )
    {
      shader = mFactoryCache.GetShader( shaderType );
      if( DALI_UNLIKELY( !shader ) )
      {
        shader = Shader::New( VERTEX_SHADER_3X3, fragmentShader );
        // Only cache vanilla 9 patch shaders
        mFactoryCache.SaveShader( shaderType, shader );
      }
    }
    else if( xStretchCount > 0 || yStretchCount > 0)
    {
      std::stringstream vertexShader;
      vertexShader << "#define FACTOR_SIZE_X " << xStretchCount + 2 << "\n"
                   << "#define FACTOR_SIZE_Y " << yStretchCount + 2 << "\n"
                   << VERTEX_SHADER;

      shader = Shader::New( vertexShader.str(), fragmentShader );
    }
  }
  else
  {
    Dali::Shader::Hint::Value hints = Dali::Shader::Hint::NONE;

    if( !mImpl->mCustomShader->mFragmentShader.empty() )
    {
      fragmentShader = mImpl->mCustomShader->mFragmentShader.c_str();
    }
    hints = mImpl->mCustomShader->mHints;

    /* Apply Custom Vertex Shader only if image is 9-patch */
    if( ( xStretchCount == 1 && yStretchCount == 1 ) ||
        ( xStretchCount == 0 && yStretchCount == 0 ) )
    {
      const char* vertexShader = VERTEX_SHADER_3X3;

      if( !mImpl->mCustomShader->mVertexShader.empty() )
      {
        vertexShader = mImpl->mCustomShader->mVertexShader.c_str();
      }
      shader = Shader::New( vertexShader, fragmentShader, hints );
    }
    else if( xStretchCount > 0 || yStretchCount > 0)
    {
      std::stringstream vertexShader;
      vertexShader << "#define FACTOR_SIZE_X " << xStretchCount + 2 << "\n"
                   << "#define FACTOR_SIZE_Y " << yStretchCount + 2 << "\n"
                   << VERTEX_SHADER;

      shader = Shader::New( vertexShader.str(), fragmentShader, hints );
    }
  }

  return shader;
}

void NPatchVisual::ApplyTextureAndUniforms()
{
  const NPatchLoader::Data* data;
  TextureSet textureSet;

  if( mLoader.GetNPatchData( mId, data ) && data->loadCompleted )
  {
    textureSet = data->textureSet;

    if( data->stretchPixelsX.Size() == 1 && data->stretchPixelsY.Size() == 1 )
    {
      //special case for 9 patch
      Uint16Pair stretchX = data->stretchPixelsX[ 0 ];
      Uint16Pair stretchY = data->stretchPixelsY[ 0 ];

      uint16_t stretchWidth = ( stretchX.GetY() >= stretchX.GetX() ) ? stretchX.GetY() - stretchX.GetX() : 0;
      uint16_t stretchHeight = ( stretchY.GetY() >= stretchY.GetX() ) ? stretchY.GetY() - stretchY.GetX() : 0;

      mImpl->mRenderer.RegisterProperty( "uFixed[0]", Vector2::ZERO );
      mImpl->mRenderer.RegisterProperty( "uFixed[1]", Vector2( stretchX.GetX(), stretchY.GetX()) );
      mImpl->mRenderer.RegisterProperty( "uFixed[2]", Vector2( data->croppedWidth - stretchWidth, data->croppedHeight - stretchHeight ) );
      mImpl->mRenderer.RegisterProperty( "uStretchTotal", Vector2( stretchWidth, stretchHeight ) );
    }
    else
    {
      mImpl->mRenderer.RegisterProperty( "uNinePatchFactorsX[0]", Vector2::ZERO );
      mImpl->mRenderer.RegisterProperty( "uNinePatchFactorsY[0]", Vector2::ZERO );

      RegisterStretchProperties( mImpl->mRenderer, "uNinePatchFactorsX", data->stretchPixelsX, data->croppedWidth );
      RegisterStretchProperties( mImpl->mRenderer, "uNinePatchFactorsY", data->stretchPixelsY, data->croppedHeight );
    }
  }
  else
  {
    DALI_LOG_ERROR("The N patch image '%s' is not a valid N patch image\n", mImageUrl.GetUrl().c_str() );
    textureSet = TextureSet::New();

    Texture croppedImage = mFactoryCache.GetBrokenVisualImage();
    textureSet.SetTexture( 0u, croppedImage );
    mImpl->mRenderer.RegisterProperty( "uFixed[0]", Vector2::ZERO );
    mImpl->mRenderer.RegisterProperty( "uFixed[1]", Vector2::ZERO );
    mImpl->mRenderer.RegisterProperty( "uFixed[2]", Vector2::ZERO );
    mImpl->mRenderer.RegisterProperty( "uStretchTotal", Vector2( croppedImage.GetWidth(), croppedImage.GetHeight() ) );
  }

  if( mAuxiliaryPixelBuffer )
  {
    // If the auxiliary image is smaller than the un-stretched NPatch, use CPU resizing to enlarge it to the
    // same size as the unstretched NPatch. This will give slightly higher quality results than just relying
    // on GL interpolation alone.
    if( mAuxiliaryPixelBuffer.GetWidth() < data->croppedWidth &&
        mAuxiliaryPixelBuffer.GetHeight() < data->croppedHeight )
    {
      mAuxiliaryPixelBuffer.Resize( data->croppedWidth, data->croppedHeight );
    }

    // Note, this resets mAuxiliaryPixelBuffer handle
    auto auxiliaryPixelData = Devel::PixelBuffer::Convert( mAuxiliaryPixelBuffer );

    auto texture = Texture::New( TextureType::TEXTURE_2D,
                                 auxiliaryPixelData.GetPixelFormat(), auxiliaryPixelData.GetWidth(),
                                 auxiliaryPixelData.GetHeight() );
    texture.Upload( auxiliaryPixelData );
    textureSet.SetTexture( 1, texture );
    mImpl->mRenderer.RegisterProperty( DevelImageVisual::Property::AUXILIARY_IMAGE_ALPHA,
                                       AUXILIARY_IMAGE_ALPHA_NAME, mAuxiliaryImageAlpha );
  }
  mImpl->mRenderer.SetTextures( textureSet );

  // Register transform properties
  mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
}

Geometry NPatchVisual::GetNinePatchGeometry( VisualFactoryCache::GeometryType subType )
{
  Geometry geometry = mFactoryCache.GetGeometry( subType );
  if( !geometry )
  {
    if( DALI_LIKELY( VisualFactoryCache::NINE_PATCH_GEOMETRY == subType ) )
    {
      geometry = CreateGridGeometry( Uint16Pair( 3, 3 ) );
    }
    else if( VisualFactoryCache::NINE_PATCH_BORDER_GEOMETRY == subType )
    {
      geometry = CreateBorderGeometry( Uint16Pair( 3, 3 ) );
    }
    mFactoryCache.SaveGeometry( subType, geometry );
  }
  return geometry;
}

Geometry NPatchVisual::CreateGridGeometry( Uint16Pair gridSize )
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

Geometry NPatchVisual::CreateBorderGeometry( Uint16Pair gridSize )
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

void NPatchVisual::SetResource()
{
  const NPatchLoader::Data* data;
  if( mImpl->mRenderer && mLoader.GetNPatchData( mId, data ) )
  {
    Geometry geometry = CreateGeometry();
    Shader shader = CreateShader();

    mImpl->mRenderer.SetGeometry( geometry );
    mImpl->mRenderer.SetShader( shader );

    Actor actor = mPlacementActor.GetHandle();
    if( actor )
    {
      ApplyTextureAndUniforms();
      actor.AddRenderer( mImpl->mRenderer );
      mPlacementActor.Reset();

      // npatch loaded and ready to display
      ResourceReady( Toolkit::Visual::ResourceStatus::READY );
    }
  }
}

void NPatchVisual::LoadComplete( bool loadSuccess, Devel::PixelBuffer pixelBuffer, const VisualUrl& url, bool preMultiplied )
{
  if( url.GetUrl() == mAuxiliaryUrl.GetUrl() )
  {
    mAuxiliaryPixelBuffer = pixelBuffer;
    const NPatchLoader::Data* data;
    if( mLoader.GetNPatchData( mId, data ) && data->loadCompleted )
    {
      SetResource();
    }
  }
  else
  {
    if( loadSuccess )
    {
      mLoader.SetNPatchData( mId, pixelBuffer );
      EnablePreMultipliedAlpha( preMultiplied );
    }

    if( mAuxiliaryPixelBuffer || !mAuxiliaryUrl.IsValid() )
    {
      SetResource();
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
