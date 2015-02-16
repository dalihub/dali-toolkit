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
  mPaintState( UnselectedState ),
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

void CheckBoxButtonDefaultPainter::Initialize( Toolkit::Button& button )
{
  Toolkit::Internal::CheckBoxButton& buttonImpl = GetCheckBoxButtonImpl( button );
  Actor& backgroundImage = buttonImpl.GetBackgroundImage();
  Actor& selectedImage = buttonImpl.GetSelectedImage();
  Actor& disabledBackgroundImage = buttonImpl.GetDisabledBackgroundImage();
  Actor& disabledSelectedImage = buttonImpl.GetDisabledSelectedImage();

  Toolkit::CheckBoxButton& checkBox = static_cast<Toolkit::CheckBoxButton&>( button );

  if( backgroundImage )
  {
    SetBackgroundImage( checkBox, backgroundImage );
  }

  if( selectedImage )
  {
    SetSelectedImage( checkBox, selectedImage );
  }

  if( disabledBackgroundImage )
  {
    SetDisabledBackgroundImage( checkBox, disabledBackgroundImage );
  }

  if( disabledSelectedImage )
  {
    SetDisabledSelectedImage( checkBox, disabledSelectedImage );
  }

  SetDisabled( button, mDisabled );
}

void CheckBoxButtonDefaultPainter::SetSize( Toolkit::Button& button, const Vector3& size )
{
  Toolkit::Internal::CheckBoxButton& buttonImpl = GetCheckBoxButtonImpl( button );
  Actor& backgroundImage = buttonImpl.GetBackgroundImage();
  Actor& selectedImage = buttonImpl.GetSelectedImage();
  Actor& disabledBackgroundImage = buttonImpl.GetDisabledBackgroundImage();
  Actor& disabledSelectedImage = buttonImpl.GetDisabledSelectedImage();

  ApplySelectedConstraint( selectedImage, FOREGROUND_DEPTH );
  ApplyConstraint( backgroundImage, BACKGROUND_DEPTH );
  ApplyConstraint( disabledSelectedImage, FOREGROUND_DEPTH );
  ApplyConstraint( disabledBackgroundImage, BACKGROUND_DEPTH );
}

