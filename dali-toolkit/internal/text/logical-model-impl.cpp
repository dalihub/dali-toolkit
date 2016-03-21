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

void LogicalModel::SetVisualToLogicalMap( const BidirectionalLineInfoRun* const bidirectionalInfo,
                                          Length numberOfRuns,
                                          CharacterIndex startIndex,
                                          Length numberOfCharacters )
{
  mVisualToLogicalMap.Resize( numberOfCharacters );
  mLogicalToVisualMap.Resize( numberOfCharacters );

  const Length numberOfCharactersPlus = numberOfCharacters + 1u;
  mVisualToLogicalCursorMap.Resize( numberOfCharactersPlus );

  CharacterIndex* modelVisualToLogicalMapBuffer = mVisualToLogicalMap.Begin();
  CharacterIndex* modelLogicalToVisualMapBuffer = mLogicalToVisualMap.Begin();

  CharacterIndex* modelVisualToLogicalCursorMap = mVisualToLogicalCursorMap.Begin();

  CharacterIndex lastIndex = startIndex;
  for( unsigned int bidiIndex = 0u; bidiIndex < numberOfRuns; ++bidiIndex )
  {
    const BidirectionalLineInfoRun& bidiLineInfo = *( bidirectionalInfo + bidiIndex );

    if( bidiLineInfo.characterRun.characterIndex + bidiLineInfo.characterRun.numberOfCharacters <= startIndex )
    {
      // Skip this paragraph. It has been already processed.
      continue;
    }

    if( lastIndex < bidiLineInfo.characterRun.characterIndex )
    {
      // Fill with the identity.
      for( ; lastIndex < bidiLineInfo.characterRun.characterIndex; ++lastIndex )
      {
        *( modelVisualToLogicalMapBuffer + lastIndex ) = lastIndex;
      }
    }

    // Fill the conversion table of the run.
    for( CharacterIndex index = 0u;
         index < bidiLineInfo.characterRun.numberOfCharacters;
         ++index, ++lastIndex )
    {
      *( modelVisualToLogicalMapBuffer + lastIndex ) = bidiLineInfo.characterRun.characterIndex + *( bidiLineInfo.visualToLogicalMap + index );
    }
  }

  // Complete with the identity if there are some left to right characters after the last right to left.
  for( ; lastIndex < numberOfCharacters; ++lastIndex )
  {
    *( modelVisualToLogicalMapBuffer + lastIndex ) = lastIndex;
  }

  // Sets the logical to visual conversion map.
  for( CharacterIndex index = startIndex; index < numberOfCharacters; ++index )
  {
    *( modelLogicalToVisualMapBuffer + *( modelVisualToLogicalMapBuffer + index ) ) = index;
  }

  // Sets the visual to logical conversion map for cursor positions.

  const Length numberOfBidirectionalParagraphs = mBidirectionalParagraphInfo.Count();
  BidirectionalParagraphInfoRun* bidirectionalParagraphInfoBuffer = mBidirectionalParagraphInfo.Begin();
  BidirectionalParagraphInfoRun* bidirectionalParagraph = bidirectionalParagraphInfoBuffer;

  const CharacterDirection* const modelCharacterDirections = mCharacterDirections.Begin();

  Length bidirectionalParagraphIndex = 0u;
  bool isRightToLeftParagraph = false;
  for( CharacterIndex index = startIndex; index < numberOfCharactersPlus; ++index )
  {
    if( bidirectionalParagraph &&
        ( bidirectionalParagraph->characterRun.characterIndex == index ) )
    {
      isRightToLeftParagraph = *( modelCharacterDirections + index );
    }

    if( 0u == index )
    {
      if( isRightToLeftParagraph )
      {
        *( modelVisualToLogicalCursorMap + index ) = numberOfCharacters;
      }
      else // else logical position is zero.
      {
        *( modelVisualToLogicalCursorMap + index ) = 0u;
      }
    }
    else if( numberOfCharacters == index )
    {
      if( isRightToLeftParagraph )
      {
        *( modelVisualToLogicalCursorMap + index ) = 0u;
      }
      else // else logical position is the number of characters.
      {
        *( modelVisualToLogicalCursorMap + index ) = numberOfCharacters;
      }
    }
    else
    {
      // Get the character indexed by  index - 1 and index
      // and calculate the logical position according the directions of
      // both characters and the direction of the paragraph.

      const CharacterIndex previousIndex = index - 1u;
      const CharacterIndex logicalPosition0 = *( modelVisualToLogicalMapBuffer + previousIndex );
      const CharacterIndex logicalPosition1 = *( modelVisualToLogicalMapBuffer + index );

      const CharacterDirection direction0 = *( modelCharacterDirections + logicalPosition0 );
      const CharacterDirection direction1 = *( modelCharacterDirections + logicalPosition1 );

      if( direction0 == direction1 )
      {
        // Both glyphs have the same direction.
        if( direction0 )
        {
          *( modelVisualToLogicalCursorMap + index ) = logicalPosition0;
        }
        else
        {
          *( modelVisualToLogicalCursorMap + index ) = logicalPosition1;
        }
      }
      else
      {
        if( isRightToLeftParagraph )
        {
          if( direction1 )
          {
            *( modelVisualToLogicalCursorMap + index ) = logicalPosition1 + 1u;
          }
          else
          {
            *( modelVisualToLogicalCursorMap + index ) = logicalPosition0;
          }
        }
        else
        {
          if( direction0 )
          {
            *( modelVisualToLogicalCursorMap + index ) = logicalPosition1;
          }
          else
          {
            *( modelVisualToLogicalCursorMap + index ) = logicalPosition0 + 1u;
          }
        }
      }
    }

    if( bidirectionalParagraph &&
        ( bidirectionalParagraph->characterRun.characterIndex + bidirectionalParagraph->characterRun.numberOfCharacters == index ) )
    {
      isRightToLeftParagraph = false;
      ++bidirectionalParagraphIndex;
      if( bidirectionalParagraphIndex < numberOfBidirectionalParagraphs )
      {
        bidirectionalParagraph = bidirectionalParagraphInfoBuffer + bidirectionalParagraphIndex;
      }
      else
      {
        bidirectionalParagraph = NULL;
      }
    }
  }
}

CharacterIndex LogicalModel::GetLogicalCharacterIndex( CharacterIndex visualCharacterIndex ) const
{
  if( 0u == mVisualToLogicalMap.Count() )
  {
    // If there is no visual to logical info is because the whole text is left to right.
    // Return the identity.
    return visualCharacterIndex;
  }

  return *( mVisualToLogicalMap.Begin() + visualCharacterIndex );
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

  // Reset the run index.
  runIndex = 0u;
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
{
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
