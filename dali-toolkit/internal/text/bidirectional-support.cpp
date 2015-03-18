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
#include <dali/public-api/text-abstraction/bidirectional-support.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

namespace
{

/**
 * @brief Get the lines of a paragraph.
 *
 * @param[in] paragraphInfo The paragraph.
 * @param[in] lines The lines.
 * @param[in] lineIndex Index pointing the first line to be checked.
 * @param[out] firstLine Index to the first line of the paragraph.
 * @param[out] numberOfLines The number of lines.
 */
void GetLines( const BidirectionalParagraphInfoRun& paragraphInfo,
               const Vector<LineRun>& lines,
               unsigned int lineIndex,
               unsigned int& firstLine,
               unsigned int& numberOfLines )
{
  firstLine = lineIndex;
  numberOfLines = 0u;

  const CharacterIndex lastCharacterIndex = paragraphInfo.characterRun.characterIndex + paragraphInfo.characterRun.numberOfCharacters;
  bool firstLineFound = false;

  for( Vector<LineRun>::ConstIterator it = lines.Begin() + lineIndex,
         endIt = lines.End();
       it != endIt;
       ++it )
  {
    const LineRun& line = *it;

    if( ( line.characterRun.characterIndex + line.characterRun.numberOfCharacters > paragraphInfo.characterRun.characterIndex ) &&
        ( lastCharacterIndex > line.characterRun.characterIndex ) )
    {
      firstLineFound = true;
      ++numberOfLines;
    }
    else if( lastCharacterIndex <= line.characterRun.characterIndex )
    {
      // nothing else to do.
      break;
    }

    if( !firstLineFound )
    {
      ++firstLine;
    }
  }
}

} // namespace

void SetBidirectionalInfo( const Vector<Character>& text,
                           const Vector<ScriptRun>& scripts,
                           const Vector<LineBreakInfo>& lineBreakInfo,
                           Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo )
{
  // Traverse the script runs. If there is one with a right to left script, create the bidirectional info for the paragraph containing that script is needed.
  // From the bidirectional point of view, a paragraph is the piece of text between two LINE_MUST_BREAK.

  // Index pointing the first character of the current paragraph.
  CharacterIndex paragraphCharacterIndex = 0u;

  // Pointer to the text buffer.
  const Character* textBuffer = text.Begin();

  // Pointer to the line break info buffer.
  const LineBreakInfo* lineBreakInfoBuffer = lineBreakInfo.Begin();

  // The number of characters.
  const Length numberOfCharacters = text.Count();

  // Handle to the bidirectional info module in text-abstraction.
  TextAbstraction::BidirectionalSupport bidirectionalSupport = TextAbstraction::BidirectionalSupport::Get();

  for( Vector<ScriptRun>::ConstIterator it = scripts.Begin(),
         endIt = scripts.End();
       it != endIt;
       ++it )
  {
    const ScriptRun& scriptRun = *it;
    const CharacterIndex lastScriptRunIndex = scriptRun.characterRun.characterIndex + scriptRun.characterRun.numberOfCharacters;

    if( TextAbstraction::IsRightToLeftScript( scriptRun.script ) && // The script is right to left.
        ( lastScriptRunIndex > paragraphCharacterIndex ) )          // It isn't part of a previous paragraph.
    {
      // Find the paragraphs which contains this script run.
      // Consider:
      //  1) Different paragraphs may contain this script run.
      //  ------||------------------- rtl sr ------------------------||-------------------
      //  --||----- p -----||------------------ p -------------||-------- p ------||------
      //
      //  2) The paragraph which contains this script run may contain other right to left script runs.
      //  -----||--- rtl sr ---||---- ltr sr ----||---------- rtl sr -----------||--------
      //  -----||---------------------------------- p -----------------------------------|

      while( lastScriptRunIndex > paragraphCharacterIndex )
      {
        // There is a paragraph which contains the current script.

        Length index = paragraphCharacterIndex;
        while( ( index < numberOfCharacters ) && ( paragraphCharacterIndex < lastScriptRunIndex ) )
        {
          if( TextAbstraction::LINE_MUST_BREAK == *( lineBreakInfoBuffer + index ) )
          {
            if( index >= scriptRun.characterRun.characterIndex )
            {
              // The Bidirectional run must have the same number of characters than the paragraph.
              BidirectionalParagraphInfoRun bidirectionalRun;
              bidirectionalRun.characterRun.characterIndex = paragraphCharacterIndex;
              bidirectionalRun.characterRun.numberOfCharacters = ( index - paragraphCharacterIndex ) + 1u; // The must break character is part of the paragrah.

              // Create the bidirectional info for the whole paragraph and store the index to the table with this info in the run.
              bidirectionalRun.bidirectionalInfoIndex = bidirectionalSupport.CreateInfo( textBuffer + bidirectionalRun.characterRun.characterIndex,
                                                                                         bidirectionalRun.characterRun.numberOfCharacters );

              bidirectionalInfo.PushBack( bidirectionalRun );
            }

            // Update the character index of the next paragraph.
            paragraphCharacterIndex = index + 1u;
          }
          ++index;
        }

        // The last character is always a must-break, so there is no need to check if there is characters left.
      }
    }
  }
}

