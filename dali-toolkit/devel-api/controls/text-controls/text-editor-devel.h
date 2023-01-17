#ifndef DALI_TOOLKIT_TEXT_EDITOR_DEVEL_H
#define DALI_TOOLKIT_TEXT_EDITOR_DEVEL_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/text-controls/input-filter-properties.h>
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
  TEXT                                 = Dali::Toolkit::TextEditor::Property::TEXT,
  TEXT_COLOR                           = Dali::Toolkit::TextEditor::Property::TEXT_COLOR,
  FONT_FAMILY                          = Dali::Toolkit::TextEditor::Property::FONT_FAMILY,
  FONT_STYLE                           = Dali::Toolkit::TextEditor::Property::FONT_STYLE,
  POINT_SIZE                           = Dali::Toolkit::TextEditor::Property::POINT_SIZE,
  HORIZONTAL_ALIGNMENT                 = Dali::Toolkit::TextEditor::Property::HORIZONTAL_ALIGNMENT,
  SCROLL_THRESHOLD                     = Dali::Toolkit::TextEditor::Property::SCROLL_THRESHOLD,
  SCROLL_SPEED                         = Dali::Toolkit::TextEditor::Property::SCROLL_SPEED,
  PRIMARY_CURSOR_COLOR                 = Dali::Toolkit::TextEditor::Property::PRIMARY_CURSOR_COLOR,
  SECONDARY_CURSOR_COLOR               = Dali::Toolkit::TextEditor::Property::SECONDARY_CURSOR_COLOR,
  ENABLE_CURSOR_BLINK                  = Dali::Toolkit::TextEditor::Property::ENABLE_CURSOR_BLINK,
  CURSOR_BLINK_INTERVAL                = Dali::Toolkit::TextEditor::Property::CURSOR_BLINK_INTERVAL,
  CURSOR_BLINK_DURATION                = Dali::Toolkit::TextEditor::Property::CURSOR_BLINK_DURATION,
  CURSOR_WIDTH                         = Dali::Toolkit::TextEditor::Property::CURSOR_WIDTH,
  GRAB_HANDLE_IMAGE                    = Dali::Toolkit::TextEditor::Property::GRAB_HANDLE_IMAGE,
  GRAB_HANDLE_PRESSED_IMAGE            = Dali::Toolkit::TextEditor::Property::GRAB_HANDLE_PRESSED_IMAGE,
  SELECTION_HANDLE_IMAGE_LEFT          = Dali::Toolkit::TextEditor::Property::SELECTION_HANDLE_IMAGE_LEFT,
  SELECTION_HANDLE_IMAGE_RIGHT         = Dali::Toolkit::TextEditor::Property::SELECTION_HANDLE_IMAGE_RIGHT,
  SELECTION_HANDLE_PRESSED_IMAGE_LEFT  = Dali::Toolkit::TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT,
  SELECTION_HANDLE_PRESSED_IMAGE_RIGHT = Dali::Toolkit::TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT,
  SELECTION_HANDLE_MARKER_IMAGE_LEFT   = Dali::Toolkit::TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT,
  SELECTION_HANDLE_MARKER_IMAGE_RIGHT  = Dali::Toolkit::TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT,
  SELECTION_HIGHLIGHT_COLOR            = Dali::Toolkit::TextEditor::Property::SELECTION_HIGHLIGHT_COLOR,
  DECORATION_BOUNDING_BOX              = Dali::Toolkit::TextEditor::Property::DECORATION_BOUNDING_BOX,
  ENABLE_MARKUP                        = Dali::Toolkit::TextEditor::Property::ENABLE_MARKUP,
  INPUT_COLOR                          = Dali::Toolkit::TextEditor::Property::INPUT_COLOR,
  INPUT_FONT_FAMILY                    = Dali::Toolkit::TextEditor::Property::INPUT_FONT_FAMILY,
  INPUT_FONT_STYLE                     = Dali::Toolkit::TextEditor::Property::INPUT_FONT_STYLE,
  INPUT_POINT_SIZE                     = Dali::Toolkit::TextEditor::Property::INPUT_POINT_SIZE,
  LINE_SPACING                         = Dali::Toolkit::TextEditor::Property::LINE_SPACING,
  INPUT_LINE_SPACING                   = Dali::Toolkit::TextEditor::Property::INPUT_LINE_SPACING,
  UNDERLINE                            = Dali::Toolkit::TextEditor::Property::UNDERLINE,
  INPUT_UNDERLINE                      = Dali::Toolkit::TextEditor::Property::INPUT_UNDERLINE,
  SHADOW                               = Dali::Toolkit::TextEditor::Property::SHADOW,
  INPUT_SHADOW                         = Dali::Toolkit::TextEditor::Property::INPUT_SHADOW,
  EMBOSS                               = Dali::Toolkit::TextEditor::Property::EMBOSS,
  INPUT_EMBOSS                         = Dali::Toolkit::TextEditor::Property::INPUT_EMBOSS,
  OUTLINE                              = Dali::Toolkit::TextEditor::Property::OUTLINE,
  INPUT_OUTLINE                        = Dali::Toolkit::TextEditor::Property::INPUT_OUTLINE,
  SMOOTH_SCROLL                        = Dali::Toolkit::TextEditor::Property::SMOOTH_SCROLL,
  SMOOTH_SCROLL_DURATION               = Dali::Toolkit::TextEditor::Property::SMOOTH_SCROLL_DURATION,
  ENABLE_SCROLL_BAR                    = Dali::Toolkit::TextEditor::Property::ENABLE_SCROLL_BAR,
  SCROLL_BAR_SHOW_DURATION             = Dali::Toolkit::TextEditor::Property::SCROLL_BAR_SHOW_DURATION,
  SCROLL_BAR_FADE_DURATION             = Dali::Toolkit::TextEditor::Property::SCROLL_BAR_FADE_DURATION,
  PIXEL_SIZE                           = Dali::Toolkit::TextEditor::Property::PIXEL_SIZE,
  LINE_COUNT                           = Dali::Toolkit::TextEditor::Property::LINE_COUNT,
  ENABLE_SELECTION                     = Dali::Toolkit::TextEditor::Property::ENABLE_SELECTION,
  PLACEHOLDER                          = Dali::Toolkit::TextEditor::Property::PLACEHOLDER,
  LINE_WRAP_MODE                       = Dali::Toolkit::TextEditor::Property::LINE_WRAP_MODE,

  /**
   * @brief The text to display when the TextEditor is empty and inactive.
   * @details Name "placeholderText", type Property::STRING.
   */
  PLACEHOLDER_TEXT,

  /**
   * @brief The placeholder-text color.
   * @details Name "placeholderTextColor", type Property::VECTOR4.
   */
  PLACEHOLDER_TEXT_COLOR,

  /**
   * @brief Enables Text selection using Shift key.
   * @details Name "enableShiftSelection", type Property::BOOLEAN.
   */
  ENABLE_SHIFT_SELECTION,

  /**
   * @brief Enables the grab handles for text selection.
   * @details Name "enableGrabHandle", type Property::BOOLEAN.
   * @note The default value is true, which means the grab handles are enabled by default.
   */
  ENABLE_GRAB_HANDLE,

  /**
   * @brief Modifies the default text alignment to match the direction of the system language.
   * @details Name "matchSystemLanguageDirection", type (Property::BOOLEAN), Read/Write
   * @note The default value is true
   */
  MATCH_SYSTEM_LANGUAGE_DIRECTION,

  /**
   * @brief The type or rendering e.g. bitmap-based.
   * @details Name "renderingBackend", type Property::INTEGER.
   */
  RENDERING_BACKEND,

  /**
   * @brief The maximum number of characters that can be inserted.
   * @details Name "maxLength", type Property::INTEGER.
   */
  MAX_LENGTH,

  /**
   * @brief The selected text start position.
   * @details Name "selectedTextStart", type Property::INTEGER.
   */
  SELECTED_TEXT_START,

  /**
   * @brief The selected text range end position.
   * @details Name "selectedTextEnd", type Property::INTEGER.
   */
  SELECTED_TEXT_END,

  /**
   * @brief The horizontal scroll position in pixels.
   * @details Name "horizontalScrollPosition", type Property::FLOAT.
   */
  HORIZONTAL_SCROLL_POSITION,

  /**
   * @brief The vertical scroll position in pixels.
   * @details Name "verticalScrollPosition", type Property::FLOAT.
   */
  VERTICAL_SCROLL_POSITION,

  /**
   * @brief The Editable state of control.
   * @details Name "enableEditing", type Property::BOOLEAN.
   */
  ENABLE_EDITING,

  /**
   * @brief The selected text in UTF-8 format.
   * @details Name "selectedText", type Property::STRING.
   * @note This property is read-only.
   */
  SELECTED_TEXT,

  /**
   * @brief The font size scale.
   * @details name "fontSizeScale", type Property::FLOAT.
   * @note The default value is 1.0 which does nothing.
   * The given font size scale value is used for multiplying the specified font size before querying fonts.
   *
   * e.g. The rendering results of both cases are same.
   *  - fontSize: 15pt, fontSizeScale: 1.0
   *  - fontSize: 10pt, fontSizeScale: 1.5
   */
  FONT_SIZE_SCALE,

  /**
   * @brief True to enable the font size scale or false to disable.
   * @details Name "enableFontSizeScale", type Property::BOOLEAN.
   * @note The default value is true.
   * If false, font size scale is not apppied.
   */
  ENABLE_FONT_SIZE_SCALE,

  /**
   * @brief The position for primary cursor.
   * @details Name "primaryCursorPosition", type Property::INTEGER.
   */
  PRIMARY_CURSOR_POSITION,

  /**
   * @brief The color of the grab color.
   * @details Name "grabHandleColor", type Property::VECTOR4.
   */
  GRAB_HANDLE_COLOR,

  /**
   * @brief Enables the grab handle popup for text selection.
   * @details Name "enableGrabHandlePopup", type Property::BOOLEAN.
   * @note The default value is true, which means the grab handle popup is enabled by default.
   */
  ENABLE_GRAB_HANDLE_POPUP,

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
   *   editor.SetProperty(DevelTextEditor::Property::INPUT_METHOD_SETTINGS, propertyMap);
   * @endcode
   */
  INPUT_METHOD_SETTINGS,

  /**
   * @brief The input filter
   * @details Name "inputFilter", type Property::MAP.
   *
   * The inputFilter map contains the following keys:
   *
   * | %Property Name       | Type     | Required | Description                                                                                                         |
   * |----------------------|----------|----------|---------------------------------------------------------------------------------------------------------------------|
   * | accepted             | STRING   | No       | A regular expression in the set of characters to be accepted by the inputFilter (the default value is empty string) |
   * | rejected             | STRING   | No       | A regular expression in the set of characters to be rejected by the inputFilter (the default value is empty string) |
   *
   * @note Optional.
   * The character set must follow the regular expression rules.
   * Behaviour can not be guaranteed for incorrect grammars.
   * Refer the link below for detailed rules.
   * The functions in std::regex library use the ECMAScript grammar:
   * http://cplusplus.com/reference/regex/ECMAScript/
   *
   * You can use enums instead of "accepted" and "rejected" strings.
   * @see Dali::Toolkit::InputFilter::Property::Type
   *
   * Example Usage:
   * @code
   *   Property::Map filter;
   *   filter[InputFilter::Property::ACCEPTED] = "[\\d]"; // accept whole digits
   *   filter[InputFilter::Property::REJECTED] = "[0-5]"; // reject 0, 1, 2, 3, 4, 5
   *
   *   editor.SetProperty(DevelTextEditor::Property::INPUT_FILTER, filter); // acceptable inputs are 6, 7, 8, 9
   * @endcode
   */
  INPUT_FILTER,

  /**
  * @brief Whether we should show the ellipsis if required.
  * @details Name "ellipsis", type Property::BOOLEAN.
  */
  ELLIPSIS,

  /**
  * @brief The enumerations used to specify whether to position the ellipsis at the END, START or MIDDLE of the text.
  * @details Name "EllipsisPosition", type [Type](@ref Dali::Toolkit::DevelText::EllipsisPosition::Type) (Property::INTEGER), or Property::STRING. Read/Write
  * @note Default is EllipsisPosition::END.
  * @see DevelText::EllipsisPosition
  */
  ELLIPSIS_POSITION,

  /**
   * @brief Sets the height of the line in points.
   * @details Name "minLineSize", type Property::FLOAT.
   * @note If the font size is larger than the line size, it works with the font size.
   */
  MIN_LINE_SIZE,

  /**
   * @brief A horizontal line through the text center.
   * @details Name "strikethrough", type Property::MAP.
   */
  STRIKETHROUGH,

  /**
   * @brief The strikethrough parameters of the new input text.
   * @details Name "inputStrikethrough", type Property::MAP.
   */
  INPUT_STRIKETHROUGH,

  /**
  * @brief The spaces between characters in Pixels.
  * @details Name "characterSpacing", type Property::FLOAT.
  * @note
  *   A positive value will make the characters far apart (expanded) and a negative value will bring them closer (condensed).
  *   The default value is 0.f which does nothing.
  */
  CHARACTER_SPACING,

  /**
   * @brief the relative height of the line (a factor that will be multiplied by text height).
   * @details Name "relativeLineSize", type Property::FLOAT.
   * @note If the value is less than 1, the lines could to be overlapped.
   */
  RELATIVE_LINE_SIZE,

  /**
   * @brief The line vertical alignment.
   * @details Name "verticalAlignment", type Property::STRING or type VerticalAlignment::Type (Property::INTEGER).
   *          Values "TOP", "CENTER", "BOTTOM", default TOP.
   * @note Return type is Property::STRING
   */
  VERTICAL_ALIGNMENT,

  /**
   * @brief Sets the selection popup style
   * @details Name "selectionPopupStyle", type Property::MAP.
   * @see Dali::Toolkit::TextSelectionPopup::Property
   */
  SELECTION_POPUP_STYLE,
};

} // namespace Property

