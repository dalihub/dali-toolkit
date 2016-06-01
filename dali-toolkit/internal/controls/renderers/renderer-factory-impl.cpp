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
#include <dali-toolkit/internal/controls/renderers/primitive/primitive-renderer.h>
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
    else if( typeValue == PRIMITIVE_RENDERER )
    {
      rendererType = PRIMITIVE;
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

Toolkit::ControlRenderer RendererFactory::CreateControlRenderer( const Property::Map& propertyMap )
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
    case PRIMITIVE:
    {
      rendererPtr = new PrimitiveRenderer( *( mFactoryCache.Get() ) );
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

Toolkit::ControlRenderer RendererFactory::CreateControlRenderer( const Image& image )
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

Toolkit::ControlRenderer RendererFactory::CreateControlRenderer( const std::string& url, ImageDimensions size )
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
