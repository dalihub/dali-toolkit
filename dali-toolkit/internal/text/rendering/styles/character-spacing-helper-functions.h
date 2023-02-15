#ifndef DALI_TOOLKIT_TEXT_RENDERING_STYLES_CHARACTER_SPACING_HELPER_FUNCTIONS_H
#define DALI_TOOLKIT_TEXT_RENDERING_STYLES_CHARACTER_SPACING_HELPER_FUNCTIONS_H

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
#include <dali-toolkit/internal/text/character-spacing-glyph-run.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Check the glyph at index is had character-spacing or not then return its value. Otherwise return the model character-spacing height.
 *
 * @param[in] index the index of glyph.
 * @param[in] characterSpacingGlyphRuns the character-spacing glyph runs.
 * @param[in] modelCharacterSpacing the model character-spacing value.
 *
 * @return the determined character-spacing value
 */
float GetGlyphCharacterSpacing(const GlyphIndex&                       index,
                               const Vector<CharacterSpacingGlyphRun>& characterSpacingGlyphRuns,
                               const float                             modelCharacterSpacing);
} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_RENDERING_STYLES_CHARACTER_SPACING_HELPER_FUNCTIONS_H
