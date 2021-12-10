/**
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/devel-api/adaptor-framework/accessibility-bridge.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>

#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/dbus-wrapper.h>

void utc_dali_toolkit_accessibility_accessible_startup(void)
{
  test_return_value = TET_UNDEF;
  DBusWrapper::Install(std::unique_ptr<DBusWrapper>(new TestDBusWrapper));
}

void utc_dali_toolkit_accessibility_accessible_cleanup(void)
{
  test_return_value = TET_PASS;
}

int utcDaliAccessibilityCheckBoxButtonGetStates(void)
{
  ToolkitTestApplication application;

  auto check_box_button = Toolkit::CheckBoxButton::New();
  auto q = Dali::Accessibility::Accessible::Get( check_box_button );
  DALI_TEST_CHECK( q );
  auto states = q->GetStates();
  DALI_TEST_EQUALS( (int) states[ Dali::Accessibility::State::SELECTABLE ], (int) true, TEST_LOCATION );

  END_TEST;
}

int utcDaliAccessibilityCheckLabelText(void)
{
  ToolkitTestApplication application;

  auto check_box_button = Toolkit::CheckBoxButton::New();
  //check_box_button.SetLabelText( "button" );
  check_box_button.SetProperty(Toolkit::Button::Property::LABEL, "button");
  auto q = Dali::Accessibility::Accessible::Get( check_box_button );
  DALI_TEST_CHECK( q );
  DALI_TEST_EQUALS( q->GetName(), "button", TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityCheckShowingState(void)
{
  ToolkitTestApplication application;

  auto parentButton = Toolkit::PushButton::New();
  parentButton.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
  parentButton.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  parentButton.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  parentButton.SetProperty(Actor::Property::POSITION, Dali::Vector2(0.0f, 0.0f));
  parentButton.SetProperty(Actor::Property::SIZE, Dali::Vector2(200.0f, 200.0f));
  application.GetScene().Add( parentButton );

  // Toatally inside of parent
  auto buttonA = Toolkit::PushButton::New();
  buttonA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  buttonA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  buttonA.SetProperty(Actor::Property::POSITION, Dali::Vector2(0.0f, 0.0f));
  buttonA.SetProperty(Actor::Property::SIZE, Dali::Vector2(100.0f, 100.0f));
  parentButton.Add(buttonA);

  // Toatally outside of parent
  auto buttonB = Toolkit::PushButton::New();
  buttonB.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  buttonB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  buttonB.SetProperty(Actor::Property::POSITION, Dali::Vector2(300.0f, 300.0f));
  buttonB.SetProperty(Actor::Property::SIZE, Dali::Vector2(100.0f, 100.0f));
  parentButton.Add(buttonB);

  // Partially inside of parent
  auto buttonC = Toolkit::PushButton::New();
  buttonC.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  buttonC.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  buttonC.SetProperty(Actor::Property::POSITION, Dali::Vector2(100.0f,100.0f));
  buttonC.SetProperty(Actor::Property::SIZE, Dali::Vector2(200.0f, 200.0f));
  parentButton.Add(buttonC);

  application.SendNotification();
  application.Render(16);

  auto q = Dali::Accessibility::Accessible::Get(buttonA);
  DALI_TEST_CHECK(q);
  auto states = q->GetStates();
  DALI_TEST_EQUALS((int) states[Dali::Accessibility::State::SHOWING], (int) true, TEST_LOCATION);

  q = Dali::Accessibility::Accessible::Get(buttonB);
  DALI_TEST_CHECK(q);
  states = q->GetStates();
  DALI_TEST_EQUALS((int) states[Dali::Accessibility::State::SHOWING], (int) false, TEST_LOCATION);

  q = Dali::Accessibility::Accessible::Get(buttonC);
  DALI_TEST_CHECK(q);
  states = q->GetStates();
  DALI_TEST_EQUALS((int) states[Dali::Accessibility::State::SHOWING], (int) true, TEST_LOCATION);

  // Make SHOWING object invisible
  buttonC.SetProperty(Actor::Property::VISIBLE, false);

  application.SendNotification();
  application.Render(16);

  states = q->GetStates();
  DALI_TEST_EQUALS((int) states[Dali::Accessibility::State::SHOWING], (int) false, TEST_LOCATION);

  // Make SHOWING parent invisible
  parentButton.SetProperty(Actor::Property::VISIBLE, false);

  application.SendNotification();
  application.Render(16);

  q = Dali::Accessibility::Accessible::Get(buttonA);
  DALI_TEST_CHECK(q);
  states = q->GetStates();
  DALI_TEST_EQUALS((int) states[Dali::Accessibility::State::SHOWING], (int) false, TEST_LOCATION);

  END_TEST;
}
