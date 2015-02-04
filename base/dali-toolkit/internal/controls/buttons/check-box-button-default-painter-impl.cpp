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
#include "check-box-button-default-painter-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraints.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/object/ref-object.h>
#include "check-box-button-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
const float FOREGROUND_DEPTH( 0.5f );
const float BACKGROUND_DEPTH( 0.25f );

const float ANIMATION_TIME( 0.26f );  // EFL checkbox tick time

/**
 * Constraint to wrap an actor in y that is moving vertically
 */
Vector3 EqualToPercentageWidthConstraint( const Vector3& current,
                                          const PropertyInput& percentageProperty,
                                          const PropertyInput& parentSizeProperty )
{
  float percentage = percentageProperty.GetFloat();
  const Vector3& parentSize = parentSizeProperty.GetVector3();

  Vector3 size( parentSize );
  size.x *= percentage;

  return size;
}


inline Toolkit::Internal::CheckBoxButton& GetCheckBoxButtonImpl( Toolkit::Button& button )
{
  DALI_ASSERT_ALWAYS( button );

  Dali::RefObject& handle = button.GetImplementation();

  return static_cast<Toolkit::Internal::CheckBoxButton&>( handle );
}

inline const Toolkit::Internal::CheckBoxButton& GetCheckBoxButtonImpl( const Toolkit::Button& button )
{
  DALI_ASSERT_ALWAYS( button );

  const Dali::RefObject& handle = button.GetImplementation();

  return static_cast<const Toolkit::Internal::CheckBoxButton&>( handle );
}

}

CheckBoxButtonDefaultPainter::CheckBoxButtonDefaultPainter()
: CheckBoxButtonPainter(),
  mDisabled( false ),
  mPaintState( UncheckedState ),
  mButton(NULL),
  mAnimationTime( ANIMATION_TIME )
{
}

CheckBoxButtonDefaultPainter::~CheckBoxButtonDefaultPainter()
{
  if( mCheckInAnimation )
  {
    mCheckInAnimation.Clear();
  }
  if( mCheckOutAnimation )
  {
    mCheckOutAnimation.Clear();
  }
}

void CheckBoxButtonDefaultPainter::SetBackgroundImage( Toolkit::CheckBoxButton& checkBox, Actor image )
{
  Toolkit::Internal::CheckBoxButton& checkBoxImpl = GetImplementation( checkBox );
  Actor& backgroundImage = checkBoxImpl.GetBackgroundImage();
  Actor& fadeOutBackgroundImage = checkBoxImpl.GetFadeOutBackgroundImage();

  switch( mPaintState )
  {
    case UncheckedState:             // FALLTHROUGH
    case CheckedState:
    case UncheckedCheckedTransition:
    case CheckedUncheckedTransition:
    {
      if( backgroundImage && backgroundImage.GetParent() )
      {
        StopCheckOutAnimation( checkBox );
        FadeOutImage( checkBox, Background, backgroundImage  );

        backgroundImage = image;

        FadeInImage( checkBox, backgroundImage );

        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();
      }
      else
      {
        backgroundImage = image;
        checkBox.Add( backgroundImage );
      }
      break;
    }
    case DisabledUncheckedTransition: // FALLTHROUGH
    case DisabledCheckedTransition:
    {
      StopCheckInAnimation();
      checkBox.Remove( backgroundImage );

      backgroundImage = image;

      FadeInImage( checkBox, backgroundImage );
      StartCheckInAnimation();
      break;
    }
    case CheckedDisabledTransition:   // FALLTHROUGH
    case UncheckedDisabledTransition:
    {
      float opacity = 1.f;
      if( fadeOutBackgroundImage )
      {
        opacity = fadeOutBackgroundImage.GetCurrentOpacity();
      }
      StopCheckOutAnimation( checkBox );

      // Replaces the button image.
      backgroundImage = image;

      checkBox.Add( backgroundImage );
      FadeOutImage( checkBox, Background, backgroundImage, opacity );

      StartCheckOutAnimation( checkBox );
      break;
    }
    default:
    {
      backgroundImage = image;
      break;
    }
  }

  backgroundImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  backgroundImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  ApplyConstraint( backgroundImage, BACKGROUND_DEPTH );
}

