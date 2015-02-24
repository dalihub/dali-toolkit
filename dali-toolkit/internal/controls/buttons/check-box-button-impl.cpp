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
#include "check-box-button-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/object/type-registry.h>

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

const Vector3 DISTANCE_BETWEEN_IMAGE_AND_LABEL(5.0f, 0.0f, 0.0f);

BaseHandle Create()
{
  return Toolkit::CheckBoxButton::New();
}

TypeRegistration mType( typeid(Toolkit::CheckBoxButton), typeid(Toolkit::Button), Create );

}

Dali::Toolkit::CheckBoxButton CheckBoxButton::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr< CheckBoxButton > internalCheckBoxButton = new CheckBoxButton();

  // Pass ownership to CustomActor
  Dali::Toolkit::CheckBoxButton checkBoxButton( *internalCheckBoxButton );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalCheckBoxButton->Initialize();

  return checkBoxButton;
}

CheckBoxButton::CheckBoxButton()
: Button(),
  mPaintState( UnselectedState )
{
  SetTogglableButton( true );

  SetAnimationTime( ANIMATION_TIME );
}

CheckBoxButton::~CheckBoxButton()
{
  if( mCheckInAnimation )
  {
    mCheckInAnimation.Clear();
  }
}

void CheckBoxButton::SetSelectedImage( Actor image )
{
  Actor& selectedImage = GetSelectedImage();

  switch( mPaintState )
  {
    case SelectedState:
    {
      if( selectedImage && selectedImage.GetParent() )
      {
        Self().Remove( selectedImage );
      }

      selectedImage = image;
      Self().Add( selectedImage );
      break;
    }
    case UnselectedSelectedTransition:
    {
      StopCheckInAnimation();
      Self().Remove( selectedImage );

      selectedImage = image;
      Self().Add( selectedImage );

      mPaintState = SelectedState;
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
  selectedImage.SetZ( FOREGROUND_DEPTH );
}

void CheckBoxButton::SetBackgroundImage( Actor image )
{
  Actor& backgroundImage = GetBackgroundImage();

  switch( mPaintState )
  {
    case UnselectedState:             // FALLTHROUGH
    case SelectedState:
    case UnselectedSelectedTransition:
    {
      if( backgroundImage && backgroundImage.GetParent() )
      {
        Self().Remove( backgroundImage );

        Actor& label = GetLabel();

        if( label )
        {
          backgroundImage.Remove( label );
          image.Add( label );
        }
      }

      backgroundImage = image;
      Self().Add( backgroundImage );
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
  backgroundImage.SetZ( BACKGROUND_DEPTH );
}

void CheckBoxButton::SetDisabledSelectedImage( Actor image )
{
  Actor& disabledSelectedImage = GetDisabledSelectedImage();

  switch( mPaintState )
  {
    case DisabledSelectedState:
    {
      if( disabledSelectedImage && disabledSelectedImage.GetParent() )
      {
        Self().Remove( disabledSelectedImage );
      }

      disabledSelectedImage = image;
      Self().Add( disabledSelectedImage );
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
  disabledSelectedImage.SetZ( FOREGROUND_DEPTH );
}

void CheckBoxButton::SetDisabledBackgroundImage( Actor image )
{
  Actor& disabledBackgroundImage = GetDisabledBackgroundImage();

  switch( mPaintState )
  {
    case DisabledSelectedState:
    case DisabledUnselectedState:
    {
      if( disabledBackgroundImage && disabledBackgroundImage.GetParent() )
      {
        Self().Remove( disabledBackgroundImage );

        Actor& label = GetLabel();

        if( label )
        {
          disabledBackgroundImage.Remove( label );
          image.Add( label );
        }
      }

      disabledBackgroundImage = image;
      Self().Add( disabledBackgroundImage );
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
  disabledBackgroundImage.SetZ( BACKGROUND_DEPTH );
}

void CheckBoxButton::OnLabelSet()
{
  Actor& label = GetLabel();

  if( label )
  {
    label.SetParentOrigin( ParentOrigin::CENTER_RIGHT );
    label.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
    label.MoveBy( DISTANCE_BETWEEN_IMAGE_AND_LABEL );

    if( IsDisabled() && GetDisabledBackgroundImage() )
    {
      GetDisabledBackgroundImage().Add( label );
    }
    else if ( GetBackgroundImage() )
    {
      GetBackgroundImage().Add( label );
    }
    else
    {
      Self().Add( label );
    }
  }
}

void CheckBoxButton::OnSelected( bool selected )
{
  Actor& selectedImage = GetSelectedImage();

  switch( mPaintState )
  {
    case UnselectedState:
    {
      AddChild( selectedImage );
      StartCheckInAnimation( selectedImage );    // Animate in the check actor

      mPaintState = UnselectedSelectedTransition;
      break;
    }
    case SelectedState:
    {
      RemoveChild( selectedImage );

      mPaintState = UnselectedState;
      break;
    }
    case UnselectedSelectedTransition:
    {
      StopCheckInAnimation();
      RemoveChild( selectedImage );

      mPaintState = UnselectedState;
      break;
    }
    default:
      break;
  }
}

void CheckBoxButton::OnDisabled( bool disabled )
{
  Actor& backgroundImage = GetBackgroundImage();
  Actor& selectedImage = GetSelectedImage();
  Actor& disabledBackgroundImage = GetDisabledBackgroundImage();
  Actor& disabledSelectedImage = GetDisabledSelectedImage();

  switch( mPaintState )
  {
    case UnselectedState:
    {
      if( disabled )
      {
        RemoveChild( backgroundImage );
        AddChild( disabledBackgroundImage );
        mPaintState = DisabledUnselectedState;
      }
      break;
    }
    case SelectedState:
    {
      if( disabled )
      {
        RemoveChild( backgroundImage );
        RemoveChild( selectedImage );
        AddChild( disabledBackgroundImage );
        AddChild( disabledSelectedImage );

        mPaintState = DisabledSelectedState;
      }
      break;
    }
    case DisabledUnselectedState:
    {
      if( !disabled )
      {
        RemoveChild( disabledBackgroundImage );
        AddChild( backgroundImage );

        mPaintState = UnselectedState;
      }
      break;
    }
    case DisabledSelectedState:
    {
      if( !disabled )
      {
        RemoveChild( disabledBackgroundImage );
        RemoveChild( disabledSelectedImage );
        AddChild( backgroundImage );
        AddChild( selectedImage );

        mPaintState = SelectedState;
      }
      break;
    }
    case UnselectedSelectedTransition:
    {
      if( disabled )
      {
        StopCheckInAnimation();

        RemoveChild( backgroundImage );
        RemoveChild( selectedImage );
        AddChild( disabledBackgroundImage );
        AddChild( disabledSelectedImage );

        mPaintState = DisabledSelectedState;
      }
      break;
    }
    default:
      break;
  }

  Actor& label = GetLabel();

  if( label )
  {
    if( label.GetParent() )
    {
      label.GetParent().Remove( label );
    }

    if( disabled && disabledBackgroundImage)
    {
      disabledBackgroundImage.Add( label );
    }
    else if( backgroundImage )
    {
      backgroundImage.Add( label );
    }
  }
}

void CheckBoxButton::OnRelayout( const Vector2& size, ActorSizeContainer& container )
{
  Vector3 newSize;

  if( IsDisabled() && GetDisabledBackgroundImage() )
  {
    newSize = GetDisabledBackgroundImage().GetNaturalSize();
  }
  else if( GetBackgroundImage() )
  {
    newSize = GetBackgroundImage().GetNaturalSize();
  }

  Actor& label = GetLabel();

  if( label )
  {
    // Offset the label from the radio button image
    newSize.width += DISTANCE_BETWEEN_IMAGE_AND_LABEL.width;

    // Find the size of the control using size negotiation
    Vector3 actorNaturalSize( label.GetNaturalSize() );
    Control::Relayout( label, Vector2( actorNaturalSize.width, actorNaturalSize.height ), container );

    Vector3 actorSize( label.GetSize() );
    newSize.width += actorSize.width;
    newSize.height = std::max( newSize.height, actorSize.height );
  }

  Self().SetSize( newSize );
}

void CheckBoxButton::AddChild( Actor& actor )
{
  if( actor )
  {
    Self().Add( actor);
  }
}

void CheckBoxButton::RemoveChild( Actor& actor )
{
  if( actor )
  {
    Self().Remove( actor );
  }
}

void CheckBoxButton::StartCheckInAnimation( Actor& actor )
{
  if( actor )
  {
    if( !mTickUVEffect )
    {
      ImageActor imageActor = ImageActor::DownCast( actor );
      mTickUVEffect = ImageRegionEffect::New();
      imageActor.SetShaderEffect( mTickUVEffect );
    }

    actor.SetScale( Vector3( 0.0f, 1.0f, 1.0f ) );

    mTickUVEffect.SetBottomRight( Vector2( 0.0f, 1.0f ) );

    if( !mCheckInAnimation )
    {
      mCheckInAnimation = Dali::Animation::New( GetAnimationTime()  );
    }

    // UV anim
    mCheckInAnimation.AnimateTo( Property( mTickUVEffect, mTickUVEffect.GetBottomRightPropertyName() ), Vector2( 1.0f, 1.0f ) );

    // Actor size anim
    mCheckInAnimation.AnimateTo( Property( actor, Actor::Property::ScaleX ), 1.0f );

    mCheckInAnimation.FinishedSignal().Connect( this, &CheckBoxButton::CheckInAnimationFinished );
    mCheckInAnimation.Play();
  }
}

void CheckBoxButton::StopCheckInAnimation()
{
  if( mCheckInAnimation )
  {
    mCheckInAnimation.Clear();
    mCheckInAnimation.Reset();
  }
}

void CheckBoxButton::CheckInAnimationFinished( Dali::Animation& source )
{
  switch( mPaintState )
  {
    case UnselectedSelectedTransition:
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
