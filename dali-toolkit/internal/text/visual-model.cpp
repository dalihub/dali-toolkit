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
#include <dali-toolkit/internal/text/visual-model.h>

// EXTERNAL INCLUDES
#include <memory.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/vector2.h>
#include <dali-toolkit/internal/text/line-run.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief caches some temporary values of the GetNumberOfLines( glyphIndex, numberOfGlyphs ) operation
 * as they are going to be used in the GetLinesOfGlyphRange() call.
 */
struct GetLineCache
{
  GlyphIndex glyphIndex;     ///< The glyph index.
  Length     numberOfGlyphs; ///< The number of glyphs.
  Length     firstLine;      ///< Index to the first line.
  Length     numberOfLines;  ///< The number of lines.
};

struct VisualModel::Impl
{
  Vector<GlyphInfo>      mGlyphs;             ///< For each glyph, the font's id, glyph's index within the font and glyph's metrics.
  Vector<CharacterIndex> mGlyphsToCharacters; ///< For each glyph, the index of the first character.
  Vector<GlyphIndex>     mCharactersToGlyph;  ///< For each character, the index of the first glyph.
  Vector<Length>         mCharactersPerGlyph; ///< For each glyph, the number of characters that form the glyph.
  Vector<Vector2>        mGlyphPositions;     ///< For each glyph, the position.
  Vector<LineRun>        mLines;              ///< The laid out lines.

  Size                   mNaturalSize;        ///< Size of the text with no line wrapping.
  Size                   mActualSize;         ///< Size of the laid-out text considering the layout properties set.

  GetLineCache           mGetLineCache;       ///< Caches the GetNumberOfLines( glyphIndex, numberOfGlyphs ) operation.
};

VisualModelPtr VisualModel::New()
{
  return VisualModelPtr( new VisualModel() );
}

void VisualModel::SetGlyphs( const GlyphInfo* glyphs,
                             const CharacterIndex* characterIndices,
                             const Length* charactersPerGlyph,
                             Length numberOfGlyphs )
{
  Vector<GlyphInfo>& modelGlyphs = mImpl->mGlyphs;
  modelGlyphs.Resize( numberOfGlyphs );
  memcpy( modelGlyphs.Begin(), glyphs, numberOfGlyphs * sizeof( GlyphInfo ) );

  Vector<CharacterIndex>& modelGlyphsToCharacters = mImpl->mGlyphsToCharacters;
  modelGlyphsToCharacters.Resize( numberOfGlyphs );
  memcpy( modelGlyphsToCharacters.Begin(), characterIndices, numberOfGlyphs * sizeof( CharacterIndex ) );

  Vector<Length>& modelCharactersPerGlyph = mImpl->mCharactersPerGlyph;
  modelCharactersPerGlyph.Resize( numberOfGlyphs );
  memcpy( modelCharactersPerGlyph.Begin(), charactersPerGlyph, numberOfGlyphs * sizeof( Length ) );

  // Build the characters to glyph conversion table.
  Vector<GlyphIndex>& modelCharactersToGlyph = mImpl->mCharactersToGlyph;

  // 1) Reserve some space for the characters to avoid reallocations.
  modelCharactersToGlyph.Reserve( static_cast<Length> ( static_cast<float>( numberOfGlyphs ) * 1.3f ) );

  // 2) Traverse the glyphs and set the glyph indices.
  GlyphIndex glyphIndex = 0u;
  Length totalNumberOfCharacters = 0u;
  for( Vector<Length>::ConstIterator it = modelCharactersPerGlyph.Begin(),
         endIt = modelCharactersPerGlyph.End();
       it != endIt;
       ++it, ++glyphIndex )
  {
    const Length numberOfCharacters = *it;

    for( Length index = 0u; index < numberOfCharacters; ++index, ++totalNumberOfCharacters )
    {
      modelCharactersToGlyph.PushBack( glyphIndex );
    }
  }
}

