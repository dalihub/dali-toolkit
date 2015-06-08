/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_default_controls_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_default_controls_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliDefaultControlsCreateSolidColorActor(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliDefaultControlsCreateSolidColorActor");

  ImageActor image1 = CreateSolidColorActor( Color::RED );
  ImageActor image2 = CreateSolidColorActor( Color::RED, true, Color::BLUE, 2 );
  ImageActor image3 = CreateSolidColorActor( Color::RED, true, Color::BLUE, 12 );

  DALI_TEST_CHECK(image1);
  DALI_TEST_CHECK(image2);
  DALI_TEST_CHECK(!image3);
  END_TEST;
}
