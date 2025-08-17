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

#include <dali/devel-api/adaptor-framework/accessibility-bridge.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>

#include <dali-scene3d/public-api/controls/model/model.h>
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>

#include <tuple>

using namespace Dali::Toolkit;
using namespace Dali::Toolkit::DevelControl;

void utc_dali_accessibility_highlight_overlay_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_accessibility_highlight_overlay_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliAccessibilityHighlihtOverlayCheckMode(void)
{
  ToolkitTestApplication application;
  AccessibilityHighlightOverlay mHighlightOverlay;

  auto mode = mHighlightOverlay.GetOverlayMode();
  DALI_TEST_EQUALS(mode, OverlayHighlightMode::AUTO, TEST_LOCATION);

  mHighlightOverlay.SetOverlayMode(OverlayHighlightMode::MANUAL);
  mode = mHighlightOverlay.GetOverlayMode();
  DALI_TEST_EQUALS(mode, OverlayHighlightMode::MANUAL, TEST_LOCATION);

  mHighlightOverlay.ResetCustomHighlight();
  mode = mHighlightOverlay.GetOverlayMode();
  DALI_TEST_EQUALS(mode, OverlayHighlightMode::AUTO, TEST_LOCATION);

  auto customPosition = Vector2(10, 10);
  auto customSize     = Vector2(100, 100);
  mHighlightOverlay.SetCustomHighlight(customPosition, customSize);
  mode = mHighlightOverlay.GetOverlayMode();
  DALI_TEST_EQUALS(mode, OverlayHighlightMode::MANUAL, TEST_LOCATION);
  END_TEST;
}
