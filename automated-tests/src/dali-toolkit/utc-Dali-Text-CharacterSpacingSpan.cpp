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


#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/text/spans/character-spacing-span.h>

using namespace Dali;
using namespace Toolkit;

int UtcDaliToolkitCharacterSpacingSpanNew(void)
{
    ToolkitTestApplication application;
    tet_infoline("UtcDaliToolkitCharacterSpacingSpanNew");

    auto charSpacingSpan = Text::CharacterSpacingSpan::New(5.6f);
    DALI_TEST_CHECK(charSpacingSpan);

    END_TEST;
}

int UtcDaliToolkitGetCharacterSpacingSpan(void)
{
    ToolkitTestApplication application;
    tet_infoline("UtcDaliToolkitCharacterSpacingSpanNew");

    auto charSpacingSpan = Text::CharacterSpacingSpan::New(9.1f);
    DALI_TEST_CHECK(charSpacingSpan);
    DALI_TEST_EQUALS(9.1f, charSpacingSpan.GetCharacterSpacing(), TEST_LOCATION);

    END_TEST;
}

int UtcDaliToolkitCharacterSpacingSpanDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliToolkitCharacterSpacingSpanDownCast");

  Text::BaseSpan baseSpan = Text::CharacterSpacingSpan::New(3.6f);
  DALI_TEST_CHECK(baseSpan);

  Text::CharacterSpacingSpan charSpacingSpan = Text::CharacterSpacingSpan::DownCast(baseSpan);
  DALI_TEST_CHECK(charSpacingSpan);

  END_TEST;
}

int UtcDaliToolkitIsCharacterSpacingDefined(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliToolkitIsCharacterSpacingDefined");

  auto charSpacingSpan = Text::CharacterSpacingSpan::New(4.2f);
  DALI_TEST_CHECK(charSpacingSpan);
  DALI_TEST_EQUALS(true, charSpacingSpan.IsCharacterSpacingDefined(), TEST_LOCATION);

  END_TEST;
}
