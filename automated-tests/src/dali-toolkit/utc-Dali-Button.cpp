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
#include <dali/integration-api/events/touch-event-integ.h>

using namespace Dali;
using namespace Toolkit;


void utc_dali_toolkit_button_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_button_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
static bool gButtonClicked = false;

static bool ButtonClicked( Button button )
{
  gButtonClicked = true;
  return false;
}

const Dali::TouchPoint pointDownInside( 0, TouchPoint::Down, 240, 400 );
const Dali::TouchPoint pointUpInside( 0, TouchPoint::Up, 240, 400 );
const Dali::TouchPoint pointLeave( 0, TouchPoint::Leave, 240, 400 );
const Dali::TouchPoint pointEnter( 0, TouchPoint::Motion, 240, 400 );
const Dali::TouchPoint pointDownOutside( 0, TouchPoint::Down, 10, 10 );
const Dali::TouchPoint pointUpOutside( 0, TouchPoint::Up, 10, 10 );

static float ANIMATION_TIME( 0.5f );
} // namespace



// Positive test case for a method
int UtcDaliButtonNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonNew");

  PushButton pushButton = PushButton::New();

  DALI_TEST_CHECK( pushButton );

  PushButton pushButton2( pushButton );

  DALI_TEST_CHECK( pushButton2 );

  pushButton2.Reset();

  // Test down cast
  Handle handleButton;
  handleButton = pushButton;
  Button downCastPushButton = Button::DownCast( handleButton );
  DALI_TEST_CHECK( downCastPushButton );
  PushButton downCastPushButton2 = PushButton::DownCast( handleButton );
  DALI_TEST_CHECK( downCastPushButton2 );

  END_TEST;
}

int UtcDaliButtonSetProperty(void)
{
  tet_infoline("UtcDaliButtonSetProperty: ");
  ToolkitTestApplication application;

  PushButton pushButton = PushButton::New();

  pushButton.SetProperty(pushButton.GetPropertyIndex("disabled"), false);
  DALI_TEST_CHECK( false == pushButton.IsDisabled() );
  pushButton.SetProperty(pushButton.GetPropertyIndex("disabled"), true);
  DALI_TEST_CHECK( true == pushButton.IsDisabled() );
  END_TEST;
}

int UtcDaliButtonSetGetDimmed(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonSetGetDimmed");

  PushButton pushButton = PushButton::New();

  pushButton.SetDisabled( true );

  DALI_TEST_CHECK( pushButton.IsDisabled() );

  pushButton.SetDisabled( false );

  DALI_TEST_CHECK( !pushButton.IsDisabled() );

  pushButton.SetDisabled( true );

  DALI_TEST_CHECK( pushButton.IsDisabled() );

  pushButton.SetDisabled( false );

  DALI_TEST_CHECK( !pushButton.IsDisabled() );
  END_TEST;
}

int UtcDaliButtonSize(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonSize");

  ImageActor image01 = ImageActor::New(CreateBitmapImage());
  image01.SetSize( 100, 50 );

  PushButton pushButton;

  Vector3 size;

  // Test1 Size is set through Actor API

  // First an image is set, then SetSize is called.
  pushButton = PushButton::New();

  pushButton.SetBackgroundImage( image01 );
  pushButton.SetSize( 10.f, 10.f );

  application.SendNotification();
  application.Render();

  size = pushButton.GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 10.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 10.f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliButtonClicked(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonClicked");

  PushButton pushButton = PushButton::New();
  pushButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  pushButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pushButton.SetPosition( 240, 400 );
  pushButton.SetSize( 100, 100 );

  Stage::GetCurrent().Add( pushButton );

  application.SendNotification();
  application.Render();

  // connect to its touch signal
  pushButton.ClickedSignal().Connect( &ButtonClicked );

  Dali::Integration::TouchEvent event;

  // Test1. Touch point down and up inside the button.

  gButtonClicked = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gButtonClicked );

  // Test2. Touch point down and up outside the button.

  gButtonClicked = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gButtonClicked );

  // Test3. Touch point down inside and up outside the button.

  gButtonClicked = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointLeave );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gButtonClicked );

  // Test4. Touch point down outside and up inside the button.

  gButtonClicked = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointEnter );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gButtonClicked );
  END_TEST;
}

namespace
{

static bool gClickedCallBackCalled;

static bool TestClickedCallback(Button button)
{
  gClickedCallBackCalled = true;
  return true;
}

} // namespace

int UtcDaliButtonConnectSignal(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliButtonConnectSignal()");

  gClickedCallBackCalled = false;

  PushButton pushButton = PushButton::New();
  pushButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  pushButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pushButton.SetPosition( 240, 400 );
  pushButton.SetSize( 100, 100 );

  Stage::GetCurrent().Add( pushButton );

  application.SendNotification();
  application.Render();

  // connect to its clicked signal
  pushButton.ClickedSignal().Connect(TestClickedCallback);

  Dali::Integration::TouchEvent event;

  // Touch point down and up inside the button.

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gClickedCallBackCalled == true );

  gClickedCallBackCalled = false;
  pushButton.ClickedSignal().Disconnect(TestClickedCallback);

  // simulate another touch event
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gClickedCallBackCalled == false );
  END_TEST;
}

int UtcDaliButtonSetGetAnimationTime(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonSetGetAnimationTime");

  PushButton pushButton = PushButton::New();

  pushButton.SetAnimationTime( ANIMATION_TIME );

  DALI_TEST_EQUALS( pushButton.GetAnimationTime(), ANIMATION_TIME, TEST_LOCATION );

  END_TEST;
}
