/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <test-application.h>
#include <dali/public-api/images/pixel-data.h>
#include <toolkit-event-thread-callback.h>
#include <dali-toolkit/devel-api/controls/canvas-view/canvas-view.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer-shape.h>
#include <dali-toolkit/internal/controls/canvas-view/canvas-view-rasterize-thread.h>

using namespace Dali;
using namespace Toolkit;

namespace
{


} // namespace

void utc_dali_toolkit_canvasview_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_canvasview_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliCanvasViewConstructorP(void)
{
  ToolkitTestApplication application;
  CanvasView canvasView;

  DALI_TEST_CHECK( !canvasView );
  END_TEST;
}

int UtcDaliCanvasViewMoveAssignment(void)
{
  ToolkitTestApplication application;
  CanvasView canvasView = CanvasView::New(Vector2(100, 100));

  CanvasView moved = std::move( canvasView );
  DALI_TEST_CHECK( moved );
  DALI_TEST_EQUALS( 1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION );
  DALI_TEST_CHECK( !canvasView );

  END_TEST;
}

int UtcDaliCanvasViewAssignmentConstructorP(void)
{
  ToolkitTestApplication application;
  CanvasView canvasView = CanvasView::New(Vector2(100, 100));

  CanvasView copy( canvasView );
  DALI_TEST_CHECK( copy );

  DALI_TEST_CHECK( canvasView == copy );
  END_TEST;
}

int UtcDaliCanvasViewAssignmentOperatorP(void)
{
  ToolkitTestApplication application;
  const CanvasView canvasView = CanvasView::New(Vector2(100, 100));

  CanvasView assign;
  DALI_TEST_CHECK( !assign );

  assign = canvasView ;
  DALI_TEST_CHECK( assign == canvasView);
  END_TEST;
}

int UtcDaliCanvasViewNewP(void)
{
  ToolkitTestApplication application;
  CanvasView canvasView = CanvasView::New(Vector2(100, 100));

  DALI_TEST_CHECK( canvasView );
  END_TEST;
}

int UtcDaliCanvasViewNewWithoutViewboxP(void)
{
  ToolkitTestApplication application;
  CanvasView canvasView = CanvasView::New();

  DALI_TEST_CHECK( canvasView );
  END_TEST;
}

int UtcDaliCanvasViewDownCastP(void)
{
  ToolkitTestApplication application;
  CanvasView canvasView = CanvasView::New(Vector2(100,100));

  BaseHandle object(canvasView);

  CanvasView canvasView2 = CanvasView::DownCast( object );
  DALI_TEST_CHECK(canvasView2);

  CanvasView canvasView3 = DownCast< CanvasView >(object);
  DALI_TEST_CHECK(canvasView3);
  END_TEST;
}

int UtcDaliCanvasViewDownCastN(void)
{
  ToolkitTestApplication application;
  BaseHandle unInitializedObject;

  CanvasView canvasView1 = CanvasView::DownCast( unInitializedObject );
  DALI_TEST_CHECK( !canvasView1 );

  CanvasView canvasView2 = DownCast< CanvasView >( unInitializedObject );
  DALI_TEST_CHECK( !canvasView2 );
  END_TEST;
}

int UtcDaliCanvasViewAddP(void)
{
  ToolkitTestApplication application;
  CanvasView canvasView = CanvasView::New(Vector2(100,100));
  DALI_TEST_CHECK( canvasView );

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();

  shape.AddRect(Rect<float>(10, 10, 10, 10), Vector2(0, 0));

  canvasView.AddDrawable(shape);

  END_TEST;
}

int UtcDaliCanvasViewAddN(void)
{
  ToolkitTestApplication application;
  CanvasView canvasView = CanvasView::New(Vector2(100,100));
  DALI_TEST_CHECK( canvasView );

  Dali::CanvasRenderer::Shape shape;

  canvasView.AddDrawable(shape);

  END_TEST;
}

int UtcDaliCanvasViewRemoveP(void)
{
  ToolkitTestApplication application;
  CanvasView canvasView = CanvasView::New(Vector2(100,100));
  DALI_TEST_CHECK( canvasView );

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();

  canvasView.AddDrawable(shape);

  DALI_TEST_CHECK( canvasView.RemoveDrawable(shape) );

  END_TEST;
}

