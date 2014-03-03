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
#include <dali-toolkit/dali-toolkit.h>

#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/mouse-wheel-event-integ.h>
#include <dali/integration-api/events/long-press-gesture-event.h>
#include <dali/integration-api/events/pinch-gesture-event.h>
#include <dali/integration-api/events/pan-gesture-event.h>
#include <dali/integration-api/events/tap-gesture-event.h>
#include <dali/integration-api/events/touch-event-integ.h>

#include <dali-toolkit-test-suite-utils.h>

#include "dummy-control.h"

using namespace Dali;
using namespace Dali::Toolkit;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliControlImplNew, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlImplTypeRegistry, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlImplEnableGestureDetector, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlImplDisableGestureDetector, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlImplOnGestureMethods, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlImplChildAddAndRemove, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlImplStageConnection, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlImplSizeSet, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlImplSizeAnimation, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlImplTouchEvent, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlImplMouseWheelEvent, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlImplKeyEvent, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlImplStyleChange, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlImplKeyInputFocusGained, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlImplKeyInputFocusLost, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliControlImplNew()
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control;

  DALI_TEST_CHECK( !Control::DownCast(control) );

  control = ControlImpl::New();

  DALI_TEST_CHECK( Control::DownCast(control) );
}

static void UtcDaliControlImplTypeRegistry()
{
  ToolkitTestApplication application;

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "Control" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  // Check if it's a control
  DALI_TEST_CHECK( Control::DownCast(handle) );
}

static void UtcDaliControlImplEnableGestureDetector()
{
  ToolkitTestApplication application;

  // Enable individually
  {
    DummyControl dummy = DummyControl::New();
    DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

    DALI_TEST_CHECK( !dummyImpl.GetPinchGestureDetector() );
    dummyImpl.EnableGestureDetection(Gesture::Pinch);
    DALI_TEST_CHECK( dummyImpl.GetPinchGestureDetector() );

    DALI_TEST_CHECK( !dummyImpl.GetPanGestureDetector() );
    dummyImpl.EnableGestureDetection(Gesture::Pan);
    DALI_TEST_CHECK( dummyImpl.GetPanGestureDetector() );

    DALI_TEST_CHECK( !dummyImpl.GetTapGestureDetector() );
    dummyImpl.EnableGestureDetection(Gesture::Tap);
    DALI_TEST_CHECK( dummyImpl.GetTapGestureDetector() );

    DALI_TEST_CHECK( !dummyImpl.GetLongPressGestureDetector() );
    dummyImpl.EnableGestureDetection(Gesture::LongPress);
    DALI_TEST_CHECK( dummyImpl.GetLongPressGestureDetector() );
  }

  // Enable All
  {
    DummyControl dummy = DummyControl::New();
    DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

    DALI_TEST_CHECK( !dummyImpl.GetPinchGestureDetector() );
    DALI_TEST_CHECK( !dummyImpl.GetPanGestureDetector() );
    DALI_TEST_CHECK( !dummyImpl.GetTapGestureDetector() );
    DALI_TEST_CHECK( !dummyImpl.GetLongPressGestureDetector() );

    dummyImpl.EnableGestureDetection( Gesture::Type(Gesture::Pinch | Gesture::Pan | Gesture::Tap | Gesture::LongPress) );

    DALI_TEST_CHECK( dummyImpl.GetPinchGestureDetector() );
    DALI_TEST_CHECK( dummyImpl.GetPanGestureDetector() );
    DALI_TEST_CHECK( dummyImpl.GetTapGestureDetector() );
    DALI_TEST_CHECK( dummyImpl.GetLongPressGestureDetector() );

    // Enable when already enabled

    dummyImpl.EnableGestureDetection( Gesture::Type(Gesture::Pinch | Gesture::Pan | Gesture::Tap | Gesture::LongPress) );

    DALI_TEST_CHECK( dummyImpl.GetPinchGestureDetector() );
    DALI_TEST_CHECK( dummyImpl.GetPanGestureDetector() );
    DALI_TEST_CHECK( dummyImpl.GetTapGestureDetector() );
    DALI_TEST_CHECK( dummyImpl.GetLongPressGestureDetector() );
  }
}

