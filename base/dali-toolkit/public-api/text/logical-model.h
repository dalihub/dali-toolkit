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
