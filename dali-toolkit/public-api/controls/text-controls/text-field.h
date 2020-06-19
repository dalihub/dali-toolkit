#ifndef DALI_TOOLKIT_TEXT_FIELD_H
#define DALI_TOOLKIT_TEXT_FIELD_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
class TextField;
}
/**
 * @addtogroup dali_toolkit_controls_text_controls
 * @{
 */

/**
 * @brief A control which provides a single-line editable text field.
 *
 * Signals
 * | %Signal Name         | Method                         |                    |
 * |----------------------|--------------------------------|--------------------|
 * | textChanged          | @ref TextChangedSignal()       | @SINCE_1_0.0       |
 * | maxLengthReached     | @ref MaxLengthReachedSignal()  | @SINCE_1_0.0       |
 * | inputStyleChanged    | @ref InputStyleChangedSignal() | @SINCE_1_2_2       |
 */
class DALI_TOOLKIT_API TextField : public Control
{
public:

  /**
   * @brief The start and end property ranges for this control.
   * @SINCE_1_0.0
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1, ///< @SINCE_1_0.0
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices @SINCE_1_0.0
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the TextField class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    /**
     * @brief Enumeration for the instance of properties belonging to the TextField class.
     * @SINCE_1_0.0
     */
    enum
    {
      /**
       * @brief The type or rendering e.g. bitmap-based.
       * @details Name "renderingBackend", type Property::INTEGER.
       * @SINCE_1_0.0
       */
      RENDERING_BACKEND = PROPERTY_START_INDEX,

      /**
       * @brief The text to display in UTF-8 format.
       * @details Name "text", type Property::STRING.
       * @SINCE_1_0.0
       */
      TEXT,

      /**
       * @brief The text to display when the TextField is empty and inactive.
       * @details Name "placeholderText", type Property::STRING.
       * @SINCE_1_0.0
       */
      PLACEHOLDER_TEXT,

      /**
       * @brief The text to display when the TextField is empty with key-input focus.
       * @details Name "placeholderTextFocused", type Property::STRING.
       * @SINCE_1_0.0
       */
      PLACEHOLDER_TEXT_FOCUSED,

      /**
       * @brief The requested font family.
       * @details Name "fontFamily", type Property::STRING.
       * @SINCE_1_0.0
       */
      FONT_FAMILY,

      /**
       * @brief The requested font style
       * @details Name "fontStyle", type Property::STRING or Property::MAP.
       * @SINCE_1_2.13
       */
      FONT_STYLE,

      /**
       * @brief The size of font in points.
       * @details Name "pointSize", type Property::FLOAT.
       *          (Conversion from Pixel size to Point size : Point size = Pixel size * 72 / DPI).
       * @SINCE_1_0.0
       */
      POINT_SIZE,

      /**
       * @brief The maximum number of characters that can be inserted.
       * @details Name "maxLength", type Property::INTEGER.
       * @SINCE_1_0.0
       */
      MAX_LENGTH,

      /**
       * @brief Specifies how the text is truncated when it does not fit.
       * @details Name "exceedPolicy", type Property::INTEGER.
       * @SINCE_1_0.0
       */
      EXCEED_POLICY,

      /**
       * @brief The line horizontal alignment.
       * @details Name "horizontalAlignment", type Property::STRING or type HorizontalAlignment::Type (Property::INTEGER)
       *          Values "BEGIN", "CENTER", "END".
       * @note Return type is Property::STRING
       * @SINCE_1_0.0
       */
      HORIZONTAL_ALIGNMENT,

      /**
       * @brief The line vertical alignment.
       * @details Name "verticalAlignment", type Property::STRING type VerticalAlignment::Type (Property::INTEGER)
       *          Values "TOP",   "CENTER", "BOTTOM".
       * @note Return type is Property::STRING
       * @SINCE_1_0.0
       */
      VERTICAL_ALIGNMENT,

      /**
       * @brief The text color.
       * @details Name "textColor", type Property::VECTOR4.
       * @SINCE_1_0.0
       */
      TEXT_COLOR,

      /**
       * @brief The placeholder-text color.
       * @details Name "placeholderTextColor", type Property::VECTOR4.
       * @SINCE_1_0.0
       */
      PLACEHOLDER_TEXT_COLOR,

