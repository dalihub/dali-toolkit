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

void dali_slider_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_slider_cleanup(void)
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

int UtcDaliSliderNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliSliderNew");

  // Create the Slider actor
  Slider slider;

  DALI_TEST_CHECK( !slider );

  slider = Slider::New();

  DALI_TEST_CHECK( slider );

  Slider slider2(slider);

  DALI_TEST_CHECK( slider2 == slider );

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    Slider slider = Slider::New();
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
  END_TEST;
}

int UtcDaliSliderDestructor(void)
{
  ToolkitTestApplication application;

  Slider* slider = new Slider();
  delete slider;

  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliSliderDownCast(void)
{
  ToolkitTestApplication application;

  Handle handle = Slider::New();

  Slider slider = Slider::DownCast( handle );

  DALI_TEST_CHECK( slider == handle );
  END_TEST;
}

static bool gSliderValueChangedCallBackCalled=false;
static bool OnSliderValueChanged( Slider slider, float value )
{
  gSliderValueChangedCallBackCalled = true;
  return true;
}

static bool gSliderMarkCallBackCalled=false;
static bool OnSliderMark( Slider slider, int value )
{
  gSliderMarkCallBackCalled = true;
  return true;
}

static bool gSliderSlidingFinishedCallBackCalled=false;
static bool OnSlidingFinished( Slider slider, float value )
{
  gSliderSlidingFinishedCallBackCalled = true;
  return true;
}

int UtcDaliSliderSignals1(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliSliderSignals");

  // Create the Popup actor
  Slider slider = Slider::New();
  Stage::GetCurrent().Add( slider );
  slider.SetParentOrigin(ParentOrigin::TOP_LEFT);
  slider.SetAnchorPoint(ParentOrigin::TOP_LEFT);
  slider.SetSize( Vector2( Stage::GetCurrent().GetSize().x, 20.0f ) );
  slider.SetPosition( 0.0f, 0.0f );

  const float MIN_BOUND = 0.0f;
  const float MAX_BOUND = 1.0f;
  const int NUM_MARKS = 5;
  Property::Array marks;
  for( int i = 0; i < NUM_MARKS; ++i )
  {
    marks.PushBack( MIN_BOUND + ( static_cast<float>(i) / ( NUM_MARKS - 1) ) * ( MAX_BOUND - MIN_BOUND ) );
  }
  slider.SetProperty( Slider::Property::MARKS, marks );
  slider.SetProperty( Slider::Property::MARK_TOLERANCE, 0.1f );

  slider.ValueChangedSignal().Connect( &OnSliderValueChanged );
  slider.MarkReachedSignal().Connect( &OnSliderMark );
  slider.SlidingFinishedSignal().Connect( &OnSlidingFinished );

  application.SendNotification();
  application.Render();

  gSliderValueChangedCallBackCalled = false;
  gSliderMarkCallBackCalled = false;
  gSliderSlidingFinishedCallBackCalled = false;

  {
    Dali::Integration::TouchEvent event = Dali::Integration::TouchEvent();
    Integration::Point pointDown;
    pointDown.SetState( PointState::DOWN );
    pointDown.SetScreenPosition( Vector2( 10.0f, 10.0f ) );
    event.AddPoint( pointDown );

    application.ProcessEvent( event );
    application.SendNotification();
    application.Render();
  }

  for( int i = 0; i < 5; ++i )
  {
    Dali::Integration::TouchEvent event = Dali::Integration::TouchEvent();
    Integration::Point pointMotion;
    pointMotion.SetState( PointState::MOTION );
    pointMotion.SetScreenPosition( Vector2( 10.0f + i * 10.0f, 10.0f ) );
    event.AddPoint( pointMotion );

    application.ProcessEvent( event );
    application.SendNotification();
    application.Render();
  }

  {
    Dali::Integration::TouchEvent event = Dali::Integration::TouchEvent();
    Integration::Point pointUp;
    pointUp.SetState( PointState::UP );
    pointUp.SetScreenPosition( Vector2( 10.0f, 10.0f ) );
    event.AddPoint( pointUp );

    application.ProcessEvent( event );
    application.SendNotification();
    application.Render();
  }

  DALI_TEST_CHECK(gSliderValueChangedCallBackCalled);
  DALI_TEST_CHECK(gSliderMarkCallBackCalled);
  DALI_TEST_CHECK(gSliderSlidingFinishedCallBackCalled);
  END_TEST;
}

namespace
{
bool gSliderSignal=false;
struct SliderSignalFunctor
{
  SliderSignalFunctor()
  {
  }

  void operator()()
  {
    gSliderSignal = true;
  }
};
} // anonymous



int UtcDaliSliderSignals2(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliSliderSignals1");

  // Create the Popup actor
  Slider slider = Slider::New();
  Stage::GetCurrent().Add( slider );
  slider.SetParentOrigin(ParentOrigin::TOP_LEFT);
  slider.SetAnchorPoint(ParentOrigin::TOP_LEFT);
  slider.SetSize( Vector2( Stage::GetCurrent().GetSize().x, 20.0f ) );
  slider.SetPosition( 0.0f, 0.0f );

  const float MIN_BOUND = 0.0f;
  const float MAX_BOUND = 1.0f;
  const int NUM_MARKS = 5;
  Property::Array marks;
  for( int i = 0; i < NUM_MARKS; ++i )
  {
    marks.PushBack( MIN_BOUND + ( static_cast<float>(i) / ( NUM_MARKS - 1) ) * ( MAX_BOUND - MIN_BOUND ) );
  }
  slider.SetProperty( Slider::Property::MARKS, marks );
  slider.SetProperty( Slider::Property::MARK_TOLERANCE, 0.1f );

  gSliderSignal = false;
  ConnectionTracker* testTracker = new ConnectionTracker();
  slider.ConnectSignal( testTracker, "valueChanged",   SliderSignalFunctor() );

  application.SendNotification();
  application.Render();

  gSliderValueChangedCallBackCalled = false;
  gSliderMarkCallBackCalled = false;

  Dali::Integration::TouchEvent event;

  event = Dali::Integration::TouchEvent();

  Integration::Point pointDown;
  pointDown.SetState( PointState::DOWN );
  pointDown.SetScreenPosition( Vector2( 10.0f, 10.0f ) );
  event.AddPoint( pointDown );

  for( int i = 0; i < 5; ++i )
  {
    Integration::Point pointMotion;
    pointMotion.SetState( PointState::MOTION );
    pointMotion.SetScreenPosition( Vector2( 10.0f + i * 10.0f, 10.0f ) );
    event.AddPoint( pointMotion );
  }

  Integration::Point pointUp;
  pointUp.SetState( PointState::UP );
  pointUp.SetScreenPosition( Vector2( 10.0f, 10.0f ) );
  event.AddPoint( pointUp );

  application.ProcessEvent( event );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSliderSignal);
  END_TEST;
}

int UtcDaliSetPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliSetPropertyP" );

  Slider slider = Slider::New();
  slider.SetParentOrigin(ParentOrigin::TOP_LEFT);
  slider.SetAnchorPoint(ParentOrigin::TOP_LEFT);
  slider.SetSize( Vector2( Stage::GetCurrent().GetSize().x, 20.0f ) );
  slider.SetPosition( 0.0f, 0.0f );

  Stage::GetCurrent().Add(slider);
  application.SendNotification();
  application.Render();

  slider.SetProperty(Slider::Property::LOWER_BOUND,        1.0f);
  slider.SetProperty(Slider::Property::UPPER_BOUND,        5.0f);
  slider.SetProperty(Slider::Property::VALUE,              3.0f);
  slider.SetProperty(Slider::Property::DISABLED_COLOR,     Color::BLACK);
  slider.SetProperty(Slider::Property::VALUE_PRECISION,    4);
  slider.SetProperty(Slider::Property::SHOW_POPUP,         true);
  slider.SetProperty(Slider::Property::SHOW_VALUE,         true);
  slider.SetProperty(Slider::Property::MARKS,              false);
  slider.SetProperty(Slider::Property::SNAP_TO_MARKS,      false);
  slider.SetProperty(Slider::Property::MARK_TOLERANCE,     0.5f);

  float lb = slider.GetProperty<float>(Slider::Property::LOWER_BOUND);
  DALI_TEST_EQUALS(lb, 1.0f, TEST_LOCATION);
  float ub = slider.GetProperty<float>(Slider::Property::UPPER_BOUND);
  DALI_TEST_EQUALS(ub, 5.0f, TEST_LOCATION);
  float val = slider.GetProperty<float>(Slider::Property::VALUE);
  DALI_TEST_EQUALS(val, 3.0f, TEST_LOCATION);
  Vector4 color = slider.GetProperty<Vector4>(Slider::Property::DISABLED_COLOR);
  DALI_TEST_EQUALS( color, Color::BLACK, TEST_LOCATION );
  int precision = slider.GetProperty<int>(Slider::Property::VALUE_PRECISION);
  DALI_TEST_EQUALS( precision, 4, TEST_LOCATION);
  bool showPopup = slider.GetProperty<bool>(Slider::Property::SHOW_POPUP);
  DALI_TEST_EQUALS( showPopup, true , TEST_LOCATION);
  bool showValue = slider.GetProperty<bool>(Slider::Property::SHOW_VALUE);
  DALI_TEST_EQUALS( showValue, true, TEST_LOCATION );
  bool marks = slider.GetProperty<bool>(Slider::Property::MARKS);
  DALI_TEST_EQUALS( marks, false, TEST_LOCATION );
  bool snapToMarks = slider.GetProperty<bool>(Slider::Property::SNAP_TO_MARKS);
  DALI_TEST_EQUALS( snapToMarks, false, TEST_LOCATION );
  float tolerance = slider.GetProperty<float>(Slider::Property::MARK_TOLERANCE);
  DALI_TEST_EQUALS( tolerance, 0.5f, TEST_LOCATION );

  {
    Property::Map map;
    map["rendererType"] = "image";
    map["size"] = Vector2(200, 200);
    map["url"] = "track2.png";
    slider.SetProperty(Slider::Property::TRACK_VISUAL,       map);
    map["url"] = "handle2.png";
    slider.SetProperty(Slider::Property::HANDLE_VISUAL,      map);
    map["url"] = "progress2.png";
    slider.SetProperty(Slider::Property::PROGRESS_VISUAL,    map);
    map["url"] = "popup2.png";
    slider.SetProperty(Slider::Property::POPUP_VISUAL,       map);
    map["url"] = "popupArrow2.png";
    slider.SetProperty(Slider::Property::POPUP_ARROW_VISUAL, map);

    Property::Value value = slider.GetProperty(Slider::Property::TRACK_VISUAL);
    Property::Map* resultMap = value.GetMap();
    DALI_TEST_CHECK( resultMap );
    Property::Value* url = resultMap->Find("url");
    DALI_TEST_CHECK( url ) ;
    DALI_TEST_EQUALS( *url, "track2.png", TEST_LOCATION );

    value = slider.GetProperty(Slider::Property::HANDLE_VISUAL);
    resultMap = value.GetMap();
    DALI_TEST_CHECK( resultMap );
    url = resultMap->Find("url");
    DALI_TEST_CHECK( url ) ;
    DALI_TEST_EQUALS( *url, "handle2.png", TEST_LOCATION );

    value = slider.GetProperty(Slider::Property::PROGRESS_VISUAL);
    resultMap = value.GetMap();
    DALI_TEST_CHECK( resultMap );
    url = resultMap->Find("url");
    DALI_TEST_CHECK( url ) ;
    DALI_TEST_EQUALS( *url, "progress2.png", TEST_LOCATION );

    value = slider.GetProperty(Slider::Property::POPUP_VISUAL);
    resultMap = value.GetMap();
    DALI_TEST_CHECK( resultMap );
    url = resultMap->Find("url");
    DALI_TEST_CHECK( url ) ;
    DALI_TEST_EQUALS( *url, "popup2.png", TEST_LOCATION );

    value = slider.GetProperty(Slider::Property::POPUP_ARROW_VISUAL);
    resultMap = value.GetMap();
    DALI_TEST_CHECK( resultMap );
    url = resultMap->Find("url");
    DALI_TEST_CHECK( url ) ;
    DALI_TEST_EQUALS( *url, "popupArrow2.png", TEST_LOCATION );
  }

  END_TEST;
}
