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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <stdlib.h>
#include <iostream>

#include <dali-toolkit/devel-api/toolkit-pre-initialize.h>

using namespace Dali;
using namespace Toolkit;

void dali_toolkit_preinitialize_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_toolkit_preinitialize_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcToolkitPreInitialize01(void)
{
  ToolkitTestApplication::PREINITIALIZE_ADAPTOR_CREATION_ENABLED = true;
  DaliToolkitPreInitialize(nullptr, nullptr, nullptr);

  Control control = Control::New();
  control.SetBackgroundColor(Color::RED);

  ToolkitTestApplication& application = Test::ToolkitTestApplication::GetPreInitializedApplication();

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render();

  // Make it always pass
  DALI_TEST_CHECK(true);

  END_TEST;
}

int UtcToolkitPreInitialize02(void)
{
  ToolkitTestApplication::PREINITIALIZE_ADAPTOR_CREATION_ENABLED = false;
  DaliToolkitPreInitialize(nullptr, nullptr, nullptr);

  ToolkitTestApplication& application = Test::ToolkitTestApplication::GetPreInitializedApplication();

  ToolkitTestApplication::PREINITIALIZE_ADAPTOR_CREATION_ENABLED = true;

  Control control = Control::New();
  control.SetBackgroundColor(Color::RED);

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render();

  // Make it always pass
  DALI_TEST_CHECK(true);

  END_TEST;
}

int UtcToolkitPreInitializeMultipleTimes(void)
{
  DaliToolkitPreInitialize(nullptr, nullptr, nullptr);
  DaliToolkitPreInitialize(nullptr, nullptr, nullptr);
  DaliToolkitPreInitialize(nullptr, nullptr, nullptr);

  ToolkitTestApplication& application = Test::ToolkitTestApplication::GetPreInitializedApplication();

  Control control = Control::New();
  control.SetBackgroundColor(Color::RED);

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render();

  // Make it always pass
  DALI_TEST_CHECK(true);

  END_TEST;
}
