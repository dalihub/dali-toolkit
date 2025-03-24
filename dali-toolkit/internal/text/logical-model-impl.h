#ifndef DALI_TOOLKIT_TEXT_LOGICAL_MODEL_IMPL_H
#define DALI_TOOLKIT_TEXT_LOGICAL_MODEL_IMPL_H

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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/anchor.h>
#include <dali-toolkit/internal/text/bidirectional-line-info-run.h>
#include <dali-toolkit/internal/text/bidirectional-paragraph-info-run.h>
#include <dali-toolkit/internal/text/bidirectional-support.h>
#include <dali-toolkit/internal/text/bounded-paragraph-run.h>
#include <dali-toolkit/internal/text/character-spacing-character-run.h>
#include <dali-toolkit/internal/text/color-run.h>
#include <dali-toolkit/internal/text/embedded-item.h>
#include <dali-toolkit/internal/text/font-description-run.h>
#include <dali-toolkit/internal/text/font-run.h>
#include <dali-toolkit/internal/text/paragraph-run.h>
#include <dali-toolkit/internal/text/script-run.h>
#include <dali-toolkit/internal/text/strikethrough-character-run.h>
#include <dali-toolkit/internal/text/underlined-character-run.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
class LogicalModel;
typedef IntrusivePtr<LogicalModel> LogicalModelPtr;
struct InputStyle;

/**
 * @brief A logical text model contains layout independent information.
 *
 * This includes:
 * - A series of UTF-32 characters in logical order
 */
class LogicalModel : public RefObject
{
public:
  /**
   * @brief Create a new instance of a LogicalModel.
   *
   * @return A pointer to a new LogicalModel.
   */
  static LogicalModelPtr New();

  // Language support interface.

  /**
   * @brief Retrieves the script for the given character index.
   *
   * @param[in] characterIndex Index to the character.
   *
   * @return The character's script.
   */
  Script GetScript(CharacterIndex characterIndex) const;

  // Bidirectional support interface.

  /**
   * @brief Retrieves the direction of a characters.
   *
   * See GetCharacterDirections().
   *
   * @param[in] characterIndex Index to a character.
   *
   * @return The character's direction.
   */
  CharacterDirection GetCharacterDirection(CharacterIndex characterIndex) const;

  // Visual <--> Logical conversion tables.

  /**
   * @brief Retrieves the logical cursor index for the given visual cursor index.
   *
   * @pre The method FetchBidirectionalLineInfo() must be called before. If the result of FetchBidirectionalLineInfo() is false,
   *      then the character is not in a bidirectional line and the result will be invalid.
   *
   * @param[in] visualCursorIndex The visual cursor index.
   *
   * @return The logical cursor index.
   */
  CharacterIndex GetLogicalCursorIndex(CharacterIndex visualCursorIndex);

  /**
   * @brief Retrieves the logical character index for the given visual character index.
   *
   * @pre The method FetchBidirectionalLineInfo() must be called before. If the result of FetchBidirectionalLineInfo() is false,
   *      then the character is not in a bidirectional line and the result will be invalid.
   *
   * @param[in] visualCharacterIndex The visual character index.
   *
   * @return The logical character index.
   */
  CharacterIndex GetLogicalCharacterIndex(CharacterIndex visualCharacterIndex);

  /**
   * @brief Fetch the bidirectional line info for the given character.
   *
   * Call GetBidirectionalLineInfo() to retrieve the last fetched line.
   *
   * @param[in] characterIndex The character index.
   *
   * @return @e true if the given @e character is in a bidirectional line.
   */
  bool FetchBidirectionalLineInfo(CharacterIndex characterIndex);

  /**
   * @brief Retrieves the last fetched bidirectional line info.
   *
   * @return The index of the bidirectional line info.
   */
  BidirectionalLineRunIndex GetBidirectionalLineInfo() const;

  // Text style.

  /**
   * @brief Updates the text's style runs with the added or removed text.
   *
   * @param[in] index The character's index.
   * @param[in] numberOfCharacters The number of characters added or removed. If the value is negative the characters are removed.
   */
  void UpdateTextStyleRuns(CharacterIndex index, int numberOfCharacters);

  /**
   * @brief Retrieves the text's style for the given character index.
   *
   * @param[in] index The character index.
   * @param[out] style The text's style in the given style.
   */
  void RetrieveStyle(CharacterIndex index, InputStyle& style);

  /**
   * @brief Clears the font description runs.
   */
  void ClearFontDescriptionRuns();

  /**
   * @brief Clears the strikethrough runs.
   */
  void ClearStrikethroughRuns();

  /**
   * @brief Clears the underline runs.
   */
  void ClearUnderlineRuns();

