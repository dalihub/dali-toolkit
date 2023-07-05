/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/internal/text/cursor-helper-functions.h>
#include <toolkit-text-utils.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the following functions.
//
// LineIndex GetClosestLine( VisualModelPtr visualModel,
//                           float visualY,
//                           bool& isLineHit )
// CharacterIndex GetClosestCursorIndex( VisualModelPtr visualModel,
//                                       LogicalModelPtr logicalModel,
//                                       MetricsPtr metrics,
//                                       float visualX,
//                                       float visualY,
//                                       CharacterHitTest::Mode mode,
//                                       bool& isCharacterHit )

//////////////////////////////////////////////////////////

namespace
{
const std::string  DEFAULT_FONT_DIR("/resources/fonts");
const unsigned int DEFAULT_FONT_SIZE = 1152u;

struct GetClosestLineData
{
  std::string  description;   ///< Description of the test.
  std::string  text;          ///< Input text.
  unsigned int numberOfTests; ///< The number of tests.
  float*       visualY;       ///< The visual 'y' position for each test.
  LineIndex*   lineIndex;     ///< The expected line index for each test.
  bool*        isLineHit;     ///< The expected line hit value for each test.
};

struct GetClosestCursorIndexData
{
  std::string             description;            ///< Description of the test.
  std::string             text;                   ///< Input text.
  unsigned int            numberOfTests;          ///< The number of tests.
  float*                  visualX;                ///< The visual 'x' position for each test.
  float*                  visualY;                ///< The visual 'y' position for each test.
  CharacterHitTest::Mode* mode;                   ///< The type of hit test.
  bool                    markupProcessorEnabled; //< Enable markup processor to use markup text.
  CharacterIndex*         logicalIndex;           ///< The expected logical cursor index for each test.
  bool*                   isCharacterHit;         ///< The expected character hit value for each test.
};

struct GetCursorPositionData
{
  std::string     description;   ///< Description of the test.
  std::string     text;          ///< Input text.
  unsigned int    numberOfTests; ///< The number of tests.
  CharacterIndex* logicalIndex;  ///< The logical cursor index for each test.
  float*          visualX;       ///< The expected visual 'x' position for each test.
  float*          visualY;       ///< The expected visual 'y' position for each test.
};

struct FindSelectionIndicesData
{
  std::string     description;    ///< Description of the test.
  std::string     text;           ///< Input text.
  unsigned int    numberOfTests;  ///< The number of tests.
  float*          visualX;        ///< The visual 'x' position for each test.
  float*          visualY;        ///< The visual 'y' position for each test.
  bool*           found;          ///< Whether selection indices are found.
  CharacterIndex* startIndex;     ///< The expected start cursor index for each test.
  CharacterIndex* endIndex;       ///< The expected end cursor index for each test.
  CharacterIndex* noTextHitIndex; ///< The expected character index when there is no hit.
};

struct PrimaryCursorHeightData
{
  std::string     description;   ///< Description of the test.
  std::string     text;          ///< Input text.
  unsigned int    numberOfTests; ///< The number of tests.
  CharacterIndex* logicalIndex;  ///< The logical cursor index for each test.
  float*          heights;       ///< The expected primary cursor height for each test.
};

bool GetClosestLineTest(const GetClosestLineData& data)
{
  std::cout << "  testing : " << data.description << std::endl;

  // 1) Create the model.
  ModelPtr   textModel;
  MetricsPtr metrics;
  Size       textArea(400.f, 600.f);
  Size       layoutSize;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  LayoutOptions              options;
  CreateTextModel(data.text,
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
                  0.0f, // lineSpacing
                  0.0f  // characterSpacing
  );

  LogicalModelPtr logicalModel = textModel->mLogicalModel;
  VisualModelPtr  visualModel  = textModel->mVisualModel;

  for(unsigned int index = 0; index < data.numberOfTests; ++index)
  {
    bool            isLineHit = false;
    const LineIndex lineIndex = GetClosestLine(visualModel,
                                               data.visualY[index],
                                               isLineHit);

    if(lineIndex != data.lineIndex[index])
    {
      std::cout << "  test " << index << " failed. Different line index : " << lineIndex << ", expected : " << data.lineIndex[index] << std::endl;
      return false;
    }
    if(isLineHit != data.isLineHit[index])
    {
      std::cout << "  test " << index << " failed. Different line hit value : " << isLineHit << ", expected : " << data.isLineHit[index] << std::endl;
      return false;
    }
  }

  return true;
}

bool GetClosestCursorIndexTest(const GetClosestCursorIndexData& data)
{
  std::cout << "  testing : " << data.description << std::endl;

  // 1) Create the model.
  ModelPtr   textModel;
  MetricsPtr metrics;
  Size       textArea(400.f, 600.f);
  Size       layoutSize;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  LayoutOptions              options;
  CreateTextModel(data.text,
                  textArea,
                  fontDescriptionRuns,
                  options,
                  layoutSize,
                  textModel,
                  metrics,
                  data.markupProcessorEnabled,
                  LineWrap::WORD,
                  false,
                  Toolkit::DevelText::EllipsisPosition::END,
                  0.0f, // lineSpacing
                  0.0f  // characterSpacing
  );

  LogicalModelPtr logicalModel = textModel->mLogicalModel;
  VisualModelPtr  visualModel  = textModel->mVisualModel;

  for(unsigned int index = 0; index < data.numberOfTests; ++index)
  {
    bool                 isCharacterHit     = false;
    const CharacterIndex logicalCursorIndex = GetClosestCursorIndex(visualModel,
                                                                    logicalModel,
                                                                    metrics,
                                                                    data.visualX[index],
                                                                    data.visualY[index],
                                                                    data.mode[index],
                                                                    isCharacterHit);
    if(logicalCursorIndex != data.logicalIndex[index])
    {
      std::cout << "  test " << index << " failed. Different logical cursor index : " << logicalCursorIndex << ", expected : " << data.logicalIndex[index] << std::endl;
      return false;
    }
    if(isCharacterHit != data.isCharacterHit[index])
    {
      std::cout << "  test " << index << " failed. Different character hit value : " << isCharacterHit << ", expected : " << data.isCharacterHit[index] << std::endl;
      return false;
    }
  }

  return true;
}

bool GetCursorPositionTest(const GetCursorPositionData& data)
{
  std::cout << "  testing : " << data.description << std::endl;

  // 1) Create the model.
  ModelPtr   textModel;
  MetricsPtr metrics;
  Size       textArea(400.f, 600.f);
  Size       layoutSize;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  LayoutOptions              options;
  CreateTextModel(data.text,
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
                  0.0f, // lineSpacing
                  0.0f  // characterSpacing
  );

  LogicalModelPtr logicalModel = textModel->mLogicalModel;
  VisualModelPtr  visualModel  = textModel->mVisualModel;

  GetCursorPositionParameters parameters;
  parameters.visualModel  = visualModel;
  parameters.logicalModel = logicalModel;
  parameters.metrics      = metrics;
  parameters.isMultiline  = true;

  for(unsigned int index = 0; index < data.numberOfTests; ++index)
  {
    CursorInfo cursorInfo;
    parameters.logical = data.logicalIndex[index];

    GetCursorPosition(parameters,
                      0.f, // Since this test case is not testing the primary cursor height, the default font line height can be set to 0.f.
                      cursorInfo);

    if(floor(cursorInfo.primaryPosition.x) != data.visualX[index])
    {
      std::cout << "  test " << index << " failed. Different 'x' cursor position : " << cursorInfo.primaryPosition.x << ", expected : " << data.visualX[index] << std::endl;
      return false;
    }
    if(floor(cursorInfo.primaryPosition.y) != data.visualY[index])
    {
      std::cout << "  test " << index << " failed. Different 'y' cursor position : " << cursorInfo.primaryPosition.y << ", expected : " << data.visualY[index] << std::endl;
      return false;
    }
  }

  return true;
}

bool FindSelectionIndicesTest(const FindSelectionIndicesData& data)
{
  std::cout << "  testing : " << data.description << std::endl;

  // 1) Create the model.
  ModelPtr   textModel;
  MetricsPtr metrics;
  Size       textArea(400.f, 600.f);
  Size       layoutSize;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  LayoutOptions              options;
  CreateTextModel(data.text,
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
                  0.0f, // lineSpacing
                  0.0f  // characterSpacing
  );

  LogicalModelPtr logicalModel = textModel->mLogicalModel;
  VisualModelPtr  visualModel  = textModel->mVisualModel;

  for(unsigned int index = 0; index < data.numberOfTests; ++index)
  {
    CharacterIndex startIndex     = 0;
    CharacterIndex endIndex       = 0;
    CharacterIndex noTextHitIndex = 0;
    const bool     found          = FindSelectionIndices(visualModel,
                                            logicalModel,
                                            metrics,
                                            data.visualX[index],
                                            data.visualY[index],
                                            startIndex,
                                            endIndex,
                                            noTextHitIndex);

    if(found != data.found[index])
    {
      std::cout << "  test " << index << " failed. Different found value : " << found << ", expected : " << data.found[index] << std::endl;
      return false;
    }
    if(startIndex != data.startIndex[index])
    {
      std::cout << "  test " << index << " failed. Different start index : " << startIndex << ", expected : " << data.startIndex[index] << std::endl;
      return false;
    }
    if(endIndex != data.endIndex[index])
    {
      std::cout << "  test " << index << " failed. Different end index : " << endIndex << ", expected : " << data.endIndex[index] << std::endl;
      return false;
    }
    if(noTextHitIndex != data.noTextHitIndex[index])
    {
      std::cout << "  test " << index << " failed. Different no text hit index : " << noTextHitIndex << ", expected : " << data.noTextHitIndex[index] << std::endl;
      return false;
    }
  }
  return true;
}

bool PrimaryCursorHeightTest(const PrimaryCursorHeightData& data)
{
  std::cout << "  testing : " << data.description << std::endl;

  // 1) Create the model.
  ModelPtr   textModel;
  MetricsPtr metrics;
  Size       textArea(400.f, 600.f);
  Size       layoutSize;

  Vector<FontDescriptionRun> fontDescriptionRuns;

  const std::string fontFamily("DejaVuSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun1;
  fontDescriptionRun1.characterRun.characterIndex     = 0u;
  fontDescriptionRun1.characterRun.numberOfCharacters = 13u;
  fontDescriptionRun1.familyLength                    = fontFamily.size();
  fontDescriptionRun1.familyName                      = new char[fontDescriptionRun1.familyLength];
  memcpy(fontDescriptionRun1.familyName, fontFamily.c_str(), fontDescriptionRun1.familyLength);
  fontDescriptionRun1.familyDefined = true;
  fontDescriptionRun1.weightDefined = false;
  fontDescriptionRun1.widthDefined  = false;
  fontDescriptionRun1.slantDefined  = false;
  fontDescriptionRun1.sizeDefined   = true;
  fontDescriptionRun1.size          = 768u; //Font size = 12.0f (768/64 = 12)

  fontDescriptionRuns.PushBack(fontDescriptionRun1);

  LayoutOptions options;
  CreateTextModel(data.text,
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
                  50.0f, // lineSpacing
                  50.0f  // characterSpacing
  );

  LogicalModelPtr logicalModel = textModel->mLogicalModel;
  VisualModelPtr  visualModel  = textModel->mVisualModel;

  GetCursorPositionParameters parameters;
  parameters.visualModel  = visualModel;
  parameters.logicalModel = logicalModel;
  parameters.metrics      = metrics;
  parameters.isMultiline  = true;

  for(unsigned int index = 0; index < data.numberOfTests; ++index)
  {
    CursorInfo cursorInfo;
    parameters.logical = data.logicalIndex[index];

    // Load some fonts.
    TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
    fontClient.SetDpi(93u, 93u);

    char*             pathNamePtr = get_current_dir_name();
    const std::string pathName(pathNamePtr);
    free(pathNamePtr);

    FontId fontID = fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/dejavu/DejaVuSans.ttf");

    Text::FontMetrics fontMetrics;
    MetricsPtr        mMetrics = Metrics::New(fontClient);
    mMetrics->GetFontMetrics(fontID, fontMetrics);
    float defaultFontLineHeight = (fontMetrics.ascender - fontMetrics.descender);

    GetCursorPosition(parameters,
                      defaultFontLineHeight,
                      cursorInfo);

    if(floor(cursorInfo.primaryCursorHeight) != data.heights[index])
    {
      std::cout << "  test " << index << " failed. Different primaryCursorHeight : " << cursorInfo.primaryCursorHeight << ", expected : " << data.heights[index] << std::endl;
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
// UtcDaliGetCursorPosition
// UtcDaliFindSelectionIndices
// UtcDaliPrimaryCursorHeight
//
//////////////////////////////////////////////////////////

int UtcDaliGetClosestLine(void)
{
  tet_infoline(" UtcDaliGetClosestLine");

  float     visualY01[]     = {-4.f, 3.f, 1000.f};
  LineIndex lineIndices01[] = {0, 0, 0};
  bool      isLineHit01[]   = {false, false, false};

  float     visualY02[]     = {-4.f, 3.f, 1000.f};
  LineIndex lineIndices02[] = {0, 0, 0};
  bool      isLineHit02[]   = {false, true, false};

  float     visualY03[]     = {-4.f, 11.f, 30.f, 51.f, 68.f, 87.f, 109.f, 130.f};
  LineIndex lineIndices03[] = {0, 0, 1u, 2u, 3u, 4u, 5u, 5u};
  bool      isLineHit03[]   = {false, true, true, true, true, true, true, false};

  struct GetClosestLineData data[] =
    {
      {"void text.",
       "",
       3u,
       visualY01,
       lineIndices01,
       isLineHit01},
      {"Single line text.",
       "hello world",
       3u,
       visualY02,
       lineIndices02,
       isLineHit02},
      {"Multi-line text.",
       "abcשנבdefגקכghiעיןjklחלךmnoצמםpqrפרףstuדאוvwxה"
       "סתyzטזץabcשנבdefגקכghiעיןjklחלךmnoצמםpqrפרףstuד"
       "אוvwxהסתyzטזץabcשנבdefגקכghiעיןjklחלךmnoצמםpqr"
       "פרףstuדאוvwxהסתyzטזץabcשנבdefגקכghiעיןjklחלךmno"
       "צמםpqrפרףstuדאוvwxהסתyzטזץabcשנבdefגקכghiעיןjkl"
       "חלךmnoצמםpqrפרףstuדאוvwxהסתyzטזץ",
       8u,
       visualY03,
       lineIndices03,
       isLineHit03}};
  const unsigned int numberOfTests = 3u;

  for(unsigned int index = 0; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!GetClosestLineTest(data[index]))
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

  float                  visualX01[]        = {-100.f};
  float                  visualY01[]        = {-100.f};
  CharacterHitTest::Mode mode01[]           = {CharacterHitTest::TAP};
  CharacterIndex         logicalIndex01[]   = {0};
  bool                   isCharacterHit01[] = {false};

  float                  visualX02[]        = {-100.f, 1000.f, 60.f, 79.f, 83.f, 148.f, 99.f};
  float                  visualY02[]        = {-100.f, 1000.f, 12.f, 12.f, 12.f, 12.f, 12.f};
  CharacterHitTest::Mode mode02[]           = {CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP};
  CharacterIndex         logicalIndex02[]   = {0, 21u, 7u, 10u, 11u, 14u, 20u};
  bool                   isCharacterHit02[] = {false, false, true, true, true, true, true};

  float                  visualX03[]        = {19.f, 104.f, -2.f, 127.f};
  float                  visualY03[]        = {12.f, 12.f, 12.f, 12.f};
  CharacterHitTest::Mode mode03[]           = {CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP};
  CharacterIndex         logicalIndex03[]   = {3u, 13u, 0, 18u};
  bool                   isCharacterHit03[] = {true, true, false, false};

  //  0     5 _ 6     11  12
  //   Hello     world  \n
  // 12    16 _ 17    21   22
  //   שלום       עולם  \n
  // 22         31_32      40  41
  //   different     الأربعاء  \n
  float                  visualX04[]        = {-100.f, 40.f, 44.f, 85.f, 500.f, 500.f, 367.f, 359.f, 329.f, -100.f, -100.f, 19.f, 64.f, 72.f, 104.f, 111.f, 500.f};
  float                  visualY04[]        = {-100.f, 12.f, 12.f, 12.f, 12.f, 30.f, 30.f, 30.f, 30.f, 30.f, 50.f, 50.f, 50.f, 50.f, 50.f, 50.f, 50.f};
  CharacterHitTest::Mode mode04[]           = {CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP};
  CharacterIndex         logicalIndex04[]   = {0, 5u, 6u, 11u, 11u, 12u, 16u, 17u, 21u, 21u, 22u, 25u, 31u, 32u, 35u, 34u, 40u, 41u};
  bool                   isCharacterHit04[] = {false, true, true, true, false, false, true, true, true, false, false, true, true, true, true, true, false};

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

  float                  visualX05[]        = {-100.f, 96.f, 155.f, 250.f, 344.f, 500.f, -100.f, 36.f, 124.f, 190.f, 280.f, 500.f, -100.f, 56.f, 158.f, 237.f, 303.f, 500.f, -100.f, 98.f, 184.f, 261.f, 337.f, 500.f, -100.f, 40.f, 113.f, 223.f, 302.f, 500.f, -100.f, 82.f, 160.f, 253.f, 500.f};
  float                  visualY05[]        = {-100.f, 12.f, 12.f, 12.f, 12.f, 12.f, 30.f, 30.f, 30.f, 30.f, 30.f, 30.f, 50.f, 50.f, 50.f, 50.f, 50.f, 50.f, 67.f, 67.f, 67.f, 67.f, 67.f, 67.f, 87.f, 87.f, 87.f, 87.f, 87.f, 87.f, 107.f, 107.f, 107.f, 107.f, 107.f};
  CharacterHitTest::Mode mode05[]           = {CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP};
  CharacterIndex         logicalIndex05[]   = {0, 10u, 19u, 28u, 41u, 44u, 45u, 49u, 59u, 66u, 78u, 89u, 90u, 97u, 107u, 117u, 126u, 134u, 135u, 147u, 155u, 163u, 172u, 180u, 181u, 185u, 192u, 204u, 213u, 222u, 223u, 234u, 242u, 252u, 265u};
  bool                   isCharacterHit05[] = {false, true, true, true, true, false, false, true, true, true, true, false, false, true, true, true, true, false, false, true, true, true, true, false, false, true, true, true, true, false, false, true, true, true, false};

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

  float                  visualX06[]        = {500.f, 307.f, 237.f, 148.f, 55.f, -100.f, 500.f, 362.f, 276.f, 213.f, 121.f, -100.f, 500.f, 344.f, 238.f, 167.f, 93.f, -100.f, 500.f, 306.f, 216.f, 142.f, 58.f, -100.f, 500.f, 355.f, 279.f, 182.f, 92.f, -100.f, 500.f, 326.f, 238.f, 150.f, -100.f};
  float                  visualY06[]        = {-100.f, 12.f, 12.f, 12.f, 12.f, 12.f, 30.f, 30.f, 30.f, 30.f, 30.f, 30.f, 50.f, 50.f, 50.f, 50.f, 50.f, 50.f, 67.f, 67.f, 67.f, 67.f, 67.f, 67.f, 87.f, 87.f, 87.f, 87.f, 87.f, 87.f, 107.f, 107.f, 107.f, 107.f, 107.f};
  CharacterHitTest::Mode mode06[]           = {CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP, CharacterHitTest::TAP};
  CharacterIndex         logicalIndex06[]   = {0, 10u, 19u, 28u, 39u, 44u, 45u, 49u, 59u, 66u, 78u, 89u, 90u, 97u, 108u, 117u, 126u, 134u, 135u, 145u, 155u, 163u, 174u, 180u, 181u, 186u, 194u, 204u, 214u, 222u, 224u, 235u, 243u, 252u, 265u};
  bool                   isCharacterHit06[] = {false, true, true, true, true, false, false, true, true, true, true, false, false, true, true, true, true, false, false, true, true, true, true, false, false, true, true, true, true, false, false, true, true, true, false};

  float                  visualX07[]        = {395.f};
  float                  visualY07[]        = {12.f};
  CharacterHitTest::Mode mode07[]           = {CharacterHitTest::TAP};
  CharacterIndex         logicalIndex07[]   = {1u};
  bool                   isCharacterHit07[] = {true};

  float                  visualX08[]        = {7.f};
  float                  visualY08[]        = {12.f};
  CharacterHitTest::Mode mode08[]           = {CharacterHitTest::TAP};
  CharacterIndex         logicalIndex08[]   = {1u};
  bool                   isCharacterHit08[] = {true};

  float                  visualX09[]        = {9.f};
  float                  visualY09[]        = {12.f};
  CharacterHitTest::Mode mode09[]           = {CharacterHitTest::TAP};
  CharacterIndex         logicalIndex09[]   = {1u};
  bool                   isCharacterHit09[] = {true};

  struct GetClosestCursorIndexData data[] =
    {
      {"Void text.",
       "",
       1u,
       visualX01,
       visualY01,
       mode01,
       false,
       logicalIndex01,
       isCharacterHit01},

      {"Single line text.",
       "Hello world שלום עולם",
       7u,
       visualX02,
       visualY02,
       mode02,
       false,
       logicalIndex02,
       isCharacterHit02},

      {"Single line with ligatures",
       "different الأربعاء",
       4u,
       visualX03,
       visualY03,
       mode03,
       false,
       logicalIndex03,
       isCharacterHit03},

      {"Multiline. Single line paragraphs",
       "Hello world\n"
       "שלום עולם\n"
       "different الأربعاء\n",
       17u,
       visualX04,
       visualY04,
       mode04,
       false,
       logicalIndex04,
       isCharacterHit04},

      {"Multiline. Single bidirectional paragraph, starts LTR, wrapped lines",
       "abcשנבdefגקכghiעיןjklחלךmnoצמםpqrפרףstuדאוvwxה"
       "סתyzטזץabcשנבdefגקכghiעיןjklחלךmnoצמםpqrפרףstuד"
       "אוvwxהסתyzטזץabcשנבdefגקכghiעיןjklחלךmnoצמםpqr"
       "פרףstuדאוvwxהסתyzטזץabcשנבdefגקכghiעיןjklחלךmno"
       "צמםpqrפרףstuדאוvwxהסתyzטזץabcשנבdefגקכghiעיןjkl"
       "חלךmnoצמםpqrפרףstuדאוvwxהסתyzטזץ",
       35u,
       visualX05,
       visualY05,
       mode05,
       false,
       logicalIndex05,
       isCharacterHit05},

      {"Multiline. Single bidirectional paragraph, starts RTL, wrapped lines",
       "שנבabcגקכdefעיןghiחלךjklצמםmnoפרףpqrדאוstuהסתv"
       "wxטזץyzשנבabcגקכdefעיןghiחלךjklצמםmnoפרףpqrדאוs"
       "tuהסתvwxטזץyzשנבabcגקכdefעיןghiחלךjklצמםmnoפרף"
       "pqrדאוstuהסתvwxטזץyzשנבabcגקכdefעיןghiחלךjklצמם"
       "mnoפרףpqrדאוstuהסתvwxטזץyzשנבabcגקכdefעיןghiחל"
       "ךjklצמםmnoפרףpqrדאוstuהסתvwxטזץyz",
       35u,
       visualX06,
       visualY06,
       mode06,
       false,
       logicalIndex06,
       isCharacterHit06},

      {"Testing complex characters. Arabic ligatures",
       "الأَبْجَدِيَّة العَرَبِيَّة",
       1u,
       visualX07,
       visualY07,
       mode07,
       false,
       logicalIndex07,
       isCharacterHit07},

      {"Testing complex characters. Latin ligatures",
       "fi ligature",
       1u,
       visualX08,
       visualY08,
       mode08,
       false,
       logicalIndex08,
       isCharacterHit08},

      {"Testing complex characters. Emoji",
       "A&#x1F468;&#x200D;&#x1F469;&#x200D;&#x1F467;&#x200D;&#x1F466;B",
       1u,
       visualX09,
       visualY09,
       mode09,
       true,
       logicalIndex09,
       isCharacterHit09}

    };
  const unsigned int numberOfTests = 9u;

  for(unsigned int index = 0; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!GetClosestCursorIndexTest(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliGetCursorPosition(void)
{
  tet_infoline(" UtcDaliGetCursorPosition");

  float          visualX08[]      = {5.f};
  float          visualY08[]      = {0.f};
  CharacterIndex logicalIndex08[] = {1u};

  struct GetCursorPositionData data[] =
    {
      {
        "Testing complex characters. Latin ligatures",
        "fi ligature",
        1u,
        logicalIndex08,
        visualX08,
        visualY08,
      }};
  const unsigned int numberOfTests = 1u;

  for(unsigned int index = 0; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!GetCursorPositionTest(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliFindSelectionIndices(void)
{
  tet_infoline(" UtcDaliFindSelectionIndices");

  float          visualX01[]    = {-100.f};
  float          visualY01[]    = {-100.f};
  bool           found01[]      = {false};
  CharacterIndex startIndex01[] = {0};
  CharacterIndex endIndex01[]   = {0};
  CharacterIndex noHitText01[]  = {0};

  float          visualX02[]    = {-100.f, 1000.f, 1000.f};
  float          visualY02[]    = {-100.f, 12.f, 1000.f};
  bool           found02[]      = {false, false, false};
  CharacterIndex startIndex02[] = {0, 6u, 6u};
  CharacterIndex endIndex02[]   = {5u, 11u, 11u};
  CharacterIndex noHitText02[]  = {0, 11u, 11u};

  float          visualX03[]    = {70.f};
  float          visualY03[]    = {12.f};
  bool           found03[]      = {true};
  CharacterIndex startIndex03[] = {6u};
  CharacterIndex endIndex03[]   = {11u};
  CharacterIndex noHitText03[]  = {0u};

  float          visualX04[]    = {131.f};
  float          visualY04[]    = {12.f};
  bool           found04[]      = {true};
  CharacterIndex startIndex04[] = {12u};
  CharacterIndex endIndex04[]   = {16u};
  CharacterIndex noHitText04[]  = {0u};

  float          visualX05[]    = {0.f};
  float          visualY05[]    = {12.f};
  bool           found05[]      = {true};
  CharacterIndex startIndex05[] = {0};
  CharacterIndex endIndex05[]   = {1u};
  CharacterIndex noHitText05[]  = {0};

  float          visualX06[]    = {10.f};
  float          visualY06[]    = {12.f};
  bool           found06[]      = {true};
  CharacterIndex startIndex06[] = {0};
  CharacterIndex endIndex06[]   = {1u};
  CharacterIndex noHitText06[]  = {0u};

  struct FindSelectionIndicesData data[] =
    {
      {"void text",
       "",
       1u,
       visualX01,
       visualY01,
       found01,
       startIndex01,
       endIndex01,
       noHitText01},
      {"touch out of text's boundaries",
       "Hello world",
       3u,
       visualX02,
       visualY02,
       found02,
       startIndex02,
       endIndex02,
       noHitText02},
      {"touch on the text",
       "Hello world demo",
       1u,
       visualX03,
       visualY03,
       found03,
       startIndex03,
       endIndex03,
       noHitText03},
      {"touch on the new paragraph character at the end of line",
       "Hello world demo\n",
       1u,
       visualX04,
       visualY04,
       found04,
       startIndex04,
       endIndex04,
       noHitText04},
      {"touch on a white space character. is the unique character of the line",
       " ",
       1u,
       visualX05,
       visualY05,
       found05,
       startIndex05,
       endIndex05,
       noHitText05},
      {"touch on a white space character. is between two words",
       "h ello",
       1u,
       visualX06,
       visualY06,
       found06,
       startIndex06,
       endIndex06,
       noHitText06},
    };
  const unsigned int numberOfTests = 6u;

  for(unsigned int index = 0; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!FindSelectionIndicesTest(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliPrimaryCursorHeight(void)
{
  tet_infoline(" UtcDaliPrimaryCursorHeight");

  float          heights[]      = {19.f};
  CharacterIndex logicalIndex[] = {1u};

  struct PrimaryCursorHeightData data[] =
    {
      {
        "Testing primary cursor height when line spacing is used.",
        "Hello World",
        1u,
        logicalIndex,
        heights,
      }};
  const unsigned int numberOfTests = 1u;

  for(unsigned int index = 0; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!PrimaryCursorHeightTest(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}
