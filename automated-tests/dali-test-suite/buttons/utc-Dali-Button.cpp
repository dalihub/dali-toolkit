//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-toolkit/public-api/controls/buttons/check-box-button.h>
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Toolkit;

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

static bool gObjectCreatedCallBackCalled;

static void TestObjectCreatedCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

static float ANIMATION_TIME( 0.5f );
} // namespace

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliButtonNew, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliButtonProperties, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliButtonSetGetDimmed, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliButtonSize, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliButtonClicked, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliButtonConnectSignal, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliButtonSetGetAnimationTime, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}


// Positive test case for a method
static void UtcDaliButtonNew()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonNew");

  CheckBoxButton checkBoxButton = CheckBoxButton::New();

  DALI_TEST_CHECK( checkBoxButton );

  PushButton pushButton = PushButton::New();

  DALI_TEST_CHECK( pushButton );

  CheckBoxButton checkBoxButton2( checkBoxButton );

  DALI_TEST_CHECK( checkBoxButton2 );

  PushButton pushButton2( pushButton );

  DALI_TEST_CHECK( pushButton2 );

  checkBoxButton2 = NULL;
  pushButton2 = NULL;


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
  Handle handleButton;
  handleButton = pushButton;
  Button downCastPushButton = Button::DownCast( handleButton );
  DALI_TEST_CHECK( downCastPushButton );
  PushButton downCastPushButton2 = PushButton::DownCast( handleButton );
  DALI_TEST_CHECK( downCastPushButton2 );

  handleButton = checkBoxButton;
  Button downCastCheckBoxButton = Button::DownCast( handleButton );
  DALI_TEST_CHECK( downCastCheckBoxButton );
  CheckBoxButton downCastCheckBoxButton2 = CheckBoxButton::DownCast( handleButton );
  DALI_TEST_CHECK( downCastCheckBoxButton2 );
}

static void UtcDaliButtonProperties()
{
  tet_infoline("UtcDaliButtonSetProperty: ");
  ToolkitTestApplication application;

  CheckBoxButton checkBoxButton = CheckBoxButton::New();
  PushButton pushButton = PushButton::New();

  //Test various properties
  checkBoxButton.SetProperty( Button::PROPERTY_DIMMED, false );
  DALI_TEST_CHECK( false == checkBoxButton.IsDimmed() );
  DALI_TEST_CHECK( false == checkBoxButton.GetProperty< bool >( Button::PROPERTY_DIMMED ) );
  checkBoxButton.SetProperty( Button::PROPERTY_DIMMED, true );
  DALI_TEST_CHECK( true == checkBoxButton.IsDimmed() );
  DALI_TEST_CHECK( true == checkBoxButton.GetProperty< bool >( Button::PROPERTY_DIMMED ) );

  pushButton.SetProperty( Button::PROPERTY_DIMMED, false );
  DALI_TEST_CHECK( false == pushButton.IsDimmed() );
  DALI_TEST_CHECK( false == pushButton.GetProperty< bool >( Button::PROPERTY_DIMMED ) );
  pushButton.SetProperty( Button::PROPERTY_DIMMED, true );
  DALI_TEST_CHECK( true == pushButton.IsDimmed() );
  DALI_TEST_CHECK( true == pushButton.GetProperty< bool >( Button::PROPERTY_DIMMED ) );
}

static void UtcDaliButtonSetGetDimmed()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonSetGetDimmed");

  CheckBoxButton checkBoxButton = CheckBoxButton::New();
  PushButton pushButton = PushButton::New();

  checkBoxButton.SetDimmed( true );
  pushButton.SetDimmed( true );

  DALI_TEST_CHECK( checkBoxButton.IsDimmed() );
  DALI_TEST_CHECK( pushButton.IsDimmed() );

  checkBoxButton.SetDimmed( false );
  pushButton.SetDimmed( false );

  DALI_TEST_CHECK( !checkBoxButton.IsDimmed() );
  DALI_TEST_CHECK( !pushButton.IsDimmed() );

  checkBoxButton.SetDimmed( true );
  pushButton.SetDimmed( true );

  DALI_TEST_CHECK( checkBoxButton.IsDimmed() );
  DALI_TEST_CHECK( pushButton.IsDimmed() );

  checkBoxButton.SetDimmed( false );
  pushButton.SetDimmed( false );

  DALI_TEST_CHECK( !checkBoxButton.IsDimmed() );
  DALI_TEST_CHECK( !pushButton.IsDimmed() );
}

static void UtcDaliButtonSize()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonSize");

  // Creates 100x50 images.
  ImageActor image01 = CreateSolidColorActor( Color::RED );
  image01.SetSize( 100, 50 );

  CheckBoxButton checkBoxButton;
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
}

static void UtcDaliButtonClicked()
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
  application.GetCore().SendEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.GetCore().SendEvent( event );

  DALI_TEST_CHECK( gButtonClicked );

  // Test2. Touch point down and up outside the button.

  gButtonClicked = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.GetCore().SendEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.GetCore().SendEvent( event );

  DALI_TEST_CHECK( !gButtonClicked );

  // Test3. Touch point down inside and up outside the button.

  gButtonClicked = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.GetCore().SendEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointLeave );
  application.GetCore().SendEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.GetCore().SendEvent( event );

  DALI_TEST_CHECK( !gButtonClicked );

  // Test4. Touch point down outside and up inside the button.

  gButtonClicked = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.GetCore().SendEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointEnter );
  application.GetCore().SendEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.GetCore().SendEvent( event );

  DALI_TEST_CHECK( !gButtonClicked );
}

static bool gClickedCallBackCalled;

static bool TestClickedCallback(Button button)
{
  gClickedCallBackCalled = true;
  return true;
}

static void UtcDaliButtonConnectSignal()
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
  application.GetCore().SendEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.GetCore().SendEvent( event );

  DALI_TEST_CHECK( gClickedCallBackCalled == true );

  gClickedCallBackCalled = false;
  pushButton.ClickedSignal().Disconnect(TestClickedCallback);

  // simulate another touch event
  application.GetCore().SendEvent( event );

  DALI_TEST_CHECK( gClickedCallBackCalled == false );
}

static void UtcDaliButtonSetGetAnimationTime()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonSetGetAnimationTime");

  CheckBoxButton checkBoxButton = CheckBoxButton::New();
  PushButton pushButton = PushButton::New();

  checkBoxButton.SetAnimationTime( ANIMATION_TIME );
  pushButton.SetAnimationTime( ANIMATION_TIME );

  DALI_TEST_EQUALS( checkBoxButton.GetAnimationTime(), ANIMATION_TIME, TEST_LOCATION );
  DALI_TEST_EQUALS( pushButton.GetAnimationTime(), ANIMATION_TIME, TEST_LOCATION );

}
