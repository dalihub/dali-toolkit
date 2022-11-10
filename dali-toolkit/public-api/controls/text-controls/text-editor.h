#ifndef DALI_TOOLKIT_TEXT_EDITOR_H
#define DALI_TOOLKIT_TEXT_EDITOR_H

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
class TextEditor;
}
/**
 * @addtogroup dali_toolkit_controls_text_controls
 * @{
 */

/**
 * @brief A control which provides a multi-line editable text editor.
 *
 * Signals
 * | %Signal Name         | Method                         |                    |
 * |----------------------|--------------------------------|--------------------|
 * | textChanged          | @ref TextChangedSignal()       | @SINCE_1_1.37      |
 * | inputStyleChanged    | @ref InputStyleChangedSignal() | @SINCE_1_2_2       |
 *
 */
class DALI_TOOLKIT_API TextEditor : public Control
{
public:
  /**
   * @brief Enumeration for the start and end property ranges for this control.
   * @SINCE_1_1.37
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000 ///< Reserve property indices
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the TextEditor class.
   * @SINCE_1_1.37
   */
  struct Property
  {
    /**
     * @brief Enumeration for the instance of properties belonging to the TextEditor class.
     * @SINCE_1_1.37
     */
    enum
    {
      /**
       * @brief The text to display in UTF-8 format.
       * @details Name "text", type Property::STRING.
       * @SINCE_1_1.37
       */
      TEXT = PROPERTY_START_INDEX,

      /**
       * @brief The text color.
       * @details Name "textColor", type Property::VECTOR4.
       * @SINCE_1_1.37
       */
      TEXT_COLOR,

      /**
       * @brief The requested font family.
       * @details Name "fontFamily", type Property::STRING.
       * @SINCE_1_1.37
       */
      FONT_FAMILY,

      /**
       * @brief The requested font style.
       * @details Name "fontStyle", type Property::STRING or Property::MAP.
       * @SINCE_1_2.13
       */
      FONT_STYLE,

      /**
       * @brief The size of font in points.
       * @details Name "pointSize", type Property::FLOAT.
       *          Conversion from Pixel size to Point size : Point size = Pixel size * 72 / DPI.
       * @SINCE_1_1.37
       */
      POINT_SIZE,

      /**
       * @brief The text horizontal alignment.
       * @details Name "horizontalAlignment", type Property::STRING or type HorizontalAlignment::Type (Property::INTEGER)
       *          Values "BEGIN" "CENTER" "END".
       * @note Return type is Property::STRING
       * @SINCE_1_1.37
       */
      HORIZONTAL_ALIGNMENT,

      /**
       * @brief Vertical scrolling will occur if the cursor is this close to the control border.
       * @details Name "scrollThreshold", type Property::FLOAT.
       * @SINCE_1_1.37
       */
      SCROLL_THRESHOLD,

      /**
       * @brief The scroll speed in pixels per second.
       * @details Name "scrollSpeed", type Property::FLOAT.
       * @SINCE_1_1.37
       */
      SCROLL_SPEED,

      /**
       * @brief The color to apply to the primary cursor.
       * @details Name "primaryCursorColor", type Property::VECTOR4.
       * @SINCE_1_1.37
       */
      PRIMARY_CURSOR_COLOR,

      /**
       * @brief The color to apply to the secondary cursor.
       * @details Name "secondaryCursorColor", type Property::VECTOR4.
       * @SINCE_1_1.37
       */
      SECONDARY_CURSOR_COLOR,

      /**
       * @brief Whether the cursor should blink or not.
       * @details Name "enableCursorBlink", type Property::BOOLEAN.
       * @SINCE_1_1.37
       */
      ENABLE_CURSOR_BLINK,

      /**
       * @brief The time interval in seconds between cursor on/off states.
       * @details Name "cursorBlinkInterval", type Property::FLOAT.
       * @SINCE_1_1.37
       */
      CURSOR_BLINK_INTERVAL,

      /**
       * @brief The cursor will stop blinking after this number of seconds (if non-zero).
       * @details Name "cursorBlinkDuration", type Property::FLOAT.
       * @SINCE_1_1.37
       */
      CURSOR_BLINK_DURATION,

      /**
       * @brief The cursor width.
       * @details Name "cursorWidth", type Property::INTEGER.
       * @SINCE_1_1.37
       */
      CURSOR_WIDTH,

      /**
       * @brief The image to display for the grab handle.
       * @details Name "grabHandleImage", type Property::STRING.
       * @SINCE_1_1.37
       */
      GRAB_HANDLE_IMAGE,

