#ifndef DALI_TOOLKIT_TEXT_UPDATER_H
#define DALI_TOOLKIT_TEXT_UPDATER_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-controller.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief Methods that update the text
 */
struct Controller::TextUpdater
{
  /// @copydoc Text::Contoller::SetText
  /// @param[in] controller The controller
  static void SetText(Controller& controller, const std::string& text);

  /// @copydoc Text::Contoller::InsertText
  /// @param[in] controller The controller
  static void InsertText(Controller& controller, const std::string& text, Controller::InsertType type);

  /// @copydoc Text::Contoller::PasteText
  /// @param[in] controller The controller
  static void PasteText(Controller& controller, const std::string& stringToPaste);

  /// @copydoc Text::Contoller::RemoveText
  /// @param[in] controller The controller
  static bool RemoveText(Controller& controller, int cursorOffset, int numberOfCharacters, UpdateInputStyleType type);

  /// @copydoc Text::Contoller::RemoveSelectedText
  /// @param[in] controller The controller
  static bool RemoveSelectedText(Controller& controller);

  /// @copydoc Text::Contoller::ResetText
  /// @param[in] controller The controller
  static void ResetText(Controller& controller);
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_UPDATER_H
