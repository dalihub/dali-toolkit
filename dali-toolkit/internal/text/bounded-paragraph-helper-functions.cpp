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

// FILE HEADER
#include <dali-toolkit/internal/text/bounded-paragraph-helper-functions.h>

// INTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/script.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
void MergeBoundedParagraphRunsWhenRemoveCharacters(const Vector<Character>&     text,
                                                   const CharacterIndex&        index,
                                                   const int&                   numberOfCharacters,
                                                   Vector<BoundedParagraphRun>& boundedParagraphRuns)
{
  // This works on boundedParagraphRuns before applying on them the changes of the removed characters.
  // Update to merge only when characters have been removed.
  if(numberOfCharacters >= 0)
  {
    return;
  }

  //No runs to merge
  if(boundedParagraphRuns.Count() == 0u)
  {
    return;
  }

  const Length          numberOfRemovedCharacters     = (Length)(-1 * numberOfCharacters);
  const Length          totalNumberOfCharacters       = text.Count();
  const CharacterIndex& firstIndexOfRemovedCharacters = index;
  const CharacterIndex  lastIndexOfRemovedCharacters  = ((firstIndexOfRemovedCharacters + numberOfRemovedCharacters) > 0u) ? (firstIndexOfRemovedCharacters + numberOfRemovedCharacters - 1u) : firstIndexOfRemovedCharacters; // Note: Length is uint32. Extra validation to avoid  a potential defects.

  Length numberOfRuns          = boundedParagraphRuns.Count();
  Length firstRunIndexToUpdate = 0;
  Length lastRunIndexToUpdate  = 0;
  bool   noNeedToMerge         = false;

  // Find the first boundedParagraphRuns that is possible to be updated.
  while(firstRunIndexToUpdate < numberOfRuns)
  {
    const CharacterIndex startCharIndex = boundedParagraphRuns[firstRunIndexToUpdate].characterRun.characterIndex;
    const CharacterIndex endCharIndex   = boundedParagraphRuns[firstRunIndexToUpdate].characterRun.GetEndCharacterIndex();

    // In-case the paragraph separator of the plain text (before the current bounded paragraph) is removed.
    // In-case the start index of the current bounded paragraph is between start and end indices of removed characters.
    // In-case the end index of the current bounded paragraph is between start and end indices of removed characters.
    if((startCharIndex == lastIndexOfRemovedCharacters + 1u) ||
       ((firstIndexOfRemovedCharacters <= startCharIndex) && (startCharIndex <= lastIndexOfRemovedCharacters)) ||
       ((firstIndexOfRemovedCharacters <= endCharIndex) && (endCharIndex <= lastIndexOfRemovedCharacters)))
    {
      break;
    }
    else if(lastIndexOfRemovedCharacters + 1u < startCharIndex)
    { // The whole removed characters are exist before the remaining bounded paragraphs.
      noNeedToMerge = true;
      break;
    }
    firstRunIndexToUpdate++;
  }

  // There is no run was affected by the removed characters.
  if(noNeedToMerge || (firstRunIndexToUpdate == numberOfRuns))
  {
    return;
  }

  // Find the last boundedParagraphRuns that is possible to be updated.
  lastRunIndexToUpdate = firstRunIndexToUpdate;
  while(lastRunIndexToUpdate < numberOfRuns - 1u)
  {
    const CharacterIndex startCharIndex = boundedParagraphRuns[lastRunIndexToUpdate].characterRun.characterIndex;
    const CharacterIndex endCharIndex   = boundedParagraphRuns[lastRunIndexToUpdate].characterRun.GetEndCharacterIndex();

    if((lastIndexOfRemovedCharacters < endCharIndex) ||
       (lastIndexOfRemovedCharacters + 1u <= startCharIndex))
    {
      break;
    }

    lastRunIndexToUpdate++;
  }

  // Remove all boundedParagraphRun between firstRunIndexToUpdate and lastRunIndexToUpdate
  // At least one boundedParagraphRun between firstRunIndexToUpdate and lastRunIndexToUpdate
  if(firstRunIndexToUpdate + 1u < lastRunIndexToUpdate)
  {
    Length runIndexToDelete = firstRunIndexToUpdate + 1u;

    while(runIndexToDelete < lastRunIndexToUpdate)
    {
      Dali::Vector<BoundedParagraphRun>::Iterator paragraphToDelete = boundedParagraphRuns.Begin() + (runIndexToDelete);
      boundedParagraphRuns.Remove(paragraphToDelete);

      lastRunIndexToUpdate--;
      numberOfRuns--;
    }
  }

  CharacterIndex endCharIndexFirstRun = boundedParagraphRuns[firstRunIndexToUpdate].characterRun.GetEndCharacterIndex();
  ;
  if(firstRunIndexToUpdate == lastRunIndexToUpdate)
  {
    if(endCharIndexFirstRun < lastIndexOfRemovedCharacters)
    {
      boundedParagraphRuns[firstRunIndexToUpdate].characterRun.numberOfCharacters += (lastIndexOfRemovedCharacters - endCharIndexFirstRun);
    }
  }
  else
  {
    CharacterIndex startCharIndexLastRun = boundedParagraphRuns[lastRunIndexToUpdate].characterRun.characterIndex;
    boundedParagraphRuns[firstRunIndexToUpdate].characterRun.numberOfCharacters += ((startCharIndexLastRun - endCharIndexFirstRun) > 0u) ? (startCharIndexLastRun - endCharIndexFirstRun - 1u) : 0u; // Note: Length is uint32. Extra validation to avoid  a potential defects.

    CharacterIndex endCharIndexLastRun = boundedParagraphRuns[lastRunIndexToUpdate].characterRun.GetEndCharacterIndex();

    if(endCharIndexLastRun < lastIndexOfRemovedCharacters)
    {
      boundedParagraphRuns[lastRunIndexToUpdate].characterRun.numberOfCharacters += (lastIndexOfRemovedCharacters - endCharIndexLastRun);
    }
  }

  // Each endCharIndex for boundedParagraphRun is a paragraph separator.
  // If not then keep adding characters until find paragraph separator.
  Length runIndex = firstRunIndexToUpdate;
  while(runIndex <= lastRunIndexToUpdate)
  {
    CharacterIndex endCharIndex = boundedParagraphRuns[runIndex].characterRun.GetEndCharacterIndex();

    // The remaining text was not affected.
    if(endCharIndex > lastIndexOfRemovedCharacters)
    {
      break;
    }

    // Reference for numberOfCharacters in current run to update it
    CharacterIndex& numberOfCharactersInRun = boundedParagraphRuns[runIndex].characterRun.numberOfCharacters;

    // In-case arrived to the start of the next run and there is no paragraph separator.
    // Merging the next run with the current run. Removing the next run.
    if((runIndex + 1u < numberOfRuns) &&
       (endCharIndex <= lastIndexOfRemovedCharacters) &&
       ((lastIndexOfRemovedCharacters + 1u) < totalNumberOfCharacters) &&
       (!TextAbstraction::IsNewParagraph(*(text.Begin() + lastIndexOfRemovedCharacters + 1u))))
    {
      numberOfCharactersInRun += boundedParagraphRuns[runIndex + 1u].characterRun.numberOfCharacters;

      Dali::Vector<BoundedParagraphRun>::Iterator paragraphToDelete = boundedParagraphRuns.Begin() + (runIndex + 1u);
      boundedParagraphRuns.Remove(paragraphToDelete);

      numberOfRuns--;
      lastRunIndexToUpdate--;

      continue;
    }

    runIndex++;
  }

  // Each startCharIndex-1u for boundedParagraphRun is a paragraph separator.
  // If not then remove boundedParagraphRun.
  if(numberOfRuns > 0u &&
     firstIndexOfRemovedCharacters > 0u &&
     boundedParagraphRuns[firstRunIndexToUpdate].characterRun.characterIndex > 0u)
  {
    const CharacterIndex startCharIndex = boundedParagraphRuns[firstRunIndexToUpdate].characterRun.characterIndex;

    if(firstIndexOfRemovedCharacters <= startCharIndex && startCharIndex <= lastIndexOfRemovedCharacters + 1u)
    {
      // Verify the paragraph separator before the first run to update
      // In-case the paragraph separator is removed before the boundedParagraphRun.
      // Remove the boundedParagraphRun.
      if((!TextAbstraction::IsNewParagraph(*(text.Begin() + firstIndexOfRemovedCharacters - 1u))) &&
         (!TextAbstraction::IsNewParagraph(*(text.Begin() + startCharIndex))))
      {
        Dali::Vector<BoundedParagraphRun>::Iterator paragraphToDelete = boundedParagraphRuns.Begin() + (firstRunIndexToUpdate);
        boundedParagraphRuns.Remove(paragraphToDelete);
      }
    }
  }
}
} // namespace Text

} // namespace Toolkit

} // namespace Dali