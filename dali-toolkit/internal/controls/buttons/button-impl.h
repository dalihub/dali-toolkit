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
#include "button-painter-impl.h"

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
public:

  // Properties
  enum
  {
    BUTTON_PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    BUTTON_PROPERTY_END_INDEX = BUTTON_PROPERTY_START_INDEX + 1000 ///< Reserving 1000 property indices
  };

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
  virtual void SetSelected( bool selected );

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
  virtual void SetLabel( Actor label );   // TODO: After refactoring painter, virtual will be removed

  /**
   * @copydoc Dali::Toolkit::Button::GetLabel()
   */
  Actor GetLabel() const;

  /**
   * Used by the painter only.
   * @return A reference to the label actor.
   */
  Actor& GetLabel();

  /**
   * @copydoc Dali::Toolkit::PushButton::SetButtonImage( const Image image )
   */
  void SetButtonImage( Image image );

  /**
   * @copydoc Dali::Toolkit::PushButton::SetButtonImage( Actor image )
   */
  void SetButtonImage( Actor image );

  /**
   * @copydoc Dali::Toolkit::PushButton::GetButtonImage()
   */
  Actor GetButtonImage() const;

  /**
   * Used by the painter only.
   * @return A reference to the button image.
   */
  Actor& GetButtonImage();

  /**
   * @copydoc Dali::Toolkit::PushButton::SetSelectedImage( const Image image )
   */
  void SetSelectedImage( Image image );

  /**
   * @copydoc Dali::Toolkit::PushButton::SetSelectedImage( Actor image )
   */
  void SetSelectedImage( Actor image );

  /**
   * @copydoc Dali::Toolkit::PushButton::GetSelectedImage()
   */
  Actor GetSelectedImage() const;

  /**
   * Used by the painter only.
   * @return A reference to the selected image.
   */
  Actor& GetSelectedImage();

  /**
   * @copydoc Dali::Toolkit::PushButton::SetBackgroundImage( const Image image )
   */
  void SetBackgroundImage( Image image );

  /**
   * @copydoc Dali::Toolkit::PushButton::SetBackgroundImage( Actor image )
   */
  void SetBackgroundImage( Actor image );

  /**
   * @copydoc Dali::Toolkit::PushButton::GetBackgroundImage()
   */
  Actor GetBackgroundImage() const;

  /**
   * Used by the painter only.
   * @return A reference to the background image.
   */
  Actor& GetBackgroundImage();

  /**
   * @copydoc Dali::Toolkit::PushButton::SetDisabledImage( Image image )
   */
  void SetDisabledImage( Image image );

  /**
   * @copydoc Dali::Toolkit::PushButton::SetDisabledImage( Actor image )
   */
  void SetDisabledImage( Actor image );

  /**
   * @copydoc Dali::Toolkit::PushButton::GetDisabledImage()
   */
  Actor GetDisabledImage() const;

  /**
   * Used by the painter only.
   * @return A reference to the disabled button image.
   */
  Actor& GetDisabledImage();

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::SetDisabledSelectedImage( Image image )
   */
  void SetDisabledSelectedImage( Image image );

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::SetDisabledSelectedImage( Actor image )
   */
  void SetDisabledSelectedImage( Actor image );

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::GetDisabledSelectedImage()
   */
  Actor GetDisabledSelectedImage() const;

  /**
   * Used by the painter only.
   * @return A reference to the disabled selected image.
   */
  Actor& GetDisabledSelectedImage();

  /**
   * @copydoc Dali::Toolkit::PushButton::SetDisabledBackgroundImage( Image image )
   */
  void SetDisabledBackgroundImage( Image image );

  /**
   * @copydoc Dali::Toolkit::PushButton::SetDisabledBackgroundImage( Actor image )
   */
  void SetDisabledBackgroundImage( Actor image );

  /**
   * @copydoc Dali::Toolkit::PushButton::GetDisabledBackgroundImage()
   */
  Actor GetDisabledBackgroundImage() const;

  /**
   * Used by the painter only.
   * @return A reference to the disabled background image.
   */
  Actor& GetDisabledBackgroundImage();

  /**
   * Used by the painter only.
   * @return A reference to the button image that is fading out.
   */
  Actor& GetFadeOutButtonImage();

  /**
   * Used by the painter only.
   * @return A reference to the selected image that is fading out.
   */
  Actor& GetFadeOutSelectedImage();

  /**
   * Used by the painter only.
   * @return A reference to the background image that is fading out.
   */
  Actor& GetFadeOutBackgroundImage();

  /**
   * Performs actions as requested using the action name.
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] attributes The attributes with which to perfrom this action.
   * @return true if action has been accepted by this control
   */
  static bool DoAction( BaseObject* object, const std::string& actionName, const PropertyValueContainer& attributes );

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
   * This method is called when the animation time is set.
   * Needs to be reimplemented in subclasses to set the animation time in different buttons.
   * @param animationTime The animation time in seconds.
   */
  virtual void OnAnimationTimeSet( float animationTime );

  /**
   * This method is called when the animation time is requested.
   * Needs to be reimplemented in subclases to return the animation time.
   * @return The animation time in seconds.
   */
  virtual float OnAnimationTimeRequested() const;

  /**
   * This method is called when the button is removed from the stage.
   * Could be reimplemented in subclasses to provide specific behaviour.
   */
  virtual void OnButtonStageDisconnection();

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
   * @copydoc Toolkit::Control::OnControlSizeSet( const Vector3& targetSize )
   */
  virtual void OnControlSizeSet( const Vector3& targetSize );

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

