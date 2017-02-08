#ifndef __DALI_TOOLKIT_TEXT_FIELD_H__
#define __DALI_TOOLKIT_TEXT_FIELD_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
class DALI_IMPORT_API TextField : public Control
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
      RENDERING_BACKEND = PROPERTY_START_INDEX, ///< name "renderingBackend",                    The type or rendering e.g. bitmap-based,                                           type INT @SINCE_1_0.0
      TEXT,                                     ///< name "text",                                The text to display in UTF-8 format,                                               type STRING @SINCE_1_0.0
      PLACEHOLDER_TEXT,                         ///< name "placeholderText",                     The text to display when the TextField is empty and inactive,                      type STRING @SINCE_1_0.0
      PLACEHOLDER_TEXT_FOCUSED,                 ///< name "placeholderTextFocused",              The text to display when the TextField is empty with key-input focus,              type STRING @SINCE_1_0.0
      FONT_FAMILY,                              ///< name "fontFamily",                          The requested font family,                                                         type STRING @SINCE_1_0.0
      FONT_STYLE,                               ///< name "fontStyle",                           The requested font style,                                                          type STRING or MAP @SINCE_1_2.13
      POINT_SIZE,                               ///< name "pointSize",                           The size of font in points,                                                        type FLOAT @SINCE_1_0.0
      MAX_LENGTH,                               ///< name "maxLength"                            The maximum number of characters that can be inserted,                             type INTEGER @SINCE_1_0.0
      EXCEED_POLICY,                            ///< name "exceedPolicy"                         Specifies how the text is truncated when it does not fit,                          type INTEGER @SINCE_1_0.0
      HORIZONTAL_ALIGNMENT,                     ///< name "horizontalAlignment",                 The line horizontal alignment,                                                     type STRING,  values "BEGIN", "CENTER", "END" @SINCE_1_0.0
      VERTICAL_ALIGNMENT,                       ///< name "verticalAlignment",                   The line vertical alignment,                                                       type STRING,  values "TOP",   "CENTER", "BOTTOM" @SINCE_1_0.0
      TEXT_COLOR,                               ///< name "textColor",                           The text color,                                                                    type VECTOR4 @SINCE_1_0.0
      PLACEHOLDER_TEXT_COLOR,                   ///< name "placeholderTextColor",                The placeholder-text color,                                                        type VECTOR4 @SINCE_1_0.0
      SHADOW_OFFSET,                            ///< name "shadowOffset",                        The drop shadow offset 0 indicates no shadow,                                      type VECTOR2 @DEPRECATED_1_1.37 Use SHADOW instead.
      SHADOW_COLOR,                             ///< name "shadowColor",                         The color of a drop shadow,                                                        type VECTOR4 @DEPRECATED_1_1.37 Use SHADOW instead.
      PRIMARY_CURSOR_COLOR,                     ///< name "primaryCursorColor",                  The color to apply to the primary cursor,                                          type VECTOR4 @SINCE_1_0.0
      SECONDARY_CURSOR_COLOR,                   ///< name "secondaryCursorColor",                The color to apply to the secondary cursor,                                        type VECTOR4 @SINCE_1_0.0
      ENABLE_CURSOR_BLINK,                      ///< name "enableCursorBlink",                   Whether the cursor should blink or not,                                            type BOOLEAN @SINCE_1_0.0
      CURSOR_BLINK_INTERVAL,                    ///< name "cursorBlinkInterval",                 The time interval in seconds between cursor on/off states,                         type FLOAT @SINCE_1_0.0
      CURSOR_BLINK_DURATION,                    ///< name "cursorBlinkDuration",                 The cursor will stop blinking after this number of seconds (if non-zero),          type FLOAT @SINCE_1_0.0
      CURSOR_WIDTH,                             ///< name "cursorWidth",                         The cursor width,                                                                  type INTEGER @SINCE_1_0.0
      GRAB_HANDLE_IMAGE,                        ///< name "grabHandleImage",                     The image to display for the grab handle,                                          type STRING @SINCE_1_0.0
      GRAB_HANDLE_PRESSED_IMAGE,                ///< name "grabHandlePressedImage",              The image to display when the grab handle is pressed,                              type STRING @SINCE_1_0.0
      SCROLL_THRESHOLD,                         ///< name "scrollThreshold"                      Horizontal scrolling will occur if the cursor is this close to the control border, type FLOAT @SINCE_1_0.0
      SCROLL_SPEED,                             ///< name "scrollSpeed"                          The scroll speed in pixels per second,                                             type FLOAT @SINCE_1_0.0
      SELECTION_HANDLE_IMAGE_LEFT,              ///< name "selectionHandleImageLeft",            The image to display for the left selection handle,                                type MAP @SINCE_1_0.0
      SELECTION_HANDLE_IMAGE_RIGHT,             ///< name "selectionHandleImageRight",           The image to display for the right selection handle,                               type MAP @SINCE_1_0.0
      SELECTION_HANDLE_PRESSED_IMAGE_LEFT,      ///< name "selectionHandlePressedImageLeft",     The image to display when the left selection handle is pressed,                    type MAP @SINCE_1_0.0
      SELECTION_HANDLE_PRESSED_IMAGE_RIGHT,     ///< name "selectionHandlePressedImageRight",    The image to display when the right selection handle is pressed,                   type MAP @SINCE_1_0.0
      SELECTION_HANDLE_MARKER_IMAGE_LEFT,       ///< name "selectionHandleMarkerImageLeft",      The image to display for the left selection handle marker,                         type MAP @SINCE_1_0.0
      SELECTION_HANDLE_MARKER_IMAGE_RIGHT,      ///< name "selectionHandleMarkerImageRight",     The image to display for the right selection handle marker,                        type MAP @SINCE_1_0.0
      SELECTION_HIGHLIGHT_COLOR,                ///< name "selectionHighlightColor",             The color of the selection highlight,                                              type VECTOR4 @SINCE_1_0.0
      DECORATION_BOUNDING_BOX,                  ///< name "decorationBoundingBox",               The decorations (handles etc) will positioned within this area on-screen,          type RECTANGLE @SINCE_1_0.0
      INPUT_METHOD_SETTINGS,                    ///< name "inputMethodSettings",                 The settings to relating to the System's Input Method, Key and Value               type MAP @SINCE_1_0.0
      INPUT_COLOR,                              ///< name "inputColor",                          The color of the new input text,                                                   type VECTOR4 @SINCE_1_0.0
      ENABLE_MARKUP,                            ///< name "enableMarkup",                        Whether the mark-up processing is enabled.                                         type BOOLEAN @SINCE_1_0.0
      INPUT_FONT_FAMILY,                        ///< name "inputFontFamily",                     The font's family of the new input text,                                           type STRING @SINCE_1_0.0
      INPUT_FONT_STYLE,                         ///< name "inputFontStyle",                      The font's style of the new input text,                                            type STRING or MAP @SINCE_1_2.13
      INPUT_POINT_SIZE,                         ///< name "inputPointSize",                      The font's size of the new input text in points,                                   type FLOAT @SINCE_1_0.0
      UNDERLINE,                                ///< name "underline"                            The default underline parameters,                                                  type STRING or MAP @SINCE_1_2.13
      INPUT_UNDERLINE,                          ///< name "inputUnderline"                       The underline parameters of the new input text,                                    type STRING or MAP @SINCE_1_2.13
      SHADOW,                                   ///< name "shadow"                               The default shadow parameters,                                                     type STRING or MAP @SINCE_1_2.13
      INPUT_SHADOW,                             ///< name "inputShadow"                          The shadow parameters of the new input text,                                       type STRING or MAP @SINCE_1_2.13
      EMBOSS,                                   ///< name "emboss"                               The default emboss parameters,                                                     type STRING or MAP @SINCE_1_2.13
      INPUT_EMBOSS,                             ///< name "inputEmboss"                          The emboss parameters of the new input text,                                       type STRING or MAP @SINCE_1_2.13
      OUTLINE,                                  ///< name "outline"                              The default outline parameters,                                                    type STRING or MAP @SINCE_1_2.13
      INPUT_OUTLINE,                            ///< name "inputOutline"                         The outline parameters of the new input text,                                      type STRING or MAP @SINCE_1_2.13
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

#endif // __DALI_TOOLKIT_TEXT_FIELD_H__
