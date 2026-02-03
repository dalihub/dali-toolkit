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
 */

#include <dali-toolkit-test-suite-utils.h>
#include <dummy-control.h>
#include <toolkit-event-thread-callback.h>

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* TEST_SVG_FILE_NAME                   = TEST_RESOURCE_DIR "/svg1.svg";
const char* TEST_SVG_INVALID_LOAD_FILE_NAME      = TEST_RESOURCE_DIR "/invalid.svg";  ///< Load fail.
const char* TEST_SVG_INVALID_RASTERIZE_FILE_NAME = TEST_RESOURCE_DIR "/invalid1.svg"; ///< Load succes but rasterize fail.

bool    gResourceReadySignalFired[10];
int32_t gResourceReadySignalTestId;

void ResourceReadySignal(Control control)
{
  int32_t id = control.GetProperty<int32_t>(control.GetPropertyIndex("ResourceReadySignalTestId"));
  DALI_TEST_CHECK(id > 0);
  gResourceReadySignalFired[id] = true;
}

void PrepareResourceReadySignalTest()
{
  for(int i = 0; i < 10; ++i)
  {
    gResourceReadySignalFired[i] = false;
  }
  gResourceReadySignalTestId = 0;
}

void AttachResourceReadySignal(Control control)
{
  control.RegisterProperty("ResourceReadySignalTestId", ++gResourceReadySignalTestId);
  control.ResourceReadySignal().Connect(&ResourceReadySignal);
}

void TestResourceReadySignalFired(Control control, bool expected, const char* location, bool resetFlag = true)
{
  int32_t id = control.GetProperty<int32_t>(control.GetPropertyIndex("ResourceReadySignalTestId"));
  DALI_TEST_CHECK(id > 0);
  DALI_TEST_EQUALS(gResourceReadySignalFired[id], expected, location);
  if(resetFlag)
  {
    gResourceReadySignalFired[id] = false;
  }
}
} // namespace

void dali_svg_visual_startup(void)
{
  test_return_value = TET_UNDEF;

  PrepareResourceReadySignalTest();
}

void dali_svg_visual_cleanup(void)
{
  test_return_value = TET_PASS;
}

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
  AttachResourceReadySignal(control);

  application.SendNotification();

  // Wait for loading
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  TestResourceReadySignalFired(control, false, TEST_LOCATION);

  control.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
  application.GetScene().Add(control);

  visual.SetTransformAndSize(Property::Map(), Vector2(100, 100));

  // Wait for rasterization but not execute the callback
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 30, false), true, TEST_LOCATION);

  TestResourceReadySignalFired(control, false, TEST_LOCATION);

  // Change actor size before finishing rasterization
  control.SetProperty(Actor::Property::SIZE, Vector2(300, 300));
  visual.SetTransformAndSize(Property::Map(), Vector2(300, 300));

  application.SendNotification();

  TestResourceReadySignalFired(control, false, TEST_LOCATION);

  // Wait for rasterization
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  TestResourceReadySignalFired(control, true, TEST_LOCATION);

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
  AttachResourceReadySignal(control1);
  AttachResourceReadySignal(control2);
  AttachResourceReadySignal(control3);

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

  TestResourceReadySignalFired(control1, false, TEST_LOCATION);
  TestResourceReadySignalFired(control2, false, TEST_LOCATION);
  TestResourceReadySignalFired(control3, false, TEST_LOCATION);

  // Wait for rasterization
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  TestResourceReadySignalFired(control1, true, TEST_LOCATION);
  TestResourceReadySignalFired(control2, true, TEST_LOCATION);
  TestResourceReadySignalFired(control3, true, TEST_LOCATION);

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

