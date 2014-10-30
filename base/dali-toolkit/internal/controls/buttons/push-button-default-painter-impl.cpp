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
#include <dali-toolkit/public-api/controls/text-view/text-view.h>
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
  mDimmed( false ),
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

void PushButtonDefaultPainter::SetButtonImage( Toolkit::PushButton& pushButton, Actor image )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetImplementation( pushButton );
  Actor& buttonImage = pushButtonImpl.GetButtonImage();
  Actor& fadeOutButtonImage = pushButtonImpl.GetFadeOutButtonImage();

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
    case ReleasedDimmedTransition:
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
    case DimmedReleasedTransition:
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

void PushButtonDefaultPainter::SetBackgroundImage( Toolkit::PushButton& pushButton, Actor image )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetImplementation( pushButton );
  Actor& backgroundImage = pushButtonImpl.GetBackgroundImage();
  Actor& fadeOutBackgroundImage = pushButtonImpl.GetFadeOutBackgroundImage();

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
    case ReleasedDimmedTransition: // FALLTHROUGH
    case PressedDimmedTransition:
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
    case DimmedReleasedTransition: // FALLTHROUGH
    case DimmedPressedTransition:
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

void PushButtonDefaultPainter::SetPressedImage( Toolkit::PushButton& pushButton, Actor image )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetImplementation( pushButton );
  Actor& pressedImage = pushButtonImpl.GetPressedImage();
  Actor& fadeOutButtonImage = pushButtonImpl.GetFadeOutButtonImage();

  switch( mPaintState )
  {
    case PressedState:
    {
      if( pressedImage && pressedImage.GetParent() )
      {
        StopFadeOutAnimation( pushButton );
        FadeOutImage( pushButton, Foreground, pressedImage  );

        pressedImage = image;

        FadeInImage( pushButton, pressedImage );

        StartFadeOutAnimation( pushButton );
        StartFadeInAnimation();
      }
      else
      {
        pressedImage = image;
        pushButton.Add( pressedImage );
      }
      break;
    }
    case PressedReleasedTransition: // FALLTHROUGH
    case PressedDimmedTransition:
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton );

      // Replaces the button image.
      pressedImage = image;

      pushButton.Add( pressedImage );
      FadeOutImage( pushButton, Foreground, pressedImage, opacity );

      StartFadeOutAnimation( pushButton );
      break;
    }
    case ReleasedPressedTransition: // FALLTHROUGH
    case DimmedPressedTransition:
    {
      StopFadeInAnimation();
      pushButton.Remove( pressedImage );

      pressedImage = image;

      FadeInImage( pushButton, pressedImage );
      StartFadeInAnimation();
      break;
    }
    default:
      pressedImage = image;
      break;
  }

  pressedImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  pressedImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  ApplyConstraint( pressedImage, FOREGROUND_DEPTH );
}

void PushButtonDefaultPainter::SetDimmedImage( Toolkit::PushButton& pushButton, Actor image )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetImplementation( pushButton );
  Actor& dimmedImage = pushButtonImpl.GetDimmedImage();
  Actor& fadeOutButtonImage = pushButtonImpl.GetFadeOutButtonImage();

  switch( mPaintState )
  {
    case DimmedReleasedState: // FALLTHROUGH
    case DimmedPressedState:
    {
      if( dimmedImage && dimmedImage.GetParent() )
      {
        StopFadeOutAnimation( pushButton );
        FadeOutImage( pushButton, Foreground, dimmedImage  );

        dimmedImage = image;

        FadeInImage( pushButton, dimmedImage );

        StartFadeOutAnimation( pushButton );
        StartFadeInAnimation();
      }
      else
      {
        dimmedImage = image;
        pushButton.Add( dimmedImage );
      }
      break;
    }
    case ReleasedDimmedTransition: // FALLTHROUGH
    case PressedDimmedTransition:
    {
      StopFadeInAnimation();
      pushButton.Remove( dimmedImage );

      dimmedImage = image;

      FadeInImage( pushButton, dimmedImage );
      StartFadeInAnimation();
      break;
    }
    case DimmedReleasedTransition: // FALLTHROUGH
    case DimmedPressedTransition:
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton );

      // Replaces the button image.
      dimmedImage = image;

      pushButton.Add( dimmedImage );
      FadeOutImage( pushButton, Foreground, dimmedImage, opacity );

      StartFadeOutAnimation( pushButton );
      break;
    }
    default:
      dimmedImage = image;
      break;
  }

  dimmedImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  dimmedImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  ApplyConstraint( dimmedImage, FOREGROUND_DEPTH );
}

