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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
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
 * @param[in] startCharacterIndex The character from where the text is shaped.
 * @param[in] startGlyphIndex The glyph from where the text is shaped.
 * @param[in] numberOfCharacters The number of characters to be shaped.
 * @param[out] glyphs Vector of glyphs in the visual order.
 * @param[out] glyphToCharacterMap Vector containing the first character in the logical model that each glyph relates to.
 * @param[out] charactersPerGlyph Vector containing the number of characters per glyph.
 * @param[out] newParagraphGlyphs Vector containing the indices to the new paragraph glyphs.
 */
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
                Vector<GlyphIndex>& newParagraphGlyphs );

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_SHAPER_H__
