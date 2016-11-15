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
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_pushbutton_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_pushbutton_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
static const char* TEST_IMAGE_ONE = TEST_RESOURCE_DIR "/gallery-small-1.jpg";

static bool gPushButtonSelectedState = false;
bool PushButtonSelected( Button button )
{
  gPushButtonSelectedState = button.IsSelected();
  return true;
}

static bool gPushButtonPressed = false;

static bool PushButtonPressed( Button button )
{
  gPushButtonPressed = true;
  return true;
}

static bool gPushButtonReleased = false;

static bool PushButtonReleased( Button button )
{
  gPushButtonReleased = true;
  return true;
}

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

} //namespace

int UtcDaliPushButtonConstructorP(void)
{
  TestApplication application;

  PushButton button;

  DALI_TEST_CHECK( !button );
  END_TEST;
}

int UtcDaliPushButtonCopyConstructorP(void)
{
  TestApplication application;

  // Initialize an object, ref count == 1
  PushButton button = PushButton::New();

  PushButton copy( button );
  DALI_TEST_CHECK( copy );
  END_TEST;
}

int UtcDaliPushButtonAssignmentOperatorP(void)
{
  TestApplication application;

  PushButton button = PushButton::New();

  PushButton copy( button );
  DALI_TEST_CHECK( copy );

  DALI_TEST_CHECK( button == copy );
  END_TEST;
}

int UtcDaliPushButtonNewP(void)
{
  TestApplication application;

  PushButton button = PushButton::New();

  DALI_TEST_CHECK( button );
  END_TEST;
}

int UtcDaliPushButtonDownCastP(void)
{
  TestApplication application;

  PushButton button = PushButton::New();

  BaseHandle object(button);

  PushButton button2 = PushButton::DownCast( object );
  DALI_TEST_CHECK(button2);

  PushButton button3 = DownCast< PushButton >(object);
  DALI_TEST_CHECK(button3);
  END_TEST;
}

int UtcDaliPushButtonDownCastN(void)
{
  TestApplication application;

  BaseHandle unInitializedObject;

  PushButton button1 = PushButton::DownCast( unInitializedObject );
  DALI_TEST_CHECK( !button1 );

  PushButton button2 = DownCast< PushButton >( unInitializedObject );
  DALI_TEST_CHECK( !button2 );
  END_TEST;
}

int UtcDaliPushButtonSetGetAutoRepeating(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetAutoRepeating");

  PushButton pushButton = PushButton::New();

  pushButton.SetAutoRepeating( true );

  DALI_TEST_CHECK( pushButton.IsAutoRepeating() );

  pushButton.SetAutoRepeating( false );

  DALI_TEST_CHECK( !pushButton.IsAutoRepeating() );

  pushButton.SetAutoRepeating( true );

  DALI_TEST_CHECK( pushButton.IsAutoRepeating() );
  END_TEST;
}

int UtcDaliPushButtonSetAutoRepeating(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliPushButtonSetAutoRepeating\n");
  tet_infoline("Ensure setting AutoRepeating on a SELECTED Toggle button switches off Toggle\n");
  PushButton pushButton = PushButton::New();

  const bool INITIAL_TOGGLE_VALUE = true;
  const bool INITIAL_SELECTED_VALUE = true;

  pushButton.SetProperty( Button::Property::TOGGLABLE, INITIAL_TOGGLE_VALUE);
  pushButton.SetProperty( Button::Property::SELECTED, INITIAL_SELECTED_VALUE );

  DALI_TEST_EQUALS( pushButton.GetProperty<bool>( Button::Property::TOGGLABLE  ), INITIAL_TOGGLE_VALUE , TEST_LOCATION );
  DALI_TEST_EQUALS( pushButton.GetProperty<bool>( Button::Property::SELECTED  ), INITIAL_SELECTED_VALUE , TEST_LOCATION );

  pushButton.SetProperty( Button::Property::AUTO_REPEATING, true );

  DALI_TEST_EQUALS( pushButton.GetProperty<bool>( Button::Property::TOGGLABLE ), !INITIAL_TOGGLE_VALUE , TEST_LOCATION );

  END_TEST;
}