static void UtcDaliControlImplDisableGestureDetector()
{
  ToolkitTestApplication application;

  // Disable individually
  {
    DummyControl dummy = DummyControl::New();
    DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

    dummyImpl.EnableGestureDetection( Gesture::Type(Gesture::Pinch | Gesture::Pan | Gesture::Tap | Gesture::LongPress) );

    DALI_TEST_CHECK( dummyImpl.GetPinchGestureDetector() );
    dummyImpl.DisableGestureDetection(Gesture::Pinch);
    DALI_TEST_CHECK( !dummyImpl.GetPinchGestureDetector() );

    DALI_TEST_CHECK( dummyImpl.GetPanGestureDetector() );
    dummyImpl.DisableGestureDetection(Gesture::Pan);
    DALI_TEST_CHECK( !dummyImpl.GetPanGestureDetector() );

    DALI_TEST_CHECK( dummyImpl.GetTapGestureDetector() );
    dummyImpl.DisableGestureDetection(Gesture::Tap);
    DALI_TEST_CHECK( !dummyImpl.GetTapGestureDetector() );

    DALI_TEST_CHECK( dummyImpl.GetLongPressGestureDetector() );
    dummyImpl.DisableGestureDetection(Gesture::LongPress);
    DALI_TEST_CHECK( !dummyImpl.GetLongPressGestureDetector() );
  }

  // Disable All
  {
    DummyControl dummy = DummyControl::New();
    DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

    dummyImpl.EnableGestureDetection( Gesture::Type(Gesture::Pinch | Gesture::Pan | Gesture::Tap | Gesture::LongPress) );

    DALI_TEST_CHECK( dummyImpl.GetPinchGestureDetector() );
    DALI_TEST_CHECK( dummyImpl.GetPanGestureDetector() );
    DALI_TEST_CHECK( dummyImpl.GetTapGestureDetector() );
    DALI_TEST_CHECK( dummyImpl.GetLongPressGestureDetector() );

    dummyImpl.DisableGestureDetection( Gesture::Type(Gesture::Pinch | Gesture::Pan | Gesture::Tap | Gesture::LongPress) );

    DALI_TEST_CHECK( !dummyImpl.GetPinchGestureDetector() );
    DALI_TEST_CHECK( !dummyImpl.GetPanGestureDetector() );
    DALI_TEST_CHECK( !dummyImpl.GetTapGestureDetector() );
    DALI_TEST_CHECK( !dummyImpl.GetLongPressGestureDetector() );
  }

  // Disable When not enabled
  {
    DummyControl dummy = DummyControl::New();
    DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

    DALI_TEST_CHECK( !dummyImpl.GetPinchGestureDetector() );
    DALI_TEST_CHECK( !dummyImpl.GetPanGestureDetector() );
    DALI_TEST_CHECK( !dummyImpl.GetTapGestureDetector() );
    DALI_TEST_CHECK( !dummyImpl.GetLongPressGestureDetector() );

    dummyImpl.DisableGestureDetection( Gesture::Type(Gesture::Pinch | Gesture::Pan | Gesture::Tap | Gesture::LongPress) );

    DALI_TEST_CHECK( !dummyImpl.GetPinchGestureDetector() );
    DALI_TEST_CHECK( !dummyImpl.GetPanGestureDetector() );
    DALI_TEST_CHECK( !dummyImpl.GetTapGestureDetector() );
    DALI_TEST_CHECK( !dummyImpl.GetLongPressGestureDetector() );
  }

  // Ensure control is detached if gesture detector is not deleted
  {
    DummyControl dummy = DummyControl::New();
    DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

    dummyImpl.EnableGestureDetection( Gesture::Type(Gesture::Pinch | Gesture::Pan | Gesture::Tap | Gesture::LongPress) );

    PinchGestureDetector pinch = dummyImpl.GetPinchGestureDetector();
    PanGestureDetector pan   = dummyImpl.GetPanGestureDetector();
    TapGestureDetector tap = dummyImpl.GetTapGestureDetector();
    LongPressGestureDetector longPress = dummyImpl.GetLongPressGestureDetector();

    DALI_TEST_EQUALS( pinch.GetAttachedActors().empty(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( pan.GetAttachedActors().empty(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( tap.GetAttachedActors().empty(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( longPress.GetAttachedActors().empty(), false, TEST_LOCATION );

    dummyImpl.DisableGestureDetection( Gesture::Type(Gesture::Pinch | Gesture::Pan | Gesture::Tap | Gesture::LongPress) );

    DALI_TEST_EQUALS( pinch.GetAttachedActors().empty(), true, TEST_LOCATION );
    DALI_TEST_EQUALS( pan.GetAttachedActors().empty(), true, TEST_LOCATION );
    DALI_TEST_EQUALS( tap.GetAttachedActors().empty(), true, TEST_LOCATION );
    DALI_TEST_EQUALS( longPress.GetAttachedActors().empty(), true, TEST_LOCATION );
  }
}

static void UtcDaliControlImplOnGestureMethods()
{
  ToolkitTestApplication application;

  // Check gesture actually happens
  {
    DummyControl dummy = DummyControl::New(true);
    dummy.SetSize( Vector3(100.0f, 100.0f, 100.0f) );

    dummy.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    Stage::GetCurrent().Add(dummy);

    // Render and notify a couple of times
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());
    dummyImpl.EnableGestureDetection( Gesture::Type(Gesture::Pinch | Gesture::Pan | Gesture::Tap | Gesture::LongPress) );

    DALI_TEST_CHECK( dummyImpl.pinchCalled == false );
    Integration::PinchGestureEvent pinch(Gesture::Started);
    pinch.scale = 10.0f;
    pinch.speed = 50.0f;
    pinch.centerPoint = Vector2(20.0f, 20.0f);
    application.GetCore().SendEvent(pinch);
    DALI_TEST_CHECK( dummyImpl.pinchCalled == true );

    DALI_TEST_CHECK( dummyImpl.panCalled == false );
    Integration::PanGestureEvent pan(Gesture::Possible);
    pan.previousPosition = Vector2(10.0f, 20.0f);
    pan.currentPosition = Vector2(20.0f, 20.0f);
    pan.timeDelta = 10;
    pan.numberOfTouches = 1u;
    application.GetCore().SendEvent(pan);
    pan.state = Gesture::Started;
    application.GetCore().SendEvent(pan);
    DALI_TEST_CHECK( dummyImpl.panCalled == true );

    DALI_TEST_CHECK( dummyImpl.tapCalled == false );
    Integration::TapGestureEvent tap(Gesture::Possible);
    tap.numberOfTaps = 1u;
    tap.numberOfTouches = 1u;
    tap.point = Vector2(50.0f, 50.0f);
    application.GetCore().SendEvent(tap);
    tap.state = Gesture::Started;
    application.GetCore().SendEvent(tap);
    DALI_TEST_CHECK( dummyImpl.tapCalled == true );

    DALI_TEST_CHECK( dummyImpl.longPressCalled == false );
    Integration::LongPressGestureEvent longPress(Gesture::Possible);
    longPress.numberOfTouches = 1u;
    longPress.point = Vector2(50.0f, 50.0f);
    application.GetCore().SendEvent(longPress);
    longPress.state = Gesture::Started;
    application.GetCore().SendEvent(longPress);
    DALI_TEST_CHECK( dummyImpl.longPressCalled == true );
    longPress.state = Gesture::Finished;
    application.GetCore().SendEvent(longPress);

    Stage::GetCurrent().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();
    dummy.SetSize( Vector3(100.0f, 100.0f, 100.0f) );

    dummy.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    Stage::GetCurrent().Add(dummy);

    // Render and notify a couple of times
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());
    dummyImpl.EnableGestureDetection( Gesture::Type(Gesture::Pinch | Gesture::Pan | Gesture::Tap | Gesture::LongPress) );

    DALI_TEST_CHECK( dummy.GetCurrentScale().x != 10.0f );
    Integration::PinchGestureEvent pinch(Gesture::Started);
    pinch.scale = 10.0f;
    pinch.speed = 50.0f;
    pinch.centerPoint = Vector2(20.0f, 20.0f);
    application.GetCore().SendEvent(pinch);

    // Render and notify a couple of times
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK( dummy.GetCurrentScale().x == 10.0f );

    Integration::PanGestureEvent pan(Gesture::Possible);
    pan.previousPosition = Vector2(10.0f, 20.0f);
    pan.currentPosition = Vector2(20.0f, 20.0f);
    pan.timeDelta = 10;
    pan.numberOfTouches = 1u;
    application.GetCore().SendEvent(pan);
    pan.state = Gesture::Started;
    application.GetCore().SendEvent(pan);

    Integration::TapGestureEvent tap(Gesture::Possible);
    tap.numberOfTaps = 1u;
    tap.numberOfTouches = 1u;
    tap.point = Vector2(50.0f, 50.0f);
    application.GetCore().SendEvent(tap);
    tap.state = Gesture::Started;
    application.GetCore().SendEvent(tap);

    Integration::LongPressGestureEvent longPress(Gesture::Possible);
    longPress.numberOfTouches = 1u;
    longPress.point = Vector2(50.0f, 50.0f);
    application.GetCore().SendEvent(longPress);
    longPress.state = Gesture::Started;
    application.GetCore().SendEvent(longPress);
    longPress.state = Gesture::Finished;
    application.GetCore().SendEvent(longPress);

    Stage::GetCurrent().Remove(dummy);
  }
}

static void UtcDaliControlImplChildAddAndRemove()
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New( true );
    Stage::GetCurrent().Add(dummy);
    DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());

    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS( dummyImpl.childAddCalled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( dummy.GetChildCount(), 0u, TEST_LOCATION );
    Actor actor = RenderableActor::New();
    dummy.Add(actor);
    DALI_TEST_EQUALS( dummyImpl.childAddCalled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( dummy.GetChildCount(), 1u, TEST_LOCATION );

    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS( dummyImpl.childRemoveCalled, false, TEST_LOCATION );
    dummy.Remove( actor );
    DALI_TEST_EQUALS( dummyImpl.childRemoveCalled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( dummy.GetChildCount(), 0u, TEST_LOCATION );

    application.Render();
    application.SendNotification();

    Stage::GetCurrent().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();
    Stage::GetCurrent().Add(dummy);

    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS( dummy.GetChildCount(), 0u, TEST_LOCATION );
    Actor actor = RenderableActor::New();
    dummy.Add(actor);
    DALI_TEST_EQUALS( dummy.GetChildCount(), 1u, TEST_LOCATION );

    application.Render();
    application.SendNotification();

    dummy.Remove( actor );
    DALI_TEST_EQUALS( dummy.GetChildCount(), 0u, TEST_LOCATION );

    application.Render();
    application.SendNotification();

    Stage::GetCurrent().Remove(dummy);
  }
}

static void UtcDaliControlImplStageConnection()
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New( true );
    DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());

    DALI_TEST_EQUALS( dummyImpl.stageConnectionCalled, false, TEST_LOCATION );
    Stage::GetCurrent().Add(dummy);
    application.Render();
    application.SendNotification();
    DALI_TEST_EQUALS( dummyImpl.stageConnectionCalled, true, TEST_LOCATION );

    DALI_TEST_EQUALS( dummyImpl.stageDisconnectionCalled, false, TEST_LOCATION );
    Stage::GetCurrent().Remove(dummy);
    application.Render();
    application.SendNotification();
    DALI_TEST_EQUALS( dummyImpl.stageDisconnectionCalled, true, TEST_LOCATION );
  }

  // Ensure full code coverage
  {
    unsigned int stageChildren = Stage::GetCurrent().GetLayer(0).GetChildCount();
    DummyControl dummy = DummyControl::New();

    DALI_TEST_EQUALS( Stage::GetCurrent().GetLayer(0).GetChildCount(), stageChildren, TEST_LOCATION );
    Stage::GetCurrent().Add(dummy);
    application.Render();
    application.SendNotification();
    DALI_TEST_EQUALS( Stage::GetCurrent().GetLayer(0).GetChildCount(), stageChildren + 1, TEST_LOCATION );

    Stage::GetCurrent().Remove(dummy);
    application.Render();
    application.SendNotification();
    DALI_TEST_EQUALS( Stage::GetCurrent().GetLayer(0).GetChildCount(), stageChildren, TEST_LOCATION );
  }
}

static void UtcDaliControlImplSizeSet()
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New( true );
    DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());

    Stage::GetCurrent().Add(dummy);
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS( dummyImpl.sizeSetCalled, false, TEST_LOCATION );
    Vector3 size(100.0f, 200.0f, 0.0f);
    dummy.SetSize(size);

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS(size, dummy.GetCurrentSize(), TEST_LOCATION);
    DALI_TEST_EQUALS( dummyImpl.sizeSetCalled, true, TEST_LOCATION );

    Stage::GetCurrent().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();
    Stage::GetCurrent().Add(dummy);

    Vector3 size(100.0f, 200.0f, 0.0f);
    DALI_TEST_CHECK( size != dummy.GetCurrentSize() );

    application.Render();
    application.SendNotification();

    dummy.SetSize(size);

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS(size, dummy.GetCurrentSize(), TEST_LOCATION);

    Stage::GetCurrent().Remove(dummy);
  }
}

