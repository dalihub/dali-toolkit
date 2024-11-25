#ifndef DALI_TOOLKIT_TEXT_FIELD_DEVEL_H
#define DALI_TOOLKIT_TEXT_FIELD_DEVEL_H

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
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/text-controls/input-filter-properties.h>
#include <dali-toolkit/public-api/controls/text-controls/text-field.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelTextField
{
namespace Property
{
enum
{
  TEXT                                 = Dali::Toolkit::TextField::Property::TEXT,
  PLACEHOLDER_TEXT                     = Dali::Toolkit::TextField::Property::PLACEHOLDER_TEXT,
  PLACEHOLDER_TEXT_FOCUSED             = Dali::Toolkit::TextField::Property::PLACEHOLDER_TEXT_FOCUSED,
  FONT_FAMILY                          = Dali::Toolkit::TextField::Property::FONT_FAMILY,
  FONT_STYLE                           = Dali::Toolkit::TextField::Property::FONT_STYLE,
  POINT_SIZE                           = Dali::Toolkit::TextField::Property::POINT_SIZE,
  MAX_LENGTH                           = Dali::Toolkit::TextField::Property::MAX_LENGTH,
  EXCEED_POLICY                        = Dali::Toolkit::TextField::Property::EXCEED_POLICY,
  HORIZONTAL_ALIGNMENT                 = Dali::Toolkit::TextField::Property::HORIZONTAL_ALIGNMENT,
  VERTICAL_ALIGNMENT                   = Dali::Toolkit::TextField::Property::VERTICAL_ALIGNMENT,
  TEXT_COLOR                           = Dali::Toolkit::TextField::Property::TEXT_COLOR,
  PLACEHOLDER_TEXT_COLOR               = Dali::Toolkit::TextField::Property::PLACEHOLDER_TEXT_COLOR,
  PRIMARY_CURSOR_COLOR                 = Dali::Toolkit::TextField::Property::PRIMARY_CURSOR_COLOR,
  SECONDARY_CURSOR_COLOR               = Dali::Toolkit::TextField::Property::SECONDARY_CURSOR_COLOR,
  ENABLE_CURSOR_BLINK                  = Dali::Toolkit::TextField::Property::ENABLE_CURSOR_BLINK,
  CURSOR_BLINK_INTERVAL                = Dali::Toolkit::TextField::Property::CURSOR_BLINK_INTERVAL,
  CURSOR_BLINK_DURATION                = Dali::Toolkit::TextField::Property::CURSOR_BLINK_DURATION,
  CURSOR_WIDTH                         = Dali::Toolkit::TextField::Property::CURSOR_WIDTH,
  GRAB_HANDLE_IMAGE                    = Dali::Toolkit::TextField::Property::GRAB_HANDLE_IMAGE,
  GRAB_HANDLE_PRESSED_IMAGE            = Dali::Toolkit::TextField::Property::GRAB_HANDLE_PRESSED_IMAGE,
  SCROLL_THRESHOLD                     = Dali::Toolkit::TextField::Property::SCROLL_THRESHOLD,
  SCROLL_SPEED                         = Dali::Toolkit::TextField::Property::SCROLL_SPEED,
  SELECTION_HANDLE_IMAGE_LEFT          = Dali::Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_LEFT,
  SELECTION_HANDLE_IMAGE_RIGHT         = Dali::Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT,
  SELECTION_HANDLE_PRESSED_IMAGE_LEFT  = Dali::Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT,
  SELECTION_HANDLE_PRESSED_IMAGE_RIGHT = Dali::Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT,
  SELECTION_HANDLE_MARKER_IMAGE_LEFT   = Dali::Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT,
  SELECTION_HANDLE_MARKER_IMAGE_RIGHT  = Dali::Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT,
  SELECTION_HIGHLIGHT_COLOR            = Dali::Toolkit::TextField::Property::SELECTION_HIGHLIGHT_COLOR,
  DECORATION_BOUNDING_BOX              = Dali::Toolkit::TextField::Property::DECORATION_BOUNDING_BOX,
  INPUT_METHOD_SETTINGS                = Dali::Toolkit::TextField::Property::INPUT_METHOD_SETTINGS,
  INPUT_COLOR                          = Dali::Toolkit::TextField::Property::INPUT_COLOR,
  ENABLE_MARKUP                        = Dali::Toolkit::TextField::Property::ENABLE_MARKUP,
  INPUT_FONT_FAMILY                    = Dali::Toolkit::TextField::Property::INPUT_FONT_FAMILY,
  INPUT_FONT_STYLE                     = Dali::Toolkit::TextField::Property::INPUT_FONT_STYLE,
  INPUT_POINT_SIZE                     = Dali::Toolkit::TextField::Property::INPUT_POINT_SIZE,
  UNDERLINE                            = Dali::Toolkit::TextField::Property::UNDERLINE,
  INPUT_UNDERLINE                      = Dali::Toolkit::TextField::Property::INPUT_UNDERLINE,
  SHADOW                               = Dali::Toolkit::TextField::Property::SHADOW,
  INPUT_SHADOW                         = Dali::Toolkit::TextField::Property::INPUT_SHADOW,
  EMBOSS                               = Dali::Toolkit::TextField::Property::EMBOSS,
  INPUT_EMBOSS                         = Dali::Toolkit::TextField::Property::INPUT_EMBOSS,
  OUTLINE                              = Dali::Toolkit::TextField::Property::OUTLINE,
  INPUT_OUTLINE                        = Dali::Toolkit::TextField::Property::INPUT_OUTLINE,
  HIDDEN_INPUT_SETTINGS                = Dali::Toolkit::TextField::Property::HIDDEN_INPUT_SETTINGS,
  PIXEL_SIZE                           = Dali::Toolkit::TextField::Property::PIXEL_SIZE,
  ENABLE_SELECTION                     = Dali::Toolkit::TextField::Property::ENABLE_SELECTION,
  PLACEHOLDER                          = Dali::Toolkit::TextField::Property::PLACEHOLDER,
  ELLIPSIS                             = Dali::Toolkit::TextField::Property::ELLIPSIS,

  /**
   * @brief Enables Text selection using Shift key.
   * @details Name "enableShiftSelection", type Property::BOOLEAN.
   */
  ENABLE_SHIFT_SELECTION = ELLIPSIS + 1,

  /**
   * @brief Enables the grab handles for text selection.
   * @details Name "enableGrabHandle", type Property::BOOLEAN.
   * @note The default value is true, which means the grab handles are enabled by default.
   */
  ENABLE_GRAB_HANDLE = ELLIPSIS + 2,

  /**
   * @brief Modifies the default text alignment to match the direction of the system language.
   * @details Name "matchSystemLanguageDirection", type (Property::BOOLEAN), Read/Write
   * @note The default value is true
   */
  MATCH_SYSTEM_LANGUAGE_DIRECTION = ELLIPSIS + 3,

  /**
   * @brief Enables the grab handle popup for text selection.
   * @details Name "enableGrabHandlePopup", type Property::BOOLEAN.
   * @note The default value is true, which means the grab handle popup is enabled by default.
   */
  ENABLE_GRAB_HANDLE_POPUP = ELLIPSIS + 4,

  /**
   * @brief The default text background parameters.
   * @details Name "textBackground", type Property::VECTOR4.
   * @note Use "textBackground" as property name to avoid conflict with Control's "background" property.
   * @note The default value is Color::TRANSPARENT.
   */
  BACKGROUND = ELLIPSIS + 5,

  /**
   * @brief The selected text in UTF-8 format.
   * @details Name "selectedText", type Property::STRING.
   * @note This property is read-only.
   */
  SELECTED_TEXT = ELLIPSIS + 6,

  /**
   * @brief The type or rendering e.g. bitmap-based.
   * @details Name "renderingBackend", type Property::INTEGER.
   */
  RENDERING_BACKEND = ELLIPSIS + 7,

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
   * @brief The Editable state of control.
   * @details Name "enableEditing", type Property::BOOLEAN.
   */
  ENABLE_EDITING,

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
   *   field.SetProperty(DevelTextField::Property::INPUT_FILTER, filter); // acceptable inputs are 6, 7, 8, 9
   * @endcode
   */
  INPUT_FILTER,

  /**
  * @brief The enumerations used to specify whether to position the ellipsis at the END, START or MIDDLE of the text.
  * @details Name "EllipsisPosition", type [Type](@ref Dali::Toolkit::DevelText::EllipsisPosition::Type) (Property::INTEGER), or Property::STRING. Read/Write
  * @note Default is EllipsisPosition::END.
  * @see DevelText::EllipsisPosition
  */
  ELLIPSIS_POSITION,

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
   * @brief Sets the selection popup style
   * @details Name "selectionPopupStyle", type Property::MAP.
   * @see Dali::Toolkit::TextSelectionPopup::Property
   */
  SELECTION_POPUP_STYLE,

  /**
   * @brief Whether to trim the xBearing of first glyph of the text.
   * @details Name "removeFrontInset", type Property::BOOLEAN.
   */
  REMOVE_FRONT_INSET,

  /**
   * @brief Whether to trim the advance of last glyph of the text.
   * @details Name "removeBackInset", type Property::BOOLEAN.
   */
  REMOVE_BACK_INSET,

  /**
   * @brief Enables customization of fonts with variations such as weight and slant.
   * @details Name "fontVariations", type Property::MAP.
   * @note This property can be used only when using variable fonts.
   */
  FONT_VARIATIONS,
};

} // namespace Property

/**
 * @brief Return the input method context of TextField.
 *
 * @param[in] textField The instance of TextField.
 * @return InputMethodContext instance.
 */
DALI_TOOLKIT_API InputMethodContext GetInputMethodContext(TextField textField);

/**
 * @brief Anchor clicked signal type.
 *
 * @note Signal
 *  - const char* : href of clicked anchor.
 *  - uint32_t    : length of href.
 */
using AnchorClickedSignalType = Signal<void(TextField, const char*, uint32_t)>;

/**
 * @brief This signal is emitted when the anchor is clicked.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName(TextField textField, const char* href, uint32_t hrefLength);
 * @endcode
 * @param[in] textField The instance of TextField.
 * @return The signal to connect to.
 */
DALI_TOOLKIT_API AnchorClickedSignalType& AnchorClickedSignal(TextField textField);

/**
 * @brief cursor position changed signal type.
 *
 * @note Signal
 *  - uint32_t    : old position.
 */
using CursorPositionChangedSignalType = Signal<void(TextField, uint32_t)>;

/**
 * @brief This signal is emitted when the cursor position has been changed.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName(TextField textField, uint32_t oldPosition);
 * @endcode
 * @param[in] textField The instance of TextField.
 * @return The signal to connect to.
 */
DALI_TOOLKIT_API CursorPositionChangedSignalType& CursorPositionChangedSignal(TextField textField);

/**
 * @brief Input filtered signal type.
 */
using InputFilteredSignalType = Signal<void(TextField, Toolkit::InputFilter::Property::Type)>;

/**
 * @brief This signal is emitted when the character to be inserted is filtered by the input filter.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName(TextField textField, Toolkit::InputFilter::Property::Type type);
 *
 *   DevelTextField::InputFilteredSignal(textField).Connect(this, &OnInputFiltered);
 *
 *   void OnInputFiltered(TextField textField, InputFilter::Property::Type type)
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
 * @param[in] textField The instance of TextField.
 * @return The signal to connect to.
 */
DALI_TOOLKIT_API InputFilteredSignalType& InputFilteredSignal(TextField textField);

/**
 * @brief selection changed signal type.
 *
 * @note Signal
 *  - uint32_t  : selection start before the change.
 *  - uint32_t  : selection end before the change.
 */
using SelectionChangedSignalType = Signal<void(TextField, uint32_t, uint32_t)>;

/**
 * @brief This signal is emitted when the selection has been changed.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName( TextField textField, uint32_t oldStart, uint32_t oldEnd);
 * @endcode
 * @param[in] textField The instance of TextField.
 * @return The signal to connect to
 */
DALI_TOOLKIT_API SelectionChangedSignalType& SelectionChangedSignal(TextField textField);

/**
 * @brief selection cleared signal type.
 */
using SelectionClearedSignalType = Signal<void(TextField)>;

/**
 * @brief This signal is emitted when the selection has been cleared.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName( TextField textField);
 * @endcode
 * @param[in] textField The instance of TextField.
 * @return The signal to connect to
 */
DALI_TOOLKIT_API SelectionClearedSignalType& SelectionClearedSignal(TextField textField);

/**
 * @brief selection start signal type.
 */
using SelectionStartedSignalType = Signal<void(TextField)>;

/**
 * @brief This signal is emitted when the selection start.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName( TextField textField);
 * @endcode
 * @param[in] textField The instance of TextField.
 * @return The signal to connect to
 */
DALI_TOOLKIT_API SelectionStartedSignalType& SelectionStartedSignal(TextField textField);

/**
 * @brief Get the rendered size of a specific text range.
 * if the requested text is at multilines, multiple sizes will be returned for each text located in a separate line.
 * if a line contains characters with different directions, multiple sizes will be returned for each block of contiguous characters with the same direction.
 *
 * @param[in] textField The instance of TextField.
 * @param[in] startIndex start index of the text requested to calculate size for.
 * @param[in] endIndex end index(included) of the text requested to calculate size for.
 * @return list of sizes of the reuested text.
 */
DALI_TOOLKIT_API Vector<Vector2> GetTextSize(TextField textField, const uint32_t startIndex, const uint32_t endIndex);

/**
 * @brief Get the top/left rendered position of a specific text range.
 * if the requested text is at multilines, multiple positions will be returned for each text located in a separate line.
 * if a line contains characters with different directions, multiple positions will be returned for each block of contiguous characters with the same direction.
 *
 * @param[in] textField The instance of TextField.
 * @param[in] startIndex start index of the text requested to get position to.
 * @param[in] endIndex end index(included) of the text requested to get position to.
 * @return list of positions of the requested text.
 */
DALI_TOOLKIT_API Vector<Vector2> GetTextPosition(TextField textField, const uint32_t startIndex, const uint32_t endIndex);

/**
 * @brief Gets the bounding box of a specific text range.
 *
 * @param[in] textField The instance of textField.
 * @param[in] startIndex start index of the text requested to get bounding box to.
 * @param[in] endIndex end index(included) of the text requested to get bounding box to.
 * @return bounding box of the requested text.
 */
DALI_TOOLKIT_API Rect<> GetTextBoundingRectangle(TextField textField, uint32_t startIndex, uint32_t endIndex);

/**
 * @brief Select the whole text of TextField.
 *
 * @param[in] textField The instance of TextField.
 */
DALI_TOOLKIT_API void SelectWholeText(TextField textField);

/**
 * @brief Unselect the whole text of TextField.
 *
 * @param[in] textField The instance of TextField.
 */
DALI_TOOLKIT_API void SelectNone(TextField textField);

/**
 * @brief Select the text from start index to end index of TextField.
 * @note
 * The selection index is based on the cursor index.
 *
 * text   H e l l o
 * index 0 1 2 3 4 5
 * if textField.SelectText(1, 4); is executed, "ell" is selected.
 *
 * @param[in] textField The instance of TextField.
 * @param[in] start The start index of the text to select. (The starting point of start index is 0.)
 * @param[in] end The end index of the text to select. (If end index > text's length, the end index is set to the length of the text.)
 */
DALI_TOOLKIT_API void SelectText(TextField textField, const uint32_t start, const uint32_t end);

/**
 * @brief Copy and return the selected text of TextField.
 *
 * @param[in] textField The instance of TextField.
 * @return The copied text.
 */
DALI_TOOLKIT_API std::string CopyText(TextField textField);

/**
 * @brief Cut and return the selected text of TextField.
 *
 * @param[in] textField The instance of TextField.
 * @return The cut text.
 */
DALI_TOOLKIT_API std::string CutText(TextField textField);

/**
 * @brief Paste the most recent clipboard text item into the TextField.
 *
 * @param[in] textField The instance of TextField.
 */
DALI_TOOLKIT_API void PasteText(TextField textField);

/**
 * @brief Set removing front inset to TextField.
 *
 * @param[in] textField The instance of TextField.
 * @param[in] remove Whether front inset of TextField has to be removed or not.
 */
DALI_TOOLKIT_API void SetRemoveFrontInset(TextField textField, const bool remove);

/**
 * @brief Whether front inset of TextField is removed or not.
 *
 * @param[in] textField The instance of TextField.
 * @return True if the front inset of TextField is removed.
 */
DALI_TOOLKIT_API bool IsRemoveFrontInset(TextField textField);

/**
 * @brief Set removing back inset to TextField.
 *
 * @param[in] textField The instance of TextField.
 * @param[in] remove Whether back inset of TextField has to be removed or not.
 */
DALI_TOOLKIT_API void SetRemoveBackInset(TextField textField, const bool remove);

/**
 * @brief Whether back inset of TextField is removed or not.
 *
 * @param[in] textField The instance of TextField.
 * @return True if the back inset of TextField is removed.
 */
DALI_TOOLKIT_API bool IsRemoveBackInset(TextField textField);

/**
 * @brief Registers a new font variation property based on the provided tag.
 *
 * @param[in] textField The instance of TextField.
 * @param[in] tag A 4-character string representing the variation property tag.
 * @return The index of the registered variation property. Property::INVALID_INDEX if failed.
 */
DALI_TOOLKIT_API Dali::Property::Index RegisterFontVariationProperty(TextField textField, std::string tag);

} // namespace DevelTextField

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_FIELD_DEVEL_H
