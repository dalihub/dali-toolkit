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

void VisualModel::CreateCharacterToGlyphTable( CharacterIndex startIndex,
                                               GlyphIndex startGlyphIndex,
                                               Length numberOfCharacters )
{
  if( 0u == numberOfCharacters )
  {
    // Nothing to do.
    return;
  }

  DALI_ASSERT_DEBUG( mGlyphsPerCharacter.Count() != 0u );

  // Get the total number of characters.
  const Length totalNumberOfCharacters = ( 0u == mGlyphsToCharacters.Count() ) ? 0u : *( mGlyphsToCharacters.End() - 1u ) + *( mCharactersPerGlyph.End() - 1u ); // Index to the first character + the number of characters that form the last glyph.

  // Whether the current buffer is being updated or is set from scratch.
  const bool updateCurrentBuffer = numberOfCharacters < totalNumberOfCharacters;

  Vector<GlyphIndex> newCharactersToGlyph;
  GlyphIndex* charactersToGlyphBuffer = NULL;

  // 1) Reserve some space for the glyph indices to avoid reallocations.
  if( updateCurrentBuffer )
  {
    newCharactersToGlyph.Resize( numberOfCharacters );
    charactersToGlyphBuffer = newCharactersToGlyph.Begin();
  }
  else
  {
    mCharactersToGlyph.Resize( numberOfCharacters );
    charactersToGlyphBuffer = mCharactersToGlyph.Begin() + startIndex;
  }

  const Length* const glyphsPerCharacterBuffer = mGlyphsPerCharacter.Begin();

  // 2) Traverse the glyphs and set the glyph indices per character.

  // Index to the glyph.
  GlyphIndex glyphIndex = startGlyphIndex;
  CharacterIndex characterIndex = startIndex;
  const CharacterIndex lastCharacterIndexPlusOne = startIndex + numberOfCharacters;
  for( Vector<Length>::ConstIterator it = mCharactersPerGlyph.Begin() + glyphIndex,
         endIt = mCharactersPerGlyph.End();
       ( it != endIt ) && ( characterIndex < lastCharacterIndexPlusOne );
       ++it )
  {
    const Length numberOfCharactersPerGlyph = *it;

    Length numberOfGlyphs = 0u;
    // Set the glyph indices.
    for( Length index = 0u; index < numberOfCharactersPerGlyph; ++index, ++characterIndex )
    {
      *charactersToGlyphBuffer = glyphIndex;
      numberOfGlyphs += *( glyphsPerCharacterBuffer + characterIndex );
      ++charactersToGlyphBuffer;
    }
    glyphIndex += numberOfGlyphs;
  }

  // If the character to glyph buffer is updated, it needs to be inserted in the model.
  if( updateCurrentBuffer )
  {
    // Update the indices.
    const Length numberOfGlyphs = glyphIndex - startGlyphIndex;
    for( Vector<Length>::Iterator it = mCharactersToGlyph.Begin() + startIndex,
           endIt = mCharactersToGlyph.End();
         it != endIt;
         ++it )
    {
      *it += numberOfGlyphs;
    }

    mCharactersToGlyph.Insert( mCharactersToGlyph.Begin() + startIndex,
                               newCharactersToGlyph.Begin(),
                               newCharactersToGlyph.End() );

  }
}

void VisualModel::CreateGlyphsPerCharacterTable( CharacterIndex startIndex,
                                                 GlyphIndex startGlyphIndex,
                                                 Length numberOfCharacters )
{
  if( 0u == numberOfCharacters )
  {
    // Nothing to do.
    return;
  }

  // Get the total number of characters.
  const Length totalNumberOfCharacters = ( 0u == mGlyphsToCharacters.Count() ) ? 0u : *( mGlyphsToCharacters.End() - 1u ) + *( mCharactersPerGlyph.End() - 1u ); // Index to the first character + the number of characters that form the last glyph.

  // Whether the current buffer is being updated or is set from scratch.
  const bool updateCurrentBuffer = numberOfCharacters < totalNumberOfCharacters;

  Vector<Length> newGlyphsPerCharacter;
  Length* glyphsPerCharacterBuffer = NULL;

  // 1) Reserve some space for the glyphs per character to avoid reallocations.
  if( updateCurrentBuffer )
  {
    newGlyphsPerCharacter.Resize( numberOfCharacters );
    glyphsPerCharacterBuffer = newGlyphsPerCharacter.Begin();
  }
  else
  {
    mGlyphsPerCharacter.Resize( numberOfCharacters );
    glyphsPerCharacterBuffer = mGlyphsPerCharacter.Begin() + startIndex;
  }

  // 2) Traverse the glyphs and set the number of glyphs per character.

  Length traversedCharacters = 0;

  // The number of 'characters per glyph' equal to zero.
  Length zeroCharactersPerGlyph = 0u;

  for( Vector<Length>::ConstIterator it = mCharactersPerGlyph.Begin() + startGlyphIndex,
         endIt = mCharactersPerGlyph.End();
       ( it != endIt ) && ( traversedCharacters < numberOfCharacters );
       ++it )
  {
    const Length numberOfCharactersPerGlyph = *it;
    traversedCharacters += numberOfCharactersPerGlyph;

    // Set the glyphs per character.
    if( 0u == numberOfCharactersPerGlyph )
    {
      ++zeroCharactersPerGlyph;
    }
    else
    {
      const Length numberOfZeroGlyphsPerCharacter = ( numberOfCharactersPerGlyph - 1u );
      for( Length zeroIndex = 0u; zeroIndex < numberOfZeroGlyphsPerCharacter; ++zeroIndex )
      {
        *glyphsPerCharacterBuffer = 0u;

        // Point to the next position in the buffer.
        ++glyphsPerCharacterBuffer;
      }

      *glyphsPerCharacterBuffer = zeroCharactersPerGlyph + 1u;

      zeroCharactersPerGlyph = 0u;

      // Point to the next position in the buffer.
      ++glyphsPerCharacterBuffer;
    }
  }

  // If the glyphs per character buffer is updated, it needs to be inserted in the model.
  if( updateCurrentBuffer )
  {
    mGlyphsPerCharacter.Insert( mGlyphsPerCharacter.Begin() + startIndex,
                                newGlyphsPerCharacter.Begin(),
                                newGlyphsPerCharacter.End() );
  }
}

