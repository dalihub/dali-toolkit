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
 */

// CLASS HEADER
#include <dali-toolkit/internal/controls/renderers/renderer-factory-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/renderers/border/border-renderer.h>
#include <dali-toolkit/internal/controls/renderers/color/color-renderer.h>
#include <dali-toolkit/internal/controls/renderers/debug/debug-renderer.h>
#include <dali-toolkit/internal/controls/renderers/gradient/gradient-renderer.h>
#include <dali-toolkit/internal/controls/renderers/npatch/npatch-renderer.h>
#include <dali-toolkit/internal/controls/renderers/image/image-renderer.h>
#include <dali-toolkit/internal/controls/renderers/svg/svg-renderer.h>
#include <dali-toolkit/internal/controls/renderers/mesh/mesh-renderer.h>
#include <dali-toolkit/internal/controls/renderers/renderer-factory-cache.h>
#include <dali-toolkit/internal/controls/renderers/renderer-string-constants.h>
#include <dali-toolkit/internal/controls/renderers/image-atlas-manager.h>

namespace
{
const char * const BROKEN_RENDERER_IMAGE_URL( DALI_IMAGE_DIR "broken.png");
}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

BaseHandle Create()
{
  BaseHandle handle = Toolkit::RendererFactory::Get();

  return handle;
}

DALI_TYPE_REGISTRATION_BEGIN_CREATE( Toolkit::RendererFactory, Dali::BaseHandle, Create, true )
DALI_TYPE_REGISTRATION_END()

} // namespace

RendererFactory::RendererFactory( bool debugEnabled )
:mDebugEnabled( debugEnabled )
{
}

RendererFactory::~RendererFactory()
{
}

RendererFactory::RendererType RendererFactory::GetRendererType( const Property::Map& propertyMap )
{
  RendererType rendererType = UNDEFINED;

  Property::Value* type = propertyMap.Find( RENDERER_TYPE );
  std::string typeValue ;
  if( type && type->Get( typeValue ))
  {
    if( typeValue == COLOR_RENDERER )
    {
      rendererType = COLOR;
    }
    else if( typeValue == BORDER_RENDERER )
    {
      rendererType = BORDER;
    }
    else if( typeValue == GRADIENT_RENDERER )
    {
      rendererType = GRADIENT;
    }
    else if( typeValue == IMAGE_RENDERER )
    {
      rendererType = IMAGE;
    }
    else if( typeValue == MESH_RENDERER )
    {
      rendererType = MESH;
    }
  }

  // check the url if exist, to decide the renderer type
  if( rendererType == IMAGE || rendererType == UNDEFINED )
  {
    Property::Value* imageURLValue = propertyMap.Find( IMAGE_URL_NAME );
    std::string imageUrl;
    if( imageURLValue && imageURLValue->Get( imageUrl ))
    {
      if( NinePatchImage::IsNinePatchUrl( imageUrl ) )
      {
        rendererType = N_PATCH;
      }
      else if( SvgRenderer::IsSvgUrl( imageUrl ) )
      {
        rendererType = SVG;
      }
      else
      {
        rendererType = IMAGE;
      }
    }
  }

  return rendererType;
}

