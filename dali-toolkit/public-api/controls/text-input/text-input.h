#ifndef __DALI_TOOLKIT_TEXT_INPUT_H__
#define __DALI_TOOLKIT_TEXT_INPUT_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/text-view/text-view.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class TextInput;
}

/**
 * @brief TextInput Actor takes input one character at a time and displays it as a string within an input box.
 * Characters can be removed from the end of the string until it is empty. A maximum length of displayed string
 * can be set.
 */
class DALI_IMPORT_API TextInput : public Control
{

public:

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 512              ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the TextInput class.
   */
  struct Property
  {
    enum
    {
      HIGHLIGHT_COLOR = PROPERTY_START_INDEX, // Property, name "highlight-color",                     type Vector4
      CUT_AND_PASTE_COLOR,                    // Property, name "cut-and-paste-bg-color",              type Vector4
      CUT_AND_PASTE_PRESSED_COLOR,            // Property, name "cut-and-paste-pressed-color",         type Vector4
      CUT_AND_PASTE_BORDER_COLOR,             // Property, name "cut-and-paste-border-color",          type Vector4
      CUT_AND_PASTE_ICON_COLOR,               // Property, name "cut-and-paste-icon-color",            type Vector4
      CUT_AND_PASTE_ICON_PRESSED_COLOR,       // Property, name "cut-and-paste-icon-pressed-color",    type Vector4
      CUT_AND_PASTE_TEXT_COLOR,               // Property, name "cut-and-paste-text-color",            type Vector4
      CUT_AND_PASTE_TEXT_PRESSED_COLOR,       // Property, name "cut-and-paste-text-pressed-color",    type Vector4
      CUT_BUTTON_POSITION_PRIORITY,           // Property, name "cut-button-position-priority",        type unsigned int
      COPY_BUTTON_POSITION_PRIORITY,          // Property, name "copy-button-position-priority",       type unsigned int
      PASTE_BUTTON_POSITION_PRIORITY,         // Property, name "paste-button-position-priority",      type unsigned int
      SELECT_BUTTON_POSITION_PRIORITY,        // Property, name "select-button-position-priority",     type unsigned int
      SELECT_ALL_BUTTON_POSITION_PRIORITY,    // Property, name "select-all-button-position-priority", type unsigned int
      CLIPBOARD_BUTTON_POSITION_PRIORITY,     // Property, name "clipboard-button-position-priority",  type unsigned int
      POP_UP_OFFSET_FROM_TEXT,                // Property, name "popup-offset-from-text",              type Vector4
      CURSOR_COLOR,                           // Property, name "cursor-color",                        type Vector4
    };
  };

public:

  /**
   * @brief Create an uninitialized TextInput; this can be initialized with TextView::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  TextInput();

  /**
   * @brief Copy constructor.
   *
   * @param handle to be copied
   */
  TextInput( const TextInput& handle );

  /**
   * @brief Assignment operator.
   *
   * @param handle to object we want to point to
   * @return handle to the TextInput
   */
  TextInput& operator=( const TextInput& handle );

  /**
   * @brief Create an initialised TextInput.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static TextInput New();

  /**
   * @brief Downcast an Object handle to TextInput.
   *
   * If handle points to a TextInput the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return handle to a TextInput or an uninitialized handle
   */
  static TextInput DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~TextInput();

  /**
   * @brief Get the inputed text currently being displayed.
   *
   * @return string, the currently displayed string.
   */
  std::string GetText() const;

  /**
   * @brief Get the inputed text currently being displayed together with mark-up tags.
   *
   * @return string, the currently displayed string with mark-up.
   */
  std::string GetMarkupText() const;

  /**
   * @brief Set the maximum number of characters for the Text Input.
   *
   * @param [in] maxChars the max number of characters
   */
  void SetMaxCharacterLength(std::size_t maxChars);

  /**
   * @brief Limits the number of lines of text Text Input will display.
   *
   * @param [in] maxLines the max number of lines to display, must be greater than 0.
   * Currently the only valid limit is 1. Which turns TextInput into Single line mode. Any number higher than 1 results in no limit.
   */
  void SetNumberOfLinesLimit(std::size_t maxLines);

  /**
   * @brief Returns the limit of lines Text Input is allowed to display.
   *
   * @return max line number limit
   */
  std::size_t GetNumberOfLinesLimit() const;

  /**
   * @brief Returns the number of characters TextInput is displaying.
   *
   * @return number of characters
   */
  std::size_t GetNumberOfCharacters() const;

  /**
   *  @brief Sets a place holder text to be displayed when the text-input is empty.
   *
   *  If not set or set to an empty string then no place holder will be shown.
   *  @param [in] placeHolderText text to be used as place holder.
   */
  void SetPlaceholderText( const std::string& placeHolderText );

