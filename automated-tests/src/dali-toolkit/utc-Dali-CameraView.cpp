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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/public-api/controls/camera-view/camera-view.h>
#include <stdlib.h>
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;

// Negative test case for a method
int UtcDaliCameraViewUninitialized(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCameraViewUninitialized");

  Toolkit::CameraView view;

  try
  {
    // New() must be called to create a CameraView or it wont be valid.
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
int UtcDaliCameraViewNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCameraViewNew");
  Any                 dummy(0);
  Toolkit::CameraView view = Toolkit::CameraView::New(dummy);
  DALI_TEST_CHECK(view);
  END_TEST;
}

// Positive test case for a method
int UtcDaliCameraViewDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCameraViewDownCast");

  Any                 dummy(0);
  Toolkit::CameraView view = Toolkit::CameraView::New(dummy);
  BaseHandle          handle(view);

  Toolkit::CameraView view2 = Toolkit::CameraView::DownCast(handle);
  DALI_TEST_CHECK(view);
  DALI_TEST_CHECK(view2);
  DALI_TEST_CHECK(view == view2);
  END_TEST;
}

int UtcDaliCameraViewCopyAndAssignment(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliCameraViewCopyAndAssignment");

  Any        dummy(0);
  CameraView view = Toolkit::CameraView::New(dummy);
  DALI_TEST_CHECK(view);

  CameraView copy(view);
  DALI_TEST_CHECK(view == copy);

  CameraView assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == view);

  END_TEST;
}

int UtcDaliCameraViewMoveAssignment(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliCameraViewMoveAssignment");

  Any        dummy(0);
  CameraView view = Toolkit::CameraView::New(dummy);
  DALI_TEST_EQUALS(1, view.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  CameraView moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!view);

  END_TEST;
}

int UtcDaliCameraViewTypeRegistry(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliCameraViewTypeRegistry");

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK(typeRegistry);

  TypeInfo typeInfo = typeRegistry.GetTypeInfo("CameraView");
  DALI_TEST_CHECK(typeInfo);

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK(handle);

  CameraView view = CameraView::DownCast(handle);
  DALI_TEST_CHECK(view);

  END_TEST;
}

int UtcDaliCameraViewWindowDisplayType(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliCameraViewWindowDisplayType");

  Any        dummy(0);
  CameraView cameraView = CameraView::New(dummy, Dali::Toolkit::CameraView::DisplayType::WINDOW);
  DALI_TEST_CHECK(cameraView);

  application.GetScene().Add(cameraView);

  try
  {
    cameraView.Update();
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(true);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_CHECK(false);
  }

  END_TEST;
}

int UtcDaliCameraViewImageDisplayType(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliCameraViewImageDisplayType");

  Any        dummy(0);
  CameraView cameraView = CameraView::New(dummy, Dali::Toolkit::CameraView::DisplayType::IMAGE);
  DALI_TEST_CHECK(cameraView);

  application.GetScene().Add(cameraView);

  try
  {
    cameraView.Update();
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(true);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_CHECK(false);
  }

  END_TEST;
}

int UtcDaliCameraViewCoverUpdateDisplayArea1(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliCameraViewUpdateDisplayArea1");

  Any        dummy(0);
  CameraView view = CameraView::New(dummy, Dali::Toolkit::CameraView::DisplayType::WINDOW);
  DALI_TEST_CHECK(view);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  Vector3 vector(100.0f, 100.0f, 0.0f);
  view.SetProperty(Actor::Property::SIZE, vector);

  application.SendNotification();
  application.Render();

  // Check the size in the new frame
  DALI_TEST_CHECK(vector == view.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  application.GetScene().Remove(view);

  END_TEST;
}

int UtcDaliCameraViewCoverUpdateDisplayArea2(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliCameraViewUpdateDisplayArea2");

  Any        dummy(0);
  CameraView view = CameraView::New(dummy, Dali::Toolkit::CameraView::DisplayType::WINDOW);
  DALI_TEST_CHECK(view);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  Vector3 vector(100.0f, 100.0f, 0.0f);
  view.SetProperty(Actor::Property::SIZE, vector);

  application.SendNotification();
  application.Render();

  // Check the size in the new frame
  DALI_TEST_CHECK(vector == view.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  application.GetScene().Remove(view);

  END_TEST;
}
