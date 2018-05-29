#ifndef DALI_TOOLKIT_TEXT_EDITOR_DEVEL_H
#define DALI_TOOLKIT_TEXT_EDITOR_DEVEL_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/text-controls/text-editor.h>

namespace Dali
{

namespace Toolkit
{

namespace DevelTextEditor
{

namespace Property
{
  enum Type
  {
      RENDERING_BACKEND = Dali::Toolkit::TextEditor::Property::RENDERING_BACKEND,
      TEXT = Dali::Toolkit::TextEditor::Property::TEXT,
      TEXT_COLOR = Dali::Toolkit::TextEditor::Property::TEXT_COLOR,
      FONT_FAMILY = Dali::Toolkit::TextEditor::Property::FONT_FAMILY,
      FONT_STYLE = Dali::Toolkit::TextEditor::Property::FONT_STYLE,
      POINT_SIZE = Dali::Toolkit::TextEditor::Property::POINT_SIZE,
      HORIZONTAL_ALIGNMENT = Dali::Toolkit::TextEditor::Property::HORIZONTAL_ALIGNMENT,
      SCROLL_THRESHOLD = Dali::Toolkit::TextEditor::Property::SCROLL_THRESHOLD,
      SCROLL_SPEED = Dali::Toolkit::TextEditor::Property::SCROLL_SPEED,
      PRIMARY_CURSOR_COLOR = Dali::Toolkit::TextEditor::Property::PRIMARY_CURSOR_COLOR,
      SECONDARY_CURSOR_COLOR = Dali::Toolkit::TextEditor::Property::SECONDARY_CURSOR_COLOR,
      ENABLE_CURSOR_BLINK = Dali::Toolkit::TextEditor::Property::ENABLE_CURSOR_BLINK,
      CURSOR_BLINK_INTERVAL = Dali::Toolkit::TextEditor::Property::CURSOR_BLINK_INTERVAL,
      CURSOR_BLINK_DURATION = Dali::Toolkit::TextEditor::Property::CURSOR_BLINK_DURATION,
      CURSOR_WIDTH = Dali::Toolkit::TextEditor::Property::CURSOR_WIDTH,
      GRAB_HANDLE_IMAGE = Dali::Toolkit::TextEditor::Property::GRAB_HANDLE_IMAGE,
      GRAB_HANDLE_PRESSED_IMAGE = Dali::Toolkit::TextEditor::Property::GRAB_HANDLE_PRESSED_IMAGE,
      SELECTION_HANDLE_IMAGE_LEFT = Dali::Toolkit::TextEditor::Property::SELECTION_HANDLE_IMAGE_LEFT,
      SELECTION_HANDLE_IMAGE_RIGHT = Dali::Toolkit::TextEditor::Property::SELECTION_HANDLE_IMAGE_RIGHT,
      SELECTION_HANDLE_PRESSED_IMAGE_LEFT = Dali::Toolkit::TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT,
      SELECTION_HANDLE_PRESSED_IMAGE_RIGHT = Dali::Toolkit::TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT,
      SELECTION_HANDLE_MARKER_IMAGE_LEFT = Dali::Toolkit::TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT,
      SELECTION_HANDLE_MARKER_IMAGE_RIGHT = Dali::Toolkit::TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT,
      SELECTION_HIGHLIGHT_COLOR = Dali::Toolkit::TextEditor::Property::SELECTION_HIGHLIGHT_COLOR,
      DECORATION_BOUNDING_BOX = Dali::Toolkit::TextEditor::Property::DECORATION_BOUNDING_BOX,
      ENABLE_MARKUP = Dali::Toolkit::TextEditor::Property::ENABLE_MARKUP,
      INPUT_COLOR = Dali::Toolkit::TextEditor::Property::INPUT_COLOR,
      INPUT_FONT_FAMILY = Dali::Toolkit::TextEditor::Property::INPUT_FONT_FAMILY,
      INPUT_FONT_STYLE = Dali::Toolkit::TextEditor::Property::INPUT_FONT_STYLE,
      INPUT_POINT_SIZE = Dali::Toolkit::TextEditor::Property::INPUT_POINT_SIZE,
      LINE_SPACING = Dali::Toolkit::TextEditor::Property::LINE_SPACING,
      INPUT_LINE_SPACING = Dali::Toolkit::TextEditor::Property::INPUT_LINE_SPACING,
      UNDERLINE = Dali::Toolkit::TextEditor::Property::UNDERLINE,
      INPUT_UNDERLINE = Dali::Toolkit::TextEditor::Property::INPUT_UNDERLINE,
      SHADOW = Dali::Toolkit::TextEditor::Property::SHADOW,
      INPUT_SHADOW = Dali::Toolkit::TextEditor::Property::INPUT_SHADOW,
      EMBOSS = Dali::Toolkit::TextEditor::Property::EMBOSS,
      INPUT_EMBOSS = Dali::Toolkit::TextEditor::Property::INPUT_EMBOSS,
      OUTLINE = Dali::Toolkit::TextEditor::Property::OUTLINE,
      INPUT_OUTLINE = Dali::Toolkit::TextEditor::Property::INPUT_OUTLINE,
      SMOOTH_SCROLL = Dali::Toolkit::TextEditor::Property::SMOOTH_SCROLL,
      SMOOTH_SCROLL_DURATION = Dali::Toolkit::TextEditor::Property::SMOOTH_SCROLL_DURATION,
      ENABLE_SCROLL_BAR = Dali::Toolkit::TextEditor::Property::ENABLE_SCROLL_BAR,
      SCROLL_BAR_SHOW_DURATION = Dali::Toolkit::TextEditor::Property::SCROLL_BAR_SHOW_DURATION,
      SCROLL_BAR_FADE_DURATION = Dali::Toolkit::TextEditor::Property::SCROLL_BAR_FADE_DURATION,
      PIXEL_SIZE = Dali::Toolkit::TextEditor::Property::PIXEL_SIZE,
      LINE_COUNT = Dali::Toolkit::TextEditor::Property::LINE_COUNT,
      ENABLE_SELECTION = Dali::Toolkit::TextEditor::Property::ENABLE_SELECTION,
      PLACEHOLDER = Dali::Toolkit::TextEditor::Property::PLACEHOLDER,
      LINE_WRAP_MODE = Dali::Toolkit::TextEditor::Property::LINE_WRAP_MODE,

      /**
       * @brief The text to display when the TextEditor is empty and inactive.
       * @details Name "placeholderText", type Property::STRING.
       */
      PLACEHOLDER_TEXT,

      /**
       * @brief The placeholder-text color.
       * @details Name "placeholderTextColor", type Property::VECTOR4.
       */
      PLACEHOLDER_TEXT_COLOR,

      /**
       * @brief Enables Text selection using Shift key.
       * @details Name "enableShiftSelection", type Property::BOOLEAN.
       */
      ENABLE_SHIFT_SELECTION,

      /**
       * @brief Enables the grab handles for text selection.
       * @details Name "enableGrabHandle", type Property::BOOLEAN.
       * @note The default value is true, which means the grab handles are enabled by default.
       */
      ENABLE_GRAB_HANDLE
  };

} // namespace Property

/**
 * @brief Return the input method context of TextEditor.
 *
 * @param[in] textEditor The instance of TextEditor.
 * @return InputMethodContext instance.
 */
DALI_TOOLKIT_API InputMethodContext GetInputMethodContext( TextEditor textEditor );

} // namespace DevelTextEditor

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_EDITOR_DEVEL_H