  /**
   * @return the current set place holder text, empty string returned if not set.
   */
  std::string GetPlaceholderText();

  /**
   *  @brief set initial text to be displayed in text-input.
   *
   *  Can be used to edit a pre-existing string.
   *  @param [in] initialText text to be initially displayed
   */
  void SetInitialText(const std::string& initialText);

  /**
   *  @brief Manual method to set the focus on the TextInput so it starts or stops edit state.
   *
   *  @pre The text input actor has been initialised.
   *  @param[in] editMode true or false to indicate editMode on or off
   */
   void SetEditable(bool editMode);

   /**
    * @see SetEditable(bool editMode).
    *
    * It sets the cursor in the closest character to the given touch point.
    *
   *  @param[in] editMode true or false to indicate editMode on or off
    * @param[in] touchPoint A position in actor coordinates within the text-input.
    */
   void SetEditable(bool editMode, const Vector2& touchPoint);

   /**
    *  @brief Check if TextInput is in edit state.
    *
    *  @pre The text input actor has been initialised.
    *  @return  True or False to indicate editMode on or off
    */
   bool IsEditable() const;

   /**
    * @brief Method to enable or disable edit on touch/tap.
    *
    * If not enabled (set to false) then SetEditable(true) will be used to start edit mode.
    * @pre The text input actor has been initialised.
    * @param[in] editOnTouch true or false to indicate if editing should start on touch
    *            default is for editing to start on touching textinput
    */
   void SetEditOnTouch(bool editOnTouch = true);

   /**
    *  @brief Check if TextInput starts edit mode on touch.
    *
    *  @pre The text input actor has been initialised.
    *  @return  True or False to indicate editOnTouch on or off
    */
   bool IsEditOnTouch() const;

   /**
    *  @brief Check if Text Selection is enabled so required text can be highlighted.
    *
    *  @pre The text input actor has been initialised.
    *  @param[in] textSelectable true or false to indicate if text can be selected or not
    *             default is for text to be select-able when in edit mode
    */
   void SetTextSelectable(bool textSelectable = true);

   /**
    *  @brief Check if Text can be selected.
    *
    *  @pre The text input actor has been initialised.
    *  @return  True or False to indicate if text can be selected or not
    */
   bool IsTextSelectable() const;

   /**
    * @brief Check if any text is currently selected, can be used to determine if ApplyStyle or SetActiveStyle should be used.
    *
    * @pre The text input actor has been initialised.
    * @return True if text selected else False
    */
   bool IsTextSelected() const;

   /**
    * @brief Selects text between the given positions.
    *
    * @pre TextInput should be in edit mode.
    * @param start position to start selection
    * @param end position to end selection, inclusive of this character.
    * Providing 0 and result from GetNumberOfCharacters() will select all text.
    */
   void SelectText(std::size_t start, std::size_t end);

   /**
    * @brief If any text is selected then de-select it and hide highlight.
    *
    * @pre The text input actor has been initialised.
    */
   void DeSelectText();

   /**
    * @brief Set the image to be used as the cursor grab hander.
    *
    * @pre The text input actor has been initialised.
    * @param[in] image The image to be used.
    */
   void SetGrabHandleImage( Image image );

   /**
    * Depreciated API.
    * @brief Set the image to be used for the regular left to right cursor.
    *
    * @pre The text input actor has been initialised.
    * @param[in] image The image to be used.
    * @param[in] border The nine patch border for the image.
    */
   void SetCursorImage(Dali::Image image, const Vector4& border );

   /**
    * @brief Retrieve the selection handle size.
    *
    * Both handles have same size.
    * @return Vector3 the selection handle size.
    */
   Vector3 GetSelectionHandleSize();

   /**
    * @brief Set the image to be used for the Right to Left cursor.
    *
    * @pre The text input actor has been initialised.
    * @param[in] image The image to be used.
    * @param[in] border The nine patch border for the image.
    */
    void SetRTLCursorImage(Dali::Image image, const Vector4& border );

   /**
    * @brief Toggle to enable the grab handle, used to position cursor when magnifier not being used.
    *
    * Default behaviour is to use the magnifier to position the cursor, enabling this prevents the magnifier from being shown.
    * @param[in] toggle true to enable, false to disable grab handle
    */
   void EnableGrabHandle(bool toggle);

   /**
    * @brief Method to check if grab handle is enabled, if false then the magnifier will be used to position cursor.
    *
    * @return bool returns true is grab handle enabled.
    */
   bool IsGrabHandleEnabled();