int UtcDaliCanvasViewRemoveN(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(100,100));
  DALI_TEST_CHECK( canvasView );

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();

  DALI_TEST_CHECK( !canvasView.RemoveDrawable(shape) );

  END_TEST;
}

int UtcDaliCanvasViewRemoveAllP(void)
{
  ToolkitTestApplication application;
  CanvasView canvasView = CanvasView::New(Vector2(100,100));
  DALI_TEST_CHECK( canvasView );

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();

  canvasView.AddDrawable(shape);

  canvasView.RemoveAllDrawables();

  END_TEST;
}

int UtcDaliCanvasViewRemoveAllN(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(100,100));
  DALI_TEST_CHECK( canvasView );

  canvasView.RemoveAllDrawables();

  END_TEST;
}


int UtcDaliCanvasViewChangeSizeP(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(100,100));
  DALI_TEST_CHECK( canvasView );

  application.GetScene().Add(canvasView);

  canvasView.SetProperty(Actor::Property::SIZE, Vector2(300, 300));

  application.SendNotification();
  application.Render();

  Property::Value pv = canvasView.GetProperty(Actor::Property::SIZE);
  Vector3 v3;
  pv.Get(v3);
  DALI_TEST_EQUALS( v3, Vector3(300, 300, 0), TEST_LOCATION );

  END_TEST;
}

int UtcDaliCanvasViewSizeN(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(100,100));
  DALI_TEST_CHECK( canvasView );

  application.GetScene().Add(canvasView);

  canvasView.SetProperty(Actor::Property::SIZE, Vector2(-999, -999));

  application.SendNotification();
  application.Render();

  Property::Value pv = canvasView.GetProperty(Actor::Property::SIZE);
  Vector3 v3;
  pv.Get(v3);
  DALI_TEST_EQUALS( v3, Vector3(-999, -999, 0), TEST_LOCATION );

  END_TEST;
}

int UtcDaliCanvasViewRasterizeTaskP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::Internal::CanvasView* dummyInternalCanvasView = new Dali::Toolkit::Internal::CanvasView(Vector2(100,100));
  DALI_TEST_CHECK( dummyInternalCanvasView );

  Dali::CanvasRenderer dummyCanvasRenderer = Dali::CanvasRenderer::New(Vector2(100, 100));
  DALI_TEST_CHECK( dummyCanvasRenderer );

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyInternalCanvasView, dummyCanvasRenderer);
  DALI_TEST_CHECK( task );

  END_TEST;
}

int UtcDaliCanvasViewRasterizeTaskGetCanvasViewP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::Internal::CanvasView* dummyInternalCanvasView = new Dali::Toolkit::Internal::CanvasView(Vector2(100,100));
  DALI_TEST_CHECK( dummyInternalCanvasView );

  Dali::CanvasRenderer dummyCanvasRenderer = Dali::CanvasRenderer::New(Vector2(100, 100));
  DALI_TEST_CHECK( dummyCanvasRenderer );

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyInternalCanvasView, dummyCanvasRenderer);
  DALI_TEST_CHECK( task );

  DALI_TEST_EQUALS( task->GetCanvasView(), dummyInternalCanvasView, TEST_LOCATION );

  END_TEST;
}

int UtcDaliCanvasViewRasterizeTaskGetBufferSizeP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::Internal::CanvasView* dummyInternalCanvasView = new Dali::Toolkit::Internal::CanvasView(Vector2(100,100));
  DALI_TEST_CHECK( dummyInternalCanvasView );

  Dali::CanvasRenderer dummyCanvasRenderer = Dali::CanvasRenderer::New(Vector2(100, 100));
  DALI_TEST_CHECK( dummyCanvasRenderer );

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyInternalCanvasView, dummyCanvasRenderer);
  DALI_TEST_CHECK( task );

  //There is no rasterized buffer.
  DALI_TEST_EQUALS( task->GetBufferSize(), Vector2(0, 0), TEST_LOCATION );

  END_TEST;
}

