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
 * @param[in] characterColorRuns The color runs in characters (set in the mark-up string).
 * @param[in] charactersToGlyph Conversion table from characters to glyphs.
 * @param[in] glyphsPerCharacter Table with the number of glyphs for each character.
 * @param[out] glyphColorRuns The color runs in glyphs.
 */
void SetColorSegmentationInfo( const Vector<ColorRun>& characterColorRuns,
                               const Vector<GlyphIndex>& charactersToGlyph,
                               const Vector<Length>& glyphsPerCharacter,
                               Vector<ColorGlyphRun>& glyphColorRuns );

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_COLOR_SEGMENTATION_H__
