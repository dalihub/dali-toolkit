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
#include <dali-toolkit/internal/text/logical-model.h>

// EXTERNAL INCLUDES
#include <memory.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bidirectional-line-info-run.h>
#include <dali-toolkit/internal/text/bidirectional-paragraph-info-run.h>
#include <dali-toolkit/internal/text/font-run.h>
#include <dali-toolkit/internal/text/script-run.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{
/**
 * @brief caches some temporary values of the GetNumberOfScriptRuns( characterIndex, numberOfCharacters )
 * operation and the GetNumberOfFontRuns( characterIndex, numberOfCharacters ) as they are going to be
 * used in the GetScriptRuns() and the GetFontRuns() calls.
 */
struct GetRunCache
{
  CharacterIndex characterIndex;     ///< The character index.
  Length         numberOfCharacters; ///< The number of characters.
  Length         firstRun;           ///< Index to the first run.
  Length         numberOfRuns;       ///< The number of runs.
};

struct LogicalModel::Impl
{
  Vector<Character>                     mText;
  Vector<ScriptRun>                     mScriptRuns;
  Vector<FontRun>                       mFontRuns;
  Vector<LineBreakInfo>                 mLineBreakInfo;
  Vector<WordBreakInfo>                 mWordBreakInfo;
  Vector<BidirectionalParagraphInfoRun> mBidirectionalParagraphInfo;

  Vector<BidirectionalLineInfoRun>      mBidirectionalLineInfo;
  Vector<CharacterIndex>                mLogicalToVisualMap; ///< Bidirectional logical to visual conversion table.
  Vector<CharacterIndex>                mVisualToLogicalMap; ///< Bidirectional visual to logical conversion table.

  GetRunCache                           mGetScriptCache;             ///< Caches the GetNumberOfScriptRuns( characterIndex, numberOfCharacters ) operation.
  GetRunCache                           mGetFontCache;               ///< Caches the GetNumberOfFontRuns( characterIndex, numberOfCharacters ) operation.
  GetRunCache                           mGetBidirectionalCache;      ///< Caches the GetNumberOfBidirectionalInfoRuns( characterIndex, numberOfCharacters ) operation.
};

LogicalModelPtr LogicalModel::New()
{
  return LogicalModelPtr( new LogicalModel() );
}

void LogicalModel::SetText( const Character* const text,
                            Length numberOfCharacters )
{
  Vector<Character>& modelText = mImpl->mText;

  if( 0u == numberOfCharacters )
  {
    modelText.Clear();
  }
  else
  {
    modelText.Resize( numberOfCharacters );
    memcpy( modelText.Begin(), text, numberOfCharacters * sizeof( Character ) );
  }
}

Length LogicalModel::GetNumberOfCharacters() const
{
  return mImpl->mText.Count();
}

void LogicalModel::GetText( Character* text,
                            CharacterIndex characterIndex,
                            Length numberOfCharacters ) const
{
  Vector<Character>& modelText = mImpl->mText;
  memcpy( text, modelText.Begin() + characterIndex, numberOfCharacters * sizeof( Character ) );
}

Character LogicalModel::GetCharacter( CharacterIndex characterIndex ) const
{
  return mImpl->mText[characterIndex];
}

void LogicalModel::SetScripts( const ScriptRun* const scripts,
                               Length numberOfRuns )
{
  Vector<ScriptRun>& scriptRuns = mImpl->mScriptRuns;

  if( 0u == numberOfRuns )
  {
    scriptRuns.Clear();
  }
  else
  {
    scriptRuns.Resize( numberOfRuns );
    memcpy( scriptRuns.Begin(), scripts, numberOfRuns * sizeof( ScriptRun ) );
  }

  mImpl->mGetScriptCache.characterIndex = 0u;
  mImpl->mGetScriptCache.numberOfCharacters = 0u;
  mImpl->mGetScriptCache.firstRun = 0u;
  mImpl->mGetScriptCache.numberOfRuns = 0u;
}

