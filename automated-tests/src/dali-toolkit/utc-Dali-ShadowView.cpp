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
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/shadow-view/shadow-view.h>


using namespace Dali;
using namespace Dali::Toolkit;

void shadow_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void shadow_view_cleanup(void)
{
  test_return_value = TET_PASS;
}


// Negative test case for a method
int UtcDaliShadowViewUninitialized(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_CHECK(!view);
  }
  END_TEST;
}

// Positive test case for a method
int UtcDaliShadowViewNew(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliShadowViewNew");

  Toolkit::ShadowView view = Toolkit::ShadowView::New();
  DALI_TEST_CHECK( view );

  Toolkit::ShadowView view2 = Toolkit::ShadowView::New(1.0f, 1.0f);
  DALI_TEST_CHECK( view2 );
  END_TEST;
}

// Positive test case for a method
int UtcDaliShadowViewDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliShadowViewDownCast");

  Toolkit::ShadowView view = Toolkit::ShadowView::New();
  BaseHandle handle(view);

  Toolkit::ShadowView shadowView = Toolkit::ShadowView::DownCast( handle );
  DALI_TEST_CHECK( view );
  DALI_TEST_CHECK( shadowView );
  DALI_TEST_CHECK( shadowView == view );
  END_TEST;
}

// Positive test case for a method
int UtcDaliShadowViewPropertyNames(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliShadowViewPropertyNames");

  Toolkit::ShadowView view = Toolkit::ShadowView::New();
  DALI_TEST_CHECK( view );

  // Check the names, this names are used in the shader code,
  // if they change in the shader code, then it has to be updated here.
  DALI_TEST_EQUALS( view.GetBlurStrengthPropertyIndex(), view.GetPropertyIndex("BlurStrengthProperty"), TEST_LOCATION );
  DALI_TEST_EQUALS( view.GetShadowColorPropertyIndex(), view.GetPropertyIndex("ShadowColorProperty"), TEST_LOCATION );
  END_TEST;
}

// Positive test case for a method
int UtcDaliShadowViewAddRemove(void)
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
  END_TEST;
}

// Positive test case for a method
int UtcDaliShadowViewActivateDeactivate(void)
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
  END_TEST;
}
