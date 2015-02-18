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
#include "push-button-default-painter-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/object/ref-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
#include "push-button-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
const float LABEL_DEPTH( 0.75f );
const float FOREGROUND_DEPTH( 0.5f );
const float BACKGROUND_DEPTH( 0.25f );

const float ANIMATION_TIME( 0.2f );

inline Toolkit::Internal::PushButton& GetPushButtonImpl( Toolkit::Button& button )
{
  DALI_ASSERT_ALWAYS( button );

  Dali::RefObject& handle = button.GetImplementation();

  return static_cast<Toolkit::Internal::PushButton&>( handle );
}

inline const Toolkit::Internal::PushButton& GetPushButtonImpl( const Toolkit::Button& button )
{
  DALI_ASSERT_ALWAYS( button );

  const Dali::RefObject& handle = button.GetImplementation();

  return static_cast<const Toolkit::Internal::PushButton&>( handle );
}

}

PushButtonDefaultPainter::PushButtonDefaultPainter()
: PushButtonPainter(),
  mAutoRepeating( false ),
  mDisabled( false ),
  mPaintState( ReleasedState ),
  mButton( NULL ),
  mAnimationTime( ANIMATION_TIME ),
  mSize()
{
}

PushButtonDefaultPainter::~PushButtonDefaultPainter()
{
  if( mFadeInAnimation )
  {
    mFadeInAnimation.Clear();
  }
  if( mFadeOutAnimation )
  {
    mFadeOutAnimation.Clear();
  }
}

void PushButtonDefaultPainter::Initialize( Toolkit::Button& button )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
  Actor& buttonImage = pushButtonImpl.GetButtonImage();
  Actor& selectedImage = pushButtonImpl.GetSelectedImage();
  Actor& backgroundImage = pushButtonImpl.GetBackgroundImage();
  Actor& disabledImage = pushButtonImpl.GetDisabledImage();
  Actor& disabledBackgroundImage = pushButtonImpl.GetDisabledBackgroundImage();
  Actor& label = pushButtonImpl.GetLabel();

  Toolkit::PushButton& pushButton = static_cast<Toolkit::PushButton&>( button );

  if( buttonImage )
  {
    SetButtonImage( pushButton, buttonImage );
  }

  if( backgroundImage )
  {
    SetBackgroundImage( pushButton, backgroundImage );
  }

  if( selectedImage )
  {
    SetSelectedImage( pushButton, selectedImage );
  }

  if( disabledImage )
  {
    SetDisabledImage( pushButton, disabledImage );
  }

  if( disabledBackgroundImage )
  {
    SetDisabledBackgroundImage( pushButton, disabledBackgroundImage );
  }

  if( label )
  {
    SetLabel( pushButton, label );
  }

  SetDisabled( pushButton, mDisabled );
}

void PushButtonDefaultPainter::SetSize( Toolkit::Button& button, const Vector3& size )
{
  if( size != mSize )
  {
    mSize = size;

    Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
    Actor& buttonImage = pushButtonImpl.GetButtonImage();
    Actor& selectedImage = pushButtonImpl.GetSelectedImage();
    Actor& backgroundImage = pushButtonImpl.GetBackgroundImage();
    Actor& disabledImage = pushButtonImpl.GetDisabledImage();
    Actor& disabledBackgroundImage = pushButtonImpl.GetDisabledBackgroundImage();
    Actor& label = pushButtonImpl.GetLabel();

    ApplyConstraint( buttonImage, FOREGROUND_DEPTH );
    ApplyConstraint( backgroundImage, BACKGROUND_DEPTH );
    ApplyConstraint( selectedImage, FOREGROUND_DEPTH );
    ApplyConstraint( disabledImage, FOREGROUND_DEPTH );
    ApplyConstraint( disabledBackgroundImage, BACKGROUND_DEPTH );

    if( label )
    {
      label.SetPosition( 0.f, 0.f, LABEL_DEPTH );
      label.SetSize( mSize );
    }
  }
}