/**
 * @brief Return the input method context of TextEditor.
 *
 * @param[in] textEditor The instance of TextEditor.
 * @return InputMethodContext instance.
 */
DALI_TOOLKIT_API InputMethodContext GetInputMethodContext(TextEditor textEditor);

/**
 * @brief Max Characters Exceed signal type.
  */
using MaxLengthReachedSignalType = Signal<void(TextEditor)>;

/**
 * @brief This signal is emitted when inserted text exceeds the maximum character limit.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName( TextEditor textEditor );
 * @endcode
 * @param[in] textEditor The instance of TextEditor.
 * @return The signal to connect to
 */
DALI_TOOLKIT_API MaxLengthReachedSignalType& MaxLengthReachedSignal(TextEditor textEditor);

/**
 * @brief Anchor clicked signal type.
 *
 * @note Signal
 *  - const char* : href of clicked anchor.
 *  - uint32_t    : length of href.
 */
using AnchorClickedSignalType = Signal<void(TextEditor, const char*, uint32_t)>;

/**
 * @brief This signal is emitted when the anchor is clicked.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName(TextEditor textEditor, const char* href, uint32_t hrefLength);
 * @endcode
 * @param[in] textEditor The instance of TextEditor.
 * @return The signal to connect to.
 */
DALI_TOOLKIT_API AnchorClickedSignalType& AnchorClickedSignal(TextEditor textEditor);

