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
#include <dali/public-api/images/resource-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

const float TEXT_PADDING = 12.0f;
const float ANIMATION_TIME( 0.2f );

BaseHandle Create()
{
  return Toolkit::PushButton::New();
}

TypeRegistration typeRegistration( typeid(Toolkit::PushButton), typeid(Toolkit::Button), Create );

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

  Image buttonImage = Dali::ResourceImage::New( UNSELECTED_BUTTON_IMAGE_DIR, ResourceImage::ON_DEMAND, ResourceImage::NEVER );
  Image selectedImage = Dali::ResourceImage::New( SELECTED_BUTTON_IMAGE_DIR, ResourceImage::ON_DEMAND, ResourceImage::NEVER );
  Image disabledImage = Dali::ResourceImage::New( DISABLED_UNSELECTED_BUTTON_IMAGE_DIR, ResourceImage::ON_DEMAND, ResourceImage::NEVER );
  Image disabledSelectedImage = Dali::ResourceImage::New( DISABLED_SELECTED_BUTTON_IMAGE_DIR, ResourceImage::ON_DEMAND, ResourceImage::NEVER );

  SetButtonImage( ImageActor::New( buttonImage ) );
  SetSelectedImage( ImageActor::New( selectedImage ) );
  SetDisabledImage( ImageActor::New( disabledImage ) );
  SetDisabledSelectedImage( ImageActor::New( disabledSelectedImage ) );
}

void PushButton::OnLabelSet()
{
  Actor& label = GetLabel();

  if( label )
  {
    label.SetAnchorPoint( AnchorPoint::CENTER );
    label.SetParentOrigin( ParentOrigin::CENTER );

    Toolkit::TextLabel textLabel = Toolkit::TextLabel::DownCast( label );
    if( textLabel )
    {
      textLabel.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
      textLabel.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
      textLabel.SetProperty( Toolkit::TextLabel::Property::MULTI_LINE, true );
    }

    ConfigureSizeNegotiation();
  }
}

void PushButton::OnButtonImageSet()
{
  ConfigureSizeNegotiation();
  RelayoutRequest();
}

void PushButton::OnSelectedImageSet()
{
  ConfigureSizeNegotiation();
  RelayoutRequest();
}

void PushButton::OnBackgroundImageSet()
{
  ConfigureSizeNegotiation();
  RelayoutRequest();
}

void PushButton::OnSelectedBackgroundImageSet()
{
  ConfigureSizeNegotiation();
  RelayoutRequest();
}

void PushButton::OnDisabledImageSet()
{
  ConfigureSizeNegotiation();
  RelayoutRequest();
}

void PushButton::OnDisabledSelectedImageSet()
{
  ConfigureSizeNegotiation();
  RelayoutRequest();
}

void PushButton::OnDisabledBackgroundImageSet()
{
  ConfigureSizeNegotiation();
  RelayoutRequest();
}

void PushButton::OnSizeSet( const Vector3& targetSize )
{
  if( targetSize != mSize )
  {
    mSize = targetSize;

    Actor& label = GetLabel();

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
  Toolkit::TextLabel label = Toolkit::TextLabel::DownCast( GetLabel() );
  if( label )
  {
    size.width  = std::max( size.width,  label.GetRelayoutSize( Dimension::WIDTH ) );
    size.height = std::max( size.height, label.GetRelayoutSize( Dimension::HEIGHT ) );
  }
  else
  {
    // Check Image and Background image and use the largest size as the control's Natural size.
    SizeOfActorIfLarger( GetButtonImage(), size );
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

  images.push_back( GetButtonImage() );
  images.push_back( GetSelectedImage() );
  images.push_back( GetSelectedBackgroundImage() );
  images.push_back( GetBackgroundImage() );
  images.push_back( GetDisabledImage() );
  images.push_back( GetDisabledSelectedImage() );
  images.push_back( GetDisabledBackgroundImage() );

  Actor label = GetLabel();

  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i )
  {
    ConfigureSizeNegotiationDimension( static_cast< Dimension::Type >( 1 << i ), images, label );
  }

  if( label )
  {
    Padding padding;

    if( label.GetResizePolicy( Dimension::WIDTH ) == ResizePolicy::USE_NATURAL_SIZE )
    {
      padding.left = TEXT_PADDING;
      padding.right = TEXT_PADDING;
    }

    if( label.GetResizePolicy( Dimension::HEIGHT ) == ResizePolicy::USE_NATURAL_SIZE )
    {
      padding.top = TEXT_PADDING;
      padding.bottom = TEXT_PADDING;
    }

    label.SetPadding( padding );
  }
}

void PushButton::ConfigureSizeNegotiationDimension( Dimension::Type dimension, const std::vector< Actor >& images, Actor& label )
{
  ResizePolicy::Type imageResizePolicy = ResizePolicy::FILL_TO_PARENT;
  ResizePolicy::Type labelResizePolicy = ResizePolicy::FILL_TO_PARENT;

  switch( Self().GetResizePolicy( dimension ) )
  {
    case ResizePolicy::FIT_TO_CHILDREN:
    {
      imageResizePolicy = labelResizePolicy = ResizePolicy::USE_NATURAL_SIZE;
      break;
    }
    case ResizePolicy::USE_NATURAL_SIZE:
    {
      if( label )
      {
        labelResizePolicy = ResizePolicy::USE_NATURAL_SIZE;
      }
      else
      {
        imageResizePolicy = ResizePolicy::USE_NATURAL_SIZE;
      }
      break;
    }
    default:
    {
      break;
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
