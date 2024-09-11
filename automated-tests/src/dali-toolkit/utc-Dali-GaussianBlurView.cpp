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
 *
 */

#include <stdlib.h>
#include <iostream>

#include <dali-toolkit-test-suite-utils.h>

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/gaussian-blur-view/gaussian-blur-view.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* TEST_IMAGE_FILE_NAME = TEST_RESOURCE_DIR "/gallery-small-1.jpg";
} // namespace

void utc_gaussian_blur_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_gaussian_blur_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

class TestCallback : public ConnectionTracker
{
public:
  TestCallback(Dali::Toolkit::GaussianBlurView& blurView)
  : mBlurView(blurView)
  {
    mFinished = false;
  }

  void Connect()
  {
    mBlurView.FinishedSignal().Connect(this, &TestCallback::OnFinished);
  }

  void OnFinished(Dali::Toolkit::GaussianBlurView source)
  {
    mFinished = true;
  }

  bool                             mFinished;
  Dali::Toolkit::GaussianBlurView& mBlurView;
};

// Negative test case for a method
int UtcDaliGaussianBlurViewUninitialized(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewUninitialized");

  Toolkit::GaussianBlurView view;

  try
  {
    // New() must be called to create a GaussianBlurView or it wont be valid.
    Actor a = Actor::New();
    view.Add(a);
    DALI_TEST_CHECK(false);
  }
  catch(Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_CHECK(!view);
  }
  END_TEST;
}

// Positive test case for a method
int UtcDaliGaussianBlurViewNew(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewNew");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  DALI_TEST_CHECK(view);

  Toolkit::GaussianBlurView view2 = Toolkit::GaussianBlurView::New(5, 1.5f, Pixel::RGB888, 0.5f, 0.5f, false);
  DALI_TEST_CHECK(view2);

  // Don't assert even if we give big number.
  Toolkit::GaussianBlurView view3 = Toolkit::GaussianBlurView::New(2147483647, 1.5f, Pixel::RGB888, 0.5f, 0.5f, false);
  DALI_TEST_CHECK(view3);
  END_TEST;
}

// Positive test case for a method
int UtcDaliGaussianBlurViewDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewDownCast");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  BaseHandle                handle(view);

  Toolkit::GaussianBlurView gaussianBlurView = Toolkit::GaussianBlurView::DownCast(handle);
  DALI_TEST_CHECK(view);
  DALI_TEST_CHECK(gaussianBlurView);
  DALI_TEST_CHECK(gaussianBlurView == view);
  END_TEST;
}

