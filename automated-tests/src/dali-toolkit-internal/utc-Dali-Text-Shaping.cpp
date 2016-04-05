/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <iostream>
#include <stdlib.h>

#include <dali-toolkit/internal/text/shaper.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <toolkit-text-model.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the following function.
// void ShapeText( const Vector<Character>& text,
//                 const Vector<LineBreakInfo>& lineBreakInfo,
//                 const Vector<ScriptRun>& scripts,
//                 const Vector<FontRun>& fonts,
//                 CharacterIndex startCharacterIndex,
//                 GlyphIndex startGlyphIndex,
//                 Length numberOfCharacters,
//                 Vector<GlyphInfo>& glyphs,
//                 Vector<CharacterIndex>& glyphToCharacterMap,
//                 Vector<Length>& charactersPerGlyph,
//                 Vector<GlyphIndex>& newParagraphGlyphs );

//////////////////////////////////////////////////////////

namespace
{

struct GlyphInfoData
{
  FontId fontId;     ///< Identifies the font containing the glyph
  GlyphIndex index;  ///< Uniquely identifies a glyph for a given FontId
  float width;       ///< The width of the glyph
  float height;      ///< The height of the glyph
  float xBearing;    ///< The distance from the cursor position to the leftmost border of the glyph
  float yBearing;    ///< The distance from the baseline to the topmost border of the glyph
  float advance;     ///< The distance to move the cursor for this glyph
  float scaleFactor; ///< The scaling applied (fixed-size fonts only)
};

bool IsEqualGlyph ( const GlyphInfoData& glyphData, const GlyphInfo& glyph )
{
  if( glyphData.fontId != glyph.fontId )
  {
    return false;
  }
  if( glyphData.index != glyph.index )
  {
    return false;
  }
  if( fabsf( glyphData.width - glyph.width ) > Math::MACHINE_EPSILON_1000 )
  {
    return false;
  }
  if( fabsf( glyphData.height - glyph.height ) > Math::MACHINE_EPSILON_1000 )
  {
    return false;
  }
  if( fabsf( glyphData.xBearing - glyph.xBearing ) > Math::MACHINE_EPSILON_1000 )
  {
    return false;
  }
  if( fabsf( glyphData.yBearing - glyph.yBearing ) > Math::MACHINE_EPSILON_1000 )
  {
    return false;
  }
  if( fabsf( glyphData.advance - glyph.advance ) > Math::MACHINE_EPSILON_1000 )
  {
    return false;
  }
  if( fabsf( glyphData.scaleFactor - glyph.scaleFactor ) > Math::MACHINE_EPSILON_1000 )
  {
    return false;
  }

  return true;
}

struct ShapeInfoData
{
  std::string     description;                        ///< Description of the test.
  std::string     text;                               ///< input text.
  uint32_t        index;                              ///< The index from where to start to query the break info.
  uint32_t        numberOfCharacters;                 ///< The requested number of characters.
  uint32_t        expectedNumberOfGlyphs;             ///< The expected number of glyphs.
  GlyphInfoData*  glyphs;                             ///< The glyphs.
  CharacterIndex* characterIndices;                   ///< The character index for each glyph.
  Length*         charactersPerGlyph;                 ///< The characters per glyph.
  uint32_t        expectedNumberOfNewParagraphGlyphs; ///< The expected number of glyphs.
  GlyphIndex*     newParagraphGlyphs;                 ///< Indices to the new paragraphs glyphs.
};

bool ShapeInfoTest( const ShapeInfoData& data )
{
  // 1) Create the model.
  LogicalModelPtr logicalModel = LogicalModel::New();
  VisualModelPtr visualModel = VisualModel::New();
  Size textArea(100.f, 60.f);
  Size layoutSize;

  const Vector<FontDescriptionRun> fontDescriptions;
  const LayoutOptions options;
  CreateTextModel( data.text,
                   textArea,
                   fontDescriptions,
                   options,
                   layoutSize,
                   logicalModel,
                   visualModel );

  // 2) Clear the model.

  Vector<GlyphInfo>& glyphs = visualModel->mGlyphs;
  Vector<CharacterIndex>& glyphToCharacter = visualModel->mGlyphsToCharacters;
  Vector<Length>& charactersPerGlyph = visualModel->mCharactersPerGlyph;
  Vector<GlyphIndex>& charactersToGlyph = visualModel->mCharactersToGlyph;
  Vector<Length>& glyphsPerCharacter = visualModel->mGlyphsPerCharacter;

  // Get the glyph index.
  GlyphIndex glyphIndex = 0u;
  if( 0u != visualModel->mCharactersToGlyph.Count() )
  {
    glyphIndex = *( visualModel->mCharactersToGlyph.Begin() + data.index );

    const CharacterIndex lastCharacterIndex = data.index + data.numberOfCharacters - 1u;
    const Length numberOfGlyphs = *( visualModel->mCharactersToGlyph.Begin() + lastCharacterIndex ) + *( visualModel->mGlyphsPerCharacter.Begin() + lastCharacterIndex ) - glyphIndex;

    // Erase the glyph info from the text model.
    // Got from the ShapeText() function.
    glyphs.Erase( glyphs.Begin() + glyphIndex, glyphs.Begin() + glyphIndex + numberOfGlyphs );
    glyphToCharacter.Erase( glyphToCharacter.Begin() + glyphIndex, glyphToCharacter.Begin() + glyphIndex + numberOfGlyphs );
    charactersPerGlyph.Erase( charactersPerGlyph.Begin() + glyphIndex, charactersPerGlyph.Begin() + glyphIndex + numberOfGlyphs );

    // Got from the VisualModel::CreateCharacterToGlyphTable() and the VisualModel::CreateGlyphsPerCharacterTable() methods.
    charactersToGlyph.Erase( charactersToGlyph.Begin() + data.index,
                             charactersToGlyph.Begin() + data.index + data.numberOfCharacters );
    glyphsPerCharacter.Erase( glyphsPerCharacter.Begin() + data.index,
                              glyphsPerCharacter.Begin() + data.index + data.numberOfCharacters );

    // Update the glyph to character indices.
    for( Vector<CharacterIndex>::Iterator it = glyphToCharacter.Begin() + glyphIndex,
           endIt = glyphToCharacter.End();
         it != endIt;
         ++it )
    {
      CharacterIndex& index = *it;
      index -= data.numberOfCharacters;
    }

  }

  // Reset the metrics got from the model as the ShapeText() function doesn't retrieve them.
  for( Vector<GlyphInfo>::Iterator it = glyphs.Begin(),
         endIt = glyphs.End();
       it != endIt;
       ++it )
  {
    GlyphInfo& info = *it;
    info.width = 0.f;
    info.height = 0.f;
    info.xBearing = 0.f;
    info.yBearing = 0.f;
    info.scaleFactor = 0.f;
  }

  // 3) Call the ShapeText() function.

  Vector<GlyphIndex> newParagraphGlyphs;

  ShapeText( logicalModel->mText,
             logicalModel->mLineBreakInfo,
             logicalModel->mScriptRuns,
             logicalModel->mFontRuns,
             data.index,
             glyphIndex,
             data.numberOfCharacters,
             glyphs,
             glyphToCharacter,
             charactersPerGlyph,
             newParagraphGlyphs );

  // Clear the advance of the new paragraph glyphs.
  for( Vector<GlyphIndex>::Iterator it = newParagraphGlyphs.Begin(),
         endIt = newParagraphGlyphs.End();
       it != endIt;
       ++it )
  {
    GlyphInfo& info = *( glyphs.Begin() + *it );
    info.advance = 0.f;
  }

  // 4) Compare the results.

  if( data.expectedNumberOfGlyphs != glyphs.Count() )
  {
    std::cout << "  Different number of glyphs : " << glyphs.Count() << ", expected : " << data.expectedNumberOfGlyphs << std::endl;
    return false;
  }

  for( unsigned int index = 0u; index < data.expectedNumberOfGlyphs; ++index )
  {
    if( !IsEqualGlyph( data.glyphs[index], glyphs[index] ) )
    {
      std::cout << "  different glyph info, index : " << index << std::endl;
      return false;
    }
  }

  for( unsigned int index = 0u; index < data.expectedNumberOfGlyphs; ++index )
  {
    if( data.characterIndices[index] != glyphToCharacter[index] )
    {
      std::cout << "  different character index, index : " << index << std::endl;
      return false;
    }
  }

  for( unsigned int index = 0u; index < data.expectedNumberOfGlyphs; ++index )
  {
    if( data.charactersPerGlyph[index] != charactersPerGlyph[index] )
    {
      std::cout << "  different character per glyph, index : " << index << std::endl;
      return false;
    }
  }

  if( data.expectedNumberOfNewParagraphGlyphs != newParagraphGlyphs.Count() )
  {
    std::cout << "  Different number of new paragraph glyphs : " << newParagraphGlyphs.Count() << ", expected : " << data.expectedNumberOfNewParagraphGlyphs << std::endl;
    return false;
  }

  for( unsigned int index = 0u; index < data.expectedNumberOfNewParagraphGlyphs; ++index )
  {
    if( data.newParagraphGlyphs[index] != newParagraphGlyphs[index] )
    {
      std::cout << "  different new paragraph glyph, index : " << index << std::endl;
      return false;
    }
  }

  return true;
}

} // namespace

