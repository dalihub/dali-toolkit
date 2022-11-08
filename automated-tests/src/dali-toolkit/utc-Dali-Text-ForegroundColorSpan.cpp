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
#include <dali-toolkit/devel-api/text/spans/foreground-color-span.h>

using namespace Dali;
using namespace Toolkit;

int UtcDaliToolkitTextForegroundColorSpanNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextForegroundColorSpanNew");

  auto greenSpan = Text::ForegroundColorSpan::New(Color::GREEN);
  DALI_TEST_CHECK(greenSpan);

  END_TEST;
}

int UtcDaliToolkitTextGetForegroundColor(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextGetForegroundColor");

  auto greenSpan = Text::ForegroundColorSpan::New(Color::GREEN);
  DALI_TEST_CHECK(greenSpan);
  DALI_TEST_EQUALS(Color::GREEN, greenSpan.GetForegroundColor(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextIsForegroundColorDefined(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextIsForegroundColorDefined");

  auto greenSpan = Text::ForegroundColorSpan::New(Color::GREEN);
  DALI_TEST_CHECK(greenSpan);
  DALI_TEST_EQUALS(true, greenSpan.IsForegroundColorDefined(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextForegroundColorSpanDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextForegroundColorSpanDownCast");

  Text::BaseSpan baseSpan = Text::ForegroundColorSpan::New(Color::GREEN);
  DALI_TEST_CHECK(baseSpan);

  Text::ForegroundColorSpan greenSpan = Text::ForegroundColorSpan::DownCast(baseSpan);
  DALI_TEST_CHECK(greenSpan);

  END_TEST;
}
