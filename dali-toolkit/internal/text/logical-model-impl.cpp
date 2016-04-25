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

// CLASS HEADER
#include <dali-toolkit/internal/text/logical-model-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/input-style.h>
#include <dali-toolkit/internal/text/text-run-container.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

void FreeFontFamilyNames( Vector<FontDescriptionRun>& fontDescriptionRuns )
{
  for( Vector<FontDescriptionRun>::Iterator it = fontDescriptionRuns.Begin(),
         endIt = fontDescriptionRuns.End();
       it != endIt;
       ++it )
  {
    delete (*it).familyName;
  }

  fontDescriptionRuns.Clear();
}

LogicalModelPtr LogicalModel::New()
{
  return LogicalModelPtr( new LogicalModel() );
}

Script LogicalModel::GetScript( CharacterIndex characterIndex ) const
{
  // If this operation is too slow, consider a binary search.

  for( Length index = 0u, length = mScriptRuns.Count(); index < length; ++index )
  {
    const ScriptRun* const scriptRun = mScriptRuns.Begin() + index;

    if( ( scriptRun->characterRun.characterIndex <= characterIndex ) &&
        ( characterIndex < scriptRun->characterRun.characterIndex + scriptRun->characterRun.numberOfCharacters ) )
    {
      return scriptRun->script;
    }
  }

  return TextAbstraction::UNKNOWN;
}

CharacterDirection LogicalModel::GetCharacterDirection( CharacterIndex characterIndex ) const
{
  if( characterIndex >= mCharacterDirections.Count() )
  {
    // The model has no right to left characters, so the vector of directions is void.
    return false;
  }

  return *( mCharacterDirections.Begin() + characterIndex );
}

CharacterIndex LogicalModel::GetLogicalCursorIndex( CharacterIndex visualCursorIndex )
{
  // The total number of characters.
  const Length numberOfCharacters = mText.Count();

  const bool fetch = FetchBidirectionalLineInfo( visualCursorIndex );
  if( !fetch )
  {
    // The character is not inside a bidi line.
    return visualCursorIndex;
  }

  // The character's directions buffer.
  const CharacterDirection* const modelCharacterDirections = mCharacterDirections.Begin();

  // The bidirectional line info.
  const BidirectionalLineInfoRun* const bidirectionalLineInfo = mBidirectionalLineInfo.Begin() + mBidirectionalLineIndex;

  // Whether the paragraph starts with a right to left character.
  const bool isRightToLeftParagraph = bidirectionalLineInfo->direction;

  CharacterIndex logicalCursorIndex = 0u;

  if( 0u == visualCursorIndex )
  {
    if( isRightToLeftParagraph )
    {
      logicalCursorIndex = numberOfCharacters;
    }
    else // else logical position is zero.
    {
      logicalCursorIndex = 0u;
    }
  }
  else if( numberOfCharacters == visualCursorIndex )
  {
    if( isRightToLeftParagraph )
    {
      logicalCursorIndex = 0u;
    }
    else // else logical position is the number of characters.
    {
      logicalCursorIndex = numberOfCharacters;
    }
  }
  else
  {
    // Get the character indexed by  index - 1 and index
    // and calculate the logical position according the directions of
    // both characters and the direction of the paragraph.

    const CharacterIndex previousVisualCursorIndex = visualCursorIndex - 1u;
    const CharacterIndex previousLogicalCursorIndex = *( bidirectionalLineInfo->visualToLogicalMap + previousVisualCursorIndex - bidirectionalLineInfo->characterRun.characterIndex ) + bidirectionalLineInfo->characterRun.characterIndex;
    const CharacterIndex currentLogicalCursorIndex = *( bidirectionalLineInfo->visualToLogicalMap + visualCursorIndex - bidirectionalLineInfo->characterRun.characterIndex ) + bidirectionalLineInfo->characterRun.characterIndex;

    const CharacterDirection previousCharacterDirection = *( modelCharacterDirections + previousLogicalCursorIndex );
    const CharacterDirection currentCharacterDirection = *( modelCharacterDirections + currentLogicalCursorIndex );

    if( previousCharacterDirection == currentCharacterDirection )
    {
      // Both glyphs have the same direction.
      if( previousCharacterDirection )
      {
        logicalCursorIndex = previousLogicalCursorIndex;
      }
      else
      {
        logicalCursorIndex = currentLogicalCursorIndex;
      }
    }
    else
    {
      if( isRightToLeftParagraph )
      {
        if( currentCharacterDirection )
        {
          logicalCursorIndex = currentLogicalCursorIndex + 1u;
        }
        else
        {
          logicalCursorIndex = previousLogicalCursorIndex;
        }
      }
      else
      {
        if( previousCharacterDirection )
        {
          logicalCursorIndex = currentLogicalCursorIndex;
        }
        else
        {
          logicalCursorIndex = previousLogicalCursorIndex + 1u;
        }
      }
    }
  }

  return logicalCursorIndex;
}