void PushButtonDefaultPainter::SetDisabled( Toolkit::Button& button, bool disabled )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
  Actor& buttonImage = pushButtonImpl.GetButtonImage();
  Actor& selectedImage = pushButtonImpl.GetSelectedImage();
  Actor& backgroundImage = pushButtonImpl.GetBackgroundImage();
  Actor& disabledImage = pushButtonImpl.GetDisabledImage();
  Actor& disabledBackgroundImage = pushButtonImpl.GetDisabledBackgroundImage();
  Actor& fadeOutButtonImage = pushButtonImpl.GetFadeOutButtonImage();

  Toolkit::PushButton& pushButton = static_cast<Toolkit::PushButton&>( button );

  mDisabled = disabled;

  switch( mPaintState )
  {
  case ReleasedState:
  {
    if( disabled )
    {
      StopFadeOutAnimation( pushButton );
      FadeOutImage( pushButton, Background, backgroundImage );
      FadeOutImage( pushButton, Foreground, buttonImage );
      FadeInImage( pushButton, disabledBackgroundImage );
      FadeInImage( pushButton, disabledImage );
      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = ReleasedDisabledTransition;
      }
      else
      {
        mPaintState = DisabledReleasedState;
      }
    }
    break;
  }
  case PressedState:
  {
    if( disabled )
    {
      StopFadeOutAnimation( pushButton );
      FadeOutImage( pushButton, Background, backgroundImage );
      FadeOutImage( pushButton, Foreground, selectedImage );
      FadeInImage( pushButton, disabledBackgroundImage );
      FadeInImage( pushButton, disabledImage );
      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( selectedImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = PressedDisabledTransition;
      }
      else
      {
        mPaintState = DisabledPressedState;
      }
    }
    break;
  }
  case DisabledReleasedState:
  {
    if( !disabled )
    {
      StopFadeOutAnimation( pushButton );
      FadeOutImage( pushButton, Background, disabledBackgroundImage );
      FadeOutImage( pushButton, Foreground, disabledImage );
      FadeInImage( pushButton, backgroundImage );
      FadeInImage( pushButton, buttonImage );
      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = DisabledReleasedTransition;
      }
      else
      {
        mPaintState = ReleasedState;
      }
    }
    break;
  }
  case DisabledPressedState:
  {
    if( !disabled )
    {
      StopFadeOutAnimation( pushButton );
      FadeOutImage( pushButton, Background, disabledBackgroundImage );
      FadeOutImage( pushButton, Foreground, disabledImage );
      FadeInImage( pushButton, backgroundImage );
      FadeInImage( pushButton, selectedImage );
      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( selectedImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = DisabledPressedTransition;
      }
      else
      {
        mPaintState = PressedState;
      }
    }
    break;
  }
  case ReleasedPressedTransition:
  {
    if( disabled )
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton );
      StopFadeInAnimation();

      FadeOutImage( pushButton, Foreground, selectedImage, 1.f - opacity );
      FadeOutImage( pushButton, Background, backgroundImage );

      FadeInImage( pushButton, disabledImage );
      FadeInImage( pushButton, disabledBackgroundImage );

      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( selectedImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = PressedDisabledTransition;
      }
      else
      {
        mPaintState = DisabledPressedState;
      }
    }
    break;
  }
  case PressedReleasedTransition:
  {
    if( disabled )
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton );
      StopFadeInAnimation();

      FadeOutImage( pushButton, Foreground, buttonImage, 1.f - opacity );
      FadeOutImage( pushButton, Background, backgroundImage );

      FadeInImage( pushButton, disabledImage );
      FadeInImage( pushButton, disabledBackgroundImage );

      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = ReleasedDisabledTransition;
      }
      else
      {
        mPaintState = DisabledReleasedState;
      }
    }
    break;
  }
  case ReleasedDisabledTransition:
  {
    if( !disabled )
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton, false );
      StopFadeInAnimation();

      FadeOutImage( pushButton, Foreground, disabledImage, 1.f - opacity );
      FadeOutImage( pushButton, Background, disabledBackgroundImage, 1.f - opacity );
      FadeInImage( pushButton, buttonImage, opacity );
      FadeInImage( pushButton, backgroundImage, opacity );

      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = DisabledReleasedTransition;
      }
      else
      {
        mPaintState = ReleasedState;
      }
    }
    break;
  }
  case DisabledReleasedTransition:
  {
    if( disabled )
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton, false );
      StopFadeInAnimation();

      FadeOutImage( pushButton, Foreground, buttonImage, 1.f - opacity );
      FadeOutImage( pushButton, Background, backgroundImage, 1.f - opacity );
      FadeInImage( pushButton, disabledImage, opacity );
      FadeInImage( pushButton, disabledBackgroundImage, opacity );

      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = ReleasedDisabledTransition;
      }
      else
      {
        mPaintState = DisabledReleasedState;
      }
    }
    break;
  }
  case PressedDisabledTransition:
  {
    if( !disabled )
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton, false );
      StopFadeInAnimation();

      FadeOutImage( pushButton, Foreground, disabledImage, 1.f - opacity );
      FadeOutImage( pushButton, Background, disabledBackgroundImage, 1.f - opacity );
      FadeInImage( pushButton, selectedImage, opacity );
      FadeInImage( pushButton, backgroundImage, opacity );

      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( selectedImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = DisabledPressedTransition;
      }
      else
      {
        mPaintState = PressedState;
      }
    }
    break;
  }
  case DisabledPressedTransition:
  {
    if( disabled )
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton, false );
      StopFadeInAnimation();

      FadeOutImage( pushButton, Foreground, selectedImage, 1.f - opacity );
      FadeOutImage( pushButton, Background, backgroundImage, 1.f - opacity );
      FadeInImage( pushButton, disabledImage, opacity );
      FadeInImage( pushButton, disabledBackgroundImage, opacity );

      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( selectedImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = PressedDisabledTransition;
      }
      else
      {
        mPaintState = DisabledPressedState;
      }
    }
    break;
  }
  default:
    break;
  }
}

