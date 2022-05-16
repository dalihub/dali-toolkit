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
#include <dali-toolkit/internal/text/bounded-paragraph-run.h>
#include <dali-toolkit/internal/text/character-spacing-character-run.h>
#include <dali-toolkit/internal/text/color-run.h>
#include <dali-toolkit/internal/text/font-description-run.h>
#include <dali-toolkit/internal/text/strikethrough-character-run.h>
#include <dali-toolkit/internal/text/underlined-character-run.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the following functions for AbstractStyleCharacterRun.
//  CharacterIndex GetStartCharacterIndex() const;
//  Length GetNumberOfCharacters() const;
//  CharacterIndex GetEndCharacterIndex() const;

template<typename TYPE_OF_RUN>
void TestAbstractStyleCharacterRunEmptyCharacterRun(std::string test_name)
{
  tet_infoline(" TestAbstractStyleCharacterRunEmptyCharacterRun ");
  std::cout << "  testing " << test_name << std::endl;

  tet_infoline(" Default Constructor ");
  TYPE_OF_RUN abstractStyleCharacterRun;

  tet_infoline(" AbstractStyleCharacterRun_GetStartCharacterIndex ");
  DALI_TEST_EQUALS(abstractStyleCharacterRun.GetStartCharacterIndex(), 0u, TEST_LOCATION);

  tet_infoline(" AbstractStyleCharacterRun_GetNumberOfCharacters ");
  DALI_TEST_EQUALS(abstractStyleCharacterRun.GetNumberOfCharacters(), 0u, TEST_LOCATION);

  tet_infoline(" AbstractStyleCharacterRun_GetEndCharacterIndex ");
  DALI_TEST_EQUALS(abstractStyleCharacterRun.GetEndCharacterIndex(), 0u, TEST_LOCATION);
}

template<typename TYPE_OF_RUN>
void TestAbstractStyleCharacterRunOneCharacter(std::string test_name)
{
  tet_infoline(" TestAbstractStyleCharacterRunOneCharacter ");
  std::cout << "  testing " << test_name << std::endl;

  TYPE_OF_RUN abstractStyleCharacterRun;
  abstractStyleCharacterRun.characterRun.numberOfCharacters = 1u;
  tet_infoline(" AbstractStyleCharacterRun_GetStartCharacterIndex ");
  DALI_TEST_EQUALS(abstractStyleCharacterRun.GetStartCharacterIndex(), 0u, TEST_LOCATION);

  tet_infoline(" AbstractStyleCharacterRun_GetNumberOfCharacters ");
  DALI_TEST_EQUALS(abstractStyleCharacterRun.GetNumberOfCharacters(), 1u, TEST_LOCATION);

  tet_infoline(" AbstractStyleCharacterRun_GetEndCharacterIndex ");
  DALI_TEST_EQUALS(abstractStyleCharacterRun.GetEndCharacterIndex(), 0u, TEST_LOCATION);

  abstractStyleCharacterRun.characterRun.characterIndex = 5u;
  tet_infoline(" AbstractStyleCharacterRun_GetStartCharacterIndex ");
  DALI_TEST_EQUALS(abstractStyleCharacterRun.GetStartCharacterIndex(), 5u, TEST_LOCATION);

  tet_infoline(" AbstractStyleCharacterRun_GetNumberOfCharacters ");
  DALI_TEST_EQUALS(abstractStyleCharacterRun.GetNumberOfCharacters(), 1u, TEST_LOCATION);

  tet_infoline(" AbstractStyleCharacterRun_GetEndCharacterIndex ");
  DALI_TEST_EQUALS(abstractStyleCharacterRun.GetEndCharacterIndex(), 5u, TEST_LOCATION);
}

