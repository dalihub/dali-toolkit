#ifndef __DALI_TOOLKIT_INTERNAL_BUTTON_H__
#define __DALI_TOOLKIT_INTERNAL_BUTTON_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/timer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/buttons/button.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{

namespace Toolkit
{

class Button;

namespace Internal
{

/**
 * Button is the base class implementation for all buttons.
 */
class Button : public Control
{

protected:

  /**
   * Construct a new Button.
   */
  Button();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Button();

public:

  /**
   * @copydoc Dali::Toolkit::Button::SetDisabled( bool disabled )
   */
  void SetDisabled( bool disabled );

  /**
   * @copydoc Dali::Toolkit::Button::IsDisabled() const
   */
  bool IsDisabled() const;

  /**
   * @copydoc Dali::Toolkit::Button::SetAutoRepeating( bool autoRepeating )
   */
  void SetAutoRepeating( bool autoRepeating );

  /**
   * @copydoc Dali::Toolkit::Button::IsAutoRepeating() const
   */
  bool IsAutoRepeating() const;

  /**
   * @copydoc Dali::Toolkit::Button::SetInitialAutoRepeatingDelay( float initialAutoRepeatingDelay )
   */
  void SetInitialAutoRepeatingDelay( float initialAutoRepeatingDelay );

  /**
   * @copydoc Dali::Toolkit::Button::GetInitialAutoRepeatingDelay() const
   */
  float GetInitialAutoRepeatingDelay() const;

  /**
   * @copydoc Dali::Toolkit::Button::SetNextAutoRepeatingDelay( float nextAutoRepeatingDelay )
   */
  void SetNextAutoRepeatingDelay( float nextAutoRepeatingDelay );

  /**
   * @copydoc Dali::Toolkit::Button::GetNextAutoRepeatingDelay() const
   */
  float GetNextAutoRepeatingDelay() const;

  /**
   * @copydoc Dali::Toolkit::Button::SetTogglableButton( bool togglable )
   */
  void SetTogglableButton( bool togglable );

  /**
   * @copydoc Dali::Toolkit::Button::IsTogglableButton() const
   */
  bool IsTogglableButton() const;

  /**
   * @copydoc Dali::Toolkit::Button::SetSelected( bool selected )
   */
  void SetSelected( bool selected );

  /**
   * @copydoc Dali::Toolkit::Button::IsSelected() const
   */
  bool IsSelected() const;

  /**
   * @copydoc Dali::Toolkit::Button::SetAnimationTime()
   */
  void SetAnimationTime( float animationTime );

  /**
   * @copydoc Dali::Toolkit::Button::GetAnimationTime()
   */
  float GetAnimationTime() const;

  /**
   * @copydoc Dali::Toolkit::Button::SetLabel( const std::string& label )
   */
  void SetLabel( const std::string& label );

  /**
   * @copydoc Dali::Toolkit::Button::SetLabel( Actor label )
   */
  void SetLabel( Actor label );

  /**
   * @copydoc Dali::Toolkit::Button::GetLabel()
   */
  Actor GetLabel() const;

  /**
   * @copydoc Dali::Toolkit::PushButton::SetButtonImage( Actor image )
   */
  void SetButtonImage( Actor image );

  /**
   * @copydoc Dali::Toolkit::PushButton::GetButtonImage()
   */
  Actor GetButtonImage() const;

  /**
   * Internal use only.
   * @return A reference to the button image.
   */
  Actor& GetButtonImage();

  /**
   * @copydoc Dali::Toolkit::PushButton::SetSelectedImage( Actor image )
   */
  void SetSelectedImage( Actor image );

  /**
   * @copydoc Dali::Toolkit::PushButton::GetSelectedImage()
   */
  Actor GetSelectedImage() const;

  /**
   * Internal use only.
   * @return A reference to the selected image.
   */
  Actor& GetSelectedImage();

  /**
   * @copydoc Dali::Toolkit::PushButton::SetBackgroundImage( Actor image )
   */
  void SetBackgroundImage( Actor image );

  /**
   * @copydoc Dali::Toolkit::PushButton::GetBackgroundImage()
   */
  Actor GetBackgroundImage() const;

  /**
   * Internal use only.
   * @return A reference to the background image.
   */
  Actor& GetBackgroundImage();

  /**
   * @copydoc Dali::Toolkit::PushButton::SetSelectedBackgroundImage( Actor image )
   */
  void SetSelectedBackgroundImage( Actor image );

  /**
   * @copydoc Dali::Toolkit::PushButton::GetSelectedBackgroundImage()
   */
  Actor GetSelectedBackgroundImage() const;

  /**
   * Internal use only.
   * @return A reference to the selected background image.
   */
  Actor& GetSelectedBackgroundImage();

  /**
   * @copydoc Dali::Toolkit::PushButton::SetDisabledImage( Actor image )
   */
  void SetDisabledImage( Actor image );

  /**
   * @copydoc Dali::Toolkit::PushButton::GetDisabledImage()
   */
  Actor GetDisabledImage() const;

