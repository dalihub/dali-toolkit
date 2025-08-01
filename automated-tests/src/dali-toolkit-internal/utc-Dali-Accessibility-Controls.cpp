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
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit/devel-api/controls/control-accessible.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali/devel-api/adaptor-framework/accessibility-bridge.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/devel-api/atspi-interfaces/action.h>
#include <dali/devel-api/atspi-interfaces/editable-text.h>
#include <dali/devel-api/atspi-interfaces/hyperlink.h>
#include <dali/devel-api/atspi-interfaces/hypertext.h>
#include <dali/devel-api/atspi-interfaces/text.h>
#include <dali/public-api/common/vector-wrapper.h>

#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/dbus-wrapper.h>

#include <tuple>

using namespace Dali::Toolkit;

void utc_dali_accessibility_controls_startup(void)
{
  test_return_value = TET_UNDEF;
  DBusWrapper::Install(std::unique_ptr<DBusWrapper>(new TestDBusWrapper));
}

void utc_dali_accessibility_controls_cleanup(void)
{
  test_return_value = TET_PASS;
  DBusWrapper::Install({}); // Clean up TestDBusWrapper
}

int UtcDaliControlAccessibileGet01(void)
{
  ToolkitTestApplication application;

  auto control    = Control::New();
  auto accessible = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(control));
  DALI_TEST_CHECK(accessible);

  END_TEST;
}

int UtcDaliControlAccessibileGet02(void)
{
  ToolkitTestApplication application;

  auto control = Control::New();

  // Default is true.
  DALI_TEST_EQUALS(DevelControl::IsCreateAccessibleEnabled(control), true, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelControl::IsAccessibleCreated(control), false, TEST_LOCATION);

  DevelControl::EnableCreateAccessible(control, false);
  DALI_TEST_EQUALS(DevelControl::IsCreateAccessibleEnabled(control), false, TEST_LOCATION);
  auto accessible = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(control));

  // Check that we don't create accessible yet.
  DALI_TEST_EQUALS(DevelControl::IsAccessibleCreated(control), false, TEST_LOCATION);
  DALI_TEST_CHECK(!accessible);

  DevelControl::EnableCreateAccessible(control, true);
  DALI_TEST_EQUALS(DevelControl::IsCreateAccessibleEnabled(control), true, TEST_LOCATION);
  accessible = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(control));
  DALI_TEST_EQUALS(DevelControl::IsAccessibleCreated(control), true, TEST_LOCATION);
  DALI_TEST_CHECK(accessible);

  DevelControl::EnableCreateAccessible(control, false);
  DALI_TEST_EQUALS(DevelControl::IsCreateAccessibleEnabled(control), false, TEST_LOCATION);
  accessible = dynamic_cast<DevelControl::ControlAccessible*>(Dali::Accessibility::Accessible::Get(control));

  // Check that we got created accessible well.
  DALI_TEST_EQUALS(DevelControl::IsAccessibleCreated(control), true, TEST_LOCATION);
  DALI_TEST_CHECK(accessible);

  END_TEST;
}

