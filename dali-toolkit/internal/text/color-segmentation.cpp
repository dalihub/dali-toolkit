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

// FILE HEADER
#include <dali-toolkit/internal/text/color-segmentation.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

#include <iostream>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

void SetColorSegmentationInfo( const Vector<ColorRun>& characterColorRuns,
                               const Vector<GlyphIndex>& charactersToGlyph,
                               const Vector<Length>& glyphsPerCharacter,
                               Vector<ColorGlyphRun>& glyphColorRuns )
{
  const VectorBase::SizeType numberOfColorRuns = characterColorRuns.Count();

  if( 0u == numberOfColorRuns )
  {
    // Nothing to do.
    return;
  }

  // Resize the color runs for the glyphs.
  glyphColorRuns.Resize( numberOfColorRuns );

  // Get pointers to the buffers.
  ColorGlyphRun* glyphColorRunsBuffer = glyphColorRuns.Begin();
  const GlyphIndex* const charactersToGlyphBuffer = charactersToGlyph.Begin();
  const Length* const glyphsPerCharacterBuffer = glyphsPerCharacter.Begin();

  // Convert from characters to glyphs.
  Length index = 0u;
  for( Vector<ColorRun>::ConstIterator it = characterColorRuns.Begin(),
         endIt = characterColorRuns.End();
       it != endIt;
       ++it, ++index )
  {
    const ColorRun& colorRun = *it;

    if( 0u < colorRun.characterRun.numberOfCharacters )
    {
      // Get the next color glyph run.
      ColorGlyphRun& colorGlyphRun = *( glyphColorRunsBuffer + index );
      colorGlyphRun.color = colorRun.color;

      // Convert the color run index from character to glyph.
      colorGlyphRun.glyphRun.glyphIndex = *( charactersToGlyphBuffer + colorRun.characterRun.characterIndex );

      // Get the index to the last character of the run.
      const CharacterIndex lastIndex = colorRun.characterRun.characterIndex + colorRun.characterRun.numberOfCharacters - 1u;

      // Calculate the number of glyphs.
      colorGlyphRun.glyphRun.numberOfGlyphs = *( charactersToGlyphBuffer + lastIndex ) + *( glyphsPerCharacterBuffer + lastIndex ) - colorGlyphRun.glyphRun.glyphIndex;
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
