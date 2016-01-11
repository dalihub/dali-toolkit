#ifndef __DALI_TOOLKIT_TEXT_FIELD_H__
#define __DALI_TOOLKIT_TEXT_FIELD_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 *  * Signals
 * | %Signal Name         | Method                                              |
 * |----------------------|-----------------------------------------------------|
 * | textChanged          | @ref TextChangedSignal()                            |
 * | maxLengthReached     | @ref MaxLengthReachedSignal()                       |
 *
 */
class DALI_IMPORT_API TextField : public Control
{
public:

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the TextLabel class.
   */
  struct Property
  {
    enum
    {
      RENDERING_BACKEND = PROPERTY_START_INDEX, ///< name "renderingBackend",                    The type or rendering e.g. bitmap-based,                                  type INT
      TEXT,                                     ///< name "text",                                The text to display in UTF-8 format,                                      type STRING
      PLACEHOLDER_TEXT,                         ///< name "placeholderText",                     The text to display when the TextField is empty and inactive,             type STRING
      PLACEHOLDER_TEXT_FOCUSED,                 ///< name "placeholderTextFocused",              The text to display when the TextField is empty with key-input focus,     type STRING
      FONT_FAMILY,                              ///< name "fontFamily",                          The requested font family,                                                type STRING
      FONT_STYLE,                               ///< name "fontStyle",                           The requested font style,                                                 type STRING
      POINT_SIZE,                               ///< name "pointSize",                           The size of font in points,                                               type FLOAT
      MAX_LENGTH,                               ///< name "maxLength"                            The maximum number of characters that can be inserted,                    type INTEGER
      EXCEED_POLICY,                            ///< name "exceedPolicy"                         Specifies how the text is truncated when it does not fit,                 type INTEGER
      HORIZONTAL_ALIGNMENT,                     ///< name "horizontalAlignment",                 The line horizontal alignment,                                            type STRING,  values "BEGIN", "CENTER", "END"
      VERTICAL_ALIGNMENT,                       ///< name "verticalAlignment",                   The line vertical alignment,                                              type STRING,  values "TOP",   "CENTER", "BOTTOM"
      TEXT_COLOR,                               ///< name "textColor",                           The text color,                                                           type VECTOR4
      PLACEHOLDER_TEXT_COLOR,                   ///< name "placeholderTextColor",                The placeholder-text color,                                               type VECTOR4
      SHADOW_OFFSET,                            ///< name "shadowOffset",                        The drop shadow offset 0 indicates no shadow,                             type VECTOR2
      SHADOW_COLOR,                             ///< name "shadowColor",                         The color of a drop shadow,                                               type VECTOR4
      PRIMARY_CURSOR_COLOR,                     ///< name "primaryCursorColor",                  The color to apply to the primary cursor,                                 type VECTOR4
      SECONDARY_CURSOR_COLOR,                   ///< name "secondaryCursorColor",                The color to apply to the secondary cursor,                               type VECTOR4
      ENABLE_CURSOR_BLINK,                      ///< name "enableCursorBlink",                   Whether the cursor should blink or not,                                   type BOOLEAN
      CURSOR_BLINK_INTERVAL,                    ///< name "cursorBlinkInterval",                 The time interval in seconds between cursor on/off states,                type FLOAT
      CURSOR_BLINK_DURATION,                    ///< name "cursorBlinkDuration",                 The cursor will stop blinking after this number of seconds (if non-zero), type FLOAT
      CURSOR_WIDTH,                             ///< name "cursorWidth",                         The cursor width,                                                         type INTEGER
      GRAB_HANDLE_IMAGE,                        ///< name "grabHandleImage",                     The image to display for the grab handle,                                 type STRING
      GRAB_HANDLE_PRESSED_IMAGE,                ///< name "grabHandlePressedImage",              The image to display when the grab handle is pressed,                     type STRING
      SCROLL_THRESHOLD,                         ///< name "scrollThreshold"                      Scrolling will occur if the cursor is this close to the control border,   type FLOAT
      SCROLL_SPEED,                             ///< name "scrollSpeed"                          The scroll speed in pixels per second,                                    type FLOAT
      SELECTION_HANDLE_IMAGE_LEFT,              ///< name "selectionHandleImageLeft",            The image to display for the left selection handle,                       type MAP
      SELECTION_HANDLE_IMAGE_RIGHT,             ///< name "selectionHandleImageRight",           The image to display for the right selection handle,                      type MAP
      SELECTION_HANDLE_PRESSED_IMAGE_LEFT,      ///< name "selectionHandlePressedImageLeft",     The image to display when the left selection handle is pressed,           type MAP
      SELECTION_HANDLE_PRESSED_IMAGE_RIGHT,     ///< name "selectionHandlePressedImageRight",    The image to display when the right selection handle is pressed,          type MAP
      SELECTION_HANDLE_MARKER_IMAGE_LEFT,       ///< name "selectionHandleMarkerImageLeft",      The image to display for the left selection handle marker,                type MAP
      SELECTION_HANDLE_MARKER_IMAGE_RIGHT,      ///< name "selectionHandleMarkerImageRight",     The image to display for the right selection handle marker,               type MAP
      SELECTION_HIGHLIGHT_COLOR,                ///< name "selectionHighlightColor",             The color of the selection highlight,                                     type VECTOR4
      DECORATION_BOUNDING_BOX,                  ///< name "decorationBoundingBox",               The decorations (handles etc) will positioned within this area on-screen, type RECTANGLE
      INPUT_METHOD_SETTINGS,                    ///< name "inputMethodSettings",                 The settings to relating to the System's Input Method, Key and Value      type MAP
      INPUT_COLOR,                              ///< name "inputColor",                          The color of the new input text,                                          type VECTOR4
      ENABLE_MARKUP,                            ///< name "enableMarkup",                        Whether the mark-up processing is enabled.                                type BOOLEAN
      INPUT_FONT_FAMILY,                        ///< name "inputFontFamily",                     The font's family of the new input text,                                  type STRING
      INPUT_FONT_STYLE,                         ///< name "inputFontStyle",                      The font's style of the new input text,                                   type STRING
      INPUT_POINT_SIZE                          ///< name "inputPointSize",                      The font's size of the new input text in points,                          type FLOAT
    };
  };

