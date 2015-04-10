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
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>

/**
 * Button states and contents
 *                                         (3) mSelectedContent
 *  (2) mButtonContent                     (2) mSelectedBackgroundContent
 *  (1) mBackgroundContent                 (1) mBackgroundContent
 * < unselected > ----------------------- < selected >
 *       |                OnSelect()            |
 *       | OnDisabled()                         | OnDisabled()
 *       |                                      |
 * < disabled >                           < disabled-selected >
 *  (2) mDisabledContent                   (2) mDisabledSelectedContent
 *  (1) mDisabledBackgroundContent         (1) mDisabledBackgroundContent
 *
 * The drawing order of child actors is as follows.
 *
 *  Top      mLabel
 *   |       mButtonContent / mSelectedContent / mDisabledContent / mDisabledSelectedContent
 *   |       mSelectedBackgroundContent
 * Bottom    mBackgroundContent / mDisabledBackgroundContent
 *
 * Some of contents may be missed.
 * And 2 images - fade-in image and fade-out image - in the same layer can be shown during the transition animation. Fade-in image should be above fade-out image.
 */

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
  // empty handle as we cannot create button (but type registered for clicked signal)
  return BaseHandle();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::Button, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Button, "disabled",                     BOOLEAN, DISABLED                     )
DALI_PROPERTY_REGISTRATION( Button, "auto-repeating",               BOOLEAN, AUTO_REPEATING               )
DALI_PROPERTY_REGISTRATION( Button, "initial-auto-repeating-delay", FLOAT,   INITIAL_AUTO_REPEATING_DELAY )
DALI_PROPERTY_REGISTRATION( Button, "next-auto-repeating-delay",    FLOAT,   NEXT_AUTO_REPEATING_DELAY    )
DALI_PROPERTY_REGISTRATION( Button, "togglable",                    BOOLEAN, TOGGLABLE                    )
DALI_PROPERTY_REGISTRATION( Button, "selected",                     BOOLEAN, SELECTED                     )
DALI_PROPERTY_REGISTRATION( Button, "normal-state-actor",           MAP,     NORMAL_STATE_ACTOR           )
DALI_PROPERTY_REGISTRATION( Button, "selected-state-actor",         MAP,     SELECTED_STATE_ACTOR         )
DALI_PROPERTY_REGISTRATION( Button, "disabled-state-actor",         MAP,     DISABLED_STATE_ACTOR         )
DALI_PROPERTY_REGISTRATION( Button, "label-actor",                  MAP,     LABEL_ACTOR                  )

DALI_SIGNAL_REGISTRATION(   Button, "pressed",                               SIGNAL_PRESSED               )
DALI_SIGNAL_REGISTRATION(   Button, "released",                              SIGNAL_RELEASED              )
DALI_SIGNAL_REGISTRATION(   Button, "clicked",                               SIGNAL_CLICKED               )
DALI_SIGNAL_REGISTRATION(   Button, "state-changed",                         SIGNAL_STATE_CHANGED         )

DALI_ACTION_REGISTRATION(   Button, "button-click",                          ACTION_BUTTON_CLICK          )

DALI_TYPE_REGISTRATION_END()

