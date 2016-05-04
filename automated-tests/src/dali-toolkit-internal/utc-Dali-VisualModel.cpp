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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <toolkit-text-model.h>


using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the following functions.
//
// void CreateCharacterToGlyphTable( CharacterIndex startIndex,
//                                   Length numberOfCharacters )
//
// void CreateGlyphsPerCharacterTable( CharacterIndex startIndex,
//                                     Length numberOfCharacters )


//////////////////////////////////////////////////////////

namespace
{

struct SetGlyphsPerCharacterData
{
  std::string   description;             ///< Description of the test.
  std::string   text;                    ///< Input text.
  unsigned int  startIndex;              ///< The start index from where the glyphs per character table is set.
  unsigned int  numberOfCharacters;      ///< The number of characters to set.
  unsigned int  totalNumberOfCharacters; ///< The total number of characters.
  unsigned int* glyphsPerCharacter;      ///< The number of glyphs per character.
};

struct SetCharacterToGlyphData
{
  std::string   description;             ///< Description of the test.
  std::string   text;                    ///< Input text.
  unsigned int  startIndex;              ///< The start index from where the character to glyph table is set.
  unsigned int  numberOfCharacters;      ///< The number of characters to set.
  unsigned int  totalNumberOfCharacters; ///< The total number of characters.
  unsigned int* glyphsIndices;           ///< The glyph indices.
};

bool SetGlyphsPerCharacterTest( const SetGlyphsPerCharacterData& data )
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

  Vector<GlyphIndex>& charactersToGlyph = visualModel->mCharactersToGlyph;
  Vector<Length>& glyphsPerCharacter = visualModel->mGlyphsPerCharacter;

  // 2) Clear the model.

  GlyphIndex startGlyphIndex = 0u;
  if( 0u != charactersToGlyph.Count() )
  {
    // The number of glyphs to be removed.
    const Length numberOfGlyphs = charactersToGlyph[data.startIndex + data.numberOfCharacters - 1u] + glyphsPerCharacter[data.startIndex + data.numberOfCharacters - 1u] - charactersToGlyph[data.startIndex];
    startGlyphIndex = charactersToGlyph[data.startIndex];

    charactersToGlyph.Erase( charactersToGlyph.Begin() + data.startIndex,
                             charactersToGlyph.Begin() + data.startIndex + data.numberOfCharacters );
    glyphsPerCharacter.Erase( glyphsPerCharacter.Begin() + data.startIndex,
                              glyphsPerCharacter.Begin() + data.startIndex + data.numberOfCharacters );

    // Update the character to glyph indices.
    for( Vector<GlyphIndex>::Iterator it = charactersToGlyph.Begin() + data.startIndex,
           endIt = charactersToGlyph.End();
         it != endIt;
         ++it )
    {
      *it -= numberOfGlyphs;
    }
  }

  // 3) Call the CreateGlyphsPerCharacterTable() function
  visualModel->CreateGlyphsPerCharacterTable( data.startIndex,
                                              startGlyphIndex,
                                              data.numberOfCharacters );

  // 4) Compare the results.
  if( data.totalNumberOfCharacters != glyphsPerCharacter.Count() )
  {
    std::cout << "  Different number of characters : " << glyphsPerCharacter.Count() << ", expected : " << data.totalNumberOfCharacters << std::endl;
    return false;
  }

  for( unsigned int i = 0u; i < data.totalNumberOfCharacters; ++i )
  {
    if( data.glyphsPerCharacter[i] != glyphsPerCharacter[i] )
    {
      std::cout << "  Different number of glyphs for index " << i << std::endl;
      for( unsigned int j = 0; j < data.totalNumberOfCharacters; ++j )
      {
        std::cout << glyphsPerCharacter[j] << " ";
      }
      std::cout << std::endl;
      std::cout << "  expected" << std::endl;
      for( unsigned int j = 0; j < data.totalNumberOfCharacters; ++j )
      {
        std::cout << data.glyphsPerCharacter[j] << " ";
      }
      std::cout << std::endl;
      return false;
    }
  }

  return true;
}

bool SetCharacterToGlyphTest( const SetCharacterToGlyphData& data )
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

  Vector<GlyphIndex>& charactersToGlyph = visualModel->mCharactersToGlyph;
  Vector<Length>& glyphsPerCharacter = visualModel->mGlyphsPerCharacter;

  // 2) Clear the model.

  GlyphIndex startGlyphIndex = 0u;
  if( 0u != charactersToGlyph.Count() )
  {
    // The number of glyphs to be removed.
    const Length numberOfGlyphs = charactersToGlyph[data.startIndex + data.numberOfCharacters - 1u] + glyphsPerCharacter[data.startIndex + data.numberOfCharacters - 1u] - charactersToGlyph[data.startIndex];
    startGlyphIndex = charactersToGlyph[data.startIndex];

    charactersToGlyph.Erase( charactersToGlyph.Begin() + data.startIndex,
                             charactersToGlyph.Begin() + data.startIndex + data.numberOfCharacters );

    // Update the character to glyph indices.
    for( Vector<GlyphIndex>::Iterator it = charactersToGlyph.Begin() + data.startIndex,
           endIt = charactersToGlyph.End();
         it != endIt;
         ++it )
    {
      *it -= numberOfGlyphs;
    }
  }

  // 3) Call the CreateCharacterToGlyphTable() function
  visualModel->CreateCharacterToGlyphTable( data.startIndex,
                                            startGlyphIndex,
                                            data.numberOfCharacters );

  // 4) Compare the results.
  if( data.totalNumberOfCharacters != charactersToGlyph.Count() )
  {
    std::cout << "  Different number of character : " << charactersToGlyph.Count() << ", expected : " << data.totalNumberOfCharacters << std::endl;
    return false;
  }

  for( unsigned int i = 0u; i < data.totalNumberOfCharacters; ++i )
  {
    if( data.glyphsIndices[i] != charactersToGlyph[i] )
    {
      std::cout << "  Different number of character to glyph index " << i << std::endl;
      for( unsigned int j = 0; j < data.totalNumberOfCharacters; ++j )
      {
        std::cout << charactersToGlyph[j] << " ";
      }
      std::cout << std::endl;
      std::cout << "  expected" << std::endl;
      for( unsigned int j = 0; j < data.totalNumberOfCharacters; ++j )
      {
        std::cout << data.glyphsIndices[j] << " ";
      }
      std::cout << std::endl;
      return false;
    }
  }

  return true;
}

} // namespace

