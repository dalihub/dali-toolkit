//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

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

int UtcDaliSliderSignals(void)
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
  END_TEST;
}
