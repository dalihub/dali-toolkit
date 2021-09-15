/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <unistd.h>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/internal/text/text-run-container.h>
#include <dali-toolkit/dali-toolkit.h>
#include <toolkit-text-utils.h>


using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the following functions.
//
// void CreateParagraphInfo( CharacterIndex startIndex,
//                           Length numberOfCharacters );
// void FindParagraphs( CharacterIndex index,
//                      Length numberOfCharacters,
//                      Vector<ParagraphRunIndex>& paragraphs );
// bool FetchBidirectionalLineInfo( CharacterIndex characterIndex )
// CharacterIndex GetLogicalCharacterIndex( CharacterIndex visualCharacterIndex ) const;
// CharacterIndex GetLogicalCursorIndex( CharacterIndex visualCursorIndex ) const;

//////////////////////////////////////////////////////////

namespace
{
const std::string DEFAULT_FONT_DIR( "/resources/fonts" );

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

struct FetchBidirectionalLineInfoData
{
  std::string   description;    ///< Description of the test.
  std::string   text;           ///< Input text.
  unsigned int  numberOfTests;  ///< The number of tests.
  unsigned int* characterIndex; ///< The logical character index.
  bool*         fetched;        ///< Whether the expected bidi line exists.
  unsigned int* bidiLineIndex;  ///< Index to the expected bidi line.
};

struct GetLogicalCharacterIndexData
{
  std::string   description;        ///< Description of the test.
  std::string   text;               ///< Input text.
  Size          textArea;           ///< The text area.
  unsigned int  numberOfIndices;    ///< The number of characters to set.
  unsigned int* visualToLogical;    ///< The expected visual to logical conversion table.
  unsigned int* cachedBidiLine;     ///< The cached bidi line index for each character.
};

struct GetLogicalCursorIndexData
{
  std::string         description;        ///< Description of the test.
  std::string         text;               ///< Input text.
  Size                textArea;           ///< The text area.
  unsigned int        numberOfFonts;      ///< The number of fonts.
  FontDescriptionRun* fontDescriptions;   ///< The font descriptions.
  unsigned int        numberOfIndices;    ///< The number of characters to set.
  unsigned int*       visualCursorIndex;  ///< The given cursor visual index.
  unsigned int*       characterIndex;     ///< Index to the first logical character of the line.
  unsigned int*       logicalCursorIndex; ///< The expected cursor logical index.
  unsigned int*       cachedBidiLine;     ///< The cached bidi line index for each character.
};

bool CreateParagraphTest( const CreateParagraphData& data )
{
  // 1) Create the model.
  ModelPtr textModel;
  MetricsPtr metrics;
  Size textArea(100.f, 60.f);
  Size layoutSize;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  LayoutOptions options;
  CreateTextModel( data.text,
                   textArea,
                   fontDescriptionRuns,
                   options,
                   layoutSize,
                   textModel,
                   metrics,
                   false,
                   LineWrap::WORD,
                   false,
                   Toolkit::DevelText::EllipsisPosition::END,
                   0.f );

  LogicalModelPtr logicalModel = textModel->mLogicalModel;
  VisualModelPtr visualModel = textModel->mVisualModel;

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
  ModelPtr textModel;
  MetricsPtr metrics;
  Size textArea(100.f, 60.f);
  Size layoutSize;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  LayoutOptions options;
  CreateTextModel( data.text,
                   textArea,
                   fontDescriptionRuns,
                   options,
                   layoutSize,
                   textModel,
                   metrics,
                   false,
                   LineWrap::WORD,
                   false,
                   Toolkit::DevelText::EllipsisPosition::END,
                   0.f );

  LogicalModelPtr logicalModel = textModel->mLogicalModel;
  VisualModelPtr visualModel = textModel->mVisualModel;

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

bool FetchBidirectionalLineInfoTest( const FetchBidirectionalLineInfoData& data )
{
  std::cout << "  testing : " << data.description << std::endl;
  // Create the model.
  ModelPtr textModel;
  MetricsPtr metrics;
  Size textArea( 100.f, 300.f );
  Size layoutSize;

  // Create the model with the whole text.
  const Vector<FontDescriptionRun> fontDescriptions;
  const LayoutOptions options;
  CreateTextModel( data.text,
                   textArea,
                   fontDescriptions,
                   options,
                   layoutSize,
                   textModel,
                   metrics,
                   false,
                   LineWrap::WORD,
                   false,
                   Toolkit::DevelText::EllipsisPosition::END,
                   0.f );

  LogicalModelPtr logicalModel = textModel->mLogicalModel;
  VisualModelPtr visualModel = textModel->mVisualModel;

  for( unsigned int index = 0; index < data.numberOfTests; ++index )
  {
    const bool fetched = logicalModel->FetchBidirectionalLineInfo( data.characterIndex[index] );

    if( fetched != data.fetched[index] )
    {
      std::cout << "  Different fetched result : " << fetched << ", expected : " << data.fetched[index] << std::endl;
      return false;
    }

    if( fetched )
    {
      if( logicalModel->mBidirectionalLineIndex != data.bidiLineIndex[index] )
      {
        std::cout << "  Different bidi line index : " << logicalModel->mBidirectionalLineIndex << ", expected : " << data.bidiLineIndex << std::endl;
        return false;
      }
    }
  }

  return true;
}

bool GetLogicalCharacterIndexTest( const GetLogicalCharacterIndexData& data )
{
  std::cout << "  testing : " << data.description << std::endl;
  // Create the model.
  ModelPtr textModel;
  MetricsPtr metrics;
  Size layoutSize;

  // Create the model with the whole text.
  const Vector<FontDescriptionRun> fontDescriptions;
  const LayoutOptions options;
  CreateTextModel( data.text,
                   data.textArea,
                   fontDescriptions,
                   options,
                   layoutSize,
                   textModel,
                   metrics,
                   false,
                   LineWrap::WORD,
                   false,
                   Toolkit::DevelText::EllipsisPosition::END,
                   0.f );

  LogicalModelPtr logicalModel = textModel->mLogicalModel;
  VisualModelPtr visualModel = textModel->mVisualModel;

  for( unsigned int index = 0u; index < data.numberOfIndices; ++index )
  {
    // Check the current cached bidi line index. (Check it before call the GetLogicalCharacterIndex() method )
    if( data.cachedBidiLine[index] != logicalModel->mBidirectionalLineIndex )
    {
      std::cout << "  index : " << index << ", different cached bidi index : " << logicalModel->mBidirectionalLineIndex << ", expected : " << data.cachedBidiLine[index] << std::endl;
      return false;
    }

    const bool fetched = logicalModel->FetchBidirectionalLineInfo( index );
    const Character logicalIndex = fetched ? logicalModel->GetLogicalCharacterIndex( index ) : index;

    if( data.visualToLogical[index] != logicalIndex )
    {
      std::cout << "  visual index : " << index << ", different logical index : " << logicalIndex << ", expected : " << data.visualToLogical[index] << std::endl;
      return false;
    }
  }
  return true;
}

bool GetLogicalCursorIndexTest( const GetLogicalCursorIndexData& data )
{
  tet_printf( "  testing : %s\n", data.description.c_str() );

  // Load some fonts.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi( 96u, 96u );

  char* pathNamePtr = get_current_dir_name();
  const std::string pathName( pathNamePtr );
  free( pathNamePtr );

  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf" );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansHebrewRegular.ttf" );

  // Create the model.
  ModelPtr textModel;
  MetricsPtr metrics;
  Size layoutSize;

  // Create the model with the whole text.
  Vector<FontDescriptionRun> fontDescriptionRuns;
  if( 0u != data.numberOfFonts )
  {
    fontDescriptionRuns.Insert( fontDescriptionRuns.End(),
                                data.fontDescriptions,
                                data.fontDescriptions + data.numberOfFonts );
  }

  const LayoutOptions options;
  CreateTextModel( data.text,
                   data.textArea,
                   fontDescriptionRuns,
                   options,
                   layoutSize,
                   textModel,
                   metrics,
                   false,
                   LineWrap::WORD,
                   false,
                   Toolkit::DevelText::EllipsisPosition::END,
                   0.f );

  LogicalModelPtr logicalModel = textModel->mLogicalModel;
  VisualModelPtr visualModel = textModel->mVisualModel;

  for( unsigned int index = 0u; index < data.numberOfIndices; ++index )
  {
    const bool fetched = logicalModel->FetchBidirectionalLineInfo( data.characterIndex[index] );
    tet_printf("  fetched %d, line index %d, expected line index %d\n", fetched, logicalModel->mBidirectionalLineIndex, data.cachedBidiLine[index] );

    if( logicalModel->mBidirectionalLineIndex != data.cachedBidiLine[index] )
    {
      tet_printf( "  test : %d, different cached line index : %d, expected : %d\n", index, logicalModel->mBidirectionalLineIndex, data.cachedBidiLine[index] );
      return false;
    }

    const CharacterIndex visualCharacterIndex = data.visualCursorIndex[index];
    const CharacterIndex logicalCursorIndex = fetched ? logicalModel->GetLogicalCursorIndex( visualCharacterIndex ) : visualCharacterIndex;
    tet_printf("  visual index %d, logical index %d\n", visualCharacterIndex, logicalCursorIndex);

    if( logicalCursorIndex != data.logicalCursorIndex[index] )
    {
      tet_printf( "  test : %d, visual index : %d, different logical cursor index :%d, expected : %d\n", index, visualCharacterIndex, logicalCursorIndex, data.logicalCursorIndex[index] );
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
// UtcDaliFetchBidirectionalLineInfo
// UtcDaliGetLogicalCharacterIndex
// UtcDaliGetLogicalCursorIndex
//
//////////////////////////////////////////////////////////

int UtcDaliCreateParagraph(void)
{
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
    ToolkitTestApplication application;
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

int UtcDaliFetchBidirectionalLineInfo(void)
{
  tet_infoline(" UtcDaliFetchBidirectionalLineInfo");

  unsigned int logicalIndex01[] = { 0u };
  bool fetched01[] = { false };
  unsigned int bidiLine01[] = { 0u };

  unsigned int logicalIndex02[] = { 3u };
  bool fetched02[] = { false };
  unsigned int bidiLine02[] = { 0u };

  unsigned int logicalIndex03[] = { 0u, 11u, 12u, 21u, 22u, 33u, 34u, 43u, 44u, 54u};
  bool fetched03[] = { false, false, true, true, false, false, true, true, false, false };
  unsigned int bidiLine03[] = { 0u, 0u, 0u, 0u, 0u, 0u, 1u, 1u, 0u, 0u };

  struct FetchBidirectionalLineInfoData data[] =
  {
    {
      "Void text",
      "",
      1u,
      logicalIndex01,
      fetched01,
      bidiLine01
    },
    {
      "LTR text",
      "Hello world",
      1u,
      logicalIndex02,
      fetched02,
      bidiLine02
    },
    {
      "Bidi text",
      "Hello world\nשלום עולם\nhello world\nשלום עולם\nhello world",
      10u,
      logicalIndex03,
      fetched03,
      bidiLine03
    }
  };
  const unsigned int numberOfTests = 3u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    ToolkitTestApplication application;
    if( !FetchBidirectionalLineInfoTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliGetLogicalCharacterIndex(void)
{
  tet_infoline(" UtcDaliSetVisualToLogicalMap");

  unsigned int visualToLogical01[] = {};
  unsigned int  cachedBidiLine01[] = {};
  unsigned int visualToLogical02[] = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u };
  unsigned int  cachedBidiLine02[] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,  0u };
  unsigned int visualToLogical03[] = { 12u, 11u, 10u, 9u, 8u, 7u, 6u, 5u, 4u, 3u, 2u, 1u, 0u };
  unsigned int  cachedBidiLine03[] = {  0u,  0u,  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u };

  unsigned int visualToLogical04[] = { 0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u, 10u, 11u, 12u, 25u, 24u, 23u, 22u, 21u, 20u, 19u, 18u, 17u, 16u, 15u, 14u, 13u, 26u, 27u, 28u, 29u, 30u, 31u, 32u, 33u, 34u, 35u, 36u, 37u, 38u, 39u, 81u, 80u, 79u, 78u, 77u, 76u, 75u, 74u, 73u, 72u, 71u, 70u, 69u, 68u, 67u, 66u, 55u, 56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u, 64u, 65u, 54u, 53u, 52u, 51u, 50u, 49u, 48u, 47u, 46u, 45u, 44u, 43u, 42u, 41u, 40u, 95u, 94u, 93u, 92u, 91u, 90u, 89u, 88u, 87u, 86u, 85u, 84u, 83u, 82u, 96u, 97u, 98u, 99u, 100u, 101u, 102u, 103u, 104u, 105u, 106u };
  unsigned int  cachedBidiLine04[] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
                                       0u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
                                       1u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u };

// size 300, 300
// LO   H  e  l  l  o  _  w  o  r  l  d  ,  _  م  ر  ح  ب   ا  _  ب  ا  ل  ع   ا  ل  م  ,   _  h  e  l  l  o  _  w  o  r  l  d \n
//      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39
// VO   H  e  l  l  o  _  w  o  r  l  d  ,  _  م  ل  ا  ع   ل  ا  ب  _  ا   ب  ح  ر  م  ,   _  h  e  l  l  o  _  w  o  r  l  d \n
//      0  1  2  3  4  5  6  7  8  9 10 11 12 25 24 23 22 21 20 19 18 17 16 15 14 13 26 27 28 29 30 31 32 33 34 35 36 37 38 39

// LO   م  ر  ح  ب   ا  _  ب  ا  ل  ع   ا  ل  م  ,  _  h  e  l  l  o  _  w  o  r  l  d   ,  _  م  ر  ح  ب   ا  _  ب  ا  ل  ع   ا  ل  م  \n
//     40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81
// VO  \n  م  ل  ا   ع  ل  ا  ب  _  ا   ب  ح  ر  م  _  ,  h  e  l  l  o  _  w  o  r  l  d   _  ,  م  ل  ا   ع  ل  ا  ب  _  ا   ب  ح  ر  م
//     81 80 79 78 77 76 75 74 73 72 71 70 69 68 67 66 55 56 57 58 59 60 61 62 63 64 65 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40

// LO   م  ر  ح  ب   ا  _  ب  ا  ل  ع   ا  ل  م  \n
//     82 83 84 85 86 87 88 89 90 91 92 93 94 95
// VO  \n  م  ل  ا  ع  ل   ا  ب  _  ا   ب  ح  ر  م
//     95 94 93 92 91 90 89 88 87 86 85 84 83 82


// LO   h   e   l   l   o   _   w   o   r   l   d
//     96  97  98  99 100 101 102 103 104 105 106
// VO   h   e   l   l   o   _   w   o   r   l   d
//     96  97  98  99 100 101 102 103 104 105 106

  unsigned int visualToLogical05[] = { 0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u, 10u, 11u, 12u, 25u, 24u, 23u, 22u, 21u, 20u, 19u, 18u, 17u, 16u, 15u, 14u, 13u, 26u, 27u, 28u, 29u, 30u, 31u, 32u, 33u, 34u, 35u, 36u, 37u, 38u, 39u, 67u, 66u, 55u, 56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u, 64u, 65u, 54u, 53u, 52u, 51u, 50u, 49u, 48u, 47u, 46u, 45u, 44u, 43u, 42u, 41u, 40u, 81u, 80u, 79u, 78u, 77u, 76u, 75u, 74u, 73u, 72u, 71u, 70u, 69u, 68u, 95u, 94u, 93u, 92u, 91u, 90u, 89u, 88u, 87u, 86u, 85u, 84u, 83u, 82u, 96u, 97u, 98u, 99u, 100u, 101u, 102u, 103u, 104u, 105u, 106u };
  unsigned int  cachedBidiLine05[] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u };

// size 300, 300
// LO   H  e  l  l  o  _  w  o  r  l  d  ,  _  م  ر  ح  ب   ا  _  ب  ا  ل  ع   ا  ل  م  ,   _
//      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27
// VO   H  e  l  l  o  _  w  o  r  l  d  ,  _  م  ل  ا  ع   ل  ا  ب  _  ا   ب  ح  ر  م  ,   _
//      0  1  2  3  4  5  6  7  8  9 10 11 12 25 24 23 22 21 20 19 18 17 16 15 14 13 26 27

// LO    h  e  l  l  o  _  w  o  r  l  d \n
//      28 29 30 31 32 33 34 35 36 37 38 39
// VO    h  e  l  l  o  _  w  o  r  l  d \n
//      28 29 30 31 32 33 34 35 36 37 38 39

// LO   م  ر  ح  ب   ا  _  ب  ا  ل  ع   ا  ل  م  ,  _  h  e  l  l  o  _  w  o  r  l  d   ,  _
//     40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67
// VO  _  ,  h  e  l  l  o  _  w  o  r  l  d   _  ,  م  ل  ا   ع  ل  ا  ب  _  ا   ب  ح  ر  م
//     67 66 55 56 57 58 59 60 61 62 63 64 65 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40

// LO   م  ر  ح  ب   ا  _  ب  ا  ل  ع   ا  ل  م  \n
//     68 69 70 71 72 73 74 75 76 77 78 79 80 81
// VO  \n  م  ل  ا   ع  ل  ا  ب  _  ا   ب  ح  ر  م
//     81 80 79 78 77 76 75 74 73 72 71 70 69 68

// LO   م  ر  ح  ب   ا  _  ب  ا  ل  ع   ا  ل  م  \n
//     82 83 84 85 86 87 88 89 90 91 92 93 94 95
// VO  \n  م  ل  ا  ع  ل   ا  ب  _  ا   ب  ح  ر  م
//     95 94 93 92 91 90 89 88 87 86 85 84 83 82


// LO   h   e   l   l   o   _   w   o   r   l   d
//     96  97  98  99 100 101 102 103 104 105 106
// VO   h   e   l   l   o   _   w   o   r   l   d
//     96  97  98  99 100 101 102 103 104 105 106

  unsigned int visualToLogical06[] = { 0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u, 10u, 11u, 12u, 25u, 24u, 23u, 22u, 21u, 20u, 19u, 18u, 17u, 16u, 15u, 14u, 13u, 26u, 27u, 28u, 29u, 30u, 31u, 32u, 33u, 34u, 35u, 36u, 37u, 38u, 39u, 54u, 53u, 52u, 51u, 50u, 49u, 48u, 47u, 46u, 45u, 44u, 43u, 42u, 41u, 40u, 67u, 66u, 55u, 56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u, 64u, 65u, 81u, 80u, 79u, 78u, 77u, 76u, 75u, 74u, 73u, 72u, 71u, 70u, 69u, 68u, 95u, 94u, 93u, 92u, 91u, 90u, 89u, 88u, 87u, 86u, 85u, 84u, 83u, 82u, 96u, 97u, 98u, 99u, 100u, 101u, 102u, 103u, 104u, 105u, 106u };
  unsigned int  cachedBidiLine06[] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
                                       0u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
                                       1u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u,
                                       2u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u,
                                       3u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u,
                                       4u, 5u, 5u, 5u, 5u, 5u, 5u, 5u, 5u, 5u, 5u, 5u, 5u, 5u,
                                       5u, 6u, 6u, 6u, 6u, 6u, 6u, 6u, 6u, 6u, 6u, 6u, 6u, 6u,
                                       6u, 6u, 6u, 6u, 6u, 6u, 6u, 6u, 6u, 6u, 6u };

// size 100, 600
// LO   H  e  l  l  o  _  w  o  r  l  d  ,  _
//      0  1  2  3  4  5  6  7  8  9 10 11 12
// VO   H  e  l  l  o  _  w  o  r  l  d  ,  _
//      0  1  2  3  4  5  6  7  8  9 10 11 12

// LO    م  ر  ح  ب   ا  _  ب  ا  ل  ع   ا  ل  م  ,   _
//      13 14 15 16 17 18 19 20 21 22 23 24 25 26 27
// VO    م  ل  ا  ع   ل  ا  ب  _  ا   ب  ح  ر  م  ,   _
//      25 24 23 22 21 20 19 18 17 16 15 14 13 26 27

// LO    h  e  l  l  o  _  w  o  r  l  d \n
//      28 29 30 31 32 33 34 35 36 37 38 39
// VO    h  e  l  l  o  _  w  o  r  l  d \n
//      28 29 30 31 32 33 34 35 36 37 38 39

// LO   م  ر  ح  ب   ا  _  ب  ا  ل  ع   ا  ل  م  ,  _
//     40 41 42 43 44 45 46 47 48 49 50 51 52 53 54
// VO   _  ,  م  ل  ا   ع  ل  ا  ب  _  ا   ب  ح  ر  م
//     54 53 52 51 50 49 48 47 46 45 44 43 42 41 40

// LO   h  e  l  l  o  _  w  o  r  l  d   ,  _
//     55 56 57 58 59 60 61 62 63 64 65 66 67
// VO   _  ,  h  e  l  l  o  _  w  o  r  l  d
//     67 66 55 56 57 58 59 60 61 62 63 64 65

// LO   م  ر  ح  ب   ا  _  ب  ا  ل  ع   ا  ل  م  \n
//     68 69 70 71 72 73 74 75 76 77 78 79 80 81
// VO  \n  م  ل  ا   ع  ل  ا  ب  _  ا   ب  ح  ر  م
//     81 80 79 78 77 76 75 74 73 72 71 70 69 68

// LO   م  ر  ح  ب   ا  _  ب  ا  ل  ع   ا  ل  م  \n
//     82 83 84 85 86 87 88 89 90 91 92 93 94 95
// VO  \n  م  ل  ا  ع  ل   ا  ب  _  ا   ب  ح  ر  م
//     95 94 93 92 91 90 89 88 87 86 85 84 83 82


// LO   h   e   l   l   o   _   w   o   r   l   d
//     96  97  98  99 100 101 102 103 104 105 106
// VO   h   e   l   l   o   _   w   o   r   l   d
//     96  97  98  99 100 101 102 103 104 105 106

  struct GetLogicalCharacterIndexData data[] =
  {
    {
      "Zero characters text",
      "",
      Size( 300.f, 300.f ),
      0u,
      visualToLogical01,
      cachedBidiLine01
    },
    {
      "Left to right text only",
      "Hello world",
      Size( 300.f, 300.f ),
      11u,
      visualToLogical02,
      cachedBidiLine02
    },
    {
      "Right to left text only",
      "مرحبا بالعالم",
      Size( 300.f, 300.f ),
      13u,
      visualToLogical03,
      cachedBidiLine03
    },
    {
      "Mix of left to right and right to left text.",
      "Hello world, مرحبا بالعالم, hello world\nمرحبا بالعالم, hello world, مرحبا بالعالم\nمرحبا بالعالم\nhello world",
      Size( 300.f, 300.f ),
      107u,
      visualToLogical04,
      cachedBidiLine04
    },
    {
      "Mix of left to right and right to left text.",
      "Hello world, مرحبا بالعالم, hello world\nمرحبا بالعالم, hello world, مرحبا بالعالم\nمرحبا بالعالم\nhello world",
      Size( 200.f, 400.f ),
      107u,
      visualToLogical05,
      cachedBidiLine05
    },
    {
      "Mix of left to right and right to left text.",
      "Hello world, مرحبا بالعالم, hello world\nمرحبا بالعالم, hello world, مرحبا بالعالم\nمرحبا بالعالم\nhello world",
      Size( 100.f, 600.f ),
      107u,
      visualToLogical06,
      cachedBidiLine06
    },
  };
  const unsigned int numberOfTests = 6u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    ToolkitTestApplication application;
    if( !GetLogicalCharacterIndexTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliGetLogicalCursorIndex(void)
{
  tet_infoline(" UtcDaliGetLogicalCursorIndex");

  const std::string fontFamily( "TizenSans" );
  const std::string fontFamilyHebrew( "TizenSansHebrew" );



  unsigned int visualIndex01[] = { 10u };
  unsigned int characterIndex01[] = { 0u };
  unsigned int logicalIndex01[] = { 10u };
  unsigned int bidirectionalLineIndex01[] = { 0u };

  //  0           11
  //   Hello world  \n
  // 12    16
  //   demo

  // Set a known font description
  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 0u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 11u;
  fontDescriptionRun02.familyLength = fontFamily.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontFamily.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns02;
  fontDescriptionRuns02.PushBack( fontDescriptionRun02 );

  unsigned int visualIndex02[] = { 0u, 16u, 11u, 12u };
  unsigned int characterIndex02[] = { 0u, 0u, 0u, 0u };
  unsigned int logicalIndex02[] = { 0u, 16u, 11u, 12u };
  unsigned int bidirectionalLineIndex02[] = { 0u, 0u, 0u, 0u };

// LO     H  e  l  l  o  _  w  o  r  l  d  \n
//       0  1  2  3  4  5  6  7  8  9 10 11  12
// VO     H  e  l  l  o  _  w  o  r  l  d  \n

// LO         ש  ל  ו  ם  _  ע  ו  ל  ם \n
//          12 13 14 15 16 17 18 19 20 21  22
// VO     \n  ם  ל  ו  ע _  ם  ו  ל  ש

// LO      h  e  l  l  o  _  w  o  r  l  d  _  ש  ל  ו  ם  _  ע ו  ל  ם  \n
//       22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43  44
// VO      h  e  l  l  o  _  w  o  r  l  d  _  ם  ל  ו  ע  _  ם  ו  ל  ש  \n

// LO      ש  ל  ו  ם  _  ע  ו  ל  ם  _  h  e  l  l  o  _  w  o  r   l  d \n
//       44 45 46 47 48 49 50 51  52 52 54 55 56 57 58 59 60 61 62 63  64 65 66
// VO      \n h  e  l  l  o  _  w   o  r  l  d  _  ם  ל  ו  ע  _  ם  ו  ל  ש


  // Set a known font description
  FontDescriptionRun fontDescriptionRun0301;
  fontDescriptionRun0301.characterRun.characterIndex = 0u;
  fontDescriptionRun0301.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun0301.familyLength = fontFamily.size();
  fontDescriptionRun0301.familyName = new char[fontDescriptionRun0301.familyLength];
  memcpy( fontDescriptionRun0301.familyName, fontFamily.c_str(), fontDescriptionRun0301.familyLength );
  fontDescriptionRun0301.familyDefined = true;
  fontDescriptionRun0301.weightDefined = false;
  fontDescriptionRun0301.widthDefined = false;
  fontDescriptionRun0301.slantDefined = false;
  fontDescriptionRun0301.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun0302;
  fontDescriptionRun0302.characterRun.characterIndex = 12u;
  fontDescriptionRun0302.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun0302.familyLength = fontFamilyHebrew.size();
  fontDescriptionRun0302.familyName = new char[fontDescriptionRun0302.familyLength];
  memcpy( fontDescriptionRun0302.familyName, fontFamilyHebrew.c_str(), fontDescriptionRun0302.familyLength );
  fontDescriptionRun0302.familyDefined = true;
  fontDescriptionRun0302.weightDefined = false;
  fontDescriptionRun0302.widthDefined = false;
  fontDescriptionRun0302.slantDefined = false;
  fontDescriptionRun0302.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun0303;
  fontDescriptionRun0303.characterRun.characterIndex = 22u;
  fontDescriptionRun0303.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun0303.familyLength = fontFamily.size();
  fontDescriptionRun0303.familyName = new char[fontDescriptionRun0303.familyLength];
  memcpy( fontDescriptionRun0303.familyName, fontFamily.c_str(), fontDescriptionRun0303.familyLength );
  fontDescriptionRun0303.familyDefined = true;
  fontDescriptionRun0303.weightDefined = false;
  fontDescriptionRun0303.widthDefined = false;
  fontDescriptionRun0303.slantDefined = false;
  fontDescriptionRun0303.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun0304;
  fontDescriptionRun0304.characterRun.characterIndex = 34u;
  fontDescriptionRun0304.characterRun.numberOfCharacters = 20u;
  fontDescriptionRun0304.familyLength = fontFamilyHebrew.size();
  fontDescriptionRun0304.familyName = new char[fontDescriptionRun0304.familyLength];
  memcpy( fontDescriptionRun0304.familyName, fontFamilyHebrew.c_str(), fontDescriptionRun0304.familyLength );
  fontDescriptionRun0304.familyDefined = true;
  fontDescriptionRun0304.weightDefined = false;
  fontDescriptionRun0304.widthDefined = false;
  fontDescriptionRun0304.slantDefined = false;
  fontDescriptionRun0304.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun0305;
  fontDescriptionRun0305.characterRun.characterIndex = 54u;
  fontDescriptionRun0305.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun0305.familyLength = fontFamily.size();
  fontDescriptionRun0305.familyName = new char[fontDescriptionRun0305.familyLength];
  memcpy( fontDescriptionRun0305.familyName, fontFamily.c_str(), fontDescriptionRun0305.familyLength );
  fontDescriptionRun0305.familyDefined = true;
  fontDescriptionRun0305.weightDefined = false;
  fontDescriptionRun0305.widthDefined = false;
  fontDescriptionRun0305.slantDefined = false;
  fontDescriptionRun0305.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns03;
  fontDescriptionRuns03.PushBack( fontDescriptionRun0301 );
  fontDescriptionRuns03.PushBack( fontDescriptionRun0302 );
  fontDescriptionRuns03.PushBack( fontDescriptionRun0303 );
  fontDescriptionRuns03.PushBack( fontDescriptionRun0304 );
  fontDescriptionRuns03.PushBack( fontDescriptionRun0305 );

  unsigned int visualIndex03[] = {  0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u, 10u, 11u,
                                   13u, 14u, 15u, 16u, 17u, 18u, 19u, 20u, 21u, 22u,
                                   22u, 23u, 24u, 25u, 26u, 27u, 28u, 29u, 30u, 31u, 32u, 33u, 34u, 35u, 36u, 37u, 38u, 39u, 40u, 41u, 42u, 43u,
                                   45u, 46u, 47u, 48u, 49u, 50u, 51u, 52u, 53u, 54u, 55u, 56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u, 64u, 65u, 66u };

  unsigned int characterIndex03[] = {  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,
                                      12u, 12u, 12u, 12u, 12u, 12u, 12u, 12u, 12u, 12u,
                                      22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u,
                                      44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u};

  unsigned int logicalIndex03[] = {  0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u, 10u, 11u,
                                    21u, 20u, 19u, 18u, 17u, 16u, 15u, 14u, 13u, 12u,
                                    22u, 23u, 24u, 25u, 26u, 27u, 28u, 29u, 30u, 31u, 32u, 33u, 34u, 42u, 41u, 40u, 39u, 38u, 37u, 36u, 35u, 43u,
                                    65u, 55u, 56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u, 64u, 54u, 53u, 52u, 51u, 50u, 49u, 48u, 47u, 46u, 45u, 44u };

  unsigned int bidirectionalLineIndex03[] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
                                              0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
                                              1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
                                              2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u };


// LO     ש  ל  ו  ם  _  ע  ו  ל  ם \n
//       0  1  2  3  4  5  6  7  8  9 10
// VO  \n ם  ל  ו  ע  _  ם  ו  ל  ש

//      h  e  l  l  o  _  w  o  r  l  d  \n
// LO 10 11 12 13 14 15 16 17 18 19 20 21 22
//      h  e  l  l  o  _  w  o  r  l  d  \n

//         ש  ל  ו  ם  _  ע  ו  ל  ם _  h  e  l  l  o  _  w  o  r  l  d  \n
// LO    22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44
//     \n  h  e  l  l  o  _  w  o  r  l  d  _  ם  ל  ו  ע  _  ם  ו  ל  ש

//      h  e  l  l  o  _  w  o  r  l  d  _  ש  ל  ו  ם  _  ע  ו  ל  ם \n
// LO 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66
//      h  e  l  l  o  _  w  o  r  l  d  _  ם  ל  ו  ע  _  ם  ו  ל  ש \n

  // Set a known font description
  FontDescriptionRun fontDescriptionRun0401;
  fontDescriptionRun0401.characterRun.characterIndex = 0u;
  fontDescriptionRun0401.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun0401.familyLength = fontFamilyHebrew.size();
  fontDescriptionRun0401.familyName = new char[fontDescriptionRun0401.familyLength];
  memcpy( fontDescriptionRun0401.familyName, fontFamilyHebrew.c_str(), fontDescriptionRun0401.familyLength );
  fontDescriptionRun0401.familyDefined = true;
  fontDescriptionRun0401.weightDefined = false;
  fontDescriptionRun0401.widthDefined = false;
  fontDescriptionRun0401.slantDefined = false;
  fontDescriptionRun0401.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun0402;
  fontDescriptionRun0402.characterRun.characterIndex = 10u;
  fontDescriptionRun0402.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun0402.familyLength = fontFamily.size();
  fontDescriptionRun0402.familyName = new char[fontDescriptionRun0402.familyLength];
  memcpy( fontDescriptionRun0402.familyName, fontFamily.c_str(), fontDescriptionRun0402.familyLength );
  fontDescriptionRun0402.familyDefined = true;
  fontDescriptionRun0402.weightDefined = false;
  fontDescriptionRun0402.widthDefined = false;
  fontDescriptionRun0402.slantDefined = false;
  fontDescriptionRun0402.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun0403;
  fontDescriptionRun0403.characterRun.characterIndex = 22u;
  fontDescriptionRun0403.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun0403.familyLength = fontFamilyHebrew.size();
  fontDescriptionRun0403.familyName = new char[fontDescriptionRun0403.familyLength];
  memcpy( fontDescriptionRun0403.familyName, fontFamilyHebrew.c_str(), fontDescriptionRun0403.familyLength );
  fontDescriptionRun0403.familyDefined = true;
  fontDescriptionRun0403.weightDefined = false;
  fontDescriptionRun0403.widthDefined = false;
  fontDescriptionRun0403.slantDefined = false;
  fontDescriptionRun0403.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun0404;
  fontDescriptionRun0404.characterRun.characterIndex = 32u;
  fontDescriptionRun0404.characterRun.numberOfCharacters = 24u;
  fontDescriptionRun0404.familyLength = fontFamily.size();
  fontDescriptionRun0404.familyName = new char[fontDescriptionRun0404.familyLength];
  memcpy( fontDescriptionRun0404.familyName, fontFamily.c_str(), fontDescriptionRun0404.familyLength );
  fontDescriptionRun0404.familyDefined = true;
  fontDescriptionRun0404.weightDefined = false;
  fontDescriptionRun0404.widthDefined = false;
  fontDescriptionRun0404.slantDefined = false;
  fontDescriptionRun0404.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun0405;
  fontDescriptionRun0405.characterRun.characterIndex = 56u;
  fontDescriptionRun0405.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun0405.familyLength = fontFamilyHebrew.size();
  fontDescriptionRun0405.familyName = new char[fontDescriptionRun0405.familyLength];
  memcpy( fontDescriptionRun0405.familyName, fontFamilyHebrew.c_str(), fontDescriptionRun0405.familyLength );
  fontDescriptionRun0405.familyDefined = true;
  fontDescriptionRun0405.weightDefined = false;
  fontDescriptionRun0405.widthDefined = false;
  fontDescriptionRun0405.slantDefined = false;
  fontDescriptionRun0405.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns04;
  fontDescriptionRuns04.PushBack( fontDescriptionRun0401 );
  fontDescriptionRuns04.PushBack( fontDescriptionRun0402 );
  fontDescriptionRuns04.PushBack( fontDescriptionRun0403 );
  fontDescriptionRuns04.PushBack( fontDescriptionRun0404 );
  fontDescriptionRuns04.PushBack( fontDescriptionRun0405 );

  unsigned int  visualIndex04[] = {  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u, 10u,
                                    10u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u, 20u, 21u,
                                    23u, 24u, 25u, 26u, 27u, 28u, 29u, 30u, 31u, 32u, 33u, 34u, 35u, 36u, 37u, 38u, 39u, 40u, 41u, 42u, 43u, 44u,
                                    44u, 45u, 46u, 47u, 48u, 49u, 50u, 51u, 52u, 53u, 54u, 55u, 56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u, 64u, 65u };

  unsigned int characterIndex04[] = {  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,
                                      10u, 10u, 10u, 10u, 10u, 10u, 10u, 10u, 10u, 10u, 10u,
                                      22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 22u,
                                      44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u, 44u };

  unsigned int logicalIndex04[] = {  9u,  8u,  7u,  6u,  5u,  4u,  3u,  2u,  1u,  0u,
                                    10u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u, 20u, 21u,
                                    43u, 33u, 34u, 35u, 36u, 37u, 38u, 39u, 40u, 41u, 42u, 32u, 31u, 30u, 29u, 28u, 27u, 26u, 25u, 24u, 23u, 22u,
                                    44u, 45u, 46u, 47u, 48u, 49u, 50u, 51u, 52u, 53u, 54u, 55u, 56u, 64u, 63u, 62u, 61u, 60u, 59u, 58u, 57u, 65u };

  unsigned int bidirectionalLineIndex04[] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
                                              0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
                                              1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
                                              2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u };

// LO   A  B  C  D  E  F  G  H  I  J  K
//     0  1  2  3  4  5  6  7  8  9 10 11
// LO   L  M  N
//    11 12 13 14

  // Set a known font description
  FontDescriptionRun fontDescriptionRun0501;
  fontDescriptionRun0501.characterRun.characterIndex = 0u;
  fontDescriptionRun0501.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun0501.familyLength = fontFamily.size();
  fontDescriptionRun0501.familyName = new char[fontDescriptionRun0501.familyLength];
  memcpy( fontDescriptionRun0501.familyName, fontFamily.c_str(), fontDescriptionRun0501.familyLength );
  fontDescriptionRun0501.familyDefined = true;
  fontDescriptionRun0501.weightDefined = false;
  fontDescriptionRun0501.widthDefined = false;
  fontDescriptionRun0501.slantDefined = false;
  fontDescriptionRun0501.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns05;
  fontDescriptionRuns05.PushBack( fontDescriptionRun0501 );

  unsigned int  visualIndex05[] = {  0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u, 10u,
                                    11u, 12u, 13u, 14u };

  unsigned int characterIndex05[] = { 0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,
                                      11u, 11u, 11u, 11u };

  unsigned int logicalIndex05[] = { 0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u, 10u,
                                    11u, 12u, 13u, 14u };

  unsigned int bidirectionalLineIndex05[] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
                                              0u, 0u, 0u, 0u };

// LO      ק  ר  א  ט  ו  ן  ם  פ  ש  ד  ג  כ
//        0  1  2  3  4  5  6  7  8  9  10 11 12
// VO      כ  ג  ד  ש  פ  ם  ן  ו  ט  א  ר  ק

// LO      ע  י  ח  ל
//       12 13 14 15 16
// VO      ל  ח  י  ע


  // Set a known font description
  FontDescriptionRun fontDescriptionRun0601;
  fontDescriptionRun0601.characterRun.characterIndex = 0u;
  fontDescriptionRun0601.characterRun.numberOfCharacters = 16u;
  fontDescriptionRun0601.familyLength = fontFamilyHebrew.size();
  fontDescriptionRun0601.familyName = new char[fontDescriptionRun0601.familyLength];
  memcpy( fontDescriptionRun0601.familyName, fontFamilyHebrew.c_str(), fontDescriptionRun0601.familyLength );
  fontDescriptionRun0601.familyDefined = true;
  fontDescriptionRun0601.weightDefined = false;
  fontDescriptionRun0601.widthDefined = false;
  fontDescriptionRun0601.slantDefined = false;
  fontDescriptionRun0601.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns06;
  fontDescriptionRuns06.PushBack( fontDescriptionRun0601 );

  unsigned int  visualIndex06[] = {  0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u, 10u, 11u, 12u,
                                    12u, 13u, 14u, 15u, 16u };

  unsigned int characterIndex06[] = {  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,
                                      12u, 12u, 12u, 12u, 12u };

  unsigned int logicalIndex06[] = { 12u, 11u, 10u,  9u,  8u,  7u,  6u,  5u,  4u,  3u,  2u,  1u, 0u,
                                    16u, 15u, 14u, 13u, 12u };

  unsigned int bidirectionalLineIndex06[] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
                                              1u, 1u, 1u, 1u, 1u, };

