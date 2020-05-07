#ifndef DALI_TOOLKIT_TEXT_FIELD_DEVEL_H
#define DALI_TOOLKIT_TEXT_FIELD_DEVEL_H

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
#include <dali-toolkit/public-api/controls/text-controls/text-field.h>

namespace Dali
{

namespace Toolkit
{

namespace DevelTextField
{

namespace Property
{
  enum
  {
      RENDERING_BACKEND = Dali::Toolkit::TextField::Property::RENDERING_BACKEND,
      TEXT = Dali::Toolkit::TextField::Property::TEXT,
      PLACEHOLDER_TEXT = Dali::Toolkit::TextField::Property::PLACEHOLDER_TEXT,
      PLACEHOLDER_TEXT_FOCUSED = Dali::Toolkit::TextField::Property::PLACEHOLDER_TEXT_FOCUSED,
      FONT_FAMILY = Dali::Toolkit::TextField::Property::FONT_FAMILY,
      FONT_STYLE = Dali::Toolkit::TextField::Property::FONT_STYLE,
      POINT_SIZE = Dali::Toolkit::TextField::Property::POINT_SIZE,
      MAX_LENGTH = Dali::Toolkit::TextField::Property::MAX_LENGTH,
      EXCEED_POLICY = Dali::Toolkit::TextField::Property::EXCEED_POLICY,
      HORIZONTAL_ALIGNMENT = Dali::Toolkit::TextField::Property::HORIZONTAL_ALIGNMENT,
      VERTICAL_ALIGNMENT = Dali::Toolkit::TextField::Property::VERTICAL_ALIGNMENT,
      TEXT_COLOR = Dali::Toolkit::TextField::Property::TEXT_COLOR,
      PLACEHOLDER_TEXT_COLOR = Dali::Toolkit::TextField::Property::PLACEHOLDER_TEXT_COLOR,
      RESERVED_PROPERTY_01 = Dali::Toolkit::TextField::Property::RESERVED_PROPERTY_01,
      RESERVED_PROPERTY_02 = Dali::Toolkit::TextField::Property::RESERVED_PROPERTY_02,
      PRIMARY_CURSOR_COLOR = Dali::Toolkit::TextField::Property::PRIMARY_CURSOR_COLOR,
      SECONDARY_CURSOR_COLOR = Dali::Toolkit::TextField::Property::SECONDARY_CURSOR_COLOR,
      ENABLE_CURSOR_BLINK = Dali::Toolkit::TextField::Property::ENABLE_CURSOR_BLINK,
      CURSOR_BLINK_INTERVAL = Dali::Toolkit::TextField::Property::CURSOR_BLINK_INTERVAL,
      CURSOR_BLINK_DURATION = Dali::Toolkit::TextField::Property::CURSOR_BLINK_DURATION,
      CURSOR_WIDTH = Dali::Toolkit::TextField::Property::CURSOR_WIDTH,
      GRAB_HANDLE_IMAGE = Dali::Toolkit::TextField::Property::GRAB_HANDLE_IMAGE,
      GRAB_HANDLE_PRESSED_IMAGE = Dali::Toolkit::TextField::Property::GRAB_HANDLE_PRESSED_IMAGE,
      SCROLL_THRESHOLD = Dali::Toolkit::TextField::Property::SCROLL_THRESHOLD,
      SCROLL_SPEED = Dali::Toolkit::TextField::Property::SCROLL_SPEED,
      SELECTION_HANDLE_IMAGE_LEFT = Dali::Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_LEFT,
      SELECTION_HANDLE_IMAGE_RIGHT = Dali::Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT,
      SELECTION_HANDLE_PRESSED_IMAGE_LEFT = Dali::Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT,
      SELECTION_HANDLE_PRESSED_IMAGE_RIGHT = Dali::Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT,
      SELECTION_HANDLE_MARKER_IMAGE_LEFT = Dali::Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT,
      SELECTION_HANDLE_MARKER_IMAGE_RIGHT = Dali::Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT,
      SELECTION_HIGHLIGHT_COLOR = Dali::Toolkit::TextField::Property::SELECTION_HIGHLIGHT_COLOR,
      DECORATION_BOUNDING_BOX = Dali::Toolkit::TextField::Property::DECORATION_BOUNDING_BOX,
      INPUT_METHOD_SETTINGS = Dali::Toolkit::TextField::Property::INPUT_METHOD_SETTINGS,
      INPUT_COLOR = Dali::Toolkit::TextField::Property::INPUT_COLOR,
      ENABLE_MARKUP = Dali::Toolkit::TextField::Property::ENABLE_MARKUP,
      INPUT_FONT_FAMILY = Dali::Toolkit::TextField::Property::INPUT_FONT_FAMILY,
      INPUT_FONT_STYLE = Dali::Toolkit::TextField::Property::INPUT_FONT_STYLE,
      INPUT_POINT_SIZE = Dali::Toolkit::TextField::Property::INPUT_POINT_SIZE,
      UNDERLINE = Dali::Toolkit::TextField::Property::UNDERLINE,
      INPUT_UNDERLINE = Dali::Toolkit::TextField::Property::INPUT_UNDERLINE,
      SHADOW = Dali::Toolkit::TextField::Property::SHADOW,
      INPUT_SHADOW = Dali::Toolkit::TextField::Property::INPUT_SHADOW,
      EMBOSS = Dali::Toolkit::TextField::Property::EMBOSS,
      INPUT_EMBOSS = Dali::Toolkit::TextField::Property::INPUT_EMBOSS,
      OUTLINE = Dali::Toolkit::TextField::Property::OUTLINE,
      INPUT_OUTLINE = Dali::Toolkit::TextField::Property::INPUT_OUTLINE,
      HIDDEN_INPUT_SETTINGS = Dali::Toolkit::TextField::Property::HIDDEN_INPUT_SETTINGS,
      PIXEL_SIZE = Dali::Toolkit::TextField::Property::PIXEL_SIZE,
      ENABLE_SELECTION = Dali::Toolkit::TextField::Property::ENABLE_SELECTION,
      PLACEHOLDER = Dali::Toolkit::TextField::Property::PLACEHOLDER,
      ELLIPSIS = Dali::Toolkit::TextField::Property::ELLIPSIS,

