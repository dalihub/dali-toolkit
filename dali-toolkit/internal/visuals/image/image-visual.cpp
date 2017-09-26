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
#include <dali-toolkit/internal/visuals/image/image-visual.h>

// EXTERNAL HEADERS
#include <cstring> // for strlen()
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/images/native-image.h>
#include <dali/devel-api/images/texture-set-image.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>

// INTERNAL HEADERS
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/internal/visuals/texture-manager-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-url.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

// property names
const char * const IMAGE_FITTING_MODE( "fittingMode" );
const char * const IMAGE_SAMPLING_MODE( "samplingMode" );
const char * const IMAGE_DESIRED_WIDTH( "desiredWidth" );
const char * const IMAGE_DESIRED_HEIGHT( "desiredHeight" );
const char * const SYNCHRONOUS_LOADING( "synchronousLoading" );
const char * const IMAGE_ATLASING("atlasing");
const char * const ALPHA_MASK_URL("alphaMaskUrl");

// fitting modes
DALI_ENUM_TO_STRING_TABLE_BEGIN( FITTING_MODE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::FittingMode, SHRINK_TO_FIT )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::FittingMode, SCALE_TO_FILL )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::FittingMode, FIT_WIDTH )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::FittingMode, FIT_HEIGHT )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::FittingMode, DEFAULT )
DALI_ENUM_TO_STRING_TABLE_END( FITTING_MODE )

// sampling modes
DALI_ENUM_TO_STRING_TABLE_BEGIN( SAMPLING_MODE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::SamplingMode, BOX )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::SamplingMode, NEAREST )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::SamplingMode, LINEAR )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::SamplingMode, BOX_THEN_NEAREST )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::SamplingMode, BOX_THEN_LINEAR )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::SamplingMode, NO_FILTER )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::SamplingMode, DONT_CARE )
DALI_ENUM_TO_STRING_TABLE_END( SAMPLING_MODE )

// wrap modes
DALI_ENUM_TO_STRING_TABLE_BEGIN( WRAP_MODE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::WrapMode, DEFAULT )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::WrapMode, CLAMP_TO_EDGE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::WrapMode, REPEAT )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::WrapMode, MIRRORED_REPEAT )
DALI_ENUM_TO_STRING_TABLE_END( WRAP_MODE )

// release policies
DALI_ENUM_TO_STRING_TABLE_BEGIN( RELEASE_POLICY )
DALI_ENUM_TO_STRING_WITH_SCOPE( DevelImageVisual::ReleasePolicy, DETACHED )
DALI_ENUM_TO_STRING_WITH_SCOPE( DevelImageVisual::ReleasePolicy, DESTROYED )
DALI_ENUM_TO_STRING_WITH_SCOPE( DevelImageVisual::ReleasePolicy, NEVER )
DALI_ENUM_TO_STRING_TABLE_END( RELEASE_POLICY )

const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

const char* DEFAULT_SAMPLER_TYPENAME = "sampler2D";

const float PIXEL_ALIGN_ON = 1.0f;
const float PIXEL_ALIGN_OFF = 0.0f;

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  uniform mediump mat4 uModelMatrix;\n
  uniform mediump mat4 uViewMatrix;\n
  uniform mediump mat4 uProjection;\n
  uniform mediump vec3 uSize;\n
  uniform mediump vec4 pixelArea;
  varying mediump vec2 vTexCoord;\n
  uniform lowp float uPixelAligned;\n
  \n
  //Visual size and offset
  uniform mediump vec2 offset;\n
  uniform mediump vec2 size;\n
  uniform mediump vec4 offsetSizeMode;\n
  uniform mediump vec2 origin;\n
  uniform mediump vec2 anchorPoint;\n
\n
  vec4 ComputeVertexPosition()\n
  {\n
    vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );\n
    vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);\n
    return vec4( (aPosition + anchorPoint)*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );\n
  }\n
\n
  void main()\n
  {\n
    mediump vec4 vertexPosition = uViewMatrix * uModelMatrix * ComputeVertexPosition();\n
    vec4 alignedVertexPosition = vertexPosition;\n
    alignedVertexPosition.xy = floor ( vertexPosition.xy );\n // Pixel alignment
    vertexPosition = uProjection * mix( vertexPosition, alignedVertexPosition, uPixelAligned );\n
    vTexCoord = pixelArea.xy+pixelArea.zw*(aPosition + vec2(0.5) );\n
    gl_Position = vertexPosition;\n
  }\n
);

