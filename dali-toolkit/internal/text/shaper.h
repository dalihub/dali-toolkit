#ifndef __DALI_TOOLKIT_TEXT_SHAPER_H__
#define __DALI_TOOLKIT_TEXT_SHAPER_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali-toolkit/internal/text/font-run.h>
#include <dali-toolkit/internal/text/script-run.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

class LogicalModel;
class VisualModel;

/**
 * Shapes the whole text.
 *
 * @param[in] text Vector of UTF-32 characters.
 * @param[in] lineBreakInfo The line break info.
 * @param[in] scripts Vector containing the script runs for the whole text.
 * @param[in] fonts Vector with validated fonts.
 * @param[out] glyphs Vector of glyphs in the visual order.
 * @param[out] characterIndices Vector containing the first character in the logical model that each glyph relates to.
 * @param[out] charactersPerGlyph Vector containing the number of characters per glyph.
 */
void ShapeText( const Vector<Character>& text,
                const Vector<LineBreakInfo>& lineBreakInfo,
                const Vector<ScriptRun>& scripts,
                const Vector<FontRun>& fonts,
                Vector<GlyphInfo>& glyphs,
                Vector<CharacterIndex>& characterIndices,
                Vector<Length>& charactersPerGlyph );

/**
 * Replaces the shape info of the given range of characters.
 *
 * @pre The @p model needs to have a text set.
 * @pre The @p model needs to have the scripts set.
 * @pre The @p model needs to have the fonts set.
 * @pre The @p model needs to have the bidirectional info set.
 *
 * If the @p numberOfCharactersToRemove is zero, this operation is like an insert.
 * If the @p numberOfCharactersToInsert is zero, this operation is like a remove.
 *
 * @param[in] logicalModel The text's logical model.
 * @param[in,out] visualModel The text's logical model.
 * @param[in] characterIndex Index to the first character.
 * @param[in] numberOfCharactersToRemove The number of characters removed from the text.
 * @param[in] numberOfCharactersToInsert The number of characters inserted in the text.
 */
void ShapeText( const LogicalModel& logicalModel,
                VisualModel& visualModel,
                CharacterIndex characterIndex,
                Length numberOfCharactersToRemove,
                Length numberOfCharactersToInsert );

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_SHAPER_H__