void CheckBoxButtonDefaultPainter::SetCheckedImage( Toolkit::CheckBoxButton& checkBox, Actor image )
{
  Toolkit::Internal::CheckBoxButton& checkBoxImpl = GetImplementation( checkBox );
  Actor& checkedImage = checkBoxImpl.GetCheckedImage();
  Actor& fadeOutCheckedImage = checkBoxImpl.GetFadeOutCheckedImage();

  switch( mPaintState )
  {
    case CheckedState:
    {
      if( checkedImage && checkedImage.GetParent() )
      {
        StopCheckOutAnimation( checkBox );
        FadeOutImage( checkBox, Foreground, checkedImage );

        checkedImage = image;

        FadeInImage( checkBox, checkedImage );

        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();
      }
      else
      {
        checkedImage = image;
        checkBox.Add( checkedImage );
      }
      break;
    }
    case UncheckedCheckedTransition: // FALLTHROUGH
    case DisabledCheckedTransition:
    {
      StopCheckInAnimation();
      checkBox.Remove( checkedImage );

      checkedImage = image;

      FadeInImage( checkBox, checkedImage );
      StartCheckInAnimation();
      break;
    }
    case CheckedUncheckedTransition: // FALLTHROUGH
    case CheckedDisabledTransition:
    {
      float opacity = 1.f;
      if( fadeOutCheckedImage )
      {
        opacity = fadeOutCheckedImage.GetCurrentOpacity();
      }
      StopCheckOutAnimation( checkBox );

      // Replaces the button image.
      checkedImage = image;

      checkBox.Add( checkedImage );
      FadeOutImage( checkBox, Foreground, checkedImage, opacity );

      StartCheckOutAnimation( checkBox );
      break;
    }
    default:
    {
      checkedImage = image;
      break;
    }
    }

  checkedImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  checkedImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  ApplyCheckedConstraint( checkedImage, FOREGROUND_DEPTH );
}

void CheckBoxButtonDefaultPainter::SetDisabledCheckedImage( Toolkit::CheckBoxButton& checkBox, Actor image )
{
  Toolkit::Internal::CheckBoxButton& checkBoxImpl = GetImplementation( checkBox );
  Actor& disabledCheckedImage = checkBoxImpl.GetDisabledCheckedImage();
  Actor& fadeOutCheckedImage = checkBoxImpl.GetFadeOutCheckedImage();

  switch( mPaintState )
  {
    case DisabledCheckedState:
    {
      if( disabledCheckedImage && disabledCheckedImage.GetParent() )
      {
        StopCheckOutAnimation( checkBox );
        FadeOutImage( checkBox, Foreground, disabledCheckedImage );

        disabledCheckedImage = image;

        FadeInImage( checkBox, disabledCheckedImage );

        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();
      }
      else
      {
        disabledCheckedImage = image;
        checkBox.Add( disabledCheckedImage );
      }
      break;
    }
    case CheckedDisabledTransition:
    {
      StopCheckInAnimation();
      checkBox.Remove( disabledCheckedImage );

      disabledCheckedImage = image;

      FadeInImage( checkBox, disabledCheckedImage );
      StartCheckInAnimation();
      break;
    }
    case DisabledCheckedTransition:
    {
      float opacity = 1.f;
      if( fadeOutCheckedImage )
      {
        opacity = fadeOutCheckedImage.GetCurrentOpacity();
      }
      StopCheckOutAnimation( checkBox );

      // Replaces the button image.
      disabledCheckedImage = image;

      checkBox.Add( disabledCheckedImage );
      FadeOutImage( checkBox, Foreground, disabledCheckedImage, opacity );

      StartCheckOutAnimation( checkBox );
      break;
    }
    default:
    {
      disabledCheckedImage = image;
      break;
    }
  }

  disabledCheckedImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  disabledCheckedImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  ApplyConstraint( disabledCheckedImage, FOREGROUND_DEPTH );
}