  /**
   * @brief Set the bounding rectangle which handles, popup and similar decorations will not exceed.
   *
   * The default value is the width and height of the stage from the top left origin.
   * If a title bar for example is on the top of the screen then the y should be the title's height and
   * the boundary height the stage height minus the title's height.
   * Restrictions - The boundary box should be set up with a fixed z position for the text-input and the default camera.
   * @param[in] boundingOriginAndSize Rect( x coordinate, y coordinate, width, height )
   *                                  @code
   *                                  +----------------------------------------+
   *                                  |(x,y)                                   |
   *                                  |+--------------------------------------+|
   *                                  ||                                      ||
   *                                  ||            Bounding Box              || boundary height
   *                                  ||                                      ||
   *                                  |+--------------------------------------+|
   *                                  +----------------------------------------+
   *                                                boundary width
   *                                  @endcode
   */
  void SetBoundingRectangle( const Rect<float>& boundingOriginAndSize );

  /**
   * @brief Retrieve the bounding box origin and dimensions.
   *
   * default is set once control is added to stage, before this the return vector will be Vector4:ZERO
   * @return Rect the bounding rectangle
   */
  const Rect<float> GetBoundingRectangle() const;

   /**
    * @brief Sets the style for new text being typed.
    *
    * By default all style settings are applied but a bit mask could be used to modify only certain style settings.
    * @pre The text input actor has been initialised.
    * @param[in] style The style for the new text.
    * @param[in] mask The bit mask.
    */
   void SetActiveStyle( const TextStyle& style, const TextStyle::Mask mask = TextStyle::ALL );

   /**
    * @brief Applies the given style to the selected text.
    *
    * By default all style settings are applied but a bit mask could be used to modify only certain style settings.
    * Introduced text after this call uses the new style.
    * @param[in] style The given style.
    * @param[in] mask The bit mask.
    */
   void ApplyStyle( const TextStyle& style, const TextStyle::Mask mask = TextStyle::ALL );

   /**
     * @brief Applies the given style to all text, selected or not selected.
     *
     * By default all style settings are applied but a bit mask could be used to modify only certain style settings.
     * @param[in] style The given style.
     * @param[in] mask The bit mask.
     */
   void ApplyStyleToAll( const TextStyle& style, const TextStyle::Mask mask = TextStyle::ALL );

   /**
    * @brief Get the style of the Text character before the cursor.
    *
    * If no character before then return the InputStyle.
    * @return TextStyle, the style of the character before the cursor
    */
   TextStyle GetStyleAtCursor() const;

  /**
   * @brief Set the current text alignment (overrides default setting).
   *
   * The default alignment is dependent on the current text in the text field.
   * If the text begins using LTR characters (e.g. European text) then the
   * alignment is HorizontalLeft. If the text begins using RTL characters
   * (e.g. Hebrew/Arabic text) then the alignment is HorizontalRight.
   * If there is no text, then the alignment defaults to:
   * (HorizontalLeft | VerticalCenter)
   * @param[in] align The new alignment option.
   */
  void SetTextAlignment( Toolkit::Alignment::Type align );

  /**
   * @brief Set the current line justification. (overrides default setting).
   *
   * The default justification is dependent on the current text in the text field.
   * If the text begins using LTR characters (e.g. European text) then the
   * justification is HorizontalLeft. If the text begins using RTL characters
   * (e.g. Hebrew/Arabic text) then the justification is HorizontalRight.
   * If there is no text, then the justification defaults to:
   * (HorizontalLeft | VerticalCenter)
   * @param[in] justification The new line justification.
   */
  void SetTextLineJustification( Toolkit::TextView::LineJustification justification );

  /**
   * @brief Sets a fade boundary.
   *
   * @see TextView::FadeBoundary.
   *
   * @param[in] fadeBoundary The given fade boundary.
   */
  void SetFadeBoundary( const Toolkit::TextView::FadeBoundary& fadeBoundary );

  /**
   * @brief Retrieves the fade boundary.
   *
   * @see TextView::FadeBoundary.
   *
   * @return The fade boundary.
   */
  const Toolkit::TextView::FadeBoundary& GetFadeBoundary() const;

  /**
   * @brief Get the current text alignment combined into a single value.
   *
   * The values can be tested by using the & operator
   * and the desired flag. e.g. if (GetTextAlignment() & HorizontalCentre) ...
   * @return The combined text alignment
   */
  Toolkit::Alignment::Type GetTextAlignment() const;

  /**
   * @brief Sets how to split the text in lines policy.
   *
   * @param policy The multi-line policy.
   */
   void SetMultilinePolicy( Toolkit::TextView::MultilinePolicy policy );

  /**
   * @brief Gets the split in lines policy.
   *
   * @return The multi-line policy.
   */
  Toolkit::TextView::MultilinePolicy GetMultilinePolicy() const;

  /**
   * @brief Sets how to display the text inside the TextView when it exceeds the text-view's width.
   *
   * @param policy The exceed policy.
   */
  void SetWidthExceedPolicy( Toolkit::TextView::ExceedPolicy policy );