const char* FRAGMENT_SHADER_NO_ATLAS = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  uniform lowp vec3 mixColor;\n
  uniform lowp float opacity;\n
  uniform lowp float preMultipliedAlpha;\n
  \n
  lowp vec4 visualMixColor()\n
  {\n
    return vec4( mixColor * mix( 1.0, opacity, preMultipliedAlpha ), opacity );\n
  }\n
  void main()\n
  {\n
      gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor * visualMixColor();\n
  }\n
);

const char* FRAGMENT_SHADER_ATLAS_CLAMP = DALI_COMPOSE_SHADER(
    varying mediump vec2 vTexCoord;\n
    uniform sampler2D sTexture;\n
    uniform mediump vec4 uAtlasRect;\n
    uniform lowp vec4 uColor;\n
    uniform lowp vec3 mixColor;\n
    uniform lowp float opacity;\n
    uniform lowp float preMultipliedAlpha;\n
    \n
    lowp vec4 visualMixColor()\n
    {\n
        return vec4( mixColor * mix( 1.0, opacity, preMultipliedAlpha ), opacity );\n
    }\n
    \n
    void main()\n
    {\n
        mediump vec2 texCoord = clamp( mix( uAtlasRect.xy, uAtlasRect.zw, vTexCoord ), uAtlasRect.xy, uAtlasRect.zw );\n
        gl_FragColor = texture2D( sTexture, texCoord ) * uColor * visualMixColor();\n
     }\n
);

const char* FRAGMENT_SHADER_ATLAS_VARIOUS_WRAP = DALI_COMPOSE_SHADER(
    varying mediump vec2 vTexCoord;\n
    uniform sampler2D sTexture;\n
    uniform mediump vec4 uAtlasRect;\n
    // WrapMode -- 0: CLAMP; 1: REPEAT; 2: REFLECT;
    uniform lowp vec2 wrapMode;\n
    uniform lowp vec4 uColor;\n
    uniform lowp vec3 mixColor;\n
    uniform lowp float opacity;\n
    uniform lowp float preMultipliedAlpha;\n
    \n
    mediump float wrapCoordinate( mediump vec2 range, mediump float coordinate, lowp float wrap )\n
    {\n
      mediump float coord;\n
      if( wrap > 1.5 )\n // REFLECT
        coord = 1.0-abs(fract(coordinate*0.5)*2.0 - 1.0);\n
      else \n// warp == 0 or 1
        coord = mix(coordinate, fract( coordinate ), wrap);\n
      return clamp( mix(range.x, range.y, coord), range.x, range.y );
    }\n
    \n
    lowp vec4 visualMixColor()\n
    {\n
      return vec4( mixColor * mix( 1.0, opacity, preMultipliedAlpha ), opacity );\n
    }\n
    \n
    void main()\n
    {\n
        mediump vec2 texCoord = vec2( wrapCoordinate( uAtlasRect.xz, vTexCoord.x, wrapMode.x ),
                                      wrapCoordinate( uAtlasRect.yw, vTexCoord.y, wrapMode.y ) );\n
        gl_FragColor = texture2D( sTexture, texCoord ) * uColor * visualMixColor();\n
    }\n
);

Geometry CreateGeometry( VisualFactoryCache& factoryCache, ImageDimensions gridSize )
{
  Geometry geometry;

  if( gridSize == ImageDimensions( 1, 1 ) )
  {
    geometry = factoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );
  }
  else
  {
    geometry = VisualFactoryCache::CreateGridGeometry( gridSize );
  }

  return geometry;
}

} // unnamed namespace

ImageVisualPtr ImageVisual::New( VisualFactoryCache& factoryCache,
                                 const VisualUrl& imageUrl,
                                 const Property::Map& properties,
                                 ImageDimensions size,
                                 FittingMode::Type fittingMode,
                                 Dali::SamplingMode::Type samplingMode )
{
  ImageVisualPtr imageVisualPtr( new ImageVisual( factoryCache, imageUrl, size, fittingMode, samplingMode ) );
  imageVisualPtr->SetProperties( properties );
  return imageVisualPtr;
}

ImageVisualPtr ImageVisual::New( VisualFactoryCache& factoryCache,
                                 const VisualUrl& imageUrl,
                                 ImageDimensions size,
                                 FittingMode::Type fittingMode,
                                 Dali::SamplingMode::Type samplingMode )
{
  return new ImageVisual( factoryCache, imageUrl, size, fittingMode, samplingMode );
}

ImageVisualPtr ImageVisual::New( VisualFactoryCache& factoryCache, const Image& image )
{
  return new ImageVisual( factoryCache, image );
}

