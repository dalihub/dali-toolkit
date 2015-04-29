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
 * @brief A control which provides a single-line editable text field.
 *
 *  * Signals
 * | %Signal Name           | Method                                              |
 * |------------------------|-----------------------------------------------------|
 * | max-length-reached     | @ref MaxLengthReachedSignal()                       |
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
      RENDERING_BACKEND = PROPERTY_START_INDEX, ///< name "rendering-backend",                   The type or rendering e.g. bitmap-based,                                  type INT
      TEXT,                                     ///< name "text",                                The text to display in UTF-8 format,                                      type STRING
      PLACEHOLDER_TEXT,                         ///< name "placeholder-text",                    The text to display when the TextField is empty and inactive,             type STRING
      PLACEHOLDER_TEXT_FOCUSED,                 ///< name "placeholder-text-focused",            The text to display when the TextField is empty with key-input focus,     type STRING
      FONT_FAMILY,                              ///< name "font-family",                         The requested font family,                                                type STRING
      FONT_STYLE,                               ///< name "font-style",                          The requested font style e.g. Regular/Italic,                             type STRING
      POINT_SIZE,                               ///< name "point-size",                          The size of font in points,                                               type FLOAT
      MAX_LENGTH,                               ///< name "max-length"                           The maximum number of characters that can be inserted,                    type INTEGER
      EXCEED_POLICY,                            ///< name "exceed-policy"                        Specifies how the text is truncated when it does not fit,                 type INTEGER
      HORIZONTAL_ALIGNMENT,                     ///< name "horizontal-alignment",                The line horizontal alignment,                                            type STRING,  values "BEGIN", "CENTER", "END"
      VERTICAL_ALIGNMENT,                       ///< name "vertical-alignment",                  The line vertical alignment,                                              type STRING,  values   "TOP",   "CENTER",   "BOTTOM"
      TEXT_COLOR,                               ///< name "text-color",                          The text color,                                                           type VECTOR4
      PLACEHOLDER_TEXT_COLOR,                   ///< name "placeholder-text-color",              The placeholder-text color,                                               type VECTOR4
      SHADOW_OFFSET,                            ///< name "shadow-offset",                       The drop shadow offset 0 indicates no shadow,                             type VECTOR2
      SHADOW_COLOR,                             ///< name "shadow-color",                        The color of a drop shadow,                                               type VECTOR4
      PRIMARY_CURSOR_COLOR,                     ///< name "primary-cursor-color",                The color to apply to the primary cursor,                                 type VECTOR4
      SECONDARY_CURSOR_COLOR,                   ///< name "secondary-cursor-color",              The color to apply to the secondary cursor,                               type VECTOR4
      ENABLE_CURSOR_BLINK,                      ///< name "enable-cursor-blink",                 Whether the cursor should blink or not,                                   type BOOLEAN
      CURSOR_BLINK_INTERVAL,                    ///< name "cursor-blink-interval",               The time interval in seconds between cursor on/off states,                type FLOAT
      CURSOR_BLINK_DURATION,                    ///< name "cursor-blink-duration",               The cursor will stop blinking after this number of seconds (if non-zero), type FLOAT
      GRAB_HANDLE_IMAGE,                        ///< name "grab-handle-image",                   The image to display for the grab handle,                                 type STRING
      GRAB_HANDLE_PRESSED_IMAGE,                ///< name "grab-handle-pressed-image",           The image to display when the grab handle is pressed,                     type STRING
      SCROLL_THRESHOLD,                         ///< name "scroll-threshold"                     Scrolling will occur if the cursor is this close to the control border,   type FLOAT
      SCROLL_SPEED,                             ///< name "scroll-speed"                         The scroll speed in pixels per second,                                    type FLOAT
      SELECTION_HANDLE_IMAGE_LEFT,              ///< name "selection-handle-image-left",         The image to display for the left selection handle,                       type STRING
      SELECTION_HANDLE_IMAGE_RIGHT,             ///< name "selection-handle-image-right",        The image to display for the right selection handle,                      type STRING
      SELECTION_HANDLE_PRESSED_IMAGE_LEFT,      ///< name "selection-handle-pressed-image-left"  The image to display when the left selection handle is pressed,           type STRING
      SELECTION_HANDLE_PRESSED_IMAGE_RIGHT,     ///< name "selection-handle-pressed-image-right" The image to display when the right selection handle is pressed,          type STRING
      SELECTION_HIGHLIGHT_COLOR,                ///< name "selection-highlight-color"            The color of the selection highlight,                                     type VECTOR4
      DECORATION_BOUNDING_BOX                   ///< name "decoration-bounding-box"              The decorations (handles etc) will positioned within this area on-screen, type RECTANGLE
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

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_FIELD_H__
