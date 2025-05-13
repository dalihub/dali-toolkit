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

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali.h>

#include <dali/devel-api/adaptor-framework/accessibility-bridge.h>
#include <dali/devel-api/adaptor-framework/accessibility.h>
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
  DBusWrapper::Install({}); // Clean up TestDBusWrapper
}

int utcDaliAccessibilityCheckBoxButtonGetStates(void)
{
  ToolkitTestApplication application;

  auto check_box_button = Toolkit::CheckBoxButton::New();
  auto q                = Dali::Accessibility::Accessible::Get(check_box_button);
  DALI_TEST_CHECK(q);
  auto states = q->GetStates();
  DALI_TEST_EQUALS((int)states[Dali::Accessibility::State::SELECTABLE], (int)true, TEST_LOCATION);

  END_TEST;
}

int utcDaliAccessibilityCheckLabelText(void)
{
  ToolkitTestApplication application;

  auto check_box_button = Toolkit::CheckBoxButton::New();
  //check_box_button.SetLabelText( "button" );
  check_box_button.SetProperty(Toolkit::Button::Property::LABEL, "button");
  auto q = Dali::Accessibility::Accessible::Get(check_box_button);
  DALI_TEST_CHECK(q);
  DALI_TEST_EQUALS(q->GetName(), "button", TEST_LOCATION);

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
  application.GetScene().Add(parentButton);

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
  buttonC.SetProperty(Actor::Property::POSITION, Dali::Vector2(100.0f, 100.0f));
  buttonC.SetProperty(Actor::Property::SIZE, Dali::Vector2(200.0f, 200.0f));
  parentButton.Add(buttonC);

  application.SendNotification();
  application.Render(16);

  auto q = Dali::Accessibility::Accessible::Get(buttonA);
  DALI_TEST_CHECK(q);
  auto states = q->GetStates();
  DALI_TEST_EQUALS((int)states[Dali::Accessibility::State::SHOWING], (int)true, TEST_LOCATION);

  q = Dali::Accessibility::Accessible::Get(buttonB);
  DALI_TEST_CHECK(q);
  states = q->GetStates();
  DALI_TEST_EQUALS((int)states[Dali::Accessibility::State::SHOWING], (int)true, TEST_LOCATION);

  q = Dali::Accessibility::Accessible::Get(buttonC);
  DALI_TEST_CHECK(q);
  states = q->GetStates();
  DALI_TEST_EQUALS((int)states[Dali::Accessibility::State::SHOWING], (int)true, TEST_LOCATION);

  // Make SHOWING object invisible
  buttonC.SetProperty(Actor::Property::VISIBLE, false);

  application.SendNotification();
  application.Render(16);

  states = q->GetStates();
  DALI_TEST_EQUALS((int)states[Dali::Accessibility::State::SHOWING], (int)false, TEST_LOCATION);

  // Make SHOWING parent invisible
  parentButton.SetProperty(Actor::Property::VISIBLE, false);

  application.SendNotification();
  application.Render(16);

  q = Dali::Accessibility::Accessible::Get(buttonA);
  DALI_TEST_CHECK(q);
  states = q->GetStates();
  DALI_TEST_EQUALS((int)states[Dali::Accessibility::State::SHOWING], (int)false, TEST_LOCATION);

  END_TEST;
}