ImageVisual::ImageVisual( VisualFactoryCache& factoryCache,
                          const VisualUrl& imageUrl,
                          ImageDimensions size,
                          FittingMode::Type fittingMode,
                          Dali::SamplingMode::Type samplingMode )
: Visual::Base( factoryCache ),
  mImage(),
  mPixelArea( FULL_TEXTURE_RECT ),
  mPlacementActor(),
  mImageUrl( imageUrl ),
  mMaskingData( ),
  mDesiredSize( size ),
  mTextureId( TextureManager::INVALID_TEXTURE_ID ),
  mFittingMode( fittingMode ),
  mSamplingMode( samplingMode ),
  mWrapModeU( WrapMode::DEFAULT ),
  mWrapModeV( WrapMode::DEFAULT ),
  mReleasePolicy( DevelImageVisual::ReleasePolicy::DETACHED ),
  mAttemptAtlasing( false ),
  mLoading( false )
{
}

ImageVisual::ImageVisual( VisualFactoryCache& factoryCache, const Image& image )
: Visual::Base( factoryCache ),
  mImage( image ),
  mPixelArea( FULL_TEXTURE_RECT ),
  mPlacementActor(),
  mImageUrl(),
  mMaskingData( ),
  mDesiredSize(),
  mTextureId( TextureManager::INVALID_TEXTURE_ID ),
  mFittingMode( FittingMode::DEFAULT ),
  mSamplingMode( SamplingMode::DEFAULT ),
  mWrapModeU( WrapMode::DEFAULT ),
  mWrapModeV( WrapMode::DEFAULT ),
  mReleasePolicy( DevelImageVisual::ReleasePolicy::DETACHED ),
  mAttemptAtlasing( false ),
  mLoading( false )
{
}

ImageVisual::~ImageVisual()
{
  if( Stage::IsInstalled() )
  {
    if( mMaskingData )
    {
      // TextureManager could have been deleted before the actor that contains this
      // ImageVisual is destroyed (e.g. due to stage shutdown). Ensure the stage
      // is still valid before accessing texture manager.
      if( mMaskingData->mAlphaMaskId != TextureManager::INVALID_TEXTURE_ID )
      {
        TextureManager& textureManager = mFactoryCache.GetTextureManager();
        textureManager.Remove( mMaskingData->mAlphaMaskId );
      }
    }

    // ImageVisual destroyed so remove texture unless ReleasePolicy is set to never release
    if( ( mTextureId != TextureManager::INVALID_TEXTURE_ID  ) && ( mReleasePolicy != DevelImageVisual::ReleasePolicy::NEVER ) )
    {
      RemoveTexture();
    }
  }
}

void ImageVisual::DoSetProperties( const Property::Map& propertyMap )
{
  // Url is already received in constructor
  for( Property::Map::SizeType iter = 0; iter < propertyMap.Count(); ++iter )
  {
    KeyValuePair keyValue = propertyMap.GetKeyValue( iter );
    if( keyValue.first.type == Property::Key::INDEX )
    {
      DoSetProperty( keyValue.first.indexKey, keyValue.second );
    }
    else
    {
      if( keyValue.first == IMAGE_FITTING_MODE )
      {
        DoSetProperty( Toolkit::ImageVisual::Property::FITTING_MODE, keyValue.second );
      }
      else if( keyValue.first == IMAGE_SAMPLING_MODE )
      {
        DoSetProperty( Toolkit::ImageVisual::Property::SAMPLING_MODE, keyValue.second );
      }
      else if( keyValue.first == IMAGE_DESIRED_WIDTH )
      {
        DoSetProperty( Toolkit::ImageVisual::Property::DESIRED_WIDTH, keyValue.second );
      }
      else if( keyValue.first == IMAGE_DESIRED_HEIGHT )
      {
        DoSetProperty( Toolkit::ImageVisual::Property::DESIRED_HEIGHT, keyValue.second );
      }
      else if( keyValue.first == PIXEL_AREA_UNIFORM_NAME )
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
      else if( keyValue.first == SYNCHRONOUS_LOADING )
      {
        DoSetProperty( Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, keyValue.second );
      }
      else if( keyValue.first == IMAGE_ATLASING )
      {
        DoSetProperty( Toolkit::ImageVisual::Property::ATLASING, keyValue.second );
      }
      else if( keyValue.first == ALPHA_MASK_URL )
      {
        DoSetProperty( Toolkit::ImageVisual::Property::ALPHA_MASK_URL, keyValue.second );
      }
      else if( keyValue.first == MASK_CONTENT_SCALE_NAME )
      {
        DoSetProperty( Toolkit::ImageVisual::Property::MASK_CONTENT_SCALE, keyValue.second );
      }
      else if( keyValue.first == CROP_TO_MASK_NAME )
      {
        DoSetProperty( Toolkit::ImageVisual::Property::CROP_TO_MASK, keyValue.second );
      }
      else if( keyValue.first == RELEASE_POLICY_NAME )
      {
        DoSetProperty( Toolkit::DevelImageVisual::Property::RELEASE_POLICY, keyValue.second );
      }
    }
  }
}

