#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_TEXTVIEW_CHARACTER_POSITIONS_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_TEXTVIEW_CHARACTER_POSITIONS_H__

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
#include <dali-toolkit/internal/controls/text-input/text-input-text-style-impl.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/text-view/text-view.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class TextInputTextStyle;

/**
 * Class which contains functions related to the TextView.
 * Functions which are needed by various parts of the TextInput to determine the position of characters.
 * Functions to Set the TextView so text justification, alignment and wrapping behaviour in the desired manner for text input.
 * The functionality of this class may be divided in future.
 */
class TextViewCharacterPositioning
{

public:

  /**
   * @brief Constructor
   *
   * @param[in] displayedTextView TextView to be used with this class.
   * @param[in] textStyle TextInputTextStyle to be used
   */
  TextViewCharacterPositioning( Toolkit::TextView displayedTextView, TextInputTextStyle& textStyle ):mTextStyle( textStyle ){};

  /**
   * Destructor
   */
  ~TextViewCharacterPositioning(){};

  /**
   * @brief Creates the TextView to be used within this class.
   */
  void CreateTextView(){};

  /*
   * @brief Get a handle to the current text view object
   * If one does not exist then it's create.
   * @return TextView
   */
  Toolkit::TextView GetTextView() const{return Toolkit::TextView();};

  /**
   * @brief Get the TextLayoutInfo from the TextView so know the updated positions of characters.
   */
  void UpdateTextLayoutInfo(){};


  /**
   *  @brief Get the TextLayoutInfo structure for the TextView
   *  @return the TextLayoutInfo structure
   */
  const Toolkit::TextView::TextLayoutInfo& GetLayoutInfo() const{return mTextLayoutInfo;};


  /**
   * @brief Get the number of characters visually represented in the text
   * @return the number of characters
   */
  std::size_t GetNumberOfCharactersInText() const{return 0;};

  /**
   * @brief Check if any text exists in TextView's character layout table
   * @return bool true if empty
   */
  bool IsTextEmpty() const{return false;};

  /**
   * @brief Get the height of the line at the given cursor position
   * @param[in] position
   * @return float the height of the line
   */
  float GetLineHeight( std::size_t position ) const{return 0.0f;};

  /**
   * @brief Sets if the inputed text can exceed the text-input boundary.
   * By default is enabled.
   * @param[in] enable Whether the inputed text can exceed its boundary.
   */
  void SetTextExceedEnabled( bool enable ) {};

  /**
   * @brief Retrieves whether inputed text can exceed the text-input boundary.
   * @return \e true if text inputed can exceed the boundary, otherwise \e false.
   */
  bool IsTextExceedEnabled() const{ return false;};

  /**
   * @brief From the given x and y vector the closest character position is returned
   * @param[in] source the x and y position
   * @return the position in the string of the character closest to the source.
   */
  std::size_t ReturnClosestIndex( const Vector2& source ){return 0;};

  /**
   * @brief If no text exists then this function uses the text alignment to position cursor.
   * @param[in] cursorPosition
   */
  void GetActualPositionFromCharacterPositionWhenNoText( Vector3& cursorPosition ) const{};

  /**
   * @brief Function to position cursor when a word is wrapped to another line
   * @param[in] characterPosition that actual position is required for
   * @return the actual position of the cursor.
   */
  Vector3 PositionCursorAfterWordWrap( std::size_t characterPosition ) const{return Vector3::ZERO;};

  /**
   * @brief Returns the x-position of the current line justification
   * (relative to left of text-view container)
   * @return x position for line justification
   */
  float GetLineJustificationPosition() const{return 0.0f;};

  /**
   * @brief Retrieve the dimensions (and min-max) of this row of text that the character resides on.
   * @param[in] characterPosition the position in the 'string' of characters.
   * @param[out] min the top-left position of the rectangle representing this row
   * @param[out] max the bottom-right position of the rectangle representing this row
   * @return The size of the rectangle representing this row (max - min)
   */
  Size GetRowRectFromCharacterPosition(std::size_t characterPosition, Vector2& min, Vector2& max) const{return Vector2::ZERO;};

  /**
   * @brief Retrieve the character position of the first character on the row of text
   * that this character resides on.
   * @param[in] logicalPosition the position in the 'string' of characters.
   * @return logical character position of start of row.
   */
  std::size_t GetRowStartFromCharacterPosition(std::size_t logicalPosition) const{return 0;};

  /**
   * @brief Gets the visual position of a logical position.
   * @note This is preferred over directly accessing the Map, as it resolves visual
   * positions outside of the character map range.
   * @param[in] logicalPosition The logical position
   * @return Visual position is returned.
   */
  std::size_t GetVisualPosition(std::size_t logicalPosition) const{return 0;};

  /**
   * @brief Return a vector which is the actual position for the given character position
   * The character position is where a cursor would be position for that character.
   * @param[in] characterPosition the logical (input) position in the 'string' of characters.
   *
   * @return Vector3 the actual x,y,z position
   */
  Vector3 GetActualPositionFromCharacterPosition(std::size_t characterPosition ) const{return Vector3::ZERO;};

