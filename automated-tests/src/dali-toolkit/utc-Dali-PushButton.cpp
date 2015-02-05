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

static bool gPushButtonToggleState = false;
bool PushButtonToggled( Button button, bool toggled )
{
  gPushButtonToggleState = toggled && ( toggled == static_cast<PushButton&>( button ).IsToggled() );
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
  BitmapImage imageData = BitmapImage::New( width, height, Pixel::RGBA8888 );

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

int UtcDaliPushButtonSetGetToggleButton(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetToggleButton");

  PushButton pushButton = PushButton::New();

  pushButton.SetToggleButton( true );

  DALI_TEST_CHECK( pushButton.IsToggleButton() );

  pushButton.SetToggleButton( false );

  DALI_TEST_CHECK( !pushButton.IsToggleButton() );

  pushButton.SetToggleButton( true );

  DALI_TEST_CHECK( pushButton.IsToggleButton() );
  END_TEST;
}

int UtcDaliPushButtonSetGetAutoRepeatingAndToggleButton(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetAutoRepeatingAndToggleButton");

  PushButton pushButton = PushButton::New();

  pushButton.SetAutoRepeating( true );
  pushButton.SetToggleButton( true );

  DALI_TEST_CHECK( pushButton.IsToggleButton() );
  DALI_TEST_CHECK( !pushButton.IsAutoRepeating() );

  pushButton.SetToggleButton( true );
  pushButton.SetAutoRepeating( true );

  DALI_TEST_CHECK( pushButton.IsAutoRepeating() );
  DALI_TEST_CHECK( !pushButton.IsToggleButton() );
  END_TEST;
}

int UtcDaliPushButtonSetGetToggled01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetToggled01");

  PushButton pushButton = PushButton::New();

  pushButton.SetToggleButton( true );
  pushButton.StateChangedSignal().Connect( &PushButtonToggled );

  gPushButtonToggleState = false;
  pushButton.SetToggled( true );

  DALI_TEST_CHECK( pushButton.IsToggled() );
  DALI_TEST_CHECK( gPushButtonToggleState );

  pushButton.SetToggled( false );

  DALI_TEST_CHECK( !pushButton.IsToggled() );
  DALI_TEST_CHECK( !gPushButtonToggleState );

  pushButton.SetToggled( true );

  DALI_TEST_CHECK( pushButton.IsToggled() );
  DALI_TEST_CHECK( gPushButtonToggleState );
  END_TEST;
}

int UtcDaliPushButtonSetGetToggled02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetGetToggled02");

  PushButton pushButton = PushButton::New();

  pushButton.SetToggleButton( false );
  pushButton.StateChangedSignal().Connect( &PushButtonToggled );

  gPushButtonToggleState = false;
  pushButton.SetToggled( true );

  DALI_TEST_CHECK( !pushButton.IsToggled() );
  DALI_TEST_CHECK( !gPushButtonToggleState );

  pushButton.SetToggled( false );

  DALI_TEST_CHECK( !pushButton.IsToggled() );
  DALI_TEST_CHECK( !gPushButtonToggleState );

  pushButton.SetToggled( true );

  DALI_TEST_CHECK( !pushButton.IsToggled() );
  DALI_TEST_CHECK( !gPushButtonToggleState );
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

int UtcDaliPushButtonSetImages(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonSetImages");

  Actor imageActor;

  Image image01 = CreateSolidColorImage( Color::RED, 10, 10 );
  ImageActor imageActor01 = CreateSolidColorActor( Color::RED );
  imageActor01.SetSize( 20.f, 20.f );

  Image image02 = CreateSolidColorImage( Color::RED, 30, 30 );
  ImageActor imageActor02 = CreateSolidColorActor( Color::RED );
  imageActor02.SetSize( 40.f, 40.f );

  Image image03 = CreateSolidColorImage( Color::RED, 50, 50 );
  ImageActor imageActor03 = CreateSolidColorActor( Color::RED );
  imageActor03.SetSize( 60.f, 60.f );

  Image image04 = CreateSolidColorImage( Color::RED, 70, 70 );
  ImageActor imageActor04 = CreateSolidColorActor( Color::RED );
  imageActor04.SetSize( 80.f, 80.f );

  Image image05 = CreateSolidColorImage( Color::RED, 90, 90 );
  ImageActor imageActor05 = CreateSolidColorActor( Color::RED );
  imageActor05.SetSize( 100.f, 100.f );

  Vector3 size;
  PushButton pushButton = PushButton::New();

  application.SendNotification();
  application.Render();

  // Just check if check box button size changes when a bigger image is set.

  pushButton.SetButtonImage( image01 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetButtonImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 10.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 10.f, TEST_LOCATION );

  pushButton.SetButtonImage( imageActor01 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetButtonImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 20.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 20.f, TEST_LOCATION );

  pushButton.SetBackgroundImage( image02 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetBackgroundImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 30.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 30.f, TEST_LOCATION );

  pushButton.SetBackgroundImage( imageActor02 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetBackgroundImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 40.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 40.f, TEST_LOCATION );

  pushButton.SetSelectedImage( image03 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetSelectedImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 50.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 50.f, TEST_LOCATION );

  pushButton.SetSelectedImage( imageActor03 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetSelectedImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 60.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 60.f, TEST_LOCATION );

  pushButton.SetDisabledBackgroundImage( image04 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetDisabledBackgroundImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 70.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 70.f, TEST_LOCATION );

  pushButton.SetDisabledBackgroundImage( imageActor04 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetDisabledBackgroundImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 80.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 80.f, TEST_LOCATION );

  pushButton.SetDisabledImage( image05 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetDisabledImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 90.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 90.f, TEST_LOCATION );

  pushButton.SetDisabledImage( imageActor05 );

  application.SendNotification();
  application.Render();

  size = pushButton.GetDisabledImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 100.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 100.f, TEST_LOCATION );
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

  TextView textView;

  pushButton.SetLabel( STR );

  textView = TextView::DownCast( pushButton.GetLabel() );
  DALI_TEST_CHECK( STR == textView.GetText() );

  TextView text = TextView::New( STR );
  pushButton.SetLabel( text );

  textView = TextView::DownCast( pushButton.GetLabel() );
  DALI_TEST_CHECK( STR == textView.GetText() );
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

int UtcDaliPushButtonToggled(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPushButtonToggled");

  PushButton pushButton = PushButton::New();
  pushButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  pushButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  pushButton.SetPosition( 240, 400 );
  pushButton.SetSize( 100, 100 );

  Stage::GetCurrent().Add( pushButton );

  application.SendNotification();
  application.Render();

  // connect to its touch signal
  pushButton.StateChangedSignal().Connect( &PushButtonToggled );

  Dali::Integration::TouchEvent event;

  // Test1. No toggle button.

  gPushButtonToggleState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonToggleState );

  // Set toggle property.
  pushButton.SetToggleButton( true );

  // Test2. Touch point down and up inside the button twice.
  gPushButtonToggleState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gPushButtonToggleState );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonToggleState );

  // Test3. Touch point down and up outside the button.

  gPushButtonToggleState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonToggleState );

  // Test4. Touch point down inside and up outside the button.

  gPushButtonToggleState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointLeave );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonToggleState );

  // Test5. Touch point down outside and up inside the button.

  gPushButtonToggleState = false;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointEnter );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( !gPushButtonToggleState );
  END_TEST;
}
