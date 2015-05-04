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

void PushButton::OnDisabledBackgroundImageSet()
{
  ConfigureSizeNegotiation();
  RelayoutRequest();
}

bool PushButton::OnSelected()
{
  Actor& buttonImage = GetButtonImage();
  Actor& selectedImage = GetSelectedImage();
  Actor& selectedBackgroundImage = GetSelectedBackgroundImage();

  PaintState paintState = GetPaintState();

  switch( paintState )
  {
    case UnselectedState:
    {
      FadeOutImage( buttonImage );
      FadeInImage( selectedBackgroundImage );
      FadeInImage( selectedImage );
      StartTransitionAnimation();
      break;
    }
    case SelectedState:
    {
      FadeOutImage( selectedBackgroundImage );
      FadeOutImage( selectedImage );
      FadeInImage( buttonImage );
      StartTransitionAnimation();
      break;
    }
    case UnselectedSelectedTransition:
    {
      float opacity = 1.f;
      if( selectedImage )
      {
        opacity = selectedImage.GetCurrentOpacity();
      }

      StopTransitionAnimation( false );
      FadeOutImage( selectedBackgroundImage, opacity );
      FadeOutImage( selectedImage, opacity );
      FadeInImage( buttonImage, 1.f - opacity );
      StartTransitionAnimation();
      break;
    }
    case SelectedUnselectedTransition:
    {
      float opacity = 0.f;
      if( selectedImage )
      {
        opacity = selectedImage.GetCurrentOpacity();
      }

      StopTransitionAnimation( false );
      FadeOutImage( buttonImage, 1.f - opacity );
      FadeInImage( selectedBackgroundImage, opacity );
      FadeInImage( selectedImage, opacity );
      StartTransitionAnimation();
      break;
    }
    case DisabledUnselectedTransition:
    {
      StopTransitionAnimation();
      FadeOutImage( buttonImage );
      FadeInImage( selectedBackgroundImage );
      FadeInImage( selectedImage );
      StartTransitionAnimation();
      break;
    }
    case DisabledSelectedTransition:
    {
      StopTransitionAnimation();
      FadeOutImage( selectedBackgroundImage );
      FadeOutImage( selectedImage );
      FadeInImage( buttonImage );
      StartTransitionAnimation();
      break;
    }
    default:
    {
      break;
    }
  }

  if( mTransitionAnimation )
  {
    return true;
  }

  return false;
}