const unsigned int INITIAL_AUTOREPEATING_DELAY( 0.15f );
const unsigned int NEXT_AUTOREPEATING_DELAY( 0.05f );

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
  mState( ButtonUp ),
  mPaintState( UnselectedState )
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
    unsigned int backgroundIndex;
    unsigned int buttonIndex;

    bool animationStarted = false;

    mDisabled = disabled;

    switch( mPaintState )
    {
      case UnselectedState:
      {
        buttonIndex = FindChildIndex( mLabel );
        InsertChild( buttonIndex, mDisabledContent );

        if( mBackgroundContent )
        {
          backgroundIndex = 1;
        }
        else
        {
          backgroundIndex = 0;
        }

        InsertChild( backgroundIndex, mDisabledBackgroundContent );

        animationStarted = OnDisabled();

        if( animationStarted )
        {
          mPaintState = UnselectedDisabledTransition;
        }
        else
        {
          mPaintState = DisabledUnselectedState;
        }
        break;
      }
      case SelectedState:
      {
        buttonIndex = FindChildIndex( mLabel );
        InsertChild( buttonIndex, mDisabledSelectedContent );

        if( mBackgroundContent )
        {
          backgroundIndex = 1;
        }
        else
        {
          backgroundIndex = 0;
        }

        InsertChild( backgroundIndex, mDisabledBackgroundContent );

        animationStarted = OnDisabled();

        if( animationStarted )
        {
          mPaintState = SelectedDisabledTransition;
        }
        else
        {
          mPaintState = DisabledSelectedState;
        }
        break;
      }
      case DisabledUnselectedState:
      {
        buttonIndex = FindChildIndex( mLabel );
        InsertChild( buttonIndex, mButtonContent );

        if( mDisabledBackgroundContent )
        {
          backgroundIndex = 1;
        }
        else
        {
          backgroundIndex = 0;
        }

        InsertChild( backgroundIndex, mBackgroundContent );

        animationStarted = OnDisabled();

        if( animationStarted )
        {
          mPaintState = DisabledUnselectedTransition;
        }
        else
        {
          mPaintState = UnselectedState;
        }
        break;
      }
      case DisabledSelectedState:
      {
        buttonIndex = FindChildIndex( mLabel );
        InsertChild( buttonIndex, mSelectedContent );

        if( mDisabledBackgroundContent )
        {
          backgroundIndex = 1;
        }
        else
        {
          backgroundIndex = 0;
        }

        InsertChild( backgroundIndex, mSelectedBackgroundContent );
        InsertChild( backgroundIndex, mBackgroundContent );

        animationStarted = OnDisabled();

        if( animationStarted )
        {
          mPaintState = DisabledSelectedTransition;
        }
        else
        {
          mPaintState = SelectedState;
        }
        break;
      }
      case UnselectedSelectedTransition:
      {
        buttonIndex = FindChildIndex( mLabel );
        InsertChild( buttonIndex, mDisabledSelectedContent );

        if( mBackgroundContent )
        {
          backgroundIndex = 1;
        }
        else
        {
          backgroundIndex = 0;
        }

        InsertChild( backgroundIndex, mDisabledBackgroundContent );

        animationStarted = OnDisabled();

        if( animationStarted )
        {
          mPaintState = SelectedDisabledTransition;
        }
        else
        {
          mPaintState = DisabledSelectedState;
        }
        break;
      }
      case SelectedUnselectedTransition:
      {
        buttonIndex = FindChildIndex( mLabel );
        InsertChild( buttonIndex, mDisabledContent );

        if( mBackgroundContent )
        {
          backgroundIndex = 1;
        }
        else
        {
          backgroundIndex = 0;
        }

        InsertChild( backgroundIndex, mDisabledBackgroundContent );

        animationStarted = OnDisabled();

        if( animationStarted )
        {
          mPaintState = UnselectedDisabledTransition;
        }
        else
        {
          mPaintState = DisabledUnselectedState;
        }
        break;
      }
      case UnselectedDisabledTransition:
      {
        animationStarted = OnDisabled();

        if( animationStarted )
        {
          mPaintState = DisabledUnselectedTransition;
        }
        else
        {
          mPaintState = UnselectedState;
        }
        break;
      }
      case DisabledUnselectedTransition:
      {
        animationStarted = OnDisabled();

        if( animationStarted )
        {
          mPaintState = UnselectedDisabledTransition;
        }
        else
        {
          mPaintState = DisabledUnselectedState;
        }
        break;
      }
      case SelectedDisabledTransition:
      {
        animationStarted = OnDisabled();

        if( animationStarted )
        {
          mPaintState = DisabledSelectedTransition;
        }
        else
        {
          mPaintState = SelectedState;
        }
        break;
      }
      case DisabledSelectedTransition:
      {
        animationStarted = OnDisabled();

        if( animationStarted )
        {
          mPaintState = SelectedDisabledTransition;
        }
        else
        {
          mPaintState = DisabledSelectedState;
        }
        break;
      }
    }
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
      SetSelected( false, false );
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
    SetSelected( selected, true );
  }
}

