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

// CLASS HEADER
#include <dali-toolkit/internal/text/visual-model-impl.h>

// EXTERNAL INCLUDES
#include <memory.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

VisualModelPtr VisualModel::New()
{
  return VisualModelPtr( new VisualModel() );
}

void VisualModel::SetGlyphs( const GlyphInfo* glyphs,
                             const CharacterIndex* characterIndices,
                             const Length* charactersPerGlyph,
                             Length numberOfGlyphs )
{
  if( 0u == numberOfGlyphs )
  {
    mGlyphs.Clear();
    mGlyphsToCharacters.Clear();
    mCharactersToGlyph.Clear();
    mCharactersPerGlyph.Clear();
    mGlyphsPerCharacter.Clear();
  }
  else
  {
    if( NULL != glyphs )
    {
      mGlyphs.Resize( numberOfGlyphs );
      memcpy( mGlyphs.Begin(), glyphs, numberOfGlyphs * sizeof( GlyphInfo ) );
    }

    if( NULL != characterIndices )
    {
      mGlyphsToCharacters.Resize( numberOfGlyphs );
      memcpy( mGlyphsToCharacters.Begin(), characterIndices, numberOfGlyphs * sizeof( CharacterIndex ) );
    }

    if( NULL != charactersPerGlyph )
    {
      mCharactersPerGlyph.Resize( numberOfGlyphs );
      memcpy( mCharactersPerGlyph.Begin(), charactersPerGlyph, numberOfGlyphs * sizeof( Length ) );

      // Build the glyphs per character table.
      CreateGlyphsPerCharacterTable();

      // Build the characters to glyph conversion table.
      CreateCharacterToGlyphTable();
    }
  }
}

void VisualModel::CreateCharacterToGlyphTable( Length numberOfCharacters )
{
  // 1) Reserve some space for the characters to avoid reallocations.
  if( 0u == numberOfCharacters )
  {
    // If no number of characters is given, just set something sensible to avoid reallocations.
    numberOfCharacters = static_cast<Length> ( static_cast<float>( mGlyphs.Count() ) * 1.3f );
  }
  mCharactersToGlyph.Reserve( numberOfCharacters );

  DALI_ASSERT_DEBUG( mGlyphsPerCharacter.Count() != 0u ||
                     ( 0u == numberOfCharacters ) );

  const Length* const glyphsPerCharacterBuffer = mGlyphsPerCharacter.Begin();

  // 2) Traverse the glyphs and set the glyph indices per character.

  // Index to the glyph.
  GlyphIndex glyphIndex = 0u;
  CharacterIndex characterIndex = 0u;
  for( Vector<Length>::ConstIterator it = mCharactersPerGlyph.Begin(),
         endIt = mCharactersPerGlyph.End();
       it != endIt;
       ++it )
  {
    const Length numberOfCharactersPerGlyph = *it;

    Length numberOfGlyphs = 0u;
    // Set the glyph indices.
    for( Length index = 0u; index < numberOfCharactersPerGlyph; ++index, ++characterIndex )
    {
      mCharactersToGlyph.PushBack( glyphIndex );
      numberOfGlyphs += *( glyphsPerCharacterBuffer + characterIndex );
    }
    glyphIndex += numberOfGlyphs;
  }
}

