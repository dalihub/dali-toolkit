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
#include "image-visual.h"

// EXTERNAL HEADER
#include <cstring> // for strncasecmp
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/images/native-image.h>
#include <dali/devel-api/images/atlas.h>
#include <dali/devel-api/images/texture-set-image.h>
#include <dali/devel-api/adaptor-framework/bitmap-loader.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>

// INTERNAL HEADER
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>

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

// property names
const char * const IMAGE_FITTING_MODE( "fittingMode" );
const char * const IMAGE_SAMPLING_MODE( "samplingMode" );
const char * const IMAGE_DESIRED_WIDTH( "desiredWidth" );
const char * const IMAGE_DESIRED_HEIGHT( "desiredHeight" );
const char * const IMAGE_WRAP_MODE_U("wrapModeU");
const char * const IMAGE_WRAP_MODE_V("wrapModeV");
const char * const SYNCHRONOUS_LOADING( "synchronousLoading" );
const char * const BATCHING_ENABLED( "batchingEnabled" );

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

const std::string PIXEL_AREA_UNIFORM_NAME = "pixelArea";
const std::string WRAP_MODE_UNIFORM_NAME = "wrapMode";

const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

const char* DEFAULT_SAMPLER_TYPENAME = "sampler2D";

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  uniform mediump vec4 pixelArea;
  varying mediump vec2 vTexCoord;\n
  \n
  void main()\n
  {\n
    mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
    vertexPosition.xyz *= uSize;\n
    vertexPosition = uMvpMatrix * vertexPosition;\n
    \n
    vTexCoord = pixelArea.xy+pixelArea.zw*(aPosition + vec2(0.5) );\n
    gl_Position = vertexPosition;\n
  }\n
);

const char* FRAGMENT_SHADER_NO_ATLAS = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
  }\n
);

const char* FRAGMENT_SHADER_ATLAS_CLAMP = DALI_COMPOSE_SHADER(
    varying mediump vec2 vTexCoord;\n
    uniform sampler2D sTexture;\n
    uniform mediump vec4 uAtlasRect;\n
    uniform lowp vec4 uColor;\n
    \n
    void main()\n
    {\n
      mediump vec2 texCoord = clamp( mix( uAtlasRect.xy, uAtlasRect.zw, vTexCoord ), uAtlasRect.xy, uAtlasRect.zw );\n
      gl_FragColor = texture2D( sTexture, texCoord ) * uColor;\n
    }\n
);

const char* FRAGMENT_SHADER_ATLAS_VARIOUS_WRAP = DALI_COMPOSE_SHADER(
    varying mediump vec2 vTexCoord;\n
    uniform sampler2D sTexture;\n
    uniform mediump vec4 uAtlasRect;\n
    // WrapMode -- 0: CLAMP; 1: REPEAT; 2: REFLECT;
    uniform lowp vec2 wrapMode;\n
    uniform lowp vec4 uColor;\n
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
    void main()\n
    {\n
      mediump vec2 texCoord = vec2( wrapCoordinate( uAtlasRect.xz, vTexCoord.x, wrapMode.x ),
                                    wrapCoordinate( uAtlasRect.yw, vTexCoord.y, wrapMode.y ) );\n
      gl_FragColor = texture2D( sTexture, texCoord ) * uColor;\n
    }\n
);

Geometry CreateGeometry( VisualFactoryCache& factoryCache, ImageDimensions gridSize )
{
  Geometry geometry;

  if( gridSize == ImageDimensions( 1, 1 ) )
  {
    geometry = factoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );
    if( !geometry )
    {
      geometry =  VisualFactoryCache::CreateQuadGeometry();
      factoryCache.SaveGeometry( VisualFactoryCache::QUAD_GEOMETRY, geometry );
    }
  }
  else
  {
    geometry = VisualFactoryCache::CreateGridGeometry( gridSize );
  }

  return geometry;
}

} //unnamed namespace

