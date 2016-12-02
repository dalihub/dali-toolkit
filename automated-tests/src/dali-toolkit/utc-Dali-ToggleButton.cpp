/*
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

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-toolkit/devel-api/controls/buttons/toggle-button.h>


using namespace Dali;
using namespace Dali::Toolkit;

void dali_toggle_button_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_toggle_button_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
static const char* TEST_IMAGE_ONE   = TEST_RESOURCE_DIR "/icon-delete.png";
static const char* TEST_IMAGE_TWO   = TEST_RESOURCE_DIR "/icon-edit.png";
static const char* TEST_IMAGE_THREE = TEST_RESOURCE_DIR "/popup_tail_down.png";
static const char* TEST_IMAGE_FOUR  = TEST_RESOURCE_DIR "/popup_tail_up.png";

static const Vector2 INSIDE_TOUCH_POINT_POSITON  = Vector2( 240, 400 );
static const Vector3 BUTTON_POSITON_TO_GET_INSIDE_TOUCH_EVENTS  = Vector3( 200, 360, 0 );
static const Size BUTTON_SIZE_TO_GET_INSIDE_TOUCH_EVENTS  = Size( 100, 100 );

static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

Dali::Integration::Point GetPointDownInside()
{
  Dali::Integration::Point point;
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( INSIDE_TOUCH_POINT_POSITON );
  return point;
}

Dali::Integration::Point GetPointUpInside()
{
  Dali::Integration::Point point;
  point.SetState( PointState::UP );
  point.SetScreenPosition( INSIDE_TOUCH_POINT_POSITON );
  return point;
}

}

int UtcDaliToggleButtonConstructorP(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliToggleButtonConstructorP");

  ToggleButton button;
  DALI_TEST_CHECK( !button );
  END_TEST;
}

int UtcDaliToggleButtonCopyConstructorP(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliToggleButtonCopyConstructorP");

  // Initialize an object, ref count == 1
  ToggleButton button = ToggleButton::New();

  ToggleButton copy( button );
  DALI_TEST_CHECK( copy );
  END_TEST;
}

int UtcDaliToggleButtonAssignmentOperatorP(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliToggleButtonAssignmentOperatorP");

  ToggleButton button = ToggleButton::New();

  ToggleButton copy( button );
  DALI_TEST_CHECK( copy );

  DALI_TEST_CHECK( button == copy );
  END_TEST;
}

int UtcDaliToggleButtonNewP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToggleButtonNewP");

  // Create the Slider actor
  ToggleButton toggleButton;

  DALI_TEST_CHECK( !toggleButton );

  toggleButton = ToggleButton::New();

  DALI_TEST_CHECK( toggleButton );

  ToggleButton toggleButton2(toggleButton);

  DALI_TEST_CHECK( toggleButton2 == toggleButton );

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    ToggleButton toggleButton = ToggleButton::New();
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
  END_TEST;
}

int UtcDaliToggleButtonDestructorP(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliToggleButtonDestructorP");

  ToggleButton* toggleButton = new ToggleButton();
  delete toggleButton;

  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliToggleButtonDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliToggleButtonDownCast");

  Handle handle = ToggleButton::New();
  ToggleButton toggleButton = ToggleButton::DownCast( handle );

  DALI_TEST_CHECK( toggleButton == handle );
  END_TEST;
}

int UtcDaliToggleButtonToggleStatesProperty(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliToggleButtonToggleStatesProperty");

  // Create the ToggleButton actor
  ToggleButton toggleButton = ToggleButton::New();
  Stage::GetCurrent().Add( toggleButton );
  toggleButton.SetParentOrigin(ParentOrigin::TOP_LEFT);
  toggleButton.SetAnchorPoint(ParentOrigin::TOP_LEFT);
  toggleButton.SetPosition( 0.0f, 0.0f );

  {// Check empty array
    Property::Array toggleIcons;
    toggleButton.SetProperty( Toolkit::ToggleButton::Property::STATE_VISUALS, toggleIcons );

    application.SendNotification();
    application.Render();

    Property::Array resultIcons;
    resultIcons = toggleButton.GetProperty( Toolkit::ToggleButton::Property::STATE_VISUALS ).Get<Property::Array>();
    DALI_TEST_CHECK( resultIcons.Count() == 0 );
  }

  {// Check non-empty Array
    Property::Array toggleIcons;
    toggleIcons.PushBack( TEST_IMAGE_ONE ); //Icons path
    toggleIcons.PushBack( TEST_IMAGE_TWO );
    toggleIcons.PushBack( TEST_IMAGE_THREE );
    toggleIcons.PushBack( TEST_IMAGE_FOUR );
    toggleButton.SetProperty( Toolkit::ToggleButton::Property::STATE_VISUALS, toggleIcons );

    application.SendNotification();
    application.Render();

    Property::Array resultIcons;
    resultIcons = toggleButton.GetProperty( Toolkit::ToggleButton::Property::STATE_VISUALS ).Get<Property::Array>();

    // Check that the result is the same as
    DALI_TEST_EQUALS( toggleIcons.Count(), resultIcons.Count(), TEST_LOCATION );
    DALI_TEST_CHECK( toggleIcons[0].Get<std::string>() == resultIcons[0].Get<std::string>() );
    DALI_TEST_CHECK( toggleIcons[1].Get<std::string>() == resultIcons[1].Get<std::string>() );
    DALI_TEST_CHECK( toggleIcons[2].Get<std::string>() == resultIcons[2].Get<std::string>() );
    DALI_TEST_CHECK( toggleIcons[3].Get<std::string>() == resultIcons[3].Get<std::string>() );
  }

  {// Check property::map
    Property::Map propertyMap1;
    Vector4 testColor1( 1.f, 0.5f, 0.3f, 0.2f );
    propertyMap1.Insert( Toolkit::Visual::Property::TYPE,  Toolkit::Visual::COLOR);
    propertyMap1.Insert(Toolkit::ColorVisual::Property::MIX_COLOR,  testColor1);

    Property::Map propertyMap2;
    Vector4 testColor2( 0.5f, 1.f, 0.3f, 0.2f );
    propertyMap2.Insert(Toolkit::Visual::Property::TYPE,  Toolkit::Visual::COLOR);
    propertyMap2.Insert(Toolkit::ColorVisual::Property::MIX_COLOR,  testColor2);

    Property::Map propertyMap3;
    Vector4 testColor3( 1.f, 0.5f, 1.f, 0.2f );
    propertyMap3.Insert(Toolkit::Visual::Property::TYPE,  Toolkit::Visual::COLOR);
    propertyMap3.Insert(Toolkit::ColorVisual::Property::MIX_COLOR,  testColor3);

    Property::Array toggleMaps;
    toggleMaps.Add( propertyMap1 );
    toggleMaps.Add( propertyMap2 );
    toggleMaps.Add( propertyMap3 );
    toggleButton.SetProperty( Toolkit::ToggleButton::Property::STATE_VISUALS, toggleMaps );

    application.SendNotification();
    application.Render();

    Property::Array resultMaps;
    resultMaps = toggleButton.GetProperty( Toolkit::ToggleButton::Property::STATE_VISUALS ).Get<Property::Array>();

    // Check that the result
    DALI_TEST_EQUALS( toggleMaps.Count(), resultMaps.Count(), TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliToggleButtonToggleTipsProperty( void )
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliToggleButtonToggleTipsProperty");

  // Create the ToggleButton actor
  ToggleButton toggleButton = ToggleButton::New();
  Stage::GetCurrent().Add( toggleButton );
  toggleButton.SetParentOrigin(ParentOrigin::TOP_LEFT);
  toggleButton.SetAnchorPoint(ParentOrigin::TOP_LEFT);
  toggleButton.SetPosition( 0.0f, 0.0f );

  { // Check empty tip array
    Property::Array toggleIcons;
    toggleIcons.PushBack( TEST_IMAGE_ONE ); //Icons path
    toggleIcons.PushBack( TEST_IMAGE_TWO );
    toggleIcons.PushBack( TEST_IMAGE_THREE );
    toggleButton.SetProperty( Toolkit::ToggleButton::Property::STATE_VISUALS, toggleIcons );

    Property::Array toggleTips;
    toggleButton.SetProperty( Toolkit::ToggleButton::Property::TOOLTIPS, toggleTips );

    application.SendNotification();
    application.Render();

    Property::Array resultTips;
    resultTips = toggleButton.GetProperty( Toolkit::ToggleButton::Property::TOOLTIPS ).Get<Property::Array>();
    DALI_TEST_CHECK( resultTips.Count() == 0 );
  }

  { // Check non-empty tip array
    Property::Array toggleIcons;
    toggleIcons.PushBack( TEST_IMAGE_ONE ); //Icons path
    toggleIcons.PushBack( TEST_IMAGE_TWO );
    toggleIcons.PushBack( TEST_IMAGE_THREE );
    toggleButton.SetProperty( Toolkit::ToggleButton::Property::STATE_VISUALS, toggleIcons );

    Property::Array toggleTips;
    toggleTips.PushBack( "Button State A" ); //Tooltip string
    toggleTips.PushBack( "Button State B" );
    toggleTips.PushBack( "Button State C" );
    toggleButton.SetProperty( Toolkit::ToggleButton::Property::TOOLTIPS, toggleTips );

    application.SendNotification();
    application.Render();

    Property::Array resultTips;
    resultTips = toggleButton.GetProperty( Toolkit::ToggleButton::Property::TOOLTIPS ).Get<Property::Array>();

    //Check that the result is the same as
    DALI_TEST_EQUALS( toggleTips.Count(), resultTips.Count(), TEST_LOCATION );
    DALI_TEST_CHECK( toggleTips[0].Get<std::string>() == resultTips[0].Get<std::string>() );
    DALI_TEST_CHECK( toggleTips[1].Get<std::string>() == resultTips[1].Get<std::string>() );
    DALI_TEST_CHECK( toggleTips[2].Get<std::string>() == resultTips[2].Get<std::string>() );
    DALI_TEST_CHECK( toggleTips[3].Get<std::string>() == resultTips[3].Get<std::string>() );
  }
  END_TEST;
}

int UtcDaliToggleButtonStateChange(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliToggleButtonStateChange");

  // Create the ToggleButton actor
  ToggleButton toggleButton = ToggleButton::New();
  Stage::GetCurrent().Add( toggleButton );
  toggleButton.SetParentOrigin(ParentOrigin::TOP_LEFT);
  toggleButton.SetAnchorPoint(ParentOrigin::TOP_LEFT);
  toggleButton.SetPosition( BUTTON_POSITON_TO_GET_INSIDE_TOUCH_EVENTS );
  toggleButton.SetSize( BUTTON_SIZE_TO_GET_INSIDE_TOUCH_EVENTS );

  Property::Array toggleIcons;
  toggleIcons.PushBack( TEST_IMAGE_ONE ); //Icons path
  toggleIcons.PushBack( TEST_IMAGE_TWO );
  toggleIcons.PushBack( TEST_IMAGE_THREE );
  toggleButton.SetProperty( Toolkit::ToggleButton::Property::STATE_VISUALS, toggleIcons );

  Property::Array toggleTips;
  toggleTips.PushBack( "Button State A" ); //Tooltip string
  toggleTips.PushBack( "Button State B" );
  toggleTips.PushBack( "Button State C" );
  toggleButton.SetProperty( Toolkit::ToggleButton::Property::TOOLTIPS, toggleTips );

  application.SendNotification();
  application.Render();

  Property::Array resultIcons;
  resultIcons = toggleButton.GetProperty( Toolkit::ToggleButton::Property::STATE_VISUALS ).Get<Property::Array>();
  DALI_TEST_EQUALS( toggleIcons.Count(), resultIcons.Count(), TEST_LOCATION );

  Property::Array resultTips;
  resultTips = toggleButton.GetProperty( Toolkit::ToggleButton::Property::TOOLTIPS ).Get<Property::Array>();
  DALI_TEST_EQUALS( toggleTips.Count(), resultTips.Count(), TEST_LOCATION );

  int index;
  DALI_TEST_CHECK( toggleButton.GetProperty( Toolkit::ToggleButton::Property::CURRENT_STATE_INDEX ).Get( index ) );
  DALI_TEST_EQUALS( index, 0, TEST_LOCATION );

  Dali::Integration::TouchEvent event;

  // Touch point down and up inside the button 3 times.
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownInside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpInside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( toggleButton.GetProperty( Toolkit::ToggleButton::Property::CURRENT_STATE_INDEX ).Get( index ) );
  DALI_TEST_EQUALS( index, 1, TEST_LOCATION );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownInside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpInside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( toggleButton.GetProperty( Toolkit::ToggleButton::Property::CURRENT_STATE_INDEX ).Get( index ) );
  DALI_TEST_EQUALS( index, 2, TEST_LOCATION );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownInside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpInside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( toggleButton.GetProperty( Toolkit::ToggleButton::Property::CURRENT_STATE_INDEX ).Get( index ) );
  DALI_TEST_EQUALS( index, 0, TEST_LOCATION );

  END_TEST;
}