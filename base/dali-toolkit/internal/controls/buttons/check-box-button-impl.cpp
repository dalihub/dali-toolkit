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
#include <algorithm>
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include "check-box-button-default-painter-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

BaseHandle Create()
{
  return Toolkit::CheckBoxButton::New();
}

TypeRegistration mType( typeid(Toolkit::CheckBoxButton), typeid(Toolkit::Button), Create );

TypeAction a1(mType, Toolkit::CheckBoxButton::ACTION_CHECK_BOX_BUTTON_CLICK, &CheckBoxButton::DoAction);

}

namespace
{
  // Helper function used to cast a ButtonPainterPtr to CheckBoxButtonDefaultPainterPtr
  CheckBoxButtonDefaultPainterPtr GetCheckBoxButtonPainter( ButtonPainterPtr painter )
  {
    return static_cast<CheckBoxButtonDefaultPainter*>( painter.Get() );
  }
} // namespace

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

void CheckBoxButton::SetChecked( bool checked )
{
  if( !mDimmed && ( checked != mChecked ) )
  {
    // Stores the state.
    mChecked = checked;

    Toolkit::CheckBoxButton handle( GetOwner() );

    // Notifies the painter the checkbox has been checked.
    GetCheckBoxButtonPainter( mPainter )->Checked( handle );

    // Raise toggled signal
    mToggledSignalV2.Emit( handle, mChecked );
  }
}

bool CheckBoxButton::IsChecked() const
{
  return mChecked;
}

void CheckBoxButton::SetBackgroundImage( Image image )
{
  SetBackgroundImage( ImageActor::New( image ) );
}

void CheckBoxButton::SetBackgroundImage( Actor image )
{
  Toolkit::CheckBoxButton handle( GetOwner() );
  GetCheckBoxButtonPainter( mPainter )->SetBackgroundImage( handle, image );
}

Actor& CheckBoxButton::GetBackgroundImage()
{
  return mBackgroundImage;
}

Actor CheckBoxButton::GetBackgroundImage() const
{
  return mBackgroundImage;
}

void CheckBoxButton::SetCheckedImage( Image image )
{
  SetCheckedImage( ImageActor::New( image ) );
}

void CheckBoxButton::SetCheckedImage( Actor image )
{
  Toolkit::CheckBoxButton handle( GetOwner() );
  GetCheckBoxButtonPainter( mPainter )->SetCheckedImage( handle, image );
}

Actor& CheckBoxButton::GetCheckedImage()
{
  return mCheckedImage;
}

Actor CheckBoxButton::GetCheckedImage() const
{
  return mCheckedImage;
}

void CheckBoxButton::SetDimmedBackgroundImage( Image image )
{
  SetDimmedBackgroundImage( ImageActor::New( image ) );
}

void CheckBoxButton::SetDimmedBackgroundImage( Actor image )
{
  Toolkit::CheckBoxButton handle( GetOwner() );
  GetCheckBoxButtonPainter( mPainter )->SetDimmedBackgroundImage( handle, image );
}

Actor& CheckBoxButton::GetDimmedBackgroundImage()
{
  return mDimmedBackgroundImage;
}

Actor CheckBoxButton::GetDimmedBackgroundImage() const
{
  return mDimmedBackgroundImage;
}

void CheckBoxButton::SetDimmedCheckedImage( Image image )
{
  SetDimmedCheckedImage( ImageActor::New( image ) );
}

void CheckBoxButton::SetDimmedCheckedImage( Actor image )
{
  Toolkit::CheckBoxButton handle( GetOwner() );
  GetCheckBoxButtonPainter( mPainter )->SetDimmedCheckedImage( handle, image );
}

Actor& CheckBoxButton::GetDimmedCheckedImage()
{
  return mDimmedCheckedImage;
}

Actor CheckBoxButton::GetDimmedCheckedImage() const
{
  return mDimmedCheckedImage;
}

Actor& CheckBoxButton::GetFadeOutBackgroundImage()
{
  return mFadeOutBackgroundImage;
}

Actor& CheckBoxButton::GetFadeOutCheckedImage()
{
  return mFadeOutCheckedImage;
}

void CheckBoxButton::OnButtonInitialize()
{
  mUseFadeAnimationProperty = Self().RegisterProperty( Toolkit::CheckBoxButton::USE_FADE_ANIMATION_PROPERTY_NAME, false );
  mUseCheckAnimationProperty = Self().RegisterProperty( Toolkit::CheckBoxButton::USE_CHECK_ANIMATION_PROPERTY_NAME, true );
}

void CheckBoxButton::OnButtonUp()
{
  if( ButtonDown == mState )
  {
    // Stores the state, notifies the painter and emits a signal.
    SetChecked( !mChecked );
  }
}

void CheckBoxButton::OnAnimationTimeSet( float animationTime )
{
  GetCheckBoxButtonPainter( mPainter )->SetAnimationTime( animationTime );
}

float CheckBoxButton::OnAnimationTimeRequested() const
{
  return GetCheckBoxButtonPainter( mPainter )->GetAnimationTime();
}

void CheckBoxButton::OnActivated()
{
  // When the button is activated, it performs the click action
  PropertyValueContainer attributes;
  DoClickAction(attributes);
}

void CheckBoxButton::DoClickAction(const PropertyValueContainer& attributes)
{
  // Prevents the button signals from doing a recursive loop by sending an action
  // and re-emitting the signals.
  if(!mClickActionPerforming)
  {
    mClickActionPerforming = true;
    SetChecked( !mChecked );
    mClickActionPerforming = false;
  }
}

bool CheckBoxButton::DoAction(BaseObject* object, const std::string& actionName, const PropertyValueContainer& attributes)
{
  bool ret = false;

  Dali::BaseHandle handle(object);

  Toolkit::CheckBoxButton button = Toolkit::CheckBoxButton::DownCast(handle);

  if(Toolkit::CheckBoxButton::ACTION_CHECK_BOX_BUTTON_CLICK == actionName)
  {
    GetImplementation(button).DoClickAction(attributes);
    ret = true;
  }

  return ret;
}

CheckBoxButton::CheckBoxButton()
: Button(),
  mChecked( false ),
  mClickActionPerforming(false),
  mUseFadeAnimationProperty(Property::INVALID_INDEX),
  mUseCheckAnimationProperty(Property::INVALID_INDEX)
{
  // Creates specific painter.
  mPainter = new CheckBoxButtonDefaultPainter();
}

CheckBoxButton::~CheckBoxButton()
{
  mPainter = NULL;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
