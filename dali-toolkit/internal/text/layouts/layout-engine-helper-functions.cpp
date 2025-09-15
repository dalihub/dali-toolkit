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
#include <dali-toolkit/internal/text/layouts/layout-engine-helper-functions.h>

// INTERNAL INCLUDE
#include <dali-toolkit/internal/text/glyph-metrics-helper.h>
#include <dali-toolkit/internal/text/rendering/styles/character-spacing-helper-functions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Layout
{
void CalculateGlyphPositionsLTR(const VisualModelPtr&  visualModel,
                                const LogicalModelPtr& logicalModel,
                                const float            interGlyphExtraAdvance,
                                const Length           numberOfGlyphs,
                                const GlyphIndex       startIndexForGlyph,
                                const GlyphIndex       startIndexForGlyphPositions,
                                Vector2*               glyphPositionsBuffer,
                                float&                 penX)
{
  const GlyphInfo* const glyphsBuffer          = visualModel->mGlyphs.Begin();
  const float            modelCharacterSpacing = visualModel->GetCharacterSpacing();
  const Character* const textBuffer            = logicalModel->mText.Begin();

  Vector<CharacterIndex>& glyphToCharacterMap       = visualModel->mGlyphsToCharacters;
  const CharacterIndex*   glyphToCharacterMapBuffer = glyphToCharacterMap.Begin();

  // Get the character-spacing runs.
  const Vector<CharacterSpacingGlyphRun>& characterSpacingGlyphRuns = visualModel->GetCharacterSpacingGlyphRuns();

  float calculatedAdvance = 0.f;

  unsigned int numberOfGlyphsToCalculate = numberOfGlyphs;
  if(startIndexForGlyph + numberOfGlyphs < (unsigned int)visualModel->mGlyphs.Count())
  {
    ++numberOfGlyphsToCalculate;
  }

  for(GlyphIndex i = 0u; i < numberOfGlyphsToCalculate; ++i)
  {
    const GlyphInfo& glyph    = *(glyphsBuffer + startIndexForGlyph + i);
    Vector2&         position = *(glyphPositionsBuffer + startIndexForGlyphPositions + i);

    position.x = penX + glyph.xBearing;
    position.y = -glyph.yBearing;

    const float characterSpacing = GetGlyphCharacterSpacing((startIndexForGlyph + i), characterSpacingGlyphRuns, modelCharacterSpacing);
    calculatedAdvance            = GetCalculatedAdvance(*(textBuffer + (*(glyphToCharacterMapBuffer + (startIndexForGlyph + i)))), characterSpacing, glyph.advance);
    penX += (calculatedAdvance + interGlyphExtraAdvance);
  }
}

void CalculateGlyphPositionsRTL(const VisualModelPtr&            visualModel,
                                const LogicalModelPtr&           logicalModel,
                                const BidirectionalLineRunIndex& bidiLineIndex,
                                const GlyphIndex&                startGlyphIndex,
                                Vector2*                         glyphPositionsBuffer,
                                CharacterIndex&                  characterVisualIndex,
                                CharacterIndex&                  characterLogicalIndex,
                                float&                           penX)
{
  const Character* const          textBuffer               = logicalModel->mText.Begin();
  const BidirectionalLineInfoRun& bidiLine                 = logicalModel->mBidirectionalLineInfo[bidiLineIndex];
  const GlyphInfo* const          glyphsBuffer             = visualModel->mGlyphs.Begin();
  const GlyphIndex* const         charactersToGlyphsBuffer = visualModel->mCharactersToGlyph.Begin();
  const float                     modelCharacterSpacing    = visualModel->GetCharacterSpacing();

  // Get the character-spacing runs.
  const Vector<CharacterSpacingGlyphRun>& characterSpacingGlyphRuns = visualModel->GetCharacterSpacingGlyphRuns();

  float calculatedAdvance = 0.f;

  while(TextAbstraction::IsWhiteSpace(*(textBuffer + characterVisualIndex)))
  {
    const GlyphIndex glyphIndex = *(charactersToGlyphsBuffer + characterVisualIndex);
    const GlyphInfo& glyph      = *(glyphsBuffer + glyphIndex);

    Vector2& position = *(glyphPositionsBuffer + static_cast<std::size_t>(glyphIndex - startGlyphIndex));
    position.x        = penX;
    position.y        = -glyph.yBearing;

    const float characterSpacing = GetGlyphCharacterSpacing(glyphIndex, characterSpacingGlyphRuns, modelCharacterSpacing);
    calculatedAdvance            = GetCalculatedAdvance(*(textBuffer + characterVisualIndex), characterSpacing, glyph.advance);
    penX += calculatedAdvance;

    ++characterLogicalIndex;
    characterVisualIndex = bidiLine.characterRun.characterIndex + *(bidiLine.visualToLogicalMap + characterLogicalIndex);
  }
}

void TraversesCharactersForGlyphPositionsRTL(const VisualModelPtr&  visualModel,
                                             const Character* const textBuffer,
                                             const GlyphIndex&      startGlyphIndex,
                                             const float            interGlyphExtraAdvance,
                                             const CharacterRun&    bidiLineCharacterRun,
                                             CharacterIndex*        bidiLineVisualToLogicalMap,
                                             Vector2*               glyphPositionsBuffer,
                                             CharacterIndex&        characterLogicalIndex,
                                             float&                 penX)
{
  const GlyphInfo* const  glyphsBuffer             = visualModel->mGlyphs.Begin();
  const GlyphIndex* const charactersToGlyphsBuffer = visualModel->mCharactersToGlyph.Begin();
  const float             modelCharacterSpacing    = visualModel->GetCharacterSpacing();
  const Length* const     glyphsPerCharacterBuffer = visualModel->mGlyphsPerCharacter.Begin();

  // Get the character-spacing runs.
  const Vector<CharacterSpacingGlyphRun>& characterSpacingGlyphRuns = visualModel->GetCharacterSpacingGlyphRuns();

  float calculatedAdvance = 0.f;

  for(; characterLogicalIndex < bidiLineCharacterRun.numberOfCharacters;
      ++characterLogicalIndex)
  {
    // Convert the character in the logical order into the character in the visual order.
    const CharacterIndex characterVisualIndex = bidiLineCharacterRun.characterIndex + *(bidiLineVisualToLogicalMap + characterLogicalIndex);

    // Get the number of glyphs of the character.
    const Length numberOfGlyphs = *(glyphsPerCharacterBuffer + characterVisualIndex);

    for(GlyphIndex index = 0u; index < numberOfGlyphs; ++index)
    {
      // Convert the character in the visual order into the glyph in the visual order.
      const GlyphIndex glyphIndex = *(charactersToGlyphsBuffer + characterVisualIndex) + index;

      DALI_ASSERT_DEBUG(glyphIndex < visualModel->mGlyphs.Count());

      const GlyphInfo& glyph    = *(glyphsBuffer + glyphIndex);
      Vector2&         position = *(glyphPositionsBuffer + static_cast<std::size_t>(glyphIndex - startGlyphIndex));

      position.x = penX + glyph.xBearing;
      position.y = -glyph.yBearing;

      const float characterSpacing = GetGlyphCharacterSpacing(glyphIndex, characterSpacingGlyphRuns, modelCharacterSpacing);
      calculatedAdvance            = GetCalculatedAdvance(*(textBuffer + characterVisualIndex), characterSpacing, glyph.advance);
      penX += (calculatedAdvance + interGlyphExtraAdvance);
    }
  }
}

} // namespace Layout

} // namespace Text

} // namespace Toolkit

} // namespace Dali