      /**
       * @brief The color to apply to the primary cursor.
       * @details Name "primaryCursorColor", type Property::VECTOR4.
       * @SINCE_1_0.0
       */
      PRIMARY_CURSOR_COLOR,

      /**
       * @brief The color to apply to the secondary cursor.
       * @details Name "secondaryCursorColor", type Property::VECTOR4.
       * @SINCE_1_0.0
       */
      SECONDARY_CURSOR_COLOR,

      /**
       * @brief Whether the cursor should blink or not.
       * @details Name "enableCursorBlink", type Property::BOOLEAN.
       * @SINCE_1_0.0
       */
      ENABLE_CURSOR_BLINK,

      /**
       * @brief The time interval in seconds between cursor on/off states.
       * @details Name "cursorBlinkInterval", type Property::FLOAT.
       * @SINCE_1_0.0
       */
      CURSOR_BLINK_INTERVAL,

      /**
       * @brief The cursor will stop blinking after this number of seconds (if non-zero)
       * @details Name "cursorBlinkDuration", type Property::FLOAT.
       * @SINCE_1_0.0
       */
      CURSOR_BLINK_DURATION,

      /**
       * @brief The cursor width.
       * @details Name "cursorWidth", type Property::INTEGER.
       * @SINCE_1_0.0
       */
      CURSOR_WIDTH,

      /**
       * @brief The image to display for the grab handle.
       * @details Name "grabHandleImage", type Property::STRING.
       * @SINCE_1_0.0
       */
      GRAB_HANDLE_IMAGE,

      /**
       * @brief The image to display when the grab handle is pressed
       * @details Name "grabHandlePressedImage", type Property::STRING.
       * @SINCE_1_0.0
       */
      GRAB_HANDLE_PRESSED_IMAGE,

      /**
       * @brief Horizontal scrolling will occur if the cursor is this close to the control border.
       * @details Name "scrollThreshold", type Property::FLOAT.
       * @SINCE_1_0.0
       */
      SCROLL_THRESHOLD,

      /**
       * @brief The scroll speed in pixels per second.
       * @details Name "scrollSpeed", type Property::FLOAT.
       * @SINCE_1_0.0
       */
      SCROLL_SPEED,

      /**
       * @brief The image to display for the left selection handle.
       * @details Name "selectionHandleImageLeft", type Property::MAP.
       * @SINCE_1_0.0
       */
      SELECTION_HANDLE_IMAGE_LEFT,

      /**
       * @brief The image to display for the right selection handle.
       * @details Name "selectionHandleImageRight", type Property::MAP.
       * @SINCE_1_0.0
       */
      SELECTION_HANDLE_IMAGE_RIGHT,

      /**
       * @brief The image to display when the left selection handle is pressed.
       * @details Name "selectionHandlePressedImageLeft", type Property::MAP.
       * @SINCE_1_0.0
       */
      SELECTION_HANDLE_PRESSED_IMAGE_LEFT,

      /**
       * @brief The image to display when the right selection handle is pressed.
       * @details Name "selectionHandlePressedImageRight", type Property::MAP.
       * @SINCE_1_0.0
       */
      SELECTION_HANDLE_PRESSED_IMAGE_RIGHT,

      /**
       * @brief The image to display for the left selection handle marker.
       * @details Name "selectionHandleMarkerImageLeft", type Property::MAP.
       * @SINCE_1_0.0
       */
      SELECTION_HANDLE_MARKER_IMAGE_LEFT,

      /**
       * @brief The image to display for the right selection handle marker.
       * @details Name "selectionHandleMarkerImageRight", type Property::MAP.
       * @SINCE_1_0.0
       */
      SELECTION_HANDLE_MARKER_IMAGE_RIGHT,

      /**
       * @brief The color of the selection highlight.
       * @details Name "selectionHighlightColor", type Property::VECTOR4.
       * @SINCE_1_0.0
       */
      SELECTION_HIGHLIGHT_COLOR,

      /**
       * @brief The decorations (handles etc) will positioned within this area on-screen.
       * @details Name "decorationBoundingBox", type Property::RECTANGLE.
       * @SINCE_1_0.0
       */
      DECORATION_BOUNDING_BOX,

