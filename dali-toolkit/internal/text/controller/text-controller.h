#ifndef DALI_TOOLKIT_TEXT_CONTROLLER_H
#define DALI_TOOLKIT_TEXT_CONTROLLER_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/text/spanned.h>
#include <dali/devel-api/adaptor-framework/clipboard.h>
#include <dali/devel-api/adaptor-framework/input-method-context.h>
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/events/gesture.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/text-controls/text-anchor-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup-callback-interface.h>
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali-toolkit/internal/text/decorator/text-decorator.h>
#include <dali-toolkit/internal/text/hidden-text.h>
#include <dali-toolkit/internal/text/input-filter.h>
#include <dali-toolkit/internal/text/layouts/layout-engine.h>
#include <dali-toolkit/internal/text/text-anchor-control-interface.h>
#include <dali-toolkit/internal/text/text-model-interface.h>
#include <dali-toolkit/internal/text/text-selectable-control-interface.h>
#include <dali-toolkit/public-api/text/text-enumerations.h>

namespace Dali::Toolkit::Text
{
class Controller;
class ControlInterface;
class EditableControlInterface;
class View;
class RenderingController;

/**
   * @brief Text selection operations .
   */
enum SelectionType
{
  INTERACTIVE = 0x0000, ///< Select the word where the cursor is located.
  ALL         = 0x0001, ///< Select the whole text.
  NONE        = 0x0002, ///< Unselect the whole text.
  RANGE       = 0x0003  ///< Select the range text.
};

typedef IntrusivePtr<Controller> ControllerPtr;

/**
 * @brief A Text Controller is used by UI Controls which display text.
 *
 * It manipulates the Logical & Visual text models on behalf of the UI Controls.
 * It provides a view of the text that can be used by rendering back-ends.
 *
 * For selectable/editable UI controls, the controller handles input events from the UI control
 * and decorations (grab handles etc) via the Decorator::ControllerInterface interface.
 *
 * The text selection popup button callbacks are as well handled via the TextSelectionPopupCallbackInterface interface.
 */
class Controller : public RefObject,
                   public Decorator::ControllerInterface,
                   public TextSelectionPopupCallbackInterface,
                   public HiddenText::Observer,
                   public ConnectionTracker,
                   public Integration::Processor
{
public: // Enumerated types.
  /**
   * @brief Text related operations to be done in the relayout process.
   */
  enum OperationsMask
  {
    NO_OPERATION       = 0x0000,
    CONVERT_TO_UTF32   = 0x0001,
    GET_SCRIPTS        = 0x0002,
    VALIDATE_FONTS     = 0x0004,
    GET_LINE_BREAKS    = 0x0008,
    BIDI_INFO          = 0x0010,
    SHAPE_TEXT         = 0x0020,
    GET_GLYPH_METRICS  = 0x0040,
    LAYOUT             = 0x0080,
    UPDATE_LAYOUT_SIZE = 0x0100,
    REORDER            = 0x0200,
    ALIGN              = 0x0400,
    COLOR              = 0x0800,
    UPDATE_DIRECTION   = 0x1000,
    ALL_OPERATIONS     = 0xFFFF
  };

  /**
   * @brief Used to distinguish between regular key events and InputMethodContext events
   */
  enum InsertType
  {
    COMMIT,
    PRE_EDIT
  };

  /**
   * @brief Used to specify whether to update the input style.
   */
  enum UpdateInputStyleType
  {
    UPDATE_INPUT_STYLE,
    DONT_UPDATE_INPUT_STYLE
  };

  /**
   * @brief Used to specify what has been updated after the Relayout() method has been called.
   */
  enum UpdateTextType
  {
    NONE_UPDATED      = 0x0, ///< Nothing has been updated.
    MODEL_UPDATED     = 0x1, ///< The text's model has been updated.
    DECORATOR_UPDATED = 0x2  ///< The decoration has been updated.
  };

  /**
   * @brief Different placeholder-text can be shown when the control is active/inactive.
   */
  enum PlaceholderType
  {
    PLACEHOLDER_TYPE_ACTIVE,
    PLACEHOLDER_TYPE_INACTIVE,
  };

  /**
   * @brief Enumeration for Font Size Type.
   */
  enum FontSizeType
  {
    POINT_SIZE, // The size of font in points.
    PIXEL_SIZE  // The size of font in pixels.
  };

  struct NoTextTap
  {
    enum Action
    {
      NO_ACTION,           ///< Does no action if there is a tap on top of an area with no text.
      HIGHLIGHT,           ///< Highlights the nearest text (at the beginning or end of the text) and shows the text's selection popup.
      SHOW_SELECTION_POPUP ///< Shows the text's selection popup.
    };
  };

  struct TextFitInfo
  {
    enum Property
    {
      TEXT_FIT_ENABLE,
      TEXT_FIT_MIN_SIZE,
      TEXT_FIT_MAX_SIZE,
      TEXT_FIT_STEP_SIZE,
      TEXT_FIT_FONT_SIZE_TYPE
    };
  };

public: // Constructor.
  /**
   * @brief Create a new instance of a Controller.
   *
   * @return A pointer to a new Controller.
   */
  static ControllerPtr New()
  {
    return ControllerPtr(new Controller());
  }

  /**
   * @brief Create a new instance of a Controller.
   *
   * @param[in] controlInterface The control's interface.
   *
   * @return A pointer to a new Controller.
   */
  static ControllerPtr New(ControlInterface* controlInterface)
  {
    return ControllerPtr(new Controller(controlInterface));
  }

  /**
   * @brief Create a new instance of a Controller.
   *
   * @param[in] controlInterface The control's interface.
   * @param[in] editableControlInterface The editable control's interface.
   * @param[in] selectableControlInterface The selectable control's interface.
   * @param[in] anchorControlInterface The anchor control's interface.
   *
   * @return A pointer to a new Controller.
   */
  static ControllerPtr New(ControlInterface*           controlInterface,
                           EditableControlInterface*   editableControlInterface,
                           SelectableControlInterface* selectableControlInterface,
                           AnchorControlInterface*     anchorControlInterface)
  {
    return ControllerPtr(new Controller(controlInterface,
                                        editableControlInterface,
                                        selectableControlInterface,
                                        anchorControlInterface));
  }

public: // Configure the text controller.
  /**
   * @brief Called to enable text input.
   *
   * @note Selectable or editable controls should call this once after Controller::New().
   * @param[in] decorator Used to create cursor, selection handle decorations etc.
   * @param[in] inputMethodContext Used to manager ime.
   */
  void EnableTextInput(DecoratorPtr decorator, InputMethodContext& inputMethodContext);

  /**
   * @brief Used to switch between bitmap & vector based glyphs
   *
   * @param[in] glyphType The type of glyph; note that metrics for bitmap & vector based glyphs are different.
   */
  void SetGlyphType(TextAbstraction::GlyphType glyphType);

  /**
   * @brief Enables/disables the mark-up processor.
   *
   * By default is disabled.
   *
   * @param[in] enable Whether to enable the mark-up processor.
   */
  void SetMarkupProcessorEnabled(bool enable);

  /**
   * @brief Retrieves whether the mark-up processor is enabled.
   *
   * By default is disabled.
   *
   * @return @e true if the mark-up processor is enabled, otherwise returns @e false.
   */
  bool IsMarkupProcessorEnabled() const;

  /**
   * @brief Retrieves whether the current text contains anchors.
   *
   * @return @e true if the current text contains anchors. @e false.
   */
  bool HasAnchors() const;

  /**
   * @brief Enables/disables the auto text scrolling
   *
   * By default is disabled.
   *
   * @param[in] enable Whether to enable the auto scrolling
   * @param[in] requestRelayout Whether to request the relayout
   */
  void SetAutoScrollEnabled(bool enable, bool requestRelayout = true);

  /**
   * @brief Whether the auto scrolling texture exceed max texture.
   *
   * By default is false.
   *
   * @param[in] exceed Whether the auto scrolling texture exceed max texture.
   */
  void SetAutoScrollMaxTextureExceeded(bool exceed);

  /**
   * @brief Retrieves whether auto text scrolling is enabled.
   *
   * By default is disabled.
   *
   * @return @e true if auto scrolling is enabled, otherwise returns @e false.
   */
  bool IsAutoScrollEnabled() const;

  /**
   * @brief Get direction of the text from the first line of text,
   * @return bool rtl (right to left) is true
   */
  CharacterDirection GetAutoScrollDirection() const;

  /**
   * @brief Get the alignment offset of the first line of text.
   *
   * @return The alignment offset.
   */
  float GetAutoScrollLineAlignment() const;

  /**
   * @brief Enables the horizontal scrolling.
   *
   * @param[in] enable Whether to enable the horizontal scrolling.
   */
  void SetHorizontalScrollEnabled(bool enable);

  /**
   * @brief Retrieves whether the horizontal scrolling is enabled.
   *
   * @return @e true if the horizontal scrolling is enabled, otherwise it returns @e false.
   */
  bool IsHorizontalScrollEnabled() const;

  /**
   * @brief Enables the vertical scrolling.
   *
   * @param[in] enable Whether to enable the vertical scrolling.
   */
  void SetVerticalScrollEnabled(bool enable);

  /**
   * @brief Retrieves whether the verticall scrolling is enabled.
   *
   * @return @e true if the vertical scrolling is enabled, otherwise it returns @e false.
   */
  bool IsVerticalScrollEnabled() const;

  /**
   * @brief Enables the smooth handle panning.
   *
   * @param[in] enable Whether to enable the smooth handle panning.
   */
  void SetSmoothHandlePanEnabled(bool enable);

  /**
   * @brief Retrieves whether the smooth handle panning is enabled.
   *
   * @return @e true if the smooth handle panning is enabled.
   */
  bool IsSmoothHandlePanEnabled() const;

  /**
   * @brief Sets the maximum number of characters that can be inserted into the TextModel
   *
   * @param[in] maxCharacters maximum number of characters to be accepted
   */
  void SetMaximumNumberOfCharacters(Length maxCharacters);

  /**
   * @brief Sets the maximum number of characters that can be inserted into the TextModel
   *
   * @param[in] maxCharacters maximum number of characters to be accepted
   */
  int GetMaximumNumberOfCharacters();

  /**
   * @brief Called to enable/disable cursor blink.
   *
   * @note Only editable controls should calls this.
   * @param[in] enabled Whether the cursor should blink or not.
   */
  void SetEnableCursorBlink(bool enable);

  /**
   * @brief Query whether cursor blink is enabled.
   *
   * @return Whether the cursor should blink or not.
   */
  bool GetEnableCursorBlink() const;

  /**
   * @brief Whether to enable the multi-line layout.
   *
   * @param[in] enable \e true enables the multi-line (by default)
   */
  void SetMultiLineEnabled(bool enable);

  /**
   * @return Whether the multi-line layout is enabled.
   */
  bool IsMultiLineEnabled() const;

  /**
   * @brief Sets the text's horizontal alignment.
   *
   * @param[in] alignment The horizontal alignment.
   */
  void SetHorizontalAlignment(HorizontalAlignment::Type alignment);

  /**
   * @copydoc ModelInterface::GetHorizontalAlignment()
   */
  HorizontalAlignment::Type GetHorizontalAlignment() const;

  /**
   * @brief Sets the text's vertical alignment.
   *
   * @param[in] alignment The vertical alignment.
   */
  void SetVerticalAlignment(VerticalAlignment::Type alignment);

  /**
   * @copydoc ModelInterface::GetVerticalAlignment()
   */
  VerticalAlignment::Type GetVerticalAlignment() const;

  /**
   * @brief Sets the text's wrap mode
   * @param[in] text wrap mode The unit of wrapping
   */
  void SetLineWrapMode(Text::LineWrap::Mode textWarpMode);

  /**
   * @brief Retrieve text wrap mode previously set.
   * @return text wrap mode
   */
  Text::LineWrap::Mode GetLineWrapMode() const;

  /**
   * @brief Enable or disable the text elide.
   *
   * @param[in] enabled Whether to enable the text elide.
   */
  void SetTextElideEnabled(bool enabled);

  /**
   * @copydoc ModelInterface::IsTextElideEnabled()
   */
  bool IsTextElideEnabled() const;

  /**
   * @brief Enable or disable the text fit.
   *
   * @param[in] enabled Whether to enable the text fit.
   */
  void SetTextFitEnabled(bool enabled);

  /**
   * @brief Whether the text fit is enabled or not.
   *
   * @return True if the text fit is enabled
   */
  bool IsTextFitEnabled() const;

  /**
   * @brief Sets current line size.
   *
   * @param[in] lineSize line size value to store the MinLineSize set by user when TextFitArray is enabled.
   */
  void SetCurrentLineSize(float lineSize);

  /**
   * @brief Retrieves the current line size.
   *
   * @return The current line size
   */
  float GetCurrentLineSize() const;

  /**
   * @brief Sets minimum size valid for text fit.
   *
   * @param[in] minimum size value.
   * @param[in] type The font size type is point size or pixel size
   */
  void SetTextFitMinSize(float pointSize, FontSizeType type);

  /**
   * @brief Retrieves the minimum point size valid for text fit.
   *
   * @return The minimum point size valid for text fit
   */
  float GetTextFitMinSize() const;

  /**
   * @brief Sets maximum size valid for text fit.
   *
   * @param[in] maximum size value.
   * @param[in] type The font size type is point size or pixel size
   */
  void SetTextFitMaxSize(float pointSize, FontSizeType type);

  /**
   * @brief Retrieves the maximum point size valid for text fit.
   *
   * @return The maximum point size valid for text fit
   */
  float GetTextFitMaxSize() const;

  /**
   * @brief Sets step size for font increase valid for text fit.
   *
   * @param[in] step size value.
   * @param[in] type The font size type is point size or pixel size
   */
  void SetTextFitStepSize(float step, FontSizeType type);

  /**
   * @brief Retrieves the step point size valid for text fit.
   *
   * @return The step point size valid for text fit
   */
  float GetTextFitStepSize() const;

  /**
   * @brief Sets content size valid for text fit.
   *
   * @param[in] Content size value.
   */
  void SetTextFitContentSize(Vector2 size);

  /**
   * @brief Retrieves the content size valid for text fit.
   *
   * @return The content size valid for text fit
   */
  Vector2 GetTextFitContentSize() const;

  /**
   * @brief Retrieve the fited point size.
   *
   * @return The fited point size.
   */
  float GetTextFitPointSize() const;

  /**
   * @brief Sets the text fit point size.
   *
   * @param[in] pointSize The fited point size.
   */
  void SetTextFitPointSize(float pointSize);

  /**
   * @brief Sets whether the text fit properties have changed.
   *
   * @param[in] changed Whether to changed the text fit.
   */
  void SetTextFitChanged(bool changed);

  /**
   * @brief Whether the text fit properties are changed or not.
   *
   * @return True if the text fit properties are changed
   */
  bool IsTextFitChanged() const;

  /**
   * @brief Sets line size valid for text fit.
   *
   * @param[in] lineSize line size value
   */
  void SetTextFitLineSize(float lineSize);

  /**
   * @brief Enable or disable the text fit array.
   *
   * @param[in] enabled Whether to enable the text fit array.
   */
  void SetTextFitArrayEnabled(bool enabled);

  /**
   * @brief Whether the text fit array is enabled or not.
   *
   * @return True if the text fit array is enabled.
   */
  bool IsTextFitArrayEnabled() const;

  /**
   * @brief Sets the text fit array.
   *
   * @param[in] fitOptions The list of text fit options.
   */
  void SetTextFitArray(std::vector<Toolkit::DevelTextLabel::FitOption>& fitOptions);

  /**
   * @brief Retrieve the text fit array.
   *
   * @return The list of text fit options.
   */
  std::vector<Toolkit::DevelTextLabel::FitOption>& GetTextFitArray();

  /**
   * @brief Sets disabled color opacity.
   *
   * @param[in] opacity The color opacity value in disabled state.
   */
  void SetDisabledColorOpacity(float opacity);

  /**
   * @brief Retrieves the disabled color opacity.
   *
   * @return The disabled color opacity value for disabled state.
   */
  float GetDisabledColorOpacity() const;

  /**
   * @brief Enable or disable the placeholder text elide.
   * @param enabled Whether to enable the placeholder text elide.
   */
  void SetPlaceholderTextElideEnabled(bool enabled);

  /**
   * @brief Whether the placeholder text elide property is enabled.
   * @return True if the placeholder text elide property is enabled, false otherwise.
   */
  bool IsPlaceholderTextElideEnabled() const;

  /**
   * @brief Enable or disable the text selection.
   * @param[in] enabled Whether to enable the text selection.
   */
  void SetSelectionEnabled(bool enabled);

  /**
   * @brief Whether the text selection is enabled or not.
   * @return True if the text selection is enabled
   */
  bool IsSelectionEnabled() const;

  /**
   * @brief Enable or disable the text selection using Shift key.
   * @param enabled Whether to enable the text selection using Shift key
   */
  void SetShiftSelectionEnabled(bool enabled);

  /**
   * @brief Whether the text selection using Shift key is enabled or not.
   * @return True if the text selection using Shift key is enabled
   */
  bool IsShiftSelectionEnabled() const;

  /**
   * @brief Enable or disable the grab handles for text selection.
   *
   * @param[in] enabled Whether to enable the grab handles
   */
  void SetGrabHandleEnabled(bool enabled);

  /**
   * @brief Returns whether the grab handles are enabled.
   *
   * @return True if the grab handles are enabled
   */
  bool IsGrabHandleEnabled() const;

  /**
   * @brief Enable or disable the grab handles for text selection.
   *
   * @param[in] enabled Whether to enable the grab handles
   */
  void SetGrabHandlePopupEnabled(bool enabled);

  /**
   * @brief Returns whether the grab handles are enabled.
   *
   * @return True if the grab handles are enabled
   */
  bool IsGrabHandlePopupEnabled() const;

  /**
   * @brief Sets input type to password
   *
   * @note The string is displayed hidden character
   *
   * @param[in] passwordInput True if password input is enabled.
   */
  void SetInputModePassword(bool passwordInput);

  /**
   * @brief Returns whether the input mode type is set as password.
   *
   * @return True if input mode type is password
   */
  bool IsInputModePassword();

  /**
   * @brief Sets the action when there is a double tap event on top of a text area with no text.
   *
   * @param[in] action The action to do.
   */
  void SetNoTextDoubleTapAction(NoTextTap::Action action);

  /**
   * @brief Retrieves the action when there is a double tap event on top of a text area with no text.
   *
   * @return The action to do.
   */
  NoTextTap::Action GetNoTextDoubleTapAction() const;

  /**
   * @briefSets the action when there is a long press event on top of a text area with no text.
   *
   * @param[in] action The action to do.
   */
  void SetNoTextLongPressAction(NoTextTap::Action action);

  /**
   * @brief Retrieves the action when there is a long press event on top of a text area with no text.
   *
   * @return The action to do.
   */
  NoTextTap::Action GetNoTextLongPressAction() const;

  /**
   * @brief Query if Underline settings were provided by string or map
   * @return bool true if set by string
   */
  bool IsUnderlineSetByString();

  /**
   * Set method underline setting were set by
   * @param[in] bool, true if set by string
   */
  void UnderlineSetByString(bool setByString);

  /**
   * @brief Query if shadow settings were provided by string or map
   * @return bool true if set by string
   */
  bool IsShadowSetByString();

  /**
   * Set method shadow setting were set by
   * @param[in] bool, true if set by string
   */
  void ShadowSetByString(bool setByString);

  /**
   * @brief Query if outline settings were provided by string or map
   * @return bool true if set by string
   */
  bool IsOutlineSetByString();

  /**
   * Set method outline setting were set by
   * @param[in] bool, true if set by string
   */
  void OutlineSetByString(bool setByString);

  /**
   * @brief Query if font style settings were provided by string or map
   * @return bool true if set by string
   */
  bool IsFontStyleSetByString();

  /**
   * Set method font style setting were set by
   * @param[in] bool, true if set by string
   */
  void FontStyleSetByString(bool setByString);

  /**
   * @brief Query if Strikethrough settings were provided by string or map
   * @return bool true if set by string
   */
  bool IsStrikethroughSetByString();

  /**
   * Set method Strikethrough setting were set by
   * @param[in] bool, true if set by string
   */
  void StrikethroughSetByString(bool setByString);

  /**
   * @brief Set the override used for strikethrough height, 0 indicates height will be supplied by font metrics
   *
   * @param[in] height The height in pixels of the strikethrough
   */
  void SetStrikethroughHeight(float height);

  /**
   * @brief Retrieves the override height of an strikethrough, 0 indicates height is supplied by font metrics
   *
   * @return The height of the strikethrough, or 0 if height is not overrided.
   */
  float GetStrikethroughHeight() const;

  /**
   * @brief Set the strikethrough color.
   *
   * @param[in] color color of strikethrough.
   */
  void SetStrikethroughColor(const Vector4& color);

  /**
   * @brief Retrieve the strikethrough color.
   *
   * @return The strikethrough color.
   */
  const Vector4& GetStrikethroughColor() const;

  /**
   * @brief Set the strikethrough enabled flag.
   *
   * @param[in] enabled The strikethrough enabled flag.
   */
  void SetStrikethroughEnabled(bool enabled);

  /**
   * @brief Returns whether the text has a strikethrough or not.
   *
   * @return The strikethrough state.
   */
  bool IsStrikethroughEnabled() const;

public: // Update.
  /**
   * @brief Replaces any text previously set.
   *
   * @note This will be converted into UTF-32 when stored in the text model.
   * @param[in] text A string of UTF-8 characters.
   */
  void SetText(const std::string& text);

  /**
   * @brief Retrieve any text previously set.
   *
   * @param[out] text A string of UTF-8 characters.
   */
  void GetText(std::string& text) const;

  /**
   * @brief Retrieve raw text previously set.
   *
   * @note Raw text including markup tag.
   * @param[out] text A string of UTF-8 characters.
   */
  void GetRawText(std::string& text) const;

  /**
   * @brief Retrieves number of characters previously set.
   *
   * @return A length of string of UTF-32 characters.
   */
  Length GetNumberOfCharacters() const;

  /**
 * @brief Set the @p spannedText
 * the spanned text contains content (text) and  format (spans with ranges)
 * the text is copied into text-controller and the spans are applied on ranges
 *
 * @param[in] spannedText the text with spans.
 */
  void SetSpannedText(const Text::Spanned& spannedText);

  /**
   * @brief Replaces any placeholder text previously set.
   *
   * @param[in] type Different placeholder-text can be shown when the control is active/inactive.
   * @param[in] text A string of UTF-8 characters.
   */
  void SetPlaceholderText(PlaceholderType type, const std::string& text);

  /**
   * @brief Retrieve any placeholder text previously set.
   *
   * @param[in] type Different placeholder-text can be shown when the control is active/inactive.
   * @param[out] A string of UTF-8 characters.
   */
  void GetPlaceholderText(PlaceholderType type, std::string& text) const;

  /**
   * @ brief Update the text after a font change
   * @param[in] newDefaultFont The new font to change to
   */
  void UpdateAfterFontChange(const std::string& newDefaultFont);

  /**
   * @brief The method acquires currently selected text
   * @param selectedText variable to place selected text in
   */
  void RetrieveSelection(std::string& selectedText) const;

  /**
   * @brief The method sets selection in given range
   * @param start index of first character
   * @param end   index of first character after selection
   */
  void SetSelection(int start, int end);

  /**
   * @brief This method retrieve indexes of current selection
   *
   * @return a pair, where first element is left index of selection and second is the right one
   */
  std::pair<int, int> GetSelectionIndexes() const;

  /**
   * Place string in system clipboard
   * @param source std::string
   */
  void CopyStringToClipboard(const std::string& source);

  /**
   * Place currently selected text in system clipboard
   * @param deleteAfterSending flag pointing if text should be deleted after sending to clipboard
   */
  void SendSelectionToClipboard(bool deleteAfterSending);

public: // Default style & Input style
  /**
   * @brief Set the default font family.
   *
   * @param[in] defaultFontFamily The default font family.
   */
  void SetDefaultFontFamily(const std::string& defaultFontFamily);

  /**
   * @brief Retrieve the default font family.
   *
   * @return The default font family.
   */
  std::string GetDefaultFontFamily() const;

  /**
   * @brief Sets the placeholder text font family.
   * @param[in] placeholderTextFontFamily The placeholder text font family.
   */
  void SetPlaceholderFontFamily(const std::string& placeholderTextFontFamily);

  /**
   * @brief Retrieves the placeholder text font family.
   *
   * @return The placeholder text font family
   */
  std::string GetPlaceholderFontFamily() const;

  /**
   * @brief Sets the default font weight.
   *
   * @param[in] weight The font weight.
   */
  void SetDefaultFontWeight(FontWeight weight);

  /**
   * @brief Whether the font's weight has been defined.
   */
  bool IsDefaultFontWeightDefined() const;

  /**
   * @brief Retrieves the default font weight.
   *
   * @return The default font weight.
   */
  FontWeight GetDefaultFontWeight() const;

  /**
   * @brief Sets the placeholder text font weight.
   *
   * @param[in] weight The font weight
   */
  void SetPlaceholderTextFontWeight(FontWeight weight);

  /**
   * @brief Whether the font's weight has been defined.
   *
   * @return True if the placeholder text font weight is defined
   */
  bool IsPlaceholderTextFontWeightDefined() const;

  /**
   * @brief Retrieves the placeholder text font weight.
   *
   * @return The placeholder text font weight
   */
  FontWeight GetPlaceholderTextFontWeight() const;

  /**
   * @brief Sets the default font width.
   *
   * @param[in] width The font width.
   */
  void SetDefaultFontWidth(FontWidth width);

  /**
   * @brief Whether the font's width has been defined.
   */
  bool IsDefaultFontWidthDefined() const;

  /**
   * @brief Retrieves the default font width.
   *
   * @return The default font width.
   */
  FontWidth GetDefaultFontWidth() const;

  /**
   * @brief Sets the placeholder text font width.
   *
   * @param[in] width The font width
   */
  void SetPlaceholderTextFontWidth(FontWidth width);

  /**
   * @brief Whether the font's width has been defined.
   *
   * @return True if the placeholder text font width is defined
   */
  bool IsPlaceholderTextFontWidthDefined() const;

  /**
   * @brief Retrieves the placeholder text font width.
   *
   * @return The placeholder text font width
   */
  FontWidth GetPlaceholderTextFontWidth() const;

  /**
   * @brief Sets the default font slant.
   *
   * @param[in] slant The font slant.
   */
  void SetDefaultFontSlant(FontSlant slant);

  /**
   * @brief Whether the font's slant has been defined.
   */
  bool IsDefaultFontSlantDefined() const;

  /**
   * @brief Retrieves the default font slant.
   *
   * @return The default font slant.
   */
  FontSlant GetDefaultFontSlant() const;

  /**
   * @brief Sets the placeholder text font slant.
   *
   * @param[in] slant The font slant
   */
  void SetPlaceholderTextFontSlant(FontSlant slant);

  /**
   * @brief Whether the font's slant has been defined.
   *
   * @return True if the placeholder text font slant is defined
   */
  bool IsPlaceholderTextFontSlantDefined() const;

  /**
   * @brief Retrieves the placeholder text font slant.
   *
   * @return The placeholder text font slant
   */
  FontSlant GetPlaceholderTextFontSlant() const;

  /**
   * @brief Set the default font size.
   *
   * @param[in] fontSize The default font size
   * @param[in] type The font size type is point size or pixel size
   */
  void SetDefaultFontSize(float fontSize, FontSizeType type);

  /**
   * @brief Retrieve the default point size.
   *
   * @param[in] type The font size type
   * @return The default point size.
   */
  float GetDefaultFontSize(FontSizeType type) const;

  /**
   * @brief Set the font size scale.
   *
   * @param[in] scale The font size scale
   */
  void SetFontSizeScale(float scale);

  /**
   * @brief Get the font size scale.
   *
   * @return The font size scale.
   */
  float GetFontSizeScale() const;

  /**
   * @brief Set the font size scale enabled flag.
   *
   * @param[in] enabled whether to enable the font size scale.
   */
  void SetFontSizeScaleEnabled(bool enabled);

  /**
   * @brief Returns whether the font size scale is enabled or not.
   *
   * @return @e true if the font size scale is enabled, otherwise returns @e false.
   */
  bool IsFontSizeScaleEnabled() const;

  /**
   * @brief Sets the Placeholder text font size.
   * @param[in] fontSize The placeholder text font size
   * @param[in] type The font size type is point size or pixel size
   */
  void SetPlaceholderTextFontSize(float fontSize, FontSizeType type);

  /**
   * @brief Retrieves the Placeholder text font size.
   * @param[in] type The font size type
   * @return The placeholder font size
   */
  float GetPlaceholderTextFontSize(FontSizeType type) const;

  /**
   * @brief Sets the text's default color.
   *
   * @param color The default color.
   */
  void SetDefaultColor(const Vector4& color);

  /**
   * @brief Retrieves the text's default color.
   *
   * @return The default color.
   */
  const Vector4& GetDefaultColor() const;

  /**
   * @brief Sets the anchor's default color.
   *
   * @param color The anchor color.
   */
  void SetAnchorColor(const Vector4& color);

  /**
   * @brief Retrieves the anchor's default color.
   *
   * @return The anchor color.
   */
  const Vector4& GetAnchorColor() const;

  /**
   * @brief Sets the anchor's clicked color.
   *
   * @param color The anchor color.
   */
  void SetAnchorClickedColor(const Vector4& color);

  /**
   * @brief Retrieves the anchor's clicked color.
   *
   * @return The anchor color.
   */
  const Vector4& GetAnchorClickedColor() const;

  /**
   * @brief Sets the user interaction enabled.
   *
   * @param enabled whether to enable the user interaction.
   */
  void SetUserInteractionEnabled(bool enabled);

  /**
   * @brief Whether the user interaction is enabled.
   *
   * @return true if the user interaction is enabled, false otherwise.
   */
  bool IsUserInteractionEnabled() const;

  /**
   * @brief Set the text color
   *
   * @param textColor The text color
   */
  void SetPlaceholderTextColor(const Vector4& textColor);

  /**
   * @brief Retrieve the text color
   *
   * @return The text color
   */
  const Vector4& GetPlaceholderTextColor() const;

  /**
   * @brief Set the shadow offset.
   *
   * @param[in] shadowOffset The shadow offset, 0,0 indicates no shadow.
   */
  void SetShadowOffset(const Vector2& shadowOffset);

  /**
   * @brief Retrieve the shadow offset.
   *
   * @return The shadow offset.
   */
  const Vector2& GetShadowOffset() const;

  /**
   * @brief Set the shadow color.
   *
   * @param[in] shadowColor The shadow color.
   */
  void SetShadowColor(const Vector4& shadowColor);

  /**
   * @brief Retrieve the shadow color.
   *
   * @return The shadow color.
   */
  const Vector4& GetShadowColor() const;

  /**
   * @brief Set the shadow blur radius.
   *
   * @param[in] shadowBlurRadius The shadow blur radius, 0,0 indicates no blur.
   */
  void SetShadowBlurRadius(const float& shadowBlurRadius);

  /**
   * @brief Retrieve the shadow blur radius.
   *
   * @return The shadow blur radius.
   */
  const float& GetShadowBlurRadius() const;

  /**
   * @brief Set the underline color.
   *
   * @param[in] color color of underline.
   */
  void SetUnderlineColor(const Vector4& color);

  /**
   * @brief Retrieve the underline color.
   *
   * @return The underline color.
   */
  const Vector4& GetUnderlineColor() const;

  /**
   * @brief Set the underline enabled flag.
   *
   * @param[in] enabled The underline enabled flag.
   */
  void SetUnderlineEnabled(bool enabled);

  /**
   * @brief Returns whether the text is underlined or not.
   *
   * @return The underline state.
   */
  bool IsUnderlineEnabled() const;

  /**
   * @brief Set the override used for underline height, 0 indicates height will be supplied by font metrics
   *
   * @param[in] height The height in pixels of the underline
   */
  void SetUnderlineHeight(float height);

  /**
   * @brief Retrieves the override height of an underline, 0 indicates height is supplied by font metrics
   *
   * @return The height of the underline, or 0 if height is not overrided.
   */
  float GetUnderlineHeight() const;

  /**
   * @brief Sets the underline type.
   * @param[in] type The underline type.
   */
  void SetUnderlineType(Text::Underline::Type type);

  /**
   * @brief Retrieve underline type.
   * @return The underline type.
   */
  Text::Underline::Type GetUnderlineType() const;

  /**
   * @brief Set the width of the dashes of the dashed underline.
   *
   * @param[in] width The width in pixels of the dashes of the dashed underline.
   */
  void SetDashedUnderlineWidth(float width);

  /**
   * @brief Retrieves the width of the dashes of the dashed underline.
   *
   * @return The width of the dashes of the dashed underline.
   */
  float GetDashedUnderlineWidth() const;

  /**
   * @brief Set the gap between the dashes of the dashed underline.
   *
   * @param[in] gap The gap between the dashes of the dashed underline.
   */
  void SetDashedUnderlineGap(float gap);

  /**
   * @brief Retrieves the gap between the dashes of the dashed underline.
   *
   * @return The The gap between the dashes of the dashed underline.
   */
  float GetDashedUnderlineGap() const;

  /**
   * @brief Set the outline offset.
   *
   * @param[in] outlineOffset The outline offset.
   */
  void SetOutlineOffset(const Vector2& outlineOffset);

  /**
   * @brief Retrieve the outline offset.
   *
   * @return The outline offset.
   */
  const Vector2& GetOutlineOffset() const;

  /**
   * @brief Set the outline color.
   *
   * @param[in] color color of outline.
   */
  void SetOutlineColor(const Vector4& color);

  /**
   * @brief Retrieve the outline color.
   *
   * @return The outline color.
   */
  const Vector4& GetOutlineColor() const;

  /**
   * @brief Set the outline width
   *
   * @param[in] width The width in pixels of the outline, 0 indicates no outline
   */
  void SetOutlineWidth(uint16_t width);

  /**
   * @brief Retrieves the width of an outline
   *
   * @return The width of the outline.
   */
  uint16_t GetOutlineWidth() const;

  /**
   * @brief Set the outline blur radius.
   *
   * @param[in] outlineBlurRadius The outline blur radius, 0,0 indicates no blur.
   */
  void SetOutlineBlurRadius(const float& outlineBlurRadius);

  /**
   * @brief Retrieve the outline blur radius.
   *
   * @return The outline blur radius.
   */
  const float& GetOutlineBlurRadius() const;

  /**
   * @brief Set the background color.
   *
   * @param[in] color color of background.
   */
  void SetBackgroundColor(const Vector4& color);

  /**
   * @brief Retrieve the background color.
   *
   * @return The background color.
   */
  const Vector4& GetBackgroundColor() const;

  /**
   * @brief Set the background enabled flag.
   *
   * @param[in] enabled The background enabled flag.
   */
  void SetBackgroundEnabled(bool enabled);

  /**
   * @brief Returns whether to enable text background or not.
   *
   * @return Whether text background is enabled.
   */
  bool IsBackgroundEnabled() const;

  /**
   * @brief Sets the emboss's properties string.
   *
   * @note The string is stored to be recovered.
   *
   * @param[in] embossProperties The emboss's properties string.
   */
  void SetDefaultEmbossProperties(const std::string& embossProperties);

  /**
   * @brief Retrieves the emboss's properties string.
   *
   * @return The emboss's properties string.
   */
  std::string GetDefaultEmbossProperties() const;

  /**
   * @brief Sets the outline's properties string.
   *
   * @note The string is stored to be recovered.
   *
   * @param[in] outlineProperties The outline's properties string.
   */
  void SetDefaultOutlineProperties(const std::string& outlineProperties);

  /**
   * @brief Retrieves the outline's properties string.
   *
   * @return The outline's properties string.
   */
  std::string GetDefaultOutlineProperties() const;

  /**
   * @brief Sets the default line spacing.
   *
   * @param[in] lineSpacing The line spacing.
   *
   * @return True if lineSpacing has been updated, false otherwise
   */
  bool SetDefaultLineSpacing(float lineSpacing);

  /**
   * @brief Retrieves the default line spacing.
   *
   * @return The line spacing.
   */
  float GetDefaultLineSpacing() const;

  /**
   * @brief Sets the default line size.
   *
   * @param[in] lineSize The line size.
   *
   * @return True if lineSize has been updated, false otherwise
   */
  bool SetDefaultLineSize(float lineSize);

  /**
   * @brief Retrieves the default line size.
   *
   * @return The line size.
   */
  float GetDefaultLineSize() const;

  /**
   * @brief Sets the relative line size to the original line size.
   *
   * @param[in] relativeLineSize The relativeline size.
   *
   * @return True if relativeLineSize has been updated, false otherwise
   */
  bool SetRelativeLineSize(float lineSize);

  /**
   * @brief Retrieves the relative line size.
   *
   * @return The relative line size.
   */
  float GetRelativeLineSize() const;

  /**
   * @brief Sets the input text's color.
   *
   * @param[in] color The input text's color.
   */
  void SetInputColor(const Vector4& color);

  /**
   * @brief Retrieves the input text's color.
   *
   * @return The input text's color.
   */
  const Vector4& GetInputColor() const;

  /**
   * @brief Sets the input text's font family name.
   *
   * @param[in] fontFamily The text's font family name.
   */
  void SetInputFontFamily(const std::string& fontFamily);

  /**
   * @brief Retrieves the input text's font family name.
   *
   * @return The input text's font family name.
   */
  std::string GetInputFontFamily() const;

  /**
   * @brief Sets the input font's weight.
   *
   * @param[in] weight The input font's weight.
   */
  void SetInputFontWeight(FontWeight weight);

  /**
   * @return Whether the font's weight has been defined.
   */
  bool IsInputFontWeightDefined() const;

  /**
   * @brief Retrieves the input font's weight.
   *
   * @return The input font's weight.
   */
  FontWeight GetInputFontWeight() const;

  /**
   * @brief Sets the input font's width.
   *
   * @param[in] width The input font's width.
   */
  void SetInputFontWidth(FontWidth width);

  /**
   * @return Whether the font's width has been defined.
   */
  bool IsInputFontWidthDefined() const;

  /**
   * @brief Retrieves the input font's width.
   *
   * @return The input font's width.
   */
  FontWidth GetInputFontWidth() const;

  /**
   * @brief Sets the input font's slant.
   *
   * @param[in] slant The input font's slant.
   */
  void SetInputFontSlant(FontSlant slant);

  /**
   * @return Whether the font's slant has been defined.
   */
  bool IsInputFontSlantDefined() const;

  /**
   * @brief Retrieves the input font's slant.
   *
   * @return The input font's slant.
   */
  FontSlant GetInputFontSlant() const;

  /**
   * @brief Sets the input font's point size.
   *
   * @param[in] size The input font's point size.
   * @param[in] defaultFontSizeUpdated True If the default font size is updated and sets the input point size, false otherwise.
   */
  void SetInputFontPointSize(float size, bool defaultFontSizeUpdated = false);

  /**
   * @brief Retrieves the input font's point size.
   *
   * @return The input font's point size.
   */
  float GetInputFontPointSize() const;

  /**
   * @brief Sets the input line spacing.
   *
   * @param[in] lineSpacing The line spacing.
   */
  void SetInputLineSpacing(float lineSpacing);

  /**
   * @brief Retrieves the input line spacing.
   *
   * @return The line spacing.
   */
  float GetInputLineSpacing() const;

  /**
   * @brief Sets the input shadow's properties string.
   *
   * @note The string is stored to be recovered.
   *
   * @param[in] shadowProperties The shadow's properties string.
   */
  void SetInputShadowProperties(const std::string& shadowProperties);

  /**
   * @brief Retrieves the input shadow's properties string.
   *
   * @return The shadow's properties string.
   */
  std::string GetInputShadowProperties() const;

  /**
   * @brief Sets the input underline's properties string.
   *
   * @note The string is stored to be recovered.
   *
   * @param[in] underlineProperties The underline's properties string.
   */
  void SetInputUnderlineProperties(const std::string& underlineProperties);

  /**
   * @brief Retrieves the input underline's properties string.
   *
   * @return The underline's properties string.
   */
  std::string GetInputUnderlineProperties() const;

  /**
   * @brief Sets the input emboss's properties string.
   *
   * @note The string is stored to be recovered.
   *
   * @param[in] embossProperties The emboss's properties string.
   */
  void SetInputEmbossProperties(const std::string& embossProperties);

  /**
   * @brief Retrieves the input emboss's properties string.
   *
   * @return The emboss's properties string.
   */
  std::string GetInputEmbossProperties() const;

  /**
   * @brief Sets input the outline's properties string.
   *
   * @note The string is stored to be recovered.
   *
   * @param[in] outlineProperties The outline's properties string.
   */
  void SetInputOutlineProperties(const std::string& outlineProperties);

  /**
   * @brief Retrieves the input outline's properties string.
   *
   * @return The outline's properties string.
   */
  std::string GetInputOutlineProperties() const;

  /**
   * @brief Sets the input strikethrough's properties string.
   *
   * @note The string is stored to be recovered.
   *
   * @param[in] strikethroughProperties The strikethrough's properties string.
   */
  void SetInputStrikethroughProperties(const std::string& strikethroughProperties);

  /**
   * @brief Retrieves the input strikethrough's properties string.
   *
   * @return The strikethrough's properties string.
   */
  std::string GetInputStrikethroughProperties() const;

  /**
   * @brief Set the control's interface.
   *
   * @param[in] controlInterface The control's interface.
   */
  void SetControlInterface(ControlInterface* controlInterface);

  /**
   * @brief Set the anchor control's interface.
   *
   * @param[in] anchorControlInterface The control's interface.
   */
  void SetAnchorControlInterface(AnchorControlInterface* anchorControlInterface);

  /**
   * @brief Sets the character spacing.
   *
   * @note A positive value will make the characters far apart (expanded) and a negative value will bring them closer (condensed).
   *
   * @param[in] characterSpacing The character spacing.
   */
  void SetCharacterSpacing(float characterSpacing);

  /**
   * @brief Retrieves the character spacing.
   *
   * @note A positive value will make the characters far apart (expanded) and a negative value will bring them closer (condensed).
   *
   * @return The character spacing.
   */
  const float GetCharacterSpacing() const;

  /**
   * @brief Sets the visual transform offset.
   *
   * @param[in] offset The offset.
   */
  void SetVisualTransformOffset(Vector2 offset);

  /**
   * @brief Sets whether background color with cutout is enabled.
   *
   * @param[in] enable True if enabled.
   */
  void SetBackgroundWithCutoutEnabled(bool enable);

  /**
   * @brief Whether background color with cutout is enabled.
   *
   * @return True if enabled.
   */
  bool IsBackgroundWithCutoutEnabled() const;

  /**
   * @brief Sets whether background color with cutout.
   *
   * @param[in] color The color to set.
   */
  void SetBackgroundColorWithCutout(const Vector4& color);

  /**
   * @brief Retrieves background color with cutout.
   *
   * @return The color.
   */
  const Vector4 GetBackgroundColorWithCutout() const;

  /**
   * @brief Sets offset with cutout.
   *
   * @param[in] offset The offset.
   */
  void SetOffsetWithCutout(const Vector2& offset);

public: // Queries & retrieves.
  /**
   * @brief Return the layout engine.
   *
   * @return A reference to the layout engine.
   */
  Layout::Engine& GetLayoutEngine();

  /**
   * @brief Return a view of the text.
   *
   * @return A reference to the view.
   */
  View& GetView();

  /**
   * @copydoc Control::GetNaturalSize()
   */
  Vector3 GetNaturalSize();

  /**
   * @copydoc Control::GetHeightForWidth()
   */
  float GetHeightForWidth(float width);

  /**
   * @brief Calculates the point size for text for given layout()
   */
  void FitPointSizeforLayout(Size layoutSize);

  /**
   * @brief Calculates the point size for text for given layout() using fit array.
   */
  void FitArrayPointSizeforLayout(Size layoutSize);

  /**
   * @brief Checks if the point size fits within the layout size.
   *
   * @return Whether the point size fits within the layout size.
   */
  bool CheckForTextFit(float pointSize, Size& layoutSize);

  /**
   * @brief Retrieves the text's number of lines for a given width.
   * @param[in] width The width of the text's area.
   * @ return The number of lines.
   */
  int GetLineCount(float width);

  /**
   * @brief Retrieves the text's model.
   *
   * @return A pointer to the text's model.
   */
  const ModelInterface* GetTextModel() const;

  /**
   * @brief Used to get scrolled distance by user input
   *
   * @return Distance from last scroll offset to new scroll offset
   */
  float GetScrollAmountByUserInput();

  /**
   * @brief Get latest scroll amount, control size and layout size
   *
   * This method is used to get information of control's scroll
   * @param[out] scrollPosition The current scrolled position
   * @param[out] controlHeight The size of a UI control
   * @param[out] layoutHeight The size of a bounding box to layout text within.
   *
   * @return Whether the text scroll position is changed or not after last update.
   */
  bool GetTextScrollInfo(float& scrollPosition, float& controlHeight, float& layoutHeight);

  /**
   * @brief Used to set the hidden input option
   */
  void SetHiddenInputOption(const Property::Map& options);

  /**
   * @brief Used to get the hidden input option
   */
  void GetHiddenInputOption(Property::Map& options);

  /**
   * @brief Used to set the input filter option
   */
  void SetInputFilterOption(const Property::Map& options);

  /**
   * @brief Used to get the input filter option
   */
  void GetInputFilterOption(Property::Map& options);

  /**
   * @brief Sets the Placeholder Properties.
   *
   * @param[in] map The placeholder property map
   */
  void SetPlaceholderProperty(const Property::Map& map);

  /**
   * @brief Retrieves the Placeholder Property map.
   *
   * @param[out] map The property map
   */
  void GetPlaceholderProperty(Property::Map& map);

  /**
   * @brief Checks text direction.
   * @return The text direction.
   */
  Toolkit::DevelText::TextDirection::Type GetTextDirection();

  /**
   * @brief Retrieves vertical line alignment
   * @return The vertical line alignment
   */
  Toolkit::DevelText::VerticalLineAlignment::Type GetVerticalLineAlignment() const;

  /**
   * @brief Sets vertical line alignment
   * @param[in] alignment The vertical line alignment for the text
   */
  void SetVerticalLineAlignment(Toolkit::DevelText::VerticalLineAlignment::Type alignment);

  /**
   * @brief Retrieves ellipsis position
   * @return The ellipsis position
   */
  Toolkit::DevelText::EllipsisPosition::Type GetEllipsisPosition() const;

  /**
   * @brief Sets ellipsis position
   * @param[in] ellipsisPosition The ellipsis position for the text
   */
  void SetEllipsisPosition(Toolkit::DevelText::EllipsisPosition::Type ellipsisPosition);

  /**
   * @brief Retrieves ellipsis mode
   * @return The ellipsis mode
   */
  Toolkit::DevelText::Ellipsize::Mode GetEllipsisMode() const;

  /**
   * @brief Sets ellipsis mode
   * @param[in] ellipsisMode The ellipsis mode for the text
   */
  void SetEllipsisMode(Toolkit::DevelText::Ellipsize::Mode ellipsisMode);

  /**
   * @brief Retrieves ignoreSpaceAfterText value from model
   * @return The value of ignoreSpaceAfterText
   */
  bool IsIgnoreSpacesAfterText() const;

  /**
   * @brief Sets ignoreSpaceAfterText value to model
   * @param[in] ignore The value of ignoreSpacesAfterText for the text
   */
  void SetIgnoreSpacesAfterText(bool ignore);

  /**
   * @brief Retrieves removeFrontInset value from model
   * @return The value of removeFrontInset
   */
  bool IsRemoveFrontInset() const;

  /**
   * @brief Sets removeFrontInset value to model
   * @param[in] remove The value of removeFrontInset for the text
   */
  void SetRemoveFrontInset(bool remove);

  /**
   * @brief Retrieves removeBackInset value from model
   * @return The value of removeBackInset
   */
  bool IsRemoveBackInset() const;

  /**
   * @brief Sets removeBackInset value to model
   * @param[in] remove The value of removeBackInset for the text
   */
  void SetRemoveBackInset(bool remove);

  /**
   * @brief Retrieves cutout value to model
   * @return The value of cutout for the text
   */
  bool IsTextCutout() const;

  /**
   * @brief Sets cutout value to model
   * @param[in] cutout The value of cutout for the text
   */
  void SetTextCutout(bool cutout);

  /**
   * @brief Retrieves variation value to model
   * @param[out] map The value of cutout for the text
   */
  void GetVariationsMap(Property::Map& map);

  /**
   * @brief Sets variation value to model
   * @param[in] map The value of cutout for the text
   */
  void SetVariationsMap(const Property::Map& map);

  /**
   * @brief Sets SetMatchLayoutDirection value to model
   * @param[in] match The value of matchLayoutDirection for the text
   */
  void SetMatchLayoutDirection(DevelText::MatchLayoutDirection type);

  /**
   * @brief Retrieves matchLayoutDirection value from model
   * @return The value of matchLayoutDirection
   */
  DevelText::MatchLayoutDirection GetMatchLayoutDirection() const;

  /**
   * @brief Sets layoutDirection type value.
   * @param[in] layoutDirection The value of the layout direction type.
   */
  void SetLayoutDirection(Dali::LayoutDirection::Type layoutDirection);

  /**
   * @brief Gets layoutDirection type value.
   * @param[in] actor The actor which will get the layout direction type.
   * @return The value of the layout direction type.
   */
  Dali::LayoutDirection::Type GetLayoutDirection(Dali::Actor& actor) const;

  /**
   * @brief Get the rendered size of a specific text range.
   * if the requested text is at multilines, multiple sizes will be returned for each text located in a separate line.
   * if a line contains characters with different directions, multiple sizes will be returned for each block of contiguous characters with the same direction.
   *
   * @param[in] startIndex start index of the text requested to calculate size for.
   * @param[in] endIndex end index(included) of the text requested to calculate size for.
   * @return list of sizes of the reuested text.
   */
  Vector<Vector2> GetTextSize(CharacterIndex startIndex, CharacterIndex endIndex);

  /**
   * @brief Get the top/left rendered position of a specific text range.
   * if the requested text is at multilines, multiple positions will be returned for each text located in a separate line.
   * if a line contains characters with different directions, multiple positions will be returned for each block of contiguous characters with the same direction.
   *
   * @param[in] startIndex start index of the text requested to get position to.
   * @param[in] endIndex end index(included) of the text requested to get position to.
   * @return list of positions of the requested text.
   */
  Vector<Vector2> GetTextPosition(CharacterIndex startIndex, CharacterIndex endIndex);

  /**
   * @brief Get the line bounding rectangle.
   * if the requested index is out of range or the line is not yet rendered, a rect of {0, 0, 0, 0} is returned.
   *
   * @param[in] lineIndex line index to which we want to calculate the geometry for.
   * @return bounding rectangle.
   */
  Rect<float> GetLineBoundingRectangle(const uint32_t lineIndex);

  /**
   * @brief Get the char bounding rectangle.
   * If the text is not yet rendered or the index > text.Count(); a rect of {0, 0, 0, 0} is returned.
   *
   * @param[in] charIndex character index to which we want to calculate the geometry for.
   * @return bounding rectangle.
   */
  Rect<float> GetCharacterBoundingRectangle(const uint32_t charIndex);

  /**
   * @brief Get the character index.
   * If the text is not yet rendered or the text is empty, -1 is returned.
   *
   * @param[in] visualX visual x position.
   * @param[in] visualY visual y position.
   * @return character index.
   */
  int GetCharacterIndexAtPosition(float visualX, float visualY);

  /**
   * @brief Gets the bounding box of a specific text range.
   *
   * @param[in] startIndex start index of the text requested to get bounding box to.
   * @param[in] endIndex end index(included) of the text requested to get bounding box to.
   * @return bounding box of the requested text.
   */
  Rect<> GetTextBoundingRectangle(CharacterIndex startIndex, CharacterIndex endIndex);

  /**
   * @brief Sets the layout direction changed.
   */
  void ChangedLayoutDirection();

  /**
   * @brief Clear font and style data.
   */
  void ResetFontAndStyleData();

  /**
   * @brief Retrieves if showing real text or not.
   * @return The value of showing real text.
   */
  bool IsShowingRealText() const;

  /**
   * @brief Sets the render mode of the text.
   *
   * By default is DevelTextLabel::Render::Sync.
   *
   * @param[in] renderMode Render mode of the text.
   */
  void SetRenderMode(DevelTextLabel::Render::Mode renderMode);

  /**
   * @brief Gets the render mode of the text.
   * @return The value of render mode of the text.
   */
  DevelTextLabel::Render::Mode GetRenderMode();


public: // Relayout.
  /**
   * @brief Triggers a relayout which updates View (if necessary).
   *
   * @note UI Controls are expected to minimize calls to this method e.g. call once after size negotiation.
   * @param[in] size A the size of a bounding box to layout text within.
   * @param[in] layoutDirection The direction of the system language.
   *
   * @return Whether the text model or decorations were updated.
   */
  UpdateTextType Relayout(const Size& size, Dali::LayoutDirection::Type layoutDirection = Dali::LayoutDirection::LEFT_TO_RIGHT);

  /**
   * @brief Request a relayout using the ControlInterface.
   */
  void RequestRelayout();

public: // Input style change signals.
  /**
   * @return Whether the queue of input style changed signals is empty.
   */
  bool IsInputStyleChangedSignalsQueueEmpty();

  /**
   * @brief Request process all pending input style changed signals.
   *
   * Request to calls the Text::ControlInterface::InputStyleChanged() method which is overriden by the
   * text controls. Text controls may send signals to state the input style has changed.
   *
   * The signal will be execute next idle time, or skip if we fail to add idler.
   */
  void RequestProcessInputStyleChangedSignals();

private:
  /**
   * @brief Callbacks called on idle.
   *
   * If there are notifications of change of input style on the queue, Toolkit::TextField::InputStyleChangedSignal() are emitted.
   */
  void OnIdleSignal();

public: // Text-input Event Queuing.
  /**
   * @brief Called by editable UI controls when keyboard focus is gained.
   */
  void KeyboardFocusGainEvent();

  /**
   * @brief Called by editable UI controls when focus is lost.
   */
  void KeyboardFocusLostEvent();

  /**
   * @brief Called by editable UI controls when key events are received.
   *
   * @param[in] event The key event.
   * @param[in] type Used to distinguish between regular key events and InputMethodContext events.
   */
  bool KeyEvent(const Dali::KeyEvent& event);

  /**
   * @brief Called by anchor when a tap gesture occurs.
   * @param[in] x The x position relative to the top-left of the parent control.
   * @param[in] y The y position relative to the top-left of the parent control.
   */
  void AnchorEvent(float x, float y);

  /**
   * @brief Called by editable UI controls when a tap gesture occurs.
   * @param[in] tapCount The number of taps.
   * @param[in] x The x position relative to the top-left of the parent control.
   * @param[in] y The y position relative to the top-left of the parent control.
   */
  void TapEvent(unsigned int tapCount, float x, float y);

  /**
   * @brief Called by editable UI controls when a pan gesture occurs.
   *
   * @param[in] state The state of the gesture.
   * @param[in] displacement This distance panned since the last pan gesture.
   */
  void PanEvent(GestureState state, const Vector2& displacement);

  /**
   * @brief Called by editable UI controls when a long press gesture occurs.
   *
   * @param[in] state The state of the gesture.
   * @param[in] x The x position relative to the top-left of the parent control.
   * @param[in] y The y position relative to the top-left of the parent control.
   */
  void LongPressEvent(GestureState state, float x, float y);

  /**
   * @brief Used to get the Primary cursor position.
   *
   * @return Primary cursor position.
   */
  CharacterIndex GetPrimaryCursorPosition() const;

  /**
   * @brief Used to set the Primary cursor position.
   *
   * @param[in] index for the Primary cursor position.
   * @param[in] focused true if UI control has gained focus to receive key event, false otherwise.
   * @return[in] true if cursor position changed, false otherwise.
   */
  bool SetPrimaryCursorPosition(CharacterIndex index, bool focused);

  /**
   * @brief Creates a selection event.
   *
   * It could be called from the TapEvent (double tap) or when the text selection popup's sellect all button is pressed.
   *
   * @param[in] x The x position relative to the top-left of the parent control.
   * @param[in] y The y position relative to the top-left of the parent control.
   * @param[in] selection type like the whole text is selected or unselected.
   */
  void SelectEvent(float x, float y, SelectionType selection);

  /**
   * @copydoc Text::SelectableControlInterface::SetTextSelectionRange()
   */
  void SetTextSelectionRange(const uint32_t* start, const uint32_t* end);

  /**
   * @copydoc Text::SelectableControlInterface::GetTextSelectionRange()
   */
  Uint32Pair GetTextSelectionRange() const;

  /**
   * @copydoc Text::SelectableControlInterface::SelectWholeText()
   */
  void SelectWholeText();

  /**
   * @copydoc Text::EditableControlInterface::CopyText()
   */
  string CopyText();

  /**
   * @copydoc Text::EditableControlInterface::CutText()
   */
  string CutText();

  /**
   * @copydoc Text::EditableControlInterface::PasteText()
   */
  void PasteText();

  /**
   * @copydoc Text::SelectableControlInterface::SelectNone()
   */
  void SelectNone();

  /**
   * @copydoc Text::SelectableControlInterface::SelectText()
   */
  void SelectText(const uint32_t start, const uint32_t end);

  /**
   * @copydoc Text::SelectableControlInterface::GetSelectedText()
   */
  string GetSelectedText() const;

  /**
   * @copydoc Text::EditableControlInterface::IsEditable()
   */
  virtual bool IsEditable() const;

  /**
   * @copydoc Text::EditableControlInterface::SetEditable()
   */
  virtual void SetEditable(bool editable);

  /**
   * @copydoc Dali::Toolkit::Internal::TextEditor::ScrollBy()
   */
  virtual void ScrollBy(Vector2 scroll);

  /**
   * @brief Whether the text is scrollable.
   * @return Returns true if the text is scrollable.
   */
  bool IsScrollable(const Vector2& displacement);

  /**
   * @copydoc Dali::Toolkit::Internal::TextEditor::GetHorizontalScrollPosition()
   */
  float GetHorizontalScrollPosition();

  /**
   * @copydoc Dali::Toolkit::Internal::TextEditor::GetVerticalScrollPosition()
   */
  float GetVerticalScrollPosition();

  /**
   * @brief Event received from input method context
   *
   * @param[in] inputMethodContext The input method context.
   * @param[in] inputMethodContextEvent The event received.
   * @return A data struture indicating if update is needed, cursor position and current text.
   */
  InputMethodContext::CallbackData OnInputMethodContextEvent(InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent);

  /**
   * @brief Event from Clipboard notifying an Item has been selected for pasting
   *
   * @param[in] id The id of the data request.
   * @param[in] mimeType The mime type of data received.
   * @param[in] data The data received.
   * @note
   * This event is executed by receiving the Clipboard's DataReceivedSignal.
   */
  void PasteClipboardItemEvent(uint32_t id, const char* mimeType, const char* data);

  /**
   * @brief Return true when text control should clear key input focus when escape key is pressed.
   *
   * @return Whether text control should clear key input focus or not when escape key is pressed.
   */
  bool ShouldClearFocusOnEscape() const;

  /**
   * @brief Create an actor that renders the text background color
   *
   * @return the created actor or an empty handle if no background color needs to be rendered.
   */
  Actor CreateBackgroundActor();

  /**
   * @brief Used to reset the cursor position after setting a new text.
   *
   * @param[in] cursorIndex Where to place the cursor.
   */
  void ResetCursorPosition(CharacterIndex cursorIndex);

  /**
   * @brief The method acquires current position of cursor
   * @return unsigned value with cursor position
   */
  CharacterIndex GetCursorPosition();

  /**
   * @brief Resets a provided vector with actors that marks the position of anchors in markup enabled text
   *
   * @param[out] anchorActors the vector of actor (empty collection if no anchors available).
   */
  void GetAnchorActors(std::vector<Toolkit::TextAnchor>& anchorActors);

  /**
   * @brief Return an index of first anchor in the anchor vector whose boundaries includes given character offset
   *
   * @param[in] characterOffset A position in text coords.
   *
   * @return the index in anchor vector (-1 if an anchor not found)
   */
  int GetAnchorIndex(size_t characterOffset);

protected: // Inherit from Text::Decorator::ControllerInterface.
  /**
   * @copydoc Dali::Toolkit::Text::Decorator::ControllerInterface::GetTargetSize()
   */
  void GetTargetSize(Vector2& targetSize) override;

  /**
   * @copydoc Dali::Toolkit::Text::Decorator::ControllerInterface::AddDecoration()
   */
  void AddDecoration(Actor& actor, DecorationType type, bool needsClipping) override;

  /**
   * @copydoc Dali::Toolkit::Text::Decorator::ControllerInterface::DecorationEvent()
   */
  void DecorationEvent(HandleType handle, HandleState state, float x, float y) override;

protected: // Inherit from TextSelectionPopup::TextPopupButtonCallbackInterface.
  /**
   * @copydoc Dali::Toolkit::TextSelectionPopup::TextPopupButtonCallbackInterface::TextPopupButtonTouched()
   */
  void TextPopupButtonTouched(Dali::Toolkit::TextSelectionPopup::Buttons button) override;

protected: // Inherit from HiddenText.
  /**
   * @brief Invoked from HiddenText when showing time of the last character was expired
   */
  void DisplayTimeExpired() override;

protected: // Inherit from Integration::Processor
  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  void Process(bool postProcess) override;

  /**
   * @copydoc Dali::Integration::Processor::GetProcessorName()
   */
  std::string_view GetProcessorName() const override
  {
    return "Text::Controller";
  }

private: // Private contructors & copy operator.
  /**
   * @brief Private constructor.
   */
  Controller()
  : Controller(nullptr, nullptr, nullptr, nullptr)
  {
  }

  /**
   * @brief Private constructor.
   */
  Controller(ControlInterface* controlInterface)
  : Controller(controlInterface, nullptr, nullptr, nullptr)
  {
  }

  /**
   * @brief Private constructor.
   */
  Controller(ControlInterface*           controlInterface,
             EditableControlInterface*   editableControlInterface,
             SelectableControlInterface* selectableControlInterface,
             AnchorControlInterface*     anchorControlInterface);

  Controller(const Controller& handle) = delete;
  Controller& operator=(const Controller& handle) = delete;

protected: // Destructor.
  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~Controller();

public:
  struct Impl; ///< Made public for testing purposes

private:
  struct EventHandler;
  struct InputFontHandler;
  struct InputProperties;
  struct PlaceholderHandler;
  struct Relayouter;
  struct TextUpdater;
  struct SpannableHandler;

  std::unique_ptr<Impl> mImpl{nullptr};
};

} // namespace Dali::Toolkit::Text

#endif // DALI_TOOLKIT_TEXT_CONTROLLER_H
