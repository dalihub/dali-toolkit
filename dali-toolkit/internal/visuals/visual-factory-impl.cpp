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
 */

// CLASS HEADER
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/devel-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/text-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/visuals/border/border-visual.h>
#include <dali-toolkit/internal/visuals/color/color-visual.h>
#include <dali-toolkit/internal/visuals/gradient/gradient-visual.h>
#include <dali-toolkit/internal/visuals/image/image-visual.h>
#include <dali-toolkit/internal/visuals/mesh/mesh-visual.h>
#include <dali-toolkit/internal/visuals/npatch/npatch-visual.h>
#include <dali-toolkit/internal/visuals/primitive/primitive-visual.h>
#include <dali-toolkit/internal/visuals/svg/svg-visual.h>
#include <dali-toolkit/internal/visuals/text/text-visual.h>
#include <dali-toolkit/internal/visuals/animated-image/animated-image-visual.h>
#include <dali-toolkit/internal/visuals/wireframe/wireframe-visual.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-factory-resolve-url.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>

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

Toolkit::Visual::Base VisualFactory::CreateVisual( const Property::Map& propertyMap )
{
  // Create factory cache if it hasn't already been
  if( !mFactoryCache )
  {
    mFactoryCache = new VisualFactoryCache();
  }

  Visual::BasePtr visualPtr;

  Property::Value* typeValue = propertyMap.Find( Toolkit::DevelVisual::Property::TYPE, VISUAL_TYPE );
  Toolkit::DevelVisual::Type visualType = Toolkit::DevelVisual::IMAGE; // Default to IMAGE type.
  if( typeValue )
  {
    Scripting::GetEnumerationProperty( *typeValue, VISUAL_TYPE_TABLE, VISUAL_TYPE_TABLE_COUNT, visualType );
  }

  switch( visualType )
  {
    case Toolkit::Visual::BORDER:
    {
      visualPtr = BorderVisual::New( *( mFactoryCache.Get() ), propertyMap );
      break;
    }

    case Toolkit::Visual::COLOR:
    {
      visualPtr = ColorVisual::New( *( mFactoryCache.Get() ), propertyMap );
      break;
    }

    case Toolkit::Visual::GRADIENT:
    {
      visualPtr = GradientVisual::New( *( mFactoryCache.Get() ), propertyMap );
      break;
    }

    case Toolkit::Visual::IMAGE:
    {
      Property::Value* imageURLValue = propertyMap.Find( Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME );
      std::string imageUrl;
      if( imageURLValue && imageURLValue->Get( imageUrl ) )
      {
        // first resolve url type to know which visual to create
        UrlType::Type type = ResolveUrlType( imageUrl );
        if( UrlType::N_PATCH == type )
        {
          visualPtr = NPatchVisual::New( *( mFactoryCache.Get() ), imageUrl, propertyMap );
        }
        else if( UrlType::SVG == type )
        {
          visualPtr = SvgVisual::New( *( mFactoryCache.Get() ), imageUrl, propertyMap );
        }
        else if( UrlType::GIF == type )
        {
          visualPtr = AnimatedImageVisual::New( *( mFactoryCache.Get() ), imageUrl, propertyMap );
        }
        else // Regular image
        {
          visualPtr = ImageVisual::New( *( mFactoryCache.Get() ), imageUrl, propertyMap );
        }
      }

      break;
    }

    case Toolkit::Visual::MESH:
    {
      visualPtr = MeshVisual::New( *( mFactoryCache.Get() ), propertyMap );
      break;
    }

    case Toolkit::Visual::PRIMITIVE:
    {
      visualPtr = PrimitiveVisual::New( *( mFactoryCache.Get() ), propertyMap );
      break;
    }

    case Toolkit::Visual::WIREFRAME:
    {
      visualPtr = WireframeVisual::New( *( mFactoryCache.Get() ), propertyMap );
      break;
    }

    case Toolkit::DevelVisual::TEXT:
    {
      visualPtr = TextVisual::New( *( mFactoryCache.Get() ), propertyMap );
      break;
    }

    case Toolkit::DevelVisual::N_PATCH:
    {
      Property::Value* imageURLValue = propertyMap.Find( Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME );
      std::string imageUrl;
      if( imageURLValue && imageURLValue->Get( imageUrl ) )
      {
        visualPtr = NPatchVisual::New( *( mFactoryCache.Get() ), imageUrl, propertyMap );
      }
      break;
    }

    case Toolkit::DevelVisual::SVG:
    {
      Property::Value* imageURLValue = propertyMap.Find( Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME );
      std::string imageUrl;
      if( imageURLValue && imageURLValue->Get( imageUrl ) )
      {
        visualPtr = SvgVisual::New( *( mFactoryCache.Get() ), imageUrl, propertyMap );
      }
      break;
    }

    case Toolkit::DevelVisual::ANIMATED_IMAGE:
    {
      Property::Value* imageURLValue = propertyMap.Find( Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME );
      std::string imageUrl;
      if( imageURLValue && imageURLValue->Get( imageUrl ) )
      {
        visualPtr = AnimatedImageVisual::New( *( mFactoryCache.Get() ), imageUrl, propertyMap );
      }
      break;
    }
  }

  if( !visualPtr )
  {
    DALI_LOG_ERROR( "Renderer type unknown\n" );
  }

  if( mDebugEnabled && visualType !=  Toolkit::DevelVisual::WIREFRAME )
  {
    //Create a WireframeVisual if we have debug enabled
    visualPtr = WireframeVisual::New( *( mFactoryCache.Get() ), visualPtr, propertyMap );
  }

  return Toolkit::Visual::Base( visualPtr.Get() );
}

Toolkit::Visual::Base VisualFactory::CreateVisual( const Image& image )
{
  if( !mFactoryCache )
  {
    mFactoryCache = new VisualFactoryCache();
  }

  Visual::BasePtr visualPtr;

  NinePatchImage npatchImage = NinePatchImage::DownCast( image );
  if( npatchImage )
  {
    visualPtr = NPatchVisual::New( *( mFactoryCache.Get() ), npatchImage );
  }
  else
  {
    visualPtr = ImageVisual::New( *( mFactoryCache.Get() ), image );
  }

  if( mDebugEnabled )
  {
    //Create a WireframeVisual if we have debug enabled
    visualPtr = WireframeVisual::New( *( mFactoryCache.Get() ), visualPtr );
  }

  return Toolkit::Visual::Base( visualPtr.Get() );
}

Toolkit::Visual::Base VisualFactory::CreateVisual( const std::string& url, ImageDimensions size )
{
  if( !mFactoryCache )
  {
    mFactoryCache = new VisualFactoryCache();
  }

  Visual::BasePtr visualPtr;

  // first resolve url type to know which visual to create
  UrlType::Type type = ResolveUrlType( url );
  if( UrlType::N_PATCH == type )
  {
    visualPtr = NPatchVisual::New( *( mFactoryCache.Get() ), url );
  }
  else if( UrlType::SVG == type )
  {
    visualPtr = SvgVisual::New( *( mFactoryCache.Get() ), url );
  }
  else if( UrlType::GIF == type )
  {
    visualPtr = AnimatedImageVisual::New( *( mFactoryCache.Get() ), url );
  }
  else // Regular image
  {
    visualPtr = ImageVisual::New( *( mFactoryCache.Get() ), url, size );
  }

  if( mDebugEnabled )
  {
    //Create a WireframeVisual if we have debug enabled
    visualPtr = WireframeVisual::New( *( mFactoryCache.Get() ), visualPtr );
  }

  return Toolkit::Visual::Base( visualPtr.Get() );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
