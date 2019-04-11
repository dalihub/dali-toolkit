 /*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/public-api/visuals/text-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/internal/visuals/border/border-visual.h>
#include <dali-toolkit/internal/visuals/color/color-visual.h>
#include <dali-toolkit/internal/visuals/gradient/gradient-visual.h>
#include <dali-toolkit/internal/visuals/animated-gradient/animated-gradient-visual.h>
#include <dali-toolkit/internal/visuals/image/image-visual.h>
#include <dali-toolkit/internal/visuals/mesh/mesh-visual.h>
#include <dali-toolkit/internal/visuals/npatch/npatch-visual.h>
#include <dali-toolkit/internal/visuals/primitive/primitive-visual.h>
#include <dali-toolkit/internal/visuals/svg/svg-visual.h>
#include <dali-toolkit/internal/visuals/text/text-visual.h>
#include <dali-toolkit/internal/visuals/animated-image/animated-image-visual.h>
#include <dali-toolkit/internal/visuals/animated-vector-image/animated-vector-image-visual.h>
#include <dali-toolkit/internal/visuals/wireframe/wireframe-visual.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-url.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/image-visual-shader-factory.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_CONTROL_VISUALS");
#endif

BaseHandle Create()
{
  BaseHandle handle = Toolkit::VisualFactory::Get();

  return handle;
}

DALI_TYPE_REGISTRATION_BEGIN_CREATE( Toolkit::VisualFactory, Dali::BaseHandle, Create, true )
DALI_TYPE_REGISTRATION_END()
const char * const  BROKEN_IMAGE_URL( DALI_IMAGE_DIR "broken.png" ); ///< URL For the broken image

} // namespace

VisualFactory::VisualFactory( bool debugEnabled )
: mFactoryCache(),
  mImageVisualShaderFactory(),
  mSlotDelegate(this),
  mDebugEnabled( debugEnabled ),
  mPreMultiplyOnLoad( true )
{
}

VisualFactory::~VisualFactory()
{
}

void VisualFactory::OnStyleChangedSignal( Toolkit::StyleManager styleManager, StyleChange::Type type)
{
  if( type == StyleChange::THEME_CHANGE )
  {
    std::string brokenImageUrl(BROKEN_IMAGE_URL);

    Property::Map config = Toolkit::DevelStyleManager::GetConfigurations( styleManager );
    config["brokenImageUrl"].Get( brokenImageUrl );

    if( mFactoryCache )
    {
      mFactoryCache->SetBrokenImageUrl(brokenImageUrl);
    }
  }
}

Toolkit::Visual::Base VisualFactory::CreateVisual( const Property::Map& propertyMap )
{
  Visual::BasePtr visualPtr;

  Property::Value* typeValue = propertyMap.Find( Toolkit::Visual::Property::TYPE, VISUAL_TYPE );
  Toolkit::DevelVisual::Type visualType = Toolkit::DevelVisual::IMAGE; // Default to IMAGE type.
  if( typeValue )
  {
    Scripting::GetEnumerationProperty( *typeValue, VISUAL_TYPE_TABLE, VISUAL_TYPE_TABLE_COUNT, visualType );
  }

  switch( visualType )
  {
    case Toolkit::Visual::BORDER:
    {
      visualPtr = BorderVisual::New( GetFactoryCache(), propertyMap );
      break;
    }

    case Toolkit::Visual::COLOR:
    {
      visualPtr = ColorVisual::New( GetFactoryCache(), propertyMap );
      break;
    }

    case Toolkit::Visual::GRADIENT:
    {
      visualPtr = GradientVisual::New( GetFactoryCache(), propertyMap );
      break;
    }

    case Toolkit::Visual::IMAGE:
    {
      Property::Value* imageURLValue = propertyMap.Find( Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME );
      std::string imageUrl;
      if( imageURLValue )
      {
        if( imageURLValue->Get( imageUrl ) )
        {
          if( !imageUrl.empty() )
          {
            VisualUrl visualUrl( imageUrl );

            switch( visualUrl.GetType() )
            {
              case VisualUrl::N_PATCH:
              {
                visualPtr = NPatchVisual::New( GetFactoryCache(), visualUrl, propertyMap );
                break;
              }
              case VisualUrl::SVG:
              {
                visualPtr = SvgVisual::New( GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, propertyMap );
                break;
              }
              case VisualUrl::GIF:
              {
                visualPtr = AnimatedImageVisual::New( GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, propertyMap );
                break;
              }
              case VisualUrl::JSON:
              {
                visualPtr = AnimatedVectorImageVisual::New( GetFactoryCache(),  GetImageVisualShaderFactory(), imageUrl, propertyMap );
                break;
              }
              case VisualUrl::REGULAR_IMAGE:
              {
                visualPtr = ImageVisual::New( GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, propertyMap );
                break;
              }
            }
          }
        }
        else
        {
          Property::Array* array = imageURLValue->GetArray();
          if( array )
          {
            visualPtr = AnimatedImageVisual::New( GetFactoryCache(), GetImageVisualShaderFactory(), *array, propertyMap );
          }
        }
      }
      break;
    }

    case Toolkit::Visual::MESH:
    {
      visualPtr = MeshVisual::New( GetFactoryCache(), propertyMap );
      break;
    }

    case Toolkit::Visual::PRIMITIVE:
    {
      visualPtr = PrimitiveVisual::New( GetFactoryCache(), propertyMap );
      break;
    }

    case Toolkit::Visual::WIREFRAME:
    {
      visualPtr = WireframeVisual::New( GetFactoryCache(), propertyMap );
      break;
    }

    case Toolkit::Visual::TEXT:
    {
      visualPtr = TextVisual::New( GetFactoryCache(), propertyMap );
      break;
    }

    case Toolkit::Visual::N_PATCH:
    {
      Property::Value* imageURLValue = propertyMap.Find( Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME );
      std::string imageUrl;
      if( imageURLValue && imageURLValue->Get( imageUrl ) )
      {
        visualPtr = NPatchVisual::New( GetFactoryCache(), imageUrl, propertyMap );
      }
      break;
    }

    case Toolkit::Visual::SVG:
    {
      Property::Value* imageURLValue = propertyMap.Find( Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME );
      std::string imageUrl;
      if( imageURLValue && imageURLValue->Get( imageUrl ) )
      {
        visualPtr = SvgVisual::New( GetFactoryCache(), GetImageVisualShaderFactory(), imageUrl, propertyMap );
      }
      break;
    }

    case Toolkit::Visual::ANIMATED_IMAGE:
    {
      Property::Value* imageURLValue = propertyMap.Find( Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME );
      std::string imageUrl;
      if( imageURLValue )
      {
        if( imageURLValue->Get( imageUrl ) )
        {
          visualPtr = AnimatedImageVisual::New( GetFactoryCache(), GetImageVisualShaderFactory(), imageUrl, propertyMap );
        }
        else
        {
          Property::Array* array = imageURLValue->GetArray();
          if( array )
          {
            visualPtr = AnimatedImageVisual::New( GetFactoryCache(), GetImageVisualShaderFactory(), *array, propertyMap );
          }
        }
      }
      break;
    }

    case Toolkit::DevelVisual::ANIMATED_GRADIENT:
    {
      visualPtr = AnimatedGradientVisual::New( GetFactoryCache(), propertyMap );
      break;
    }

    case Toolkit::DevelVisual::ANIMATED_VECTOR_IMAGE:
    {
      Property::Value* imageURLValue = propertyMap.Find( Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME );
      std::string imageUrl;
      if( imageURLValue && imageURLValue->Get( imageUrl ) )
      {
        visualPtr = AnimatedVectorImageVisual::New( GetFactoryCache(),  GetImageVisualShaderFactory(), imageUrl, propertyMap );
      }
      break;
    }
  }

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "VisualFactory::CreateVisual( VisualType:%s %s%s)\n",
                 Scripting::GetEnumerationName<Toolkit::DevelVisual::Type>( visualType,
                                                                            VISUAL_TYPE_TABLE,
                                                                            VISUAL_TYPE_TABLE_COUNT ),
                 visualType==Toolkit::DevelVisual::IMAGE?"url:":"",
                 visualType==Toolkit::DevelVisual::IMAGE ?
                 propertyMap.Find( Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME)->Get<std::string>().c_str()
                 :"" );


  if( !visualPtr )
  {
    DALI_LOG_ERROR( "VisualType unknown\n" );
  }

  if( mDebugEnabled && visualType !=  Toolkit::DevelVisual::WIREFRAME )
  {
    //Create a WireframeVisual if we have debug enabled
    visualPtr = WireframeVisual::New(GetFactoryCache(), visualPtr, propertyMap );
  }

  return Toolkit::Visual::Base( visualPtr.Get() );
}

Toolkit::Visual::Base VisualFactory::CreateVisual( const Image& image )
{
  Visual::BasePtr visualPtr;

  if( image )
  {
    NinePatchImage npatchImage = NinePatchImage::DownCast( image );
    if( npatchImage )
    {
      visualPtr = NPatchVisual::New( GetFactoryCache(), npatchImage );
    }
    else
    {
      visualPtr = ImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), image );
    }
  }

  if( mDebugEnabled )
  {
    //Create a WireframeVisual if we have debug enabled
    visualPtr = WireframeVisual::New( GetFactoryCache(), visualPtr );
  }

  return Toolkit::Visual::Base( visualPtr.Get() );
}

Toolkit::Visual::Base VisualFactory::CreateVisual( const std::string& url, ImageDimensions size )
{
  Visual::BasePtr visualPtr;

  if( !url.empty() )
  {
    // first resolve url type to know which visual to create
    VisualUrl visualUrl( url );
    switch( visualUrl.GetType() )
    {
      case VisualUrl::N_PATCH:
      {
        visualPtr = NPatchVisual::New( GetFactoryCache(), visualUrl );
        break;
      }
      case VisualUrl::SVG:
      {
        visualPtr = SvgVisual::New( GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl );
        break;
      }
      case VisualUrl::GIF:
      {
        visualPtr = AnimatedImageVisual::New( GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl );
        break;
      }
      case VisualUrl::JSON:
      {
        visualPtr = AnimatedVectorImageVisual::New( GetFactoryCache(),  GetImageVisualShaderFactory(), visualUrl );
        break;
      }
      case VisualUrl::REGULAR_IMAGE:
      {
        visualPtr = ImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, size );
        break;
      }
    }
  }

  if( mDebugEnabled )
  {
    //Create a WireframeVisual if we have debug enabled
    visualPtr = WireframeVisual::New( GetFactoryCache(), visualPtr );
  }

  return Toolkit::Visual::Base( visualPtr.Get() );
}

void VisualFactory::SetPreMultiplyOnLoad( bool preMultiply )
{
  if( mPreMultiplyOnLoad != preMultiply )
  {
    GetFactoryCache().SetPreMultiplyOnLoad( preMultiply );
  }
  mPreMultiplyOnLoad = preMultiply;
}

bool VisualFactory::GetPreMultiplyOnLoad() const
{
  return mPreMultiplyOnLoad;
}

Internal::TextureManager& VisualFactory::GetTextureManager()
{
  return GetFactoryCache().GetTextureManager();
}

Internal::VisualFactoryCache& VisualFactory::GetFactoryCache()
{
  if( !mFactoryCache )
  {
    mFactoryCache = std::unique_ptr<VisualFactoryCache>( new VisualFactoryCache( mPreMultiplyOnLoad ) );

    std::string brokenImageUrl(BROKEN_IMAGE_URL);
    Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
    if( styleManager )
    {
      Property::Map config = Toolkit::DevelStyleManager::GetConfigurations( styleManager );
      config["brokenImageUrl"].Get( brokenImageUrl );
      styleManager.StyleChangedSignal().Connect( mSlotDelegate, &VisualFactory::OnStyleChangedSignal );
    }

    mFactoryCache->SetBrokenImageUrl(brokenImageUrl);
  }
  return *mFactoryCache;
}

ImageVisualShaderFactory& VisualFactory::GetImageVisualShaderFactory()
{
  if( !mImageVisualShaderFactory )
  {
    mImageVisualShaderFactory = std::unique_ptr< ImageVisualShaderFactory >( new ImageVisualShaderFactory() );
  }
  return *mImageVisualShaderFactory;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
