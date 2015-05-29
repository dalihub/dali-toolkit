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

// EXTERNAL INCLUDES
#include <memory.h>

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

void LogicalModel::SetText( const Character* const text,
                            Length numberOfCharacters )
{
  if( 0u == numberOfCharacters )
  {
    mText.Clear();
  }
  else
  {
    mText.Resize( numberOfCharacters );
    memcpy( mText.Begin(), text, numberOfCharacters * sizeof( Character ) );
  }
}

Length LogicalModel::GetNumberOfCharacters() const
{
  return mText.Count();
}

void LogicalModel::GetText( Character* text,
                            CharacterIndex characterIndex,
                            Length numberOfCharacters ) const
{
  memcpy( text, mText.Begin() + characterIndex, numberOfCharacters * sizeof( Character ) );
}

Character LogicalModel::GetCharacter( CharacterIndex characterIndex ) const
{
  return mText[characterIndex];
}

void LogicalModel::ReplaceText( CharacterIndex characterIndex,
                                Length numberOfCharactersToRemove,
                                const Character* const text,
                                Length numberOfCharactersToInsert )
{
}

void LogicalModel::SetScripts( const ScriptRun* const scripts,
                               Length numberOfRuns )
{
  if( 0u == numberOfRuns )
  {
    mScriptRuns.Clear();
  }
  else
  {
    mScriptRuns.Resize( numberOfRuns );
    memcpy( mScriptRuns.Begin(), scripts, numberOfRuns * sizeof( ScriptRun ) );
  }
}

void LogicalModel::GetNumberOfScriptRuns( CharacterIndex characterIndex,
                                          Length numberOfCharacters,
                                          ScriptRunIndex& firstScriptRun,
                                          Length& numberOfScriptRuns ) const
{
  // Initialize the number of scripts and the index to the first script.
  firstScriptRun = 0u;
  numberOfScriptRuns = 0;
  bool firstScriptFound = false;

  const CharacterIndex lastCharacterIndex = characterIndex + numberOfCharacters;

  // Traverse the scripts and count those scripts within the range of characters.
  for( Vector<ScriptRun>::ConstIterator it = mScriptRuns.Begin(),
         endIt = mScriptRuns.End();
       it != endIt;
       ++it )
  {
    const ScriptRun& script = *it;

    if( ( script.characterRun.characterIndex + script.characterRun.numberOfCharacters > characterIndex ) &&
        ( lastCharacterIndex > script.characterRun.characterIndex ) )
    {
      firstScriptFound = true;
      ++numberOfScriptRuns;
    }
    else if( lastCharacterIndex <= script.characterRun.characterIndex )
    {
      // nothing else to do.
      break;
    }

    if( !firstScriptFound )
    {
      ++firstScriptRun;
    }
  }
}

