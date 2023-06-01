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
 */

#include <dali-toolkit-test-suite-utils.h>
#include <dummy-control.h>
#include <toolkit-event-thread-callback.h>

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>

using namespace Dali;
using namespace Dali::Toolkit;

void dali_svg_visual_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_svg_visual_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* TEST_SVG_FILE_NAME = TEST_RESOURCE_DIR "/svg1.svg";
} // namespace

int UtcDaliSvgVisualChageSize(void)
{
  tet_infoline("Test change transform");

  ToolkitTestApplication application;

  TraceCallStack& textureTrace = application.GetGlAbstraction().GetTextureTrace();
  textureTrace.Enable(true);

  Visual::Base visual = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_FILE_NAME));
  DALI_TEST_CHECK(visual);

  DummyControl      control   = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(control.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  application.SendNotification();

  // Wait for loading
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  control.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
  application.GetScene().Add(control);

  visual.SetTransformAndSize(Property::Map(), Vector2(100, 100));

  // Wait for rasterization but not execute the callback
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 30, false), true, TEST_LOCATION);

  // Change actor size before finishing rasterization
  control.SetProperty(Actor::Property::SIZE, Vector2(300, 300));
  visual.SetTransformAndSize(Property::Map(), Vector2(300, 300));

  application.SendNotification();

  // Wait for rasterization
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  TraceCallStack::NamedParams params;
  params["width"] << 300;
  params["height"] << 300;
  // Should be the final size
  DALI_TEST_EQUALS(textureTrace.FindMethodAndParams("TexImage2D", params), true, TEST_LOCATION);

  END_TEST;
}