ImageVisual::ImageVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache ),
  mImage(),
  mPixels(),
  mPixelArea( FULL_TEXTURE_RECT ),
  mPlacementActor(),
  mImageUrl(),
  mDesiredSize(),
  mFittingMode( FittingMode::DEFAULT ),
  mSamplingMode( SamplingMode::DEFAULT ),
  mWrapModeU( WrapMode::DEFAULT ),
  mWrapModeV( WrapMode::DEFAULT ),
  mNativeFragmentShaderCode(),
  mNativeImageFlag( false )
{
}

ImageVisual::ImageVisual( VisualFactoryCache& factoryCache,
                          const std::string& imageUrl,
                          ImageDimensions size,
                          FittingMode::Type fittingMode,
                          Dali::SamplingMode::Type samplingMode )
: Visual::Base( factoryCache ),
  mImage(),
  mPixels(),
  mPixelArea( FULL_TEXTURE_RECT ),
  mPlacementActor(),
  mImageUrl( imageUrl ),
  mDesiredSize( size ),
  mFittingMode( fittingMode ),
  mSamplingMode( samplingMode ),
  mWrapModeU( WrapMode::DEFAULT ),
  mWrapModeV( WrapMode::DEFAULT ),
  mNativeFragmentShaderCode(),
  mNativeImageFlag( false )
{
}

ImageVisual::ImageVisual( VisualFactoryCache& factoryCache, const Image& image )
: Visual::Base( factoryCache ),
  mImage( image ),
  mPixels(),
  mPixelArea( FULL_TEXTURE_RECT ),
  mPlacementActor(),
  mImageUrl(),
  mDesiredSize(),
  mFittingMode( FittingMode::DEFAULT ),
  mSamplingMode( SamplingMode::DEFAULT ),
  mWrapModeU( WrapMode::DEFAULT ),
  mWrapModeV( WrapMode::DEFAULT ),
  mNativeFragmentShaderCode(),
  mNativeImageFlag( false )
{
  NativeImage newNativeImage = NativeImage::DownCast( image );
  if( newNativeImage )
  {
    SetNativeFragmentShaderCode( newNativeImage );
    mNativeImageFlag = true;
  }
  else
  {
    mNativeImageFlag = false;
  }
}

ImageVisual::~ImageVisual()
{
}

