#ifndef __DALI_TOOLKIT_TEXT_LOGICAL_MODEL_H__
#define __DALI_TOOLKIT_TEXT_LOGICAL_MODEL_H__

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
#include <dali/public-api/common/intrusive-ptr.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/text-definitions.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct FontRun;
class LogicalModel;
typedef IntrusivePtr<LogicalModel> LogicalModelPtr;
struct ScriptRun;

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

  // Text interface.

  /**
   * @brief Replaces any text previously set.
   *
   * @param[in] text An array of UTF-32 characters.
   * @param[in] length The length of the array.
   */
  void SetText( const Character* text,
                Length length );

  /**
   * @brief Retrieves the number of characters of the text.
   *
   * @return The number of characters.
   */
  Length GetNumberOfCharacters() const;

  /**
   * @brief Retrieves characters from the text in the given buffer.
   *
   * @pre The size of the @p text buffer needs to be big enough to copy the @p numberOfCharacters.
   * @param[in] characterIndex The index to the first character to copy.
   * @param[out] text Pointer to a buffer where the text is copied.
   * @param[in] numberOfCharacters The number of characters to be copied.
   */
  void GetText( CharacterIndex characterIndex,
                Character* text,
                Length numberOfCharacters ) const;

  // Language support interface.

  /**
   * Sets the script runs.
   *
   * Replaces any scripts previously set.
   *
   * A run is a group of consecutive characters. A script run contains the script for a run.
   *
   * @param[in] scripts Pointer to a buffer with all the script runs.
   * @param[in] numberOfRuns The number of script runs.
   */
  void SetScripts( const ScriptRun* const scripts,
                   Length numberOfRuns );

  /**
   * Retrieves the number of script runs for the given range of characters.
   *
   * A run is a group of consecutive characters. A script run contains the script for a run.
   *
   * @param[in] characterIndex Index to the first character.
   * @param[in] numberOfCharacters The number of characters.
   *
   * @return The number of script runs.
   */
  Length GetNumberOfScriptRuns( CharacterIndex characterIndex,
                                Length numberOfCharacters ) const;

  /**
   * Retrieves the script runs for the given range of characters.
   *
   * The @p scriptRuns buffer needs to be big enough to copy the number of script runs.
   * Call GetNumberOfScriptRuns() to retrieve the number of script runs.
   *
   * @param[out] scriptRuns Pointer to a buffer where the script runs are copied.
   * @param[in] characterIndex Index to the first character.
   * @param[in] numberOfCharacters The number of characters.
   */
  void GetScriptRuns( ScriptRun* scriptRuns,
                      CharacterIndex characterIndex,
                      Length numberOfCharacters ) const;

  /**
   * Retrieves the script for the given character index.
   *
   * @param[in] characterIndex Index to the character.
   *
   * @return The character's script.
   */
  Script GetScript( CharacterIndex characterIndex ) const;

  /**
   * Sets the font runs.
   *
   * Replaces any fonts previously set.
   *
   * A run is a group of consecutive characters. A font run contains the font id for a run.
   *
   * @param[in] fonts Pointer to a buffer with all the font runs.
   * @param[in] numberOfRuns The number of font runs.
   */
  void SetFonts( const FontRun* const fonts,
                 Length numberOfRuns );

  /**
   * Retrieves the number of font runs for the given range of characters.
   *
   * A run is a group of consecutive characters. A font run contains the font id for a run.
   *
   * @param[in] characterIndex Index to the first character.
   * @param[in] numberOfCharacters The number of characters.
   *
   * @return The number of font runs.
   */
  Length GetNumberOfFontRuns( CharacterIndex characterIndex,
                              Length numberOfCharacters ) const;

  /**
   * Retrieves the font runs for the given range of characters.
   *
   * The @p fontRuns buffer needs to be big enough to copy the number of font runs.
   * Call GetNumberOfFontRuns() to retrieve the number of font runs.
   *
   * @param[out] fontRuns Pointer to a buffer where the font runs are copied.
   * @param[in] characterIndex Index to the first character.
   * @param[in] numberOfCharacters The number of characters.
   */
  void GetFontRuns( FontRun* fontRuns,
                    CharacterIndex characterIndex,
                    Length numberOfCharacters ) const;

  /**
   * Retrieves the font id for the given character index.
   *
   * @param[in] characterIndex Index to the first character.
   *
   * @return The font id.
   */
  FontId GetFont( CharacterIndex characterIndex ) const;

  // Break info interface.

  /**
   * Sets the line break info.
   *
   * See GetLineBreakInfo() to get how the line break info is encoded.
   *
   * Replaces any line break info previously set.
   *
   * @param[in] lineBreakInfo Pointer to a buffer with the line break info.
   * @param[in] length The size of the buffer.
   */
  void SetLineBreakInfo( const LineBreakInfo* const lineBreakInfo,
                         Length length );

  /**
   * Retrieves the line break info in the given buffer.
   *
   * The size of the @p lineBreakInfo buffer needs to be big enough to copy the @p numberOfItems.
   *
   * Possible values for LineBreakInfo are:
   *
   *  - 0 is a LINE_MUST_BREAK.  Text must be broken into a new line.
   *  - 1 is a LINE_ALLOW_BREAK. Is possible to break the text into a new line.
   *  - 2 is a LINE_NO_BREAK.    Text can't be broken into a new line.
   *
     @verbatim
     i.e. Hello big\nworld produces:
          2222212220 22220
     @endverbatim
   *
   * @param[out] lineBreakInfo Pointer to a buffer where the line break info is copied.
   * @param[in] characterIndex Index to the first line break info item.
   * @param[in] numberOfItems The number of items to be copied.
   */
  void GetLineBreakInfo( LineBreakInfo* lineBreakInfo,
                         CharacterIndex characterIndex,
                         Length numberOfItems ) const;

  /**
   * Retrieves the line break info for the given item index.
   *
   * @param[in] characterIndex Index to the line break info item.
   */
  LineBreakInfo GetLineBreakInfo( CharacterIndex characterIndex ) const;

  /**
   * Sets the word break info.
   *
   * See GetWordBreakInfo() to get how the word break info is encoded.
   *
   * Replaces any word break info previously set.
   *
   * @param[in] wordBreakInfo Pointer to a buffer with the word break info.
   * @param[in] length The size of the buffer.
   */
  void SetWordBreakInfo( const WordBreakInfo* const wordBreakInfo,
                         Length length );

  /**
   * Retrieves the word break info in the given buffer.
   *
   * The size of the @p wordBreakInfo buffer needs to be big enough to copy the @p numberOfItems.
   *
   * The size of the buffer has to be big enough to store the whole word break info per character.
   * Call GetNumberOfCharacters() to get the number of characters.
   *
   * Possible values for WordBreakInfo are:
   *
   * - 0 is a WORD_BREAK.    Text can be broken into a new word.
   * - 1 is a WORD_NO_BREAK. Text can't be broken into a new word.
   *
     @verbatim
     i.e. Hello big\nworld produces:
          1111001100 11110
     @endverbatim
   *
   * @param[out] wordBreakInfo Pointer to a buffer where the word break info is copied.
   * @param[in] characterIndex Index to the first word break info item.
   * @param[in] numberOfItems The number of items to be copied.
   */
  void GetWordBreakInfo( WordBreakInfo* wordBreakInfo,
                         CharacterIndex characterIndex,
                         Length numberOfItems ) const;

  /**
   * Retrieves the word break info for the given item index.
   *
   * @param[in] characterIndex Index to the word break info item.
   */
  WordBreakInfo GetWordBreakInfo( CharacterIndex characterIndex ) const;

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

private:

  struct Impl;
  Impl* mImpl;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_LOGICAL_MODEL_H__