void PushButtonDefaultPainter::SetDimmedBackgroundImage( Toolkit::PushButton& pushButton, Actor image )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetImplementation( pushButton );
  Actor& dimmedBackgroundImage = pushButtonImpl.GetDimmedBackgroundImage();
  Actor& fadeOutBackgroundImage = pushButtonImpl.GetFadeOutBackgroundImage();

  switch( mPaintState )
  {
    case DimmedReleasedState: // FALLTHROUGH
    case DimmedPressedState:
    {
      if( dimmedBackgroundImage && dimmedBackgroundImage.GetParent() )
      {
        StopFadeOutAnimation( pushButton );
        FadeOutImage( pushButton, Background, dimmedBackgroundImage  );

        dimmedBackgroundImage = image;

        FadeInImage( pushButton, dimmedBackgroundImage );

        StartFadeOutAnimation( pushButton );
        StartFadeInAnimation();
      }
      else
      {
        dimmedBackgroundImage = image;
        pushButton.Add( dimmedBackgroundImage );
      }
      break;
    }
    case ReleasedDimmedTransition: // FALLTHROUGH
    case PressedDimmedTransition:
    {
      StopFadeInAnimation();
      pushButton.Remove( dimmedBackgroundImage );

      dimmedBackgroundImage = image;

      FadeInImage( pushButton, dimmedBackgroundImage );
      StartFadeInAnimation();
      break;
    }
    case DimmedReleasedTransition: // FALLTHROUGH
    case DimmedPressedTransition:
    {
      float opacity = 1.f;
      if( fadeOutBackgroundImage )
      {
        opacity = fadeOutBackgroundImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton );

      // Replaces the button image.
      dimmedBackgroundImage = image;

      pushButton.Add( dimmedBackgroundImage );
      FadeOutImage( pushButton, Background, dimmedBackgroundImage, opacity );

      StartFadeOutAnimation( pushButton );
      break;
    }
    default:
      dimmedBackgroundImage = image;
      break;
  }

  dimmedBackgroundImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  dimmedBackgroundImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  ApplyConstraint( dimmedBackgroundImage, BACKGROUND_DEPTH );
}

void PushButtonDefaultPainter::SetLabelText( Toolkit::PushButton& pushButton, Actor text )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetImplementation( pushButton );
  Actor& label = pushButtonImpl.GetLabel();

  if( label && label.GetParent() )
  {
    label.GetParent().Remove( label );
  }

  label = text;
  label.SetAnchorPoint( AnchorPoint::CENTER );
  label.SetParentOrigin( ParentOrigin::CENTER );

  label.SetPosition( 0.f, 0.f, LABEL_DEPTH );
  label.SetSize( mSize );

  pushButton.Add( label  );
}

void PushButtonDefaultPainter::Initialize( Toolkit::Button& button )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
  Actor& buttonImage = pushButtonImpl.GetButtonImage();
  Actor& pressedImage = pushButtonImpl.GetPressedImage();
  Actor& backgroundImage = pushButtonImpl.GetBackgroundImage();
  Actor& dimmedImage = pushButtonImpl.GetDimmedImage();
  Actor& dimmedBackgroundImage = pushButtonImpl.GetDimmedBackgroundImage();
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

  if( pressedImage )
  {
    SetPressedImage( pushButton, pressedImage );
  }

  if( dimmedImage )
  {
    SetDimmedImage( pushButton, dimmedImage );
  }

  if( dimmedBackgroundImage )
  {
    SetDimmedBackgroundImage( pushButton, dimmedBackgroundImage );
  }

  if( label )
  {
    SetLabelText( pushButton, label );
  }

  SetDimmed( pushButton, mDimmed );
}

