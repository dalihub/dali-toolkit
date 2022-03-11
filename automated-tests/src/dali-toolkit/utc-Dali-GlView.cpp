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
int UtcDaliGlViewNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliGlViewNew");
  GlView view = GlView::New(GlView::ColorFormat::RGBA8888);
  DALI_TEST_CHECK(view);
  END_TEST;
}

// Positive test case for a method
int UtcDaliGlViewDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliGlViewDownCast");

  GlView     view = GlView::New(GlView::ColorFormat::RGB888);
  BaseHandle handle(view);

  Toolkit::GlView view2 = Toolkit::GlView::DownCast(handle);
  DALI_TEST_CHECK(view);
  DALI_TEST_CHECK(view2);
  DALI_TEST_CHECK(view == view2);
  END_TEST;
}

int UtcDaliGlViewCopyAndAssignment(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewCopyAndAssignment");

  GlView view = Toolkit::GlView::New(GlView::ColorFormat::RGB888);
  DALI_TEST_CHECK(view);

  GlView copy(view);
  DALI_TEST_CHECK(view == copy);

  GlView assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == view);

  END_TEST;
}

int UtcDaliGlViewMoveAssignment(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewMoveAssignment");

  GlView view = Toolkit::GlView::New(GlView::ColorFormat::RGB888);
  DALI_TEST_EQUALS(1, view.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  GlView moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!view);

  END_TEST;
}

int UtcDaliGlViewSetGraphicsConfigGles20N(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewSetGraphicsConfigGles20");
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

int UtcDaliGlViewSetGraphicsConfigGles30(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewSetGraphicsConfigGles30");
  GlView view = Toolkit::GlView::New(GlView::ColorFormat::RGB888);

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

int UtcDaliGlViewRenderingMode(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewRenderingMode");
  GlView view = Toolkit::GlView::New(GlView::ColorFormat::RGB888);

  view.SetRenderingMode(GlView::RenderingMode::ON_DEMAND);

  GlView::RenderingMode mode = view.GetRenderingMode();

  DALI_TEST_EQUALS(GlView::RenderingMode::ON_DEMAND, mode, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGlViewOnSizeSet(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewOnSizeSet");
  GlView view = Toolkit::GlView::New(GlView::ColorFormat::RGB888);

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

int UtcDaliGlViewRegisterGlCallbacksN(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewRegisterGlCallbacksN");
  GlView view;

  try
  {
    view.RegisterGlCallbacks(Dali::MakeCallback(glInit), Dali::MakeCallback(glRenderFrame), Dali::MakeCallback(glTerminate));
    DALI_TEST_CHECK(false);
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }
  END_TEST;
}

int UtcDaliGlViewSetResizeCallbackN(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewSetResizeCallback");
  GlView view;

  try
  {
    view.SetResizeCallback(Dali::MakeCallback(resizeCB));
    DALI_TEST_CHECK(false);
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }
  END_TEST;
}

int UtcDaliGlViewRenderOnce(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewRenderOnce");
  GlView view = Toolkit::GlView::New(GlView::ColorFormat::RGB888);

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

int UtcDaliGlViewWindowVisibilityChanged(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewWindowVisibilityChanged");
  GlView view = Toolkit::GlView::New(GlView::ColorFormat::RGB888);
  application.GetScene().Add(view);
  view.SetRenderingMode(GlView::RenderingMode::CONTINUOUS);
  view.SetGraphicsConfig(true, true, 0, GlView::GraphicsApiVersion::GLES_VERSION_2_0);
  view.RegisterGlCallbacks(Dali::MakeCallback(glInit), Dali::MakeCallback(glRenderFrame), Dali::MakeCallback(glTerminate));
  view.SetResizeCallback(Dali::MakeCallback(resizeCB));

  application.SendNotification();
  application.Render();

  Window window = DevelWindow::Get(view);
  window.Hide();
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliGlViewOnScene(void)
{
  ToolkitTestApplication application;

  GlView view = Toolkit::GlView::New(GlView::ColorFormat::RGB888);

  //Onscene
  application.GetScene().Add(view);
  view.SetRenderingMode(GlView::RenderingMode::CONTINUOUS);
  view.SetGraphicsConfig(true, true, 0, GlView::GraphicsApiVersion::GLES_VERSION_2_0);
  view.RegisterGlCallbacks(Dali::MakeCallback(glInit), Dali::MakeCallback(glRenderFrame), Dali::MakeCallback(glTerminate));

  application.SendNotification();
  application.Render();

  //Offscene
  application.GetScene().Remove(view);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliGlViewControlVisibilityChanged(void)
{
  ToolkitTestApplication application;

  GlView view = Toolkit::GlView::New(GlView::ColorFormat::RGB888);
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

int UtcDaliGlViewResize(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGlViewResize");
  GlView view = Toolkit::GlView::New(GlView::ColorFormat::RGB888);

  application.GetScene().Add(view);
  view.SetGraphicsConfig(true, true, 0, GlView::GraphicsApiVersion::GLES_VERSION_2_0);
  view.RegisterGlCallbacks(Dali::MakeCallback(glInit), Dali::MakeCallback(glRenderFrame), Dali::MakeCallback(glTerminate));
  view.SetResizeCallback(Dali::MakeCallback(resizeCB));
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
