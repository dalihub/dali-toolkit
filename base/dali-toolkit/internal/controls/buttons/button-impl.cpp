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
#include "button-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/object/type-registry.h>

namespace Dali
{

namespace Toolkit
{

const Property::Index Button::PROPERTY_DISABLED                     = Internal::Button::BUTTON_PROPERTY_START_INDEX;
const Property::Index Button::PROPERTY_AUTO_REPEATING               = Internal::Button::BUTTON_PROPERTY_START_INDEX + 1;
const Property::Index Button::PROPERTY_INITIAL_AUTO_REPEATING_DELAY = Internal::Button::BUTTON_PROPERTY_START_INDEX + 2;
const Property::Index Button::PROPERTY_NEXT_AUTO_REPEATING_DELAY    = Internal::Button::BUTTON_PROPERTY_START_INDEX + 3;
const Property::Index Button::PROPERTY_TOGGLABLE                    = Internal::Button::BUTTON_PROPERTY_START_INDEX + 4;
const Property::Index Button::PROPERTY_TOGGLED                      = Internal::Button::BUTTON_PROPERTY_START_INDEX + 5;
const Property::Index Button::PROPERTY_NORMAL_STATE_ACTOR           = Internal::Button::BUTTON_PROPERTY_START_INDEX + 6;
const Property::Index Button::PROPERTY_SELECTED_STATE_ACTOR         = Internal::Button::BUTTON_PROPERTY_START_INDEX + 7;
const Property::Index Button::PROPERTY_DISABLED_STATE_ACTOR         = Internal::Button::BUTTON_PROPERTY_START_INDEX + 8;
const Property::Index Button::PROPERTY_LABEL_ACTOR                  = Internal::Button::BUTTON_PROPERTY_START_INDEX + 9;

namespace Internal
{

namespace
{

BaseHandle Create()
{
  // empty handle as we cannot create button (but type registered for clicked signal)
  return BaseHandle();
}

TypeRegistration typeRegistration( typeid(Toolkit::Button), typeid(Toolkit::Control), Create );

SignalConnectorType signalConnector1( typeRegistration, Toolkit::Button::SIGNAL_CLICKED, &Button::DoConnectSignal );
SignalConnectorType signalConnector2( typeRegistration, Toolkit::Button::SIGNAL_STATE_CHANGED, &Button::DoConnectSignal );

PropertyRegistration property1( typeRegistration, "disabled",                     Toolkit::Button::PROPERTY_DISABLED,                     Property::BOOLEAN, &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property2( typeRegistration, "auto-repeating",               Toolkit::Button::PROPERTY_AUTO_REPEATING,               Property::BOOLEAN, &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property3( typeRegistration, "initial-auto-repeating-delay", Toolkit::Button::PROPERTY_INITIAL_AUTO_REPEATING_DELAY, Property::FLOAT,   &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property4( typeRegistration, "next-auto-repeating-delay",    Toolkit::Button::PROPERTY_NEXT_AUTO_REPEATING_DELAY,    Property::FLOAT,   &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property5( typeRegistration, "togglable",                    Toolkit::Button::PROPERTY_TOGGLABLE,                    Property::BOOLEAN, &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property6( typeRegistration, "toggled",                      Toolkit::Button::PROPERTY_TOGGLED,                      Property::BOOLEAN, &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property7( typeRegistration, "normal-state-actor",           Toolkit::Button::PROPERTY_NORMAL_STATE_ACTOR,           Property::MAP,     &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property8( typeRegistration, "selected-state-actor",         Toolkit::Button::PROPERTY_SELECTED_STATE_ACTOR,         Property::MAP,     &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property9( typeRegistration, "disabled-state-actor",         Toolkit::Button::PROPERTY_DISABLED_STATE_ACTOR,         Property::MAP,     &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property10( typeRegistration, "label-actor",                 Toolkit::Button::PROPERTY_LABEL_ACTOR,                  Property::MAP,     &Button::SetProperty, &Button::GetProperty );

} // unnamed namespace

Button::Button()
: Control( ControlBehaviour( REQUIRES_TOUCH_EVENTS | REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mPainter( NULL ),
  mState( ButtonUp ),
  mDisabled( false )
{
}

Button::~Button()
{
}

void Button::SetDisabled( bool disabled )
{
  mDisabled = disabled;

  // Notifies the painter.
  Toolkit::Button handle( GetOwner() );
  if( mPainter )
  {
    mPainter->SetDisabled( handle, mDisabled );
  }
}

bool Button::IsDisabled() const
{
  return mDisabled;
}

void Button::SetAnimationTime( float animationTime )
{
  OnAnimationTimeSet( animationTime );
}

float Button::GetAnimationTime() const
{
  return OnAnimationTimeRequested();
}

void Button::OnAnimationTimeSet( float animationTime )
{
  // nothing to do.
}

void Button::OnButtonDown()
{

}

void Button::OnButtonUp()
{

}

float Button::OnAnimationTimeRequested() const
{
  return 0.f;
}

Toolkit::Button::ClickedSignalType& Button::ClickedSignal()
{
  return mClickedSignal;
}

Toolkit::Button::StateChangedSignalType& Button::StateChangedSignal()
{
  return mStateChangedSignal;
}

bool Button::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::Button button = Toolkit::Button::DownCast(handle);

  if( Dali::Toolkit::Button::SIGNAL_CLICKED == signalName )
  {
    button.ClickedSignal().Connect( tracker, functor );
  }
  else if( Dali::Toolkit::Button::SIGNAL_STATE_CHANGED == signalName )
  {
    button.StateChangedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

bool Button::OnTouchEvent(const TouchEvent& event)
{
  // Only events are processed when the button is not disabled and the touch event has only
  // one touch point.
  if( ( !mDisabled ) && ( 1 == event.GetPointCount() ) )
  {
    switch( event.GetPoint(0).state )
    {
      case TouchPoint::Down:
      {
        OnButtonDown(); // Notification for derived classes.

        // Sets the button state to ButtonDown.
        mState = ButtonDown;
        break;
      }
      case TouchPoint::Up:
      {
        OnButtonUp(); // Notification for derived classes.

        // Sets the button state to ButtonUp.
        mState = ButtonUp;
        break;
      }
      case TouchPoint::Interrupted:
      {
        OnTouchPointInterrupted(); // Notification for derived classes.

        // Sets the button state to the default (ButtonUp).
        mState = ButtonUp;
        break;
      }
      case TouchPoint::Leave:
      {
        OnTouchPointLeave(); // Notification for derived classes.

        // Sets the button state to the default (ButtonUp).
        mState = ButtonUp;
        break;
      }
      case TouchPoint::Motion:
      case TouchPoint::Stationary: // FALLTHROUGH
      {
        // Nothing to do
        break;
      }
      default:
      {
        DALI_ASSERT_ALWAYS( !"Point status unhandled." );
        break;
      }
    }
  }
  else if( 1 < event.GetPointCount() )
  {
    OnTouchPointLeave(); // Notification for derived classes.

    // Sets the button state to the default (ButtonUp).
    mState = ButtonUp;
  }

  return false;
}

void Button::OnInitialize()
{
  // Initialize the painter and notifies subclasses.
  Toolkit::Button handle( GetOwner() );
  if( mPainter )
  {
    mPainter->Initialize( handle );
  }

  Actor self = Self();

  mTapDetector = TapGestureDetector::New();
  mTapDetector.Attach( self );
  mTapDetector.DetectedSignal().Connect(this, &Button::OnTap);

  OnButtonInitialize();

  self.SetKeyboardFocusable( true );
}

void Button::OnControlSizeSet(const Vector3& targetSize)
{
  Toolkit::Button handle( GetOwner() );
  if( mPainter )
  {
    mPainter->SetSize( handle, targetSize );
  }
}

void Button::OnTap(Actor actor, const TapGesture& tap)
{
  // Do nothing.
}

void Button::OnControlStageDisconnection()
{
  OnButtonStageDisconnection(); // Notification for derived classes.
  mState = ButtonUp;
}

void Button::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::Button button = Toolkit::Button::DownCast( Dali::BaseHandle( object ) );

  if ( button && ( index == Toolkit::Button::PROPERTY_DISABLED ) )
  {
    GetImplementation( button ).SetDisabled( value.Get<bool>() );
  }
}

Property::Value Button::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Toolkit::Button button = Toolkit::Button::DownCast( Dali::BaseHandle( object ) );

  if ( button && ( propertyIndex == Toolkit::Button::PROPERTY_DISABLED ) )
  {
    return Property::Value( GetImplementation( button ).mDisabled );
  }

  return Property::Value();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