int UtcDaliCanvasViewRasterizeTaskGetPixelDataP(void)
{

  Dali::Toolkit::Internal::CanvasView* dummyInternalCanvasView = new Dali::Toolkit::Internal::CanvasView(Vector2(100,100));
  DALI_TEST_CHECK( dummyInternalCanvasView );

  Dali::CanvasRenderer dummyCanvasRenderer = Dali::CanvasRenderer::New(Vector2(100, 100));
  DALI_TEST_CHECK( dummyCanvasRenderer );

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyInternalCanvasView, dummyCanvasRenderer);
  DALI_TEST_CHECK( task );

  DALI_TEST_EQUALS( task->GetPixelData(), PixelData(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliCanvasViewRasterizeThreadP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::Internal::CanvasView* dummyInternalCanvasView = new Dali::Toolkit::Internal::CanvasView(Vector2(100,100));
  DALI_TEST_CHECK( dummyInternalCanvasView );

  Dali::CanvasRenderer dummyCanvasRenderer = Dali::CanvasRenderer::New(Vector2(100, 100));
  DALI_TEST_CHECK( dummyCanvasRenderer );

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyInternalCanvasView, dummyCanvasRenderer);
  DALI_TEST_CHECK( task );

  Dali::Toolkit::Internal::CanvasViewRasterizeThread *dummyThread = new Dali::Toolkit::Internal::CanvasViewRasterizeThread();
  DALI_TEST_CHECK( dummyThread );

  END_TEST;
}

int UtcDaliCanvasViewRasterizeThreadAddTaskP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::Internal::CanvasView* dummyInternalCanvasView = new Dali::Toolkit::Internal::CanvasView(Vector2(100,100));
  DALI_TEST_CHECK( dummyInternalCanvasView );

  Dali::CanvasRenderer dummyCanvasRenderer = Dali::CanvasRenderer::New(Vector2(100, 100));
  DALI_TEST_CHECK( dummyCanvasRenderer );

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyInternalCanvasView, dummyCanvasRenderer);
  DALI_TEST_CHECK( task );

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task2 = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyInternalCanvasView, dummyCanvasRenderer);
  DALI_TEST_CHECK( task2 );

  Dali::Toolkit::Internal::CanvasViewRasterizeThread *dummyThread = new Dali::Toolkit::Internal::CanvasViewRasterizeThread();
  DALI_TEST_CHECK( dummyThread );

  dummyThread->AddTask(task);
  dummyThread->AddTask(task2);

  END_TEST;
}

int UtcDaliCanvasViewRasterizeThreadAddRemoveTaskP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::Internal::CanvasView* dummyInternalCanvasView = new Dali::Toolkit::Internal::CanvasView(Vector2(100,100));
  DALI_TEST_CHECK( dummyInternalCanvasView );

  Dali::CanvasRenderer dummyCanvasRenderer = Dali::CanvasRenderer::New(Vector2(100, 100));
  DALI_TEST_CHECK( dummyCanvasRenderer );

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyInternalCanvasView, dummyCanvasRenderer);
  DALI_TEST_CHECK( task );

  Dali::Toolkit::Internal::CanvasViewRasterizeThread *dummyThread = new Dali::Toolkit::Internal::CanvasViewRasterizeThread();
  DALI_TEST_CHECK( dummyThread );

  dummyThread->AddTask(task);

  dummyThread->RemoveTask(dummyInternalCanvasView);

  END_TEST;
}

int UtcDaliCanvasViewRasterizeThreadApplyRasterizedP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::Internal::CanvasView* dummyInternalCanvasView = new Dali::Toolkit::Internal::CanvasView(Vector2(100,100));
  DALI_TEST_CHECK( dummyInternalCanvasView );

  Dali::CanvasRenderer dummyCanvasRenderer = Dali::CanvasRenderer::New(Vector2(100, 100));
  DALI_TEST_CHECK( dummyCanvasRenderer );

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyInternalCanvasView, dummyCanvasRenderer);
  DALI_TEST_CHECK( task );

  Dali::Toolkit::Internal::CanvasViewRasterizeThread *dummyThread = new Dali::Toolkit::Internal::CanvasViewRasterizeThread();
  DALI_TEST_CHECK( dummyThread );

  dummyThread->AddTask(task);

  dummyThread->ApplyRasterized();

  END_TEST;
}

int UtcDaliCanvasViewRasterizeThreadTerminateThreadP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::Internal::CanvasView* dummyInternalCanvasView = new Dali::Toolkit::Internal::CanvasView(Vector2(100,100));
  DALI_TEST_CHECK( dummyInternalCanvasView );

  Dali::CanvasRenderer dummyCanvasRenderer = Dali::CanvasRenderer::New(Vector2(100, 100));
  DALI_TEST_CHECK( dummyCanvasRenderer );

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyInternalCanvasView, dummyCanvasRenderer);
  DALI_TEST_CHECK( task );

  Dali::Toolkit::Internal::CanvasViewRasterizeThread *dummyThread = new Dali::Toolkit::Internal::CanvasViewRasterizeThread();
  DALI_TEST_CHECK( dummyThread );

  dummyThread->AddTask(task);

  Dali::Toolkit::Internal::CanvasViewRasterizeThread::TerminateThread(dummyThread);

  END_TEST;
}

