/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

void dali_toolkit_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_toolkit_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcToolkitIsVertical(void)
{
  ToolkitTestApplication application;

  bool bRet = false;

  tet_infoline(" UtcToolkitIsVertical");
  bRet = IsVertical(ControlOrientation::Up);
  DALI_TEST_EQUALS( bRet, true, TEST_LOCATION );

  bRet = IsVertical(ControlOrientation::Down);
  DALI_TEST_EQUALS( bRet, true, TEST_LOCATION );

  END_TEST;
}

int UtcToolkitIsHorizontal(void)
{
  ToolkitTestApplication application;

  bool bRet = false;

  tet_infoline(" UtcToolkitIsHorizontal");
  bRet = IsHorizontal(ControlOrientation::Left);
  DALI_TEST_EQUALS( bRet, true, TEST_LOCATION );

  bRet = IsHorizontal(ControlOrientation::Right);
  DALI_TEST_EQUALS( bRet, true, TEST_LOCATION );

  END_TEST;
}

