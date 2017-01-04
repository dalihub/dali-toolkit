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
#include "dali-toolkit-test-utils/toolkit-timer.h"

#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/integration-api/events/touch-event-integ.h>

#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/text-visual-properties.h>

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
static const char* TEST_IMAGE_ONE = TEST_RESOURCE_DIR "/gallery-small-1.jpg";

static bool gIsCalledButtonCallback = false;

const int RENDER_FRAME_INTERVAL = 16;

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

int UtcDaliButtonSetDisabledWithDifferentStates01P(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliButtonSetDisabledWithDifferentStates01P\n");

  Button button = PushButton::New();

  bool SELECTED = true;

  button.SetProperty( Button::Property::TOGGLABLE, true);
  button.SetProperty( Button::Property::SELECTED, SELECTED );

  button.SetProperty( Button::Property::DISABLED, true);

  tet_infoline("Set button to SELECTED = false whilst disabled, should not change to false\n");
  button.SetProperty( Button::Property::SELECTED, !SELECTED );

  bool isSelected = button.GetProperty<bool>( Button::Property::SELECTED ) ;

  DALI_TEST_EQUALS( isSelected, SELECTED , TEST_LOCATION );

  END_TEST;
}

int UtcDaliButtonSetDisabledWithDifferentStates02P(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliButtonSetDisabledWithDifferentStates02\n");

  Button button = PushButton::New();

  bool SELECTED = true;

  button.SetProperty( Button::Property::TOGGLABLE, true );
  button.SetProperty( Button::Property::SELECTED, SELECTED );
  button.SetProperty( Button::Property::DISABLED, true );

  bool isSelected =  button.GetProperty<bool>( Button::Property::SELECTED );
  DALI_TEST_EQUALS( isSelected, SELECTED , TEST_LOCATION );
  tet_infoline("Set button to DISABLED = false whilst disabled and then set to unselected\n");

  button.SetProperty( Button::Property::DISABLED, false );
  button.SetProperty( Button::Property::SELECTED, !SELECTED );

  isSelected = button.GetProperty<bool>( Button::Property::SELECTED );
  DALI_TEST_EQUALS( isSelected, !SELECTED , TEST_LOCATION );

  END_TEST;
}

int UtcDaliButtonPropertyGetLabelAlignment(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonPropertyGetLabelAlignment\n");

  Button button = PushButton::New();
  button.SetProperty( Toolkit::DevelButton::Property::LABEL_RELATIVE_ALIGNMENT, "END"  );
  DALI_TEST_EQUALS( button.GetProperty<std::string>( Toolkit::DevelButton::Property::LABEL_RELATIVE_ALIGNMENT ), "END", TEST_LOCATION );

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

int UtcDaliButtonAutoRepeatingP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonPressedSignalP  Setup Autorepeating and check multiple clicked signals received\n");

  const float AUTO_REPEATING_DELAY = 0.15f;

  Button button = PushButton::New();
  button.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  button.SetParentOrigin( ParentOrigin::TOP_LEFT );
  button.SetPosition( 240, 400 );
  button.SetSize( 100, 100 );
  Stage::GetCurrent().Add( button );

  application.SendNotification();
  application.Render();

  button.SetProperty( Toolkit::Button::Property::AUTO_REPEATING, true  );
  button.SetProperty( Toolkit::Button::Property::INITIAL_AUTO_REPEATING_DELAY, AUTO_REPEATING_DELAY );
  // connect to its touch signal
  ConnectionTracker* testTracker = new ConnectionTracker();
  button.PressedSignal().Connect( &ButtonCallback );
  button.ClickedSignal().Connect( &ButtonCallback );
  bool clickedSignal = false;
  bool pressedSignal = false;
  button.ConnectSignal( testTracker, "pressed", CallbackFunctor(&pressedSignal) );
  button.ConnectSignal( testTracker, "clicked", CallbackFunctor(&clickedSignal) );

  Dali::Integration::TouchEvent event;

  // Touch point down and up inside the button.

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownInside() );
  application.ProcessEvent( event );

  DALI_TEST_EQUALS( gIsCalledButtonCallback, true, TEST_LOCATION );
  DALI_TEST_EQUALS( pressedSignal, true, TEST_LOCATION );
  tet_infoline("Consume first clicked signal then wait\n");

  gIsCalledButtonCallback = false;
  Dali::Timer timer = Timer::New( AUTO_REPEATING_DELAY );
  timer.MockEmitSignal();
  application.Wait( AUTO_REPEATING_DELAY*2 );
  DALI_TEST_EQUALS( clickedSignal, true, TEST_LOCATION );
  tet_infoline("Check gIsCalledButtonCallback was called again after last consumption of it.\n");

  DALI_TEST_EQUALS( gIsCalledButtonCallback, true, TEST_LOCATION );

  gIsCalledButtonCallback = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpInside() );
  application.ProcessEvent( event );

  DALI_TEST_EQUALS( gIsCalledButtonCallback, true, TEST_LOCATION );

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

  button.SetProperty( Button::Property::NEXT_AUTO_REPEATING_DELAY, 0.2f );

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
  button.SetProperty( Toolkit::Button::Property::LABEL,
                      Property::Map().Add( Toolkit::Visual::Property::TYPE, Toolkit::DevelVisual::TEXT )
                                     .Add( Toolkit::TextVisual::Property::POINT_SIZE, 15.0f )
                     );

  button.SetLabelText( "Button Label" );

  DALI_TEST_EQUALS( button.GetLabelText(), "Button Label", TEST_LOCATION );
  END_TEST;
}