int UtcDaliPushButtonSetGetTogglableButton(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetTogglableButton");

  PushButton pushButton = PushButton::New();

  pushButton.SetTogglableButton( true );

  DALI_TEST_CHECK( pushButton.IsTogglableButton() );

  pushButton.SetTogglableButton( false );

  DALI_TEST_CHECK( !pushButton.IsTogglableButton() );

  pushButton.SetTogglableButton( true );

  DALI_TEST_CHECK( pushButton.IsTogglableButton() );
  END_TEST;
}

int UtcDaliPushButtonSetGetAutoRepeatingAndTogglableButton(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetAutoRepeatingAndTogglableButton");

  PushButton pushButton = PushButton::New();

  pushButton.SetAutoRepeating( true );
  pushButton.SetTogglableButton( true );

  DALI_TEST_CHECK( pushButton.IsTogglableButton() );
  DALI_TEST_CHECK( !pushButton.IsAutoRepeating() );

  pushButton.SetTogglableButton( true );
  pushButton.SetAutoRepeating( true );

  DALI_TEST_CHECK( pushButton.IsAutoRepeating() );
  DALI_TEST_CHECK( !pushButton.IsTogglableButton() );
  END_TEST;
}

int UtcDaliPushButtonSetGetSelected01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetSelected01");

  PushButton pushButton = PushButton::New();

  pushButton.SetTogglableButton( true );
  pushButton.StateChangedSignal().Connect( &PushButtonSelected );

  gPushButtonSelectedState = false;
  pushButton.SetSelected( true );

  DALI_TEST_CHECK( pushButton.IsSelected() );
  DALI_TEST_CHECK( gPushButtonSelectedState );

  pushButton.SetSelected( false );

  DALI_TEST_CHECK( !pushButton.IsSelected() );
  DALI_TEST_CHECK( !gPushButtonSelectedState );

  pushButton.SetSelected( true );

  DALI_TEST_CHECK( pushButton.IsSelected() );
  DALI_TEST_CHECK( gPushButtonSelectedState );
  END_TEST;
}

int UtcDaliPushButtonSetGetSelected02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetSelected02");

  PushButton pushButton = PushButton::New();

  pushButton.SetTogglableButton( false );
  pushButton.StateChangedSignal().Connect( &PushButtonSelected );

  gPushButtonSelectedState = false;
  pushButton.SetSelected( true );

  DALI_TEST_CHECK( !pushButton.IsSelected() );
  DALI_TEST_CHECK( !gPushButtonSelectedState );

  pushButton.SetSelected( false );

  DALI_TEST_CHECK( !pushButton.IsSelected() );
  DALI_TEST_CHECK( !gPushButtonSelectedState );

  pushButton.SetSelected( true );

  DALI_TEST_CHECK( !pushButton.IsSelected() );
  DALI_TEST_CHECK( !gPushButtonSelectedState );
  END_TEST;
}

int UtcDaliPushButtonSetGetAutorepeatingDelayValues01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetAutorepeatingDelayValues01");

  PushButton pushButton = PushButton::New();

  pushButton.SetAutoRepeating( true );

  pushButton.SetInitialAutoRepeatingDelay( 1.f );
  DALI_TEST_EQUALS( pushButton.GetInitialAutoRepeatingDelay(), 1.f, TEST_LOCATION );

  pushButton.SetNextAutoRepeatingDelay( 1.f );
  DALI_TEST_EQUALS( pushButton.GetNextAutoRepeatingDelay(), 1.f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPushButtonSetGetAutorepeatingDelayValues02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetAutorepeatingDelayValues02");

  PushButton pushButton = PushButton::New();

  bool assert1( false );
  bool assert2( false );

  pushButton.SetAutoRepeating( true );

  try
  {
    pushButton.SetInitialAutoRepeatingDelay( -1.f );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_EQUALS(e.condition, "initialAutoRepeatingDelay > 0.f", TEST_LOCATION);
    assert1 = true;
  }

  try
  {
    pushButton.SetNextAutoRepeatingDelay( -1.f );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_EQUALS(e.condition, "nextAutoRepeatingDelay > 0.f", TEST_LOCATION);
    assert2 = true;
  }

  DALI_TEST_CHECK( assert1 && assert2 );
  END_TEST;
}