/**
 * @brief cursor position changed signal type.
 *
 * @note Signal
 *  - uint32_t    : old position.
 */
using CursorPositionChangedSignalType = Signal<void(TextEditor, uint32_t)>;

/**
 * @brief This signal is emitted when the cursor position has been changed.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName(TextEditor textEditor, uint32_t oldPosition);
 * @endcode
 * @param[in] textEditor The instance of TextEditor.
 * @return The signal to connect to.
 */
DALI_TOOLKIT_API CursorPositionChangedSignalType& CursorPositionChangedSignal(TextEditor textEditor);

/**
 * @brief Input filtered signal type.
 */
using InputFilteredSignalType = Signal<void(TextEditor, Toolkit::InputFilter::Property::Type)>;

/**
 * @brief This signal is emitted when the character to be inserted is filtered by the input filter.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName(TextEditor textEditor, Toolkit::InputFilter::Property::Type type);
 *
 *   DevelTextEditor::InputFilteredSignal(textEditor).Connect(this, &OnInputFiltered);
 *
 *   void OnInputFiltered(TextEditor textEditor, InputFilter::Property::Type type)
 *   {
 *     if (type == InputFilter::Property::ACCEPTED)
 *     {
 *       // If the input has been filtered with an accepted filter, the type is ACCEPTED.
 *     }
 *     else if (type == InputFilter::Property::REJECTED)
 *     {
 *       // If the input has been filtered with an rejected filter, the type is REJECTED.
 *     }
 *   }
 * @endcode
 * @param[in] textEditor The instance of TextEditor.
 * @return The signal to connect to.
 */
