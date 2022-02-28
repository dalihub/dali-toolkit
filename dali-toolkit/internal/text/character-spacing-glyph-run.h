#ifndef DALI_TOOLKIT_TEXT_CHARACTER_SPACING_GLYPH_RUN_H
#define DALI_TOOLKIT_TEXT_CHARACTER_SPACING_GLYPH_RUN_H

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
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/glyph-run.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Run of character-spacing glyphs with same properties.
 */
struct CharacterSpacingGlyphRun
{
  /**
   * Default constructor to set the default values of bitfields
   */
  CharacterSpacingGlyphRun()
  : glyphRun{},
    value{0.f} //The default value is 0.f which does nothing.
  {
  }

  GlyphRun glyphRun; ///< The initial glyph index and the number of glyphs in the run.
  float    value;    ///< The spaces between characters in Pixels.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_CHARACTER_SPACING_GLYPH_RUN_H