void ImageVisual::DoSetProperty( Property::Index index, const Property::Value& value )
{
  switch( index )
  {
    case Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING:
    {
      bool sync;
      if( value.Get( sync ) )
      {
        if( sync )
        {
          mImpl->mFlags |= Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
        }
        else
        {
          mImpl->mFlags &= ~Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
        }
      }
      else
      {
        DALI_LOG_ERROR("ImageVisual: synchronousLoading property has incorrect type\n");
      }
      break;
    }

    case Toolkit::ImageVisual::Property::DESIRED_WIDTH:
    {
      float desiredWidth;
      if( value.Get( desiredWidth ) )
      {
        mDesiredSize.SetWidth( desiredWidth );
      }
      else
      {
        DALI_LOG_ERROR("ImageVisual: desiredWidth property has incorrect type\n");
      }
      break;
    }

    case Toolkit::ImageVisual::Property::DESIRED_HEIGHT:
    {
      float desiredHeight;
      if( value.Get( desiredHeight ) )
      {
        mDesiredSize.SetHeight( desiredHeight );
      }
      else
      {
        DALI_LOG_ERROR("ImageVisual: desiredHeight property has incorrect type\n");
      }
      break;
    }

    case Toolkit::ImageVisual::Property::FITTING_MODE:
    {
      int fittingMode;
      Scripting::GetEnumerationProperty( value, FITTING_MODE_TABLE, FITTING_MODE_TABLE_COUNT, fittingMode );
      mFittingMode = Dali::FittingMode::Type( fittingMode );
      break;
    }

    case Toolkit::ImageVisual::Property::SAMPLING_MODE:
    {
      int samplingMode;
      Scripting::GetEnumerationProperty( value, SAMPLING_MODE_TABLE, SAMPLING_MODE_TABLE_COUNT, samplingMode );
      mSamplingMode = Dali::SamplingMode::Type( samplingMode );
      break;
    }

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

    case Toolkit::ImageVisual::Property::ATLASING:
    {
      bool atlasing = false;
      mAttemptAtlasing = value.Get( atlasing );
      break;
    }

    case Toolkit::ImageVisual::Property::ALPHA_MASK_URL:
    {
      std::string alphaUrl;
      if( value.Get( alphaUrl ) )
      {
        AllocateMaskData();
        // Immediately trigger the alpha mask loading (it may just get a cached value)
        mMaskingData->mAlphaMaskUrl = alphaUrl;
        TextureManager& textureManager = mFactoryCache.GetTextureManager();
        mMaskingData->mAlphaMaskId = textureManager.RequestMaskLoad( alphaUrl );
      }
      break;
    }

    case Toolkit::ImageVisual::Property::MASK_CONTENT_SCALE:
    {
      float scale;
      if( value.Get( scale ) )
      {
        AllocateMaskData();
        mMaskingData->mContentScaleFactor = scale;
      }
      break;
    }

    case Toolkit::ImageVisual::Property::CROP_TO_MASK:
    {
      bool crop=false;
      if( value.Get( crop ) )
      {
        AllocateMaskData();
        mMaskingData->mCropToMask = crop;
      }
      break;
    }

    case Toolkit::DevelImageVisual::Property::RELEASE_POLICY:
    {
      int releasePolicy;
      Scripting::GetEnumerationProperty( value, RELEASE_POLICY_TABLE, RELEASE_POLICY_TABLE_COUNT, releasePolicy );
      mReleasePolicy = DevelImageVisual::ReleasePolicy::Type( releasePolicy );
      break;
    }
  }
}

void ImageVisual::AllocateMaskData()
{
  if( !mMaskingData )
  {
    mMaskingData.reset(new TextureManager::MaskingData());
  }
}

