// Copyright (c) 2014 Samsung Electronics Co., Ltd.

// CLASS HEADER
#include "image-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>

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
: Control( ControlBehaviour( ACTOR_BEHAVIOUR_NONE ) ),
  mPremultipledAlphaEnabled( false )
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
  if( ( mImage != image ) ||
      ! mUrl.empty()      ||   // If we're changing from a URL type to an Image type
      ! mPropertyMap.Empty() ) // If we're changing from a property map type to an Image type
  {
    mUrl.clear();
    mPropertyMap.Clear();

    mImage = image;

    Actor self = Self();
    Toolkit::RendererFactory::Get().ResetRenderer( mRenderer, self, image );
    mImageSize = image ? ImageDimensions( image.GetWidth(), image.GetHeight() ) : ImageDimensions( 0, 0 );

    RelayoutRequest();
  }
}

void ImageView::SetImage( Property::Map map )
{
  mUrl.clear();
  mImage.Reset();
  mPropertyMap = map;

  Actor self = Self();
  Toolkit::RendererFactory::Get().ResetRenderer( mRenderer, self, mPropertyMap );

  Property::Value* widthValue = mPropertyMap.Find( "width" );
  if( widthValue )
  {
    int width;
    if( widthValue->Get( width ) )
    {
      mImageSize = ImageDimensions( width, mImageSize.GetHeight() );
    }
  }

  Property::Value* heightValue = mPropertyMap.Find( "height" );
  if( heightValue )
  {
    int height;
    if( heightValue->Get( height ) )
    {
      mImageSize = ImageDimensions( mImageSize.GetWidth(), height );
    }
  }

  RelayoutRequest();
}

void ImageView::SetImage( const std::string& url, ImageDimensions size )
{
  if( ( mUrl != url ) ||
      mImage          ||       // If we're changing from an Image type to a URL type
      ! mPropertyMap.Empty() ) // If we're changing from a property map type to a URL type
  {
    mImage.Reset();
    mPropertyMap.Clear();

    mUrl = url;

    if( size.GetWidth() != 0u && size.GetHeight() != 0u )
    {
      mImageSize = size;
    }

    Actor self = Self();
    Toolkit::RendererFactory::Get().ResetRenderer( mRenderer, self, mUrl, size );

    RelayoutRequest();
  }
}

Image ImageView::GetImage() const
{
  return mImage;
}

void ImageView::EnablePreMultipliedAlpha( bool preMultipled )
{
  if( mRenderer )
  {
     GetImplementation( mRenderer ).EnablePreMultipliedAlpha( preMultipled );
  }
}

bool ImageView::IsPreMultipliedAlphaEnabled() const
{
  if( mRenderer )
  {
    return GetImplementation( mRenderer ).IsPreMultipliedAlphaEnabled();
  }
  return false;
}

void ImageView::SetDepthIndex( int depthIndex )
{
  if( mRenderer )
  {
    mRenderer.SetDepthIndex( depthIndex );
  }
}

Vector3 ImageView::GetNaturalSize()
{
  if( mRenderer )
  {
    Vector2 rendererNaturalSize;
    mRenderer.GetNaturalSize( rendererNaturalSize );
    return Vector3( rendererNaturalSize );
  }

  Vector3 size;
  size.x = mImageSize.GetWidth();
  size.y = mImageSize.GetHeight();

  if( size.x > 0 && size.y > 0 )
  {
    size.z = std::min(size.x, size.y);
    return size;
  }
  else
  {
    // if no image then use Control's natural size
    return Control::GetNaturalSize();
  }
}

float ImageView::GetHeightForWidth( float width )
{
  if( mImageSize.GetWidth() > 0 && mImageSize.GetHeight() > 0 )
  {
    return GetHeightForWidthBase( width );
  }
  else
  {
    return Control::GetHeightForWidth( width );
  }
}

float ImageView::GetWidthForHeight( float height )
{
  if( mImageSize.GetWidth() > 0 && mImageSize.GetHeight() > 0 )
  {
    return GetWidthForHeightBase( height );
  }
  else
  {
    return Control::GetWidthForHeight( height );
  }
}


///////////////////////////////////////////////////////////
//
// Private methods
//

void ImageView::OnStageConnection( int depth )
{
  Control::OnStageConnection( depth );

  if( mRenderer )
  {
    CustomActor self = Self();
    mRenderer.SetOnStage( self );
  }
}

void ImageView::OnStageDisconnection()
{
  if( mRenderer )
  {
    CustomActor self = Self();
    mRenderer.SetOffStage( self );
  }

  Control::OnStageDisconnection();
}

void ImageView::OnSizeSet( const Vector3& targetSize )
{
  Control::OnSizeSet( targetSize );

  if( mRenderer )
  {
    Vector2 size( targetSize );
    mRenderer.SetSize( size );
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
    switch ( index )
    {
      case Toolkit::ImageView::Property::RESOURCE_URL:
      {
        std::string imageUrl;
        if( value.Get( imageUrl ) )
        {
          GetImpl( imageView ).SetImage( imageUrl, ImageDimensions() );
        }
        break;
      }

      case Toolkit::ImageView::Property::IMAGE:
      {
        std::string imageUrl;
        if( value.Get( imageUrl ) )
        {
          ImageView& impl = GetImpl( imageView );
          impl.SetImage( imageUrl, ImageDimensions() );
        }

        // if its not a string then get a Property::Map from the property if possible.
        Property::Map map;
        if( value.Get( map ) )
        {
          ImageView& impl = GetImpl( imageView );
          impl.SetImage( map );
        }

        break;
      }

      case Toolkit::ImageView::Property::PRE_MULTIPLIED_ALPHA:
      {
        bool IsPre;
        if( value.Get( IsPre ) )
        {
          GetImpl(imageView).EnablePreMultipliedAlpha( IsPre );
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
