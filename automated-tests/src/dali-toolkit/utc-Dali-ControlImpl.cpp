/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include "toolkit-style-monitor.h"

#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/wheel-event-integ.h>
#include <dali/integration-api/events/long-press-gesture-event.h>
#include <dali/integration-api/events/pinch-gesture-event.h>
#include <dali/integration-api/events/pan-gesture-event.h>
#include <dali/integration-api/events/tap-gesture-event.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/hover-event-integ.h>


#include "dummy-control.h"

using namespace Dali;
using namespace Dali::Toolkit;

void utc_dali_toolkit_control_impl_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_control_impl_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliControlImplNew(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control;

  DALI_TEST_CHECK( !Control::DownCast(control) );

  control = Toolkit::Internal::Control::New();

  DALI_TEST_CHECK( Control::DownCast(control) );
  END_TEST;
}


int UtcDaliControlImplEnableGestureDetector(void)
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
  END_TEST;
}

int UtcDaliControlImplDisableGestureDetector(void)
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

    DALI_TEST_EQUALS( 0 == pinch.GetAttachedActorCount(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( 0 == pan.GetAttachedActorCount(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( 0 == tap.GetAttachedActorCount(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( 0 == longPress.GetAttachedActorCount(), false, TEST_LOCATION );

    dummyImpl.DisableGestureDetection( Gesture::Type(Gesture::Pinch | Gesture::Pan | Gesture::Tap | Gesture::LongPress) );

    DALI_TEST_EQUALS( 0 == pinch.GetAttachedActorCount(), true, TEST_LOCATION );
    DALI_TEST_EQUALS( 0 == pan.GetAttachedActorCount(), true, TEST_LOCATION );
    DALI_TEST_EQUALS( 0 == tap.GetAttachedActorCount(), true, TEST_LOCATION );
    DALI_TEST_EQUALS( 0 == longPress.GetAttachedActorCount(), true, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliControlImplOnGestureMethods(void)
{
  ToolkitTestApplication application;

  // Check gesture actually happens
  {
    DummyControl dummy = DummyControl::New(true);
    dummy.SetSize( Vector2(100.0f, 100.0f ) );

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
    application.ProcessEvent(pinch);
    DALI_TEST_CHECK( dummyImpl.pinchCalled == true );

    DALI_TEST_CHECK( dummyImpl.panCalled == false );
    Integration::PanGestureEvent pan(Gesture::Possible);
    pan.previousPosition = Vector2(10.0f, 20.0f);
    pan.currentPosition = Vector2(20.0f, 20.0f);
    pan.timeDelta = 10;
    pan.numberOfTouches = 1u;
    application.ProcessEvent(pan);
    pan.state = Gesture::Started;
    application.ProcessEvent(pan);
    DALI_TEST_CHECK( dummyImpl.panCalled == true );

    DALI_TEST_CHECK( dummyImpl.tapCalled == false );
    Integration::TapGestureEvent tap(Gesture::Possible);
    tap.numberOfTaps = 1u;
    tap.numberOfTouches = 1u;
    tap.point = Vector2(50.0f, 50.0f);
    application.ProcessEvent(tap);
    tap.state = Gesture::Started;
    application.ProcessEvent(tap);
    DALI_TEST_CHECK( dummyImpl.tapCalled == true );

    DALI_TEST_CHECK( dummyImpl.longPressCalled == false );
    Integration::LongPressGestureEvent longPress(Gesture::Possible);
    longPress.numberOfTouches = 1u;
    longPress.point = Vector2(50.0f, 50.0f);
    application.ProcessEvent(longPress);
    longPress.state = Gesture::Started;
    application.ProcessEvent(longPress);
    DALI_TEST_CHECK( dummyImpl.longPressCalled == true );
    longPress.state = Gesture::Finished;
    application.ProcessEvent(longPress);

    Stage::GetCurrent().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();
    dummy.SetSize( Vector2( 100.0f, 100.0f ) );

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
    application.ProcessEvent(pinch);

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
    application.ProcessEvent(pan);
    pan.state = Gesture::Started;
    application.ProcessEvent(pan);

    Integration::TapGestureEvent tap(Gesture::Possible);
    tap.numberOfTaps = 1u;
    tap.numberOfTouches = 1u;
    tap.point = Vector2(50.0f, 50.0f);
    application.ProcessEvent(tap);
    tap.state = Gesture::Started;
    application.ProcessEvent(tap);

    Integration::LongPressGestureEvent longPress(Gesture::Possible);
    longPress.numberOfTouches = 1u;
    longPress.point = Vector2(50.0f, 50.0f);
    application.ProcessEvent(longPress);
    longPress.state = Gesture::Started;
    application.ProcessEvent(longPress);
    longPress.state = Gesture::Finished;
    application.ProcessEvent(longPress);

    Stage::GetCurrent().Remove(dummy);
  }
  END_TEST;
}

int UtcDaliControlImplChildAddAndRemove(void)
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
    Actor actor = Actor::New();
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
    Actor actor = Actor::New();
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
  END_TEST;
}

int UtcDaliControlImplStageConnection(void)
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
  END_TEST;
}

int UtcDaliControlImplSizeSetP(void)
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New( true );
    DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());

    Stage::GetCurrent().Add(dummy);
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS( dummyImpl.sizeSetCalled, false, TEST_LOCATION ); // Size not set, no onSizeSet called
    Vector2 size(100.0f, 200.0f);
    dummy.SetSize( size );

    DALI_TEST_EQUALS( dummyImpl.sizeSetCalled, false, TEST_LOCATION ); // Size is going to get negotiated, no onSizeSet called

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( size, dummy.GetCurrentSize().GetVectorXY(), TEST_LOCATION );
    DALI_TEST_EQUALS( dummyImpl.sizeSetCalled, true, TEST_LOCATION );

    Stage::GetCurrent().Remove(dummy);
  }

  END_TEST;
}

int UtcDaliControlImplSizeSet2P(void)
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New();
    Stage::GetCurrent().Add(dummy);

    Vector2 size(100.0f, 200.0f);
    DALI_TEST_CHECK( size != dummy.GetCurrentSize().GetVectorXY() );

    application.SendNotification();
    application.Render();

    dummy.SetSize(size);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(size, dummy.GetCurrentSize().GetVectorXY(), TEST_LOCATION);

    Stage::GetCurrent().Remove(dummy);
  }
  END_TEST;
}