void ImageVisual::GetNaturalSize( Vector2& naturalSize )
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
  else if( mImpl->mRenderer ) // Check if we have a loaded image
  {
    auto textureSet = mImpl->mRenderer.GetTextures();

    if( textureSet )
    {
      auto texture = textureSet.GetTexture(0);
      naturalSize.x = texture.GetWidth();
      naturalSize.y = texture.GetHeight();
      return;
    }
  }

  if( mMaskingData != NULL && mMaskingData->mAlphaMaskUrl.IsValid() &&
           mMaskingData->mCropToMask )
  {
    ImageDimensions dimensions = Dali::GetClosestImageSize( mMaskingData->mAlphaMaskUrl.GetUrl() );
    if( dimensions != ImageDimensions( 0, 0 ) )
    {
      naturalSize.x = dimensions.GetWidth();
      naturalSize.y = dimensions.GetHeight();
    }
    return;
  }
  else if( mImageUrl.IsValid() )
  {
    if( mImageUrl.GetProtocolType() == VisualUrl::LOCAL )
    {
      ImageDimensions dimensions = Dali::GetClosestImageSize( mImageUrl.GetUrl() );

      if( dimensions != ImageDimensions( 0, 0 ) )
      {
        naturalSize.x = dimensions.GetWidth();
        naturalSize.y = dimensions.GetHeight();
      }
      else
      {
        Image brokenImage = VisualFactoryCache::GetBrokenVisualImage();

        naturalSize.x = brokenImage.GetWidth();
        naturalSize.y = brokenImage.GetWidth();
      }
      return;
    }
  }

  naturalSize = Vector2::ZERO;
}

void ImageVisual::CreateRenderer( TextureSet& textureSet )
{
  Geometry geometry;
  Shader shader;

  if( !mImpl->mCustomShader )
  {
    geometry = CreateGeometry( mFactoryCache, ImageDimensions( 1, 1 ) );

    shader = GetImageShader( mFactoryCache,
                             mImpl->mFlags & Impl::IS_ATLASING_APPLIED,
                             mWrapModeU <= WrapMode::CLAMP_TO_EDGE && mWrapModeV <= WrapMode::CLAMP_TO_EDGE );
  }
  else
  {
    geometry = CreateGeometry( mFactoryCache, mImpl->mCustomShader->mGridSize );
    if( mImpl->mCustomShader->mVertexShader.empty() && mImpl->mCustomShader->mFragmentShader.empty() )
    {
      shader = GetImageShader( mFactoryCache, false, true );
    }
    else
    {
      shader  = Shader::New( mImpl->mCustomShader->mVertexShader.empty() ? VERTEX_SHADER : mImpl->mCustomShader->mVertexShader,
                             mImpl->mCustomShader->mFragmentShader.empty() ? FRAGMENT_SHADER_NO_ATLAS : mImpl->mCustomShader->mFragmentShader,
                             mImpl->mCustomShader->mHints );
      if( mImpl->mCustomShader->mVertexShader.empty() )
      {
        shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
      }
    }
  }

  // Set pixel align off as default.
  // ToDo: Pixel align causes issues such as rattling image animation.
  // We should trun it off until issues are resolved
  shader.RegisterProperty( PIXEL_ALIGNED_UNIFORM_NAME, PIXEL_ALIGN_OFF );

  mImpl->mRenderer = Renderer::New( geometry, shader );
  if( textureSet )
  {
    mImpl->mRenderer.SetTextures( textureSet );
  }
  // else still waiting for texture load to finish.

  //Register transform properties
  mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
}

void ImageVisual::CreateNativeImageRenderer( NativeImage& nativeImage )
{
  Geometry geometry;
  Shader shader;

  std::string fragmentShader;
  const char* fragmentPreFix = nativeImage.GetCustomFragmentPreFix();
  const char* customSamplerTypename = nativeImage.GetCustomSamplerTypename();

  if( fragmentPreFix )
  {
    fragmentShader = fragmentPreFix;
    fragmentShader += "\n";
  }

  if( mImpl->mCustomShader && !mImpl->mCustomShader->mFragmentShader.empty() )
  {
    fragmentShader += mImpl->mCustomShader->mFragmentShader;
  }
  else
  {
    fragmentShader += FRAGMENT_SHADER_NO_ATLAS;
  }

  if( customSamplerTypename )
  {
    fragmentShader.replace( fragmentShader.find( DEFAULT_SAMPLER_TYPENAME ), strlen( DEFAULT_SAMPLER_TYPENAME ), customSamplerTypename );
  }

  if( !mImpl->mCustomShader )
  {
    geometry = CreateGeometry( mFactoryCache, ImageDimensions( 1, 1 ) );

    shader  = Shader::New( VERTEX_SHADER, fragmentShader );
    shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
  }
  else
  {
    geometry = CreateGeometry( mFactoryCache, mImpl->mCustomShader->mGridSize );
    shader  = Shader::New( mImpl->mCustomShader->mVertexShader.empty() ? VERTEX_SHADER : mImpl->mCustomShader->mVertexShader,
                           fragmentShader,
                           mImpl->mCustomShader->mHints );
    if( mImpl->mCustomShader->mVertexShader.empty() )
    {
      shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
    }
  }

  mImpl->mRenderer = Renderer::New( geometry, shader );

  //Register transform properties
  mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
}


