#ifndef DALI_TOOLKIT_TEXT_GLYPH_RUN_H
#define DALI_TOOLKIT_TEXT_GLYPH_RUN_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief A run of consecutive glyphs.
 */
struct GlyphRun
{
  GlyphIndex glyphIndex{0u};     ///< Index to the first glyph.
  Length     numberOfGlyphs{0u}; ///< Number of glyphs in the run.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_GLYPH_RUN_H
