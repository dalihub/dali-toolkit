// Copyright (c) 2016 Samsung Electronics Co., Ltd.

// CLASS HEADER
#include "image-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/devel-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>

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
  return Toolkit::ImageView::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::ImageView, Toolkit::Control, Create );
DALI_PROPERTY_REGISTRATION( Toolkit, ImageView, "resourceUrl", STRING, RESOURCE_URL )
DALI_PROPERTY_REGISTRATION( Toolkit, ImageView, "image", MAP, IMAGE )
DALI_PROPERTY_REGISTRATION( Toolkit, ImageView, "preMultipliedAlpha", BOOLEAN, PRE_MULTIPLIED_ALPHA )

DALI_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT( Toolkit, ImageView, "pixelArea", Vector4(0.f, 0.f, 1.f, 1.f), PIXEL_AREA )
DALI_TYPE_REGISTRATION_END()

} // anonymous namespace

using namespace Dali;

ImageView::ImageView()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) )
{
}

ImageView::~ImageView()
{
}

Toolkit::ImageView ImageView::New()
{
  ImageView* impl = new ImageView();

  Toolkit::ImageView handle = Toolkit::ImageView( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

/////////////////////////////////////////////////////////////

void ImageView::SetImage( Image image )
{
  // Don't bother comparing if we had a visual previously, just drop old visual and create new one
  mImage = image;
  mUrl.clear();
  mPropertyMap.Clear();

  mVisual =  Toolkit::VisualFactory::Get().CreateVisual( image );
  RegisterVisual( Toolkit::ImageView::Property::IMAGE, mVisual  );

  RelayoutRequest();
}

void ImageView::SetImage( const Property::Map& map )
{
  // Comparing a property map is too expensive so just creating a new visual
  mPropertyMap = map;
  mUrl.clear();
  mImage.Reset();

  mVisual =  Toolkit::VisualFactory::Get().CreateVisual( mPropertyMap );
  RegisterVisual( Toolkit::ImageView::Property::IMAGE, mVisual  );

  RelayoutRequest();
}

void ImageView::SetImage( const std::string& url, ImageDimensions size )
{
  // Don't bother comparing if we had a visual previously, just drop old visual and create new one
  mUrl = url;
  mImage.Reset();
  mPropertyMap.Clear();

  mVisual =  Toolkit::VisualFactory::Get().CreateVisual( url, size );
  RegisterVisual( Toolkit::ImageView::Property::IMAGE, mVisual );

  RelayoutRequest();
}

Image ImageView::GetImage() const
{
  return mImage;
}

void ImageView::EnablePreMultipliedAlpha( bool preMultipled )
{
  if( mVisual )
  {
    Toolkit::GetImplementation( mVisual ).EnablePreMultipliedAlpha( preMultipled );
  }
}

bool ImageView::IsPreMultipliedAlphaEnabled() const
{
  if( mVisual )
  {
    return Toolkit::GetImplementation( mVisual ).IsPreMultipliedAlphaEnabled();
  }
  return false;
}

void ImageView::SetDepthIndex( int depthIndex )
{
  if( mVisual )
  {
    mVisual.SetDepthIndex( depthIndex );
  }
}

Vector3 ImageView::GetNaturalSize()
{
  if( mVisual )
  {
    Vector2 rendererNaturalSize;
    mVisual.GetNaturalSize( rendererNaturalSize );
    return Vector3( rendererNaturalSize );
  }

  // if no visual then use Control's natural size
  return Control::GetNaturalSize();
}

float ImageView::GetHeightForWidth( float width )
{
  if( mVisual )
  {
    return mVisual.GetHeightForWidth( width );
  }
  else
  {
    return Control::GetHeightForWidth( width );
  }
}

float ImageView::GetWidthForHeight( float height )
{
  if( mVisual )
  {
    return mVisual.GetWidthForHeight( height );
  }
  else
  {
    return Control::GetWidthForHeight( height );
  }
}

void ImageView::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  Control::OnRelayout( size, container );

  if( mVisual )
  {
    // Pass in an empty map which uses default transform values meaning our visual fills the control
    // Should provide a transform that handles aspect ratio according to image size
    mVisual.SetTransformAndSize( Property::Map(), size );
  }
}

///////////////////////////////////////////////////////////
//
// Properties
//

void ImageView::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::ImageView imageView = Toolkit::ImageView::DownCast( Dali::BaseHandle( object ) );

  if ( imageView )
  {
    ImageView& impl = GetImpl( imageView );
    switch ( index )
    {
      case Toolkit::ImageView::Property::RESOURCE_URL:
      {
        std::string imageUrl;
        if( value.Get( imageUrl ) )
        {
          impl.SetImage( imageUrl, ImageDimensions() );
        }
        break;
      }

      case Toolkit::ImageView::Property::IMAGE:
      {
        std::string imageUrl;
        Property::Map* map;
        if( value.Get( imageUrl ) )
        {
          impl.SetImage( imageUrl, ImageDimensions() );
        }
        // if its not a string then get a Property::Map from the property if possible.
        else
        {
          map = value.GetMap();
          if( map )
          {
            Property::Value* shaderValue = map->Find( Toolkit::DevelVisual::Property::SHADER, CUSTOM_SHADER );
            // set image only if property map contains image information other than custom shader
            if( map->Count() > 1u ||  !shaderValue )
            {
              impl.SetImage( *map );
            }
            // the property map contains only the custom shader
            else if( ( impl.mVisual )&&( map->Count() == 1u )&&( shaderValue ) )
            {
              Property::Map* shaderMap = shaderValue->GetMap();
              if( shaderMap )
              {
                Internal::Visual::Base& visual = Toolkit::GetImplementation( impl.mVisual );
                visual.SetCustomShader( *shaderMap );
                if( imageView.OnStage() )
                {
                  // force to create new core renderer to use the newly set shader
                  visual.SetOffStage( imageView );
                  visual.SetOnStage( imageView );
                }
              }
            }
          }
        }
        break;
      }

      case Toolkit::ImageView::Property::PRE_MULTIPLIED_ALPHA:
      {
        bool isPre;
        if( value.Get( isPre ) )
        {
          impl.EnablePreMultipliedAlpha( isPre );
        }
        break;
      }
    }
  }
}

Property::Value ImageView::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Toolkit::ImageView imageview = Toolkit::ImageView::DownCast( Dali::BaseHandle( object ) );

  if ( imageview )
  {
    ImageView& impl = GetImpl( imageview );
    switch ( propertyIndex )
    {
      case Toolkit::ImageView::Property::RESOURCE_URL:
      {
        if ( !impl.mUrl.empty() )
        {
          value = impl.mUrl;
        }
        break;
      }

      case Toolkit::ImageView::Property::IMAGE:
      {
        if ( !impl.mUrl.empty() )
        {
          value = impl.mUrl;
        }
        else if( impl.mImage )
        {
          Property::Map map;
          Scripting::CreatePropertyMap( impl.mImage, map );
          value = map;
        }
        else if( !impl.mPropertyMap.Empty() )
        {
          value = impl.mPropertyMap;
        }
        break;
      }

      case Toolkit::ImageView::Property::PRE_MULTIPLIED_ALPHA:
      {
        value = impl.IsPreMultipliedAlphaEnabled();
        break;
      }
    }
  }

  return value;
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
