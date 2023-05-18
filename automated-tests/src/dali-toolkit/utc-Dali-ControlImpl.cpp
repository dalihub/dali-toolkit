/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

#include <stdlib.h>
#include <iostream>

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>
#include "toolkit-style-monitor.h"

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali.h>
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/wheel-event-integ.h>

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
  ToolkitTestApplication application; // Exceptions require ToolkitTestApplication

  Control control;

  DALI_TEST_CHECK(!Control::DownCast(control));

  control = Toolkit::Internal::Control::New();

  DALI_TEST_CHECK(Control::DownCast(control));
  END_TEST;
}

int UtcDaliControlImplEnableGestureDetector(void)
{
  ToolkitTestApplication application;

  // Enable individually
  {
    DummyControl      dummy     = DummyControl::New();
    DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

    DALI_TEST_CHECK(!dummyImpl.GetPinchGestureDetector());
    dummyImpl.EnableGestureDetection(GestureType::PINCH);
    DALI_TEST_CHECK(dummyImpl.GetPinchGestureDetector());

    DALI_TEST_CHECK(!dummyImpl.GetPanGestureDetector());
    dummyImpl.EnableGestureDetection(GestureType::PAN);
    DALI_TEST_CHECK(dummyImpl.GetPanGestureDetector());

    DALI_TEST_CHECK(!dummyImpl.GetTapGestureDetector());
    dummyImpl.EnableGestureDetection(GestureType::TAP);
    DALI_TEST_CHECK(dummyImpl.GetTapGestureDetector());

    DALI_TEST_CHECK(!dummyImpl.GetLongPressGestureDetector());
    dummyImpl.EnableGestureDetection(GestureType::LONG_PRESS);
    DALI_TEST_CHECK(dummyImpl.GetLongPressGestureDetector());
  }

  // Enable All
  {
    DummyControl      dummy     = DummyControl::New();
    DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

    DALI_TEST_CHECK(!dummyImpl.GetPinchGestureDetector());
    DALI_TEST_CHECK(!dummyImpl.GetPanGestureDetector());
    DALI_TEST_CHECK(!dummyImpl.GetTapGestureDetector());
    DALI_TEST_CHECK(!dummyImpl.GetLongPressGestureDetector());

    dummyImpl.EnableGestureDetection(GestureType::Value(GestureType::PINCH | GestureType::PAN | GestureType::TAP | GestureType::LONG_PRESS));

    DALI_TEST_CHECK(dummyImpl.GetPinchGestureDetector());
    DALI_TEST_CHECK(dummyImpl.GetPanGestureDetector());
    DALI_TEST_CHECK(dummyImpl.GetTapGestureDetector());
    DALI_TEST_CHECK(dummyImpl.GetLongPressGestureDetector());

    // Enable when already enabled

    dummyImpl.EnableGestureDetection(GestureType::Value(GestureType::PINCH | GestureType::PAN | GestureType::TAP | GestureType::LONG_PRESS));

    DALI_TEST_CHECK(dummyImpl.GetPinchGestureDetector());
    DALI_TEST_CHECK(dummyImpl.GetPanGestureDetector());
    DALI_TEST_CHECK(dummyImpl.GetTapGestureDetector());
    DALI_TEST_CHECK(dummyImpl.GetLongPressGestureDetector());
  }
  END_TEST;
}