//////////////////////////////////////////////////////////

int UtcDaliSetGlyphsPerCharacter(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliSetGlyphsPerCharacter");

  Length glyphsPerCharacter02[] = { 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u };
  Length glyphsPerCharacter03[] = { 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 0u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u };
  Length glyphsPerCharacter04[] = { 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 0u, 1u,
                                    1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
                                    1u, 1u, 1u, 1u, 0u, 1u, 0u, 2u, 1u, 0u,
                                    2u, 0u, 2u, 0u, 2u, 1u, 1u, 0u, 0u, 0u,
                                    2u, 1u, 1u, 1u, 1u, 1u, 0u, 0u, 2u, 1u,
                                    0u, 2u, 1u, 1u };

  struct SetGlyphsPerCharacterData data[] =
  {
    {
      "Zero characters text",
      "",
      0u,
      0u,
      0u,
      NULL
    },
    {
      "Simple 1 to 1 text",
      "Hello world",
      0u,
      11u,
      11u,
      glyphsPerCharacter02,
    },
    {
      "Text with different number of glyphs and characters.",
      "Hello different world",
      0u,
      21u,
      21u,
      glyphsPerCharacter03,
    },
    {
      "Text paragraphs with different number of glyphs and characters. Update initial paragraphs.",
      "Hello different world\nनमस्ते दुनिया\nမင်္ဂလာပါကမ္ဘာလောက",
      0u,
      22u,
      54u,
      glyphsPerCharacter04,
    },
    {
      "Text paragraphs with different number of glyphs and characters. Update mid paragraphs.",
      "Hello different world\nनमस्ते दुनिया\nမင်္ဂလာပါကမ္ဘာလောက",
      22u,
      14u,
      54u,
      glyphsPerCharacter04,
    },
    {
      "Text paragraphs with different number of glyphs and characters. Update final paragraphs.",
      "Hello different world\nनमस्ते दुनिया\nမင်္ဂလာပါကမ္ဘာလောက",
      36u,
      18u,
      54u,
      glyphsPerCharacter04,
    },
  };
  const unsigned int numberOfTests = 6u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    if( !SetGlyphsPerCharacterTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliSetCharacterToGlyph(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliSetGlyphsPerCharacter");

  GlyphIndex glyphIndices02[] = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u };
  GlyphIndex glyphIndices03[] = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u };
  GlyphIndex glyphIndices04[] = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u, 20u,
                                  21u, 22u, 23u, 23u, 24u, 24u, 26u, 27u, 27u, 29u, 29u, 31u, 31u, 33u,
                                  34u, 35u, 35u, 35u, 35u, 37u, 38u, 39u, 40u, 41u, 42u, 42u, 42u, 44u, 45u, 45u, 47u, 48u };

  struct SetCharacterToGlyphData data[] =
  {
    {
      "Zero characters text",
      "",
      0u,
      0u,
      0u,
      NULL
    },
    {
      "Simple 1 to 1 text",
      "Hello world",
      0u,
      11u,
      11u,
      glyphIndices02,
    },
    {
      "Text with different number of glyphs and characters.",
      "Hello different world",
      0u,
      21u,
      21u,
      glyphIndices03,
    },
    {
      "Text paragraphs with different number of glyphs and characters. Update initial paragraphs.",
      "Hello different world\nनमस्ते दुनिया\nမင်္ဂလာပါကမ္ဘာလောက",
      0u,
      22u,
      54u,
      glyphIndices04,
    },
    {
      "Text paragraphs with different number of glyphs and characters. Update mid paragraphs.",
      "Hello different world\nनमस्ते दुनिया\nမင်္ဂလာပါကမ္ဘာလောက",
      22u,
      14u,
      54u,
      glyphIndices04,
    },
    {
      "Text paragraphs with different number of glyphs and characters. Update final paragraphs.",
      "Hello different world\nनमस्ते दुनिया\nမင်္ဂလာပါကမ္ဘာလောက",
      36u,
      18u,
      54u,
      glyphIndices04,
    },
  };

  const unsigned int numberOfTests = 6u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    if( !SetCharacterToGlyphTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}