      /**
       * @brief The image to display when the grab handle is pressed.
       * @details Name "grabHandlePressedImage", type Property::STRING.
       * @SINCE_1_1.37
       */
      GRAB_HANDLE_PRESSED_IMAGE,

      /**
       * @brief The image to display for the left selection handle.
       * @details Name "selectionHandleImageLeft", type Property::MAP.
       * @SINCE_1_1.37
       */
      SELECTION_HANDLE_IMAGE_LEFT,

      /**
       * @brief The image to display for the right selection handle.
       * @details Name "selectionHandleImageRight", type Property::MAP.
       * @SINCE_1_1.37
       */
      SELECTION_HANDLE_IMAGE_RIGHT,

      /**
       * @brief The image to display when the left selection handle is pressed.
       * @details Name "selectionHandlePressedImageLeft", type Property::MAP.
       * @SINCE_1_1.37
       */
      SELECTION_HANDLE_PRESSED_IMAGE_LEFT,

      /**
       * @brief The image to display when the right selection handle is pressed.
       * @details Name "selectionHandlePressedImageRight", type Property::MAP.
       * @SINCE_1_1.37
       */
      SELECTION_HANDLE_PRESSED_IMAGE_RIGHT,

      /**
       * @brief The image to display for the left selection handle marker.
       * @details Name "selectionHandleMarkerImageLeft", type Property::MAP.
       * @SINCE_1_1.37
       */
      SELECTION_HANDLE_MARKER_IMAGE_LEFT,

      /**
       * @brief The image to display for the right selection handle marker.
       * @details Name "selectionHandleMarkerImageRight", type Property::MAP.
       * @SINCE_1_1.37
       */
      SELECTION_HANDLE_MARKER_IMAGE_RIGHT,

      /**
       * @brief The color of the selection highlight.
       * @details Name "selectionHighlightColor", type Property::VECTOR4.
       * @SINCE_1_1.37
       */
      SELECTION_HIGHLIGHT_COLOR,

      /**
       * @brief The decorations (handles etc) will positioned within this area on-screen.
       * @details Name "decorationBoundingBox", type Property::RECTANGLE.
       * @SINCE_1_1.37
       */
      DECORATION_BOUNDING_BOX,

      /**
       * @brief Whether the mark-up processing is enabled.
       * @details Name "enableMarkup", type Property::BOOLEAN.
       * @SINCE_1_1.37
       */
      ENABLE_MARKUP,

      /**
       * @brief The color of the new input text.
       * @details Name "inputColor", type Property::VECTOR4.
       * @SINCE_1_1.37
       */
      INPUT_COLOR,

      /**
       * @brief The font's family of the new input text.
       * @details Name "inputFontFamily", type Property::STRING.
       * @SINCE_1_1.37
       */
      INPUT_FONT_FAMILY,

      /**
       * @brief The font's style of the new input text.
       * @details Name "inputFontStyle", type Property::MAP.
       * @SINCE_1_2.13
       */
      INPUT_FONT_STYLE,

      /**
       * @brief The font's size of the new input text in points.
       * @details Name "inputPointSize", type Property::FLOAT.
       * @SINCE_1_1.37
       */
      INPUT_POINT_SIZE,

      /**
       * @brief The default extra space between lines in points.
       * @details Name "lineSpacing", type Property::FLOAT.
       * @SINCE_1_1.37
       */
      LINE_SPACING,

      /**
       * @brief The extra space between lines in points.
       * @details Name "inputLineSpacing",  type Property::FLOAT.
       * @SINCE_1_1.37
       * @note This affects the whole paragraph where the new input text is inserted.
       */
      INPUT_LINE_SPACING,

      /**
       * @copydoc Dali::Toolkit::TextLabel::Property::UNDERLINE
       */
      UNDERLINE,

      /**
       * @brief The underline parameters of the new input text.
       * @details Name "inputUnderline", type Property::MAP.
       * @SINCE_1_2.13
       */
      INPUT_UNDERLINE,

      /**
       * @copydoc Dali::Toolkit::TextLabel::Property::SHADOW
       */
      SHADOW,

      /**
       * @brief The shadow parameters of the new input text.
       * @details Name "inputShadow", type Property::MAP.
       * @SINCE_1_2.13
       */
      INPUT_SHADOW,

      /**
       * @brief The default emboss parameters.
       * @details Name "emboss", type Property::MAP.
       * @SINCE_1_2.13
       */
      EMBOSS,

