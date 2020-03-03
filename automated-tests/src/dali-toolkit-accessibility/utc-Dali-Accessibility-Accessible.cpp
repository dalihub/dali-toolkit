/**
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali/devel-api/adaptor-framework/accessibility.h>

void utc_dali_toolkit_accessibility_accessible_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_accessibility_accessible_cleanup(void)
{
  test_return_value = TET_PASS;
}

int utcDaliAccessibilityCheckBoxButtonGetStates(void)
{
  ToolkitTestApplication application;

  auto check_box_button = Toolkit::CheckBoxButton::New();
  auto q = Dali::Accessibility::Accessible::Get( check_box_button );
  DALI_TEST_CHECK( q );
  auto states = q->GetStates();
  DALI_TEST_EQUALS( (int) states[ Dali::Accessibility::State::SELECTABLE ], (int) true, TEST_LOCATION );

  END_TEST;
}

int utcDaliAccessibilityCheckLabelText(void)
{
  ToolkitTestApplication application;

  auto check_box_button = Toolkit::CheckBoxButton::New();
  check_box_button.SetLabelText( "button" );
  auto q = Dali::Accessibility::Accessible::Get( check_box_button );
  DALI_TEST_CHECK( q );
  DALI_TEST_EQUALS( q->GetName(), "button", TEST_LOCATION );

  END_TEST;
}
