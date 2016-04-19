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
#include <dali-toolkit/internal/text/shaper.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/shaping.h>

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
                CharacterIndex startCharacterIndex,
                GlyphIndex startGlyphIndex,
                Length numberOfCharacters,
                Vector<GlyphInfo>& glyphs,
                Vector<CharacterIndex>& glyphToCharacterMap,
                Vector<Length>& charactersPerGlyph,
                Vector<GlyphIndex>& newParagraphGlyphs )
{
  if( 0u == numberOfCharacters )
  {
    // Nothing to do if there are no characters.
    return;
  }

#ifdef DEBUG_ENABLED
  const Length numberOfFontRuns = fonts.Count();
  const Length numberOfScriptRuns = scripts.Count();
  const Length totalNumberOfCharacters = text.Count();
#endif

  DALI_ASSERT_DEBUG( ( 0u != numberOfFontRuns ) &&
                     ( totalNumberOfCharacters == fonts[numberOfFontRuns - 1u].characterRun.characterIndex + fonts[numberOfFontRuns - 1u].characterRun.numberOfCharacters ) &&
                     "Toolkit::Text::ShapeText. All characters must have a font set." );

  DALI_ASSERT_DEBUG( ( 0u != numberOfScriptRuns ) &&
                     ( totalNumberOfCharacters == scripts[numberOfScriptRuns - 1u].characterRun.characterIndex + scripts[numberOfScriptRuns - 1u].characterRun.numberOfCharacters ) &&
                     "Toolkit::Text::ShapeText. All characters must have a script set." );

  // The text needs to be split in chunks of consecutive characters.
  // Each chunk must contain characters with the same font id and script set.
  // A chunk of consecutive characters must not contain a LINE_MUST_BREAK, if there is one a new chunk has to be created.

  TextAbstraction::Shaping shaping = TextAbstraction::Shaping::Get();

  // To shape the text a font and an script is needed.

  // Get the font run containing the startCharacterIndex character.
  Vector<FontRun>::ConstIterator fontRunIt = fonts.Begin();
  for( Vector<FontRun>::ConstIterator endIt = fonts.End(); fontRunIt < endIt; ++fontRunIt )
  {
    const FontRun& run = *fontRunIt;
    if( startCharacterIndex < run.characterRun.characterIndex + run.characterRun.numberOfCharacters )
    {
      // Found.
      break;
    }
  }

  // Get the script run containing the startCharacterIndex character.
  Vector<ScriptRun>::ConstIterator scriptRunIt = scripts.Begin();
  for( Vector<ScriptRun>::ConstIterator endIt = scripts.End(); scriptRunIt < endIt; ++scriptRunIt )
  {
    const ScriptRun& run = *scriptRunIt;
    if( startCharacterIndex < run.characterRun.characterIndex + run.characterRun.numberOfCharacters )
    {
      // Found.
      break;
    }
  }

  // Index to the the next one to be shaped. Is pointing the character after the last one it was shaped.
  CharacterIndex previousIndex = 0u;

  // The current font id and script used to shape the text.
  FontId currentFontId = 0u;
  Script currentScript = TextAbstraction::UNKNOWN;

  // Reserve some space to allocate the glyphs and the glyph to character map.
  // There is no way to know the number of glyphs before shaping the text.
  // To avoid reallocations it's reserved space for a slightly biger number of glyphs than the number of characters.

  const Length currentNumberOfGlyphs = glyphs.Count();
  const Length numberOfGlyphsReserved = static_cast<Length>( numberOfCharacters * 1.3f );
  glyphs.Resize( currentNumberOfGlyphs + numberOfGlyphsReserved );
  glyphToCharacterMap.Resize( currentNumberOfGlyphs + numberOfGlyphsReserved );

  // The actual number of glyphs.
  Length totalNumberOfGlyphs = currentNumberOfGlyphs;
  // The number of new glyphs.
  Length numberOfNewGlyphs = 0u;

  const Character* const textBuffer = text.Begin();
  const LineBreakInfo* const lineBreakInfoBuffer = lineBreakInfo.Begin();
  CharacterIndex* glyphToCharacterMapBuffer = glyphToCharacterMap.Begin();

  Length glyphIndex = startGlyphIndex;

  // Traverse the characters and shape the text.
  const CharacterIndex lastCharacter = startCharacterIndex + numberOfCharacters;
  for( previousIndex = startCharacterIndex; previousIndex < lastCharacter; )
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
    bool mustBreak = false;

    // Check if the current index is a new paragraph character.
    // A new paragraph character is going to be shaped in order to not to mess the conversion tables.
    // However, the metrics need to be changed in order to not to draw a square.
    bool isNewParagraph = false;

    for( CharacterIndex index = previousIndex; index < currentIndex; ++index )
    {
      mustBreak = TextAbstraction::LINE_MUST_BREAK == *( lineBreakInfoBuffer + index );
      if( mustBreak )
      {
        isNewParagraph = TextAbstraction::IsNewParagraph( *( textBuffer + index ) );
        currentIndex = index + 1u;
        break;
      }
    }

    // Shape the text for the current chunk.
    const Length numberOfGlyphs = shaping.Shape( textBuffer + previousIndex,
                                                 ( currentIndex - previousIndex ), // The number of characters to shape.
                                                 currentFontId,
                                                 currentScript );

    // Retrieve the glyphs and the glyph to character conversion map.
    Vector<GlyphInfo> tmpGlyphs;
    Vector<CharacterIndex> tmpGlyphToCharacterMap;
    tmpGlyphs.Resize( numberOfGlyphs );
    tmpGlyphToCharacterMap.Resize( numberOfGlyphs );
    shaping.GetGlyphs( tmpGlyphs.Begin(),
                       tmpGlyphToCharacterMap.Begin() );

    // Update the new indices of the glyph to character map.
    if( 0u != totalNumberOfGlyphs )
    {
      for( Vector<CharacterIndex>::Iterator it = tmpGlyphToCharacterMap.Begin(),
             endIt = tmpGlyphToCharacterMap.End();
           it != endIt;
           ++it )
      {
        *it += previousIndex;
      }
    }

    totalNumberOfGlyphs += numberOfGlyphs;
    numberOfNewGlyphs += numberOfGlyphs;

    glyphs.Insert( glyphs.Begin() + glyphIndex, tmpGlyphs.Begin(), tmpGlyphs.End() );
    glyphToCharacterMap.Insert( glyphToCharacterMap.Begin() + glyphIndex, tmpGlyphToCharacterMap.Begin(), tmpGlyphToCharacterMap.End() );
    glyphIndex += numberOfGlyphs;

    // Set the buffer pointers again.
    glyphToCharacterMapBuffer = glyphToCharacterMap.Begin();

    if( isNewParagraph )
    {
      // Add the index of the new paragraph glyph to a vector.
      // Their metrics will be updated in a following step.
      newParagraphGlyphs.PushBack( glyphIndex - 1u );
    }

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

  // Update indices.
  for( Length index = startGlyphIndex + numberOfNewGlyphs; index < totalNumberOfGlyphs; ++index )
  {
    CharacterIndex& characterIndex = *( glyphToCharacterMapBuffer + index );
    characterIndex += numberOfCharacters;
  }

  // Add the number of characters per glyph.
  charactersPerGlyph.Reserve( totalNumberOfGlyphs );
  Length* charactersPerGlyphBuffer = charactersPerGlyph.Begin();

  const GlyphIndex lastGlyph = startGlyphIndex + numberOfNewGlyphs;
  previousIndex = startCharacterIndex;
  for( Length index = startGlyphIndex + 1u; index < lastGlyph; ++index )
  {
    const CharacterIndex characterIndex = *( glyphToCharacterMapBuffer + index );

    charactersPerGlyph.Insert( charactersPerGlyphBuffer + index - 1u, characterIndex - previousIndex );

    previousIndex = characterIndex;
  }
  charactersPerGlyph.Insert( charactersPerGlyphBuffer + lastGlyph - 1u, numberOfCharacters + startCharacterIndex - previousIndex );

  // Resize the vectors to set the right number of items.
  glyphs.Resize( totalNumberOfGlyphs );
  glyphToCharacterMap.Resize( totalNumberOfGlyphs );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