      /**
       * @brief The emboss parameters of the new input text.
       * @details Name "inputEmboss", type Property::MAP.
       * @SINCE_1_2.13
       */
      INPUT_EMBOSS,

      /**
       * @copydoc Dali::Toolkit::TextLabel::Property::OUTLINE
       */
      OUTLINE,

      /**
       * @brief The outline parameters of the new input text.
       * @details Name "inputOutline", type Property::MAP.
       * @SINCE_1_2.13
       */
      INPUT_OUTLINE,

      /**
       * @brief Enable or disable the smooth scroll animation.
       * @details Name "smoothScroll", type Property::BOOLEAN.
       * @SINCE_1_2.60
       */
      SMOOTH_SCROLL,

      /**
       * @brief Sets the duration of smooth scroll animation.
       * @details Name "smoothScrollDuration", type Property::FLOAT.
       * @SINCE_1_2.60
       */
      SMOOTH_SCROLL_DURATION,

      /**
       * @brief Enable or disable the scroll bar.
       * @details Name "enableScrollBar", type Property::BOOLEAN.
       * @SINCE_1_2.60
       */
      ENABLE_SCROLL_BAR,

      /**
       * @brief Sets the duration of scroll bar to show.
       * @details Name "scrollBarShowDuration", type Property::FLOAT.
       * @SINCE_1_2.60
       */
      SCROLL_BAR_SHOW_DURATION,

      /**
       * @brief Sets the duration of scroll bar to fade out.
       * @details Name "scrollBarFadeDuration", type Property::FLOAT.
       * @SINCE_1_2.60
       */
      SCROLL_BAR_FADE_DURATION,

      /**
       * @brief The size of font in pixels.
       * @details Name "pixelSize", type Property::FLOAT.
       *          Conversion from Point size to Pixel size:
       *           Pixel size = Point size * DPI / 72
       * @SINCE_1_2.60
       */
      PIXEL_SIZE,

      /**
       * @brief The line count of text.
       * @details Name "lineCount", type Property::INTEGER.
       * @SINCE_1_2.60
       * @note This property is read-only.
       */
      LINE_COUNT,

      /**
       * @brief Enables Text selection, such as the cursor, handle, clipboard, and highlight color.
       * @details Name "enableSelection", type Property::BOOLEAN.
       * @SINCE_1_2.60
       */
      ENABLE_SELECTION,

      /**
       * @brief Sets the placeholder : text, color, font family, font style, point size, and pixel size.
       * @details Name "placeholder", type Property::MAP.
       * Example:
       * @code
       *   Property::Map propertyMap;
       *   propertyMap["placeholderText"] = "Setting Placeholder Text";
       *   propertyMap["placeholderTextFocused"] = "Setting Placeholder Text Focused";
       *   propertyMap["placeholderColor"] = Color::RED;
       *   propertyMap["placeholderFontFamily"] = "Arial";
       *   propertyMap["placeholderPointSize"] = 12.0f;
       *
       *   Property::Map fontStyleMap;
       *   fontStyleMap.Insert( "weight", "bold" );
       *   fontStyleMap.Insert( "width", "condensed" );
       *   fontStyleMap.Insert( "slant", "italic" );
       *   propertyMap["placeholderFontStyle"] = fontStyleMap;
       *
       *   editor.SetProperty( TextEditor::Property::PLACEHOLDER, propertyMap );
       * @endcode
       *
       * @SINCE_1_2.60
       */
      PLACEHOLDER,

      /**
       * @brief Line wrap mode when text lines are greater than the layout width.
       * @details Name "lineWrapMode", type Text::LineWrap::Mode (Text::Property::INTEGER) or Property::STRING.
       * @SINCE_1_2.60
       * @note Default is Text::LineWrap::WORD.
       * @note Return type is Text::LineWrap::Mode (Text::Property::INTEGER).
       * @see Text::LineWrap
       */
      LINE_WRAP_MODE,
    };
  };

  /**
   * @brief Mask used by the signal InputStyleChangedSignal(). Notifies which parameters of the input style have changed.
   *
   * @SINCE_1_2_2
   */
  struct InputStyle
  {
    /**
     * @brief Enumeration for mask used by the signal InputStyleChangedSignal().
     * @SINCE_1_2_2
     */
    enum Mask
    {
      NONE          = 0x0000, ///< @SINCE_1_2_2
      COLOR         = 0x0001, ///< @SINCE_1_2_2
      FONT_FAMILY   = 0x0002, ///< @SINCE_1_2_2
      POINT_SIZE    = 0x0004, ///< @SINCE_1_2_2
      FONT_STYLE    = 0x0008, ///< @SINCE_1_2_2
      LINE_SPACING  = 0x0010, ///< @SINCE_1_2_2
      UNDERLINE     = 0x0020, ///< @SINCE_1_2_2
      SHADOW        = 0x0040, ///< @SINCE_1_2_2
      EMBOSS        = 0x0080, ///< @SINCE_1_2_2
      OUTLINE       = 0x0100, ///< @SINCE_1_2_2
      STRIKETHROUGH = 0x0200,
    };
  };

