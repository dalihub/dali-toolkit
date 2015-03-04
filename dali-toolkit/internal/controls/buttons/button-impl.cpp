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
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/text-view/text-view.h>

namespace Dali
{

namespace Toolkit
{

const Property::Index Button::PROPERTY_DISABLED                     = Internal::Button::BUTTON_PROPERTY_START_INDEX;
const Property::Index Button::PROPERTY_AUTO_REPEATING               = Internal::Button::BUTTON_PROPERTY_START_INDEX + 1;
const Property::Index Button::PROPERTY_INITIAL_AUTO_REPEATING_DELAY = Internal::Button::BUTTON_PROPERTY_START_INDEX + 2;
const Property::Index Button::PROPERTY_NEXT_AUTO_REPEATING_DELAY    = Internal::Button::BUTTON_PROPERTY_START_INDEX + 3;
const Property::Index Button::PROPERTY_TOGGLABLE                    = Internal::Button::BUTTON_PROPERTY_START_INDEX + 4;
const Property::Index Button::PROPERTY_SELECTED                     = Internal::Button::BUTTON_PROPERTY_START_INDEX + 5;
const Property::Index Button::PROPERTY_NORMAL_STATE_ACTOR           = Internal::Button::BUTTON_PROPERTY_START_INDEX + 6;
const Property::Index Button::PROPERTY_SELECTED_STATE_ACTOR         = Internal::Button::BUTTON_PROPERTY_START_INDEX + 7;
const Property::Index Button::PROPERTY_DISABLED_STATE_ACTOR         = Internal::Button::BUTTON_PROPERTY_START_INDEX + 8;
const Property::Index Button::PROPERTY_LABEL_ACTOR                  = Internal::Button::BUTTON_PROPERTY_START_INDEX + 9;

namespace Internal
{

namespace
{

const unsigned int INITIAL_AUTOREPEATING_DELAY( 0.15f );
const unsigned int NEXT_AUTOREPEATING_DELAY( 0.05f );

// Signals

const char* const SIGNAL_PRESSED =       "pressed";
const char* const SIGNAL_RELEASED =      "released";
const char* const SIGNAL_CLICKED =       "clicked";
const char* const SIGNAL_STATE_CHANGED = "state-changed";

// Actions

const char* const ACTION_BUTTON_CLICK =  "button-click";

BaseHandle Create()
{
  // empty handle as we cannot create button (but type registered for clicked signal)
  return BaseHandle();
}

TypeRegistration typeRegistration( typeid( Toolkit::Button ), typeid( Toolkit::Control ), Create );

SignalConnectorType signalConnector1( typeRegistration, SIGNAL_PRESSED , &Button::DoConnectSignal );
SignalConnectorType signalConnector2( typeRegistration, SIGNAL_RELEASED, &Button::DoConnectSignal );
SignalConnectorType signalConnector3( typeRegistration, SIGNAL_CLICKED, &Button::DoConnectSignal );
SignalConnectorType signalConnector4( typeRegistration, SIGNAL_STATE_CHANGED, &Button::DoConnectSignal );

TypeAction action1( typeRegistration, ACTION_BUTTON_CLICK, &Button::DoAction );

PropertyRegistration property1( typeRegistration, "disabled",                     Toolkit::Button::PROPERTY_DISABLED,                     Property::BOOLEAN, &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property2( typeRegistration, "auto-repeating",               Toolkit::Button::PROPERTY_AUTO_REPEATING,               Property::BOOLEAN, &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property3( typeRegistration, "initial-auto-repeating-delay", Toolkit::Button::PROPERTY_INITIAL_AUTO_REPEATING_DELAY, Property::FLOAT,   &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property4( typeRegistration, "next-auto-repeating-delay",    Toolkit::Button::PROPERTY_NEXT_AUTO_REPEATING_DELAY,    Property::FLOAT,   &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property5( typeRegistration, "togglable",                    Toolkit::Button::PROPERTY_TOGGLABLE,                    Property::BOOLEAN, &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property6( typeRegistration, "selected",                     Toolkit::Button::PROPERTY_SELECTED,                     Property::BOOLEAN, &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property7( typeRegistration, "normal-state-actor",           Toolkit::Button::PROPERTY_NORMAL_STATE_ACTOR,           Property::MAP,     &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property8( typeRegistration, "selected-state-actor",         Toolkit::Button::PROPERTY_SELECTED_STATE_ACTOR,         Property::MAP,     &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property9( typeRegistration, "disabled-state-actor",         Toolkit::Button::PROPERTY_DISABLED_STATE_ACTOR,         Property::MAP,     &Button::SetProperty, &Button::GetProperty );
PropertyRegistration property10( typeRegistration, "label-actor",                 Toolkit::Button::PROPERTY_LABEL_ACTOR,                  Property::MAP,     &Button::SetProperty, &Button::GetProperty );

} // unnamed namespace

Button::Button()
: Control( ControlBehaviour( REQUIRES_TOUCH_EVENTS | REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mAutoRepeatingTimer(),
  mDisabled( false ),
  mAutoRepeating( false ),
  mTogglableButton( false ),
  mSelected( false ),
  mInitialAutoRepeatingDelay( INITIAL_AUTOREPEATING_DELAY ),
  mNextAutoRepeatingDelay( NEXT_AUTOREPEATING_DELAY ),
  mAnimationTime( 0.0f ),
  mClickActionPerforming( false ),
  mState( ButtonUp )
{
}

Button::~Button()
{
  if( mAutoRepeatingTimer )
  {
    mAutoRepeatingTimer.Reset();
  }
}

void Button::SetDisabled( bool disabled )
{
  if( disabled != mDisabled )
  {
    mDisabled = disabled;

    OnDisabled( mDisabled );
  }
}

bool Button::IsDisabled() const
{
  return mDisabled;
}

void Button::SetAutoRepeating( bool autoRepeating )
{
  mAutoRepeating = autoRepeating;

  // An autorepeating button can't be a togglable button.
  if( autoRepeating )
  {
    mTogglableButton = false;

    if( mSelected )
    {
      // Emit a signal is not wanted, only change the appearance.
      OnSelected( false );

      mSelected = false;

      RelayoutRequest();
    }
  }
}

bool Button::IsAutoRepeating() const
{
  return mAutoRepeating;
}

void Button::SetInitialAutoRepeatingDelay( float initialAutoRepeatingDelay )
{
  DALI_ASSERT_ALWAYS( initialAutoRepeatingDelay > 0.f );
  mInitialAutoRepeatingDelay = initialAutoRepeatingDelay;
}

float Button::GetInitialAutoRepeatingDelay() const
{
  return mInitialAutoRepeatingDelay;
}

void Button::SetNextAutoRepeatingDelay( float nextAutoRepeatingDelay )
{
  DALI_ASSERT_ALWAYS( nextAutoRepeatingDelay > 0.f );
  mNextAutoRepeatingDelay = nextAutoRepeatingDelay;
}

float Button::GetNextAutoRepeatingDelay() const
{
  return mNextAutoRepeatingDelay;
}

void Button::SetTogglableButton( bool togglable )
{
  mTogglableButton = togglable;

  // A togglable button can't be an autorepeating button.
  if( togglable )
  {
    mAutoRepeating = false;
  }
}

bool Button::IsTogglableButton() const
{
  return mTogglableButton;
}

void Button::SetSelected( bool selected )
{
  if( !mDisabled && mTogglableButton && ( selected != mSelected ) )
  {
    // Notifies the derived class the button has been selected.
    OnSelected( selected );

    mSelected = selected;

    Toolkit::Button handle( GetOwner() );

    // Emit signal.
    mStateChangedSignal.Emit( handle );

    RelayoutRequest();
  }
}

bool Button::IsSelected() const
{
  return mTogglableButton && mSelected;
}

void Button::SetAnimationTime( float animationTime )
{
  mAnimationTime = animationTime;
}

float Button::GetAnimationTime() const
{
  return mAnimationTime;
}

void Button::SetLabel( const std::string& label )
{
  Toolkit::TextView textView = Toolkit::TextView::New( label );
  textView.SetWidthExceedPolicy( Toolkit::TextView::ShrinkToFit ); // Make sure our text always fits inside the button
  SetLabel( textView );
}

void Button::SetLabel( Actor label )
{
  if( mLabel != label )
  {
    if( mLabel && mLabel.GetParent() )
    {
      mLabel.GetParent().Remove( mLabel );
    }

    mLabel = label;

    OnLabelSet();

    RelayoutRequest();
  }
}

Actor Button::GetLabel() const
{
  return mLabel;
}

Actor& Button::GetLabel()
{
  return mLabel;
}

Actor Button::GetButtonImage() const
{
  return mButtonContent;
}

Actor& Button::GetButtonImage()
{
  return mButtonContent;
}

Actor Button::GetSelectedImage() const
{
  return mSelectedContent;
}

Actor& Button::GetSelectedImage()
{
  return mSelectedContent;
}

Actor Button::GetBackgroundImage() const
{
  return mBackgroundContent;
}

Actor& Button::GetBackgroundImage()
{
  return mBackgroundContent;
}

Actor Button::GetDisabledImage() const
{
  return mDisabledContent;
}

Actor& Button::GetDisabledImage()
{
  return mDisabledContent;
}

Actor Button::GetDisabledSelectedImage() const
{
  return mDisabledSelectedContent;
}

Actor& Button::GetDisabledSelectedImage()
{
  return mDisabledSelectedContent;
}

Actor Button::GetDisabledBackgroundImage() const
{
  return mDisabledBackgroundContent;
}

Actor& Button::GetDisabledBackgroundImage()
{
  return mDisabledBackgroundContent;
}

bool Button::DoAction( BaseObject* object, const std::string& actionName, const PropertyValueContainer& attributes )
{
  bool ret = false;

  Dali::BaseHandle handle( object );

  Toolkit::Button button = Toolkit::Button::DownCast( handle );

  DALI_ASSERT_ALWAYS( button );

  if( 0 == strcmp( actionName.c_str(), ACTION_BUTTON_CLICK ) )
  {
    GetImplementation( button ).DoClickAction( attributes );
    ret = true;
  }

  return ret;
}

void Button::DoClickAction( const PropertyValueContainer& attributes )
{
  // Prevents the button signals from doing a recursive loop by sending an action
  // and re-emitting the signals.
  if( !mClickActionPerforming )
  {
    mClickActionPerforming = true;
    OnButtonDown();
    mState = ButtonDown;
    OnButtonUp();
    mClickActionPerforming = false;
  }
}

void Button::OnButtonStageDisconnection()
{
  if( ButtonDown == mState )
  {
    if( !mTogglableButton )
    {
      Toolkit::Button handle( GetOwner() );

      // Notifies the derived class the button has been released.
      OnReleased();

      if( mAutoRepeating )
      {
        mAutoRepeatingTimer.Reset();
      }
    }
  }
}

void Button::OnButtonDown()
{
  if( !mTogglableButton )
  {
    Toolkit::Button handle( GetOwner() );

    // Notifies the derived class the button has been pressed.
    OnPressed();

    if( mAutoRepeating )
    {
      SetUpTimer( mInitialAutoRepeatingDelay );
    }

    //Emit signal.
    mPressedSignal.Emit( handle );
  }
}

void Button::OnButtonUp()
{
  if( ButtonDown == mState )
  {
    if( mTogglableButton )
    {
      SetSelected( !mSelected );
    }
    else
    {
      // Notifies the derived class the button has been clicked.
      OnReleased();
      OnClicked();

      if( mAutoRepeating )
      {
        mAutoRepeatingTimer.Reset();
      }

      Toolkit::Button handle( GetOwner() );

      //Emit signal.
      mReleasedSignal.Emit( handle );
      mClickedSignal.Emit( handle );
    }
  }
}

void Button::OnTouchPointLeave()
{
  if( ButtonDown == mState )
  {
    if( !mTogglableButton )
    {
      Toolkit::Button handle( GetOwner() );

      // Notifies the derived class the button has been released.
      OnReleased();

      if( mAutoRepeating )
      {
        mAutoRepeatingTimer.Reset();
      }

      //Emit signal.
      mReleasedSignal.Emit( handle );
    }
  }
}

void Button::OnTouchPointInterrupted()
{
  OnTouchPointLeave();
}

Toolkit::Button::ButtonSignalType& Button::PressedSignal()
{
  return mPressedSignal;
}

Toolkit::Button::ButtonSignalType& Button::ReleasedSignal()
{
  return mReleasedSignal;
}

Toolkit::Button::ButtonSignalType& Button::ClickedSignal()
{
  return mClickedSignal;
}

Toolkit::Button::ButtonSignalType& Button::StateChangedSignal()
{
  return mStateChangedSignal;
}

bool Button::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::Button button = Toolkit::Button::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), SIGNAL_PRESSED ) )
  {
    button.PressedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_RELEASED ) )
  {
    button.ReleasedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_CLICKED ) )
  {
    button.ClickedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_STATE_CHANGED ) )
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
  Actor self = Self();

  mTapDetector = TapGestureDetector::New();
  mTapDetector.Attach( self );
  mTapDetector.DetectedSignal().Connect(this, &Button::OnTap);

  OnButtonInitialize();

  self.SetKeyboardFocusable( true );
}

void Button::OnActivated()
{
  // When the button is activated, it performs the click action
  PropertyValueContainer attributes;
  DoClickAction( attributes );
}

void Button::OnTap(Actor actor, const TapGesture& tap)
{
  // Do nothing.
}

void Button::SetUpTimer( float delay )
{
  mAutoRepeatingTimer = Dali::Timer::New( static_cast<unsigned int>( 1000.f * delay ) );
  mAutoRepeatingTimer.TickSignal().Connect( this, &Button::AutoRepeatingSlot );
  mAutoRepeatingTimer.Start();
}

bool Button::AutoRepeatingSlot()
{
  bool consumed = false;
  if( !mDisabled )
  {
    // Restart the autorepeat timer.
    SetUpTimer( mNextAutoRepeatingDelay );

    Toolkit::Button handle( GetOwner() );

    // Notifies the derived class the button has been pressed.
    OnPressed();

    //Emit signal.
    consumed = mReleasedSignal.Emit( handle );
    consumed |= mClickedSignal.Emit( handle );
    consumed |= mPressedSignal.Emit( handle );
 }

  return consumed;
}

void Button::OnControlStageDisconnection()
{
  OnButtonStageDisconnection(); // Notification for derived classes.
  mState = ButtonUp;
}

Button::ButtonState Button::GetState()
{
  return mState;
}

void Button::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::Button button = Toolkit::Button::DownCast( Dali::BaseHandle( object ) );

  if ( button )
  {
    switch ( index )
    {
      case Toolkit::Button::PROPERTY_DISABLED:
      {
        GetImplementation( button ).SetDisabled( value.Get<bool>() );
        break;
      }

      case Toolkit::Button::PROPERTY_AUTO_REPEATING:
      {
        GetImplementation( button ).SetAutoRepeating( value.Get< bool >() );
        break;
      }

      case Toolkit::Button::PROPERTY_INITIAL_AUTO_REPEATING_DELAY:
      {
        GetImplementation( button ).SetInitialAutoRepeatingDelay( value.Get< float >() );
        break;
      }

      case Toolkit::Button::PROPERTY_NEXT_AUTO_REPEATING_DELAY:
      {
        GetImplementation( button ).SetNextAutoRepeatingDelay( value.Get< float >() );
        break;
      }

      case Toolkit::Button::PROPERTY_TOGGLABLE:
      {
        GetImplementation( button ).SetTogglableButton( value.Get< bool >() );
        break;
      }

      case Toolkit::Button::PROPERTY_SELECTED:
      {
        GetImplementation( button ).SetSelected( value.Get< bool >() );
        break;
      }

      case Toolkit::Button::PROPERTY_NORMAL_STATE_ACTOR:
      {
        GetImplementation( button ).SetButtonImage( Scripting::NewActor( value.Get< Property::Map >() ) );
        break;
      }

      case Toolkit::Button::PROPERTY_SELECTED_STATE_ACTOR:
      {
        GetImplementation( button ).SetSelectedImage( Scripting::NewActor( value.Get< Property::Map >() ) );
        break;
      }

      case Toolkit::Button::PROPERTY_DISABLED_STATE_ACTOR:
      {
        GetImplementation( button ).SetDisabledImage( Scripting::NewActor( value.Get< Property::Map >() ) );
        break;
      }

      case Toolkit::Button::PROPERTY_LABEL_ACTOR:
      {
        GetImplementation( button ).SetLabel( Scripting::NewActor( value.Get< Property::Map >() ) );
        break;
      }
    }
  }
}

Property::Value Button::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Toolkit::Button button = Toolkit::Button::DownCast( Dali::BaseHandle( object ) );

  if ( button )
  {
    switch ( propertyIndex )
    {
      case Toolkit::Button::PROPERTY_DISABLED:
      {
        value = GetImplementation( button ).mDisabled;
        break;
      }

      case Toolkit::Button::PROPERTY_AUTO_REPEATING:
      {
        value = GetImplementation( button ).mAutoRepeating;
        break;
      }

      case Toolkit::Button::PROPERTY_INITIAL_AUTO_REPEATING_DELAY:
      {
        value = GetImplementation( button ).mInitialAutoRepeatingDelay;
        break;
      }

      case Toolkit::Button::PROPERTY_NEXT_AUTO_REPEATING_DELAY:
      {
        value = GetImplementation( button ).mNextAutoRepeatingDelay;
        break;
      }

      case Toolkit::Button::PROPERTY_TOGGLABLE:
      {
        value = GetImplementation( button ).mTogglableButton;
        break;
      }

      case Toolkit::Button::PROPERTY_SELECTED:
      {
        value = GetImplementation( button ).mSelected;
        break;
      }

      case Toolkit::Button::PROPERTY_NORMAL_STATE_ACTOR:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( GetImplementation( button ).mButtonContent, map );
        value = map;
        break;
      }

      case Toolkit::Button::PROPERTY_SELECTED_STATE_ACTOR:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( GetImplementation( button ).mSelectedContent, map );
        value = map;
        break;
      }

      case Toolkit::Button::PROPERTY_DISABLED_STATE_ACTOR:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( GetImplementation( button ).mDisabledContent, map );
        value = map;
        break;
      }

      case Toolkit::Button::PROPERTY_LABEL_ACTOR:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( GetImplementation( button ).mLabel, map );
        value = map;
        break;
      }
    }
  }

  return value;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
