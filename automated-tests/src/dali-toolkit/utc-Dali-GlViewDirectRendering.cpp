/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <unistd.h>
#include <thread>

#include <dali-toolkit-test-suite-utils.h>

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/public-api/controls/gl-view/gl-view.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;

// Positive test case for a method
int UtcDaliGlViewDirectRenderingNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliGlViewDirectRenderingNew");
  GlView view = GlView::New(GlView::BackendMode::DIRECT_RENDERING, GlView::ColorFormat::RGBA8888);
  DALI_TEST_CHECK(view);

  auto mode1 = view.GetBackendMode();

  DALI_TEST_EQUALS(mode1, GlView::BackendMode::DIRECT_RENDERING, TEST_LOCATION);

  GlView view2 = GlView::New(GlView::BackendMode::EGL_IMAGE_OFFSCREEN_RENDERING, GlView::ColorFormat::RGBA8888);
  DALI_TEST_CHECK(view2);

  auto mode2 = view2.GetBackendMode();
  DALI_TEST_EQUALS(mode2, GlView::BackendMode::EGL_IMAGE_OFFSCREEN_RENDERING, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewDirectRenderingNewN(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliGlViewDirectRenderingNewN");
  // Invalid backend mode
  GlView view = GlView::New(GlView::BackendMode(11111), GlView::ColorFormat::RGBA8888);
  DALI_TEST_CHECK(!view);

  END_TEST;
}

// Positive test case for a method
int UtcDaliGlViewDirectRenderingDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliGlViewDirectRenderingDownCast");

  GlView     view = GlView::New(GlView::BackendMode::DIRECT_RENDERING, GlView::ColorFormat::RGB888);
  BaseHandle handle(view);

  Toolkit::GlView view2 = Toolkit::GlView::DownCast(handle);
  DALI_TEST_CHECK(view);
  DALI_TEST_CHECK(view2);
  DALI_TEST_CHECK(view == view2);
  END_TEST;
}

int UtcDaliGlViewDirectRenderingCopyAndAssignment(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewDirectRenderingCopyAndAssignment");

  GlView view = Toolkit::GlView::New(GlView::BackendMode::DIRECT_RENDERING, GlView::ColorFormat::RGB888);
  DALI_TEST_CHECK(view);

  GlView copy(view);
  DALI_TEST_CHECK(view == copy);

  GlView assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == view);

  END_TEST;
}

int UtcDaliGlViewDirectRenderingMoveAssignment(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewDirectRenderingMoveAssignment");

  GlView view = Toolkit::GlView::New(GlView::BackendMode::DIRECT_RENDERING, GlView::ColorFormat::RGB888);
  DALI_TEST_EQUALS(1, view.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  GlView moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!view);

  END_TEST;
}

int UtcDaliGlViewDirectRenderingSetGraphicsConfigGles20N(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewDirectRenderingSetGraphicsConfigGles20");
  GlView view;
  try
  {
    view.SetGraphicsConfig(true, true, 0, GlView::GraphicsApiVersion::GLES_VERSION_2_0);
    DALI_TEST_CHECK(false);
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }
  END_TEST;
}

int UtcDaliGlViewDirectRenderingSetGraphicsConfigGles30(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewDirectRenderingSetGraphicsConfigGles30");
  GlView view = Toolkit::GlView::New(GlView::BackendMode::DIRECT_RENDERING, GlView::ColorFormat::RGB888);

  try
  {
    view.SetGraphicsConfig(true, true, 0, GlView::GraphicsApiVersion::GLES_VERSION_3_0);
    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false);
  }
  END_TEST;
}

