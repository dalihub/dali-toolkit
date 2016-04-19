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
#include <dali-toolkit/internal/text/text-run-container.h>
#include <dali-toolkit/dali-toolkit.h>
#include <toolkit-text-model.h>


using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the following functions.
//
// void SetVisualToLogicalMap( const BidirectionalLineInfoRun* const bidirectionalInfo,
//                             Length numberOfRuns,
//                             CharacterIndex startIndex  )


//////////////////////////////////////////////////////////

namespace
{
struct CreateParagraphData
{
  std::string    description;                    ///< Description of the test.
  std::string    text;                           ///< Input text.
  CharacterIndex index;                          ///< The first character index.
  Length         numberOfCharacters;             ///< The number of characters.
  unsigned int   numberOfParagraphs;             ///< The expected number of paragraphs.
  unsigned int*  indices;                        ///< The expected paragraph info indices.
  unsigned int*  numberOfCharactersPerParagraph; ///< The expected number of characters of each paragraph.
};

struct FindParagraphData
{
  std::string    description;        ///< Description of the test.
  std::string    text;               ///< Input text.
  CharacterIndex index;              ///< The first character index.
  Length         numberOfCharacters; ///< The number of characters.
  unsigned int   numberOfParagraphs; ///< The expected number of paragraphs.
  unsigned int*  paragraphs;         ///< The expected paragraph info.
};

struct SetVisualToLogicalMapData
{
  std::string   description;                ///< Description of the test.
  std::string   text;                       ///< Input text.
  unsigned int  startIndex;                 ///< The start index from where the visual to logical conversion table is set.
  unsigned int  numberOfCharacters;         ///< The number of characters to set.
  Size          textArea;                   ///< The size of the area where the text is laid-out.
  unsigned int  expectedNumberOfCharacters; ///< The expected number of characters.
  unsigned int* visualToLogical;            ///< The expected visual to logical conversion table.
};

bool CreateParagraphTest( const CreateParagraphData& data )
{
  // 1) Create the model.
  LogicalModelPtr logicalModel = LogicalModel::New();
  VisualModelPtr visualModel = VisualModel::New();
  Size textArea(100.f, 60.f);
  Size layoutSize;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  LayoutOptions options;
  CreateTextModel( data.text,
                   textArea,
                   fontDescriptionRuns,
                   options,
                   layoutSize,
                   logicalModel,
                   visualModel );

  // 2) Clear the paragraphs.
  Vector<ParagraphRun>& paragraphs = logicalModel->mParagraphInfo;
  ClearCharacterRuns( data.index,
                      data.index + data.numberOfCharacters - 1u,
                      paragraphs );

  // 3) Call the LogicalModel::CreateParagraphInfo() method
  logicalModel->CreateParagraphInfo( data.index,
                                     data.numberOfCharacters );

  // 4) Compare the results.
  if( data.numberOfParagraphs != paragraphs.Count() )
  {
    std::cout << "  Different number of paragraphs : " << paragraphs.Count() << ", expected : " << data.numberOfParagraphs << std::endl;
    return false;
  }

  unsigned int index = 0u;
  for( Vector<ParagraphRun>::ConstIterator it = paragraphs.Begin(),
         endIt = paragraphs.End();
       it != endIt;
       ++it, ++index )
  {
    const ParagraphRun& paragraph( *it );

    if( data.indices[index] != paragraph.characterRun.characterIndex )
    {
      std::cout << "  Different character index for paragraph : " << index << ", " << paragraph.characterRun.characterIndex << ", expected : " << data.indices[index] << std::endl;
      return false;
    }
    if( data.numberOfCharactersPerParagraph[index] != paragraph.characterRun.numberOfCharacters )
    {
      std::cout << "  Different number of characters for paragraph : " << index << ", " << paragraph.characterRun.numberOfCharacters << ", expected : " << data.numberOfCharactersPerParagraph[index] << std::endl;
      return false;
    }
  }

  return true;
}

bool FindParagraphTest( const FindParagraphData& data )
{
  // 1) Create the model.
  LogicalModelPtr logicalModel = LogicalModel::New();
  VisualModelPtr visualModel = VisualModel::New();
  Size textArea(100.f, 60.f);
  Size layoutSize;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  LayoutOptions options;
  CreateTextModel( data.text,
                   textArea,
                   fontDescriptionRuns,
                   options,
                   layoutSize,
                   logicalModel,
                   visualModel );

  // 2) Find the paragraphs.
  Vector<ParagraphRunIndex> paragraphs;
  logicalModel->FindParagraphs( data.index, data.numberOfCharacters, paragraphs );

  // 3) compare the results.
  if( data.numberOfParagraphs != paragraphs.Count() )
  {
    return false;
  }

  unsigned int index = 0u;
  for( Vector<ParagraphRunIndex>::ConstIterator it = paragraphs.Begin(),
         endIt = paragraphs.End();
       it != endIt;
       ++it, ++index )
  {
    const ParagraphRunIndex paragraphIndex = *it;

    if( paragraphIndex != data.paragraphs[index] )
    {
      return false;
    }
  }

  return true;
}

bool SetVisualToLogicalMapTest( const SetVisualToLogicalMapData& data )
{
  std::cout << "  testing : " << data.description << std::endl;
  // Create the model.
  LogicalModelPtr logicalModel = LogicalModel::New();
  VisualModelPtr visualModel = VisualModel::New();
  Size layoutSize;

  // Create the model with the whole text.
  const Vector<FontDescriptionRun> fontDescriptions;
  const LayoutOptions options;
  CreateTextModel( data.text,
                   data.textArea,
                   fontDescriptions,
                   options,
                   layoutSize,
                   logicalModel,
                   visualModel );

  // Get the visual to logical map.
  Vector<CharacterIndex>& visualToLogicalMap = logicalModel->mVisualToLogicalMap;

  // Compare the results.
  if( data.expectedNumberOfCharacters != visualToLogicalMap.Count() )
  {
    std::cout << "  differetn number of characters : " << visualToLogicalMap.Count() << ", expected : " << data.expectedNumberOfCharacters << std::endl;
    return false;
  }

  for( unsigned int index = 0u; index < data.expectedNumberOfCharacters; ++index )
  {
    if( data.visualToLogical[index] != visualToLogicalMap[index] )
    {
      std::cout << "  different visualToLogical table : " << std::endl;
      for( unsigned int i = 0; i < data.expectedNumberOfCharacters; ++i )
      {
        std::cout << visualToLogicalMap[i] << " ";
      }
      std::cout << std::endl;
      std::cout << "                         expected : " << std::endl;
      for( unsigned int i = 0; i < data.expectedNumberOfCharacters; ++i )
      {
        std::cout << data.visualToLogical[i] << " ";
      }
      std::cout << std::endl;
      return false;
    }
  }

  return true;
}

} // namespace

