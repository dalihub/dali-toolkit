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

// FILE HEADER
#include <dali-toolkit/internal/text/bidirectional-support.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/bidirectional-support.h>
#include <memory.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
void SetBidirectionalInfo(const Vector<Character>&               text,
                          const Vector<ScriptRun>&               scripts,
                          const Vector<LineBreakInfo>&           lineBreakInfo,
                          CharacterIndex                         startIndex,
                          Length                                 numberOfCharacters,
                          Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo,
                          bool                                   matchSystemLanguageDirection,
                          Dali::LayoutDirection::Type            layoutDirection)
{
  // Find where to insert the new paragraphs.
  BidirectionalRunIndex bidiInfoIndex = 0u;
  for(Vector<BidirectionalParagraphInfoRun>::ConstIterator it    = bidirectionalInfo.Begin(),
                                                           endIt = bidirectionalInfo.End();
      it != endIt;
      ++it)
  {
    const BidirectionalParagraphInfoRun& run = *it;

    if(startIndex < run.characterRun.characterIndex + run.characterRun.numberOfCharacters)
    {
      // Found where to insert the bidi info.
      break;
    }
    ++bidiInfoIndex;
  }

  // Traverse the script runs. If there is one with a right to left script, create the bidirectional info for the paragraph containing that script is needed.
  // From the bidirectional point of view, a paragraph is the piece of text between two LINE_MUST_BREAK.

  // Index pointing the first character of the current paragraph.
  CharacterIndex paragraphCharacterIndex = startIndex;

  // Pointer to the text buffer.
  const Character* textBuffer = text.Begin();

  // Pointer to the line break info buffer.
  const LineBreakInfo* lineBreakInfoBuffer = lineBreakInfo.Begin();

  // Handle to the bidirectional info module in text-abstraction.
  TextAbstraction::BidirectionalSupport bidirectionalSupport = TextAbstraction::BidirectionalSupport::Get();

  const CharacterIndex lastCharacter = startIndex + numberOfCharacters;

  bool hasRightToLeftScript = false;

  for(Vector<ScriptRun>::ConstIterator it    = scripts.Begin(),
                                       endIt = scripts.End();
      it != endIt;
      ++it)
  {
    const ScriptRun&     scriptRun          = *it;
    const CharacterIndex lastScriptRunIndex = scriptRun.characterRun.characterIndex + scriptRun.characterRun.numberOfCharacters - 1u;

    if(startIndex > lastScriptRunIndex)
    {
      // Skip the run as it has already been processed.
      continue;
    }

    if(lastCharacter <= scriptRun.characterRun.characterIndex)
    {
      // Do not get bidirectional info beyond startIndex + numberOfCharacters.
      break;
    }

    if(!hasRightToLeftScript && scriptRun.isRightToLeft)
    {
      // The script is right to left.
      hasRightToLeftScript = true;
    }

    if(TextAbstraction::LINE_MUST_BREAK == *(lineBreakInfoBuffer + lastScriptRunIndex))
    {
      // A new paragraph has been found.

      if(hasRightToLeftScript)
      {
        // The Bidirectional run must have the same number of characters than the paragraph.
        BidirectionalParagraphInfoRun bidirectionalRun;
        bidirectionalRun.characterRun.characterIndex     = paragraphCharacterIndex;
        bidirectionalRun.characterRun.numberOfCharacters = (lastScriptRunIndex - paragraphCharacterIndex) + 1u; // The must break character is part of the paragrah.

        // Create the bidirectional info for the whole paragraph and store the index to the table with this info in the run.
        bidirectionalRun.bidirectionalInfoIndex = bidirectionalSupport.CreateInfo(textBuffer + bidirectionalRun.characterRun.characterIndex,
                                                                                  bidirectionalRun.characterRun.numberOfCharacters,
                                                                                  matchSystemLanguageDirection,
                                                                                  layoutDirection);

        bidirectionalRun.direction = bidirectionalSupport.GetParagraphDirection(bidirectionalRun.bidirectionalInfoIndex);

        bidirectionalInfo.Insert(bidirectionalInfo.Begin() + bidiInfoIndex, bidirectionalRun);
        ++bidiInfoIndex;
      }

      // Point to the next paragraph.
      paragraphCharacterIndex = lastScriptRunIndex + 1u;

      // Reset whether there is a right to left script.
      hasRightToLeftScript = false;
    }
  }

  // Update indices of the bidi runs.
  for(Vector<BidirectionalParagraphInfoRun>::Iterator it    = bidirectionalInfo.Begin() + bidiInfoIndex,
                                                      endIt = bidirectionalInfo.End();
      it != endIt;
      ++it)
  {
    BidirectionalParagraphInfoRun& run = *it;

    run.characterRun.characterIndex += numberOfCharacters;
  }
}