void PushButtonDefaultPainter::SetAnimationTime( float animationTime )
{
  mAnimationTime = animationTime;
}

float PushButtonDefaultPainter::GetAnimationTime() const
{
  return mAnimationTime;
}

void PushButtonDefaultPainter::SetAutoRepeating( bool autorepeating )
{
  mAutoRepeating = autorepeating;
}

void PushButtonDefaultPainter::SetLabel( Toolkit::Button& button, Actor label )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
  Actor& labelActor = pushButtonImpl.GetLabel();

  Toolkit::PushButton& pushButton = static_cast<Toolkit::PushButton&>( button );

  if( labelActor && labelActor.GetParent() )
  {
    labelActor.GetParent().Remove( labelActor );
  }

  labelActor = label;
  labelActor.SetAnchorPoint( AnchorPoint::CENTER );
  labelActor.SetParentOrigin( ParentOrigin::CENTER );

  labelActor.SetPosition( 0.f, 0.f, LABEL_DEPTH );
  labelActor.SetSize( mSize );

  pushButton.Add( labelActor  );
}

void PushButtonDefaultPainter::SetButtonImage( Toolkit::Button& button, Actor image )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
  Actor& buttonImage = pushButtonImpl.GetButtonImage();
  Actor& fadeOutButtonImage = pushButtonImpl.GetFadeOutButtonImage();

  Toolkit::PushButton& pushButton = static_cast<Toolkit::PushButton&>( button );

  switch( mPaintState )
  {
    case ReleasedState:
    {
      if( buttonImage && buttonImage.GetParent() )
      {
        StopFadeOutAnimation( pushButton );
        FadeOutImage( pushButton, Foreground, buttonImage );

        buttonImage = image;

        FadeInImage( pushButton, buttonImage );

        StartFadeOutAnimation( pushButton );
        StartFadeInAnimation();
      }
      else
      {
        buttonImage = image;
        pushButton.Add( buttonImage );
      }
      break;
    }
    case ReleasedPressedTransition: // FALLTHROUGH
    case ReleasedDisabledTransition:
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton );

      // Replaces the button image.
      buttonImage = image;

      pushButton.Add( buttonImage );
      FadeOutImage( pushButton, Foreground, buttonImage, opacity );

      StartFadeOutAnimation( pushButton );
      break;
    }
    case PressedReleasedTransition: // FALLTHROUGH
    case DisabledReleasedTransition:
    {
      StopFadeInAnimation();
      pushButton.Remove( buttonImage );

      buttonImage = image;

      FadeInImage( pushButton, buttonImage );
      StartFadeInAnimation();
      break;
    }
    default:
      buttonImage = image;
      break;
  }

  buttonImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  buttonImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  ApplyConstraint( buttonImage, FOREGROUND_DEPTH );
}

