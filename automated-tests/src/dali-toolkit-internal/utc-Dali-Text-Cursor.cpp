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
#include <dali-toolkit/internal/text/cursor-helper-functions.h>
#include <dali-toolkit/dali-toolkit.h>
#include <toolkit-text-model.h>


using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the following functions.
//
// LineIndex GetClosestLine( VisualModelPtr visualModel,
//                           float visualY )
// CharacterIndex GetClosestCursorIndex( VisualModelPtr visualModel,
//                                       LogicalModelPtr logicalModel,
//                                       MetricsPtr metrics,
//                                       float visualX,
//                                       float visualY )

//////////////////////////////////////////////////////////

namespace
{

struct GetClosestLineData
{
  std::string    description;                    ///< Description of the test.
  std::string    text;                           ///< Input text.
  unsigned int   numberOfTests;                  ///< The number of tests.
  float*         visualY;                        ///< The visual 'y' position for each test.
  LineIndex*     lineIndex;                      ///< The expected line index for each test.
};

struct GetClosestCursorIndexData
{
  std::string     description;                    ///< Description of the test.
  std::string     text;                           ///< Input text.
  unsigned int    numberOfTests;                  ///< The number of tests.
  float*          visualX;                        ///< The visual 'x' position for each test.
  float*          visualY;                        ///< The visual 'y' position for each test.
  CharacterIndex* logicalIndex;                   ///< The expected logical cursor index for each test.
};

bool GetClosestLineTest( const GetClosestLineData& data )
{
  std::cout << "  testing : " << data.description << std::endl;

  // 1) Create the model.
  LogicalModelPtr logicalModel;
  VisualModelPtr visualModel;
  MetricsPtr metrics;
  Size textArea(400.f, 600.f);
  Size layoutSize;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  LayoutOptions options;
  CreateTextModel( data.text,
                   textArea,
                   fontDescriptionRuns,
                   options,
                   layoutSize,
                   logicalModel,
                   visualModel,
                   metrics );

  for( unsigned int index = 0u; index < data.numberOfTests; ++index )
  {
    const LineIndex lineIndex = GetClosestLine( visualModel,
                                                data.visualY[index] );

    if( lineIndex != data.lineIndex[index] )
    {
      std::cout << "  test " << index << " failed. Different line index : " << lineIndex << ", expected : " << data.lineIndex[index] << std::endl;
      return false;
    }
  }

  return true;
}

bool GetClosestCursorIndexTest( const GetClosestCursorIndexData& data )
{
  std::cout << "  testing : " << data.description << std::endl;

  // 1) Create the model.
  LogicalModelPtr logicalModel;
  VisualModelPtr visualModel;
  MetricsPtr metrics;
  Size textArea(400.f, 600.f);
  Size layoutSize;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  LayoutOptions options;
  CreateTextModel( data.text,
                   textArea,
                   fontDescriptionRuns,
                   options,
                   layoutSize,
                   logicalModel,
                   visualModel,
                   metrics );

  for( unsigned int index = 0u; index < data.numberOfTests; ++index )
  {
    const CharacterIndex logicalCursorIndex = GetClosestCursorIndex( visualModel,
                                                                     logicalModel,
                                                                     metrics,
                                                                     data.visualX[index],
                                                                     data.visualY[index] );

    if( logicalCursorIndex != data.logicalIndex[index] )
    {
      std::cout << "  test " << index << " failed. Different logical cursor index : " << logicalCursorIndex << ", expected : " << data.logicalIndex[index] << std::endl;
      return false;
    }
  }

  return true;
}

} // namespace

//////////////////////////////////////////////////////////
//
// UtcDaliGetClosestLine
// UtcDaliGetClosestCursorIndex
//
//////////////////////////////////////////////////////////