int UtcDaliControlImplSizeAnimation(void)
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New( true );
    DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());

    Stage::GetCurrent().Add(dummy);

    DALI_TEST_EQUALS( dummyImpl.sizeAnimationCalled, false, TEST_LOCATION );
    Animation animation = Animation::New(1.0f);
    animation.AnimateTo( Property( dummy, Actor::Property::SIZE ), Vector3( 100.0f, 150.0f, 200.0f ) );
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
    animation.AnimateTo( Property( dummy, Actor::Property::SIZE ), Vector3( 100.0f, 150.0f, 200.0f ) );
    animation.Play();

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    Stage::GetCurrent().Remove(dummy);
  }
  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int UtcDaliControlImplTouchEvent(void)
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New( true );
    DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());

    dummy.SetSize( Vector2( 100.0f, 100.0f ) );
    dummy.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    Stage::GetCurrent().Add(dummy);

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS( dummyImpl.touchEventCalled, false, TEST_LOCATION );
    Integration::TouchEvent touchEvent(1);
    Integration::Point point;
    point.SetDeviceId( 1 );
    point.SetState( PointState::DOWN );
    point.SetScreenPosition( Vector2( 20.0f, 20.0f ) );
    touchEvent.AddPoint(point);
    application.ProcessEvent(touchEvent);
    DALI_TEST_EQUALS( dummyImpl.touchEventCalled, true, TEST_LOCATION );

    Stage::GetCurrent().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();

    dummy.SetSize( Vector2( 100.0f, 100.0f ) );
    dummy.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    Stage::GetCurrent().Add(dummy);

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    Integration::TouchEvent touchEvent(1);
    Integration::Point point;
    point.SetDeviceId( 1 );
    point.SetState( PointState::DOWN );
    point.SetScreenPosition( Vector2( 20.0f, 20.0f ) );
    touchEvent.AddPoint(point);
    application.ProcessEvent(touchEvent);

    Stage::GetCurrent().Remove(dummy);
  }
  END_TEST;
}

int UtcDaliControlImplHoverEvent(void)
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New( true );
    DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());

    dummy.SetSize( Vector2( 100.0f, 100.0f ) );
    dummy.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    Stage::GetCurrent().Add(dummy);

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS( dummyImpl.hoverEventCalled, false, TEST_LOCATION );
    Integration::HoverEvent event(1);
    Integration::Point point;
    point.SetDeviceId( 1 );
    point.SetState( PointState::MOTION );
    point.SetScreenPosition( Vector2( 20.0f, 20.0f ) );
    event.AddPoint(point);
    application.ProcessEvent( event );
    DALI_TEST_EQUALS( dummyImpl.hoverEventCalled, true, TEST_LOCATION );

    Stage::GetCurrent().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();

    dummy.SetSize( Vector2( 100.0f, 100.0f ) );
    dummy.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    Stage::GetCurrent().Add(dummy);

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    Integration::HoverEvent event(1);
    Integration::Point point;
    point.SetDeviceId( 1 );
    point.SetState( PointState::MOTION );
    point.SetScreenPosition( Vector2( 20.0f, 20.0f ) );
    event.AddPoint(point);
    application.ProcessEvent( event );

    Stage::GetCurrent().Remove(dummy);
  }
  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////////////////////////


