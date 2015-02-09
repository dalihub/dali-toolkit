#ifndef __DALI_TOOLKIT_TEXT_PROCESSOR_BIDIRECTIONAL_INFO_H__
#define __DALI_TOOLKIT_TEXT_PROCESSOR_BIDIRECTIONAL_INFO_H__

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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/text/text.h>

// EXTERNAL INCLUDES
#include <fribidi/fribidi.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace TextProcessor
{

/**
 * Stores the text containing right to left characters and info for each character needed by fribidi to reorder a line.
 */
struct BidirectionalParagraphInfo
{
  /**
   * Default constructor.
   *
   * Initializes all members to their default values.
   */
  BidirectionalParagraphInfo();

  /**
   * Default destructor.
   */
  ~BidirectionalParagraphInfo();

  /**
   * Copy constructor.
   */
  BidirectionalParagraphInfo( const BidirectionalParagraphInfo& info );

  /**
   * Assignment operator.
   */
  BidirectionalParagraphInfo& operator=( const BidirectionalParagraphInfo& info );


  /**
   * @return Whether the paragraph is right to left.
   */
  bool IsRightToLeftParagraph() const;

  FriBidiParType               mDirection;            ///< The paragraph direction.
  std::vector<FriBidiCharType> mCharactersTypeBuffer; ///< Character type buffer.
  std::vector<FriBidiLevel>    mLevelsBuffer;         ///< Levels buffer.
  std::vector<FriBidiChar>     mLogicalUnicodeBuffer; ///< Text buffer in logical order. Coded in unicode.
};

/**
 * Stores the reordered text, the conversion tables for a paragraph's line,
 * the index to the first character of the line and the number of characters.
 */
struct BidirectionalLineInfo
{
  /**
   * Default constructor.
   *
   * Initializes all members to their default values.
   */
  BidirectionalLineInfo();

  /**
   * Default destructor.
   */
  ~BidirectionalLineInfo();

  /**
   * Copy constructor.
   */
  BidirectionalLineInfo( const BidirectionalLineInfo& info );

  /**
   * Assignment operator.
   */
  BidirectionalLineInfo& operator=( const BidirectionalLineInfo& info );

  std::size_t mCharacterParagraphIndex; ///< Index within the paragraph of the first character of the line.
  std::size_t mNumberOfCharacters;      ///< Number of characters of the line.
  Text        mText;                    ///< Text in visual order.
  Vector<int> mVisualToLogicalMap;      ///< The character position map from the visual output text to the logical input text.
  Vector<int> mLogicalToVisualMap;      ///< The character position map from the logical input text to the visual output text.
};

/**
 * Whether the text begins with right-to-left (bidirectional) character.
 * @param [in] text The given text.
 * @return \e true if the text begins right-to-left character.
 */
bool BeginsRightToLeftCharacter( const Text& text );

/**
 * Whether the text contains any right-to-left (bidirectional) character.
 * @param [in] text The given text.
 * @return \e true if the text contains right-to-left character.
 */
bool ContainsRightToLeftCharacter( const Text& text );

/**
 * Processes a bidirectional paragraph's text.
 *
 * It stores the paragraph's direction (the direction of the first non neutral character),
 * the direction of all characters, and the ligatures in case of arabic glyphs.
 *
 * It doesn't reorder the paragraph as this task must be done per line.
 * The stored info is needed to reorder each line of the paragraph.
 *
 * @param[in] paragraph The paragraph to be processed.
 * @param[out] info Struct containing the needed info to reorder each line of the paragraph.
 */
void ProcessBidirectionalText( Text& paragraph,
                               BidirectionalParagraphInfo* info );

/**
 * Reorders one line of the paragraph according the Unicode Bidirectional Algorithm.
 *
 * The result is the text in the visual order and the conversion tables: logical <--> visual order
 *
 * @param[in] paragraphInfo Struct containing the needed info to reorder each line of the paragraph.
 * @param[in,out] lineInfo Struct containing (in) A character index pointing the first character of the line and the number of characters, (out) the reordered line and the conversion tables.
 */
void ReorderLine( BidirectionalParagraphInfo* paragraphInfo,
                  BidirectionalLineInfo* lineInfo );

} // namespace TextProcessor

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_PROCESSOR_BIDIRECTIONAL_INFO_H__

