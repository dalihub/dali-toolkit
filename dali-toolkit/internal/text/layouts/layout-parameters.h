#ifndef __DALI_TOOLKIT_TEXT_LAYOUT_PARAMETERS_H__
#define __DALI_TOOLKIT_TEXT_LAYOUT_PARAMETERS_H__

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
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief Struct used to pass parameters.
 */
struct LayoutParameters
{
  /**
   * Constructor with the needed parameters to layout the text.
   *
   * @param[in] boundingBox The size of the box containing the text.
   * @param[in] lineBreakInfoBuffer The line break info.
   * @param[in] wordBreakInfoBuffer The word break info.
   * @param[in] totalNumberOfGlyphs The number of glyphs.
   * @param[in] glyphsBuffer A vector with glyphs.
   * @param[in] glyphsToCharactersBuffer Vector with indices pointing the first character of each glyph.
   * @param[in] charactersPerGlyphBuffer Vector with the number of characters that forms each glyph.
   */
  LayoutParameters( const Vector2& boundingBox,
                    const LineBreakInfo* const lineBreakInfoBuffer,
                    const WordBreakInfo* const wordBreakInfoBuffer,
                    Length totalNumberOfGlyphs,
                    const GlyphInfo* const glyphsBuffer,
                    const CharacterIndex* const glyphsToCharactersBuffer,
                    const Length* const charactersPerGlyphBuffer )
  : boundingBox( boundingBox ),
    lineBreakInfoBuffer( lineBreakInfoBuffer ),
    wordBreakInfoBuffer( wordBreakInfoBuffer ),
    totalNumberOfGlyphs( totalNumberOfGlyphs ),
    glyphsBuffer( glyphsBuffer ),
    glyphsToCharactersBuffer( glyphsToCharactersBuffer ),
    charactersPerGlyphBuffer( charactersPerGlyphBuffer )
  {}

  Vector2                     boundingBox;
  const LineBreakInfo* const  lineBreakInfoBuffer;
  const WordBreakInfo* const  wordBreakInfoBuffer;
  Length                      totalNumberOfGlyphs;
  const GlyphInfo* const      glyphsBuffer;
  const CharacterIndex* const glyphsToCharactersBuffer;
  const Length* const         charactersPerGlyphBuffer;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_LAYOUT_PARAMETERS_H__