void CheckBoxButtonDefaultPainter::SetDisabledBackgroundImage( Toolkit::CheckBoxButton& checkBox, Actor image )
{
  Toolkit::Internal::CheckBoxButton& checkBoxImpl = GetImplementation( checkBox );
  Actor& disabledBackgroundImage = checkBoxImpl.GetDisabledBackgroundImage();
  Actor& fadeOutBackgroundImage = checkBoxImpl.GetFadeOutBackgroundImage();

  switch( mPaintState )
  {
    case DisabledCheckedState:   // FALLTHROUGH
    case DisabledUncheckedState:
    {
      if( disabledBackgroundImage && disabledBackgroundImage.GetParent() )
      {
        StopCheckOutAnimation( checkBox );
        FadeOutImage( checkBox, Background, disabledBackgroundImage  );

        disabledBackgroundImage = image;

        FadeInImage( checkBox, disabledBackgroundImage );

        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();
      }
      else
      {
        disabledBackgroundImage = image;
        checkBox.Add( disabledBackgroundImage );
      }
      break;
    }
    case UncheckedDisabledTransition: // FALLTHROUGH
    case CheckedDisabledTransition:
    {
      StopCheckInAnimation();
      checkBox.Remove( disabledBackgroundImage );

      disabledBackgroundImage = image;

      FadeInImage( checkBox, disabledBackgroundImage );
      StartCheckInAnimation();
      break;
    }
    case DisabledUncheckedTransition: // FALLTHROUGH
    case DisabledCheckedTransition:
    {
      float opacity = 1.f;
      if( fadeOutBackgroundImage )
      {
        opacity = fadeOutBackgroundImage.GetCurrentOpacity();
      }
      StopCheckOutAnimation( checkBox );

      // Replaces the button image.
      disabledBackgroundImage = image;

      checkBox.Add( disabledBackgroundImage );
      FadeOutImage( checkBox, Background, disabledBackgroundImage, opacity );

      StartCheckOutAnimation( checkBox );
      break;
    }
    default:
    {
      disabledBackgroundImage = image;
      break;
    }
  }

  disabledBackgroundImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  disabledBackgroundImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  ApplyConstraint( disabledBackgroundImage, BACKGROUND_DEPTH );
}

void CheckBoxButtonDefaultPainter::Initialize( Toolkit::Button& button )
{
  Toolkit::Internal::CheckBoxButton& buttonImpl = GetCheckBoxButtonImpl( button );
  Actor& backgroundImage = buttonImpl.GetBackgroundImage();
  Actor& checkedImage = buttonImpl.GetCheckedImage();
  Actor& disabledBackgroundImage = buttonImpl.GetDisabledBackgroundImage();
  Actor& disabledCheckedImage = buttonImpl.GetDisabledCheckedImage();

  Toolkit::CheckBoxButton& checkBox = static_cast<Toolkit::CheckBoxButton&>( button );

  if( backgroundImage )
  {
    SetBackgroundImage( checkBox, backgroundImage );
  }

  if( checkedImage )
  {
    SetCheckedImage( checkBox, checkedImage );
  }

  if( disabledBackgroundImage )
  {
    SetDisabledBackgroundImage( checkBox, disabledBackgroundImage );
  }

  if( disabledCheckedImage )
  {
    SetDisabledCheckedImage( checkBox, disabledCheckedImage );
  }

  SetDisabled( button, mDisabled );
}

void CheckBoxButtonDefaultPainter::SetSize( Toolkit::Button& button, const Vector3& size )
{
  Toolkit::Internal::CheckBoxButton& buttonImpl = GetCheckBoxButtonImpl( button );
  Actor& backgroundImage = buttonImpl.GetBackgroundImage();
  Actor& checkedImage = buttonImpl.GetCheckedImage();
  Actor& disabledBackgroundImage = buttonImpl.GetDisabledBackgroundImage();
  Actor& disabledCheckedImage = buttonImpl.GetDisabledCheckedImage();

  ApplyCheckedConstraint( checkedImage, FOREGROUND_DEPTH );
  ApplyConstraint( backgroundImage, BACKGROUND_DEPTH );
  ApplyConstraint( disabledCheckedImage, FOREGROUND_DEPTH );
  ApplyConstraint( disabledBackgroundImage, BACKGROUND_DEPTH );
}

