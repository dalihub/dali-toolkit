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

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliBloomViewUninitialized();
static void UtcDaliBloomViewNew();
static void UtcDaliBloomViewDownCast();
static void UtcDaliBloomViewPropertyNames();
static void UtcDaliBloomViewAddRemove();
static void UtcDaliBloomActivateDeactivate();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliBloomViewUninitialized, NEGATIVE_TC_IDX },
    { UtcDaliBloomViewNew, POSITIVE_TC_IDX },
    { UtcDaliBloomViewDownCast, POSITIVE_TC_IDX },
    { UtcDaliBloomViewPropertyNames, POSITIVE_TC_IDX },
    { UtcDaliBloomViewAddRemove, POSITIVE_TC_IDX },
    { UtcDaliBloomActivateDeactivate, POSITIVE_TC_IDX },
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
static void UtcDaliBloomViewUninitialized()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliBloomViewUninitialized");

  Toolkit::BloomView view;

  try
  {
    // New() must be called to create a BloomView or it wont be valid.
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
static void UtcDaliBloomViewNew()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliBloomViewNew");

  Toolkit::BloomView view = Toolkit::BloomView::New();
  DALI_TEST_CHECK( view );

  Toolkit::BloomView view2 = Toolkit::BloomView::New(10, 1.0f, Pixel::RGB888, 0.5f, 0.5f);
  DALI_TEST_CHECK( view2 );
}

// Positive test case for a method
static void UtcDaliBloomViewDownCast()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliBloomViewDownCast");

  Toolkit::BloomView view = Toolkit::BloomView::New();
  BaseHandle handle(view);

  Toolkit::BloomView bloomView = Toolkit::BloomView::DownCast( handle );
  DALI_TEST_CHECK( view );
  DALI_TEST_CHECK( bloomView );
  DALI_TEST_CHECK( bloomView == view );
}


// Positive test case for a method
static void UtcDaliBloomViewPropertyNames()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliBloomViewPropertyNames");

  Toolkit::BloomView view = Toolkit::BloomView::New();
  DALI_TEST_CHECK( view );

  // Check the names, this names are used in the shader code,
  // if they change in the shader code, then it has to be updated here.
  DALI_TEST_EQUALS( view.GetBloomThresholdPropertyIndex(), view.GetPropertyIndex("uBloomThreshold"), TEST_LOCATION );
  DALI_TEST_EQUALS( view.GetBlurStrengthPropertyIndex(), view.GetPropertyIndex("BlurStrengthProperty"), TEST_LOCATION );
  DALI_TEST_EQUALS( view.GetBloomIntensityPropertyIndex(), view.GetPropertyIndex("uBloomIntensity"), TEST_LOCATION );
  DALI_TEST_EQUALS( view.GetBloomSaturationPropertyIndex(), view.GetPropertyIndex("uBloomSaturation"), TEST_LOCATION );
  DALI_TEST_EQUALS( view.GetImageIntensityPropertyIndex(), view.GetPropertyIndex("uImageIntensity"), TEST_LOCATION );
  DALI_TEST_EQUALS( view.GetImageSaturationPropertyIndex(), view.GetPropertyIndex("uImageSaturation"), TEST_LOCATION );
}

// Positive test case for a method
static void UtcDaliBloomViewAddRemove()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliBloomViewAddRemove");

  Toolkit::BloomView view = Toolkit::BloomView::New();
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
static void UtcDaliBloomActivateDeactivate()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliBloomActivateDeactivate");

  Toolkit::BloomView view = Toolkit::BloomView::New();
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