int UtcDaliButtonSetLabelPropertyP(void)
{
  ToolkitTestApplication application;

  const std::string TEST_LABEL1 = "test label one";
  const std::string TEST_LABEL2 = "test label two";

  Button button = PushButton::New();

  button.SetProperty( Toolkit::Button::Property::LABEL,
                        Property::Map().Add( Toolkit::Visual::Property::TYPE, Toolkit::DevelVisual::TEXT )
                                       .Add( Toolkit::TextVisual::Property::POINT_SIZE, 15.0f )
                     );

  button.SetProperty( Button::Property::LABEL_TEXT, TEST_LABEL1 );

  std::string labelText = button.GetProperty<std::string>( Button::Property::LABEL_TEXT );

  DALI_TEST_EQUALS( labelText, TEST_LABEL1,  TEST_LOCATION );

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE,  Toolkit::DevelVisual::TEXT );
  propertyMap.Insert( Toolkit::TextVisual::Property::TEXT,  TEST_LABEL2 );
  propertyMap.Insert( Toolkit::TextVisual::Property::TEXT_COLOR, Color::BLUE );
  propertyMap.Insert( Toolkit::TextVisual::Property::POINT_SIZE, 15.0f );
  button.SetProperty( Button::Property::LABEL, propertyMap );

  labelText = button.GetProperty<std::string>( Button::Property::LABEL_TEXT );

  DALI_TEST_EQUALS( labelText, TEST_LABEL2,  TEST_LOCATION );

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

  pushButton.SetUnselectedImage( "Image.jpg" );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( pushButton );

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

  pushButton.SetSelectedImage( "Image.jpg" );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( pushButton );

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

int UtcDaliButtonSetLabeActorlP(void)
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
    ResourceImage image1 = ResourceImage::New( TEST_IMAGE_ONE );
    button.SetButtonImage( image1 );

    Property::Value value = button.GetProperty(Button::Property::UNSELECTED_STATE_IMAGE );
    DALI_TEST_CHECK( value.Get<std::string>() == TEST_IMAGE_ONE );
  }
  catch(...)
  {
    DALI_TEST_CHECK( false );
  }

  std::string imageUrl;

  Dali::Actor actor = button.GetButtonImage();

  Toolkit::ImageView imageView = Toolkit::ImageView ::DownCast( actor );

  tet_infoline(" UtcDaliButtonSetButtonImageP Ensure an ImageView is returned\n");
  DALI_TEST_CHECK ( imageView )

  END_TEST;
}

int UtcDaliButtonSetButtonImageN(void)
{
  ToolkitTestApplication application;

  PushButton button;

  try
  {
    ResourceImage image1 = ResourceImage::New( TEST_IMAGE_ONE );
    button.SetButtonImage( image1 );

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
  ResourceImage image1 = ResourceImage::New( TEST_IMAGE_ONE );

  try
  {
    button.SetSelectedImage( image1 );
    Property::Value value = button.GetProperty( Button::Property::SELECTED_STATE_IMAGE );
    DALI_TEST_CHECK( value.Get<std::string>() == TEST_IMAGE_ONE );
  }
  catch(...)
  {
    DALI_TEST_CHECK( false );
  }

  std::string imageUrl;

  Dali::Actor actor = button.GetSelectedImage();

  Toolkit::ImageView imageView = Toolkit::ImageView::DownCast( actor );

  tet_infoline(" UtcDaliButtonSetSelectedImageWithImageP Ensure an ImageView is returned\n");

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

int UtcDaliButtonSetImagesWithDeprecatedProperties(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonSetImagesWithDeprecatedProperties");

  PushButton pushButton = PushButton::New();

  Stage::GetCurrent().Add( pushButton );

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);

  DALI_TEST_EQUALS( pushButton.GetRendererCount(), 0, TEST_LOCATION );

  pushButton.SetProperty( Toolkit::Button::Property::UNSELECTED_STATE_IMAGE, propertyMap );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( pushButton.GetRendererCount(), 1, TEST_LOCATION );

  tet_infoline(" Set state to selected and provide SELECTED visual");
  pushButton.SetProperty( Toolkit::Button::Property::SELECTED_STATE_IMAGE, propertyMap );
  pushButton.SetProperty( Toolkit::Button::Property::SELECTED, true );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( pushButton.GetRendererCount(), 1, TEST_LOCATION );

  tet_infoline(" Set state to selected, disabled and provide DISABLED_STATE_IMAGE visual");
  pushButton.SetProperty( Toolkit::Button::Property::SELECTED, false );
  pushButton.SetProperty( Toolkit::Button::Property::DISABLED, true );
  pushButton.SetProperty( Toolkit::Button::Property::DISABLED_STATE_IMAGE, propertyMap );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( pushButton.GetRendererCount(), 1, TEST_LOCATION );

END_TEST;
}

int UtcDaliButtonSetGetDepreciatedPropertiesWithURL(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliButtonSetGetDepreciatedPropertiesWithURL");

  PushButton button = PushButton::New();
  Stage::GetCurrent().Add( button );

  tet_infoline(" Set state to selected, disabled and provide DISABLED_STATE_IMAGE visual");
  button.SetProperty( Toolkit::Button::Property::DISABLED, true );
  button.SetProperty( Toolkit::Button::Property::DISABLED_STATE_IMAGE, TEST_IMAGE_ONE );

  Property::Value value = button.GetProperty(Button::Property::DISABLED_STATE_IMAGE );
  DALI_TEST_EQUALS( value.Get<std::string>(),  TEST_IMAGE_ONE, TEST_LOCATION );

END_TEST;
}
