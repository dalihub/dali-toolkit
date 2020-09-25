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
#include "svg-visual.h"

// INTERNAL INCLUDES
#ifdef NO_THORVG
#include <dali-toolkit/third-party/nanosvg/nanosvg.h>
#include <dali-toolkit/third-party/nanosvg/nanosvgrast.h>
#endif /* NO_THORVG */
#include <dali-toolkit/internal/visuals/svg/svg-rasterize-thread.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/image-visual-shader-factory.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/adaptor-framework/file-loader.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
// property name
const char * const UNITS("px");

const Dali::Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

}

SvgVisualPtr SvgVisual::New( VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, const Property::Map& properties )
{
  SvgVisualPtr svgVisual( new SvgVisual( factoryCache, shaderFactory, imageUrl ) );
#ifdef NO_THORVG
  svgVisual->ParseFromUrl( imageUrl );
  svgVisual->SetProperties( properties );
#else /* NO_THORVG */
  svgVisual->Load();
  svgVisual->SetProperties( properties );
#endif /* NO_THORVG */

  return svgVisual;
}

SvgVisualPtr SvgVisual::New( VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl )
{
  SvgVisualPtr svgVisual( new SvgVisual( factoryCache, shaderFactory, imageUrl ) );
#ifdef NO_THORVG
  svgVisual->ParseFromUrl( imageUrl );
#else /* NO_THORVG */
  svgVisual->Load();
#endif /* NO_THORVG */

  return svgVisual;
}

SvgVisual::SvgVisual( VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl )
: Visual::Base( factoryCache, Visual::FittingMode::FILL, Toolkit::Visual::SVG ),
  mImageVisualShaderFactory( shaderFactory ),
  mAtlasRect( FULL_TEXTURE_RECT ),
  mImageUrl( imageUrl ),
#ifdef NO_THORVG
  mParsedImage( NULL ),
#else
  mVectorRenderer( VectorImageRenderer::New() ),
  mDefaultWidth( 0 ),
  mDefaultHeight( 0 ),
  mLoaded( false ),
  mLocalResource( true ),
#endif /* NO_THORVG */
  mPlacementActor(),
  mVisualSize(Vector2::ZERO),
  mAttemptAtlasing( false )
{
  // the rasterized image is with pre-multiplied alpha format
  mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;
}