//////////////////////////////////////////////////////////

int UtcDaliTextShape(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextShape");

  struct GlyphInfoData glyphs02[] =
  {
    { 1u, 43u, 0.f, 0.f, 0.f, 0.f, 12.f, 0.f },
    { 1u, 72u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 79u, 0.f, 0.f, 0.f, 0.f,  4.f, 0.f },
    { 1u, 79u, 0.f, 0.f, 0.f, 0.f,  4.f, 0.f },
    { 1u, 82u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u,  3u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 1u, 90u, 0.f, 0.f, 0.f, 0.f, 13.f, 0.f },
    { 1u, 82u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 85u, 0.f, 0.f, 0.f, 0.f,  6.f, 0.f },
    { 1u, 79u, 0.f, 0.f, 0.f, 0.f,  4.f, 0.f },
    { 1u, 71u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
  };

  CharacterIndex characterIndices02[] = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u };
  Length charactersPerGlyph02[] = { 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u };

  struct GlyphInfoData glyphs03[] =
  {
    { 1u, 43u, 0.f, 0.f, 0.f, 0.f, 12.f, 0.f },
    { 1u, 72u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 79u, 0.f, 0.f, 0.f, 0.f,  4.f, 0.f },
    { 1u, 79u, 0.f, 0.f, 0.f, 0.f,  4.f, 0.f },
    { 1u, 82u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u,  3u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 1u, 90u, 0.f, 0.f, 0.f, 0.f, 13.f, 0.f },
    { 1u, 82u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 85u, 0.f, 0.f, 0.f, 0.f,  6.f, 0.f },
    { 1u, 79u, 0.f, 0.f, 0.f, 0.f,  4.f, 0.f },
    { 1u, 71u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u,  0u, 0.f, 0.f, 0.f, 0.f,  0.f, 0.f },
    { 1u, 71u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u, 72u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 80u, 0.f, 0.f, 0.f, 0.f, 15.f, 0.f },
    { 1u, 82u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u,  0u, 0.f, 0.f, 0.f, 0.f,  0.f, 0.f },
  };

  CharacterIndex characterIndices03[] = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u };
  Length charactersPerGlyph03[] = { 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u };
  CharacterIndex newParagraphGlyphs03[] = { 11u, 16u };

  struct GlyphInfoData glyphs04[] =
  {
    { 2u, 160u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 2u, 123u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 2u, 153u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 2u, 160u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 2u, 150u, 0.f, 0.f, 0.f, 0.f, 14.f, 0.f },
    { 2u, 153u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 2u, 160u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 2u, 151u, 0.f, 0.f, 0.f, 0.f, 12.f, 0.f },
    { 2u, 153u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 2u, 160u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 2u, 147u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 2u, 153u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
  };

  CharacterIndex characterIndices04[] = { 0u, 0u, 0u, 2u, 2u, 2u, 4u, 4u, 4u, 6u, 6u, 6u };
  Length charactersPerGlyph04[] = { 0u, 0u, 2u, 0u, 0u, 2u, 0u, 0u, 2u, 0u, 0u, 2u };

  struct GlyphInfoData glyphs05[] =
  {
    { 1u, 47u, 0.f, 0.f, 0.f, 0.f,  8.f, 0.f },
    { 1u, 82u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 85u, 0.f, 0.f, 0.f, 0.f,  6.f, 0.f },
    { 1u, 72u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 80u, 0.f, 0.f, 0.f, 0.f, 15.f, 0.f },
    { 1u,  3u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 1u, 76u, 0.f, 0.f, 0.f, 0.f,  4.f, 0.f },
    { 1u, 83u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u, 86u, 0.f, 0.f, 0.f, 0.f,  8.f, 0.f },
    { 1u, 88u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u, 80u, 0.f, 0.f, 0.f, 0.f, 15.f, 0.f },
    { 1u,  3u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 1u, 71u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u, 82u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 79u, 0.f, 0.f, 0.f, 0.f,  4.f, 0.f },
    { 1u, 82u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 85u, 0.f, 0.f, 0.f, 0.f,  6.f, 0.f },
    { 1u,  3u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 1u, 86u, 0.f, 0.f, 0.f, 0.f,  8.f, 0.f },
    { 1u, 76u, 0.f, 0.f, 0.f, 0.f,  4.f, 0.f },
    { 1u, 87u, 0.f, 0.f, 0.f, 0.f,  6.f, 0.f },
    { 1u,  3u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 1u, 68u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 80u, 0.f, 0.f, 0.f, 0.f, 15.f, 0.f },
    { 1u, 72u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 87u, 0.f, 0.f, 0.f, 0.f,  6.f, 0.f },
    { 1u,  0u, 0.f, 0.f, 0.f, 0.f,  0.f, 0.f },
    { 1u, 68u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 72u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 84u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u, 88u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u, 72u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u,  3u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 1u, 71u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u, 72u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 5034u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u, 81u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u, 76u, 0.f, 0.f, 0.f, 0.f,  4.f, 0.f },
    { 1u, 72u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 69u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u, 68u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 86u, 0.f, 0.f, 0.f, 0.f,  8.f, 0.f },
    { 1u,  3u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 1u, 72u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 68u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u,  3u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 1u, 80u, 0.f, 0.f, 0.f, 0.f, 15.f, 0.f },
    { 1u, 72u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 76u, 0.f, 0.f, 0.f, 0.f,  4.f, 0.f },
    { 1u,  0u, 0.f, 0.f, 0.f, 0.f,  0.f, 0.f },
    { 1u, 83u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u, 82u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 86u, 0.f, 0.f, 0.f, 0.f,  8.f, 0.f },
    { 1u, 86u, 0.f, 0.f, 0.f, 0.f,  8.f, 0.f },
    { 1u, 72u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u,  3u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 1u, 76u, 0.f, 0.f, 0.f, 0.f,  4.f, 0.f },
    { 1u, 85u, 0.f, 0.f, 0.f, 0.f,  6.f, 0.f },
    { 1u, 68u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u, 70u, 0.f, 0.f, 0.f, 0.f,  8.f, 0.f },
    { 1u, 88u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u, 81u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u, 71u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u, 76u, 0.f, 0.f, 0.f, 0.f,  4.f, 0.f },
    { 1u, 68u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u,  3u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 1u, 81u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u, 72u, 0.f, 0.f, 0.f, 0.f,  9.f, 0.f },
    { 1u,  3u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 1u, 70u, 0.f, 0.f, 0.f, 0.f,  8.f, 0.f },
    { 1u, 88u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f },
    { 1u, 80u, 0.f, 0.f, 0.f, 0.f, 15.f, 0.f },
    { 1u, 17u, 0.f, 0.f, 0.f, 0.f,  5.f, 0.f },
    { 1u,  0u, 0.f, 0.f, 0.f, 0.f,  0.f, 0.f },
  };

  CharacterIndex characterIndices05[] = {  0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u,
                                          10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u,
                                          20u, 21u, 22u, 23u, 24u, 25u, 26u, 27u, 28u, 29u,
                                          30u, 31u, 32u, 33u, 34u, 35u, 37u, 38u, 39u, 40u,
                                          41u, 42u, 43u, 44u, 45u, 46u, 47u, 48u, 49u, 50u,
                                          51u, 52u, 53u, 54u, 55u, 56u, 57u, 58u, 59u, 60u,
                                          61u, 62u, 63u, 64u, 65u, 66u, 67u, 68u, 69u, 70u,
                                          71u, 72u, 73u, 74u };
  Length charactersPerGlyph05[] = { 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
                                    1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
                                    1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
                                    1u, 1u, 1u, 1u, 1u, 2u, 1u, 1u, 1u, 1u,
                                    1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
                                    1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
                                    1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
                                    1u, 1u, 1u, 1u };
  CharacterIndex newParagraphGlyphs05[] = { 26u };
  CharacterIndex newParagraphGlyphs06[] = { 49u };
  CharacterIndex newParagraphGlyphs07[] = { 73u };

  struct ShapeInfoData data[] =
  {
    {
      "Zero characters",
      "",
      0u,
      0u,
      0u,
      NULL,
      NULL,
      NULL,
      0u,
      NULL
    },
    {
      "Latin script",
      "Hello world",
      0u,
      11u,
      11u,
      glyphs02,
      characterIndices02,
      charactersPerGlyph02,
      0u,
      NULL
    },
    {
      "Latin script. Some paragraphs.",
      "Hello world\ndemo\n",
      0u,
      17u,
      17u,
      glyphs03,
      characterIndices03,
      charactersPerGlyph03,
      2u,
      newParagraphGlyphs03
    },
    {
      "Malayalam script. More glyphs than characters.",
      "ജോസോഹോവോ",
      0u,
      8u,
      12u,
      glyphs04,
      characterIndices04,
      charactersPerGlyph04,
      0u,
      NULL
    },
    {
      "Latin script with some paragraphs. Update initial paragraph.",
      "Lorem ipsum dolor sit amet\naeque definiebas ea mei\nposse iracundia ne cum.\n",
      0u,
      27u,
      74u,
      glyphs05,
      characterIndices05,
      charactersPerGlyph05,
      1u,
      newParagraphGlyphs05
    },
    {
      "Latin script with some paragraphs. Update mid paragraph.",
      "Lorem ipsum dolor sit amet\naeque definiebas ea mei\nposse iracundia ne cum.\n",
      27u,
      24u,
      74u,
      glyphs05,
      characterIndices05,
      charactersPerGlyph05,
      1u,
      newParagraphGlyphs06
    },
    {
      "Latin script with some paragraphs. Update final paragraph.",
      "Lorem ipsum dolor sit amet\naeque definiebas ea mei\nposse iracundia ne cum.\n",
      51u,
      24u,
      74u,
      glyphs05,
      characterIndices05,
      charactersPerGlyph05,
      1u,
      newParagraphGlyphs07
    },
  };
  const unsigned int numberOfTests = 7u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    if( !ShapeInfoTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}