int UtcDaliControlAccessibileBlockAccessibleCreation(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::ReadingInfoTypes infoTypes{};
  Dali::Accessibility::RelationType     relationType{Accessibility::RelationType::NULL_OF};

  auto control  = Control::New();
  auto control2 = Control::New();

  // Default is true.
  DALI_TEST_EQUALS(DevelControl::IsCreateAccessibleEnabled(control), true, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelControl::IsAccessibleCreated(control), false, TEST_LOCATION);

  DevelControl::EnableCreateAccessible(control, false);
  DALI_TEST_EQUALS(DevelControl::IsCreateAccessibleEnabled(control), false, TEST_LOCATION);

  try
  {
    // Should not throw exception even if accessible is not created.
    DALI_TEST_EQUALS(DevelControl::ClearAccessibilityHighlight(control), false, TEST_LOCATION);
    DALI_TEST_EQUALS(DevelControl::GrabAccessibilityHighlight(control), false, TEST_LOCATION);
    DevelControl::GetAccessibilityStates(control);
    DevelControl::NotifyAccessibilityStateChange(control, Dali::Accessibility::States{}, false);
    DevelControl::NotifyAccessibilityStateChange(control, Dali::Accessibility::States{}, true);
    DevelControl::EmitAccessibilityStateChanged(control, Accessibility::State::SHOWING, 0);
    DevelControl::EmitAccessibilityStateChanged(control, Accessibility::State::SHOWING, 1);
    DevelControl::AppendAccessibilityRelation(control, control2, relationType);
    DevelControl::RemoveAccessibilityRelation(control, control2, relationType);
    DevelControl::GetAccessibilityRelations(control);
    DevelControl::ClearAccessibilityRelations(control);
    DevelControl::AppendAccessibilityAttribute(control, "dummy", "i_am_dummy");
    DevelControl::RemoveAccessibilityAttribute(control, "dummy");
    DevelControl::ClearAccessibilityAttributes(control);
    DevelControl::SetAccessibilityReadingInfoType(control, infoTypes);
    DevelControl::GetAccessibilityReadingInfoType(control);

    // Signal relative API.
    DevelControl::AccessibilityActivateSignal(control);
    DevelControl::AccessibilityReadingSkippedSignal(control);
    DevelControl::AccessibilityReadingPausedSignal(control);
    DevelControl::AccessibilityReadingResumedSignal(control);
    DevelControl::AccessibilityReadingCancelledSignal(control);
    DevelControl::AccessibilityReadingStoppedSignal(control);
    DevelControl::AccessibilityGetNameSignal(control);
    DevelControl::AccessibilityGetDescriptionSignal(control);
    DevelControl::AccessibilityDoGestureSignal(control);
    DevelControl::AccessibilityActionSignal(control);
    DevelControl::AccessibilityHighlightedSignal(control);

    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false);
  }

  // Check that we don't create accessible yet.
  DALI_TEST_EQUALS(DevelControl::IsAccessibleCreated(control), false, TEST_LOCATION);

  // Dummy function call - for line coverage
  DevelControl::EnableCreateAccessible(control, true);
  DALI_TEST_EQUALS(DevelControl::IsCreateAccessibleEnabled(control), true, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelControl::IsAccessibleCreated(control), false, TEST_LOCATION);

  try
  {
    // Should not throw exception even if accessible is not created.
    DevelControl::ClearAccessibilityHighlight(control);
    DevelControl::GrabAccessibilityHighlight(control);
    DevelControl::GetAccessibilityStates(control);
    DevelControl::NotifyAccessibilityStateChange(control, Dali::Accessibility::States{}, false);
    DevelControl::NotifyAccessibilityStateChange(control, Dali::Accessibility::States{}, true);
    DevelControl::EmitAccessibilityStateChanged(control, Accessibility::State::SHOWING, 0);
    DevelControl::EmitAccessibilityStateChanged(control, Accessibility::State::SHOWING, 1);
    DevelControl::AppendAccessibilityRelation(control, control2, relationType);
    DevelControl::RemoveAccessibilityRelation(control, control2, relationType);
    DevelControl::GetAccessibilityRelations(control);
    DevelControl::ClearAccessibilityRelations(control);
    DevelControl::AppendAccessibilityAttribute(control, "dummy", "i_am_dummy");
    DevelControl::RemoveAccessibilityAttribute(control, "dummy");
    DevelControl::ClearAccessibilityAttributes(control);
    DevelControl::SetAccessibilityReadingInfoType(control, infoTypes);
    DevelControl::GetAccessibilityReadingInfoType(control);

    // Signal relative API.
    DevelControl::AccessibilityActivateSignal(control);
    DevelControl::AccessibilityReadingSkippedSignal(control);
    DevelControl::AccessibilityReadingPausedSignal(control);
    DevelControl::AccessibilityReadingResumedSignal(control);
    DevelControl::AccessibilityReadingCancelledSignal(control);
    DevelControl::AccessibilityReadingStoppedSignal(control);
    DevelControl::AccessibilityGetNameSignal(control);
    DevelControl::AccessibilityGetDescriptionSignal(control);
    DevelControl::AccessibilityDoGestureSignal(control);
    DevelControl::AccessibilityActionSignal(control);
    DevelControl::AccessibilityHighlightedSignal(control);
    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false);
  }

  END_TEST;
}

// This test shows that when the accessibility bridge is
// not up, there is no possibility to grab or clear highlight
int UtcDaliControlAccessibilityHighlight(void)
{
  ToolkitTestApplication application;
  auto                   controla = Control::New();
  auto                   controlb = Control::New();

  DALI_TEST_EQUALS(false, DevelControl::GrabAccessibilityHighlight(controla), TEST_LOCATION);
  DALI_TEST_EQUALS(false, DevelControl::GrabAccessibilityHighlight(controlb), TEST_LOCATION);
  DALI_TEST_EQUALS(false, DevelControl::ClearAccessibilityHighlight(controla), TEST_LOCATION);
  DALI_TEST_EQUALS(false, DevelControl::ClearAccessibilityHighlight(controlb), TEST_LOCATION);

  END_TEST;
}