void CheckBoxButtonDefaultPainter::SetDisabled( Toolkit::Button& button, bool disabled )
{
  mDisabled = disabled;

  Toolkit::Internal::CheckBoxButton& buttonImpl = GetCheckBoxButtonImpl( button );
  Actor& backgroundImage = buttonImpl.GetBackgroundImage();
  Actor& selectedImage = buttonImpl.GetSelectedImage();
  Actor& disabledBackgroundImage = buttonImpl.GetDisabledBackgroundImage();
  Actor& disabledSelectedImage = buttonImpl.GetDisabledSelectedImage();
  Actor& fadeOutSelectedImage = buttonImpl.GetFadeOutSelectedImage();
  Actor& fadeOutBackgroundImage = buttonImpl.GetFadeOutBackgroundImage();

  Toolkit::CheckBoxButton& checkBox = static_cast<Toolkit::CheckBoxButton&>( button );

  switch( mPaintState )
  {
    case UnselectedState:
    {
      if( disabled )
      {
        StopCheckOutAnimation( checkBox );
        FadeOutImage( checkBox, Background, backgroundImage );
        FadeInImage( checkBox, disabledBackgroundImage );
        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = UnselectedDisabledTransition;
      }
      break;
    }
    case SelectedState:
    {
      if( disabled )
      {
        StopCheckOutAnimation( checkBox );
        FadeOutImage( checkBox, Background, backgroundImage );
        FadeOutImage( checkBox, Foreground, selectedImage );
        FadeInImage( checkBox, disabledSelectedImage );
        FadeInImage( checkBox, disabledBackgroundImage );
        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = SelectedDisabledTransition;
      }
      break;
    }
    case DisabledUnselectedState:
    {
      if( !disabled )
      {
        StopCheckOutAnimation( checkBox );
        FadeOutImage( checkBox, Background, disabledBackgroundImage );
        FadeInImage( checkBox, backgroundImage );
        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = DisabledUnselectedTransition;
      }
      break;
    }
    case DisabledSelectedState:
    {
      if( !disabled )
      {
        StopCheckOutAnimation( checkBox );
        FadeOutImage( checkBox, Background, disabledBackgroundImage );
        FadeOutImage( checkBox, Foreground, disabledSelectedImage );
        FadeInImage( checkBox, backgroundImage );
        FadeInImage( checkBox, selectedImage );
        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = DisabledSelectedTransition;
      }
      break;
    }
    case UnselectedSelectedTransition:
    {
      if( disabled )
      {
        float opacity = 1.f;
        if( selectedImage )
        {
          opacity = selectedImage.GetCurrentOpacity();
        }
        StopCheckOutAnimation( checkBox );
        StopCheckInAnimation();

        FadeOutImage( checkBox, Foreground, selectedImage, opacity );
        FadeOutImage( checkBox, Background, backgroundImage );

        FadeInImage( checkBox, disabledSelectedImage );
        FadeInImage( checkBox, disabledBackgroundImage );

        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = SelectedDisabledTransition;
      }
      break;
    }
    case SelectedUnselectedTransition:
    {
      if( disabled )
      {
        float opacity = 1.f;
        if( fadeOutSelectedImage )
        {
          opacity = fadeOutSelectedImage.GetCurrentOpacity();
        }
        StopCheckOutAnimation( checkBox );
        StopCheckInAnimation();

        button.Add( disabledSelectedImage );
        FadeOutImage( checkBox, Foreground, disabledSelectedImage, opacity );
        FadeOutImage( checkBox, Background, backgroundImage );

        FadeInImage( checkBox, disabledBackgroundImage );

        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = UnselectedDisabledTransition;
      }
      break;
    }
    case UnselectedDisabledTransition:
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

        mPaintState = DisabledUnselectedTransition;
      }
      break;
    }
    case DisabledUnselectedTransition:
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

        mPaintState = UnselectedDisabledTransition;
      }
      break;
    }
    case SelectedDisabledTransition:
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

        FadeOutImage( checkBox, Foreground, disabledSelectedImage, 1.f - opacity );
        FadeOutImage( checkBox, Background, disabledBackgroundImage, 1.f - opacity );
        FadeInImage( checkBox, selectedImage, opacity );
        FadeInImage( checkBox, backgroundImage, opacity );

        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = DisabledSelectedTransition;
      }
      break;
    }
    case DisabledSelectedTransition:
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

        FadeOutImage( checkBox, Foreground, selectedImage, 1.f - opacity );
        FadeOutImage( checkBox, Background, backgroundImage, 1.f - opacity );
        FadeInImage( checkBox, disabledSelectedImage, opacity );
        FadeInImage( checkBox, disabledBackgroundImage, opacity );

        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();

        mPaintState = SelectedDisabledTransition;
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

void CheckBoxButtonDefaultPainter::SetSelectedImage( Toolkit::Button& button, Actor image )
{
  Toolkit::Internal::CheckBoxButton& checkBoxImpl = GetCheckBoxButtonImpl( button );
  Actor& selectedImage = checkBoxImpl.GetSelectedImage();
  Actor& fadeOutSelectedImage = checkBoxImpl.GetFadeOutSelectedImage();

  Toolkit::CheckBoxButton& checkBox = static_cast<Toolkit::CheckBoxButton&>( button );

  switch( mPaintState )
  {
    case SelectedState:
    {
      if( selectedImage && selectedImage.GetParent() )
      {
        StopCheckOutAnimation( checkBox );
        FadeOutImage( checkBox, Foreground, selectedImage );

        selectedImage = image;

        FadeInImage( checkBox, selectedImage );

        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();
      }
      else
      {
        selectedImage = image;
        checkBox.Add( selectedImage );
      }
      break;
    }
    case UnselectedSelectedTransition: // FALLTHROUGH
    case DisabledSelectedTransition:
    {
      StopCheckInAnimation();
      checkBox.Remove( selectedImage );

      selectedImage = image;

      FadeInImage( checkBox, selectedImage );
      StartCheckInAnimation();
      break;
    }
    case SelectedUnselectedTransition: // FALLTHROUGH
    case SelectedDisabledTransition:
    {
      float opacity = 1.f;
      if( fadeOutSelectedImage )
      {
        opacity = fadeOutSelectedImage.GetCurrentOpacity();
      }
      StopCheckOutAnimation( checkBox );

      // Replaces the button image.
      selectedImage = image;

      checkBox.Add( selectedImage );
      FadeOutImage( checkBox, Foreground, selectedImage, opacity );

      StartCheckOutAnimation( checkBox );
      break;
    }
    default:
    {
      selectedImage = image;
      break;
    }
    }

  selectedImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  selectedImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  ApplySelectedConstraint( selectedImage, FOREGROUND_DEPTH );
}

