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

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/focus-manager/keyboard-focus-manager-devel.h>
#include <dali-toolkit/internal/focus-manager/keyboard-focus-manager-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelKeyboardFocusManager
{
void SetCustomAlgorithm(KeyboardFocusManager keyboardFocusManager, CustomAlgorithmInterface& interface)
{
  GetImpl(keyboardFocusManager).SetCustomAlgorithm(interface);
}

void EnableFocusIndicator(KeyboardFocusManager keyboardFocusManager, bool enable)
{
  GetImpl(keyboardFocusManager).EnableFocusIndicator(enable);
}

bool IsFocusIndicatorEnabled(KeyboardFocusManager keyboardFocusManager)
{
  return GetImpl(keyboardFocusManager).IsFocusIndicatorEnabled();
}

void EnableDefaultAlgorithm(KeyboardFocusManager keyboardFocusManager, bool enable)
{
  GetImpl(keyboardFocusManager).EnableDefaultAlgorithm(enable);
}

bool IsDefaultAlgorithmEnabled(KeyboardFocusManager keyboardFocusManager)
{
  return GetImpl(keyboardFocusManager).IsDefaultAlgorithmEnabled();
}

} // namespace DevelKeyboardFocusManager

} // namespace Toolkit

} // namespace Dali
