#ifndef __DALI_TOOLKIT_TEXT_COLOR_SEGMENTATION_H__
#define __DALI_TOOLKIT_TEXT_COLOR_SEGMENTATION_H__

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
#include <dali-toolkit/internal/text/color-run.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

class LogicalModel;

/**
 * @brief Creates color glyph runs.
 *
 * @param[in] colorRuns The color runs in characters (set in the mark-up string).
 * @param[in] charactersToGlyph Conversion table from characters to glyphs.
 * @param[in] glyphsPerCharacter Table with the number of glyphs for each character.
 * @param[in] startCharacterIndex The character from where the text is shaped.
 * @param[in] startGlyphIndex The glyph from where the text is shaped.
 * @param[in] numberOfCharacters The number of characters to be shaped.
 * @param[out] colors The vector of colors.
 * @param[out] colorIndices Indices to the vector of colors.
 */
void SetColorSegmentationInfo( const Vector<ColorRun>& colorRuns,
                               const Vector<GlyphIndex>& charactersToGlyph,
                               const Vector<Length>& glyphsPerCharacter,
                               CharacterIndex startCharacterIndex,
                               GlyphIndex startGlyphIndex,
                               Length numberOfCharacters,
                               Vector<Vector4>& colors,
                               Vector<ColorIndex>& colorIndices );

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_COLOR_SEGMENTATION_H__