Length VisualModel::GetNumberOfGlyphs() const
{
  return mImpl->mGlyphs.Count();
}

void VisualModel::GetGlyphs( GlyphInfo* glyphs,
                             GlyphIndex glyphIndex,
                             Length numberOfGlyphs ) const
{
  const Vector<GlyphInfo>& modelGlyphs = mImpl->mGlyphs;
  memcpy( glyphs, modelGlyphs.Begin() + glyphIndex, numberOfGlyphs * sizeof( GlyphInfo ) );
}

const GlyphInfo& VisualModel::GetGlyphInfo( GlyphIndex glyphIndex ) const
{
  return mImpl->mGlyphs[glyphIndex];
}

CharacterIndex VisualModel::GetCharacterIndex( GlyphIndex glyphIndex ) const
{
  return mImpl->mGlyphsToCharacters[glyphIndex];
}

Length VisualModel::GetCharactersPerGlyph( GlyphIndex glyphIndex ) const
{
  return mImpl->mCharactersPerGlyph[glyphIndex];
}

GlyphIndex VisualModel::GetGlyphIndex( CharacterIndex characterIndex ) const
{
  return mImpl->mCharactersToGlyph[characterIndex];
}

void VisualModel::GetCharacterToGlyphMap( GlyphIndex* characterToGlyphMap,
                                          CharacterIndex characterIndex,
                                          Length numberOfCharacters ) const
{
  const Vector<GlyphIndex>& modelCharactersToGlyph = mImpl->mCharactersToGlyph;
  memcpy( characterToGlyphMap, modelCharactersToGlyph.Begin() + characterIndex, numberOfCharacters * sizeof( GlyphIndex ) );
}

void VisualModel::GetCharactersPerGlyphMap( Length* charactersPerGlyph,
                                            GlyphIndex glyphIndex,
                                            Length numberOfGlyphs ) const
{
  const Vector<Length>& modelCharactersPerGlyph = mImpl->mCharactersPerGlyph;
  memcpy( charactersPerGlyph, modelCharactersPerGlyph.Begin() + glyphIndex, numberOfGlyphs * sizeof( Length ) );
}

void VisualModel::GetGlyphToCharacterMap( CharacterIndex* glyphToCharacter,
                                          GlyphIndex glyphIndex,
                                          Length numberOfGlyphs ) const
{
  const Vector<CharacterIndex>& modelGlyphsToCharacters = mImpl->mGlyphsToCharacters;
  memcpy( glyphToCharacter, modelGlyphsToCharacters.Begin() + glyphIndex, numberOfGlyphs * sizeof( CharacterIndex ) );
}

void VisualModel::SetGlyphPositions( const Vector2* glyphPositions,
                                     Length numberOfGlyphs )
{
  Vector<Vector2>& modelPositions = mImpl->mGlyphPositions;
  modelPositions.Resize( numberOfGlyphs );
  memcpy( modelPositions.Begin(), glyphPositions, numberOfGlyphs * sizeof( Vector2 ) );
}

void VisualModel::GetGlyphPositions( Vector2* glyphPositions,
                                     GlyphIndex glyphIndex,
                                     Length numberOfGlyphs ) const
{
  const Vector<Vector2>& modelPositions = mImpl->mGlyphPositions;
  memcpy( glyphPositions, modelPositions.Begin() + glyphIndex, numberOfGlyphs * sizeof( Vector2 ) );
}

const Vector2& VisualModel::GetGlyphPosition( GlyphIndex glyphIndex ) const
{
  return *( mImpl->mGlyphPositions.Begin() + glyphIndex );
}

void VisualModel::SetLines( const LineRun* const lines,
                            Length numberOfLines )
{
  Vector<LineRun>& modelLines = mImpl->mLines;
  GetLineCache& lineCache = mImpl->mGetLineCache;

  modelLines.Resize( numberOfLines );
  memcpy( modelLines.Begin(), lines, numberOfLines * sizeof( LineRun ) );

  // Clear the get line cache.
  lineCache.glyphIndex = 0u;
  lineCache.numberOfGlyphs = 0u;
  lineCache.firstLine = 0u;
  lineCache.numberOfLines = 0u;
}