void VisualModel::CreateGlyphsPerCharacterTable( Length numberOfCharacters )
{
  // 1) Reserve some space for the characters to avoid reallocations.
  if( 0u == numberOfCharacters )
  {
    // If no number of characters is given, just set something sensible to avoid reallocations.
    numberOfCharacters = static_cast<Length> ( static_cast<float>( mGlyphs.Count() ) * 1.3f );
  }
  mGlyphsPerCharacter.Reserve( numberOfCharacters );

  // 2) Traverse the glyphs and set the number of glyphs per character.

  // The number of 'characters per glyph' equal to zero.
  Length zeroCharactersPerGlyph = 0u;

  for( Vector<Length>::ConstIterator it = mCharactersPerGlyph.Begin(),
         endIt = mCharactersPerGlyph.End();
       it != endIt;
       ++it )
  {
    const Length numberOfCharactersPerGlyph = *it;

    // Set the glyphs per character.
    if( 0u == numberOfCharactersPerGlyph )
    {
      ++zeroCharactersPerGlyph;
    }
    else
    {
      const Length numberOfZeroGlyphsPerCharacter = ( numberOfCharactersPerGlyph - 1u );
      for( Length zeroIndex = 0u; zeroIndex < numberOfZeroGlyphsPerCharacter ; ++zeroIndex )
      {
        mGlyphsPerCharacter.PushBack( 0u );
      }

      mGlyphsPerCharacter.PushBack( 1u + zeroCharactersPerGlyph );

      zeroCharactersPerGlyph = 0u;
    }
  }
}

Length VisualModel::GetNumberOfGlyphs() const
{
  return mGlyphs.Count();
}

void VisualModel::GetGlyphs( GlyphInfo* glyphs,
                             GlyphIndex glyphIndex,
                             Length numberOfGlyphs ) const
{
  memcpy( glyphs, mGlyphs.Begin() + glyphIndex, numberOfGlyphs * sizeof( GlyphInfo ) );
}

const GlyphInfo& VisualModel::GetGlyphInfo( GlyphIndex glyphIndex ) const
{
  return mGlyphs[glyphIndex];
}

void VisualModel::ReplaceGlyphs( GlyphIndex glyphIndex,
                                 Length numberOfGlyphsToRemove,
                                 const GlyphInfo* const glyphs,
                                 const Length* const numberOfCharacters,
                                 Length numberOfGlyphsToInsert )
{
}

CharacterIndex VisualModel::GetCharacterIndex( GlyphIndex glyphIndex ) const
{
  return mGlyphsToCharacters[glyphIndex];
}

Length VisualModel::GetCharactersPerGlyph( GlyphIndex glyphIndex ) const
{
  return mCharactersPerGlyph[glyphIndex];
}

GlyphIndex VisualModel::GetGlyphIndex( CharacterIndex characterIndex ) const
{
  return mCharactersToGlyph[characterIndex];
}

void VisualModel::GetCharacterToGlyphMap( GlyphIndex* characterToGlyphMap,
                                          CharacterIndex characterIndex,
                                          Length numberOfCharacters ) const
{
  memcpy( characterToGlyphMap, mCharactersToGlyph.Begin() + characterIndex, numberOfCharacters * sizeof( GlyphIndex ) );
}

void VisualModel::GetGlyphToCharacterMap( CharacterIndex* glyphToCharacter,
                                          GlyphIndex glyphIndex,
                                          Length numberOfGlyphs ) const
{
  memcpy( glyphToCharacter, mGlyphsToCharacters.Begin() + glyphIndex, numberOfGlyphs * sizeof( CharacterIndex ) );
}

void VisualModel::GetCharactersPerGlyphMap( Length* charactersPerGlyph,
                                            GlyphIndex glyphIndex,
                                            Length numberOfGlyphs ) const
{
  memcpy( charactersPerGlyph, mCharactersPerGlyph.Begin() + glyphIndex, numberOfGlyphs * sizeof( Length ) );
}

void VisualModel::GetGlyphsPerCharacterMap( Length* glyphsPerCharacter,
                                            CharacterIndex characterIndex,
                                            Length numberOfCharacters ) const
{
  memcpy( glyphsPerCharacter, mGlyphsPerCharacter.Begin() + characterIndex, numberOfCharacters * sizeof( Length ) );
}

void VisualModel::SetGlyphPositions( const Vector2* glyphPositions,
                                     Length numberOfGlyphs )
{
  if( 0u == numberOfGlyphs )
  {
    mGlyphPositions.Clear();
  }
  else
  {
    mGlyphPositions.Resize( numberOfGlyphs );
    memcpy( mGlyphPositions.Begin(), glyphPositions, numberOfGlyphs * sizeof( Vector2 ) );
  }
}

