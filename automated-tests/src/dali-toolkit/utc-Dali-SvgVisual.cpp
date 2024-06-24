/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

int UtcDaliSvgVisualSvgCacheFileAndRasterizedTexture(void)
{
  tet_infoline("Test rasterized texture cached");

  ToolkitTestApplication application;

  TraceCallStack& textureTrace = application.GetGlAbstraction().GetTextureTrace();
  textureTrace.Enable(true);

  Visual::Base visual1 = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_FILE_NAME));
  DALI_TEST_CHECK(visual1);
  Visual::Base visual2 = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_FILE_NAME));
  DALI_TEST_CHECK(visual2);
  Visual::Base visual3 = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_FILE_NAME));
  DALI_TEST_CHECK(visual3);

  DummyControl      control1   = DummyControl::New();
  DummyControlImpl& dummyImpl1 = static_cast<DummyControlImpl&>(control1.GetImplementation());
  dummyImpl1.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual1);

  DummyControl      control2   = DummyControl::New();
  DummyControlImpl& dummyImpl2 = static_cast<DummyControlImpl&>(control2.GetImplementation());
  dummyImpl2.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual2);

  DummyControl      control3   = DummyControl::New();
  DummyControlImpl& dummyImpl3 = static_cast<DummyControlImpl&>(control3.GetImplementation());
  dummyImpl3.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual3);

  application.SendNotification();

  // Wait for loading only one time
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  Vector2 size1(100.0f, 100.0f);
  Vector2 size2(300.0f, 300.0f);

  tet_printf("Rasterize control1 and control3 as 100x100, control2 as 300x300\n");

  control1.SetProperty(Actor::Property::SIZE, size1);
  application.GetScene().Add(control1);
  control2.SetProperty(Actor::Property::SIZE, size2);
  application.GetScene().Add(control2);
  control3.SetProperty(Actor::Property::SIZE, size1);
  application.GetScene().Add(control3);

  visual1.SetTransformAndSize(Property::Map(), size1);
  visual2.SetTransformAndSize(Property::Map(), size2);
  visual3.SetTransformAndSize(Property::Map(), size1);

  // Wait for rasterization
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Check we upload only 2 textures.
  TraceCallStack::NamedParams params;
  params["width"] << 100;
  params["height"] << 100;
  DALI_TEST_EQUALS(textureTrace.FindMethodAndParams("TexImage2D", params), true, TEST_LOCATION);
  params.mParams.clear();
  params["width"] << 300;
  params["height"] << 300;
  DALI_TEST_EQUALS(textureTrace.FindMethodAndParams("TexImage2D", params), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  textureTrace.Reset();

  tet_printf("Change control3 size from 100x100, to 300x300\n");

  control3.SetProperty(Actor::Property::SIZE, size2);
  visual3.SetTransformAndSize(Property::Map(), size2);

  application.SendNotification();
  application.Render();

  // Check we don't doing any additional rasterization
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Check we don't upload any additional texture upload
  DALI_TEST_EQUALS(textureTrace.CountMethod("GenTextures"), 0, TEST_LOCATION);

  END_TEST;
}