CharacterIndex LogicalModel::GetLogicalCharacterIndex( CharacterIndex visualCharacterIndex )
{
  const bool fetch = FetchBidirectionalLineInfo( visualCharacterIndex );
  if( !fetch )
  {
    // The character is not inside a bidi line.
    return visualCharacterIndex;
  }

  // The bidirectional line info.
  const BidirectionalLineInfoRun* const bidirectionalLineInfo = mBidirectionalLineInfo.Begin() + mBidirectionalLineIndex;

  return *( bidirectionalLineInfo->visualToLogicalMap + visualCharacterIndex - bidirectionalLineInfo->characterRun.characterIndex ) + bidirectionalLineInfo->characterRun.characterIndex;
}

bool LogicalModel::FetchBidirectionalLineInfo( CharacterIndex characterIndex )
{
  // The number of bidirectional lines.
  const Length numberOfBidirectionalLines = mBidirectionalLineInfo.Count();

  if( 0u == numberOfBidirectionalLines )
  {
    // If there is no bidirectional info.
    return false;
  }

  // Find the bidi line where the character is laid-out.

  const BidirectionalLineInfoRun* const bidirectionalLineInfoBuffer = mBidirectionalLineInfo.Begin();

  // Check first if the character is in the previously fetched line.

  BidirectionalLineRunIndex bidiLineIndex = 0u;
  CharacterIndex lastCharacterOfRightToLeftRun = 0u;
  if( mBidirectionalLineIndex < numberOfBidirectionalLines )
  {
    const BidirectionalLineInfoRun& bidiLineRun = *( bidirectionalLineInfoBuffer + mBidirectionalLineIndex );

    // Whether the character index is just after the last one. i.e The cursor position after the last character.
    const bool isLastIndex = characterIndex == mText.Count();

    const CharacterIndex lastCharacterOfRunPlusOne = bidiLineRun.characterRun.characterIndex + bidiLineRun.characterRun.numberOfCharacters;
    if( ( bidiLineRun.characterRun.characterIndex <= characterIndex ) &&
        ( ( characterIndex < lastCharacterOfRunPlusOne ) || ( isLastIndex && ( characterIndex == lastCharacterOfRunPlusOne ) ) ) )
    {
      // The character is in the previously fetched bidi line.
      return true;
    }
    else
    {
      // The character is not in the previously fetched line.

      if( isLastIndex )
      {
        // The given index is one after the last character, so it's not in a bidi line.
        // Check if it's just after the last bidi line.
        const BidirectionalLineRunIndex lastBidiLineIndex = numberOfBidirectionalLines - 1u;
        const BidirectionalLineInfoRun& bidiLineRun = *( bidirectionalLineInfoBuffer + lastBidiLineIndex );

        if( characterIndex == bidiLineRun.characterRun.characterIndex + bidiLineRun.characterRun.numberOfCharacters )
        {
          // The character is in the last bidi line.
          mBidirectionalLineIndex = lastBidiLineIndex;
          return true;
        }
      }

      // Set the bidi line index from where to start the fetch.

      if( characterIndex < bidiLineRun.characterRun.characterIndex )
      {
        // Start the fetch from the beginning.
        bidiLineIndex = 0u;
      }
      else
      {
        // Start the fetch from the next line.
        bidiLineIndex = mBidirectionalLineIndex + 1u;
        lastCharacterOfRightToLeftRun = lastCharacterOfRunPlusOne - 1u;
      }
    }
  }

  // The character has not been found in the previously fetched bidi line.
  for( Vector<BidirectionalLineInfoRun>::ConstIterator it = bidirectionalLineInfoBuffer + bidiLineIndex,
         endIt = mBidirectionalLineInfo.End();
       it != endIt;
       ++it, ++bidiLineIndex )
  {
    const BidirectionalLineInfoRun& bidiLineRun = *it;

    if( ( lastCharacterOfRightToLeftRun < characterIndex ) && ( characterIndex < bidiLineRun.characterRun.characterIndex ) )
    {
      // The character is not inside a bidi line.
      return false;
    }

    const CharacterIndex lastCharacterOfRunPlusOne = bidiLineRun.characterRun.characterIndex + bidiLineRun.characterRun.numberOfCharacters;
    lastCharacterOfRightToLeftRun = lastCharacterOfRunPlusOne - 1u;
    if( ( bidiLineRun.characterRun.characterIndex <= characterIndex ) &&
        ( characterIndex < lastCharacterOfRunPlusOne ) )
    {
      // Bidi line found. Fetch the line.
      mBidirectionalLineIndex = bidiLineIndex;
      return true;
    }
  }

  return false;
}