void PushButtonDefaultPainter::SetSelectedImage( Toolkit::Button& button, Actor image )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
  Actor& selectedImage = pushButtonImpl.GetSelectedImage();
  Actor& fadeOutButtonImage = pushButtonImpl.GetFadeOutButtonImage();

  Toolkit::PushButton& pushButton = static_cast<Toolkit::PushButton&>( button );

  switch( mPaintState )
  {
    case PressedState:
    {
      if( selectedImage && selectedImage.GetParent() )
      {
        StopFadeOutAnimation( pushButton );
        FadeOutImage( pushButton, Foreground, selectedImage  );

        selectedImage = image;

        FadeInImage( pushButton, selectedImage );

        StartFadeOutAnimation( pushButton );
        StartFadeInAnimation();
      }
      else
      {
        selectedImage = image;
        pushButton.Add( selectedImage );
      }
      break;
    }
    case PressedReleasedTransition: // FALLTHROUGH
    case PressedDisabledTransition:
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton );

      // Replaces the button image.
      selectedImage = image;

      pushButton.Add( selectedImage );
      FadeOutImage( pushButton, Foreground, selectedImage, opacity );

      StartFadeOutAnimation( pushButton );
      break;
    }
    case ReleasedPressedTransition: // FALLTHROUGH
    case DisabledPressedTransition:
    {
      StopFadeInAnimation();
      pushButton.Remove( selectedImage );

      selectedImage = image;

      FadeInImage( pushButton, selectedImage );
      StartFadeInAnimation();
      break;
    }
    default:
      selectedImage = image;
      break;
  }

  selectedImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  selectedImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  ApplyConstraint( selectedImage, FOREGROUND_DEPTH );
}

void PushButtonDefaultPainter::SetBackgroundImage( Toolkit::Button& button, Actor image )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
  Actor& backgroundImage = pushButtonImpl.GetBackgroundImage();
  Actor& fadeOutBackgroundImage = pushButtonImpl.GetFadeOutBackgroundImage();

  Toolkit::PushButton& pushButton = static_cast<Toolkit::PushButton&>( button );

  switch( mPaintState )
  {
    case ReleasedState:             // FALLTHROUGH
    case PressedState:
    case ReleasedPressedTransition:
    case PressedReleasedTransition:
    {
      if( backgroundImage && backgroundImage.GetParent() )
      {
        StopFadeOutAnimation( pushButton );
        FadeOutImage( pushButton, Background, backgroundImage  );

        backgroundImage = image;

        FadeInImage( pushButton, backgroundImage );

        StartFadeOutAnimation( pushButton );
        StartFadeInAnimation();
      }
      else
      {
        backgroundImage = image;
        pushButton.Add( backgroundImage );
      }
      break;
    }
    case ReleasedDisabledTransition: // FALLTHROUGH
    case PressedDisabledTransition:
    {
      float opacity = 1.f;
      if( fadeOutBackgroundImage )
      {
        opacity = fadeOutBackgroundImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton );

      // Replaces the button image.
      backgroundImage = image;

      pushButton.Add( backgroundImage );
      FadeOutImage( pushButton, Background, backgroundImage, opacity );

      StartFadeOutAnimation( pushButton );
      break;
    }
    case DisabledReleasedTransition: // FALLTHROUGH
    case DisabledPressedTransition:
    {
      StopFadeInAnimation();
      pushButton.Remove( backgroundImage );

      backgroundImage = image;

      FadeInImage( pushButton, backgroundImage );
      StartFadeInAnimation();
      break;
    }
    default:
      backgroundImage = image;
      break;
    }

  backgroundImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  backgroundImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  ApplyConstraint( backgroundImage, BACKGROUND_DEPTH );
}

void PushButtonDefaultPainter::SetDisabledImage( Toolkit::Button& button, Actor image )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
  Actor& disabledImage = pushButtonImpl.GetDisabledImage();
  Actor& fadeOutButtonImage = pushButtonImpl.GetFadeOutButtonImage();

  Toolkit::PushButton& pushButton = static_cast<Toolkit::PushButton&>( button );

  switch( mPaintState )
  {
    case DisabledReleasedState: // FALLTHROUGH
    case DisabledPressedState:
    {
      if( disabledImage && disabledImage.GetParent() )
      {
        StopFadeOutAnimation( pushButton );
        FadeOutImage( pushButton, Foreground, disabledImage  );

        disabledImage = image;

        FadeInImage( pushButton, disabledImage );

        StartFadeOutAnimation( pushButton );
        StartFadeInAnimation();
      }
      else
      {
        disabledImage = image;
        pushButton.Add( disabledImage );
      }
      break;
    }
    case ReleasedDisabledTransition: // FALLTHROUGH
    case PressedDisabledTransition:
    {
      StopFadeInAnimation();
      pushButton.Remove( disabledImage );

      disabledImage = image;

      FadeInImage( pushButton, disabledImage );
      StartFadeInAnimation();
      break;
    }
    case DisabledReleasedTransition: // FALLTHROUGH
    case DisabledPressedTransition:
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton );

      // Replaces the button image.
      disabledImage = image;

      pushButton.Add( disabledImage );
      FadeOutImage( pushButton, Foreground, disabledImage, opacity );

      StartFadeOutAnimation( pushButton );
      break;
    }
    default:
      disabledImage = image;
      break;
  }

  disabledImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  disabledImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  ApplyConstraint( disabledImage, FOREGROUND_DEPTH );
}