void LogicalModel::GetScriptRuns( ScriptRun* scriptRuns,
                                  CharacterIndex characterIndex,
                                  Length numberOfCharacters ) const
{
  ScriptRunIndex firstScriptRun = 0u;
  Length numberOfScriptRuns = 0u;

  GetNumberOfScriptRuns( characterIndex,
                         numberOfCharacters,
                         firstScriptRun,
                         numberOfScriptRuns );

  memcpy( scriptRuns, mScriptRuns.Begin() + firstScriptRun, numberOfScriptRuns * sizeof( ScriptRun ) );
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

void LogicalModel::ReplaceScripts( CharacterIndex characterIndex,
                                   Length numberOfCharactersToRemove,
                                   const ScriptRun* const scriptRuns,
                                   Length numberOfCharactersToInsert )
{
}

void LogicalModel::SetFonts( const FontRun* const fonts,
                             Length numberOfRuns )
{
  if( 0u == numberOfRuns )
  {
    mFontRuns.Clear();
  }
  else
  {
    mFontRuns.Resize( numberOfRuns );
    memcpy( mFontRuns.Begin(), fonts, numberOfRuns * sizeof( FontRun ) );
  }
}

void LogicalModel::GetNumberOfFontRuns( CharacterIndex characterIndex,
                                        Length numberOfCharacters,
                                        FontRunIndex& firstFontRun,
                                        Length& numberOfFontRuns ) const
{
  // Initialize the number of fonts and the index to the first font.
  firstFontRun = 0u;
  numberOfFontRuns = 0;
  bool firstFontFound = false;

  const CharacterIndex lastCharacterIndex = characterIndex + numberOfCharacters;

  // Traverse the fonts and count those fonts within the range of characters.
  for( Vector<FontRun>::ConstIterator it = mFontRuns.Begin(),
         endIt = mFontRuns.End();
       it != endIt;
       ++it )
  {
    const FontRun& font = *it;

    if( ( font.characterRun.characterIndex + font.characterRun.numberOfCharacters > characterIndex ) &&
        ( characterIndex + numberOfCharacters > font.characterRun.characterIndex ) )
    {
      firstFontFound = true;
      ++numberOfFontRuns;
    }
    else if( lastCharacterIndex <= font.characterRun.characterIndex )
    {
      // nothing else to do.
      break;
    }

    if( !firstFontFound )
    {
      ++firstFontRun;
    }
  }
}

void LogicalModel::GetFontRuns( FontRun* fontRuns,
                                CharacterIndex characterIndex,
                                Length numberOfCharacters ) const
{
  FontRunIndex firstFontRun = 0u;
  Length numberOfFontRuns = 0u;

  GetNumberOfFontRuns( characterIndex,
                       numberOfCharacters,
                       firstFontRun,
                       numberOfFontRuns );

  memcpy( fontRuns, mFontRuns.Begin() + firstFontRun, numberOfFontRuns * sizeof( FontRun ) );
}

FontId LogicalModel::GetFont( CharacterIndex characterIndex ) const
{
  for( Length index = 0u, length = mFontRuns.Count(); index < length; ++index )
  {
    const FontRun* const fontRun = mFontRuns.Begin() + index;

    if( ( fontRun->characterRun.characterIndex <= characterIndex ) &&
        ( characterIndex < fontRun->characterRun.characterIndex + fontRun->characterRun.numberOfCharacters ) )
    {
      return fontRun->fontId;
    }
  }

  return 0u;
}

void LogicalModel::ReplaceFonts( CharacterIndex characterIndex,
                                 Length numberOfCharactersToRemove,
                                 const FontRun* const fontRuns,
                                 Length numberOfCharactersToInsert )
{
}

void LogicalModel::SetLineBreakInfo( const LineBreakInfo* const lineBreakInfo,
                                     Length length )
{
  if( 0u == length )
  {
    mLineBreakInfo.Clear();
  }
  else
  {
    mLineBreakInfo.Resize( length );
    memcpy( mLineBreakInfo.Begin(), lineBreakInfo, length * sizeof( LineBreakInfo ) );
  }
}

void LogicalModel::GetLineBreakInfo( LineBreakInfo* lineBreakInfo,
                                     CharacterIndex characterIndex,
                                     Length numberOfItems ) const
{
  memcpy( lineBreakInfo, mLineBreakInfo.Begin() + characterIndex, numberOfItems * sizeof( LineBreakInfo ) );
}

LineBreakInfo LogicalModel::GetLineBreakInfo( CharacterIndex characterIndex ) const
{
  return *( mLineBreakInfo.Begin() + characterIndex );
}

void LogicalModel::ReplaceLineBreakInfo( CharacterIndex characterIndex,
                                         Length numberOfItemsToRemove,
                                         const LineBreakInfo* const lineBreakInfo,
                                         Length numberOfItemsToInsert )
{
}

void LogicalModel::SetWordBreakInfo( const WordBreakInfo* const wordBreakInfo,
                                     Length length )
{
  if( 0u == length )
  {
    mWordBreakInfo.Clear();
  }
  else
  {
    mWordBreakInfo.Resize( length );
    memcpy( mWordBreakInfo.Begin(), wordBreakInfo, length * sizeof( WordBreakInfo ) );
  }
}

void LogicalModel::GetWordBreakInfo( WordBreakInfo* wordBreakInfo,
                                     CharacterIndex characterIndex,
                                     Length numberOfItems ) const
{
  memcpy( wordBreakInfo, mWordBreakInfo.Begin() + characterIndex, numberOfItems * sizeof( WordBreakInfo ) );
}

WordBreakInfo LogicalModel::GetWordBreakInfo( CharacterIndex characterIndex ) const
{
  return *( mWordBreakInfo.Begin() + characterIndex );
}

void LogicalModel::ReplaceWordBreakInfo( CharacterIndex characterIndex,
                                         Length numberOfItemsToRemove,
                                         const WordBreakInfo* const wordBreakInfo,
                                         Length numberOfItemsToInsert )
{
}

void LogicalModel::SetBidirectionalInfo( const BidirectionalParagraphInfoRun* const bidirectionalInfo,
                                         Length numberOfRuns )
{
  if( 0u == numberOfRuns )
  {
    mBidirectionalParagraphInfo.Clear();
  }
  else
  {
    mBidirectionalParagraphInfo.Resize( numberOfRuns );
    memcpy( mBidirectionalParagraphInfo.Begin(), bidirectionalInfo, numberOfRuns * sizeof( BidirectionalParagraphInfoRun ) );
  }
}

void LogicalModel::GetNumberOfBidirectionalInfoRuns( CharacterIndex characterIndex,
                                                     Length numberOfCharacters,
                                                     BidirectionalRunIndex& firstBidirectionalRun,
                                                     Length& numberOfFontRuns  ) const
{
  // Initialize the number of bidi paragraphs and the index to the first paragraph.
  firstBidirectionalRun = 0u;
  numberOfFontRuns = 0;
  bool firstParagraphFound = false;

  // Traverse the bidirectional paragraph info and count those bidi paragraphs within the range of characters.
  for( Vector<BidirectionalParagraphInfoRun>::ConstIterator it = mBidirectionalParagraphInfo.Begin(),
         endIt = mBidirectionalParagraphInfo.End();
       it != endIt;
       ++it )
  {
    const BidirectionalParagraphInfoRun& bidi = *it;

    if( ( bidi.characterRun.characterIndex + bidi.characterRun.numberOfCharacters > characterIndex ) &&
        ( characterIndex + numberOfCharacters > bidi.characterRun.characterIndex ) )
    {
      firstParagraphFound = true;
      ++numberOfFontRuns;
    }

    if( !firstParagraphFound )
    {
      ++firstBidirectionalRun;
    }
  }
}

void LogicalModel::GetBidirectionalInfo( BidirectionalParagraphInfoRun* bidirectionalInfo,
                                         CharacterIndex characterIndex,
                                         Length numberOfCharacters ) const
{
  BidirectionalRunIndex firstBidirectionalRun = 0u;
  Length numberOfFontRuns = 0u;

  GetNumberOfBidirectionalInfoRuns( characterIndex,
                                    numberOfCharacters,
                                    firstBidirectionalRun,
                                    numberOfFontRuns );

  memcpy( bidirectionalInfo, mBidirectionalParagraphInfo.Begin() + firstBidirectionalRun, numberOfFontRuns * sizeof( BidirectionalParagraphInfoRun ) );
}

void ReplaceBidirectionalInfo( CharacterIndex characterIndex,
                               Length numberOfCharactersToRemove,
                               const BidirectionalParagraphInfoRun* const bidirectionalInfo,
                               Length numberOfCharactersToInsert )
{
}

void LogicalModel::SetCharacterDirections( const CharacterDirection* const directions,
                                           Length numberOfCharacters )
{
  if( 0u == numberOfCharacters )
  {
    mCharacterDirections.Clear();
  }
  else
  {
    mCharacterDirections.Resize( numberOfCharacters );
    memcpy( mCharacterDirections.Begin(), directions, numberOfCharacters * sizeof( CharacterDirection ) );
  }
}

void LogicalModel::GetCharacterDirections( CharacterDirection* directions,
                                           CharacterIndex characterIndex,
                                           Length numberOfCharacters ) const
{
  if( 0u == mCharacterDirections.Count() )
  {
    // Nothing to retrieve if the model has no right to left characters.
    return;
  }

  memcpy( directions, mCharacterDirections.Begin() + characterIndex, numberOfCharacters * sizeof( CharacterDirection ) );
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

void LogicalModel::ReplaceVisualToLogicalMap( CharacterIndex characterIndex,
                                              Length numberOfCharactersToRemove,
                                              const BidirectionalLineInfoRun* const bidirectionalInfo,
                                              Length numberOfCharactersToInsert )
{
}

CharacterIndex LogicalModel::GetVisualCharacterIndex( CharacterIndex logicalCharacterIndex ) const
{
  if( 0u == mLogicalToVisualMap.Count() )
  {
    // If there is no logical to visual info is because the whole text is left to right.
    // Return the identity.
    return logicalCharacterIndex;
  }

  return *( mLogicalToVisualMap.Begin() + logicalCharacterIndex );
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

void LogicalModel::GetLogicalToVisualMap( CharacterIndex* logicalToVisualMap,
                                          CharacterIndex characterIndex,
                                          Length numberOfCharacters ) const
{
  memcpy( logicalToVisualMap, mLogicalToVisualMap.Begin() + characterIndex, numberOfCharacters * sizeof( CharacterIndex ) );
}

void LogicalModel::GetVisualToLogicalMap( CharacterIndex* visualToLogicalMap,
                                          CharacterIndex characterIndex,
                                          Length numberOfCharacters ) const
{
  memcpy( visualToLogicalMap, mVisualToLogicalMap.Begin() + characterIndex, numberOfCharacters * sizeof( CharacterIndex ) );
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