int UtcDaliPushButtonSetLabelText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetLabelText");

  const std::string STR( "Hola!" );

  PushButton pushButton = PushButton::New();

  application.SendNotification();
  application.Render();

  pushButton.SetLabelText( STR );

  DALI_TEST_EQUALS( pushButton.GetLabelText(), STR, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPushButtonPressed(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonPressed");

  PushButton pushButton = PushButton::New();
  pushButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  pushButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pushButton.SetPosition( 240, 400 );
  pushButton.SetSize( 100, 100 );

  Stage::GetCurrent().Add( pushButton );

  application.SendNotification();
  application.Render();

  gPushButtonPressed = false;

  // connect to its touch signal
  pushButton.PressedSignal().Connect( &PushButtonPressed );

  Dali::Integration::TouchEvent eventDown;
  eventDown.AddPoint( GetPointDownInside() );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  application.ProcessEvent( eventDown );

  DALI_TEST_CHECK( gPushButtonPressed );
  END_TEST;
}

int UtcDaliPushButtonReleased(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonReleased");

  PushButton pushButton = PushButton::New();
  pushButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  pushButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pushButton.SetPosition( 240, 400 );
  pushButton.SetSize( 100, 100 );

  Stage::GetCurrent().Add( pushButton );

  application.SendNotification();
  application.Render();

  // connect to its touch signal
  pushButton.ReleasedSignal().Connect( &PushButtonReleased );

  Dali::Integration::TouchEvent event;

  // Test1. Touch point down and up inside the button.

  gPushButtonReleased = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownInside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpInside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gPushButtonReleased );

  // Test2. Touch point down and up outside the button.

  gPushButtonReleased = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownOutside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpOutside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonReleased );

  // Test3. Touch point down inside and up outside the button.

  gPushButtonReleased = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownInside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointLeave() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpOutside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gPushButtonReleased );

  // Test4. Touch point down outside and up inside the button.

  gPushButtonReleased = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownOutside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointEnter() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpInside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonReleased );
  END_TEST;
}

int UtcDaliPushButtonSelected(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSelected");

  PushButton pushButton = PushButton::New();
  pushButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  pushButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pushButton.SetPosition( 240, 400 );
  pushButton.SetSize( 100, 100 );

  Stage::GetCurrent().Add( pushButton );

  application.SendNotification();
  application.Render();

  // connect to its touch signal
  pushButton.StateChangedSignal().Connect( &PushButtonSelected );

  Dali::Integration::TouchEvent event;

  // Test1. No togglable button.

  gPushButtonSelectedState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownInside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpInside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonSelectedState );

  // Set togglable property.
  pushButton.SetTogglableButton( true );

  // Test2. Touch point down and up inside the button twice.
  gPushButtonSelectedState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownInside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpInside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gPushButtonSelectedState );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownInside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpInside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonSelectedState );

  // Test3. Touch point down and up outside the button.

  gPushButtonSelectedState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownOutside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpOutside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonSelectedState );

  // Test4. Touch point down inside and up outside the button.
  //        State changes on Button down
  gPushButtonSelectedState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownInside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointLeave() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpOutside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gPushButtonSelectedState );

  // Test5. Touch point down outside and up inside the button.
  // Start in unselected state
  pushButton.SetProperty( Button::Property::SELECTED, false );
  DALI_TEST_CHECK( !pushButton.IsSelected());

  gPushButtonSelectedState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownOutside() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointEnter() );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpInside() );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonSelectedState );
  END_TEST;
}

int UtcDaliPushButtonPropertySetIconAlignment(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonPropertySetIconAlignment");

  PushButton pushButton = PushButton::New();
  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_ALIGNMENT, "TOP" );
  DALI_TEST_EQUALS( pushButton.GetProperty<std::string>( Toolkit::PushButton::Property::ICON_ALIGNMENT ), "TOP", TEST_LOCATION );

  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_ALIGNMENT, "RIGHT" );
  DALI_TEST_EQUALS( pushButton.GetProperty<std::string>( Toolkit::PushButton::Property::ICON_ALIGNMENT ), "RIGHT", TEST_LOCATION );

  END_TEST;
}