void CheckBoxButtonDefaultPainter::SetDisabled( Toolkit::Button& button, bool disabled )
{
  mDisabled = disabled;

  Toolkit::Internal::CheckBoxButton& buttonImpl = GetCheckBoxButtonImpl( button );
  Actor& backgroundImage = buttonImpl.GetBackgroundImage();
  Actor& checkedImage = buttonImpl.GetCheckedImage();
  Actor& disabledBackgroundImage = buttonImpl.GetDisabledBackgroundImage();
  Actor& disabledCheckedImage = buttonImpl.GetDisabledCheckedImage();
  Actor& fadeOutCheckedImage = buttonImpl.GetFadeOutCheckedImage();
  Actor& fadeOutBackgroundImage = buttonImpl.GetFadeOutBackgroundImage();

  Toolkit::CheckBoxButton& checkBox = static_cast<Toolkit::CheckBoxButton&>( button );

  switch( mPaintState )
  {
    case UncheckedState:
    {
      if( disabled )
      {
        StopCheckOutAnimation( checkBox );
        FadeOutImage( checkBox, Background, backgroundImage );
        FadeInImage( checkBox, disabledBackgroundImage );
        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = UncheckedDisabledTransition;
      }
      break;
    }
    case CheckedState:
    {
      if( disabled )
      {
        StopCheckOutAnimation( checkBox );
        FadeOutImage( checkBox, Background, backgroundImage );
        FadeOutImage( checkBox, Foreground, checkedImage );
        FadeInImage( checkBox, disabledCheckedImage );
        FadeInImage( checkBox, disabledBackgroundImage );
        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = CheckedDisabledTransition;
      }
      break;
    }
    case DisabledUncheckedState:
    {
      if( !disabled )
      {
        StopCheckOutAnimation( checkBox );
        FadeOutImage( checkBox, Background, disabledBackgroundImage );
        FadeInImage( checkBox, backgroundImage );
        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = DisabledUncheckedTransition;
      }
      break;
    }
    case DisabledCheckedState:
    {
      if( !disabled )
      {
        StopCheckOutAnimation( checkBox );
        FadeOutImage( checkBox, Background, disabledBackgroundImage );
        FadeOutImage( checkBox, Foreground, disabledCheckedImage );
        FadeInImage( checkBox, backgroundImage );
        FadeInImage( checkBox, checkedImage );
        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = DisabledCheckedTransition;
      }
      break;
    }
    case UncheckedCheckedTransition:
    {
      if( disabled )
      {
        float opacity = 1.f;
        if( checkedImage )
        {
          opacity = checkedImage.GetCurrentOpacity();
        }
        StopCheckOutAnimation( checkBox );
        StopCheckInAnimation();

        FadeOutImage( checkBox, Foreground, checkedImage, opacity );
        FadeOutImage( checkBox, Background, backgroundImage );

        FadeInImage( checkBox, disabledCheckedImage );
        FadeInImage( checkBox, disabledBackgroundImage );

        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = CheckedDisabledTransition;
      }
      break;
    }
    case CheckedUncheckedTransition:
    {
      if( disabled )
      {
        float opacity = 1.f;
        if( fadeOutCheckedImage )
        {
          opacity = fadeOutCheckedImage.GetCurrentOpacity();
        }
        StopCheckOutAnimation( checkBox );
        StopCheckInAnimation();

        button.Add( disabledCheckedImage );
        FadeOutImage( checkBox, Foreground, disabledCheckedImage, opacity );
        FadeOutImage( checkBox, Background, backgroundImage );

        FadeInImage( checkBox, disabledBackgroundImage );

        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = UncheckedDisabledTransition;
      }
      break;
    }
    case UncheckedDisabledTransition:
    {
      if( !disabled )
      {
        float opacity = 1.f;
        if( fadeOutBackgroundImage )
        {
          opacity = fadeOutBackgroundImage.GetCurrentOpacity();
        }
        StopCheckOutAnimation( checkBox, false );
        StopCheckInAnimation();

        FadeOutImage( checkBox, Background, disabledBackgroundImage, 1.f - opacity );
        FadeInImage( checkBox, backgroundImage, opacity );

        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = DisabledUncheckedTransition;
      }
      break;
    }
    case DisabledUncheckedTransition:
    {
      if( disabled )
      {
        float opacity = 1.f;
        if( fadeOutBackgroundImage )
        {
          opacity = fadeOutBackgroundImage.GetCurrentOpacity();
        }
        StopCheckOutAnimation( checkBox, false );
        StopCheckInAnimation();

        FadeOutImage( checkBox, Background, backgroundImage, 1.f - opacity );
        FadeInImage( checkBox, disabledBackgroundImage, opacity );

        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = UncheckedDisabledTransition;
      }
      break;
    }
    case CheckedDisabledTransition:
    {
      if( !disabled )
      {
        float opacity = 1.f;
        if( fadeOutBackgroundImage )
        {
          opacity = fadeOutBackgroundImage.GetCurrentOpacity();
        }
        StopCheckOutAnimation( checkBox, false );
        StopCheckInAnimation();

        FadeOutImage( checkBox, Foreground, disabledCheckedImage, 1.f - opacity );
        FadeOutImage( checkBox, Background, disabledBackgroundImage, 1.f - opacity );
        FadeInImage( checkBox, checkedImage, opacity );
        FadeInImage( checkBox, backgroundImage, opacity );

        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = DisabledCheckedTransition;
      }
      break;
    }
    case DisabledCheckedTransition:
    {
      if( disabled )
      {
        float opacity = 1.f;
        if( fadeOutBackgroundImage )
        {
          opacity = fadeOutBackgroundImage.GetCurrentOpacity();
        }
        StopCheckOutAnimation( checkBox, false );
        StopCheckInAnimation();

        FadeOutImage( checkBox, Foreground, checkedImage, 1.f - opacity );
        FadeOutImage( checkBox, Background, backgroundImage, 1.f - opacity );
        FadeInImage( checkBox, disabledCheckedImage, opacity );
        FadeInImage( checkBox, disabledBackgroundImage, opacity );

        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = CheckedDisabledTransition;
      }
      break;
    }
    default:
      break;
  }
}