bool PushButton::OnDisabled()
{
  Actor& buttonImage = GetButtonImage();
  Actor& selectedImage = GetSelectedImage();
  Actor& selectedBackgroundImage = GetSelectedBackgroundImage();
  Actor& backgroundImage = GetBackgroundImage();
  Actor& disabledImage = GetDisabledImage();
  Actor& disabledSelectedImage = GetDisabledSelectedImage();
  Actor& disabledBackgroundImage = GetDisabledBackgroundImage();

  PaintState paintState = GetPaintState();

  switch( paintState )
  {
    case UnselectedState:
    {
      FadeOutImage( backgroundImage );
      FadeOutImage( buttonImage );
      FadeInImage( disabledBackgroundImage );
      FadeInImage( disabledImage );
      StartTransitionAnimation();
      break;
    }
    case SelectedState:
    {
      FadeOutImage( backgroundImage );
      FadeOutImage( selectedBackgroundImage );
      FadeOutImage( selectedImage );
      FadeInImage( disabledBackgroundImage );
      FadeInImage( disabledSelectedImage );
      StartTransitionAnimation();
      break;
    }
    case DisabledUnselectedState:
    {
      FadeOutImage( disabledBackgroundImage );
      FadeOutImage( disabledImage );
      FadeInImage( backgroundImage );
      FadeInImage( buttonImage );
      StartTransitionAnimation();
      break;
    }
    case DisabledSelectedState:
    {
      FadeOutImage( disabledBackgroundImage );
      FadeOutImage( disabledSelectedImage );
      FadeInImage( backgroundImage );
      FadeInImage( selectedBackgroundImage );
      FadeInImage( selectedImage );
      StartTransitionAnimation();
      break;
    }
    case UnselectedSelectedTransition:
    {
      float opacity = 1.f;
      if( selectedImage )
      {
        opacity = selectedImage.GetCurrentOpacity();
      }

      StopTransitionAnimation();
      FadeOutImage( backgroundImage );
      FadeOutImage( selectedBackgroundImage, opacity );
      FadeOutImage( selectedImage, opacity );
      FadeInImage( disabledBackgroundImage );
      FadeInImage( disabledSelectedImage );
      StartTransitionAnimation();
      break;
    }
    case SelectedUnselectedTransition:
    {
      float opacity = 1.f;
      if( buttonImage )
      {
        opacity = buttonImage.GetCurrentOpacity();
      }

      StopTransitionAnimation();
      FadeOutImage( backgroundImage );
      FadeOutImage( buttonImage, opacity );
      FadeInImage( disabledBackgroundImage );
      FadeInImage( disabledImage );
      StartTransitionAnimation();
      break;
    }
    case UnselectedDisabledTransition:
    {
      float opacity = 1.f;
      if( disabledImage )
      {
        opacity = disabledImage.GetCurrentOpacity();
      }

      StopTransitionAnimation( false );
      FadeOutImage( disabledBackgroundImage, opacity );
      FadeOutImage( disabledImage, opacity );
      FadeInImage( backgroundImage, 1.f - opacity );
      FadeInImage( buttonImage, 1.f - opacity );
      StartTransitionAnimation();
      break;
    }
    case DisabledUnselectedTransition:
    {
      float opacity = 1.f;
      if( buttonImage )
      {
        opacity = buttonImage.GetCurrentOpacity();
      }

      StopTransitionAnimation( false );
      FadeOutImage( backgroundImage, opacity );
      FadeOutImage( buttonImage, opacity );
      FadeInImage( disabledBackgroundImage, 1.f - opacity );
      FadeInImage( disabledImage, 1.f - opacity );
      StartTransitionAnimation();
      break;
    }
    case SelectedDisabledTransition:
    {
      float opacity = 1.f;
      if( disabledSelectedImage )
      {
        opacity = disabledSelectedImage.GetCurrentOpacity();
      }

      StopTransitionAnimation( false );
      FadeOutImage( disabledBackgroundImage, opacity );
      FadeOutImage( disabledSelectedImage, opacity );
      FadeInImage( backgroundImage, 1.f - opacity );
      FadeInImage( selectedBackgroundImage, 1.f - opacity );
      FadeInImage( selectedImage, 1.f - opacity );
      StartTransitionAnimation();
      break;
    }
    case DisabledSelectedTransition:
    {
      float opacity = 1.f;
      if( selectedImage )
      {
        opacity = selectedImage.GetCurrentOpacity();
      }

      StopTransitionAnimation( false );
      FadeOutImage( backgroundImage, opacity );
      FadeOutImage( selectedBackgroundImage, opacity );
      FadeOutImage( selectedImage, opacity );
      FadeInImage( disabledBackgroundImage, 1.f - opacity );
      FadeInImage( disabledSelectedImage, 1.f - opacity );
      StartTransitionAnimation();
      break;
    }
  }

  if( mTransitionAnimation )
  {
    return true;
  }

  return false;
}

bool PushButton::OnPressed()
{
  Actor& buttonImage = GetButtonImage();
  Actor& selectedImage = GetSelectedImage();
  Actor& selectedBackgroundImage = GetSelectedBackgroundImage();

  PaintState paintState = GetPaintState();

  switch( paintState )
  {
    case UnselectedState:
    {
      FadeOutImage( buttonImage );
      FadeInImage( selectedBackgroundImage );
      FadeInImage( selectedImage );
      StartTransitionAnimation();
      break;
    }
    case SelectedUnselectedTransition:
    {
      float opacity = 1.f;
      if( buttonImage )
      {
        opacity = buttonImage.GetCurrentOpacity();
      }

      StopTransitionAnimation( false );
      FadeOutImage( buttonImage, opacity );
      FadeInImage( selectedBackgroundImage, 1.f - opacity );
      FadeInImage( selectedImage, 1.f - opacity );
      StartTransitionAnimation();
      break;
    }
    case DisabledUnselectedTransition:
    {
      float opacity = 1.f;
      if( buttonImage )
      {
        opacity = buttonImage.GetCurrentOpacity();
      }

      StopTransitionAnimation();
      FadeOutImage( buttonImage, opacity );
      FadeInImage( selectedBackgroundImage );
      FadeInImage( selectedImage );
      StartTransitionAnimation();
      break;
    }
    default:
      break;
  }

  if( mTransitionAnimation )
  {
    return true;
  }

  return false;
}

