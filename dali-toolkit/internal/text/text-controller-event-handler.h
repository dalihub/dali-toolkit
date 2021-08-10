#ifndef DALI_TOOLKIT_TEXT_CONTROLLER_EVENT_HANDLER_H
#define DALI_TOOLKIT_TEXT_CONTROLLER_EVENT_HANDLER_H

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
#include <dali/devel-api/adaptor-framework/input-method-context.h>
#include <dali/public-api/events/gesture-enumerations.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup.h>
#include <dali-toolkit/internal/text/decorator/text-decorator.h>
#include <dali-toolkit/internal/text/text-controller.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
struct Controller::EventHandler
{
  static void KeyboardFocusGainEvent(Controller& controller);
  static void KeyboardFocusLostEvent(Controller& controller);
  static bool KeyEvent(Controller& controller, const Dali::KeyEvent& keyEvent);
  static void AnchorEvent(Controller& controller, float x, float y);
  static void TapEvent(Controller& controller, unsigned int tapCount, float x, float y);
  static void PanEvent(Controller& controller, GestureState state, const Vector2& displacement);
  static void LongPressEvent(Controller& controller, GestureState state, float x, float y);
  static void SelectEvent(Controller& controller, float x, float y, SelectionType selectType);
  static void SelectEvent(Controller& controller, const uint32_t start, const uint32_t end, SelectionType selectType);
  static void ProcessModifyEvents(Controller& controller);
  static void TextReplacedEvent(Controller& controller);
  static void TextInsertedEvent(Controller& controller);
  static void TextDeletedEvent(Controller& controller);
  static bool DeleteEvent(Controller& controller, int keyCode);

  static InputMethodContext::CallbackData OnInputMethodContextEvent(Controller&                          controller,
                                                                    InputMethodContext&                  inputMethodContext,
                                                                    const InputMethodContext::EventData& inputMethodContextEvent);

  static void PasteClipboardItemEvent(Controller& controller);
  static void DecorationEvent(Controller& controller, HandleType handleType, HandleState state, float x, float y);
  static void TextPopupButtonTouched(Controller& controller, Dali::Toolkit::TextSelectionPopup::Buttons button);
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_CONTROLLER_EVENT_HANDLER_H