void ReorderLine(const BidirectionalParagraphInfoRun& bidirectionalParagraphInfo,
                 Vector<BidirectionalLineInfoRun>&    lineInfoRuns,
                 BidirectionalLineRunIndex            bidiLineIndex,
                 CharacterIndex                       startIndex,
                 Length                               numberOfCharacters,
                 CharacterDirection                   direction)
{
  // Handle to the bidirectional info module in text-abstraction.
  TextAbstraction::BidirectionalSupport bidirectionalSupport = TextAbstraction::BidirectionalSupport::Get();

  // Creates a bidirectional info for the line run.
  BidirectionalLineInfoRun lineInfoRun;
  lineInfoRun.characterRun.characterIndex     = startIndex;
  lineInfoRun.characterRun.numberOfCharacters = numberOfCharacters;
  lineInfoRun.direction                       = direction;
  lineInfoRun.isIdentity                      = true;

  // Allocate space for the conversion maps.
  // The memory is freed after the visual to logical to visual conversion tables are built in the logical model.
  lineInfoRun.visualToLogicalMap = reinterpret_cast<CharacterIndex*>(malloc(numberOfCharacters * sizeof(CharacterIndex)));

  if(nullptr != lineInfoRun.visualToLogicalMap)
  {
    // Reorders the line.
    bidirectionalSupport.Reorder(bidirectionalParagraphInfo.bidirectionalInfoIndex,
                                 lineInfoRun.characterRun.characterIndex - bidirectionalParagraphInfo.characterRun.characterIndex,
                                 lineInfoRun.characterRun.numberOfCharacters,
                                 lineInfoRun.visualToLogicalMap);

    // For those LTR lines inside a bidirectional paragraph.
    // It will save to relayout the line after reordering.
    for(unsigned int i = 0; i < numberOfCharacters; ++i)
    {
      if(i != *(lineInfoRun.visualToLogicalMap + i))
      {
        lineInfoRun.isIdentity = false;
        break;
      }
    }
  }

  // Push the run into the vector.
  lineInfoRuns.Insert(lineInfoRuns.Begin() + bidiLineIndex, lineInfoRun);
}