int UtcDaliPushButtonPropertySetLabelPadding(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonPropertySetLabelPadding");

  PushButton pushButton = PushButton::New();
  pushButton.SetProperty( Toolkit::PushButton::Property::LABEL_PADDING, Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
  DALI_TEST_EQUALS( pushButton.GetProperty<Vector4>( Toolkit::PushButton::Property::LABEL_PADDING ), Vector4( 1.0f, 1.0f, 1.0f, 1.0f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  pushButton.SetProperty( Toolkit::PushButton::Property::LABEL_PADDING, Vector4( 10.0f, 10.0f, 10.0f, 10.0f ) );
  DALI_TEST_EQUALS( pushButton.GetProperty<Vector4>( Toolkit::PushButton::Property::LABEL_PADDING ), Vector4( 10.0f, 10.0f, 10.0f, 10.0f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPushButtonPropertySetIconPadding(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonPropertySetIconPadding");

  PushButton pushButton = PushButton::New();
  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_PADDING, Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
  DALI_TEST_EQUALS( pushButton.GetProperty<Vector4>( Toolkit::PushButton::Property::ICON_PADDING ), Vector4( 1.0f, 1.0f, 1.0f, 1.0f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_PADDING, Vector4( 10.0f, 10.0f, 10.0f, 10.0f ) );
  DALI_TEST_EQUALS( pushButton.GetProperty<Vector4>( Toolkit::PushButton::Property::ICON_PADDING ), Vector4( 10.0f, 10.0f, 10.0f, 10.0f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPushButtonPaddingLayout(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonPaddingLayout");

  // This test creates padding for an icon and a label.
  // The icon and label are each enabled and disabled to confirm the correct padding is used.
  PushButton pushButton = PushButton::New();

  const Vector4 TEST_ICON_PADDING( 20.0f, 20.0f, 20.0f, 20.0f );
  const Vector4 TEST_LABEL_PADDING( 10.0f, 10.0f, 10.0f ,10.0f );
  const Vector2 TEST_IMAGE_SIZE = Vector2( 5.0f, 5.0f);

  pushButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  pushButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pushButton.SetPosition( 0.0f, 0.0f );
  pushButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

  Stage::GetCurrent().Add( pushButton );

  application.SendNotification();
  application.Render();

  // First test the size is zero.
  // No padding should be added as there is no label or icon.
  Vector2 size( Vector2::ZERO );
  size.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  size.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );
  tet_printf( "Button Natural Size(%f,%f)\n", pushButton.GetNaturalSize().width, pushButton.GetNaturalSize().height );

  DALI_TEST_EQUALS( size, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Check label only padding
  pushButton.SetLabelText( "Label" );

  application.SendNotification();
  application.Render();

  Vector2 sizeWithLabelWithoutPadding( Vector2::ZERO );
  sizeWithLabelWithoutPadding.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  sizeWithLabelWithoutPadding.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );

  tet_printf( "Button RelayoutSize label without padding (%f,%f)\n", sizeWithLabelWithoutPadding.width, sizeWithLabelWithoutPadding.height );

  // Add label padding to label
  pushButton.SetProperty( Toolkit::PushButton::Property::LABEL_PADDING, TEST_LABEL_PADDING );
  application.SendNotification();
  application.Render();

  Vector2 sizeLabelAndPadding( Vector2::ZERO );
  sizeLabelAndPadding.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  sizeLabelAndPadding.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );
  tet_printf( "Button RelayoutSize after label padding(%f,%f)\n", sizeLabelAndPadding.width, sizeLabelAndPadding.height );

  // If control size has increased beyond size of just label then padding has been applied
  DALI_TEST_GREATER( sizeLabelAndPadding.width, sizeWithLabelWithoutPadding.width+TEST_LABEL_PADDING.x, TEST_LOCATION );
  DALI_TEST_GREATER( sizeLabelAndPadding.height, sizeWithLabelWithoutPadding.height+TEST_LABEL_PADDING.w, TEST_LOCATION );

  // Re-initialise the button so we can setup icon-only padding.
  pushButton.Unparent();
  pushButton = PushButton::New();

  pushButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  pushButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pushButton.SetPosition( 0.0f, 0.0f );
  pushButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

  Stage::GetCurrent().Add( pushButton );

  TestPlatformAbstraction& platform = application.GetPlatform();
  platform.SetClosestImageSize( TEST_IMAGE_SIZE );

  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_ALIGNMENT, "RIGHT" );
  pushButton.SetProperty( Toolkit::PushButton::Property::UNSELECTED_ICON, TEST_IMAGE_ONE );
  pushButton.SetProperty( Toolkit::PushButton::Property::SELECTED_ICON, TEST_IMAGE_ONE );

  application.SendNotification();
  application.Render();

  // Size of button with just icon
  size.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  size.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );
  tet_printf( "Button RelayoutSize with icon(%f,%f)\n", size.width, size.height );

  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_PADDING, TEST_ICON_PADDING );

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( size, TEST_IMAGE_SIZE, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  size.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  size.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );
  tet_printf( "Button RelayoutSize after icon padding(%f,%f)\n", size.width, size.height );
  const Vector2 expectedIconAndPaddingSize( TEST_ICON_PADDING.x+TEST_ICON_PADDING.y+TEST_IMAGE_SIZE.width, TEST_ICON_PADDING.w+TEST_ICON_PADDING.z +TEST_IMAGE_SIZE.height );
  DALI_TEST_EQUALS( size, expectedIconAndPaddingSize, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Now test padding for both label and icon simultaneously.
  pushButton.SetLabelText( "Label" );
  application.SendNotification();
  application.Render();

  size.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  size.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );
  tet_printf( "Button RelayoutSize after label added(%f,%f)\n", size.width, size.height );

  pushButton.SetProperty( Toolkit::PushButton::Property::LABEL_PADDING, TEST_LABEL_PADDING );

  application.SendNotification();
  application.Render();

  size.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  size.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );
  tet_printf( "Button RelayoutSize after icon and label padding(%f,%f)\n", size.width, size.height );

  DALI_TEST_EQUALS( size.width, sizeLabelAndPadding.width + expectedIconAndPaddingSize.width, TEST_LOCATION );
  DALI_TEST_GREATER( size.height, expectedIconAndPaddingSize.width, TEST_LOCATION ); // Test height of control is greater than icon and padding. As Text set to larger values.

  END_TEST;
}

int UtcDaliPushButtonAlignmentLayout(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonAlignmentLayout");

  /*
   * This test checks different alignments for the icon against the label.
   * The icon is then moved around the label in each of it's alignments.
   * The final relayed out size is checked to confirm the layout has been done correctly.
   *
   * There is an Icon which has 0 width and height, but with 75 padding on all sides.
   *  - Therefore total width and height are both 150.
   *
   * There is a Label which has "an unknown" width and height, but with 30 padding on all sides.
   *  - Therefore total width and height are 60+x and 60+y respectively.
   *    Where x & y are the width and height of the text.
   *
   * The width of the button will always expand to the largest of the icon and label sizes (plus padding).
   * So We use the padding to help us determine the orientation is correct for each alignment.
   *
   * |<- 150 ->|         |<-- 60+x -->|
   *
   * +---------+   -
   * |         |   ^     +------------+   -
   * |         |   |     |            |   ^
   * |  Icon   |  150    |   Label    |  60+y
   * |         |   |     |            |   v
   * |         |   v     +------------+   -
   * +---------+   -
   */

  const Vector4 TEST_ICON_PADDING( 70.0f, 70.0f, 70.0f, 70.0f );
  const Vector4 TEST_LABEL_PADDING( 30.0f, 30.0f, 30.0f, 30.0f );
  const Vector2 TEST_IMAGE_SIZE = Vector2( 10.0f, 10.0f);

  PushButton pushButton = PushButton::New();

  pushButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  pushButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pushButton.SetPosition( 0.0f, 0.0f );
  pushButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

  Stage::GetCurrent().Add( pushButton );

  // Add a label and get size of control
  pushButton.SetLabelText( "Label" );
  application.SendNotification();
  application.Render();

  // First get the size of control with just label
  Vector2 justLabelSize( Vector2::ZERO );
  justLabelSize.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  justLabelSize.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );
  tet_printf( "Button RelayoutSize with just label and no padding(%f,%f)\n", justLabelSize.width, justLabelSize.height );

  pushButton.SetProperty( Toolkit::PushButton::Property::LABEL_PADDING, TEST_LABEL_PADDING );
  application.SendNotification();
  application.Render();

  // Size of Label and Padding
  Vector2 expectedLabelAndPaddingSize( Vector2::ZERO );
  expectedLabelAndPaddingSize.width = justLabelSize.width + TEST_LABEL_PADDING.x + TEST_LABEL_PADDING.y;
  expectedLabelAndPaddingSize.height = justLabelSize.height + TEST_LABEL_PADDING.w + TEST_LABEL_PADDING.z;

  Vector2 labelAndPaddingSize( Vector2::ZERO );
  labelAndPaddingSize.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  labelAndPaddingSize.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );

  DALI_TEST_EQUALS( labelAndPaddingSize, expectedLabelAndPaddingSize , Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  const Vector2 testImageWithPaddingSize = Vector2 ( ( TEST_IMAGE_SIZE.width + TEST_ICON_PADDING.x + TEST_ICON_PADDING.y ),
                                                     ( TEST_IMAGE_SIZE.height + TEST_ICON_PADDING.w + TEST_ICON_PADDING.z ) );

  TestPlatformAbstraction& platform = application.GetPlatform();
  platform.SetClosestImageSize( TEST_IMAGE_SIZE );

  // Add Icon and set its alignment
  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_ALIGNMENT, "RIGHT" );
  pushButton.SetProperty( Toolkit::PushButton::Property::UNSELECTED_ICON, TEST_IMAGE_ONE );
  pushButton.SetProperty( Toolkit::PushButton::Property::SELECTED_ICON, TEST_IMAGE_ONE );
  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_PADDING, TEST_ICON_PADDING );

  application.SendNotification();
  application.Render();

  Vector2 size( Vector2::ZERO );
  size.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  size.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );

  /*
   * Test Icon right alignment.
   * Height grows to largest of Icon or Label (+ padding).
   * Normally this will be Icons height, except with very large font sizes.
   *
   *  +------------+---------+
   *  |............+         |
   *  |            |         |
   *  |   Label    |  Icon   |
   *  |            |         |
   *  |............+         |
   *  +------------+---------+
   */
  DALI_TEST_EQUALS( size.width, ( testImageWithPaddingSize.width + labelAndPaddingSize.width ) , TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, ( std::max( testImageWithPaddingSize.height, labelAndPaddingSize.height) ) , Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Now test left alignment matches right for size.
  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_ALIGNMENT, "LEFT" );

  application.SendNotification();
  application.Render();

  size.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  size.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );

  /*
   * Test Icon left alignment.
   * Height grows to largest of Icon or Label (+ padding).
   * Normally this will be Icons height, except with very large font sizes.
   *
   *  +---------+------------+
   *  |         +............|
   *  |         |            |
   *  |  Icon   |   Label    |
   *  |         |            |
   *  |         +............|
   *  +---------+------------+
   */
  DALI_TEST_EQUALS( size.width, ( testImageWithPaddingSize.width + labelAndPaddingSize.width ) , TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, ( std::max( testImageWithPaddingSize.height, labelAndPaddingSize.height) ) , Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  tet_infoline(" Test Icon TOP alignment - Width grows to largest of Icon or label (plus padding)");
  /*
   *
   *  +---------+
   *  |         |
   *  |         |
   *  |  Icon   |
   *  |         |
   *  |         |
   *  +---------+
   *  |         |
   *  |  Label  |
   *  |         |
   *  +---------+
   *
   */

  tet_infoline("SetProperty on ICON_ALIGNMENT should relayout the Button");
  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_ALIGNMENT, "TOP" );

  application.SendNotification();
  application.Render();

  size.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  size.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );

  tet_printf("Natural width (%f)\n",pushButton.GetNaturalSize().width);
  tet_printf("Natural height (%f)\n",pushButton.GetNaturalSize().height);

  tet_printf(" UtcDaliPushButtonAlignmentLayout Top layout - Image and Padding size (%f,%f)\n", testImageWithPaddingSize.width, testImageWithPaddingSize.height );
  tet_printf(" UtcDaliPushButtonAlignmentLayout Top layout - Text and Padding size (%f,%f)\n", labelAndPaddingSize.width, labelAndPaddingSize.height );

  DALI_TEST_EQUALS( size.width, ( std::max( testImageWithPaddingSize.width, labelAndPaddingSize.width ) ) , TEST_LOCATION );

  DALI_TEST_EQUALS( size.height,( testImageWithPaddingSize.height + labelAndPaddingSize.height ) , TEST_LOCATION );

  /*
   * Test Icon bottom alignment.
   * Width grows to largest of Icon or Label (+ padding).
   *
   *  +---------+
   *  |         |
   *  |  Label  |
   *  |         |
   *  +---------+
   *  |         |
   *  |         |
   *  |  Icon   |
   *  |         |
   *  |         |
   *  +---------+
   */
  tet_infoline(" Test Icon BOTTOM alignment - Width grows to largest of Icon or label (plus padding)");
  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_ALIGNMENT, "BOTTOM" );

  application.SendNotification();
  application.Render();

  size.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  size.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );

  DALI_TEST_EQUALS( size.width, ( std::max(testImageWithPaddingSize.width, labelAndPaddingSize.width )) , TEST_LOCATION );
  DALI_TEST_EQUALS( size.height,( testImageWithPaddingSize.height + labelAndPaddingSize.height ) , TEST_LOCATION );

  END_TEST;
}