int UtcDaliControlImplKeyEvent(void)
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
    application.ProcessEvent(keyEvent);
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
    application.ProcessEvent(keyEvent);

    Stage::GetCurrent().Remove(dummy);
  }
  END_TEST;
}

int UtcDaliControlImplKeyInputFocusGained(void)
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
  END_TEST;
}

int UtcDaliControlImplKeyInputFocusLost(void)
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

    dummyImpl.IsKeyboardNavigationSupported();
    dummyImpl.IsKeyboardFocusGroup();

    Stage::GetCurrent().Remove(dummy);
  }
  END_TEST;
}

int UtcDaliControlImplTypeRegistry(void)
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
  END_TEST;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
namespace
{
static bool WheelEventCallback(Actor actor, const WheelEvent& event)
{
  return false;
}
}

int UtcDaliControlImplWheelEvent(void)
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New( true );
    DummyControlImplOverride& dummyImpl = static_cast<DummyControlImplOverride&>(dummy.GetImplementation());

    dummy.SetSize( Vector2( 100.0f, 100.0f ) );
    dummy.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    Stage::GetCurrent().Add(dummy);

    dummy.WheelEventSignal().Connect(&WheelEventCallback);

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS( dummyImpl.wheelEventCalled, false, TEST_LOCATION );

    // simulate a wheel event
    Vector2 screenCoordinates( 10.0f, 10.0f );
    Integration::WheelEvent event( Integration::WheelEvent::MOUSE_WHEEL, 0, 0u, screenCoordinates, 1, 1000u );
    application.ProcessEvent( event );
    DALI_TEST_EQUALS( dummyImpl.wheelEventCalled, true, TEST_LOCATION );

    Stage::GetCurrent().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();

    dummy.SetSize( Vector2( 100.0f, 100.0f ) );
    dummy.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    Stage::GetCurrent().Add(dummy);

    dummy.WheelEventSignal().Connect(&WheelEventCallback);

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    // simulate a wheel event
    Vector2 screenCoordinates( 20.0f, 20.0f );
    Integration::WheelEvent event( Integration::WheelEvent::MOUSE_WHEEL, 0, 0u, screenCoordinates, 1, 1000u );
    application.ProcessEvent( event );

    Stage::GetCurrent().Remove(dummy);
  }
  END_TEST;
}

int UtcDaliControlImplSetStyleName(void)
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New( true );

    dummy.SetSize( Vector2( 100.0f, 100.0f ) );
    dummy.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    Stage::GetCurrent().Add(dummy);

    dummy.SetStyleName("TestStyle");

    DALI_TEST_CHECK( dummy.GetStyleName() == "TestStyle" );

    Stage::GetCurrent().Remove(dummy);
  }
  END_TEST;
}

int UtcDaliControlImplOnStyleChangeN(void)
{
  ToolkitTestApplication application;
  Control dummy = Control::New();
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( dummy );

  // test that style manager is being used, passing an empty handle does nothing but does not crash either
  Dali::Toolkit::StyleManager styleManager;
  controlImpl.OnStyleChange( styleManager, StyleChange::THEME_CHANGE );
  // no crash so test passes
  tet_result(TET_PASS);

  END_TEST;
}


