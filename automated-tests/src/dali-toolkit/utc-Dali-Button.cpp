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
static bool gIsCalledButtonCallback = false;

static bool ButtonCallback( Button button )
{
  gIsCalledButtonCallback = true;
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

int UtcDaliButtonConstructorP(void)
{
  TestApplication application;

  Button button;

  DALI_TEST_CHECK( !button );
  END_TEST;
}

int UtcDaliButtonCopyConstructorP(void)
{
  TestApplication application;

  // Initialize an object, ref count == 1
  Button button = PushButton::New();

  Button copy( button );
  DALI_TEST_CHECK( copy );
  END_TEST;
}

int UtcDaliButtonAssignmentOperatorP(void)
{
  TestApplication application;

  Button button = PushButton::New();

  Button copy( button );
  DALI_TEST_CHECK( copy );

  DALI_TEST_CHECK( button == copy );
  END_TEST;
}

int UtcDaliButtonDownCastP(void)
{
  TestApplication application;

  Button button = PushButton::New();

  BaseHandle object(button);

  Button button2 = Button::DownCast( object );
  DALI_TEST_CHECK(button2);

  Button button3 = DownCast< Button >(object);
  DALI_TEST_CHECK(button3);
  END_TEST;
}

int UtcDaliButtonDownCastN(void)
{
  TestApplication application;

  BaseHandle unInitializedObject;

  Button button1 = Button::DownCast( unInitializedObject );
  DALI_TEST_CHECK( !button1 );

  Button button2 = DownCast< Button >( unInitializedObject );
  DALI_TEST_CHECK( !button2 );
  END_TEST;
}

int UtcDaliButtonSetDisabledP(void)
{
  ToolkitTestApplication application;

  Button button = PushButton::New();

  button.SetDisabled( true );

  DALI_TEST_CHECK( button.IsDisabled() );

  button.SetDisabled( false );

  DALI_TEST_CHECK( !button.IsDisabled() );

  button.SetDisabled( true );

  DALI_TEST_CHECK( button.IsDisabled() );

  button.SetDisabled( false );

  DALI_TEST_CHECK( !button.IsDisabled() );
  END_TEST;
}

int UtcDaliButtonIsDisabledP(void)
{
  ToolkitTestApplication application;

  Button button = PushButton::New();

  button.SetDisabled( true );

  DALI_TEST_CHECK( button.IsDisabled() );

  button.SetDisabled( false );

  DALI_TEST_CHECK( !button.IsDisabled() );
  END_TEST;
}

int UtcDaliButtonSetAutoRepeatingP(void)
{
  ToolkitTestApplication application;

  Button button = PushButton::New();

  button.SetAutoRepeating( true );

  DALI_TEST_CHECK( button.IsAutoRepeating() );

  button.SetAutoRepeating( false );

  DALI_TEST_CHECK( !button.IsAutoRepeating() );

  button.SetAutoRepeating( true );

  DALI_TEST_CHECK( button.IsAutoRepeating() );

  button.SetAutoRepeating( false );

  DALI_TEST_CHECK( !button.IsAutoRepeating() );
  END_TEST;
}

int UtcDaliButtonIsAutoRepeatingP(void)
{
  ToolkitTestApplication application;

  Button button = PushButton::New();

  button.SetAutoRepeating( true );

  DALI_TEST_CHECK( button.IsAutoRepeating() );

  button.SetAutoRepeating( false );

  DALI_TEST_CHECK( !button.IsAutoRepeating() );
  END_TEST;
}

int UtcDaliButtonSetInitialAutoRepeatingDelayP(void)
{
  ToolkitTestApplication application;

  Button button = PushButton::New();

  button.SetInitialAutoRepeatingDelay( 0.5f );

  DALI_TEST_EQUALS( button.GetInitialAutoRepeatingDelay(), 0.5f, TEST_LOCATION );

  button.SetInitialAutoRepeatingDelay( 0.2f );

  DALI_TEST_EQUALS( button.GetInitialAutoRepeatingDelay(), 0.2f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliButtonSetNextAutoRepeatingDelayP(void)
{
  ToolkitTestApplication application;

  Button button = PushButton::New();

  button.SetNextAutoRepeatingDelay( 0.5f );

  DALI_TEST_EQUALS( button.GetNextAutoRepeatingDelay(), 0.5f, TEST_LOCATION );

  button.SetNextAutoRepeatingDelay( 0.2f );

  DALI_TEST_EQUALS( button.GetNextAutoRepeatingDelay(), 0.2f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliButtonSetTogglableButtonP(void)
{
  ToolkitTestApplication application;

  Button button = PushButton::New();

  button.SetTogglableButton( true );

  DALI_TEST_CHECK( button.IsTogglableButton() );

  button.SetTogglableButton( false );

  DALI_TEST_CHECK( !button.IsTogglableButton() );
  END_TEST;
}

int UtcDaliButtonSetSelectedP(void)
{
  ToolkitTestApplication application;

  Button button = PushButton::New();
  button.SetTogglableButton( true );

  button.SetSelected( true );

  DALI_TEST_CHECK( button.IsSelected() );

  button.SetSelected( false );

  DALI_TEST_CHECK( !button.IsSelected() );
  END_TEST;
}

int UtcDaliButtonSetAnimationTimeP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonSetAnimationTimeP");

  Button button = PushButton::New();

  button.SetAnimationTime( ANIMATION_TIME );

  DALI_TEST_EQUALS( button.GetAnimationTime(), ANIMATION_TIME, TEST_LOCATION );
  END_TEST;
}

int UtcDaliButtonSetLabelStringP(void)
{
  ToolkitTestApplication application;

  Button button = PushButton::New();

  button.SetLabel( "Button Label" );

  DALI_TEST_CHECK( button.GetLabel() );
  END_TEST;
}

int UtcDaliButtonSetLabelActorP(void)
{
  ToolkitTestApplication application;

  Button button = PushButton::New();

  TextLabel textLabel = TextLabel::New( "Button Label" );
  button.SetLabel( textLabel );

  DALI_TEST_CHECK( button.GetLabel() );
  END_TEST;
}

int UtcDaliButtonPressedSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonPressedSignalP");

  Button button = PushButton::New();
  button.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  button.SetParentOrigin( ParentOrigin::TOP_LEFT );
  button.SetPosition( 240, 400 );
  button.SetSize( 100, 100 );

  Stage::GetCurrent().Add( button );

  application.SendNotification();
  application.Render();

  // connect to its touch signal
  button.PressedSignal().Connect( &ButtonCallback );
  button.ReleasedSignal().Connect( &ButtonCallback );

  Dali::Integration::TouchEvent event;

  // Test1. Touch point down and up inside the button.

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gIsCalledButtonCallback );

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gIsCalledButtonCallback );

  // Test2. Touch point down and up outside the button.

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gIsCalledButtonCallback );

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gIsCalledButtonCallback );

  // Test3. Touch point down inside and up outside the button.

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gIsCalledButtonCallback );

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointLeave );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gIsCalledButtonCallback );

  // Test4. Touch point down outside and up inside the button.

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gIsCalledButtonCallback );

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointEnter );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gIsCalledButtonCallback );
  END_TEST;
}

int UtcDaliButtonClickedSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonClickedSignalP");

  Button button = PushButton::New();
  button.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  button.SetParentOrigin( ParentOrigin::TOP_LEFT );
  button.SetPosition( 240, 400 );
  button.SetSize( 100, 100 );

  Stage::GetCurrent().Add( button );

  application.SendNotification();
  application.Render();

  // connect to its touch signal
  button.ClickedSignal().Connect( &ButtonCallback );

  Dali::Integration::TouchEvent event;

  // Test1. Touch point down and up inside the button.

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gIsCalledButtonCallback );

  // Test2. Touch point down and up outside the button.

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gIsCalledButtonCallback );

  // Test3. Touch point down inside and up outside the button.

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointLeave );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gIsCalledButtonCallback );

  // Test4. Touch point down outside and up inside the button.

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointEnter );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gIsCalledButtonCallback );
  END_TEST;
}

int UtcDaliButtonStateChangedSignalP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonStateChangedSignalP");

  Button button = PushButton::New();
  button.SetTogglableButton( true );

  Stage::GetCurrent().Add( button );

  application.SendNotification();
  application.Render();

  // connect to its signal
  button.StateChangedSignal().Connect( &ButtonCallback );

  gIsCalledButtonCallback = false;
  button.SetSelected( true );

  DALI_TEST_CHECK( gIsCalledButtonCallback );

  gIsCalledButtonCallback = false;
  button.SetSelected( false );

  DALI_TEST_CHECK( gIsCalledButtonCallback );
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

int UtcDaliButtonSize(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonSize");

  ImageActor image01 = ImageActor::New(CreateBufferImage());
  image01.SetSize( 100, 50 );

  PushButton pushButton;

  Vector3 size;

  // Test1 Size is set through Actor API

  // First an image is set, then SetSize is called.
  pushButton = PushButton::New();
  Stage::GetCurrent().Add( pushButton );

  pushButton.SetBackgroundImage( image01 );
  pushButton.SetSize( 10.f, 10.f );

  application.SendNotification();
  application.Render();

  size = pushButton.GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 10.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 10.f, TEST_LOCATION );
  END_TEST;
}