DALI_TOOLKIT_API InputFilteredSignalType& InputFilteredSignal(TextEditor textEditor);

/**
 * @brief selection changed signal type.
 *
 * @note Signal
 *  - uint32_t  : selection start before the change.
 *  - uint32_t  : selection end before the change.
 */
using SelectionChangedSignalType = Signal<void(TextEditor, uint32_t, uint32_t)>;

/**
 * @brief This signal is emitted when the selection has been changed.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName( TextEditor textEditor, uint32_t oldStart, uint32_t oldEnd);
 * @endcode
 * @param[in] textEditor The instance of TextEditor.
 * @return The signal to connect to
 */
DALI_TOOLKIT_API SelectionChangedSignalType& SelectionChangedSignal(TextEditor textEditor);

/**
 * @brief selection cleared signal type.
 */
using SelectionClearedSignalType = Signal<void(TextEditor)>;

/**
 * @brief This signal is emitted when the selection has been cleared.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName( TextEditor textEditor);
 * @endcode
 * @param[in] textEditor The instance of TextEditor.
 * @return The signal to connect to
 */
DALI_TOOLKIT_API SelectionClearedSignalType& SelectionClearedSignal(TextEditor textEditor);

/**
 * @brief selection start signal type.
 */
using SelectionStartedSignalType = Signal<void(TextEditor)>;

