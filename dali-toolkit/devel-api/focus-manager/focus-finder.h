#ifndef DALI_TOOLKIT_FOCUS_FINDER_H
#define DALI_TOOLKIT_FOCUS_FINDER_H

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
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class FocusFinder;
}

/**
 * FocusFinder
 * This class used for finding the next focusable actor in a given direction
 * from a actor that currently has focus.
 */
class DALI_TOOLKIT_API FocusFinder : public BaseHandle
{
public:
  /**
   * Create a FocusFinder handle; this can be initialised with FocusFinder::Get()
   * Calling member functions with an uninitialised handle is not allowed.
   */
  FocusFinder();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~FocusFinder();

  /**
   * @brief Get the singleton of FocusFinder object.
   * @return A handle to the FocusFinder control.
   */
  static FocusFinder Get();

  /**
   * Get the nearest focusable actor.
   * @param [in] focusedActor The current focused actor.
   * @param [in] direction The direction.
   * @return The nearest focusable actor, or null if none exists.
   */
  Actor GetNearestFocusableActor(Actor focusedActor, Toolkit::Control::KeyboardFocus::Direction direction);

private:
  explicit DALI_INTERNAL FocusFinder(Internal::FocusFinder* impl);

}; // class FocusFinder

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_FOCUS_FINDER_H
