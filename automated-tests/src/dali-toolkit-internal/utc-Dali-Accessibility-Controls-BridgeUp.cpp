/**
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/accessibility-test-utils.h>
#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/dbus-wrapper.h>
#include <automated-tests/src/dali-toolkit/dali-toolkit-test-utils/toolkit-timer.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/buttons/toggle-button.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/popup/popup.h>
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali-toolkit/devel-api/controls/web-view/web-view.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/accessibility-bridge.h>
#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/devel-api/atspi-interfaces/action.h>
#include <dali/devel-api/atspi-interfaces/component.h>
#include <dali/devel-api/common/stage.h>

#include <cstdlib>

using namespace Dali::Toolkit;

namespace
{
const auto flushCoalescableMessage = [](Dali::ToolkitTestApplication& application) {
  Dali::Timer timer = Timer::New(0);
  for(int i = 0; i < 11; ++i)
  {
    application.SendNotification();
    application.Render();
    timer.MockEmitSignal();
  }
};

static void TestVoidCallback()
{
}

static void Wait(ToolkitTestApplication& application)
{
  application.SendNotification();
  application.Render(16);
}
} // namespace

void utc_dali_toolkit_accessibility_control_bridgeup_startup(void)
{
  test_return_value = TET_UNDEF;
  DBusWrapper::Install(std::unique_ptr<DBusWrapper>(new TestDBusWrapper));
}

void utc_dali_toolkit_accessibility_control_bridgeup_cleanup(void)
{
  test_return_value = TET_PASS;
  DBusWrapper::Install({}); // Clean up TestDBusWrapper
}

namespace Dali
{
namespace Accessibility
{
std::ostream& operator<<(std::ostream& stream, const Address& address)
{
  stream << address.ToString();
  return stream;
}
} // namespace Accessibility
} // namespace Dali

int UtcDaliControlAccessibilityRaiseBridge(void)
{
  DALI_TEST_CHECK(!Accessibility::IsUp());

  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  DALI_TEST_CHECK(Accessibility::IsUp());

  // auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
  // for(auto &a : wr->daliMethods) {
  //   const char *mt;
  //   if (std::get<3>(a.first) == MethodType::Method) mt = "MethodType::Method";
  //   else if (std::get<3>(a.first) == MethodType::Getter) mt = "MethodType::Getter";
  //   else if (std::get<3>(a.first) == MethodType::Setter) mt = "MethodType::Setter";
  //   else assert(0);
  //   printf("%s %s %s %s\n", std::get<0>(a.first).c_str(), std::get<1>(a.first).c_str(), std::get<2>(a.first).c_str(), mt);
  // }

  Dali::Accessibility::TestEnableSC(false);

  DALI_TEST_CHECK(!Accessibility::IsUp());

  END_TEST;
}

int UtcDaliControlAccessibilityName(void)
{
  ToolkitTestApplication application;

  auto control = Control::New();
  Stage::GetCurrent().Add(control);

  auto q = Dali::Accessibility::Accessible::Get(control);
  DALI_TEST_CHECK(q);

  DALI_TEST_EQUALS("", q->GetName(), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_NAME, "Accessibility_Name");
  DALI_TEST_EQUALS("Accessibility_Name", q->GetName(), TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetProperty(DevelControl::Property::ACCESSIBILITY_NAME).Get<std::string>(), "Accessibility_Name", TEST_LOCATION);

  auto nameChangeCallback = [](std::string& accessibilityName) { accessibilityName = "Accessibility_Name_With_Callback"; };

  DevelControl::AccessibilityGetNameSignal(control).Connect(nameChangeCallback);

  DALI_TEST_EQUALS("Accessibility_Name_With_Callback", q->GetName(), TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(true);
  DALI_TEST_CHECK(!Dali::Accessibility::TestPropertyChangeCalled());

  DALI_TEST_EQUALS("Accessibility_Name_With_Callback", TestGetName(q->GetAddress()), TEST_LOCATION);

  DevelControl::AccessibilityGetNameSignal(control).Disconnect(nameChangeCallback);

  auto i = dynamic_cast<Dali::Accessibility::Component*>(q);

  DALI_TEST_CHECK(i);
  i->GrabHighlight();

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_NAME, "Changed_Accessiblity_Name");
  DALI_TEST_EQUALS("Changed_Accessiblity_Name", q->GetName(), TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetProperty(DevelControl::Property::ACCESSIBILITY_NAME).Get<std::string>(), "Changed_Accessiblity_Name", TEST_LOCATION);

  // test emission of property change signal
  DALI_TEST_CHECK(Dali::Accessibility::TestPropertyChangeCalled());

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliControlAccessibilityDescription(void)
{
  ToolkitTestApplication application;

  auto control = Control::New();

  auto q = Dali::Accessibility::Accessible::Get(control);
  DALI_TEST_CHECK(q);

  DALI_TEST_EQUALS("", q->GetDescription(), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_DESCRIPTION, "Accessibility_Description");
  DALI_TEST_EQUALS("Accessibility_Description", q->GetDescription(), TEST_LOCATION);

  auto property = control.GetProperty(DevelControl::Property::ACCESSIBILITY_DESCRIPTION).Get<std::string>();
  DALI_TEST_EQUALS("Accessibility_Description", property, TEST_LOCATION);

  auto descriptionChangeCallback = [](std::string& accessibilityDescription) { accessibilityDescription = "Accessibility_Description_With_Callback"; };

  DevelControl::AccessibilityGetDescriptionSignal(control).Connect(descriptionChangeCallback);

  DALI_TEST_EQUALS("Accessibility_Description_With_Callback", q->GetDescription(), TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(true);
  DALI_TEST_CHECK(!Dali::Accessibility::TestPropertyChangeCalled());

  DALI_TEST_EQUALS("Accessibility_Description_With_Callback", TestGetDescription(q->GetAddress()), TEST_LOCATION);

  DevelControl::AccessibilityGetDescriptionSignal(control).Disconnect(descriptionChangeCallback);

  auto i = dynamic_cast<Dali::Accessibility::Component*>(q);

  DALI_TEST_CHECK(i);
  i->GrabHighlight();

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_DESCRIPTION, "Changed_Accessiblity_Description");
  DALI_TEST_EQUALS("Changed_Accessiblity_Description", q->GetDescription(), TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetProperty(DevelControl::Property::ACCESSIBILITY_DESCRIPTION).Get<std::string>(), "Changed_Accessiblity_Description", TEST_LOCATION);

  // test emission of property change signal
  DALI_TEST_CHECK(Dali::Accessibility::TestPropertyChangeCalled());

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliControlAccessibilityValue(void)
{
  ToolkitTestApplication application;

  auto control = Control::New();

  auto q = Dali::Accessibility::Accessible::Get(control);
  DALI_TEST_CHECK(q);

  DALI_TEST_EQUALS("", q->GetValue(), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_VALUE, "Accessibility_Value");
  DALI_TEST_EQUALS("Accessibility_Value", q->GetValue(), TEST_LOCATION);

  auto property = control.GetProperty(DevelControl::Property::ACCESSIBILITY_VALUE).Get<std::string>();
  DALI_TEST_EQUALS("Accessibility_Value", property, TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(true);
  DALI_TEST_CHECK(!Dali::Accessibility::TestPropertyChangeCalled());

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_VALUE, "Changed_Accessiblity_Value");
  DALI_TEST_EQUALS("Changed_Accessiblity_Value", q->GetValue(), TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetProperty(DevelControl::Property::ACCESSIBILITY_VALUE).Get<std::string>(), "Changed_Accessiblity_Value", TEST_LOCATION);

  // value property change signal is not emitted if not highlighted
  DALI_TEST_CHECK(!Dali::Accessibility::TestPropertyChangeCalled());

  auto component = dynamic_cast<Dali::Accessibility::Component*>(q);

  DALI_TEST_CHECK(component);
  component->GrabHighlight();

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_VALUE, "Changed_Accessiblity_Value_2");
  DALI_TEST_EQUALS("Changed_Accessiblity_Value_2", q->GetValue(), TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetProperty(DevelControl::Property::ACCESSIBILITY_VALUE).Get<std::string>(), "Changed_Accessiblity_Value_2", TEST_LOCATION);

  // value property change signal is emitted if highlighted
  DALI_TEST_CHECK(Dali::Accessibility::TestPropertyChangeCalled());

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliControlAccessibilityRole(void)
{
  ToolkitTestApplication application;

  auto control = Control::New();

  auto role_none       = DevelControl::AccessibilityRole::NONE;
  auto role_unknown    = Dali::Accessibility::Role::UNKNOWN;
  auto role_pushbutton = Dali::Accessibility::Role::PUSH_BUTTON;

  DALI_TEST_EQUALS(role_none, control.GetProperty(DevelControl::Property::ACCESSIBILITY_ROLE).Get<DevelControl::AccessibilityRole>(), TEST_LOCATION);

  auto accessible = Dali::Accessibility::Accessible::Get(control);
  DALI_TEST_EQUALS(role_unknown, accessible->GetRole(), TEST_LOCATION);
  DALI_TEST_EQUALS("unknown", accessible->GetRoleName(), TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(true);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(static_cast<uint32_t>(role_unknown), TestGetRole(accessible->GetAddress()), TEST_LOCATION);
  DALI_TEST_EQUALS("unknown", TestGetRoleName(accessible->GetAddress()), TEST_LOCATION);
  DALI_TEST_EQUALS("unknown", TestGetLocalizedRoleName(accessible->GetAddress()), TEST_LOCATION);

  // Invalid role
  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, 9999);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::UNKNOWN), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  // V2 Roles
  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::ADJUSTABLE);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::SLIDER), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::ALERT);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::ALERT), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::BUTTON);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::PUSH_BUTTON), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::CHECK_BOX);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::CHECK_BOX), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::COMBO_BOX);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::COMBO_BOX), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::CONTAINER);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::FILLER), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::DIALOG);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::DIALOG), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::ENTRY);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::ENTRY), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::HEADER);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::HEADER), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::IMAGE);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::IMAGE), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::LINK);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::LINK), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::LIST);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::LIST), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::LIST_ITEM);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::LIST_ITEM), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::MENU);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::MENU), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::MENU_BAR);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::MENU_BAR), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::MENU_ITEM);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::MENU_ITEM), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::NONE);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::UNKNOWN), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::NOTIFICATION);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::NOTIFICATION), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::PASSWORD_TEXT);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::PASSWORD_TEXT), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::POPUP_MENU);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::POPUP_MENU), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::PROGRESS_BAR);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::PROGRESS_BAR), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::RADIO_BUTTON);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::RADIO_BUTTON), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::SCROLL_BAR);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::SCROLL_BAR), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::SPIN_BUTTON);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::SPIN_BUTTON), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::TAB);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::PAGE_TAB), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::TAB_LIST);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::PAGE_TAB_LIST), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::TEXT);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::LABEL), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::TOGGLE_BUTTON);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::TOGGLE_BUTTON), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::TOOL_BAR);
  DALI_TEST_EQUALS(static_cast<uint32_t>(Dali::Accessibility::Role::TOOL_BAR), TestGetRole(accessible->GetAddress()), TEST_LOCATION);

  // Directly sets ATSPI role  (V1)
  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::PUSH_BUTTON);
  DALI_TEST_EQUALS(static_cast<uint32_t>(role_pushbutton), TestGetRole(accessible->GetAddress()), TEST_LOCATION);
  DALI_TEST_EQUALS("push button", TestGetRoleName(accessible->GetAddress()), TEST_LOCATION);
  DALI_TEST_EQUALS("push button", TestGetLocalizedRoleName(accessible->GetAddress()), TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);

  DALI_TEST_EQUALS(role_pushbutton, accessible->GetRole(), TEST_LOCATION);
  DALI_TEST_EQUALS("push button", accessible->GetRoleName(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlAccessibilityRoleToggleButton(void)
{
  ToolkitTestApplication application;

  auto control = Dali::Toolkit::ToggleButton::New();
  auto button  = Dali::Accessibility::Role::TOGGLE_BUTTON;

  control.SetProperty(Toolkit::ToggleButton::Property::TOOLTIPS,
                      Property::Array{"option1", "option2"});

  Dali::Accessibility::TestEnableSC(true);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, button);
  auto q = Dali::Accessibility::Accessible::Get(control);

  DALI_TEST_EQUALS(button, q->GetRole(), TEST_LOCATION);
  DALI_TEST_EQUALS("toggle button", q->GetRoleName(), TEST_LOCATION);

  Dali::Accessibility::States states = q->GetStates();
  DALI_TEST_EQUALS(true, (bool)states[Dali::Accessibility::State::VISIBLE], TEST_LOCATION);

  DALI_TEST_EQUALS("option1", q->GetDescription(), TEST_LOCATION);

  auto i = dynamic_cast<Dali::Accessibility::Component*>(q);

  DALI_TEST_CHECK(i);
  i->GrabHighlight();

  control.SetProperty(Toolkit::Button::Property::LABEL, "ToggleButton2");
  DALI_TEST_EQUALS("ToggleButton2", TestGetName(q->GetAddress()), TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliControlAccessibilityButtonLabel(void)
{
  ToolkitTestApplication application;

  auto control = Dali::Toolkit::PushButton::New();
  auto button  = Dali::Accessibility::Role::PUSH_BUTTON;

  Dali::Accessibility::TestEnableSC(true);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, button);

  auto q = Dali::Accessibility::Accessible::Get(control);
  auto i = dynamic_cast<Dali::Accessibility::Component*>(q);

  DALI_TEST_CHECK(i);
  i->GrabHighlight();

  control.SetProperty(Toolkit::Button::Property::LABEL, "Button2");

  DALI_TEST_EQUALS("Button2", TestGetName(q->GetAddress()), TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliControlAccessibilityState(void)
{
  ToolkitTestApplication application;

  auto control = Control::New();
  control.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
  control.SetProperty(Actor::Property::POSITION, Vector3(10, 10, 100));

  application.GetScene().Add(control);
  auto accessible = Dali::Accessibility::Accessible::Get(control);

  Dali::Accessibility::TestEnableSC(true);
  DALI_TEST_CHECK(!Dali::Accessibility::TestStateChangedCalled());

  // Test setting AccessibilityState property updates at-spi states
  DevelControl::AccessibilityStates inputStates;
  {
    inputStates[DevelControl::AccessibilityState::ENABLED] = false;
    inputStates[DevelControl::AccessibilityState::CHECKED] = true;
    inputStates[DevelControl::AccessibilityState::BUSY]    = true;

    control.SetProperty(DevelControl::Property::ACCESSIBILITY_STATES, static_cast<int32_t>(inputStates.GetRawData32()));

    DALI_TEST_CHECK(!Dali::Accessibility::TestStateChangedCalled());

    auto states = DevelControl::GetAccessibilityStates(control);
    DALI_TEST_CHECK(!states[Dali::Accessibility::State::ENABLED]);
    DALI_TEST_CHECK(!states[Dali::Accessibility::State::SELECTED]);
    DALI_TEST_CHECK(states[Dali::Accessibility::State::CHECKED]);
    DALI_TEST_CHECK(states[Dali::Accessibility::State::BUSY]);
    DALI_TEST_CHECK(!states[Dali::Accessibility::State::EXPANDED]);
  }

  // state-changed:checked event is NOT emitted if the object is not highlighted
  {
    control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::CHECK_BOX);

    inputStates[DevelControl::AccessibilityState::CHECKED] = false; // CHECKED: true -> false

    control.SetProperty(DevelControl::Property::ACCESSIBILITY_STATES, static_cast<int32_t>(inputStates.GetRawData32()));

    DALI_TEST_CHECK(!Dali::Accessibility::TestStateChangedCalled());

    auto states = DevelControl::GetAccessibilityStates(control);
    DALI_TEST_CHECK(!states[Dali::Accessibility::State::ENABLED]);
    DALI_TEST_CHECK(!states[Dali::Accessibility::State::SELECTED]);
    DALI_TEST_CHECK(!states[Dali::Accessibility::State::CHECKED]);
    DALI_TEST_CHECK(states[Dali::Accessibility::State::BUSY]);
    DALI_TEST_CHECK(!states[Dali::Accessibility::State::EXPANDED]);
  }

  auto component = dynamic_cast<Dali::Accessibility::Component*>(accessible);
  component->GrabHighlight();

  // state-changed:checked event is emitted if the object is highlighted and checkable
  const std::array<DevelControl::AccessibilityRole, 3> checkableRoles{DevelControl::AccessibilityRole::CHECK_BOX, DevelControl::AccessibilityRole::RADIO_BUTTON, DevelControl::AccessibilityRole::TOGGLE_BUTTON};
  for(auto role : checkableRoles)
  {
    control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, role);

    // CHECKED: false -> true
    inputStates[DevelControl::AccessibilityState::CHECKED] = true;
    control.SetProperty(DevelControl::Property::ACCESSIBILITY_STATES, static_cast<int32_t>(inputStates.GetRawData32()));

    DALI_TEST_CHECK(Dali::Accessibility::TestStateChangedCalled());
    DALI_TEST_CHECK(Dali::Accessibility::TestStateChangedResult("checked", 1));

    auto states = DevelControl::GetAccessibilityStates(control);
    DALI_TEST_CHECK(states[Dali::Accessibility::State::CHECKED]);

    Dali::Accessibility::TestResetStateChangedResult();
    flushCoalescableMessage(application);

    // CHECKED: true -> false
    inputStates[DevelControl::AccessibilityState::CHECKED] = false;
    control.SetProperty(DevelControl::Property::ACCESSIBILITY_STATES, static_cast<int32_t>(inputStates.GetRawData32()));

    DALI_TEST_CHECK(Dali::Accessibility::TestStateChangedCalled());
    DALI_TEST_CHECK(Dali::Accessibility::TestStateChangedResult("checked", 0));

    states = DevelControl::GetAccessibilityStates(control);
    DALI_TEST_CHECK(!states[Dali::Accessibility::State::CHECKED]);

    Dali::Accessibility::TestResetStateChangedResult();
    flushCoalescableMessage(application);
  }

  // state-changed:selected event is emitted if the object is highlighted and selectable
  const std::array<DevelControl::AccessibilityRole, 4> selectableRoles{DevelControl::AccessibilityRole::BUTTON, DevelControl::AccessibilityRole::LIST_ITEM, DevelControl::AccessibilityRole::MENU_ITEM, DevelControl::AccessibilityRole::TAB};
  for(auto role : selectableRoles)
  {
    control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, role);

    // SELECTED: false -> true
    inputStates[DevelControl::AccessibilityState::SELECTED] = true;
    control.SetProperty(DevelControl::Property::ACCESSIBILITY_STATES, static_cast<int32_t>(inputStates.GetRawData32()));

    DALI_TEST_CHECK(Dali::Accessibility::TestStateChangedCalled());
    DALI_TEST_CHECK(Dali::Accessibility::TestStateChangedResult("selected", 1));

    auto states = DevelControl::GetAccessibilityStates(control);
    DALI_TEST_CHECK(states[Dali::Accessibility::State::SELECTED]);

    Dali::Accessibility::TestResetStateChangedResult();
    flushCoalescableMessage(application);

    // SELECTED: true -> false
    inputStates[DevelControl::AccessibilityState::SELECTED] = false;
    control.SetProperty(DevelControl::Property::ACCESSIBILITY_STATES, static_cast<int32_t>(inputStates.GetRawData32()));

    DALI_TEST_CHECK(Dali::Accessibility::TestStateChangedCalled());
    DALI_TEST_CHECK(Dali::Accessibility::TestStateChangedResult("selected", 0));

    states = DevelControl::GetAccessibilityStates(control);
    DALI_TEST_CHECK(!states[Dali::Accessibility::State::SELECTED]);
    Dali::Accessibility::TestResetStateChangedResult();
    flushCoalescableMessage(application);
  }

  // state-changed event is NOT emitted if object is not checkable or selectable
  {
    control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::CONTAINER);

    inputStates[DevelControl::AccessibilityState::CHECKED]  = true; // CHECKED: false -> true
    inputStates[DevelControl::AccessibilityState::SELECTED] = true; // SELECTED: false -> true

    control.SetProperty(DevelControl::Property::ACCESSIBILITY_STATES, static_cast<int32_t>(inputStates.GetRawData32()));

    DALI_TEST_CHECK(!Dali::Accessibility::TestStateChangedCalled());

    auto states = DevelControl::GetAccessibilityStates(control);
    DALI_TEST_CHECK(states[Dali::Accessibility::State::SELECTED]);
    DALI_TEST_CHECK(states[Dali::Accessibility::State::CHECKED]);
  }

  // state-changed event is NOT emitted if object is v1 role
  {
    control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::CHECK_BOX);

    inputStates[DevelControl::AccessibilityState::CHECKED] = false; // CHECKED: true -> false

    control.SetProperty(DevelControl::Property::ACCESSIBILITY_STATES, static_cast<int32_t>(inputStates.GetRawData32()));

    DALI_TEST_CHECK(!Dali::Accessibility::TestStateChangedCalled());

    auto states = DevelControl::GetAccessibilityStates(control);
    DALI_TEST_CHECK(!states[Dali::Accessibility::State::CHECKED]);
  }

  // Test bridge behavior
  {
    auto states_by_bridge = Dali::Accessibility::States{TestGetStates(accessible->GetAddress())};
    auto states           = DevelControl::GetAccessibilityStates(control);
    DALI_TEST_CHECK(states_by_bridge == states);
  }

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliControlAccessibilityModal(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  // Modal state is set by Dialog role
  {
    auto control    = Dali::Toolkit::Popup::New();
    auto accessible = Dali::Accessibility::Accessible::Get(control);

    auto states_by_bridge = Dali::Accessibility::States{TestGetStates(accessible->GetAddress())};
    DALI_TEST_CHECK(states_by_bridge[Dali::Accessibility::State::MODAL]);
  }

  // Modal state is set by isModal property
  {
    auto control = Control::New();
    control.SetProperty(DevelControl::Property::ACCESSIBILITY_IS_MODAL, true);

    auto accessible = Dali::Accessibility::Accessible::Get(control);

    auto states_by_bridge = Dali::Accessibility::States{TestGetStates(accessible->GetAddress())};
    DALI_TEST_CHECK(states_by_bridge[Dali::Accessibility::State::MODAL]);
  }

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliControlAccessibilityHighlightable(void)
{
  ToolkitTestApplication application;
  auto                   control = Control::New();

  auto highlightable = control.GetProperty<bool>(DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE);
  DALI_TEST_EQUALS(highlightable, false, TEST_LOCATION);

  // negative testcase - trying to set unconvertible value
  control.SetProperty(DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, "deadbeef");
  highlightable = control.GetProperty<bool>(DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE);
  DALI_TEST_EQUALS(highlightable, false, TEST_LOCATION);

  auto accessible = Dali::Accessibility::Accessible::Get(control);

  Dali::Accessibility::TestEnableSC(true);

  auto states_by_bridge = Dali::Accessibility::States{TestGetStates(accessible->GetAddress())};
  DALI_TEST_CHECK(!states_by_bridge[Dali::Accessibility::State::HIGHLIGHTABLE]);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, true);
  DALI_TEST_EQUALS(Property::BOOLEAN, control.GetProperty(DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE).GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, control.GetProperty(DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE).Get<bool>(), TEST_LOCATION);

  states_by_bridge = Dali::Accessibility::States{TestGetStates(accessible->GetAddress())};
  DALI_TEST_CHECK(states_by_bridge[Dali::Accessibility::State::HIGHLIGHTABLE]);

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, false);
  DALI_TEST_EQUALS(Property::BOOLEAN, control.GetProperty(DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE).GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, control.GetProperty(DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE).Get<bool>(), TEST_LOCATION);

  states_by_bridge = Dali::Accessibility::States{TestGetStates(accessible->GetAddress())};
  DALI_TEST_CHECK(!states_by_bridge[Dali::Accessibility::State::HIGHLIGHTABLE]);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliControlAccessibilityHighlightableV2(void)
{
  ToolkitTestApplication application;
  auto                   control    = Control::New();
  auto                   accessible = Dali::Accessibility::Accessible::Get(control);

  Dali::Accessibility::TestEnableSC(true);

  auto states_by_bridge = Dali::Accessibility::States{TestGetStates(accessible->GetAddress())};
  // Is not highlightable if no role is set
  DALI_TEST_CHECK(!states_by_bridge[Dali::Accessibility::State::HIGHLIGHTABLE]);

  // Is highlightable by default if V2 role is set and is not Role::None
  control.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::CONTAINER);
  states_by_bridge = Dali::Accessibility::States{TestGetStates(accessible->GetAddress())};
  DALI_TEST_CHECK(states_by_bridge[Dali::Accessibility::State::HIGHLIGHTABLE]);

  // Returns explicitly set highlightable property: false
  control.SetProperty(DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, false);
  states_by_bridge = Dali::Accessibility::States{TestGetStates(accessible->GetAddress())};
  DALI_TEST_CHECK(!states_by_bridge[Dali::Accessibility::State::HIGHLIGHTABLE]);

  // Returns explicitly set highlightable property: true
  control.SetProperty(DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, true);
  states_by_bridge = Dali::Accessibility::States{TestGetStates(accessible->GetAddress())};
  DALI_TEST_CHECK(states_by_bridge[Dali::Accessibility::State::HIGHLIGHTABLE]);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliControlAccessibilityScrollable(void)
{
  ToolkitTestApplication application;
  auto                   control = Control::New();

  auto scrollable = control.GetProperty<bool>(DevelControl::Property::ACCESSIBILITY_SCROLLABLE);
  DALI_TEST_EQUALS(scrollable, false, TEST_LOCATION);

  // negative testcase - trying to set unconvertible value
  control.SetProperty(DevelControl::Property::ACCESSIBILITY_SCROLLABLE, "deadbeef");
  scrollable = control.GetProperty<bool>(DevelControl::Property::ACCESSIBILITY_SCROLLABLE);
  DALI_TEST_EQUALS(scrollable, false, TEST_LOCATION);

  auto accessible = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(control));

  Dali::Accessibility::TestEnableSC(true);

  DALI_TEST_CHECK(!accessible->IsScrollable());

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_SCROLLABLE, true);
  DALI_TEST_EQUALS(Property::BOOLEAN, control.GetProperty(DevelControl::Property::ACCESSIBILITY_SCROLLABLE).GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, control.GetProperty(DevelControl::Property::ACCESSIBILITY_SCROLLABLE).Get<bool>(), TEST_LOCATION);

  DALI_TEST_CHECK(accessible->IsScrollable());

  control.SetProperty(DevelControl::Property::ACCESSIBILITY_SCROLLABLE, false);
  DALI_TEST_EQUALS(Property::BOOLEAN, control.GetProperty(DevelControl::Property::ACCESSIBILITY_SCROLLABLE).GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, control.GetProperty(DevelControl::Property::ACCESSIBILITY_SCROLLABLE).Get<bool>(), TEST_LOCATION);

  DALI_TEST_CHECK(!accessible->IsScrollable());

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliControlAccessibilityHighlightBridgeUp(void)
{
  ToolkitTestApplication application;

  auto controla = Control::New();
  auto controlb = Control::New();
  controla.Add(controlb);

  controla.SetProperty(DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, true);
  controlb.SetProperty(DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, true);

  Dali::Accessibility::TestEnableSC(true);

  auto accessible_a = Dali::Accessibility::Accessible::Get(controla);
  auto accessible_b = Dali::Accessibility::Accessible::Get(controlb);

  auto a = dynamic_cast<Dali::Accessibility::Component*>(accessible_a);
  auto b = dynamic_cast<Dali::Accessibility::Component*>(accessible_b);

  auto states_by_bridge_a = Dali::Accessibility::States{TestGetStates(a->GetAddress())};
  auto states_by_bridge_b = Dali::Accessibility::States{TestGetStates(b->GetAddress())};

  DALI_TEST_CHECK(!states_by_bridge_a[Dali::Accessibility::State::HIGHLIGHTED]);
  DALI_TEST_CHECK(!states_by_bridge_b[Dali::Accessibility::State::HIGHLIGHTED]);

  DALI_TEST_EQUALS(true, DevelControl::GrabAccessibilityHighlight(controla), TEST_LOCATION);

  states_by_bridge_a = Dali::Accessibility::States{TestGetStates(a->GetAddress())};
  states_by_bridge_b = Dali::Accessibility::States{TestGetStates(b->GetAddress())};

  DALI_TEST_CHECK(states_by_bridge_a[Dali::Accessibility::State::HIGHLIGHTED]);
  DALI_TEST_CHECK(!states_by_bridge_b[Dali::Accessibility::State::HIGHLIGHTED]);

  DALI_TEST_EQUALS(true, DevelControl::GrabAccessibilityHighlight(controlb), TEST_LOCATION);
  DALI_TEST_EQUALS(true, DevelControl::GrabAccessibilityHighlight(controlb), TEST_LOCATION);

  states_by_bridge_a = Dali::Accessibility::States{TestGetStates(a->GetAddress())};
  states_by_bridge_b = Dali::Accessibility::States{TestGetStates(b->GetAddress())};

  DALI_TEST_CHECK(!states_by_bridge_a[Dali::Accessibility::State::HIGHLIGHTED]);
  DALI_TEST_CHECK(states_by_bridge_b[Dali::Accessibility::State::HIGHLIGHTED]);

  DALI_TEST_EQUALS(false, DevelControl::ClearAccessibilityHighlight(controla), TEST_LOCATION);

  states_by_bridge_a = Dali::Accessibility::States{TestGetStates(a->GetAddress())};
  states_by_bridge_b = Dali::Accessibility::States{TestGetStates(b->GetAddress())};

  DALI_TEST_CHECK(!states_by_bridge_a[Dali::Accessibility::State::HIGHLIGHTED]);
  DALI_TEST_CHECK(states_by_bridge_b[Dali::Accessibility::State::HIGHLIGHTED]);

  DALI_TEST_EQUALS(true, DevelControl::ClearAccessibilityHighlight(controlb), TEST_LOCATION);

  states_by_bridge_a = Dali::Accessibility::States{TestGetStates(a->GetAddress())};
  states_by_bridge_b = Dali::Accessibility::States{TestGetStates(b->GetAddress())};

  DALI_TEST_CHECK(!states_by_bridge_a[Dali::Accessibility::State::HIGHLIGHTED]);
  DALI_TEST_CHECK(!states_by_bridge_b[Dali::Accessibility::State::HIGHLIGHTED]);

  DALI_TEST_CHECK(TestGrabHighlight(a->GetAddress()));

  states_by_bridge_a = Dali::Accessibility::States{TestGetStates(a->GetAddress())};
  states_by_bridge_b = Dali::Accessibility::States{TestGetStates(b->GetAddress())};

  DALI_TEST_CHECK(states_by_bridge_a[Dali::Accessibility::State::HIGHLIGHTED]);
  DALI_TEST_CHECK(!states_by_bridge_b[Dali::Accessibility::State::HIGHLIGHTED]);

  DALI_TEST_CHECK(TestGrabHighlight(b->GetAddress()));

  states_by_bridge_a = Dali::Accessibility::States{TestGetStates(a->GetAddress())};
  states_by_bridge_b = Dali::Accessibility::States{TestGetStates(b->GetAddress())};

  DALI_TEST_CHECK(!states_by_bridge_a[Dali::Accessibility::State::HIGHLIGHTED]);
  DALI_TEST_CHECK(states_by_bridge_b[Dali::Accessibility::State::HIGHLIGHTED]);

  DALI_TEST_CHECK(TestClearHighlight(b->GetAddress()));

  states_by_bridge_a = Dali::Accessibility::States{TestGetStates(a->GetAddress())};
  states_by_bridge_b = Dali::Accessibility::States{TestGetStates(b->GetAddress())};

  DALI_TEST_CHECK(!states_by_bridge_a[Dali::Accessibility::State::HIGHLIGHTED]);
  DALI_TEST_CHECK(!states_by_bridge_b[Dali::Accessibility::State::HIGHLIGHTED]);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int utcDaliAccessibilityControlAttributes(void)
{
  ToolkitTestApplication application;
  auto                   check_box_button = Toolkit::Control::New();

  std::string value;

  auto attributes     = check_box_button.GetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES);
  auto attributes_map = attributes.GetMap();

  auto position = attributes_map->Find("access_key1");
  DALI_TEST_CHECK(!position);

  DevelControl::AppendAccessibilityAttribute(check_box_button, "access_key1", "access_value1");
  attributes     = check_box_button.GetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES);
  attributes_map = attributes.GetMap();

  DALI_TEST_EQUALS((attributes_map->Find("access_key1"))->Get<std::string>(), "access_value1", TEST_LOCATION);

  DevelControl::AppendAccessibilityAttribute(check_box_button, "access_key2", "access_value2_a");
  attributes     = check_box_button.GetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES);
  attributes_map = attributes.GetMap();

  DALI_TEST_EQUALS((attributes_map->Find("access_key1"))->Get<std::string>(), "access_value1", TEST_LOCATION);
  DALI_TEST_EQUALS((attributes_map->Find("access_key2"))->Get<std::string>(), "access_value2_a", TEST_LOCATION);

  DevelControl::AppendAccessibilityAttribute(check_box_button, "access_key2", "access_value2_b");
  attributes     = check_box_button.GetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES);
  attributes_map = attributes.GetMap();

  DALI_TEST_EQUALS((attributes_map->Find("access_key2"))->Get<std::string>(), "access_value2_b", TEST_LOCATION);

  DevelControl::RemoveAccessibilityAttribute(check_box_button, "access_key2");
  attributes     = check_box_button.GetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES);
  attributes_map = attributes.GetMap();

  // In case when we are removing one of attributes the property is setting for NONE type.
  DALI_TEST_EQUALS((attributes_map->Find("access_key2"))->GetType(), Property::NONE, TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(true);

  auto ptr                  = Dali::Accessibility::Accessible::Get(check_box_button);
  auto attribute_map_bridge = TestGetAttributes(ptr->GetAddress());
  auto counter              = 0u;

  // Refresh the attributes since ControlAccessible::GetAttributes() might have added something
  attributes     = check_box_button.GetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES);
  attributes_map = attributes.GetMap();
  for(auto i = 0u; i < attributes_map->Count(); ++i)
    if((attributes_map->GetValue(i)).GetType() != Property::NONE)
      ++counter;

  DALI_TEST_EQUALS(counter, attribute_map_bridge.size(), TEST_LOCATION);

  for(auto it : attribute_map_bridge)
    DALI_TEST_EQUALS((attributes_map->Find(it.first))->Get<std::string>(), it.second, TEST_LOCATION);

  DevelControl::ClearAccessibilityAttributes(check_box_button);
  attributes     = check_box_button.GetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES);
  attributes_map = attributes.GetMap();

  position = attributes_map->Find("access_key1");
  DALI_TEST_CHECK(!position);

  position = attributes_map->Find("access_key2");
  DALI_TEST_CHECK(!position);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliControlReadingInfoType(void)
{
  ToolkitTestApplication application;
  auto                   control = Control::New();

  auto reading_info_type = DevelControl::GetAccessibilityReadingInfoType(control);

  for(auto i = 0u; i < 4; ++i)
    DALI_TEST_CHECK(reading_info_type[static_cast<Dali::Accessibility::ReadingInfoType>(i)]);

  reading_info_type[Dali::Accessibility::ReadingInfoType::DESCRIPTION] = false;
  reading_info_type[Dali::Accessibility::ReadingInfoType::STATE]       = false;
  reading_info_type[Dali::Accessibility::ReadingInfoType::NAME]        = false;
  reading_info_type[Dali::Accessibility::ReadingInfoType::ROLE]        = false;

  DevelControl::SetAccessibilityReadingInfoType(control, reading_info_type);

  reading_info_type = DevelControl::GetAccessibilityReadingInfoType(control);

  for(auto i = 0u; i < 4; ++i)
    DALI_TEST_CHECK(false == reading_info_type[static_cast<Dali::Accessibility::ReadingInfoType>(i)]);

  reading_info_type[Dali::Accessibility::ReadingInfoType::DESCRIPTION] = true;
  reading_info_type[Dali::Accessibility::ReadingInfoType::STATE]       = true;
  reading_info_type[Dali::Accessibility::ReadingInfoType::NAME]        = true;
  reading_info_type[Dali::Accessibility::ReadingInfoType::ROLE]        = true;

  DevelControl::SetAccessibilityReadingInfoType(control, reading_info_type);

  auto q = control.GetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES);
  auto z = q.GetMap();

  DALI_TEST_EQUALS((z->Find("reading_info_type"))->Get<std::string>(), "name|role|description|state", TEST_LOCATION);
  reading_info_type = DevelControl::GetAccessibilityReadingInfoType(control);
  for(auto i = 0u; i < 4; ++i)
    DALI_TEST_CHECK(reading_info_type[static_cast<Dali::Accessibility::ReadingInfoType>(i)]);

  END_TEST;
}

int UtcDaliControlDoGesture(void)
{
  ToolkitTestApplication application;
  auto                   control = Control::New();
  Dali::Accessibility::TestEnableSC(true);

  auto accessible  = Dali::Accessibility::Accessible::Get(control);
  auto gesture_one = Dali::Accessibility::GestureInfo{Dali::Accessibility::Gesture::ONE_FINGER_FLICK_LEFT, 600, 100, 500, 500, Dali::Accessibility::GestureState::BEGIN, 1000};
  auto gesture_two = Dali::Accessibility::GestureInfo{Dali::Accessibility::Gesture::ONE_FINGER_FLICK_RIGHT, 600, 100, 500, 500, Dali::Accessibility::GestureState::BEGIN, 1000};

  DALI_TEST_CHECK(!accessible->DoGesture(gesture_one));
  DALI_TEST_CHECK(!TestDoGesture(accessible->GetAddress(), Dali::Accessibility::Gesture::ONE_FINGER_FLICK_LEFT, 600, 100, 500, 500, Dali::Accessibility::GestureState::BEGIN, 1000));

  DevelControl::AccessibilityDoGestureSignal(control).Connect([](std::pair<Dali::Accessibility::GestureInfo, bool>& gesture) {
    if(gesture.first.type == Dali::Accessibility::Gesture::ONE_FINGER_FLICK_LEFT)
      gesture.second = true;
    else
      gesture.second = false;
  });

  DALI_TEST_CHECK(accessible->DoGesture(gesture_one));
  DALI_TEST_CHECK(TestDoGesture(accessible->GetAddress(), Dali::Accessibility::Gesture::ONE_FINGER_FLICK_LEFT, 600, 100, 500, 500, Dali::Accessibility::GestureState::BEGIN, 1000));

  DALI_TEST_CHECK(!accessible->DoGesture(gesture_two));
  DALI_TEST_CHECK(!TestDoGesture(accessible->GetAddress(), Dali::Accessibility::Gesture::ONE_FINGER_FLICK_RIGHT, 600, 100, 500, 500, Dali::Accessibility::GestureState::BEGIN, 1000));

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibilityRelation(void)
{
  ToolkitTestApplication application;
  Dali::Accessibility::TestEnableSC(true);

  auto rel          = Accessibility::RelationType::FLOWS_TO;
  auto control      = Control::New();
  auto destination1 = Control::New();
  auto destination2 = Control::New();

  DevelControl::AppendAccessibilityRelation(control, destination1, rel);
  auto relations = DevelControl::GetAccessibilityRelations(control);
  DALI_TEST_EQUALS(relations.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(relations[0].mRelationType, rel, TEST_LOCATION);
  DALI_TEST_EQUALS(relations[0].mTargets.size(), 1u, TEST_LOCATION);

  DevelControl::AppendAccessibilityRelation(control, destination2, rel);
  relations = DevelControl::GetAccessibilityRelations(control);
  DALI_TEST_EQUALS(relations.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(relations[0].mRelationType, rel, TEST_LOCATION);
  DALI_TEST_EQUALS(relations[0].mTargets.size(), 2u, TEST_LOCATION);

  auto accessible              = Dali::Accessibility::Accessible::Get(control);
  auto accessible_destination1 = Dali::Accessibility::Accessible::Get(destination1);
  auto accessible_destination2 = Dali::Accessibility::Accessible::Get(destination2);
  auto relationset             = accessible->GetRelationSet();

  DALI_TEST_CHECK(relationset[0].mRelationType == rel);
  DALI_TEST_CHECK(relationset[0].mTargets[0] == accessible_destination1 || relationset[0].mTargets[1] == accessible_destination1);
  DALI_TEST_CHECK(relationset[0].mTargets[0] == accessible_destination2 || relationset[0].mTargets[1] == accessible_destination2);

  auto relationset_bridge = TestGetRelationSet(accessible->GetAddress());
  DALI_TEST_CHECK(static_cast<uint32_t>(relationset[0].mRelationType) == std::get<0>(relationset_bridge[0]));

  DALI_TEST_CHECK(relationset[0].mTargets[0]->GetAddress() == std::get<1>(relationset_bridge[0])[0] || relationset[0].mTargets[1]->GetAddress() == std::get<1>(relationset_bridge[0])[0]);
  DALI_TEST_CHECK(relationset[0].mTargets[0]->GetAddress() == std::get<1>(relationset_bridge[0])[1] || relationset[0].mTargets[1]->GetAddress() == std::get<1>(relationset_bridge[0])[1]);

  DevelControl::RemoveAccessibilityRelation(control, destination2, rel);
  relations = DevelControl::GetAccessibilityRelations(control);
  DALI_TEST_EQUALS(relations.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(relations[0].mTargets.size(), 1u, TEST_LOCATION);

  DevelControl::ClearAccessibilityRelations(control);
  relations = DevelControl::GetAccessibilityRelations(control);
  DALI_TEST_EQUALS(relations.size(), 0u, TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibilityParentChildren(void)
{
  ToolkitTestApplication application;
  Dali::Accessibility::TestEnableSC(true);

  auto parent  = Control::New();
  auto child_1 = Control::New();
  auto child_2 = Control::New();

  auto parent_accessible  = Dali::Accessibility::Accessible::Get(parent);
  auto child_1_accessible = Dali::Accessibility::Accessible::Get(child_1);
  auto child_2_accessible = Dali::Accessibility::Accessible::Get(child_2);

  auto children = TestGetChildren(parent_accessible->GetAddress());
  DALI_TEST_EQUALS(children.size(), 0, TEST_LOCATION);

  try
  {
    TestGetIndexInParent(child_1_accessible->GetAddress());
    DALI_ABORT("Object has parent, test abort");
  }
  catch(TestDBusWrapper::error&)
  {
  }

  try
  {
    TestGetChildAtIndex(parent_accessible->GetAddress(), -1);
    DALI_ABORT("Positive index, test abort");
  }
  catch(TestDBusWrapper::error&)
  {
  }

  DALI_TEST_EQUALS(parent_accessible->GetChildCount(), 0, TEST_LOCATION);

  try
  {
    child_1_accessible->GetIndexInParent();
    DALI_ABORT("Object has parent, test abort");
  }
  catch(const std::domain_error&)
  {
  }

  parent.Add(child_1);
  parent.Add(child_2);

  children = TestGetChildren(parent_accessible->GetAddress());
  DALI_TEST_EQUALS(children.size(), 2, TEST_LOCATION);

  DALI_TEST_EQUALS(parent_accessible->GetAddress(), TestGetParent(child_1_accessible->GetAddress()), TEST_LOCATION);
  DALI_TEST_EQUALS(child_2_accessible->GetAddress(), TestGetChildAtIndex(parent_accessible->GetAddress(), TestGetIndexInParent(child_2_accessible->GetAddress())), TEST_LOCATION);

  DALI_TEST_EQUALS(parent_accessible, child_1_accessible->GetParent(), TEST_LOCATION);
  DALI_TEST_EQUALS(child_2_accessible, parent_accessible->GetChildAtIndex(child_2_accessible->GetIndexInParent()), TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibilityGetLayer(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  auto control              = Control::New();
  auto accessible_obj       = Dali::Accessibility::Accessible::Get(control);
  auto accessible_component = dynamic_cast<Dali::Accessibility::Component*>(accessible_obj);
  DALI_TEST_CHECK(accessible_component);
  DALI_TEST_EQUALS(Dali::Accessibility::ComponentLayer::WINDOW, accessible_component->GetLayer(), TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Accessibility::ComponentLayer::WINDOW, TestGetLayer(accessible_component->GetAddress()), TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibilityGrabFocus(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  auto controla = Control::New();
  auto controlb = Control::New();

  Stage::GetCurrent().Add(controla);
  Stage::GetCurrent().Add(controlb);

  controla.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  controlb.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  auto a = Dali::Accessibility::Accessible::Get(controla);
  auto b = Dali::Accessibility::Accessible::Get(controlb);

  auto a_component = dynamic_cast<Dali::Accessibility::Component*>(a);
  auto b_component = dynamic_cast<Dali::Accessibility::Component*>(b);

  auto states_by_bridge_a = Dali::Accessibility::States{TestGetStates(a_component->GetAddress())};
  auto states_by_bridge_b = Dali::Accessibility::States{TestGetStates(b_component->GetAddress())};

  DALI_TEST_CHECK(!states_by_bridge_a[Dali::Accessibility::State::FOCUSED]);
  DALI_TEST_CHECK(!states_by_bridge_b[Dali::Accessibility::State::FOCUSED]);

  DALI_TEST_CHECK(a_component->GrabFocus());

  states_by_bridge_a = Dali::Accessibility::States{TestGetStates(a_component->GetAddress())};
  states_by_bridge_b = Dali::Accessibility::States{TestGetStates(b_component->GetAddress())};

  DALI_TEST_CHECK(states_by_bridge_a[Dali::Accessibility::State::FOCUSED]);
  DALI_TEST_CHECK(!states_by_bridge_b[Dali::Accessibility::State::FOCUSED]);

  DALI_TEST_CHECK(TestGrabFocus(b_component->GetAddress()));

  states_by_bridge_a = Dali::Accessibility::States{TestGetStates(a_component->GetAddress())};
  states_by_bridge_b = Dali::Accessibility::States{TestGetStates(b_component->GetAddress())};

  DALI_TEST_CHECK(!states_by_bridge_a[Dali::Accessibility::State::FOCUSED]);
  DALI_TEST_CHECK(states_by_bridge_b[Dali::Accessibility::State::FOCUSED]);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibilityGetExtentsScreenAndWindowPositionMatch(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAccessibilityGetExtentsScreenAndWindowPositionMatch");

  Dali::Accessibility::TestEnableSC(true);

  auto control = Control::New();
  Stage::GetCurrent().GetRootLayer().Add(control);

  auto window = Dali::DevelWindow::Get(control);
  DALI_TEST_CHECK(window);

  //window.SetPosition({0,0});
  DevelWindow::SetPositionSize(window, PositionSize(0, 0, 480, 240));

  control.SetProperty(Actor::Property::POSITION, Vector3(10, 10, 100));
  control.SetProperty(Actor::Property::SIZE, Vector2(10, 10));

  application.SendNotification();
  application.Render(1);

  auto a           = Dali::Accessibility::Accessible::Get(control);
  auto a_component = dynamic_cast<Dali::Accessibility::Component*>(a);

  auto extents = a_component->GetExtents(Dali::Accessibility::CoordinateType::SCREEN);
  DALI_TEST_EQUALS(extents.x, 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.y, 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.height, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.width, 10.0f, TEST_LOCATION);

  auto bridge_extents = TestGetExtents(a_component->GetAddress(), Dali::Accessibility::CoordinateType::SCREEN);
  DALI_TEST_EQUALS(std::get<0>(bridge_extents), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<1>(bridge_extents), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<2>(bridge_extents), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<3>(bridge_extents), 10, TEST_LOCATION);

  extents = a_component->GetExtents(Dali::Accessibility::CoordinateType::WINDOW);
  DALI_TEST_EQUALS(extents.x, 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.y, 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.height, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.width, 10.0f, TEST_LOCATION);

  bridge_extents = TestGetExtents(a_component->GetAddress(), Dali::Accessibility::CoordinateType::WINDOW);
  DALI_TEST_EQUALS(std::get<0>(bridge_extents), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<1>(bridge_extents), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<2>(bridge_extents), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<3>(bridge_extents), 10, TEST_LOCATION);

  control.SetProperty(Dali::DevelActor::Property::POSITION_USES_ANCHOR_POINT, false);
  application.SendNotification();
  application.Render(1);

  extents = a_component->GetExtents(Dali::Accessibility::CoordinateType::SCREEN);
  DALI_TEST_EQUALS(extents.x, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.y, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.height, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.width, 10.0f, TEST_LOCATION);

  bridge_extents = TestGetExtents(a_component->GetAddress(), Dali::Accessibility::CoordinateType::SCREEN);
  DALI_TEST_EQUALS(std::get<0>(bridge_extents), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<1>(bridge_extents), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<2>(bridge_extents), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<3>(bridge_extents), 10, TEST_LOCATION);

  extents = a_component->GetExtents(Dali::Accessibility::CoordinateType::WINDOW);
  DALI_TEST_EQUALS(extents.x, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.y, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.height, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.width, 10.0f, TEST_LOCATION);

  bridge_extents = TestGetExtents(a_component->GetAddress(), Dali::Accessibility::CoordinateType::WINDOW);
  DALI_TEST_EQUALS(std::get<0>(bridge_extents), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<1>(bridge_extents), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<2>(bridge_extents), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<3>(bridge_extents), 10, TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibilityGetExtentsScreenAndWindowPositionDoNotMatch(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  auto control = Control::New();
  Stage::GetCurrent().GetRootLayer().Add(control);
  auto window = Dali::DevelWindow::Get(control);
  //window.SetPosition({33,33});
  DevelWindow::SetPositionSize(window, PositionSize(33, 33, 480, 240));

  control.SetProperty(Actor::Property::POSITION, Vector3(10, 10, 100));
  control.SetProperty(Actor::Property::SIZE, Vector2(10, 10));

  application.SendNotification();
  application.Render(1);

  auto a           = Dali::Accessibility::Accessible::Get(control);
  auto a_component = dynamic_cast<Dali::Accessibility::Component*>(a);

  auto extents = a_component->GetExtents(Dali::Accessibility::CoordinateType::SCREEN);
  DALI_TEST_EQUALS(extents.x, 38.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.y, 38.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.height, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.width, 10.0f, TEST_LOCATION);

  auto bridge_extents = TestGetExtents(a_component->GetAddress(), Dali::Accessibility::CoordinateType::SCREEN);
  DALI_TEST_EQUALS(std::get<0>(bridge_extents), 38, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<1>(bridge_extents), 38, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<2>(bridge_extents), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<3>(bridge_extents), 10, TEST_LOCATION);

  extents = a_component->GetExtents(Dali::Accessibility::CoordinateType::WINDOW);
  DALI_TEST_EQUALS(extents.x, 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.y, 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.height, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.width, 10.0f, TEST_LOCATION);

  bridge_extents = TestGetExtents(a_component->GetAddress(), Dali::Accessibility::CoordinateType::WINDOW);
  DALI_TEST_EQUALS(std::get<0>(bridge_extents), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<1>(bridge_extents), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<2>(bridge_extents), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<3>(bridge_extents), 10, TEST_LOCATION);

  control.SetProperty(Dali::DevelActor::Property::POSITION_USES_ANCHOR_POINT, false);
  application.SendNotification();
  application.Render(1);

  extents = a_component->GetExtents(Dali::Accessibility::CoordinateType::SCREEN);
  DALI_TEST_EQUALS(extents.x, 43.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.y, 43.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.height, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.width, 10.0f, TEST_LOCATION);

  bridge_extents = TestGetExtents(a_component->GetAddress(), Dali::Accessibility::CoordinateType::SCREEN);
  DALI_TEST_EQUALS(std::get<0>(bridge_extents), 43, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<1>(bridge_extents), 43, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<2>(bridge_extents), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<3>(bridge_extents), 10, TEST_LOCATION);

  extents = a_component->GetExtents(Dali::Accessibility::CoordinateType::WINDOW);
  DALI_TEST_EQUALS(extents.x, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.y, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.height, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(extents.width, 10.0f, TEST_LOCATION);

  bridge_extents = TestGetExtents(a_component->GetAddress(), Dali::Accessibility::CoordinateType::WINDOW);
  DALI_TEST_EQUALS(std::get<0>(bridge_extents), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<1>(bridge_extents), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<2>(bridge_extents), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(std::get<3>(bridge_extents), 10, TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibilityGetAlpha(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  auto control     = Control::New();
  auto a           = Dali::Accessibility::Accessible::Get(control);
  auto a_component = dynamic_cast<Dali::Accessibility::Component*>(a);

  DALI_TEST_EQUALS(0.0, a_component->GetAlpha(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.0, TestGetAlpha(a_component->GetAddress()), TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibilityGetMdiZOrder(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  auto control     = Control::New();
  auto a           = Dali::Accessibility::Accessible::Get(control);
  auto a_component = dynamic_cast<Dali::Accessibility::Component*>(a);

  DALI_TEST_EQUALS(0, static_cast<int>(a_component->GetMdiZOrder()), TEST_LOCATION);
  DALI_TEST_EQUALS(0, TestGetMdiZOrder(a_component->GetAddress()), TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibilityAction(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  auto control = Control::New();
  auto a       = Dali::Accessibility::Accessible::Get(control);
  auto b       = dynamic_cast<Dali::Accessibility::Action*>(a);

  std::vector<std::string> actions{"activate", "escape", "increment", "decrement", "ReadingSkipped", "ReadingCancelled", "ReadingStopped", "ReadingPaused", "ReadingResumed", "show", "hide"};
  auto                     count = b->GetActionCount();

  DALI_TEST_EQUALS(count, 11, TEST_LOCATION);

  for(auto i = 0u; i < count; ++i)
  {
    DALI_TEST_CHECK(Dali::Accessibility::Find(actions, b->GetActionName(i)));
    DALI_TEST_EQUALS(b->GetActionName(i), b->GetLocalizedActionName(i), TEST_LOCATION);
    DALI_TEST_EQUALS(b->GetActionDescription(i), "", TEST_LOCATION);
    DALI_TEST_EQUALS(b->GetActionKeyBinding(i), "", TEST_LOCATION);
  }

  // Empty strings should be returned for invalid indices
  DALI_TEST_EQUALS(b->GetActionDescription(count), "", TEST_LOCATION);
  DALI_TEST_EQUALS(b->GetActionName(count), "", TEST_LOCATION);
  DALI_TEST_EQUALS(b->GetLocalizedActionName(count), "", TEST_LOCATION);
  DALI_TEST_EQUALS(b->GetActionKeyBinding(count), "", TEST_LOCATION);

  count = TestGetActionCount(b->GetAddress());

  DALI_TEST_EQUALS(count, 11, TEST_LOCATION);

  for(auto i = 0u; i < count; ++i)
  {
    DALI_TEST_CHECK(Dali::Accessibility::Find(actions, TestGetActionName(b->GetAddress(), i)));
    DALI_TEST_EQUALS(TestGetActionName(b->GetAddress(), i), TestGetLocalizedActionName(b->GetAddress(), i), TEST_LOCATION);
    DALI_TEST_EQUALS(TestGetActionDescription(b->GetAddress(), i), "", TEST_LOCATION);
    DALI_TEST_EQUALS(TestGetActionKeyBinding(b->GetAddress(), i), "", TEST_LOCATION);
  }

  DALI_TEST_EQUALS(TestGetActionDescription(b->GetAddress(), count), "", TEST_LOCATION);
  DALI_TEST_EQUALS(TestGetActionName(b->GetAddress(), count), "", TEST_LOCATION);
  DALI_TEST_EQUALS(TestGetLocalizedActionName(b->GetAddress(), count), "", TEST_LOCATION);
  DALI_TEST_EQUALS(TestGetActionKeyBinding(b->GetAddress(), count), "", TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibilityDoAction(void)
{
  using Dali::Accessibility::ActionType;

  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);
  thread_local std::vector<ActionType> actions_done;
  thread_local std::vector<bool>       legacy_actions_done(5, false);

  const auto reset_results = [&]() {
    actions_done.clear();
    std::fill(legacy_actions_done.begin(), legacy_actions_done.end(), false);
  };

  const auto check_actions_done = [&](std::vector<ActionType> actions_sent) {
    for(ActionType action : actions_sent)
    {
      auto it = std::find(actions_done.begin(), actions_done.end(), action);
      DALI_TEST_CHECK(it != actions_done.end());
    }
  };

  const auto check_all_actions_done_and_reset = [&]() {
    check_actions_done({ActionType::ACTIVATE, ActionType::ESCAPE, ActionType::INCREMENT, ActionType::DECREMENT});
    DALI_TEST_CHECK(std::all_of(legacy_actions_done.begin(), legacy_actions_done.end(), [](bool x) { return x == true; }));
    reset_results();
  };

  auto                     control = Control::New();
  auto                     a       = Dali::Accessibility::Accessible::Get(control);
  auto                     b       = dynamic_cast<Dali::Accessibility::Action*>(a);
  std::vector<std::string> actions{"activate", "escape", "increment", "decrement", "ReadingSkipped", "ReadingCancelled", "ReadingStopped", "ReadingPaused", "ReadingResumed"};

  // Negative test of calling action with not defined name
  DALI_TEST_CHECK(!b->DoAction("undefined"));

  // Returns fail if no signal is connected
  DALI_TEST_CHECK(!b->DoAction(actions[0])); // activate
  DALI_TEST_CHECK(!b->DoAction(actions[1])); // escape
  DALI_TEST_CHECK(!b->DoAction(actions[2])); // increment
  DALI_TEST_CHECK(!b->DoAction(actions[3])); // decrement

  DevelControl::AccessibilityActionSignal(control).Connect([](const Dali::Accessibility::ActionInfo& action_info) {
    actions_done.push_back(action_info.type);
    return true;
  });
  DevelControl::AccessibilityReadingSkippedSignal(control).Connect([]() {
    legacy_actions_done[0] = true;
  });
  DevelControl::AccessibilityReadingCancelledSignal(control).Connect([]() {
    legacy_actions_done[1] = true;
  });
  DevelControl::AccessibilityReadingStoppedSignal(control).Connect([]() {
    legacy_actions_done[2] = true;
  });
  DevelControl::AccessibilityReadingPausedSignal(control).Connect([]() {
    legacy_actions_done[3] = true;
  });
  DevelControl::AccessibilityReadingResumedSignal(control).Connect([]() {
    legacy_actions_done[4] = true;
  });

  // Test calling action by index
  for(size_t i = 0; i < actions.size(); ++i)
  {
    DALI_TEST_CHECK(b->DoAction(i));
  }

  check_all_actions_done_and_reset();

  // Test calling action by name
  for(size_t i = 0; i < actions.size(); ++i)
  {
    DALI_TEST_CHECK(b->DoAction(actions[i]));
  }

  check_all_actions_done_and_reset();

  // Test "DoAction" through d-bus call
  for(size_t i = 0; i < actions.size(); ++i)
  {
    DALI_TEST_CHECK(TestDoAction(b->GetAddress(), i));
  }

  check_all_actions_done_and_reset();

  // Test "DoActionName" through d-bus call
  for(size_t i = 0; i < actions.size(); ++i)
  {
    DALI_TEST_CHECK(TestDoAction(b->GetAddress(), actions[i]));
  }

  check_all_actions_done_and_reset();

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibilityActivateFallbackToLegacy(void)
{
  using Dali::Accessibility::ActionType;

  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);
  thread_local std::vector<ActionType> actions_done;
  thread_local bool                    legacy_activate_done = false;

  auto control = Control::New();
  auto a       = Dali::Accessibility::Accessible::Get(control);
  auto b       = dynamic_cast<Dali::Accessibility::Action*>(a);

  DevelControl::AccessibilityActivateSignal(control).Connect([]() {
    legacy_activate_done = true;
  });

  DALI_TEST_CHECK(b->DoAction("activate"));   // fallback to legacy "activate" when ActionSignal is not connected
  DALI_TEST_CHECK(!b->DoAction("increment")); // "increment" does not exist in legacy actions

  DALI_TEST_CHECK(legacy_activate_done);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibilitySignals(void)
{
  ToolkitTestApplication application;
  ConnectionTracker      connectionTracker;
  Control                control = Control::New();

  DALI_TEST_CHECK(DevelControl::AccessibilityGetNameSignal(control).Empty());
  control.ConnectSignal(&connectionTracker, "getName", &TestVoidCallback);
  DALI_TEST_CHECK(!DevelControl::AccessibilityGetNameSignal(control).Empty());

  DALI_TEST_CHECK(DevelControl::AccessibilityGetDescriptionSignal(control).Empty());
  control.ConnectSignal(&connectionTracker, "getDescription", &TestVoidCallback);
  DALI_TEST_CHECK(!DevelControl::AccessibilityGetDescriptionSignal(control).Empty());

  DALI_TEST_CHECK(DevelControl::AccessibilityDoGestureSignal(control).Empty());
  control.ConnectSignal(&connectionTracker, "doGesture", &TestVoidCallback);
  DALI_TEST_CHECK(!DevelControl::AccessibilityDoGestureSignal(control).Empty());

  END_TEST;
}

int UtcDaliAccessibilityScrollToChildScrollView(void)
{
  ToolkitTestApplication application;
  Dali::Accessibility::TestEnableSC(true);

  ScrollView scrollView = ScrollView::New();
  application.GetScene().Add(scrollView);

  PushButton          actorA    = PushButton::New();
  const Dali::Vector3 positionA = Vector3(100.0f, 400.0f, 0.0f);
  actorA.SetProperty(Dali::Actor::Property::POSITION, positionA);
  scrollView.Add(actorA);

  PushButton          actorB    = PushButton::New();
  const Dali::Vector3 positionB = Vector3(500.0f, 200.0f, 0.0f);
  actorB.SetProperty(Dali::Actor::Property::POSITION, positionB);
  scrollView.Add(actorB);

  TableView tableView = TableView::New(2, 2); // 2 by 2 grid.
  tableView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  scrollView.Add(tableView);

  PushButton actorC = PushButton::New();
  actorC.SetProperty(Actor::Property::SIZE, Vector2(50.0f, 50.0f));
  tableView.AddChild(actorC, TableView::CellPosition(0, 0));

  PushButton actorD = PushButton::New();
  application.GetScene().Add(actorD);

  Wait(application);

  auto* accessibleParent = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(scrollView));
  DALI_TEST_CHECK(accessibleParent);
  auto* accessibleA = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(actorA));
  DALI_TEST_CHECK(accessibleA);
  auto* accessibleB = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(actorB));
  DALI_TEST_CHECK(accessibleB);
  auto* accessibleC = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(actorC));
  DALI_TEST_CHECK(accessibleC);

  accessibleA->GrabHighlight(); // == scrollView.ScrollTo(actorA)
  Wait(application);
  accessibleB->GrabHighlight(); // == scrollView.ScrollTo(actorB)
  Wait(application);

  // scrollView is ancestor of actorC
  // This should work without a crash
  accessibleC->GrabHighlight(); // == scrollView.ScrollTo(actorC)
  Wait(application);

  // negative testcase calling ScrollToChild using non-child actor
  accessibleParent->ScrollToChild(actorD);

  Dali::Accessibility::TestEnableSC(false);
  END_TEST;
}

int UtcDaliAccessibilityScrollToChildCustomScrollable(void)
{
  ToolkitTestApplication application;

  thread_local Dali::Accessibility::ActionInfo action_done;
  const auto                                   check_scroll_to_child_action_done_and_reset = [&](Actor child) {
    DALI_TEST_CHECK(action_done.type == Dali::Accessibility::ActionType::SCROLL_TO_CHILD);
    DALI_TEST_CHECK(action_done.target == child);
    action_done = Dali::Accessibility::ActionInfo{};
  };

  Dali::Accessibility::TestEnableSC(true);

  auto scrollable = Control::New();
  // set control as scrollable
  scrollable.SetProperty(DevelControl::Property::ACCESSIBILITY_SCROLLABLE, true);

  DevelControl::AccessibilityActionSignal(scrollable).Connect([](const Dali::Accessibility::ActionInfo& action_info) {
    action_done = action_info;
    return true;
  });

  application.GetScene().Add(scrollable);

  PushButton          actorA    = PushButton::New();
  const Dali::Vector3 positionA = Vector3(100.0f, 400.0f, 0.0f);
  actorA.SetProperty(Dali::Actor::Property::POSITION, positionA);
  scrollable.Add(actorA);

  PushButton          actorB    = PushButton::New();
  const Dali::Vector3 positionB = Vector3(500.0f, 200.0f, 0.0f);
  actorB.SetProperty(Dali::Actor::Property::POSITION, positionB);
  scrollable.Add(actorB);

  TableView tableView = TableView::New(2, 2); // 2 by 2 grid.
  tableView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  scrollable.Add(tableView);

  PushButton actorC = PushButton::New();
  actorC.SetProperty(Actor::Property::SIZE, Vector2(50.0f, 50.0f));
  tableView.AddChild(actorC, TableView::CellPosition(0, 0));

  PushButton actorD = PushButton::New();
  application.GetScene().Add(actorD);

  Wait(application);

  auto* accessibleParent = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(scrollable));
  DALI_TEST_CHECK(accessibleParent);
  auto* accessibleA = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(actorA));
  DALI_TEST_CHECK(accessibleA);
  auto* accessibleB = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(actorB));
  DALI_TEST_CHECK(accessibleB);
  auto* accessibleC = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(actorC));
  DALI_TEST_CHECK(accessibleC);
  auto* accessibleD = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(actorD));
  DALI_TEST_CHECK(accessibleD);

  accessibleA->GrabHighlight();
  Wait(application);
  check_scroll_to_child_action_done_and_reset(actorA);

  accessibleB->GrabHighlight();
  Wait(application);
  check_scroll_to_child_action_done_and_reset(actorB);

  // scrollable is ancestor of actorC
  // This should work without a crash
  accessibleC->GrabHighlight();
  check_scroll_to_child_action_done_and_reset(actorC);

  // Grabbing highlight on a non-child actor to scrollable does not emit SCROLL_TO_CHILD
  accessibleD->GrabHighlight();
  DALI_TEST_CHECK(action_done.type == Dali::Accessibility::ActionType::MAX_COUNT);
  DALI_TEST_CHECK(action_done.target != actorD);

  Dali::Accessibility::TestEnableSC(false);
  END_TEST;
}

int UtcDaliAccessibilityScrollToChild(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  auto parent = Control::New();

  auto                child    = Control::New();
  const Dali::Vector3 position = Vector3(100.0f, 400.0f, 0.0f);
  child.SetProperty(Dali::Actor::Property::POSITION, position);

  auto* accessibleParent = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(parent));
  DALI_TEST_CHECK(accessibleParent);

  // ScrollToChild fails if no ActionSignal is connected
  DALI_TEST_CHECK(!accessibleParent->ScrollToChild(child));

  DevelControl::AccessibilityActionSignal(parent).Connect([](const Dali::Accessibility::ActionInfo& action_info) {
    return true;
  });

  // ScrollToChild succeeds is an ActionSinal is connected
  DALI_TEST_CHECK(accessibleParent->ScrollToChild(child));

  Dali::Accessibility::TestEnableSC(false);
  END_TEST;
}

namespace
{
class TestItemFactory : public ItemFactory
{
public:
  TestItemFactory()
  {
  }

  unsigned int GetNumberOfItems() override
  {
    return 2;
  }

  Dali::Actor NewItem(unsigned int itemId) override
  {
    return TextLabel::New(std::to_string(itemId));
  }
};
} // namespace

int UtcDaliAccessibilityScrollToChildItemView(void)
{
  ToolkitTestApplication application;
  Dali::Accessibility::TestEnableSC(true);

  TestItemFactory factory;
  ItemView        view = ItemView::New(factory);
  Dali::Vector3   vec(480.0f, 800.0f, 0.0f);
  ItemLayoutPtr   layout = DefaultItemLayout::New(DefaultItemLayout::DEPTH);

  view.AddLayout(*layout);
  view.SetProperty(Actor::Property::SIZE, vec);

  application.GetScene().Add(view);
  layout->SetOrientation(ControlOrientation::Down);
  view.ActivateLayout(0, vec, 0.0f);

  Wait(application);

  auto* accessibleParent = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(view));
  DALI_TEST_CHECK(accessibleParent);
  auto* accessibleA = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(view.GetItem(0)));
  DALI_TEST_CHECK(accessibleA);
  auto* accessibleB = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(view.GetItem(1)));
  DALI_TEST_CHECK(accessibleB);

  accessibleA->GrabHighlight(); // == view.ScrollToItem(view.GetItemId(actorA))
  Wait(application);
  accessibleB->GrabHighlight(); // == view.ScrollToItem(view.GetItemId(actorB))
  Wait(application);

  Dali::Accessibility::TestEnableSC(false);
  END_TEST;
}

int UtcDaliAccessibilityScrollToChildNonScrollable(void)
{
  ToolkitTestApplication application;
  Dali::Accessibility::TestEnableSC(true);

  TextLabel label = TextLabel::New("123");

  auto* accessible = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(label));
  DALI_TEST_CHECK(accessible);

  DALI_TEST_EQUALS(accessible->IsScrollable(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->ScrollToChild({}), false, TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetInternalActor(), label, TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);
  END_TEST;
}

int UtcDaliAccessibilityCheckHighlight(void)
{
  ToolkitTestApplication application;
  Dali::Accessibility::TestEnableSC(true);
  Dali::Accessibility::TestResetMoveOutedCalled();

  // Make precondition two children exist in parent area
  PushButton parentButton = PushButton::New();
  parentButton.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
  parentButton.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  parentButton.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  parentButton.SetProperty(Actor::Property::POSITION, Dali::Vector2(0.0f, 0.0f));
  parentButton.SetProperty(Actor::Property::SIZE, Dali::Vector2(100.0f, 200.0f));
  application.GetScene().Add(parentButton);

  PushButton buttonA = PushButton::New();
  buttonA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  buttonA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  buttonA.SetProperty(Actor::Property::POSITION, Dali::Vector2(0.0f, 0.0f));
  buttonA.SetProperty(Actor::Property::SIZE, Dali::Vector2(100.0f, 100.0f));
  parentButton.Add(buttonA);

  PushButton buttonB = PushButton::New();
  buttonB.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  buttonB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  buttonB.SetProperty(Actor::Property::POSITION, Dali::Vector2(0.0f, 100.0f));
  buttonB.SetProperty(Actor::Property::SIZE, Dali::Vector2(100.0f, 100.0f));
  parentButton.Add(buttonB);
  Wait(application);

  thread_local bool buttonAHighlighted = false;
  thread_local bool buttonBHighlighted = false;

  DevelControl::AccessibilityHighlightedSignal(buttonA).Connect([](bool highlighted) {
    buttonAHighlighted = highlighted;
  });

  DevelControl::AccessibilityHighlightedSignal(buttonB).Connect([](bool highlighted) {
    buttonBHighlighted = highlighted;
  });

  // Set highlight to first child (A) to enable movement tracking
  auto* accessible = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(buttonA));
  DALI_TEST_CHECK(accessible);
  accessible->GrabHighlight();
  Wait(application);

  DALI_TEST_CHECK(buttonAHighlighted);
  DALI_TEST_CHECK(!buttonBHighlighted);

  // Move first child (A) out of parent area through the parent's area top edge - single move outed event expected for A object and OUTGOING_TOP_LEFT direction
  buttonA.SetProperty(Actor::Property::POSITION, Dali::Vector2(0.0f, -200.0f));
  Wait(application);
  // Need one more notificaiton to get correct updated position
  application.SendNotification();
  DALI_TEST_EQUALS(true, Dali::Accessibility::TestGetMoveOutedCalled(), TEST_LOCATION);

  // Reset verdict data
  Dali::Accessibility::TestResetMoveOutedCalled();

  // Move first child (A) outside of parent area (both start and end position are outside of parent area) - no move outed event expected for A object
  buttonA.SetProperty(Dali::Actor::Property::POSITION, Dali::Vector2(0.0f, -300.0f));
  Wait(application);
  // Need one more notificaiton to get correct updated position
  application.SendNotification();
  DALI_TEST_EQUALS(false, Dali::Accessibility::TestGetMoveOutedCalled(), TEST_LOCATION);

  // Reset verdict data
  Dali::Accessibility::TestResetMoveOutedCalled();

  // Set highlight to second child (B) to enable movement tracking
  accessible = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(buttonB));
  DALI_TEST_CHECK(accessible);
  accessible->GrabHighlight();
  Wait(application);

  DALI_TEST_CHECK(!buttonAHighlighted);
  DALI_TEST_CHECK(buttonBHighlighted);

  // Move second child (B) inside of parent area (both start and end position are inside of parent area) - no move outed event expected for B object
  // B: (0,100) --> (0, 50)
  buttonB.SetProperty(Dali::Actor::Property::POSITION, Dali::Vector2(0.0f, 50.0f));
  Wait(application);
  // Need one more notificaiton to get correct updated position
  application.SendNotification();
  DALI_TEST_EQUALS(false, Dali::Accessibility::TestGetMoveOutedCalled(), TEST_LOCATION);

  // Reset verdict data
  Dali::Accessibility::TestResetMoveOutedCalled();

  // Move second child (B) out of parent area through the parent's area right edge - single move outed event expected for B object and OUTGOING_BOTTOM_RIGHT direction
  buttonB.SetProperty(Actor::Property::POSITION, Dali::Vector2(300.0f, 100.0f));
  Wait(application);
  // Need one more notificaiton to get correct updated position
  application.SendNotification();
  DALI_TEST_EQUALS(true, Dali::Accessibility::TestGetMoveOutedCalled(), TEST_LOCATION);

  // Reset verdict data
  Dali::Accessibility::TestResetMoveOutedCalled();

  // Move second child (B) back into parent area (start position is outside but end position is inside of parent area) - no move outed event expected for B object
  // B: (300,100) --> (0, 100)
  buttonB.SetProperty(Dali::Actor::Property::POSITION, Dali::Vector2(0.0f, 100.0f));
  Wait(application);
  // Need one more seding notificaiton to get correct updated position
  application.SendNotification();
  DALI_TEST_EQUALS(false, Dali::Accessibility::TestGetMoveOutedCalled(), TEST_LOCATION);

  // Reset verdict data
  Dali::Accessibility::TestResetMoveOutedCalled();

  // Disable movement tracking on B by giving highlight to A
  accessible = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(buttonA));
  DALI_TEST_CHECK(accessible);
  accessible->GrabHighlight();
  Wait(application);

  DALI_TEST_CHECK(buttonAHighlighted);
  DALI_TEST_CHECK(!buttonBHighlighted);

  // Move B (untracked) out of parent area through the parent's area right edge - no move outed event expected for B object
  // B: (0,100) --> (300, 100)
  buttonB.SetProperty(Dali::Actor::Property::POSITION, Dali::Vector2(300.0f, 100.0f));
  Wait(application);
  // Need one more seding notificaiton to get correct updated position
  application.SendNotification();
  DALI_TEST_EQUALS(false, Dali::Accessibility::TestGetMoveOutedCalled(), TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);
  END_TEST;
}

int UtcDaliWebViewAccessible(void)
{
  ToolkitTestApplication application;

  auto webView           = Dali::Toolkit::WebView::New();
  auto webViewAccessible = Dali::Accessibility::Accessible::Get(webView);

  DALI_TEST_CHECK(webViewAccessible);

  auto children = webViewAccessible->GetChildren();

  DALI_TEST_CHECK(children.empty());

  // Enables accessibility
  Dali::Accessibility::TestEnableSC(true);

  // Assuming the webengine lazy sets accessibility address on LoadUrl
  children = webViewAccessible->GetChildren();
  DALI_TEST_CHECK(children.empty());

  // our test webengine sets accessibility address here
  webView.LoadUrl("http://www.somewhere.valid1.com");

  children = webViewAccessible->GetChildren();
  DALI_TEST_EQUALS(children.size(), 1u, TEST_LOCATION);

  auto* child = children[0];

  DALI_TEST_CHECK(child);
  DALI_TEST_CHECK(child->IsProxy());
  DALI_TEST_EQUALS(child->GetParent(), webViewAccessible, TEST_LOCATION);

  auto address = child->GetAddress();

  DALI_TEST_CHECK(address);
  DALI_TEST_NOT_EQUALS(address.GetBus(), webViewAccessible->GetAddress().GetBus(), 0.0f, TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);

  children = webViewAccessible->GetChildren();

  DALI_TEST_CHECK(children.empty());

  END_TEST;
}

int UtcDaliEmitAccessibilityStateChanged(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  auto root = Control::New();
  root.SetProperty(Actor::Property::SIZE, Vector2(300, 300));
  root.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::CONTAINER);

  auto dialog = Control::New();
  dialog.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
  dialog.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::DIALOG);
  root.Add(dialog);

  auto button = Control::New();
  button.SetProperty(Actor::Property::SIZE, Vector2(20, 20));
  button.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::BUTTON);
  root.Add(button);

  auto rootAccessible   = dynamic_cast<DevelControl::ControlAccessible*>(Accessibility::Accessible::Get(root));
  auto dialogAccessible = dynamic_cast<DevelControl::ControlAccessible*>(Accessibility::Accessible::Get(dialog));
  auto buttonAccessible = dynamic_cast<DevelControl::ControlAccessible*>(Accessibility::Accessible::Get(button));

  application.GetScene().Add(root);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(Accessibility::Bridge::GetCurrentBridge()->GetDefaultLabel(rootAccessible) != dialogAccessible);

  // modal role: Showing State is emitted and Default label is registered
  DevelControl::EmitAccessibilityStateChanged(dialog, Accessibility::State::SHOWING, 1);

  DALI_TEST_CHECK(Dali::Accessibility::TestStateChangedCalled());
  DALI_TEST_CHECK(Dali::Accessibility::TestStateChangedResult("showing", 1));
  DALI_TEST_CHECK(Accessibility::Bridge::GetCurrentBridge()->GetDefaultLabel(rootAccessible) == dialogAccessible);

  Dali::Accessibility::TestResetStateChangedResult();
  flushCoalescableMessage(application);

  // modal role: Showing State is emitted and Default label is unregistered
  DevelControl::EmitAccessibilityStateChanged(dialog, Accessibility::State::SHOWING, 0);

  DALI_TEST_CHECK(Dali::Accessibility::TestStateChangedCalled());
  DALI_TEST_CHECK(Dali::Accessibility::TestStateChangedResult("showing", 0));
  DALI_TEST_CHECK(Accessibility::Bridge::GetCurrentBridge()->GetDefaultLabel(rootAccessible) != dialogAccessible);

  Dali::Accessibility::TestResetStateChangedResult();
  flushCoalescableMessage(application);

  // modal role: Visible State is not emitted
  DevelControl::EmitAccessibilityStateChanged(dialog, Accessibility::State::VISIBLE, 1);

  DALI_TEST_CHECK(!Dali::Accessibility::TestStateChangedCalled());
  DALI_TEST_CHECK(Accessibility::Bridge::GetCurrentBridge()->GetDefaultLabel(rootAccessible) != dialogAccessible);

  Dali::Accessibility::TestResetStateChangedResult();
  flushCoalescableMessage(application);

  // non-modal role: Showing State is not emitted and Default label is not registered
  DevelControl::EmitAccessibilityStateChanged(button, Accessibility::State::SHOWING, 1);

  DALI_TEST_CHECK(!Dali::Accessibility::TestStateChangedCalled());
  DALI_TEST_CHECK(Accessibility::Bridge::GetCurrentBridge()->GetDefaultLabel(rootAccessible) != buttonAccessible);

  Dali::Accessibility::TestResetStateChangedResult();
  flushCoalescableMessage(application);

  // non-modal role: Visible State is not emitted
  DevelControl::EmitAccessibilityStateChanged(button, Accessibility::State::VISIBLE, 1);

  DALI_TEST_CHECK(!Dali::Accessibility::TestStateChangedCalled());
  DALI_TEST_CHECK(Accessibility::Bridge::GetCurrentBridge()->GetDefaultLabel(rootAccessible) != buttonAccessible);

  Dali::Accessibility::TestResetStateChangedResult();
  flushCoalescableMessage(application);

  // non-modal role: Showing State is emitted if highlighted and not showing
  buttonAccessible->GrabHighlight();
  DevelControl::EmitAccessibilityStateChanged(button, Accessibility::State::SHOWING, 0);
  DALI_TEST_CHECK(Dali::Accessibility::TestStateChangedCalled());
  DALI_TEST_CHECK(Dali::Accessibility::TestStateChangedResult("showing", 0));

  Dali::Accessibility::TestResetStateChangedResult();
  flushCoalescableMessage(application);

  // non-modal role: Showing State is not emitted if highlighted and showing
  DevelControl::EmitAccessibilityStateChanged(button, Accessibility::State::SHOWING, 1);
  DALI_TEST_CHECK(!Dali::Accessibility::TestStateChangedCalled());

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibleDefaultLabel(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  auto root           = Control::New();
  auto rootAccessible = Accessibility::Accessible::Get(root);

  auto control1 = Control::New();
  auto control2 = Control::New();

  application.GetScene().Add(root);
  application.SendNotification();
  application.Render();

  auto bridge = Accessibility::Bridge::GetCurrentBridge();

  // control1 is off scene: not registered as default label
  bridge->RegisterDefaultLabel(control1);
  DALI_TEST_CHECK(bridge->GetDefaultLabel(rootAccessible) == rootAccessible);

  // Add control1 and control2 to the scene
  root.Add(control1);
  root.Add(control2);
  application.SendNotification();
  application.Render();

  // control1 is on scene; registered as default label
  bridge->RegisterDefaultLabel(control1);
  DALI_TEST_CHECK(bridge->GetDefaultLabel(rootAccessible) == Accessibility::Accessible::Get(control1));

  // control2 is on scene; registered as default label; GetDefaultLabel returns the last added default label.
  bridge->RegisterDefaultLabel(control2);
  DALI_TEST_CHECK(bridge->GetDefaultLabel(rootAccessible) == Accessibility::Accessible::Get(control2));

  // Remove and Reset control2
  root.Remove(control2);
  control2.Reset();
  application.SendNotification();
  application.Render();

  // GetDefaultLabel returns control1 as control2 becomes unavailable.
  DALI_TEST_CHECK(bridge->GetDefaultLabel(rootAccessible) == Accessibility::Accessible::Get(control1));

  // UnregisterDefaultLabel immediately remove the control1 from the list.
  bridge->UnregisterDefaultLabel(control1);
  DALI_TEST_CHECK(bridge->GetDefaultLabel(rootAccessible) == rootAccessible);

  // Does not crash when unregistering already unregistered actors.
  bridge->UnregisterDefaultLabel(control1);
  bridge->UnregisterDefaultLabel(control2);

  // GetDefaultLabel returns nullptr if nullptr is passed.
  DALI_TEST_CHECK(bridge->GetDefaultLabel(nullptr) == nullptr);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibleRemovalOnActorDestoyed(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  auto layer = Layer::New();

  auto control = Control::New();

  std::weak_ptr<Accessibility::Accessible> layerAccessible   = Accessibility::Accessible::GetOwningPtr(layer);   // AdaptorAccessible
  std::weak_ptr<Accessibility::Accessible> controlAccessible = Accessibility::Accessible::GetOwningPtr(control); // ControlAccessible
  DALI_TEST_CHECK(layerAccessible.lock());
  DALI_TEST_CHECK(controlAccessible.lock());

  // Test Getting already added accessible from the map
  DALI_TEST_CHECK(!layerAccessible.expired());
  DALI_TEST_CHECK(!controlAccessible.expired());
  DALI_TEST_CHECK(Accessibility::Accessible::Get(layer) == layerAccessible.lock().get());
  DALI_TEST_CHECK(Accessibility::Accessible::Get(control) == controlAccessible.lock().get());

  // Test ControlAccessible Removal
  control.Reset();
  DALI_TEST_CHECK(controlAccessible.expired());
  DALI_TEST_CHECK(Accessibility::Accessible::Get(control) == nullptr);

  // Test AdaptorAccessible Removal
  layer.Reset();
  DALI_TEST_CHECK(layerAccessible.expired());
  DALI_TEST_CHECK(Accessibility::Accessible::Get(layer) == nullptr);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibleIncludeHidden(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  auto root          = Layer::New();
  auto control       = Control::New();
  auto hiddenControl = Control::New();
  hiddenControl.SetProperty(DevelControl::Property::ACCESSIBILITY_HIDDEN, true);

  application.GetScene().Add(root);
  root.Add(control);
  root.Add(hiddenControl);

  auto appAccessible = Accessibility::Bridge::GetCurrentBridge()->GetApplication();
  DALI_TEST_CHECK(appAccessible);

  auto rootAccessible = Accessibility::Accessible::Get(root);
  DALI_TEST_CHECK(rootAccessible);

  auto appAddress = appAccessible->GetAddress();

  DALI_TEST_CHECK(!TestGetIncludeHidden(appAddress));
  DALI_TEST_CHECK(rootAccessible->GetChildCount() == 1); // hidden control is excluded by default

  TestSetIncludeHidden(appAddress, true);
  DALI_TEST_CHECK(TestGetIncludeHidden(appAddress));
  DALI_TEST_CHECK(rootAccessible->GetChildCount() == 2); // hidden control is included after setting includeHidden to true

  TestSetIncludeHidden(appAddress, false);
  DALI_TEST_CHECK(!TestGetIncludeHidden(appAddress));
  DALI_TEST_CHECK(rootAccessible->GetChildCount() == 1); // hidden control is excluded after setting includeHidden to false

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

namespace
{
enum class MatchType : int32_t
{
  INVALID,
  ALL,
  ANY,
  NONE,
  EMPTY
};

enum class SortOrder : uint32_t
{
  INVALID,
  CANONICAL,
  FLOW,
  TAB,
  REVERSE_CANONICAL,
  REVERSE_FLOW,
  REVERSE_TAB,
  LAST_DEFINED
};

static bool TestTouchCallback(Actor, const TouchEvent&)
{
  return true;
}

class TestMatcheableView
{
private:
  Actor MakeClickableActor()
  {
    auto actor = Control::New();
    actor.SetProperty(Actor::Property::SENSITIVE, true);
    actor.SetProperty(DevelActor::Property::USER_INTERACTION_ENABLED, true);
    actor.TouchedSignal().Connect(TestTouchCallback);
    return actor;
  }

  Actor MakeNonClickableActor()
  {
    auto actor = Control::New();
    actor.SetProperty(Actor::Property::SENSITIVE, false);
    return actor;
  }

  Actor MakeInvisibleActor()
  {
    auto actor = Control::New();
    actor.SetProperty(Actor::Property::VISIBLE, false);
    actor.SetProperty(Actor::Property::SIZE, Vector2(10.f, 10.f));
    return actor;
  }

  Actor MakeContainer(bool isClickable, std::string label)
  {
    Actor   container = isClickable ? MakeClickableActor() : MakeNonClickableActor();
    Vector4 color(0.5f, 0.6f, 0.5f, 1.0f);
    container.SetProperty(Actor::Property::COLOR, color);
    container.SetProperty(Actor::Property::VISIBLE, true);
    container.SetProperty(Actor::Property::SIZE, Vector2(480.f / N, 480.f / N));

    // button
    auto button = PushButton::New();
    button.SetProperty(Actor::Property::POSITION, Vector2(0.f, 0.f));
    button.SetProperty(Actor::Property::SIZE, Vector2(10.f, 10.f));
    button.SetProperty(Actor::Property::VISIBLE, true);
    button.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    button.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    button.SetProperty(DevelControl::Property::AUTOMATION_ID, label + "_0");
    container.Add(button);

    // text label
    auto text = TextLabel::New(label);
    text.SetProperty(Actor::Property::VISIBLE, true);
    text.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    text.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    text.SetProperty(DevelControl::Property::AUTOMATION_ID, label + "_1");
    container.Add(text);

    // invisible actor
    auto invisibleActor = MakeInvisibleActor();
    invisibleActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::RIGHT);
    invisibleActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    invisibleActor.SetProperty(DevelControl::Property::AUTOMATION_ID, label + "_2");
    container.Add(invisibleActor);

    // add dummy attribute
    DevelControl::AppendAccessibilityAttribute(Dali::Toolkit::Control::DownCast(container), "dummy", "i_am_dummy");

    return container;
  };

public:
  TestMatcheableView(int n)
  : N{n}
  {
    view = TableView::New(N, N);                                      // N by N grid.
    view.SetProperty(Actor::Property::SIZE, Vector2(480.0f, 800.0f)); // full screen
    //view.SetProperty(Actor::Property::POSITION, Vector2(0.f, 0.f));
    view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

    for(int i = 0; i < N; ++i)
    {
      for(int j = 0; j < N; ++j)
      {
        bool        isClickable = (i * N + j) % 2;
        std::string label       = "test_\n" + std::to_string(i) + "_\t" + std::to_string(j);
        view.AddChild(MakeContainer(isClickable, std::move(label)), TableView::CellPosition(i, j));
      }
    }
  }

  TableView view;
  const int N;
};

static Accessibility::Collection::MatchRule GetMatchRule(std::vector<Accessibility::State> states, std::vector<Accessibility::Role> roles)
{
  Accessibility::States  statesRule;
  MatchType              stateMatchType = MatchType::INVALID;
  std::array<int32_t, 2> statesConverted{0, 0};
  if(!states.empty())
  {
    for(auto state : states)
    {
      statesRule[state] = true;
    }
    const auto statesRaw = statesRule.GetRawData();
    statesConverted      = {static_cast<int32_t>(statesRaw[0]), static_cast<int32_t>(statesRaw[1])};
    stateMatchType       = MatchType::ALL;
  }

  Accessibility::EnumBitSet<Accessibility::Role, Accessibility::Role::MAX_COUNT> rolesRule;
  MatchType                                                                      roleMatchType = MatchType::INVALID;
  std::array<int32_t, 4>                                                         rolesConverted{0, 0, 0, 0};
  if(!roles.empty())
  {
    for(auto role : roles)
    {
      rolesRule[role] = true;
    }
    const auto rolesRaw = rolesRule.GetRawData();
    rolesConverted      = {static_cast<int32_t>(rolesRaw[0]), static_cast<int32_t>(rolesRaw[1]), static_cast<int32_t>(rolesRaw[2]), static_cast<int32_t>(rolesRaw[3])};
    roleMatchType       = MatchType::ALL;
  }

  return {
    std::move(statesConverted),
    static_cast<int32_t>(stateMatchType),
    {},
    static_cast<int32_t>(MatchType::INVALID),
    std::move(rolesConverted),
    static_cast<int32_t>(roleMatchType),
    {},
    static_cast<int32_t>(MatchType::INVALID),
    false};
}
} // namespace

int UtcDaliAccessibleGetMatches(void)
{
  const int              N = 48;
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  application.GetScene().Add(TestMatcheableView(N).view);
  application.SendNotification();
  application.Render();

  auto appAccessible = Accessibility::Bridge::GetCurrentBridge()->GetApplication();
  DALI_TEST_CHECK(appAccessible);
  auto collection = dynamic_cast<Accessibility::Collection*>(appAccessible);
  DALI_TEST_CHECK(collection);

  auto         rule          = GetMatchRule({Accessibility::State::SENSITIVE, Accessibility::State::SHOWING}, {});
  auto         results       = collection->GetMatches(std::move(rule), static_cast<uint32_t>(SortOrder::CANONICAL), 0);
  const size_t numContainers = N * N;
  DALI_TEST_CHECK(results.size() == 1 + numContainers / 2 + numContainers); // 1 (root) + num(half of containers) + num(buttons);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibleGetMatchesInMatches(void)
{
  const int              N = 48;
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  application.GetScene().Add(TestMatcheableView(N).view);
  application.SendNotification();
  application.Render();

  auto appAccessible = Accessibility::Bridge::GetCurrentBridge()->GetApplication();
  DALI_TEST_CHECK(appAccessible);
  auto collection = dynamic_cast<Accessibility::Collection*>(appAccessible);
  DALI_TEST_CHECK(collection);

  auto rule1   = GetMatchRule({Accessibility::State::SENSITIVE, Accessibility::State::SHOWING}, {});
  auto rule2   = GetMatchRule({Accessibility::State::SHOWING}, {Accessibility::Role::LABEL});
  auto results = collection->GetMatchesInMatches(std::move(rule1), std::move(rule2), static_cast<uint32_t>(SortOrder::CANONICAL), 0, 0);

  const size_t numLabels = N * N;
  DALI_TEST_CHECK(results.size() == numLabels); // text labels

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibleDumpTree(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  application.GetScene().Add(TestMatcheableView(2).view);
  application.SendNotification();
  application.Render();

  auto appAccessible = Accessibility::Bridge::GetCurrentBridge()->GetApplication();
  DALI_TEST_CHECK(appAccessible);

  {
    const std::string expected = R"({ "appname": "bus", "path": "/org/a11y/atspi/accessible/root", "role": "application", "states": 1107296514, "text": "TestApp", "x": 0, "y": 0, "w": 480, "h": 800, "toolkit": "dali", "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/1", "role": "window", "states": 1124073730, "text": "RootLayer", "type" : "Layer", "x": 0, "y": 0, "w": 480, "h": 800, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/2", "role": "redundant object", "states": 1107296256, "text": "DefaultCamera", "type" : "CameraActor", "x": 240, "y": 400, "w": 0, "h": 0 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/3", "role": "table", "states": 1107298560, "type" : "TableView", "x": 0, "y": 0, "w": 480, "h": 800, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/4", "role": "unknown", "states": 1107296512, "type" : "Control", "x": 0, "y": 0, "w": 240, "h": 240, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/5", "role": "push button", "states": 35185500358912, "type" : "PushButton", "automationId" : "test_\n0_\t0_0", "x": 0, "y": 0, "w": 10, "h": 10 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/6", "role": "label", "states": 35185479385344, "text": "test_\n0_\t0", "type" : "TextLabel", "automationId" : "test_\n0_\t0_1", "x": 120, "y": 120, "w": 240, "h": 64 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/7", "role": "unknown", "states": 256, "type" : "Control", "automationId" : "test_\n0_\t0_2", "x": 240, "y": 240, "w": 10, "h": 10 }] }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/8", "role": "unknown", "states": 1124073728, "type" : "Control", "x": 240, "y": 0, "w": 240, "h": 240, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/9", "role": "push button", "states": 35185500358912, "type" : "PushButton", "automationId" : "test_\n0_\t1_0", "x": 240, "y": 0, "w": 10, "h": 10 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/10", "role": "label", "states": 35185479385344, "text": "test_\n0_\t1", "type" : "TextLabel", "automationId" : "test_\n0_\t1_1", "x": 360, "y": 120, "w": 240, "h": 64 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/11", "role": "unknown", "states": 256, "type" : "Control", "automationId" : "test_\n0_\t1_2", "x": 480, "y": 240, "w": 10, "h": 10 }] }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/12", "role": "unknown", "states": 1107296512, "type" : "Control", "x": 0, "y": 400, "w": 240, "h": 240, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/13", "role": "push button", "states": 35185500358912, "type" : "PushButton", "automationId" : "test_\n1_\t0_0", "x": 0, "y": 400, "w": 10, "h": 10 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/14", "role": "label", "states": 35185479385344, "text": "test_\n1_\t0", "type" : "TextLabel", "automationId" : "test_\n1_\t0_1", "x": 120, "y": 520, "w": 240, "h": 64 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/15", "role": "unknown", "states": 256, "type" : "Control", "automationId" : "test_\n1_\t0_2", "x": 240, "y": 640, "w": 10, "h": 10 }] }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/16", "role": "unknown", "states": 1124073728, "type" : "Control", "x": 240, "y": 400, "w": 240, "h": 240, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/17", "role": "push button", "states": 35185500358912, "type" : "PushButton", "automationId" : "test_\n1_\t1_0", "x": 240, "y": 400, "w": 10, "h": 10 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/18", "role": "label", "states": 35185479385344, "text": "test_\n1_\t1", "type" : "TextLabel", "automationId" : "test_\n1_\t1_1", "x": 360, "y": 520, "w": 240, "h": 64 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/19", "role": "unknown", "states": 256, "type" : "Control", "automationId" : "test_\n1_\t1_2", "x": 480, "y": 640, "w": 10, "h": 10 }] }] }] }] })";

    auto result = appAccessible->DumpTree(Accessibility::Accessible::DumpDetailLevel::DUMP_SHORT);
    DALI_TEST_EQUALS(result, expected, TEST_LOCATION);
  }

  {
    const std::string expected = R"({ "appname": "bus", "path": "/org/a11y/atspi/accessible/root", "role": "application", "states": 1107296514, "text": "TestApp", "x": 0, "y": 0, "w": 480, "h": 800, "toolkit": "dali", "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/1", "role": "window", "states": 1124073730, "text": "RootLayer", "type" : "Layer", "x": 0, "y": 0, "w": 480, "h": 800, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/2", "role": "redundant object", "states": 1107296256, "text": "DefaultCamera", "type" : "CameraActor", "x": 240, "y": 400, "w": 0, "h": 0 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/3", "role": "table", "states": 1107298560, "type" : "TableView", "x": 0, "y": 0, "w": 480, "h": 800, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/4", "role": "unknown", "states": 1107296512, "type" : "Control", "x": 0, "y": 0, "w": 240, "h": 240, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/5", "role": "push button", "states": 35185500358912, "type" : "PushButton", "automationId" : "test_\n0_\t0_0", "x": 0, "y": 0, "w": 10, "h": 10 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/6", "role": "label", "states": 35185479385344, "text": "test_\n0_\t0", "type" : "TextLabel", "automationId" : "test_\n0_\t0_1", "x": 120, "y": 120, "w": 240, "h": 64 }] }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/8", "role": "unknown", "states": 1124073728, "type" : "Control", "x": 240, "y": 0, "w": 240, "h": 240, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/9", "role": "push button", "states": 35185500358912, "type" : "PushButton", "automationId" : "test_\n0_\t1_0", "x": 240, "y": 0, "w": 10, "h": 10 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/10", "role": "label", "states": 35185479385344, "text": "test_\n0_\t1", "type" : "TextLabel", "automationId" : "test_\n0_\t1_1", "x": 360, "y": 120, "w": 240, "h": 64 }] }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/12", "role": "unknown", "states": 1107296512, "type" : "Control", "x": 0, "y": 400, "w": 240, "h": 240, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/13", "role": "push button", "states": 35185500358912, "type" : "PushButton", "automationId" : "test_\n1_\t0_0", "x": 0, "y": 400, "w": 10, "h": 10 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/14", "role": "label", "states": 35185479385344, "text": "test_\n1_\t0", "type" : "TextLabel", "automationId" : "test_\n1_\t0_1", "x": 120, "y": 520, "w": 240, "h": 64 }] }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/16", "role": "unknown", "states": 1124073728, "type" : "Control", "x": 240, "y": 400, "w": 240, "h": 240, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/17", "role": "push button", "states": 35185500358912, "type" : "PushButton", "automationId" : "test_\n1_\t1_0", "x": 240, "y": 400, "w": 10, "h": 10 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/18", "role": "label", "states": 35185479385344, "text": "test_\n1_\t1", "type" : "TextLabel", "automationId" : "test_\n1_\t1_1", "x": 360, "y": 520, "w": 240, "h": 64 }] }] }] }] })";

    auto result = appAccessible->DumpTree(Accessibility::Accessible::DumpDetailLevel::DUMP_SHORT_SHOWING_ONLY);
    DALI_TEST_EQUALS(result, expected, TEST_LOCATION);
  }

  {
    const std::string expected = R"({ "appname": "bus", "path": "/org/a11y/atspi/accessible/root", "role": "application", "states": 1107296514, "text": "TestApp", "x": 0, "y": 0, "w": 480, "h": 800, "toolkit": "dali", "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/1", "role": "window", "states": 1124073730, "text": "RootLayer", "type" : "Layer", "x": 0, "y": 0, "w": 480, "h": 800, "attributes": { "resID": "123" }, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/2", "role": "redundant object", "states": 1107296256, "text": "DefaultCamera", "type" : "CameraActor", "x": 240, "y": 400, "w": 0, "h": 0 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/3", "role": "table", "states": 1107298560, "type" : "TableView", "x": 0, "y": 0, "w": 480, "h": 800, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/4", "role": "unknown", "states": 1107296512, "type" : "Control", "x": 0, "y": 0, "w": 240, "h": 240, "attributes": { "dummy": "i_am_dummy" }, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/5", "role": "push button", "states": 35185500358912, "type" : "PushButton", "automationId" : "test_\n0_\t0_0", "x": 0, "y": 0, "w": 10, "h": 10 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/6", "role": "label", "states": 35185479385344, "text": "test_\n0_\t0", "type" : "TextLabel", "automationId" : "test_\n0_\t0_1", "x": 120, "y": 120, "w": 240, "h": 64 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/7", "role": "unknown", "states": 256, "type" : "Control", "automationId" : "test_\n0_\t0_2", "x": 240, "y": 240, "w": 10, "h": 10 }] }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/8", "role": "unknown", "states": 1124073728, "type" : "Control", "x": 240, "y": 0, "w": 240, "h": 240, "attributes": { "dummy": "i_am_dummy" }, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/9", "role": "push button", "states": 35185500358912, "type" : "PushButton", "automationId" : "test_\n0_\t1_0", "x": 240, "y": 0, "w": 10, "h": 10 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/10", "role": "label", "states": 35185479385344, "text": "test_\n0_\t1", "type" : "TextLabel", "automationId" : "test_\n0_\t1_1", "x": 360, "y": 120, "w": 240, "h": 64 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/11", "role": "unknown", "states": 256, "type" : "Control", "automationId" : "test_\n0_\t1_2", "x": 480, "y": 240, "w": 10, "h": 10 }] }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/12", "role": "unknown", "states": 1107296512, "type" : "Control", "x": 0, "y": 400, "w": 240, "h": 240, "attributes": { "dummy": "i_am_dummy" }, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/13", "role": "push button", "states": 35185500358912, "type" : "PushButton", "automationId" : "test_\n1_\t0_0", "x": 0, "y": 400, "w": 10, "h": 10 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/14", "role": "label", "states": 35185479385344, "text": "test_\n1_\t0", "type" : "TextLabel", "automationId" : "test_\n1_\t0_1", "x": 120, "y": 520, "w": 240, "h": 64 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/15", "role": "unknown", "states": 256, "type" : "Control", "automationId" : "test_\n1_\t0_2", "x": 240, "y": 640, "w": 10, "h": 10 }] }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/16", "role": "unknown", "states": 1124073728, "type" : "Control", "x": 240, "y": 400, "w": 240, "h": 240, "attributes": { "dummy": "i_am_dummy" }, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/17", "role": "push button", "states": 35185500358912, "type" : "PushButton", "automationId" : "test_\n1_\t1_0", "x": 240, "y": 400, "w": 10, "h": 10 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/18", "role": "label", "states": 35185479385344, "text": "test_\n1_\t1", "type" : "TextLabel", "automationId" : "test_\n1_\t1_1", "x": 360, "y": 520, "w": 240, "h": 64 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/19", "role": "unknown", "states": 256, "type" : "Control", "automationId" : "test_\n1_\t1_2", "x": 480, "y": 640, "w": 10, "h": 10 }] }] }] }] })";

    auto result = appAccessible->DumpTree(Accessibility::Accessible::DumpDetailLevel::DUMP_FULL);
    DALI_TEST_EQUALS(result, expected, TEST_LOCATION);
  }

  {
    const std::string expected = R"({ "appname": "bus", "path": "/org/a11y/atspi/accessible/root", "role": "application", "states": 1107296514, "text": "TestApp", "x": 0, "y": 0, "w": 480, "h": 800, "toolkit": "dali", "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/1", "role": "window", "states": 1124073730, "text": "RootLayer", "type" : "Layer", "x": 0, "y": 0, "w": 480, "h": 800, "attributes": { "resID": "123" }, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/2", "role": "redundant object", "states": 1107296256, "text": "DefaultCamera", "type" : "CameraActor", "x": 240, "y": 400, "w": 0, "h": 0 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/3", "role": "table", "states": 1107298560, "type" : "TableView", "x": 0, "y": 0, "w": 480, "h": 800, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/4", "role": "unknown", "states": 1107296512, "type" : "Control", "x": 0, "y": 0, "w": 240, "h": 240, "attributes": { "dummy": "i_am_dummy" }, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/5", "role": "push button", "states": 35185500358912, "type" : "PushButton", "automationId" : "test_\n0_\t0_0", "x": 0, "y": 0, "w": 10, "h": 10 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/6", "role": "label", "states": 35185479385344, "text": "test_\n0_\t0", "type" : "TextLabel", "automationId" : "test_\n0_\t0_1", "x": 120, "y": 120, "w": 240, "h": 64 }] }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/8", "role": "unknown", "states": 1124073728, "type" : "Control", "x": 240, "y": 0, "w": 240, "h": 240, "attributes": { "dummy": "i_am_dummy" }, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/9", "role": "push button", "states": 35185500358912, "type" : "PushButton", "automationId" : "test_\n0_\t1_0", "x": 240, "y": 0, "w": 10, "h": 10 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/10", "role": "label", "states": 35185479385344, "text": "test_\n0_\t1", "type" : "TextLabel", "automationId" : "test_\n0_\t1_1", "x": 360, "y": 120, "w": 240, "h": 64 }] }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/12", "role": "unknown", "states": 1107296512, "type" : "Control", "x": 0, "y": 400, "w": 240, "h": 240, "attributes": { "dummy": "i_am_dummy" }, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/13", "role": "push button", "states": 35185500358912, "type" : "PushButton", "automationId" : "test_\n1_\t0_0", "x": 0, "y": 400, "w": 10, "h": 10 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/14", "role": "label", "states": 35185479385344, "text": "test_\n1_\t0", "type" : "TextLabel", "automationId" : "test_\n1_\t0_1", "x": 120, "y": 520, "w": 240, "h": 64 }] }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/16", "role": "unknown", "states": 1124073728, "type" : "Control", "x": 240, "y": 400, "w": 240, "h": 240, "attributes": { "dummy": "i_am_dummy" }, "children": [ { "appname": "bus", "path": "/org/a11y/atspi/accessible/17", "role": "push button", "states": 35185500358912, "type" : "PushButton", "automationId" : "test_\n1_\t1_0", "x": 240, "y": 400, "w": 10, "h": 10 }, { "appname": "bus", "path": "/org/a11y/atspi/accessible/18", "role": "label", "states": 35185479385344, "text": "test_\n1_\t1", "type" : "TextLabel", "automationId" : "test_\n1_\t1_1", "x": 360, "y": 520, "w": 240, "h": 64 }] }] }] }] })";

    auto result = appAccessible->DumpTree(Accessibility::Accessible::DumpDetailLevel::DUMP_FULL_SHOWING_ONLY);
    DALI_TEST_EQUALS(result, expected, TEST_LOCATION);
  }

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliWebViewCheckResumeOnAccessibilityMode(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC(true);

  WebView view = WebView::New();
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION, Vector2(0, 0));
  view.SetProperty(Actor::Property::SIZE, Vector2(800, 600));
  DALI_TEST_CHECK(view);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  view.Resume();
  DALI_TEST_CHECK(view.HasKeyInputFocus());

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}