static void UtcDaliControlImplSizeAnimation()
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New( true );
    DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());

    Stage::GetCurrent().Add(dummy);

    DALI_TEST_EQUALS( dummyImpl.sizeAnimationCalled, false, TEST_LOCATION );
    Animation animation = Animation::New(1.0f);
    animation.Resize(dummy, Vector3(100.0f, 150.0f, 200.0f));
    animation.Play();

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS( dummyImpl.sizeAnimationCalled, true, TEST_LOCATION );

    Stage::GetCurrent().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();

    Stage::GetCurrent().Add(dummy);

    Animation animation = Animation::New(1.0f);
    animation.Resize(dummy, Vector3(100.0f, 150.0f, 200.0f));
    animation.Play();

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    Stage::GetCurrent().Remove(dummy);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

static void UtcDaliControlImplTouchEvent()
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New( true );
    DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());

    dummy.SetSize(100.0f, 100.0f);
    dummy.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    Stage::GetCurrent().Add(dummy);

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS( dummyImpl.touchEventCalled, false, TEST_LOCATION );
    Integration::TouchEvent touchEvent(1);
    TouchPoint point(1, TouchPoint::Down, 20.0f, 20.0f);
    touchEvent.AddPoint(point);
    application.GetCore().SendEvent(touchEvent);
    DALI_TEST_EQUALS( dummyImpl.touchEventCalled, true, TEST_LOCATION );

    Stage::GetCurrent().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();

    dummy.SetSize(100.0f, 100.0f);
    dummy.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    Stage::GetCurrent().Add(dummy);

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    Integration::TouchEvent touchEvent(1);
    TouchPoint point(1, TouchPoint::Down, 20.0f, 20.0f);
    touchEvent.AddPoint(point);
    application.GetCore().SendEvent(touchEvent);

    Stage::GetCurrent().Remove(dummy);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

