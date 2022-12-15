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
#include <dali-toolkit/devel-api/text/spans/underline-span.h>

using namespace Dali;
using namespace Toolkit;

int UtcDaliToolkitTextUnderlineSpanNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextUnderlineSpanNew");

  auto underlineSpan = Text::UnderlineSpan::New();
  DALI_TEST_CHECK(underlineSpan);

  END_TEST;
}

int UtcDaliToolkitTextUnderlineSpanNewSolid(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextUnderlineSpanNewSolid");

  auto underlineSpan = Text::UnderlineSpan::NewSolid(Color::GREEN, 5.0f);
  DALI_TEST_CHECK(underlineSpan);

  END_TEST;
}

int UtcDaliToolkitTextUnderlineSpanNewDashed(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextUnderlineSpanNewDashed");

  auto underlineSpan = Text::UnderlineSpan::NewDashed(Color::GREEN, 5.0f, 2.0f, 3.0f);
  DALI_TEST_CHECK(underlineSpan);

  END_TEST;
}

int UtcDaliToolkitTextUnderlineSpanNewDouble(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextUnderlineSpanNewDouble");

  auto underlineSpan = Text::UnderlineSpan::NewDouble(Color::GREEN, 5.0f);
  DALI_TEST_CHECK(underlineSpan);

  END_TEST;
}

int UtcDaliToolkitTextGetType(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextGetType");

  auto underlineSpan = Text::UnderlineSpan::NewDouble(Color::GREEN, 5.0f);
  DALI_TEST_CHECK(underlineSpan);
  DALI_TEST_EQUALS(Text::Underline::DOUBLE, underlineSpan.GetType(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextIsTypeDefined(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextIsTypeDefined");

  auto underlineSpan = Text::UnderlineSpan::NewDouble(Color::GREEN, 5.0f);
  DALI_TEST_CHECK(underlineSpan);
  DALI_TEST_EQUALS(true, underlineSpan.IsTypeDefined(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextGetColor(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextGetColor");

  auto underlineSpan = Text::UnderlineSpan::NewSolid(Color::GREEN, 5.0f);
  DALI_TEST_CHECK(underlineSpan);
  DALI_TEST_EQUALS(Color::GREEN, underlineSpan.GetColor(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextIsColorDefined(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextIsColorDefined");

  auto underlineSpan = Text::UnderlineSpan::NewSolid(Color::GREEN, 5.0f);
  DALI_TEST_CHECK(underlineSpan);
  DALI_TEST_EQUALS(true, underlineSpan.IsColorDefined(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextGetHeight(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextGetHeight");

  auto underlineSpan = Text::UnderlineSpan::NewSolid(Color::GREEN, 5.0f);
  DALI_TEST_CHECK(underlineSpan);
  DALI_TEST_EQUALS(5.0f, underlineSpan.GetHeight(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextIsHeightDefined(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextIsHeightDefined");

  auto underlineSpan = Text::UnderlineSpan::NewSolid(Color::GREEN, 5.0f);
  DALI_TEST_CHECK(underlineSpan);
  DALI_TEST_EQUALS(true, underlineSpan.IsHeightDefined(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextGetDashGap(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextGetDashGap");

  auto underlineSpan = Text::UnderlineSpan::NewDashed(Color::GREEN, 5.0f, 2.0f, 3.0f);
  DALI_TEST_CHECK(underlineSpan);
  DALI_TEST_EQUALS(2.0f, underlineSpan.GetDashGap(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextIsDashGapDefined(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextIsDashGapDefined");

  auto underlineSpan = Text::UnderlineSpan::NewDashed(Color::GREEN, 5.0f, 2.0f, 3.0f);
  DALI_TEST_CHECK(underlineSpan);
  DALI_TEST_EQUALS(true, underlineSpan.IsDashGapDefined(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextGetDashWidth(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextGetDashWidth");

  auto underlineSpan = Text::UnderlineSpan::NewDashed(Color::GREEN, 5.0f, 2.0f, 3.0f);
  DALI_TEST_CHECK(underlineSpan);
  DALI_TEST_EQUALS(3.0f, underlineSpan.GetDashWidth(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextIsDashWidthDefined(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextIsDashWidthDefined");

  auto underlineSpan = Text::UnderlineSpan::NewDashed(Color::GREEN, 5.0f, 2.0f, 3.0f);
  DALI_TEST_CHECK(underlineSpan);
  DALI_TEST_EQUALS(true, underlineSpan.IsDashWidthDefined(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextUnderlineSpanDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextUnderlineSpanDownCast");

  Text::BaseSpan baseSpan = Text::UnderlineSpan::New();
  DALI_TEST_CHECK(baseSpan);

  Text::UnderlineSpan underlineSpan = Text::UnderlineSpan::DownCast(baseSpan);
  DALI_TEST_CHECK(underlineSpan);

  END_TEST;
}