void ReplaceBidirectionalInfo( LogicalModel& model,
                               CharacterIndex characterIndex,
                               Length numberOfCharactersToRemove,
                               Length numberOfCharactersToInsert )
{
}

void ReorderLines( const Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo,
                   const Vector<LineRun>& lineRuns,
                   Vector<BidirectionalLineInfoRun>& lineInfoRuns )
{
  // Handle to the bidirectional info module in text-abstraction.
  TextAbstraction::BidirectionalSupport bidirectionalSupport = TextAbstraction::BidirectionalSupport::Get();

  // Keep an index to the first line to be checked if it's contained inside the paragraph.
  // Avoids check the lines from the beginning for each paragraph.
  unsigned int lineIndex = 0u;

  for( Vector<BidirectionalParagraphInfoRun>::ConstIterator it = bidirectionalInfo.Begin(),
         endIt = bidirectionalInfo.End();
       it != endIt;
       ++it )
  {
    const BidirectionalParagraphInfoRun& paragraphInfo = *it;
    const CharacterDirection direction = bidirectionalSupport.GetParagraphDirection( paragraphInfo.bidirectionalInfoIndex );

    // Get the lines for this paragraph.
    unsigned int firstLine = 0u;
    unsigned int numberOfLines = 0u;

    // Get an index to the first line and the number of lines of the current paragraph.
    GetLines( paragraphInfo,
              lineRuns,
              lineIndex,
              firstLine,
              numberOfLines );

    lineIndex = firstLine + numberOfLines;

    // Traverse the lines and reorder them
    for( Vector<LineRun>::ConstIterator lineIt = lineRuns.Begin() + firstLine,
           endLineIt = lineRuns.Begin() + firstLine + numberOfLines;
           lineIt != endLineIt;
         ++lineIt )
    {
      const LineRun& line = *lineIt;

      // Creates a bidirectional info for the line run.
      BidirectionalLineInfoRun lineInfoRun;
      lineInfoRun.characterRun.characterIndex = line.characterRun.characterIndex;
      lineInfoRun.characterRun.numberOfCharacters = line.characterRun.numberOfCharacters;
      lineInfoRun.direction = direction;

      // Allocate space for the conversion maps.
      // The memory is freed after the visual to logical to visual conversion tables are built in the logical model.
      lineInfoRun.visualToLogicalMap = reinterpret_cast<CharacterIndex*>( malloc( line.characterRun.numberOfCharacters * sizeof( CharacterIndex ) ) );

      // Reorders the line.
      bidirectionalSupport.Reorder( paragraphInfo.bidirectionalInfoIndex,
                                    line.characterRun.characterIndex,
                                    line.characterRun.numberOfCharacters,
                                    lineInfoRun.visualToLogicalMap );

      // Push the run into the vector.
      lineInfoRuns.PushBack( lineInfoRun );
    }
  }
}

void ReorderLines( LogicalModel& logicalModel,
                   const VisualModel& visualModel,
                   CharacterIndex characterIndex,
                   Length numberOfCharactersToRemove,
                   Length numberOfCharactersToInsert )
{
}

bool GetMirroredText( const Vector<Character>& text,
                      Vector<Character>& mirroredText )
{
  // Handle to the bidirectional info module in text-abstraction.
  TextAbstraction::BidirectionalSupport bidirectionalSupport = TextAbstraction::BidirectionalSupport::Get();

  mirroredText = text;

  return bidirectionalSupport.GetMirroredText( mirroredText.Begin(),
                                               mirroredText.Count() );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
