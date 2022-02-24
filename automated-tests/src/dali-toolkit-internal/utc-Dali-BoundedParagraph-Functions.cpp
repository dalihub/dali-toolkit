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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bounded-paragraph-helper-functions.h>
#include <dali-toolkit/internal/text/bounded-paragraph-run.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

struct BoundedParagraphData
{
  CharacterIndex characterIndex;
  Length         numberOfCharacters;
};

struct TestCaseData
{
  std::string           description;                       ///< Description of the test.
  std::string           text;                              ///< The text.
  CharacterIndex        firstIndexOfRemovedCharacters;     ///< The first index of removed characters.
  int                   numberOfRemovedCharacters;         ///< The number of removed characters.
  Length                numberOfBoundedParagraphs;         ///< The number of bounded paragraphs before merging.
  BoundedParagraphData* boundedParagraphs;                 ///< The bounded paragraphs info before merging.
  Length                numberOfExpectedBoundedParagraphs; ///< The number of expected bounded paragraphs after merging.
  BoundedParagraphData* expectedBoundedParagraphs;         ///< The expected bounded paragraphs info after merging.
};

void CreateBoundedParagraphRunsFromBoundedParagraphData(
  Vector<BoundedParagraphRun>& boundedParagraphRuns,
  const BoundedParagraphData*  boundedParagraphs,
  const Length&                numberOfBoundedParagraphs)
{
  boundedParagraphRuns.Clear();

  if(boundedParagraphs != nullptr)
  {
    for(Length index = 0u; index < numberOfBoundedParagraphs; index++)
    {
      BoundedParagraphRun boundedParagraphRun;
      boundedParagraphRun.characterRun.characterIndex     = boundedParagraphs[index].characterIndex;
      boundedParagraphRun.characterRun.numberOfCharacters = boundedParagraphs[index].numberOfCharacters;

      boundedParagraphRuns.PushBack(boundedParagraphRun);
    }
  }
}

bool MergeBoundedParagraphRunsTest(TestCaseData testCase)
{
  // 1) Convert boundedParagraphs to vector of BoundedParagraphRun
  Vector<BoundedParagraphRun> boundedParagraphRuns;
  CreateBoundedParagraphRunsFromBoundedParagraphData(boundedParagraphRuns, testCase.boundedParagraphs, testCase.numberOfBoundedParagraphs);

  // 2) Convert expectedBoundedParagraphs to vector of BoundedParagraphRun
  Vector<BoundedParagraphRun> expectedBoundedParagraphRuns;
  CreateBoundedParagraphRunsFromBoundedParagraphData(expectedBoundedParagraphRuns, testCase.expectedBoundedParagraphs, testCase.numberOfExpectedBoundedParagraphs);

  // 3) Convert string text to vector of character utf32
  Vector<Character> utf32Text;
  utf32Text.Resize(testCase.text.size());
  const uint32_t numberOfCharacters = (testCase.text.size() == 0) ? 0 : Utf8ToUtf32(reinterpret_cast<const uint8_t* const>(testCase.text.c_str()), testCase.text.size(), &utf32Text[0u]);
  utf32Text.Resize(numberOfCharacters);

  // 4) Call MergeBoundedParagraphRunsWhenRemoveCharacters
  MergeBoundedParagraphRunsWhenRemoveCharacters(utf32Text, testCase.firstIndexOfRemovedCharacters, testCase.numberOfRemovedCharacters, boundedParagraphRuns);

  // 5) Verify actual with expected
  if(testCase.numberOfExpectedBoundedParagraphs != boundedParagraphRuns.Count())
  {
    std::cout << "  Different number of bounded paragraph runs after merging: " << boundedParagraphRuns.Count() << ", expected : " << testCase.numberOfExpectedBoundedParagraphs << std::endl;
    return false;
  }

  for(unsigned int index = 0u; index < testCase.numberOfExpectedBoundedParagraphs; ++index)
  {
    if(expectedBoundedParagraphRuns[index].characterRun.characterIndex != boundedParagraphRuns[index].characterRun.characterIndex)
    {
      std::cout << "  Different bounded paragraph runs after merging, index : " << index << std::endl;
      std::cout << "  Different characterIndex, actual: " << boundedParagraphRuns[index].characterRun.characterIndex
                << ", expected : " << expectedBoundedParagraphRuns[index].characterRun.characterIndex << std::endl;
      return false;
    }

    if(expectedBoundedParagraphRuns[index].characterRun.numberOfCharacters != boundedParagraphRuns[index].characterRun.numberOfCharacters)
    {
      std::cout << "  Different bounded paragraph runs after merging, index : " << index << std::endl;
      std::cout << "  Different numberOfCharacters, actual: " << boundedParagraphRuns[index].characterRun.numberOfCharacters
                << ", expected : " << expectedBoundedParagraphRuns[index].characterRun.numberOfCharacters << std::endl;
      return false;
    }
  }

  return true;
}