int utcDaliAccessibilityHidden(void)
{
  ToolkitTestApplication application;

  auto  parent            = Toolkit::Control::New();
  auto* parentAccessible  = Accessibility::Accessible::Get(parent);
  auto  control           = Toolkit::Control::New();
  auto* controlAccessible = Accessibility::Accessible::Get(control);

  parent.Add(control);

  // Check not hidden
  DALI_TEST_CHECK(parentAccessible && controlAccessible);
  DALI_TEST_CHECK(!controlAccessible->IsHidden());
  DALI_TEST_CHECK(!control.GetProperty<bool>(Toolkit::DevelControl::Property::ACCESSIBILITY_HIDDEN));
  DALI_TEST_EQUALS(parentAccessible->GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(parentAccessible->GetChildAtIndex(0), controlAccessible, TEST_LOCATION);

  control.SetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_HIDDEN, true);

  // Check hidden
  DALI_TEST_CHECK(controlAccessible->IsHidden());
  DALI_TEST_CHECK(control.GetProperty<bool>(Toolkit::DevelControl::Property::ACCESSIBILITY_HIDDEN));
  DALI_TEST_EQUALS(parentAccessible->GetChildCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int utcDaliAccessibilityAutomationId(void)
{
  ToolkitTestApplication application;
  Dali::Property::Index  automationIdIndex = Toolkit::DevelControl::Property::AUTOMATION_ID;
  std::string            automationIdKey   = "automationId";
  std::string            automationIdValue = "test123";

  auto  control           = Toolkit::Control::New();
  auto* controlAccessible = Accessibility::Accessible::Get(control);

  // Check that there is no automationId initially
  DALI_TEST_EQUALS(control.GetProperty<std::string>(automationIdIndex).empty(), true, TEST_LOCATION);
  auto attributes = controlAccessible->GetAttributes();
  DALI_TEST_EQUALS(attributes.find(automationIdKey) == attributes.end(), true, TEST_LOCATION);

  // Set automationId
  control.SetProperty(automationIdIndex, automationIdValue);

  // Check that automationId is set
  DALI_TEST_EQUALS(control.GetProperty<std::string>(automationIdIndex), automationIdValue, TEST_LOCATION);
  attributes = controlAccessible->GetAttributes();
  DALI_TEST_EQUALS(attributes.find(automationIdKey) != attributes.end(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(attributes[automationIdKey], automationIdValue, TEST_LOCATION);

  // Unset automationId
  control.SetProperty(automationIdIndex, "");

  // Check that there is no automationId
  DALI_TEST_EQUALS(control.GetProperty<std::string>(automationIdIndex).empty(), true, TEST_LOCATION);
  attributes = controlAccessible->GetAttributes();
  DALI_TEST_EQUALS(attributes.find(automationIdKey) == attributes.end(), true, TEST_LOCATION);

  END_TEST;
}

int utcDaliAccessibilityImgSrc(void)
{
  ToolkitTestApplication application;
  const std::string      imageSrcKey = "imgSrc";
  // Check that imgSrc is NOT added for non-image view w/ no additional property
  {
    const auto checkBoxButton    = Toolkit::CheckBoxButton::New();
    const auto controlAccessible = Dali::Accessibility::Accessible::Get(checkBoxButton);
    auto       attributes        = controlAccessible->GetAttributes();
    DALI_TEST_CHECK(attributes.find(imageSrcKey) == attributes.end());
  }

  // Check that imgSrc is NOT added for non-image view w/ additional properties
  {
    const auto textLabel         = Toolkit::TextLabel::New("Hello");
    const auto controlAccessible = Dali::Accessibility::Accessible::Get(textLabel);
    auto       attributes        = controlAccessible->GetAttributes();
    DALI_TEST_CHECK(attributes.find(imageSrcKey) == attributes.end());
  }

  // Check that imgSrc is added for image view w/ Url
  {
    const std::string imagePath         = "gallery-small-1.jpg";
    const auto        imageView         = Toolkit::ImageView::New(imagePath);
    const auto        controlAccessible = Dali::Accessibility::Accessible::Get(imageView);
    auto              attributes        = controlAccessible->GetAttributes();
    DALI_TEST_CHECK(attributes.find(imageSrcKey) != attributes.end());
    DALI_TEST_EQUALS(attributes[imageSrcKey], imagePath, TEST_LOCATION);
  }

  // Check that imgSrc is added for image view w/ imageMap; single url case
  {
    const std::string imagePathForImageMap = "icon-edit.png";
    Property::Map     imageMap;
    imageMap[Toolkit::ImageVisual::Property::URL]            = imagePathForImageMap;
    imageMap[Toolkit::ImageVisual::Property::RELEASE_POLICY] = Toolkit::ImageVisual::ReleasePolicy::DETACHED;

    auto imageView = Toolkit::ImageView::New();
    imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);

    const auto controlAccessible = Dali::Accessibility::Accessible::Get(imageView);
    auto       attributes        = controlAccessible->GetAttributes();
    DALI_TEST_CHECK(attributes.find(imageSrcKey) != attributes.end());
    DALI_TEST_EQUALS(attributes[imageSrcKey], imagePathForImageMap, TEST_LOCATION);
  }

  // Check that imgSrc is added for image view w/ imageMap; url array returns first element
  {
    const std::string image1 = "application-icon-20.png";
    const std::string image2 = "application-icon-21.png";
    Property::Map     imageMap;
    Property::Array   urls{image1, image2};
    imageMap[Toolkit::ImageVisual::Property::URL]            = urls;
    imageMap[Toolkit::ImageVisual::Property::RELEASE_POLICY] = Toolkit::ImageVisual::ReleasePolicy::DETACHED;

    auto imageView = Toolkit::ImageView::New();
    imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);

    const auto controlAccessible = Dali::Accessibility::Accessible::Get(imageView);
    auto       attributes        = controlAccessible->GetAttributes();
    DALI_TEST_CHECK(attributes.find(imageSrcKey) != attributes.end());
    DALI_TEST_EQUALS(attributes[imageSrcKey], image1, TEST_LOCATION);
  }

  END_TEST;
}
