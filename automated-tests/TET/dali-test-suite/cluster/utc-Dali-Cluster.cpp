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

static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

} // namespace

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
TEST_FUNCTION( UtcDaliClusterNew,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliClusterDownCast,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliClusterAddAndRemoveChild,       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliClusterExpandAndCollapseChild,  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliClusterSetAndGetStyle,          POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliClusterNew()
{
  ToolkitTestApplication application;

  // Create the Cluster actor
  ClusterStyle style = ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle1);
  Cluster cluster = Cluster::New(style);

  DALI_TEST_CHECK(cluster);

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect(&TestCallback);
  {
    ClusterStyle style = ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle1);
    Cluster cluster = Cluster::New(style);
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
}

static void UtcDaliClusterDownCast()
{
  ToolkitTestApplication application;

  // Create the Cluster actor
  ClusterStyle style = ClusterStyleRandom::New();
  const Cluster clusterConst = Cluster::New(style);
  Cluster cluster(clusterConst);

  BaseHandle handle(cluster);

  Cluster newCluster = Cluster::DownCast( handle );
  DALI_TEST_CHECK( cluster );
  DALI_TEST_CHECK( newCluster == cluster );
}

static void UtcDaliClusterAddAndRemoveChild()
{
  ToolkitTestApplication application;

  // Create the Cluster actor
  ClusterStyle style = ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle1);
  Cluster cluster = Cluster::New(style);

  Actor childActor1 = Actor::New();
  Actor childActor2 = Actor::New();
  Actor childActor3 = Actor::New();
  Actor childActor4 = Actor::New();

  // Add the first child and check it is added to the end
  cluster.AddChild(childActor1);
  DALI_TEST_CHECK( cluster.GetChildAt(0) == childActor1);
  DALI_TEST_CHECK( !cluster.GetChildAt(1) );
  DALI_TEST_CHECK( cluster.GetTotalCount() == 1 );

  // Add the second child to the given position and check it is added
  cluster.AddChild(childActor2, 1);
  DALI_TEST_CHECK( cluster.GetChildAt(1) == childActor2);
  DALI_TEST_CHECK( cluster.GetTotalCount() == 2 );

  // Add the third child with depth index 1 and check it is added to the end
  cluster.AddChildAt(childActor3, 1);
  DALI_TEST_CHECK( cluster.GetChildAt(2) == childActor3);
  DALI_TEST_CHECK( cluster.GetTotalCount() == 3 );

  // Add the fourth child with depth index 2 to the given position and check it is added
  cluster.AddChildAt(childActor4, 2, 3);
  DALI_TEST_CHECK( cluster.GetChildAt(3) == childActor4);
  DALI_TEST_CHECK( cluster.GetTotalCount() == 4 );

  // Remove the child in the given position and check it's removed
  cluster.RemoveChildAt(3);
  DALI_TEST_CHECK( !cluster.GetChildAt(3) );
  DALI_TEST_CHECK( cluster.GetTotalCount() == 3 );
}

static void UtcDaliClusterExpandAndCollapseChild()
{
  ToolkitTestApplication application;

  // Create the Cluster actor
  ClusterStyle style = ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle1);
  Cluster cluster = Cluster::New(style);

  Actor childActor1 = Actor::New();
  Actor childActor2 = Actor::New();
  Actor childActor3 = Actor::New();
  Actor childActor4 = Actor::New();

  // Add the child actors
  cluster.AddChild(childActor1);
  cluster.AddChild(childActor2);
  cluster.AddChildAt(childActor3, 1);
  cluster.AddChildAt(childActor4, 2, 3);

  // Expand child actor 3
  cluster.ExpandChild(2);
  DALI_TEST_CHECK( cluster.GetExpandedCount() == 1 );

  // Expand child actor 4
  cluster.ExpandChild(3);
  DALI_TEST_CHECK( cluster.GetExpandedCount() == 2 );

  // Collapse child actor 3
  cluster.CollapseChild(2);
  DALI_TEST_CHECK( cluster.GetExpandedCount() == 1 );

  // Expand all children
  cluster.ExpandAllChildren();
  DALI_TEST_CHECK( cluster.GetExpandedCount() == 4 );

  // Collpase all children
  cluster.CollapseAllChildren();
  DALI_TEST_CHECK( cluster.GetExpandedCount() == 0 );

  // Transform and restore the child
  cluster.TransformChild(1, Vector3(10.0f, 10.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, Vector3::YAXIS), AlphaFunctions::EaseOut, 0.5f);
  cluster.RestoreChild(1, AlphaFunctions::EaseOut, 0.25f, true);
}

static void UtcDaliClusterSetAndGetStyle()
{
  ToolkitTestApplication application;

  // Create the default cluster style
  ClusterStyle defaultStyle = ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle1);
  DALI_TEST_CHECK( defaultStyle.GetMaximumNumberOfChildren() > 0 );

  // Add style to background and title
  Actor background = Actor::New();
  Actor title = Actor::New();
  defaultStyle.ApplyStyleToBackground(background, AlphaFunctions::EaseOut, 1.0f);
  defaultStyle.ApplyStyleToTitle(title, AlphaFunctions::EaseOut, 1.0f);

  // Create the Cluster actor with the default style
  Cluster cluster = Cluster::New(defaultStyle);
  DALI_TEST_CHECK( cluster.GetStyle() == defaultStyle );
  cluster.SetBackgroundImage(background);
  cluster.SetTitle(title);

  // Create a new style and apply it to the cluster
  ClusterStyle newStyle = ClusterStyleRandom::New();
  cluster.SetStyle(newStyle);
  DALI_TEST_CHECK( cluster.GetStyle() == newStyle );
}