      /**
       * @brief The settings to relating to the System's Input Method, Key and Value.
       * @details Name "inputMethodSettings", type Property::MAP.
       *
       * @note VARIATION key can be changed depending on PANEL_LAYOUT.
       * For example, when PANEL_LAYOUT key is InputMethod::PanelLayout::NORMAL,
       * then VARIATION would be among NORMAL, WITH_FILENAME, and WITH_PERSON_NAME in Dali::InputMethod::NormalLayout.
       * For more information, see Dali::InputMethod::Category.
       *
       * Example Usage:
       * @code
       *   Property::Map propertyMap;
       *   InputMethod::PanelLayout::Type panelLayout = InputMethod::PanelLayout::NUMBER;
       *   InputMethod::AutoCapital::Type autoCapital = InputMethod::AutoCapital::WORD;
       *   InputMethod::ButtonAction::Type buttonAction = InputMethod::ButtonAction::GO;
       *   int inputVariation = 1;
       *   propertyMap["PANEL_LAYOUT"] = panelLayout;
       *   propertyMap["AUTO_CAPITALIZE"] = autoCapital;
       *   propertyMap["BUTTON_ACTION"] = buttonAction;
       *   propertyMap["VARIATION"] = inputVariation;
       *
       *   field.SetProperty( TextField::Property::INPUT_METHOD_SETTINGS, propertyMap );
       * @endcode
       * @SINCE_1_0.0
       */
      INPUT_METHOD_SETTINGS,

      /**
       * @brief The color of the new input text.
       * @details Name "inputColor", type Property::VECTOR4.
       * @SINCE_1_0.0
       */
      INPUT_COLOR,

      /**
       * @brief Whether the mark-up processing is enabled.
       * @details Name "enableMarkup", type Property::BOOLEAN.
       * @SINCE_1_0.0
       */
      ENABLE_MARKUP,

      /**
       * @brief The font's family of the new input text.
       * @details Name "inputFontFamily", type Property::STRING.
       * @SINCE_1_0.0
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
       * @SINCE_1_0.0
       */
      INPUT_POINT_SIZE,

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
       * @brief Hides the input characters and instead shows a default character for password or pin entry.
       * @details Name "hiddenInputSettings", type Property::MAP.
       * @SINCE_1_2.60
       * @note Optional.
       * @see HiddenInput::Property
       */
      HIDDEN_INPUT_SETTINGS,

      /**
       * @brief The size of font in pixels.
       * @details Name "pixelSize", type Property::FLOAT.
       *          Conversion from Point size to Pixel size:
       *           Pixel size = Point size * DPI / 72
       * @SINCE_1_2.60
       */
      PIXEL_SIZE,

      /**
       * @brief Enables Text selection, such as the cursor, handle, clipboard, and highlight color.
       * @details Name "enableSelection", type Property::BOOLEAN.
       * @SINCE_1_2.60
       */
      ENABLE_SELECTION,

      /**
       * @brief Sets the placeholder : text, color, font family, font style, point size, and pixel size.
       * @details Name "placeholder", type Property::MAP.
       * Example Usage:
       * @code
       *   Property::Map propertyMap;
       *   propertyMap[ Text::PlaceHolder::Property::TEXT ] = "Setting Placeholder Text";
       *   propertyMap[ Text::PlaceHolder::Property::TEXT_FOCUSED] = "Setting Placeholder Text Focused";
       *   propertyMap[ Text::PlaceHolder::Property::COLOR] = Color::RED;
       *   propertyMap[ Text::PlaceHolder::Property::FONT_FAMILY ] = "Arial";
       *   propertyMap[ Text::PlaceHolder::Property::POINT_SIZE ] = 12.0f;
       *   propertyMap[ Text::PlaceHolder::Property::ELLIPSIS ] = true;
       *
       *   Property::Map fontStyleMap;
       *   fontStyleMap.Insert( "weight", "bold" );
       *   fontStyleMap.Insert( "width", "condensed" );
       *   fontStyleMap.Insert( "slant", "italic" );
       *   propertyMap[ Text::PlaceHolder::Property::FONT_STYLE] = fontStyleMap;
       *
       *   field.SetProperty( TextField::Property::PLACEHOLDER, propertyMap );
       * @endcode
       * @SINCE_1_2.60
       */
      PLACEHOLDER,

