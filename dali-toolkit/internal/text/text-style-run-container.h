#ifndef __DALI_TOOLKIT_TEXT_STYLE_RUN_CONTAINER_H__
#define __DALI_TOOLKIT_TEXT_STYLE_RUN_CONTAINER_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/character-run.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

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

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_STYLE_RUN_CONTAINER_H__
