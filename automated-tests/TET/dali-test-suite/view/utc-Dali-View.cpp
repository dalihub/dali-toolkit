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
#include <dali-toolkit/public-api/controls/view/view.h>
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Toolkit;

namespace
{

static bool gAnimationStarted = false;

void StartAnimation( View, Animation& animation, const Orientation& orientation )
{
  gAnimationStarted = true;
}


static bool gObjectCreatedCallBackCalled;
static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}


}

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliViewNew, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliViewAddGetRemoveContentLayer01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliViewAddGetRemoveContentLayer02, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliViewSetGetBackgroundLayer01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliViewSetGetBackgroundLayer02, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliViewSetOrientationFunction, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliViewOrientationChanged, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcSetAutoRotate, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliViewNew()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliViewNew");

  View view1;
  DALI_TEST_CHECK( !view1 );

  view1 = View::New();
  DALI_TEST_CHECK( view1 );

  View view2( view1 );
  DALI_TEST_CHECK( view2 );

  View view3 = view2;
  DALI_TEST_CHECK( view3 );

  view1 = NULL;
  view2 = NULL;
  view3 = NULL;

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    View view = View::New();
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
}

static void UtcDaliViewAddGetRemoveContentLayer01()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliViewAddGetRemoveContentLayer01");

  View view = View::New();
  Layer layer1;
  Layer layer2;
  Layer layer3;
  Layer layer4;

  // Test: add and get layers.
  try
  {
    layer1 = Layer::New();
    layer1.SetName( "Layer1" );
    layer2 = Layer::New();
    layer2.SetName( "Layer2" );

    unsigned int layerId1 = view.AddContentLayer( layer1 );
    unsigned int layerId2 = view.AddContentLayer( layer2 );

    layer3 = view.GetContentLayer( layerId1 );
    layer4 = view.GetContentLayer( layerId2 );

    DALI_TEST_EQUALS( layer1.GetName(), layer3.GetName(), TEST_LOCATION );
    DALI_TEST_EQUALS( layer2.GetName(), layer4.GetName(), TEST_LOCATION );
  }
  catch( ... )
  {
    tet_printf( "UtcDaliViewAddGetRemoveContentLayer: Exception while adding and geting layers to/from view.\n" );
    tet_result(TET_FAIL);
  }

  bool test1 = false;
  bool test2 = false;
  // Test: remove layers.
  try
  {
    view.RemoveContentLayer( layer3 );
    view.RemoveContentLayer( layer4 );
    test1 = true;
  }
  catch( ... )
  {
    tet_printf( "UtcDaliViewAddGetRemoveContentLayer: Exception while removing layers from view.\n" );
    tet_result(TET_FAIL);
  }

  // Test: add same layers again.
  try
  {
    view.AddContentLayer( layer1 );
    view.AddContentLayer( layer2 );
    test2 = true;
  }
  catch( ... )
  {
    tet_printf( "UtcDaliViewAddGetRemoveContentLayer: Exception while adding layers from view after have been removed.\n" );
    tet_result(TET_FAIL);
  }

  DALI_TEST_CHECK( test1 && test2 );
}

static void UtcDaliViewAddGetRemoveContentLayer02()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliViewAddGetRemoveContentLayer02");

  View view = View::New();

  Layer layer1 = Layer::New();
  layer1.SetName( "Layer1" );
  Layer layer2 = Layer::New();
  layer2.SetName( "Layer2" );

  view.AddContentLayer( layer1 );
  view.AddContentLayer( layer2 );

  // Test: add a layer twice.
  try
  {
    view.AddContentLayer( layer1 );
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  // Test: add an unitialized layer.
  try
  {
    Layer layer;
    view.AddContentLayer( layer );
  }
  catch( DaliException& e )
  {
    tet_printf("Assertion %s failed at %s when an unitialized layer is added.\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_EQUALS( e.mCondition, "layer", TEST_LOCATION );
  }

  // Test: get a layer which was not added before.
  Layer layer = view.GetContentLayer( 100 );
  DALI_TEST_CHECK( !layer );

  // Test: Remove a layer which was not added before.
  try
  {
    Layer layer = Layer::New();
    view.RemoveContentLayer( layer );
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
}

static void UtcDaliViewSetGetBackgroundLayer01()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliViewSetGetBackgroundLayer01");

  View view;
  Layer layer1, layer2;

  // Test with an actor.

  view = View::New();
  Stage::GetCurrent().Add( view );

  ImageActor background = CreateSolidColorActor( Color::RED );

  view.SetBackground( background );

  layer1 = view.GetBackgroundLayer();

  DALI_TEST_CHECK( layer1 );

  background = CreateSolidColorActor( Color::GREEN );

  view.SetBackground( background );

  layer2 = view.GetBackgroundLayer();

  DALI_TEST_CHECK( layer2 );

  Stage::GetCurrent().Remove( view );
}

static void UtcDaliViewSetGetBackgroundLayer02()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliViewSetGetBackgroundLayer02");

  bool assert = false;

  try
  {
    View view = View::New();

    ImageActor background = CreateSolidColorActor( Color::RED );

    view.SetBackground( background );
  }
  catch( DaliException& e )
  {
    tet_printf("Assertion %s failed at %s when trying to add background to the view and the view is not on the stage.\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_EQUALS( e.mCondition, "mBackgroundLayer.OnStage()", TEST_LOCATION );
    assert = true;
  }

  DALI_TEST_CHECK( assert );
}

static void UtcDaliViewSetOrientationFunction()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliViewSetOrientationFunction");

  // Test it doesn't crash
  try
  {
    View view = View::New();
    Stage::GetCurrent().Add( view );

    view.SetSize( 480, 800 );
    view.SetOrientationFunction( Degree( 0.f ), Degree( 90.f ), Degree( 180.f ), Degree( 270.f ) );
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
}

static void UtcDaliViewOrientationChanged()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliViewOrientationChanged");

  gAnimationStarted = false;

  // Test it doesn't crash
  try
  {
    View view = View::New();
    Stage::GetCurrent().Add( view );

    view.SetSize( 480, 800 );

    view.OrientationAnimationStartedSignal().Connect( &StartAnimation );

    application.SendNotification(); // Remove these two lines causes
    application.Render();           // ToolkitTestApplication destructor to crash

    Orientation orientation = application.GetOrientation().GetHandle();
    application.GetOrientation().SetDegrees( 90 );
    view.OrientationChanged( orientation );
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  // Check the view animation started.
  DALI_TEST_CHECK( gAnimationStarted );
}

static void UtcSetAutoRotate()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcSetAutoRotate");

  gAnimationStarted = false;

  View view;

  // Test it doesn't crash
  try
  {
    view = View::New();
    Stage::GetCurrent().Add( view );

    view.SetSize( 480, 800 );

    view.OrientationAnimationStartedSignal().Connect( &StartAnimation );

    application.SendNotification();
    application.Render();

    Orientation orientation = application.GetOrientation().GetHandle();
    application.GetOrientation().SetDegrees( 90 );
    view.OrientationChanged( orientation );
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  // Check the view animation started.
  DALI_TEST_CHECK( gAnimationStarted );


  gAnimationStarted = false;

  try
  {
    view = View::New();
    view.SetAutoRotate( false ); // Animation shouldn't start.
    Stage::GetCurrent().Add( view );

    view.SetSize( 480, 800 );

    application.SendNotification();
    application.Render();

    Orientation orientation = application.GetOrientation().GetHandle();
    application.GetOrientation().SetDegrees( 180 );
    view.OrientationChanged( orientation );
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  // Check the view animation didn't start.
  DALI_TEST_CHECK( !gAnimationStarted );
}
