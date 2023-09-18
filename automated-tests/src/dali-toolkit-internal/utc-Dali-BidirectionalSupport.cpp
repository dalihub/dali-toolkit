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
#include <iostream>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/internal/text/bidirectional-support.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/text-run-container.h>
#include <dali/devel-api/text-abstraction/bidirectional-support.h>
#include <toolkit-text-utils.h>

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
// bool GetMirroredText( const Vector<Character>& text,
//                       Vector<CharacterDirection>& directions,
//                       const Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo,
//                       CharacterIndex startIndex,
//                       Length numberOfCharacters,
//                       Vector<Character>& mirroredText )
// void GetCharactersDirection( const Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo,
//                              Length totalNumberOfCharacters,
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
  std::string  description;            ///< Description of the test.
  std::string  text;                   ///< Input text.
  unsigned int startIndex;             ///< The index from where the model is updated.
  unsigned int numberOfCharacters;     ///< The number of characters.
  bool*        directions;             ///< The expected directions.
  bool         markupProcessorEnabled; ///< Enable markup processor to use markup text.
};

bool SetBidirectionalInfoTest(const SetBidirectionalInfoData& data)
{
  // 1) Create the model.
  ModelPtr   textModel;
  MetricsPtr metrics;
  Size       textArea(100.f, 60.f);
  Size       layoutSize;

  // Create the model.
  const Vector<FontDescriptionRun> fontDescriptions;
  const LayoutOptions              options;
  CreateTextModel(data.text,
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
                  0.0f, // lineSpacing
                  0.0f  // characterSpacing
  );

  LogicalModelPtr logicalModel = textModel->mLogicalModel;
  VisualModelPtr  visualModel  = textModel->mVisualModel;

  // 2) Clear the bidirectional paragraph info data.
  Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo = logicalModel->mBidirectionalParagraphInfo;
  if(0u != data.numberOfCharacters)
  {
    ClearCharacterRuns(data.startIndex,
                       data.startIndex + data.numberOfCharacters - 1u,
                       bidirectionalInfo);
  }

  // 3) Call the SetBidirectionalInfo() function.
  SetBidirectionalInfo(logicalModel->mText,
                       logicalModel->mScriptRuns,
                       logicalModel->mLineBreakInfo,
                       data.startIndex,
                       data.numberOfCharacters,
                       bidirectionalInfo);

  // 4) Compare with the expected results.
  TextAbstraction::BidirectionalSupport bidirectionalSupport = TextAbstraction::BidirectionalSupport::Get();

  if(data.numberOfParagraphs != bidirectionalInfo.Count())
  {
    // Different number of expected bidirectional paragraphs.
    std::cout << "  Different number of bidi paragraphs : " << bidirectionalInfo.Count() << ", expected : " << data.numberOfParagraphs << std::endl;
    return false;
  }

  for(unsigned int index = 0u; index < data.numberOfParagraphs; ++index)
  {
    const BidirectionalParagraphInfoRun& run = bidirectionalInfo[index];

    const CharacterDirection direction = bidirectionalSupport.GetParagraphDirection(run.bidirectionalInfoIndex);
    if(direction != data.directions[index])
    {
      std::cout << "  Different direction" << std::endl;
      std::cout << "        paragraph : " << index << std::endl;
      std::cout << "            index : " << run.characterRun.characterIndex << ", num chars : " << run.characterRun.numberOfCharacters << ", direction : " << direction << std::endl;
      std::cout << "  expected, index : " << data.indices[index] << ", num chars : " << data.numberOfParagraphCharacters[index] << ", direction : " << data.directions[index] << std::endl;
      return false;
    }

    if(run.characterRun.characterIndex != data.indices[index])
    {
      std::cout << "  Different index" << std::endl;
      std::cout << "        paragraph : " << index << std::endl;
      std::cout << "            index : " << run.characterRun.characterIndex << ", num chars : " << run.characterRun.numberOfCharacters << ", direction : " << direction << std::endl;
      std::cout << "  expected, index : " << data.indices[index] << ", num chars : " << data.numberOfParagraphCharacters[index] << ", direction : " << data.directions[index] << std::endl;
      return false;
    }
    if(run.characterRun.numberOfCharacters != data.numberOfParagraphCharacters[index])
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

bool GetMirroredTextTest(const GetMirroredTextData& data)
{
  // 1) Create the model.
  ModelPtr   textModel;
  MetricsPtr metrics;
  Size       textArea(100.f, 60.f);
  Size       layoutSize;

  // Create the model.
  const Vector<FontDescriptionRun> fontDescriptions;
  const LayoutOptions              options;
  CreateTextModel(data.text,
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
                  0.0f, // lineSpacing
                  0.0f  // characterSpacing
  );

  LogicalModelPtr logicalModel = textModel->mLogicalModel;
  VisualModelPtr  visualModel  = textModel->mVisualModel;

  // 2) Call the GetMirroredText() function for the whole text
  Vector<Character> mirroredText;
  bool              mirrored = false;
  mirrored                   = GetMirroredText(logicalModel->mText,
                             logicalModel->mCharacterDirections,
                             logicalModel->mBidirectionalParagraphInfo,
                             0u,
                             logicalModel->mText.Count(),
                             mirroredText);

  // 3) Call the GetMirroredText() function for the given index + number of characters
  mirrored = GetMirroredText(logicalModel->mText,
                             logicalModel->mCharacterDirections,
                             logicalModel->mBidirectionalParagraphInfo,
                             data.startIndex,
                             data.numberOfCharacters,
                             mirroredText);

  // 4) Compare the results.

  // Convert to utf8
  std::string mirroredString;
  Utf32ToUtf8(mirroredText.Begin(),
              mirroredText.Count(),
              mirroredString);

  if(!mirrored && (mirroredString != data.text))
  {
    std::cout << "  No mirrored text and mirroredString != data.text." << std::endl;
    std::cout << "  mirrored string : [" << mirroredString << "]" << std::endl;
    std::cout << "             text : [" << data.text << "]" << std::endl;
    return false;
  }

  if(mirrored && (mirroredString == data.text))
  {
    std::cout << "  Mirrored text and mirroredString == data.text." << std::endl;
    std::cout << "  mirrored string : [" << mirroredString << "]" << std::endl;
    std::cout << "             text : [" << data.text << "]" << std::endl;
    return false;
  }

  if(mirrored && (mirroredString != data.mirroredText))
  {
    std::cout << "  Mirrored text and mirroredString != data.mirroredText." << std::endl;
    std::cout << "  mirrored string : [" << mirroredString << "]" << std::endl;
    std::cout << "             text : [" << data.mirroredText << "]" << std::endl;
    return false;
  }

  return true;
}

bool GetCharactersDirectionTest(const GetCharactersDirectionData& data)
{
  // 1) Create the model.
  ModelPtr   textModel;
  MetricsPtr metrics;
  Size       textArea(100.f, 60.f);
  Size       layoutSize;

  // Create the model.
  const Vector<FontDescriptionRun> fontDescriptions;
  const LayoutOptions              options;
  CreateTextModel(data.text,
                  textArea,
                  fontDescriptions,
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

  Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo = logicalModel->mBidirectionalParagraphInfo;

  // 2) Clear the direction info data.
  Vector<CharacterDirection>& directions = logicalModel->mCharacterDirections;

  if(directions.Count() >= data.startIndex + data.numberOfCharacters)
  {
    directions.Erase(directions.Begin() + data.startIndex,
                     directions.Begin() + data.startIndex + data.numberOfCharacters);
  }

  // 3) Call GetCharactersDirection() function.

  GetCharactersDirection(bidirectionalInfo,
                         logicalModel->mText.Count(),
                         data.startIndex,
                         data.numberOfCharacters,
                         directions);

  for(unsigned int index = 0u; index < logicalModel->mText.Count(); ++index)
  {
    if(data.directions[index] != directions[index])
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
  tet_infoline(" UtcDaliSetBidirectionalInfo");

  unsigned int indices01[]            = {};
  unsigned int numberOfCharacters01[] = {};
  bool         direction01[]          = {};
  unsigned int indices02[]            = {};
  unsigned int numberOfCharacters02[] = {};
  bool         direction02[]          = {};
  unsigned int indices03[]            = {17u, 48u};
  unsigned int numberOfCharacters03[] = {14u, 14u};
  bool         direction03[]          = {true, true};
  unsigned int indices04[]            = {17u, 31u, 79u};
  unsigned int numberOfCharacters04[] = {14u, 48u, 31u};
  bool         direction04[]          = {true, false, true};
  unsigned int indices05[]            = {17u, 41u, 117u};
  unsigned int numberOfCharacters05[] = {24u, 76u, 49u};
  bool         direction05[]          = {true, false, true};
  unsigned int indices06[]            = {17u, 48u};
  unsigned int numberOfCharacters06[] = {14u, 14u};
  bool         direction06[]          = {true, true};
  unsigned int indices07[]            = {17u, 31u, 79u};
  unsigned int numberOfCharacters07[] = {14u, 48u, 31u};
  bool         direction07[]          = {true, false, true};
  unsigned int indices08[]            = {17u, 41u, 117u};
  unsigned int numberOfCharacters08[] = {24u, 76u, 49u};
  bool         direction08[]          = {true, false, true};

  struct SetBidirectionalInfoData data[] =
    {
      {"Zero characters",
       "",
       0u,
       0u,
       0u,
       indices01,
       numberOfCharacters01,
       direction01},

      {"Some left to right paragraphs",
       "Hello world\ndemo\n\n",
       0u,
       18u,
       0u,
       indices02,
       numberOfCharacters02,
       direction02},

      {"A mix of left to right and right to left paragraphs.",
       "Hello world demo\nمرحبا بالعالم\nhello world demo\nمرحبا بالعالم\nhello world demo",
       0u,
       78u,
       2u,
       indices03,
       numberOfCharacters03,
       direction03},

      {"A mix of left to right and right to left paragraphs. Paragraphs also contain a mix of bidirectional text.",
       "Hello world demo\nمرحبا بالعالم\nhello world demo مرحبا بالعالم hello world demo\nمرحبا hello world demo بالعالم\nhello world demo",
       0u,
       126u,
       3u,
       indices04,
       numberOfCharacters04,
       direction04},

      {"A mix of left to right and right to left paragraphs. Paragraphs also contain a mix of bidirectional text and a mix of right to left scripts.",
       "Hello world demo\nمرحبا שלום עולם بالعالم\nhello world שלום بالعالم עולם demo مرحبا שלום עולם بالعالم hello world demo\nمرحبا hello שלום بالعالم עולם world demo بالعالم\nhello world demo",
       0u,
       182u,
       3u,
       indices05,
       numberOfCharacters05,
       direction05},

      {"A mix of left to right and right to left paragraphs. Updates a left to right paragraph.",
       "Hello world demo\nمرحبا بالعالم\nhello world demo\nمرحبا بالعالم\nhello world demo",
       31u,
       17u,
       2u,
       indices06,
       numberOfCharacters06,
       direction06},

      {"A mix of left to right and right to left paragraphs. Paragraphs also contain a mix of bidirectional text.",
       "Hello world demo\nمرحبا بالعالم\nhello world demo مرحبا بالعالم hello world demo\nمرحبا hello world demo بالعالم\nhello world demo",
       0u,
       126u,
       3u,
       indices07,
       numberOfCharacters07,
       direction07},

      {"A mix of left to right and right to left paragraphs. Paragraphs also contain a mix of bidirectional text and a mix of right to left scripts. Updates initial paragraphs.",
       "Hello world demo\nمرحبا שלום עולם بالعالم\nhello world שלום بالعالم עולם demo مرحبا שלום עולם بالعالم hello world demo\nمرحبا hello שלום بالعالم עולם world demo بالعالم\nhello world demo",
       0u,
       41u,
       3u,
       indices08,
       numberOfCharacters08,
       direction08},

      {"A mix of left to right and right to left paragraphs. Paragraphs also contain a mix of bidirectional text and a mix of right to left scripts. Updates mid paragraphs.",
       "Hello world demo\nمرحبا שלום עולם بالعالم\nhello world שלום بالعالم עולם demo مرحبا שלום עולם بالعالم hello world demo\nمرحبا hello שלום بالعالم עולם world demo بالعالم\nhello world demo",
       41u,
       76u,
       3u,
       indices08,
       numberOfCharacters08,
       direction08},

      {"A mix of left to right and right to left paragraphs. Paragraphs also contain a mix of bidirectional text and a mix of right to left scripts. Updates from character 85",
       "Hello world demo\nمرحبا שלום עולם بالعالم\nhello world שלום بالعالم עולם demo مرحبا שלום עולם بالعالم hello world demo\nمرحبا hello שלום بالعالم עולם world demo بالعالم\nhello world demo",
       117u,
       65u,
       3u,
       indices08,
       numberOfCharacters08,
       direction08},
    };
  const unsigned int numberOfTests = 10u;

  for(unsigned int index = 0u; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!SetBidirectionalInfoTest(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliGetMirroredText(void)
{
  tet_infoline(" UtcDaliGetMirroredText");

  struct GetMirroredTextData data[] =
    {
      {"Zero characters.",
       "",
       0u,
       0u,
       ""},

      {"Left to right characters only.",
       "Hello world\nhello world demo.",
       0u,
       29u,
       "Hello world\nhello world demo."},

      {"Right to left characters but with no characters to mirror.",
       "שלום עולם\nمرحبا بالعالم",
       0u,
       23u,
       "שלום עולם\nمرحبا بالعالم"},

      {"Right to left characters with some characters to mirror.",
       "שלום עולם\n(مرحبا بالعالم)",
       0u,
       25u,
       "שלום עולם\n)مرحبا بالعالم("},

      {"Right to left characters with some characters to mirror. Update last paragraph.",
       "שלום עולם\n(مرحبا بالعالم)",
       10u,
       15u,
       "שלום עולם\n)مرحبا بالعالم("},

      {"Mix of bidirectional text. With more paragraphs.",
       "Hello world demo\nhello world\nhello world (مرحبا بالعالم שלום) עולם\nשלום مرحبا بالعالم עולם (hello) مرحبا بالعالم world"
       " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום (hello) world demo (עולם)\nשלום (مرحبا بالعالم עולם) (hello)",
       0u,
       239u,
       "Hello world demo\nhello world\nhello world (مرحبا بالعالم שלום) עולם\nשלום مرحبا بالعالم עולם )hello( مرحبا بالعالم world"
       " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום )hello( world demo )עולם(\nשלום )مرحبا بالعالم עולם( )hello("},

      {"Mix of bidirectional text. With more paragraphs. Update middle paragraphs.",
       "Hello world demo\nhello world\nhello world (مرحبا بالعالم שלום) עולם\nשלום مرحبا بالعالم עולם (hello) مرحبا بالعالم world"
       " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום (hello) world demo (עולם)\nשלום (مرحبا بالعالم עולם) (hello)",
       29u,
       38u,
       "Hello world demo\nhello world\nhello world (مرحبا بالعالم שלום( עולם\nשלום مرحبا بالعالم עולם (hello) مرحبا بالعالم world"
       " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום (hello) world demo (עולם)\nשלום (مرحبا بالعالم עולם) (hello)"},

      {"Mix of bidirectional text. With more paragraphs. Update middle paragraphs (2).",
       "Hello world demo\nhello world\nhello world (مرحبا بالعالم שלום) עולם\nשלום مرحبا بالعالم עולם (hello) مرحبا بالعالم world"
       " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום (hello) world demo (עולם)\nשלום (مرحبا بالعالم עולם) (hello)",
       67u,
       100u,
       "Hello world demo\nhello world\nhello world (مرحبا بالعالم שלום) עולם\nשלום مرحبا بالعالم עולם )hello( مرحبا بالعالم world"
       " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום (hello) world demo (עולם)\nשלום (مرحبا بالعالم עולם) (hello)"},
    };
  const unsigned int numberOfTests = 8u;

  for(unsigned int index = 0u; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!GetMirroredTextTest(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliGetCharactersDirection(void)
{
  tet_infoline(" UtcDaliGetCharactersDirection");

  bool directions01[] = {};
  bool directions02[] = {
    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
  bool directions03[] = {
    true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true};
  bool directions04[] = {
    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, false, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false};
  bool directions05[] = {
    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false};

  bool directions06[] = {
    true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

  struct GetCharactersDirectionData data[] =
    {
      {"Zero characters",
       "",
       0u,
       0u,
       directions01,
       false},

      {"Left to right characters only",
       "Hello world\nhello world demo",
       0u,
       28u,
       directions02,
       false},

      {"Right to left characters only",
       "שלום עולם\nשלום עולם",
       0u,
       19u,
       directions03,
       false},

      {"Mix of bidirectional text",
       "Hello world\nhello world שלום עולם\nשלום עולם hello world",
       0u,
       55u,
       directions04,
       false},

      {"Mix of bidirectional text. With more paragraphs.",
       "Hello world demo\nhello world\nhello world مرحبا بالعالم שלום עולם\nשלום مرحبا بالعالم עולם hello مرحبا بالعالم world"
       " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום hello world demo עולם\nשלום مرحبا بالعالم עולם hello",
       0u,
       227u,
       directions05,
       false},

      {"Mix of bidirectional text. With more paragraphs. Update first paragraph.",
       "Hello world demo\nhello world\nhello world مرحبا بالعالم שלום עולם\nשלום مرحبا بالعالم עולם hello مرحبا بالعالم world"
       " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום hello world demo עולם\nשלום مرحبا بالعالم עולם hello",
       0u,
       17u,
       directions05,
       false},

      {"Mix of bidirectional text. With more paragraphs. Update from character 29",
       "Hello world demo\nhello world\nhello world مرحبا بالعالم שלום עולם\nשלום مرحبا بالعالم עולם hello مرحبا بالعالم world"
       " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום hello world demo עולם\nשלום مرحبا بالعالم עולם hello",
       29u,
       134u,
       directions05,
       false},

      {"Mix of bidirectional text. With more paragraphs. Update from character 163",
       "Hello world demo\nhello world\nhello world مرحبا بالعالم שלום עולם\nשלום مرحبا بالعالم עולם hello مرحبا بالعالم world"
       " مرحبا بالعالم שלום עולם hello world hello world\nبالعالم שלום hello world demo עולם\nשלום مرحبا بالعالم עולם hello",
       163u,
       35u,
       directions05,
       false},

      {"Mix of bidirectional text. With brackets and LRM",
       "שלום עולם &lrm;(hello)[world]&lrm;",
       0u,
       26u,
       directions06,
       true}};

  const unsigned int numberOfTests = 9u;

  for(unsigned int index = 0u; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!GetCharactersDirectionTest(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}
