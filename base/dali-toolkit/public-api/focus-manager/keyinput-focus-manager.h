#ifndef __DALI_TOOLKIT_KEYINPUT_FOCUS_MANAGER_H__
#define __DALI_TOOLKIT_KEYINPUT_FOCUS_MANAGER_H__

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
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class KeyInputFocusManager;
}

/**
 * KeyInputFocusManager
 * This class provides the functionality of registering for keyboard events for controls.
 * The keyinput focus manager maintains a stack of controls, With the last added control receiving
 * all the keyboard events first. And if the conrol doesn't consume the event it is passed to
 * the next control in the stack. If none of the controls in the stack consume the key event then
 * UnhandledKeyEventSignal() is emitted.
 */

 class KeyInputFocusManager : public BaseHandle
 {
 public:
  //Signal Names
  static const char* const SIGNAL_KEY_INPUT_FOCUS_CHANGED;
  static const char* const SIGNAL_UNHANDLED_KEY_EVENT;

  // KeyInputFocusChanged
  typedef SignalV2< void (Control, Control) > KeyInputFocusChangedSignalV2;

  // Unhandled Key Event
  typedef SignalV2< void (const KeyEvent&) > UnhandledKeyEventSignalV2;

 public:

  /**
   * Create a KeyInputFocusManager handle; this can be initialised with KeyInputFocusManager::Get()
   * Calling member functions with an uninitialised handle is not allowed.
   */
  KeyInputFocusManager();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~KeyInputFocusManager();

  /**
   * Get the singleton of KeyInputFocusManager object.
   * @return A handle to the KeyInputFocusManager control.
   */
  static KeyInputFocusManager Get();

  /**
   * Sets keyboard focus for a control.
   * Note: A control can be set to be in focus and still not receive all the key events if another control has over ridden it.
   * As the key input focus mechanism works like a stack, the top most control receives all the key events, and passes on the
   * unhandled events to the controls below in the stack. A control in the stack will regain key input focus when there are no more
   * controls above it in the focus stack.
   *
   * @pre The Control is not in the focus stack. If it is allready present in the top of the stack it results in a no-op, If it is
   * present in the stack but not on the top of the stack, then the control is moved to the top of the focus stack.
   * @param[in] control The Control to receive keyboard input
   */
  void SetFocus(Control control);

  /**
   * Query for the control that is currently set to be on top of the fcous stack and receives all
   * keyboard input events first.
   * @return Pointer to the control set to receive keyboard inputs.
   */
  Control GetCurrentFocusControl() const;

  /**
   * Removes focus for the given control, The control will no longer receive events from keyboard.
   * @param [in] control which should be removed from focus.
   */
  void RemoveFocus(Control control);

  /**
   * Queries whether a control is currently part of the focus stack.
   * @param [in] control which should be queried.
   * @return True if it is part of the foucus stack False otherwise.
   */
  bool IsKeyboardListener(Control control);

 public: // Signals

  /**
   * This signal is emitted when the key input focus control changes.
   * Two control parameters are sent as part of this signal, the first being the signal that now has the focus, the second
   * being the one that has lost focus.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback(Control focusGainedControl, Control focusLostActor);
   * @endcode
   * @return The signal to connect to.
   */
  KeyInputFocusChangedSignalV2& KeyInputFocusChangedSignal();

  /**
   * This signal is emitted when a key event was received, and none of the focused controls on the stage have consumed it.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(const KeyEvent& event);
   * @endcode
   * @return The signal to connect to.
   */
  UnhandledKeyEventSignalV2& UnhandledKeyEventSignal();

private:

  KeyInputFocusManager(Internal::KeyInputFocusManager *impl);

}; // class KeyInputFocusManager

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_KEYINPUT_FOCUS_MANAGER_H__
