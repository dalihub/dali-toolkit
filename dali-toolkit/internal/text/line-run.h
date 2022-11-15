#ifndef DALI_TOOLKIT_TEXT_LINE_RUN_H
#define DALI_TOOLKIT_TEXT_LINE_RUN_H

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
#include <dali-toolkit/internal/text/character-run.h>
#include <dali-toolkit/internal/text/glyph-run.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief LineRun
 */
struct LineRun
{
  GlyphRun           glyphRun;                      ///< The initial glyph index and the number of glyphs of the run.
  CharacterRun       characterRun;                  ///< The initial character index and the number of characters of the run.
  float              width;                         ///< The line's width.
  float              ascender;                      ///< The line's ascender.
  float              descender;                     ///< The line's descender.
  float              extraLength;                   ///< The length of the white spaces at the end of the line.
  float              alignmentOffset;               ///< The horizontal alignment offset.
  float              lineSpacing;                   ///< The line's spacing
  CharacterDirection direction : 1;                 ///< Direction of the first character of the paragraph.
  bool               ellipsis : 1;                  ///< Wheter ellipsis is added to the line.
  bool               isSplitToTwoHalves;            ///< Whether the second half is defined. When split line to two halves to set Ellipsis in the MIDDLE of line. The second half is the second part of line after Ellipsis.
  GlyphRun           glyphRunSecondHalf;            ///< The initial glyph index and the number of glyphs of the run for the second half of line.
  CharacterRun       characterRunForSecondHalfLine; ///< The initial character index and the number of characters of the run for the second half of line.
};

/**
 * @brief Get the line height for the specified line run.
 *
 * @param[in] lineRun The line runs to get the height for.
 * @param[in] isLastLine whether this is the last line in the text.
 */
float GetLineHeight(const LineRun lineRun, bool isLastLine);

/**
 * @brief Get the line width for the specified line run.
 * The width in the geometry = glyph.advance, not glyph.width.
 *
 * @param[in] lineRun The line runs to get the width for.
 * @return the width of the line.
 */
float GetLineWidth(const LineRun& lineRun);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_LINE_RUN_H