bool PushButton::OnReleased()
{
  Actor& buttonImage = GetButtonImage();
  Actor& selectedImage = GetSelectedImage();
  Actor& selectedBackgroundImage = GetSelectedBackgroundImage();

  PaintState paintState = GetPaintState();

  switch( paintState )
  {
    case SelectedState:
    {
      FadeOutImage( selectedBackgroundImage );
      FadeOutImage( selectedImage );
      FadeInImage( buttonImage );
      StartTransitionAnimation();
      break;
    }
    case UnselectedSelectedTransition:
    {
      float opacity = 1.f;
      if( selectedImage )
      {
        opacity = selectedImage.GetCurrentOpacity();
      }

      StopTransitionAnimation( false );
      FadeOutImage( selectedBackgroundImage, opacity );
      FadeOutImage( selectedImage, opacity );
      FadeInImage( buttonImage, 1.f - opacity );
      StartTransitionAnimation();
      break;
    }
    case DisabledSelectedTransition:
    {
      float opacity = 1.f;
      if( selectedImage )
      {
        opacity = selectedImage.GetCurrentOpacity();
      }

      StopTransitionAnimation();
      FadeOutImage( selectedBackgroundImage, opacity );
      FadeOutImage( selectedImage, opacity );
      FadeInImage( buttonImage );
      StartTransitionAnimation();
      break;
    }
    default:
    {
      break;
    }
  }

  if( mTransitionAnimation )
  {
    return true;
  }

  return false;
}

void PushButton::StopAllAnimations()
{
  StopTransitionAnimation();
}

void PushButton::OnControlSizeSet( const Vector3& targetSize )
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

void PushButton::StartTransitionAnimation()
{
  if( mTransitionAnimation )
  {
    mTransitionAnimation.FinishedSignal().Connect( this, &PushButton::TransitionAnimationFinished );
    mTransitionAnimation.Play();
  }
}

void PushButton::StopTransitionAnimation( bool remove )
{
  if( mTransitionAnimation )
  {
    mTransitionAnimation.Clear();
    mTransitionAnimation.Reset();
  }

  if( remove )
  {
    UpdatePaintTransitionState();
  }
}

void PushButton::FadeInImage( Actor& image, float opacity, Vector3 scale )
{
  if( image )
  {
    image.SetOpacity( opacity );
    image.SetScale( scale );

    if( !mTransitionAnimation )
    {
      mTransitionAnimation = Dali::Animation::New( GetAnimationTime() );
    }

    mTransitionAnimation.AnimateTo( Property( image, Actor::Property::COLOR_ALPHA ), 1.f );
  }
}

void PushButton::FadeOutImage( Actor& image, float opacity, Vector3 scale )
{
  if( image )
  {
    image.SetOpacity( opacity );
    image.SetScale( scale );

    if( !mTransitionAnimation )
    {
      mTransitionAnimation = Dali::Animation::New( GetAnimationTime() );
    }

    mTransitionAnimation.AnimateTo( Property( image, Actor::Property::COLOR_ALPHA ), 0.f );
  }
}

void PushButton::TransitionAnimationFinished( Dali::Animation& source )
{
  StopTransitionAnimation();
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
  ActorContainer images;
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

void PushButton::ConfigureSizeNegotiationDimension( Dimension::Type dimension, const ActorContainer& images, Actor& label )
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

  for( ActorConstIter it = images.begin(), itEnd = images.end(); it != itEnd; ++it )
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
