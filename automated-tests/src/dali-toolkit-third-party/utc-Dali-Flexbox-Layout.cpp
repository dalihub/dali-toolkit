/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

extern "C"
{
#include "facebook-flexbox/layout-test-utils.h"
}

//////////////////////////////////////////////////////////

int UtcDaliFlexboxLayoutTest(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliFlexboxLayoutTest");

  // The test function is a 3rd party function that should return true if the test passes
  if( perform_layout_test() )
  {
    tet_result(TET_PASS);
  }
  else
  {
    tet_result(TET_FAIL);
  }

  END_TEST;
}
