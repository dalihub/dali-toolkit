//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* TEST_IMAGE_FILE_NAME =  "gallery_image_01.jpg";
} // namespace

void utc_gaussian_blur_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_gaussian_blur_view_cleanup(void)
{
  test_return_value = TET_PASS;
}



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
    view.Add( a );
    DALI_TEST_CHECK( false );
  }
  catch (Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
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
  DALI_TEST_CHECK( view );

  Toolkit::GaussianBlurView view2 = Toolkit::GaussianBlurView::New(5, 1.5f, Pixel::RGB888, 0.5f, 0.5f, false);
  DALI_TEST_CHECK( view2 );
  END_TEST;
}

// Positive test case for a method
int UtcDaliGaussianBlurViewDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewDownCast");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  BaseHandle handle(view);

  Toolkit::GaussianBlurView gaussianBlurView = Toolkit::GaussianBlurView::DownCast( handle );
  DALI_TEST_CHECK( view );
  DALI_TEST_CHECK( gaussianBlurView );
  DALI_TEST_CHECK( gaussianBlurView == view );
  END_TEST;
}


// Positive test case for a method
int UtcDaliGaussianBlurViewPropertyNames(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewPropertyNames");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  DALI_TEST_CHECK( view );

  // Check the names, this names are used in the shader code,
  // if they change in the shader code, then it has to be updated here.
  DALI_TEST_EQUALS( view.GetBlurStrengthPropertyIndex(), view.GetPropertyIndex("GaussianBlurStrengthPropertyName"), TEST_LOCATION );
  END_TEST;
}

// Positive test case for a method
int UtcDaliGaussianBlurViewAddRemove(void)
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
  END_TEST;
}

// Positive test case for a method
int UtcDaliGaussianBlurActivateDeactivate(void)
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
  END_TEST;
}

// Positive test case for a method
int UtcDaliGaussianBlurViewSetGetBackgroundColor(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewSetGetBackgroundColor");

  Toolkit::GaussianBlurView view = Toolkit::GaussianBlurView::New();
  DALI_TEST_CHECK( view );

  view.SetBackgroundColor(Dali::Color::RED);
  Vector4 color = view.GetBackgroundColor();
  DALI_TEST_CHECK( color == Dali::Color::RED );
  END_TEST;
}

// Positive test case for a method
int UtcDaliGaussianBlurViewSetGetRenderTarget(void)
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
  END_TEST;
}
