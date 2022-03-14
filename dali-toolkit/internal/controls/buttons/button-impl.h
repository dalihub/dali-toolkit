#ifndef DALI_TOOLKIT_INTERNAL_BUTTON_H
#define DALI_TOOLKIT_INTERNAL_BUTTON_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{
namespace Toolkit
{
class Button;

namespace Internal
{
/**
 * @copydoc Toolkit::Button
 *
 * Button is the base class implementation for all buttons.
 *
 * @note
 *
 * All Foreground/Icon visuals expected to be the same size.
 * Background visuals will take the size of the control.
 * Padding and struts take size precedence over visuals when available space is limited.
 * Icon/Foreground visuals take size precedence over Labels when available space is limited.
 */
class Button : public Control
{
public:
  /**
   * Enum describing the position the text label can be in relation to the control (and foreground/icon)
   */
  enum Align
  {
    BEGIN, // At the start of the control before the foreground/icon
    END,   // At the end of the control after the foreground/icon
    TOP,   // At the top of the control above the foreground/icon
    BOTTOM // At the bottom of the control below the foreground/icon
  };

public:
  /**
   * @brief Sets the button as \e disabled.
   * @param[in] disabled Disabled property
   */
  void SetDisabled(bool disabled);

  /**
   * @brief Returns if the button is disabled.
   * @return \e true if the button is \e disabled
   */
  bool IsDisabled() const;

  /**
   * @brief Sets the \e autorepeating property.
   * @param[in] autoRepeating \e autorepeating property
   */
  void SetAutoRepeating(bool autoRepeating);

  /**
   * @brief Sets the initial autorepeating delay.
   * @param[in] initialAutoRepeatingDelay in seconds
   */
  void SetInitialAutoRepeatingDelay(float initialAutoRepeatingDelay);

  /**
   * @brief Sets the next autorepeating delay.
   * @param[in] nextAutoRepeatingDelay in seconds
   */
  void SetNextAutoRepeatingDelay(float nextAutoRepeatingDelay);

  /**
   * @brief Sets the \e togglable property.
   * @param[in] togglable Togglable property
   */
  void SetTogglableButton(bool togglable);

  /**
   * @brief Sets the button as selected or unselected.
   * @param[in] selected Selected property
   */
  void SetSelected(bool selected);

  /**
   * @brief Returns if the selected property is set and the button is togglable.
   * @return \e true if the button is \e selected
   */
  bool IsSelected() const;

  /**
   * @brief Produces a Property::Map of Text properties to create a Text Visual, merging existing properties with supplied map
   * If the label does not exist yet, it is created.
   * The derived buttons are notified if any properties are changed.
   * @param[in] properties A Property::Map of key-value pairs of properties to set.
   * @param[out] properties A Property::Map of text visual properties to set after merging inMap with existing maps
   */
  void MergeWithExistingLabelProperties(const Property::Map& inMap, Property::Map& outMap);

  /**
   * Performs actions as requested using the action name.
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] attributes The attributes with which to perfrom this action.
   * @return true if action has been accepted by this control
   */
  static bool DoAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes);

public:
  /**
   * Button's state
   */
  enum State
  {
    UNSELECTED_STATE,          ///< The button is unselected.
    SELECTED_STATE,            ///< The button is selected.
    DISABLED_UNSELECTED_STATE, ///< The button is disabled and unselected.
    DISABLED_SELECTED_STATE,   ///< The button is disabled and selected.
    STATE_COUNT,               ///< Number of States
  };

  /**
   * Enum to distinguish the different style-able components of the button
   */
  enum Visuals
  {
    UNSELECTED_FOREGROUND = 0,
    SELECTED_FOREGROUND,
    DISABLED_SELECTED_FOREGROUND,
    DISABLED_UNSELECTED_FOREGROUND,
    UNSELECTED_BACKGROUND,
    SELECTED_BACKGROUND,
    DISABLED_UNSELECTED_BACKGROUND,
    DISABLED_SELECTED_BACKGROUND,
    VISUALS_COUNT
  };

  /**
   * Enum to list types of visual a state can have.
   */
  enum VisualState
  {
    BACKGROUND = 0,
    FOREGROUND,
    VISUAL_STATE_COUNT
  };

protected:
  /**
   * Button press state which is not the same as the actual button's state.
   * For example An UNSELECTED button can be DEPRESSED, but until released, the actual button state doesn't change to SELECTED
   */
  enum PressState
  {
    DEPRESSED,        ///< The button is up.
    UNPRESSED,        ///< The button is down.
    TOGGLE_DEPRESSED, ///< The button has been pressed down and will stay depressed when released.
  };