#include <dali-toolkit/devel-api/controls/tool-bar/tool-bar.h>
int UtcDaliAccessibilityToolBarConstructor(void)
{
  ToolkitTestApplication application;

  auto toolbar = ToolBar::New();
  DALI_TEST_CHECK(toolbar);

  auto accessible = Dali::Accessibility::Accessible::Get(toolbar);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::TOOL_BAR, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityPushButtonConstructor(void)
{
  ToolkitTestApplication application;

  auto pushbutton = PushButton::New();
  DALI_TEST_CHECK(pushbutton);

  auto accessible = Dali::Accessibility::Accessible::Get(pushbutton);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::PUSH_BUTTON, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityPushButtonStates(void)
{
  ToolkitTestApplication application;

  auto pushbutton = PushButton::New();
  DALI_TEST_CHECK(pushbutton);

  auto accessible = Dali::Accessibility::Accessible::Get(pushbutton);
  DALI_TEST_CHECK(accessible);

  Dali::Accessibility::TestEnableSC(true);

  /* add to scene and remove from scene to touch AccessibilityDeregister */
  application.GetScene().Add(pushbutton);

  auto states = accessible->GetStates();
  DALI_TEST_EQUALS(static_cast<unsigned int>(states[Accessibility::State::PRESSED]), false, TEST_LOCATION);

  // auto button = dynamic_cast<Dali::Toolkit::Button* >( accessible ) ;
  pushbutton.SetProperty(Toolkit::Button::Property::TOGGLABLE, true);
  pushbutton.SetProperty(Toolkit::Button::Property::SELECTED, true);

  states = accessible->GetStates();
  DALI_TEST_EQUALS(static_cast<unsigned int>(states[Accessibility::State::PRESSED]), true, TEST_LOCATION);

  // Grab highlite now.
  auto i = dynamic_cast<Dali::Accessibility::Component*>(accessible);
  DALI_TEST_CHECK(i);
  i->GrabHighlight();

  pushbutton.SetProperty(Toolkit::Button::Property::SELECTED, false);

  states = accessible->GetStates();
  DALI_TEST_EQUALS(static_cast<unsigned int>(states[Accessibility::State::PRESSED]), false, TEST_LOCATION);

  /* add to scene and remove from scene to touch AccessibilityDeregister */
  application.GetScene().Remove(pushbutton);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

#include <dali-toolkit/devel-api/controls/buttons/toggle-button.h>
int UtcDaliAccessibilityToggleButtonConstructor(void)
{
  ToolkitTestApplication application;

  auto togglebutton = ToggleButton::New();
  DALI_TEST_CHECK(togglebutton);

  auto accessible = Dali::Accessibility::Accessible::Get(togglebutton);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::TOGGLE_BUTTON, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityToggleButtonStates(void)
{
  ToolkitTestApplication application;

  auto togglebutton = ToggleButton::New();
  DALI_TEST_CHECK(togglebutton);

  auto accessible = Dali::Accessibility::Accessible::Get(togglebutton);
  DALI_TEST_CHECK(accessible);

  Dali::Accessibility::TestEnableSC(true);

  /* add to scene and remove from scene to touch AccessibilityDeregister */
  application.GetScene().Add(togglebutton);

  auto states = accessible->GetStates();
  DALI_TEST_EQUALS(static_cast<unsigned int>(states[Accessibility::State::CHECKED]), false, TEST_LOCATION);

  // auto button = dynamic_cast<Dali::Toolkit::Button* >( accessible ) ;
  togglebutton.SetProperty(Toolkit::Button::Property::TOGGLABLE, true);
  togglebutton.SetProperty(Toolkit::Button::Property::SELECTED, true);

  states = accessible->GetStates();
  DALI_TEST_EQUALS(static_cast<unsigned int>(states[Accessibility::State::CHECKED]), true, TEST_LOCATION);

  // Grab highlite now.
  auto i = dynamic_cast<Dali::Accessibility::Component*>(accessible);
  DALI_TEST_CHECK(i);
  i->GrabHighlight();

  togglebutton.SetProperty(Toolkit::Button::Property::SELECTED, false);

  states = accessible->GetStates();
  DALI_TEST_EQUALS(static_cast<unsigned int>(states[Accessibility::State::CHECKED]), false, TEST_LOCATION);

  /* add to scene and remove from scene to touch AccessibilityDeregister */
  application.GetScene().Remove(togglebutton);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;

  END_TEST;
}

#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup.h>
int UtcDaliAccessibilityTextSelectionPopupConstructor(void)
{
  ToolkitTestApplication application;

  auto textselectionpopup = TextSelectionPopup::New(NULL);
  DALI_TEST_CHECK(textselectionpopup);

  auto accessible = Dali::Accessibility::Accessible::Get(textselectionpopup);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::DIALOG, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityAlignmentConstructor(void)
{
  ToolkitTestApplication application;

  auto alignment = Alignment::New();
  DALI_TEST_CHECK(alignment);

  auto accessible = Dali::Accessibility::Accessible::Get(alignment);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::FILLER, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityRadioButtonStates(void)
{
  ToolkitTestApplication application;

  auto radiobutton = RadioButton::New();
  DALI_TEST_CHECK(radiobutton);

  auto accessible = Dali::Accessibility::Accessible::Get(radiobutton);
  DALI_TEST_CHECK(accessible);

  Dali::Accessibility::TestEnableSC(true);

  application.GetScene().Add(radiobutton);

  auto states = accessible->GetStates();
  DALI_TEST_CHECK(states);
  DALI_TEST_EQUALS(static_cast<unsigned int>(states[Accessibility::State::CHECKED]), false, TEST_LOCATION);
  radiobutton.SetProperty(Toolkit::RadioButton::Property::SELECTED, true);
  states = accessible->GetStates();
  DALI_TEST_CHECK(states);
  DALI_TEST_EQUALS(static_cast<unsigned int>(states[Accessibility::State::CHECKED]), true, TEST_LOCATION);

  // Grab highlite now.
  auto i = dynamic_cast<Dali::Accessibility::Component*>(accessible);
  DALI_TEST_CHECK(i);
  i->GrabHighlight();

  radiobutton.SetProperty(Toolkit::RadioButton::Property::SELECTED, false);
  states = accessible->GetStates();
  DALI_TEST_CHECK(states);
  DALI_TEST_EQUALS(static_cast<unsigned int>(states[Accessibility::State::CHECKED]), false, TEST_LOCATION);

  application.GetScene().Remove(radiobutton);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibilityFlexContainerConstructor(void)
{
  ToolkitTestApplication application;

  auto flexcontainer = FlexContainer::New();
  DALI_TEST_CHECK(flexcontainer);

  auto accessible = Dali::Accessibility::Accessible::Get(flexcontainer);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::FILLER, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityCheckBoxButton(void)
{
  ToolkitTestApplication application;

  auto checkboxbutton = CheckBoxButton::New();
  DALI_TEST_CHECK(checkboxbutton);

  auto accessible = Dali::Accessibility::Accessible::Get(checkboxbutton);
  DALI_TEST_CHECK(accessible);

  Dali::Accessibility::TestEnableSC(true);

  application.GetScene().Add(checkboxbutton);

  auto states = accessible->GetStates();
  DALI_TEST_EQUALS(static_cast<unsigned int>(states[Accessibility::State::CHECKED]), false, TEST_LOCATION);
  checkboxbutton.SetProperty(Toolkit::CheckBoxButton::Property::SELECTED, true);
  states = accessible->GetStates();
  DALI_TEST_EQUALS(static_cast<unsigned int>(states[Accessibility::State::CHECKED]), true, TEST_LOCATION);

  // Grab highlite now.
  auto i = dynamic_cast<Dali::Accessibility::Component*>(accessible);
  DALI_TEST_CHECK(i);
  i->GrabHighlight();

  checkboxbutton.SetProperty(Toolkit::CheckBoxButton::Property::SELECTED, false);
  states = accessible->GetStates();
  DALI_TEST_EQUALS(static_cast<unsigned int>(states[Accessibility::State::CHECKED]), false, TEST_LOCATION);

  application.GetScene().Remove(checkboxbutton);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

#include <dali-toolkit/devel-api/controls/text-controls/text-selection-toolbar.h>
int UtcDaliAccessibilityTextSelectionConstructor(void)
{
  ToolkitTestApplication application;

  auto textselectiontoolbar = TextSelectionToolbar::New();
  DALI_TEST_CHECK(textselectiontoolbar);

  auto accessible = Dali::Accessibility::Accessible::Get(textselectiontoolbar);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::TOOL_BAR, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityModel3dViewConstructor(void)
{
  ToolkitTestApplication application;

  auto model3dview = Model3dView::New();
  DALI_TEST_CHECK(model3dview);

  auto accessible = Dali::Accessibility::Accessible::Get(model3dview);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::IMAGE, TEST_LOCATION);

  END_TEST;
}

#include <dali-toolkit/internal/controls/effects-view/effects-view-impl.h>
int UtcDaliAccessibilityEffectsViewConstructor(void)
{
  ToolkitTestApplication application;

  auto etype       = Dali::Toolkit::EffectsView::EffectType::DROP_SHADOW;
  auto effectsview = EffectsView::New(etype);
  DALI_TEST_CHECK(effectsview);

  auto accessible = Dali::Accessibility::Accessible::Get(effectsview);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::FILLER, TEST_LOCATION);

  END_TEST;
}

#include <dali-toolkit/internal/controls/super-blur-view/super-blur-view-impl.h>
int UtcDaliAccessibilitySuperBlurViewConstructor(void)
{
  ToolkitTestApplication application;

  auto superblurview = SuperBlurView::New(1);
  DALI_TEST_CHECK(superblurview);

  auto accessible = Dali::Accessibility::Accessible::Get(superblurview);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::FILLER, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityImageViewConstructor(void)
{
  ToolkitTestApplication application;

  auto imageview = ImageView::New();
  DALI_TEST_CHECK(imageview);

  auto accessible = Dali::Accessibility::Accessible::Get(imageview);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::IMAGE, TEST_LOCATION);

  END_TEST;
}

#include <dali-toolkit/devel-api/controls/page-turn-view/page-factory.h>
class TestPageFactory : public PageFactory
{
public:
  TestPageFactory(bool returnValidTexture = true)
  : mValidTexture(returnValidTexture)
  {
    mTotalPageNumber = 100;
  }

  /**
   * Query the number of pages available from the factory.
   * The maximum available page has an ID of GetNumberOfPages()-1.
   */
  virtual unsigned int GetNumberOfPages()
  {
    return mTotalPageNumber;
  }

  /**
   * Create an texture to represent a page content.
   * @param[in] pageId The ID of the page to create.
   * @return An image, or an empty handle if the ID is out of range.
   */
  virtual Texture NewPage(unsigned int pageId)
  {
    if(mValidTexture)
    {
      return Texture::New(Dali::TextureType::TEXTURE_2D, Pixel::RGB888, 100, 100);
    }
    return Texture(); // empty handle
  }

private:
  unsigned int mTotalPageNumber;
  bool         mValidTexture;
};

#include <dali-toolkit/internal/controls/page-turn-view/page-turn-landscape-view-impl.h>
int UtcDaliAccessibilityPageTurnViewConstructor(void)
{
  ToolkitTestApplication application;

  auto testpagefactory       = TestPageFactory();
  auto vector2               = Vector2(1.0, 1.0);
  auto pageturnlandscapeview = PageTurnLandscapeView::New(testpagefactory, vector2);
  DALI_TEST_CHECK(pageturnlandscapeview);

  auto accessible = Dali::Accessibility::Accessible::Get(pageturnlandscapeview);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::PAGE_TAB_LIST, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityGaussianBlurViewConstructor(void)
{
  ToolkitTestApplication application;

  auto gaussianblurview = GaussianBlurView::New();
  DALI_TEST_CHECK(gaussianblurview);

  auto accessible = Dali::Accessibility::Accessible::Get(gaussianblurview);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::FILLER, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityShadowViewConstructor(void)
{
  ToolkitTestApplication application;

  auto shadowview = ShadowView::New();
  DALI_TEST_CHECK(shadowview);

  auto accessible = Dali::Accessibility::Accessible::Get(shadowview);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::FILLER, TEST_LOCATION);

  END_TEST;
}

#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-impl.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
int UtcDaliAccessibilityScrollableConstructor(void)
{
  ToolkitTestApplication application;

  auto scrollview = ScrollView::New();
  DALI_TEST_CHECK(scrollview);

  auto accessible = Dali::Accessibility::Accessible::Get(scrollview);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::SCROLL_PANE, TEST_LOCATION);

  END_TEST;
}

#include <dali-toolkit/internal/controls/magnifier/magnifier-impl.h>
int UtcDaliAccessibilityMagnifierConstructor(void)
{
  ToolkitTestApplication application;

  auto magnifier = Magnifier::New();
  DALI_TEST_CHECK(magnifier);

  auto accessible = Dali::Accessibility::Accessible::Get(magnifier);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::FILLER, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityTableViewConstructor(void)
{
  ToolkitTestApplication application;

  auto tableview = TableView::New(10, 10);
  DALI_TEST_CHECK(tableview);

  auto accessible = Dali::Accessibility::Accessible::Get(tableview);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::TABLE, TEST_LOCATION);

  END_TEST;
}

#include <dali-toolkit/internal/controls/bloom-view/bloom-view-impl.h>
int UtcDaliAccessibilityBloomViewConstructor(void)
{
  ToolkitTestApplication application;

  auto bloomview = BloomView::New();
  DALI_TEST_CHECK(bloomview);

  auto accessible = Dali::Accessibility::Accessible::Get(bloomview);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::ANIMATION, TEST_LOCATION);

  END_TEST;
}

#include <dali-toolkit/internal/controls/text-controls/text-anchor-impl.h>
#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>
int UtcDaliAccessibilityTextAnchor(void)
{
  ToolkitTestApplication application;

  auto textanchor = TextAnchor::New();
  DALI_TEST_CHECK(textanchor);

  auto textlabel = TextLabel::New();
  DALI_TEST_CHECK(textlabel);

  Dali::Accessibility::TestEnableSC(true);

  textlabel.Add(textanchor);
  auto accessible = Dali::Accessibility::Accessible::Get(textanchor);
  DALI_TEST_CHECK(accessible);
  auto hyperlink = dynamic_cast<Dali::Accessibility::Hyperlink*>(accessible);
  DALI_TEST_CHECK(hyperlink);
  textanchor.SetProperty(Toolkit::TextAnchor::Property::URI, "https://www.tizen.org");
  DALI_TEST_EQUALS(hyperlink->IsValid(), true, TEST_LOCATION);
  auto action = dynamic_cast<Dali::Accessibility::Action*>(accessible);
  // activation of valid hyperlink
  DALI_TEST_CHECK(action->DoAction("activate"));
  // making hyperlink invalid
  textanchor.SetProperty(Toolkit::TextAnchor::Property::URI, "");
  DALI_TEST_EQUALS(hyperlink->IsValid(), false, TEST_LOCATION);
  DALI_TEST_CHECK(!action->DoAction("activate"));

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibilityTextField(void)
{
  ToolkitTestApplication application;

  auto textfield = TextField::New();
  DALI_TEST_CHECK(textfield);

  textfield.SetProperty(Actor::Property::NAME, "test");
  DALI_TEST_EQUALS(textfield.GetProperty<std::string>(Actor::Property::NAME), "test", TEST_LOCATION);

  auto accessible = Dali::Accessibility::Accessible::Get(textfield);
  DALI_TEST_CHECK(accessible);

  DALI_TEST_EQUALS(accessible->GetName(), "", TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::ENTRY, TEST_LOCATION);
  auto states = accessible->GetStates();
  DALI_TEST_EQUALS(static_cast<unsigned int>(states[Accessibility::State::EDITABLE]), true, TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(true);

  textfield.SetProperty(Toolkit::TextField::Property::TEXT, "test");
  auto text = dynamic_cast<Dali::Accessibility::Text*>(accessible);
  DALI_TEST_CHECK(text);
  DALI_TEST_EQUALS(text->GetText(0, 10), "", TEST_LOCATION);
  DALI_TEST_EQUALS(text->SetCursorOffset(100), false, TEST_LOCATION);
  DALI_TEST_EQUALS(text->SetCursorOffset(2), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetCursorOffset(), 2, TEST_LOCATION);

  auto editabletext = dynamic_cast<Dali::Accessibility::EditableText*>(accessible);
  DALI_TEST_CHECK(editabletext);
  DALI_TEST_EQUALS(editabletext->CopyText(3, 1), false, TEST_LOCATION);
  DALI_TEST_EQUALS(editabletext->CopyText(1, 3), true, TEST_LOCATION);
  DALI_TEST_EQUALS(editabletext->CutText(3, 1), false, TEST_LOCATION);
  DALI_TEST_EQUALS(editabletext->CutText(1, 3), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0, 1), "t", TEST_LOCATION);

  auto range = text->GetRangeOfSelection(1);
  DALI_TEST_EQUALS(range.startOffset, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(range.endOffset, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(range.content, "", TEST_LOCATION);
  DALI_TEST_EQUALS(text->SetRangeOfSelection(1, 0, 1), false, TEST_LOCATION);
  DALI_TEST_EQUALS(text->RemoveSelection(1), false, TEST_LOCATION);

  // Insert into empty field
  DALI_TEST_EQUALS(editabletext->SetTextContents(""), true, TEST_LOCATION);
  DALI_TEST_EQUALS(editabletext->InsertText(1, "xyz"), false, TEST_LOCATION);
  DALI_TEST_EQUALS(editabletext->InsertText(0, "abc"), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0, 3), "abc", TEST_LOCATION);

  // Append at end
  DALI_TEST_EQUALS(editabletext->InsertText(3, "xyz"), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0, 6), "abcxyz", TEST_LOCATION);

  DALI_TEST_EQUALS(editabletext->SetTextContents("adef"), true, TEST_LOCATION);
  DALI_TEST_EQUALS(editabletext->InsertText(1, "bc"), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0, 6), "abcdef", TEST_LOCATION);
  DALI_TEST_EQUALS(editabletext->DeleteText(5, 1), false, TEST_LOCATION);
  DALI_TEST_EQUALS(editabletext->DeleteText(1, 5), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0, 2), "af", TEST_LOCATION);

  auto hypertext = dynamic_cast<Dali::Accessibility::Hypertext*>(accessible);
  DALI_TEST_CHECK(hypertext);
  // text without the anchors markup and ENABLE_MARKUP property set (by default) to false
  DALI_TEST_EQUALS(hypertext->GetLinkCount(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(-1), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(0), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(5), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(-1) == nullptr, true, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(0) == nullptr, true, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(5) == nullptr, true, TEST_LOCATION);
  // text with the anchors markup and ENABLE_MARKUP property set (by default) to false
  textfield.SetProperty(Toolkit::TextField::Property::TEXT, "12345<a href = 'https://www.tizen.org'>anchor1</a>12345<a href = 'https://www.tizen.org' >veryveryveryveryveryveryveryverylonganchor2</a>12345<a href = 'https://www.tizen.org'>anchor3</a>12345");
  DALI_TEST_EQUALS(hypertext->GetLinkCount(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(-1), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(0), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(5), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(-1) == nullptr, true, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(0) == nullptr, true, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(5) == nullptr, true, TEST_LOCATION);
  // text with the anchors markup and ENABLE_MARKUP property set to true
  textfield.SetProperty(Toolkit::TextField::Property::ENABLE_MARKUP, true);
  DALI_TEST_EQUALS(hypertext->GetLinkCount(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(-1), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(0), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(5), 0, TEST_LOCATION);  //1st anchor index
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(17), 1, TEST_LOCATION); //2nd anchor index
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(66), 2, TEST_LOCATION); //3rd anchor index
  DALI_TEST_EQUALS(hypertext->GetLink(-1) == nullptr, true, TEST_LOCATION);
  auto hyperlink = hypertext->GetLink(0);
  DALI_TEST_CHECK(hyperlink);
  DALI_TEST_EQUALS(hyperlink->GetStartIndex(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(hyperlink->GetEndIndex(), 12, TEST_LOCATION);
  DALI_TEST_EQUALS(hyperlink->GetAnchorCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(hyperlink->GetAnchorUri(0), "https://www.tizen.org", TEST_LOCATION);
  auto anchorAccessible = hyperlink->GetAnchorAccessible(0);
  DALI_TEST_EQUALS(hyperlink == anchorAccessible, true, TEST_LOCATION);
  hyperlink = hypertext->GetLink(1);
  DALI_TEST_CHECK(hyperlink);
  DALI_TEST_EQUALS(hyperlink->GetStartIndex(), 17, TEST_LOCATION);
  DALI_TEST_EQUALS(hyperlink->GetEndIndex(), 60, TEST_LOCATION);
  hyperlink = hypertext->GetLink(2);
  DALI_TEST_CHECK(hyperlink);
  DALI_TEST_EQUALS(hyperlink->GetStartIndex(), 65, TEST_LOCATION);
  DALI_TEST_EQUALS(hyperlink->GetEndIndex(), 72, TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>
int UtcDaliAccessibilityTextEditor(void)
{
  ToolkitTestApplication application;

  auto texteditor = TextEditor::New();
  DALI_TEST_CHECK(texteditor);

  texteditor.SetProperty(Actor::Property::NAME, "test");
  DALI_TEST_EQUALS(texteditor.GetProperty<std::string>(Actor::Property::NAME), "test", TEST_LOCATION);

  auto accessible = Dali::Accessibility::Accessible::Get(texteditor);
  DALI_TEST_CHECK(accessible);

  DALI_TEST_EQUALS(accessible->GetName(), "", TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::ENTRY, TEST_LOCATION);
  auto states = accessible->GetStates();
  DALI_TEST_EQUALS(static_cast<unsigned int>(states[Accessibility::State::EDITABLE]), true, TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(true);

  texteditor.SetProperty(Toolkit::TextEditor::Property::TEXT, "test");
  auto text = dynamic_cast<Dali::Accessibility::Text*>(accessible);
  DALI_TEST_CHECK(text);
  DALI_TEST_EQUALS(text->GetText(0, 10), "", TEST_LOCATION);
  DALI_TEST_EQUALS(text->SetCursorOffset(100), false, TEST_LOCATION);
  DALI_TEST_EQUALS(text->SetCursorOffset(2), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetCursorOffset(), 2, TEST_LOCATION);

  auto editabletext = dynamic_cast<Dali::Accessibility::EditableText*>(accessible);
  DALI_TEST_CHECK(editabletext);
  DALI_TEST_EQUALS(editabletext->CopyText(3, 1), false, TEST_LOCATION);
  DALI_TEST_EQUALS(editabletext->CopyText(1, 3), true, TEST_LOCATION);
  DALI_TEST_EQUALS(editabletext->CutText(3, 1), false, TEST_LOCATION);
  DALI_TEST_EQUALS(editabletext->CutText(1, 3), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0, 1), "t", TEST_LOCATION);

  auto range = text->GetRangeOfSelection(1);
  DALI_TEST_EQUALS(range.startOffset, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(range.endOffset, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(range.content, "", TEST_LOCATION);
  DALI_TEST_EQUALS(text->SetRangeOfSelection(1, 0, 1), false, TEST_LOCATION);
  DALI_TEST_EQUALS(text->RemoveSelection(1), false, TEST_LOCATION);

  // Insert into empty field
  DALI_TEST_EQUALS(editabletext->SetTextContents(""), true, TEST_LOCATION);
  DALI_TEST_EQUALS(editabletext->InsertText(1, "xyz"), false, TEST_LOCATION);
  DALI_TEST_EQUALS(editabletext->InsertText(0, "abc"), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0, 3), "abc", TEST_LOCATION);

  // Append at end
  DALI_TEST_EQUALS(editabletext->InsertText(3, "xyz"), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0, 6), "abcxyz", TEST_LOCATION);

  DALI_TEST_EQUALS(editabletext->SetTextContents("adef"), true, TEST_LOCATION);
  DALI_TEST_EQUALS(editabletext->InsertText(1, "bc"), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0, 6), "abcdef", TEST_LOCATION);
  DALI_TEST_EQUALS(editabletext->DeleteText(5, 1), false, TEST_LOCATION);
  DALI_TEST_EQUALS(editabletext->DeleteText(1, 5), true, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0, 2), "af", TEST_LOCATION);

  auto hypertext = dynamic_cast<Dali::Accessibility::Hypertext*>(accessible);
  DALI_TEST_CHECK(hypertext);
  // text without the anchors markup and ENABLE_MARKUP property set (by default) to false
  DALI_TEST_EQUALS(hypertext->GetLinkCount(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(-1), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(0), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(5), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(-1) == nullptr, true, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(0) == nullptr, true, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(5) == nullptr, true, TEST_LOCATION);
  // text with the anchors markup and ENABLE_MARKUP property set (by default) to false
  texteditor.SetProperty(Toolkit::TextEditor::Property::TEXT, "12345<a href = 'https://www.tizen.org'>anchor1</a>12345<a href = 'https://www.tizen.org' >veryveryveryveryveryveryveryverylonganchor2</a>12345<a href = 'https://www.tizen.org'>anchor3</a>12345");
  DALI_TEST_EQUALS(hypertext->GetLinkCount(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(-1), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(0), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(5), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(-1) == nullptr, true, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(0) == nullptr, true, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(5) == nullptr, true, TEST_LOCATION);
  // text with the anchors markup and ENABLE_MARKUP property set to true
  texteditor.SetProperty(Toolkit::TextEditor::Property::ENABLE_MARKUP, true);
  DALI_TEST_EQUALS(hypertext->GetLinkCount(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(-1), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(0), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(5), 0, TEST_LOCATION);  //1st anchor index
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(17), 1, TEST_LOCATION); //2nd anchor index
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(66), 2, TEST_LOCATION); //3rd anchor index
  DALI_TEST_EQUALS(hypertext->GetLink(-1) == nullptr, true, TEST_LOCATION);
  auto hyperlink = hypertext->GetLink(0);
  DALI_TEST_CHECK(hyperlink);
  DALI_TEST_EQUALS(hyperlink->GetStartIndex(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(hyperlink->GetEndIndex(), 12, TEST_LOCATION);
  DALI_TEST_EQUALS(hyperlink->GetAnchorCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(hyperlink->GetAnchorUri(0), "https://www.tizen.org", TEST_LOCATION);
  auto anchorAccessible = hyperlink->GetAnchorAccessible(0);
  DALI_TEST_EQUALS(hyperlink == anchorAccessible, true, TEST_LOCATION);
  hyperlink = hypertext->GetLink(1);
  DALI_TEST_CHECK(hyperlink);
  DALI_TEST_EQUALS(hyperlink->GetStartIndex(), 17, TEST_LOCATION);
  DALI_TEST_EQUALS(hyperlink->GetEndIndex(), 60, TEST_LOCATION);
  hyperlink = hypertext->GetLink(2);
  DALI_TEST_CHECK(hyperlink);
  DALI_TEST_EQUALS(hyperlink->GetStartIndex(), 65, TEST_LOCATION);
  DALI_TEST_EQUALS(hyperlink->GetEndIndex(), 72, TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

int UtcDaliAccessibilityTextLabel(void)
{
  ToolkitTestApplication application;

  auto textlabel = TextLabel::New();
  DALI_TEST_CHECK(textlabel);

  application.GetScene().Add(textlabel);
  application.SendNotification();

  textlabel.SetProperty(Actor::Property::NAME, "test");
  DALI_TEST_EQUALS(textlabel.GetProperty<std::string>(Actor::Property::NAME), "test", TEST_LOCATION);

  auto accessible = Dali::Accessibility::Accessible::Get(textlabel);
  DALI_TEST_CHECK(accessible);

  DALI_TEST_EQUALS(accessible->GetName(), "test", TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetRole(), Accessibility::Role::LABEL, TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(true);

  textlabel.SetProperty(Toolkit::TextLabel::Property::TEXT, "test");
  auto text = dynamic_cast<Dali::Accessibility::Text*>(accessible);
  DALI_TEST_CHECK(text);
  DALI_TEST_EQUALS(text->GetText(0, 10), "", TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetText(0, 4), "test", TEST_LOCATION);
  DALI_TEST_EQUALS(text->SetCursorOffset(0), false, TEST_LOCATION);
  DALI_TEST_EQUALS(text->GetCursorOffset(), 0, TEST_LOCATION);

  auto range = text->GetRangeOfSelection(1);
  DALI_TEST_EQUALS(range.startOffset, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(range.endOffset, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(range.content, "", TEST_LOCATION);
  DALI_TEST_EQUALS(text->SetRangeOfSelection(1, 0, 1), false, TEST_LOCATION);
  DALI_TEST_EQUALS(text->RemoveSelection(1), false, TEST_LOCATION);

  auto hypertext = dynamic_cast<Dali::Accessibility::Hypertext*>(accessible);
  DALI_TEST_CHECK(hypertext);
  // text without the anchors markup and ENABLE_MARKUP property set (by default) to false
  DALI_TEST_EQUALS(hypertext->GetLinkCount(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(-1), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(0), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(5), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(-1) == nullptr, true, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(0) == nullptr, true, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(5) == nullptr, true, TEST_LOCATION);
  // text with the anchors; No anchor created until layout
  textlabel.SetProperty(Toolkit::TextLabel::Property::TEXT, "12345<a href = 'https://www.tizen.org'>anchor1</a>12345<a href = 'https://www.tizen.org' >veryveryveryveryveryveryveryverylonganchor2</a>12345<a href = 'https://www.tizen.org'>anchor3</a>12345");
  DALI_TEST_EQUALS(hypertext->GetLinkCount(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(-1), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(0), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(5), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(-1) == nullptr, true, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(0) == nullptr, true, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLink(5) == nullptr, true, TEST_LOCATION);
  textlabel.SetProperty(Toolkit::TextLabel::Property::ENABLE_MARKUP, true);
  // Triggers text layout
  application.SendNotification();
  application.Render(1);
  // Anchors are created after layout
  DALI_TEST_EQUALS(hypertext->GetLinkCount(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(-1), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(0), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(5), 0, TEST_LOCATION);  //1st anchor index
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(17), 1, TEST_LOCATION); //2nd anchor index
  DALI_TEST_EQUALS(hypertext->GetLinkIndex(66), 2, TEST_LOCATION); //3rd anchor index
  DALI_TEST_EQUALS(hypertext->GetLink(-1) == nullptr, true, TEST_LOCATION);
  auto hyperlink = hypertext->GetLink(0);
  DALI_TEST_CHECK(hyperlink);
  DALI_TEST_EQUALS(hyperlink->GetStartIndex(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(hyperlink->GetEndIndex(), 12, TEST_LOCATION);
  DALI_TEST_EQUALS(hyperlink->GetAnchorCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(hyperlink->GetAnchorUri(0), "https://www.tizen.org", TEST_LOCATION);
  auto anchorAccessible = hyperlink->GetAnchorAccessible(0);
  DALI_TEST_EQUALS(hyperlink == anchorAccessible, true, TEST_LOCATION);
  hyperlink = hypertext->GetLink(1);
  DALI_TEST_CHECK(hyperlink);
  DALI_TEST_EQUALS(hyperlink->GetStartIndex(), 17, TEST_LOCATION);
  DALI_TEST_EQUALS(hyperlink->GetEndIndex(), 60, TEST_LOCATION);
  hyperlink = hypertext->GetLink(2);
  DALI_TEST_CHECK(hyperlink);
  DALI_TEST_EQUALS(hyperlink->GetStartIndex(), 65, TEST_LOCATION);
  DALI_TEST_EQUALS(hyperlink->GetEndIndex(), 72, TEST_LOCATION);

  Dali::Accessibility::TestEnableSC(false);

  END_TEST;
}

#include <dali-toolkit/internal/controls/navigation-view/navigation-view-impl.h>
int UtcDaliAccessibilityNavigationViewConstructor(void)
{
  ToolkitTestApplication application;

  auto navigationview = NavigationView::New();
  DALI_TEST_CHECK(navigationview);

  auto accessible = Dali::Accessibility::Accessible::Get(navigationview);
  DALI_TEST_CHECK(accessible);

  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Accessibility::Role::FILLER, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAccessibilityVideoViewConstructor(void)
{
  ToolkitTestApplication application;

  auto videoview = VideoView::New();
  DALI_TEST_CHECK(videoview);

  auto accessible = Dali::Accessibility::Accessible::Get(videoview);
  DALI_TEST_CHECK(accessible);

  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Accessibility::Role::VIDEO, TEST_LOCATION);

  END_TEST;
}