int UtcDaliControlImplDisableGestureDetector(void)
{
  ToolkitTestApplication application;

  // Disable individually
  {
    DummyControl      dummy     = DummyControl::New();
    DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

    dummyImpl.EnableGestureDetection(GestureType::Value(GestureType::PINCH | GestureType::PAN | GestureType::TAP | GestureType::LONG_PRESS));

    DALI_TEST_CHECK(dummyImpl.GetPinchGestureDetector());
    dummyImpl.DisableGestureDetection(GestureType::PINCH);
    DALI_TEST_CHECK(!dummyImpl.GetPinchGestureDetector());

    DALI_TEST_CHECK(dummyImpl.GetPanGestureDetector());
    dummyImpl.DisableGestureDetection(GestureType::PAN);
    DALI_TEST_CHECK(!dummyImpl.GetPanGestureDetector());

    DALI_TEST_CHECK(dummyImpl.GetTapGestureDetector());
    dummyImpl.DisableGestureDetection(GestureType::TAP);
    DALI_TEST_CHECK(!dummyImpl.GetTapGestureDetector());

    DALI_TEST_CHECK(dummyImpl.GetLongPressGestureDetector());
    dummyImpl.DisableGestureDetection(GestureType::LONG_PRESS);
    DALI_TEST_CHECK(!dummyImpl.GetLongPressGestureDetector());
  }

  // Disable All
  {
    DummyControl      dummy     = DummyControl::New();
    DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

    dummyImpl.EnableGestureDetection(GestureType::Value(GestureType::PINCH | GestureType::PAN | GestureType::TAP | GestureType::LONG_PRESS));

    DALI_TEST_CHECK(dummyImpl.GetPinchGestureDetector());
    DALI_TEST_CHECK(dummyImpl.GetPanGestureDetector());
    DALI_TEST_CHECK(dummyImpl.GetTapGestureDetector());
    DALI_TEST_CHECK(dummyImpl.GetLongPressGestureDetector());

    dummyImpl.DisableGestureDetection(GestureType::Value(GestureType::PINCH | GestureType::PAN | GestureType::TAP | GestureType::LONG_PRESS));

    DALI_TEST_CHECK(!dummyImpl.GetPinchGestureDetector());
    DALI_TEST_CHECK(!dummyImpl.GetPanGestureDetector());
    DALI_TEST_CHECK(!dummyImpl.GetTapGestureDetector());
    DALI_TEST_CHECK(!dummyImpl.GetLongPressGestureDetector());
  }

  // Disable When not enabled
  {
    DummyControl      dummy     = DummyControl::New();
    DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

    DALI_TEST_CHECK(!dummyImpl.GetPinchGestureDetector());
    DALI_TEST_CHECK(!dummyImpl.GetPanGestureDetector());
    DALI_TEST_CHECK(!dummyImpl.GetTapGestureDetector());
    DALI_TEST_CHECK(!dummyImpl.GetLongPressGestureDetector());

    dummyImpl.DisableGestureDetection(GestureType::Value(GestureType::PINCH | GestureType::PAN | GestureType::TAP | GestureType::LONG_PRESS));

    DALI_TEST_CHECK(!dummyImpl.GetPinchGestureDetector());
    DALI_TEST_CHECK(!dummyImpl.GetPanGestureDetector());
    DALI_TEST_CHECK(!dummyImpl.GetTapGestureDetector());
    DALI_TEST_CHECK(!dummyImpl.GetLongPressGestureDetector());
  }

  // Ensure control is detached if gesture detector is not deleted
  {
    DummyControl      dummy     = DummyControl::New();
    DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

    dummyImpl.EnableGestureDetection(GestureType::Value(GestureType::PINCH | GestureType::PAN | GestureType::TAP | GestureType::LONG_PRESS));

    PinchGestureDetector     pinch     = dummyImpl.GetPinchGestureDetector();
    PanGestureDetector       pan       = dummyImpl.GetPanGestureDetector();
    TapGestureDetector       tap       = dummyImpl.GetTapGestureDetector();
    LongPressGestureDetector longPress = dummyImpl.GetLongPressGestureDetector();

    DALI_TEST_EQUALS(0 == pinch.GetAttachedActorCount(), false, TEST_LOCATION);
    DALI_TEST_EQUALS(0 == pan.GetAttachedActorCount(), false, TEST_LOCATION);
    DALI_TEST_EQUALS(0 == tap.GetAttachedActorCount(), false, TEST_LOCATION);
    DALI_TEST_EQUALS(0 == longPress.GetAttachedActorCount(), false, TEST_LOCATION);

    dummyImpl.DisableGestureDetection(GestureType::Value(GestureType::PINCH | GestureType::PAN | GestureType::TAP | GestureType::LONG_PRESS));

    DALI_TEST_EQUALS(0 == pinch.GetAttachedActorCount(), true, TEST_LOCATION);
    DALI_TEST_EQUALS(0 == pan.GetAttachedActorCount(), true, TEST_LOCATION);
    DALI_TEST_EQUALS(0 == tap.GetAttachedActorCount(), true, TEST_LOCATION);
    DALI_TEST_EQUALS(0 == longPress.GetAttachedActorCount(), true, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliControlImplOnGestureMethods(void)
{
  ToolkitTestApplication application;

  // Check gesture actually happens
  {
    DummyControl dummy = DummyControl::New(true);
    dummy.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

    dummy.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    application.GetScene().Add(dummy);

    // Render and notify a couple of times
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.EnableGestureDetection(GestureType::Value(GestureType::PINCH | GestureType::PAN | GestureType::TAP | GestureType::LONG_PRESS));

    DALI_TEST_CHECK(dummyImpl.pinchCalled == false);

    TestGeneratePinch(application);

    DALI_TEST_CHECK(dummyImpl.pinchCalled == true);

    DALI_TEST_CHECK(dummyImpl.panCalled == false);

    TestGenerateMiniPan(application);

    DALI_TEST_CHECK(dummyImpl.panCalled == true);

    DALI_TEST_CHECK(dummyImpl.tapCalled == false);

    TestGenerateTap(application);

    DALI_TEST_CHECK(dummyImpl.tapCalled == true);

    DALI_TEST_CHECK(dummyImpl.longPressCalled == false);

    TestGenerateLongPress(application);

    DALI_TEST_CHECK(dummyImpl.longPressCalled == true);

    application.GetScene().Remove(dummy);
  }

  END_TEST;
}

int UtcDaliControlImplChildAddAndRemove(void)
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New(true);
    application.GetScene().Add(dummy);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());

    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS(dummyImpl.childAddCalled, false, TEST_LOCATION);
    DALI_TEST_EQUALS(dummy.GetChildCount(), 0u, TEST_LOCATION);
    Actor actor = Actor::New();
    dummy.Add(actor);
    DALI_TEST_EQUALS(dummyImpl.childAddCalled, true, TEST_LOCATION);
    DALI_TEST_EQUALS(dummy.GetChildCount(), 1u, TEST_LOCATION);

    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS(dummyImpl.childRemoveCalled, false, TEST_LOCATION);
    dummy.Remove(actor);
    DALI_TEST_EQUALS(dummyImpl.childRemoveCalled, true, TEST_LOCATION);
    DALI_TEST_EQUALS(dummy.GetChildCount(), 0u, TEST_LOCATION);

    application.Render();
    application.SendNotification();

    application.GetScene().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();
    application.GetScene().Add(dummy);

    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS(dummy.GetChildCount(), 0u, TEST_LOCATION);
    Actor actor = Actor::New();
    dummy.Add(actor);
    DALI_TEST_EQUALS(dummy.GetChildCount(), 1u, TEST_LOCATION);

    application.Render();
    application.SendNotification();

    dummy.Remove(actor);
    DALI_TEST_EQUALS(dummy.GetChildCount(), 0u, TEST_LOCATION);

    application.Render();
    application.SendNotification();

    application.GetScene().Remove(dummy);
  }
  END_TEST;
}