void VisualModel::GetGlyphs( GlyphInfo* glyphs,
                             GlyphIndex glyphIndex,
                             Length numberOfGlyphs ) const
{
  memcpy( glyphs, mGlyphs.Begin() + glyphIndex, numberOfGlyphs * sizeof( GlyphInfo ) );
}

void VisualModel::GetGlyphPositions( Vector2* glyphPositions,
                                     GlyphIndex glyphIndex,
                                     Length numberOfGlyphs ) const
{
  memcpy( glyphPositions, mGlyphPositions.Begin() + glyphIndex, numberOfGlyphs * sizeof( Vector2 ) );
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

    if( ( line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs > glyphIndex ) &&
        ( lastGlyphIndex > line.glyphRun.glyphIndex ) )
    {
      firstLineFound = true;
      ++numberOfLines;
    }
    else if( lastGlyphIndex <= line.glyphRun.glyphIndex )
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

LineIndex VisualModel::GetLineOfCharacter( CharacterIndex characterIndex )
{
  // 1) Check first in the cached line.

  const LineRun& lineRun = *( mLines.Begin() + mCachedLineIndex );

  if( ( lineRun.characterRun.characterIndex <= characterIndex ) &&
      ( characterIndex < lineRun.characterRun.characterIndex + lineRun.characterRun.numberOfCharacters ) )
  {
    return mCachedLineIndex;
  }

  // 2) Is not in the cached line. Check in the other lines.

  LineIndex index = characterIndex < lineRun.characterRun.characterIndex ? 0u : mCachedLineIndex + 1u;

  for( Vector<LineRun>::ConstIterator it = mLines.Begin() + index,
         endIt = mLines.End();
       it != endIt;
       ++it, ++index )
  {
    const LineRun& lineRun = *it;

    if( characterIndex < lineRun.characterRun.characterIndex + lineRun.characterRun.numberOfCharacters )
    {
      mCachedLineIndex = index;
      break;
    }
  }

  return index;
}

void VisualModel::GetUnderlineRuns( GlyphRun* underlineRuns,
                                    UnderlineRunIndex index,
                                    Length numberOfRuns ) const
{
  memcpy( underlineRuns,
          mUnderlineRuns.Begin() + index,
          numberOfRuns * sizeof( GlyphRun ) );
}

void VisualModel::SetNaturalSize( const Vector2& size  )
{
  mNaturalSize = size;
}

const Vector2& VisualModel::GetNaturalSize() const
{
  return mNaturalSize;
}

void VisualModel::SetLayoutSize( const Vector2& size )
{
  mLayoutSize = size;
}

const Vector2& VisualModel::GetLayoutSize() const
{
  return mLayoutSize;
}

void VisualModel::SetTextColor( const Vector4& textColor )
{
  mTextColor = textColor;

  if ( !mUnderlineColorSet )
  {
    mUnderlineColor = textColor;
  }
}

void VisualModel::SetShadowOffset( const Vector2& shadowOffset )
{
  mShadowOffset = shadowOffset;
}

void VisualModel::SetShadowColor( const Vector4& shadowColor )
{
  mShadowColor = shadowColor;
}

void VisualModel::SetUnderlineColor( const Vector4& color )
{
  mUnderlineColor = color;
  mUnderlineColorSet = true;
}

void VisualModel::SetUnderlineEnabled( bool enabled )
{
  mUnderlineEnabled = enabled;
}

void VisualModel::SetUnderlineHeight( float height )
{
  mUnderlineHeight = height;
}

const Vector4& VisualModel::GetTextColor() const
{
  return mTextColor;
}

const Vector2& VisualModel::GetShadowOffset() const
{
  return mShadowOffset;
}

const Vector4& VisualModel::GetShadowColor() const
{
  return mShadowColor;
}

const Vector4& VisualModel::GetUnderlineColor() const
{
  return mUnderlineColor;
}

bool VisualModel::IsUnderlineEnabled() const
{
  return mUnderlineEnabled;
}

float VisualModel::GetUnderlineHeight() const
{
  return mUnderlineHeight;
}

void VisualModel::ClearCaches()
{
  mCachedLineIndex = 0u;
}

VisualModel::~VisualModel()
{
}

VisualModel::VisualModel()
: mGlyphs(),
  mGlyphsToCharacters(),
  mCharactersToGlyph(),
  mCharactersPerGlyph(),
  mGlyphsPerCharacter(),
  mGlyphPositions(),
  mLines(),
  mTextColor( Color::BLACK ),
  mShadowColor( Color::BLACK ),
  mUnderlineColor( Color::BLACK ),
  mShadowOffset( Vector2::ZERO ),
  mUnderlineHeight( 0.0f ),
  mNaturalSize(),
  mLayoutSize(),
  mCachedLineIndex( 0u ),
  mUnderlineEnabled( false ),
  mUnderlineColorSet( false )
{
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
