#ifndef __DALI_TOOLKIT_TEXT_LOGICAL_MODEL_IMPL_H__
#define __DALI_TOOLKIT_TEXT_LOGICAL_MODEL_IMPL_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/ref-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bidirectional-line-info-run.h>
#include <dali-toolkit/internal/text/bidirectional-paragraph-info-run.h>
#include <dali-toolkit/internal/text/font-run.h>
#include <dali-toolkit/internal/text/script-run.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

class LogicalModel;
typedef IntrusivePtr<LogicalModel> LogicalModelPtr;

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
  Script GetScript( CharacterIndex characterIndex ) const;

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
  CharacterDirection GetCharacterDirection( CharacterIndex characterIndex ) const;

  // Visual <--> Logical conversion tables.

  /**
   * @brief Sets the visual to logical and the logical to visual map tables.
   *
   * Replaces any map tables previously set.
   *
   * @note If the number of runs is zero the bidirectional info buffer is cleared.
   *
   * @param[in] bidirectionalInfo Pointer to a buffer with all the bidirectional info runs.
   * @param[in] numberOfRuns The number of bidirectional info runs.
   */
  void SetVisualToLogicalMap( const BidirectionalLineInfoRun* const bidirectionalInfo,
                              Length numberOfRuns );

  /**
   * @brief Retrieves the logical character index for the given visual character index.
   *
   * @param[in] visualCharacterIndex The visual character index.
   *
   * @return The logical character index.
   */
  CharacterIndex GetLogicalCharacterIndex( CharacterIndex visualCharacterIndex ) const;

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
  LogicalModel( const LogicalModel& handle );

  // Undefined
  LogicalModel& operator=( const LogicalModel& handle );

public:

  Vector<Character>                     mText;
  Vector<ScriptRun>                     mScriptRuns;
  Vector<FontRun>                       mFontRuns;
  Vector<LineBreakInfo>                 mLineBreakInfo;
  Vector<WordBreakInfo>                 mWordBreakInfo;
  Vector<BidirectionalParagraphInfoRun> mBidirectionalParagraphInfo;
  Vector<CharacterDirection>            mCharacterDirections;        ///< For each character, whether is right to left. ( @e flase is left to right, @e true right to left ).
  Vector<BidirectionalLineInfoRun>      mBidirectionalLineInfo;
  Vector<CharacterIndex>                mLogicalToVisualMap;         ///< Bidirectional logical to visual conversion table.
  Vector<CharacterIndex>                mVisualToLogicalMap;         ///< Bidirectional visual to logical conversion table.
  Vector<CharacterIndex>                mVisualToLogicalCursorMap;   ///< Bidirectional visual to logical cursor conversion table.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_LOGICAL_MODEL_IMPL_H__
