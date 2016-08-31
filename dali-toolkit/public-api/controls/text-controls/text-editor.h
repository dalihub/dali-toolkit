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
 * | %Signal Name         | Method                         |                    |
 * |----------------------|--------------------------------|--------------------|
 * | textChanged          | @ref TextChangedSignal()       | @SINCE_1_1.37      |
 * | inputStyleChanged    | @ref InputStyleChangedSignal() | @SINCE_1_2_2       |
 *
 */
class DALI_IMPORT_API TextEditor : public Control
{
public:

  /**
   * @brief The start and end property ranges for this control.
   * @SINCE_1_1.37
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the TextEditor class.
   * @SINCE_1_1.37
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

  /**
   * @brief Mask used by the signal InputStyleChangedSignal(). Notifies which parameters of the input style have changed.
   *
   * @SINCE_1_2_2
   */
  struct InputStyle
  {
    enum Mask
    {
      NONE         = 0x0000, ///< @SINCE_1_2_2
      COLOR        = 0x0001, ///< @SINCE_1_2_2
      FONT_FAMILY  = 0x0002, ///< @SINCE_1_2_2
      POINT_SIZE   = 0x0004, ///< @SINCE_1_2_2
      FONT_STYLE   = 0x0008, ///< @SINCE_1_2_2
      LINE_SPACING = 0x0010, ///< @SINCE_1_2_2
      UNDERLINE    = 0x0020, ///< @SINCE_1_2_2
      SHADOW       = 0x0040, ///< @SINCE_1_2_2
      EMBOSS       = 0x0080, ///< @SINCE_1_2_2
      OUTLINE      = 0x0100  ///< @SINCE_1_2_2
    };
  };

  // Type Defs

  /**
   * @brief Text changed signal type.
   * @SINCE_1_1.37
   */
  typedef Signal<void ( TextEditor ) > TextChangedSignalType;

  /**
   * @brief Input Style changed signal type.
   * @SINCE_1_2_2
   */
  typedef Signal<void ( TextEditor, InputStyle::Mask ) > InputStyleChangedSignalType;

  /**
   * @brief Create the TextEditor control.
   *
   * @SINCE_1_1.37
   * @return A handle to the TextEditor control.
   */
  static TextEditor New();

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
   * @param[in] handle The handle to copy from.
   */
  TextEditor( const TextEditor& handle );

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_1.37
   * @param[in] handle The handle to copy from.
   * @return A reference to this.
   */
  TextEditor& operator=( const TextEditor& handle );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_1.37
   */
  ~TextEditor();

  /**
   * @brief Downcast a handle to TextEditor.
   *
   * If the BaseHandle points is a TextEditor the downcast returns a valid handle.
   * If not the returned handle is left empty.
   *
   * @SINCE_1_1.37
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
   *
   * @SINCE_1_1.37
   * @return The signal to connect to.
   */
  TextChangedSignalType& TextChangedSignal();

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
   * @return The signal to connect to.
   */
  InputStyleChangedSignalType& InputStyleChangedSignal();

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_1.37
   * @param[in] implementation The Control implementation.
   */
  DALI_INTERNAL TextEditor( Internal::TextEditor& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @SINCE_1_1.37
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
