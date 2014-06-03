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
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-toolkit/public-api/controls/slider/slider.h>

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Toolkit;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliSliderNew();
static void UtcDaliSliderDestructor();
static void UtcDaliSliderDownCast();
static void UtcDaliSliderSignals();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliSliderNew, POSITIVE_TC_IDX },
    { UtcDaliSliderDestructor, POSITIVE_TC_IDX },
    { UtcDaliSliderDownCast, POSITIVE_TC_IDX },
    { UtcDaliSliderSignals, POSITIVE_TC_IDX },
    { NULL, 0 }
  };
}

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

static void UtcDaliSliderNew()
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
}

static void UtcDaliSliderDestructor()
{
  ToolkitTestApplication application;

  Slider* slider = new Slider();
  delete slider;

  DALI_TEST_CHECK( true );
}

static void UtcDaliSliderDownCast()
{
  ToolkitTestApplication application;

  Handle handle = Slider::New();

  Slider slider = Slider::DownCast( handle );

  DALI_TEST_CHECK( slider == handle );
}

static bool gSliderValueChangedCallBackCalled;
static bool OnSliderValueChanged( Slider slider, float value )
{
  gSliderValueChangedCallBackCalled = true;
  return true;
}

static bool gSliderMarkCallBackCalled;
static bool OnSliderMark( Slider slider, int value )
{
  gSliderMarkCallBackCalled = true;
  return true;
}

static void UtcDaliSliderSignals()
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliSliderSignals");

  // Create the Popup actor
  Slider slider = Slider::New();
  Stage::GetCurrent().Add( slider );
  slider.SetParentOrigin(ParentOrigin::TOP_LEFT);
  slider.SetAnchorPoint(ParentOrigin::TOP_LEFT);
  slider.SetSize( Stage::GetCurrent().GetSize().x, 20.0f );
  slider.SetPosition( 0.0f, 0.0f );

  const float MIN_BOUND = 0.0f;
  const float MAX_BOUND = 1.0f;
  const int NUM_MARKS = 5;
  Property::Array marks;
  for( int i = 0; i < NUM_MARKS; ++i )
  {
    marks.push_back( MIN_BOUND + ( static_cast<float>(i) / ( NUM_MARKS - 1) ) * ( MAX_BOUND - MIN_BOUND ) );
  }
  slider.SetProperty( Slider::MARKS_PROPERTY, marks );
  slider.SetProperty( Slider::MARK_TOLERANCE_PROPERTY, 0.1f );

  slider.ValueChangedSignal().Connect( &OnSliderValueChanged );
  slider.MarkSignal().Connect( &OnSliderMark );

  application.SendNotification();
  application.Render();

  gSliderValueChangedCallBackCalled = false;
  gSliderMarkCallBackCalled = false;

  Dali::Integration::TouchEvent event;

  event = Dali::Integration::TouchEvent();

  const Dali::TouchPoint pointDown( 0, TouchPoint::Down, 10.0f, 10.0f );
  event.AddPoint( pointDown );

  for( int i = 0; i < 5; ++i )
  {
    const Dali::TouchPoint pointDown( 0, TouchPoint::Motion, 10.0f + i * 10.0f, 10.0f );
    event.AddPoint( pointDown );
  }

  const Dali::TouchPoint pointUp( 0, TouchPoint::Up, 50.0f, 10.0f );
  event.AddPoint( pointUp );

  application.ProcessEvent( event );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSliderValueChangedCallBackCalled);
  DALI_TEST_CHECK(gSliderMarkCallBackCalled);
}