  /**
   * @brief Specifies how the text is truncated when it does not fit
   *
   * The default value is \e EXCEED_POLICY_CLIP.
   */
  enum ExceedPolicy
  {
    EXCEED_POLICY_ORIGINAL,        ///< The text will be display at original size, and may exceed the TextField boundary.
    EXCEED_POLICY_CLIP             ///< The end of text will be clipped to fit within the TextField.
  };

  // Type Defs

  /// @brief Max Characters Exceed signal type;
  typedef Signal<void ( TextField ) > TextChangedSignalType;
  typedef Signal<void ( TextField ) > MaxLengthReachedSignalType;

  /**
   * Create the TextField control.
   * @return A handle to the TextField control.
   */
  static TextField New();

  /**
   * @brief Creates an empty handle.
   */
  TextField();

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle The handle to copy from.
   */
  TextField( const TextField& handle );

  /**
   * @brief Assignment operator.
   *
   * @param[in] handle The handle to copy from.
   * @return A reference to this.
   */
  TextField& operator=( const TextField& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~TextField();

  /**
   * @brief Downcast a handle to TextField.
   *
   * If the BaseHandle points is a TextField the downcast returns a valid handle.
   * If not the returned handle is left empty.
   *
   * @param[in] handle Handle to an object
   * @return handle to a TextField or an empty handle
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
   * @return The signal to connect to.
   */
  MaxLengthReachedSignalType& MaxLengthReachedSignal();

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The Control implementation.
   */
  DALI_INTERNAL TextField( Internal::TextField& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL TextField( Dali::Internal::CustomActor* internal );
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_FIELD_H__