  /**
   * @brief Gets the width exceed policy.
   *
   * @return The exceed policy.
   */
  TextView::ExceedPolicy GetWidthExceedPolicy() const;

  /**
   * @brief Sets how to display the text inside the TextView when it exceeds the text-view's height.
   *
   * @param policy The exceed policy.
   */
  void SetHeightExceedPolicy( Toolkit::TextView::ExceedPolicy policy );

  /**
   * @brief Gets the height exceed policy.
   *
   * @return The exceed policy.
   */
  TextView::ExceedPolicy GetHeightExceedPolicy() const;

  /**
   * @brief Sets if the inputed text can exceed the text-input boundary.
   *
   * By default is enabled.
   *
   * @param[in] enable Whether the inputed text can exceed its boundary.
   */
  void SetExceedEnabled( bool enable );

  /**
   * @brief Retrieves whether inputed text can exceed the text-input boundary.
   *
   * @return \e true if text inputed can exceed the boundary, otherwise \e false.
   */
  bool GetExceedEnabled() const;

  /**
   * @brief Allows modification of text-actor's position in the depth sort algorithm.
   *
   * @see Dali::RenderableActor::SetSortModifier()
   * @param [in] depthOffset the offset to be given to the internal text-actors. Positive values pushing it further back.
   */
  void SetSortModifier( float depthOffset );

  /**
   * @brief Sets whether text-view renders text using a previously generated snapshot.
   *
   * @see TextView::SetSnapshotModeEnabled()
   *
   * @param[in] enable Whether text-view is using or not a snapshot to render text.
   */
  void SetSnapshotModeEnabled( bool enable );

  /**
   * @brief Retrieves whether text-view is using a snapshot to render text.
   *
   * @see TextView::IsSnapshotModeEnabled()
   *
   * @return \e true if text-view is using a snapshot to render text, otherwhise it returns \e false.
   */
  bool IsSnapshotModeEnabled() const;

  /**
   * @copydoc TextView::SetScrollEnabled()
   */
  void SetScrollEnabled( bool enable );

  /**
   * @copydoc TextView::IsScrollEnabled()
   */
  bool IsScrollEnabled() const;

  /**
   * @copydoc TextView::SetScrollPosition()
   */
  void SetScrollPosition( const Vector2& position );

  /**
   * @copydoc TextView::GetScrollPosition()
   */
  Vector2 GetScrollPosition() const;

  /**
   * @brief Sets whether markup processing should be carried out.
   *
   * @param[in] enable whether markup processing is carried out or not.
   */
  void SetMarkupProcessingEnabled( bool enable );

  /**
   * @brief Returns whether markup processing is enabled or not
   *
   * @return true is markup processing is enabled
   */
  bool IsMarkupProcessingEnabled() const;


public: /* Signals */

  /// @brief Input Signal.
  typedef Signal< void ( TextInput textInput ) > InputSignalType;

  /// @brief Input style changed signal.
  typedef Signal< void ( TextInput textInput, const TextStyle& style ) > StyleChangedSignalType;

  /// @brief Text modified signal.
  typedef Signal< void ( TextInput textInput  ) > TextModifiedSignalType;

  /// @brief Max input characters reached signal.
  typedef Signal< void ( TextInput textInput ) > MaxInputCharactersReachedSignalType;

  /// @brief Input text exceeds boundaries signal.
  typedef Signal< void ( TextInput textInput ) > InputTextExceedBoundariesSignalType;

  /**
   * @brief Signal emitted when the Text-Input starts receiving input.
   */
  InputSignalType& InputStartedSignal();

  /**
   * @brief Signal emitted when the Text-Input is finished receiving input.
   *
   * TextInput::GetText() can be called to get current text string.
   * @return The signal to connect to
   */
  InputSignalType& InputFinishedSignal();

  /**
   * @brief Signal emitted when the cut and paste toolbar is displayed.
   *
   * @return The signal to connect to
   */
  InputSignalType& CutAndPasteToolBarDisplayedSignal();

  /**
   * @brief Signal emitted when style changes.
   *
   * @return The signal to connect to
   */
  StyleChangedSignalType& StyleChangedSignal();

  /**
   * @brief Signal emitted when text changes.
   *
   * @return The signal to connect to.
   */
  TextModifiedSignalType& TextModifiedSignal();

  /**
   * @brief Signal emitted when max input characters are reached during text input.
   *
   * @return The signal to connect to
   */
  MaxInputCharactersReachedSignalType& MaxInputCharactersReachedSignal();

  /**
   * @brief Signal emitted when input text exceeds the boundaries of the text-input.
   *
   * @return The signal to connect to
   */
  InputTextExceedBoundariesSignalType& InputTextExceedBoundariesSignal();

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL TextInput(Internal::TextInput& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL TextInput(Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_INPUT_H__
