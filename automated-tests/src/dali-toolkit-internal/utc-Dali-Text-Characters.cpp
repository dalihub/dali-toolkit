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
#include <dali-toolkit/internal/text/characters-helper-functions.h>
#include <toolkit-text-utils.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the following function.
//
//  CharacterRun RetrieveClusteredCharactersOfCharacterIndex(const VisualModelPtr&  visualModel,
//                                                          const LogicalModelPtr& logicalModel,
//                                                          const CharacterIndex&  characterIndex)

//////////////////////////////////////////////////////////

namespace
{
const std::string  DEFAULT_FONT_DIR("/resources/fonts");
const unsigned int DEFAULT_FONT_SIZE = 1152u;

struct RetrieveClusteredCharactersOfCharacterIndexData
{
  std::string     description;            ///< Description of the test.
  std::string     text;                   ///< Input text.
  bool            markupProcessorEnabled; //< Enable markup processor to use markup text.
  unsigned int    numberOfTests;          ///< The number of tests.
  CharacterIndex* characterIndex;         ///< The character index for each test.
  CharacterRun*   clusteredCharacters;    ///< The expected clustered characters run for each test.
};

bool GetRetrieveClusteredCharactersOfCharacterIndexTest(const RetrieveClusteredCharactersOfCharacterIndexData& data)
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
    CharacterRun clusteredCharacters = RetrieveClusteredCharactersOfCharacterIndex(visualModel, logicalModel, data.characterIndex[index]);

    if(clusteredCharacters.characterIndex != data.clusteredCharacters[index].characterIndex)
    {
      std::cout << "  test " << index << " failed. Different clusteredCharacters.characterIndex : " << clusteredCharacters.characterIndex << ", expected : " << data.clusteredCharacters[index].characterIndex << std::endl;
      return false;
    }

    if(clusteredCharacters.numberOfCharacters != data.clusteredCharacters[index].numberOfCharacters)
    {
      std::cout << "  test " << index << " failed. Different clusteredCharacters.numberOfCharacters : " << clusteredCharacters.numberOfCharacters << ", expected : " << data.clusteredCharacters[index].numberOfCharacters << std::endl;
      return false;
    }
  }

  return true;
}

} // namespace

//////////////////////////////////////////////////////////
//
// UtcDaliRetrieveClusteredCharactersOfCharacterIndex
//
//////////////////////////////////////////////////////////

int UtcDaliRetrieveClusteredCharactersOfCharacterIndex(void)
{
  tet_infoline(" UtcDaliRetrieveClusteredCharactersOfCharacterIndex");

  CharacterIndex characterIndex01[] = {0u, 1u, 2u, 10u};

  CharacterRun clusteredCharacters01[] = {
    {0u, 1u},
    {1u, 1u},
    {2u, 1u},
    {10u, 1u}};

  CharacterIndex characterIndex02[] = {0u, 4u, 6u};

  CharacterRun clusteredCharacters02[] = {
    {0u, 7u},
    {0u, 7u},
    {0u, 7u}};

  CharacterIndex characterIndex03[] = {3u, 9u, 14u};

  CharacterRun clusteredCharacters03[] = {
    {2u, 7u},
    {9u, 1u},
    {11u, 4u}};

  CharacterIndex characterIndex04[] = {0u, 1u, 2u, 10u};

  CharacterRun clusteredCharacters04[] = {
    {0u, 1u},
    {1u, 1u},
    {2u, 1u},
    {10u, 1u}};

  struct RetrieveClusteredCharactersOfCharacterIndexData data[] =
    {
      {"Easy latin script",
       "Hello world",
       true,
       4u,
       characterIndex01,
       clusteredCharacters01},

      {"FamilyManWomanGirlBoy Single Complex Emoji script",
       "&#x1F468;&#x200D;&#x1F469;&#x200D;&#x1F467;&#x200D;&#x1F466;",
       true,
       3u,
       characterIndex02,
       clusteredCharacters02},

      {"Long text many Emojis with letters",
       "AB&#x1F468;&#x200D;&#x1F469;&#x200D;&#x1F467;&#x200D;&#x1F466;AB&#x1F469;&#x1F3FB;&#x200D;&#x1F52C;B&#x1F468;&#x200D;&#x1F469;&#x200D;&#x1F467;&#x200D;&#x1F466;AA&#x262a;&#xfe0e;B&#x262a;&#xfe0f;AB",
       true,
       3u,
       characterIndex03,
       clusteredCharacters03},

      {"Arabic script",
       "اهلا و سهلا",
       true,
       4u,
       characterIndex04,
       clusteredCharacters04},
    };
  const unsigned int numberOfTests = 4u;

  for(unsigned int index = 0; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!GetRetrieveClusteredCharactersOfCharacterIndexTest(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}