int UtcDaliControlImplStageConnection(void)
{
  ToolkitTestApplication application;

  {
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());

    DALI_TEST_EQUALS(dummyImpl.stageConnectionCalled, false, TEST_LOCATION);
    application.GetScene().Add(dummy);
    application.Render();
    application.SendNotification();
    DALI_TEST_EQUALS(dummyImpl.stageConnectionCalled, true, TEST_LOCATION);

    DALI_TEST_EQUALS(dummyImpl.stageDisconnectionCalled, false, TEST_LOCATION);
    application.GetScene().Remove(dummy);
    application.Render();
    application.SendNotification();
    DALI_TEST_EQUALS(dummyImpl.stageDisconnectionCalled, true, TEST_LOCATION);
  }

  // Ensure full code coverage
  {
    unsigned int stageChildren = application.GetScene().GetLayer(0).GetChildCount();
    DummyControl dummy         = DummyControl::New();

    DALI_TEST_EQUALS(application.GetScene().GetLayer(0).GetChildCount(), stageChildren, TEST_LOCATION);
    application.GetScene().Add(dummy);
    application.Render();
    application.SendNotification();
    DALI_TEST_EQUALS(application.GetScene().GetLayer(0).GetChildCount(), stageChildren + 1, TEST_LOCATION);

    application.GetScene().Remove(dummy);
    application.Render();
    application.SendNotification();
    DALI_TEST_EQUALS(application.GetScene().GetLayer(0).GetChildCount(), stageChildren, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliControlImplSizeSetP(void)
{
  ToolkitTestApplication application;

  {
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());

    application.GetScene().Add(dummy);
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS(dummyImpl.sizeSetCalled, false, TEST_LOCATION); // Size not set, no onSizeSet called
    Vector2 size(100.0f, 200.0f);
    dummy.SetProperty(Actor::Property::SIZE, size);

    DALI_TEST_EQUALS(dummyImpl.sizeSetCalled, false, TEST_LOCATION); // Size is going to get negotiated, no onSizeSet called

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(size, dummy.GetCurrentProperty<Vector3>(Actor::Property::SIZE).GetVectorXY(), TEST_LOCATION);
    DALI_TEST_EQUALS(dummyImpl.sizeSetCalled, true, TEST_LOCATION);

    application.GetScene().Remove(dummy);
  }

  END_TEST;
}

int UtcDaliControlImplSizeSet2P(void)
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New();
    application.GetScene().Add(dummy);

    Vector2 size(100.0f, 200.0f);
    DALI_TEST_CHECK(size != dummy.GetCurrentProperty<Vector3>(Actor::Property::SIZE).GetVectorXY());

    application.SendNotification();
    application.Render();

    dummy.SetProperty(Actor::Property::SIZE, size);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(size, dummy.GetCurrentProperty<Vector3>(Actor::Property::SIZE).GetVectorXY(), TEST_LOCATION);

    application.GetScene().Remove(dummy);
  }
  END_TEST;
}

