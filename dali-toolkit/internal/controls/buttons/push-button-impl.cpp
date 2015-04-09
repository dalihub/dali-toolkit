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
    size.width = std::max( root.GetRelayoutSize( WIDTH ), size.width );
    size.height = std::max( root.GetRelayoutSize( HEIGHT ), size.height );
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
  self.SetResizePolicy( USE_NATURAL_SIZE, ALL_DIMENSIONS );
}

void PushButton::OnLabelSet()
{
  Actor& label = GetLabel();

  if( label )
  {
    label.SetAnchorPoint( AnchorPoint::CENTER );
    label.SetParentOrigin( ParentOrigin::CENTER );
    label.SetSize( mSize );
  }
}

void PushButton::OnButtonImageSet()
{
  Actor& buttonImage = GetButtonImage();

  buttonImage.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );

  buttonImage.RelayoutRequestTree();

  RelayoutRequest();
}

void PushButton::OnSelectedImageSet()
{
  Actor& selectedImage = GetSelectedImage();

  selectedImage.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );

  selectedImage.RelayoutRequestTree();

  RelayoutRequest();
}

void PushButton::OnBackgroundImageSet()
{
  Actor& backgroundImage = GetBackgroundImage();

  backgroundImage.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );

  backgroundImage.RelayoutRequestTree();

  RelayoutRequest();
}

void PushButton::OnSelectedBackgroundImageSet()
{
  Actor& selectedBackgroundImage = GetSelectedBackgroundImage();

  selectedBackgroundImage.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
}

void PushButton::OnDisabledImageSet()
{
  Actor& disabledImage = GetDisabledImage();

  disabledImage.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );

  disabledImage.RelayoutRequestTree();

  RelayoutRequest();
}

void PushButton::OnDisabledBackgroundImageSet()
{
  Actor& disabledBackgroundImage = GetDisabledBackgroundImage();

  disabledBackgroundImage.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );

  disabledBackgroundImage.RelayoutRequestTree();

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

Vector3 PushButton::GetNaturalSize()
{
  Vector3 size;

  // Check Image and Background image and use the largest size as the control's Natural size.
  SizeOfActorIfLarger( GetButtonImage(), size );
  SizeOfActorIfLarger( GetBackgroundImage(), size );

  // If label, test against it's size
  Toolkit::TextLabel label = Toolkit::TextLabel::DownCast( GetLabel() );
  if( label )
  {
    Vector3 labelSize = label.GetNaturalSize();

    size.width  = std::max( size.width,  labelSize.width  + TEXT_PADDING * 2.0f );
    size.height = std::max( size.height, labelSize.height + TEXT_PADDING * 2.0f );
  }

  return size;
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

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
