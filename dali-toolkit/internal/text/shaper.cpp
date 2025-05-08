/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <chrono>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_TEXT_PERFORMANCE_MARKER, false);

CharacterIndex min(CharacterIndex index0,
                   CharacterIndex index1)
{
  return (index0 < index1) ? index0 : index1;
}

#if defined(TRACE_ENABLED)
uint32_t GetMilliSeconds()
{
  // Get the time of a monotonic clock since its epoch.
  auto epoch = std::chrono::steady_clock::now().time_since_epoch();

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);

  return static_cast<uint32_t>(duration.count());
}
#endif

void ShapeText(TextAbstraction::Shaping&    shaping,
               TextAbstraction::FontClient& fontClient,
               const Vector<Character>&     text,
               const Vector<LineBreakInfo>& lineBreakInfo,
               const Vector<ScriptRun>&     scripts,
               const Vector<FontRun>&       fonts,
               CharacterIndex               startCharacterIndex,
               GlyphIndex                   startGlyphIndex,
               Length                       numberOfCharacters,
               Vector<GlyphInfo>&           glyphs,
               Vector<CharacterIndex>&      glyphToCharacterMap,
               Vector<Length>&              charactersPerGlyph,
               Vector<GlyphIndex>&          newParagraphGlyphs)
{
  if(0u == numberOfCharacters)
  {
    // Nothing to do if there are no characters.
    return;
  }

  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_SHAPE_TEXT");

#if defined(TRACE_ENABLED)
  uint32_t sumPre = 0, sumShape = 0, sumPost = 0;

  uint32_t logThreshold = TextAbstraction::FontClient::GetPerformanceLogThresholdTime();
  bool     logEnabled   = TextAbstraction::FontClient::IsPerformanceLogEnabled();
#endif

#ifdef DEBUG_ENABLED
  const Length numberOfFontRuns        = fonts.Count();
  const Length numberOfScriptRuns      = scripts.Count();
  const Length totalNumberOfCharacters = text.Count();
#endif

  DALI_ASSERT_DEBUG((0u != numberOfFontRuns) &&
                    (totalNumberOfCharacters == fonts[numberOfFontRuns - 1u].characterRun.characterIndex + fonts[numberOfFontRuns - 1u].characterRun.numberOfCharacters) &&
                    "Toolkit::Text::ShapeText. All characters must have a font set.");

  DALI_ASSERT_DEBUG((0u != numberOfScriptRuns) &&
                    (totalNumberOfCharacters == scripts[numberOfScriptRuns - 1u].characterRun.characterIndex + scripts[numberOfScriptRuns - 1u].characterRun.numberOfCharacters) &&
                    "Toolkit::Text::ShapeText. All characters must have a script set.");

  // The text needs to be split in chunks of consecutive characters.
  // Each chunk must contain characters with the same font id and script set.
  // A chunk of consecutive characters must not contain a LINE_MUST_BREAK, if there is one a new chunk has to be created.

  // To shape the text a font and an script is needed.

  // Get the font run containing the startCharacterIndex character.
  Vector<FontRun>::ConstIterator fontRunIt = fonts.Begin();
  for(Vector<FontRun>::ConstIterator endIt = fonts.End(); fontRunIt < endIt; ++fontRunIt)
  {
    const FontRun& run = *fontRunIt;
    if(startCharacterIndex < run.characterRun.characterIndex + run.characterRun.numberOfCharacters)
    {
      // Found.
      break;
    }
  }

  // Get the script run containing the startCharacterIndex character.
  Vector<ScriptRun>::ConstIterator scriptRunIt = scripts.Begin();
  for(Vector<ScriptRun>::ConstIterator endIt = scripts.End(); scriptRunIt < endIt; ++scriptRunIt)
  {
    const ScriptRun& run = *scriptRunIt;
    if(startCharacterIndex < run.characterRun.characterIndex + run.characterRun.numberOfCharacters)
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

  GlyphInfo glyphInfo;
  glyphInfo.isItalicRequired = false;
  glyphInfo.isBoldRequired   = false;

  const Length currentNumberOfGlyphs  = glyphs.Count();
  const Length numberOfGlyphsReserved = static_cast<Length>(numberOfCharacters * 1.3f);
  glyphs.Reserve(currentNumberOfGlyphs + numberOfGlyphsReserved);
  glyphToCharacterMap.Reserve(currentNumberOfGlyphs + numberOfGlyphsReserved);

  // The actual number of glyphs.
  Length totalNumberOfGlyphs = currentNumberOfGlyphs;
  // The number of new glyphs.
  Length numberOfNewGlyphs = 0u;

  const Character* const     textBuffer                = text.Begin();
  const LineBreakInfo* const lineBreakInfoBuffer       = lineBreakInfo.Begin();
  CharacterIndex*            glyphToCharacterMapBuffer = glyphToCharacterMap.Begin();

  Length glyphIndex = startGlyphIndex;

  // Traverse the characters and shape the text.
  const CharacterIndex lastCharacter = startCharacterIndex + numberOfCharacters;
  for(previousIndex = startCharacterIndex; previousIndex < lastCharacter;)
  {
#if defined(TRACE_ENABLED)
    uint32_t timeStamps[4];
    uint32_t timeStampIndex = 0;

    if(logEnabled)
    {
      timeStamps[timeStampIndex++] = GetMilliSeconds();
    }
#endif

    // Get the font id and the script.
    const FontRun&   fontRun   = *fontRunIt;
    const ScriptRun& scriptRun = *scriptRunIt;

    currentFontId               = fontRun.fontId;
    currentScript               = scriptRun.script;
    const bool isItalicRequired = fontRun.isItalicRequired;
    const bool isBoldRequired   = fontRun.isBoldRequired;

    // Get the min index to the last character of both runs.
    CharacterIndex currentIndex = min(fontRun.characterRun.characterIndex + fontRun.characterRun.numberOfCharacters,
                                      scriptRun.characterRun.characterIndex + scriptRun.characterRun.numberOfCharacters);

    // Check if there is a line must break.
    bool mustBreak = false;

    // Check if the current index is a new paragraph character.
    // A new paragraph character is going to be shaped in order to not to mess the conversion tables.
    // However, the metrics need to be changed in order to not to draw a square.
    bool isNewParagraph = false;

    for(CharacterIndex index = previousIndex; index < currentIndex; ++index)
    {
      mustBreak = TextAbstraction::LINE_MUST_BREAK == *(lineBreakInfoBuffer + index);
      if(mustBreak)
      {
        isNewParagraph = TextAbstraction::IsNewParagraph(*(textBuffer + index));
        currentIndex   = index + 1u;
        break;
      }
    }

#if defined(TRACE_ENABLED)
    if(logEnabled)
    {
      timeStamps[timeStampIndex++] = GetMilliSeconds();
    }
#endif

    // Shape the text for the current chunk.
    const Length numberOfGlyphs = shaping.Shape(fontClient,
                                                textBuffer + previousIndex,
                                                (currentIndex - previousIndex), // The number of characters to shape.
                                                currentFontId,
                                                currentScript);

#if defined(TRACE_ENABLED)
    if(logEnabled)
    {
      timeStamps[timeStampIndex++] = GetMilliSeconds();
    }
#endif

    // Retrieve the glyphs and the glyph to character conversion map.
    Vector<GlyphInfo>      tmpGlyphs;
    Vector<CharacterIndex> tmpGlyphToCharacterMap;

    GlyphInfo glyphInfo;
    glyphInfo.isItalicRequired = isItalicRequired;
    glyphInfo.isBoldRequired   = isBoldRequired;

    tmpGlyphs.Resize(numberOfGlyphs, glyphInfo);
    tmpGlyphToCharacterMap.Resize(numberOfGlyphs);
    shaping.GetGlyphs(tmpGlyphs.Begin(),
                      tmpGlyphToCharacterMap.Begin());

    // Update the new indices of the glyph to character map.
    if(0u != totalNumberOfGlyphs)
    {
      for(Vector<CharacterIndex>::Iterator it    = tmpGlyphToCharacterMap.Begin(),
                                           endIt = tmpGlyphToCharacterMap.End();
          it != endIt;
          ++it)
      {
        *it += previousIndex;
      }
    }

    totalNumberOfGlyphs += numberOfGlyphs;
    numberOfNewGlyphs += numberOfGlyphs;

    glyphs.Insert(glyphs.Begin() + glyphIndex, tmpGlyphs.Begin(), tmpGlyphs.End());
    glyphToCharacterMap.Insert(glyphToCharacterMap.Begin() + glyphIndex, tmpGlyphToCharacterMap.Begin(), tmpGlyphToCharacterMap.End());
    glyphIndex += numberOfGlyphs;

    // Set the buffer pointers again.
    glyphToCharacterMapBuffer = glyphToCharacterMap.Begin();

    if(isNewParagraph)
    {
      // Add the index of the new paragraph glyph to a vector.
      // Their metrics will be updated in a following step.
      DALI_ASSERT_ALWAYS(glyphIndex > 0u);
      newParagraphGlyphs.PushBack(glyphIndex - 1u);
    }

    // Update the iterators to get the next font or script run.
    if(currentIndex == fontRun.characterRun.characterIndex + fontRun.characterRun.numberOfCharacters)
    {
      ++fontRunIt;
    }
    if(currentIndex == scriptRun.characterRun.characterIndex + scriptRun.characterRun.numberOfCharacters)
    {
      ++scriptRunIt;
    }

    // Update the previous index.
    previousIndex = currentIndex;

#if defined(TRACE_ENABLED)
    if(logEnabled)
    {
      timeStamps[timeStampIndex++] = GetMilliSeconds();
      sumPre   += timeStamps[1] - timeStamps[0];
      sumShape += timeStamps[2] - timeStamps[1];
      sumPost  += timeStamps[3] - timeStamps[2];
    }
#endif
  }

  // Update indices.
  for(Length index = startGlyphIndex + numberOfNewGlyphs; index < totalNumberOfGlyphs; ++index)
  {
    CharacterIndex& characterIndex = *(glyphToCharacterMapBuffer + index);
    characterIndex += numberOfCharacters;
  }

  // Add the number of characters per glyph.
  charactersPerGlyph.Reserve(totalNumberOfGlyphs);
  Length* charactersPerGlyphBuffer = charactersPerGlyph.Begin();

  const GlyphIndex lastGlyph = startGlyphIndex + numberOfNewGlyphs;
  previousIndex              = startCharacterIndex;
  for(Length index = startGlyphIndex + 1u; index < lastGlyph; ++index)
  {
    const CharacterIndex characterIndex = *(glyphToCharacterMapBuffer + index);

    charactersPerGlyph.Insert(charactersPerGlyphBuffer + index - 1u, characterIndex - previousIndex);

    previousIndex = characterIndex;
  }
  charactersPerGlyph.Insert(charactersPerGlyphBuffer + lastGlyph - 1u, numberOfCharacters + startCharacterIndex - previousIndex);

  // Resize the vectors to set the right number of items.
  glyphs.Resize(totalNumberOfGlyphs);
  glyphToCharacterMap.Resize(totalNumberOfGlyphs);

#if defined(TRACE_ENABLED)
  if(logEnabled)
  {
    if(sumPre + sumShape + sumPost > logThreshold)
    {
      DALI_LOG_DEBUG_INFO("DALI_TEXT_SHAPE_TEXT updated:%u/%u, pre:%u ms, shape:%u ms, post:%u ms\n", numberOfNewGlyphs, totalNumberOfGlyphs, sumPre, sumShape, sumPost);
    }
  }
#endif
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