int UtcDaliControlImplSizeAnimation(void)
{
  ToolkitTestApplication application;

  {
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());

    application.GetScene().Add(dummy);

    DALI_TEST_EQUALS(dummyImpl.sizeAnimationCalled, false, TEST_LOCATION);
    Animation animation = Animation::New(1.0f);
    animation.AnimateTo(Property(dummy, Actor::Property::SIZE), Vector3(100.0f, 150.0f, 200.0f));
    animation.Play();

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS(dummyImpl.sizeAnimationCalled, true, TEST_LOCATION);

    application.GetScene().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();

    application.GetScene().Add(dummy);

    Animation animation = Animation::New(1.0f);
    animation.AnimateTo(Property(dummy, Actor::Property::SIZE), Vector3(100.0f, 150.0f, 200.0f));
    animation.Play();

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    application.GetScene().Remove(dummy);
  }
  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int UtcDaliControlImplKeyEvent(void)
{
  ToolkitTestApplication application;

  {
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());

    application.GetScene().Add(dummy);
    dummy.SetKeyInputFocus();

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS(dummyImpl.keyEventCalled, false, TEST_LOCATION);
    Integration::KeyEvent keyEvent;
    application.ProcessEvent(keyEvent);
    DALI_TEST_EQUALS(dummyImpl.keyEventCalled, true, TEST_LOCATION);

    application.GetScene().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();

    application.GetScene().Add(dummy);
    dummy.SetKeyInputFocus();

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    Integration::KeyEvent keyEvent;
    application.ProcessEvent(keyEvent);

    application.GetScene().Remove(dummy);
  }
  END_TEST;
}

int UtcDaliControlImplKeyInputFocusGained(void)
{
  ToolkitTestApplication application;

  {
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());

    application.GetScene().Add(dummy);

    DALI_TEST_EQUALS(dummyImpl.keyInputFocusGained, false, TEST_LOCATION);

    dummy.SetKeyInputFocus();

    DALI_TEST_EQUALS(dummyImpl.keyInputFocusGained, true, TEST_LOCATION);

    application.GetScene().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();

    application.GetScene().Add(dummy);
    dummy.SetKeyInputFocus();
    application.GetScene().Remove(dummy);
  }
  END_TEST;
}

int UtcDaliControlImplKeyInputFocusLost(void)
{
  ToolkitTestApplication application;

  {
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());

    application.GetScene().Add(dummy);

    DALI_TEST_EQUALS(dummyImpl.keyInputFocusLost, false, TEST_LOCATION);

    dummy.SetKeyInputFocus();
    dummy.ClearKeyInputFocus();

    DALI_TEST_EQUALS(dummyImpl.keyInputFocusLost, true, TEST_LOCATION);

    application.GetScene().Remove(dummy);
  }

  // Ensure full code coverage
  {
    DummyControl dummy = DummyControl::New();

    application.GetScene().Add(dummy);
    dummy.SetKeyInputFocus();
    dummy.ClearKeyInputFocus();

    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());

    dummyImpl.IsKeyboardNavigationSupported();
    dummyImpl.IsKeyboardFocusGroup();

    application.GetScene().Remove(dummy);
  }
  END_TEST;
}