bool ImageVisual::IsSynchronousResourceLoading() const
{
  return mImpl->mFlags & Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
}

/*
( VisualUrl& url, Dali::ImageDimensions desiredSize, Dali::FittingMode::Type fittingMode, Dali::SamplingMode::Type samplingMode,
                                        ImageVisual::MaskingData* maskInfo, bool synchronousLoading,
                                        TextureManager::TextureId textureId, Vector4& textureRect, bool& atlasingStatus, bool& loadingStatus
 */
void ImageVisual::InitializeRenderer()
{
  mImpl->mFlags &= ~Impl::IS_ATLASING_APPLIED;

  TextureManager& textureManager = mFactoryCache.GetTextureManager();

  if( ! mImpl->mCustomShader && mImageUrl.GetProtocolType() == VisualUrl::LOCAL )
  {
    bool defaultWrapMode = mWrapModeU <= WrapMode::CLAMP_TO_EDGE && mWrapModeV <= WrapMode::CLAMP_TO_EDGE;

    Vector4 atlasRect;

    auto attemptAtlasing = mAttemptAtlasing;

    // texture set has to be created first as we need to know if atlasing succeeded or not
    // when selecting the shader
    TextureSet textures =
        textureManager.LoadTexture(mImageUrl, mDesiredSize, mFittingMode, mSamplingMode,
                                   mMaskingData, IsSynchronousResourceLoading(), mTextureId,
                                   atlasRect, attemptAtlasing, mLoading, mWrapModeU,
                                   mWrapModeV, this, this, mFactoryCache.GetAtlasManager());
    if(attemptAtlasing)
    {
      mImpl->mFlags |= Impl::IS_ATLASING_APPLIED;
    }
    CreateRenderer( textures );

    if( mImpl->mFlags & Impl::IS_ATLASING_APPLIED ) // the texture is packed inside atlas
    {
      mImpl->mRenderer.RegisterProperty( ATLAS_RECT_UNIFORM_NAME, atlasRect );
      if( !defaultWrapMode ) // custom wrap mode
      {
        Vector2 wrapMode(mWrapModeU-WrapMode::CLAMP_TO_EDGE, mWrapModeV-WrapMode::CLAMP_TO_EDGE);
        wrapMode.Clamp( Vector2::ZERO, Vector2( 2.f, 2.f ) );
        mImpl->mRenderer.RegisterProperty( WRAP_MODE_UNIFORM_NAME, wrapMode );
      }
    }
  }
  else
  {
    auto attemptAtlasing = false;
    // for custom shader or remote image, atlas is not applied
    Vector4 atlasRect; // ignored in this case
    TextureSet textures =
        textureManager.LoadTexture(mImageUrl, mDesiredSize, mFittingMode, mSamplingMode,
                                   mMaskingData, IsSynchronousResourceLoading(), mTextureId,
                                   atlasRect, attemptAtlasing, mLoading, mWrapModeU, mWrapModeV, this,
                                   nullptr, nullptr); // no atlasing
    DALI_ASSERT_DEBUG(attemptAtlasing == false);
    CreateRenderer( textures );
  }
}

void ImageVisual::InitializeRenderer( const Image& image )
{
  TextureSet textures = TextureSet::New();

  NativeImage nativeImage = NativeImage::DownCast( image );
  if( nativeImage )
  {
    CreateNativeImageRenderer( nativeImage );
    DALI_ASSERT_DEBUG( textures );
    mImpl->mRenderer.SetTextures( textures );
  }
  else
  {
    // reuse existing code for regular images
    CreateRenderer( textures );
  }
  ApplyImageToSampler( image );
}