int UtcDaliSvgVisualLoadPolicyImmediate(void)
{
  tet_infoline("Test load and rasterize request immediate");

  ToolkitTestApplication application;

  TraceCallStack& textureTrace = application.GetGlAbstraction().GetTextureTrace();
  textureTrace.Enable(true);
  textureTrace.EnableLogging(true);

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::SVG);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_SVG_FILE_NAME);
  propertyMap.Insert(ImageVisual::Property::DESIRED_WIDTH, 20);
  propertyMap.Insert(ImageVisual::Property::DESIRED_HEIGHT, 30);
  propertyMap.Insert("loadPolicy", ImageVisual::LoadPolicy::IMMEDIATE);

  Visual::Base visual1 = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual1);

  // Wait for loading and rasterizing
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  Visual::Base visual2 = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_FILE_NAME).Add(ImageVisual::Property::DESIRED_WIDTH, 20).Add(ImageVisual::Property::DESIRED_HEIGHT, 30));
  DALI_TEST_CHECK(visual2);

  DummyControl      control1   = DummyControl::New();
  DummyControlImpl& dummyImpl1 = static_cast<DummyControlImpl&>(control1.GetImplementation());
  dummyImpl1.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual1);

  DummyControl      control2   = DummyControl::New();
  DummyControlImpl& dummyImpl2 = static_cast<DummyControlImpl&>(control2.GetImplementation());
  dummyImpl2.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual2);
  AttachResourceReadySignal(control1);
  AttachResourceReadySignal(control2);

  // Check we don't request load.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);
  TestResourceReadySignalFired(control1, false, TEST_LOCATION);
  TestResourceReadySignalFired(control2, false, TEST_LOCATION);

  application.GetScene().Add(control1);
  application.GetScene().Add(control2);

  // Check we don't rasterize again.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);
  TestResourceReadySignalFired(control1, true, TEST_LOCATION);
  TestResourceReadySignalFired(control2, true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Check we upload only 1 textures.
  DALI_TEST_EQUALS(textureTrace.CountMethod("GenTextures"), 1, TEST_LOCATION);
  TraceCallStack::NamedParams params;
  params["width"] << 20;
  params["height"] << 30;
  DALI_TEST_EQUALS(textureTrace.FindMethodAndParams("TexImage2D", params), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  textureTrace.Reset();

  END_TEST;
}

int UtcDaliSvgVisualReleasePolicy01(void)
{
  tet_infoline("Test release policy destroyed");

  ToolkitTestApplication application;

  TraceCallStack& textureTrace = application.GetGlAbstraction().GetTextureTrace();
  textureTrace.Enable(true);
  textureTrace.EnableLogging(true);

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::SVG);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_SVG_FILE_NAME);
  propertyMap.Insert(ImageVisual::Property::RELEASE_POLICY, ImageVisual::ReleasePolicy::DESTROYED);

  Visual::Base visual1 = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual1);

  Visual::Base visual2 = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_FILE_NAME));
  DALI_TEST_CHECK(visual2);

  DummyControl      control1   = DummyControl::New();
  DummyControlImpl& dummyImpl1 = static_cast<DummyControlImpl&>(control1.GetImplementation());
  dummyImpl1.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual1);

  DummyControl      control2   = DummyControl::New();
  DummyControlImpl& dummyImpl2 = static_cast<DummyControlImpl&>(control2.GetImplementation());
  dummyImpl2.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual2);
  AttachResourceReadySignal(control1);
  AttachResourceReadySignal(control2);

  // Check we request load only 1 time.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  Vector2 size(100.0f, 100.0f);
  control1.SetProperty(Actor::Property::SIZE, size);
  application.GetScene().Add(control1);
  control2.SetProperty(Actor::Property::SIZE, size);
  application.GetScene().Add(control2);

  visual1.SetTransformAndSize(Property::Map(), size);
  visual2.SetTransformAndSize(Property::Map(), size);
  TestResourceReadySignalFired(control1, false, TEST_LOCATION);
  TestResourceReadySignalFired(control2, false, TEST_LOCATION);

  // Check we request rasterize 1 time.
  DALI_TEST_EQUALS(control1.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(control2.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  TestResourceReadySignalFired(control1, true, TEST_LOCATION);
  TestResourceReadySignalFired(control2, true, TEST_LOCATION);
  DALI_TEST_EQUALS(control1.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(control2.GetRendererCount(), 1u, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Check we upload only 1 textures.
  DALI_TEST_EQUALS(textureTrace.CountMethod("GenTextures"), 1, TEST_LOCATION);
  TraceCallStack::NamedParams params;
  params["width"] << 100;
  params["height"] << 100;
  DALI_TEST_EQUALS(textureTrace.FindMethodAndParams("TexImage2D", params), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  textureTrace.Reset();

  tet_printf("Detach visual1 and visual2\n");
  control1.Unparent();
  control2.Unparent();
  DALI_TEST_EQUALS(control1.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(control2.GetRendererCount(), 0u, TEST_LOCATION);

  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();

  tet_printf("Check whether svg cached\n");
  {
    Visual::Base dummyVisual = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_FILE_NAME));
    DALI_TEST_CHECK(dummyVisual);

    DummyControl      dummyControl     = DummyControl::New();
    DummyControlImpl& dummyControlImpl = static_cast<DummyControlImpl&>(dummyControl.GetImplementation());
    dummyControlImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, dummyVisual);
    AttachResourceReadySignal(dummyControl);

    dummyControl.SetProperty(Actor::Property::SIZE, size);
    application.GetScene().Add(dummyControl);

    // Still rasterize not requested. The number of renderer still zero.
    DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 0u, TEST_LOCATION);
    dummyVisual.SetTransformAndSize(Property::Map(), size);
    // After request rasterize, we will use cached svg texture. The number of renderer is one.
    DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 1u, TEST_LOCATION);

    // Check we don't request load and rasterize.
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);
    TestResourceReadySignalFired(dummyControl, true, TEST_LOCATION);

    dummyControl.Unparent();
    dummyControl.Reset();
    dummyVisual.Reset();

    application.SendNotification();
    application.Render();
    application.RunIdles();
    application.SendNotification();
    application.Render();
  }

  tet_printf("SceneOn the same visual again\n");
  application.GetScene().Add(control1);
  application.GetScene().Add(control2);

  // Only control1 have renderer this time, since release policy is not DETACHED.
  DALI_TEST_EQUALS(control1.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(control2.GetRendererCount(), 0u, TEST_LOCATION);

  // Note : We don't send ResourceReady signal again for release-policy is not DETACHED.
  TestResourceReadySignalFired(control1, false, TEST_LOCATION);
  TestResourceReadySignalFired(control2, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(control1.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(control2.GetRendererCount(), 1u, TEST_LOCATION);
  TestResourceReadySignalFired(control1, false, TEST_LOCATION);
  TestResourceReadySignalFired(control2, true, TEST_LOCATION);
  application.RunIdles();
  application.SendNotification();
  application.Render();

  tet_printf("Check whether svg cached\n");
  {
    Visual::Base dummyVisual = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_FILE_NAME));
    DALI_TEST_CHECK(dummyVisual);

    DummyControl      dummyControl     = DummyControl::New();
    DummyControlImpl& dummyControlImpl = static_cast<DummyControlImpl&>(dummyControl.GetImplementation());
    dummyControlImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, dummyVisual);
    AttachResourceReadySignal(dummyControl);

    dummyControl.SetProperty(Actor::Property::SIZE, size);
    application.GetScene().Add(dummyControl);

    // Still rasterize not requested. The number of renderer still zero.
    DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 0u, TEST_LOCATION);
    dummyVisual.SetTransformAndSize(Property::Map(), size);
    // After request rasterize, we will use cached svg texture. The number of renderer is one.
    DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 1u, TEST_LOCATION);

    // Check we don't request load and rasterize.
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);
    TestResourceReadySignalFired(dummyControl, true, TEST_LOCATION);

    dummyControl.Unparent();
    dummyControl.Reset();
    dummyVisual.Reset();

    application.SendNotification();
    application.Render();
    application.RunIdles();
    application.SendNotification();
    application.Render();
  }

  tet_printf("Destroy visual1 and visual2\n");
  control1.Unparent();
  control2.Unparent();
  control1.Reset();
  control2.Reset();
  visual1.Reset();
  visual2.Reset();

  // Ensure to remove cache.
  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();

  tet_printf("Check whether svg cache removed\n");
  {
    Visual::Base dummyVisual = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_FILE_NAME));
    DALI_TEST_CHECK(dummyVisual);

    DummyControl      dummyControl     = DummyControl::New();
    DummyControlImpl& dummyControlImpl = static_cast<DummyControlImpl&>(dummyControl.GetImplementation());
    dummyControlImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, dummyVisual);
    AttachResourceReadySignal(dummyControl);

    dummyControl.SetProperty(Actor::Property::SIZE, size);
    application.GetScene().Add(dummyControl);

    DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 0u, TEST_LOCATION);
    dummyVisual.SetTransformAndSize(Property::Map(), size);
    DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 0u, TEST_LOCATION);

    // Check we request load and rasterize.
    TestResourceReadySignalFired(dummyControl, false, TEST_LOCATION);
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
    TestResourceReadySignalFired(dummyControl, true, TEST_LOCATION);
    DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 1u, TEST_LOCATION);

    dummyControl.Unparent();
    dummyControl.Reset();
    dummyVisual.Reset();

    application.SendNotification();
    application.Render();
    application.RunIdles();
    application.SendNotification();
    application.Render();
  }

  END_TEST;
}

