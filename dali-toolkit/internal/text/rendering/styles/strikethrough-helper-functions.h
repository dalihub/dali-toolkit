#ifndef DALI_TOOLKIT_TEXT_RENDERING_STYLES_STRIKETHROUGH_HELPER_FUNCTIONS_H
#define DALI_TOOLKIT_TEXT_RENDERING_STYLES_STRIKETHROUGH_HELPER_FUNCTIONS_H

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
#include <dali-toolkit/internal/text/strikethrough-glyph-run.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Whether the glyph at index is strikethrough or not. If true then return iterator to the run containes index.
 *
 * @param[in] index the index of glyph.
 * @param[in] strikethroughRuns the strikethrough runs.
 * @param[out] currentStrikethroughGlyphRunIt the iterator of current strikethrough glyph run.
 *
 * @return true if glyph at index is strikethrough
 */
bool IsGlyphStrikethrough(GlyphIndex                                    index,
                          const Vector<StrikethroughGlyphRun>&          strikethroughRuns,
                          Vector<StrikethroughGlyphRun>::ConstIterator& currentStrikethroughGlyphRunIt);

/**
 * @brief Check the current strikethrough glyph run iterator if not empty and isGlyphStrikethrough is true then return its StrikethroughProperties. Otherwise return the common strikethrough properties.
 *
 * @param[in] index the index of glyph.
 * @param[in] isGlyphStrikethrough whether the glyph is strikethrough.
 * @param[in] strikethroughRuns the strikethrough runs.
 * @param[in] currentStrikethroughGlyphRunIt the iterator of current strikethrough glyph run.
 * @param[in] commonStrikethroughProperties the common strikethrough properties.
 *
 * @return the determined strikethrough properties
 */
StrikethroughStyleProperties GetCurrentStrikethroughProperties(GlyphIndex                                    index,
                                                               const bool&                                   isGlyphStrikethrough,
                                                               const Vector<StrikethroughGlyphRun>&          strikethroughRuns,
                                                               Vector<StrikethroughGlyphRun>::ConstIterator& currentStrikethroughGlyphRunIt,
                                                               const StrikethroughStyleProperties&           commonStrikethroughProperties);

/**
 * @brief Calculate the current strikethrough height and update maximum strikethrough height
 *
 * @param[inout] currentStrikethroughHeight the current strikethrough height.
 * @param[inout] maxStrikethroughHeight the maximum strikethrough height.
 *
 */
void CalcualteStrikethroughHeight(float& currentStrikethroughHeight, float& maxStrikethroughHeight);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_RENDERING_STYLES_STRIKETHROUGH_HELPER_FUNCTIONS_H