PixelData CreatePixelData( unsigned int width, unsigned int height )
{
  unsigned int bufferSize = width*height*Pixel::GetBytesPerPixel( Pixel::RGBA8888 );

  unsigned char* buffer= reinterpret_cast<unsigned char*>( malloc( bufferSize ) );
  PixelData pixelData = PixelData::New( buffer, bufferSize, width, height, Pixel::RGBA8888, PixelData::FREE );

  return pixelData;
}

int UtcDaliCanvasViewRasterizeThreadCallProcessP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::Internal::CanvasView* dummyInternalCanvasView = new Dali::Toolkit::Internal::CanvasView(Vector2(100,100));
  DALI_TEST_CHECK( dummyInternalCanvasView );

  Dali::CanvasRenderer dummyCanvasRenderer = Dali::CanvasRenderer::New(Vector2(100, 100));
  DALI_TEST_CHECK( dummyCanvasRenderer );

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyInternalCanvasView, dummyCanvasRenderer);
  DALI_TEST_CHECK( task );

  Dali::Toolkit::Internal::CanvasViewRasterizeThread *dummyThread = new Dali::Toolkit::Internal::CanvasViewRasterizeThread();
  DALI_TEST_CHECK( dummyThread );

  dummyThread->AddTask(task);

  dummyThread->Process(false);

  END_TEST;
}

int UtcDaliCanvasViewRasterizeThreadRasterizationCompletedSignalP(void)
{
  ToolkitTestApplication application;

  Dali::Toolkit::Internal::CanvasView* dummyInternalCanvasView = new Dali::Toolkit::Internal::CanvasView(Vector2(100,100));
  DALI_TEST_CHECK( dummyInternalCanvasView );

  Dali::CanvasRenderer dummyCanvasRenderer = Dali::CanvasRenderer::New(Vector2(100, 100));
  DALI_TEST_CHECK( dummyCanvasRenderer );

  IntrusivePtr<Dali::Toolkit::Internal::CanvasRendererRasterizingTask> task = new Dali::Toolkit::Internal::CanvasRendererRasterizingTask(dummyInternalCanvasView, dummyCanvasRenderer);
  DALI_TEST_CHECK( task );

  Dali::Toolkit::Internal::CanvasViewRasterizeThread *dummyThread = new Dali::Toolkit::Internal::CanvasViewRasterizeThread();
  DALI_TEST_CHECK( dummyThread );

  dummyThread->AddTask(task);

  dummyThread->Process(false);

  PixelData pixelData = CreatePixelData( 100, 100 );

  dummyThread->RasterizationCompletedSignal().Connect(dummyInternalCanvasView, &Dali::Toolkit::Internal::CanvasView::ApplyRasterizedImage);
  dummyThread->RasterizationCompletedSignal().Emit(pixelData);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliCanvasViewSetSizeAndAddDrawable(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(100,100));
  DALI_TEST_CHECK( canvasView );

  application.GetScene().Add(canvasView);

  canvasView.SetProperty(Actor::Property::SIZE, Vector2(300, 300));

  application.SendNotification();
  application.Render();

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();

  shape.AddRect(Rect<float>(10, 10, 10, 10), Vector2(0, 0));

  canvasView.AddDrawable(shape);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliCanvasViewViewBoxP(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(300, 300));
  DALI_TEST_CHECK( canvasView );

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
  DALI_TEST_CHECK( canvasView );

  application.GetScene().Add(canvasView);

  canvasView.SetProperty(Actor::Property::SIZE, Vector2(300, 300));
  canvasView.SetProperty(Toolkit::CanvasView::Property::VIEW_BOX, Vector2(-999, -999));

  application.SendNotification();
  application.Render();

  Vector2 viewBox = canvasView.GetProperty(Toolkit::CanvasView::Property::VIEW_BOX).Get<Vector2>();
  DALI_TEST_EQUALS( viewBox, Vector2(-999, -999), TEST_LOCATION );

  END_TEST;
}