void ImageVisual::DoSetOnStage( Actor& actor )
{
  if( mImageUrl.IsValid() )
  {
    InitializeRenderer();
  }
  else if( mImage )
  {
    InitializeRenderer( mImage );
  }

  if( !mImpl->mRenderer )
  {
    return;
  }

  mPlacementActor = actor;
  // Search the Actor tree to find if Layer UI behaviour set.
  Layer layer = actor.GetLayer();
  if( layer && layer.GetBehavior() == Layer::LAYER_3D )
  {
     // Layer 3D set, do not align pixels
     mImpl->mRenderer.RegisterProperty( PIXEL_ALIGNED_UNIFORM_NAME, PIXEL_ALIGN_OFF );
  }

  if( mPixelArea != FULL_TEXTURE_RECT )
  {
    mImpl->mRenderer.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, mPixelArea );
  }

  if( mLoading == false )
  {
    actor.AddRenderer( mImpl->mRenderer );
    mPlacementActor.Reset();

    // Image loaded and ready to display
    ResourceReady();
  }
}

void ImageVisual::DoSetOffStage( Actor& actor )
{
  // Visual::Base::SetOffStage only calls DoSetOffStage if mRenderer exists (is on onstage)

  // Image release is dependent on the ReleasePolicy, renderer is destroyed.
  actor.RemoveRenderer( mImpl->mRenderer);
  if( mReleasePolicy == DevelImageVisual::ReleasePolicy::DETACHED )
  {
    RemoveTexture(); // If INVALID_TEXTURE_ID then removal will be attempted on atlas
  }

  if( mImageUrl.IsValid() )
  {
    // Legacy support for deprecated Dali::Image
    mImage.Reset();
  }
  mLoading = false;
  mImpl->mRenderer.Reset();
  mPlacementActor.Reset();
}

void ImageVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE );

  bool sync = IsSynchronousResourceLoading();
  map.Insert( SYNCHRONOUS_LOADING, sync );
  if( mImageUrl.IsValid() )
  {
    map.Insert( Toolkit::ImageVisual::Property::URL, mImageUrl.GetUrl() );
    map.Insert( Toolkit::ImageVisual::Property::DESIRED_WIDTH, mDesiredSize.GetWidth() );
    map.Insert( Toolkit::ImageVisual::Property::DESIRED_HEIGHT, mDesiredSize.GetHeight() );
  }
  else if( mImage )
  {
    map.Insert( Toolkit::ImageVisual::Property::DESIRED_WIDTH, static_cast<int>(mImage.GetWidth()) );
    map.Insert( Toolkit::ImageVisual::Property::DESIRED_HEIGHT, static_cast<int>(mImage.GetHeight()) );

    ResourceImage resourceImage = ResourceImage::DownCast(mImage);
    if( resourceImage )
    {
      map.Insert( Toolkit::ImageVisual::Property::URL, resourceImage.GetUrl() );
    }
  }

  map.Insert( Toolkit::ImageVisual::Property::FITTING_MODE, mFittingMode );
  map.Insert( Toolkit::ImageVisual::Property::SAMPLING_MODE, mSamplingMode );

  map.Insert( Toolkit::ImageVisual::Property::PIXEL_AREA, mPixelArea );
  map.Insert( Toolkit::ImageVisual::Property::WRAP_MODE_U, mWrapModeU );
  map.Insert( Toolkit::ImageVisual::Property::WRAP_MODE_V, mWrapModeV );

  map.Insert( Toolkit::ImageVisual::Property::ATLASING, mAttemptAtlasing );
  if( mMaskingData != NULL )
  {
    map.Insert( Toolkit::ImageVisual::Property::ALPHA_MASK_URL, mMaskingData->mAlphaMaskUrl.GetUrl() );
    map.Insert( Toolkit::ImageVisual::Property::MASK_CONTENT_SCALE, mMaskingData->mContentScaleFactor );
    map.Insert( Toolkit::ImageVisual::Property::CROP_TO_MASK, mMaskingData->mCropToMask );
  }

  map.Insert( Toolkit::DevelImageVisual::Property::RELEASE_POLICY, mReleasePolicy );

}

void ImageVisual::DoCreateInstancePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE );
  if( mImageUrl.IsValid() )
  {
    map.Insert( Toolkit::ImageVisual::Property::DESIRED_WIDTH, mDesiredSize.GetWidth() );
    map.Insert( Toolkit::ImageVisual::Property::DESIRED_HEIGHT, mDesiredSize.GetHeight() );
  }
  else if( mImage )
  {
    map.Insert( Toolkit::ImageVisual::Property::DESIRED_WIDTH, static_cast<int>(mImage.GetWidth()) );
    map.Insert( Toolkit::ImageVisual::Property::DESIRED_HEIGHT, static_cast<int>(mImage.GetHeight()) );
  }
}