int UtcDaliSvgVisualReleasePolicy02(void)
{
  tet_infoline("Test release policy never");

  ToolkitTestApplication application;

  TraceCallStack& textureTrace = application.GetGlAbstraction().GetTextureTrace();
  textureTrace.Enable(true);
  textureTrace.EnableLogging(true);

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::SVG);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_SVG_FILE_NAME);
  propertyMap.Insert(ImageVisual::Property::RELEASE_POLICY, ImageVisual::ReleasePolicy::NEVER);

  Visual::Base visual1 = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual1);

  Visual::Base visual2 = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_FILE_NAME));
  DALI_TEST_CHECK(visual2);

  DummyControl      control1   = DummyControl::New();
  DummyControlImpl& dummyImpl1 = static_cast<DummyControlImpl&>(control1.GetImplementation());
  dummyImpl1.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual1);

  DummyControl      control2   = DummyControl::New();
  DummyControlImpl& dummyImpl2 = static_cast<DummyControlImpl&>(control2.GetImplementation());
  dummyImpl2.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual2);
  AttachResourceReadySignal(control1);
  AttachResourceReadySignal(control2);

  application.SendNotification();

  // Check we request load only 1 time.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  Vector2 size(100.0f, 100.0f);
  control1.SetProperty(Actor::Property::SIZE, size);
  application.GetScene().Add(control1);
  control2.SetProperty(Actor::Property::SIZE, size);
  application.GetScene().Add(control2);

  visual1.SetTransformAndSize(Property::Map(), size);
  visual2.SetTransformAndSize(Property::Map(), size);
  TestResourceReadySignalFired(control1, false, TEST_LOCATION);
  TestResourceReadySignalFired(control2, false, TEST_LOCATION);

  // Check we request rasterize 1 time.
  DALI_TEST_EQUALS(control1.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(control2.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  TestResourceReadySignalFired(control1, true, TEST_LOCATION);
  TestResourceReadySignalFired(control2, true, TEST_LOCATION);
  DALI_TEST_EQUALS(control1.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(control2.GetRendererCount(), 1u, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Check we upload only 1 textures.
  DALI_TEST_EQUALS(textureTrace.CountMethod("GenTextures"), 1, TEST_LOCATION);
  TraceCallStack::NamedParams params;
  params["width"] << 100;
  params["height"] << 100;
  DALI_TEST_EQUALS(textureTrace.FindMethodAndParams("TexImage2D", params), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  textureTrace.Reset();

  tet_printf("Detach visual1 and visual2\n");
  control1.Unparent();
  control2.Unparent();
  DALI_TEST_EQUALS(control1.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(control2.GetRendererCount(), 0u, TEST_LOCATION);

  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();

  tet_printf("Check whether svg cached\n");
  {
    Visual::Base dummyVisual = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_FILE_NAME));
    DALI_TEST_CHECK(dummyVisual);

    DummyControl      dummyControl     = DummyControl::New();
    DummyControlImpl& dummyControlImpl = static_cast<DummyControlImpl&>(dummyControl.GetImplementation());
    dummyControlImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, dummyVisual);
    AttachResourceReadySignal(dummyControl);

    dummyControl.SetProperty(Actor::Property::SIZE, size);
    application.GetScene().Add(dummyControl);

    // Still rasterize not requested. The number of renderer still zero.
    DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 0u, TEST_LOCATION);
    dummyVisual.SetTransformAndSize(Property::Map(), size);
    // After request rasterize, we will use cached svg texture. The number of renderer is one.
    DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 1u, TEST_LOCATION);

    // Check we don't request load and rasterize.
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);
    TestResourceReadySignalFired(dummyControl, true, TEST_LOCATION);

    dummyControl.Unparent();
    dummyControl.Reset();
    dummyVisual.Reset();

    application.SendNotification();
    application.Render();
    application.RunIdles();
    application.SendNotification();
    application.Render();
  }

  tet_printf("SceneOn the same visual again\n");
  application.GetScene().Add(control1);
  application.GetScene().Add(control2);

  // Only control1 have renderer this time, since release policy is not DETACHED.
  DALI_TEST_EQUALS(control1.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(control2.GetRendererCount(), 0u, TEST_LOCATION);

  // Note : We don't send ResourceReady signal again for release-policy is not DETACHED.
  TestResourceReadySignalFired(control1, false, TEST_LOCATION);
  TestResourceReadySignalFired(control2, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(control1.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(control2.GetRendererCount(), 1u, TEST_LOCATION);
  TestResourceReadySignalFired(control1, false, TEST_LOCATION);
  TestResourceReadySignalFired(control2, true, TEST_LOCATION);

  application.RunIdles();
  application.SendNotification();
  application.Render();

  tet_printf("Check whether svg cached\n");
  {
    Visual::Base dummyVisual = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_FILE_NAME));
    DALI_TEST_CHECK(dummyVisual);

    DummyControl      dummyControl     = DummyControl::New();
    DummyControlImpl& dummyControlImpl = static_cast<DummyControlImpl&>(dummyControl.GetImplementation());
    dummyControlImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, dummyVisual);
    AttachResourceReadySignal(dummyControl);

    dummyControl.SetProperty(Actor::Property::SIZE, size);
    application.GetScene().Add(dummyControl);

    // Still rasterize not requested. The number of renderer still zero.
    DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 0u, TEST_LOCATION);
    dummyVisual.SetTransformAndSize(Property::Map(), size);
    // After request rasterize, we will use cached svg texture. The number of renderer is one.
    DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 1u, TEST_LOCATION);

    // Check we don't request load and rasterize.
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);
    TestResourceReadySignalFired(dummyControl, true, TEST_LOCATION);

    dummyControl.Unparent();
    dummyControl.Reset();
    dummyVisual.Reset();

    application.SendNotification();
    application.Render();
    application.RunIdles();
    application.SendNotification();
    application.Render();
  }

  tet_printf("Destroy visual1 and visual2\n");
  control1.Unparent();
  control2.Unparent();
  control1.Reset();
  control2.Reset();
  visual1.Reset();
  visual2.Reset();

  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();

  tet_printf("Check whether svg cached\n");
  {
    Visual::Base dummyVisual = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_FILE_NAME));
    DALI_TEST_CHECK(dummyVisual);

    DummyControl      dummyControl     = DummyControl::New();
    DummyControlImpl& dummyControlImpl = static_cast<DummyControlImpl&>(dummyControl.GetImplementation());
    dummyControlImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, dummyVisual);
    AttachResourceReadySignal(dummyControl);

    dummyControl.SetProperty(Actor::Property::SIZE, size);
    application.GetScene().Add(dummyControl);

    // Still rasterize not requested. The number of renderer still zero.
    DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 0u, TEST_LOCATION);
    dummyVisual.SetTransformAndSize(Property::Map(), size);
    // After request rasterize, we will use cached svg texture. The number of renderer is one.
    DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 1u, TEST_LOCATION);

    // Check we don't request load and rasterize.
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);
    TestResourceReadySignalFired(dummyControl, true, TEST_LOCATION);

    dummyControl.Unparent();
    dummyControl.Reset();
    dummyVisual.Reset();

    application.SendNotification();
    application.Render();
    application.RunIdles();
    application.SendNotification();
    application.Render();
  }

  END_TEST;
}

