/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* TEST_IMAGE_FILE_NAME = DALI_IMAGE_DIR "gallery_image_01.jpg";
} // namespace

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliGaussianBlurViewUninitialized();
static void UtcDaliGaussianBlurViewNew();
static void UtcDaliGaussianBlurViewDownCast();
static void UtcDaliGaussianBlurViewPropertyNames();
static void UtcDaliGaussianBlurViewAddRemove();
static void UtcDaliGaussianBlurActivateDeactivate();
static void UtcDaliGaussianBlurViewSetGetBackgroundColor();
static void UtcDaliGaussianBlurViewSetGetRenderTarget();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
      { UtcDaliGaussianBlurViewUninitialized, NEGATIVE_TC_IDX },
      { UtcDaliGaussianBlurViewNew, POSITIVE_TC_IDX },
      { UtcDaliGaussianBlurViewDownCast, POSITIVE_TC_IDX },
      { UtcDaliGaussianBlurViewPropertyNames, POSITIVE_TC_IDX },
      { UtcDaliGaussianBlurViewAddRemove, POSITIVE_TC_IDX },
      { UtcDaliGaussianBlurActivateDeactivate, POSITIVE_TC_IDX },
      { UtcDaliGaussianBlurViewSetGetBackgroundColor, POSITIVE_TC_IDX },
      { UtcDaliGaussianBlurViewSetGetRenderTarget, POSITIVE_TC_IDX },
    { NULL, 0 }
  };
}

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

// Negative test case for a method
static void UtcDaliGaussianBlurViewUninitialized()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewUninitialized");

  Toolkit::GaussianBlurView view;

  try
  {
    // New() must be called to create a GaussianBlurView or it wont be valid.
    Actor a = Actor::New();
    view.Add( a );
    DALI_TEST_CHECK( false );
  }
  catch (Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_CHECK(!view);
  }
}

// Positive test case for a method
static void UtcDaliGaussianBlurViewNew()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewNew");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  DALI_TEST_CHECK( view );

  Toolkit::GaussianBlurView view2 = Toolkit::GaussianBlurView::New(5, 1.5f, Pixel::RGB888, 0.5f, 0.5f, false);
  DALI_TEST_CHECK( view2 );
}

// Positive test case for a method
static void UtcDaliGaussianBlurViewDownCast()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewDownCast");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  BaseHandle handle(view);

  Toolkit::GaussianBlurView gaussianBlurView = Toolkit::GaussianBlurView::DownCast( handle );
  DALI_TEST_CHECK( view );
  DALI_TEST_CHECK( gaussianBlurView );
  DALI_TEST_CHECK( gaussianBlurView == view );
}


// Positive test case for a method
static void UtcDaliGaussianBlurViewPropertyNames()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewPropertyNames");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  DALI_TEST_CHECK( view );

  // Check the names, this names are used in the shader code,
  // if they change in the shader code, then it has to be updated here.
  DALI_TEST_EQUALS( view.GetBlurStrengthPropertyIndex(), view.GetPropertyIndex("GaussianBlurStrengthPropertyName"), TEST_LOCATION );
}

// Positive test case for a method
static void UtcDaliGaussianBlurViewAddRemove()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewAddRemove");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  DALI_TEST_CHECK( view );

  Actor actor = Actor::New();
  DALI_TEST_CHECK( !actor.OnStage() );


  view.SetParentOrigin(ParentOrigin::CENTER);
  view.SetSize(Stage::GetCurrent().GetSize());
  view.Add(actor);
  Stage::GetCurrent().Add(view);

  DALI_TEST_CHECK( actor.OnStage() );

  view.Remove(actor);

  DALI_TEST_CHECK( !actor.OnStage() );
}

// Positive test case for a method
static void UtcDaliGaussianBlurActivateDeactivate()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurActivateDeactivate");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  DALI_TEST_CHECK( view );

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  DALI_TEST_CHECK( 1u == taskList.GetTaskCount() );

  view.SetParentOrigin(ParentOrigin::CENTER);
  view.SetSize(Stage::GetCurrent().GetSize());
  view.Add(Actor::New());
  Stage::GetCurrent().Add(view);
  view.Activate();

  RenderTaskList taskList2 = Stage::GetCurrent().GetRenderTaskList();
  DALI_TEST_CHECK( 1u != taskList2.GetTaskCount() );

  view.Deactivate();

  RenderTaskList taskList3 = Stage::GetCurrent().GetRenderTaskList();
  DALI_TEST_CHECK( 1u == taskList3.GetTaskCount() );
}

// Positive test case for a method
static void UtcDaliGaussianBlurViewSetGetBackgroundColor()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewSetGetBackgroundColor");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  DALI_TEST_CHECK( view );

  view.SetBackgroundColor(Dali::Color::RED);
  Vector4 color = view.GetBackgroundColor();
  DALI_TEST_CHECK( color == Dali::Color::RED );
}

// Positive test case for a method
static void UtcDaliGaussianBlurViewSetGetRenderTarget()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewSetGetRenderTarget");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New(5, 1.5f, Pixel::RGB888, 0.5f, 0.5f, true);
  DALI_TEST_CHECK( view );

  view.SetParentOrigin(ParentOrigin::CENTER);
  view.SetSize(Stage::GetCurrent().GetSize());
  view.Add(Actor::New());
  Stage::GetCurrent().Add(view);
  view.Activate();

  FrameBufferImage renderTarget = FrameBufferImage::New( 480.0f, 800.0f, Pixel::RGB888 );
  view.SetUserImageAndOutputRenderTarget(Image::New(TEST_IMAGE_FILE_NAME), renderTarget);
  DALI_TEST_CHECK( view.GetBlurredRenderTarget() == renderTarget );
}