//////////////////////////////////////////////////////////
//
// UtcDaliCreateParagraph
// UtcDaliFindParagraph
// UtcDaliSetVisualToLogicalMap
//
//////////////////////////////////////////////////////////

int UtcDaliCreateParagraph(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCreateParagraph");

  unsigned int paragraphsIndices01[] = { 0u };
  unsigned int paragraphsNumberOfCharacters01[] = { 0u };
  unsigned int paragraphsIndices02[] = { 0u, 12u, 17u };
  unsigned int paragraphsNumberOfCharacters02[] = { 12u, 5u, 1u };
  unsigned int paragraphsIndices03[] = { 0u, 12u, 17u, 34u };
  unsigned int paragraphsNumberOfCharacters03[] = { 12u, 5u, 17u ,1u };

  struct CreateParagraphData data[] =
  {
    {
      "Zero characters",
      "",
      0u,
      0u,
      0u,
      paragraphsIndices01,
      paragraphsNumberOfCharacters01,
    },
    {
      "Some paragraphs",
      "Hello world\ndemo\n\n",
      0u,
      18u,
      3u,
      paragraphsIndices02,
      paragraphsNumberOfCharacters02,
    },
    {
      "Some paragraphs. Update the initial paragraphs.",
      "Hello world\ndemo\nhello world demo\n\n",
      0u,
      17u,
      4u,
      paragraphsIndices03,
      paragraphsNumberOfCharacters03,
    },
    {
      "Some paragraphs. Update the mid paragraphs.",
      "Hello world\ndemo\nhello world demo\n\n",
      12u,
      5u,
      4u,
      paragraphsIndices03,
      paragraphsNumberOfCharacters03,
    },
    {
      "Some paragraphs. Update the final paragraphs.",
      "Hello world\ndemo\nhello world demo\n\n",
      17u,
      18u,
      4u,
      paragraphsIndices03,
      paragraphsNumberOfCharacters03,
    },
  };
  const unsigned int numberOfTests = 5u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    // ToolkitTestApplication application;
    if( !CreateParagraphTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliFindParagraph(void)
{
  tet_infoline(" UtcDaliFindParagraph");

  unsigned int paragraphs01[] = {};
  unsigned int paragraphs02[] = { 0u, 1u, 2u };
  unsigned int paragraphs03[] = { 0u };
  unsigned int paragraphs04[] = { 1u };
  unsigned int paragraphs05[] = { 0u, 1u, 2u };

  struct FindParagraphData data[] =
  {
    {
      "Zero characters",
      "",
      0u,
      100u,
      0u,
      paragraphs01,
    },
    {
      "Some paragraphs",
      "Hello world\ndemo\n\n",
      0u,
      18u,
      3u,
      paragraphs02
    },
    {
      "Some paragraphs",
      "Hello world\ndemo\n\n",
      0u,
      12u,
      1u,
      paragraphs03
    },
    {
      "Some paragraphs",
      "Hello world\ndemo\n\n",
      12u,
      5u,
      1u,
      paragraphs04
    },
    {
      "Some paragraphs",
      "Hello world\ndemo\n\n",
      3u,
      15u,
      3u,
      paragraphs05
    },
  };
  const unsigned int numberOfTests = 5u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    ToolkitTestApplication application;
    if( !FindParagraphTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliSetVisualToLogicalMap(void)
{
  tet_infoline(" UtcDaliSetVisualToLogicalMap");

  unsigned int* visualToLogical01 = NULL;
  unsigned int* visualToLogical02 = NULL;
  unsigned int  visualToLogical03[] = { 12u, 11u, 10u, 9u, 8u, 7u, 6u, 5u, 4u, 3u, 2u, 1u, 0u };
  unsigned int  visualToLogical04[] = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 25u, 24u, 23u, 22u, 21u, 20u, 19u, 18u, 17u, 16u, 15u, 14u, 13u, };
  unsigned int  visualToLogical05[] = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 26u, 25u, 24u, 23u, 22u, 21u, 20u, 19u, 18u, 17u, 16u, 15u, 14u };
  unsigned int  visualToLogical06[] = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 25u, 24u, 23u, 22u, 21u, 20u, 19u, 18u, 17u, 16u, 15u, 14u, 13u, 26u, 27u, 28u, 29u, 30u, 31u, 32u, 33u, 34u, 35u, 36u, 37u, 38u, 39u, 54u, 53u, 52u, 51u, 50u, 49u, 48u, 47u, 46u, 45u, 44u, 43u, 42u, 41u, 40u, 67u, 66u, 55u, 56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u, 64u, 65u, 81u, 80u, 79u, 78u, 77u, 76u, 75u, 74u, 73u, 72u, 71u, 70u, 69u, 68u, 95u, 94u, 93u, 92u, 91u, 90u, 89u, 88u, 87u, 86u, 85u, 84u, 83u, 82u, 96u, 97u, 98u, 99u, 100u, 101u, 102u, 103u, 104u, 105u, 106u };
  unsigned int  visualToLogical07[] = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 25u, 24u, 23u, 22u, 21u, 20u, 19u, 18u, 17u, 16u, 15u, 14u, 13u, 26u, 27u, 28u, 29u, 30u, 31u, 32u, 33u, 34u, 35u, 36u, 37u, 38u, 39u, 54u, 53u, 52u, 51u, 50u, 49u, 48u, 47u, 46u, 45u, 44u, 43u, 42u, 41u, 40u, 67u, 66u, 55u, 56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u, 64u, 65u, 81u, 80u, 79u, 78u, 77u, 76u, 75u, 74u, 73u, 72u, 71u, 70u, 69u, 68u, 95u, 94u, 93u, 92u, 91u, 90u, 89u, 88u, 87u, 86u, 85u, 84u, 83u, 82u, 96u, 97u, 98u, 99u, 100u, 101u, 102u, 103u, 104u, 105u, 106u };
  unsigned int  visualToLogical08[] = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 25u, 24u, 23u, 22u, 21u, 20u, 19u, 18u, 17u, 16u, 15u, 14u, 13u, 26u, 27u, 28u, 29u, 30u, 31u, 32u, 33u, 34u, 35u, 36u, 37u, 38u, 39u, 54u, 53u, 52u, 51u, 50u, 49u, 48u, 47u, 46u, 45u, 44u, 43u, 42u, 41u, 40u, 67u, 66u, 55u, 56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u, 64u, 65u, 81u, 80u, 79u, 78u, 77u, 76u, 75u, 74u, 73u, 72u, 71u, 70u, 69u, 68u, 95u, 94u, 93u, 92u, 91u, 90u, 89u, 88u, 87u, 86u, 85u, 84u, 83u, 82u, 96u, 97u, 98u, 99u, 100u, 101u, 102u, 103u, 104u, 105u, 106u };
  unsigned int  visualToLogical09[] = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 25u, 24u, 23u, 22u, 21u, 20u, 19u, 18u, 17u, 16u, 15u, 14u, 13u, 26u, 27u, 28u, 29u, 30u, 31u, 32u, 33u, 34u, 35u, 36u, 37u, 38u, 39u, 54u, 53u, 52u, 51u, 50u, 49u, 48u, 47u, 46u, 45u, 44u, 43u, 42u, 41u, 40u, 67u, 66u, 55u, 56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u, 64u, 65u, 81u, 80u, 79u, 78u, 77u, 76u, 75u, 74u, 73u, 72u, 71u, 70u, 69u, 68u, 95u, 94u, 93u, 92u, 91u, 90u, 89u, 88u, 87u, 86u, 85u, 84u, 83u, 82u, 96u, 97u, 98u, 99u, 100u, 101u, 102u, 103u, 104u, 105u, 106u };

  struct SetVisualToLogicalMapData data[] =
  {
    {
      "Zero characters text",
      "",
      0u,
      0u,
      Size( 100.f, 300.f ),
      0u,
      visualToLogical01
    },
    {
      "Left to right text only",
      "Hello world",
      0u,
      11u,
      Size( 100.f, 300.f ),
      0u,
      visualToLogical02
    },
    {
      "Right to left text only",
      "مرحبا بالعالم",
      0u,
      13u,
      Size( 100.f, 300.f ),
      13u,
      visualToLogical03
    },
    {
      "Mix of left to right and right to left text.",
      "Hello world, مرحبا بالعالم",
      0u,
      26u,
      Size( 100.f, 300.f ),
      26u,
      visualToLogical04
    },
    {
      "Mix of left to right and right to left text.",
      "Hello world, \nمرحبا بالعالم",
      0u,
      27u,
      Size( 100.f, 300.f ),
      27u,
      visualToLogical05
    },
    {
      "Mix of left to right and right to left text.",
      "Hello world, مرحبا بالعالم, hello world\nمرحبا بالعالم, hello world, مرحبا بالعالم\nمرحبا بالعالم\nhello world",
      0u,
      107u,
      Size( 100.f, 300.f ),
      107u,
      visualToLogical06
    },
    {
      "Mix of left to right and right to left text. Updates from character index 5",
      "Hello world, مرحبا بالعالم, hello world\nمرحبا بالعالم, hello world, مرحبا بالعالم\nمرحبا بالعالم\nhello world",
      5u,
      107u,
      Size( 100.f, 300.f ),
      107u,
      visualToLogical07
    },
    {
      "Mix of left to right and right to left text. Updates from character index 39",
      "Hello world, مرحبا بالعالم, hello world\nمرحبا بالعالم, hello world, مرحبا بالعالم\nمرحبا بالعالم\nhello world",
      39u,
      107u,
      Size( 100.f, 300.f ),
      107u,
      visualToLogical08
    },
    {
      "Mix of left to right and right to left text. Updates from character index 70",
      "Hello world, مرحبا بالعالم, hello world\nمرحبا بالعالم, hello world, مرحبا بالعالم\nمرحبا بالعالم\nhello world",
      70u,
      107u,
      Size( 100.f, 300.f ),
      107u,
      visualToLogical09
    }
  };
  const unsigned int numberOfTests = 9u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    ToolkitTestApplication application;
    if( !SetVisualToLogicalMapTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}