SvgVisual::~SvgVisual()
{
#ifdef NO_THORVG
  if( mParsedImage )
  {
    nsvgDelete( mParsedImage );
  }
#endif /* NO_THORVG */
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
    else if( keyValue.first == SYNCHRONOUS_LOADING )
    {
      DoSetProperty( Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, keyValue.second );
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
    case Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING:
    {
      bool sync = false;
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
  }
}

void SvgVisual::DoSetOnScene( Actor& actor )
{
  Shader shader;
  if( !mImpl->mCustomShader )
  {
    shader = mImageVisualShaderFactory.GetShader( mFactoryCache, mAttemptAtlasing, true, false );
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

void SvgVisual::DoSetOffScene( Actor& actor )
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
#ifdef NO_THORVG
  if( mParsedImage )
  {
    naturalSize.x = mParsedImage->width;
    naturalSize.y = mParsedImage->height;
  }
#else /* NO_THORVG */
  if ( mLoaded )
  {
    naturalSize.x = mDefaultWidth;
    naturalSize.y = mDefaultHeight;
  }
#endif /* NO_THORVG */
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
  map.Insert( Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, IsSynchronousLoadingRequired() );
}

void SvgVisual::DoCreateInstancePropertyMap( Property::Map& map ) const
{
  // Do nothing
}

#ifdef NO_THORVG
void SvgVisual::ParseFromUrl( const VisualUrl& imageUrl )
{
  mImageUrl = imageUrl;
  if( mImageUrl.IsLocalResource() )
  {
    Vector2 dpi = Stage::GetCurrent().GetDpi();
    float meanDpi = ( dpi.height + dpi.width ) * 0.5f;
    Dali::Vector<char> buffer;
    if ( Dali::FileLoader::ReadFile( mImageUrl.GetUrl(), buffer ) )
    {
      buffer.PushBack( '\0' );
      mParsedImage = nsvgParse( buffer.Begin(), UNITS, meanDpi );
    }
  }
}
#else /* NO_THORVG */
void SvgVisual::Load()
{
  if( mLoaded || !mLocalResource )
  {
    return;
  }

  mLocalResource = mImageUrl.IsLocalResource();

  if( !mLocalResource )
  {
    // load remote resource on svg rasterize thread.
    return;
  }

  if( !mVectorRenderer.Load( mImageUrl.GetUrl() ) )
  {
    DALI_LOG_ERROR( "Failed to load file!\n" );
    return;
  }

  mVectorRenderer.GetDefaultSize(mDefaultWidth, mDefaultHeight);
  mLoaded = true;
}
#endif /* NO_THORVG */


void SvgVisual::AddRasterizationTask( const Vector2& size )
{
  if( mImpl->mRenderer )
  {
    unsigned int width = static_cast<unsigned int>(size.width);
    unsigned int height = static_cast<unsigned int>( size.height );

    Vector2 dpi = Stage::GetCurrent().GetDpi();
    float meanDpi = ( dpi.height + dpi.width ) * 0.5f;

#ifdef NO_THORVG
    RasterizingTaskPtr newTask = new RasterizingTask( this, mParsedImage, mImageUrl, meanDpi, width, height );
#else /* NO_THORVG */
    RasterizingTaskPtr newTask = new RasterizingTask( this, mVectorRenderer, mImageUrl, meanDpi, width, height, mLoaded );
#endif /* NO_THORVG */
    if ( IsSynchronousLoadingRequired() )
    {
#ifdef NO_THORVG
      newTask->Rasterize();
      ApplyRasterizedImage( newTask->GetParsedImage(), newTask->GetPixelData() );
#else /* NO_THORVG */
      newTask->Load();
      newTask->Rasterize();
      ApplyRasterizedImage( newTask->GetVectorRenderer(), newTask->GetPixelData(), newTask->IsLoaded() );
#endif /* NO_THORVG */
    }
    else
    {
      mFactoryCache.GetSVGRasterizationThread()->AddTask( newTask );
    }
  }
}

#ifdef NO_THORVG
void SvgVisual::ApplyRasterizedImage( NSVGimage* parsedSvg, PixelData rasterizedPixelData )
{
  if( mParsedImage == NULL)
  {
    mParsedImage = parsedSvg;
  }

  if( mParsedImage && IsOnScene() )
#else /* NO_THORVG */
void SvgVisual::ApplyRasterizedImage( VectorImageRenderer vectorRenderer, PixelData rasterizedPixelData, bool isLoaded )
{
  mLoaded = isLoaded;

  if( isLoaded && rasterizedPixelData && IsOnScene() )
#endif /* NO_THORVG */
  {
    TextureSet currentTextureSet = mImpl->mRenderer.GetTextures();
    if( mImpl->mFlags & Impl::IS_ATLASING_APPLIED )
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
#ifdef NO_THORVG
  else if( !mParsedImage )
#else /* NO_THORVG */
  else if( !isLoaded || !rasterizedPixelData )
#endif /* NO_THORVG */
  {
    ResourceReady( Toolkit::Visual::ResourceStatus::FAILED );
  }
}

void SvgVisual::OnSetTransform()
{
  Vector2 visualSize = mImpl->mTransform.GetVisualSize( mImpl->mControlSize );

  if( IsOnScene() )
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

bool SvgVisual::IsResourceReady() const
{
  return ( mImpl->mResourceStatus == Toolkit::Visual::ResourceStatus::READY ||
           mImpl->mResourceStatus == Toolkit::Visual::ResourceStatus::FAILED );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
