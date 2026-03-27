/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/internal/controls/control/control-internal.h>
#include <dali-toolkit/internal/controls/image-view/image-view-impl.h>
#include <toolkit-event-thread-callback.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
// resolution: 34*34, pixel format: RGBA8888
static const char* gImage_34_RGBA = TEST_RESOURCE_DIR "/icon-edit.png";

} // namespace

void utc_dali_toolkit_internal_image_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_internal_image_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliImageViewOnUpdateVisualProperties(void)
{
  tet_infoline("Test OnUpdateVisualProperties method to cover the UPDATE_PROPERTY action code block.");

  ToolkitTestApplication application;

  // Create ImageView with an image
  Property::Map imageMap;
  imageMap[Toolkit::Visual::Property::TYPE]     = Toolkit::Visual::IMAGE;
  imageMap[Toolkit::ImageVisual::Property::URL] = gImage_34_RGBA;

  ImageView imageView = ImageView::New();
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  application.GetScene().Add(imageView);

  application.SendNotification();
  application.Render();

  // Wait for image to load
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Verify the image visual is ready
  DALI_TEST_EQUALS(imageView.IsResourceReady(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(imageView.GetRendererCount(), 1u, TEST_LOCATION);

  // Get the internal implementation
  Toolkit::ControlImpl& controlImpl = GetImplementation(imageView);
  Dali::Toolkit::Internal::Control& internalControl = Dali::Toolkit::Internal::Control::Get(controlImpl);

  // Create property updates for the image visual using Dali::Pair
  Dali::Vector<Dali::Pair<Dali::Property::Index, Dali::Property::Map>> properties;

  Property::Map updateMap;
  updateMap[Toolkit::Visual::Property::OPACITY] = 0.5f;

  properties.PushBack(Dali::Pair<Dali::Property::Index, Dali::Property::Map>(Toolkit::ImageView::Property::IMAGE, updateMap));

  // Call UpdateVisualProperties which triggers OnUpdateVisualProperties internally
  internalControl.UpdateVisualProperties(properties);

  application.SendNotification();
  application.Render();

  // Verify the visual was updated
  Toolkit::Visual::Base visual = DevelControl::GetVisual(controlImpl, Toolkit::ImageView::Property::IMAGE);
  DALI_TEST_CHECK(visual);

  Property::Map resultMap;
  visual.CreatePropertyMap(resultMap);

  // Check that opacity was updated
  Property::Value* opacityValue = resultMap.Find(Toolkit::Visual::Property::OPACITY);
  DALI_TEST_CHECK(opacityValue);

  float opacity;
  DALI_TEST_CHECK(opacityValue->Get(opacity));
  DALI_TEST_EQUALS(opacity, 0.5f, TEST_LOCATION);

  END_TEST;
}
