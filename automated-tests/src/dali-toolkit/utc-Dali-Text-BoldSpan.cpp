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
#include <dali-toolkit/devel-api/text/spans/bold-span.h>

using namespace Dali;
using namespace Toolkit;

int UtcDaliToolkitTextBoldSpanNew(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliToolkitTextBoldSpanNew");

  auto boldSpan = Text::BoldSpan::New();

  DALI_TEST_CHECK(boldSpan);

  END_TEST;
}

int UtcDaliToolkitTextBoldSpanDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliToolkitTextBoldSpanDownCast");

  Text::BaseSpan baseSpan = Text::BoldSpan::New();
  DALI_TEST_CHECK(baseSpan);

  Text::BoldSpan boldSpan = Text::BoldSpan::DownCast(baseSpan);
  DALI_TEST_CHECK(boldSpan);

  END_TEST;
}
