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
#include <dali-toolkit/public-api/text/shaper.h>

// INTERNAL INCLUDES
#include <dali/public-api/text-abstraction/shaping.h>
#include <dali-toolkit/public-api/text/font-run.h>
#include <dali-toolkit/public-api/text/logical-model.h>
#include <dali-toolkit/public-api/text/script-run.h>
#include <dali-toolkit/public-api/text/visual-model.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

CharacterIndex min( CharacterIndex index0,
                    CharacterIndex index1 )
{
  return ( index0 < index1 ) ? index0 : index1;
}

void ShapeText( const Vector<Character>& text,
                const Vector<LineBreakInfo>& lineBreakInfo,
                const Vector<ScriptRun>& scripts,
                const Vector<FontRun>& fonts,
                Vector<GlyphInfo>& glyphs,
                Vector<CharacterIndex>& characterIndices,
                Vector<Length>& charactersPerGlyph )
{
  const Length numberOfCharacters = text.Count();

  if( 0u == numberOfCharacters )
  {
    // Nothing to do if there are no characters.
    return;
  }

  const Length numberOfFontRuns = fonts.Count();

  DALI_ASSERT_DEBUG( ( 0u != numberOfFontRuns ) &&
                     ( numberOfCharacters == fonts[numberOfFontRuns - 1u].characterRun.characterIndex + fonts[numberOfFontRuns - 1u].characterRun.numberOfCharacters ) &&
                     "Toolkit::Text::ShapeText. All characters must have a font set." );

  const Length numberOfScriptRuns = scripts.Count();

  DALI_ASSERT_DEBUG( ( 0u != numberOfScriptRuns ) &&
                     ( numberOfCharacters == scripts[numberOfScriptRuns - 1u].characterRun.characterIndex + scripts[numberOfScriptRuns - 1u].characterRun.numberOfCharacters ) &&
                     "Toolkit::Text::ShapeText. All characters must have a script set." );

  // The text needs to be split in chunks of consecutive characters.
  // Each chunk must contain characters with the same font id and script set.
  // A chunk of consecutive characters must not contain a LINE_MUST_BREAK, if there is one a new chunk have to be created.

  TextAbstraction::Shaping shaping = TextAbstraction::Shaping::Get();

  // To shape the text a font and an script is needed.
  Vector<FontRun>::ConstIterator fontRunIt = fonts.Begin();
  Vector<ScriptRun>::ConstIterator scriptRunIt = scripts.Begin();

  // The line must break token converted to LineBreakInfo to be compared and avoid a compile error.
  const LineBreakInfo MUST_BREAK = static_cast<LineBreakInfo>( TextAbstraction::LINE_MUST_BREAK );

  // Index to the the next one to be shaped. Is pointing the character after the last one it was shaped.
  CharacterIndex previousIndex = 0u;

  // The current font id and script used to shape the text.
  FontId currentFontId = 0u;
  Script currentScript = TextAbstraction::UNKNOWN;

  // Reserve some space to allocate the glyphs and the glyph to character map.
  // There is no way to know the number of glyphs before shaping the text.
  // To avoid reallocations it's reserved space for a slightly biger number of glyphs than the number of characters.

  Length numberOfGlyphsReserved = static_cast<Length>( numberOfCharacters * 1.3f );
  glyphs.Resize( numberOfGlyphsReserved );
  charactersPerGlyph.Resize( numberOfGlyphsReserved );

  // The actual number of glyphs.
  Length totalNumberOfGlyphs = 0u;

  // Traverse the characters and shape the text.
  for( previousIndex = 0; previousIndex < numberOfCharacters; )
  {
    // Get the font id and the script.
    const FontRun& fontRun = *fontRunIt;
    const ScriptRun& scriptRun = *scriptRunIt;

    currentFontId = fontRun.fontId;
    currentScript = scriptRun.script;

    // Get the min index to the last character of both runs.
    CharacterIndex currentIndex = min( fontRun.characterRun.characterIndex + fontRun.characterRun.numberOfCharacters,
                                       scriptRun.characterRun.characterIndex + scriptRun.characterRun.numberOfCharacters );

    // Check if there is a line must break.
    for( CharacterIndex index = previousIndex; index < currentIndex; ++index )
    {
      if( MUST_BREAK == lineBreakInfo.Begin() + index )
      {
        currentIndex = index;
        break;
      }
    }

    // Shape the text for the current chunk.
    const Length numberOfGlyphs = shaping.Shape( text.Begin() + previousIndex,
                                                 currentIndex - previousIndex,
                                                 currentFontId,
                                                 currentScript );

    const Length glyphIndex = totalNumberOfGlyphs;
    totalNumberOfGlyphs += numberOfGlyphs;

    if( totalNumberOfGlyphs > numberOfGlyphsReserved )
    {
      // Resize the vectors to get enough space.
      numberOfGlyphsReserved = static_cast<Length>( totalNumberOfGlyphs * 1.3f );
      glyphs.Resize( numberOfGlyphsReserved );
      charactersPerGlyph.Resize( numberOfGlyphsReserved );
    }

    // Retrieve the glyphs and the glyph to character conversion map.
    shaping.GetGlyphs( glyphs.Begin() + glyphIndex,
                       charactersPerGlyph.Begin() + glyphIndex );

    // Update the iterators to get the next font or script run.
    if( currentIndex == fontRun.characterRun.characterIndex + fontRun.characterRun.numberOfCharacters )
    {
      ++fontRunIt;
    }
    if( currentIndex == scriptRun.characterRun.characterIndex + scriptRun.characterRun.numberOfCharacters )
    {
      ++scriptRunIt;
    }

    // Update the previous index.
    previousIndex = currentIndex;
  }

  characterIndices.Reserve( totalNumberOfGlyphs );
  CharacterIndex characterIndex = 0u;
  characterIndices.PushBack( characterIndex );
  for( Length index = 0u, length = totalNumberOfGlyphs - 1u; index < length; ++index )
  {
    characterIndex += *( charactersPerGlyph.Begin() + index );
    characterIndices.PushBack( characterIndex );
  }

  // Resize the vectors to set the right number of items.
  glyphs.Resize( totalNumberOfGlyphs );
  // characterIndices.Resize( totalNumberOfGlyphs );
  charactersPerGlyph.Resize( totalNumberOfGlyphs );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