Toolkit::ControlRenderer RendererFactory::GetControlRenderer( const Property::Map& propertyMap )
{
  ControlRenderer* rendererPtr = NULL;

  RendererType type = GetRendererType( propertyMap );
  if( type != UNDEFINED)
  {
    if( !mFactoryCache )
    {
      mFactoryCache = new RendererFactoryCache();
    }

    if( mDebugEnabled )
    {
      return Toolkit::ControlRenderer( new DebugRenderer( *( mFactoryCache.Get() ) ) );
    }
  }

  switch( type )
  {
    case COLOR:
    {
      rendererPtr = new ColorRenderer( *( mFactoryCache.Get() ) );
      break;
    }
     case GRADIENT:
     {
       rendererPtr = new GradientRenderer( *( mFactoryCache.Get() ) );
       break;
     }
    case BORDER:
    {
      rendererPtr = new BorderRenderer( *( mFactoryCache.Get() ) );
      break;
    }
    case IMAGE:
    {
      CreateAtlasManager();
      rendererPtr = new ImageRenderer( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
      break;
    }
    case N_PATCH:
    {
      rendererPtr = new NPatchRenderer( *( mFactoryCache.Get() ) );
      break;
    }
    case SVG:
    {
      CreateAtlasManager();
      rendererPtr = new SvgRenderer( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
      break;
    }
    case MESH:
    {
      rendererPtr = new MeshRenderer( *( mFactoryCache.Get() ) );
      break;
    }
    case UNDEFINED:
    default:
    {
      break;
    }
  }

  if( rendererPtr )
  {
    Actor actor;
    rendererPtr->Initialize( actor, propertyMap );
  }
  else
  {
    DALI_LOG_ERROR( "Renderer type unknown" );
  }

  return Toolkit::ControlRenderer( rendererPtr );
}

Toolkit::ControlRenderer RendererFactory::GetControlRenderer( const Vector4& color )
{
  if( !mFactoryCache )
  {
    mFactoryCache = new RendererFactoryCache();
  }

  if( mDebugEnabled )
  {
    return Toolkit::ControlRenderer( new DebugRenderer( *( mFactoryCache.Get() ) ) );
  }

  ColorRenderer* rendererPtr = new ColorRenderer( *( mFactoryCache.Get() ) );
  rendererPtr->SetColor( color );

  return Toolkit::ControlRenderer( rendererPtr );
}

void RendererFactory::ResetRenderer( Toolkit::ControlRenderer& renderer, Actor& actor, const Vector4& color )
{
  if( mDebugEnabled && renderer )
  {
    return;
  }

  if( renderer )
  {
    ColorRenderer* rendererPtr = dynamic_cast< ColorRenderer* >( &GetImplementation( renderer ) );
    if( rendererPtr )
    {
      rendererPtr->SetColor( color );
      return;
    }

    renderer.RemoveAndReset( actor );
  }

  renderer = GetControlRenderer( color );
  if( actor && actor.OnStage() )
  {
    renderer.SetOnStage( actor );
  }
}

Toolkit::ControlRenderer RendererFactory::GetControlRenderer( float borderSize, const Vector4& borderColor, bool antiAliasing )
{
  if( !mFactoryCache )
  {
    mFactoryCache = new RendererFactoryCache();
  }

  if( mDebugEnabled )
  {
    return Toolkit::ControlRenderer( new DebugRenderer( *( mFactoryCache.Get() ) ) );
  }

  BorderRenderer* rendererPtr = new BorderRenderer( *mFactoryCache.Get() );

  rendererPtr->SetBorderSize( borderSize );
  rendererPtr->SetBorderColor( borderColor );
  rendererPtr->RequireAntiAliasing( antiAliasing );

  return Toolkit::ControlRenderer( rendererPtr );
}

Toolkit::ControlRenderer RendererFactory::GetControlRenderer( const Image& image )
{
  if( !mFactoryCache )
  {
    mFactoryCache = new RendererFactoryCache();
  }

  if( mDebugEnabled )
  {
    return Toolkit::ControlRenderer( new DebugRenderer( *( mFactoryCache.Get() ) ) );
  }

  NinePatchImage npatchImage = NinePatchImage::DownCast( image );
  if( npatchImage )
  {
    NPatchRenderer* rendererPtr = new NPatchRenderer( *( mFactoryCache.Get() ) );
    rendererPtr->SetImage( npatchImage );

    return Toolkit::ControlRenderer( rendererPtr );
  }
  else
  {
    CreateAtlasManager();
    ImageRenderer* rendererPtr = new ImageRenderer( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
    Actor actor;
    rendererPtr->SetImage( actor, image );

    return Toolkit::ControlRenderer( rendererPtr );
  }
}

void RendererFactory::ResetRenderer( Toolkit::ControlRenderer& renderer, Actor& actor, const Image& image )
{
  if( mDebugEnabled && renderer )
  {
    return;
  }

  if( renderer )
  {
    if( ! image )
    {
      // If the image is empty, then reset the renderer and return
      renderer.RemoveAndReset( actor );
      return;
    }

    NinePatchImage npatchImage = NinePatchImage::DownCast( image );
    if( npatchImage )
    {
      NPatchRenderer* rendererPtr = dynamic_cast< NPatchRenderer* >( &GetImplementation( renderer ) );
      if( rendererPtr )
      {
        rendererPtr->SetImage( npatchImage );
        return;
      }
    }
    else
    {
      ImageRenderer* rendererPtr = dynamic_cast< ImageRenderer* >( &GetImplementation( renderer ) );
      if( rendererPtr )
      {
        rendererPtr->SetImage( actor, image );
        return;
      }
    }

    renderer.RemoveAndReset( actor );
  }

  renderer = GetControlRenderer( image );
  if( actor && actor.OnStage() )
  {
    renderer.SetOnStage( actor );
  }
}

Toolkit::ControlRenderer RendererFactory::GetControlRenderer( const std::string& url, ImageDimensions size )
{
  if( !mFactoryCache )
  {
    mFactoryCache = new RendererFactoryCache();
  }

  if( mDebugEnabled )
  {
    return Toolkit::ControlRenderer( new DebugRenderer( *( mFactoryCache.Get() ) ) );
  }

  if( NinePatchImage::IsNinePatchUrl( url ) )
  {
    NPatchRenderer* rendererPtr = new NPatchRenderer( *( mFactoryCache.Get() ) );
    rendererPtr->SetImage( url );

    return Toolkit::ControlRenderer( rendererPtr );
  }
  else if( SvgRenderer::IsSvgUrl( url ) )
  {
    CreateAtlasManager();
    SvgRenderer* rendererPtr = new SvgRenderer( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
    rendererPtr->SetImage( url, size );
    return Toolkit::ControlRenderer( rendererPtr );
  }
  else
  {
    CreateAtlasManager();
    ImageRenderer* rendererPtr = new ImageRenderer( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
    Actor actor;
    rendererPtr->SetImage( actor, url, size );

    return Toolkit::ControlRenderer( rendererPtr );
  }
}

void RendererFactory::ResetRenderer( Toolkit::ControlRenderer& renderer, Actor& actor, const std::string& url, ImageDimensions size )
{
  if( mDebugEnabled && renderer )
  {
    return;
  }

  if( renderer )
  {
    if( url.empty() )
    {
      // If the URL is empty, then reset the renderer and return
      renderer.RemoveAndReset( actor );
      return;
    }
    else if( NinePatchImage::IsNinePatchUrl( url ) )
    {
      NPatchRenderer* rendererPtr = dynamic_cast< NPatchRenderer* >( &GetImplementation( renderer ) );
      if( rendererPtr )
      {
        rendererPtr->SetImage( url );
        return;
      }
    }
    else if( SvgRenderer::IsSvgUrl( url ) )
    {
      SvgRenderer* rendererPtr = dynamic_cast< SvgRenderer* >( &GetImplementation( renderer ) );
      if( rendererPtr )
      {
        rendererPtr->SetImage( url, size );
        return;
      }
    }
    else
    {
      ImageRenderer* rendererPtr = dynamic_cast< ImageRenderer* >( &GetImplementation( renderer ) );
      if( rendererPtr )
      {
        rendererPtr->SetImage( actor, url, size );
        return;
      }
    }

    renderer.RemoveAndReset( actor );
  }

  renderer = GetControlRenderer( url, size );
  if( actor && actor.OnStage() )
  {
    renderer.SetOnStage( actor );
  }
}

void RendererFactory::ResetRenderer( Toolkit::ControlRenderer& renderer, Actor& actor, const Property::Map& propertyMap )
{
  if( mDebugEnabled && renderer )
  {
    return;
  }

  if( renderer )
  {
    ControlRenderer& controlRenderer = GetImplementation( renderer );

    RendererType type = GetRendererType( propertyMap );

    //If there's no renderer type specified or if there hasn't been a renderer type change then we can reuse the renderer
    if( type == UNDEFINED ||
        ( type == IMAGE     && typeid( controlRenderer ) == typeid( ImageRenderer ) ) ||
        ( type == N_PATCH   && typeid( controlRenderer ) == typeid( NPatchRenderer ) ) ||
        ( type == COLOR     && typeid( controlRenderer ) == typeid( ColorRenderer ) ) ||
        ( type == GRADIENT  && typeid( controlRenderer ) == typeid( GradientRenderer ) ) ||
        ( type == BORDER    && typeid( controlRenderer ) == typeid( BorderRenderer ) ) ||
        ( type == SVG       && typeid( controlRenderer ) == typeid( SvgRenderer ) ) ||
        ( type == MESH      && typeid( controlRenderer ) == typeid( MeshRenderer ) ) )
    {
      controlRenderer.Initialize( actor, propertyMap );
      return;
    }

    renderer.RemoveAndReset( actor );
  }

  renderer = GetControlRenderer( propertyMap );
  if( renderer && actor && actor.OnStage() )
  {
    renderer.SetOnStage( actor );
  }
}

Image RendererFactory::GetBrokenRendererImage()
{
  return ResourceImage::New( BROKEN_RENDERER_IMAGE_URL );
}

void RendererFactory::CreateAtlasManager()
{
  if( !mAtlasManager )
  {
    Shader shader = ImageRenderer::GetImageShader( *( mFactoryCache.Get() ) );
    mAtlasManager = new ImageAtlasManager();
    mAtlasManager->SetBrokenImage( BROKEN_RENDERER_IMAGE_URL );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