static bool MouseWheelEventCallback(Actor actor, const MouseWheelEvent& event)
{
  return false;
}

static void UtcDaliControlImplMouseWheelEvent()
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New( true );
    DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());

    dummy.SetSize(100.0f, 100.0f);
    dummy.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    Stage::GetCurrent().Add(dummy);

    dummy.MouseWheelEventSignal().Connect(&MouseWheelEventCallback);

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS( dummyImpl.mouseWheelEventCalled, false, TEST_LOCATION );

    // simulate a mouse wheel event
    Vector2 screenCoordinates( 10.0f, 10.0f );
    Integration::MouseWheelEvent event(0, 0u, screenCoordinates, 1, 1000u);
    application.GetCore().SendEvent(event);
    DALI_TEST_EQUALS( dummyImpl.mouseWheelEventCalled, true, TEST_LOCATION );

    Stage::GetCurrent().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();

    dummy.SetSize(100.0f, 100.0f);
    dummy.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    Stage::GetCurrent().Add(dummy);

    dummy.MouseWheelEventSignal().Connect(&MouseWheelEventCallback);

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    // simulate a mouse wheel event
    Vector2 screenCoordinates( 20.0f, 20.0f );
    Integration::MouseWheelEvent event(0, 0u, screenCoordinates, 1, 1000u);
    application.GetCore().SendEvent(event);

    Stage::GetCurrent().Remove(dummy);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

