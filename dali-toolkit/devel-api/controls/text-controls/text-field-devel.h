#ifndef DALI_TOOLKIT_TEXT_FIELD_DEVEL_H
#define DALI_TOOLKIT_TEXT_FIELD_DEVEL_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/text-controls/text-field.h>

namespace Dali
{

namespace Toolkit
{

namespace DevelTextField
{

namespace Property
{
  enum Type
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
      SHADOW_OFFSET = Dali::Toolkit::TextField::Property::SHADOW_OFFSET,
      SHADOW_COLOR = Dali::Toolkit::TextField::Property::SHADOW_COLOR,
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

      /**
       * @brief Hides the input characters and instead shows a default character for password or pin entry.
       * @details name "hiddenInputSettings", type map.
       * @note Optional.
       * @see HiddenInput
       */
      HIDDEN_INPUT_SETTINGS = INPUT_OUTLINE + 1,

      /**
       * @brief The size of font in pixels.
       *
       * Conversion from Point size to Pixel size :
       *  Pixel size = Point size * DPI / 72
       * @details name "pixelSize", type float
       */
      PIXEL_SIZE = INPUT_OUTLINE + 2,

      /**
       * @brief Enables Text selection, such as the cursor, handle, clipboard, and highlight color.
       * @details name "enableSelection", type bool
       */
      ENABLE_SELECTION = INPUT_OUTLINE + 3,

      /**
       * @brief Sets the placeholder : text, color, font family, font style, point size, and pixel size.
       *
       * @code
       *   Property::Map propertyMap;
       *   propertyMap["placeholderText"] = "Setting Placeholder Text";
       *   propertyMap["placeholderColor"] = Color::RED;
       *   propertyMap["placeholderFontFamily"] = "Arial";
       *   propertyMap["placeholderPointSize"] = 12.0f;
       *
       *   Property::Map fontStyleMap;
       *   fontstyleMap.Insert( "weight", "bold" );
       *   fontstyleMap.Insert( "width", "condensed" );
       *   fontstyleMap.Insert( "slant", "italic" );
       *   propertyMap["placeholderFontStyle"] = fontStyleMap;
       *
       *   field.SetProperty( DevelTextField::Property::PLACEHOLDER, propertyMap );
       * @endcode
       *
       * @details name "placeholder", type MAP
       */
      PLACEHOLDER = INPUT_OUTLINE + 4
  };
} // namespace Property

} // namespace DevelText

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_FIELD_DEVEL_H