Length LogicalModel::GetNumberOfScriptRuns( CharacterIndex characterIndex,
                                            Length numberOfCharacters ) const
{
  GetRunCache& scriptCache = mImpl->mGetScriptCache;

  // Set the character index and the number of characters into the cache.
  scriptCache.characterIndex = characterIndex;
  scriptCache.numberOfCharacters = numberOfCharacters;

  if( ( 0u == characterIndex ) &&
      ( mImpl->mText.Count() == numberOfCharacters ) )
  {
    scriptCache.firstRun = 0u;
    scriptCache.numberOfRuns = mImpl->mScriptRuns.Count();
    return scriptCache.numberOfRuns;
  }

  // Initialize the number of scripts and the index to the first script.
  scriptCache.firstRun = 0u;
  scriptCache.numberOfRuns = 0;
  bool firstScriptFound = false;

  const Vector<ScriptRun>& modelScripts = mImpl->mScriptRuns;
  const CharacterIndex lastCharacterIndex = characterIndex + numberOfCharacters;

  // Traverse the scripts and count those scripts within the range of characters.
  for( Vector<ScriptRun>::ConstIterator it = modelScripts.Begin(),
         endIt = modelScripts.End();
       it != endIt;
       ++it )
  {
    const ScriptRun& script = *it;

    if( ( script.characterRun.characterIndex + script.characterRun.numberOfCharacters > characterIndex ) &&
        ( lastCharacterIndex > script.characterRun.characterIndex ) )
    {
      firstScriptFound = true;
      ++scriptCache.numberOfRuns;
    }
    else if( lastCharacterIndex <= script.characterRun.characterIndex )
    {
      // nothing else to do.
      break;
    }

    if( !firstScriptFound )
    {
      ++scriptCache.firstRun;
    }
  }

  return scriptCache.numberOfRuns;
}

void LogicalModel::GetScriptRuns( ScriptRun* scriptRuns,
                                  CharacterIndex characterIndex,
                                  Length numberOfCharacters ) const
{
  const Vector<ScriptRun>& modelScripts = mImpl->mScriptRuns;
  GetRunCache& scriptCache = mImpl->mGetScriptCache;

  if( ( characterIndex != scriptCache.characterIndex ) ||
      ( numberOfCharacters != scriptCache.numberOfCharacters ) )
  {
    GetNumberOfScriptRuns( characterIndex,
                           numberOfCharacters );
  }

  memcpy( scriptRuns, modelScripts.Begin() + scriptCache.firstRun, scriptCache.numberOfRuns * sizeof( ScriptRun ) );
}

Script LogicalModel::GetScript( CharacterIndex characterIndex ) const
{
  // If this operation is too slow, consider a binary search.

  for( Length index = 0u, length = mImpl->mScriptRuns.Count(); index < length; ++index )
  {
    const ScriptRun* const scriptRun = mImpl->mScriptRuns.Begin() + index;

    if( ( scriptRun->characterRun.characterIndex <= characterIndex ) &&
        ( characterIndex < scriptRun->characterRun.characterIndex + scriptRun->characterRun.numberOfCharacters ) )
    {
      return scriptRun->script;
    }
  }

  return TextAbstraction::UNKNOWN;
}

void LogicalModel::SetFonts( const FontRun* const fonts,
                             Length numberOfRuns )
{
  Vector<FontRun>& fontRuns = mImpl->mFontRuns;

  if( 0u == numberOfRuns )
  {
    fontRuns.Clear();
  }
  else
  {
    fontRuns.Resize( numberOfRuns );
    memcpy( fontRuns.Begin(), fonts, numberOfRuns * sizeof( FontRun ) );
  }

  mImpl->mGetFontCache.characterIndex = 0u;
  mImpl->mGetFontCache.numberOfCharacters = 0u;
  mImpl->mGetFontCache.firstRun = 0u;
  mImpl->mGetFontCache.numberOfRuns = 0u;
}