static void UtcDaliControlImplKeyEvent()
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New( true );
    DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());

    Stage::GetCurrent().Add(dummy);
    dummy.SetKeyInputFocus();

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS( dummyImpl.keyEventCalled, false, TEST_LOCATION );
    Integration::KeyEvent keyEvent;
    application.GetCore().SendEvent(keyEvent);
    DALI_TEST_EQUALS( dummyImpl.keyEventCalled, true, TEST_LOCATION );

    Stage::GetCurrent().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();

    Stage::GetCurrent().Add(dummy);
    dummy.SetKeyInputFocus();

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    Integration::KeyEvent keyEvent;
    application.GetCore().SendEvent(keyEvent);

    Stage::GetCurrent().Remove(dummy);
  }
}

static void UtcDaliControlImplStyleChange()
{
  ToolkitTestApplication application;

  DummyControl dummy = DummyControl::New( true );
  DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());

  Stage::GetCurrent().Add(dummy);

  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();

  // Add a Control and normal Actor as children
  DummyControl dummyChild = DummyControl::New();
  dummy.Add(dummyChild);

  Actor actor = Actor::New();
  dummy.Add(actor);

  DALI_TEST_EQUALS( dummyImpl.styleChangeCalled, false, TEST_LOCATION );
  StyleChange styleChange;
  styleChange.defaultFontChange = true;
  application.GetAdaptor().GetToolkitStyleMonitor().EmitSignalStyleChange(styleChange);
  DALI_TEST_EQUALS( dummyImpl.styleChangeCalled, true, TEST_LOCATION );

  Stage::GetCurrent().Remove(dummy);
}

