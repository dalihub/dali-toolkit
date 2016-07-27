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
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/border/border-visual.h>
#include <dali-toolkit/internal/visuals/color/color-visual.h>
#include <dali-toolkit/internal/visuals/debug/debug-visual.h>
#include <dali-toolkit/internal/visuals/gradient/gradient-visual.h>
#include <dali-toolkit/internal/visuals/npatch/npatch-visual.h>
#include <dali-toolkit/internal/visuals/image/image-visual.h>
#include <dali-toolkit/internal/visuals/svg/svg-visual.h>
#include <dali-toolkit/internal/visuals/mesh/mesh-visual.h>
#include <dali-toolkit/internal/visuals/primitive/primitive-visual.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>

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
  BaseHandle handle = Toolkit::VisualFactory::Get();

  return handle;
}

DALI_TYPE_REGISTRATION_BEGIN_CREATE( Toolkit::VisualFactory, Dali::BaseHandle, Create, true )
DALI_TYPE_REGISTRATION_END()

} // namespace

VisualFactory::VisualFactory( bool debugEnabled )
:mDebugEnabled( debugEnabled )
{
}

VisualFactory::~VisualFactory()
{
}

VisualFactory::RendererType VisualFactory::GetRendererType( const Property::Map& propertyMap )
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
      else if( SvgVisual::IsSvgUrl( imageUrl ) )
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

Toolkit::Visual::Base VisualFactory::CreateVisual( const Property::Map& propertyMap )
{
  Visual::Base* rendererPtr = NULL;

  RendererType type = GetRendererType( propertyMap );
  if( type != UNDEFINED)
  {
    if( !mFactoryCache )
    {
      mFactoryCache = new VisualFactoryCache();
    }

    if( mDebugEnabled )
    {
      return Toolkit::Visual::Base( new DebugVisual( *( mFactoryCache.Get() ) ) );
    }
  }

  switch( type )
  {
    case COLOR:
    {
      rendererPtr = new ColorVisual( *( mFactoryCache.Get() ) );
      break;
    }
     case GRADIENT:
     {
       rendererPtr = new GradientVisual( *( mFactoryCache.Get() ) );
       break;
     }
    case BORDER:
    {
      rendererPtr = new BorderVisual( *( mFactoryCache.Get() ) );
      break;
    }
    case IMAGE:
    {
      CreateAtlasManager();
      rendererPtr = new ImageVisual( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
      break;
    }
    case N_PATCH:
    {
      rendererPtr = new NPatchVisual( *( mFactoryCache.Get() ) );
      break;
    }
    case SVG:
    {
      CreateAtlasManager();
      rendererPtr = new SvgVisual( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
      break;
    }
    case MESH:
    {
      rendererPtr = new MeshVisual( *( mFactoryCache.Get() ) );
      break;
    }
    case PRIMITIVE:
    {
      rendererPtr = new PrimitiveVisual( *( mFactoryCache.Get() ) );
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

  return Toolkit::Visual::Base( rendererPtr );
}

Toolkit::Visual::Base VisualFactory::CreateVisual( const Image& image )
{
  if( !mFactoryCache )
  {
    mFactoryCache = new VisualFactoryCache();
  }

  if( mDebugEnabled )
  {
    return Toolkit::Visual::Base( new DebugVisual( *( mFactoryCache.Get() ) ) );
  }

  NinePatchImage npatchImage = NinePatchImage::DownCast( image );
  if( npatchImage )
  {
    NPatchVisual* rendererPtr = new NPatchVisual( *( mFactoryCache.Get() ) );
    rendererPtr->SetImage( npatchImage );

    return Toolkit::Visual::Base( rendererPtr );
  }
  else
  {
    CreateAtlasManager();
    ImageVisual* rendererPtr = new ImageVisual( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
    Actor actor;
    rendererPtr->SetImage( actor, image );

    return Toolkit::Visual::Base( rendererPtr );
  }
}

Toolkit::Visual::Base VisualFactory::CreateVisual( const std::string& url, ImageDimensions size )
{
  if( !mFactoryCache )
  {
    mFactoryCache = new VisualFactoryCache();
  }

  if( mDebugEnabled )
  {
    return Toolkit::Visual::Base( new DebugVisual( *( mFactoryCache.Get() ) ) );
  }

  if( NinePatchImage::IsNinePatchUrl( url ) )
  {
    NPatchVisual* rendererPtr = new NPatchVisual( *( mFactoryCache.Get() ) );
    rendererPtr->SetImage( url );

    return Toolkit::Visual::Base( rendererPtr );
  }
  else if( SvgVisual::IsSvgUrl( url ) )
  {
    CreateAtlasManager();
    SvgVisual* rendererPtr = new SvgVisual( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
    rendererPtr->SetImage( url, size );
    return Toolkit::Visual::Base( rendererPtr );
  }
  else
  {
    CreateAtlasManager();
    ImageVisual* rendererPtr = new ImageVisual( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
    Actor actor;
    rendererPtr->SetImage( actor, url, size );

    return Toolkit::Visual::Base( rendererPtr );
  }
}

Image VisualFactory::GetBrokenRendererImage()
{
  return ResourceImage::New( BROKEN_RENDERER_IMAGE_URL );
}

void VisualFactory::CreateAtlasManager()
{
  if( !mAtlasManager )
  {
    Shader shader = ImageVisual::GetImageShader( *( mFactoryCache.Get() ) );
    mAtlasManager = new ImageAtlasManager();
    mAtlasManager->SetBrokenImage( BROKEN_RENDERER_IMAGE_URL );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
