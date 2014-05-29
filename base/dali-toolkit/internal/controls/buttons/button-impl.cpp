//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER

#include "button-impl.h"

namespace
{
const char* const PROPERTY_DIMMED = "dimmed";
} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

const Property::Index Button::PROPERTY_DIMMED( Internal::Button::BUTTON_PROPERTY_START_INDEX );

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

PropertyRegistration property1( typeRegistration, "dimmed", Toolkit::Button::PROPERTY_DIMMED, Property::BOOLEAN, &Button::SetProperty, &Button::GetProperty );

} // unnamed namespace

Button::Button()
: Control( ControlBehaviour( REQUIRES_TOUCH_EVENTS | REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mState( ButtonUp ),
  mDimmed( false ),
  mPainter( NULL )
{
}

Button::~Button()
{
}

void Button::SetDimmed( bool dimmed )
{
  mDimmed = dimmed;

  // Notifies the painter.
  Toolkit::Button handle( GetOwner() );
  if( mPainter )
  {
    mPainter->SetDimmed( handle, mDimmed );
  }
}

bool Button::IsDimmed() const
{
  return mDimmed;
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

float Button::OnAnimationTimeRequested() const
{
  return 0.f;
}

Toolkit::Button::ClickedSignalV2& Button::ClickedSignal()
{
  return mClickedSignalV2;
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
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

bool Button::OnTouchEvent(const TouchEvent& event)
{
  // Only events are processed when the button is not dimmed and the touch event has only
  // one touch point.
  if( ( !mDimmed ) && ( 1 == event.GetPointCount() ) )
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

void Button::OnTap(Actor actor, TapGesture tap)
{
  // Do nothing.
}

void Button::OnStageDisconnection()
{
  if( ButtonUp != mState )
  {
    OnTouchPointLeave(); // Notification for derived classes.
    mState = ButtonUp;
  }
}

void Button::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::Button button = Toolkit::Button::DownCast( Dali::BaseHandle( object ) );

  if ( button && ( index == Toolkit::Button::PROPERTY_DIMMED ) )
  {
    GetImplementation( button ).SetDimmed( value.Get<bool>() );
  }
}

Property::Value Button::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Toolkit::Button button = Toolkit::Button::DownCast( Dali::BaseHandle( object ) );

  if ( button && ( propertyIndex == Toolkit::Button::PROPERTY_DIMMED ) )
  {
    return Property::Value( GetImplementation( button ).mDimmed );
  }

  return Property::Value();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