void PushButtonDefaultPainter::SetDisabledBackgroundImage( Toolkit::Button& button, Actor image )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
  Actor& disabledBackgroundImage = pushButtonImpl.GetDisabledBackgroundImage();
  Actor& fadeOutBackgroundImage = pushButtonImpl.GetFadeOutBackgroundImage();

  Toolkit::PushButton& pushButton = static_cast<Toolkit::PushButton&>( button );

  switch( mPaintState )
  {
    case DisabledReleasedState: // FALLTHROUGH
    case DisabledPressedState:
    {
      if( disabledBackgroundImage && disabledBackgroundImage.GetParent() )
      {
        StopFadeOutAnimation( pushButton );
        FadeOutImage( pushButton, Background, disabledBackgroundImage  );

        disabledBackgroundImage = image;

        FadeInImage( pushButton, disabledBackgroundImage );

        StartFadeOutAnimation( pushButton );
        StartFadeInAnimation();
      }
      else
      {
        disabledBackgroundImage = image;
        pushButton.Add( disabledBackgroundImage );
      }
      break;
    }
    case ReleasedDisabledTransition: // FALLTHROUGH
    case PressedDisabledTransition:
    {
      StopFadeInAnimation();
      pushButton.Remove( disabledBackgroundImage );

      disabledBackgroundImage = image;

      FadeInImage( pushButton, disabledBackgroundImage );
      StartFadeInAnimation();
      break;
    }
    case DisabledReleasedTransition: // FALLTHROUGH
    case DisabledPressedTransition:
    {
      float opacity = 1.f;
      if( fadeOutBackgroundImage )
      {
        opacity = fadeOutBackgroundImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton );

      // Replaces the button image.
      disabledBackgroundImage = image;

      pushButton.Add( disabledBackgroundImage );
      FadeOutImage( pushButton, Background, disabledBackgroundImage, opacity );

      StartFadeOutAnimation( pushButton );
      break;
    }
    default:
      disabledBackgroundImage = image;
      break;
  }

  disabledBackgroundImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  disabledBackgroundImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  ApplyConstraint( disabledBackgroundImage, BACKGROUND_DEPTH );
}

void PushButtonDefaultPainter::Pressed( Toolkit::Button& button )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
  Actor& selectedImage = pushButtonImpl.GetSelectedImage();
  Actor& buttonImage = pushButtonImpl.GetButtonImage();
  Actor& fadeOutButtonImage = pushButtonImpl.GetFadeOutButtonImage();

  Toolkit::PushButton& pushButton = static_cast<Toolkit::PushButton&>( button );

  switch( mPaintState )
  {
    case ReleasedState:
    {
      StopFadeOutAnimation( pushButton );
      FadeOutImage( pushButton, Foreground, buttonImage );
      FadeInImage( pushButton, selectedImage );
      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || selectedImage )
      {
        mPaintState = ReleasedPressedTransition;
      }
      else
      {
        mPaintState = PressedState;
      }
      break;
    }
    case ReleasedPressedTransition:
    {
      if( !mAutoRepeating )
      {
        mPaintState = PressedReleasedTransition;
      }
      break;
    }
    case PressedReleasedTransition:
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton, false );
      StopFadeInAnimation();

      FadeOutImage( pushButton, Foreground, buttonImage, 1.f - opacity );
      FadeInImage( pushButton, selectedImage, opacity );

      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || selectedImage )
      {
        mPaintState = ReleasedPressedTransition;
      }
      else
      {
        mPaintState = PressedState;
      }
      break;
    }
    default:
      break;
  }
}

