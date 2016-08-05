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

struct CallbackFunctor
{
  CallbackFunctor(bool* callbackFlag)
  : mCallbackFlag( callbackFlag )
  {
  }

  void operator()()
  {
    *mCallbackFlag = true;
  }
  bool* mCallbackFlag;
};

Dali::Integration::Point GetPointDownInside()
{
  Dali::Integration::Point point;
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 240, 400 ) );
  return point;
}

Dali::Integration::Point GetPointUpInside()
{
  Dali::Integration::Point point;
  point.SetState( PointState::UP );
  point.SetScreenPosition( Vector2( 240, 400 ) );
  return point;
}

Dali::Integration::Point GetPointLeave()
{
  Dali::Integration::Point point;
  point.SetState( PointState::LEAVE );
  point.SetScreenPosition( Vector2( 240, 400 ) );
  return point;
}

Dali::Integration::Point GetPointEnter()
{
  Dali::Integration::Point point;
  point.SetState( PointState::MOTION );
  point.SetScreenPosition( Vector2( 240, 400 ) );
  return point;
}

Dali::Integration::Point GetPointDownOutside()
{
  Dali::Integration::Point point;
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 10, 10 ) );
  return point;
}

Dali::Integration::Point GetPointUpOutside()
{
  Dali::Integration::Point point;
  point.SetState( PointState::UP );
  point.SetScreenPosition( Vector2( 10, 10 ) );
  return point;
}


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

  button.SetLabelText( "Button Label" );

  DALI_TEST_EQUALS( button.GetLabelText(), "Button Label", TEST_LOCATION );
  END_TEST;
}

int UtcDaliButtonSetLabelActorP(void)
{
  ToolkitTestApplication application;

  Button button = PushButton::New();

  button.SetLabelText( "Button Label" );

  DALI_TEST_EQUALS( button.GetLabelText(), "Button Label", TEST_LOCATION );
  END_TEST;
}

int UtcDaliButtonSetUnselectedImageP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonSetUnselectedImageP");

  PushButton pushButton = PushButton::New();
  Stage::GetCurrent().Add( pushButton );

  application.SendNotification();
  application.Render();

  pushButton.SetSize( Vector2( 20.0f, 20.0f ) );
  pushButton.SetUnselectedImage( "Image.jpg" );

  application.SendNotification();
  application.Render();

  Vector3 size = pushButton.GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 20.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 20.f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliButtonSetSelectedImageP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonSetButtonImage");

  PushButton pushButton = PushButton::New();
  Stage::GetCurrent().Add( pushButton );

  application.SendNotification();
  application.Render();

  pushButton.SetSize( Vector2( 20.0f, 20.0f ) );
  pushButton.SetSelectedImage( "Image.jpg" );

  application.SendNotification();
  application.Render();

  Vector3 size = pushButton.GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 20.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 20.f, TEST_LOCATION );

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
  ConnectionTracker* testTracker = new ConnectionTracker();
  button.PressedSignal().Connect( &ButtonCallback );
  button.ReleasedSignal().Connect( &ButtonCallback );
  bool pressedSignal = false;
  bool releasedSignal = false;
  button.ConnectSignal( testTracker, "pressed",   CallbackFunctor(&pressedSignal) );
  button.ConnectSignal( testTracker, "released",  CallbackFunctor(&releasedSignal) );

  Dali::Integration::TouchEvent event;

  // Test1. Touch point down and up inside the button.

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownInside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gIsCalledButtonCallback );
  DALI_TEST_CHECK( pressedSignal );

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpInside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gIsCalledButtonCallback );
  DALI_TEST_CHECK( releasedSignal );

  // Test2. Touch point down and up outside the button.

  pressedSignal = false;
  releasedSignal = false;
  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownOutside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gIsCalledButtonCallback );
  DALI_TEST_CHECK( !pressedSignal );

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpOutside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gIsCalledButtonCallback );
  DALI_TEST_CHECK( !releasedSignal );

  // Test3. Touch point down inside and up outside the button.

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownInside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gIsCalledButtonCallback );

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointLeave() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpOutside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gIsCalledButtonCallback );

  // Test4. Touch point down outside and up inside the button.

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownOutside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gIsCalledButtonCallback );

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointEnter() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpInside() );
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
  bool clickedSignal = false;
  ConnectionTracker* testTracker = new ConnectionTracker();
  button.ConnectSignal( testTracker, "clicked",   CallbackFunctor(&clickedSignal) );

  Dali::Integration::TouchEvent event;

  // Test1. Touch point down and up inside the button.

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownInside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpInside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gIsCalledButtonCallback );
  DALI_TEST_CHECK( clickedSignal );

  // Test2. Touch point down and up outside the button.

  gIsCalledButtonCallback = false;
  clickedSignal = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownOutside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpOutside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gIsCalledButtonCallback );
  DALI_TEST_CHECK( !clickedSignal );

  // Test3. Touch point down inside and up outside the button.

  gIsCalledButtonCallback = false;
  clickedSignal = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownInside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointLeave() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpOutside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gIsCalledButtonCallback );
  DALI_TEST_CHECK( !clickedSignal );

  // Test4. Touch point down outside and up inside the button.

  gIsCalledButtonCallback = false;
  clickedSignal = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownOutside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointEnter() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpInside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gIsCalledButtonCallback );
  DALI_TEST_CHECK( !clickedSignal );
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
  bool stateChangedSignal = false;
  ConnectionTracker* testTracker = new ConnectionTracker();
  button.ConnectSignal( testTracker, "stateChanged",   CallbackFunctor(&stateChangedSignal) );

  gIsCalledButtonCallback = false;
  button.SetSelected( true );

  DALI_TEST_CHECK( gIsCalledButtonCallback );
  DALI_TEST_CHECK( stateChangedSignal );

  gIsCalledButtonCallback = false;
  stateChangedSignal = false;

  button.SetSelected( false );

  DALI_TEST_CHECK( gIsCalledButtonCallback );
  DALI_TEST_CHECK( stateChangedSignal );
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

  // First an image is set, then SetSize is called.
  PushButton pushButton = PushButton::New();
  Stage::GetCurrent().Add( pushButton );

  pushButton.SetBackgroundImage( "Image.jpg" );
  pushButton.SetSize( 10.f, 10.f );

  application.SendNotification();
  application.Render();

  Vector3 size = pushButton.GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 10.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 10.f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliButtonSetSelectedBackgroundImageP(void)
{
  ToolkitTestApplication application;

  PushButton button = PushButton::New();
  Stage::GetCurrent().Add( button );

  try
  {
    button.SetSelectedBackgroundImage( "TestImage.jpg");
    DALI_TEST_CHECK( true );
  }
  catch(...)
  {
    DALI_TEST_CHECK( false );
  }

  END_TEST;
}