BidirectionalLineRunIndex LogicalModel::GetBidirectionalLineInfo() const
{
  return mBidirectionalLineIndex;
}

void LogicalModel::UpdateTextStyleRuns( CharacterIndex index, int numberOfCharacters )
{
  const Length totalNumberOfCharacters = mText.Count();

  // Process the color runs.
  Vector<ColorRun> removedColorRuns;
  UpdateCharacterRuns<ColorRun>( index,
                                 numberOfCharacters,
                                 totalNumberOfCharacters,
                                 mColorRuns,
                                 removedColorRuns );

  // Process the font description runs.
  Vector<FontDescriptionRun> removedFontDescriptionRuns;
  UpdateCharacterRuns<FontDescriptionRun>( index,
                                           numberOfCharacters,
                                           totalNumberOfCharacters,
                                           mFontDescriptionRuns,
                                           removedFontDescriptionRuns );

  // Free memory allocated for the font family name.
  FreeFontFamilyNames( removedFontDescriptionRuns );
}

void LogicalModel::RetrieveStyle( CharacterIndex index, InputStyle& style )
{
  unsigned int runIndex = 0u;

  // Set the text color.
  bool colorOverriden = false;
  unsigned int colorIndex = 0u;
  const ColorRun* const colorRunsBuffer = mColorRuns.Begin();
  for( Vector<ColorRun>::ConstIterator it = colorRunsBuffer,
         endIt = mColorRuns.End();
       it != endIt;
       ++it, ++runIndex )
  {
    const ColorRun& colorRun = *it;

    if( ( colorRun.characterRun.characterIndex <= index ) &&
        ( index < colorRun.characterRun.characterIndex + colorRun.characterRun.numberOfCharacters ) )
    {
      colorIndex = runIndex;
      colorOverriden = true;
    }
  }

  // Set the text's color if it's overriden.
  if( colorOverriden )
  {
    style.textColor = ( *( colorRunsBuffer + colorIndex ) ).color;
    style.isDefaultColor = false;
  }

  // Reset the run index.
  runIndex = 0u;

  // Set the font's parameters.
  bool nameOverriden = false;
  bool weightOverriden = false;
  bool widthOverriden = false;
  bool slantOverriden = false;
  bool sizeOverriden = false;
  unsigned int nameIndex = 0u;
  unsigned int weightIndex = 0u;
  unsigned int widthIndex = 0u;
  unsigned int slantIndex = 0u;
  unsigned int sizeIndex = 0u;
  const FontDescriptionRun* const fontDescriptionRunsBuffer = mFontDescriptionRuns.Begin();
  for( Vector<FontDescriptionRun>::ConstIterator it = fontDescriptionRunsBuffer,
         endIt = mFontDescriptionRuns.End();
       it != endIt;
       ++it, ++runIndex )
  {
    const FontDescriptionRun& fontDescriptionRun = *it;

    if( ( fontDescriptionRun.characterRun.characterIndex <= index ) &&
        ( index < fontDescriptionRun.characterRun.characterIndex + fontDescriptionRun.characterRun.numberOfCharacters ) )
    {
      if( fontDescriptionRun.familyDefined )
      {
        nameIndex = runIndex;
        nameOverriden = true;
      }

      if( fontDescriptionRun.weightDefined )
      {
        weightIndex = runIndex;
        weightOverriden = true;
      }

      if( fontDescriptionRun.widthDefined )
      {
        widthIndex = runIndex;
        widthOverriden = true;
      }

      if( fontDescriptionRun.slantDefined )
      {
        slantIndex = runIndex;
        slantOverriden = true;
      }

      if( fontDescriptionRun.sizeDefined )
      {
        sizeIndex = runIndex;
        sizeOverriden = true;
      }
    }
  }

  // Set the font's family name if it's overriden.
  if( nameOverriden )
  {
    const FontDescriptionRun& fontDescriptionRun = *( fontDescriptionRunsBuffer + nameIndex );

    style.familyName = std::string( fontDescriptionRun.familyName, fontDescriptionRun.familyLength );
    style.familyDefined = true;
  }

  // Set the font's weight if it's overriden.
  if( weightOverriden )
  {
    const FontDescriptionRun& fontDescriptionRun = *( fontDescriptionRunsBuffer + weightIndex );

    style.weight = fontDescriptionRun.weight;
    style.weightDefined = true;
  }

  // Set the font's width if it's overriden.
  if( widthOverriden )
  {
    const FontDescriptionRun& fontDescriptionRun = *( fontDescriptionRunsBuffer + widthIndex );

    style.width = fontDescriptionRun.width;
    style.widthDefined = true;
  }

  // Set the font's slant if it's overriden.
  if( slantOverriden )
  {
    const FontDescriptionRun& fontDescriptionRun = *( fontDescriptionRunsBuffer + slantIndex );

    style.slant = fontDescriptionRun.slant;
    style.slantDefined = true;
  }

  // Set the font's size if it's overriden.
  if( sizeOverriden )
  {
    const FontDescriptionRun& fontDescriptionRun = *( fontDescriptionRunsBuffer + sizeIndex );

    style.size = static_cast<float>( fontDescriptionRun.size ) / 64.f;
    style.sizeDefined = true;
  }
}

