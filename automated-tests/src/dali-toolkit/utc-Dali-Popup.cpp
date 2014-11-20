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

void utc_dali_toolkit_popup_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_popup_cleanup(void)
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

const int RENDER_FRAME_INTERVAL = 10;                          ///< Duration of each frame in ms.
const int RENDER_ANIMATION_TEST_DURATION_MS = 1000;            ///< 1000ms to test animation
const int RENDER_ANIMATION_TEST_DURATION_FRAMES = RENDER_ANIMATION_TEST_DURATION_MS / RENDER_FRAME_INTERVAL; ///< equivalent frames.
const Vector3 DEFAULT_BUTTON_SIZE(100.0f, 50.0f, 0.0f);
const Dali::TouchPoint pointDownOutside( 0, TouchPoint::Down, 10.0f, 10.0f );
const Dali::TouchPoint pointUpOutside( 0, TouchPoint::Up, 10.0f, 10.0f );

/**
 * Counts how many descendents root Actor has, including
 * itself.
 *
 * @param[in] root The root actor to count from.
 * @return The number of descendents including root actor itself.
 */
int DescendentCount(const Actor& root)
{
  unsigned int numChildren = root.GetChildCount();

  int count = 1;

  for(unsigned int i=0; i<numChildren; ++i)
  {
    count += DescendentCount(root.GetChildAt(i));
  }

  return count;
}

bool HasAncestor(Actor child, Actor ancestor)
{
  while(child && child != ancestor)
  {
    child = child.GetParent();
  }

  return (child == ancestor);
}


static bool gHidden = false;

static void OnPopupHidden()
{
  gHidden = true;
}

static bool gTouchedOutside;

static void OnPopupTouchedOutside()
{
  gTouchedOutside = true;
}


} // anon namespace

int UtcDaliPopupNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPopupNew");

  // Create the Popup actor
  Popup popup;

  DALI_TEST_CHECK( !popup );

  popup = Popup::New();

  DALI_TEST_CHECK( popup );

  Popup popup2(popup);

  DALI_TEST_CHECK( popup2 == popup );

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    Popup popup = Popup::New();
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
  END_TEST;
}

