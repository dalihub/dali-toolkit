
/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/glyph-metrics-helper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/rendering/styles/character-spacing-helper-functions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
Length GetNumberOfGlyphsOfGroup(GlyphIndex          glyphIndex,
                                GlyphIndex          lastGlyphPlusOne,
                                const Length* const charactersPerGlyphBuffer)
{
  Length numberOfGLyphsInGroup = 1u;

  for(GlyphIndex index = glyphIndex; index < lastGlyphPlusOne; ++index)
  {
    if(0u == *(charactersPerGlyphBuffer + index))
    {
      ++numberOfGLyphsInGroup;
    }
    else
    {
      break;
    }
  }

  return numberOfGLyphsInGroup;
}

void GetGlyphsMetrics(GlyphIndex             glyphIndex,
                      Length                 numberOfGlyphs,
                      GlyphMetrics&          glyphMetrics,
                      const GlyphInfo* const glyphsBuffer,
                      MetricsPtr&            metrics,
                      float                  calculatedAdvance)
{
  const GlyphInfo& firstGlyph = *(glyphsBuffer + glyphIndex);

  Text::FontMetrics fontMetrics;
  if(0u != firstGlyph.fontId)
  {
    metrics->GetFontMetrics(firstGlyph.fontId, fontMetrics);
  }
  else if(0u != firstGlyph.index)
  {
    // It may be an embedded image.
    fontMetrics.ascender  = firstGlyph.height;
    fontMetrics.descender = 0.f;
    fontMetrics.height    = fontMetrics.ascender;
  }

  const bool isItalicFont = metrics->HasItalicStyle(firstGlyph.fontId);

  glyphMetrics.fontId     = firstGlyph.fontId;
  glyphMetrics.fontHeight = fontMetrics.height;
  glyphMetrics.width      = firstGlyph.width;
  glyphMetrics.advance    = calculatedAdvance;
  glyphMetrics.ascender   = fontMetrics.ascender;
  glyphMetrics.xBearing   = firstGlyph.xBearing;

  if(1u < numberOfGlyphs)
  {
    float maxWidthEdge = firstGlyph.xBearing + firstGlyph.width;

    for(unsigned int i = 1u; i < numberOfGlyphs; ++i)
    {
      const GlyphInfo& glyphInfo = *(glyphsBuffer + glyphIndex + i);

      // update the initial xBearing if smaller.
      glyphMetrics.xBearing = std::min(glyphMetrics.xBearing, glyphMetrics.advance + glyphInfo.xBearing);

      // update the max width edge if bigger.
      const float currentMaxGlyphWidthEdge = glyphMetrics.advance + glyphInfo.xBearing + glyphInfo.width;
      maxWidthEdge                         = std::max(maxWidthEdge, currentMaxGlyphWidthEdge);

      glyphMetrics.advance += (glyphInfo.advance);
    }

    glyphMetrics.width = maxWidthEdge - glyphMetrics.xBearing;
  }

  glyphMetrics.width += (firstGlyph.isItalicRequired && !isItalicFont) ? TextAbstraction::FontClient::DEFAULT_ITALIC_ANGLE * firstGlyph.height : 0.f;
}

void GetGlyphMetricsFromCharacterIndex(CharacterIndex         index,
                                       const VisualModelPtr&  visualModel,
                                       const LogicalModelPtr& logicalModel,
                                       MetricsPtr&            metrics,
                                       GlyphMetrics&          glyphMetrics,
                                       GlyphIndex&            glyphIndex,
                                       Length&                numberOfGlyphs)
{
  const GlyphIndex* const charactersToGlyphBuffer   = visualModel->mCharactersToGlyph.Begin();
  const Length* const     glyphsPerCharacterBuffer  = visualModel->mGlyphsPerCharacter.Begin();
  const GlyphInfo* const  glyphInfoBuffer           = visualModel->mGlyphs.Begin();
  Vector<CharacterIndex>& glyphToCharacterMap       = visualModel->mGlyphsToCharacters;
  const CharacterIndex*   glyphToCharacterMapBuffer = glyphToCharacterMap.Begin();
  const float             modelCharacterSpacing     = visualModel->GetCharacterSpacing();

  // Get the character-spacing runs.
  const Vector<CharacterSpacingGlyphRun>& characterSpacingGlyphRuns = visualModel->GetCharacterSpacingGlyphRuns();

  //Takes the character index, obtains the glyph index (and the number of Glyphs) from it and finally gets the glyph metrics.
  glyphIndex     = *(charactersToGlyphBuffer + index);
  numberOfGlyphs = *(glyphsPerCharacterBuffer + index);

  float calculatedAdvance = 0.f;

  const float characterSpacing = GetGlyphCharacterSpacing(glyphIndex, characterSpacingGlyphRuns, modelCharacterSpacing);
  calculatedAdvance            = GetCalculatedAdvance(*(logicalModel->mText.Begin() + (*(glyphToCharacterMapBuffer + glyphIndex))), characterSpacing, (*(visualModel->mGlyphs.Begin() + glyphIndex)).advance);

  // Get the metrics for the group of glyphs.
  GetGlyphsMetrics(glyphIndex,
                   numberOfGlyphs,
                   glyphMetrics,
                   glyphInfoBuffer,
                   metrics,
                   calculatedAdvance);
}

float GetCalculatedAdvance(unsigned int character, float characterSpacing, float advance)
{
  //Gets the final advance value by adding the CharacterSpacing value to it
  //In some cases the CharacterSpacing should not be added. Ex: when the glyph is a ZWSP (Zero Width Space)
  return (TextAbstraction::IsZeroWidthNonJoiner(character) || TextAbstraction::IsZeroWidthJoiner(character) ||
          TextAbstraction::IsZeroWidthSpace(character) || TextAbstraction::IsNewParagraph(character) ||
          TextAbstraction::IsLeftToRightMark(character) || TextAbstraction::IsRightToLeftMark(character))
           ? advance
           : advance + characterSpacing;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