void LogicalModel::ClearFontDescriptionRuns()
{
  FreeFontFamilyNames( mFontDescriptionRuns );
}

void LogicalModel::CreateParagraphInfo( CharacterIndex startIndex,
                                        Length numberOfCharacters )
{
  const Length totalNumberOfCharacters = mLineBreakInfo.Count();

  // Count the number of LINE_MUST_BREAK to reserve some space for the vector of paragraph's info.
  Vector<CharacterIndex> paragraphs;
  paragraphs.Reserve( numberOfCharacters );
  const TextAbstraction::LineBreakInfo* lineBreakInfoBuffer = mLineBreakInfo.Begin();
  const CharacterIndex lastCharacterIndexPlusOne = startIndex + numberOfCharacters;
  for( Length index = startIndex; index < lastCharacterIndexPlusOne; ++index )
  {
    if( TextAbstraction::LINE_MUST_BREAK == *( lineBreakInfoBuffer + index ) )
    {
      paragraphs.PushBack( index );
    }
  }

  // Whether the current paragraphs are updated or set from scratch.
  const bool updateCurrentParagraphs = numberOfCharacters < totalNumberOfCharacters;

  // Reserve space for current paragraphs plus new ones.
  const Length numberOfNewParagraphs = paragraphs.Count();
  const Length totalNumberOfParagraphs = mParagraphInfo.Count() + numberOfNewParagraphs;
  mParagraphInfo.Resize( totalNumberOfParagraphs );

  ParagraphRun* paragraphInfoBuffer = NULL;
  Vector<ParagraphRun> newParagraphs;

  if( updateCurrentParagraphs )
  {
    newParagraphs.Resize( numberOfNewParagraphs );
    paragraphInfoBuffer = newParagraphs.Begin();
  }
  else
  {
    paragraphInfoBuffer = mParagraphInfo.Begin();
  }

  // Find where to insert the new paragraphs.
  ParagraphRunIndex paragraphIndex = 0u;
  CharacterIndex firstIndex = startIndex;

  if( updateCurrentParagraphs )
  {
    for( Vector<ParagraphRun>::ConstIterator it = mParagraphInfo.Begin(),
           endIt = mParagraphInfo.Begin() + totalNumberOfParagraphs - numberOfNewParagraphs;
         it != endIt;
         ++it )
    {
      const ParagraphRun& paragraph( *it );

      if( startIndex < paragraph.characterRun.characterIndex + paragraph.characterRun.numberOfCharacters )
      {
        firstIndex = paragraph.characterRun.characterIndex;
        break;
      }

      ++paragraphIndex;
    }
  }

  // Create the paragraph info.
  ParagraphRunIndex newParagraphIndex = 0u;
  for( Vector<CharacterIndex>::ConstIterator it = paragraphs.Begin(),
         endIt = paragraphs.End();
       it != endIt;
       ++it, ++newParagraphIndex )
  {
    const CharacterIndex index = *it;

    ParagraphRun& paragraph = *( paragraphInfoBuffer + newParagraphIndex );
    paragraph.characterRun.characterIndex = firstIndex;
    paragraph.characterRun.numberOfCharacters = 1u + index - firstIndex;

    firstIndex += paragraph.characterRun.numberOfCharacters;
  }


  // Insert the new paragraphs.
  if( updateCurrentParagraphs )
  {
    mParagraphInfo.Insert( mParagraphInfo.Begin() + paragraphIndex,
                           newParagraphs.Begin(),
                           newParagraphs.End() );

    mParagraphInfo.Resize( totalNumberOfParagraphs );

    // Update the next paragraph indices.
    for( Vector<ParagraphRun>::Iterator it = mParagraphInfo.Begin() + paragraphIndex + newParagraphs.Count(),
           endIt = mParagraphInfo.End();
         it != endIt;
         ++it )
    {
      ParagraphRun& paragraph( *it );

      paragraph.characterRun.characterIndex += numberOfCharacters;
    }
  }
}

void LogicalModel::FindParagraphs( CharacterIndex index,
                                   Length numberOfCharacters,
                                   Vector<ParagraphRunIndex>& paragraphs )
{
  // Reserve som space for the paragraph indices.
  paragraphs.Reserve( mParagraphInfo.Count() );

  // Traverse the paragraphs to find which ones contain the given characters.
  ParagraphRunIndex paragraphIndex = 0u;
  for( Vector<ParagraphRun>::ConstIterator it = mParagraphInfo.Begin(),
         endIt = mParagraphInfo.End();
       it != endIt;
       ++it, ++paragraphIndex )
  {
    const ParagraphRun& paragraph( *it );

    if( ( paragraph.characterRun.characterIndex + paragraph.characterRun.numberOfCharacters > index ) &&
        ( paragraph.characterRun.characterIndex < index + numberOfCharacters ) )
    {
      paragraphs.PushBack( paragraphIndex );
    }
  }
}

LogicalModel::~LogicalModel()
{
  ClearFontDescriptionRuns();
}

LogicalModel::LogicalModel()
: mBidirectionalLineIndex( 0u )
{
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