void CheckBoxButtonDefaultPainter::SetAnimationTime( float animationTime )
{
  mAnimationTime = animationTime;
}

float CheckBoxButtonDefaultPainter::GetAnimationTime() const
{
  return mAnimationTime;
}

void CheckBoxButtonDefaultPainter::Checked( Toolkit::CheckBoxButton& button )
{
  Toolkit::Internal::CheckBoxButton& checkBoxImpl = GetCheckBoxButtonImpl( button );
  Actor& checkedImage = checkBoxImpl.GetCheckedImage();
  Actor& fadeOutCheckedImage = checkBoxImpl.GetFadeOutCheckedImage();

  switch( mPaintState )
  {
    case UncheckedState:
    {
      // Fade in the 'check' actor.
      FadeInImage( button, checkedImage );
      SetupCheckedAnimation( button, checkedImage );    // Animate in the check actor
      StartCheckInAnimation();

      mPaintState = UncheckedCheckedTransition;
      break;
    }
    case CheckedState:
    {
      // Fade out the 'check' actor.
      StopCheckOutAnimation( button );
      FadeOutImage( button, Foreground, checkedImage );
      StartCheckOutAnimation( button );

      mPaintState = UncheckedState;
      break;
    }
    case UncheckedCheckedTransition:
    {
      // Stop fade in and start fade out.
      StopCheckOutAnimation( button );
      StopCheckInAnimation();

      float opacity = 0.f;
      if( checkedImage )
      {
        opacity = checkedImage.GetCurrentOpacity();
      }
      FadeOutImage( button, Foreground, checkedImage, opacity );
      StartCheckOutAnimation( button );

      mPaintState = UncheckedState;
      break;
    }
    case CheckedUncheckedTransition:
    {
      // Stop fade out and start fade in.
      float opacity = 1.f;
      if( fadeOutCheckedImage )
      {
        opacity = fadeOutCheckedImage.GetCurrentOpacity();
      }
      StopCheckOutAnimation( button );

      FadeInImage( button, checkedImage, opacity );
      StartCheckInAnimation();

      mPaintState = UncheckedCheckedTransition;
      break;
    }
    default:
      break;
  }
}

