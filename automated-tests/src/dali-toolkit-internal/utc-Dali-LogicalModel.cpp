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
// void CreateParagraphInfo( CharacterIndex startIndex,
//                           Length numberOfCharacters );
// void FindParagraphs( CharacterIndex index,
//                      Length numberOfCharacters,
//                      Vector<ParagraphRunIndex>& paragraphs );
// CharacterIndex GetLogicalCharacterIndex( CharacterIndex visualCharacterIndex ) const;
// CharacterIndex GetLogicalCursorIndex( CharacterIndex visualCursorIndex ) const;

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
  std::string    description;        ///< Description of the test.
  std::string    text;               ///< Input text.
  unsigned int   numberOfIndices;    ///< The number of characters to set.
  unsigned int*  visualCursorIndex;  ///< The given cursor visual index.
  unsigned int*  logicalCursorIndex; ///< The expected cursor logical index
  unsigned int*  cachedBidiLine;     ///< The cached bidi line index for each character.
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

bool GetLogicalCharacterIndexTest( const GetLogicalCharacterIndexData& data )
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

  for( unsigned int index = 0u; index < data.numberOfIndices; ++index )
  {
    // Check the current cached bidi line index. (Check it before call the GetLogicalCharacterIndex() method )
    if( data.cachedBidiLine[index] != logicalModel->mBidirectionalLineIndex )
    {
      std::cout << "  index : " << index << ", different cached bidi index : " << logicalModel->mBidirectionalLineIndex << ", expected : " << data.cachedBidiLine[index] << std::endl;
      return false;
    }

    const Character logicalIndex = logicalModel->GetLogicalCharacterIndex( index );
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
  std::cout << "  testing : " << data.description << std::endl;
  // Create the model.
  LogicalModelPtr logicalModel = LogicalModel::New();
  VisualModelPtr visualModel = VisualModel::New();
  Size textArea( 300.f, 300.f );
  Size layoutSize;

  // Create the model with the whole text.
  const Vector<FontDescriptionRun> fontDescriptions;
  const LayoutOptions options;
  CreateTextModel( data.text,
                   textArea,
                   fontDescriptions,
                   options,
                   layoutSize,
                   logicalModel,
                   visualModel );

  for( unsigned int index = 0u; index < data.numberOfIndices; ++index )
  {
    if( logicalModel->mBidirectionalLineIndex != data.cachedBidiLine[index] )
    {
      std::cout << "  test : " << index << ", different cached line index : " << logicalModel->mBidirectionalLineIndex << ", expected : " << data.cachedBidiLine[index] << std::endl;
      return false;
    }

    const CharacterIndex logicalCursorIndex = logicalModel->GetLogicalCursorIndex( data.visualCursorIndex[index] );

    if( logicalCursorIndex != data.logicalCursorIndex[index] )
    {
      std::cout << "  test : " << index << ", different logical cursor index : " << logicalCursorIndex << ", expected : " << data.logicalCursorIndex[index] << std::endl;
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
// UtcDaliGetLogicalCharacterIndex
// UtcDaliGetLogicalCursorIndex
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

  unsigned int visualIndex01[] = { 10u };
  unsigned int logicalIndex01[] = { 10u };
  unsigned int bidirectionalLineIndex01[] = { 0u };

  unsigned int visualIndex02[] = { 0u, 16u, 11u, 12u };
  unsigned int logicalIndex02[] = { 0u, 16u, 11u, 12u };
  unsigned int bidirectionalLineIndex02[] = { 0u, 0u, 0u, 0u };



// LO     H  e  l  l  o  _  w  o  r  l  d  ,  _  \n
//       0  1  2  3  4  5  6  7  8  9 10 11 12 13 14
// VO     H  e  l  l  o  _  w  o  r  l  d  ,  _  \n

// LO      ש  ל  ו  ם  _  ע  ו  ל  ם  ,  _ \n
//       14 15 16 17 18 19 20 21 22 23 24 25 26
// VO      \n _  ,  ם  ל  ו  ע  _  ם  ו  ל  ש

// LO      h  e  l  l  o  _  w  o  r  l  d  ,  _ \n
//       26 27 28 29 30 31 32 33 34 35 36 37 38 39 40
// VO      h  e  l  l  o  _  w  o  r  l  d  ,  _ \n

// LO      h  e  l  l  o  _  w  o  r  l  d  ,  _  ש  ל  ו  ם  _  ע  ו  ל  ם \n
//       40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63
// VO      h  e  l  l  o  _  w  o  r  l  d  ,  _  ם  ל  ו  ע  _  ם  ו  ל  ש \n

// LO      ש  ל  ו  ם  _  ע  ו  ל  ם
//       63 64 65 66 67 68 69 70 71 72
// VO      ם  ל  ו  ע  _  ם  ו  ל  ש

  unsigned int visualIndex03[] = { 0u, 18u, 25u, 60u, 54u, 65u, 0u, 18u, 65u, 33u };
  unsigned int logicalIndex03[] = { 0u, 22u, 15u, 55u, 61u, 70u, 0u, 22u, 70u, 33u };
  unsigned int bidirectionalLineIndex03[] = { 0u, 0u, 0u, 0u, 1u, 1u, 2u, 2u, 0u, 2u };


// LO      ש  ל  ו  ם  _  ע  ו  ל  ם  ,  _ \n
//        0  1  2  3  4  5  6  7  8  9 10 11 12
// VO      \n ,  ם  ל  ו  ע  _  ם  ו  ל  ש

// LO      h  e  l  l  o  _  w  o  r  l  d  ,  _  \n
//       12 13 14 15 16 17 18 19 20 21 22 23 24 25  26
// VO      h  e  l  l  o  _  w  o  r  l  d  ,  _  \n

// LO      h  e  l  l  o  _  w  o  r  l  d  ,  _  ש  ל  ו  ם  _  ע  ו  ל  ם  \n
//       26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48  49
// VO      h  e  l  l  o  _  w  o  r  l  d  ,  _  ש  ל  ו  ם  _  ע  ו  ל  ם  \n

// LO      ש  ל  ו  ם  _  ע  ו  ל  ם  ,  _  \n
//       49 50 51 52 53 54 55 56 57 58 59 60  61
// VO      ם  ל  ו  ע  _  ם  ו  ל  ש  ,  \n

// LO      h  e  l  l  o  _  w  o  r  l  d
//       61 62 63 64 65 66 67 68 69 70 71 72
// VO      h  e  l  l  o  _  w  o  r  l  d


  unsigned int visualIndex04[] = { 0u };
  unsigned int logicalIndex04[] = { 72u };
  unsigned int bidirectionalLineIndex04[] = { 0u };



  struct GetLogicalCursorIndexData data[] =
  {
    {
      "Zero characters text",
      "",
      1u,
      visualIndex01,
      logicalIndex01,
      bidirectionalLineIndex01,
    },
    {
      "All left to right text 01.",
      "Hello world\ndemo",
      4u,
      visualIndex02,
      logicalIndex02,
      bidirectionalLineIndex02,
    },
    {
      "bidirectional text 01.",
      "Hello world, \nשלום עולם, \nhello world, \nhello world, שלום עולם\nשלום עולם",
      10u,
      visualIndex03,
      logicalIndex03,
      bidirectionalLineIndex03,
    },
    {
      "bidirectional text 02.",
      "שלום עולם, \nhello world, \nhello world, שלום עולם\nשלום עולם, \nhello world",
      1u,
      visualIndex04,
      logicalIndex04,
      bidirectionalLineIndex04,
    },
  };
  const unsigned int numberOfTests = 4u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    ToolkitTestApplication application;
    if( !GetLogicalCursorIndexTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}