Length LogicalModel::GetNumberOfFontRuns( CharacterIndex characterIndex,
                                          Length numberOfCharacters ) const
{
  GetRunCache& fontCache = mImpl->mGetFontCache;

  // Set the character index and the number of characters into the cache.
  fontCache.characterIndex = characterIndex;
  fontCache.numberOfCharacters = numberOfCharacters;

  if( ( 0u == characterIndex ) &&
      ( mImpl->mText.Count() == numberOfCharacters ) )
  {
    fontCache.firstRun = 0u;
    fontCache.numberOfRuns = mImpl->mFontRuns.Count();
    return fontCache.numberOfRuns;
  }

  // Initialize the number of fonts and the index to the first font.
  fontCache.firstRun = 0u;
  fontCache.numberOfRuns = 0;
  bool firstFontFound = false;

  const Vector<FontRun>& modelFonts = mImpl->mFontRuns;
  const CharacterIndex lastCharacterIndex = characterIndex + numberOfCharacters;

  // Traverse the fonts and count those fonts within the range of characters.
  for( Vector<FontRun>::ConstIterator it = modelFonts.Begin(),
         endIt = modelFonts.End();
       it != endIt;
       ++it )
  {
    const FontRun& font = *it;

    if( ( font.characterRun.characterIndex + font.characterRun.numberOfCharacters > characterIndex ) &&
        ( characterIndex + numberOfCharacters > font.characterRun.characterIndex ) )
    {
      firstFontFound = true;
      ++fontCache.numberOfRuns;
    }
    else if( lastCharacterIndex <= font.characterRun.characterIndex )
    {
      // nothing else to do.
      break;
    }

    if( !firstFontFound )
    {
      ++fontCache.firstRun;
    }
  }

  return fontCache.numberOfRuns;
}

void LogicalModel::GetFontRuns( FontRun* fontRuns,
                                CharacterIndex characterIndex,
                                Length numberOfCharacters ) const
{
  const Vector<FontRun>& modelFonts = mImpl->mFontRuns;
  GetRunCache& fontCache = mImpl->mGetFontCache;

  if( ( characterIndex != fontCache.characterIndex ) ||
      ( numberOfCharacters != fontCache.numberOfCharacters ) )
  {
    GetNumberOfFontRuns( characterIndex,
                         numberOfCharacters );
  }

  memcpy( fontRuns, modelFonts.Begin() + fontCache.firstRun, fontCache.numberOfRuns * sizeof( FontRun ) );
}

FontId LogicalModel::GetFont( CharacterIndex characterIndex ) const
{
  for( Length index = 0u, length = mImpl->mFontRuns.Count(); index < length; ++index )
  {
    const FontRun* const fontRun = mImpl->mFontRuns.Begin() + index;

    if( ( fontRun->characterRun.characterIndex <= characterIndex ) &&
        ( characterIndex < fontRun->characterRun.characterIndex + fontRun->characterRun.numberOfCharacters ) )
    {
      return fontRun->fontId;
    }
  }

  return 0u;
}

void LogicalModel::SetLineBreakInfo( const LineBreakInfo* const lineBreakInfo,
                                     Length length )
{
  Vector<LineBreakInfo>& modelLineBreakInfo = mImpl->mLineBreakInfo;

  if( 0u == length )
  {
    modelLineBreakInfo.Clear();
  }
  else
  {
    modelLineBreakInfo.Resize( length );
    memcpy( modelLineBreakInfo.Begin(), lineBreakInfo, length * sizeof( LineBreakInfo ) );
  }
}

void LogicalModel::GetLineBreakInfo( LineBreakInfo* lineBreakInfo,
                                     CharacterIndex characterIndex,
                                     Length numberOfItems ) const
{
  memcpy( lineBreakInfo, mImpl->mLineBreakInfo.Begin() + characterIndex, numberOfItems * sizeof( LineBreakInfo ) );
}

LineBreakInfo LogicalModel::GetLineBreakInfo( CharacterIndex characterIndex ) const
{
  return *( mImpl->mLineBreakInfo.Begin() + characterIndex );
}