  /**
   * Construct a new Button.
   */
  Button();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Button();
  /**
   * @return A reference to the label actor.
   */
  Actor& GetLabelActor();

  /**
   * @return A reference to the unselected button image.
   */
  Actor GetUnselectedImage();

  /**
   * @return A reference to the selected image.
   */
  Actor GetSelectedImage();

private:
  /**
   * Perform the click action to click the button.
   * @param[in] attributes The attributes to perfrom this action.
   * @return true if this control can perform action.
   */
  bool DoClickAction(const Property::Map& attributes);

  /**
   * This method is called when the button is a Toggle button and released
   * Could be reimplemented in subclasses to provide specific behaviour.
   * @return bool returns true if state changed.
   */
  virtual bool OnToggleReleased();

  /**
   * This method is called when touch leaves the boundary of the button or several touch points are received.
   * Could be reimplemented in subclasses to provide specific behaviour.
   */
  virtual void OnTouchPointLeave();

  /**
   * This method is called when the touch is interrupted.
   * Could be reimplemented in subclasses to provide specific behaviour.
   */
  virtual void OnTouchPointInterrupted();

  /**
   * This method is called when the \e selected property is changed.
   */
  virtual void OnStateChange(State newState)
  {
  }

  /**
   * This method is called when the \e disabled property is changed.
   */
  virtual void OnDisabled()
  {
  }

  /**
   * This method is called when the button is pressed.
   */
  virtual void OnPressed()
  {
  }

  /**
   * This method is called when the button is released.
   */
  virtual void OnReleased()
  {
  }

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
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor);

  // Properties

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty(BaseObject* object, Property::Index index, const Property::Value& value);

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty(BaseObject* object, Property::Index propertyIndex);

protected: // From Control
  /**
   * @copydoc Toolkit::Control::OnInitialize()
   * @note If overridden by deriving button classes, then an up-call to Button::OnInitialize MUST be made at the start.
   */
  void OnInitialize() override;

  /**
   * @copydoc Toolkit::Control::OnAccessibilityActivated()
   */
  bool OnAccessibilityActivated() override;

  /**
   * @copydoc Toolkit::Control::OnKeyboardEnter()
   */
  bool OnKeyboardEnter() override;

  /**
   * @copydoc Toolkit::Control::OnSceneDisconnection()
   * @note If overridden by deriving button classes, then an up-call to Button::OnSceneDisconnection MUST be made at the end.
   */
  void OnSceneDisconnection() override;

  /**
   * @copydoc Toolkit::Control::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc Toolkit::Control::GetNaturalSize
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc Toolkit::Control::OnSetResizePolicy
   */
  void OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension) override;

  /**
   * @copydoc Toolkit::Control::OnRelayout
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

private:
  /**
   * @brief Handler for touch data
   * @param[in]  actor  The touched actor.
   * @param[in]  touch  The touch info.
   * @return true, if consumed, false otherwise.
   */
  bool OnTouch(Actor actor, const TouchEvent& touch);

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
  void SetUpTimer(float delay);

  /**
   * Button has been pressed
   */
  void Pressed();

  /**
   * This method is called the button is down.
   */
  void ButtonDown();

  /**
   * This method is called when the button is up.
   */
  void ButtonUp();

  /**
   * Slot called when Dali::Timer::SignalTick signal. Resets the autorepeating timer.
   */
  bool AutoRepeatingSlot();

  /**
   *  Check the requested state is an allowed transition.
   *  Some states can not be transitioned to from certain states.
   *  @param[in] requestedState check if can transition to this state
   *  @return bool true if state change valid
   */
  bool ValidateState(State requestedState);

  /**
   * Changes the button state when an action occurs on it
   * @param[in] requestedState the state to change to
   */
  void ChangeState(State requestedState);

  /**
   * This method is called when the button is released.
   */
  void Released();

