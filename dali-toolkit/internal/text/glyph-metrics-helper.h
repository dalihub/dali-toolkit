#ifndef DALI_TOOLKIT_TEXT_GLYPH_METRICS_HELPER_H
#define DALI_TOOLKIT_TEXT_GLYPH_METRICS_HELPER_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/metrics.h>
#include <dali-toolkit/internal/text/visual-model-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Some characters can be shaped in more than one glyph.
 * This struct is used to retrieve metrics from these group of glyphs.
 */
struct GlyphMetrics
{
  GlyphMetrics()
  : fontId(0u),
    fontHeight(0.f),
    width(0.f),
    advance(0.f),
    ascender(0.f),
    xBearing(0.f)
  {
  }

  ~GlyphMetrics()
  {
  }

  FontId fontId;     ///< The font id of the glyphs.
  float  fontHeight; ///< The font's height of those glyphs.
  float  width;      ///< The width of the group.
  float  advance;    ///< The sum of all the advances of all the glyphs.
  float  ascender;   ///< The font's ascender.
  float  xBearing;   ///< The x bearing of the group.
};

/**
 * @brief Returns the number of glyphs of a group of glyphs.
 *
 * @param[in] glyphIndex The first glyph of the group.
 * @param[in] lastGlyphPlusOne Index to one after the last glyph.
 * @param[in] charactersPerGlyphBuffer The number of characters per glyph buffer.
 *
 * @return The number of glyphs of the group.
 */
Length GetNumberOfGlyphsOfGroup(GlyphIndex          glyphIndex,
                                GlyphIndex          lastGlyphPlusOne,
                                const Length* const charactersPerGlyphBuffer);

/**
 * @brief Get some glyph's metrics of a group of glyphs formed as a result of shaping one character.
 *
 * @param[in] glyphIndex The index to the first glyph.
 * @param[in] numberOfGlyphs The number of glyphs.
 * @param[out] glyphMetrics Some glyph metrics (font height, advance, ascender and x bearing).
 * @param[in] glyphsBuffer The glyphs buffer.
 * @param[in] metrics Used to access metrics from FontClient.
 * @param[in] calculatedAdvance The final advance value obtained by adding the CharacterSpacing value to the original advance. In some cases the CharacterSpacing should not be added. Ex: when the glyph is a ZWSP (Zero Width Space)
 */
void GetGlyphsMetrics(GlyphIndex             glyphIndex,
                      Length                 numberOfGlyphs,
                      GlyphMetrics&          glyphMetrics,
                      const GlyphInfo* const glyphsBuffer,
                      MetricsPtr&            metrics,
                      float                  calculatedAdvance);

/**
 * @brief Gets the final advance value by adding the CharacterSpacing value to it
 *In some cases the CharacterSpacing should not be added. Ex: when the glyph is a ZWSP (Zero Width Space)
 *
 * @param[in] character The character of which the advance is to be calculated.
 * @param[in] characterSpacing The character spacing to be added to the advance.
 * @param[in] advance The original advance.
 *
 * @return The calculated advance
 */
float GetCalculatedAdvance(unsigned int character, float characterSpacing, float advance);

/**
 * @brief Takes the character index, obtains the glyph index (and the number of Glyphs) from it and finally gets the glyph metrics.
 *
 * @param[in] index The character index.
 * @param[in] visualModel The visual model.
 * @param[in] logicalModel The logical model.
 * @param[in] metrics Used to access metrics from FontClient.
 * @param[out] glyphMetrics Some glyph metrics (font height, advance, ascender and x bearing).
 * @param[out] glyphIndex The glyph index obtained from the character index.
 * @param[out] numberOfGlyphs The number of glyphs in the character of which the index was passed to the function.
 *
 */
void GetGlyphMetricsFromCharacterIndex(CharacterIndex         index,
                                       const VisualModelPtr&  visualModel,
                                       const LogicalModelPtr& logicalModel,
                                       MetricsPtr&            metrics,
                                       GlyphMetrics&          glyphMetrics,
                                       GlyphIndex&            glyphIndex,
                                       Length&                numberOfGlyphs);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_GLYPH_METRICS_HELPER_H