void LogicalModel::SetWordBreakInfo( const WordBreakInfo* const wordBreakInfo,
                                     Length length )
{
  Vector<WordBreakInfo>& modelWordBreakInfo = mImpl->mWordBreakInfo;

  if( 0u == length )
  {
    modelWordBreakInfo.Clear();
  }
  else
  {
    modelWordBreakInfo.Resize( length );
    memcpy( modelWordBreakInfo.Begin(), wordBreakInfo, length * sizeof( WordBreakInfo ) );
  }
}

void LogicalModel::GetWordBreakInfo( WordBreakInfo* wordBreakInfo,
                                     CharacterIndex characterIndex,
                                     Length numberOfItems ) const
{
  memcpy( wordBreakInfo, mImpl->mWordBreakInfo.Begin() + characterIndex, numberOfItems * sizeof( WordBreakInfo ) );
}

WordBreakInfo LogicalModel::GetWordBreakInfo( CharacterIndex characterIndex ) const
{
  return *( mImpl->mWordBreakInfo.Begin() + characterIndex );
}

void LogicalModel::SetBidirectionalInfo( const BidirectionalParagraphInfoRun* const bidirectionalInfo,
                                         Length numberOfRuns )
{
  Vector<BidirectionalParagraphInfoRun>& modelBidirectionalParagraphInfo = mImpl->mBidirectionalParagraphInfo;

  if( 0u == numberOfRuns )
  {
    modelBidirectionalParagraphInfo.Clear();
  }
  else
  {
    modelBidirectionalParagraphInfo.Resize( numberOfRuns );
    memcpy( modelBidirectionalParagraphInfo.Begin(), bidirectionalInfo, numberOfRuns * sizeof( BidirectionalParagraphInfoRun ) );
  }

  mImpl->mGetBidirectionalCache.characterIndex = 0u;
  mImpl->mGetBidirectionalCache.numberOfCharacters = 0u;
  mImpl->mGetBidirectionalCache.firstRun = 0u;
  mImpl->mGetBidirectionalCache.numberOfRuns = 0u;
}

Length LogicalModel::GetNumberOfBidirectionalInfoRuns( CharacterIndex characterIndex,
                                                       Length numberOfCharacters ) const
{
  GetRunCache& bidiCache = mImpl->mGetBidirectionalCache;

  // Set the character index and the number of characters into the cache.
  bidiCache.characterIndex = characterIndex;
  bidiCache.numberOfCharacters = numberOfCharacters;

  if( ( 0u == characterIndex ) &&
      ( mImpl->mText.Count() == numberOfCharacters ) )
  {
    bidiCache.firstRun = 0u;
    bidiCache.numberOfRuns = mImpl->mBidirectionalParagraphInfo.Count();
    return bidiCache.numberOfRuns;
  }

  // Initialize the number of bidi paragraphs and the index to the first paragraph.
  bidiCache.firstRun = 0u;
  bidiCache.numberOfRuns = 0;
  bool firstParagraphFound = false;

  const Vector<BidirectionalParagraphInfoRun>& modelBidirectionalParagraphInfo = mImpl->mBidirectionalParagraphInfo;

  // Traverse the bidirectional paragraph info and count those bidi paragraphs within the range of characters.
  for( Vector<BidirectionalParagraphInfoRun>::ConstIterator it = modelBidirectionalParagraphInfo.Begin(),
         endIt = modelBidirectionalParagraphInfo.End();
       it != endIt;
       ++it )
  {
    const BidirectionalParagraphInfoRun& bidi = *it;

    if( ( bidi.characterRun.characterIndex + bidi.characterRun.numberOfCharacters > characterIndex ) &&
        ( characterIndex + numberOfCharacters > bidi.characterRun.characterIndex ) )
    {
      firstParagraphFound = true;
      ++bidiCache.numberOfRuns;
    }

    if( !firstParagraphFound )
    {
      ++bidiCache.firstRun;
    }
  }

  return bidiCache.numberOfRuns;
}

