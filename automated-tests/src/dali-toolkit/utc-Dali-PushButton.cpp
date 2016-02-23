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

const Dali::TouchPoint pointDownInside( 0, TouchPoint::Down, 240, 400 );
const Dali::TouchPoint pointUpInside( 0, TouchPoint::Up, 240, 400 );
const Dali::TouchPoint pointLeave( 0, TouchPoint::Leave, 240, 400 );
const Dali::TouchPoint pointEnter( 0, TouchPoint::Motion, 240, 400 );
const Dali::TouchPoint pointMotionOut( 0, TouchPoint::Motion, 10, 10 );
const Dali::TouchPoint pointDownOutside( 0, TouchPoint::Down, 10, 10 );
const Dali::TouchPoint pointUpOutside( 0, TouchPoint::Up, 10, 10 );

Image CreateSolidColorImage( const Vector4& color, unsigned int width, unsigned int height )
{
  BufferImage imageData = BufferImage::New( width, height, Pixel::RGBA8888 );

  // Create the image
  PixelBuffer* pixbuf = imageData.GetBuffer();
  unsigned int size = width * height;

  for( size_t i = 0; i < size; i++ )
    {
      pixbuf[i*4+0] = 0xFF * color.r;
      pixbuf[i*4+1] = 0xFF * color.g;
      pixbuf[i*4+2] = 0xFF * color.b;
      pixbuf[i*4+3] = 0xFF * color.a;
    }

  imageData.Update();

  return imageData;
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
  eventDown.AddPoint( pointDownInside );

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
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gPushButtonReleased );

  // Test2. Touch point down and up outside the button.

  gPushButtonReleased = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonReleased );

  // Test3. Touch point down inside and up outside the button.

  gPushButtonReleased = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointLeave );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gPushButtonReleased );

  // Test4. Touch point down outside and up inside the button.

  gPushButtonReleased = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointEnter );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
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
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonSelectedState );

  // Set togglable property.
  pushButton.SetTogglableButton( true );

  // Test2. Touch point down and up inside the button twice.
  gPushButtonSelectedState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gPushButtonSelectedState );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonSelectedState );

  // Test3. Touch point down and up outside the button.

  gPushButtonSelectedState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonSelectedState );

  // Test4. Touch point down inside and up outside the button.

  gPushButtonSelectedState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointLeave );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonSelectedState );

  // Test5. Touch point down outside and up inside the button.

  gPushButtonSelectedState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointEnter );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
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

  pushButton.SetProperty( Toolkit::PushButton::Property::LABEL_PADDING, Vector4( 10.0f, 10.0f, 10.0f, 10.0f ) );
  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_PADDING, Vector4( 20.0f, 20.0f, 20.0f, 20.0f ) );

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

  DALI_TEST_EQUALS( size, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Check label only padding.
  pushButton.SetLabelText( "Label" );

  application.SendNotification();
  application.Render();

  size.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  size.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );

  // We should not test against the exact label size, we just make sure it is larger than our label padding so we know the padding has been applied.
  DALI_TEST_GREATER( size.width, 20.0f, TEST_LOCATION );
  DALI_TEST_GREATER( size.height, 20.0f, TEST_LOCATION );

  // Re-initialise the button so we can setup icon-only padding.
  pushButton.Unparent();
  pushButton = PushButton::New();

  pushButton.SetProperty( Toolkit::PushButton::Property::LABEL_PADDING, Vector4( 10.0f, 10.0f, 10.0f, 10.0f ) );
  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_PADDING, Vector4( 20.0f, 20.0f, 20.0f, 20.0f ) );

  pushButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  pushButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pushButton.SetPosition( 0.0f, 0.0f );
  pushButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

  Stage::GetCurrent().Add( pushButton );

  const char* INVALID_IMAGE_FILE_NAME = "invalid-image.jpg";
  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_ALIGNMENT, "RIGHT" );
  pushButton.SetProperty( Toolkit::PushButton::Property::UNSELECTED_ICON, INVALID_IMAGE_FILE_NAME );
  pushButton.SetProperty( Toolkit::PushButton::Property::SELECTED_ICON, INVALID_IMAGE_FILE_NAME );

  application.SendNotification();
  application.Render();

  size.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  size.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );

  DALI_TEST_EQUALS( size, Vector2( 40.0f, 40.0f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Now test padding for both label and icon simultaneously.
  pushButton.SetLabelText( "Label" );

  application.SendNotification();
  application.Render();

  size.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  size.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );

  // We should not test against the exact label size, we just make sure it is larger than our label padding so we know the padding has been applied.
  // Note we only test the width as we are horizontally aligned and the label my be less high than the icon.
  // Full directional alignment tests are done in UtcDaliPushButtonAlignmentLayout.
  DALI_TEST_GREATER( size.width, 60.0f, TEST_LOCATION );

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
  PushButton pushButton = PushButton::New();

  pushButton.SetProperty( Toolkit::PushButton::Property::LABEL_PADDING, Vector4( 30.0f, 30.0f, 30.0f, 30.0f ) );
  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_PADDING, Vector4( 75.0f, 75.0f, 75.0f, 75.0f ) );

  pushButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  pushButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pushButton.SetPosition( 0.0f, 0.0f );
  pushButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

  Stage::GetCurrent().Add( pushButton );

  const char* INVALID_IMAGE_FILE_NAME = "invalid-image.jpg";
  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_ALIGNMENT, "RIGHT" );
  pushButton.SetProperty( Toolkit::PushButton::Property::UNSELECTED_ICON, INVALID_IMAGE_FILE_NAME );
  pushButton.SetProperty( Toolkit::PushButton::Property::SELECTED_ICON, INVALID_IMAGE_FILE_NAME );

  application.SendNotification();
  application.Render();

  // First get the base size (without label).
  Vector2 baseSize( Vector2::ZERO );
  baseSize.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  baseSize.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );

  DALI_TEST_EQUALS( baseSize, Vector2( 150.0f, 150.0f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Add a label to cause size to be modified in the direction of alignment.
  pushButton.SetLabelText( "Label" );

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
  DALI_TEST_GREATER( size.width, 150.0f + 60.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 150.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Now test left alignment matches right for size.
  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_ALIGNMENT, "LEFT" );

  application.SendNotification();
  application.Render();

  Vector2 compareSize( Vector2::ZERO );
  compareSize.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  compareSize.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );

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
  DALI_TEST_EQUALS( size, compareSize, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Test top alignment.
  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_ALIGNMENT, "TOP" );

  application.SendNotification();
  application.Render();

  compareSize.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  compareSize.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );

  /*
   * Test Icon top alignment.
   * Width grows to largest of Icon or Label (+ padding).
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
   *  Note: We subtract a small number as we want to do a >= test.
   */
  DALI_TEST_GREATER( size.width, 150.0f - Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_GREATER( compareSize.height, 150.0f + 60.0f, TEST_LOCATION );

  // Test bottom alignment.
  pushButton.SetProperty( Toolkit::PushButton::Property::ICON_ALIGNMENT, "BOTTOM" );

  application.SendNotification();
  application.Render();

  size.width = pushButton.GetRelayoutSize( Dimension::WIDTH );
  size.height = pushButton.GetRelayoutSize( Dimension::HEIGHT );

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
  DALI_TEST_EQUALS( size, compareSize, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  END_TEST;
}
