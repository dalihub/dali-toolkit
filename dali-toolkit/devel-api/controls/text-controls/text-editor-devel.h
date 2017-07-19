#ifndef DALI_TOOLKIT_TEXT_EDITOR_DEVEL_H
#define DALI_TOOLKIT_TEXT_EDITOR_DEVEL_H

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

      /**
       * @brief name "smoothScroll", type bool
       * @details Enable or disable the smooth scroll animation
       */
      SMOOTH_SCROLL = INPUT_OUTLINE + 1,

      /**
       * @brief name "smoothScrollDuration", type float
       * @details Sets the duration of smooth scroll animation
       */
      SMOOTH_SCROLL_DURATION,

      /**
       * @brief name "enableScrollBar", type bool
       * @details Enable or disable the scroll bar
       */
      ENABLE_SCROLL_BAR,

      /**
       * @brief name "scrollBarShowDuration", type float
       * @details Sets the duration of scroll bar to show
       */
      SCROLL_BAR_SHOW_DURATION,

      /**
       * @brief name "scrollBarFadeDuration", type float
       * @details Sets the duration of scroll bar to fade out
       */
      SCROLL_BAR_FADE_DURATION,

      /**
       * @brief The size of font in pixels.
       *
       * Conversion from Point size to Pixel size :
       *  Pixel size = Point size * DPI / 72
       * @details name "pixelSize", type float
       */
      PIXEL_SIZE,

      /**
       * @brief The line count of text.
       * @details name "lineCount", type int
       * @note this property is read-only.
       */
      LINE_COUNT,

      /**
       * @brief The text to display when the TextEditor is empty and inactive.
       * @details name "placeholderText", type string
       */
      PLACEHOLDER_TEXT,

      /**
       * @brief The placeholder-text color.
       * @details name "placeholderTextColor", type vector4
       */
      PLACEHOLDER_TEXT_COLOR,

      /**
       * @brief Enables Text selection, such as the cursor, handle, clipboard, and highlight color.
       * @details name "enableSelection", type bool
       */
      ENABLE_SELECTION,

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
       *   editor.SetProperty( DevelTextEditor::Property::PLACEHOLDER, propertyMap );
       * @endcode
       *
       * @details name "placeholder", type MAP
       */
      PLACEHOLDER,

      /**
       * @brief line wrap mode when the text lines over layout width.
       * @details name "lineWrapMode", type string.
       */
      LINE_WRAP_MODE
  };
} // namespace Property

namespace Scroll
{
  enum Type
  {
    STARTED,   ///< Scrolling is started.
    FINISHED   ///< Scrolling is finished.
  };
} // namespace Scroll

typedef Signal< void ( TextEditor, Scroll::Type ) > ScrollStateChangedSignalType;

/**
 * @brief This signal is emitted when TextEditor scrolling is started or finished.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName( ScrollState::Type type );
 * @endcode
 * type: Whether the scrolling is started or finished.
 * @return The signal to connect to
 */
DALI_IMPORT_API ScrollStateChangedSignalType& ScrollStateChangedSignal( TextEditor textEditor );

} // namespace DevelTextEditor

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_EDITOR_DEVEL_H