template<typename TYPE_OF_RUN>
void TestAbstractStyleCharacterRunMoreThanOneCharacter(std::string test_name)
{
  tet_infoline(" TestAbstractStyleCharacterRunOneCharacter ");
  std::cout << "  testing " << test_name << std::endl;

  TYPE_OF_RUN abstractStyleCharacterRun;
  abstractStyleCharacterRun.characterRun.numberOfCharacters = 15u;
  tet_infoline(" AbstractStyleCharacterRun_GetStartCharacterIndex ");
  DALI_TEST_EQUALS(abstractStyleCharacterRun.GetStartCharacterIndex(), 0u, TEST_LOCATION);

  tet_infoline(" AbstractStyleCharacterRun_GetNumberOfCharacters ");
  DALI_TEST_EQUALS(abstractStyleCharacterRun.GetNumberOfCharacters(), 15u, TEST_LOCATION);

  tet_infoline(" AbstractStyleCharacterRun_GetEndCharacterIndex ");
  DALI_TEST_EQUALS(abstractStyleCharacterRun.GetEndCharacterIndex(), 14u, TEST_LOCATION);

  abstractStyleCharacterRun.characterRun.characterIndex = 5u;
  tet_infoline(" AbstractStyleCharacterRun_GetStartCharacterIndex ");
  DALI_TEST_EQUALS(abstractStyleCharacterRun.GetStartCharacterIndex(), 5u, TEST_LOCATION);

  tet_infoline(" AbstractStyleCharacterRun_GetNumberOfCharacters ");
  DALI_TEST_EQUALS(abstractStyleCharacterRun.GetNumberOfCharacters(), 15u, TEST_LOCATION);

  tet_infoline(" AbstractStyleCharacterRun_GetEndCharacterIndex ");
  DALI_TEST_EQUALS(abstractStyleCharacterRun.GetEndCharacterIndex(), 19u, TEST_LOCATION);
}

int UtcDaliTextAbstractStyleCharacterRun(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAbstractStyleCharacterRun");

  TestAbstractStyleCharacterRunEmptyCharacterRun<Dali::Toolkit::Text::ColorRun>("ColorRun");
  TestAbstractStyleCharacterRunEmptyCharacterRun<Dali::Toolkit::Text::CharacterSpacingCharacterRun>("CharacterSpacingCharacterRun");
  TestAbstractStyleCharacterRunEmptyCharacterRun<Dali::Toolkit::Text::FontDescriptionRun>("FontDescriptionRun");
  TestAbstractStyleCharacterRunEmptyCharacterRun<Dali::Toolkit::Text::UnderlinedCharacterRun>("UnderlinedCharacterRun");
  TestAbstractStyleCharacterRunEmptyCharacterRun<Dali::Toolkit::Text::StrikethroughCharacterRun>("StrikethroughCharacterRun");
  TestAbstractStyleCharacterRunEmptyCharacterRun<Dali::Toolkit::Text::BoundedParagraphRun>("BoundedParagraphRun");

  TestAbstractStyleCharacterRunOneCharacter<Dali::Toolkit::Text::ColorRun>("ColorRun");
  TestAbstractStyleCharacterRunOneCharacter<Dali::Toolkit::Text::CharacterSpacingCharacterRun>("CharacterSpacingCharacterRun");
  TestAbstractStyleCharacterRunOneCharacter<Dali::Toolkit::Text::FontDescriptionRun>("FontDescriptionRun");
  TestAbstractStyleCharacterRunOneCharacter<Dali::Toolkit::Text::UnderlinedCharacterRun>("UnderlinedCharacterRun");
  TestAbstractStyleCharacterRunOneCharacter<Dali::Toolkit::Text::StrikethroughCharacterRun>("StrikethroughCharacterRun");
  TestAbstractStyleCharacterRunOneCharacter<Dali::Toolkit::Text::BoundedParagraphRun>("BoundedParagraphRun");

  TestAbstractStyleCharacterRunMoreThanOneCharacter<Dali::Toolkit::Text::ColorRun>("ColorRun");
  TestAbstractStyleCharacterRunMoreThanOneCharacter<Dali::Toolkit::Text::CharacterSpacingCharacterRun>("CharacterSpacingCharacterRun");
  TestAbstractStyleCharacterRunMoreThanOneCharacter<Dali::Toolkit::Text::FontDescriptionRun>("FontDescriptionRun");
  TestAbstractStyleCharacterRunMoreThanOneCharacter<Dali::Toolkit::Text::UnderlinedCharacterRun>("UnderlinedCharacterRun");
  TestAbstractStyleCharacterRunMoreThanOneCharacter<Dali::Toolkit::Text::StrikethroughCharacterRun>("StrikethroughCharacterRun");
  TestAbstractStyleCharacterRunMoreThanOneCharacter<Dali::Toolkit::Text::BoundedParagraphRun>("BoundedParagraphRun");

  END_TEST;
}
