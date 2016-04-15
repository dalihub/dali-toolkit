#ifndef __DALI_TOOLKIT_TEXT_RUN_CONTAINER_H__
#define __DALI_TOOLKIT_TEXT_RUN_CONTAINER_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/character-run.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief Clears the runs starting from the given character index.
 *
 * @param[in] startIndex The starting character index used to remove runs.
 * @param[in] endIndex The ending character index used to remove runs.
 * @param[in,out] runs The text's runs.
 * @param[out] startRemoveIndex The index to the first run to be removed.
 * @param[out] endRemoveIndex The index to the last run to be removed.
 */
template< typename T >
void ClearCharacterRuns( CharacterIndex startIndex,
                         CharacterIndex endIndex,
                         Vector<T>& runs,
                         uint32_t& startRemoveIndex,
                         uint32_t& endRemoveIndex )
{
  T* runsBuffer = runs.Begin();
  T* run = runsBuffer;

  const Length length = runs.Count();
  Length index = 0u;
  for( index = 0u; index < length; ++index )
  {
    if( ( run->characterRun.characterIndex <= endIndex ) &&
        ( startIndex < run->characterRun.characterIndex + run->characterRun.numberOfCharacters ) )
    {
      // Run found.

      // Set the index to the first run to be removed.
      startRemoveIndex = index;
      break;
    }

    ++run;
  }

  run = ( runsBuffer + startRemoveIndex );
  for( index = startRemoveIndex; index < length; ++index )
  {
    if( ( run->characterRun.characterIndex > endIndex ) ||
        ( startIndex >= run->characterRun.characterIndex + run->characterRun.numberOfCharacters ) )
    {
      // Run found. Nothing else to do.
      break;
    }
    ++run;
  }
  endRemoveIndex = index;

  // The number of characters to remove.
  const Length numberOfCharactersRemoved = 1u + endIndex - startIndex;

  // Update the character index of the next runs.
  run = runsBuffer;
  for( Length index = 0u; index < length; ++index )
  {
    if( run->characterRun.characterIndex > startIndex )
    {
      run->characterRun.characterIndex -= numberOfCharactersRemoved;
    }

    ++run;
  }
}

/**
 * @brief Clears the runs starting from the given character index.
 *
 * @param[in] startIndex The starting character index used to remove runs.
 * @param[in] endIndex The ending character index used to remove runs.
 * @param[in,out] runs The text's runs.
 */
template< typename T >
void ClearCharacterRuns( CharacterIndex startIndex,
                         CharacterIndex endIndex,
                         Vector<T>& runs )
{
  uint32_t startRemoveIndex = runs.Count();
  uint32_t endRemoveIndex = startRemoveIndex;
  ClearCharacterRuns( startIndex,
                      endIndex,
                      runs,
                      startRemoveIndex,
                      endRemoveIndex );

  // Remove all remaining runs.
  T* runBuffer = runs.Begin();
  runs.Erase( runBuffer + startRemoveIndex, runBuffer + endRemoveIndex );
}

/**
 * @brief Updates the number of characters and the character index of the text's style runs.
 *
 * If the @p numberOfCharacters is a negative value, it means the number of characters that are removed starting from the @p index.
 *
 * It deletes runs if all their characters are removed.
 *
 * @param[in] index Index to the first character updated.
 * @param[in] numberOfCharacters The number of characters to be updated.
 * @param[in] totalNumberOfCharacters Total number of characters of the text.
 * @param[in,out] runs The text's style runs.
 * @param[out] removedRuns The text's style removed runs.
 */
