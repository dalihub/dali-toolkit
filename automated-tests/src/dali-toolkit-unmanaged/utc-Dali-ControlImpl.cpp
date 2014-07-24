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

#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/mouse-wheel-event-integ.h>
#include <dali/integration-api/events/long-press-gesture-event.h>
#include <dali/integration-api/events/pinch-gesture-event.h>
#include <dali/integration-api/events/pan-gesture-event.h>
#include <dali/integration-api/events/tap-gesture-event.h>
#include <dali/integration-api/events/touch-event-integ.h>

#include "dummy-control.h"

using namespace Dali;
using namespace Dali::Toolkit;


void control_impl_startup(void)
{
  test_return_value = TET_UNDEF;
}

void control_impl_cleanup(void)
{
  test_return_value = TET_PASS;
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
static bool MouseWheelEventCallback(Actor actor, const MouseWheelEvent& event)
{
  return false;
}
}

int UtcDaliControlImplMouseWheelEvent(void)
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
    application.ProcessEvent(event);
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
    application.ProcessEvent(event);

    Stage::GetCurrent().Remove(dummy);
  }
  END_TEST;
}