// Positive test case for a method
int UtcDaliGaussianBlurViewPropertyNames(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewPropertyNames");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  DALI_TEST_CHECK(view);

  // Check the names, this names are used in the shader code,
  // if they change in the shader code, then it has to be updated here.
  DALI_TEST_EQUALS(view.GetBlurStrengthPropertyIndex(), view.GetPropertyIndex("GaussianBlurStrengthPropertyName"), TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliGaussianBlurViewAddRemove(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewAddRemove");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  DALI_TEST_CHECK(view);

  Actor actor = Actor::New();
  DALI_TEST_CHECK(!actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  view.SetProperty(Actor::Property::SIZE, application.GetScene().GetSize());
  view.Add(actor);
  application.GetScene().Add(view);

  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  view.Remove(actor);

  DALI_TEST_CHECK(!actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));
  END_TEST;
}

// Positive test case for a method
int UtcDaliGaussianBlurActivateDeactivate(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurActivateDeactivate");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  DALI_TEST_CHECK(view);

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  DALI_TEST_CHECK(1u == taskList.GetTaskCount());

  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  view.SetProperty(Actor::Property::SIZE, application.GetScene().GetSize());
  view.Add(Actor::New());
  application.GetScene().Add(view);
  view.Activate();

  RenderTaskList taskList2 = application.GetScene().GetRenderTaskList();
  DALI_TEST_CHECK(1u != taskList2.GetTaskCount());
  DALI_TEST_CHECK(2u == view.GetChildCount());

  view.Deactivate();

  RenderTaskList taskList3 = application.GetScene().GetRenderTaskList();
  DALI_TEST_CHECK(1u == taskList3.GetTaskCount());
  DALI_TEST_CHECK(1u == view.GetChildCount());

  END_TEST;
}

// Positive test case for a method
int UtcDaliGaussianBlurActivateDeactivateRepeat(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction&     gl           = application.GetGlAbstraction();
  TraceCallStack&        textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  tet_infoline("UtcDaliGaussianBlurActivateDeactivateRepeat");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  DALI_TEST_CHECK(view);

  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  view.SetProperty(Actor::Property::SIZE, application.GetScene().GetSize());
  view.Add(Actor::New());
  application.GetScene().Add(view);
  view.Activate();

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(gl.GetLastGenTextureId(), 4, TEST_LOCATION);

  view.Deactivate();

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(gl.GetLastGenTextureId(), 4, TEST_LOCATION);

  view.Activate();

  application.SendNotification();
  application.Render(20);

  DALI_TEST_EQUALS(gl.GetLastGenTextureId(), 8, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliGaussianBlurViewSetGetBackgroundColor(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewSetGetBackgroundColor");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  DALI_TEST_CHECK(view);

  view.SetBackgroundColor(Dali::Color::RED);
  Vector4 color = view.GetBackgroundColor();
  DALI_TEST_CHECK(color == Dali::Color::RED);
  END_TEST;
}

// Positive test case for a method
int UtcDaliGaussianBlurViewSetGetRenderTarget(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewSetGetRenderTarget");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New(5, 1.5f, Pixel::RGB888, 0.5f, 0.5f, true);
  DALI_TEST_CHECK(view);

  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  view.SetProperty(Actor::Property::SIZE, application.GetScene().GetSize());
  view.Add(Actor::New());
  application.GetScene().Add(view);
  view.Activate();

  PixelData pixels  = Toolkit::SyncImageLoader::Load(TEST_IMAGE_FILE_NAME);
  Texture   texture = Texture::New(TextureType::TEXTURE_2D, pixels.GetPixelFormat(), pixels.GetWidth(), pixels.GetHeight());
  texture.Upload(pixels, 0, 0, 0, 0, pixels.GetWidth(), pixels.GetHeight());

  FrameBuffer renderTarget = FrameBuffer::New(480, 800, FrameBuffer::Attachment::NONE);
  view.SetUserImageAndOutputRenderTarget(texture, renderTarget);
  DALI_TEST_CHECK(view.GetBlurredRenderTarget() == renderTarget);
  END_TEST;
}

int UtcDaliGaussianBlurViewActivateOnce1(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurActivateOnce1");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New(5, 1.5f, Pixel::RGB888, 0.5f, 0.5f, true);
  DALI_TEST_CHECK(view);

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  DALI_TEST_CHECK(1u == taskList.GetTaskCount());

  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  view.SetProperty(Actor::Property::SIZE, application.GetScene().GetSize());
  view.Add(Actor::New());
  application.GetScene().Add(view);
  view.ActivateOnce();

  RenderTaskList taskList2 = application.GetScene().GetRenderTaskList();
  DALI_TEST_CHECK(1u != taskList2.GetTaskCount());
  application.Render();

  END_TEST;
}

// Positive test case for a method
int UtcDaliGaussianBlurActivateOnce2(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction&     gl           = application.GetGlAbstraction();
  TraceCallStack&        textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  tet_infoline("UtcDaliGaussianBlurActivateOnce2");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  DALI_TEST_CHECK(view);

  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  view.SetProperty(Actor::Property::SIZE, application.GetScene().GetSize());
  view.Add(Actor::New());
  application.GetScene().Add(view);
  view.ActivateOnce();

  application.SendNotification();
  application.Render(20);

  DALI_TEST_CHECK(gl.GetLastGenTextureId() == 4);

  view.ActivateOnce();

  application.SendNotification();
  application.Render(20);

  DALI_TEST_CHECK(gl.GetLastGenTextureId() == 8);

  END_TEST;
}

int UtcDaliGaussianBlurViewFinishedSignalN(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewFinishedSignalN");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New(5, 1.5f, Pixel::RGB888, 0.5f, 0.5f, true);
  DALI_TEST_CHECK(view);

  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  view.SetProperty(Actor::Property::SIZE, application.GetScene().GetSize());
  view.Add(Actor::New());
  application.GetScene().Add(view);
  view.Activate();

  TestCallback callback(view);
  DALI_TEST_CHECK(callback.mFinished == false);

  callback.Connect();

  view.Deactivate();
  application.SendNotification();

  // FinishedSignal is only for ActivateOnce()
  DALI_TEST_CHECK(callback.mFinished == false);

  END_TEST;
}
