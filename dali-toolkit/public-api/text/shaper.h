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
#include <dali-toolkit/public-api/text/font-run.h>
#include <dali-toolkit/public-api/text/script-run.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

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

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_SHAPER_H__
