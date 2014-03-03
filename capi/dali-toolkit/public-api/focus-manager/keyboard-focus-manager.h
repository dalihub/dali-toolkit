#ifndef __DALI_TOOLKIT_KEYBOARD_FOCUS_MANAGER_H__
#define __DALI_TOOLKIT_KEYBOARD_FOCUS_MANAGER_H__

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

// INTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class KeyboardFocusManager;
}

/**
 * KeyboardFocusManager
 * This class provides the functionality of handling keyboard navigation and maintaining
 * the two dimensional keyboard focus chain. It provides functionality of setting the
 * focus and moving the focus in four directions (i.e. Left, Right, Up and Down). It also
 * draws a highlight for the focused actor and emits a signal when the focus is changed.
 */

 class KeyboardFocusManager : public BaseHandle
 {
 public:
  //Signal Names
  static const char* const SIGNAL_PRE_FOCUS_CHANGE;
  static const char* const SIGNAL_FOCUS_CHANGED;
  static const char* const SIGNAL_FOCUS_GROUP_CHANGED;
  static const char* const SIGNAL_FOCUSED_ACTOR_ACTIVATED;

 public:

  //Pre focus change signal
  typedef SignalV2< Actor ( Actor, Actor, Control::KeyboardFocusNavigationDirection ) > PreFocusChangeSignalV2;

  //Focus changed signal
  typedef SignalV2< void ( Actor, Actor ) > FocusChangedSignalV2;

  //Focus group changed signal
  typedef SignalV2< void ( Actor, bool ) > FocusGroupChangedSignalV2;

  //Focused actor activated signal
  typedef SignalV2< void ( Actor ) > FocusedActorActivatedSignalV2;

  /**
   * Create a KeyboardFocusManager handle; this can be initialised with KeyboardFocusManager::New()
   * Calling member functions with an uninitialised handle is not allowed.
   */
  KeyboardFocusManager();

  /**
   * Virtual destructor.
   */
  virtual ~KeyboardFocusManager();

  /**
   * Get the singleton of KeyboardFocusManager object.
   * @return A handle to the KeyboardFocusManager control.
   */
  static KeyboardFocusManager Get();

  /**
   * Move the keyboard focus to the given actor. Only one actor can be focused at the same time.
   * The actor must be in the stage already and keyboard focusable.
   * @pre The KeyboardFocusManager has been initialized.
   * @pre The Actor has been initialized.
   * @param actor The actor to be focused
   * @return Whether the focus is successful or not
   */
  bool SetCurrentFocusActor(Actor actor);

  /**
   * Get the current focused actor.
   * @pre The KeyboardFocusManager has been initialized.
   * @return A handle to the current focused actor or an empty handle if no actor is focused.
   */
  Actor GetCurrentFocusActor();

  /**
   * Move the focus to the next focusable actor in the focus chain in the given direction
   * (according to the focus traversal order).
   * @pre The KeyboardFocusManager has been initialized.
   * @param direction The direction of focus movement
   * @return true if the movement was successful
   */
  bool MoveFocus(Control::KeyboardFocusNavigationDirection direction);

  /**
   * Clear the focus from the current focused actor if any, so that no actor is focused
   * in the focus chain.
   * It will emit focus changed signal without current focused actor
   * @pre The KeyboardFocusManager has been initialized.
   */
  void ClearFocus();

  /**
   * Set whether the focus movement should be looped within the same focus group.
   * The focus movement is not looped by default.
   * @pre The KeyboardFocusManager has been initialized.
   * @param enabled Whether the focus movement should be looped
   */
  void SetFocusGroupLoop(bool enabled);

  /**
   * Get whether the focus movement should be looped within the same focus group.
   * @pre The KeyboardFocusManager has been initialized.
   * @return Whether the focus movement should be looped
   */
  bool GetFocusGroupLoop() const;

  /**
   * Set whether an actor is a focus group that can limit the scope of focus movement to
   * its child actors in the focus chain. Layout controls set themselves as focus groups
   * by default.
   * @pre The KeyboardFocusManager has been initialized.
   * @pre The Actor has been initialized.
   * @param actor The actor to be set as a focus group.
   * @param isFocusGroup Whether to set the actor as a focus group or not.
   */
  void SetAsFocusGroup(Actor actor, bool isFocusGroup);

  /**
   * Check whether the actor is set as a focus group or not.
   * @pre The KeyboardFocusManager has been initialized.
   * @pre The Actor has been initialized.
   * @param actor The actor to be checked.
   * @return Whether the actor is set as a focus group.
   */
  bool IsFocusGroup(Actor actor) const;

  /**
   * Returns the closest ancestor of the given actor that is a focus group.
   * @param actor The actor to be checked for its focus group
   * @return The focus group the given actor belongs to or an empty handle if the given actor
   * doesn't belong to any focus group
   */
  Actor GetFocusGroup(Actor actor);

  /**
   * Set the focus indicator actor. This will replace the default focus indicator actor
   * in KeyboardFocusManager and will be added to the focused actor as a highlight.
   * @pre The KeyboardFocusManager has been initialized.
   * @pre The indicator actor has been initialized.
   * @param indicator The indicator actor to be added
   */
  void SetFocusIndicatorActor(Actor indicator);

  /**
   * Get the focus indicator actor.
   * @pre The KeyboardFocusManager has been initialized.
   * @return A handle to the focus indicator actor
   */
  Actor GetFocusIndicatorActor();

 public: // Signals

  /**
   * This signal is emitted before the focus is going to be changed.
   * KeyboardFocusManager makes the best guess for which actor to focus towards the given direction,
   * but applications might want to change that. By connecting with this signal, they can check the
   * proposed actor to focus and return a different actor if they wish.
   * This signal is only emitted when the navigation key is pressed and KeyboardFocusManager tries to
   * move the focus automatically. It won't be emitted for focus movement by calling SetCurrentFocusActor
   * directly.
   * A callback of the following type may be connected:
   * @code
   *   Actor YourCallbackName(Actor currentFocusedActor, Actor proposedActorToFocus, Control::KeyboardFocusNavigationDirection direction);
   * @endcode
   * @pre The Object has been initialized.
   * @return The signal to connect to.
   */
  PreFocusChangeSignalV2& PreFocusChangeSignal();

  /**
   * This signal is emitted after the current focused actor has been changed.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(Actor originalFocusedActor, Actor currentFocusedActor);
   * @endcode
   * @pre The Object has been initialized.
   * @return The signal to connect to.
   */
  FocusChangedSignalV2& FocusChangedSignal();

  /**
   * This signal is emitted when the focus group has been changed.
   * If the current focus group has a parent layout control, KeyboardFocusManager will make the best guess
   * for the next focus group to move the focus to in the given direction (forward or backward). If not,
   * the application has to set the new focus.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(Actor currentFocusedActor, bool forward);
   * @endcode
   * @pre The Object has been initialized.
   * @return The signal to connect to.
   */
  FocusGroupChangedSignalV2& FocusGroupChangedSignal();

  /**
   * This signal is emitted when the current focused actor is activated.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(Actor activatedActor);
   * @endcode
   * @pre The Object has been initialized.
   * @return The signal to connect to.
   */
  FocusedActorActivatedSignalV2& FocusedActorActivatedSignal();

  // Not intended for application developers

  /**
   * Creates a new handle from the implementation.
   * @param[in] impl A pointer to the object.
   */
  explicit DALI_INTERNAL KeyboardFocusManager(Internal::KeyboardFocusManager *impl);

}; // class KeyboardFocusManager

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_KEYBOARD_FOCUS_MANAGER_H__