Length VisualModel::GetNumberOfLines() const
{
  return mImpl->mLines.Count();
}

void VisualModel::GetLines( LineRun* lines,
                            LineIndex lineIndex,
                            Length numberOfLines ) const
{
  const Vector<LineRun>& modelLines = mImpl->mLines;
  memcpy( lines, modelLines.Begin() + lineIndex, numberOfLines * sizeof( LineRun ) );
}

Length VisualModel::GetNumberOfLines( GlyphIndex glyphIndex,
                                      Length numberOfGlyphs ) const
{
  // If is likely the user query consecutively for the number of lines with the same
  // glyph index and number of glyphs, use the cache could be considered.
  GetLineCache& lineCache = mImpl->mGetLineCache;

  // Cache the glyph index and number of glyphs to be used in the GetLinesOfGlyphRange().
  lineCache.glyphIndex = glyphIndex;
  lineCache.numberOfGlyphs = numberOfGlyphs;

  // Check first if the query is for the total number of glyphs.
  const Length totalNumberOfGlyphs = mImpl->mGlyphs.Count();

  if( ( 0u == glyphIndex ) &&
      ( totalNumberOfGlyphs == numberOfGlyphs ) )
  {
    lineCache.firstLine = 0u;
    lineCache.numberOfLines = mImpl->mLines.Count();

    return lineCache.numberOfLines;
  }

  // Initialize the number of lines and the first line.
  lineCache.numberOfLines = 0u;
  lineCache.firstLine = 0u;
  bool firstLineFound = false;

  const Vector<LineRun>& modelLines = mImpl->mLines;
  const GlyphIndex lastGlyphIndex = glyphIndex + numberOfGlyphs;

  // Traverse the lines and cound those lines within the range of glyphs.
  for( Vector<LineRun>::ConstIterator it = modelLines.Begin(),
         endIt = modelLines.End();
       it != endIt;
       ++it )
  {
    const LineRun& line = *it;

    if( ( line.glyphIndex + line.numberOfGlyphs > glyphIndex ) &&
        ( lastGlyphIndex > line.glyphIndex ) )
    {
      firstLineFound = true;
      ++lineCache.numberOfLines;
    }
    else if( lastGlyphIndex <= line.glyphIndex )
    {
      // nothing else to do.
      break;
    }

    if( !firstLineFound )
    {
      ++lineCache.firstLine;
    }
  }

  return lineCache.numberOfLines;
}

void VisualModel::GetLinesOfGlyphRange( LineRun* lines,
                                        GlyphIndex glyphIndex,
                                        Length numberOfGlyphs ) const
{
  const Vector<LineRun>& modelLines = mImpl->mLines;
  GetLineCache& lineCache = mImpl->mGetLineCache;

  if( ( glyphIndex != lineCache.glyphIndex ) ||
      ( numberOfGlyphs != lineCache.numberOfGlyphs ) )
  {
    GetNumberOfLines( glyphIndex,
                      numberOfGlyphs );
  }

  memcpy( lines, modelLines.Begin() + lineCache.firstLine, lineCache.numberOfLines * sizeof( LineRun ) );
}

void VisualModel::SetNaturalSize( const Vector2& size  )
{
  mImpl->mNaturalSize = size;
}

const Vector2& VisualModel::GetNaturalSize() const
{
  return mImpl->mNaturalSize;
}

void VisualModel::SetActualSize( const Vector2& size )
{
  mImpl->mActualSize = size;
}

const Vector2& VisualModel::GetActualSize() const
{
  return mImpl->mActualSize;
}

VisualModel::~VisualModel()
{
  delete mImpl;
}

VisualModel::VisualModel()
: mImpl( NULL )
{
  mImpl = new VisualModel::Impl();
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