bool GetMirroredText(const Vector<Character>&                     text,
                     const Vector<CharacterDirection>&            directions,
                     const Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo,
                     CharacterIndex                               startIndex,
                     Length                                       numberOfCharacters,
                     Vector<Character>&                           mirroredText)
{
  bool hasTextMirrored = false;

  // Handle to the bidirectional info module in text-abstraction.
  TextAbstraction::BidirectionalSupport bidirectionalSupport = TextAbstraction::BidirectionalSupport::Get();

  mirroredText = text;

  Character*          mirroredTextBuffer = mirroredText.Begin();
  CharacterDirection* directionsBuffer   = directions.Begin();

  CharacterIndex       index         = startIndex;
  const CharacterIndex lastCharacter = startIndex + numberOfCharacters;

  // Traverse the paragraphs and mirror the right to left ones.
  for(Vector<BidirectionalParagraphInfoRun>::ConstIterator it    = bidirectionalInfo.Begin(),
                                                           endIt = bidirectionalInfo.End();
      it != endIt;
      ++it)
  {
    const BidirectionalParagraphInfoRun& paragraph = *it;

    if(index >= paragraph.characterRun.characterIndex + paragraph.characterRun.numberOfCharacters)
    {
      // Skip the paragraph as it has already been processed.
      continue;
    }

    if(lastCharacter <= paragraph.characterRun.characterIndex)
    {
      // Do not get mirror characters beyond startIndex + numberOfCharacters.
      break;
    }

    index += paragraph.characterRun.numberOfCharacters;
    const bool tmpMirrored = bidirectionalSupport.GetMirroredText(mirroredTextBuffer + paragraph.characterRun.characterIndex,
                                                                  directionsBuffer + paragraph.characterRun.characterIndex,
                                                                  paragraph.characterRun.numberOfCharacters);

    hasTextMirrored = hasTextMirrored || tmpMirrored;
  }

  return hasTextMirrored;
}

void GetCharactersDirection(const Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo,
                            Length                                       totalNumberOfCharacters,
                            CharacterIndex                               startIndex,
                            Length                                       numberOfCharacters,
                            Vector<CharacterDirection>&                  directions)
{
  // Handle to the bidirectional info module in text-abstraction.
  TextAbstraction::BidirectionalSupport bidirectionalSupport = TextAbstraction::BidirectionalSupport::Get();

  // Resize the vector.
  directions.Resize(totalNumberOfCharacters);

  // Whether the current buffer is being updated or is set from scratch.
  const bool updateCurrentBuffer = numberOfCharacters < totalNumberOfCharacters;

  CharacterDirection*        directionsBuffer = NULL;
  Vector<CharacterDirection> newDirections;

  if(updateCurrentBuffer)
  {
    newDirections.Resize(numberOfCharacters);
    directionsBuffer = newDirections.Begin();
  }
  else
  {
    directionsBuffer = directions.Begin();
  }

  const CharacterIndex lastCharacter = startIndex + numberOfCharacters;
  CharacterIndex       index         = startIndex;

  for(Vector<BidirectionalParagraphInfoRun>::ConstIterator it    = bidirectionalInfo.Begin(),
                                                           endIt = bidirectionalInfo.End();
      it != endIt;
      ++it)
  {
    const BidirectionalParagraphInfoRun& paragraph = *it;

    if(index >= paragraph.characterRun.characterIndex + paragraph.characterRun.numberOfCharacters)
    {
      // Skip the paragraph as it has already been processed.
      continue;
    }

    if(lastCharacter <= paragraph.characterRun.characterIndex)
    {
      // Do not get the character directions beyond startIndex + numberOfCharacters.
      break;
    }

    // Set the directions of any previous left to right characters.
    const Length numberOfLeftToRightCharacters = paragraph.characterRun.characterIndex - index;
    if(numberOfLeftToRightCharacters > 0u)
    {
      memset(directionsBuffer + index - startIndex, false, numberOfLeftToRightCharacters * sizeof(bool));
    }

    // Set the directions of the bidirectional text.
    bidirectionalSupport.GetCharactersDirection(paragraph.bidirectionalInfoIndex,
                                                directionsBuffer + paragraph.characterRun.characterIndex - startIndex,
                                                paragraph.characterRun.numberOfCharacters);

    // Update the index.
    index += paragraph.characterRun.numberOfCharacters + numberOfLeftToRightCharacters;
  }

  // Fills with left to right those paragraphs without right to left characters.
  memset(directionsBuffer + index - startIndex, false, (lastCharacter - index) * sizeof(bool));

  // If the direction info is updated, it needs to be inserted in the model.
  if(updateCurrentBuffer)
  {
    // Insert the directions in the given buffer.
    directions.Insert(directions.Begin() + startIndex,
                      newDirections.Begin(),
                      newDirections.End());
    directions.Resize(totalNumberOfCharacters);
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
