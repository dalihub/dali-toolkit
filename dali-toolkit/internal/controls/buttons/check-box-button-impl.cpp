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

const float DISTANCE_BETWEEN_IMAGE_AND_LABEL( 5.0f );
const float ANIMATION_TIME( 0.26f );  // EFL checkbox tick time

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
: Button()
{
  SetTogglableButton( true );

  SetAnimationTime( ANIMATION_TIME );
}

CheckBoxButton::~CheckBoxButton()
{
  if( mTransitionAnimation )
  {
    mTransitionAnimation.Clear();
  }
}

void CheckBoxButton::OnButtonInitialize()
{
  // Wrap around all children
  Self().SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );
}

void CheckBoxButton::OnLabelSet()
{
  Actor& label = GetLabel();

  if( label )
  {
    label.SetParentOrigin( ParentOrigin::CENTER_LEFT );
    label.SetAnchorPoint( AnchorPoint::CENTER_LEFT );

    if( IsDisabled() && GetDisabledBackgroundImage() )
    {
      label.SetX( GetDisabledBackgroundImage().GetNaturalSize().width + DISTANCE_BETWEEN_IMAGE_AND_LABEL );
    }
    else if ( GetBackgroundImage() )
    {
      label.SetX( GetBackgroundImage().GetNaturalSize().width + DISTANCE_BETWEEN_IMAGE_AND_LABEL );
    }
    else
    {
      label.SetX( DISTANCE_BETWEEN_IMAGE_AND_LABEL );
    }
  }
}

bool CheckBoxButton::OnSelected()
{
  Actor& selectedImage = GetSelectedImage();

  PaintState paintState = GetPaintState();

  switch( paintState )
  {
    case UnselectedState:
    {
      StartTransitionAnimation( selectedImage );
      break;
    }
    case SelectedState:
    {
      RemoveChild( selectedImage );
      break;
    }
    case UnselectedSelectedTransition:
    {
      StopTransitionAnimation( false );
      RemoveChild( selectedImage );
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

bool CheckBoxButton::OnDisabled()
{
  Actor& backgroundImage = GetBackgroundImage();
  Actor& selectedImage = GetSelectedImage();
  Actor& disabledBackgroundImage = GetDisabledBackgroundImage();
  Actor& disabledSelectedImage = GetDisabledSelectedImage();

  PaintState paintState = GetPaintState();

  switch( paintState )
  {
    case UnselectedState:
    {
      RemoveChild( backgroundImage );
      break;
    }
    case SelectedState:
    {
      RemoveChild( backgroundImage );
      RemoveChild( selectedImage );
      break;
    }
    case DisabledUnselectedState:
    {
      RemoveChild( disabledBackgroundImage );
      break;
    }
    case DisabledSelectedState:
    {
      RemoveChild( disabledBackgroundImage );
      RemoveChild( disabledSelectedImage );
      break;
    }
    case UnselectedSelectedTransition:
    {
      StopTransitionAnimation();

      RemoveChild( backgroundImage );
      RemoveChild( selectedImage );
      break;
    }
    default:
    {
      break;
    }
  }

  Actor& label = GetLabel();

  if( label )
  {
    if( IsDisabled() && disabledBackgroundImage)
    {
      label.SetX( disabledBackgroundImage.GetNaturalSize().width + DISTANCE_BETWEEN_IMAGE_AND_LABEL );
    }
    else if( backgroundImage )
    {
      label.SetX( backgroundImage.GetNaturalSize().width + DISTANCE_BETWEEN_IMAGE_AND_LABEL );
    }
    else
    {
      label.SetX( DISTANCE_BETWEEN_IMAGE_AND_LABEL );
    }
  }

  if( mTransitionAnimation )
  {
    return true;
  }

  return false;
}

void CheckBoxButton::StopAllAnimations()
{
  StopTransitionAnimation();
}

void CheckBoxButton::StartTransitionAnimation( Actor& actor )
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

    if( !mTransitionAnimation )
    {
      mTransitionAnimation = Dali::Animation::New( GetAnimationTime()  );
    }

    // UV anim
    mTransitionAnimation.AnimateTo( Property( mTickUVEffect, mTickUVEffect.GetBottomRightPropertyName() ), Vector2( 1.0f, 1.0f ) );

    // Actor size anim
    mTransitionAnimation.AnimateTo( Property( actor, Actor::Property::SCALE_X ), 1.0f );

    mTransitionAnimation.FinishedSignal().Connect( this, &CheckBoxButton::TransitionAnimationFinished );
    mTransitionAnimation.Play();
  }
}

void CheckBoxButton::StopTransitionAnimation( bool remove )
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

void CheckBoxButton::TransitionAnimationFinished( Dali::Animation& source )
{
  StopTransitionAnimation();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
