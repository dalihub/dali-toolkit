#ifndef DALI_TOOLKIT_TEXT_RENDERING_STYLES_UNDERLINE_HELPER_FUNCTIONS_H
#define DALI_TOOLKIT_TEXT_RENDERING_STYLES_UNDERLINE_HELPER_FUNCTIONS_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/font-metrics.h>
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/underlined-glyph-run.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Whether the glyph at index is underlined or not. If true then return iterator to the run containes index.
 *
 * @param[in] index the index of glyph.
 * @param[in] underlineRuns the underline runs.
 * @param[out] currentUnderlinedGlyphRunIt the iterator of current underlined glyph run.
 *
 * @return true if glyph at index is underlined
 */
bool IsGlyphUnderlined(GlyphIndex                                 index,
                       const Vector<UnderlinedGlyphRun>&          underlineRuns,
                       Vector<UnderlinedGlyphRun>::ConstIterator& currentUnderlinedGlyphRunIt);

/**
 * @brief Check the current underlined glyph run iterator if not empty and isGlyphUnderlined is true then return its UnderlineProperties. Otherwise return the common underline properties.
 *
 * @param[in] index the index of glyph.
 * @param[in] isGlyphUnderlined whether the glyph is underlined.
 * @param[in] underlineRuns the underline runs.
 * @param[in] currentUnderlinedGlyphRunIt the iterator of current underlined glyph run.
 * @param[in] commonUnderlineProperties the common underline properties.
 *
 * @return the determined underline properties
 */
UnderlineStyleProperties GetCurrentUnderlineProperties(GlyphIndex                                 index,
                                                       const bool&                                isGlyphUnderlined,
                                                       const Vector<UnderlinedGlyphRun>&          underlineRuns,
                                                       Vector<UnderlinedGlyphRun>::ConstIterator& currentUnderlinedGlyphRunIt,
                                                       const UnderlineStyleProperties&            commonUnderlineProperties);

/**
 * @brief Fetch and calculate underline Position using font-metrics
 *
 * @param[in] fontMetrics the font metrics of glyph.
 *
 * @return the underline position
 */
float FetchUnderlinePositionFromFontMetrics(const FontMetrics& fontMetrics);

/**
 * @brief Calculate the current underline height using font-metrics and update maximum underline height
 *
 * @param[in] fontMetrics the font metrics of glyph.
 * @param[inout] currentUnderlineHeight the current underline height.
 * @param[inout] maxUnderlineHeight the maximum underline height.
 *
 */
void CalcualteUnderlineHeight(const FontMetrics& fontMetrics, float& currentUnderlineHeight, float& maxUnderlineHeight);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_RENDERING_STYLES_UNDERLINE_HELPER_FUNCTIONS_H