void CheckBoxButtonDefaultPainter::SetBackgroundImage( Toolkit::Button& button, Actor image )
{
  Toolkit::Internal::CheckBoxButton& checkBoxImpl = GetCheckBoxButtonImpl( button );
  Actor& backgroundImage = checkBoxImpl.GetBackgroundImage();
  Actor& fadeOutBackgroundImage = checkBoxImpl.GetFadeOutBackgroundImage();

  Toolkit::CheckBoxButton& checkBox = static_cast<Toolkit::CheckBoxButton&>( button );

  switch( mPaintState )
  {
    case UnselectedState:             // FALLTHROUGH
    case SelectedState:
    case UnselectedSelectedTransition:
    case SelectedUnselectedTransition:
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
    case DisabledUnselectedTransition: // FALLTHROUGH
    case DisabledSelectedTransition:
    {
      StopCheckInAnimation();
      checkBox.Remove( backgroundImage );

      backgroundImage = image;

      FadeInImage( checkBox, backgroundImage );
      StartCheckInAnimation();
      break;
    }
    case SelectedDisabledTransition:   // FALLTHROUGH
    case UnselectedDisabledTransition:
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

void CheckBoxButtonDefaultPainter::SetDisabledSelectedImage( Toolkit::Button& button, Actor image )
{
  Toolkit::Internal::CheckBoxButton& checkBoxImpl = GetCheckBoxButtonImpl( button );
  Actor& disabledSelectedImage = checkBoxImpl.GetDisabledSelectedImage();
  Actor& fadeOutSelectedImage = checkBoxImpl.GetFadeOutSelectedImage();

  Toolkit::CheckBoxButton& checkBox = static_cast<Toolkit::CheckBoxButton&>( button );

  switch( mPaintState )
  {
    case DisabledSelectedState:
    {
      if( disabledSelectedImage && disabledSelectedImage.GetParent() )
      {
        StopCheckOutAnimation( checkBox );
        FadeOutImage( checkBox, Foreground, disabledSelectedImage );

        disabledSelectedImage = image;

        FadeInImage( checkBox, disabledSelectedImage );

        StartCheckOutAnimation( checkBox );
        StartCheckInAnimation();
      }
      else
      {
        disabledSelectedImage = image;
        checkBox.Add( disabledSelectedImage );
      }
      break;
    }
    case SelectedDisabledTransition:
    {
      StopCheckInAnimation();
      checkBox.Remove( disabledSelectedImage );

      disabledSelectedImage = image;

      FadeInImage( checkBox, disabledSelectedImage );
      StartCheckInAnimation();
      break;
    }
    case DisabledSelectedTransition:
    {
      float opacity = 1.f;
      if( fadeOutSelectedImage )
      {
        opacity = fadeOutSelectedImage.GetCurrentOpacity();
      }
      StopCheckOutAnimation( checkBox );

      // Replaces the button image.
      disabledSelectedImage = image;

      checkBox.Add( disabledSelectedImage );
      FadeOutImage( checkBox, Foreground, disabledSelectedImage, opacity );

      StartCheckOutAnimation( checkBox );
      break;
    }
    default:
    {
      disabledSelectedImage = image;
      break;
    }
  }

  disabledSelectedImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  disabledSelectedImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  ApplyConstraint( disabledSelectedImage, FOREGROUND_DEPTH );
}

void CheckBoxButtonDefaultPainter::SetDisabledBackgroundImage( Toolkit::Button& button, Actor image )
{
  Toolkit::Internal::CheckBoxButton& checkBoxImpl = GetCheckBoxButtonImpl( button );
  Actor& disabledBackgroundImage = checkBoxImpl.GetDisabledBackgroundImage();
  Actor& fadeOutBackgroundImage = checkBoxImpl.GetFadeOutBackgroundImage();

  Toolkit::CheckBoxButton& checkBox = static_cast<Toolkit::CheckBoxButton&>( button );

  switch( mPaintState )
  {
    case DisabledSelectedState:   // FALLTHROUGH
    case DisabledUnselectedState:
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
    case UnselectedDisabledTransition: // FALLTHROUGH
    case SelectedDisabledTransition:
    {
      StopCheckInAnimation();
      checkBox.Remove( disabledBackgroundImage );

      disabledBackgroundImage = image;

      FadeInImage( checkBox, disabledBackgroundImage );
      StartCheckInAnimation();
      break;
    }
    case DisabledUnselectedTransition: // FALLTHROUGH
    case DisabledSelectedTransition:
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

void CheckBoxButtonDefaultPainter::Selected( Toolkit::Button& button )
{
  Toolkit::Internal::CheckBoxButton& checkBoxImpl = GetCheckBoxButtonImpl( button );
  Actor& selectedImage = checkBoxImpl.GetSelectedImage();
  Actor& fadeOutSelectedImage = checkBoxImpl.GetFadeOutSelectedImage();

  Toolkit::CheckBoxButton& checkBoxButton = static_cast<Toolkit::CheckBoxButton&>( button );

  switch( mPaintState )
  {
    case UnselectedState:
    {
      // Fade in the 'check' actor.
      FadeInImage( checkBoxButton, selectedImage );
      SetupSelectedAnimation( checkBoxButton, selectedImage );    // Animate in the check actor
      StartCheckInAnimation();

      mPaintState = UnselectedSelectedTransition;
      break;
    }
    case SelectedState:
    {
      // Fade out the 'check' actor.
      StopCheckOutAnimation( checkBoxButton );
      FadeOutImage( checkBoxButton, Foreground, selectedImage );
      StartCheckOutAnimation( checkBoxButton );

      mPaintState = UnselectedState;
      break;
    }
    case UnselectedSelectedTransition:
    {
      // Stop fade in and start fade out.
      StopCheckOutAnimation( checkBoxButton );
      StopCheckInAnimation();

      float opacity = 0.f;
      if( selectedImage )
      {
        opacity = selectedImage.GetCurrentOpacity();
      }
      FadeOutImage( checkBoxButton, Foreground, selectedImage, opacity );
      StartCheckOutAnimation( checkBoxButton );

      mPaintState = UnselectedState;
      break;
    }
    case SelectedUnselectedTransition:
    {
      // Stop fade out and start fade in.
      float opacity = 1.f;
      if( fadeOutSelectedImage )
      {
        opacity = fadeOutSelectedImage.GetCurrentOpacity();
      }
      StopCheckOutAnimation( checkBoxButton );

      FadeInImage( checkBoxButton, selectedImage, opacity );
      StartCheckInAnimation();

      mPaintState = UnselectedSelectedTransition;
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

void CheckBoxButtonDefaultPainter::ApplySelectedConstraint( Actor& actor, float depth )
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
  Actor& fadeOutSelectedImage = checkBoxImpl.GetFadeOutSelectedImage();
  Actor& fadeOutBackgroundImage = checkBoxImpl.GetFadeOutBackgroundImage();

  if( remove )
  {
    if( fadeOutSelectedImage && fadeOutSelectedImage.GetParent() )
    {
      fadeOutSelectedImage.GetParent().Remove( fadeOutSelectedImage );
    }

    if( fadeOutBackgroundImage && fadeOutBackgroundImage.GetParent() )
    {
      fadeOutBackgroundImage.GetParent().Remove( fadeOutBackgroundImage );
    }

    fadeOutSelectedImage.Reset();
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
    Actor& fadeOutSelectedImage = checkBoxImpl.GetFadeOutSelectedImage();
    Actor& fadeOutBackgroundImage = checkBoxImpl.GetFadeOutBackgroundImage();

    Actor& actorLayer = ( ( Background == layer ) ? fadeOutBackgroundImage : fadeOutSelectedImage );

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

void CheckBoxButtonDefaultPainter::SetupSelectedAnimation( Toolkit::CheckBoxButton& checkBox, Actor& image )
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
    case UnselectedSelectedTransition:
    {
      mPaintState = SelectedState;
      break;
    }
    case SelectedUnselectedTransition:
    {
      mPaintState = UnselectedState;
      break;
    }
    case UnselectedDisabledTransition:
    {
      mPaintState = DisabledUnselectedState;
      break;
    }
    case DisabledUnselectedTransition:
    {
      mPaintState = UnselectedState;
      break;
    }
    case SelectedDisabledTransition:
    {
      mPaintState = DisabledSelectedState;
      break;
    }
    case DisabledSelectedTransition:
    {
      mPaintState = SelectedState;
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
    case UnselectedSelectedTransition:
    {
      mPaintState = SelectedState;
      break;
    }
    case SelectedUnselectedTransition:
    {
      mPaintState = UnselectedState;
      break;
    }
    case UnselectedDisabledTransition:
    {
      mPaintState = DisabledUnselectedState;
      break;
    }
    case DisabledUnselectedTransition:
    {
      mPaintState = UnselectedState;
      break;
    }
    case SelectedDisabledTransition:
    {
      mPaintState = DisabledSelectedState;
      break;
    }
    case DisabledSelectedTransition:
    {
      mPaintState = SelectedState;
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