int UtcDaliButtonSetSelectedBackgroundImageN(void)
{
  ToolkitTestApplication application;

  PushButton button;

  try
  {
    button.SetSelectedBackgroundImage( "TestImage.jpg");
    DALI_TEST_CHECK( false );
  }
  catch(...)
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliButtonSetDisabledImageP(void)
{
  ToolkitTestApplication application;

  PushButton button = PushButton::New();
  Stage::GetCurrent().Add( button );

  try
  {
    button.SetDisabledImage( "TestImage.jpg");
    DALI_TEST_CHECK( true );
  }
  catch(...)
  {
    DALI_TEST_CHECK( false );
  }

  END_TEST;
}

int UtcDaliButtonSetDisabledImageN(void)
{
  ToolkitTestApplication application;

  PushButton button;

  try
  {
    button.SetDisabledImage( "TestImage.jpg");
    DALI_TEST_CHECK( false );
  }
  catch(...)
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliButtonSetDisabledSelectedImageP(void)
{
  ToolkitTestApplication application;

  PushButton button = PushButton::New();
  Stage::GetCurrent().Add( button );

  try
  {
    button.SetDisabledSelectedImage( "TestImage.jpg");
    DALI_TEST_CHECK( true );
  }
  catch(...)
  {
    DALI_TEST_CHECK( false );
  }

  END_TEST;
}

int UtcDaliButtonSetDisabledSelectedImageN(void)
{
  ToolkitTestApplication application;

  PushButton button;

  try
  {
    button.SetDisabledSelectedImage( "TestImage.jpg");
    DALI_TEST_CHECK( false );
  }
  catch(...)
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliButtonSetLabelP(void)
{
  ToolkitTestApplication application;

  PushButton button = PushButton::New();
  Stage::GetCurrent().Add( button );

  try
  {
    button.SetLabel( TextLabel::New("Hello") );
    DALI_TEST_CHECK( true );
  }
  catch(...)
  {
    DALI_TEST_CHECK( false );
  }

  END_TEST;
}

int UtcDaliButtonSetLabelN(void)
{
  ToolkitTestApplication application;

  PushButton button;

  try
  {
    button.SetLabel( TextLabel::New("Hello") );
    DALI_TEST_CHECK( false );
  }
  catch(...)
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliButtonSetButtonImageP(void)
{
  ToolkitTestApplication application;

  PushButton button = PushButton::New();
  Stage::GetCurrent().Add( button );

  try
  {
    button.SetButtonImage( CreateBufferImage( 10, 10, Color::WHITE ) );
    DALI_TEST_CHECK( ImageView::DownCast( button.GetButtonImage() ) );
  }
  catch(...)
  {
    DALI_TEST_CHECK( false );
  }

  END_TEST;
}

int UtcDaliButtonSetButtonImageN(void)
{
  ToolkitTestApplication application;

  PushButton button;

  try
  {
    button.SetButtonImage( CreateBufferImage( 10, 10, Color::WHITE ) );
    DALI_TEST_CHECK( false );
  }
  catch(...)
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliButtonSetSelectedImageWithImageP(void)
{
  ToolkitTestApplication application;

  PushButton button = PushButton::New();
  Stage::GetCurrent().Add( button );

  try
  {
    button.SetSelectedImage( CreateBufferImage( 10, 10, Color::WHITE ) );
    DALI_TEST_CHECK( ImageView::DownCast( button.GetSelectedImage() ) );
  }
  catch(...)
  {
    DALI_TEST_CHECK( false );
  }

  END_TEST;
}

int UtcDaliButtonSetSelectedImageWithImageN(void)
{
  ToolkitTestApplication application;

  PushButton button;

  try
  {
    button.SetSelectedImage( CreateBufferImage( 10, 10, Color::WHITE ) );
    DALI_TEST_CHECK( false );
  }
  catch(...)
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliButtonSetSelectedColorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonSetSelectedColorP");

  PushButton pushButton = PushButton::New();
  Stage::GetCurrent().Add( pushButton );

  application.SendNotification();
  application.Render();

  const Vector4 SET_COLOR = Color::BLUE;

  pushButton.SetSize( Vector2( 20.0f, 20.0f ) );
  pushButton.SetProperty( Button::Property::SELECTED_COLOR, SET_COLOR );

  application.SendNotification();
  application.Render();

  Vector4 color = pushButton.GetProperty<Vector4>( Button::Property::SELECTED_COLOR );

  DALI_TEST_EQUALS( color, SET_COLOR, TEST_LOCATION );

  END_TEST;
}

int UtcDaliButtonSetUnSelectedColorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonSetUnSelectedColorP");

  PushButton pushButton = PushButton::New();
  Stage::GetCurrent().Add( pushButton );

  application.SendNotification();
  application.Render();

  const Vector4 SET_COLOR = Color::BLUE;

  pushButton.SetSize( Vector2( 20.0f, 20.0f ) );
  pushButton.SetProperty( Button::Property::UNSELECTED_COLOR, SET_COLOR );

  application.SendNotification();
  application.Render();

  Vector4 color = pushButton.GetProperty<Vector4>( Button::Property::UNSELECTED_COLOR );

  DALI_TEST_EQUALS( color, SET_COLOR, TEST_LOCATION );

  END_TEST;
}

int UtcDaliButtonResetSelectedColorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonSetSelectedColorP");

  PushButton pushButton = PushButton::New();
  Stage::GetCurrent().Add( pushButton );

  application.SendNotification();
  application.Render();

  const Vector4 FIRST_COLOR = Color::BLUE;
  const Vector4 SECOND_COLOR = Color::BLUE;

  pushButton.SetSize( Vector2( 20.0f, 20.0f ) );
  pushButton.SetProperty( Button::Property::SELECTED_COLOR, FIRST_COLOR );

  application.SendNotification();
  application.Render();

  Vector4 color = pushButton.GetProperty<Vector4>( Button::Property::SELECTED_COLOR );

  DALI_TEST_EQUALS( color, FIRST_COLOR, TEST_LOCATION );

  pushButton.SetProperty( Button::Property::SELECTED_COLOR, SECOND_COLOR );

  application.SendNotification();
  application.Render();

  color = pushButton.GetProperty<Vector4>( Button::Property::SELECTED_COLOR );

  DALI_TEST_EQUALS( color, SECOND_COLOR, TEST_LOCATION );

  END_TEST;
}
