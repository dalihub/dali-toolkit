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
   * @copydoc Dali::Toolkit::Button::SetDimmed( bool dimmed )
   */
  void SetDimmed( bool dimmed );

  /**
   * @copydoc Dali::Toolkit::Button::IsDimmed() const
   */
  bool IsDimmed() const;

  /**
   * @copydoc Dali::Toolkit::Button::SetAnimationTime()
   */
  void SetAnimationTime( float animationTime );

  /**
   * @copydoc Dali::Toolkit::Button::GetAnimationTime()
   */
  float GetAnimationTime() const;

private:

  /**
   * This method is called after the button initialization.
   * Could be reimplemented in subclasses to provide specific behaviour.
   */
  virtual void OnButtonInitialize() { }

  /**
   * This method is called from the OnTouchEvent method when the button is down.
   * Could be reimplemented in subclasses to provide specific behaviour.
   */
  virtual void OnButtonDown() { }

  /**
   * This method is called from the OnTouchEvent method when the button is up.
   * Could be reimplemented in subclasses to provide specific behaviour.
   */
  virtual void OnButtonUp() { }

  /**
   * This method is called from the OnTouchEvent method when the touch point leaves the boundary of the button or
   * more than one touch points are received.
   * Could be reimplemented in subclasses to provide specific behaviour.
   */
  virtual void OnTouchPointLeave() { }

  /**
   * This method is called from the OnTouchEvent method when the touch point is interrupted.
   * Could be reimplemented in subclasses to provide specific behaviour.
   */
  virtual void OnTouchPointInterrupted() { }

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
  virtual void OnButtonStageDisconnection() { }

public:

  /**
   * @copydoc Dali::Toolkit::Button::ClickedSignal()
   */
  Toolkit::Button::ClickedSignalV2& ClickedSignal();

  /**
   * @copydoc Dali::Toolkit::Button::ToggledSignal()
   */
  Toolkit::Button::ToggledSignalV2& ToggledSignal();

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

private:

  // Undefined
  Button( const Button& );

  // Undefined
  Button& operator = ( const Button& );

protected: // Signals

  enum ButtonState
  {
    ButtonUp,                         ///< The button is up.
    ButtonDown,                       ///< The button is down.
  };

  ButtonState      mState;                ///< Stores the button state.

  bool             mDimmed;               ///< Stores the dimmed property.

  ButtonPainterPtr mPainter;              ///< Pointer to a ButtonPainter base class.

  Toolkit::Button::ClickedSignalV2 mClickedSignalV2; ///< Signal emitted when the button is clicked.
  Toolkit::Button::ToggledSignalV2 mToggledSignalV2; ///< Signal emitted when the button is toggled.

  TapGestureDetector mTapDetector;
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

