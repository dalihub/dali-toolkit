/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include "push-button-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/devel-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

const float   ANIMATION_TIME( 0.2f );
const Padding DEFAULT_LABEL_PADDING( 12.0f, 12.0f, 12.0f, 12.0f );
const Padding DEFAULT_ICON_PADDING( 12.0f, 12.0f, 12.0f, 12.0f );

BaseHandle Create()
{
  return Toolkit::PushButton::New();
}

// Properties

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::PushButton, Toolkit::Button, Create )

DALI_PROPERTY_REGISTRATION( Toolkit, PushButton, "unselectedIcon",  STRING, UNSELECTED_ICON )
DALI_PROPERTY_REGISTRATION( Toolkit, PushButton, "selectedIcon",  STRING, SELECTED_ICON )
DALI_PROPERTY_REGISTRATION( Toolkit, PushButton, "iconAlignment",  STRING, ICON_ALIGNMENT )
DALI_PROPERTY_REGISTRATION( Toolkit, PushButton, "labelPadding",  STRING, LABEL_PADDING )
DALI_PROPERTY_REGISTRATION( Toolkit, PushButton, "iconPadding",  STRING, ICON_PADDING )

DALI_TYPE_REGISTRATION_END()

/*
 * Table to define Text-to-enum conversions for IconAlignment.
 */
const Dali::Scripting::StringEnum IconAlignmentTable[] = {
  { "LEFT",   Toolkit::Internal::PushButton::LEFT },
  { "RIGHT",  Toolkit::Internal::PushButton::RIGHT },
  { "TOP",    Toolkit::Internal::PushButton::TOP },
  { "BOTTOM", Toolkit::Internal::PushButton::BOTTOM },
}; const unsigned int IconAlignmentTableCount = sizeof( IconAlignmentTable ) / sizeof( IconAlignmentTable[0] );

const char* const UNSELECTED_BUTTON_IMAGE_DIR = DALI_IMAGE_DIR "button-up.9.png";
const char* const SELECTED_BUTTON_IMAGE_DIR = DALI_IMAGE_DIR "button-down.9.png";
const char* const DISABLED_UNSELECTED_BUTTON_IMAGE_DIR = DALI_IMAGE_DIR "button-disabled.9.png";
const char* const DISABLED_SELECTED_BUTTON_IMAGE_DIR = DALI_IMAGE_DIR "button-down-disabled.9.png";

} // unnamed namespace

namespace
{

/**
 * Get size of Actor if larger than given size
 * @param[in] root the actor to get the size of
 * @param[out] size the greater of the given size or the size of the Actor
 */
void SizeOfActorIfLarger( Actor root, Vector3& size )
{
  if ( root )
  {
    // RelayoutSize retreived for Actor to use any padding set to it.
    size.width = std::max( root.GetRelayoutSize( Dimension::WIDTH ), size.width );
    size.height = std::max( root.GetRelayoutSize( Dimension::HEIGHT ), size.height );
  }
}

} // unnamed namespace

Dali::Toolkit::PushButton PushButton::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr< PushButton > internalPushButton = new PushButton();

  // Pass ownership to CustomActor
  Dali::Toolkit::PushButton pushButton( *internalPushButton );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalPushButton->Initialize();

  return pushButton;
}

PushButton::PushButton()
: Button(),
  mLabelPadding( DEFAULT_LABEL_PADDING ),
  mIconPadding( DEFAULT_ICON_PADDING ),
  mIconAlignment( RIGHT ),
  mSize()
{
  SetAnimationTime( ANIMATION_TIME );
}

PushButton::~PushButton()
{
}

void PushButton::OnButtonInitialize()
{
  // Push button requires the Leave event.
  Actor self = Self();
  self.SetLeaveRequired( true );

  // Set resize policy to natural size so that buttons will resize to background images
  self.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

  SetUnselectedImage( UNSELECTED_BUTTON_IMAGE_DIR );
  SetSelectedImage( SELECTED_BUTTON_IMAGE_DIR );
  SetDisabledImage( DISABLED_UNSELECTED_BUTTON_IMAGE_DIR );
  SetDisabledSelectedImage( DISABLED_SELECTED_BUTTON_IMAGE_DIR );
}

void PushButton::SetIcon( DecorationState state, const std::string iconFilename )
{
  mIconName[ state ] = iconFilename;
  SetDecoration( state, Toolkit::ImageView::New( iconFilename  ) );
  ConfigureSizeNegotiation();
}

