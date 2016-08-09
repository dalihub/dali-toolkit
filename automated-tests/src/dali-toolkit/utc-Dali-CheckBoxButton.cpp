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

using namespace Dali;
using namespace Toolkit;

namespace
{

static bool gCheckBoxButtonState = false;
bool CheckBoxButtonClicked( Button button )
{
  gCheckBoxButtonState = button.IsSelected();
  return true;
}

} // namespace

void checkbox_button_startup(void)
{
  test_return_value = TET_UNDEF;
}

void checkbox_button_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliCheckBoxButtonConstructorP(void)
{
  TestApplication application;

  CheckBoxButton checkBox;

  DALI_TEST_CHECK( !checkBox );
  END_TEST;
}

int UtcDaliCheckBoxButtonCopyConstructorP(void)
{
  TestApplication application;

  // Initialize an object, ref count == 1
  CheckBoxButton checkBox = CheckBoxButton::New();

  CheckBoxButton copy( checkBox );
  DALI_TEST_CHECK( copy );
  END_TEST;
}

int UtcDaliCheckBoxButtonAssignmentOperatorP(void)
{
  TestApplication application;

  CheckBoxButton checkBox = CheckBoxButton::New();

  CheckBoxButton copy( checkBox );
  DALI_TEST_CHECK( copy );

  DALI_TEST_CHECK( checkBox == copy );
  END_TEST;
}

int UtcDaliCheckBoxButtonNewP(void)
{
  TestApplication application;

  CheckBoxButton checkBox = CheckBoxButton::New();

  DALI_TEST_CHECK( checkBox );
  END_TEST;
}

int UtcDaliCheckBoxButtonDownCastP(void)
{
  TestApplication application;

  CheckBoxButton checkBox = CheckBoxButton::New();

  BaseHandle object(checkBox);

  CheckBoxButton checkBox2 = CheckBoxButton::DownCast( object );
  DALI_TEST_CHECK(checkBox2);

  CheckBoxButton checkBox3 = DownCast< CheckBoxButton >(object);
  DALI_TEST_CHECK(checkBox3);
  END_TEST;
}

int UtcDaliCheckBoxButtonDownCastN(void)
{
  TestApplication application;

  BaseHandle unInitializedObject;

  CheckBoxButton checkBox1 = CheckBoxButton::DownCast( unInitializedObject );
  DALI_TEST_CHECK( !checkBox1 );

  CheckBoxButton checkBox2 = DownCast< CheckBoxButton >( unInitializedObject );
  DALI_TEST_CHECK( !checkBox2 );
  END_TEST;
}

int UtcDaliCheckBoxButtonSetGetSelected(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCheckBoxButtonSetGetSelected");

  CheckBoxButton checkBoxButton = CheckBoxButton::New();
  checkBoxButton.StateChangedSignal().Connect( &CheckBoxButtonClicked );

  // global var used to check if CheckBoxButtonClicked is called;
  gCheckBoxButtonState = false;

  checkBoxButton.SetSelected( true );

  DALI_TEST_CHECK( checkBoxButton.IsSelected() );
  DALI_TEST_CHECK( gCheckBoxButtonState );

  checkBoxButton.SetSelected( false );

  DALI_TEST_CHECK( !checkBoxButton.IsSelected() );
  DALI_TEST_CHECK( !gCheckBoxButtonState );

  checkBoxButton.SetSelected( true );

  DALI_TEST_CHECK( checkBoxButton.IsSelected() );
  DALI_TEST_CHECK( gCheckBoxButtonState );
  END_TEST;
}