void PushButtonDefaultPainter::SetSize( Toolkit::Button& button, const Vector3& size )
{
  if( size != mSize )
  {
    mSize = size;

    Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
    Actor& buttonImage = pushButtonImpl.GetButtonImage();
    Actor& pressedImage = pushButtonImpl.GetPressedImage();
    Actor& backgroundImage = pushButtonImpl.GetBackgroundImage();
    Actor& dimmedImage = pushButtonImpl.GetDimmedImage();
    Actor& dimmedBackgroundImage = pushButtonImpl.GetDimmedBackgroundImage();
    Actor& label = pushButtonImpl.GetLabel();

    ApplyConstraint( buttonImage, FOREGROUND_DEPTH );
    ApplyConstraint( backgroundImage, BACKGROUND_DEPTH );
    ApplyConstraint( pressedImage, FOREGROUND_DEPTH );
    ApplyConstraint( dimmedImage, FOREGROUND_DEPTH );
    ApplyConstraint( dimmedBackgroundImage, BACKGROUND_DEPTH );

    if( label )
    {
      label.SetPosition( 0.f, 0.f, LABEL_DEPTH );
      label.SetSize( mSize );
    }
  }
}

void PushButtonDefaultPainter::SetDimmed( Toolkit::Button& button, bool dimmed )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
  Actor& buttonImage = pushButtonImpl.GetButtonImage();
  Actor& pressedImage = pushButtonImpl.GetPressedImage();
  Actor& backgroundImage = pushButtonImpl.GetBackgroundImage();
  Actor& dimmedImage = pushButtonImpl.GetDimmedImage();
  Actor& dimmedBackgroundImage = pushButtonImpl.GetDimmedBackgroundImage();
  Actor& fadeOutButtonImage = pushButtonImpl.GetFadeOutButtonImage();

  Toolkit::PushButton& pushButton = static_cast<Toolkit::PushButton&>( button );

  mDimmed = dimmed;

  switch( mPaintState )
  {
  case ReleasedState:
  {
    if( dimmed )
    {
      StopFadeOutAnimation( pushButton );
      FadeOutImage( pushButton, Background, backgroundImage );
      FadeOutImage( pushButton, Foreground, buttonImage );
      FadeInImage( pushButton, dimmedBackgroundImage );
      FadeInImage( pushButton, dimmedImage );
      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || dimmedImage || backgroundImage || dimmedBackgroundImage )
      {
        mPaintState = ReleasedDimmedTransition;
      }
      else
      {
        mPaintState = DimmedReleasedState;
      }
    }
    break;
  }
  case PressedState:
  {
    if( dimmed )
    {
      StopFadeOutAnimation( pushButton );
      FadeOutImage( pushButton, Background, backgroundImage );
      FadeOutImage( pushButton, Foreground, pressedImage );
      FadeInImage( pushButton, dimmedBackgroundImage );
      FadeInImage( pushButton, dimmedImage );
      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( pressedImage || dimmedImage || backgroundImage || dimmedBackgroundImage )
      {
        mPaintState = PressedDimmedTransition;
      }
      else
      {
        mPaintState = DimmedPressedState;
      }
    }
    break;
  }
  case DimmedReleasedState:
  {
    if( !dimmed )
    {
      StopFadeOutAnimation( pushButton );
      FadeOutImage( pushButton, Background, dimmedBackgroundImage );
      FadeOutImage( pushButton, Foreground, dimmedImage );
      FadeInImage( pushButton, backgroundImage );
      FadeInImage( pushButton, buttonImage );
      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || dimmedImage || backgroundImage || dimmedBackgroundImage )
      {
        mPaintState = DimmedReleasedTransition;
      }
      else
      {
        mPaintState = ReleasedState;
      }
    }
    break;
  }
  case DimmedPressedState:
  {
    if( !dimmed )
    {
      StopFadeOutAnimation( pushButton );
      FadeOutImage( pushButton, Background, dimmedBackgroundImage );
      FadeOutImage( pushButton, Foreground, dimmedImage );
      FadeInImage( pushButton, backgroundImage );
      FadeInImage( pushButton, pressedImage );
      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( pressedImage || dimmedImage || backgroundImage || dimmedBackgroundImage )
      {
        mPaintState = DimmedPressedTransition;
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
    if( dimmed )
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton );
      StopFadeInAnimation();

      FadeOutImage( pushButton, Foreground, pressedImage, 1.f - opacity );
      FadeOutImage( pushButton, Background, backgroundImage );

      FadeInImage( pushButton, dimmedImage );
      FadeInImage( pushButton, dimmedBackgroundImage );

      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( pressedImage || dimmedImage || backgroundImage || dimmedBackgroundImage )
      {
        mPaintState = PressedDimmedTransition;
      }
      else
      {
        mPaintState = DimmedPressedState;
      }
    }
    break;
  }
  case PressedReleasedTransition:
  {
    if( dimmed )
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

      FadeInImage( pushButton, dimmedImage );
      FadeInImage( pushButton, dimmedBackgroundImage );

      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || dimmedImage || backgroundImage || dimmedBackgroundImage )
      {
        mPaintState = ReleasedDimmedTransition;
      }
      else
      {
        mPaintState = DimmedReleasedState;
      }
    }
    break;
  }
  case ReleasedDimmedTransition:
  {
    if( !dimmed )
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton, false );
      StopFadeInAnimation();

      FadeOutImage( pushButton, Foreground, dimmedImage, 1.f - opacity );
      FadeOutImage( pushButton, Background, dimmedBackgroundImage, 1.f - opacity );
      FadeInImage( pushButton, buttonImage, opacity );
      FadeInImage( pushButton, backgroundImage, opacity );

      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || dimmedImage || backgroundImage || dimmedBackgroundImage )
      {
        mPaintState = DimmedReleasedTransition;
      }
      else
      {
        mPaintState = ReleasedState;
      }
    }
    break;
  }
  case DimmedReleasedTransition:
  {
    if( dimmed )
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
      FadeInImage( pushButton, dimmedImage, opacity );
      FadeInImage( pushButton, dimmedBackgroundImage, opacity );

      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( buttonImage || dimmedImage || backgroundImage || dimmedBackgroundImage )
      {
        mPaintState = ReleasedDimmedTransition;
      }
      else
      {
        mPaintState = DimmedReleasedState;
      }
    }
    break;
  }
  case PressedDimmedTransition:
  {
    if( !dimmed )
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton, false );
      StopFadeInAnimation();

      FadeOutImage( pushButton, Foreground, dimmedImage, 1.f - opacity );
      FadeOutImage( pushButton, Background, dimmedBackgroundImage, 1.f - opacity );
      FadeInImage( pushButton, pressedImage, opacity );
      FadeInImage( pushButton, backgroundImage, opacity );

      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( pressedImage || dimmedImage || backgroundImage || dimmedBackgroundImage )
      {
        mPaintState = DimmedPressedTransition;
      }
      else
      {
        mPaintState = PressedState;
      }
    }
    break;
  }
  case DimmedPressedTransition:
  {
    if( dimmed )
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( pushButton, false );
      StopFadeInAnimation();

      FadeOutImage( pushButton, Foreground, pressedImage, 1.f - opacity );
      FadeOutImage( pushButton, Background, backgroundImage, 1.f - opacity );
      FadeInImage( pushButton, dimmedImage, opacity );
      FadeInImage( pushButton, dimmedBackgroundImage, opacity );

      StartFadeOutAnimation( pushButton );
      StartFadeInAnimation();

      if( pressedImage || dimmedImage || backgroundImage || dimmedBackgroundImage )
      {
        mPaintState = PressedDimmedTransition;
      }
      else
      {
        mPaintState = DimmedPressedState;
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

void PushButtonDefaultPainter::Pressed( Toolkit::PushButton& button )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
  Actor& pressedImage = pushButtonImpl.GetPressedImage();
  Actor& buttonImage = pushButtonImpl.GetButtonImage();
  Actor& fadeOutButtonImage = pushButtonImpl.GetFadeOutButtonImage();

  switch( mPaintState )
  {
    case ReleasedState:
    {
      StopFadeOutAnimation( button );
      FadeOutImage( button, Foreground, buttonImage );
      FadeInImage( button, pressedImage );
      StartFadeOutAnimation( button );
      StartFadeInAnimation();

      if( buttonImage || pressedImage )
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
      StopFadeOutAnimation( button, false );
      StopFadeInAnimation();

      FadeOutImage( button, Foreground, buttonImage, 1.f - opacity );
      FadeInImage( button, pressedImage, opacity );

      StartFadeOutAnimation( button );
      StartFadeInAnimation();

      if( buttonImage || pressedImage )
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

void PushButtonDefaultPainter::Released( Toolkit::PushButton& button )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
  Actor& pressedImage = pushButtonImpl.GetPressedImage();
  Actor& buttonImage = pushButtonImpl.GetButtonImage();
  Actor& fadeOutButtonImage = pushButtonImpl.GetFadeOutButtonImage();

  switch( mPaintState )
  {
    case PressedState:
    {
      StopFadeOutAnimation( button );
      FadeOutImage( button, Foreground, pressedImage );
      FadeInImage( button, buttonImage );
      StartFadeOutAnimation( button );
      StartFadeInAnimation();

      if( buttonImage || pressedImage )
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
      StopFadeOutAnimation( button, false );
      StopFadeInAnimation();

      FadeOutImage( button, Foreground, pressedImage, 1.f - opacity );
      FadeInImage( button, buttonImage, opacity );

      StartFadeOutAnimation( button );
      StartFadeInAnimation();

      if( buttonImage || pressedImage )
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

void PushButtonDefaultPainter::Clicked( Toolkit::PushButton& button )
{
  Released( button );
}

void PushButtonDefaultPainter::Toggled( Toolkit::PushButton& button )
{
  Toolkit::Internal::PushButton& pushButtonImpl = GetPushButtonImpl( button );
  Actor& pressedImage = pushButtonImpl.GetPressedImage();
  Actor& buttonImage = pushButtonImpl.GetButtonImage();
  Actor& fadeOutButtonImage = pushButtonImpl.GetFadeOutButtonImage();

  switch( mPaintState )
  {
    case ReleasedState:
    {
      StopFadeOutAnimation( button );
      FadeOutImage( button, Foreground, buttonImage );
      FadeInImage( button, pressedImage );
      StartFadeOutAnimation( button );
      StartFadeInAnimation();

      if( buttonImage || pressedImage )
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
      StopFadeOutAnimation( button );
      FadeOutImage( button, Foreground, pressedImage );
      FadeInImage( button, buttonImage );
      StartFadeOutAnimation( button );
      StartFadeInAnimation();

      if( buttonImage || pressedImage )
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
      StopFadeOutAnimation( button, false );
      StopFadeInAnimation();

      FadeOutImage( button, Foreground, pressedImage, 1.f - opacity );
      FadeInImage( button, buttonImage, opacity );

      StartFadeOutAnimation( button );
      StartFadeInAnimation();

      if( buttonImage || pressedImage )
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
      StopFadeOutAnimation( button, false );
      StopFadeInAnimation();

      FadeOutImage( button, Foreground, buttonImage, 1.f - opacity );
      FadeInImage( button, pressedImage, opacity );

      StartFadeOutAnimation( button );
      StartFadeInAnimation();

      if( buttonImage || pressedImage )
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
    case ReleasedDimmedTransition:
    {
      mPaintState = DimmedReleasedState;
      break;
    }
    case DimmedReleasedTransition:
    {
      mPaintState = ReleasedState;
      break;
    }
    case PressedDimmedTransition:
    {
      mPaintState = DimmedPressedState;
      break;
    }
    case DimmedPressedTransition:
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
    case ReleasedDimmedTransition:
    {
      mPaintState = DimmedReleasedState;
      break;
    }
    case DimmedReleasedTransition:
    {
      mPaintState = ReleasedState;
      break;
    }
    case PressedDimmedTransition:
    {
      mPaintState = DimmedPressedState;
      break;
    }
    case DimmedPressedTransition:
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
