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
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
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
#include <dali-toolkit/internal/visuals/image/batch-image-visual.h>

namespace
{
const char * const BROKEN_VISUAL_IMAGE_URL( DALI_IMAGE_DIR "broken.png");
}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

DALI_ENUM_TO_STRING_TABLE_BEGIN( VISUAL_TYPE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Visual, BORDER )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Visual, COLOR )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Visual, GRADIENT )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Visual, IMAGE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Visual, MESH )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Visual, PRIMITIVE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Visual, DEBUG )
DALI_ENUM_TO_STRING_TABLE_END( VISUAL_TYPE )

const char * const VISUAL_TYPE( "visualType" );
const char * const BATCHING_ENABLED( "batchingEnabled" );
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

  // Return a new DebugVisual if we have debug enabled
  if( mDebugEnabled )
  {
    return Toolkit::Visual::Base( new DebugVisual( *( mFactoryCache.Get() ) ) );
  }

  Visual::Base* visualPtr = NULL;

  Property::Value* typeValue = propertyMap.Find( Toolkit::Visual::Property::TYPE, VISUAL_TYPE );
  Toolkit::Visual::Type visualType = Toolkit::Visual::IMAGE; // Default to IMAGE type.
  if( typeValue )
  {
    Scripting::GetEnumerationProperty( *typeValue, VISUAL_TYPE_TABLE, VISUAL_TYPE_TABLE_COUNT, visualType );
  }

  switch( visualType )
  {
    case Toolkit::Visual::BORDER:
    {
      visualPtr = new BorderVisual( *( mFactoryCache.Get() ) );
      break;
    }

    case Toolkit::Visual::COLOR:
    {
      visualPtr = new ColorVisual( *( mFactoryCache.Get() ) );
      break;
    }

    case Toolkit::Visual::GRADIENT:
    {
      visualPtr = new GradientVisual( *( mFactoryCache.Get() ) );
      break;
    }

    default: // Default to Image type if unknown (check if there is a URL)
    case Toolkit::Visual::IMAGE:
    {
      Property::Value* imageURLValue = propertyMap.Find( Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME );
      std::string imageUrl;
      if( imageURLValue && imageURLValue->Get( imageUrl ) )
      {
        Property::Value* batchingEnabledValue = propertyMap.Find( Toolkit::ImageVisual::Property::BATCHING_ENABLED, BATCHING_ENABLED );
        if( batchingEnabledValue  )
        {
          bool batchingEnabled( false );
          batchingEnabledValue->Get( batchingEnabled );
          if( batchingEnabled )
          {
            CreateAtlasManager();
            visualPtr = new BatchImageVisual( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
            break;
          }
        }
        else if( NinePatchImage::IsNinePatchUrl( imageUrl ) )
        {
          visualPtr = new NPatchVisual( *( mFactoryCache.Get() ) );
        }
        else
        {
          CreateAtlasManager();

          if( SvgVisual::IsSvgUrl( imageUrl ) )
          {
            visualPtr = new SvgVisual( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
          }
          else
          {
            visualPtr = new ImageVisual( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
          }
        }
      }
      else if( propertyMap.Find( Toolkit::Visual::Property::SHADER, CUSTOM_SHADER ) )
      {
        // Create Image Visual if it has a shader
        // TODO: This is required because of EffectsView which should be fixed
        CreateAtlasManager();
        visualPtr = new ImageVisual( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
      }
      break;
    }

    case Toolkit::Visual::MESH:
    {
      visualPtr = new MeshVisual( *( mFactoryCache.Get() ) );
      break;
    }

    case Toolkit::Visual::PRIMITIVE:
    {
      visualPtr = new PrimitiveVisual( *( mFactoryCache.Get() ) );
      break;
    }

    case Toolkit::Visual::DEBUG:
    {
      visualPtr = new DebugVisual( *( mFactoryCache.Get() ) );
      break;
    }

  }

  if( visualPtr )
  {
    Actor actor;
    visualPtr->Initialize( actor, propertyMap );
  }
  else
  {
    DALI_LOG_ERROR( "Renderer type unknown\n" );
  }

  return Toolkit::Visual::Base( visualPtr );
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
    NPatchVisual* visualPtr = new NPatchVisual( *( mFactoryCache.Get() ) );
    visualPtr->SetImage( npatchImage );

    return Toolkit::Visual::Base( visualPtr );
  }
  else
  {
    CreateAtlasManager();
    ImageVisual* visualPtr = new ImageVisual( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
    Actor actor;
    visualPtr->SetImage( actor, image );

    return Toolkit::Visual::Base( visualPtr );
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
    NPatchVisual* visualPtr = new NPatchVisual( *( mFactoryCache.Get() ) );
    visualPtr->SetImage( url );

    return Toolkit::Visual::Base( visualPtr );
  }
  else if( SvgVisual::IsSvgUrl( url ) )
  {
    CreateAtlasManager();
    SvgVisual* visualPtr = new SvgVisual( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
    visualPtr->SetImage( url, size );
    return Toolkit::Visual::Base( visualPtr );
  }
  else
  {
    CreateAtlasManager();
    ImageVisual* visualPtr = new ImageVisual( *( mFactoryCache.Get() ), *( mAtlasManager.Get() ) );
    Actor actor;
    visualPtr->SetImage( actor, url, size );

    return Toolkit::Visual::Base( visualPtr );
  }
}

Image VisualFactory::GetBrokenVisualImage()
{
  return ResourceImage::New( BROKEN_VISUAL_IMAGE_URL );
}

void VisualFactory::CreateAtlasManager()
{
  if( !mAtlasManager )
  {
    Shader shader = ImageVisual::GetImageShader( *( mFactoryCache.Get() ) );
    mAtlasManager = new ImageAtlasManager();
    mAtlasManager->SetBrokenImage( BROKEN_VISUAL_IMAGE_URL );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