void LogicalModel::GetBidirectionalInfo( BidirectionalParagraphInfoRun* bidirectionalInfo,
                                         CharacterIndex characterIndex,
                                         Length numberOfCharacters ) const
{
  const Vector<BidirectionalParagraphInfoRun>& modelBidirectionalParagraphInfo = mImpl->mBidirectionalParagraphInfo;
  GetRunCache& bidiCache = mImpl->mGetBidirectionalCache;

  if( ( characterIndex != bidiCache.characterIndex ) ||
      ( numberOfCharacters != bidiCache.numberOfCharacters ) )
  {
    GetNumberOfBidirectionalInfoRuns( characterIndex,
                                      numberOfCharacters );
  }

  memcpy( bidirectionalInfo, modelBidirectionalParagraphInfo.Begin() + bidiCache.firstRun, bidiCache.numberOfRuns * sizeof( BidirectionalParagraphInfoRun ) );
}

void LogicalModel::GetCharacterDirections( CharacterDirection* directions,
                                           CharacterIndex characterIndex,
                                           Length numberOfCharacters ) const
{
}

CharacterDirection LogicalModel::GetCharacterDirection( CharacterIndex characterIndex ) const
{
  return false;
}

void LogicalModel::SetVisualToLogicalMap( const BidirectionalLineInfoRun* const bidirectionalInfo,
                                          Length numberOfRuns )
{
  Vector<CharacterIndex>& modelVisualToLogicalMap = mImpl->mVisualToLogicalMap;
  Vector<CharacterIndex>& modelLogicalToVisualMap = mImpl->mLogicalToVisualMap;

  if( 0u == numberOfRuns )
  {
    modelVisualToLogicalMap.Clear();
    modelLogicalToVisualMap.Clear();
  }
  else
  {
    const Length numberOfCharacters = mImpl->mText.Count();
    modelVisualToLogicalMap.Resize( numberOfCharacters );
    modelLogicalToVisualMap.Resize( numberOfCharacters );

    CharacterIndex* modelVisualToLogicalMapBuffer = modelVisualToLogicalMap.Begin();
    CharacterIndex* modelLogicalToVisualMapBuffer = modelLogicalToVisualMap.Begin();

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
  }
}

CharacterIndex LogicalModel::GetVisualCharacterIndex( CharacterIndex logicalCharacterIndex ) const
{
  if( 0u == mImpl->mLogicalToVisualMap.Count() )
  {
    // If there is no logical to visual info is because the whole text is left to right.
    // Return the identity.
    return logicalCharacterIndex;
  }

  return *( mImpl->mLogicalToVisualMap.Begin() + logicalCharacterIndex );
}

CharacterIndex LogicalModel::GetLogicalCharacterIndex( CharacterIndex visualCharacterIndex ) const
{
  if( 0u == mImpl->mVisualToLogicalMap.Count() )
  {
    // If there is no visual to logical info is because the whole text is left to right.
    // Return the identity.
    return visualCharacterIndex;
  }

  return *( mImpl->mVisualToLogicalMap.Begin() + visualCharacterIndex );
}

void LogicalModel::GetLogicalToVisualMap( CharacterIndex* logicalToVisualMap,
                                          CharacterIndex characterIndex,
                                          Length numberOfCharacters ) const
{
  memcpy( logicalToVisualMap, mImpl->mLogicalToVisualMap.Begin() + characterIndex, numberOfCharacters * sizeof( CharacterIndex ) );
}

void LogicalModel::GetVisualToLogicalMap( CharacterIndex* visualToLogicalMap,
                                          CharacterIndex characterIndex,
                                          Length numberOfCharacters ) const
{
  memcpy( visualToLogicalMap, mImpl->mVisualToLogicalMap.Begin() + characterIndex, numberOfCharacters * sizeof( CharacterIndex ) );
}

LogicalModel::~LogicalModel()
{
  delete mImpl;
}

LogicalModel::LogicalModel()
: mImpl( NULL )
{
  mImpl = new LogicalModel::Impl();
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