int UtcDaliPushButtonSetUnSelectedVisual01P(void)
{
  tet_infoline(" Test adding a visual for the UNSELECTED_VISUAL property, removing Button from stage and counting renderers\n");
  ToolkitTestApplication application;

  PushButton pushButton = PushButton::New();
  pushButton.SetSize(100.0f, 100.0f);

  Stage::GetCurrent().Add( pushButton );

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);

  pushButton.SetProperty( Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, propertyMap );

  tet_infoline(" UNSELECTED_VISUAL Added to button\n");

  application.SendNotification();
  application.Render(0);

  unsigned int rendererCount = pushButton.GetRendererCount();
  tet_printf("After adding UNSELECTED_BACKGROUND_VISUAL the renderer count is(%d)\n", rendererCount );

  DALI_TEST_EQUALS( pushButton.GetRendererCount(), 1 , TEST_LOCATION );

  tet_printf("Remove button from stage\n" );

  Stage::GetCurrent().Remove( pushButton );

  rendererCount = pushButton.GetRendererCount();
  tet_printf("After removing pushbutton from stage the renderer count is(%d)\n ", rendererCount );

  DALI_TEST_EQUALS( pushButton.GetRendererCount(), 0, TEST_LOCATION );

  tet_printf("After removing pushbutton from stage the renderer count is(%d)\n ", rendererCount );

  Property::Map propertyMap2;
  propertyMap2.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap2.Insert(ColorVisual::Property::MIX_COLOR, Color::RED);
  pushButton.SetProperty( Toolkit::Button::Property::UNSELECTED_VISUAL, propertyMap2 );

  tet_printf("Added UNSELECTED_VISUAL and add button back to Stage\n");

  Stage::GetCurrent().Add( pushButton );

  tet_printf("With UNSELECTED_BACKGROUND_VISUAL and UNSELECTED_ICON the renderer count is(%d)\n", pushButton.GetRendererCount() );

  DALI_TEST_EQUALS( pushButton.GetRendererCount(), 2, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPushButtonSetSelectedVisualN(void)
{
  tet_infoline(" Test adding a broken visual for the UNSELECTED_VISUAL property");

  ToolkitTestApplication application;

  PushButton pushButton = PushButton::New();

  pushButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  pushButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pushButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

  Stage::GetCurrent().Add( pushButton );
  application.SendNotification();
  application.Render(0);

  unsigned int preRendererCount = pushButton.GetRendererCount();
  tet_printf("RendererCount prior to adding visual(%d)\n",preRendererCount);
  DALI_TEST_EQUALS( preRendererCount, 0, TEST_LOCATION );

  Stage::GetCurrent().Remove( pushButton );
  application.SendNotification();
  application.Render(0);

  Property::Map colorMap;
  const int BROKEN_VISUAL_TYPE = 999999999;

  colorMap.Insert(Visual::Property::TYPE,  BROKEN_VISUAL_TYPE);
  colorMap.Insert(BorderVisual::Property::COLOR,  Color::BLUE);
  colorMap.Insert(BorderVisual::Property::SIZE,  5.f);
  pushButton.SetProperty( Toolkit::Button::Property::UNSELECTED_VISUAL, colorMap );

  Stage::GetCurrent().Add( pushButton );
  application.SendNotification();
  application.Render(0);

  unsigned int postRendererCount  = pushButton.GetRendererCount();
  tet_printf("RendererCount post broken visual (%d)\n", postRendererCount);
  DALI_TEST_EQUALS( postRendererCount, 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPushButtonSetButtonImageP(void)
{
  ToolkitTestApplication application;

  PushButton button = PushButton::New();
  Stage::GetCurrent().Add( button );

  try
  {
    button.SetButtonImage( ImageView::New() );
    DALI_TEST_CHECK( true );
  }
  catch(...)
  {
    DALI_TEST_CHECK( false );
  }

  END_TEST;
}

int UtcDaliPushButtonSetBackgroundImageP(void)
{
  ToolkitTestApplication application;

  PushButton button = PushButton::New();
  Stage::GetCurrent().Add( button );

  try
  {
    button.SetBackgroundImage( ImageView::New() );
    DALI_TEST_CHECK( true );
  }
  catch(...)
  {
    DALI_TEST_CHECK( false );
  }

  END_TEST;
}

int UtcDaliPushButtonSetSelectedImageP(void)
{
  ToolkitTestApplication application;

  PushButton button = PushButton::New();
  Stage::GetCurrent().Add( button );

  try
  {
    button.SetSelectedImage( ImageView::New() );
    DALI_TEST_CHECK( true );
  }
  catch(...)
  {
    DALI_TEST_CHECK( false );
  }

  END_TEST;
}

int UtcDaliPushButtonSetSelectedBackgroundImageP(void)
{
  ToolkitTestApplication application;

  PushButton button = PushButton::New();
  Stage::GetCurrent().Add( button );

  try
  {
    button.SetSelectedBackgroundImage( ImageView::New() );
    DALI_TEST_CHECK( true );
  }
  catch(...)
  {
    DALI_TEST_CHECK( false );
  }

  END_TEST;
}

int UtcDaliPushButtonSetDisabledBackgroundImageP(void)
{
  ToolkitTestApplication application;

  PushButton button = PushButton::New();
  Stage::GetCurrent().Add( button );

  try
  {
    button.SetDisabledBackgroundImage( ImageView::New() );
    DALI_TEST_CHECK( true );
  }
  catch(...)
  {
    DALI_TEST_CHECK( false );
  }

  END_TEST;
}


int UtcDaliPushButtonSetDisabledImageP(void)
{
  ToolkitTestApplication application;

  PushButton button = PushButton::New();
  Stage::GetCurrent().Add( button );

  try
  {
    button.SetDisabledImage( ImageView::New() );
    DALI_TEST_CHECK( true );
  }
  catch(...)
  {
    DALI_TEST_CHECK( false );
  }

  END_TEST;
}

int UtcDaliPushButtonSetDisabledSelectedImageP(void)
{
  ToolkitTestApplication application;

  PushButton button = PushButton::New();
  Stage::GetCurrent().Add( button );

  try
  {
    button.SetDisabledSelectedImage( ImageView::New() );
    DALI_TEST_CHECK( true );
  }
  catch(...)
  {
    DALI_TEST_CHECK( false );
  }

  END_TEST;
}