std::string& PushButton::GetIcon( DecorationState state )
{
  return mIconName[ state ];
}

void PushButton::SetIconAlignment( const PushButton::IconAlignment iconAlignment )
{
  mIconAlignment = iconAlignment;
  ConfigureSizeNegotiation();
}

const PushButton::IconAlignment PushButton::GetIconAlignment() const
{
  return mIconAlignment;
}

void PushButton::SetLabelPadding( const Vector4& padding )
{
  mLabelPadding = Padding( padding.x, padding.y, padding.z, padding.w );
  ConfigureSizeNegotiation();
}

Vector4 PushButton::GetLabelPadding()
{
  return Vector4( mLabelPadding.left, mLabelPadding.right, mLabelPadding.top, mLabelPadding.bottom );
}

void PushButton::SetIconPadding( const Vector4& padding )
{
  mIconPadding = Padding( padding.x, padding.y, padding.z, padding.w );
  ConfigureSizeNegotiation();
}

Vector4 PushButton::GetIconPadding()
{
  return Vector4( mIconPadding.left, mIconPadding.right, mIconPadding.top, mIconPadding.bottom );
}

void PushButton::SetProperty( BaseObject* object, Property::Index propertyIndex, const Property::Value& value )
{
  Toolkit::PushButton pushButton = Toolkit::PushButton::DownCast( Dali::BaseHandle( object ) );

  if ( pushButton )
  {
    PushButton& pushButtonImpl( GetImplementation( pushButton ) );

    switch ( propertyIndex )
    {
      case Toolkit::PushButton::Property::UNSELECTED_ICON:
      {
        pushButtonImpl.SetIcon( UNSELECTED_DECORATION, value.Get< std::string >() );
        break;
      }
      case Toolkit::PushButton::Property::SELECTED_ICON:
      {
        pushButtonImpl.SetIcon( SELECTED_DECORATION, value.Get< std::string >() );
        break;
      }
      case Toolkit::PushButton::Property::ICON_ALIGNMENT:
      {
        IconAlignment iconAlignment;
        if( Scripting::GetEnumeration< IconAlignment >( value.Get< std::string >().c_str(), IconAlignmentTable, IconAlignmentTableCount, iconAlignment ) )
        {
          pushButtonImpl.SetIconAlignment( iconAlignment );
        }
        break;
      }
      case Toolkit::PushButton::Property::LABEL_PADDING:
      {
        pushButtonImpl.SetLabelPadding( value.Get< Vector4 >() );
        break;
      }
      case Toolkit::PushButton::Property::ICON_PADDING:
      {
        pushButtonImpl.SetIconPadding( value.Get< Vector4 >() );
        break;
      }
    }
  }
}

Property::Value PushButton::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Toolkit::PushButton pushButton = Toolkit::PushButton::DownCast( Dali::BaseHandle( object ) );

  if ( pushButton )
  {
    PushButton& pushButtonImpl( GetImplementation( pushButton ) );

    switch ( propertyIndex )
    {
      case Toolkit::PushButton::Property::UNSELECTED_ICON:
      {
        value = pushButtonImpl.GetIcon( UNSELECTED_DECORATION );
        break;
      }
      case Toolkit::PushButton::Property::SELECTED_ICON:
      {
        value = pushButtonImpl.GetIcon( UNSELECTED_DECORATION );
        break;
      }
      case Toolkit::PushButton::Property::ICON_ALIGNMENT:
      {
        value = Scripting::GetLinearEnumerationName< IconAlignment >( pushButtonImpl.GetIconAlignment(), IconAlignmentTable, IconAlignmentTableCount );
        break;
      }
      case Toolkit::PushButton::Property::LABEL_PADDING:
      {
        value = pushButtonImpl.GetLabelPadding();
        break;
      }
      case Toolkit::PushButton::Property::ICON_PADDING:
      {
        value = pushButtonImpl.GetIconPadding();
        break;
      }
    }
  }

  return value;
}

void PushButton::OnLabelSet( bool noPadding )
{
  Actor& label = GetLabelActor();

  if( label )
  {
    if( noPadding )
    {
      mLabelPadding = Padding( 0.0f, 0.0f, 0.0f, 0.0f );
    }

    Toolkit::TextLabel textLabel = Toolkit::TextLabel::DownCast( label );
    if( textLabel )
    {
      textLabel.SetProperty( Toolkit::TextLabel::Property::MULTI_LINE, false );
    }
  }
  ConfigureSizeNegotiation();
}