  /**
   * Internal use only.
   * @return A reference to the disabled button image.
   */
  Actor& GetDisabledImage();

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::SetDisabledSelectedImage( Actor image )
   */
  void SetDisabledSelectedImage( Actor image );

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::GetDisabledSelectedImage()
   */
  Actor GetDisabledSelectedImage() const;

  /**
   * Internal use only.
   * @return A reference to the disabled selected image.
   */
  Actor& GetDisabledSelectedImage();

  /**
   * @copydoc Dali::Toolkit::PushButton::SetDisabledBackgroundImage( Actor image )
   */
  void SetDisabledBackgroundImage( Actor image );

  /**
   * @copydoc Dali::Toolkit::PushButton::GetDisabledBackgroundImage()
   */
  Actor GetDisabledBackgroundImage() const;

  /**
   * Internal use only.
   * @return A reference to the disabled background image.
   */
  Actor& GetDisabledBackgroundImage();

  /**
   * Performs actions as requested using the action name.
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] attributes The attributes with which to perfrom this action.
   * @return true if action has been accepted by this control
   */
  static bool DoAction( BaseObject* object, const std::string& actionName, const PropertyValueContainer& attributes );

protected:

  /**
   * @return A reference to the label actor.
   */
  Actor& GetLabel();

  /**
   * It changes the transition state of the push button.
   */
  void UpdatePaintTransitionState();

private:

  /**
   * Perform the click action to click the button.
   * @param[in] attributes The attributes to perfrom this action.
   */
  void DoClickAction( const PropertyValueContainer& attributes );

  /**
   * This method is called after the button initialization.
   * Could be reimplemented in subclasses to provide specific behaviour.
   */
  virtual void OnButtonInitialize() { }

  /**
   * This method is called when the label is set.
   */
  virtual void OnLabelSet() {}

  /**
   * This method is called when the button image is set
   */
  virtual void OnButtonImageSet() {}

  /**
   * This method is called when the selected image is set
   */
  virtual void OnSelectedImageSet() {}

  /**
   * This method is called when the background image is set
   */
  virtual void OnBackgroundImageSet() {}

  /**
   * This method is called when the selected background image is set
   */
  virtual void OnSelectedBackgroundImageSet() {}

  /**
   * This method is called when the disabled button image is set
   */
  virtual void OnDisabledImageSet() {}

  /**
   * This method is called when the disabled selected image is set
   */
  virtual void OnDisabledSelectedImageSet() {}

  /**
   * This method is called when the disabled background image is set
   */
  virtual void OnDisabledBackgroundImageSet() {}

  /**
   * This method is called from the OnTouchEvent method when the button is down.
   * Could be reimplemented in subclasses to provide specific behaviour.
   */
  virtual void OnButtonDown();

  /**
   * This method is called from the OnTouchEvent method when the button is up.
   * Could be reimplemented in subclasses to provide specific behaviour.
   */
  virtual void OnButtonUp();

  /**
   * This method is called from the OnTouchEvent method when the touch point leaves the boundary of the button or
   * more than one touch points are received.
   * Could be reimplemented in subclasses to provide specific behaviour.
   */
  virtual void OnTouchPointLeave();

  /**
   * This method is called from the OnTouchEvent method when the touch point is interrupted.
   * Could be reimplemented in subclasses to provide specific behaviour.
   */
  virtual void OnTouchPointInterrupted();

  /**
   * This method is called when the button is removed from the stage.
   * Could be reimplemented in subclasses to provide specific behaviour.
   */
  virtual void OnButtonStageDisconnection();

  /**
   * This method is called when the \e selected property is changed.
   * @return true if the transition animation is started.
   */
  virtual bool OnSelected() { return false; }

  /**
   * This method is called when the \e disabled property is changed.
   * @return true if the transition animation is started.
   */
  virtual bool OnDisabled() { return false; }

  /**
   * This method is called when the button is pressed.
   * @return true if the transition animation is started.
   */
  virtual bool OnPressed() { return false; }

  /**
   * This method is called when the button is released.
   * @return true if the transition animation is started.
   */
  virtual bool OnReleased() { return false; }

  /**
   * This method stops all animations
   */
  virtual void StopAllAnimations() {}

public:

  /**
   * @copydoc Dali::Toolkit::PushButton::PressedSignal()
   */
  Toolkit::Button::ButtonSignalType& PressedSignal();

  /**
   * @copydoc Dali::Toolkit::PushButton::ReleasedSignal()
   */
  Toolkit::Button::ButtonSignalType& ReleasedSignal();

  /**
   * @copydoc Dali::Toolkit::Button::ClickedSignal()
   */
  Toolkit::Button::ButtonSignalType& ClickedSignal();

  /**
   * @copydoc Dali::Toolkit::Button::StateChangedSignal()
   */
  Toolkit::Button::ButtonSignalType& StateChangedSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

  // Properties

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index propertyIndex );

protected: // From CustomActorImpl

  /**
   * @copydoc Dali::CustomActorImpl::OnTouchEvent( const TouchEvent& event )
   */
  virtual bool OnTouchEvent( const TouchEvent& event );

private: // From Control

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::Control::OnActivated()
   */
  virtual void OnActivated();

