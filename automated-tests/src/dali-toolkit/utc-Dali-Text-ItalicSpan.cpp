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
#include <dali-toolkit/devel-api/text/spans/italic-span.h>

using namespace Dali;
using namespace Toolkit;

int UtcDaliToolkitTextItalicSpanNew(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliToolkitTextItalicSpanNew");

  auto italicSpan = Text::ItalicSpan::New();
  DALI_TEST_CHECK(italicSpan);

  END_TEST;
}

int UtcDaliToolkitTextItalicSpanDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliToolkitTextItalicSpanDownCast");

  Text::BaseSpan baseSpan = Text::ItalicSpan::New();

  DALI_TEST_CHECK(baseSpan);
  Text::ItalicSpan italicSpan = Text::ItalicSpan::DownCast(baseSpan);

  DALI_TEST_CHECK(italicSpan);
  END_TEST;
}