protected:
  /**
   * Set Text Label Padding
   * @param[in] padding BEGIN END BOTTOM TOP
   */
  void SetLabelPadding(const Padding& padding);

  /**
   * Get Text Label padding
   * @return Padding
   */
  Padding GetLabelPadding();

  /**
   * Set Foreground/icon Padding
   * @param[in] padding BEGIN END BOTTOM TOP
   */
  void SetForegroundPadding(const Padding& padding);

  /**
   * Get Foreground padding
   * @ return Padding
   */
  Padding GetForegroundPadding();

  /**
   * @brief Setup the button components for example foregrounds and background
   * @param[in] index the index of the visual to set
   * @param[in] value the value to set on the component
   * @param[in] visualDepth the depth of the visual if overlapping another
   */
  void CreateVisualsForComponent(Property::Index index, const Property::Value& value, const int visualDepth);

  /**
   * @brief Get the Property map for the given Visual
   * @param[in] visualIndex visual index of the required visual
   * @param[out] retreivedMap the property map used to construct the required visual
   * @return bool success flag, true if visual found
   */
  bool GetPropertyMapForVisual(Property::Index visualIndex, Property::Map& retreivedMap) const;
  /**
   * Returns the animation to be used for transition, creating the animation if needed.
   * @return The initialised transition animation.
   */
  Dali::Animation GetTransitionAnimation();

  /**
   * @brief Set the position of the label relative to foreground/icon, if both present
   * @param[in] labelAlignment given alignment setting
   */
  void SetLabelAlignment(Align labelAlignment);

  /**
   * @brief Get set alignment of label in relation to foreground/icon
   * @return Set alignment value
   */
  Align GetLabelAlignment();

  /**
   * Removes the visual from the button (un-staged)
   * If the derived button does not want the visual removed then use this virtual function to
   * define the required behaviour.
   * Can decide to only remove specified visuals via index
   */
  virtual void OnButtonVisualRemoval(Property::Index visualIndex);

private:
  /**
   * Removes the visual from the button and prepares it to be transitioned out
   * @param[in] visualIndex the visual to remove
   */
  void RemoveVisual(Property::Index visualIndex);

  /**
   * Adds the required visual to the button.
   * @param[in] visualIndex The Property index of the visual required
   */
  void SelectRequiredVisual(Property::Index visualIndex);

  // Undefined
  Button(const Button&);

  // Undefined
  Button& operator=(const Button&);

private:
  // Signals
  Toolkit::Button::ButtonSignalType mPressedSignal;      ///< Signal emitted when the button is pressed.
  Toolkit::Button::ButtonSignalType mReleasedSignal;     ///< Signal emitted when the button is released.
  Toolkit::Button::ButtonSignalType mClickedSignal;      ///< Signal emitted when the button is clicked.
  Toolkit::Button::ButtonSignalType mStateChangedSignal; ///< Signal emitted when the button's state is changed.

  Timer mAutoRepeatingTimer;

  Actor   mLabel;             ///< Stores the button text label.
  Padding mLabelPadding;      ///< The padding around the label (if present).
  Padding mForegroundPadding; ///< The padding around the foreground/icon visual (if present).

  Align mTextLabelAlignment; ///< Position of text label in relation to foreground/icon when both are present.

  TapGestureDetector mTapDetector;

  bool mAutoRepeating;     ///< Stores the autorepeating property.
  bool mTogglableButton;   ///< Stores the togglable property as a flag.
  bool mTextStringSetFlag; ///< Stores if text has been set. Required in relayout but don't want to calculate there.

  float mInitialAutoRepeatingDelay; ///< Stores the initial autorepeating delay in seconds.
  float mNextAutoRepeatingDelay;    ///< Stores the next autorepeating delay in seconds.

  float mAnimationTime;

  PressState mButtonPressedState; ///< In relation to the button being pressed/released
  State      mButtonState;
  State      mPreviousButtonState; ///< During a transition between two states, this stores the previous state so Visuals can be removed.

  // Actions
  bool mClickActionPerforming; ///< Used to manage signal emissions during action

protected:
  class ButtonAccessible : public DevelControl::ControlAccessible
  {
  public:
    using DevelControl::ControlAccessible::ControlAccessible;

    /**
     * @copydoc Dali::Toolkit::DevelControl::ControlAccessible::CalculateStates()
     */
    Dali::Accessibility::States CalculateStates() override;

    /**
     * @copydoc Dali::Toolkit::DevelControl::ControlAccessible::GetNameRaw()
     */
    std::string GetNameRaw() const override;

    /**
     * @copydoc Dali::Toolkit::DevelControl::ControlAccessible::GetNamePropertyIndex()
     */
    Property::Index GetNamePropertyIndex() override;
  };
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::Button& GetImplementation(Toolkit::Button& button)
{
  DALI_ASSERT_ALWAYS(button);

  Dali::RefObject& handle = button.GetImplementation();

  return static_cast<Toolkit::Internal::Button&>(handle);
}

inline const Toolkit::Internal::Button& GetImplementation(const Toolkit::Button& button)
{
  DALI_ASSERT_ALWAYS(button);

  const Dali::RefObject& handle = button.GetImplementation();

  return static_cast<const Toolkit::Internal::Button&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_BUTTON_H
