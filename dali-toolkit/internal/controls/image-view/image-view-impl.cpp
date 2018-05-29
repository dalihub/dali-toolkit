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
 *
 */

// CLASS HEADER
#include "image-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/devel-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
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

void ImageView::OnInitialize()
{
  // ImageView can relayout in the OnImageReady, alternative to a signal would be to have a upcall from the Control to ImageView
  Dali::Toolkit::Control handle( GetOwner() );
  handle.ResourceReadySignal().Connect( this, &ImageView::OnResourceReady );
}

void ImageView::SetImage( Image image )
{
  // Don't bother comparing if we had a visual previously, just drop old visual and create new one
  mImage = image;
  mUrl.clear();
  mPropertyMap.Clear();

  Toolkit::Visual::Base visual =  Toolkit::VisualFactory::Get().CreateVisual( image );
  if( visual )
  {
    if( !mVisual )
    {
      mVisual = visual;
    }

    DevelControl::RegisterVisual( *this, Toolkit::ImageView::Property::IMAGE, visual  );
  }
  else
  {
    // Unregister the exsiting visual
    DevelControl::UnregisterVisual( *this, Toolkit::ImageView::Property::IMAGE );

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    RelayoutRequest();
  }
}

void ImageView::SetImage( const Property::Map& map )
{
  // Comparing a property map is too expensive so just creating a new visual
  mPropertyMap = map;
  mUrl.clear();
  mImage.Reset();

  Toolkit::Visual::Base visual =  Toolkit::VisualFactory::Get().CreateVisual( mPropertyMap );
  if( visual )
  {
    // Don't set mVisual until it is ready and shown. Getters will still use current visual.
    if( !mVisual )
    {
      mVisual = visual;
    }

    DevelControl::RegisterVisual( *this, Toolkit::ImageView::Property::IMAGE, visual  );
  }
  else
  {
    // Unregister the exsiting visual
    DevelControl::UnregisterVisual( *this, Toolkit::ImageView::Property::IMAGE );

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    RelayoutRequest();
  }
}

void ImageView::SetImage( const std::string& url, ImageDimensions size )
{
  // Don't bother comparing if we had a visual previously, just drop old visual and create new one
  mUrl = url;
  mImage.Reset();
  mPropertyMap.Clear();

  // Don't set mVisual until it is ready and shown. Getters will still use current visual.
  Toolkit::Visual::Base visual =  Toolkit::VisualFactory::Get().CreateVisual( url, size );
  if( visual )
  {
    if( !mVisual )
    {
      mVisual = visual;
    }

    DevelControl::RegisterVisual( *this, Toolkit::ImageView::Property::IMAGE, visual );
  }
  else
  {
    // Unregister the exsiting visual
    DevelControl::UnregisterVisual( *this, Toolkit::ImageView::Property::IMAGE );

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    RelayoutRequest();
  }
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

    Extents padding;
    padding = Self().GetProperty<Extents>( Toolkit::Control::Property::PADDING );

    rendererNaturalSize.width += ( padding.start + padding.end );
    rendererNaturalSize.height += ( padding.top + padding.bottom );
    return Vector3( rendererNaturalSize );
  }

  // if no visual then use Control's natural size
  return Control::GetNaturalSize();
}

float ImageView::GetHeightForWidth( float width )
{
  Extents padding;
  padding = Self().GetProperty<Extents>( Toolkit::Control::Property::PADDING );

  if( mVisual )
  {
    return mVisual.GetHeightForWidth( width ) + padding.top + padding.bottom;
  }
  else
  {
    return Control::GetHeightForWidth( width ) + padding.top + padding.bottom;
  }
}

float ImageView::GetWidthForHeight( float height )
{
  Extents padding;
  padding = Self().GetProperty<Extents>( Toolkit::Control::Property::PADDING );

  if( mVisual )
  {
    return mVisual.GetWidthForHeight( height ) + padding.start + padding.end;
  }
  else
  {
    return Control::GetWidthForHeight( height ) + padding.start + padding.end;
  }
}

void ImageView::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  Control::OnRelayout( size, container );

  if( mVisual )
  {
    Property::Map transformMap = Property::Map();

    // Don't transform if fitting mode is FILL
    if(Toolkit::GetImplementation(mVisual).GetFittingMode() == Visual::FittingMode::FIT_KEEP_ASPECT_RATIO)
    {
      Extents padding;
      padding = Self().GetProperty<Extents>( Toolkit::Control::Property::PADDING );

      Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(
              Self().GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());

      if (Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
      {
        std::swap(padding.start, padding.end);
      }

      // remove padding from the size to know how much is left for the visual
      auto paddedSize = size - Vector2(padding.start + padding.end, padding.top + padding.bottom);

      Vector2 naturalSize;
      mVisual.GetNaturalSize(naturalSize);

      // scale to fit the padded area
      auto finalSize =
             naturalSize * std::min( ( naturalSize.width  ? ( paddedSize.width  / naturalSize.width  ) : 0 ),
                                     ( naturalSize.height ? ( paddedSize.height / naturalSize.height ) : 0 ) );

      // calculate final offset within the padded area
      auto finalOffset = Vector2(padding.start, padding.top) + (paddedSize - finalSize) * .5f;

      // populate the transform map
      transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET, finalOffset)
          .Add(Toolkit::Visual::Transform::Property::OFFSET_POLICY,
              Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE))
          .Add(Toolkit::Visual::Transform::Property::ORIGIN, Toolkit::Align::TOP_BEGIN)
          .Add(Toolkit::Visual::Transform::Property::ANCHOR_POINT, Toolkit::Align::TOP_BEGIN)
          .Add(Toolkit::Visual::Transform::Property::SIZE, finalSize)
          .Add(Toolkit::Visual::Transform::Property::SIZE_POLICY,
              Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE));

    }
    // Should provide a transform that handles aspect ratio according to image size
    mVisual.SetTransformAndSize( transformMap, size );
  }
}

void ImageView::OnResourceReady( Toolkit::Control control )
{
  // Visual ready so update visual attached to this ImageView, following call to RelayoutRequest will use this visual.
  mVisual = DevelControl::GetVisual( *this, Toolkit::ImageView::Property::IMAGE );
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
            Property::Value* shaderValue = map->Find( Toolkit::Visual::Property::SHADER, CUSTOM_SHADER );
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