int UtcDaliSvgVisualReleasePolicy03(void)
{
  tet_infoline("Test detached policy release cache well");

  ToolkitTestApplication application;

  TraceCallStack& textureTrace = application.GetGlAbstraction().GetTextureTrace();
  textureTrace.Enable(true);
  textureTrace.EnableLogging(true);

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::SVG);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_SVG_FILE_NAME);
  propertyMap.Insert(ImageVisual::Property::RELEASE_POLICY, ImageVisual::ReleasePolicy::DETACHED);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      control   = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(control.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
  AttachResourceReadySignal(control);

  application.SendNotification();

  // request load.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  Vector2 size(100.0f, 100.0f);
  control.SetProperty(Actor::Property::SIZE, size);
  application.GetScene().Add(control);

  visual.SetTransformAndSize(Property::Map(), size);
  TestResourceReadySignalFired(control, false, TEST_LOCATION);

  // request rasterize.
  DALI_TEST_EQUALS(control.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetRendererCount(), 1u, TEST_LOCATION);

  TestResourceReadySignalFired(control, true, TEST_LOCATION);

  // Unparent control.
  control.Unparent();

  // Ensure to remove cache.
  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();

  // Add again, and check resource ready callback comes again too.
  application.GetScene().Add(control);

  visual.SetTransformAndSize(Property::Map(), size);
  TestResourceReadySignalFired(control, false, TEST_LOCATION);

  // request rasterize.
  DALI_TEST_EQUALS(control.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetRendererCount(), 1u, TEST_LOCATION);

  TestResourceReadySignalFired(control, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSvgVisualRasterizeWithDesiredSize01(void)
{
  tet_infoline("Test desired size case rasterize request");

  ToolkitTestApplication application;

  TraceCallStack& textureTrace = application.GetGlAbstraction().GetTextureTrace();
  textureTrace.Enable(true);
  textureTrace.EnableLogging(true);

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::SVG);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_SVG_FILE_NAME);
  propertyMap.Insert(ImageVisual::Property::DESIRED_WIDTH, 100);
  propertyMap.Insert(ImageVisual::Property::DESIRED_HEIGHT, 100);

  Visual::Base visual1 = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual1);

  Visual::Base visual2 = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_FILE_NAME));
  DALI_TEST_CHECK(visual2);

  DummyControl      control1   = DummyControl::New();
  DummyControlImpl& dummyImpl1 = static_cast<DummyControlImpl&>(control1.GetImplementation());
  dummyImpl1.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual1);

  DummyControl      control2   = DummyControl::New();
  DummyControlImpl& dummyImpl2 = static_cast<DummyControlImpl&>(control2.GetImplementation());
  dummyImpl2.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual2);
  AttachResourceReadySignal(control1);
  AttachResourceReadySignal(control2);

  application.SendNotification();

  // Check we request load only 1 time.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  Vector2 size(100.0f, 100.0f);
  control1.SetProperty(Actor::Property::SIZE, size);
  application.GetScene().Add(control1);
  control2.SetProperty(Actor::Property::SIZE, size);
  application.GetScene().Add(control2);
  TestResourceReadySignalFired(control1, false, TEST_LOCATION);
  TestResourceReadySignalFired(control2, false, TEST_LOCATION);

  // Check we request rasterize 1 time, due to desired size set to visual1.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  TestResourceReadySignalFired(control1, true, TEST_LOCATION);
  TestResourceReadySignalFired(control2, false, TEST_LOCATION); ///< Control without desired size till not reasource ready. (For match old behavior)

  visual1.SetTransformAndSize(Property::Map(), size);
  visual2.SetTransformAndSize(Property::Map(), size);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);
  TestResourceReadySignalFired(control1, false, TEST_LOCATION);
  TestResourceReadySignalFired(control2, true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Check we upload only 1 textures.
  DALI_TEST_EQUALS(textureTrace.CountMethod("GenTextures"), 1, TEST_LOCATION);
  TraceCallStack::NamedParams params;
  params["width"] << 100;
  params["height"] << 100;
  DALI_TEST_EQUALS(textureTrace.FindMethodAndParams("TexImage2D", params), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  textureTrace.Reset();

  tet_printf("Change visual1 size to 200x200, and check rasterization not requested (due to desired size is 100x100)\n");
  control1.SetProperty(Actor::Property::SIZE, size * 2.0f);
  visual1.SetTransformAndSize(Property::Map(), size * 2.0f);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  tet_printf("Unparent and reload again. Check rasterization requested again.\n");
  control1.Unparent();
  control2.Unparent();

  // Ensure to remove the cache.
  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();

  // Scene on again. Rasterization should be requested again for visual1.
  application.GetScene().Add(control1);
  // rasterize only (load reference holded at visual)
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliSvgVisualRasterizeWithDesiredSize02(void)
{
  tet_infoline("Test desired size case rasterize request for invalid svg (load failed)");

  ToolkitTestApplication application;

  TraceCallStack& textureTrace = application.GetGlAbstraction().GetTextureTrace();
  textureTrace.Enable(true);
  textureTrace.EnableLogging(true);

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::SVG);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_SVG_INVALID_LOAD_FILE_NAME);
  propertyMap.Insert(ImageVisual::Property::DESIRED_WIDTH, 100);
  propertyMap.Insert(ImageVisual::Property::DESIRED_HEIGHT, 100);

  Visual::Base visual1 = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual1);

  Visual::Base visual2 = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_INVALID_LOAD_FILE_NAME));
  DALI_TEST_CHECK(visual2);

  DummyControl      control1   = DummyControl::New();
  DummyControlImpl& dummyImpl1 = static_cast<DummyControlImpl&>(control1.GetImplementation());
  dummyImpl1.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual1);

  DummyControl      control2   = DummyControl::New();
  DummyControlImpl& dummyImpl2 = static_cast<DummyControlImpl&>(control2.GetImplementation());
  dummyImpl2.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual2);
  AttachResourceReadySignal(control1);
  AttachResourceReadySignal(control2);

  application.SendNotification();

  // Check we request load only 1 time.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  Vector2 size(100.0f, 100.0f);
  control1.SetProperty(Actor::Property::SIZE, size);
  application.GetScene().Add(control1);
  control2.SetProperty(Actor::Property::SIZE, size);
  application.GetScene().Add(control2);
  TestResourceReadySignalFired(control1, true, TEST_LOCATION);
  TestResourceReadySignalFired(control2, true, TEST_LOCATION);

  // Check we don't request rasterize when we already know that file broken.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);

  visual1.SetTransformAndSize(Property::Map(), size);
  visual2.SetTransformAndSize(Property::Map(), size);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);
  TestResourceReadySignalFired(control1, false, TEST_LOCATION);
  TestResourceReadySignalFired(control2, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  tet_printf("Unparent and reload again. Check rasterization requested again.\n");
  control1.Unparent();
  control2.Unparent();

  // Ensure to remove the cache.
  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();

  // Scene on again. Rasterization should be requested again for visual1.
  application.GetScene().Add(control1);
  // rasterize will not requested here.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliSvgVisualRasterizeWithDesiredSize03(void)
{
  tet_infoline("Test desired size case rasterize request for invalid svg (rasterize failed)");

  ToolkitTestApplication application;

  TraceCallStack& textureTrace = application.GetGlAbstraction().GetTextureTrace();
  textureTrace.Enable(true);
  textureTrace.EnableLogging(true);

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::SVG);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_SVG_INVALID_RASTERIZE_FILE_NAME);
  propertyMap.Insert(ImageVisual::Property::DESIRED_WIDTH, 100);
  propertyMap.Insert(ImageVisual::Property::DESIRED_HEIGHT, 100);

  Visual::Base visual1 = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual1);

  Visual::Base visual2 = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_INVALID_RASTERIZE_FILE_NAME));
  DALI_TEST_CHECK(visual2);

  DummyControl      control1   = DummyControl::New();
  DummyControlImpl& dummyImpl1 = static_cast<DummyControlImpl&>(control1.GetImplementation());
  dummyImpl1.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual1);

  DummyControl      control2   = DummyControl::New();
  DummyControlImpl& dummyImpl2 = static_cast<DummyControlImpl&>(control2.GetImplementation());
  dummyImpl2.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual2);
  AttachResourceReadySignal(control1);
  AttachResourceReadySignal(control2);

  application.SendNotification();

  // Check we request load only 1 time.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  Vector2 size(100.0f, 100.0f);
  control1.SetProperty(Actor::Property::SIZE, size);
  application.GetScene().Add(control1);
  control2.SetProperty(Actor::Property::SIZE, size);
  application.GetScene().Add(control2);
  TestResourceReadySignalFired(control1, false, TEST_LOCATION);
  TestResourceReadySignalFired(control2, false, TEST_LOCATION);

  // Check we request rasterize 1 time, due to desired size set to visual1.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  TestResourceReadySignalFired(control1, true, TEST_LOCATION);
  TestResourceReadySignalFired(control2, false, TEST_LOCATION); ///< Control without desired size till not reasource ready. (For match old behavior)

  visual1.SetTransformAndSize(Property::Map(), size);
  visual2.SetTransformAndSize(Property::Map(), size);

  // Since rasterize result is failed, control2 try to re-rasterize. Trigger will comes.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  TestResourceReadySignalFired(control1, false, TEST_LOCATION);
  TestResourceReadySignalFired(control2, true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  tet_printf("Unparent and reload again. Check rasterization requested again.\n");
  control1.Unparent();
  control2.Unparent();

  // Ensure to remove the cache.
  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();

  // Scene on again. Rasterization should be requested again for visual1.
  application.GetScene().Add(control1);
  // rasterize will not requested here.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliSvgVisualRasterizeWithDesiredSize04(void)
{
  tet_infoline("Test desired size case rasterize request for invalid svg (rasterize failed) as synchrnously");

  ToolkitTestApplication application;

  TraceCallStack& textureTrace = application.GetGlAbstraction().GetTextureTrace();
  textureTrace.Enable(true);
  textureTrace.EnableLogging(true);

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::SVG);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_SVG_INVALID_RASTERIZE_FILE_NAME);
  propertyMap.Insert(ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  propertyMap.Insert(ImageVisual::Property::DESIRED_WIDTH, 100);
  propertyMap.Insert(ImageVisual::Property::DESIRED_HEIGHT, 100);

  Visual::Base visual1 = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual1);

  Visual::Base visual2 = VisualFactory::Get().CreateVisual(Property::Map().Add(ImageVisual::Property::URL, TEST_SVG_INVALID_RASTERIZE_FILE_NAME));
  DALI_TEST_CHECK(visual2);

  DummyControl      control1   = DummyControl::New();
  DummyControlImpl& dummyImpl1 = static_cast<DummyControlImpl&>(control1.GetImplementation());
  dummyImpl1.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual1);

  DummyControl      control2   = DummyControl::New();
  DummyControlImpl& dummyImpl2 = static_cast<DummyControlImpl&>(control2.GetImplementation());
  dummyImpl2.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual2);
  AttachResourceReadySignal(control1);
  AttachResourceReadySignal(control2);

  application.SendNotification();

  // Check we don't request load for visual2, since we already load synchronously.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);

  Vector2 size(100.0f, 100.0f);
  control1.SetProperty(Actor::Property::SIZE, size);
  application.GetScene().Add(control1);
  control2.SetProperty(Actor::Property::SIZE, size);
  application.GetScene().Add(control2);
  TestResourceReadySignalFired(control1, true, TEST_LOCATION);
  TestResourceReadySignalFired(control2, false, TEST_LOCATION);

  visual1.SetTransformAndSize(Property::Map(), size);
  visual2.SetTransformAndSize(Property::Map(), size);

  // Since rasterize result is failed, control2 try to re-rasterize. Trigger will comes.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  TestResourceReadySignalFired(control1, false, TEST_LOCATION);
  TestResourceReadySignalFired(control2, true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  tet_printf("Unparent and reload again. Check rasterization requested again.\n");
  control1.Unparent();
  control2.Unparent();

  // Ensure to remove the cache.
  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();

  // Scene on again. Rasterization should be requested again for visual1.
  application.GetScene().Add(control1);
  // rasterize will not requested here.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}