void CheckBoxButtonDefaultPainter::ApplyConstraint( Actor& actor, float depth )
{
  if( actor )
  {
    actor.RemoveConstraints();
    actor.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    actor.SetZ( depth );
  }
}

void CheckBoxButtonDefaultPainter::ApplyCheckedConstraint( Actor& actor, float depth )
{
  if( actor )
  {
    actor.RemoveConstraints();
    actor.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    actor.SetZ( depth );
  }
}

void CheckBoxButtonDefaultPainter::AddToFadeInAnimation( const Actor& actor )
{
  if( !mCheckInAnimation )
  {
    mCheckInAnimation = Dali::Animation::New( mAnimationTime  );
  }

  mCheckInAnimation.OpacityTo( actor, 1.f );

}

void CheckBoxButtonDefaultPainter::StartCheckInAnimation()
{
  if( mCheckInAnimation )
  {
    mCheckInAnimation.FinishedSignal().Connect( this, &CheckBoxButtonDefaultPainter::CheckInAnimationFinished );
    mCheckInAnimation.Play();
  }
}

void CheckBoxButtonDefaultPainter::StopCheckInAnimation()
{
  if( mCheckInAnimation )
  {
    mCheckInAnimation.Clear();
    mCheckInAnimation.Reset();
  }
}

void CheckBoxButtonDefaultPainter::AddToFadeOutAnimation( const Actor& actor )
{
  if( !mCheckOutAnimation )
  {
    mCheckOutAnimation = Dali::Animation::New( mAnimationTime );
  }

  mCheckOutAnimation.OpacityTo( actor, 0.f );
}

void CheckBoxButtonDefaultPainter::StartCheckOutAnimation( Toolkit::CheckBoxButton& checkBox )
{
  if( mCheckOutAnimation )
  {
    Toolkit::Internal::CheckBoxButton& checkBoxImpl = GetCheckBoxButtonImpl( checkBox );
    mButton = &checkBoxImpl;

    mCheckOutAnimation.FinishedSignal().Connect( this, &CheckBoxButtonDefaultPainter::CheckOutAnimationFinished );
    mCheckOutAnimation.Play();
  }
}

void CheckBoxButtonDefaultPainter::StopCheckOutAnimation( Toolkit::CheckBoxButton& checkBox, bool remove )
{
  if( mCheckOutAnimation )
  {
    mCheckOutAnimation.Clear();
    mCheckOutAnimation.Reset();
  }

  Toolkit::Internal::CheckBoxButton& checkBoxImpl = GetCheckBoxButtonImpl( checkBox );
  Actor& fadeOutCheckedImage = checkBoxImpl.GetFadeOutCheckedImage();
  Actor& fadeOutBackgroundImage = checkBoxImpl.GetFadeOutBackgroundImage();

  if( remove )
  {
    if( fadeOutCheckedImage && fadeOutCheckedImage.GetParent() )
    {
      fadeOutCheckedImage.GetParent().Remove( fadeOutCheckedImage );
    }

    if( fadeOutBackgroundImage && fadeOutBackgroundImage.GetParent() )
    {
      fadeOutBackgroundImage.GetParent().Remove( fadeOutBackgroundImage );
    }

    fadeOutCheckedImage.Reset();
    fadeOutBackgroundImage.Reset();
  }
}

void CheckBoxButtonDefaultPainter::FadeInImage( Toolkit::CheckBoxButton& checkBox, Actor& image, float opacity )
{
  if( image )
  {
    if( !image.GetParent() )
    {
      checkBox.Add( image );
    }

    image.SetOpacity( 1.0f );
  }
}