  /**
   * @brief Return a vector which is the actual position for the given character position
   * The character position is where a cursor would be positioned for that character to be inserted.
   * An additional alternatePosition is also set in circumstances where the possible writing
   * of characters would be in the opposite direction.
   * e.g. "HelloشقشلاهؤEnglish"
   *            |     |
   *            *     +
   * [*] - Primary actual position for cursor i.e. continuing writing LTR (English)
   * [+] - Alternate actual position for cursor i.e. writing RTL (Arabic)
   *
   * @param[in] characterPosition the logical (input) position in the 'string' of characters.
   * @param[out] directionRTL Whether the actual x,y,z position is after LTR (false) or RTL (true) text.
   * @param[out] alternatePosition the actual x,y,z position of the cursor if user types
   * in alternate direction to current flow of text.
   * @param[out] alternatePositionValid whether this alternate position is valid.
   * @return Vector3 the actual x,y,z position
   */
  Vector3 GetActualPositionFromCharacterPosition(std::size_t characterPosition, bool& directionRTL, Vector3& alternatePosition, bool& alternatePositionValid ) const{return Vector3::ZERO;};

  /**
   * @brief Get the currently displayed text.
   * @return The currently displayed text.
   */
  std::string GetText() const{return std::string();};

  /**
   * @brief Get the text currently being displayed together with mark-up tags.
   * @return string, the currently displayed string with mark-up.
   */
  std::string GetMarkupText() const{return std::string();};

  /**
   * @brief Sets whether markup processing should be carried out.
   *
   * @param[in] enable whether markup processing is carried out or not.
   */
  void SetMarkupProcessingEnabled( bool enable ){};

  /**
   * @brief Returns whether markup processing is enabled or not
   *
   * @return true is markup processing is enabled
   */
  bool IsMarkupProcessingEnabled() const{ return false;};

  // Styled Text

  /**
   * @brief Check if styled text is empty
   * @return bool returns true if styled text is empty
   */
  bool IsStyledTextEmpty() const{ return false;};

  /**
   * @brief The size of the style text
   * @return std::size_t returns number of characters in styled text
   */
  std::size_t StyledTextSize() const{return 0;};

  /**
   * @brief Get the styled text array
   * @return reference to MarkupProcessor::StyledTextArray
   */
  MarkupProcessor::StyledTextArray& GetStyledTextArray(){return mStyledText;};

  /**
   * @brief Get the style for the character at the given position
   * @param[in] position of character style is required for
   * @return a copy of the style structure for the character at the given position
   */
  TextStyle GetStyleAt( std::size_t position ) const{return TextStyle();};

/**
   * @briefApplies the given style to all text, selected or not selected.
   * By default all style settings are applied but a bit mask could be used to modify only certain style settings.
   * @param[in] style style to apply
   * @param[in] mask mask style should be applied to
   * @param[in] begin start position of range to apply style
   * @param[in] end end position of range to apply style
   */
  void ApplyStyleToRange( const TextStyle& style, const TextStyle::Mask mask, const std::size_t begin, const std::size_t end ){};

  // Snapshot
  /**
   * @copydoc TextView::SetSnapshotModeEnabled()
   */
  void SetSnapshotModeEnabled( bool enable ){};

  /**
   * @copydoc TextView::IsSnapshotModeEnabled()
   */
  bool IsSnapshotModeEnabled() const{ return false;};

  // Scrolling
  /**
   * @copydoc TextView::SetScrollEnabled()
   */
  void SetScrollEnabled( bool enable ){};

  /**
   * @copydoc TextView::SetScrollPosition()
   */
  void SetScrollPosition( const Vector2& position ){};

  /**
   * @copydoc TextView::IsScrollEnabled()
   */
  bool IsScrollEnabled() const{ return false;};

  /**
   * @copydoc TextView::GetScrollPosition()
   */
  Vector2 GetScrollPosition() const{return Vector2::ZERO;};

  /**
   * @copydoc TextView::GetScrollPosition()
   */
  bool IsScrollPositionTrimmed() const{ return false;};

private:

  /**
   * @brief Copy Constructor
   * @param[in] characterPositioning
   * Undefined.
   */
  TextViewCharacterPositioning(const TextViewCharacterPositioning& characterPositioning );

  /**
   * @Assignment Constructor
   * @param[in] rhs
   * Undefined.
   */
  TextViewCharacterPositioning& operator=(const TextViewCharacterPositioning& rhs);

private:

  Toolkit::TextView mDisplayedTextView;         // A text view object used to display the text.
  TextInputTextStyle& mTextStyle;               // Holds the style object related to input style

  MarkupProcessor::StyledTextArray mStyledText; // String currently displayed by TextView with style info.

  Toolkit::TextView::TextLayoutInfo      mTextLayoutInfo; // It contains a table layout info per character sorted by the character's visual index (retrieved from TextView),
                                                          // a reorder map that stores each character's visual (output) index according to its logical (input) index,
                                                          // a reorder map that stores each character's logical (input) index according to its visual (output) index

  bool mExceedEnabled:1; // flag set by user to determine if text can exceed the control's size
  bool mMarkUpEnabled:1; // If Markup is being passed with the text
};


} // namespace Internal


} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_TEXTVIEW_CHARACTER_POSITIONS_H__