      /**
       * @brief Enables Text selection using Shift key.
       * @details Name "enableShiftSelection", type Property::BOOLEAN.
       */
      ENABLE_SHIFT_SELECTION = ELLIPSIS + 1,

      /**
       * @brief Enables the grab handles for text selection.
       * @details Name "enableGrabHandle", type Property::BOOLEAN.
       * @note The default value is true, which means the grab handles are enabled by default.
       */
      ENABLE_GRAB_HANDLE = ELLIPSIS + 2,

      /**
       * @brief Modifies the default text alignment to match the direction of the system language.
       * @details Name "matchSystemLanguageDirection", type (Property::BOOLEAN), Read/Write
       * @note The default value is false
       */
      MATCH_SYSTEM_LANGUAGE_DIRECTION = ELLIPSIS + 3,

      /**
        * @brief Enables the grab handle popup for text selection.
        * @details Name "enableGrabHandlePopup", type Property::BOOLEAN.
        * @note The default value is true, which means the grab handle popup is enabled by default.
        */
      ENABLE_GRAB_HANDLE_POPUP = ELLIPSIS + 4,

      /**
       * @brief The default text background parameters.
       * @details Name "textBackground", type Property::VECTOR4.
       * @note Use "textBackground" as property name to avoid conflict with Control's "background" property.
       * @note The default value is Color::TRANSPARENT.
       */
      BACKGROUND = ELLIPSIS + 5,


      /**
       * @brief The selected text in UTF-8 format.
       * @details Name "selectedText", type Property::STRING.
       * @note This property is read-only.
       */
      SELECTED_TEXT = ELLIPSIS + 6

  };
} // namespace Property

/**
 * @brief Return the input method context of TextField.
 *
 * @param[in] textField The instance of TextField.
 * @return InputMethodContext instance.
 */
DALI_TOOLKIT_API InputMethodContext GetInputMethodContext( TextField textField );

/**
 * @brief Select the whole text of TextField.
 *
 * @param[in] textField The instance of TextField.
 */
DALI_TOOLKIT_API void SelectWholeText( TextField textField );

/**
 * @brief Unselect the whole text of TextField.
 *
 * @param[in] textField The instance of TextField.
 */
DALI_TOOLKIT_API void SelectNone( TextField textField );

/**
 * @brief Get the selected text of TextField.
 *
 * @param[in] textField The instance of TextField.
 * @return Selected text in the TextField.
 */
DALI_TOOLKIT_API std::string SelectedText( TextField textField );

} // namespace DevelText

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_FIELD_DEVEL_H