void ImageVisual::OnSetTransform()
{
  if( mImpl->mRenderer )
  {
    mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
  }
}

Shader ImageVisual::GetImageShader( VisualFactoryCache& factoryCache, bool atlasing, bool defaultTextureWrapping )
{
  Shader shader;
  if( atlasing )
  {
    if( defaultTextureWrapping )
    {
      shader = factoryCache.GetShader( VisualFactoryCache::IMAGE_SHADER_ATLAS_DEFAULT_WRAP );
      if( !shader )
      {
        shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER_ATLAS_CLAMP );
        shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
        factoryCache.SaveShader( VisualFactoryCache::IMAGE_SHADER_ATLAS_DEFAULT_WRAP, shader );
      }
    }
    else
    {
      shader = factoryCache.GetShader( VisualFactoryCache::IMAGE_SHADER_ATLAS_CUSTOM_WRAP );
      if( !shader )
      {
        shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER_ATLAS_VARIOUS_WRAP );
        shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
        factoryCache.SaveShader( VisualFactoryCache::IMAGE_SHADER_ATLAS_CUSTOM_WRAP, shader );
      }
    }
  }
  else
  {
    shader = factoryCache.GetShader( VisualFactoryCache::IMAGE_SHADER );
    if( !shader )
    {
      shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER_NO_ATLAS );
      shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
      factoryCache.SaveShader( VisualFactoryCache::IMAGE_SHADER, shader );
    }
  }

  return shader;
}

void ImageVisual::ApplyImageToSampler( const Image& image )
{
  if( image )
  {
    TextureSet textureSet = mImpl->mRenderer.GetTextures();
    DALI_ASSERT_DEBUG( textureSet ); // texture set should always exist by this time

    TextureSetImage( textureSet, 0u, image );
    Sampler sampler = Sampler::New();
    sampler.SetWrapMode(  mWrapModeU, mWrapModeV  );
    textureSet.SetSampler( 0u, sampler );
  }
}

// From existing atlas manager
void ImageVisual::UploadCompleted()
{
  // Texture has been uploaded. If weak handle is holding a placement actor, it is the time to add the renderer to actor.
  Actor actor = mPlacementActor.GetHandle();
  if( actor )
  {
    actor.AddRenderer( mImpl->mRenderer );

    // reset the weak handle so that the renderer only get added to actor once
    mPlacementActor.Reset();
  }
  mLoading = false;
}

// From Texture Manager
void ImageVisual::UploadComplete( bool loadingSuccess, int32_t textureId, TextureSet textureSet, bool usingAtlas, const Vector4& atlasRectangle )
{
  Actor actor = mPlacementActor.GetHandle();
  if( actor )
  {
    if( mImpl->mRenderer )
    {
      actor.AddRenderer( mImpl->mRenderer );
      // reset the weak handle so that the renderer only get added to actor once
      mPlacementActor.Reset();

      if( loadingSuccess )
      {
        Sampler sampler = Sampler::New();
        sampler.SetWrapMode(  mWrapModeU, mWrapModeV  );
        textureSet.SetSampler( 0u, sampler );
        mImpl->mRenderer.SetTextures(textureSet);
      }
      else
      {
        Image brokenImage = VisualFactoryCache::GetBrokenVisualImage();

        textureSet = TextureSet::New();
        mImpl->mRenderer.SetTextures( textureSet );

        ApplyImageToSampler( brokenImage );
      }
      // Image loaded and ready to display
      ResourceReady();
    }
  }
  mLoading = false;
}

void ImageVisual::RemoveTexture()
{
  if( mTextureId != TextureManager::INVALID_TEXTURE_ID )
  {
    mFactoryCache.GetTextureManager().Remove( mTextureId );
    mTextureId = TextureManager::INVALID_TEXTURE_ID;
  }
  else
  {
    Vector4 atlasRect( 0.f, 0.f, 1.f, 1.f );
    Property::Index index = mImpl->mRenderer.GetPropertyIndex( ATLAS_RECT_UNIFORM_NAME );
    if( index != Property::INVALID_INDEX )
    {
      Property::Value atlasRectValue = mImpl->mRenderer.GetProperty( index );
      atlasRectValue.Get( atlasRect );
    }

    TextureSet textureSet = mImpl->mRenderer.GetTextures();
    mImpl->mRenderer.Reset();

    if( index != Property::INVALID_INDEX )
    {
      mFactoryCache.GetAtlasManager()->Remove( textureSet, atlasRect );
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
