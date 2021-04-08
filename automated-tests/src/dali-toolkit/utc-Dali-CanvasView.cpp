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
#include <dali-toolkit/devel-api/controls/canvas-view/canvas-view.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer-shape.h>


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

int UtcDaliCanvasViewChangeSizeP(void)
{
  ToolkitTestApplication application;

  CanvasView canvasView = CanvasView::New(Vector2(100,100));
  DALI_TEST_CHECK( canvasView );

  application.GetScene().Add(canvasView);
  application.SendNotification();
  application.Render();

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
  application.SendNotification();
  application.Render();

  canvasView.SetProperty(Actor::Property::SIZE, Vector2(-999, -999));

  application.SendNotification();
  application.Render();

  Property::Value pv = canvasView.GetProperty(Actor::Property::SIZE);
  Vector3 v3;
  pv.Get(v3);
  DALI_TEST_EQUALS( v3, Vector3(-999, -999, 0), TEST_LOCATION );

  END_TEST;
}
