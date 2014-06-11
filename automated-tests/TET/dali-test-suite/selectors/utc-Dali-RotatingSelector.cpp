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
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-toolkit-test-suite-utils.h>

#include <dali-toolkit/public-api/controls/selectors/rotating-selector.h>

using namespace Dali;
using namespace Toolkit;

static void Startup();
static void Cleanup();

namespace
{
static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}
} // namespace

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliRotatingSelectorNew();
static void UtcDaliRotatingSelectorSetSelected();
static void UtcDaliRotatingSelectorSetSelectedAndUnSelectedActor();
static void UtcDaliRotatingSelectorSetSelectable();
static void UtcDaliRotatingSelectorSignalSelected();

static bool gSelectedSignalReceived = false;
static bool gSelected = false;

const Dali::TouchPoint pointDownInside( 0, TouchPoint::Down, 240, 400 );
const Dali::TouchPoint pointUpInside( 0, TouchPoint::Up, 240, 400 );

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliRotatingSelectorNew, POSITIVE_TC_IDX },
    { UtcDaliRotatingSelectorSetSelected, POSITIVE_TC_IDX },
    { UtcDaliRotatingSelectorSetSelectedAndUnSelectedActor, POSITIVE_TC_IDX },
    { UtcDaliRotatingSelectorSetSelectable, POSITIVE_TC_IDX },
    { UtcDaliRotatingSelectorSignalSelected, POSITIVE_TC_IDX },
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


static void UtcDaliRotatingSelectorNew()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliRotatingSelectorNew");
  RotatingSelector selector;

  DALI_TEST_CHECK(!selector);

  Actor unSelectedActor = Actor::New();
  Actor selectedActor = Actor::New();

  selector = RotatingSelector::New(unSelectedActor, selectedActor);

  DALI_TEST_CHECK(selector);

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect(&TestCallback);
  {
    RotatingSelector selector = RotatingSelector::New(unSelectedActor, selectedActor);
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
}

// Callback test function
void OnSelectedSignal(RotatingSelector actor, bool selected)
{
  gSelectedSignalReceived = true;
  gSelected = selected;
}

static void UtcDaliRotatingSelectorSetSelected()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliRotatingSelectorSetSelected");

  BitmapImage img = BitmapImage::New( 1,1 );
  ImageActor unSelectedActor = ImageActor::New( img );
  ImageActor selectedActor = ImageActor::New( img );

  RotatingSelector selector = RotatingSelector::New(unSelectedActor, selectedActor);

  selector.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  selector.SetParentOrigin( ParentOrigin::TOP_LEFT );
  selector.SetPosition( 240, 400 );
  selector.SetSize( 100, 100 );

  // connect to its selected signal
  selector.SelectedSignal().Connect( &OnSelectedSignal );

  Stage::GetCurrent().Add( selector );

  gSelectedSignalReceived = false;
  gSelected = false;

  selector.SetSelected(true);
  application.SendNotification();
  application.Render(1000);
  application.SendNotification();
  application.Render(1000);
  application.SendNotification();
  application.Render(1000);

  DALI_TEST_CHECK( selector.IsSelected() );
  DALI_TEST_CHECK( gSelectedSignalReceived );
  DALI_TEST_CHECK( gSelected );

  gSelectedSignalReceived = false;
  gSelected = false;

  selector.SetSelected(false);
  application.SendNotification();
  application.Render(1000);
  application.SendNotification();
  application.Render(1000);
  application.SendNotification();
  application.Render(1000);

  DALI_TEST_CHECK( gSelectedSignalReceived );
  DALI_TEST_CHECK( !gSelected );
  DALI_TEST_CHECK( !selector.IsSelected() );
}

static void UtcDaliRotatingSelectorSetSelectedAndUnSelectedActor()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliRotatingSelectorSetSelectedAndUnSelectedActor");

  BitmapImage img = BitmapImage::New( 1,1 );
  ImageActor actor1 = ImageActor::New( img );
  ImageActor actor2 = ImageActor::New( img );

  RotatingSelector selector = RotatingSelector::New(actor1, actor2);
  Stage::GetCurrent().Add( selector );

  ImageActor unSelectedActor = ImageActor::New( img );
  ImageActor selectedActor = ImageActor::New( img );

  selector.SetSelectedActor(selectedActor);

  Actor actor3 = selector.GetSelectedActor();
  DALI_TEST_CHECK( selectedActor == actor3 );

  selector.SetUnSelectedActor(unSelectedActor);

  Actor actor4 = selector.GetUnSelectedActor();
  DALI_TEST_CHECK( unSelectedActor == actor4 );

}


static void UtcDaliRotatingSelectorSetSelectable()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliRotatingSelectorSetSelectable");

  BitmapImage img = BitmapImage::New( 1,1 );
  ImageActor unSelectedActor = ImageActor::New( img );
  ImageActor selectedActor = ImageActor::New( img );

  RotatingSelector selector = RotatingSelector::New(unSelectedActor, selectedActor);

  selector.SetSelectable(true);
  DALI_TEST_CHECK( selector.IsSelectable() );

  selector.SetSelectable(false);
  DALI_TEST_CHECK( !selector.IsSelectable() );
}

static void UtcDaliRotatingSelectorSignalSelected()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliRotatingSelectorSignalSelected");

  BitmapImage img = BitmapImage::New( 1,1 );
  ImageActor unSelectedActor = ImageActor::New( img );
  ImageActor selectedActor = ImageActor::New( img );

  RotatingSelector selector = RotatingSelector::New(unSelectedActor, selectedActor);

  selector.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  selector.SetParentOrigin( ParentOrigin::TOP_LEFT );
  selector.SetPosition( 240, 400 );
  selector.SetSize( 100, 100 );

  // connect to its selected signal
  selector.SelectedSignal().Connect( &OnSelectedSignal );

  Stage::GetCurrent().Add( selector );

  DALI_TEST_CHECK( !selector.IsSelected() );

  gSelectedSignalReceived = false;
  gSelected = false;

  application.SendNotification();
  application.Render(1000);
  application.SendNotification();
  application.Render(1000);

  //Test using touch event simulation
  Dali::Integration::TouchEvent event;

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownInside );
  application.ProcessEvent( event );

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpInside );
  application.ProcessEvent( event );

  application.SendNotification();
  application.Render(1000);
  application.SendNotification();
  application.Render(1000);

  DALI_TEST_CHECK( selector.IsSelected() );
  DALI_TEST_CHECK( gSelectedSignalReceived );
  DALI_TEST_CHECK( gSelected );

}