void ImageVisual::DoInitialize( Actor& actor, const Property::Map& propertyMap )
{
  std::string oldImageUrl = mImageUrl;

  Property::Value* imageURLValue = propertyMap.Find( Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME );
  if( imageURLValue )
  {
    imageURLValue->Get( mImageUrl );
    if( !mImageUrl.empty() )
    {
      mImage.Reset();
    }

    Property::Value* fittingValue = propertyMap.Find( Toolkit::ImageVisual::Property::FITTING_MODE, IMAGE_FITTING_MODE );
    if( fittingValue )
    {
      Scripting::GetEnumerationProperty( *fittingValue, FITTING_MODE_TABLE, FITTING_MODE_TABLE_COUNT, mFittingMode );
    }

    Property::Value* samplingValue = propertyMap.Find( Toolkit::ImageVisual::Property::SAMPLING_MODE, IMAGE_SAMPLING_MODE );
    if( samplingValue )
    {
      Scripting::GetEnumerationProperty( *samplingValue, SAMPLING_MODE_TABLE, SAMPLING_MODE_TABLE_COUNT, mSamplingMode );
    }

    int desiredWidth = 0;
    Property::Value* desiredWidthValue = propertyMap.Find( Toolkit::ImageVisual::Property::DESIRED_WIDTH, IMAGE_DESIRED_WIDTH );
    if( desiredWidthValue )
    {
      desiredWidthValue->Get( desiredWidth );
    }

    int desiredHeight = 0;
    Property::Value* desiredHeightValue = propertyMap.Find( Toolkit::ImageVisual::Property::DESIRED_HEIGHT, IMAGE_DESIRED_HEIGHT );
    if( desiredHeightValue )
    {
      desiredHeightValue->Get( desiredHeight );
    }

    Property::Value* pixelAreaValue = propertyMap.Find( Toolkit::ImageVisual::Property::PIXEL_AREA, PIXEL_AREA_UNIFORM_NAME );
    if( pixelAreaValue )
    {
      pixelAreaValue->Get( mPixelArea );
    }

    Property::Value* wrapModeValueU = propertyMap.Find( Toolkit::ImageVisual::Property::WRAP_MODE_U, IMAGE_WRAP_MODE_U );
    if( wrapModeValueU )
    {
      Scripting::GetEnumerationProperty( *wrapModeValueU, WRAP_MODE_TABLE, WRAP_MODE_TABLE_COUNT, mWrapModeU );
    }

    Property::Value* wrapModeValueV = propertyMap.Find( Toolkit::ImageVisual::Property::WRAP_MODE_V, IMAGE_WRAP_MODE_V );
    if( wrapModeValueV )
    {
      Scripting::GetEnumerationProperty( *wrapModeValueV, WRAP_MODE_TABLE, WRAP_MODE_TABLE_COUNT, mWrapModeV );
    }

    mDesiredSize = ImageDimensions( desiredWidth, desiredHeight );
  }

  Property::Value* syncLoading = propertyMap.Find( Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, SYNCHRONOUS_LOADING );
  if( syncLoading )
  {
    bool sync;
    syncLoading->Get( sync );
    if( sync )
    {
      mImpl->mFlags |= Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
    }
    else
    {
      mImpl->mFlags &= ~Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
    }
  }

  // if sync loading is required, the loading should start immediately when new image url is set or the actor is off stage
  // ( for on-stage actor with image url unchanged, resource loading is already finished)
  if( imageURLValue && IsSynchronousResourceLoading() )
  {
    LoadResourceSynchronously();
  }

  NativeImage nativeImage = NativeImage::DownCast( mImage );

  if( nativeImage )
  {
    SetNativeFragmentShaderCode( nativeImage );
  }

  // if actor is on stage, create new renderer and apply to actor
  if( actor && actor.OnStage() )
  {
    SetOnStage( actor );
  }
}

void ImageVisual::GetNaturalSize( Vector2& naturalSize ) const
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

Renderer ImageVisual::CreateRenderer() const
{
  Geometry geometry;
  Shader shader;

  // If mImage is nativeImage with custom sampler or prefix, mNativeFragmentShaderCode will be applied.
  // Renderer can't be shared between NativeImage and other image types.
  if( !mNativeFragmentShaderCode.empty() )
  {
    return CreateNativeImageRenderer();
  }

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
      shader = GetImageShader(mFactoryCache, false, true);
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

  Renderer renderer = Renderer::New( geometry, shader );

  return renderer;
}

Renderer ImageVisual::CreateNativeImageRenderer() const
{
  Geometry geometry;
  Shader shader;

  if( !mImpl->mCustomShader )
  {
    geometry = CreateGeometry( mFactoryCache, ImageDimensions( 1, 1 ) );

    shader  = Shader::New( VERTEX_SHADER, mNativeFragmentShaderCode );
    shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
  }
  else
  {
    geometry = CreateGeometry( mFactoryCache, mImpl->mCustomShader->mGridSize );
    shader  = Shader::New( mImpl->mCustomShader->mVertexShader.empty() ? VERTEX_SHADER : mImpl->mCustomShader->mVertexShader,
                           mNativeFragmentShaderCode,
                           mImpl->mCustomShader->mHints );
    if( mImpl->mCustomShader->mVertexShader.empty() )
    {
      shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
    }
  }

  TextureSet textureSet = TextureSet::New();
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );
  return renderer;
}


bool ImageVisual::IsSynchronousResourceLoading() const
{
  return mImpl->mFlags & Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
}

void ImageVisual::LoadResourceSynchronously()
{
  if( !mImageUrl.empty() )
  {
    BitmapLoader loader = BitmapLoader::New( mImageUrl, mDesiredSize, mFittingMode, mSamplingMode );
    loader.Load();
    mPixels = loader.GetPixelData();
  }
}