void PushButtonDefaultPainter::Released( Toolkit::Button& button )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
  Actor& selectedImage = pushButtonImpl.GetSelectedImage();
  Actor& buttonImage = pushButtonImpl.GetButtonImage();
  Actor& fadeOutButtonImage = pushButtonImpl.GetFadeOutButtonImage();

  Toolkit::PushButton& pushButton = static_cast<Toolkit::PushButton&>( button );

  switch( mPaintState )
  {
    case PressedState:
    {
      StopFadeOutAnimation( pushButton );
      FadeOutImage( pushButton, Foreground, selectedImage );
      FadeInImage( pushButton, buttonImage );
      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || selectedImage )
      {
        mPaintState = PressedReleasedTransition;
      }
      else
      {
        mPaintState = ReleasedState;
      }
      break;
    }
    case ReleasedPressedTransition:
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton, false );
      StopFadeInAnimation();

      FadeOutImage( pushButton, Foreground, selectedImage, 1.f - opacity );
      FadeInImage( pushButton, buttonImage, opacity );

      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || selectedImage )
      {
        mPaintState = PressedReleasedTransition;
      }
      else
      {
        mPaintState = ReleasedState;
      }
      break;
    }
    default:
    {
      break;
    }
  }
}

void PushButtonDefaultPainter::Clicked( Toolkit::Button& button )
{
  Released( button );
}

void PushButtonDefaultPainter::Selected( Toolkit::Button& button )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
  Actor& selectedImage = pushButtonImpl.GetSelectedImage();
  Actor& buttonImage = pushButtonImpl.GetButtonImage();
  Actor& fadeOutButtonImage = pushButtonImpl.GetFadeOutButtonImage();

  Toolkit::PushButton& pushButton = static_cast<Toolkit::PushButton&>( button );

  switch( mPaintState )
  {
    case ReleasedState:
    {
      StopFadeOutAnimation( pushButton );
      FadeOutImage( pushButton, Foreground, buttonImage );
      FadeInImage( pushButton, selectedImage );
      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || selectedImage )
      {
        mPaintState = ReleasedPressedTransition;
      }
      else
      {
        mPaintState = PressedState;
      }
      break;
    }
    case PressedState:
    {
      StopFadeOutAnimation( pushButton );
      FadeOutImage( pushButton, Foreground, selectedImage );
      FadeInImage( pushButton, buttonImage );
      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || selectedImage )
      {
        mPaintState = PressedReleasedTransition;
      }
      else
      {
        mPaintState = ReleasedState;
      }
      break;
    }
    case ReleasedPressedTransition:
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton, false );
      StopFadeInAnimation();

      FadeOutImage( pushButton, Foreground, selectedImage, 1.f - opacity );
      FadeInImage( pushButton, buttonImage, opacity );

      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || selectedImage )
      {
        mPaintState = PressedReleasedTransition;
      }
      else
      {
        mPaintState = ReleasedState;
      }
      break;
    }
    case PressedReleasedTransition:
    {
      float opacity = 0.f;
      if( fadeOutButtonImage )
      {
        opacity = 1.f - fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton, false );
      StopFadeInAnimation();

      FadeOutImage( pushButton, Foreground, buttonImage, 1.f - opacity );
      FadeInImage( pushButton, selectedImage, opacity );

      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || selectedImage )
      {
        mPaintState = ReleasedPressedTransition;
      }
      else
      {
        mPaintState = PressedState;
      }
      break;
    }
    default:
    {
      break;
    }
  }
}

void PushButtonDefaultPainter::ApplyConstraint( Actor& actor, float depth )
{
  if( actor )
  {
    actor.SetPosition( 0.f, 0.f, depth );
    actor.RemoveConstraints();
    actor.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
  }
}

void PushButtonDefaultPainter::AddToFadeInAnimation( const Actor& actor )
{
  if( !mFadeInAnimation )
  {
    mFadeInAnimation = Dali::Animation::New( mAnimationTime );
  }

  mFadeInAnimation.OpacityTo( actor, 1.f );
}

void PushButtonDefaultPainter::StartFadeInAnimation()
{
  if( mFadeInAnimation )
  {
    mFadeInAnimation.FinishedSignal().Connect( this, &PushButtonDefaultPainter::PressedReleasedFadeInAnimationFinished );
    mFadeInAnimation.Play();
  }
}

void PushButtonDefaultPainter::StopFadeInAnimation()
{
  if( mFadeInAnimation )
  {
    mFadeInAnimation.Clear();
    mFadeInAnimation.Reset();
  }
}

