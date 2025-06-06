/*
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

#include <stdlib.h>
#include <iostream>

#include <toolkit-canvas-renderer.h>
#include <toolkit-event-thread-callback.h>
#include <toolkit-timer.h>

#include <dali-toolkit-test-suite-utils.h>

#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/internal/controls/canvas-view/canvas-view-impl.h>
#include <dali-toolkit/internal/controls/canvas-view/canvas-view-rasterize-task.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer/canvas-renderer-shape.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer/canvas-renderer.h>
#include <dali/public-api/adaptor-framework/async-task-manager.h>
#include <dali/public-api/images/pixel-data.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
} // namespace

void utc_dali_toolkit_canvasview_startup(void)
{
  test_return_value = TET_UNDEF;

  // Make sure we clean up test result before start.
  Test::CanvasRenderer::MarkRasterizationResult(true);
  Test::CanvasRenderer::ResetRasterizationFlag();
}

void utc_dali_toolkit_canvasview_cleanup(void)
{
  // Make sure we clean up test result after finish.
  Test::CanvasRenderer::MarkRasterizationResult(true);
  Test::CanvasRenderer::ResetRasterizationFlag();

  test_return_value = TET_PASS;
}

int UtcDaliCanvasViewConstructorP(void)
{
  ToolkitTestApplication application;
  CanvasView             canvasView;

  DALI_TEST_CHECK(!canvasView);
  END_TEST;
}

int UtcDaliCanvasViewMoveAssignment(void)
{
  ToolkitTestApplication application;
  CanvasView             canvasView = CanvasView::New(Vector2(100, 100));

  CanvasView moved = std::move(canvasView);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!canvasView);

  END_TEST;
}

int UtcDaliCanvasViewAssignmentConstructorP(void)
{
  ToolkitTestApplication application;
  CanvasView             canvasView = CanvasView::New(Vector2(100, 100));

  CanvasView copy(canvasView);
  DALI_TEST_CHECK(copy);

  DALI_TEST_CHECK(canvasView == copy);
  END_TEST;
}

int UtcDaliCanvasViewAssignmentOperatorP(void)
{
  ToolkitTestApplication application;
  const CanvasView       canvasView = CanvasView::New(Vector2(100, 100));

  CanvasView assign;
  DALI_TEST_CHECK(!assign);

  assign = canvasView;
  DALI_TEST_CHECK(assign == canvasView);
  END_TEST;
}

int UtcDaliCanvasViewNewP(void)
{
  ToolkitTestApplication application;
  CanvasView             canvasView = CanvasView::New(Vector2(100, 100));

  DALI_TEST_CHECK(canvasView);
  END_TEST;
}

int UtcDaliCanvasViewNewWithoutViewboxP(void)
{
  ToolkitTestApplication application;
  CanvasView             canvasView = CanvasView::New();

  DALI_TEST_CHECK(canvasView);
  END_TEST;
}

int UtcDaliCanvasViewDownCastP(void)
{
  ToolkitTestApplication application;
  CanvasView             canvasView = CanvasView::New(Vector2(100, 100));

  BaseHandle object(canvasView);

  CanvasView canvasView2 = CanvasView::DownCast(object);
  DALI_TEST_CHECK(canvasView2);

  CanvasView canvasView3 = DownCast<CanvasView>(object);
  DALI_TEST_CHECK(canvasView3);
  END_TEST;
}

int UtcDaliCanvasViewDownCastN(void)
{
  ToolkitTestApplication application;
  BaseHandle             unInitializedObject;

  CanvasView canvasView1 = CanvasView::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!canvasView1);

  CanvasView canvasView2 = DownCast<CanvasView>(unInitializedObject);
  DALI_TEST_CHECK(!canvasView2);
  END_TEST;
}

int UtcDaliCanvasViewAddP(void)
{
  ToolkitTestApplication application;
  CanvasView             canvasView = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(canvasView);

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();

  shape.AddRect(Rect<float>(10, 10, 10, 10), Vector2(0, 0));

  canvasView.AddDrawable(shape);

  END_TEST;
}

int UtcDaliCanvasViewAddN(void)
{
  ToolkitTestApplication application;
  CanvasView             canvasView = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(canvasView);

  Dali::CanvasRenderer::Shape shape;

  canvasView.AddDrawable(shape);

  END_TEST;
}

int UtcDaliCanvasViewRemoveP(void)
{
  ToolkitTestApplication application;
  CanvasView             canvasView = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(canvasView);

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();

  canvasView.AddDrawable(shape);

  DALI_TEST_CHECK(canvasView.RemoveDrawable(shape));

  END_TEST;
}

int UtcDaliCanvasViewRemoveN(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(canvasView);

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();

  DALI_TEST_CHECK(!canvasView.RemoveDrawable(shape));

  END_TEST;
}

int UtcDaliCanvasViewRemoveAllP(void)
{
  ToolkitTestApplication application;
  CanvasView             canvasView = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(canvasView);

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();

  canvasView.AddDrawable(shape);

  canvasView.RemoveAllDrawables();

  END_TEST;
}

int UtcDaliCanvasViewRemoveAllN(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(canvasView);

  canvasView.RemoveAllDrawables();

  END_TEST;
}

int UtcDaliCanvasViewChangeSizeP(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(canvasView);

  application.GetScene().Add(canvasView);

  canvasView.SetProperty(Actor::Property::SIZE, Vector2(300, 300));

  application.SendNotification();
  application.Render();

  Property::Value pv = canvasView.GetProperty(Actor::Property::SIZE);
  Vector3         v3;
  pv.Get(v3);
  DALI_TEST_EQUALS(v3, Vector3(300, 300, 0), TEST_LOCATION);

  END_TEST;
}

int UtcDaliCanvasViewSizeN(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(canvasView);

  application.GetScene().Add(canvasView);

  canvasView.SetProperty(Actor::Property::SIZE, Vector2(-999, -999));

  application.SendNotification();
  application.Render();

  Property::Value pv = canvasView.GetProperty(Actor::Property::SIZE);
  Vector3         v3;
  pv.Get(v3);
  DALI_TEST_EQUALS(v3, Vector3(-999, -999, 0), TEST_LOCATION);

  END_TEST;
}

namespace
{
bool gRasterizationCompletedSignal = false;
void rasteriztionCompleted(IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task)
{
  gRasterizationCompletedSignal = true;
}
} // namespace

int UtcDaliCanvasViewRasterizeTaskP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::Internal::CanvasView* dummyInternalCanvasView = new Dali::Toolkit::Internal::CanvasView(Vector2(100, 100));
  DALI_TEST_CHECK(dummyInternalCanvasView);

  Dali::CanvasRenderer dummyCanvasRenderer = Dali::CanvasRenderer::New(Vector2(100, 100));
  DALI_TEST_CHECK(dummyCanvasRenderer);

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyCanvasRenderer, MakeCallback(rasteriztionCompleted));
  DALI_TEST_CHECK(task);

  END_TEST;
}

int UtcDaliCanvasViewRasterizeTaskAddTaskP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::Internal::CanvasView* dummyInternalCanvasView = new Dali::Toolkit::Internal::CanvasView(Vector2(100, 100));
  DALI_TEST_CHECK(dummyInternalCanvasView);

  Dali::CanvasRenderer dummyCanvasRenderer = Dali::CanvasRenderer::New(Vector2(100, 100));
  DALI_TEST_CHECK(dummyCanvasRenderer);

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyCanvasRenderer, MakeCallback(rasteriztionCompleted));
  DALI_TEST_CHECK(task);

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task2 = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyCanvasRenderer, MakeCallback(rasteriztionCompleted));
  DALI_TEST_CHECK(task2);

  Dali::AsyncTaskManager asyncTaskManager = Dali::AsyncTaskManager::Get();
  DALI_TEST_CHECK(asyncTaskManager);

  asyncTaskManager.AddTask(task);
  asyncTaskManager.AddTask(task2);

  END_TEST;
}

int UtcDaliCanvasViewRasterizeTaskAddRemoveTaskP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::Internal::CanvasView* dummyInternalCanvasView = new Dali::Toolkit::Internal::CanvasView(Vector2(100, 100));
  DALI_TEST_CHECK(dummyInternalCanvasView);

  Dali::CanvasRenderer dummyCanvasRenderer = Dali::CanvasRenderer::New(Vector2(100, 100));
  DALI_TEST_CHECK(dummyCanvasRenderer);

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyCanvasRenderer, MakeCallback(rasteriztionCompleted));
  DALI_TEST_CHECK(task);

  Dali::AsyncTaskManager asyncTaskManager = Dali::AsyncTaskManager::Get();
  DALI_TEST_CHECK(asyncTaskManager);

  asyncTaskManager.AddTask(task);

  asyncTaskManager.RemoveTask(task);

  END_TEST;
}

int UtcDaliCanvasViewRasterizeThreadRasterizationCompletedSignalP(void)
{
  ToolkitTestApplication application;

  gRasterizationCompletedSignal = false;

  Dali::Toolkit::CanvasView canvasView = Dali::Toolkit::CanvasView::New(Vector2(100, 100));

  Dali::CanvasRenderer dummyCanvasRenderer = Dali::CanvasRenderer::New(Vector2(100, 100));
  DALI_TEST_CHECK(dummyCanvasRenderer);

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyCanvasRenderer, MakeCallback(rasteriztionCompleted));
  DALI_TEST_CHECK(task);

  Dali::AsyncTaskManager asyncTaskManager = Dali::AsyncTaskManager::Get();
  DALI_TEST_CHECK(asyncTaskManager);

  asyncTaskManager.AddTask(task);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gRasterizationCompletedSignal, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCanvasViewSetSizeAndAddDrawable(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(canvasView);

  application.GetScene().Add(canvasView);

  canvasView.SetProperty(Actor::Property::SIZE, Vector2(300, 300));

  application.SendNotification();
  application.Render();

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();

  shape.AddRect(Rect<float>(10, 10, 10, 10), Vector2(0, 0));

  canvasView.AddDrawable(shape);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliCanvasViewSetSizeAndAddDrawableAsync(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(canvasView);

  application.GetScene().Add(canvasView);

  canvasView.SetProperty(Toolkit::CanvasView::Property::SYNCHRONOUS_LOADING, false);
  canvasView.SetProperty(Actor::Property::SIZE, Vector2(300, 300));

  application.SendNotification();
  application.Render();

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();

  shape.AddRect(Rect<float>(10, 10, 10, 10), Vector2(0, 0));

  canvasView.AddDrawable(shape);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliCanvasViewViewBoxP(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(300, 300));
  DALI_TEST_CHECK(canvasView);

  application.GetScene().Add(canvasView);

  canvasView.SetProperty(Actor::Property::SIZE, Vector2(300, 300));
  canvasView.SetProperty(Toolkit::CanvasView::Property::VIEW_BOX, Vector2(100, 100));

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliCanvasViewViewBoxN(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(300, 300));
  DALI_TEST_CHECK(canvasView);

  application.GetScene().Add(canvasView);

  canvasView.SetProperty(Actor::Property::SIZE, Vector2(300, 300));
  canvasView.SetProperty(Toolkit::CanvasView::Property::VIEW_BOX, Vector2(-999, -999));

  application.SendNotification();
  application.Render();

  Vector2 viewBox = canvasView.GetProperty(Toolkit::CanvasView::Property::VIEW_BOX).Get<Vector2>();
  DALI_TEST_EQUALS(viewBox, Vector2(-999, -999), TEST_LOCATION);

  END_TEST;
}

int UtcDaliCanvasViewSychronousLoading(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(300, 300));
  DALI_TEST_CHECK(canvasView);

  application.GetScene().Add(canvasView);

  canvasView.SetProperty(Actor::Property::SIZE, Vector2(300, 300));

  application.SendNotification();
  application.Render();

  bool isSynchronous = canvasView.GetProperty(Toolkit::CanvasView::Property::SYNCHRONOUS_LOADING).Get<bool>();
  DALI_TEST_EQUALS(isSynchronous, true, TEST_LOCATION);

  canvasView.SetProperty(Toolkit::CanvasView::Property::SYNCHRONOUS_LOADING, false);

  application.SendNotification();
  application.Render();

  isSynchronous = canvasView.GetProperty(Toolkit::CanvasView::Property::SYNCHRONOUS_LOADING).Get<bool>();
  DALI_TEST_EQUALS(isSynchronous, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCanvasViewRasterizationRequestManually(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(300, 300));
  DALI_TEST_CHECK(canvasView);

  Test::CanvasRenderer::ResetRasterizationFlag();

  application.GetScene().Add(canvasView);

  canvasView.SetProperty(Actor::Property::SIZE, Vector2(300, 300));
  canvasView.SetProperty(Toolkit::CanvasView::Property::SYNCHRONOUS_LOADING, true);

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();

  shape.AddRect(Rect<float>(10, 10, 10, 10), Vector2(0, 0));

  canvasView.AddDrawable(shape);

  application.SendNotification();
  application.Render();

  // Rasterization occured
  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), true, TEST_LOCATION);
  Test::CanvasRenderer::ResetRasterizationFlag();

  bool isRasterizationManually = canvasView.GetProperty(Toolkit::CanvasView::Property::RASTERIZATION_REQUEST_MANUALLY).Get<bool>();
  DALI_TEST_EQUALS(isRasterizationManually, false, TEST_LOCATION);

  shape.AddRect(Rect<float>(10, 10, 10, 10), Vector2(0, 0));
  application.SendNotification();
  application.Render();

  shape.AddRect(Rect<float>(10, 10, 10, 10), Vector2(0, 0));
  application.SendNotification();
  application.Render();

  // Check if the canvasView is rasterized.
  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), true, TEST_LOCATION);
  Test::CanvasRenderer::ResetRasterizationFlag();

  canvasView.SetProperty(Toolkit::CanvasView::Property::RASTERIZATION_REQUEST_MANUALLY, true);

  application.SendNotification();
  application.Render();

  // Rasterization occured
  // (Note that we cannot 'cancel' the latest rasterization request even if we set RASTERIZATION_REQUEST_MANUALLY to true)
  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), true, TEST_LOCATION);
  Test::CanvasRenderer::ResetRasterizationFlag();

  application.SendNotification();
  application.Render();

  isRasterizationManually = canvasView.GetProperty(Toolkit::CanvasView::Property::RASTERIZATION_REQUEST_MANUALLY).Get<bool>();
  DALI_TEST_EQUALS(isRasterizationManually, true, TEST_LOCATION);

  shape.AddRect(Rect<float>(10, 10, 10, 10), Vector2(0, 0));
  application.SendNotification();
  application.Render();

  // Check if the canvasView is not rasterized.
  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), false, TEST_LOCATION);
  Test::CanvasRenderer::ResetRasterizationFlag();

  Dali::CanvasRenderer::Shape shape2 = Dali::CanvasRenderer::Shape::New();

  shape2.AddRect(Rect<float>(10, 10, 10, 10), Vector2(0, 0));

  canvasView.AddDrawable(shape2);

  application.SendNotification();
  application.Render();

  // Check whether the canvasView is not rasterized even if we add drawables.
  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), false, TEST_LOCATION);
  Test::CanvasRenderer::ResetRasterizationFlag();

  // Request rasterize manually
  canvasView.RequestRasterization();

  application.SendNotification();
  application.Render();

  // Check if the canvasView is rasterized.
  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), true, TEST_LOCATION);
  Test::CanvasRenderer::ResetRasterizationFlag();

  END_TEST;
}

int UtcDaliCanvasViewRasterizationRequestIfRasterizeFailed01(void)
{
  tet_infoline("Test rasterization failed case for synchronous loading mode.");
  ToolkitTestApplication application;

  Test::CanvasRenderer::ResetRasterizationFlag();

  CanvasView canvasView = CanvasView::New(Vector2(300, 300));
  DALI_TEST_CHECK(canvasView);

  application.GetScene().Add(canvasView);

  canvasView.SetProperty(Actor::Property::SIZE, Vector2(300, 300));
  canvasView.SetProperty(Toolkit::CanvasView::Property::SYNCHRONOUS_LOADING, true);
  canvasView.SetProperty(Toolkit::CanvasView::Property::RASTERIZATION_REQUEST_MANUALLY, true);

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();

  shape.AddRect(Rect<float>(10, 10, 10, 10), Vector2(0, 0));

  canvasView.AddDrawable(shape);

  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Rasterization occured
  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), true, TEST_LOCATION);
  Test::CanvasRenderer::ResetRasterizationFlag();

  application.SendNotification();
  application.Render();

  // Check whether the canvasView is not rasterized.
  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), false, TEST_LOCATION);

  // Make rasterization failed.
  Test::CanvasRenderer::MarkRasterizationResult(false);

  shape.AddRect(Rect<float>(10, 10, 10, 10), Vector2(0, 0));

  canvasView.AddDrawable(shape);

  // Request rasterize manually
  canvasView.RequestRasterization();

  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Check if the canvasView is rasterized.
  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), true, TEST_LOCATION);
  Test::CanvasRenderer::ResetRasterizationFlag();

  application.SendNotification();
  application.Render();

  // Check if the canvasView is rasterized again automatically.
  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), true, TEST_LOCATION);
  Test::CanvasRenderer::ResetRasterizationFlag();

  application.SendNotification();
  application.Render();

  // Check if the canvasView is rasterized again automatically.
  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), true, TEST_LOCATION);
  Test::CanvasRenderer::ResetRasterizationFlag();

  // Make rasterization success.
  Test::CanvasRenderer::MarkRasterizationResult(true);

  application.SendNotification();
  application.Render();

  // Check if the canvasView is rasterized.
  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), true, TEST_LOCATION);
  Test::CanvasRenderer::ResetRasterizationFlag();

  application.SendNotification();
  application.Render();

  // Check whether the canvasView is not rasterized again.
  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCanvasViewRasterizationRequestIfRasterizeFailed02(void)
{
  tet_infoline("Test rasterization failed case for async loading mode.");
  ToolkitTestApplication application;

  Test::CanvasRenderer::ResetRasterizationFlag();

  CanvasView canvasView = CanvasView::New(Vector2(300, 300));
  DALI_TEST_CHECK(canvasView);

  application.GetScene().Add(canvasView);

  canvasView.SetProperty(Actor::Property::SIZE, Vector2(300, 300));
  canvasView.SetProperty(Toolkit::CanvasView::Property::SYNCHRONOUS_LOADING, false);
  canvasView.SetProperty(Toolkit::CanvasView::Property::RASTERIZATION_REQUEST_MANUALLY, true);

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();

  shape.AddRect(Rect<float>(10, 10, 10, 10), Vector2(0, 0));

  canvasView.AddDrawable(shape);

  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Rasterization occured
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), true, TEST_LOCATION);
  Test::CanvasRenderer::ReduceRasterizationFlagCount();

  application.SendNotification();
  application.Render();

  // Check whether the canvasView is not rasterized.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), false, TEST_LOCATION);

  // Make rasterization failed.
  Test::CanvasRenderer::MarkRasterizationResult(false);

  shape.AddRect(Rect<float>(10, 10, 10, 10), Vector2(0, 0));

  canvasView.AddDrawable(shape);

  // Request rasterize manually
  canvasView.RequestRasterization();

  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Check if the canvasView is rasterized.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), true, TEST_LOCATION);
  Test::CanvasRenderer::ReduceRasterizationFlagCount();

  application.SendNotification();
  application.Render();

  // Check if the canvasView is rasterized again automatically.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), true, TEST_LOCATION);
  Test::CanvasRenderer::ReduceRasterizationFlagCount();

  application.SendNotification();
  application.Render();

  // Check if the canvasView is rasterized again automatically.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), true, TEST_LOCATION);
  Test::CanvasRenderer::ReduceRasterizationFlagCount();

  // Make rasterization success.
  Test::CanvasRenderer::MarkRasterizationResult(true);

  application.SendNotification();
  application.Render();

  // Check if the canvasView is rasterized.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), true, TEST_LOCATION);
  Test::CanvasRenderer::ReduceRasterizationFlagCount();

  application.SendNotification();
  application.Render();

  // Check whether the canvasView is not rasterized again.
  // Note that it could be trigger if latest rasterize callback finished before we mark result as true.
  // So we have to consume result one time.
  if(Test::WaitForEventThreadTrigger(1, 0))
  {
    Test::CanvasRenderer::ReduceRasterizationFlagCount();
  }
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 0), false, TEST_LOCATION);

  DALI_TEST_EQUALS(Test::CanvasRenderer::IsRasterizationCalled(), false, TEST_LOCATION);

  END_TEST;
}
