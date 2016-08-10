#ifndef __DALI_TOOLKIT_TEXT_EDITOR_H__
#define __DALI_TOOLKIT_TEXT_EDITOR_H__

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
class TextEditor;
}
/**
 * @addtogroup dali_toolkit_controls_text_controls
 * @{
 */

/**
 * @brief A control which provides a multi-line editable text editor.
 *
 *  * Signals
 * | %Signal Name         | Method                                              |
 * |----------------------|-----------------------------------------------------|
 * | textChanged          | @ref TextChangedSignal()                            |
 *
 */
class DALI_IMPORT_API TextEditor : public Control
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
   * @brief An enumeration of properties belonging to the TextEditor class.
   */
  struct Property
  {
    enum
    {
      RENDERING_BACKEND = PROPERTY_START_INDEX, ///< name "renderingBackend",                    The type or rendering e.g. bitmap-based,                                         type INT @SINCE_1_1.37
      TEXT,                                     ///< name "text",                                The text to display in UTF-8 format,                                             type STRING @SINCE_1_1.37
      TEXT_COLOR,                               ///< name "textColor",                           The text color,                                                                  type VECTOR4 @SINCE_1_1.37
      FONT_FAMILY,                              ///< name "fontFamily",                          The requested font family,                                                       type STRING @SINCE_1_1.37
      FONT_STYLE,                               ///< name "fontStyle",                           The requested font style,                                                        type STRING @SINCE_1_1.37
      POINT_SIZE,                               ///< name "pointSize",                           The size of font in points,                                                      type FLOAT @SINCE_1_1.37
      HORIZONTAL_ALIGNMENT,                     ///< name "horizontalAlignment",                 The text horizontal alignment,                                                   type STRING,  values "BEGIN", "CENTER", "END" @SINCE_1_1.37
      SCROLL_THRESHOLD,                         ///< name "scrollThreshold"                      Vertical scrolling will occur if the cursor is this close to the control border, type FLOAT @SINCE_1_1.37
      SCROLL_SPEED,                             ///< name "scrollSpeed"                          The scroll speed in pixels per second,                                           type FLOAT @SINCE_1_1.37
      PRIMARY_CURSOR_COLOR,                     ///< name "primaryCursorColor",                  The color to apply to the primary cursor,                                        type VECTOR4 @SINCE_1_1.37
      SECONDARY_CURSOR_COLOR,                   ///< name "secondaryCursorColor",                The color to apply to the secondary cursor,                                      type VECTOR4 @SINCE_1_1.37
      ENABLE_CURSOR_BLINK,                      ///< name "enableCursorBlink",                   Whether the cursor should blink or not,                                          type BOOLEAN @SINCE_1_1.37
      CURSOR_BLINK_INTERVAL,                    ///< name "cursorBlinkInterval",                 The time interval in seconds between cursor on/off states,                       type FLOAT @SINCE_1_1.37
      CURSOR_BLINK_DURATION,                    ///< name "cursorBlinkDuration",                 The cursor will stop blinking after this number of seconds (if non-zero),        type FLOAT @SINCE_1_1.37
      CURSOR_WIDTH,                             ///< name "cursorWidth",                         The cursor width,                                                                type INTEGER @SINCE_1_1.37
      GRAB_HANDLE_IMAGE,                        ///< name "grabHandleImage",                     The image to display for the grab handle,                                        type STRING @SINCE_1_1.37
      GRAB_HANDLE_PRESSED_IMAGE,                ///< name "grabHandlePressedImage",              The image to display when the grab handle is pressed,                            type STRING @SINCE_1_1.37
      SELECTION_HANDLE_IMAGE_LEFT,              ///< name "selectionHandleImageLeft",            The image to display for the left selection handle,                              type MAP @SINCE_1_1.37
      SELECTION_HANDLE_IMAGE_RIGHT,             ///< name "selectionHandleImageRight",           The image to display for the right selection handle,                             type MAP @SINCE_1_1.37
      SELECTION_HANDLE_PRESSED_IMAGE_LEFT,      ///< name "selectionHandlePressedImageLeft",     The image to display when the left selection handle is pressed,                  type MAP @SINCE_1_1.37
      SELECTION_HANDLE_PRESSED_IMAGE_RIGHT,     ///< name "selectionHandlePressedImageRight",    The image to display when the right selection handle is pressed,                 type MAP @SINCE_1_1.37
      SELECTION_HANDLE_MARKER_IMAGE_LEFT,       ///< name "selectionHandleMarkerImageLeft",      The image to display for the left selection handle marker,                       type MAP @SINCE_1_1.37
      SELECTION_HANDLE_MARKER_IMAGE_RIGHT,      ///< name "selectionHandleMarkerImageRight",     The image to display for the right selection handle marker,                      type MAP @SINCE_1_1.37
      SELECTION_HIGHLIGHT_COLOR,                ///< name "selectionHighlightColor",             The color of the selection highlight,                                            type VECTOR4 @SINCE_1_1.37
      DECORATION_BOUNDING_BOX,                  ///< name "decorationBoundingBox",               The decorations (handles etc) will positioned within this area on-screen,        type RECTANGLE @SINCE_1_1.37
      ENABLE_MARKUP,                            ///< name "enableMarkup",                        Whether the mark-up processing is enabled.                                       type BOOLEAN @SINCE_1_1.37
      INPUT_COLOR,                              ///< name "inputColor",                          The color of the new input text,                                                 type VECTOR4 @SINCE_1_1.37
      INPUT_FONT_FAMILY,                        ///< name "inputFontFamily",                     The font's family of the new input text,                                         type STRING @SINCE_1_1.37
      INPUT_FONT_STYLE,                         ///< name "inputFontStyle",                      The font's style of the new input text,                                          type STRING @SINCE_1_1.37
      INPUT_POINT_SIZE,                         ///< name "inputPointSize",                      The font's size of the new input text in points,                                 type FLOAT @SINCE_1_1.37
      LINE_SPACING,                             ///< name "lineSpacing",                         The default extra space between lines in points,                                 type FLOAT @SINCE_1_1.37
      INPUT_LINE_SPACING,                       ///< name "inputLineSpacing"                     The extra space between lines in points. It affects the whole paragraph where the new input text is inserted, type FLOAT @SINCE_1_1.37
      UNDERLINE,                                ///< name "underline"                            The default underline parameters,                                                type STRING @SINCE_1_1.37
      INPUT_UNDERLINE,                          ///< name "inputUnderline"                       The underline parameters of the new input text,                                  type STRING @SINCE_1_1.37
      SHADOW,                                   ///< name "shadow"                               The default shadow parameters,                                                   type STRING @SINCE_1_1.37
      INPUT_SHADOW,                             ///< name "inputShadow"                          The shadow parameters of the new input text,                                     type STRING @SINCE_1_1.37
      EMBOSS,                                   ///< name "emboss"                               The default emboss parameters,                                                   type STRING @SINCE_1_1.37
      INPUT_EMBOSS,                             ///< name "inputEmboss"                          The emboss parameters of the new input text,                                     type STRING @SINCE_1_1.37
      OUTLINE,                                  ///< name "outline"                              The default outline parameters,                                                  type STRING @SINCE_1_1.37
      INPUT_OUTLINE,                            ///< name "inputOutline"                         The outline parameters of the new input text,                                    type STRING @SINCE_1_1.37
    };
  };

  // Type Defs

  /// @brief Text changed signal type.
  typedef Signal<void ( TextEditor ) > TextChangedSignalType;

  /**
   * @brief Create the TextEditor control.
   * @return A handle to the TextEditor control.
   */
  static TextEditor New();

  /**
   * @brief Creates an empty handle.
   */
  TextEditor();

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle The handle to copy from.
   */
  TextEditor( const TextEditor& handle );

  /**
   * @brief Assignment operator.
   *
   * @param[in] handle The handle to copy from.
   * @return A reference to this.
   */
  TextEditor& operator=( const TextEditor& handle );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~TextEditor();

  /**
   * @brief Downcast a handle to TextEditor.
   *
   * If the BaseHandle points is a TextEditor the downcast returns a valid handle.
   * If not the returned handle is left empty.
   *
   * @param[in] handle Handle to an object.
   * @return handle to a TextEditor or an empty handle.
   */
  static TextEditor DownCast( BaseHandle handle );

  // Signals

  /**
   * @brief This signal is emitted when the text changes.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( TextEditor textEditor );
   * @endcode
   * @return The signal to connect to.
   */
  TextChangedSignalType& TextChangedSignal();

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The Control implementation.
   */
  DALI_INTERNAL TextEditor( Internal::TextEditor& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL TextEditor( Dali::Internal::CustomActor* internal );
  /// @endcond
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_EDITOR_H__