void CheckBoxButtonDefaultPainter::FadeOutImage( Toolkit::CheckBoxButton& checkBox, ImageLayer layer, Actor& image, float opacity )
{
  if( image )
  {
    Toolkit::Internal::CheckBoxButton& checkBoxImpl = GetCheckBoxButtonImpl( checkBox );
    Actor& fadeOutCheckedImage = checkBoxImpl.GetFadeOutCheckedImage();
    Actor& fadeOutBackgroundImage = checkBoxImpl.GetFadeOutBackgroundImage();

    Actor& actorLayer = ( ( Background == layer ) ? fadeOutBackgroundImage : fadeOutCheckedImage );

    actorLayer = image;

    actorLayer.SetOpacity( 0.0f );
  }
}

void CheckBoxButtonDefaultPainter::AddToCheckInAnimation( const Actor& actor )
{
  if( !mCheckInAnimation )
  {
    mCheckInAnimation = Dali::Animation::New( mAnimationTime  );
  }

  // UV anim
  mCheckInAnimation.AnimateTo( Property( mTickUVEffect, mTickUVEffect.GetBottomRightPropertyName() ), Vector2( 1.0f, 1.0f ) );

  // Actor size anim
  Handle handle = actor; // Get rid of const
  mCheckInAnimation.AnimateTo( Property( handle, Actor::SCALE_X ), 1.0f);
}

void CheckBoxButtonDefaultPainter::SetupCheckedAnimation( Toolkit::CheckBoxButton& checkBox, Actor& image )
{
  if( image )
  {
    if( !mTickUVEffect )
    {
      ImageActor imageActor = ImageActor::DownCast( image );
      mTickUVEffect = ImageRegionEffect::New();
      imageActor.SetShaderEffect( mTickUVEffect );
    }

    image.SetScale( Vector3( 0.0f, 1.0f, 1.0f ) );

    mTickUVEffect.SetBottomRight( Vector2( 0.0f, 1.0f ) );

    // Parent
    if( !image.GetParent() )
    {
      checkBox.Add( image );
    }

    AddToCheckInAnimation( image );
  }
}

void CheckBoxButtonDefaultPainter::EndCheckOutAnimation()
{
  switch( mPaintState )
  {
    case UncheckedCheckedTransition:
    {
      mPaintState = CheckedState;
      break;
    }
    case CheckedUncheckedTransition:
    {
      mPaintState = UncheckedState;
      break;
    }
    case UncheckedDisabledTransition:
    {
      mPaintState = DisabledUncheckedState;
      break;
    }
    case DisabledUncheckedTransition:
    {
      mPaintState = UncheckedState;
      break;
    }
    case CheckedDisabledTransition:
    {
      mPaintState = DisabledCheckedState;
      break;
    }
    case DisabledCheckedTransition:
    {
      mPaintState = CheckedState;
      break;
    }
    default:
    {
      break;
    }
  }
}

void CheckBoxButtonDefaultPainter::CheckOutAnimationFinished( Dali::Animation& source )
{
  EndCheckOutAnimation();

  Toolkit::CheckBoxButton handle( mButton->GetOwner() );
  StopCheckOutAnimation( handle );
  mButton = NULL;
}

void CheckBoxButtonDefaultPainter::CheckInAnimationFinished( Dali::Animation& source )
{
  switch( mPaintState )
  {
    case UncheckedCheckedTransition:
    {
      mPaintState = CheckedState;
      break;
    }
    case CheckedUncheckedTransition:
    {
      mPaintState = UncheckedState;
      break;
    }
    case UncheckedDisabledTransition:
    {
      mPaintState = DisabledUncheckedState;
      break;
    }
    case DisabledUncheckedTransition:
    {
      mPaintState = UncheckedState;
      break;
    }
    case CheckedDisabledTransition:
    {
      mPaintState = DisabledCheckedState;
      break;
    }
    case DisabledCheckedTransition:
    {
      mPaintState = CheckedState;
      break;
    }
    default:
    {
      break;
    }
  }

  StopCheckInAnimation();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