int UtcDaliControlImplTypeRegistry(void)
{
  ToolkitTestApplication application;

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo("Control");
  DALI_TEST_CHECK(type);
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK(handle);

  // Check if it's a control
  DALI_TEST_CHECK(Control::DownCast(handle));
  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int UtcDaliControlImplSetStyleName(void)
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControl::New(true);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
    dummy.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    application.GetScene().Add(dummy);

    dummy.SetStyleName("TestStyle");

    DALI_TEST_CHECK(dummy.GetStyleName() == "TestStyle");

    application.GetScene().Remove(dummy);
  }
  END_TEST;
}

int UtcDaliControlImplOnStyleChangeN(void)
{
  ToolkitTestApplication      application;
  Control                     dummy       = Control::New();
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(dummy);

  // test that style manager is being used, passing an empty handle does nothing but does not crash either
  Dali::Toolkit::StyleManager styleManager;
  controlImpl.OnStyleChange(styleManager, StyleChange::THEME_CHANGE);
  // no crash so test passes
  tet_result(TET_PASS);

  END_TEST;
}

int UtcDaliControlImplOnAccessibilityPanP(void)
{
  ToolkitTestApplication      application;
  Control                     dummy       = Control::New();
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(dummy);

  PanGesture pan;
  DALI_TEST_EQUALS(false, controlImpl.OnAccessibilityPan(pan), TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlImplOnAccessibilityActivatedP(void)
{
  ToolkitTestApplication application;

  Control                     dummy       = Control::New();
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(dummy);
  DALI_TEST_EQUALS(false, controlImpl.OnAccessibilityActivated(), TEST_LOCATION);

  // Invoke the control's activate action
  TypeInfo type = TypeRegistry::Get().GetTypeInfo("Control");
  DALI_TEST_CHECK(type);

  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK(handle);

  Property::Map attributes;
  DALI_TEST_EQUALS(false, handle.DoAction("accessibilityActivated", attributes), TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlImplGetNextKeyboardFocusableActorP(void)
{
  ToolkitTestApplication      application;
  Control                     dummy       = Control::New();
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(dummy);

  Actor currentFocusedActor;
  Actor result = controlImpl.GetNextKeyboardFocusableActor(currentFocusedActor, Control::KeyboardFocus::LEFT, false);

  DALI_TEST_EQUALS(result, currentFocusedActor, TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlImplRegisterThenReRegisterVisual(void)
{
  ToolkitTestApplication application;

  DummyControl      dummy     = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

  Property::Index index = 1;

  DALI_TEST_CHECK(!dummyImpl.GetVisual(index));

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base  visual;

  Property::Map map;
  map[Visual::Property::TYPE]           = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  visual = visualFactory.CreateVisual(map);
  DALI_TEST_CHECK(visual);

  // Register index with a color visual
  dummyImpl.RegisterVisual(index, visual);

  DALI_TEST_CHECK(dummyImpl.GetVisual(index) == visual);

  Property::Map newMap;
  newMap[Visual::Property::TYPE]           = Visual::COLOR;
  newMap[ColorVisual::Property::MIX_COLOR] = Color::BLUE;

  visual = visualFactory.CreateVisual(newMap);
  DALI_TEST_CHECK(visual);

  // ReRegister with altered color visual
  dummyImpl.RegisterVisual(index, visual);

  DALI_TEST_CHECK(dummyImpl.GetVisual(index) == visual);

  tet_result(TET_PASS);

  END_TEST;
}

int UtcDaliControlImplRegisterVisaulThenReRegisterToSelf(void)
{
  ToolkitTestApplication application;

  DummyControl      dummy     = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

  Property::Index index = 1;

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base  visual;

  Property::Map map;
  map[Visual::Property::TYPE]           = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  visual = visualFactory.CreateVisual(map);
  DALI_TEST_CHECK(visual);

  // Register index with a color visual
  dummyImpl.RegisterVisual(index, visual);

  DALI_TEST_CHECK(dummyImpl.GetVisual(index) == visual);

  // ReRegister to self
  dummyImpl.RegisterVisual(index, visual);

  DALI_TEST_CHECK(dummyImpl.GetVisual(index) == visual);

  END_TEST;
}

int UtcDaliControlImplRegisterVisualToSelf(void)
{
  ToolkitTestApplication application;

  Test::ObjectDestructionTracker objectDestructionTracker(application.GetCore().GetObjectRegistry());

  {
    DummyControl      dummy     = DummyControl::New();
    DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());
    objectDestructionTracker.Start(dummy);

    Property::Index index = 1;

    Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
    Toolkit::Visual::Base  visual;

    Property::Map map;
    map[Visual::Property::TYPE]           = Visual::COLOR;
    map[ColorVisual::Property::MIX_COLOR] = Color::RED;

    visual = visualFactory.CreateVisual(map);
    DALI_TEST_CHECK(visual);

    // Register to self
    dummyImpl.RegisterVisual(index, visual);

    DALI_TEST_EQUALS(objectDestructionTracker.IsDestroyed(), false, TEST_LOCATION); // Control not destroyed yet
    DALI_TEST_CHECK(dummyImpl.GetVisual(index) == visual);
  }

  DALI_TEST_EQUALS(objectDestructionTracker.IsDestroyed(), true, TEST_LOCATION); // Should be destroyed

  END_TEST;
}

int UtcDaliControlImplRegisterTwoVisuals(void)
{
  ToolkitTestApplication application;

  DummyControl      dummy     = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

  Property::Index index  = 1;
  Property::Index index2 = 2;

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base  visual;
  Toolkit::Visual::Base  secondVisual;

  Property::Map map;
  map[Visual::Property::TYPE]           = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  visual = visualFactory.CreateVisual(map);
  DALI_TEST_CHECK(visual);

  // Register index with a color visual
  dummyImpl.RegisterVisual(index, visual);

  Property::Map newMap;
  newMap[Visual::Property::TYPE]           = Visual::COLOR;
  newMap[ColorVisual::Property::MIX_COLOR] = Color::BLUE;

  secondVisual = visualFactory.CreateVisual(newMap);
  DALI_TEST_CHECK(secondVisual);

  // ReRegister with altered color visual
  dummyImpl.RegisterVisual(index2, secondVisual);

  DALI_TEST_CHECK(dummyImpl.GetVisual(index) == visual);
  DALI_TEST_CHECK(dummyImpl.GetVisual(index2) == secondVisual);

  END_TEST;
}

int UtcDaliControlImplRegisterUnregisterVisual(void)
{
  ToolkitTestApplication application;

  DummyControl      dummy     = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

  Property::Index index = 1;

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base  visual;

  Property::Map map;
  map[Visual::Property::TYPE]           = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  visual = visualFactory.CreateVisual(map);
  DALI_TEST_CHECK(visual);

  // Register index with a color visual
  dummyImpl.RegisterVisual(index, visual);

  tet_infoline("Add control with visual to stage and check renderer count is 1");
  application.GetScene().Add(dummy);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(dummyImpl.GetVisual(index) == visual);
  DALI_TEST_EQUALS(dummy.GetRendererCount(), 1, TEST_LOCATION);

  // Unregister visual
  dummyImpl.UnregisterVisual(index);

  tet_infoline("Remove control with visual from stage and check renderer count is 0");
  application.GetScene().Remove(dummy);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(dummy.GetRendererCount(), 0, TEST_LOCATION);
  DALI_TEST_CHECK(!dummyImpl.GetVisual(index));

  END_TEST;
}

int UtcDaliControlImplRegisterDisabledVisual(void)
{
  ToolkitTestApplication application;

  DummyControl      dummy     = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

  Property::Index TEST_PROPERTY = 1;

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base  visual;

  Property::Map map;
  map[Visual::Property::TYPE]           = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  visual = visualFactory.CreateVisual(map);
  DALI_TEST_CHECK(visual);

  // Register index with a color visual
  dummyImpl.RegisterVisual(TEST_PROPERTY, visual, false);

  DALI_TEST_CHECK(dummyImpl.GetVisual(TEST_PROPERTY) == visual);
  DALI_TEST_CHECK(dummyImpl.IsVisualEnabled(TEST_PROPERTY) == false);

  application.GetScene().Add(dummy);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(dummyImpl.IsVisualEnabled(TEST_PROPERTY) == false);

  DALI_TEST_CHECK(dummy.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE) == true);

  dummyImpl.EnableVisual(TEST_PROPERTY, true);

  DALI_TEST_CHECK(dummyImpl.IsVisualEnabled(TEST_PROPERTY) == true);

  END_TEST;
}

int UtcDaliControlImplDisableRegisteredVisual(void)
{
  ToolkitTestApplication application;

  DummyControl      dummy     = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

  Property::Index TEST_PROPERTY = 1;

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base  visual;

  Property::Map map;
  map[Visual::Property::TYPE]           = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  visual = visualFactory.CreateVisual(map);
  DALI_TEST_CHECK(visual);

  // Register index with a color visual
  dummyImpl.RegisterVisual(TEST_PROPERTY, visual);

  application.GetScene().Add(dummy);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(dummyImpl.IsVisualEnabled(TEST_PROPERTY) == true);

  DALI_TEST_CHECK(dummy.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE) == true);

  dummyImpl.EnableVisual(TEST_PROPERTY, false);

  DALI_TEST_CHECK(dummyImpl.IsVisualEnabled(TEST_PROPERTY) == false);

  END_TEST;
}

int UtcDaliControlImplEnabledVisualParentRemovedFromStage(void)
{
  // Visual enabled but then parent removed from stage, test ensures visual/renderer are also removed from stage.
  // Then adding parent back to stage should automatically put visual/renderer back

  ToolkitTestApplication application;

  DummyControl      dummy     = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

  Property::Index TEST_PROPERTY = 1;

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base  visual;

  Property::Map map;
  map[Visual::Property::TYPE]           = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  visual = visualFactory.CreateVisual(map);
  DALI_TEST_CHECK(visual);

  // Register index with a color visual
  dummyImpl.RegisterVisual(TEST_PROPERTY, visual, false);

  application.GetScene().Add(dummy);
  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(dummyImpl.IsVisualEnabled(TEST_PROPERTY) == false);
  DALI_TEST_CHECK(dummy.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE) == true);
  dummyImpl.EnableVisual(TEST_PROPERTY, true);

  // Render and notify
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(dummy.GetRendererCount() == 1u);

  // Remove control from stage, visual should be removed from stage too
  application.GetScene().Remove(dummy);
  // Render and notify
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(dummy.GetRendererCount() == 0u);

  application.GetScene().Add(dummy);
  // Render and notify
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(dummy.GetRendererCount() == 1u);

  DALI_TEST_CHECK(dummyImpl.IsVisualEnabled(TEST_PROPERTY) == true);

  END_TEST;
}

int UtcDaliControlImplRegisterTwoVisualsAndEnableOnlyOne(void)
{
  // Register 2 visuals and enable by default
  // Disable 1 visual
  // Remove control from stage then put it back
  // Check that only 1 visual/renderer is staged.

  ToolkitTestApplication application;

  DummyControl      dummy     = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

  Property::Index TEST_PROPERTY1 = 1;
  Property::Index TEST_PROPERTY2 = 2;

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base  visual1;
  Toolkit::Visual::Base  visual2;

  Property::Map map;
  map[Visual::Property::TYPE]           = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  Property::Map map2;
  map[Visual::Property::TYPE]           = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::BLUE;

  visual1 = visualFactory.CreateVisual(map);
  DALI_TEST_CHECK(visual1);

  visual2 = visualFactory.CreateVisual(map);
  DALI_TEST_CHECK(visual2);

  // Register index with a color visual
  dummyImpl.RegisterVisual(TEST_PROPERTY1, visual1);
  // Register second index with a color visual
  dummyImpl.RegisterVisual(TEST_PROPERTY2, visual2);

  application.GetScene().Add(dummy);
  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(dummy.GetRendererCount() == 2u);
  DALI_TEST_CHECK(dummyImpl.IsVisualEnabled(TEST_PROPERTY1) == true);
  DALI_TEST_CHECK(dummyImpl.IsVisualEnabled(TEST_PROPERTY1) == true);
  DALI_TEST_CHECK(dummy.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE) == true);
  dummyImpl.EnableVisual(TEST_PROPERTY2, false);

  // Render and notify
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(dummy.GetRendererCount() == 1u);

  // Remove control from stage, visual should be removed from stage too
  application.GetScene().Remove(dummy);
  // Render and notify
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(dummy.GetRendererCount() == 0u);

  application.GetScene().Add(dummy);
  // Render and notify
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(dummy.GetRendererCount() == 1u);

  DALI_TEST_CHECK(dummyImpl.IsVisualEnabled(TEST_PROPERTY1) == true);
  DALI_TEST_CHECK(dummyImpl.IsVisualEnabled(TEST_PROPERTY2) == false);

  END_TEST;
}
int UtcDaliControlImplAutoClippingWithVisuals(void)
{
  ToolkitTestApplication application;

  tet_infoline("Test to ensure a renderer does NOT get added when we've already registered a visual which we haven't enabled");

  DummyControl      control     = DummyControl::New();
  DummyControlImpl& controlImpl = static_cast<DummyControlImpl&>(control.GetImplementation());

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base  visual;
  Property::Map          map;
  map[Visual::Property::TYPE]           = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;
  visual                                = visualFactory.CreateVisual(map);
  DALI_TEST_CHECK(visual);
  controlImpl.RegisterVisual(Control::CONTROL_PROPERTY_END_INDEX + 1, visual, false);

  DALI_TEST_EQUALS(0, control.GetRendererCount(), TEST_LOCATION);

  control.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(0, control.GetRendererCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlImplAutoClippingWithVisualsAlreadyOnStage(void)
{
  ToolkitTestApplication application;

  tet_infoline("Test to ensure a renderer does NOT get added when we've already registered a visual which we haven't enabled and we're already on the stage");

  DummyControl      control     = DummyControl::New();
  DummyControlImpl& controlImpl = static_cast<DummyControlImpl&>(control.GetImplementation());

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base  visual;
  Property::Map          map;
  map[Visual::Property::TYPE]           = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;
  visual                                = visualFactory.CreateVisual(map);
  DALI_TEST_CHECK(visual);
  controlImpl.RegisterVisual(Control::CONTROL_PROPERTY_END_INDEX + 1, visual, false);

  DALI_TEST_EQUALS(0, control.GetRendererCount(), TEST_LOCATION);

  application.GetScene().Add(control);

  application.SendNotification();
  application.Render();

  control.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(0, control.GetRendererCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlImplOnPinch(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  control.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(control);

  application.SendNotification();
  application.Render();

  Toolkit::Internal::Control& impl = Toolkit::Internal::GetImplementation(control);
  impl.EnableGestureDetection(GestureType::PINCH);

  // Scale becomes 0.6666666
  TestStartPinch(application, Vector2(5.0f, 20.0f), Vector2(35.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);

  DALI_TEST_EQUALS(0.666f, control.GetProperty(Actor::Property::SCALE_X).Get<float>(), 0.01f, TEST_LOCATION);

  END_TEST;
}

// For ResourceReady
namespace
{
static bool gOnRelayoutCallBackCalled = false;
void        OnRelayoutCallback(Actor actor)
{
  gOnRelayoutCallBackCalled = true;
}

static bool gResourceReadyCalled = false;
void        OnResourceReady(Control control)
{
  gResourceReadyCalled = true;
}
} // namespace

int UtcDaliControlImplResourceReady(void)
{
  ToolkitTestApplication application;

  gOnRelayoutCallBackCalled = false;
  gResourceReadyCalled      = false;
  Control control           = Control::New();
  control.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  control.OnRelayoutSignal().Connect(OnRelayoutCallback);
  control.ResourceReadySignal().Connect(OnResourceReady);
  application.GetScene().Add(control);

  // Sanity check
  DALI_TEST_CHECK(!gOnRelayoutCallBackCalled);
  DALI_TEST_CHECK(!gResourceReadyCalled);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gOnRelayoutCallBackCalled, true, TEST_LOCATION);
  gOnRelayoutCallBackCalled = false;

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gOnRelayoutCallBackCalled, false, TEST_LOCATION);
  Toolkit::Internal::Control& impl = Toolkit::Internal::GetImplementation(control);
  // ResourceReady is true when there is no visual in the default Toolkit::Internal::Control.
  DALI_TEST_EQUALS(impl.IsResourceReady(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);
  impl.SetResourceReady();
  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gOnRelayoutCallBackCalled, false, TEST_LOCATION);
  gResourceReadyCalled = false;
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);
  impl.SetResourceReady();
  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gOnRelayoutCallBackCalled, false, TEST_LOCATION);

  END_TEST;
}