  /**
   * Callback received when the button is disconnected from the stage.
   * It resets the button status.
   */
  void OnControlStageDisconnection();

private:

  /**
   * Handler for tap events.
   * We do not actually do anything when we receive a tap as the button handles tap event through
   * the touch event system itself as it requires more than just tap handling (e.g. leave events).
   * This stops any of our parents receiving a tap gesture when it occurs within our area.
   * @param[in]  actor  The tapped actor.
   * @param[in]  tap    The tap gesture.
   */
  void OnTap(Actor actor, const TapGesture& tap);

  /**
   * Sets up the autorepeating timer.
   * @param[in] delay The delay time in seconds.
   */
  void SetUpTimer( float delay );

  /**
   * Slot called when Dali::Timer::SignalTick signal. Resets the autorepeating timer.
   */
  bool AutoRepeatingSlot();

  /**
   * Sets the button as selected or unselected.
   * @param[in] selected \e selected property value.
   * @param[in] emitSignal Emit a signal if this value is \e true.
   */
  void SetSelected( bool selected, bool emitSignal );

  /**
   * This method is called when the button is pressed.
   */
  void Pressed();

  /**
   * This method is called when the button is released.
   */
  void Released();

protected:

  enum ButtonState
  {
    ButtonUp,                                  ///< The button is up.
    ButtonDown,                                ///< The button is down.
  };

  /**
   * Button paint states.
   */
  enum PaintState
  {
    UnselectedState,              ///< The button is unselected.
    SelectedState,                ///< The button is selected.
    DisabledUnselectedState,      ///< The button is disabled and unselected.
    DisabledSelectedState,        ///< The button is disabled and selected.
    UnselectedSelectedTransition, ///< The button is in transition from unselected to selected.
    SelectedUnselectedTransition, ///< The button is in transition from selected to unselected.
    UnselectedDisabledTransition, ///< The button is in transition from unselected to disabled.
    DisabledUnselectedTransition, ///< The button is in transition from disabled to unselected.
    SelectedDisabledTransition,   ///< The button is in transition from selected to disabled.
    DisabledSelectedTransition    ///< The button is in transition from disabled to selected.
  };

  ButtonState GetState();
  PaintState GetPaintState();

  /**
   * Inserts the actor to the button.
   */
  void InsertChild( unsigned int index, Actor& actor );

  /**
   * Removes the actor from the button.
   */
  void RemoveChild( Actor& actor );

  /**
   * Finds the index of the actor.
   * If the actor doesn't exist, return the last index + 1.
   */
  unsigned int FindChildIndex( Actor& actor );

private:

  // Undefined
  Button( const Button& );

  // Undefined
  Button& operator = ( const Button& );

private:

  // Signals
  Toolkit::Button::ButtonSignalType mPressedSignal;           ///< Signal emitted when the button is pressed.
  Toolkit::Button::ButtonSignalType mReleasedSignal;          ///< Signal emitted when the button is released.
  Toolkit::Button::ButtonSignalType mClickedSignal;           ///< Signal emitted when the button is clicked.
  Toolkit::Button::ButtonSignalType mStateChangedSignal;      ///< Signal emitted when the button's state is changed.

  Timer mAutoRepeatingTimer;                   ///< Timer used to implement the autorepeating property.

  Actor mLabel;                                ///< Stores the button label.

  Actor mButtonContent;                        ///< Stores the unselected content.
  Actor mSelectedContent;                      ///< Stores the selected content.
  Actor mBackgroundContent;                    ///< Stores the background content.
  Actor mSelectedBackgroundContent;            ///< Stores the selected background content.
  Actor mDisabledContent;                      ///< Stores the disabled content.
  Actor mDisabledSelectedContent;              ///< Stores the disabled selected content.
  Actor mDisabledBackgroundContent;            ///< Stores the disabled background content.

  TapGestureDetector mTapDetector;

  bool             mDisabled;                  ///< Stores the disabled property.
  bool             mAutoRepeating;             ///< Stores the autorepeating property.
  bool             mTogglableButton;           ///< Stores the togglable property.
  bool             mSelected;                  ///< Stores the selected state.
  float            mInitialAutoRepeatingDelay; ///< Stores the initial autorepeating delay in seconds.
  float            mNextAutoRepeatingDelay;    ///< Stores the next autorepeating delay in seconds.
  float            mAnimationTime;             ///< The animation time.

  // Actions
  bool             mClickActionPerforming;

  ButtonState      mState;                     ///< Stores the button state.
  PaintState       mPaintState;                ///< Stores the paint state.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::Button& GetImplementation( Toolkit::Button& button )
{
  DALI_ASSERT_ALWAYS( button );

  Dali::RefObject& handle = button.GetImplementation();

  return static_cast<Toolkit::Internal::Button&>( handle );
}

inline const Toolkit::Internal::Button& GetImplementation( const Toolkit::Button& button )
{
  DALI_ASSERT_ALWAYS( button );

  const Dali::RefObject& handle = button.GetImplementation();

  return static_cast<const Toolkit::Internal::Button&>( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_BUTTON_H__