Image ImageVisual::LoadImage( const std::string& url, bool synchronousLoading )
{
  if( synchronousLoading )
  {
    if( !mPixels )
    {
      // use broken image
      return VisualFactoryCache::GetBrokenVisualImage();
    }
    Atlas image = Atlas::New( mPixels.GetWidth(), mPixels.GetHeight(), mPixels.GetPixelFormat() );
    image.Upload( mPixels, 0, 0 );
    return image;
  }
  else
  {
    ResourceImage resourceImage = Dali::ResourceImage::New( url, mDesiredSize, mFittingMode, mSamplingMode );
    resourceImage.LoadingFinishedSignal().Connect( this, &ImageVisual::OnImageLoaded );
    return resourceImage;
  }
}

TextureSet ImageVisual::CreateTextureSet( Vector4& textureRect, const std::string& url, bool synchronousLoading )
{
  TextureSet textureSet;
  textureRect = FULL_TEXTURE_RECT;
  if( synchronousLoading )
  {
    if( !mPixels )
    {
      // use broken image
      textureSet = TextureSet::New();
      TextureSetImage( textureSet, 0u, VisualFactoryCache::GetBrokenVisualImage() );
    }
    else
    {
      textureSet = mFactoryCache.GetAtlasManager()->Add(textureRect, mPixels );
      mImpl->mFlags |= Impl::IS_ATLASING_APPLIED;
      if( !textureSet ) // big image, no atlasing
      {
        mImpl->mFlags &= ~Impl::IS_ATLASING_APPLIED;
        Atlas image = Atlas::New( mPixels.GetWidth(), mPixels.GetHeight(), mPixels.GetPixelFormat() );
        image.Upload( mPixels, 0, 0 );
        textureSet = TextureSet::New();
        TextureSetImage( textureSet, 0u, image );
      }
    }
  }
  else
  {
    textureSet = mFactoryCache.GetAtlasManager()->Add(textureRect, url, mDesiredSize, mFittingMode, true, this );
    mImpl->mFlags |= Impl::IS_ATLASING_APPLIED;
    if( !textureSet ) // big image, no atlasing
    {
      mImpl->mFlags &= ~Impl::IS_ATLASING_APPLIED;
      ResourceImage resourceImage = Dali::ResourceImage::New( url, mDesiredSize, mFittingMode, mSamplingMode );
      resourceImage.LoadingFinishedSignal().Connect( this, &ImageVisual::OnImageLoaded );
      textureSet = TextureSet::New();
      TextureSetImage( textureSet, 0u, resourceImage );
    }
  }

  if( !(mImpl->mFlags & Impl::IS_ATLASING_APPLIED) )
  {
    Sampler sampler = Sampler::New();
    sampler.SetWrapMode(  mWrapModeU, mWrapModeV  );
    textureSet.SetSampler( 0u, sampler );
  }

  return textureSet;
}