int UtcDaliGlViewDirectRenderingRenderingMode(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewDirectRenderingRenderingMode");
  GlView view = Toolkit::GlView::New(GlView::BackendMode::DIRECT_RENDERING, GlView::ColorFormat::RGB888);

  view.SetRenderingMode(GlView::RenderingMode::ON_DEMAND);

  GlView::RenderingMode mode = view.GetRenderingMode();

  DALI_TEST_EQUALS(GlView::RenderingMode::ON_DEMAND, mode, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewDirectRenderingOnSizeSet(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewDirectRenderingOnSizeSet");
  GlView view = Toolkit::GlView::New(GlView::BackendMode::DIRECT_RENDERING, GlView::ColorFormat::RGB888);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  Vector3 size(200.0f, 300.0f, 0.0f);
  view.SetProperty(Actor::Property::SIZE, size);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(view.GetCurrentProperty<Vector3>(Actor::Property::SIZE), size, TEST_LOCATION);

  END_TEST;
}

namespace DirectRenderingCode
{

// Internal callback function
void glInit(void)
{
}

int glRenderFrame(void)
{
  static unsigned int retFlag = 0;
  return retFlag++;
}

void glTerminate(void)
{
}

void resizeCB(Vector2 size)
{
}

}

int UtcDaliGlViewDirectRenderingRegisterGlCallbacksN(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewDirectRenderingRegisterGlCallbacksN");
  GlView view;

  try
  {
    view.RegisterGlCallbacks(Dali::MakeCallback(DirectRenderingCode::glInit), Dali::MakeCallback(DirectRenderingCode::glRenderFrame), Dali::MakeCallback(DirectRenderingCode::glTerminate));
    DALI_TEST_CHECK(false);
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }
  END_TEST;
}

int UtcDaliGlViewDirectRenderingSetResizeCallbackN(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewDirectRenderingSetResizeCallback");
  GlView view;

  try
  {
    view.SetResizeCallback(Dali::MakeCallback(DirectRenderingCode::resizeCB));
    DALI_TEST_CHECK(false);
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }
  END_TEST;
}

int UtcDaliGlViewDirectRenderingRenderOnce(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewDirectRenderingRenderOnce");
  GlView view = Toolkit::GlView::New(GlView::BackendMode::DIRECT_RENDERING, GlView::ColorFormat::RGB888);

  try
  {
    view.RenderOnce();
    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false);
  }
  END_TEST;
}

int UtcDaliGlViewDirectRenderingWindowVisibilityChanged(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewDirectRenderingWindowVisibilityChanged");
  GlView view = Toolkit::GlView::New(GlView::BackendMode::DIRECT_RENDERING, GlView::ColorFormat::RGB888);
  application.GetScene().Add(view);
  view.SetRenderingMode(GlView::RenderingMode::CONTINUOUS);
  view.SetGraphicsConfig(true, true, 0, GlView::GraphicsApiVersion::GLES_VERSION_2_0);
  view.RegisterGlCallbacks(Dali::MakeCallback(DirectRenderingCode::glInit), Dali::MakeCallback(DirectRenderingCode::glRenderFrame), Dali::MakeCallback(DirectRenderingCode::glTerminate));
  view.SetResizeCallback(Dali::MakeCallback(DirectRenderingCode::resizeCB));

  application.SendNotification();
  application.Render();

  Window window = DevelWindow::Get(view);
  window.Hide();
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliGlViewDirectRenderingOnScene(void)
{
  ToolkitTestApplication application;

  GlView view = Toolkit::GlView::New(GlView::BackendMode::DIRECT_RENDERING, GlView::ColorFormat::RGB888);

  //Onscene
  application.GetScene().Add(view);
  view.SetRenderingMode(GlView::RenderingMode::CONTINUOUS);
  view.SetGraphicsConfig(true, true, 0, GlView::GraphicsApiVersion::GLES_VERSION_2_0);
  view.RegisterGlCallbacks(Dali::MakeCallback(DirectRenderingCode::glInit), Dali::MakeCallback(DirectRenderingCode::glRenderFrame), Dali::MakeCallback(DirectRenderingCode::glTerminate));

  application.SendNotification();
  application.Render();

  //Offscene
  application.GetScene().Remove(view);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliGlViewDirectRenderingControlVisibilityChanged(void)
{
  ToolkitTestApplication application;

  GlView view = Toolkit::GlView::New(GlView::BackendMode::DIRECT_RENDERING, GlView::ColorFormat::RGB888);
  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  view.SetProperty(Actor::Property::VISIBLE, false);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(view.GetCurrentProperty<bool>(Actor::Property::VISIBLE) == false);

  view.SetProperty(Actor::Property::VISIBLE, true);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(view.GetCurrentProperty<bool>(Actor::Property::VISIBLE) == true);

  END_TEST;
}

int UtcDaliGlViewDirectRenderingResize(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewDirectRenderingResize");
  GlView view = Toolkit::GlView::New(GlView::BackendMode::DIRECT_RENDERING, GlView::ColorFormat::RGB888);

  application.GetScene().Add(view);
  view.SetGraphicsConfig(true, true, 0, GlView::GraphicsApiVersion::GLES_VERSION_2_0);
  view.RegisterGlCallbacks(Dali::MakeCallback(DirectRenderingCode::glInit), Dali::MakeCallback(DirectRenderingCode::glRenderFrame), Dali::MakeCallback(DirectRenderingCode::glTerminate));
  view.SetResizeCallback(Dali::MakeCallback(DirectRenderingCode::resizeCB));
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::SIZE, Vector2(360.0f, 360.0f));

  application.SendNotification();
  application.Render();

  //To GlViewRenderThread can recognize Resize signal the main thread have to sleep.
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliGlViewDirectRenderingTerminateCallback(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewDirectRenderingTerminateCallback");
  GlView view = Toolkit::GlView::New(GlView::BackendMode::DIRECT_RENDERING, GlView::ColorFormat::RGB888);

  application.GetScene().Add(view);
  view.SetGraphicsConfig(true, true, 0, GlView::GraphicsApiVersion::GLES_VERSION_2_0);
  view.RegisterGlCallbacks(Dali::MakeCallback(DirectRenderingCode::glInit), Dali::MakeCallback(DirectRenderingCode::glRenderFrame), Dali::MakeCallback(DirectRenderingCode::glTerminate));
  view.SetResizeCallback(Dali::MakeCallback(DirectRenderingCode::resizeCB));
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::SIZE, Vector2(360.0f, 360.0f));

  application.SendNotification();
  application.Render();



  //To GlViewRenderThread can recognize Resize signal the main thread have to sleep.
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(true);
  END_TEST;
}