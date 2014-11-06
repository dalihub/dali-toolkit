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
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

void dali_button_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_button_cleanup(void)
{
  test_return_value = TET_PASS;
}


namespace
{
const Dali::TouchPoint pointDownInside( 0, TouchPoint::Down, 240, 400 );
const Dali::TouchPoint pointUpInside( 0, TouchPoint::Up, 240, 400 );
const Dali::TouchPoint pointLeave( 0, TouchPoint::Leave, 240, 400 );
const Dali::TouchPoint pointEnter( 0, TouchPoint::Motion, 240, 400 );
const Dali::TouchPoint pointDownOutside( 0, TouchPoint::Down, 10, 10 );
const Dali::TouchPoint pointUpOutside( 0, TouchPoint::Up, 10, 10 );

static bool gObjectCreatedCallBackCalled;

static void TestObjectCreatedCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

} // namespace



// Positive test case for a method
int UtcDaliButtonNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonNew");

  CheckBoxButton checkBoxButton = CheckBoxButton::New();
  DALI_TEST_CHECK( checkBoxButton );

  CheckBoxButton checkBoxButton2( checkBoxButton );
  DALI_TEST_CHECK( checkBoxButton2 );
  checkBoxButton2.Reset();


  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestObjectCreatedCallback );
  {
    CheckBoxButton checkBoxButton = CheckBoxButton::New();
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestObjectCreatedCallback );
  {
    PushButton pushButton = PushButton::New();
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );

  // Test down cast
  Handle handleButton = checkBoxButton;
  Button downCastCheckBoxButton = Button::DownCast( handleButton );
  DALI_TEST_CHECK( downCastCheckBoxButton );
  CheckBoxButton downCastCheckBoxButton2 = CheckBoxButton::DownCast( handleButton );
  DALI_TEST_CHECK( downCastCheckBoxButton2 );
  END_TEST;
}

int UtcDaliButtonProperties(void)
{
  tet_infoline("UtcDaliButtonSetProperty: ");
  ToolkitTestApplication application;

  CheckBoxButton checkBoxButton = CheckBoxButton::New();
  PushButton pushButton = PushButton::New();

  //Test various properties
  checkBoxButton.SetProperty( checkBoxButton.GetPropertyIndex("dimmed"), false );
  DALI_TEST_CHECK( false == checkBoxButton.IsDimmed() );

  checkBoxButton.SetProperty( checkBoxButton.GetPropertyIndex("dimmed"), true );
  DALI_TEST_CHECK( true == checkBoxButton.IsDimmed() );
  END_TEST;
}

int UtcDaliButtonSetGetDimmed(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonSetGetDimmed");

  CheckBoxButton checkBoxButton = CheckBoxButton::New();
  checkBoxButton.SetDimmed( true );

  DALI_TEST_CHECK( checkBoxButton.IsDimmed() );
  checkBoxButton.SetDimmed( false );

  DALI_TEST_CHECK( !checkBoxButton.IsDimmed() );
  checkBoxButton.SetDimmed( true );

  DALI_TEST_CHECK( checkBoxButton.IsDimmed() );
  checkBoxButton.SetDimmed( false );

  DALI_TEST_CHECK( !checkBoxButton.IsDimmed() );
  END_TEST;
}