int UtcDaliMergeBoundedParagraphRunsWhenRemoveCharacters(void)
{
  tet_infoline(" UtcDaliMergeBoundedParagraphRunsWhenRemoveCharacters ");

  BoundedParagraphData boundedParagraphs01[]         = {{10u, 14u}, {37u, 15u}};
  BoundedParagraphData expectedBoundedParagraphs01[] = {{10u, 42u}};

  BoundedParagraphData boundedParagraphs02[]         = {{10u, 14u}, {37u, 15u}};
  BoundedParagraphData expectedBoundedParagraphs02[] = {{37u, 15u}};

  BoundedParagraphData boundedParagraphs03[]         = {{10u, 14u}, {37u, 15u}};
  BoundedParagraphData expectedBoundedParagraphs03[] = {{10u, 14u}, {37u, 20u}};

  BoundedParagraphData boundedParagraphs04[]         = {{10u, 14u}, {37u, 15u}};
  BoundedParagraphData expectedBoundedParagraphs04[] = {{10u, 14u}, {37u, 15u}};

  BoundedParagraphData boundedParagraphs05[]         = {{10u, 14u}, {37u, 15u}};
  BoundedParagraphData expectedBoundedParagraphs05[] = {{10u, 14u}, {37u, 15u}};

  BoundedParagraphData boundedParagraphs06[]         = {{10u, 14u}, {37u, 15u}, {64u, 14u}};
  BoundedParagraphData expectedBoundedParagraphs06[] = {{10u, 68u}};

  TestCaseData testCases[] =
    {
      {

        "test-case 01",
        "text one \nParagraph two\n text three \nParagraph four\n text five",
        20u,
        -26,
        2u,
        boundedParagraphs01,
        1u,
        expectedBoundedParagraphs01

      },

      {

        "test-case 02",
        "text one \nParagraph two\n text three \nParagraph four\n text five",
        5u,
        -5,
        2u,
        boundedParagraphs02,
        1u,
        expectedBoundedParagraphs02

      },

      {

        "test-case 03",
        "text one \nParagraph two\n text three \nParagraph four\n text five",
        47u,
        -10,
        2u,
        boundedParagraphs03,
        2u,
        expectedBoundedParagraphs03

      },

      {

        "test-case 04",
        "text one \nParagraph two\n text three \nParagraph four\n text five",
        10u,
        -9,
        2u,
        boundedParagraphs04,
        2u,
        expectedBoundedParagraphs04

      },

      {

        "test-case 05",
        "text one \nParagraph two\n text three \nParagraph four\n text five",
        25u,
        -4,
        2u,
        boundedParagraphs05,
        2u,
        expectedBoundedParagraphs05

      },

      {

        "test-case 06",
        "text one \nParagraph two\n text three \nParagraph four\n text five \nParagraph six\n text seven",
        10u,
        -63,
        3u,
        boundedParagraphs06,
        1u,
        expectedBoundedParagraphs06

      },

    };

  const unsigned int numberOfTests = 6u;

  for(unsigned int index = 0u; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    tet_infoline(testCases[index].description.c_str());

    if(!MergeBoundedParagraphRunsTest(testCases[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}