/**
 * @brief This signal is emitted when the selection start.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName( TextEditor textEditor);
 * @endcode
 * @param[in] textEditor The instance of TextEditor.
 * @return The signal to connect to
 */
DALI_TOOLKIT_API SelectionStartedSignalType& SelectionStartedSignal(TextEditor textEditor);

/**
 * @brief Select the whole text of TextEditor.
 *
 * @param[in] textEditor The instance of TextEditor.
 */
DALI_TOOLKIT_API void SelectWholeText(TextEditor textEditor);

/**
 * @brief Unselect the whole text of TextEditor.
 *
 * @param[in] textEditor The instance of TextEditor.
 */
DALI_TOOLKIT_API void SelectNone(TextEditor textEditor);

/**
 * @brief Select the text from start index to end index of TextEditor.
 * @note
 * The selection index is based on the cursor index.
 *
 * text   H e l l o
 * index 0 1 2 3 4 5
 * if textEditor.SelectText(1, 4); is executed, "ell" is selected.
 *
 * @param[in] textEditor The instance of TextEditor.
 * @param[in] start The start index of the text to select. (The starting point of start index is 0.)
 * @param[in] end The end index of the text to select. (If end index > text's length, the end index is set to the length of the text.)
 */
DALI_TOOLKIT_API void SelectText(TextEditor textEditor, const uint32_t start, const uint32_t end);

/**
 * @brief Scroll the TextEditor by specific amount.
 *
 * @param[in] textEditor The instance of TextEditor.
 * @param[in] scroll amount (in pixels) of scrolling in horizontal & vectical directions.
 */
DALI_TOOLKIT_API void ScrollBy(TextEditor textEditor, Vector2 scroll);

/**
 * @brief Get the rendered size of a specific text range.
 * if the requested text is at multilines, multiple sizes will be returned for each text located in a separate line.
 * if a line contains characters with different directions, multiple sizes will be returned for each block of contiguous characters with the same direction.
 *
 * @param[in] textEditor The instance of TextEditor.
 * @param[in] startIndex start index of the text requested to calculate size for.
 * @param[in] endIndex end index(included) of the text requested to calculate size for.
 * @return list of sizes of the reuested text.
 */
DALI_TOOLKIT_API Vector<Vector2> GetTextSize(TextEditor textEditor, const uint32_t startIndex, const uint32_t endIndex);

/**
 * @brief Get the top/left rendered position of a specific text range.
 * if the requested text is at multilines, multiple positions will be returned for each text located in a separate line.
 * if a line contains characters with different directions, multiple positions will be returned for each block of contiguous characters with the same direction.
 *
 * @param[in] textEditor The instance of TextEditor.
 * @param[in] startIndex start index of the text requested to get position to.
 * @param[in] endIndex end index(included) of the text requested to get position to.
 * @return list of positions of the requested text.
 */
DALI_TOOLKIT_API Vector<Vector2> GetTextPosition(TextEditor textEditor, const uint32_t startIndex, const uint32_t endIndex);

/**
 * @brief Gets the bounding box of a specific text range.
 *
 * @param[in] textEditor The instance of textEditor.
 * @param[in] startIndex start index of the text requested to get bounding box to.
 * @param[in] endIndex end index(included) of the text requested to get bounding box to.
 * @return bounding box of the requested text.
 */
DALI_TOOLKIT_API Rect<> GetTextBoundingRectangle(TextEditor textEditor, uint32_t startIndex, uint32_t endIndex);

/**
 * @brief Copy and return the selected text of TextEditor.
 *
 * @param[in] textEditor The instance of TextEditor.
 * @return The copied text.
 */
DALI_TOOLKIT_API std::string CopyText(TextEditor textEditor);

/**
 * @brief Cut and return the selected text of TextEditor.
 *
 * @param[in] textEditor The instance of TextEditor.
 * @return The cut text.
 */
DALI_TOOLKIT_API std::string CutText(TextEditor textEditor);

/**
 * @brief Paste the most recent clipboard text item into the TextEditor.
 *
 * @param[in] textEditor The instance of TextEditor.
 */
DALI_TOOLKIT_API void PasteText(TextEditor textEditor);

} // namespace DevelTextEditor

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_EDITOR_DEVEL_H
