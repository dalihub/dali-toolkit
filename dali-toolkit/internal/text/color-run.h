#ifndef __DALI_TOOLKIT_TEXT_COLOR_RUN_H__
#define __DALI_TOOLKIT_TEXT_COLOR_RUN_H__

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
#include <dali/public-api/math/vector4.h>

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
 * @brief Run of characters with the same color.
 */
struct ColorRun
{
  CharacterRun characterRun; ///< The initial character index and the number of characters of the run.
  Vector4      color;        ///< The color of the characters.
};

struct ColorGlyphRun
{
  GlyphRun glyphRun; ///< The initial glyph index and the number of glyphs of the run.
  Vector4  color;    ///< The color of the glyphs.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_COLOR_RUN_H__