int UtcDaliControlImplOnAccessibilityPanP(void)
{
  ToolkitTestApplication application;
  Control dummy = Control::New();
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( dummy );

  PanGesture pan;
  DALI_TEST_EQUALS( false, controlImpl.OnAccessibilityPan( pan ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlImplOnAccessibilityTouchP(void)
{
  ToolkitTestApplication application;
  Control dummy = Control::New();
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( dummy );
  TouchEvent touch;
  DALI_TEST_EQUALS( false, controlImpl.OnAccessibilityTouch( touch ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlImplOnAccessibilityActivatedP(void)
{
  ToolkitTestApplication application;

  Control dummy = Control::New();
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( dummy );
  DALI_TEST_EQUALS( false, controlImpl.OnAccessibilityActivated(), TEST_LOCATION );

  // Invoke the control's activate action
  TypeInfo type = TypeRegistry::Get().GetTypeInfo( "Control" );
  DALI_TEST_CHECK( type );

  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  Property::Map attributes;
  DALI_TEST_EQUALS( false, handle.DoAction("accessibilityActivated",  attributes), TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlImplGetNextKeyboardFocusableActorP(void)
{
  ToolkitTestApplication application;
  Control dummy = Control::New();
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( dummy );

  Actor currentFocusedActor;
  Actor result = controlImpl.GetNextKeyboardFocusableActor( currentFocusedActor, Control::KeyboardFocus::LEFT, false );

  DALI_TEST_EQUALS( result, currentFocusedActor, TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlImplRegisterThenReRegisterVisual(void)
{
  ToolkitTestApplication application;

  DummyControl dummy = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

  Property::Index index =1;
  Actor placementActor = Actor::New();

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base visual;

  Property::Map map;
  map[Visual::Property::TYPE] = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  visual = visualFactory.CreateVisual( map );
  DALI_TEST_CHECK(visual);

  // Register index with a color visual
  dummyImpl.RegisterVisual( index, placementActor, visual );


  Property::Map newMap;
  newMap[Visual::Property::TYPE] = Visual::COLOR;
  newMap[ColorVisual::Property::MIX_COLOR] = Color::BLUE;

  visual = visualFactory.CreateVisual( newMap );
  DALI_TEST_CHECK(visual);

  // ReRegister with altered color visual
  dummyImpl.RegisterVisual( index, placementActor, visual );

  tet_result(TET_PASS);

  END_TEST;
}

int UtcDaliControlImplRegisterVisaulThenReRegisterToSelf(void)
{
  ToolkitTestApplication application;

  DummyControl dummy = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

  Property::Index index =1;
  Actor placementActor = Actor::New();

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base visual;

  Property::Map map;
  map[Visual::Property::TYPE] = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  visual = visualFactory.CreateVisual( map );
  DALI_TEST_CHECK(visual);

  // Register index with a color visual
  dummyImpl.RegisterVisual( index, placementActor, visual );

  // ReRegister to self
  dummyImpl.RegisterVisual( index, dummy, visual );

  END_TEST;
}

int UtcDaliControlImplRegisterVisualToSelf(void)
{
  ToolkitTestApplication application;

  Test::ObjectDestructionTracker objectDestructionTracker;

  {
    DummyControl dummy = DummyControl::New();
    DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());
    objectDestructionTracker.Start( dummy );

    Property::Index index = 1;
    Actor placementActor = Actor::New();

    Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
    Toolkit::Visual::Base visual;

    Property::Map map;
    map[Visual::Property::TYPE] = Visual::COLOR;
    map[ColorVisual::Property::MIX_COLOR] = Color::RED;

    visual = visualFactory.CreateVisual( map );
    DALI_TEST_CHECK(visual);

    // Register to self
    dummyImpl.RegisterVisual( index, dummy, visual );
    DALI_TEST_EQUALS( objectDestructionTracker.IsDestroyed(), false, TEST_LOCATION ); // Control not destroyed yet
  }

  DALI_TEST_EQUALS( objectDestructionTracker.IsDestroyed(), true, TEST_LOCATION ); // Should be destroyed

  END_TEST;
}

int UtcDaliControlImplRegisterTwoVisuals(void)
{
  ToolkitTestApplication application;

  DummyControl dummy = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

  Property::Index index =1;
  Actor placementActor = Actor::New();

  Property::Index index2 =2;
  Actor secondPlacementActor = Actor::New();

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base visual;
  Toolkit::Visual::Base secondVisual;

  Property::Map map;
  map[Visual::Property::TYPE] = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  visual = visualFactory.CreateVisual( map );
  DALI_TEST_CHECK(visual);

  // Register index with a color visual
  dummyImpl.RegisterVisual( index, placementActor, visual );

  Property::Map newMap;
  newMap[Visual::Property::TYPE] = Visual::COLOR;
  newMap[ColorVisual::Property::MIX_COLOR] = Color::BLUE;

  secondVisual = visualFactory.CreateVisual( newMap );
  DALI_TEST_CHECK( secondVisual );

  // ReRegister with altered color visual
  dummyImpl.RegisterVisual( index2, secondPlacementActor, secondVisual );

  END_TEST;
}

int UtcDaliControlImplRegisterUnregisterVisual(void)
{
  ToolkitTestApplication application;

  DummyControl dummy = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

  Property::Index index =1;
  Actor placementActor = Actor::New();

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base visual;

  Property::Map map;
  map[Visual::Property::TYPE] = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  visual = visualFactory.CreateVisual( map );
  DALI_TEST_CHECK(visual);

  // Register index with a color visual
  dummyImpl.RegisterVisual( index, placementActor, visual );

  // Unregister visual
  dummyImpl.UnregisterVisual( index );

  END_TEST;
}