void PushButtonDefaultPainter::AddToFadeOutAnimation( const Actor& actor )
{
  if( !mFadeOutAnimation )
  {
    mFadeOutAnimation = Dali::Animation::New( mAnimationTime );
  }

  mFadeOutAnimation.OpacityTo( actor, 0.f );
}

void PushButtonDefaultPainter::StartFadeOutAnimation( Toolkit::PushButton& pushButton )
{
  if( mFadeOutAnimation )
  {
    Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( pushButton );
    mButton = &pushButtonImpl;

    mFadeOutAnimation.FinishedSignal().Connect( this, &PushButtonDefaultPainter::PressedReleasedFadeOutAnimationFinished );
    mFadeOutAnimation.Play();
  }
}

void PushButtonDefaultPainter::StopFadeOutAnimation( Toolkit::PushButton& pushButton, bool remove )
{
  if( mFadeOutAnimation )
  {
    mFadeOutAnimation.Clear();
  }

  mFadeOutAnimation.Reset();

  if( remove )
  {
    Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( pushButton );
    Actor& fadeOutButtonImage = pushButtonImpl.GetFadeOutButtonImage();
    Actor& fadeOutBackgroundImage = pushButtonImpl.GetFadeOutBackgroundImage();

    if( fadeOutButtonImage && fadeOutButtonImage.GetParent() )
    {
      fadeOutButtonImage.GetParent().Remove( fadeOutButtonImage );
    }

    if( fadeOutBackgroundImage && fadeOutBackgroundImage.GetParent() )
    {
      fadeOutBackgroundImage.GetParent().Remove( fadeOutBackgroundImage );
    }

    fadeOutButtonImage.Reset();
    fadeOutBackgroundImage.Reset();
  }
}

void PushButtonDefaultPainter::FadeInImage( Toolkit::PushButton& pushButton, Actor& image, float opacity )
{
  if( image )
  {
    image.SetOpacity( opacity );
    if( !image.GetParent() )
    {
      pushButton.Add( image );
    }

    AddToFadeInAnimation( image );
  }
}

void PushButtonDefaultPainter::FadeOutImage( Toolkit::PushButton& pushButton, ImageLayer layer, Actor& image, float opacity )
{
  if( image )
  {
    Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( pushButton );
    Actor& fadeOutButtonImage = pushButtonImpl.GetFadeOutButtonImage();
    Actor& fadeOutBackgroundImage = pushButtonImpl.GetFadeOutBackgroundImage();

    Actor& actorLayer = ( ( Background == layer ) ? fadeOutBackgroundImage : fadeOutButtonImage );

    actorLayer = image;
    actorLayer.SetOpacity( opacity );

    AddToFadeOutAnimation( actorLayer );
  }
}

void PushButtonDefaultPainter::PressedReleasedFadeOutAnimationFinished( Dali::Animation& source )
{
  switch( mPaintState )
  {
    case ReleasedPressedTransition:
    {
      mPaintState = PressedState;
      break;
    }
    case PressedReleasedTransition:
    {
      mPaintState = ReleasedState;
      break;
    }
    case ReleasedDisabledTransition:
    {
      mPaintState = DisabledReleasedState;
      break;
    }
    case DisabledReleasedTransition:
    {
      mPaintState = ReleasedState;
      break;
    }
    case PressedDisabledTransition:
    {
      mPaintState = DisabledPressedState;
      break;
    }
    case DisabledPressedTransition:
    {
      mPaintState = PressedState;
      break;
    }
    default:
    {
      break;
    }
  }

  Toolkit::PushButton handle( mButton->GetOwner() );
  StopFadeOutAnimation( handle );
  mButton = NULL;
}

void PushButtonDefaultPainter::PressedReleasedFadeInAnimationFinished( Dali::Animation& source )
{
  switch( mPaintState )
  {
    case ReleasedPressedTransition:
    {
      mPaintState = PressedState;
      break;
    }
    case PressedReleasedTransition:
    {
      mPaintState = ReleasedState;
      break;
    }
    case ReleasedDisabledTransition:
    {
      mPaintState = DisabledReleasedState;
      break;
    }
    case DisabledReleasedTransition:
    {
      mPaintState = ReleasedState;
      break;
    }
    case PressedDisabledTransition:
    {
      mPaintState = DisabledPressedState;
      break;
    }
    case DisabledPressedTransition:
    {
      mPaintState = PressedState;
      break;
    }
    default:
    {
      break;
    }
  }

  StopFadeInAnimation();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