void PushButton::OnButtonImageSet()
{
  ConfigureSizeNegotiation();
}

void PushButton::OnSelectedImageSet()
{
  ConfigureSizeNegotiation();
}

void PushButton::OnBackgroundImageSet()
{
  ConfigureSizeNegotiation();
}

void PushButton::OnSelectedBackgroundImageSet()
{
  ConfigureSizeNegotiation();
}

void PushButton::OnDisabledImageSet()
{
  ConfigureSizeNegotiation();
}

void PushButton::OnDisabledSelectedImageSet()
{
  ConfigureSizeNegotiation();
}

void PushButton::OnDisabledBackgroundImageSet()
{
  ConfigureSizeNegotiation();
}

void PushButton::OnSizeSet( const Vector3& targetSize )
{
  if( targetSize != mSize )
  {
    mSize = targetSize;

    Actor& label = GetLabelActor();

    if( label )
    {
      label.SetSize( mSize );
    }
  }
}

void PushButton::PrepareForTranstionIn( Actor actor )
{
  actor.SetOpacity( 0.0f );
}

void PushButton::PrepareForTranstionOut( Actor actor )
{
  actor.SetOpacity( 1.0f );
}

void PushButton::OnTransitionIn( Actor actor )
{
  FadeImageTo( actor, 1.f );
}

void PushButton::OnTransitionOut( Actor actor )
{
  FadeImageTo( actor, 0.0f );
}

void PushButton::FadeImageTo( Actor actor, float opacity )
{
  if( actor )
  {
    Dali::Animation transitionAnimation = GetTransitionAnimation();
    DALI_ASSERT_DEBUG( transitionAnimation );

    if( transitionAnimation )
    {
      transitionAnimation.AnimateTo( Property( actor, Actor::Property::COLOR_ALPHA ), opacity );
    }
  }
}

Vector3 PushButton::GetNaturalSize()
{
  Vector3 size;

  // If label, test against it's size
  Toolkit::TextLabel label = Toolkit::TextLabel::DownCast( GetLabelActor() );

  Actor icon = GetDecoration( UNSELECTED_DECORATION );
  if( label || icon )
  {
    Vector3 labelSize( Vector3::ZERO );
    Vector3 iconSize( Vector3::ZERO );

    if( label )
    {
      Vector3 labelNaturalSize = label.GetNaturalSize();
      labelSize.width = labelNaturalSize.width + mLabelPadding.left + mLabelPadding.right;
      labelSize.height = labelNaturalSize.height + mLabelPadding.top + mLabelPadding.bottom;
    }

    if( icon )
    {
      Vector3 iconNaturalSize = icon.GetNaturalSize();
      iconSize.width = iconNaturalSize.width + mIconPadding.left + mIconPadding.right;
      iconSize.height = iconNaturalSize.height + mIconPadding.top + mIconPadding.bottom;

      switch( mIconAlignment )
      {
        case LEFT:
        case RIGHT:
        {
          size.width = labelSize.width + iconSize.width;
          size.height = std::max( labelSize.height, iconSize.height );
          break;
        }
        case TOP:
        case BOTTOM:
        {
          size.width = std::max( labelSize.width, iconSize.width );
          size.height = labelSize.height + iconSize.height;
          break;
        }
      }
    }
    else
    {
      // No icon, so size is the same as label size.
      // (If there is no label this is zero).
      size = labelSize;
    }
  }
  else
  {
    // Check Image and Background image and use the largest size as the control's Natural size.
    SizeOfActorIfLarger( GetUnselectedImage(), size );
    SizeOfActorIfLarger( GetBackgroundImage(), size );
  }

  return size;
}

void PushButton::OnSetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension )
{
  ConfigureSizeNegotiation();
}