  /**
   * @brief Enumerations for the type of scrolling.
   * @SINCE_1_2.60
   * @see ScrollStateChangedSignal()
   */
  struct Scroll
  {
    /**
     * @brief Enumerations for the type of scrolling.
     * @SINCE_1_2.60
     * @see ScrollStateChangedSignal()
     */
    enum Type
    {
      STARTED, ///< Scrolling has started. @SINCE_1_2.60
      FINISHED ///< Scrolling has finished. @SINCE_1_2.60
    };
  };

  // Type Defs

  /**
   * @brief Text changed signal type.
   * @SINCE_1_1.37
   */
  typedef Signal<void(TextEditor)> TextChangedSignalType;

  /**
   * @brief Input Style changed signal type.
   * @SINCE_1_2_2
   */
  typedef Signal<void(TextEditor, InputStyle::Mask)> InputStyleChangedSignalType;

  /**
   * @brief Scroll state changed signal type.
   * @SINCE_1_2.60
   */
  typedef Signal<void(TextEditor, Scroll::Type)> ScrollStateChangedSignalType;

  /**
   * @brief Creates the TextEditor control.
   *
   * @SINCE_1_1.37
   * @return A handle to the TextEditor control
   */
  static TextEditor
  New();

  /**
   * @brief Creates the TextEditor control with additional behaviour.
   *
   * @SINCE_2_2.3
   * @param[in] additionalBehaviour Additional control behaviour
   * @return A handle to the TextEditor control
   */
  static TextEditor New(ControlBehaviour additionalBehaviour);

  /**
   * @brief Creates an empty handle.
   *
   * @SINCE_1_1.37
   */
  TextEditor();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_1_1.37
   * @param[in] handle The handle to copy from
   */
  TextEditor(const TextEditor& handle);

  /**
   * @brief Move constructor
   * @SINCE_1_9.23
   *
   * @param[in] rhs A reference to the moved handle
   */
  TextEditor(TextEditor&& rhs);

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_1.37
   * @param[in] handle The handle to copy from
   * @return A reference to this
   */
  TextEditor& operator=(const TextEditor& handle);

  /**
   * @brief Move assignment
   * @SINCE_1_9.23
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  TextEditor& operator=(TextEditor&& rhs);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_1.37
   */
  ~TextEditor();

  /**
   * @brief Downcasts a handle to TextEditor.
   *
   * If the BaseHandle points is a TextEditor, the downcast returns a valid handle.
   * If not, the returned handle is left empty.
   *
   * @SINCE_1_1.37
   * @param[in] handle Handle to an object
   * @return Handle to a TextEditor or an empty handle
   */
  static TextEditor
  DownCast(BaseHandle handle);

  // Signals

  /**
   * @brief This signal is emitted when the text changes.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( TextEditor textEditor );
   * @endcode
   *
   * @SINCE_1_1.37
   * @return The signal to connect to
   */
  TextChangedSignalType&
  TextChangedSignal();

  /**
   * @brief This signal is emitted when the input style is updated as a consequence of a change in the cursor position.
   * i.e. The signal is not emitted when the input style is updated through the property system.
   *
   * A callback of the following type may be connected. The @p mask parameter notifies which parts of the style have changed.
   * @code
   *   void YourCallbackName( TextEditor textEditor, TextEditor::InputStyle::Mask mask );
   * @endcode
   *
   * @SINCE_1_2_2
   * @return The signal to connect to
   */
  InputStyleChangedSignalType&
  InputStyleChangedSignal();

  /**
   * @brief This signal is emitted when TextEditor scrolling is started or finished.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Scroll::Type type );
   * @endcode
   * type: Whether the scrolling is started or finished.
   *
   * @SINCE_1_2.60
   * @return The signal to connect to
   */
  ScrollStateChangedSignalType&
  ScrollStateChangedSignal();

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_1.37
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL
  TextEditor(Internal::TextEditor& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @SINCE_1_1.37
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL
  TextEditor(Dali::Internal::CustomActor* internal);
  /// @endcond
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_EDITOR_H
