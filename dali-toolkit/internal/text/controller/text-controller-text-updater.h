#ifndef DALI_TOOLKIT_TEXT_UPDATER_H
#define DALI_TOOLKIT_TEXT_UPDATER_H

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/controller/text-controller.h>

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

  /**
   * @brief Called by editable UI controls when key events are received.
   *
   * @param[in] controller The controller
   * @param[in] text The text to insert.
   * @param[in] type Used to distinguish between regular key events and InputMethodContext events.
   */
  static void InsertText(Controller& controller, const std::string& text, Controller::InsertType type);

  /// @copydoc Text::EditableControlInterface::PasteText()
  /// @param[in] controller The controller
  static void PasteText(Controller& controller, const std::string& stringToPaste);

  /**
   * @brief Remove a given number of characters
   *
   * When predictve text is used the pre-edit text is removed and inserted again with the new characters.
   * The UpdateInputStyleType @type parameter if set to DONT_UPDATE_INPUT_STYLE avoids to update the input
   * style when pre-edit text is removed.
   *
   * @param[in] controller The controller
   * @param[in] cursorOffset Start position from the current cursor position to start deleting characters.
   * @param[in] numberOfCharacters The number of characters to delete from the cursorOffset.
   * @param[in] type Whether to update the input style.
   * @param[in] isDeletingPreEdit Whether to remove pre-edit when inserting text.
   * @return True if the remove was successful.
   */
  static bool RemoveText(Controller& controller, int cursorOffset, int numberOfCharacters, UpdateInputStyleType type, bool isDeletingPreEdit);

  /**
   * @brief Checks if text is selected and if so removes it.
   * @param[in] controller The controller
   * @return true if text was removed
   */
  static bool RemoveSelectedText(Controller& controller);

  /**
   * @brief Used to remove the text included the placeholder text.
   * @param[in] controller The controller
   */
  static void ResetText(Controller& controller);

  /**
   * @brief Update anchor position from given number of inserted characters.
   *
   * @param[in] controller The controller
   * @param[in] numberOfCharacters The number of inserted characters.
   * @param[in] previousCursorIndex A cursor position before event occurs.
   */
  static void InsertTextAnchor(Controller& controller, int numberOfCharacters, CharacterIndex previousCursorIndex);

  /**
   * @brief Update anchor position from given number of removed characters.
   *
   * @param[in] controller The controller
   * @param[in] cursorOffset Start position from the current cursor position to start deleting characters.
   * @param[in] numberOfCharacters The number of removed characters.
   * @param[in] previousCursorIndex A cursor position before event occurs.
   */
  static void RemoveTextAnchor(Controller& controller, int cursorOffset, int numberOfCharacters, CharacterIndex previousCursorIndex);
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_UPDATER_H
