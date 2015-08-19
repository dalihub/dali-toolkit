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
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/scripting/script.h>

using namespace Dali;
using namespace Dali::Toolkit;

void dali_script_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_script_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliScriptExecuteFileN(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScriptExecuteFileN");

  Script script = Script::New();

  bool ok = script.ExecuteFile("non-existing file");

  DALI_TEST_CHECK( !ok );

  END_TEST;
}
