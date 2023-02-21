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

// CLASS HEADER
#include <dali-toolkit/internal/text/rendering/styles/character-spacing-helper-functions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
float GetGlyphCharacterSpacing(const GlyphIndex&                       index,
                               const Vector<CharacterSpacingGlyphRun>& characterSpacingGlyphRuns,
                               const float                             modelCharacterSpacing)
{
  float characterSpacing = modelCharacterSpacing;
  for(Vector<CharacterSpacingGlyphRun>::ConstIterator it    = characterSpacingGlyphRuns.Begin(),
                                                      endIt = characterSpacingGlyphRuns.End();
      it != endIt;
      ++it)
  {
    const CharacterSpacingGlyphRun& run = *it;

    if((run.glyphRun.glyphIndex <= index) && (index < run.glyphRun.glyphIndex + run.glyphRun.numberOfGlyphs))
    {
      // Retrieve the latest run to handle the nested case.
      //TODO: Using reverse Iterator will improve performance in this loop
      characterSpacing = run.value;
    }
  }

  return characterSpacing;
}
} // namespace Text

} // namespace Toolkit

} // namespace Dali