static void UtcDaliControlImplKeyInputFocusGained()
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New( true );
    DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());

    Stage::GetCurrent().Add(dummy);

    DALI_TEST_EQUALS( dummyImpl.keyInputFocusGained, false, TEST_LOCATION );

    dummy.SetKeyInputFocus();

    DALI_TEST_EQUALS( dummyImpl.keyInputFocusGained, true, TEST_LOCATION );

    Stage::GetCurrent().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();

    Stage::GetCurrent().Add(dummy);
    dummy.SetKeyInputFocus();
    Stage::GetCurrent().Remove(dummy);
  }
}

static void UtcDaliControlImplKeyInputFocusLost()
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New( true );
    DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());

    Stage::GetCurrent().Add(dummy);

    DALI_TEST_EQUALS( dummyImpl.keyInputFocusLost, false, TEST_LOCATION );

    dummy.SetKeyInputFocus();
    dummy.ClearKeyInputFocus();

    DALI_TEST_EQUALS( dummyImpl.keyInputFocusLost, true, TEST_LOCATION );

    Stage::GetCurrent().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();

    Stage::GetCurrent().Add(dummy);
    dummy.SetKeyInputFocus();
    dummy.ClearKeyInputFocus();

    DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());

    dummyImpl.OnAccessibilityValueChange( true );
    dummyImpl.IsKeyboardNavigationSupported();
    dummyImpl.IsKeyboardFocusGroup();

    Stage::GetCurrent().Remove(dummy);
  }
}
