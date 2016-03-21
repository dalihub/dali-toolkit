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

#include <dali/devel-api/text-abstraction/bidirectional-support.h>
#include <dali-toolkit/internal/text/bidirectional-support.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/text-run-container.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <toolkit-text-model.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the following functions.
//
// void SetBidirectionalInfo( const Vector<Character>& text,
//                            const Vector<ScriptRun>& scripts,
//                            const Vector<LineBreakInfo>& lineBreakInfo,
//                            CharacterIndex startIndex,
//                            Length numberOfCharacters,
//                            Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo )
// void ReorderLines( const Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo,
//                    CharacterIndex startIndex,
//                    Length numberOfCharacters,
//                    Vector<LineRun>& lineRuns,
//                    Vector<BidirectionalLineInfoRun>& lineInfoRuns )
// bool GetMirroredText( const Vector<Character>& text,
//                       Vector<CharacterDirection>& directions,
//                       const Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo,
//                       CharacterIndex startIndex,
//                       Length numberOfCharacters,
//                       Vector<Character>& mirroredText )
// void GetCharactersDirection( const Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo,
//                              CharacterIndex startIndex,
//                              Length numberOfCharacters,
//                              Vector<CharacterDirection>& directions )

//////////////////////////////////////////////////////////

namespace
{

struct SetBidirectionalInfoData
{
  std::string   description;                 ///< Description of the test.
  std::string   text;                        ///< Input text.
  unsigned int  startIndex;                  ///< The index from where the model is updated.
  unsigned int  numberOfCharacters;          ///< The number of characters to update.
  unsigned int  numberOfParagraphs;          ///< The expected number of bidirectional paragraphs.
  unsigned int* indices;                     ///< The expected indices to the first character of each paragraph.
  unsigned int* numberOfParagraphCharacters; ///< The expected number of characters of each paragraph.
  bool*         directions;                  ///< The expected direction of each paragraph.
};

struct BidiLineData
{
  unsigned int  characterIndex;
  unsigned int  numberOfCharacters;
  unsigned int* visualToLogical;
};

struct ReorderLinesData
{
  std::string   description;        ///< Description of the test.
  std::string   text;               ///< Input text.
  unsigned int  startIndex;         ///< The index from where the model is updated.
  unsigned int  numberOfCharacters; ///< The number of characters.
  unsigned int  numberOfLineInfo;   ///< The number or reordered lines.
  BidiLineData* bidiLineData;       ///< The bidirectional line info.
};

struct GetMirroredTextData
{
  std::string  description;        ///< Description of the test.
  std::string  text;               ///< Input text.
  unsigned int startIndex;         ///< The index from where the model is updated.
  unsigned int numberOfCharacters; ///< The number of the characters.
  std::string  mirroredText;       ///< The expected result.
};

struct GetCharactersDirectionData
{
  std::string  description;         ///< Description of the test.
  std::string  text;                ///< Input text.
  unsigned int startIndex;          ///< The index from where the model is updated.
  unsigned int numberOfCharacters;  ///< The number of characters.
  bool*        directions;          ///< The expected directions.
};

bool SetBidirectionalInfoTest( const SetBidirectionalInfoData& data )
{
  // 1) Create the model.
  LogicalModelPtr logicalModel = LogicalModel::New();
  VisualModelPtr visualModel = VisualModel::New();
  Size textArea(100.f, 60.f);
  Size layoutSize;

  // Create the model.
  CreateTextModel( data.text,
                   textArea,
                   layoutSize,
                   logicalModel,
                   visualModel );

  // 2) Clear the bidirectional paragraph info data.
  Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo = logicalModel->mBidirectionalParagraphInfo;
  if( 0u != data.numberOfCharacters )
  {
    ClearCharacterRuns( data.startIndex,
                        data.startIndex + data.numberOfCharacters - 1u,
                        bidirectionalInfo );
  }

  // 3) Call the SetBidirectionalInfo() function.
  SetBidirectionalInfo( logicalModel->mText,
                        logicalModel->mScriptRuns,
                        logicalModel->mLineBreakInfo,
                        data.startIndex,
                        data.numberOfCharacters,
                        bidirectionalInfo );

  // 4) Compare with the expected results.
  TextAbstraction::BidirectionalSupport bidirectionalSupport = TextAbstraction::BidirectionalSupport::Get();

  if( data.numberOfParagraphs != bidirectionalInfo.Count() )
  {
    // Different number of expected bidirectional paragraphs.
    std::cout << "  Different number of bidi paragraphs : " << bidirectionalInfo.Count() << ", expected : " << data.numberOfParagraphs << std::endl;
    return false;
  }

  for( unsigned int index = 0u; index < data.numberOfParagraphs; ++index )
  {
    const BidirectionalParagraphInfoRun& run = bidirectionalInfo[index];

    const CharacterDirection direction = bidirectionalSupport.GetParagraphDirection( run.bidirectionalInfoIndex );
    if( direction != data.directions[index] )
    {
      std::cout << "  Different direction" << std::endl;
      std::cout << "        paragraph : " << index << std::endl;
      std::cout << "            index : " << run.characterRun.characterIndex << ", num chars : " << run.characterRun.numberOfCharacters << ", direction : " << direction << std::endl;
      std::cout << "  expected, index : " << data.indices[index] << ", num chars : " << data.numberOfParagraphCharacters[index] << ", direction : " << data.directions[index] << std::endl;
      return false;
    }

    if( run.characterRun.characterIndex != data.indices[index] )
    {
      std::cout << "  Different index" << std::endl;
      std::cout << "        paragraph : " << index << std::endl;
      std::cout << "            index : " << run.characterRun.characterIndex << ", num chars : " << run.characterRun.numberOfCharacters << ", direction : " << direction << std::endl;
      std::cout << "  expected, index : " << data.indices[index] << ", num chars : " << data.numberOfParagraphCharacters[index] << ", direction : " << data.directions[index] << std::endl;
      return false;
    }
    if( run.characterRun.numberOfCharacters != data.numberOfParagraphCharacters[index] )
    {
      std::cout << "  Different number of characters" << std::endl;
      std::cout << "        paragraph : " << index << std::endl;
      std::cout << "            index : " << run.characterRun.characterIndex << ", num chars : " << run.characterRun.numberOfCharacters << ", direction : " << direction << std::endl;
      std::cout << "  expected, index : " << data.indices[index] << ", num chars : " << data.numberOfParagraphCharacters[index] << ", direction : " << data.directions[index] << std::endl;
      return false;
    }
  }

  return true;
}

/**
 * @brief Frees previously allocated bidirectional resources.
 *
 * @param[in] bidirectionalLineInfo Bidirectional info per line.
 * @param[in] startIndex Index to the first line with bidirectional info to be freed.
 * @param[in] endIndex Index to the last line with bidirectional info to be freed.
 */
void FreeBidirectionalLineInfoResources( Vector<BidirectionalLineInfoRun> bidirectionalLineInfo,
                                         uint32_t startIndex,
                                         uint32_t endIndex )
{
  // Free the allocated memory used to store the conversion table in the bidirectional line info run.
  for( Vector<BidirectionalLineInfoRun>::Iterator it = bidirectionalLineInfo.Begin() + startIndex,
         endIt = bidirectionalLineInfo.Begin() + endIndex;
       it != endIt;
       ++it )
  {
    BidirectionalLineInfoRun& bidiLineInfo = *it;

    free( bidiLineInfo.visualToLogicalMap );
  }
}

bool ReorderLinesTest( const ReorderLinesData& data )
{
  // 1) Create the model.
  LogicalModelPtr logicalModel = LogicalModel::New();
  VisualModelPtr visualModel = VisualModel::New();
  Size textArea(100.f, 300.f);
  Size layoutSize;

  // Create the model.
  CreateTextModel( data.text,
                   textArea,
                   layoutSize,
                   logicalModel,
                   visualModel );

  // 2) Clear the bidirectional line info data.
  uint32_t startRemoveIndex = logicalModel->mBidirectionalLineInfo.Count();
  uint32_t endRemoveIndex = startRemoveIndex;
  ClearCharacterRuns( data.startIndex,
                      data.startIndex + data.numberOfCharacters - 1u,
                      logicalModel->mBidirectionalLineInfo,
                      startRemoveIndex,
                      endRemoveIndex );

  // Index to the first index to be removed.

  FreeBidirectionalLineInfoResources( logicalModel->mBidirectionalLineInfo, startRemoveIndex, endRemoveIndex );
  BidirectionalLineInfoRun* bidiLineBuffer = logicalModel->mBidirectionalLineInfo.Begin();
  logicalModel->mBidirectionalLineInfo.Erase( bidiLineBuffer + startRemoveIndex,
                                              bidiLineBuffer + endRemoveIndex );

  // 3) Call the function ReorderLines()

  ReorderLines( logicalModel->mBidirectionalParagraphInfo,
                data.startIndex,
                data.numberOfCharacters,
                visualModel->mLines,
                logicalModel->mBidirectionalLineInfo );

  // 4) Compare the results.

  if( data.numberOfLineInfo != logicalModel->mBidirectionalLineInfo.Count() )
  {
    // Different number of bidirectional lines.
    std::cout << "  different number of bidi lines : " << logicalModel->mBidirectionalLineInfo.Count() << ", expected : " << data.numberOfLineInfo << std::endl;
    for( unsigned int index = 0u; index < logicalModel->mBidirectionalLineInfo.Count(); ++index )
    {
      const BidirectionalLineInfoRun& run = logicalModel->mBidirectionalLineInfo[index];
      const BidiLineData& bidiLineData = data.bidiLineData[index];

      std::cout << "  bidi line : " << index << ", index : " << run.characterRun.characterIndex << ", num chars : " << run.characterRun.numberOfCharacters << std::endl;
      std::cout << "                      expected index : " << bidiLineData.characterIndex << ", num chars : " << bidiLineData.numberOfCharacters << std::endl;
    }
    return false;
  }

  for( unsigned int index = 0u; index < data.numberOfLineInfo; ++index )
  {
    const BidirectionalLineInfoRun& run = logicalModel->mBidirectionalLineInfo[index];
    const BidiLineData& bidiLineData = data.bidiLineData[index];

    if( bidiLineData.characterIndex != run.characterRun.characterIndex )
    {
      std::cout << "  bidi line : " << index << ", index : " << run.characterRun.characterIndex << ", num chars : " << run.characterRun.numberOfCharacters << std::endl;
      std::cout << "                      expected index : " << bidiLineData.characterIndex << ", num chars : " << bidiLineData.numberOfCharacters << std::endl;
      return false;
    }
    if( bidiLineData.numberOfCharacters != run.characterRun.numberOfCharacters )
    {
      std::cout << "  bidi line : " << index << ", index : " << run.characterRun.characterIndex << ", num chars : " << run.characterRun.numberOfCharacters << std::endl;
      std::cout << "                      expected index : " << bidiLineData.characterIndex << ", num chars : " << bidiLineData.numberOfCharacters << std::endl;
      return false;
    }

    for( unsigned int i = 0u; i < run.characterRun.numberOfCharacters; ++i )
    {
      if( bidiLineData.visualToLogical[i] != run.visualToLogicalMap[i] )
      {
        std::cout << "           v2l : ";
        for( unsigned int i = 0u; i < run.characterRun.numberOfCharacters; ++i )
        {
          std::cout << run.visualToLogicalMap[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "  expected v2l : ";
        for( unsigned int i = 0u; i < run.characterRun.numberOfCharacters; ++i )
        {
          std::cout << bidiLineData.visualToLogical[i] << " ";
        }
        std::cout << std::endl;

        return false;
      }
    }
  }

  return true;
}

bool GetMirroredTextTest( const GetMirroredTextData& data )
{
  // 1) Create the model.
  LogicalModelPtr logicalModel = LogicalModel::New();
  VisualModelPtr visualModel = VisualModel::New();
  Size textArea(100.f, 60.f);
  Size layoutSize;

  // Create the model.
  CreateTextModel( data.text,
                   textArea,
                   layoutSize,
                   logicalModel,
                   visualModel );

  // 2) Call the GetMirroredText() function for the whole text
  Vector<Character> mirroredText;
  bool mirrored = false;
  mirrored = GetMirroredText( logicalModel->mText,
                              logicalModel->mCharacterDirections,
                              logicalModel->mBidirectionalParagraphInfo,
                              0u,
                              logicalModel->mText.Count(),
                              mirroredText );

  // 3) Call the GetMirroredText() function for the given index + number of characters
  mirrored = GetMirroredText( logicalModel->mText,
                              logicalModel->mCharacterDirections,
                              logicalModel->mBidirectionalParagraphInfo,
                              data.startIndex,
                              data.numberOfCharacters,
                              mirroredText );

  // 4) Compare the results.

  // Convert to utf8
  std::string mirroredString;
  Utf32ToUtf8( mirroredText.Begin(),
               mirroredText.Count(),
               mirroredString );

  if( !mirrored && ( mirroredString != data.text ) )
  {
    std::cout << "  No mirrored text and mirroredString != data.text." << std::endl;
    std::cout << "  mirrored string : [" << mirroredString << "]" << std::endl;
    std::cout << "             text : [" << data.text << "]" << std::endl;
    return false;
  }

  if( mirrored && ( mirroredString == data.text ) )
  {
    std::cout << "  Mirrored text and mirroredString == data.text." << std::endl;
    std::cout << "  mirrored string : [" << mirroredString << "]" << std::endl;
    std::cout << "             text : [" << data.text << "]" << std::endl;
    return false;
  }

  if( mirrored && ( mirroredString != data.mirroredText ) )
  {
    std::cout << "  Mirrored text and mirroredString != data.mirroredText." << std::endl;
    std::cout << "  mirrored string : [" << mirroredString << "]" << std::endl;
    std::cout << "             text : [" << data.mirroredText << "]" << std::endl;
    return false;
  }

  return true;
}

bool GetCharactersDirectionTest( const GetCharactersDirectionData& data )
{
  // 1) Create the model.
  LogicalModelPtr logicalModel = LogicalModel::New();
  VisualModelPtr visualModel = VisualModel::New();
  Size textArea(100.f, 60.f);
  Size layoutSize;

  // Create the model.
  CreateTextModel( data.text,
                   textArea,
                   layoutSize,
                   logicalModel,
                   visualModel );

  Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo = logicalModel->mBidirectionalParagraphInfo;

  // 2) Clear the direction info data.
  Vector<CharacterDirection>& directions = logicalModel->mCharacterDirections;

  directions.Erase( directions.Begin() + data.startIndex,
                    directions.Begin() + data.startIndex + data.numberOfCharacters );

  // 3) Call GetCharactersDirection() function.

  GetCharactersDirection( bidirectionalInfo,
                          logicalModel->mText.Count(),
                          data.startIndex,
                          data.numberOfCharacters,
                          directions );

  for( unsigned int index = 0u; index < logicalModel->mText.Count(); ++index )
  {
    if( data.directions[index] != directions[index] )
    {
      return false;
    }
  }

  return true;
}

} // namespace

//////////////////////////////////////////////////////////

int UtcDaliSetBidirectionalInfo(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliSetBidirectionalInfo");

  unsigned int indices01[] = {};
  unsigned int numberOfCharacters01[] = {};
  bool         direction01[] = {};
  unsigned int indices02[] = {};
  unsigned int numberOfCharacters02[] = {};
  bool         direction02[] = {};
  unsigned int indices03[] = { 17u, 48u };
  unsigned int numberOfCharacters03[] = { 14u, 14u };
  bool         direction03[] = { true, true };
  unsigned int indices04[] = { 17u, 31u, 79u };
  unsigned int numberOfCharacters04[] = { 14u, 48u, 31u };
  bool         direction04[] = { true, false, true };
  unsigned int indices05[] = { 17u, 41u, 117u };
  unsigned int numberOfCharacters05[] = { 24u, 76u, 49u };
  bool         direction05[] = { true, false, true };
  unsigned int indices06[] = { 17u, 48u };
  unsigned int numberOfCharacters06[] = { 14u, 14u };
  bool         direction06[] = { true, true };
  unsigned int indices07[] = { 17u, 31u, 79u };
  unsigned int numberOfCharacters07[] = { 14u, 48u, 31u };
  bool         direction07[] = { true, false, true };
  unsigned int indices08[] = { 17u, 41u, 117u };
  unsigned int numberOfCharacters08[] = { 24u, 76u, 49u };
  bool         direction08[] = { true, false, true };

  struct SetBidirectionalInfoData data[] =
  {
    {
      "Zero characters",
      "",
      0u,
      0u,
      0u,
      indices01,
      numberOfCharacters01,
      direction01
    },
    {
      "Some left to right paragraphs",
      "Hello world\ndemo\n\n",
      0u,
      18u,
      0u,
      indices02,
      numberOfCharacters02,
      direction02
    },
    {
      "A mix of left to right and right to left paragraphs.",
      "Hello world demo\nمرحبا بالعالم\nhello world demo\nمرحبا بالعالم\nhello world demo",
      0u,
      78u,
      2u,
      indices03,
      numberOfCharacters03,
      direction03
    },
    {
      "A mix of left to right and right to left paragraphs. Paragraphs also contain a mix of bidirectional text.",
      "Hello world demo\nمرحبا بالعالم\nhello world demo مرحبا بالعالم hello world demo\nمرحبا hello world demo بالعالم\nhello world demo",
      0u,
      126u,
      3u,
      indices04,
      numberOfCharacters04,
      direction04
    },
    {
      "A mix of left to right and right to left paragraphs. Paragraphs also contain a mix of bidirectional text and a mix of right to left scripts.",
      "Hello world demo\nمرحبا שלום עולם بالعالم\nhello world שלום بالعالم עולם demo مرحبا שלום עולם بالعالم hello world demo\nمرحبا hello שלום بالعالم עולם world demo بالعالم\nhello world demo",
      0u,
      182u,
      3u,
      indices05,
      numberOfCharacters05,
      direction05
    },
    {
      "A mix of left to right and right to left paragraphs. Updates a left to right paragraph.",
      "Hello world demo\nمرحبا بالعالم\nhello world demo\nمرحبا بالعالم\nhello world demo",
      31u,
      17u,
      2u,
      indices06,
      numberOfCharacters06,
      direction06
    },
    {
      "A mix of left to right and right to left paragraphs. Paragraphs also contain a mix of bidirectional text.",
      "Hello world demo\nمرحبا بالعالم\nhello world demo مرحبا بالعالم hello world demo\nمرحبا hello world demo بالعالم\nhello world demo",
      0u,
      126u,
      3u,
      indices07,
      numberOfCharacters07,
      direction07
    },
    {
      "A mix of left to right and right to left paragraphs. Paragraphs also contain a mix of bidirectional text and a mix of right to left scripts. Updates initial paragraphs.",
      "Hello world demo\nمرحبا שלום עולם بالعالم\nhello world שלום بالعالم עולם demo مرحبا שלום עולם بالعالم hello world demo\nمرحبا hello שלום بالعالم עולם world demo بالعالم\nhello world demo",
      0u,
      41u,
      3u,
      indices08,
      numberOfCharacters08,
      direction08
    },
    {
      "A mix of left to right and right to left paragraphs. Paragraphs also contain a mix of bidirectional text and a mix of right to left scripts. Updates mid paragraphs.",
      "Hello world demo\nمرحبا שלום עולם بالعالم\nhello world שלום بالعالم עולם demo مرحبا שלום עולם بالعالم hello world demo\nمرحبا hello שלום بالعالم עולם world demo بالعالم\nhello world demo",
      41u,
      76u,
      3u,
      indices08,
      numberOfCharacters08,
      direction08
    },
    {
      "A mix of left to right and right to left paragraphs. Paragraphs also contain a mix of bidirectional text and a mix of right to left scripts. Updates from character 85",
      "Hello world demo\nمرحبا שלום עולם بالعالم\nhello world שלום بالعالم עולם demo مرحبا שלום עולם بالعالم hello world demo\nمرحبا hello שלום بالعالم עולם world demo بالعالم\nhello world demo",
      117u,
      65u,
      3u,
      indices08,
      numberOfCharacters08,
      direction08
    },
  };
  const unsigned int numberOfTests = 10u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    if( !SetBidirectionalInfoTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliReorderLines(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliSetBidirectionalInfo");

  unsigned int visualToLogical0301[] = { 0u, 1u, 2u, 3u, 4u, 5u, 9u, 8u, 7u, 6u, 10u };
  unsigned int visualToLogical0302[] = { 3u, 2u, 1u, 0u, 4u, 5u, 6u, 7u, 8u, 9u, 10u };
  unsigned int visualToLogical0303[] = { 0u, 1u, 2u, 3u, 4u };
  unsigned int visualToLogical0304[] = { 12u, 11u, 10u, 9u, 8u, 7u, 6u, 5u, 4u, 3u, 2u, 1u, 0u };
  unsigned int visualToLogical0305[] = { 10u, 9u, 8u, 7u, 6u, 5u, 4u, 3u, 2u, 1u, 0u };
  unsigned int visualToLogical0306[] = { 9u, 8u, 7u, 6u, 5u, 4u, 3u, 2u, 1u, 0u };
  unsigned int visualToLogical0307[] = { 13u, 8u, 9u, 10u, 11u, 12u, 7u, 6u, 5u, 4u, 3u, 2u, 1u, 0u };
  unsigned int visualToLogical0308[] = { 5u, 0u, 1u, 2u, 3u, 4u };
  unsigned int visualToLogical0309[] = { 0u, 1u, 2u, 3u, 4u, 5u, 9u, 8u, 7u, 6u, 10u };
  unsigned int visualToLogical0310[] = { 3u, 2u, 1u, 0u, 4u, 5u, 6u, 7u, 8u, 9u, 10u };
  unsigned int visualToLogical0311[] = { 0u, 1u, 2u, 3u, 4u };
  unsigned int visualToLogical0312[] = { 12u, 11u, 10u, 9u, 8u, 7u, 6u, 5u, 4u, 3u, 2u, 1u, 0u };
  unsigned int visualToLogical0313[] = { 10u, 9u, 8u, 7u, 6u, 5u, 4u, 3u, 2u, 1u, 0u };
  unsigned int visualToLogical0314[] = { 9u, 8u, 7u, 6u, 5u, 4u, 3u, 2u, 1u, 0u };
  unsigned int visualToLogical0315[] = { 13u, 8u, 9u, 10u, 11u, 12u, 7u, 6u, 5u, 4u, 3u, 2u, 1u, 0u };
  unsigned int visualToLogical0316[] = { 0u, 1u, 2u, 3u, 4u };

  BidiLineData bidiLine01[] = {};
  BidiLineData bidiLine02[] = {};
  BidiLineData bidiLine03[] = {
    {
      17u,
      11u,
      visualToLogical0301
    },
    {
      28u,
      11u,
      visualToLogical0302
    },
    {
      39u,
      5u,
      visualToLogical0303
    },
    {
      44u,
      13u,
      visualToLogical0304
    },
    {
      57u,
      11u,
      visualToLogical0305
    },
    {
      68u,
      10u,
      visualToLogical0306
    },
    {
      78u,
      14u,
      visualToLogical0307
    },
    {
      92u,
      6u,
      visualToLogical0308
    },
    {
      115u,
      11u,
      visualToLogical0309
    },
    {
      126u,
      11u,
      visualToLogical0310
    },
    {
      137u,
      5u,
      visualToLogical0311
    },
    {
      142u,
      13u,
      visualToLogical0312
    },
    {
      155u,
      11u,
      visualToLogical0313
    },
    {
      166u,
      10u,
      visualToLogical0314
    },
    {
      176u,
      14u,
      visualToLogical0315
    },
    {
      190u,
      5u,
      visualToLogical0316
    },
  };

  struct ReorderLinesData data[] =
  {
    {
      "Zero characters.",
      "",
      0u,
      0u,
      0u,
      bidiLine01
    },
    {
      "Left to right text only.",
      "Hello world demo\nhello world demo\nhello world demo.",
      0u,
      51u,
      0u,
      bidiLine02
    },
    {
      "Bidirectional paragraphs.",
      "Hello world demo\nhello שלום עולם world demo\nשלום بالعالم עולם مرحبا שלום עולם بالعالم hello world\nHello world demo\nhello שלום עולם world demo\nשלום بالعالم עולם مرحبا שלום עולם بالعالم hello world",
      0u,
      195u,
      16u,
      bidiLine03
    },
    {
      "Bidirectional paragraphs. Update initial paragraphs.",
      "Hello world demo\nhello שלום עולם world demo\nשלום بالعالم עולם مرحبا שלום עולם بالعالم hello world\nHello world demo\nhello שלום עולם world demo\nשלום بالعالم עולם مرحبا שלום עולם بالعالم hello world",
      0u,
      44u,
      16u,
      bidiLine03
    },
    {
      "Bidirectional paragraphs. Update middle paragraphs.",
      "Hello world demo\nhello שלום עולם world demo\nשלום بالعالم עולם مرحبا שלום עולם بالعالم hello world\nHello world demo\nhello שלום עולם world demo\nשלום بالعالم עולם مرحبا שלום עולם بالعالم hello world",
      44u,
      54u,
      16u,
      bidiLine03
    },
    {
      "Bidirectional paragraphs. Update final paragraphs.",
      "Hello world demo\nhello שלום עולם world demo\nשלום بالعالم עולם مرحبا שלום עולם بالعالم hello world\nHello world demo\nhello שלום עולם world demo\nשלום بالعالم עולם مرحبا שלום עולם بالعالم hello world",
      142u,
      53u,
      16u,
      bidiLine03
    },
  };
  const unsigned int numberOfTests = 6u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    if( !ReorderLinesTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliGetMirroredText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliGetMirroredText");

  struct GetMirroredTextData data[] =
  {
    {
      "Zero characters.",
      "",
      0u,
      0u,
      ""
    },
    {
      "Left to right characters only.",
      "Hello world\nhello world demo.",
      0u,
      29u,
      "Hello world\nhello world demo."
    },
    {
      "Right to left characters but with no characters to mirror.",
      "שלום עולם\nمرحبا بالعالم",
      0u,
      23u,
      "שלום עולם\nمرحبا بالعالم"
    },
    {
      "Right to left characters with some characters to mirror.",
      "שלום עולם\n(مرحبا بالعالم)",
      0u,
      25u,
      "שלום עולם\n)مرحبا بالعالم("
    },
    {
      "Right to left characters with some characters to mirror. Update last paragraph.",
      "שלום עולם\n(مرحبا بالعالم)",
      10u,
      15u,
      "שלום עולם\n)مرحبا بالعالم("
    },
    {
      "Mix of bidirectional text. With more paragraphs.",
      "Hello world demo\nhello world\nhello world (مرحبا بالعالم שלום) עולם\nשלום مرحبا بالعالم עולם (hello) مرحبا بالعالم world"
      " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום (hello) world demo (עולם)\nשלום (مرحبا بالعالم עולם) (hello)",
      0u,
      239u,
      "Hello world demo\nhello world\nhello world (مرحبا بالعالم שלום( עולם\nשלום مرحبا بالعالم עולם )hello( مرحبا بالعالم world"
      " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום )hello) world demo )עולם(\nשלום )مرحبا بالعالم עולם( )hello("
    },
    {
      "Mix of bidirectional text. With more paragraphs. Update middle paragraphs.",
      "Hello world demo\nhello world\nhello world (مرحبا بالعالم שלום) עולם\nשלום مرحبا بالعالم עולם (hello) مرحبا بالعالم world"
      " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום (hello) world demo (עולם)\nשלום (مرحبا بالعالم עולם) (hello)",
      29u,
      38u,
      "Hello world demo\nhello world\nhello world (مرحبا بالعالم שלום( עולם\nשלום مرحبا بالعالم עולם (hello) مرحبا بالعالم world"
      " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום (hello) world demo (עולם)\nשלום (مرحبا بالعالم עולם) (hello)"
    },
    {
      "Mix of bidirectional text. With more paragraphs. Update middle paragraphs (2).",
      "Hello world demo\nhello world\nhello world (مرحبا بالعالم שלום) עולם\nשלום مرحبا بالعالم עולם (hello) مرحبا بالعالم world"
      " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום (hello) world demo (עולם)\nשלום (مرحبا بالعالم עולם) (hello)",
      67u,
      100u,
      "Hello world demo\nhello world\nhello world (مرحبا بالعالم שלום) עולם\nשלום مرحبا بالعالم עולם )hello( مرحبا بالعالم world"
      " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום (hello) world demo (עולם)\nשלום (مرحبا بالعالم עולם) (hello)"
    },
  };
  const unsigned int numberOfTests = 8u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    if( !GetMirroredTextTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliGetCharactersDirection(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliGetCharactersDirection");

  bool directions01[] = {};
  bool directions02[] = {
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false };
  bool directions03[] = {
    true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true };
  bool directions04[] = {
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, true,  true,  true,  true,  true,  true,
    true,  true,  true,  false, true,  true,  true,  true,  true,  true,
    true,  true,  true,  true,  false, false, false, false, false, false,
    false, false, false, false, false };
  bool directions05[] = {
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, true,  true,  true,  true,  true,  true,  true,  true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
    true,  true,  true,  true,  false, true,  true,  true,  true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true,  false,
    false, false, false, false, true,  true,  true,  true,  true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true,  false,
    false, false, false, false, true,  true,  true,  true,  true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true,  false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, true,  true,  true,  true,  true,  true,  true,  true,
    true,  true,  true,  true,  true,  true,  false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, true,  true,  true,  true,  true,  true,  true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
    true,  true,  false, false, false, false, false };

  struct GetCharactersDirectionData data[] =
  {
    {
      "Zero characters",
      "",
      0u,
      0u,
      directions01
    },
    {
      "Left to right characters only",
      "Hello world\nhello world demo",
      0u,
      28u,
      directions02
    },
    {
      "Right to left characters only",
      "שלום עולם\nשלום עולם",
      0u,
      19u,
      directions03
    },
    {
      "Mix of bidirectional text",
      "Hello world\nhello world שלום עולם\nשלום עולם hello world",
      0u,
      55u,
      directions04
    },
    {
      "Mix of bidirectional text. With more paragraphs.",
      "Hello world demo\nhello world\nhello world مرحبا بالعالم שלום עולם\nשלום مرحبا بالعالم עולם hello مرحبا بالعالم world"
      " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום hello world demo עולם\nשלום مرحبا بالعالم עולם hello",
      0u,
      227u,
      directions05
    },
    {
      "Mix of bidirectional text. With more paragraphs. Update first paragraph.",
      "Hello world demo\nhello world\nhello world مرحبا بالعالم שלום עולם\nשלום مرحبا بالعالم עולם hello مرحبا بالعالم world"
      " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום hello world demo עולם\nשלום مرحبا بالعالم עולם hello",
      0u,
      17u,
      directions05
    },
    {
      "Mix of bidirectional text. With more paragraphs. Update from character 29",
      "Hello world demo\nhello world\nhello world مرحبا بالعالم שלום עולם\nשלום مرحبا بالعالم עולם hello مرحبا بالعالم world"
      " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום hello world demo עולם\nשלום مرحبا بالعالم עולם hello",
      29u,
      134u,
      directions05
    },
    {
      "Mix of bidirectional text. With more paragraphs. Update from character 163",
      "Hello world demo\nhello world\nhello world مرحبا بالعالم שלום עולם\nשלום مرحبا بالعالم עולם hello مرحبا بالعالم world"
      " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום hello world demo עולם\nשלום مرحبا بالعالم עולם hello",
      163u,
      35u,
      directions05
    }
  };
  const unsigned int numberOfTests = 8u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    if( !GetCharactersDirectionTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}
