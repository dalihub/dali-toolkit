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
#include <dali/integration-api/events/touch-event-integ.h>


using namespace Dali;
using namespace Dali::Toolkit;

void dali_radio_button_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_radio_button_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

}

int UtcDaliRadioButtonNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliRadioButtonNew");

  // Create the Slider actor
  RadioButton radioButton;

  DALI_TEST_CHECK( !radioButton );

  radioButton = RadioButton::New();

  DALI_TEST_CHECK( radioButton );

  RadioButton radioButton2(radioButton);

  DALI_TEST_CHECK( radioButton2 == radioButton );

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    RadioButton radioButton = RadioButton::New();
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
  END_TEST;
}

int UtcDaliRadioButtonDestructor(void)
{
  ToolkitTestApplication application;

  RadioButton* radioButton = new RadioButton();
  delete radioButton;

  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliRadioButtonDownCast(void)
{
  ToolkitTestApplication application;

  Handle handle = RadioButton::New();

  RadioButton radioButton = RadioButton::DownCast( handle );

  DALI_TEST_CHECK( radioButton == handle );
  END_TEST;
}

int UtcDaliRadioButtonLabelActor(void)
{
  // TODO
  END_TEST;
}

int UtcDaliRadioButtonSelected(void)
{
  ToolkitTestApplication application;

  RadioButton radioButton = RadioButton::New();

  // Default selected
  DALI_TEST_CHECK( radioButton.IsSelected() == false );

  // False
  radioButton.SetSelected( false );
  DALI_TEST_CHECK( radioButton.IsSelected() == false );

  // True
  radioButton.SetSelected( true );
  DALI_TEST_CHECK( radioButton.IsSelected() == true );

  // False
  radioButton.SetSelected( false );
  DALI_TEST_CHECK( radioButton.IsSelected() == false );

  END_TEST;
}

int UtcDaliRadioButtonSelectedProperty(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliRadioButtonSelectedProperty");

  // Create the RadioButton actor
  RadioButton radioButton = RadioButton::New();
  Stage::GetCurrent().Add( radioButton );
  radioButton.SetParentOrigin(ParentOrigin::TOP_LEFT);
  radioButton.SetAnchorPoint(ParentOrigin::TOP_LEFT);
  radioButton.SetPosition( 0.0f, 0.0f );

  // Default selected
  DALI_TEST_CHECK( radioButton.GetProperty<bool>( Button::PROPERTY_SELECTED ) == false );

  // Setting false selected
  radioButton.SetProperty( Button::PROPERTY_SELECTED, false );
  DALI_TEST_CHECK( radioButton.GetProperty<bool>( Button::PROPERTY_SELECTED ) == false );

  // Setting true selected
  radioButton.SetProperty( Button::PROPERTY_SELECTED, true );
  DALI_TEST_CHECK( radioButton.GetProperty<bool>( Button::PROPERTY_SELECTED ) == true );

  // Setting false again
  radioButton.SetProperty( Button::PROPERTY_SELECTED, false );
  DALI_TEST_CHECK( radioButton.GetProperty<bool>( Button::PROPERTY_SELECTED ) == false );

  // Test selecting radio buttons
  RadioButton radioButton2 = RadioButton::New( "label" );
  radioButton2.SetParentOrigin(ParentOrigin::TOP_LEFT);
  radioButton2.SetAnchorPoint(ParentOrigin::TOP_LEFT);
  radioButton2.SetPosition( 0.0f, 0.0f );

  RadioButton radioButton3 = RadioButton::New( "label" );
  radioButton3.SetParentOrigin(ParentOrigin::TOP_LEFT);
  radioButton3.SetAnchorPoint(ParentOrigin::TOP_LEFT);
  radioButton3.SetPosition( 0.0f, 40.0f );

  Actor radioGroup = Actor::New();
  Stage::GetCurrent().Add( radioGroup );
  radioGroup.SetParentOrigin(ParentOrigin::TOP_LEFT);
  radioGroup.SetAnchorPoint(ParentOrigin::TOP_LEFT);
  radioGroup.SetPosition( 0.0f, 0.0f );
  radioGroup.SetSize( 400.0f, 400.0 );

  radioGroup.Add( radioButton2 );
  radioGroup.Add( radioButton3 );

  application.SendNotification();
  application.Render();

  // Simulate touch events
  DALI_TEST_CHECK( radioButton2.GetProperty<bool>( Button::PROPERTY_SELECTED ) == false );
  DALI_TEST_CHECK( radioButton3.GetProperty<bool>( Button::PROPERTY_SELECTED ) == false );

  // Select first radio
  {
    Dali::Integration::TouchEvent event1 = Dali::Integration::TouchEvent();
    Dali::Integration::TouchEvent event2 = Dali::Integration::TouchEvent();

    const Dali::TouchPoint pointDown( 0, TouchPoint::Down, 10.0f, 10.0f );
    const Dali::TouchPoint pointUp( 0, TouchPoint::Up, 10.0f, 10.0f );

    event1.AddPoint( pointDown );
    application.ProcessEvent( event1 );

    event2.AddPoint( pointUp );
    application.ProcessEvent( event2 );

    application.SendNotification();
    application.Render();

    DALI_TEST_CHECK( radioButton2.GetProperty<bool>( Button::PROPERTY_SELECTED ) == true );
    DALI_TEST_CHECK( radioButton3.GetProperty<bool>( Button::PROPERTY_SELECTED ) == false );
  }

  // Select an already selected radio
  {
    Dali::Integration::TouchEvent event1 = Dali::Integration::TouchEvent();
    Dali::Integration::TouchEvent event2 = Dali::Integration::TouchEvent();

    const Dali::TouchPoint pointDown( 0, TouchPoint::Down, 10.0f, 10.0f );
    const Dali::TouchPoint pointUp( 0, TouchPoint::Up, 10.0f, 10.0f );

    event1.AddPoint( pointDown );
    application.ProcessEvent( event1 );

    event2.AddPoint( pointUp );
    application.ProcessEvent( event2 );

    application.SendNotification();
    application.Render();

    DALI_TEST_CHECK( radioButton2.GetProperty<bool>( Button::PROPERTY_SELECTED ) == true );
    DALI_TEST_CHECK( radioButton3.GetProperty<bool>( Button::PROPERTY_SELECTED ) == false );
  }

  // Select second radio
  {
    Dali::Integration::TouchEvent event1 = Dali::Integration::TouchEvent();
    Dali::Integration::TouchEvent event2 = Dali::Integration::TouchEvent();

    const Dali::TouchPoint pointDown( 0, TouchPoint::Down, 10.0f, 50.0f );
    const Dali::TouchPoint pointUp( 0, TouchPoint::Up, 10.0f, 50.0f );

    event1.AddPoint( pointDown );
    application.ProcessEvent( event1 );

    event2.AddPoint( pointUp );
    application.ProcessEvent( event2 );

    application.SendNotification();
    application.Render();

    DALI_TEST_CHECK( radioButton2.GetProperty<bool>( Button::PROPERTY_SELECTED ) == false );
    DALI_TEST_CHECK( radioButton3.GetProperty<bool>( Button::PROPERTY_SELECTED ) == true );
  }

  // Select outside radio group
  {
    Dali::Integration::TouchEvent event1 = Dali::Integration::TouchEvent();
    Dali::Integration::TouchEvent event2 = Dali::Integration::TouchEvent();

    const Dali::TouchPoint pointDown( 0, TouchPoint::Down, 10.0f, 500.0f );
    const Dali::TouchPoint pointUp( 0, TouchPoint::Up, 10.0f, 500.0f );

    event1.AddPoint( pointDown );
    application.ProcessEvent( event1 );

    event2.AddPoint( pointUp );
    application.ProcessEvent( event2 );

    application.SendNotification();
    application.Render();

    DALI_TEST_CHECK( radioButton2.GetProperty<bool>( Button::PROPERTY_SELECTED ) == false );
    DALI_TEST_CHECK( radioButton3.GetProperty<bool>( Button::PROPERTY_SELECTED ) == true );
  }

  END_TEST;
}