template< typename T >
void UpdateCharacterRuns( CharacterIndex index,
                          int numberOfCharacters,
                          Length totalNumberOfCharacters,
                          Vector<T>& runs,
                          Vector<T>& removedRuns )
{
  if( 0 > numberOfCharacters )
  {
    // Remove characters.
    const Length numberOfRemovedCharacters = -numberOfCharacters;

    if( ( 0u == index ) && ( numberOfRemovedCharacters == totalNumberOfCharacters ) )
    {
      // Set the removed runs.
      removedRuns = runs;

      // All characters are removed.
      runs.Clear();

      // Nothing else to do.
      return;
    }

    const VectorBase::SizeType size = runs.Count();
    // Temporary vector used to remove runs.
    Vector<T> tempRuns;
    // Reserve some space for the temporary vector.
    tempRuns.Reserve( size );
    removedRuns.Reserve( size );

    // Whether any run has to be removed.
    bool runsRemoved = false;

    // Index to the last character added/removed.
    const CharacterIndex lastIndex = index + numberOfRemovedCharacters - 1u;

    // Update the style runs
    for( typename Vector<T>::Iterator it = runs.Begin(),
           endIt = runs.End();
         it != endIt;
         ++it )
    {
      T& run = *it;

      const CharacterIndex lastRunIndex = run.characterRun.characterIndex + run.characterRun.numberOfCharacters - 1u;

      if( lastRunIndex < index )
      {
        // The style run is not affected by the removed text.
        tempRuns.PushBack( run );
        continue;
      }

      if( ( index <= run.characterRun.characterIndex ) &&
          ( lastIndex >= lastRunIndex ) )
      {
        // Add the removed run into the vector.
        removedRuns.PushBack( run );

        // All the characters are removed.
        runsRemoved = true;
      }
      else
      {
        if( lastIndex < run.characterRun.characterIndex )
        {
          // Just move the character index.
          run.characterRun.characterIndex -= numberOfRemovedCharacters;
        }
        else
        {
          if( run.characterRun.characterIndex < index )
          {
            // Remove characters starting from a character within the run.
            run.characterRun.numberOfCharacters -= std::min( numberOfRemovedCharacters, 1u + lastRunIndex - index );
          }
          else
          {
            // Remove characters starting from a character located before the first index of the run.
            run.characterRun.numberOfCharacters -= 1u + lastIndex - run.characterRun.characterIndex;
            run.characterRun.characterIndex = index;
          }
        }

        tempRuns.PushBack( run );
      }
    }

    // Copy the temporary vector if there are runs removed.
    if( runsRemoved )
    {
      runs = tempRuns;
    }
  }
  else
  {
    // Add characters.

    // Update the style runs
    for( typename Vector<T>::Iterator it = runs.Begin(),
           endIt = runs.End();
         it != endIt;
         ++it )
    {
      T& run = *it;

      // Update the number of characters of the style run.

      if( ( 0u == index ) && ( 0u == run.characterRun.characterIndex ) )
      {
        run.characterRun.numberOfCharacters += numberOfCharacters;
      }
      else if( index <= run.characterRun.characterIndex )
      {
        run.characterRun.characterIndex += numberOfCharacters;
      }
      else if( index <= run.characterRun.characterIndex + run.characterRun.numberOfCharacters )
      {
        run.characterRun.numberOfCharacters += numberOfCharacters;
      }
    }
  }
}

/**
 * @brief Clears the runs starting from the given glyph index.
 *
 * @param[in] startIndex The starting glyph index used to remove runs.
 * @param[in] endIndex The ending glyph index used to remove runs.
 * @param[in,out] runs The text's runs.
 * @param[out] startRemoveIndex The index to the first run to be removed.
 * @param[out] endRemoveIndex The index to the last run to be removed.
 */
template< typename T >
void ClearGlyphRuns( GlyphIndex startIndex,
                     GlyphIndex endIndex,
                     Vector<T>& runs,
                     uint32_t& startRemoveIndex,
                     uint32_t& endRemoveIndex )
{
  T* runsBuffer = runs.Begin();
  T* run = runsBuffer;

  const Length length = runs.Count();
  Length index = 0u;
  for( index = 0u; index < length; ++index )
  {
    if( ( run->glyphRun.glyphIndex <= endIndex ) &&
        ( startIndex < run->glyphRun.glyphIndex + run->glyphRun.numberOfGlyphs ) )
    {
      // Run found.

      // Set the index to the first run to be removed.
      startRemoveIndex = index;
      break;
    }
    ++run;
  }

  run = ( runsBuffer + startRemoveIndex );
  for( index = startRemoveIndex; index < length; ++index )
  {
    if( ( run->glyphRun.glyphIndex > endIndex ) ||
        ( startIndex >= run->glyphRun.glyphIndex + run->glyphRun.numberOfGlyphs ) )
    {
      // Run found. Nothing else to do.
      break;
    }

    ++run;
  }
  endRemoveIndex = index;

  // The number of glyphs to remove.
  const Length numberOfGlyphsRemoved = 1u + endIndex - startIndex;

  // Update the glyph index of the next runs.
  run = runsBuffer;
  for( Length index = 0u; index < length; ++index )
  {

    if( run->glyphRun.glyphIndex > startIndex )
    {
      run->glyphRun.glyphIndex -= numberOfGlyphsRemoved;
    }
  }
}

/**
 * @brief Clears the runs starting from the given glyph index.
 *
 * @param[in] startIndex The starting glyph index used to remove runs.
 * @param[in] endIndex The ending glyph index used to remove runs.
 * @param[in,out] runs The text's runs.
 */
template< typename T >
void ClearGlyphRuns( GlyphIndex startIndex,
                     GlyphIndex endIndex,
                     Vector<T>& runs )
{
  uint32_t startRemoveIndex = runs.Count();
  uint32_t endRemoveIndex = startRemoveIndex;
  ClearGlyphRuns( startIndex,
                  endIndex,
                  runs,
                  startRemoveIndex,
                  endRemoveIndex );

  // Remove all remaining runs.
  T* runBuffer = runs.Begin();
  runs.Erase( runBuffer + startRemoveIndex, runBuffer + endRemoveIndex );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_RUN_CONTAINER_H__