void ImageVisual::InitializeRenderer( const std::string& imageUrl )
{
  if( imageUrl.empty() )
  {
    return;
  }

  mImageUrl = imageUrl;
  mImpl->mRenderer.Reset();
  mImpl->mFlags &= ~Impl::IS_ATLASING_APPLIED;

  if( !mImpl->mCustomShader &&
      ( strncasecmp( imageUrl.c_str(), HTTP_URL,  sizeof(HTTP_URL)  -1 ) != 0 ) && // ignore remote images
      ( strncasecmp( imageUrl.c_str(), HTTPS_URL, sizeof(HTTPS_URL) -1 ) != 0 ) )
  {
    bool defaultWrapMode = mWrapModeU <= WrapMode::CLAMP_TO_EDGE && mWrapModeV <= WrapMode::CLAMP_TO_EDGE;
    bool cacheable =  defaultWrapMode &&  mPixelArea == FULL_TEXTURE_RECT;

    mImpl->mFlags &= ~Impl::IS_FROM_CACHE;
    if( cacheable ) // fetch the renderer from cache if exist
    {
      mImpl->mRenderer = mFactoryCache.GetRenderer( imageUrl );
      mImpl->mFlags |= Impl::IS_FROM_CACHE;
    }

    if( !mImpl->mRenderer ) // new renderer is needed
    {
      Vector4 atlasRect;
      TextureSet textureSet = CreateTextureSet(atlasRect, imageUrl, IsSynchronousResourceLoading() );
      Geometry geometry = CreateGeometry( mFactoryCache, ImageDimensions( 1, 1 ) );
      Shader shader( GetImageShader(mFactoryCache, mImpl->mFlags & Impl::IS_ATLASING_APPLIED, defaultWrapMode) );
      mImpl->mRenderer = Renderer::New( geometry, shader );
      mImpl->mRenderer.SetTextures( textureSet );

      if( mImpl->mFlags & Impl::IS_ATLASING_APPLIED ) // the texture is packed inside atlas
      {
        mImpl->mRenderer.RegisterProperty( ATLAS_RECT_UNIFORM_NAME, atlasRect );
        if( !defaultWrapMode ) // custom wrap mode, renderer is not cached.
        {
          Vector2 wrapMode(mWrapModeU-WrapMode::CLAMP_TO_EDGE, mWrapModeV-WrapMode::CLAMP_TO_EDGE);
          wrapMode.Clamp( Vector2::ZERO, Vector2( 2.f, 2.f ) );
          mImpl->mRenderer.RegisterProperty( WRAP_MODE_UNIFORM_NAME, wrapMode );
        }
      }

      // save the renderer to cache only when default wrap mode and default pixel area is used
      if( cacheable )
      {
        mFactoryCache.SaveRenderer( imageUrl, mImpl->mRenderer );
      }
    }
  }
  else
  {
    // for custom shader or remote image, renderer is not cached and atlas is not applied
    mImpl->mFlags &= ~Impl::IS_FROM_CACHE;
    mImpl->mRenderer = CreateRenderer();
    Image image = LoadImage( imageUrl, IsSynchronousResourceLoading() );
    ApplyImageToSampler( image );
  }
}

void ImageVisual::InitializeRenderer( const Image& image )
{
  mImpl->mFlags &= ~Impl::IS_FROM_CACHE;

  mImpl->mRenderer = CreateRenderer();

  if( image )
  {
    ApplyImageToSampler( image );
  }
}

void ImageVisual::UploadCompleted()
{
  // Resource image is loaded. If weak handle is holding a placement actor, it is the time to add the renderer to actor.
  Actor actor = mPlacementActor.GetHandle();
  if( actor )
  {
    actor.AddRenderer( mImpl->mRenderer );
    // reset the weak handle so that the renderer only get added to actor once
    mPlacementActor.Reset();
  }
}

void ImageVisual::DoSetOnStage( Actor& actor )
{
  mPlacementActor = actor;

  if( !mImageUrl.empty() )
  {
    InitializeRenderer( mImageUrl );
  }
  else
  {
    InitializeRenderer( mImage );
  }

  if( mPixelArea != FULL_TEXTURE_RECT )
  {
    mImpl->mRenderer.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, mPixelArea );
  }

  if( IsSynchronousResourceLoading() || !(mImpl->mFlags & Impl::IS_ATLASING_APPLIED) )
  {
    actor.AddRenderer( mImpl->mRenderer );
    mPlacementActor.Reset();
  }
}

void ImageVisual::DoSetOffStage( Actor& actor )
{
  //If we own the image then make sure we release it when we go off stage
  if( !mImageUrl.empty() )
  {
    actor.RemoveRenderer( mImpl->mRenderer );
    CleanCache(mImageUrl);
    mImage.Reset();
  }
  else
  {
    actor.RemoveRenderer( mImpl->mRenderer );
    mImpl->mRenderer.Reset();
  }
  mPlacementActor.Reset();
}

void ImageVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE );

  bool sync = IsSynchronousResourceLoading();
  map.Insert( SYNCHRONOUS_LOADING, sync );
  if( !mImageUrl.empty() )
  {
    map.Insert( Toolkit::ImageVisual::Property::URL, mImageUrl );
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
}

void ImageVisual::DoSetProperty( Dali::Property::Index index, const Dali::Property::Value& propertyValue )
{
  // TODO
}

Dali::Property::Value ImageVisual::DoGetProperty( Dali::Property::Index index )
{
  // TODO
  return Dali::Property::Value();
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
        factoryCache.SaveShader( VisualFactoryCache::IMAGE_SHADER_ATLAS_DEFAULT_WRAP, shader );
      }
    }
    else
    {
      shader = factoryCache.GetShader( VisualFactoryCache::IMAGE_SHADER_ATLAS_CUSTOM_WRAP );
      if( !shader )
      {
        shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER_ATLAS_VARIOUS_WRAP );
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
      factoryCache.SaveShader( VisualFactoryCache::IMAGE_SHADER, shader );
    }
  }
  shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
  return shader;
}

void ImageVisual::ApplyImageToSampler( const Image& image )
{
  if( image )
  {
    TextureSet textureSet = mImpl->mRenderer.GetTextures();
    if( !textureSet )
    {
      textureSet = TextureSet::New();
      mImpl->mRenderer.SetTextures( textureSet );
    }
    TextureSetImage( textureSet, 0u, image );
    Sampler sampler = Sampler::New();
    sampler.SetWrapMode(  mWrapModeU, mWrapModeV  );
    textureSet.SetSampler( 0u, sampler );
  }
}

void ImageVisual::OnImageLoaded( ResourceImage image )
{
  if( image.GetLoadingState() == Dali::ResourceLoadingFailed )
  {
    Image brokenImage = VisualFactoryCache::GetBrokenVisualImage();
    if( mImpl->mRenderer )
    {
      ApplyImageToSampler( brokenImage );
    }
  }
}

void ImageVisual::CleanCache(const std::string& url)
{
  if( IsFromCache() )
  {
    TextureSet textureSet = mImpl->mRenderer.GetTextures();

    Vector4 atlasRect( 0.f, 0.f, 1.f, 1.f );
    Property::Index index = mImpl->mRenderer.GetPropertyIndex( ATLAS_RECT_UNIFORM_NAME );
    if( index != Property::INVALID_INDEX )
    {
      Property::Value atlasRectValue = mImpl->mRenderer.GetProperty( index );
      atlasRectValue.Get( atlasRect );
    }

    mImpl->mRenderer.Reset();
    if( mFactoryCache.CleanRendererCache( url ) && index != Property::INVALID_INDEX )
    {
      mFactoryCache.GetAtlasManager()->Remove( textureSet, atlasRect );
    }
  }
}

void ImageVisual::SetNativeFragmentShaderCode( Dali::NativeImage& nativeImage )
{
  const char* fragmentPreFix = nativeImage.GetCustomFragmentPreFix();
  const char* customSamplerTypename = nativeImage.GetCustomSamplerTypename();

  if( fragmentPreFix )
  {
    mNativeFragmentShaderCode = fragmentPreFix;
    mNativeFragmentShaderCode += "\n";
  }

  if( mImpl->mCustomShader && !mImpl->mCustomShader->mFragmentShader.empty() )
  {
    mNativeFragmentShaderCode += mImpl->mCustomShader->mFragmentShader;
  }
  else
  {
    mNativeFragmentShaderCode += FRAGMENT_SHADER_NO_ATLAS;
  }

  if( customSamplerTypename )
  {
    mNativeFragmentShaderCode.replace( mNativeFragmentShaderCode.find( DEFAULT_SAMPLER_TYPENAME ), strlen( DEFAULT_SAMPLER_TYPENAME ), customSamplerTypename );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