  // Paragraphs

  /**
   * @brief Creates the paragraph info.
   *
   * @pre The line break info must be set.
   *
   * @param[in] startIndex The character from where the paragraph info is set.
   * @param[in] numberOfCharacters The number of characters.
   */
  void CreateParagraphInfo(CharacterIndex startIndex,
                           Length         numberOfCharacters);

  /**
   * @brief Find the paragraphs which contains the given characters.
   *
   * @param[in] index The first character's index of the run.
   * @param[in] numberOfCharacters The number of characters of the run.
   * @param[out] paragraphs Indices to the paragraphs which contain the characters.
   */
  void FindParagraphs(CharacterIndex             index,
                      Length                     numberOfCharacters,
                      Vector<ParagraphRunIndex>& paragraphs);

  /**
   * @brief Retrieves the number of bounded paragraph runs.
   *
   * @return The number of bounded paragraph runs.
   */
  Length GetNumberOfBoundedParagraphRuns() const;

  /**
   * @brief Retrieves the reference for bounded paragraph runs.
   *
   * @return The reference for bounded paragraph runs.
   */
  const Vector<BoundedParagraphRun>& GetBoundedParagraphRuns() const;

  /**
   * @brief Retrieves the number of character-spacing runs.
   *
   * @return The number of character-spacing runs.
   */
  Length GetNumberOfCharacterSpacingCharacterRuns() const;

  /**
   * @brief Retrieves the reference for character-spacing runs.
   *
   * @return The reference for character-spacing runs.
   */
  const Vector<CharacterSpacingCharacterRun>& GetCharacterSpacingCharacterRuns() const;

  // Embedded images

  /**
   * @brief Clears the embedded images.
   */
  void ClearEmbeddedImages();

  /**
   * @brief Clears the anchors.
   */
  void ClearAnchors();

  /**
   * @brief Clears bidirectional paragraph info.
   * @param[in] bidirectionalSupport BidirectionalSupport to clear information.
   */
  void ClearBidirectionalParagraphInfo(TextAbstraction::BidirectionalSupport& bidirectionalSupport);

protected:
  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~LogicalModel();

private:
  /**
   * @brief Private constructor.
   */
  LogicalModel();

  // Undefined
  LogicalModel(const LogicalModel& handle);

  // Undefined
  LogicalModel& operator=(const LogicalModel& handle);

public:
  Vector<Character>                     mText;
  Vector<ScriptRun>                     mScriptRuns;
  Vector<FontRun>                       mFontRuns;
  Vector<ColorRun>                      mColorRuns;
  Vector<ColorRun>                      mBackgroundColorRuns;
  Vector<FontDescriptionRun>            mFontDescriptionRuns;
  Vector<LineBreakInfo>                 mLineBreakInfo;
  Vector<ParagraphRun>                  mParagraphInfo;
  Vector<BidirectionalParagraphInfoRun> mBidirectionalParagraphInfo;
  Vector<CharacterDirection>            mCharacterDirections; ///< For each character, whether is right to left. ( @e flase is left to right, @e true right to left ).
  Vector<BidirectionalLineInfoRun>      mBidirectionalLineInfo;
  Vector<EmbeddedItem>                  mEmbeddedItems;
  Vector<Anchor>                        mAnchors;
  Vector<UnderlinedCharacterRun>        mUnderlinedCharacterRuns;       ///< The underlined character run from markup-processor
  Vector<StrikethroughCharacterRun>     mStrikethroughCharacterRuns;    ///< The strikethrough character run from markup-processor
  Vector<BoundedParagraphRun>           mBoundedParagraphRuns;          ///< The bounded paragraph is used to handle a paragraph mark-up tag and it's attributes. Like TextAlign, TextDirection, TextIndent, LineHeight, etc.
  Vector<CharacterSpacingCharacterRun>  mCharacterSpacingCharacterRuns; ///< The character-spacing character run from markup-processor.

  Property::Map                         mVariationsMap; ///< The map for variable fonts. it might be replaced by variable map run.

  BidirectionalLineRunIndex mBidirectionalLineIndex; ///< The last fetched bidirectional line info.
  bool                      mSpannedTextPlaced : 1;  ///< Whether the spanned-text is placed.

  bool mUnderlineRunsUpdated : 1;        /// Whether the UnderlinedCharacterRuns is updated. (Added for SpannedText)
  bool mCharacterSpacingRunsUpdated : 1; // Flag for updating character-spacing.
  bool mStrikethroughRunsUpdated : 1;    /// Whether the StrikethroughCharacterRuns is updated. (Added for SpannedText)
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_LOGICAL_MODEL_IMPL_H