int UtcDaliGetClosestLine(void)
{
  tet_infoline(" UtcDaliGetClosestLine");

  float visualY01[] = { -4.f, 3.f, 1000.f };
  LineIndex lineIndices01[] = { 0u, 0u, 0u };

  float visualY02[] = { -4.f, 3.f, 1000.f };
  LineIndex lineIndices02[] = { 0u, 0u, 0u };

  float visualY03[] = { -4.f, 11.f, 30.f, 51.f, 68.f, 87.f, 109.f, 130.f };
  LineIndex lineIndices03[] = { 0u, 0u, 1u, 2u, 3u, 4u, 5u, 5u };

  struct GetClosestLineData data[] =
  {
    {
      "void text.",
      "",
      3u,
      visualY01,
      lineIndices01,
    },
    {
      "Single line text.",
      "hello world",
      3u,
      visualY02,
      lineIndices02,
    },
    {
      "Multi-line text.",
      "abcשנבdefגקכghiעיןjklחלךmnoצמםpqrפרףstuדאוvwxה"
      "סתyzטזץabcשנבdefגקכghiעיןjklחלךmnoצמםpqrפרףstuד"
      "אוvwxהסתyzטזץabcשנבdefגקכghiעיןjklחלךmnoצמםpqr"
      "פרףstuדאוvwxהסתyzטזץabcשנבdefגקכghiעיןjklחלךmno"
      "צמםpqrפרףstuדאוvwxהסתyzטזץabcשנבdefגקכghiעיןjkl"
      "חלךmnoצמםpqrפרףstuדאוvwxהסתyzטזץ",
      8u,
      visualY03,
      lineIndices03
    }
  };
  const unsigned int numberOfTests = 3u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    ToolkitTestApplication application;
    if( !GetClosestLineTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliGetClosestCursorIndex(void)
{
  tet_infoline(" UtcDaliGetClosestCursorIndex");
  float visualX01[] = { -100.f };
  float visualY01[] = { -100.f };
  CharacterIndex logicalIndex01[] = { 0u };

  float visualX02[] = { -100.f, 1000.f, 60.f, 79.f, 83.f, 148.f, 99.f };
  float visualY02[] = { -100.f, 1000.f, 12.f, 12.f, 12.f, 12.f, 12.f };
  CharacterIndex logicalIndex02[] = { 0u, 21u, 7u, 10u, 11u, 13u, 20u };

  float visualX03[] = { 19.f, 104.f, -2.f, 127.f };
  float visualY03[] = { 12.f, 12.f, 12.f, 12.f };
  CharacterIndex logicalIndex03[] = { 3u, 12u, 0u, 18u };

  //  0     5 _ 6     11  12
  //   Hello     world  \n
  // 12    16 _ 17    21   22
  //   שלום       עולם  \n
  // 22         31_32      40  41
  //   different     الأربعاء  \n
  float visualX04[] = { -100.f, 40.f, 44.f, 85.f, 500.f,
                         500.f, 367.f, 359.f, 329.f, -100.f,
                        -100.f, 19.f, 64.f, 72.f, 104.f, 111.f, 500.f};
  float visualY04[] = { -100.f, 12.f, 12.f, 12.f, 12.f,
                          30.f, 30.f, 30.f, 30.f, 30.f,
                          50.f, 50.f, 50.f, 50.f, 50.f, 50.f, 50.f };
  CharacterIndex logicalIndex04[] = {  0u,  5u,  6u, 11u, 11u,
                                       12u, 16u, 17u, 21u, 21u,
                                       22u, 25u, 31u, 32u, 34u, 40u, 40u,
                                       41u };

  //   0           10           20            30           40      46
  //    abcשנבdefג   קכghiעיןjk   lחלךmnoצמם   pqrפרףstuד   אוvwxה
  //  46     50            60            70           80               93
  //    סתyz   טזץabcשנבd    efגקכghiעי    ןjklחלךmno   צמםpqrפרףstuד
  //  93       100           110          120         130          139
  //    אוvwxהס   תyzטזץabcש   נבdefגקכgh   iעיןjklחלך   mnoצמםpqr
  // 139           150           160           170          180       186
  //    פרףstuדאוvw   xהסתyzטזץa   bcשנבdefגק    כghiעיןjkl    חלךmno
  // 186     190           200           210          220            233
  //    צמםp   qrפרףstuדא    וvwxהסתyzט   זץabcשנבde   fגקכghiעיןjkl
  // 233        240            250           260     265
  //    חלךmnoצ    מםpqrפרףst   uדאוvwxהסת    yzטזץ

  float visualX05[] = { -100.f, 96.f, 155.f, 250.f, 344.f, 500.f,
                        -100.f, 36.f, 124.f, 190.f, 280.f, 500.f,
                        -100.f, 56.f, 158.f, 237.f, 303.f, 500.f,
                        -100.f, 98.f, 184.f, 261.f, 337.f, 500.f,
                        -100.f, 40.f, 113.f, 223.f, 302.f, 500.f,
                        -100.f, 82.f, 160.f, 253.f, 500.f };
  float visualY05[] = { -100.f, 12.f, 12.f, 12.f, 12.f, 12.f,
                        30.f, 30.f, 30.f, 30.f, 30.f, 30.f,
                        50.f, 50.f, 50.f, 50.f, 50.f, 50.f,
                        67.f, 67.f, 67.f, 67.f, 67.f, 67.f,
                        87.f, 87.f, 87.f, 87.f, 87.f, 87.f,
                        107.f, 107.f, 107.f, 107.f, 107.f };
  CharacterIndex logicalIndex05[] = {   0u,  10u,  20u,  30u,  40u,  45u,
                                       46u,  50u,  60u,  70u,  80u,  92u,
                                       93u, 100u, 110u, 120u, 130u, 138u,
                                      139u, 150u, 160u, 170u, 180u, 185u,
                                      186u, 190u, 200u, 210u, 220u, 232u,
                                      233u, 240u, 250u, 260u, 265u };

  //   0            10           20           30           40        46
  //    שנבabcגקכd    efעיןghiחל   ךjklצמםmno   פרףpqrדאוs   tuהסתv
  //  46     50           60          70            80              93
  //    wxטז   ץyzשנבabcג   קכdefעיןgh   iחלךjklצמם   mnoפרףpqrדאוs
  //  93        100          110          120           130           139
  //    tuהסתvw   xטזץyzשנבa   bcגקכdefעי    ןghiחלךjkl    צמםmnoפרף
  // 139           150           160          170         180       186
  //    pqrדאוstuהס   תvwxטזץyzש   נבabcגקכde   fעיןghiחלך   jklצמם
  // 186    190          200           210           220            232
  //    mnoפ   רףpqrדאוst   uהסתvwxטזץ   yzשנבabcגק    כdefעיןghiחל
  // 232         240           250           260     265
  //    ךjklצמםm   noפרףpqrדא    וstuהסתvwx   טזץyz

  float visualX06[] = { 500.f, 307.f, 237.f, 148.f, 55.f, -100.f,
                        500.f, 362.f, 276.f, 213.f, 121.f, -100.f,
                        500.f, 344.f, 238.f, 167.f, 93.f, -100.f,
                        500.f, 306.f, 216.f, 142.f, 58.f, -100.f,
                        500.f, 355.f, 279.f, 182.f, 92.f, -100.f,
                        500.f, 326.f, 238.f, 150.f, -100.f };
  float visualY06[] = { -100.f, 12.f, 12.f, 12.f, 12.f, 12.f,
                        30.f, 30.f, 30.f, 30.f, 30.f, 30.f,
                        50.f, 50.f, 50.f, 50.f, 50.f, 50.f,
                        67.f, 67.f, 67.f, 67.f, 67.f, 67.f,
                        87.f, 87.f, 87.f, 87.f, 87.f, 87.f,
                        107.f, 107.f, 107.f, 107.f, 107.f };
  CharacterIndex logicalIndex06[] = {   0u,  10u,  20u,  30u,  40u,  45u,
                                       46u,  50u,  60u,  70u,  80u,  92u,
                                       93u, 100u, 110u, 120u, 130u, 138u,
                                      139u, 150u, 160u, 170u, 180u, 185u,
                                      186u, 190u, 200u, 210u, 220u, 231u,
                                      232u, 240u, 250u, 260u, 265u  };

  struct GetClosestCursorIndexData data[] =
  {
    {
      "Void text.",
      "",
      1u,
      visualX01,
      visualY01,
      logicalIndex01
    },
    {
      "Single line text.",
      "Hello world שלום עולם",
      7u,
      visualX02,
      visualY02,
      logicalIndex02
    },
    {
      "Single line with ligatures",
      "different الأربعاء",
      4u,
      visualX03,
      visualY03,
      logicalIndex03
    },
    {
      "Multiline. Single line paragraphs",
      "Hello world\n"
      "שלום עולם\n"
      "different الأربعاء\n",
      17u,
      visualX04,
      visualY04,
      logicalIndex04
    },
    {
      "Multiline. Single bidirectional paragraph, starts LTR, wrapped lines",
      "abcשנבdefגקכghiעיןjklחלךmnoצמםpqrפרףstuדאוvwxה"
      "סתyzטזץabcשנבdefגקכghiעיןjklחלךmnoצמםpqrפרףstuד"
      "אוvwxהסתyzטזץabcשנבdefגקכghiעיןjklחלךmnoצמםpqr"
      "פרףstuדאוvwxהסתyzטזץabcשנבdefגקכghiעיןjklחלךmno"
      "צמםpqrפרףstuדאוvwxהסתyzטזץabcשנבdefגקכghiעיןjkl"
      "חלךmnoצמםpqrפרףstuדאוvwxהסתyzטזץ",
      35u,
      visualX05,
      visualY05,
      logicalIndex05
    },
    {
      "Multiline. Single bidirectional paragraph, starts RTL, wrapped lines",
      "שנבabcגקכdefעיןghiחלךjklצמםmnoפרףpqrדאוstuהסתv"
      "wxטזץyzשנבabcגקכdefעיןghiחלךjklצמםmnoפרףpqrדאוs"
      "tuהסתvwxטזץyzשנבabcגקכdefעיןghiחלךjklצמםmnoפרף"
      "pqrדאוstuהסתvwxטזץyzשנבabcגקכdefעיןghiחלךjklצמם"
      "mnoפרףpqrדאוstuהסתvwxטזץyzשנבabcגקכdefעיןghiחל"
      "ךjklצמםmnoפרףpqrדאוstuהסתvwxטזץyz",
      35u,
      visualX06,
      visualY06,
      logicalIndex06
    }
  };
  const unsigned int numberOfTests = 6u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    ToolkitTestApplication application;
    if( !GetClosestCursorIndexTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}
