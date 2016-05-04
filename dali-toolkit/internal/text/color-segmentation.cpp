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

/**
 * @brief Finds a color in the vector of colors.
 *        It inserts the color in the vector if it's not in.
 *
 * @param[in,out] colors The vector of colors.
 * @param[in] color The color to find.
 *
 * @return The index + 1 where the color is in the vector. The index zero is reserved for the default color.
 */
ColorIndex FindColor( Vector<Vector4>& colors,
                      const Vector4& color )
{
  ColorIndex index = 1u; // The index zero is reserved for the default color.
  for( Vector<Vector4>::Iterator it = colors.Begin(),
         endIt = colors.End();
       it != endIt;
       ++it )
  {
    if( color == *it )
    {
      return index;
    }

    ++index;
  }

  colors.PushBack( color );

  return index;
}

void SetColorSegmentationInfo( const Vector<ColorRun>& colorRuns,
                               const Vector<GlyphIndex>& charactersToGlyph,
                               const Vector<Length>& glyphsPerCharacter,
                               CharacterIndex startCharacterIndex,
                               GlyphIndex startGlyphIndex,
                               Length numberOfCharacters,
                               Vector<Vector4>& colors,
                               Vector<ColorIndex>& colorIndices )
{
  if( 0u == charactersToGlyph.Count() )
  {
    // Nothing to do if there is no text.
    return;
  }

  // Get pointers to the buffers.
  const GlyphIndex* const charactersToGlyphBuffer = charactersToGlyph.Begin();
  const Length* const glyphsPerCharacterBuffer = glyphsPerCharacter.Begin();

  // Calculate the number of glyphs to insert.
  const CharacterIndex lastCharacterIndex = startCharacterIndex + numberOfCharacters - 1u;
  const Length numberOfNewGlyphs = *( charactersToGlyphBuffer + lastCharacterIndex ) + *( glyphsPerCharacterBuffer + lastCharacterIndex ) - *( charactersToGlyphBuffer + startCharacterIndex );

  // Reserve space for the new color indices.
  Vector<ColorIndex> newColorIndices;
  newColorIndices.Resize( numberOfNewGlyphs );

  ColorIndex* newColorIndicesBuffer = newColorIndices.Begin();

  // Convert from characters to glyphs.
  Length index = 0u;
  for( Vector<ColorRun>::ConstIterator it = colorRuns.Begin(),
         endIt = colorRuns.End();
       it != endIt;
       ++it, ++index )
  {
    const ColorRun& colorRun = *it;

    if( ( startCharacterIndex < colorRun.characterRun.characterIndex + colorRun.characterRun.numberOfCharacters ) &&
        ( colorRun.characterRun.characterIndex < startCharacterIndex + numberOfCharacters ) )
    {
      if( 0u < colorRun.characterRun.numberOfCharacters )
      {
        // Find the color index.
        const ColorIndex colorIndex = FindColor( colors, colorRun.color );

        // Get the index to the last character of the run.
        const CharacterIndex lastIndex = colorRun.characterRun.characterIndex + colorRun.characterRun.numberOfCharacters - 1u;

        const GlyphIndex glyphIndex = std::max( startGlyphIndex, *( charactersToGlyphBuffer + colorRun.characterRun.characterIndex ) ) - startGlyphIndex;
        // Get the number of glyphs of the run.
        const Length lastGlyphIndexPlusOne = std::min( numberOfNewGlyphs, *( charactersToGlyphBuffer + lastIndex ) + *( glyphsPerCharacterBuffer + lastIndex ) - startGlyphIndex );

        // Set the indices.
        for( GlyphIndex i = glyphIndex; i < lastGlyphIndexPlusOne; ++i )
        {
          *( newColorIndicesBuffer + i ) = colorIndex;
        }
      }
    }
  }

  // Insert the new indices.
  colorIndices.Insert( colorIndices.Begin() + startGlyphIndex,
                       newColorIndices.Begin(),
                       newColorIndices.End() );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
