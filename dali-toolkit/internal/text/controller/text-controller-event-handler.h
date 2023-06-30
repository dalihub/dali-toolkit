#ifndef DALI_TOOLKIT_TEXT_CONTROLLER_EVENT_HANDLER_H
#define DALI_TOOLKIT_TEXT_CONTROLLER_EVENT_HANDLER_H

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
#include <dali/devel-api/adaptor-framework/input-method-context.h>
#include <dali/public-api/events/gesture-enumerations.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup.h>
#include <dali-toolkit/internal/text/controller/text-controller.h>
#include <dali-toolkit/internal/text/decorator/text-decorator.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
struct Controller::EventHandler
{
  /// @copydoc Text::Controller::KeyboardFocusGainEvent
  /// @param[in] controller A reference to the controller class
  static void KeyboardFocusGainEvent(Controller& controller);

  /// @copydoc Text::Controller::KeyboardFocusLostEvent
  /// @param[in] controller A reference to the controller class
  static void KeyboardFocusLostEvent(Controller& controller);

  /// @copydoc Text::Controller::KeyEvent
  /// @param[in] controller A reference to the controller class
  static bool KeyEvent(Controller& controller, const Dali::KeyEvent& keyEvent);

  /// @copydoc Text::Controller::AnchorEvent
  /// @param[in] controller A reference to the controller class
  static void AnchorEvent(Controller& controller, float x, float y);

  /// @copydoc Text::Controller::TapEvent
  /// @param[in] controller A reference to the controller class
  static void TapEvent(Controller& controller, unsigned int tapCount, float x, float y);

  /// @copydoc Text::Controller::PanEvent
  /// @param[in] controller A reference to the controller class
  static void PanEvent(Controller& controller, GestureState state, const Vector2& displacement);

  /// @copydoc Text::Controller::LongPressEvent
  /// @param[in] controller A reference to the controller class
  static void LongPressEvent(Controller& controller, GestureState state, float x, float y);

  /// @copydoc Text::Controller::SelectEvent
  /// @param[in] controller A reference to the controller class
  static void SelectEvent(Controller& controller, float x, float y, SelectionType selectType);

  /**
   * @brief Creates a selection event with a selection index.
   *
   * It could be called from the SelectText().
   * The start and end parameters are passed through the event.
   *
   * @param[in] controller A reference to the controller class
   * @param[in] start The start selection position.
   * @param[in] end The end selection position.
   * @param[in] selection type like the range.
   */
  static void SelectEvent(Controller& controller, const uint32_t start, const uint32_t end, SelectionType selectType);

  /**
   * @brief Process queued events which modify the model.
   * @param[in] controller A reference to the controller class
   */
  static void ProcessModifyEvents(Controller& controller);

  /**
   * @brief Used to process an event queued from SetText()
   * @param[in] controller A reference to the controller class
   */
  static void TextReplacedEvent(Controller& controller);

  /**
   * @brief Used to process an event queued from key events etc.
   * @param[in] controller A reference to the controller class
   */
  static void TextInsertedEvent(Controller& controller);

  /**
   * @brief Used to process an event queued from backspace key etc.
   * @param[in] controller A reference to the controller class
   */
  static void TextDeletedEvent(Controller& controller);

  /**
   * @brief Helper to KeyEvent() to handle the backspace or delete key case.
   *
   * @param[in] controller A reference to the controller class
   * @param[in] keyCode The keycode for the key pressed
   * @return True if a character was deleted.
   */
  static bool DeleteEvent(Controller& controller, int keyCode);

  /// @copydoc Text::Controller::OnInputMethodContextEvent
  /// @param[in] controller A reference to the controller class
  static InputMethodContext::CallbackData OnInputMethodContextEvent(Controller&                          controller,
                                                                    InputMethodContext&                  inputMethodContext,
                                                                    const InputMethodContext::EventData& inputMethodContextEvent);

  /**
   * @brief This function executes the actual text update when a clipboard paste event occurs.
   *
   * @param[in] controller A reference to the controller class.
   * @param[in] text The text data to be pasted into the text controller.
   */
  static void PasteClipboardItemEvent(Controller& controller, const std::string& text);

  /// @copydoc Text::Controller::DecorationEvent
  /// @param[in] controller A reference to the controller class
  static void DecorationEvent(Controller& controller, HandleType handleType, HandleState state, float x, float y);

  /// @copydoc Text::Controller::TextPopupButtonTouched
  /// @param[in] controller A reference to the controller class
  static void TextPopupButtonTouched(Controller& controller, Dali::Toolkit::TextSelectionPopup::Buttons button);
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_CONTROLLER_EVENT_HANDLER_H