  struct GetLogicalCursorIndexData data[] =
  {
    {
      "Zero characters text",
      "",
      Size( 300.f, 300.f ),
      0u,
      nullptr,
      1u,
      visualIndex01,
      characterIndex01,
      logicalIndex01,
      bidirectionalLineIndex01,
    },
    {
      "All left to right text 01.",
      "Hello world\ndemo",
      Size( 300.f, 300.f ),
      1u,
      fontDescriptionRuns02.Begin(),
      4u,
      visualIndex02,
      characterIndex02,
      logicalIndex02,
      bidirectionalLineIndex02,
    },
    {
      "bidirectional text 01.",
      "Hello world\nשלום עולם\nhello world שלום עולם\nשלום עולם hello world\n",
      Size( 300.f, 300.f ),
      5u,
      fontDescriptionRuns03.Begin(),
      65u,
      visualIndex03,
      characterIndex03,
      logicalIndex03,
      bidirectionalLineIndex03,
    },
    {
      "bidirectional text 02.",
      "שלום עולם\nhello world\nשלום עולם hello world\nhello world שלום עולם\n",
      Size( 300.f, 300.f ),
      5u,
      fontDescriptionRuns04.Begin(),
      65u,
      visualIndex04,
      characterIndex04,
      logicalIndex04,
      bidirectionalLineIndex04,
    },
    {
      "long line 01.",
      "ABCDEFGHIJKLMN",
      Size( 100.f, 300.f ),
      1u,
      fontDescriptionRuns05.Begin(),
      13u,
      visualIndex05,
      characterIndex05,
      logicalIndex05,
      bidirectionalLineIndex05,
    },
    {
      "bidirectional text 03.",
      "קראטוןםפשדגכעיחל",
      Size( 100.f, 300.f ),
      1u,
      fontDescriptionRuns06.Begin(),
      18u,
      visualIndex06,
      characterIndex06,
      logicalIndex06,
      bidirectionalLineIndex06,
    },
  };
  const unsigned int numberOfTests = 6u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    ToolkitTestApplication application;
    if( !GetLogicalCursorIndexTest( data[index] ) )
    {
      tet_printf("Test %d failed : [%s]\n", index, data[index].description.c_str());
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}
