#ifndef DALI_TOOLKIT_TEXT_SELECTION_HANDLE_CONTROLLER_H
#define DALI_TOOLKIT_TEXT_SELECTION_HANDLE_CONTROLLER_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/cursor-helper-functions.h>
#include <dali-toolkit/internal/text/decorator/text-decorator.h>
#include <dali-toolkit/internal/text/controller/text-controller.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Updates the Selection Handles.
 */
struct SelectionHandleController
{
  /// @copydoc Controller::Impl::RepositionSelectionHandles
  /// @param[in] impl The Text controller Impl.
  static void Reposition(Controller::Impl& impl);

  /// @copydoc Controller::Impl::RepositionSelectionHandles
  /// @param[in] impl The Text controller Impl.
  static void Reposition(Controller::Impl& impl, float visualX, float visualY, Controller::NoTextTap::Action action);

  /// @copydoc Controller::Impl::UpdateSelectionHandle
  /// @param[in] impl The Text controller Impl.
  static void Update(Controller::Impl& impl, HandleType handleType, const CursorInfo& cursorInfo);
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_SELECTION_HANDLE_CONTROLLER_H
