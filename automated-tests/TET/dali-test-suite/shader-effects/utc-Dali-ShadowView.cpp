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
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Toolkit;


static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliShadowViewUninitialized();
static void UtcDaliShadowViewNew();
static void UtcDaliShadowViewDownCast();
static void UtcDaliShadowViewPropertyNames();
static void UtcDaliShadowViewAddRemove();
static void UtcDaliShadowViewActivateDeactivate();


enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
      { UtcDaliShadowViewUninitialized, NEGATIVE_TC_IDX },
      { UtcDaliShadowViewNew, POSITIVE_TC_IDX },
      { UtcDaliShadowViewDownCast, POSITIVE_TC_IDX },
      { UtcDaliShadowViewPropertyNames, POSITIVE_TC_IDX },
      { UtcDaliShadowViewAddRemove, POSITIVE_TC_IDX },
      { UtcDaliShadowViewActivateDeactivate, POSITIVE_TC_IDX },
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
static void UtcDaliShadowViewUninitialized()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliShadowViewUninitialized");

  Toolkit::ShadowView view;
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
static void UtcDaliShadowViewNew()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliShadowViewNew");

  Toolkit::ShadowView view = Toolkit::ShadowView::New();
  DALI_TEST_CHECK( view );

  Toolkit::ShadowView view2 = Toolkit::ShadowView::New(1.0f, 1.0f);
  DALI_TEST_CHECK( view2 );
}

// Positive test case for a method
static void UtcDaliShadowViewDownCast()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliShadowViewDownCast");

  Toolkit::ShadowView view = Toolkit::ShadowView::New();
  BaseHandle handle(view);

  Toolkit::ShadowView shadowView = Toolkit::ShadowView::DownCast( handle );
  DALI_TEST_CHECK( view );
  DALI_TEST_CHECK( shadowView );
  DALI_TEST_CHECK( shadowView == view );
}

// Positive test case for a method
static void UtcDaliShadowViewPropertyNames()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliShadowViewPropertyNames");

  Toolkit::ShadowView view = Toolkit::ShadowView::New();
  DALI_TEST_CHECK( view );

  // Check the names, this names are used in the shader code,
  // if they change in the shader code, then it has to be updated here.
  DALI_TEST_EQUALS( view.GetBlurStrengthPropertyIndex(), view.GetPropertyIndex("BlurStrengthProperty"), TEST_LOCATION );
  DALI_TEST_EQUALS( view.GetShadowColorPropertyIndex(), view.GetPropertyIndex("ShadowColorProperty"), TEST_LOCATION );
}

// Positive test case for a method
static void UtcDaliShadowViewAddRemove()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliShadowViewAddRemove");

  Toolkit::ShadowView view = Toolkit::ShadowView::New();
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
static void UtcDaliShadowViewActivateDeactivate()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliShadowViewActivateDeactivate");

  Toolkit::ShadowView view = Toolkit::ShadowView::New();
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
