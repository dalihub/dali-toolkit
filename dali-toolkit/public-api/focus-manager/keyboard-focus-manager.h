#ifndef __DALI_TOOLKIT_KEYBOARD_FOCUS_MANAGER_H__
#define __DALI_TOOLKIT_KEYBOARD_FOCUS_MANAGER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class KeyboardFocusManager;
}
/**
 * @addtogroup dali_toolkit_focus_manager
 * @{
 */

/**
 * @brief Provides the functionality of handling keyboard navigation
 * and maintaining the two dimensional keyboard focus chain.
 *
 * It provides functionality of setting the focus and moving the focus
 * in four directions (i.e. Left, Right, Up and Down). It also draws a
 * highlight for the focused actor and emits a signal when the focus
 * is changed.
 *
 * Signals
 * | %Signal Name                  | Method                             |
 * |-------------------------------|------------------------------------|
 * | keyboardPreFocusChange        | @ref PreFocusChangeSignal()        |
 * | keyboardFocusChanged          | @ref FocusChangedSignal()          |
 * | keyboardFocusGroupChanged     | @ref FocusGroupChangedSignal()     |
 * | keyboardFocusedActorAenterKey | @ref FocusedActorEnterKeySignal()  |
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API KeyboardFocusManager : public BaseHandle
{

public:

  /// @brief Pre focus change signal
  typedef Signal< Actor ( Actor, Actor, Control::KeyboardFocus::Direction ) > PreFocusChangeSignalType;

  /// @brief Focus changed signal
  typedef Signal< void ( Actor, Actor ) > FocusChangedSignalType;

  /// @brief Focus group changed signal
  typedef Signal< void ( Actor, bool ) > FocusGroupChangedSignalType;

  /// @brief Focused actor has the enter key pressed signal
  typedef Signal< void ( Actor ) > FocusedActorEnterKeySignalType;

  /**
   * @brief Create a KeyboardFocusManager handle; this can be initialised with KeyboardFocusManager::New().
   *
   * Calling member functions with an uninitialised handle is not allowed.
   * @SINCE_1_0.0
   */
  KeyboardFocusManager();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~KeyboardFocusManager();

  /**
   * @brief Get the singleton of KeyboardFocusManager object.
   *
   * @SINCE_1_0.0
   * @return A handle to the KeyboardFocusManager control.
   */
  static KeyboardFocusManager Get();

  /**
   * @brief Move the keyboard focus to the given actor.
   *
   * Only one actor can be focused at the same time.  The actor must
   * be in the stage already and keyboard focusable.
   *
   * @SINCE_1_0.0
   * @param actor The actor to be focused
   * @return Whether the focus is successful or not
   * @pre The KeyboardFocusManager has been initialized.
   * @pre The Actor has been initialized.
   */
  bool SetCurrentFocusActor(Actor actor);

  /**
   * @brief Get the current focused actor.
   *
   * @SINCE_1_0.0
   * @return A handle to the current focused actor or an empty handle if no actor is focused.
   * @pre The KeyboardFocusManager has been initialized.
   */
  Actor GetCurrentFocusActor();

  /**
   * @brief Move the focus to the next focusable actor in the focus
   * chain in the given direction (according to the focus traversal
   * order).
   *
   * @SINCE_1_0.0
   * @param direction The direction of focus movement
   * @return true if the movement was successful
   * @pre The KeyboardFocusManager has been initialized.
   */
  bool MoveFocus(Control::KeyboardFocus::Direction direction);

  /**
   * @brief Clear the focus from the current focused actor if any, so
   * that no actor is focused in the focus chain.
   *
   * It will emit focus changed signal without current focused actor
   * @SINCE_1_0.0
   * @pre The KeyboardFocusManager has been initialized.
   */
  void ClearFocus();

  /**
   * @brief Set whether the focus movement should be looped within the same focus group.
   *
   * The focus movement is not looped by default.
   * @SINCE_1_0.0
   * @param enabled Whether the focus movement should be looped
   * @pre The KeyboardFocusManager has been initialized.
   */
  void SetFocusGroupLoop(bool enabled);

  /**
   * @brief Get whether the focus movement should be looped within the same focus group.
   *
   * @SINCE_1_0.0
   * @return Whether the focus movement should be looped
   * @pre The KeyboardFocusManager has been initialized.
   */
  bool GetFocusGroupLoop() const;

  /**
   * @brief Set whether an actor is a focus group that can limit the
   * scope of focus movement to its child actors in the focus chain.
   *
   * Layout controls set themselves as focus groups by default.
   *
   * @SINCE_1_0.0
   * @param actor The actor to be set as a focus group.
   * @param isFocusGroup Whether to set the actor as a focus group or not.
   * @pre The KeyboardFocusManager has been initialized.
   * @pre The Actor has been initialized.
   */
  void SetAsFocusGroup(Actor actor, bool isFocusGroup);

  /**
   * @brief Check whether the actor is set as a focus group or not.
   *
   * @SINCE_1_0.0
   * @param actor The actor to be checked.
   * @return Whether the actor is set as a focus group.
   * @pre The KeyboardFocusManager has been initialized.
   * @pre The Actor has been initialized.
   */
  bool IsFocusGroup(Actor actor) const;

  /**
   * @brief Returns the closest ancestor of the given actor that is a focus group.
   *
   * @SINCE_1_0.0
   * @param actor The actor to be checked for its focus group
   * @return The focus group the given actor belongs to or an empty handle if the given actor
   * doesn't belong to any focus group
   */
  Actor GetFocusGroup(Actor actor);

  /**
   * @brief Set the focus indicator actor.
   *
   * This will replace the default focus indicator actor in
   * KeyboardFocusManager and will be added to the focused actor as a
   * highlight.
   *
   * @SINCE_1_0.0
   * @param indicator The indicator actor to be added
   * @pre The KeyboardFocusManager has been initialized.
   * @pre The indicator actor has been initialized.
   */
  void SetFocusIndicatorActor(Actor indicator);

  /**
   * @brief Get the focus indicator actor.
   *
   * @SINCE_1_0.0
   * @return A handle to the focus indicator actor
   * @pre The KeyboardFocusManager has been initialized.
   */
  Actor GetFocusIndicatorActor();