      /**
       * @brief Whether we should show the ellipsis if it is required.
       * @details Name "ellipsis", type Property::BOOLEAN.
       * @SINCE_1_2.60
       * @note PLACEHOLDER map is used to add ellipsis to placeholder text.
       */
      ELLIPSIS
    };
  };

  /**
   * @brief Enumeration for specifying how the text is truncated when it does not fit.
   *
   * The default value is \e EXCEED_POLICY_CLIP.
   * @SINCE_1_0.0
   */
  enum ExceedPolicy
  {
    EXCEED_POLICY_ORIGINAL,        ///< The text will be display at original size, and may exceed the TextField boundary. @SINCE_1_0.0
    EXCEED_POLICY_CLIP             ///< The end of text will be clipped to fit within the TextField. @SINCE_1_0.0
  };

  /**
   * @brief Mask used by the signal InputStyleChangedSignal(). Notifies which parameters of the input style have changed.
   *
   * @SINCE_1_2_2
   */
  struct InputStyle
  {
  /**
   * @brief Mask used by the signal InputStyleChangedSignal().
   *
   * @SINCE_1_2_2
   */
    enum Mask
    {
      NONE         = 0x0000, ///< @SINCE_1_2_2
      COLOR        = 0x0001, ///< @SINCE_1_2_2
      FONT_FAMILY  = 0x0002, ///< @SINCE_1_2_2
      POINT_SIZE   = 0x0004, ///< @SINCE_1_2_2
      FONT_STYLE   = 0x0008, ///< @SINCE_1_2_2
      UNDERLINE    = 0x0010, ///< @SINCE_1_2_2
      SHADOW       = 0x0020, ///< @SINCE_1_2_2
      EMBOSS       = 0x0040, ///< @SINCE_1_2_2
      OUTLINE      = 0x0080  ///< @SINCE_1_2_2
    };
  };

  // Type Defs

  /**
   * @brief Text changed signal type.
   * @SINCE_1_0.0
   */
  typedef Signal<void ( TextField ) > TextChangedSignalType;

  /**
   * @brief Max Characters Exceed signal type.
   * @SINCE_1_0.0
   */
  typedef Signal<void ( TextField ) > MaxLengthReachedSignalType;

  /**
   * @brief Input Style changed signal type.
   * @SINCE_1_2_2
   */
  typedef Signal<void ( TextField, InputStyle::Mask ) > InputStyleChangedSignalType;

  /**
   * @brief Creates the TextField control.
   * @SINCE_1_0.0
   * @return A handle to the TextField control
   */
  static TextField New();

  /**
   * @brief Creates an empty handle.
   * @SINCE_1_0.0
   */
  TextField();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_1_0.0
   * @param[in] handle The handle to copy from
   */
  TextField( const TextField& handle );

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] handle The handle to copy from
   * @return A reference to this
   */
  TextField& operator=( const TextField& handle );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~TextField();

  /**
   * @brief Downcasts a handle to TextField.
   *
   * If the BaseHandle points is a TextField, the downcast returns a valid handle.
   * If not, the returned handle is left empty.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a TextField or an empty handle
   */
  static TextField DownCast( BaseHandle handle );

  // Signals

  /**
   * @brief This signal is emitted when the text changes.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( TextField textField );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to.
   */
  TextChangedSignalType& TextChangedSignal();

  /**
   * @brief This signal is emitted when inserted text exceeds the maximum character limit.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( TextField textField );
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   */
  MaxLengthReachedSignalType& MaxLengthReachedSignal();

  /**
   * @brief This signal is emitted when the input style is updated as a consequence of a change in the cursor position.
   * i.e. The signal is not emitted when the input style is updated through the property system.
   *
   * A callback of the following type may be connected. The @p mask parameter notifies which parts of the style have changed.
   * @code
   *   void YourCallbackName( TextField textField, TextField::InputStyle::Mask mask );
   * @endcode
   *
   * @SINCE_1_2_2
   * @return The signal to connect to
   */
  InputStyleChangedSignalType& InputStyleChangedSignal();

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_0.0
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL TextField( Internal::TextField& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @SINCE_1_0.0
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL TextField( Dali::Internal::CustomActor* internal );
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_FIELD_H
