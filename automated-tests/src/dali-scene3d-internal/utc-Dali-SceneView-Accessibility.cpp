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
#include <dali-toolkit/devel-api/asset-manager/asset-manager.h>
#include <dali-toolkit/devel-api/controls/control-accessible.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>

#include <dali/devel-api/adaptor-framework/accessibility-bridge.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>

#include <dali-scene3d/public-api/controls/model/model.h>
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>

#include <tuple>

using namespace Dali::Toolkit;
using namespace Dali::Toolkit::DevelControl;

constexpr const char* OVERLAY_NAME = "HighlightOverlay";
constexpr const char* HIGHLIGHT_NAME = "HighlightIndicator";

void utc_dali_accessibility_scene_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_accessibility_scene_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliSceneViewAccessibleMarkSceneAccessible(void)
{
  ToolkitTestApplication application;

  auto controlA       = Control::New();
  auto controlParent = Control::New();
  controlParent.Add(controlA);
  application.GetScene().Add(controlParent);

  auto controlB         = Control::New();
  auto sceneParent      = Scene3D::SceneView::New();
  sceneParent.Add(controlB);
  application.GetScene().Add(sceneParent);

  application.SendNotification();
  application.Render(1);

  DALI_TEST_EQUALS(ControlAccessible::IsScene3D(controlParent), false, TEST_LOCATION);
  DALI_TEST_EQUALS(ControlAccessible::IsScene3D(sceneParent), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewAccessibilityRole(void)
{
  ToolkitTestApplication application;

  auto model         = Scene3D::Model::New();
  auto sceneView     = Scene3D::SceneView::New();
  sceneView.Add(model);
  application.GetScene().Add(sceneView);

  model.SetProperty(Property::ACCESSIBILITY_ROLE, AccessibilityRole::MODEL);
  sceneView.SetProperty(Property::ACCESSIBILITY_ROLE, AccessibilityRole::SCENE_3D);

  application.SendNotification();
  application.Render(1);

  int32_t sceneRole = static_cast<uint32_t>(DevelControl::AccessibilityRole::MAX_COUNT);
  if(sceneView.GetProperty(Property::ACCESSIBILITY_ROLE).Get(sceneRole))
  {
    DALI_TEST_EQUALS(sceneRole, static_cast<uint32_t>(AccessibilityRole::SCENE_3D), TEST_LOCATION);
  }

  int32_t modelRole = static_cast<uint32_t>(DevelControl::AccessibilityRole::MAX_COUNT);
  if(model.GetProperty(Property::ACCESSIBILITY_ROLE).Get(modelRole))
  {
    DALI_TEST_EQUALS(modelRole, static_cast<uint32_t>(AccessibilityRole::MODEL), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliSceneViewUpdateOverlay(void)
{
  ToolkitTestApplication application;
  std::string focusBorderImagePath(AssetManager::GetDaliImagePath());
  focusBorderImagePath += "/keyboard_focus.9.png";

  AccessibilityHighlightOverlay mHighlightOverlay;
  auto highlight     = Toolkit::ImageView::New(focusBorderImagePath);
  auto model         = Scene3D::Model::New();
  auto sceneView     = Scene3D::SceneView::New();
  auto container     = Control::New();

  model.Add(highlight);
  sceneView.Add(model);
  container.Add(sceneView);
  application.GetScene().Add(container);

  application.SendNotification();
  application.Render(1);
  auto childCount = container.GetChildCount();
  DALI_TEST_EQUALS(childCount, 1, TEST_LOCATION);
  mHighlightOverlay.UpdateOverlay(highlight);
  application.SendNotification();
  application.Render(1);
  DALI_TEST_EQUALS(container.GetChildCount(), childCount + 1, TEST_LOCATION);

  auto overlay = container.FindChildByName(OVERLAY_NAME);
  DALI_TEST_CHECK(overlay);
  auto indicator = overlay.FindChildByName(HIGHLIGHT_NAME);
  DALI_TEST_CHECK(indicator);
  END_TEST;
}

int UtcDaliSceneViewSetCustomHighlght01(void)
{
  ToolkitTestApplication application;
  std::string focusBorderImagePath(AssetManager::GetDaliImagePath());
  focusBorderImagePath += "/keyboard_focus.9.png";

  AccessibilityHighlightOverlay mHighlightOverlay;
  auto highlight     = Toolkit::ImageView::New(focusBorderImagePath);
  auto model         = Scene3D::Model::New();
  auto sceneView     = Scene3D::SceneView::New();
  auto container     = Control::New();

  model.Add(highlight);
  sceneView.Add(model);
  container.Add(sceneView);
  application.GetScene().Add(container);
  mHighlightOverlay.UpdateOverlay(highlight);
  application.SendNotification();
  application.Render(1);

  auto customPosition = Vector2(10, 10);
  auto customSize     = Vector2(100, 100);
  mHighlightOverlay.SetCustomHighlight(customPosition, customSize);
  mHighlightOverlay.UpdateOverlay(highlight);
  application.SendNotification();
  application.Render(1);

  auto overlay = container.FindChildByName(OVERLAY_NAME);
  auto indicator = overlay.FindChildByName(HIGHLIGHT_NAME);
  DALI_TEST_EQUALS(indicator.GetProperty<Vector2>(Actor::Property::SIZE), Vector2(100.0f, 100.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(indicator.GetProperty<Vector2>(Actor::Property::POSITION), Vector2(10.0f, 10.0f), 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSceneViewSetCustomHighlght02(void)
{
  ToolkitTestApplication application;
  std::string focusBorderImagePath(AssetManager::GetDaliImagePath());
  focusBorderImagePath += "/keyboard_focus.9.png";

  AccessibilityHighlightOverlay mHighlightOverlay;
  auto highlight     = Toolkit::ImageView::New(focusBorderImagePath);
  auto model         = Scene3D::Model::New();
  auto sceneView     = Scene3D::SceneView::New();
  auto container     = Control::New();
  model.Add(highlight);
  sceneView.Add(model);
  container.Add(sceneView);
  application.GetScene().Add(container);
  mHighlightOverlay.UpdateOverlay(highlight);
  application.SendNotification();
  application.Render(1);

  auto accessible = Dali::Accessibility::Accessible::Get(model);
  DALI_TEST_CHECK(accessible);
  if(accessible)
  {
    auto accessibleControl = dynamic_cast<DevelControl::ControlAccessible*>(accessible);
    DALI_TEST_CHECK(accessibleControl);

    auto customPosition = Vector2(10, 10);
    auto customSize     = Vector2(100, 100);
    accessibleControl->SetCustomHighlightOverlay(customPosition, customSize);
    accessibleControl->ResetCustomHighlightOverlay();
  }
  application.SendNotification();
  application.Render(1);

  auto overlay = container.FindChildByName(OVERLAY_NAME);
  auto indicator = overlay.FindChildByName(HIGHLIGHT_NAME);

  DALI_TEST_CHECK(overlay);
  DALI_TEST_CHECK(indicator);
  END_TEST;
}

int UtcDaliSceneViewHideOverlay(void)
{
  ToolkitTestApplication application;
  std::string focusBorderImagePath(AssetManager::GetDaliImagePath());
  focusBorderImagePath += "/keyboard_focus.9.png";

  AccessibilityHighlightOverlay mHighlightOverlay;
  auto highlight     = Toolkit::ImageView::New(focusBorderImagePath);
  auto model         = Scene3D::Model::New();
  auto sceneView     = Scene3D::SceneView::New();
  auto container     = Control::New();

  model.Add(highlight);
  sceneView.Add(model);
  container.Add(sceneView);
  application.GetScene().Add(container);
  mHighlightOverlay.UpdateOverlay(highlight);
  application.SendNotification();
  application.Render(1);

  auto overlay = container.FindChildByName(OVERLAY_NAME);
  auto indicator = overlay.FindChildByName(HIGHLIGHT_NAME);
  DALI_TEST_EQUALS(indicator.GetProperty<bool>(Actor::Property::VISIBLE), true, TEST_LOCATION);
  mHighlightOverlay.HideOverlay();
  application.SendNotification();
  application.Render(1);

  DALI_TEST_EQUALS(indicator.GetProperty<bool>(Actor::Property::VISIBLE), false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSceneViewRemoveHighlight(void)
{
  ToolkitTestApplication application;
  std::string focusBorderImagePath(AssetManager::GetDaliImagePath());
  focusBorderImagePath += "/keyboard_focus.9.png";

  AccessibilityHighlightOverlay mHighlightOverlay;
  auto highlight     = Toolkit::ImageView::New(focusBorderImagePath);
  auto model         = Scene3D::Model::New();
  auto sceneView     = Scene3D::SceneView::New();
  auto container     = Control::New();

  model.Add(highlight);
  sceneView.Add(model);
  container.Add(sceneView);
  application.GetScene().Add(container);
  mHighlightOverlay.UpdateOverlay(highlight);
  application.SendNotification();
  application.Render(1);

  auto overlay = container.FindChildByName(OVERLAY_NAME);
  auto indicator = overlay.FindChildByName(HIGHLIGHT_NAME);
  DALI_TEST_CHECK(overlay);

  overlay.Remove(indicator);
  mHighlightOverlay.UpdateOverlay(highlight);
  application.SendNotification();
  application.Render(1);

  auto newIndicator = overlay.FindChildByName(HIGHLIGHT_NAME);
  DALI_TEST_CHECK(newIndicator);
  END_TEST;
}
