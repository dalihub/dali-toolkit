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
#include <dali-toolkit/public-api/text/logical-model.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/font-run.h>
#include <dali-toolkit/public-api/text/script-run.h>

// EXTERNAL INCLUDES
#include <memory.h>
#include <stdlib.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct LogicalModel::Impl
{
  Vector<Character> mText;
  Vector<ScriptRun> mScriptRuns;
  Vector<FontRun>   mFontRuns;
};

LogicalModelPtr LogicalModel::New()
{
  return LogicalModelPtr( new LogicalModel() );
}

void LogicalModel::SetText( const Character* text, Length length )
{
  Vector<Character>& modelText = mImpl->mText;
  modelText.Resize( length );
  memcpy( &modelText[0], text, length*sizeof(Character) );
}

Length LogicalModel::GetNumberOfCharacters() const
{
  return mImpl->mText.Count();
}

void LogicalModel::GetText( CharacterIndex characterIndex, Character* text, Length numberOfCharacters ) const
{
  Vector<Character>& modelText = mImpl->mText;
  memcpy( text, &modelText[characterIndex], numberOfCharacters*sizeof(Character) );
}

void LogicalModel::SetScripts( const ScriptRun* const scripts,
                               Length numberOfRuns )
{
  Vector<ScriptRun>& scriptRuns = mImpl->mScriptRuns;
  scriptRuns.Resize( numberOfRuns );
  memcpy( scriptRuns.Begin(), scripts, numberOfRuns * sizeof( ScriptRun ) );
}

Length LogicalModel::GetNumberOfScriptRuns( CharacterIndex characterIndex,
                                            Length numberOfCharacters ) const
{
  if( ( 0u == characterIndex ) && ( mImpl->mText.Count() == numberOfCharacters ) )
  {
    return mImpl->mScriptRuns.Count();
  }

  const CharacterIndex charcterEndIndex = characterIndex + numberOfCharacters;
  Length numberOfScriptRuns = 0u;
  bool firstIndexFound = false;

  for( Length index = 0u, length = mImpl->mScriptRuns.Count(); index < length; ++index )
  {
    const ScriptRun* const scriptRun = mImpl->mScriptRuns.Begin() + index;

    if( !firstIndexFound &&
        ( characterIndex < scriptRun->characterRun.characterIndex + scriptRun->characterRun.numberOfCharacters ) )
    {
      // The character index is within this script run.
      // Starts the counter of script runs.
      firstIndexFound = true;
    }

    if( firstIndexFound )
    {
      ++numberOfScriptRuns;
      if( scriptRun->characterRun.characterIndex + scriptRun->characterRun.numberOfCharacters > charcterEndIndex )
      {
        // This script run exceeds the given range. The number of scripts can be returned.
        return numberOfScriptRuns;
      }
    }
  }

  return numberOfScriptRuns;
}

void LogicalModel::GetScriptRuns( ScriptRun* scriptRuns,
                                  CharacterIndex characterIndex,
                                  Length numberOfCharacters ) const
{
  // A better implementation can cache the first script run and the number of then when the GetNumberOfScriptRuns() is called.

  Length numberOfScriptRuns = GetNumberOfScriptRuns( characterIndex,
                                                     numberOfCharacters );

  for( Length index = 0u, length = mImpl->mScriptRuns.Count(); index < length; ++index )
  {
    const ScriptRun* const scriptRun = mImpl->mScriptRuns.Begin() + index;

    if( characterIndex < scriptRun->characterRun.characterIndex + scriptRun->characterRun.numberOfCharacters )
    {
      memcpy( scriptRuns, scriptRun, sizeof( ScriptRun ) * numberOfScriptRuns );
      return;
    }
  }
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
  fontRuns.Resize( numberOfRuns );
  memcpy( fontRuns.Begin(), fonts, numberOfRuns * sizeof( FontRun ) );
}

Length LogicalModel::GetNumberOfFontRuns( CharacterIndex characterIndex,
                                          Length numberOfCharacters ) const
{
  if( ( 0u == characterIndex ) && ( mImpl->mText.Count() == numberOfCharacters ) )
  {
    return mImpl->mFontRuns.Count();
  }

  const CharacterIndex charcterEndIndex = characterIndex + numberOfCharacters;
  Length numberOfFontRuns = 0u;
  bool firstIndexFound = false;

  for( Length index = 0u, length = mImpl->mFontRuns.Count(); index < length; ++index )
  {
    const FontRun* const fontRun = mImpl->mFontRuns.Begin() + index;

    if( !firstIndexFound &&
        ( characterIndex < fontRun->characterRun.characterIndex + fontRun->characterRun.numberOfCharacters ) )
    {
      // The character index is within this font run.
      // Starts the counter of font runs.
      firstIndexFound = true;
    }

    if( firstIndexFound )
    {
      ++numberOfFontRuns;
      if( fontRun->characterRun.characterIndex + fontRun->characterRun.numberOfCharacters > charcterEndIndex )
      {
        // This font run exceeds the given range. The number of fonts can be returned.
        return numberOfFontRuns;
      }
    }
  }

  return numberOfFontRuns;
}

void LogicalModel::GetFontRuns( FontRun* fontRuns,
                                CharacterIndex characterIndex,
                                Length numberOfCharacters ) const
{
  // A better implementation can cache the first font run and the number of then when the GetNumberOfFontRuns() is called.

  Length numberOfFontRuns = GetNumberOfFontRuns( characterIndex,
                                                 numberOfCharacters );

  for( Length index = 0u, length = mImpl->mFontRuns.Count(); index < length; ++index )
  {
    const FontRun* const fontRun = mImpl->mFontRuns.Begin() + index;

    if( characterIndex < fontRun->characterRun.characterIndex + fontRun->characterRun.numberOfCharacters )
    {
      memcpy( fontRuns, fontRun, sizeof( FontRun ) * numberOfFontRuns );
      return;
    }
  }
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
}

void LogicalModel::GetLineBreakInfo( LineBreakInfo* lineBreakInfo,
                                     CharacterIndex characterIndex,
                                     Length numberOfItems ) const
{
}

LineBreakInfo LogicalModel::GetLineBreakInfo( CharacterIndex characterIndex ) const
{
  return 0;
}

void LogicalModel::SetWordBreakInfo( const WordBreakInfo* const wordBreakInfo,
                                     Length length )
{
}

void LogicalModel::GetWordBreakInfo( WordBreakInfo* wordBreakInfo,
                                     CharacterIndex characterIndex,
                                     Length numberOfItems ) const
{
}

WordBreakInfo LogicalModel::GetWordBreakInfo( CharacterIndex characterIndex ) const
{
  return 0;
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