Length VisualModel::GetNumberOfGlyphPositions() const
{
  return mGlyphPositions.Count();
}

void VisualModel::GetGlyphPositions( Vector2* glyphPositions,
                                     GlyphIndex glyphIndex,
                                     Length numberOfGlyphs ) const
{
  memcpy( glyphPositions, mGlyphPositions.Begin() + glyphIndex, numberOfGlyphs * sizeof( Vector2 ) );
}

const Vector2& VisualModel::GetGlyphPosition( GlyphIndex glyphIndex ) const
{
  return *( mGlyphPositions.Begin() + glyphIndex );
}

void VisualModel::ReplaceGlyphPositions( GlyphIndex glyphIndex,
                                         Length numberOfGlyphsToRemove,
                                         const Vector2* const positions,
                                         Length numberOfGlyphsToInsert )
{
}

void VisualModel::SetLines( const LineRun* const lines,
                            Length numberOfLines )
{
  if( 0u == numberOfLines )
  {
    mLines.Clear();
  }
  else
  {
    mLines.Resize( numberOfLines );
    memcpy( mLines.Begin(), lines, numberOfLines * sizeof( LineRun ) );
  }
}

Length VisualModel::GetNumberOfLines() const
{
  return mLines.Count();
}

void VisualModel::GetLines( LineRun* lines,
                            LineIndex lineIndex,
                            Length numberOfLines ) const
{
  memcpy( lines, mLines.Begin() + lineIndex, numberOfLines * sizeof( LineRun ) );
}

void VisualModel::GetNumberOfLines( GlyphIndex glyphIndex,
                                    Length numberOfGlyphs,
                                    LineIndex& firstLine,
                                    Length& numberOfLines ) const
{
  // Initialize the number of lines and the first line.
  firstLine = 0u;
  numberOfLines = 0u;
  bool firstLineFound = false;

  const GlyphIndex lastGlyphIndex = glyphIndex + numberOfGlyphs;

  // Traverse the lines and count those lines within the range of glyphs.
  for( Vector<LineRun>::ConstIterator it = mLines.Begin(),
         endIt = mLines.End();
       it != endIt;
       ++it )
  {
    const LineRun& line = *it;

    if( ( line.glyphIndex + line.numberOfGlyphs > glyphIndex ) &&
        ( lastGlyphIndex > line.glyphIndex ) )
    {
      firstLineFound = true;
      ++numberOfLines;
    }
    else if( lastGlyphIndex <= line.glyphIndex )
    {
      // nothing else to do.
      break;
    }

    if( !firstLineFound )
    {
      ++firstLine;
    }
  }
}

void VisualModel::GetLinesOfGlyphRange( LineRun* lines,
                                        GlyphIndex glyphIndex,
                                        Length numberOfGlyphs ) const
{
  LineIndex firstLine = 0u;
  Length numberOfLines = 0u;

  GetNumberOfLines( glyphIndex,
                    numberOfGlyphs,
                    firstLine,
                    numberOfLines );

  memcpy( lines, mLines.Begin() + firstLine, numberOfLines * sizeof( LineRun ) );
}

void VisualModel::ReplaceLines( GlyphIndex glyphIndex,
                                Length numberOfGlyphsToRemove,
                                const LineRun* const lines,
                                Length numberOfGlyphsToInsert )
{
}

void VisualModel::SetNaturalSize( const Vector2& size  )
{
  mNaturalSize = size;
}

const Vector2& VisualModel::GetNaturalSize() const
{
  return mNaturalSize;
}

void VisualModel::SetActualSize( const Vector2& size )
{
  mActualSize = size;
}

const Vector2& VisualModel::GetActualSize() const
{
  return mActualSize;
}

VisualModel::~VisualModel()
{
}

VisualModel::VisualModel()
{
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
