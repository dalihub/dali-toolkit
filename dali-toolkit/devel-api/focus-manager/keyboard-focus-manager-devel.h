#ifndef DALI_TOOLKIT_KEYBOARD_FOCUS_MANAGER_DEVEL_H
#define DALI_TOOLKIT_KEYBOARD_FOCUS_MANAGER_DEVEL_H

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
#include <dali-toolkit/public-api/controls/control.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelKeyboardFocusManager
{
/**
 * @brief Interface used to provide custom keyboard focus algorithm for retrieving the next focusable actor.
 *
 * The application / toolkit can implement the interface and override the keyboard focus behaviour.
 * Upon providing an implementation of this interface, the PreFocusChangeSignal is no longer emitted.
 * If focus is changing within a layout container, then the layout container is queried first to provide
 * the next focusable actor. If this does not provide a valid actor, then the Keyboard FocusManager will
 * check focusable properties to determine next focusable actor. If focusable properties are not set,
 * then the Keyboard FocusManager calls the GetNextFocusableActor() method of this interface.
 */
class CustomAlgorithmInterface
{
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~CustomAlgorithmInterface(){};

  /**
   * @brief Called by the KeyboardFocusManager to get the next focusable actor.
   *
   * @param[in] current The current focused actor
   * @param[in] proposed The proposed focused actor
   * @param[in] direction The direction of focus movement
   * @return A handle to the next focusable actor
   */
  virtual Actor GetNextFocusableActor(Actor current, Actor proposed, Control::KeyboardFocus::Direction direction) = 0;
};

/**
 * @brief Provide the implementation of custom Focus algorithm interface
 *
 * @param[in] keyboardFocusManager The instance of KeyboardFocusManager
 * @param[in] interface The user's implementation of CustomAlgorithmInterface
 * @see DevelKeyboardFocusManager::CustomAlgorithmInterface
 */
DALI_TOOLKIT_API void SetCustomAlgorithm(KeyboardFocusManager keyboardFocusManager, CustomAlgorithmInterface& interface);

/**
 * @brief Decide using focus indicator or not
 *
 * @param[in] keyboardFocusManager The instance of KeyboardFocusManager
 * @param[in] enable Whether using focus indicator or not
 */
DALI_TOOLKIT_API void EnableFocusIndicator(KeyboardFocusManager keyboardFocusManager, bool enable);

/**
 * @brief Check focus indicator is enabled or not
 *
 * @param[in] keyboardFocusManager The instance of KeyboardFocusManager
 * @return True when focus indicator is enabled
 */
DALI_TOOLKIT_API bool IsFocusIndicatorEnabled(KeyboardFocusManager keyboardFocusManager);

/**
 * @brief Decide using default focus algorithm or not
 *
 * @param[in] keyboardFocusManager The instance of KeyboardFocusManager
 * @param[in] enable Whether using default focus algorithm or not
 */
DALI_TOOLKIT_API void EnableDefaultAlgorithm(KeyboardFocusManager keyboardFocusManager, bool enable);

/**
 * @brief Check default focus algorithm is enabled or not
 *
 * @param[in] keyboardFocusManager The instance of KeyboardFocusManager
 * @return True when default focus algorithm is enabled
 */
DALI_TOOLKIT_API bool IsDefaultAlgorithmEnabled(KeyboardFocusManager keyboardFocusManager);

} // namespace DevelKeyboardFocusManager

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_KEYBOARD_FOCUS_MANAGER_DEVEL_H
