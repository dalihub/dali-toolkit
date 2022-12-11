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
#include <dali-toolkit/devel-api/text/spans/strikethrough-span.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

using namespace Dali;
using namespace Toolkit;

int UtcDaliToolkitTextStrikethroughSpanNew(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliToolkitTextStrikethroughSpanNew");

  auto strikethroughSpan = Text::StrikethroughSpan::New();
  DALI_TEST_CHECK(strikethroughSpan);

  END_TEST;
}

int UtcDaliToolkitTextStrikethroughSpanNewColorHeight(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliToolkitTextStrikethroughSpanNewColorHeight");

  auto strikethroughSpan = Text::StrikethroughSpan::New(Color::GREEN, 5.0f);
  DALI_TEST_CHECK(strikethroughSpan);

  END_TEST;
}

int UtcDaliToolkitTextStrikethroughSpanGetColor(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliToolkitTextStrikethroughSpanGetColor");

  auto strikethroughSpan = Text::StrikethroughSpan::New(Color::GREEN, 5.0f);

  DALI_TEST_CHECK(strikethroughSpan);
  DALI_TEST_EQUALS(Color::GREEN, strikethroughSpan.GetColor(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextStrikethroughSpanIsColorDefined(void)

{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliToolkitTextStrikethroughSpanIsColorDefined");

  auto strikethroughSpan = Text::StrikethroughSpan::New(Color::GREEN, 5.0f);

  DALI_TEST_CHECK(strikethroughSpan);
  DALI_TEST_EQUALS(true, strikethroughSpan.IsColorDefined(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextStrikethroughSpanGetHeight(void)

{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliToolkitTextStrikethroughSpanGetHeight");

  auto strikethroughSpan = Text::StrikethroughSpan::New(Color::GREEN, 5.0f);

  DALI_TEST_CHECK(strikethroughSpan);
  DALI_TEST_EQUALS(5.0f, strikethroughSpan.GetHeight(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextStrikethroughSpanIsHeightDefined(void)

{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliToolkitTextStrikethroughSpanIsHeightDefined");

  auto strikethroughSpan = Text::StrikethroughSpan::New(Color::GREEN, 5.0f);

  DALI_TEST_CHECK(strikethroughSpan);
  DALI_TEST_EQUALS(true, strikethroughSpan.IsHeightDefined(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextStrikethroughSpanStrikethroughSpanDownCast(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliToolkitTextStrikethroughSpanStrikethroughSpanDownCast");

  Text::BaseSpan baseSpan = Text::StrikethroughSpan::New();
  DALI_TEST_CHECK(baseSpan);

  Text::StrikethroughSpan strikethroughSpan = Text::StrikethroughSpan::DownCast(baseSpan);
  DALI_TEST_CHECK(strikethroughSpan);

  END_TEST;
}