public: // Signals

  /**
   * @brief This signal is emitted before the focus is going to be changed.
   *
   * KeyboardFocusManager makes the best guess for which actor to
   * focus towards the given direction, but applications might want to
   * change that. By connecting with this signal, they can check the
   * proposed actor to focus and return a different actor if they
   * wish.  This signal is only emitted when the navigation key is
   * pressed and KeyboardFocusManager tries to move the focus
   * automatically. It won't be emitted for focus movement by calling
   * SetCurrentFocusActor directly.
   *
   * A callback of the following type may be connected:
   * @code
   *   Actor YourCallbackName(Actor currentFocusedActor, Actor proposedActorToFocus, Control::KeyboardFocus::Direction direction);
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to.
   * @pre The Object has been initialized.
   */
  PreFocusChangeSignalType& PreFocusChangeSignal();

  /**
   * @brief This signal is emitted after the current focused actor has been changed.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(Actor originalFocusedActor, Actor currentFocusedActor);
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to.
   * @pre The Object has been initialized.
   */
  FocusChangedSignalType& FocusChangedSignal();

  /**
   * @brief This signal is emitted when the focus group has been changed.
   *
   * If the current focus group has a parent layout control,
   * KeyboardFocusManager will make the best guess for the next focus
   * group to move the focus to in the given direction (forward or
   * backward). If not, the application has to set the new focus.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(Actor currentFocusedActor, bool forward);
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to.
   * @pre The Object has been initialized.
   */
  FocusGroupChangedSignalType& FocusGroupChangedSignal();

  /**
   * @brief This signal is emitted when the current focused actor has the enter key pressed on it.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(Actor enterPressedActor);
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to.
   * @pre The Object has been initialized.
   */
  FocusedActorEnterKeySignalType& FocusedActorEnterKeySignal();

  // Not intended for application developers

  /**
   * @brief Creates a new handle from the implementation.
   *
   * @SINCE_1_0.0
   * @param[in] impl A pointer to the object.
   */
  explicit DALI_INTERNAL KeyboardFocusManager(Internal::KeyboardFocusManager *impl);

}; // class KeyboardFocusManager

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_KEYBOARD_FOCUS_MANAGER_H__