void PushButton::ConfigureSizeNegotiation()
{
  std::vector< Actor > images;
  images.reserve( 7 );

  images.push_back( GetUnselectedImage() );
  images.push_back( GetSelectedImage() );
  images.push_back( GetSelectedBackgroundImage() );
  images.push_back( GetBackgroundImage() );
  images.push_back( GetDisabledImage() );
  images.push_back( GetDisabledSelectedImage() );
  images.push_back( GetDisabledBackgroundImage() );

  Actor label = GetLabelActor();

  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    ConfigureSizeNegotiationDimension( static_cast< Dimension::Type >( 1 << i ), images, label );
  }

  // Add any vertical padding directly to the actors.
  Actor icon = GetDecoration( UNSELECTED_DECORATION );
  Actor selectedIcon = GetDecoration( SELECTED_DECORATION );
  bool iconExists = icon || selectedIcon;

  if( label )
  {
    label.SetPadding( mLabelPadding );
  }
  if( icon )
  {
    icon.SetPadding( mIconPadding );
  }
  if( selectedIcon )
  {
    selectedIcon.SetPadding( mIconPadding );
  }

  // Calculate and apply horizontal alignments and offsets
  // to text and icon (depending on existence).
  Vector3 iconPosition( Vector3::ZERO );
  Vector3 labelPosition( Vector3::ZERO );
  Vector3 iconAnchoring( AnchorPoint::CENTER );
  Vector3 labelAnchoring( AnchorPoint::CENTER );
  std::string horizontalLabelAlignment = "CENTER";
  std::string verticalLabelAlignment = "CENTER";

  if( iconExists && label )
  {
    // There is an icon and a label to lay out.
    switch( mIconAlignment )
    {
      case LEFT:
      {
        iconPosition.x = mIconPadding.left;
        labelPosition.x = -mLabelPadding.right;
        iconAnchoring = AnchorPoint::CENTER_LEFT;
        labelAnchoring = AnchorPoint::CENTER_RIGHT;
        horizontalLabelAlignment = "END";
        break;
      }
      case RIGHT:
      {
        iconPosition.x = -mIconPadding.right;
        labelPosition.x = mLabelPadding.left;
        iconAnchoring = AnchorPoint::CENTER_RIGHT;
        labelAnchoring = AnchorPoint::CENTER_LEFT;
        horizontalLabelAlignment = "BEGIN";
        break;
      }
      case TOP:
      {
        iconPosition.y = mIconPadding.top;
        labelPosition.y = -mLabelPadding.bottom;
        iconAnchoring = AnchorPoint::TOP_CENTER;
        labelAnchoring = AnchorPoint::BOTTOM_CENTER;
        verticalLabelAlignment = "BOTTOM";
        break;
      }
      case BOTTOM:
      {
        iconPosition.y = -mIconPadding.bottom;
        labelPosition.y = mLabelPadding.top;
        iconAnchoring = AnchorPoint::BOTTOM_CENTER;
        labelAnchoring = AnchorPoint::TOP_CENTER;
        verticalLabelAlignment = "TOP";
        break;
      }
    }
  }

  // Note: If there is only an icon, or only a label, the default values are now correct.
  // Setup the icon(s) with the precalculated values.
  if( icon )
  {
    icon.SetPosition( iconPosition );
    icon.SetParentOrigin( iconAnchoring );
    icon.SetAnchorPoint( iconAnchoring );
  }
  if( selectedIcon )
  {
    selectedIcon.SetPosition( iconPosition );
    selectedIcon.SetParentOrigin( iconAnchoring );
    selectedIcon.SetAnchorPoint( iconAnchoring );
  }

  // Setup the label.
  if( label )
  {
    label.SetPosition( labelPosition );
    label.SetParentOrigin( labelAnchoring );
    label.SetAnchorPoint( labelAnchoring );
    label.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, horizontalLabelAlignment );
    label.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, verticalLabelAlignment );
  }

  RelayoutRequest();
}


void PushButton::ConfigureSizeNegotiationDimension( Dimension::Type dimension, const std::vector< Actor >& images, Actor& label )
{
  ResizePolicy::Type imageResizePolicy = ResizePolicy::FILL_TO_PARENT;
  ResizePolicy::Type labelResizePolicy = ResizePolicy::FILL_TO_PARENT;

  ResizePolicy::Type resizePolicy = Self().GetResizePolicy( dimension );

  if( resizePolicy == ResizePolicy::FIT_TO_CHILDREN || resizePolicy == ResizePolicy::USE_NATURAL_SIZE )
  {
    if( label )
    {
      labelResizePolicy = ResizePolicy::USE_NATURAL_SIZE;
    }
    else
    {
      imageResizePolicy = ResizePolicy::USE_NATURAL_SIZE;
    }
  }

  if( label )
  {
    label.SetResizePolicy( labelResizePolicy, dimension );
  }

  for( std::vector< Actor >::const_iterator it = images.begin(), itEnd = images.end(); it != itEnd; ++it )
  {
    Actor actor = *it;
    if( actor )
    {
      actor.SetResizePolicy( imageResizePolicy, dimension );
    }
  }
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali
