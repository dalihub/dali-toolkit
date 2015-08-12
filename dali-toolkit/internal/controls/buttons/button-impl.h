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
#include <dali/public-api/animation/animation.h>

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
   * @copydoc Dali::Toolkit::Button::SetDisabled
   */
  void SetDisabled( bool disabled );

  /**
   * @copydoc Dali::Toolkit::Button::IsDisabled
   */
  bool IsDisabled() const;

  /**
   * @copydoc Dali::Toolkit::Button::SetAutoRepeating
   */
  void SetAutoRepeating( bool autoRepeating );

  /**
   * @copydoc Dali::Toolkit::Button::IsAutoRepeating
   */
  bool IsAutoRepeating() const;

  /**
   * @copydoc Dali::Toolkit::Button::SetInitialAutoRepeatingDelay
   */
  void SetInitialAutoRepeatingDelay( float initialAutoRepeatingDelay );

  /**
   * @copydoc Dali::Toolkit::Button::GetInitialAutoRepeatingDelay
   */
  float GetInitialAutoRepeatingDelay() const;

  /**
   * @copydoc Dali::Toolkit::Button::SetNextAutoRepeatingDelay
   */
  void SetNextAutoRepeatingDelay( float nextAutoRepeatingDelay );

  /**
   * @copydoc Dali::Toolkit::Button::GetNextAutoRepeatingDelay
   */
  float GetNextAutoRepeatingDelay() const;

  /**
   * @copydoc Dali::Toolkit::Button::SetTogglableButton
   */
  void SetTogglableButton( bool togglable );

  /**
   * @copydoc Dali::Toolkit::Button::IsTogglableButton
   */
  bool IsTogglableButton() const;

  /**
   * @copydoc Dali::Toolkit::Button::SetSelected
   */
  void SetSelected( bool selected );

  /**
   * @copydoc Dali::Toolkit::Button::IsSelected
   */
  bool IsSelected() const;

  /**
   * @copydoc Dali::Toolkit::Button::SetAnimationTime
   */
  void SetAnimationTime( float animationTime );

  /**
   * @copydoc Dali::Toolkit::Button::GetAnimationTime
   */
  float GetAnimationTime() const;

  /**
   * @copydoc Dali::Toolkit::Button::SetLabelText
   */
  void SetLabelText( const std::string& label );

  /**
   * @copydoc Dali::Toolkit::Button::GetLabelText
   */
  std::string GetLabelText() const;

  /**
   * @copydoc Dali::Toolkit::PushButton::SetUnselectedImage
   */
  void SetUnselectedImage( const std::string& filename );

  /**
   * @copydoc Dali::Toolkit::PushButton::SetSelectedImage
   */
  void SetSelectedImage( const std::string& filename );

  /**
   * @copydoc Dali::Toolkit::PushButton::SetBackgroundImage
   */
  void SetBackgroundImage( const std::string& filename );

  /**
   * @copydoc Dali::Toolkit::PushButton::SetSelectedBackgroundImage
   */
  void SetSelectedBackgroundImage( const std::string& filename );

  /**
   * @copydoc Dali::Toolkit::PushButton::SetDisabledImage
   */
  void SetDisabledImage( const std::string& filename );

  /**
   * @copydoc Dali::Toolkit::CheckBoxButton::SetDisabledSelectedImage
   */
  void SetDisabledSelectedImage( const std::string& filename );

  /**
   * @copydoc Dali::Toolkit::PushButton::SetDisabledBackgroundImage
   */
  void SetDisabledBackgroundImage( const std::string& filename );

  /**
   * @return The filename used for the button image.
   */
  std::string GetUnselectedImageFilename() const;

  /**
   * @return The filename used for the selected image.
   */
  std::string GetSelectedImageFilename() const;

  /**
   * @return The filename used for the background image.
   */
  std::string GetBackgroundImageFilename() const;

  /**
   * @return The filename used for the selected background image.
   */
  std::string GetSelectedBackgroundImageFilename() const;

  /**
   * @return The filename used for the disabled button image.
   */
  std::string GetDisabledImageFilename() const;

  /**
   * @return The filename used for the disabled selected image.
   */
  std::string GetDisabledSelectedImageFilename() const;

  /**
   * @return The filename used for the disabled background image.
   */
  std::string GetDisabledBackgroundImageFilename() const;

  /**
   * @brief Sets the specified properties on the button label.
   * If the label does not exist yet, it is created.
   * The derived buttons are notified if any properties are changed.
   * @param[in] properties A Property::Map of key-value pairs of properties to set.
   */
  void ModifyLabel( const Property::Map& properties );

  /**
   * Performs actions as requested using the action name.
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] attributes The attributes with which to perfrom this action.
   * @return true if action has been accepted by this control
   */
  static bool DoAction( BaseObject* object, const std::string& actionName, const Property::Map& attributes );

public: // Deprecated API

  /**
   * @copydoc Dali::Toolkit::Button::SetLabel( Actor label )
   */
  void SetLabel( Actor label );

  /**
   * @deprecated Sets the unselected image with an Actor.
   * @param[in] image The Actor to use.
   */
  void SetButtonImage( Actor image );

  /**
   * @deprecated Sets the selected image with an Actor.
   * @param[in] image The Actor to use.
   */
  void SetSelectedImage( Actor image );

  /**
   * @deprecated Sets the background image with an Actor.
   * @param[in] image The Actor to use.
   */
  void SetBackgroundImage( Actor image );

  /**
   * @deprecated Sets the selected background image with an Actor.
   * @param[in] image The Actor to use.
   */
  void SetSelectedBackgroundImage( Actor image );

  /**
   * @deprecated Sets the disabled image with an Actor.
   * @param[in] image The Actor to use.
   */
  void SetDisabledImage( Actor image );

  /**
   * @deprecated Sets the disabled selected image with an Actor.
   * @param[in] image The Actor to use.
   */
  void SetDisabledSelectedImage( Actor image );

  /**
   * @deprecated Sets the disabled background image with an Actor.
   * @param[in] image The Actor to use.
   */
  void SetDisabledBackgroundImage( Actor image );

  /**
   * @copydoc Dali::Toolkit::Button::GetButtonImage
   */
  Actor GetButtonImage() const;

  /**
   * @copydoc Dali::Toolkit::Button::GetSelectedImage
   */
  Actor GetSelectedImage() const;

protected:

  /**
   * @return A reference to the label actor.
   */
  Actor& GetLabelActor();

  /**
   * @return A reference to the unselected button image.
   */
  Actor& GetUnselectedImage();

  /**
   * @return A reference to the selected image.
   */
  Actor& GetSelectedImage();

  /**
   * @return A reference to the background image.
   */
  Actor& GetBackgroundImage();

  /**
   * @return A reference to the selected background image.
   */
  Actor& GetSelectedBackgroundImage();

  /**
   * @return A reference to the disabled button image.
   */
  Actor& GetDisabledImage();

  /**
   * @return A reference to the disabled selected image.
   */
  Actor& GetDisabledSelectedImage();

  /**
   * @return A reference to the disabled background image.
   */
  Actor& GetDisabledBackgroundImage();

private:

  /**
   * Perform the click action to click the button.
   * @param[in] attributes The attributes to perfrom this action.
   * @return true if this control can perform action.
   */
  bool DoClickAction( const Property::Map& attributes );

  /**
   * This method is called after the button initialization.
   * Could be reimplemented in subclasses to provide specific behaviour.
   */
  virtual void OnButtonInitialize() { }

  /**
   * This method is called when the label is set.
   * @param[in] noPadding Used to bypass padding if the label is to be treated generically.
   */
  virtual void OnLabelSet( bool noPadding ) {}

  /**
   * This method is called when the unselected button image is set
   */
  virtual void OnUnselectedImageSet() {}

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
   */
  virtual void OnSelected() {}

  /**
   * This method is called when the \e disabled property is changed.
   */
  virtual void OnDisabled() {}

  /**
   * This method is called when the button is pressed.
   */
  virtual void OnPressed() {}

  /**
   * This method is called when the button is released.
   */
  virtual void OnReleased() {}

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
   * @copydoc Toolkit::Control::OnAccessibilityActivated()
   */
  virtual bool OnAccessibilityActivated();

  /**
   * @copydoc Toolkit::Control::OnKeyboardEnter()
   */
  virtual bool OnKeyboardEnter();

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

  /**
   * Used to perform common setup applied to images within button.
   * This will replace the current image with the specifed one.
   * @param[in]  actorToModify The image to replace.
   * @param[out] newActor The new image to use.
   */
  void SetupContent( Actor& actorToModify, Actor newActor );

  /**
   * Sets the color of the unselected image.
   * If no image exists, it is created.
   * @param[in]  color The color to use.
   */
  void SetUnselectedColor( const Vector4& color );

  /**
   * Gets the unselected content color.
   * @return     The currently used unselected color.
   */
  const Vector4 GetUnselectedColor() const;

  /**
   * Sets the color of the selected image.
   * If no image exists, it is created.
   * @param[in]  color The color to use.
   */
  void SetSelectedColor( const Vector4& color );

  /**
   * Gets the selected content color.
   * @return     The currently used selected color.
   */
  const Vector4 GetSelectedColor() const;

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
  };

  /**
   * Enum to specify which decoration when getting and setting decorations.
   */
  enum DecorationState
  {
    UNSELECTED_DECORATION = 0,
    SELECTED_DECORATION,
    DECORATION_STATES
  };

  ButtonState GetState();
  PaintState GetPaintState();
  void SetDecoration( DecorationState state, Actor actor );
  Actor& GetDecoration( DecorationState state );


  /**
   * Returns the animation to be used for transitioning creating the animation if needed.
   * @return The initialised transition animation.
   */
  Dali::Animation GetTransitionAnimation();

  /**
   * Prepares the actor to be transitioned in.
   * @param[in]  actor  The actor that will be transitioned in.
   */
  virtual void PrepareForTranstionIn( Actor actor ) {}

  /**
   * Prepares the actor to be transitioned in.
   * @param[in]  actor  The actor that will be transitioned out.
   */
  virtual void PrepareForTranstionOut( Actor actor ) {}

  /**
   * Transitions the actor in, allowing derived classes to configure
   * the GetTransitionAnimation() animation ready.
   * Button is in charge of calling Dali::Animation::Play and so derived classes
   * only need to add the animation.
   */
  virtual void OnTransitionIn( Actor actor ) {}

  /**
   * Transitions the actor out, allowing derived classes to configure
   * the GetTransitionAnimation() animation ready.
   * Button is in charge of calling Dali::Animation::Play and so derived classes
   * only need to add the animation.
   */
  virtual void OnTransitionOut( Actor actor ) {}

private:

  /**
   * Starts the transition animation.
   * Button::TransitionFinished is called when the animation finishes.
   */
  void StartTransitionAnimation();

  /**
   * This method stops all transition animations
   */
  void StopTransitionAnimation();

  /**
   * Called when the transition animation finishes.
   */
  void TransitionAnimationFinished( Dali::Animation& source );

  /**
   * Resets the Button to the base state for the current paint state.
   * Any additionally inserted images needed for transitions that are
   * no longer needed and the removed.
   */
  void ResetImageLayers();

  /**
   * Transitions out the actor
   */
  void TransitionOut( Actor actor );

  /**
   * Removes the actor from the button and prepares it to be transitioned out
   */
  void RemoveButtonImage( Actor& actor );

  /**
   * Finds the index of the actor.
   * If the actor doesn't exist, return the last index + 1.
   */
  unsigned int FindChildIndex( Actor& actor );

  /**
   * Adds an actor to the hierarchy and prepares it to be transitioned.
   * @param[in] actor The actor to add
   */
  void PrepareAddButtonImage( Actor& actor );

  /**
   * Adds an actor to the hierarchy and marks it to be transitioned.
   * @param[in] actor The actor to add
   */
  void TransitionButtonImage( Actor& actor );

  /**
   * Adds an actor to the hierarchy.
   * @param[in] actor The actor to add
   */
  void AddButtonImage( Actor& actor );

  /**
   * (Re)Adds the label (if exists) to the hierarchy (so it is always on top).
   */
  void ReAddLabel();

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

  Actor mDecoration[ DECORATION_STATES ];      ///< Stores the decorations for both selected and unselected states.

  Actor mUnselectedContent;                    ///< Stores the unselected content.
  Actor mSelectedContent;                      ///< Stores the selected content.
  Actor mBackgroundContent;                    ///< Stores the background content.
  Actor mSelectedBackgroundContent;            ///< Stores the selected background content.
  Actor mDisabledContent;                      ///< Stores the disabled content.
  Actor mDisabledSelectedContent;              ///< Stores the disabled selected content.
  Actor mDisabledBackgroundContent;            ///< Stores the disabled background content.

  Animation        mTransitionAnimation;       ///< Animation used in the state transitions.

  TapGestureDetector mTapDetector;

  Vector4          mUnselectedColor;           ///< Color to use for unselected content.
  Vector4          mSelectedColor;             ///< Color to use for selected content.

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