void Button::SetSelected( bool selected, bool emitSignal )
{
  unsigned int buttonIndex, backgroundIndex;
  bool animationStarted = false;

  mSelected = selected;

  switch( mPaintState )
  {
    case UnselectedState:
    {
      buttonIndex = FindChildIndex( mLabel );
      InsertChild( buttonIndex, mSelectedContent );

      if( mBackgroundContent )
      {
        backgroundIndex = 1;
      }
      else
      {
        backgroundIndex = 0;
      }

      InsertChild( backgroundIndex, mSelectedBackgroundContent );

      // Notifies the derived class the button has been selected.
      animationStarted = OnSelected();

      if( animationStarted )
      {
        mPaintState = UnselectedSelectedTransition;
      }
      else
      {
        mPaintState = SelectedState;
      }
      break;
    }
    case SelectedState:
    {
      buttonIndex = FindChildIndex( mLabel );
      InsertChild( buttonIndex, mButtonContent );

      // Notifies the derived class the button has been selected.
      animationStarted = OnSelected();

      if( animationStarted )
      {
        mPaintState = SelectedUnselectedTransition;
      }
      else
      {
        mPaintState = UnselectedState;
      }
      break;
    }
    case UnselectedSelectedTransition:
    {
      // Notifies the derived class the button has been selected.
      animationStarted = OnSelected();

      if( animationStarted )
      {
        mPaintState = SelectedUnselectedTransition;
      }
      else
      {
        mPaintState = UnselectedState;
      }
      break;
    }
    case SelectedUnselectedTransition:
    {
      // Notifies the derived class the button has been selected.
      animationStarted = OnSelected();

      if( animationStarted )
      {
        mPaintState = UnselectedSelectedTransition;
      }
      else
      {
        mPaintState = SelectedState;
      }
      break;
    }
    case DisabledUnselectedTransition:
    {
      buttonIndex = FindChildIndex( mLabel );
      InsertChild( buttonIndex, mSelectedContent );

      if( mDisabledBackgroundContent )
      {
        if(  mBackgroundContent )
        {
          backgroundIndex = 2;
        }
        else
        {
          backgroundIndex = 1;
        }
      }
      else if( mBackgroundContent )
      {
        backgroundIndex = 1;
      }
      else
      {
        backgroundIndex = 0;
      }

      InsertChild( backgroundIndex, mSelectedBackgroundContent );

      // Notifies the derived class the button has been selected.
      animationStarted = OnSelected();

      if( animationStarted )
      {
        mPaintState = UnselectedSelectedTransition;
      }
      else
      {
        mPaintState = SelectedState;
      }
      break;
    }
    case DisabledSelectedTransition:
    {
      buttonIndex = FindChildIndex( mLabel );
      InsertChild( buttonIndex, mButtonContent );

      // Notifies the derived class the button has been selected.
      animationStarted = OnSelected();

      if( animationStarted )
      {
        mPaintState = SelectedUnselectedTransition;
      }
      else
      {
        mPaintState = UnselectedState;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  if( emitSignal )
  {
    Toolkit::Button handle( GetOwner() );

    // Emit signal.
    mStateChangedSignal.Emit( handle );
  }

  RelayoutRequest();
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
  Toolkit::TextLabel textLabel = Toolkit::TextLabel::New( label );
  SetLabel( textLabel );
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
    mLabel.SetPosition( 0.f, 0.f );

    // label should be the top of the button
    Self().Add( mLabel );

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

void Button::SetButtonImage( Actor image )
{
  StopAllAnimations();

  if( mButtonContent && mButtonContent.GetParent() )
  {
    Self().Remove( mButtonContent );
  }

  mButtonContent = image;

  mButtonContent.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mButtonContent.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mButtonContent.SetPosition( 0.f, 0.f );

  if( mPaintState == UnselectedState )
  {
    unsigned int index = FindChildIndex( mLabel );

    Self().Insert( index, mButtonContent );
  }

  OnButtonImageSet();
}

Actor Button::GetButtonImage() const
{
  return mButtonContent;
}

Actor& Button::GetButtonImage()
{
  return mButtonContent;
}

void Button::SetSelectedImage( Actor image )
{
  StopAllAnimations();

  if( mSelectedContent && mSelectedContent.GetParent() )
  {
    Self().Remove( mSelectedContent );
  }

  mSelectedContent = image;

  mSelectedContent.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mSelectedContent.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mSelectedContent.SetPosition( 0.f, 0.f );

  if( mPaintState == SelectedState )
  {
    unsigned int index = FindChildIndex( mLabel );

    Self().Insert( index, mSelectedContent );
  }

  OnSelectedImageSet();
}

Actor Button::GetSelectedImage() const
{
  return mSelectedContent;
}

Actor& Button::GetSelectedImage()
{
  return mSelectedContent;
}

void Button::SetBackgroundImage( Actor image )
{
  StopAllAnimations();

  if( mBackgroundContent && mBackgroundContent.GetParent() )
  {
    Self().Remove( mBackgroundContent );
  }

  mBackgroundContent = image;

  mBackgroundContent.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mBackgroundContent.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mBackgroundContent.SetPosition( 0.f, 0.f );

  if( mPaintState == UnselectedState || mPaintState == SelectedState )
  {
    Self().Insert( 0, mBackgroundContent );
  }

  OnBackgroundImageSet();
}

Actor Button::GetBackgroundImage() const
{
  return mBackgroundContent;
}

Actor& Button::GetBackgroundImage()
{
  return mBackgroundContent;
}

void Button::SetSelectedBackgroundImage( Actor image )
{
  StopAllAnimations();

  if( mSelectedBackgroundContent && mSelectedBackgroundContent.GetParent() )
  {
    Self().Remove( mSelectedBackgroundContent );
  }

  mSelectedBackgroundContent = image;

  mSelectedBackgroundContent.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mSelectedBackgroundContent.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mSelectedBackgroundContent.SetPosition( 0.f, 0.f );

  if( mPaintState == SelectedState )
  {
    if( mBackgroundContent )
    {
      Self().Insert( 1, mSelectedBackgroundContent );
    }
    else
    {
      Self().Insert( 0, mSelectedBackgroundContent );
    }
  }

  OnSelectedBackgroundImageSet();
}

Actor Button::GetSelectedBackgroundImage() const
{
  return mSelectedBackgroundContent;
}

Actor& Button::GetSelectedBackgroundImage()
{
  return mSelectedBackgroundContent;
}

void Button::SetDisabledImage( Actor image )
{
  StopAllAnimations();

  if( mDisabledContent && mDisabledContent.GetParent() )
  {
    Self().Remove( mDisabledContent );
  }

  mDisabledContent = image;

  mDisabledContent.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mDisabledContent.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mDisabledContent.SetPosition( 0.f, 0.f );

  if( mPaintState == DisabledUnselectedState || mPaintState == DisabledSelectedState )
  {
    unsigned int index = FindChildIndex( mLabel );

    Self().Insert( index, mDisabledContent );
  }

  OnDisabledImageSet();
}

Actor Button::GetDisabledImage() const
{
  return mDisabledContent;
}

Actor& Button::GetDisabledImage()
{
  return mDisabledContent;
}

void Button::SetDisabledSelectedImage( Actor image )
{
  StopAllAnimations();

  if( mDisabledSelectedContent && mDisabledSelectedContent.GetParent() )
  {
    Self().Remove( mDisabledSelectedContent );
  }

  mDisabledSelectedContent = image;

  mDisabledSelectedContent.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mDisabledSelectedContent.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mDisabledSelectedContent.SetPosition( 0.f, 0.f );

  if( mPaintState == DisabledSelectedState )
  {
    unsigned int index = FindChildIndex( mLabel );

    Self().Insert( index, mDisabledSelectedContent );
  }

  OnDisabledSelectedImageSet();
}

Actor Button::GetDisabledSelectedImage() const
{
  return mDisabledSelectedContent;
}

Actor& Button::GetDisabledSelectedImage()
{
  return mDisabledSelectedContent;
}

void Button::SetDisabledBackgroundImage( Actor image )
{
  StopAllAnimations();

  if( mDisabledBackgroundContent && mDisabledBackgroundContent.GetParent() )
  {
    Self().Remove( mDisabledBackgroundContent );
  }

  mDisabledBackgroundContent = image;

  mDisabledBackgroundContent.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mDisabledBackgroundContent.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mDisabledBackgroundContent.SetPosition( 0.f, 0.f );

  if( mPaintState == DisabledUnselectedState || mPaintState == DisabledSelectedState )
  {
    Self().Insert( 0, mDisabledBackgroundContent );
  }

  OnDisabledBackgroundImageSet();
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

void Button::UpdatePaintTransitionState()
{
  switch( mPaintState )
  {
    case UnselectedSelectedTransition:
    {
      RemoveChild( mButtonContent );
      mPaintState = SelectedState;
      break;
    }
    case SelectedUnselectedTransition:
    {
      RemoveChild( mSelectedBackgroundContent );
      RemoveChild( mSelectedContent );
      mPaintState = UnselectedState;
      break;
    }
    case UnselectedDisabledTransition:
    {
      RemoveChild( mBackgroundContent );
      RemoveChild( mButtonContent );
      mPaintState = DisabledUnselectedState;
      break;
    }
    case DisabledUnselectedTransition:
    {
      RemoveChild( mDisabledBackgroundContent );
      RemoveChild( mDisabledContent );
      mPaintState = UnselectedState;
      break;
    }
    case SelectedDisabledTransition:
    {
      RemoveChild( mBackgroundContent );
      RemoveChild( mSelectedBackgroundContent );
      RemoveChild( mSelectedContent );
      mPaintState = DisabledSelectedState;
      break;
    }
    case DisabledSelectedTransition:
    {
      RemoveChild( mDisabledBackgroundContent );
      RemoveChild( mDisabledSelectedContent );
      mPaintState = SelectedState;
      break;
    }
    default:
    {
      break;
    }
  }
}

void Button::OnButtonStageDisconnection()
{
  if( ButtonDown == mState )
  {
    if( !mTogglableButton )
    {
      Released();

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

    Pressed();

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
      Released();

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

      Released();

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

  self.SetDrawMode( DrawMode::OVERLAY );
  self.SetKeyboardFocusable( true );
}

void Button::OnActivated()
{
  // When the button is activated, it performs the click action
  PropertyValueContainer attributes;
  DoClickAction( attributes );
}

void Button::OnControlStageDisconnection()
{
  OnButtonStageDisconnection(); // Notification for derived classes.
  mState = ButtonUp;
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

    Pressed();

    Toolkit::Button handle( GetOwner() );

    //Emit signal.
    consumed = mReleasedSignal.Emit( handle );
    consumed |= mClickedSignal.Emit( handle );
    consumed |= mPressedSignal.Emit( handle );
 }

  return consumed;
}

void Button::Pressed()
{
  unsigned int buttonIndex, backgroundIndex;
  bool animationStarted = false;

  switch( mPaintState )
  {
    case UnselectedState:
    {
      buttonIndex = FindChildIndex( mLabel );
      InsertChild( buttonIndex, mSelectedContent );

      if( mBackgroundContent )
      {
        backgroundIndex = 1;
      }
      else
      {
        backgroundIndex = 0;
      }

      InsertChild( backgroundIndex, mSelectedBackgroundContent );

      // Notifies the derived class the button has been pressed.
      animationStarted = OnPressed();

      if( animationStarted )
      {
        mPaintState = UnselectedSelectedTransition;
      }
      else
      {
        mPaintState = SelectedState;
      }
      break;
    }
    case SelectedUnselectedTransition:
    {
      // Notifies the derived class the button has been pressed.
      animationStarted = OnPressed();

      if( animationStarted )
      {
        mPaintState = UnselectedSelectedTransition;
      }
      else
      {
        mPaintState = SelectedState;
      }
      break;
    }
    case DisabledUnselectedTransition:
    {
      buttonIndex = FindChildIndex( mLabel );
      InsertChild( buttonIndex, mSelectedContent );

      if( mDisabledBackgroundContent )
      {
        if(  mBackgroundContent )
        {
          backgroundIndex = 2;
        }
        else
        {
          backgroundIndex = 1;
        }
      }
      else if( mBackgroundContent )
      {
        backgroundIndex = 1;
      }
      else
      {
        backgroundIndex = 0;
      }

      InsertChild( backgroundIndex, mSelectedBackgroundContent );

      // Notifies the derived class the button has been pressed.
      animationStarted = OnPressed();

      if( animationStarted )
      {
        mPaintState = UnselectedSelectedTransition;
      }
      else
      {
        mPaintState = SelectedState;
      }
      break;
    }
    default:
      break;
  }
}

void Button::Released()
{
  unsigned int buttonIndex;
  bool animationStarted = false;

  switch( mPaintState )
  {
    case SelectedState:
    {
      buttonIndex = FindChildIndex( mLabel );
      InsertChild( buttonIndex, mButtonContent );

      // Notifies the derived class the button has been released.
      animationStarted = OnReleased();

      if( animationStarted )
      {
        mPaintState = SelectedUnselectedTransition;
      }
      else
      {
        mPaintState = UnselectedState;
      }
      break;
    }
    case UnselectedSelectedTransition:
    {
      // Notifies the derived class the button has been released.
      animationStarted = OnReleased();

      if( animationStarted )
      {
        mPaintState = SelectedUnselectedTransition;
      }
      else
      {
        mPaintState = UnselectedState;
      }
      break;
    }
    case DisabledSelectedTransition:
    {
      buttonIndex = FindChildIndex( mLabel );
      InsertChild( buttonIndex, mButtonContent );

      // Notifies the derived class the button has been released.
      animationStarted = OnReleased();

      if( animationStarted )
      {
        mPaintState = SelectedUnselectedTransition;
      }
      else
      {
        mPaintState = UnselectedState;
      }
      break;
    }
    default:
    {
      break;
    }
  }
}

Button::ButtonState Button::GetState()
{
  return mState;
}

Button::PaintState Button::GetPaintState()
{
  return mPaintState;
}

void Button::InsertChild( unsigned int index, Actor& actor )
{
  if( actor )
  {
    Self().Insert( index, actor);
  }
}

void Button::RemoveChild( Actor& actor )
{
  if( actor && actor.GetParent() )
  {
    Self().Remove( actor );
  }
}

unsigned int Button::FindChildIndex( Actor& actor )
{
  Actor self = Self();
  unsigned int childrenNum = self.GetChildCount();

  for( unsigned int i = 0; i < childrenNum; i++ )
  {
    Actor child = self.GetChildAt( i );
    if( child == actor )
    {
      return i;
    }
  }

  return childrenNum;
}

void Button::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::Button button = Toolkit::Button::DownCast( Dali::BaseHandle( object ) );

  if ( button )
  {
    switch ( index )
    {
      case Toolkit::Button::Property::DISABLED:
      {
        GetImplementation( button ).SetDisabled( value.Get<bool>() );
        break;
      }

      case Toolkit::Button::Property::AUTO_REPEATING:
      {
        GetImplementation( button ).SetAutoRepeating( value.Get< bool >() );
        break;
      }

      case Toolkit::Button::Property::INITIAL_AUTO_REPEATING_DELAY:
      {
        GetImplementation( button ).SetInitialAutoRepeatingDelay( value.Get< float >() );
        break;
      }

      case Toolkit::Button::Property::NEXT_AUTO_REPEATING_DELAY:
      {
        GetImplementation( button ).SetNextAutoRepeatingDelay( value.Get< float >() );
        break;
      }

      case Toolkit::Button::Property::TOGGLABLE:
      {
        GetImplementation( button ).SetTogglableButton( value.Get< bool >() );
        break;
      }

      case Toolkit::Button::Property::SELECTED:
      {
        GetImplementation( button ).SetSelected( value.Get< bool >() );
        break;
      }

      case Toolkit::Button::Property::NORMAL_STATE_ACTOR:
      {
        GetImplementation( button ).SetButtonImage( Scripting::NewActor( value.Get< Property::Map >() ) );
        break;
      }

      case Toolkit::Button::Property::SELECTED_STATE_ACTOR:
      {
        GetImplementation( button ).SetSelectedImage( Scripting::NewActor( value.Get< Property::Map >() ) );
        break;
      }

      case Toolkit::Button::Property::DISABLED_STATE_ACTOR:
      {
        GetImplementation( button ).SetDisabledImage( Scripting::NewActor( value.Get< Property::Map >() ) );
        break;
      }

      case Toolkit::Button::Property::LABEL_ACTOR:
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
      case Toolkit::Button::Property::DISABLED:
      {
        value = GetImplementation( button ).mDisabled;
        break;
      }

      case Toolkit::Button::Property::AUTO_REPEATING:
      {
        value = GetImplementation( button ).mAutoRepeating;
        break;
      }

      case Toolkit::Button::Property::INITIAL_AUTO_REPEATING_DELAY:
      {
        value = GetImplementation( button ).mInitialAutoRepeatingDelay;
        break;
      }

      case Toolkit::Button::Property::NEXT_AUTO_REPEATING_DELAY:
      {
        value = GetImplementation( button ).mNextAutoRepeatingDelay;
        break;
      }

      case Toolkit::Button::Property::TOGGLABLE:
      {
        value = GetImplementation( button ).mTogglableButton;
        break;
      }

      case Toolkit::Button::Property::SELECTED:
      {
        value = GetImplementation( button ).mSelected;
        break;
      }

      case Toolkit::Button::Property::NORMAL_STATE_ACTOR:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( GetImplementation( button ).mButtonContent, map );
        value = map;
        break;
      }

      case Toolkit::Button::Property::SELECTED_STATE_ACTOR:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( GetImplementation( button ).mSelectedContent, map );
        value = map;
        break;
      }

      case Toolkit::Button::Property::DISABLED_STATE_ACTOR:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( GetImplementation( button ).mDisabledContent, map );
        value = map;
        break;
      }

      case Toolkit::Button::Property::LABEL_ACTOR:
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
