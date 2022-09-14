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
#include <dali-toolkit/devel-api/text/range.h>

using namespace Dali;
using namespace Toolkit;

int UtcDaliToolkitTextRangeNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextRangeNew");

  auto range15to20 = Dali::Toolkit::Text::Range::New(15u, 20u);
  DALI_TEST_CHECK(range15to20);

  auto range25to20 = Dali::Toolkit::Text::Range::New(25u, 20u);
  DALI_TEST_CHECK(range25to20);

  END_TEST;
}

int UtcDaliToolkitTextRangeGetStartIndex(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextRangeGetStartIndex");

  auto range15to20 = Dali::Toolkit::Text::Range::New(15u, 20u);
  DALI_TEST_CHECK(range15to20);
  DALI_TEST_EQUALS(15u, range15to20.GetStartIndex(), TEST_LOCATION);

  auto range25to20 = Dali::Toolkit::Text::Range::New(25u, 20u);
  DALI_TEST_CHECK(range25to20);
  DALI_TEST_EQUALS(20u, range25to20.GetStartIndex(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextRangeGetEndIndex(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextRangeGetEndIndex");

  auto range15to20 = Dali::Toolkit::Text::Range::New(15u, 20u);
  DALI_TEST_CHECK(range15to20);
  DALI_TEST_EQUALS(20u, range15to20.GetEndIndex(), TEST_LOCATION);

  auto range25to20 = Dali::Toolkit::Text::Range::New(25u, 20u);
  DALI_TEST_CHECK(range25to20);
  DALI_TEST_EQUALS(25u, range25to20.GetEndIndex(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextRangeGetNumberOfIndices(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextRangeGetNumberOfIndices");

  auto range15to20 = Dali::Toolkit::Text::Range::New(15u, 20u);
  DALI_TEST_CHECK(range15to20);
  DALI_TEST_EQUALS(6u, range15to20.GetNumberOfIndices(), TEST_LOCATION);

  auto range25to20 = Dali::Toolkit::Text::Range::New(25u, 20u);
  DALI_TEST_CHECK(range25to20);
  DALI_TEST_EQUALS(6u, range25to20.GetNumberOfIndices(), TEST_LOCATION);

  END_TEST;
}