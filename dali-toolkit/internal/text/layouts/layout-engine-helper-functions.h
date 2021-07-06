#ifndef DALI_TOOLKIT_TEXT_LAYOUT_ENGINE_HELPER_FUNCTIONS_H
#define DALI_TOOLKIT_TEXT_LAYOUT_ENGINE_HELPER_FUNCTIONS_H

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

// INTERNAL INCLUDE
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/visual-model-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Layout
{
/**
   * @brief Calculate glyph positions when set glyph positions for left to right.
   *
   * @param[in] visualModel The visual model.
   * @param[in] logicalModel The logical model.
   * @param[in] interGlyphExtraAdvance Extra advance added to each glyph.
   * @param[in] numberOfGlyphs The number of glyphs.
   * @param[in] startIndexForGlyph The index of the first glyph in the line.
   * @param[in] startIndexForGlyphPositions The index of the first glyph for GlyphPositions.
   * @param[in,out] glyphPositionsBuffer The glyph positions buffer.
   * @param[in,out] penX The current position for the horizontal pen.
   */
void CalculateGlyphPositionsLTR(const VisualModelPtr&  visualModel,
                                const LogicalModelPtr& logicalModel,
                                const float            interGlyphExtraAdvance,
                                const Length           numberOfGlyphs,
                                const GlyphIndex       startIndexForGlyph,
                                const GlyphIndex       startIndexForGlyphPositions,
                                Vector2*               glyphPositionsBuffer,
                                float&                 penX);

/**
   * @brief Calculate glyph positions when set glyph positions for right to left.
   *
   * @param[in] visualModel The visual model.
   * @param[in] logicalModel The logical model.
   * @param[in] bidiLineIndex Index to the line's bidirectional info.
   * @param[in] startGlyphIndex The index of the first glyph in the line.
   * @param[in,out] glyphPositionsBuffer The glyph positions buffer.
   * @param[in] characterVisualIndex The character visual index.
   * @param[in] characterLogicalIndex The character logical index.
   * @param[in,out] penX The current position for the horizontal pen.
   */
void CalculateGlyphPositionsRTL(const VisualModelPtr&            visualModel,
                                const LogicalModelPtr&           logicalModel,
                                const BidirectionalLineRunIndex& bidiLineIndex,
                                const GlyphIndex&                startGlyphIndex,
                                Vector2*                         glyphPositionsBuffer,
                                CharacterIndex&                  characterVisualIndex,
                                CharacterIndex&                  characterLogicalIndex,
                                float&                           penX);

/**
   * @brief Calculate glyph positions when set glyph positions for right to left.
   *
   * @param[in] visualModel The visual model.
   * @param[in] textBuffer The text buffer.
   * @param[in] startGlyphIndex The index of the first glyph in the line.
   * @param[in] interGlyphExtraAdvance Extra advance added to each glyph.
   * @param[in] bidiLineCharacterRun The initial character index within the whole text and the number of characters of the run for BidirectionalLine.
   * @param[in] bidiLineVisualToLogicalMap Pointer to the visual to logical map table for BidirectionalLine.
   * @param[in,out] glyphPositionsBuffer The glyph positions buffer.
   * @param[in,out] characterLogicalIndex The character logical index.
   * @param[in,out] penX The current position for the horizontal pen.
   */
void TraversesCharactersForGlyphPositionsRTL(const VisualModelPtr&  visualModel,
                                             const Character* const textBuffer,
                                             const GlyphIndex&      startGlyphIndex,
                                             const float            interGlyphExtraAdvance,
                                             const CharacterRun&    bidiLineCharacterRun,
                                             CharacterIndex*        bidiLineVisualToLogicalMap,
                                             Vector2*               glyphPositionsBuffer,
                                             CharacterIndex&        characterLogicalIndex,
                                             float&                 penX);

} // namespace Layout

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_LAYOUT_ENGINE_HELPER_FUNCTIONS_H