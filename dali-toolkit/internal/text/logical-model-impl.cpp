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
#include <dali-toolkit/internal/text/text-style-run-container.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

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
                                          Length numberOfRuns )
{
  if( 0u == numberOfRuns )
  {
    mVisualToLogicalMap.Clear();
    mLogicalToVisualMap.Clear();
    mVisualToLogicalCursorMap.Clear();
  }
  else
  {
    const Length numberOfCharacters = mText.Count();
    mVisualToLogicalMap.Resize( numberOfCharacters );
    mLogicalToVisualMap.Resize( numberOfCharacters );

    const Length numberOfCharactersPlus = numberOfCharacters + 1u;
    mVisualToLogicalCursorMap.Resize( numberOfCharactersPlus );

    CharacterIndex* modelVisualToLogicalMapBuffer = mVisualToLogicalMap.Begin();
    CharacterIndex* modelLogicalToVisualMapBuffer = mLogicalToVisualMap.Begin();

    CharacterIndex* modelVisualToLogicalCursorMap = mVisualToLogicalCursorMap.Begin();

    CharacterIndex lastIndex = 0u;
    for( unsigned int bidiIndex = 0u; bidiIndex < numberOfRuns; ++bidiIndex )
    {
      const BidirectionalLineInfoRun& bidiLineInfo = *( bidirectionalInfo + bidiIndex );

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
    for( CharacterIndex index = 0u; index < numberOfCharacters; ++index )
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
    for( CharacterIndex index = 0u; index < numberOfCharactersPlus; ++index )
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
}

void LogicalModel::RetrieveStyle( CharacterIndex index, InputStyle& style )
{
  unsigned int runIndex = 0u;
  unsigned int lastRunIndex = 0u;
  bool overriden = false;

  // Set the text color.
  for( Vector<ColorRun>::ConstIterator it = mColorRuns.Begin(),
         endIt = mColorRuns.End();
       it != endIt;
       ++it, ++runIndex )
  {
    const ColorRun& colorRun = *it;

    if( ( colorRun.characterRun.characterIndex <= index ) &&
        ( index < colorRun.characterRun.characterIndex + colorRun.characterRun.numberOfCharacters ) )
    {
      lastRunIndex = runIndex;
      overriden = true;
    }
  }

  // Set the text's color if it's overriden.
  if( overriden )
  {
    style.textColor = ( *( mColorRuns.Begin() + lastRunIndex ) ).color;
  }
}

LogicalModel::~LogicalModel()
{
}

LogicalModel::LogicalModel()
{
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