protected:

  enum ButtonState
  {
    ButtonUp,                                  ///< The button is up.
    ButtonDown,                                ///< The button is down.
  };

  void SetPainter(ButtonPainterPtr painter);

  ButtonState GetState();

  Actor mLabel;                                //TODO: After refactoring painter, this will be private
  bool  mTogglableButton;                      //TODO: After refactoring painter, this will be private
  bool  mSelected;                             //TODO: After refactoring painter, this will be private

private:

  // Undefined
  Button( const Button& );

  // Undefined
  Button& operator = ( const Button& );

private:

  ButtonPainterPtr mPainter;                   ///< Pointer to a ButtonPainter base class.

  // Signals
  Toolkit::Button::ButtonSignalType mPressedSignal;           ///< Signal emitted when the button is pressed.
  Toolkit::Button::ButtonSignalType mReleasedSignal;         ///< Signal emitted when the button is released.
  Toolkit::Button::ButtonSignalType mClickedSignal;           ///< Signal emitted when the button is clicked.
  Toolkit::Button::ButtonSignalType mStateChangedSignal; ///< Signal emitted when the button's state is changed.

  Timer mAutoRepeatingTimer;                   ///< Timer used to implement the autorepeating property.

//  Actor mLabel;                                ///< Stores the button label.

  Actor mButtonImage;                          ///< Stores the unselected image.
  Actor mSelectedImage;                        ///< Stores the selected image.
  Actor mBackgroundImage;                      ///< Stores the background image.
  Actor mDisabledImage;                        ///< Stores the disabled image.
  Actor mDisabledSelectedImage;                ///< Stores the disabled selected image.
  Actor mDisabledBackgroundImage;              ///< Stores the disabled background image.

  Actor mFadeOutButtonImage;                   ///< Stores a foreground image, which is in a fade out animation, to be removed when the animation finishes.
  Actor mFadeOutSelectedImage;                 ///< Stores a foreground image, which is in a fade out animation, to be removed when the animation finishes.
  Actor mFadeOutBackgroundImage;               ///< Stores a background image, which is in a fade out animation, to be removed when the animation finishes.

  TapGestureDetector mTapDetector;

  bool             mDisabled;                  ///< Stores the disabled property.
  bool             mAutoRepeating;             ///< Stores the autorepeating property.
//  bool             mTogglableButton;           ///< Stores the togglable property.
//  bool             mSelected;                  ///< Stores the selected state.
  float            mInitialAutoRepeatingDelay; ///< Stores the initial autorepeating delay in seconds.
  float            mNextAutoRepeatingDelay;    ///< Stores the next autorepeating delay in seconds.

  // Actions
  bool             mClickActionPerforming;

  ButtonState      mState;                     ///< Stores the button state.
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

