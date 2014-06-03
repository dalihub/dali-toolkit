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
using namespace Toolkit;


namespace
{
static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

static bool gSelectedSignalReceived = false;
static bool gSelected = false;

const Dali::TouchPoint pointDownInside( 0, TouchPoint::Down, 240, 400 );
const Dali::TouchPoint pointUpInside( 0, TouchPoint::Up, 240, 400 );
} // namespace


void rotating_selector_startup(void)
{
  test_return_value = TET_UNDEF;
}

void rotating_selector_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliRotatingSelectorNew(void)
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
  END_TEST;
}

namespace
{

// Callback test function
void OnSelectedSignal(RotatingSelector actor, bool selected)
{
  gSelectedSignalReceived = true;
  gSelected = selected;
}

}


int UtcDaliRotatingSelectorSetSelected(void)
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
  END_TEST;
}

int UtcDaliRotatingSelectorSetSelectedAndUnSelectedActor(void)
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

  END_TEST;
}


int UtcDaliRotatingSelectorSetSelectable(void)
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
  END_TEST;
}

int UtcDaliRotatingSelectorSignalSelected(void)
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

  END_TEST;
}