int UtcDaliPopupDestructor(void)
{
  ToolkitTestApplication application;

  Popup* popup = new Popup();
  delete popup;

  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliPopupDownCast(void)
{
  ToolkitTestApplication application;

  Handle handle = Popup::New();

  Popup popup = Popup::DownCast( handle );

  DALI_TEST_CHECK( popup == handle );
  END_TEST;
}

int UtcDaliPopoupSetProperty(void)
{
  tet_infoline("UtcDaliPopoupSetProperty: ");
  ToolkitTestApplication application;

  Popup popup = Popup::New();

  //Test properties
  std::string testString = "Hello World";
  popup.SetProperty(popup.GetPropertyIndex("title"), testString);
  DALI_TEST_EQUALS( testString, popup.GetTitle().GetText(), TEST_LOCATION );
  END_TEST;
}


int UtcDaliPopupSetBackgroundImage(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupSetBackgroundImage");

  // Create the Popup actor
  Popup popup = Popup::New();
  Stage::GetCurrent().Add( popup );

  ImageActor image = CreateSolidColorActor( Color::RED );
  DALI_TEST_CHECK( !image.GetParent() );
  popup.SetBackgroundImage(image);
  DALI_TEST_CHECK( image.GetParent() );
  END_TEST;
}

int UtcDaliPopupSetTitle(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupSetTitle");

  // Create the Popup actor
  Popup popup = Popup::New();
  Stage::GetCurrent().Add( popup );
  // Put in show state so it's layer is connected to popup (for ancestor check).
  popup.SetState(Popup::POPUP_SHOW, 0.0f);

  TextView titleActor = TextView::New();
  titleActor.SetText("title");

  DALI_TEST_CHECK( !popup.GetTitle() );
  popup.SetTitle(titleActor);
  DALI_TEST_CHECK( popup.GetTitle() == titleActor );
  DALI_TEST_CHECK( (popup.GetTitle()) && (popup.GetTitle().GetText() == "title") );
  // verify titleActor is actually inside popup, and not elsewhere on stage, or off even.
  DALI_TEST_CHECK( HasAncestor(titleActor, popup) );

  TextView titleActor2 = TextView::New();
  titleActor2.SetText("anothertitle");
  popup.SetTitle(titleActor2);
  DALI_TEST_CHECK( popup.GetTitle() != titleActor );
  DALI_TEST_CHECK( popup.GetTitle() == titleActor2 );
  DALI_TEST_CHECK( (popup.GetTitle()) && (popup.GetTitle().GetText() == "anothertitle") );
  // verify titleActor is actually inside popup, and not elsewhere on stage, or off even.
  DALI_TEST_CHECK( HasAncestor(titleActor2, popup) );
  END_TEST;
}

int UtcDaliPopupSetTitleText(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupSetTitleText");

  // Create the Popup actor
  Popup popup = Popup::New();
  Stage::GetCurrent().Add( popup );
  // Put in show state so it's layer is connected to popup (for ancestor check).
  popup.SetState(Popup::POPUP_SHOW, 0.0f);

  TextView titleActor = TextView::New();
  titleActor.SetText("title");

  DALI_TEST_CHECK( !popup.GetTitle() );
  popup.SetTitle(titleActor);
  DALI_TEST_CHECK( popup.GetTitle() == titleActor );
  DALI_TEST_CHECK( (popup.GetTitle()) && (popup.GetTitle().GetText() == "title") );
  // verify titleActor is actually inside popup, and not elsewhere on stage, or off even.
  DALI_TEST_CHECK( HasAncestor(titleActor, popup) );

  // this text should replace titleImage actor.
  popup.SetTitle("newtext");
  DALI_TEST_CHECK( popup.GetTitle() != titleActor );
  DALI_TEST_CHECK( (popup.GetTitle()) && (popup.GetTitle().GetText() == "newtext") );
  // verify titleActor is no longer inside popup. (been displaced by newtext actor)
  DALI_TEST_CHECK( !HasAncestor(titleActor, popup) );
  END_TEST;
}

int UtcDaliPopupAddButton(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupAddButton");

  // Create the Popup actor
  Popup popup = Popup::New();
  Stage::GetCurrent().Add( popup );
  // Put in show state so it's layer is connected to popup (for ancestor check).
  popup.SetState(Popup::POPUP_SHOW, 0.0f);

  PushButton button = PushButton::New();
  DALI_TEST_CHECK( !HasAncestor(button, popup) );
  popup.AddButton(button);
  // Hide and then re-show popup to cause button to be rearranged and added to popup.
  popup.SetState( Popup::POPUP_HIDE, 0.0f );
  popup.SetState( Popup::POPUP_SHOW, 0.0f );
  DALI_TEST_CHECK( HasAncestor(button, popup) );
  END_TEST;
}

int UtcDaliPopupSetState(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupSetState");

  // Create the Popup actor
  Popup popup = Popup::New();

  ImageActor backgroundImage = CreateSolidColorActor( Color::RED );
  popup.SetBackgroundImage(backgroundImage);

  // Showing/Hiding popup, results in all child Actors being
  // connected/disconnected from the stage.
  DALI_TEST_CHECK( !backgroundImage.OnStage() );
  popup.SetState(Popup::POPUP_SHOW, 0.0f);
  DALI_TEST_CHECK( backgroundImage.OnStage() );
  DALI_TEST_EQUALS( Popup::POPUP_SHOW, popup.GetState(), TEST_LOCATION );
  popup.SetState(Popup::POPUP_HIDE, 0.0f);
  DALI_TEST_CHECK( !backgroundImage.OnStage() );
  DALI_TEST_EQUALS( Popup::POPUP_HIDE, popup.GetState(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPopupSetStateSlow(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupSetStateSlow");

  // Create the Popup actor
  Popup popup = Popup::New();

  ImageActor backgroundImage = CreateSolidColorActor( Color::RED );
  popup.SetBackgroundImage(backgroundImage);

  // Showing/Hiding popup, results in all child Actors being
  // connected/disconnected from the stage.
  DALI_TEST_CHECK( !backgroundImage.OnStage() );
  popup.SetState(Popup::POPUP_SHOW, 0.0f);
  DALI_TEST_CHECK( backgroundImage.OnStage() );

  // Hide slowly
  popup.SetState(Popup::POPUP_HIDE);

  // Wait for a while (allow animation to complete), and then check state.
  for(int i = 0; i < RENDER_ANIMATION_TEST_DURATION_FRAMES; i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
  }

  DALI_TEST_CHECK( !backgroundImage.OnStage() );
  END_TEST;
}



int UtcDaliPopupShowHide(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupShowHide");

  // Create the Popup actor
  Popup popup = Popup::New();
  popup.HiddenSignal().Connect( &OnPopupHidden );

  ImageActor backgroundImage = CreateSolidColorActor( Color::RED );
  popup.SetBackgroundImage(backgroundImage);

  PushButton button1 = PushButton::New();
  PushButton button2 = PushButton::New();
  button1.SetPreferredSize(DEFAULT_BUTTON_SIZE.GetVectorXY());
  popup.AddButton(button1);
  button2.SetPreferredSize(DEFAULT_BUTTON_SIZE.GetVectorXY());
  popup.AddButton(button2);

  // Showing/Hiding popup, results in all child Actors being
  // connected/disconnected from the stage.
  DALI_TEST_CHECK( !backgroundImage.OnStage() );

  // Show
  // Note: in most popup animation implementations show would result in
  // popup being onstage immediately following Show(). However we can't
  // assume for all. e.g. If one creates a animation with a delay.
  popup.Show();

  // Wait for a while (allow animation to complete), and then check state.
  for(int i = 0; i < RENDER_ANIMATION_TEST_DURATION_FRAMES; i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
  }

  DALI_TEST_CHECK( backgroundImage.OnStage() );

  // Hide
  gHidden = false;
  popup.Hide();

  // Wait for a while (allow animation to complete), and then check state.
  for(int i = 0; i < RENDER_ANIMATION_TEST_DURATION_FRAMES; i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
  }

  DALI_TEST_CHECK( !backgroundImage.OnStage() );
  DALI_TEST_CHECK( gHidden );
  END_TEST;
}

int UtcDaliPopupShowHideTail(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupShowHideTail");

  // Create the Popup actor
  Popup popup = Popup::New();
  Stage::GetCurrent().Add( popup );
  popup.SetState(Popup::POPUP_SHOW, 0.0f);

  popup.HideTail();
  int withoutTailCount = DescendentCount(popup);

  popup.ShowTail(ParentOrigin::BOTTOM_CENTER);
  int withTailCount = DescendentCount(popup);

  // There should be more actors if the Tail has been added.
  DALI_TEST_CHECK( withTailCount > withoutTailCount );

  // Hide again
  popup.HideTail();
  int withoutTailCount2 = DescendentCount(popup);

  DALI_TEST_CHECK( withTailCount > withoutTailCount2 );
  END_TEST;
}

int UtcDaliPopupOnTouchedOutside(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliPopupOnTouchedOutside");

  // Create the Popup actor
  Popup popup = Popup::New();
  Stage::GetCurrent().Add( popup );
  popup.SetParentOrigin(ParentOrigin::CENTER);
  popup.SetAnchorPoint(ParentOrigin::CENTER);
  popup.SetState(Popup::POPUP_SHOW, 0.0f);
  popup.OutsideTouchedSignal().Connect( &OnPopupTouchedOutside );

  application.SendNotification();
  application.Render();

  gTouchedOutside = false;
  Dali::Integration::TouchEvent event;

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointDownOutside );
  application.ProcessEvent( event );

  application.SendNotification();
  application.Render();

  event = Dali::Integration::TouchEvent();
  event.AddPoint( pointUpOutside );
  application.ProcessEvent( event );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gTouchedOutside);
  END_TEST;
}
