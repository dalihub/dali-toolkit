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
using namespace Toolkit;

void dali_tableview_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_tableview_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}


struct Constraint100
{
  Constraint100( )
  {
  }

  /**
   * function operator to apply the parent size
   */
  Dali::Vector3 operator()(const Dali::Vector3& current)
  {
    return Dali::Vector3( 100.0f, 100.0f, 100.0f );
  }
};

// Convenience function to quickly set up a 10x10 table with each cell being 10x10 pixels in size by default.
static void SetupTableViewAndActors(TableView& tableView, Actor& actor1, Actor& actor2, Actor& actor3)
{
  tableView = TableView::New(10,10); // 10 by 10 grid.
  DALI_TEST_CHECK(tableView);

  Stage::GetCurrent().Add( tableView );
  tableView.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, Constraint100() ) );
  tableView.SetLayoutAnimationDuration(0.0f);

  actor1 = Actor::New();
  actor2 = Actor::New();
  actor3 = Actor::New();

  actor1.SetSize(10,10);
  actor2.SetSize(10,10);
  actor3.SetSize(10,10);

  tableView.AddChild(actor1, TableView::CellPosition(0,0));
  tableView.AddChild(actor2, TableView::CellPosition(0,1));
  tableView.AddChild(actor3, TableView::CellPosition(1,0));
}

} // namespace


int UtcDaliTableViewNew(void)
{
  ToolkitTestApplication application;

  TableView tableView = TableView::New(10,10);
  DALI_TEST_CHECK(tableView);

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect(&TestCallback);
  {
    TableView tableView = TableView::New(10,10);
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
  END_TEST;
}

// Test adjusting the metric values for the cell.
int UtcDaliTableViewMetricsPadding(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTableViewMetricsPadding");

  TableView tableView;
  Actor actor1;
  Actor actor2;
  Actor actor3;

  SetupTableViewAndActors(tableView, actor1, actor2, actor3);

  // 1. check that padding works. no padding:
  tableView.SetCellPadding(Size(0.0f, 0.0f));
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( tableView.GetCellPadding(), Size(0.0f, 0.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( actor1.GetCurrentPosition(), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2.GetCurrentPosition(), Vector3(10.0f, 0.0f, 0.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( actor3.GetCurrentPosition(), Vector3(0.0f, 10.0f, 0.0f), TEST_LOCATION );

  // 1. check that padding works. some padding:
  tableView.SetCellPadding(Size(5.0f, 10.0f));
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( tableView.GetCellPadding(), Size(5.0f, 10.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( actor1.GetCurrentPosition(), Vector3(5.0f, 10.0f, 0.0f), TEST_LOCATION );
  END_TEST;
}

// Test adjusting the metric values for the cell.
int UtcDaliTableViewMetricsFixed(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTableViewMetricsFixed");

  TableView tableView;
  Actor actor1;
  Actor actor2;
  Actor actor3;

  SetupTableViewAndActors(tableView, actor1, actor2, actor3);
  application.SendNotification();
  application.Render();

  // 1. check that with no fixed width/heights, actors are in default position.
  DALI_TEST_EQUALS( actor1.GetCurrentPosition(), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2.GetCurrentPosition(), Vector3(10.0f, 0.0f, 0.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( actor3.GetCurrentPosition(), Vector3(0.0f, 10.0f, 0.0f), TEST_LOCATION );

  // 2. check that with a fixed width & height, actors to the right and below are offsetted.
  tableView.SetFixedWidth(0, 20.0f);
  tableView.SetFixedHeight(0, 50.0f);
  DALI_TEST_EQUALS( tableView.GetFixedWidth(0), 20.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( tableView.GetFixedHeight(0), 50.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor1.GetCurrentPosition(), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2.GetCurrentPosition(), Vector3(20.0f, 0.0f, 0.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( actor3.GetCurrentPosition(), Vector3(0.0f, 50.0f, 0.0f), TEST_LOCATION );
  END_TEST;
}

// Test adjusting the metric values for the cell.
int UtcDaliTableViewMetricsRelative(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTableViewMetricsRelative");

  TableView tableView;
  Actor actor1;
  Actor actor2;
  Actor actor3;

  SetupTableViewAndActors(tableView, actor1, actor2, actor3);
  application.SendNotification();
  application.Render();

  // 1. check that with no relative width/heights, actors are in default position.
  DALI_TEST_EQUALS( actor1.GetCurrentPosition(), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2.GetCurrentPosition(), Vector3(10.0f, 0.0f, 0.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( actor3.GetCurrentPosition(), Vector3(0.0f, 10.0f, 0.0f), TEST_LOCATION );

  // 2. check that with a relative width & height, actors to the right and below are offsetted.
  tableView.SetRelativeWidth(0, 0.3f); // cell 0,0 occupies 30%x50% of the grid (i.e. 30x50 pixels)
  tableView.SetRelativeHeight(0, 0.5f);
  DALI_TEST_EQUALS( tableView.GetRelativeWidth(0), 0.3f, TEST_LOCATION );
  DALI_TEST_EQUALS( tableView.GetRelativeHeight(0), 0.5f, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor1.GetCurrentPosition(), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2.GetCurrentPosition(), Vector3(30.0f, 0.0f, 0.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( actor3.GetCurrentPosition(), Vector3(0.0f, 50.0f, 0.0f), TEST_LOCATION );
  END_TEST;
}


// Test animation duration setting.
int UtcDaliTableViewAnimation(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTableViewAnimation");
  TableView tableView = TableView::New(10,10);
  DALI_TEST_CHECK(tableView);

  tableView.SetLayoutAnimationDuration(5.0f);
  DALI_TEST_EQUALS(tableView.GetLayoutAnimationDuration(), 5.0f, TEST_LOCATION);

  tableView.SetLayoutAnimationDuration(2.5f);
  DALI_TEST_EQUALS(tableView.GetLayoutAnimationDuration(), 2.5f, TEST_LOCATION);
  END_TEST;
}

// Test Adding/Removing/Finding Children.
int UtcDaliTableViewChild(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTableViewChild");

  // Create a 10x10 table-view
  TableView tableView = TableView::New(10,10);
  DALI_TEST_CHECK( tableView );

  // Check if actor doesn't exist.
  DALI_TEST_CHECK( !tableView.GetChildAt(TableView::CellPosition(0,0)) );

  // Add an actor to it at 0,0
  Actor actor = Actor::New();
  tableView.AddChild(actor, TableView::CellPosition());

  // Check if exists.
  DALI_TEST_CHECK( tableView.GetChildAt(TableView::CellPosition(0,0)) );

  // Remove this actor
  tableView.RemoveChildAt(TableView::CellPosition());

  // Check if actor no longer exists.
  DALI_TEST_CHECK( !tableView.GetChildAt(TableView::CellPosition(0,0)) );

  // Add actor to it again, but at 2,5
  tableView.AddChild(actor, TableView::CellPosition(2,5));

  // Add another actor somewhere else 7,8
  Actor actor2 = Actor::New();
  tableView.AddChild(actor2, TableView::CellPosition(7,8));

  Actor searchActor;

  // Check that no actor exists in a few random places.
  DALI_TEST_CHECK( !tableView.GetChildAt(TableView::CellPosition(0,0)) );
  DALI_TEST_CHECK( !tableView.GetChildAt(TableView::CellPosition(2,1)) );
  DALI_TEST_CHECK( !tableView.GetChildAt(TableView::CellPosition(6,3)) );
  DALI_TEST_CHECK( !tableView.GetChildAt(TableView::CellPosition(9,5)) );

  // Check for actors at actual positions.
  searchActor = tableView.GetChildAt(TableView::CellPosition(2,5));
  DALI_TEST_CHECK( searchActor == actor);

  searchActor = tableView.GetChildAt(TableView::CellPosition(7,8));
  DALI_TEST_CHECK( searchActor == actor2);

  // Create a second table, and add already added Child to new one.
  TableView tableView2 = TableView::New(5,5);
  tableView2.AddChild(actor, TableView::CellPosition(2,2));
  DALI_TEST_CHECK( tableView2.GetChildAt(TableView::CellPosition(2,2)) );
  END_TEST;
}

// Test calling Add on it's own (to invoke the OnChildAdd)
int UtcDaliTableViewAdd(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTableViewAdd");

  // Create a 4x1 table-view, and just keep adding.
  TableView tableView = TableView::New(1,4);
  DALI_TEST_CHECK( tableView );

  for(unsigned int i = 0;i<16;i++)
  {
    Actor currentActor = Actor::New();
    TableView::CellPosition position = TableView::CellPosition();
    tableView.Add( currentActor );
    tableView.FindChildPosition(currentActor, position);
    tet_printf("%dx%d (%d,%d)\n", tableView.GetColumns(), tableView.GetRows(), position.columnIndex, position.rowIndex);

    DALI_TEST_EQUALS((position.rowIndex * 4 + position.columnIndex), i, TEST_LOCATION);
  }
  END_TEST;
}

// Test cell modification.
int UtcDaliTableViewCells(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTableViewCells");

  // Create a 10x10 table-view
  TableView tableView = TableView::New(10,10);
  DALI_TEST_CHECK( tableView );

  // Add a few actors to the table.
  Actor actor1 = Actor::New();
  Actor actor2 = Actor::New();
  Actor actor3 = Actor::New();
  actor1.SetName("Actor1");
  actor2.SetName("Actor2");
  actor3.SetName("Actor3");

  // note: positions are specified in reversed cartesian coords - row,col (i.e. y,x)
  tableView.AddChild(actor1, TableView::CellPosition(0,0));
  tableView.AddChild(actor2, TableView::CellPosition(5,5));
  tableView.AddChild(actor3, TableView::CellPosition(7,2));

  DALI_TEST_CHECK( tableView.GetRows() == 10 && tableView.GetColumns() == 10 );

  // Add a row between actor1 and actor2 | insert column on actor1 and see what happens...
  tableView.InsertRow(3);
  tableView.InsertColumn(0);
  DALI_TEST_CHECK( tableView.GetRows() == 11 && tableView.GetColumns() == 11 );

  TableView::CellPosition cellPosition;
  bool result;

  result = tableView.FindChildPosition(actor1, cellPosition);
  DALI_TEST_CHECK( result && cellPosition.rowIndex == 0 && cellPosition.columnIndex == 1);
  result = tableView.FindChildPosition(actor2, cellPosition);
  DALI_TEST_CHECK( result && cellPosition.rowIndex == 6 && cellPosition.columnIndex == 6);
  result = tableView.FindChildPosition(actor3, cellPosition);
  DALI_TEST_CHECK( result && cellPosition.rowIndex == 8 && cellPosition.columnIndex == 3);

  // Delete a row between actor2 and actor3 | delete column on actor2 and see what happens...
  tableView.DeleteRow(7);
  tableView.DeleteColumn(6);
  DALI_TEST_CHECK( tableView.GetRows() == 10 && tableView.GetColumns() == 10 );

  result = tableView.FindChildPosition(actor1, cellPosition);
  DALI_TEST_CHECK( result && cellPosition.rowIndex == 0 && cellPosition.columnIndex == 1);
  result = tableView.FindChildPosition(actor2, cellPosition);
  DALI_TEST_CHECK( !result );
  result = tableView.FindChildPosition(actor3, cellPosition);
  DALI_TEST_CHECK( result && cellPosition.rowIndex == 7 && cellPosition.columnIndex == 3);

  // Delete the other two remaining actors by a row delete and a column delete.
  std::vector<Actor> actorsRemoved;
  tableView.DeleteRow(0, actorsRemoved);
  tet_printf("Row Delete >> Actors Removed: %d {", actorsRemoved.size());
  for(size_t i = 0;i<actorsRemoved.size();i++) tet_printf("%d => %s, ", i, actorsRemoved[i].GetName().c_str());
  tet_printf("}\n");
  DALI_TEST_EQUALS( static_cast<int>(actorsRemoved.size()), 1, TEST_LOCATION );
  DALI_TEST_CHECK( actorsRemoved[0] == actor1 );

  actorsRemoved.clear();
  tableView.DeleteColumn(3, actorsRemoved);
  tet_printf("Column Delete >> Actors Removed: %d {", actorsRemoved.size());
  for(size_t i = 0;i<actorsRemoved.size();i++) tet_printf("%d => %s, ", i, actorsRemoved[i].GetName().c_str());
  tet_printf("}\n");
  DALI_TEST_EQUALS( static_cast<int>(actorsRemoved.size()), 1, TEST_LOCATION );
  DALI_TEST_CHECK( actorsRemoved[0] == actor3 );

  DALI_TEST_CHECK( tableView.GetRows() == 9 && tableView.GetColumns() == 9 );

  tableView.AddChild(actor1, TableView::CellPosition(5,8));
  tableView.Resize(100,100);
  DALI_TEST_CHECK( tableView.GetRows() == 100 && tableView.GetColumns() == 100 );

  tableView.AddChild(actor2, TableView::CellPosition(69,57));
  DALI_TEST_CHECK( tableView.FindChildPosition(actor1, cellPosition) && tableView.FindChildPosition(actor2, cellPosition) );

  tableView.Resize(20,20);
  DALI_TEST_CHECK( tableView.FindChildPosition(actor1, cellPosition) && !tableView.FindChildPosition(actor2, cellPosition) );

  actorsRemoved.clear();
  tableView.Resize(1,1, actorsRemoved);
  DALI_TEST_CHECK( !tableView.FindChildPosition(actor1, cellPosition) && !tableView.FindChildPosition(actor2, cellPosition) );
  DALI_TEST_EQUALS( static_cast<int>(actorsRemoved.size()), 1, TEST_LOCATION );
  DALI_TEST_CHECK( actorsRemoved[0] == actor1 );

  // Add child outside table size, forcing a resize.
  tableView.AddChild(actor1, TableView::CellPosition(100, 100, 1, 1));
  DALI_TEST_CHECK( tableView.GetRows() == 101 && tableView.GetColumns() == 101 );

  // Add child outside table size, forcing a resize.
  tableView.AddChild(actor1, TableView::CellPosition(110, 110, 5, 5));
  DALI_TEST_CHECK( tableView.GetRows() == 115 && tableView.GetColumns() == 115 );

  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliTableViewChildAssert(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTableViewChildAssert");

  // Create a 10x10 table-view
  TableView tableView = TableView::New(10,10);
  DALI_TEST_CHECK( tableView );
  Actor childActor;

  try
  {
    tableView.AddChild( childActor, TableView::CellPosition(0,0,5,5) );
    // should assert
    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException &e )
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_EQUALS(e.mCondition, "child", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTableViewMetricsAssert(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTableViewChildAssert");

  // Create a 10x10 table-view
  TableView tableView = TableView::New(10,10);
  DALI_TEST_CHECK( tableView );

  // fixeds...

  try
  {
    tableView.SetFixedHeight( 10, 1.0f );

    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException &e)
  {
    tet_printf("1. Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_EQUALS(e.mCondition, "rowIndex < mFixedHeights.size()", TEST_LOCATION);
  }

  try
  {
    tableView.GetFixedHeight( 10 );

    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException &e)
  {
    tet_printf("2. Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_EQUALS(e.mCondition, "rowIndex < mFixedHeights.size()", TEST_LOCATION);
  }

  try
  {
    tableView.SetFixedWidth( 10, 1.0f );

    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException &e)
  {
    tet_printf("3. Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_EQUALS(e.mCondition, "columnIndex < mFixedWidths.size()", TEST_LOCATION);
  }

  try
  {
    tableView.GetFixedWidth( 10 );

    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException &e)
  {
    tet_printf("4. Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_EQUALS(e.mCondition, "columnIndex < mFixedWidths.size()", TEST_LOCATION);
  }

  // relatives...

  try
  {
    tableView.SetRelativeHeight( 10, 0.1f );

    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException &e)
  {
    tet_printf("5. Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_EQUALS(e.mCondition, "rowIndex < mRelativeHeights.size()", TEST_LOCATION);
  }

  try
  {
    tableView.GetRelativeHeight( 10 );

    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException &e)
  {
    tet_printf("6. Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_EQUALS(e.mCondition, "rowIndex < mRelativeHeights.size()", TEST_LOCATION);
  }

  try
  {
    tableView.SetRelativeWidth( 10, 0.1f );

    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException &e)
  {
    tet_printf("7. Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_EQUALS(e.mCondition, "columnIndex < mRelativeWidths.size()", TEST_LOCATION);
  }

  try
  {
    tableView.GetRelativeWidth( 10 );

    tet_result(TET_FAIL);
  }
  catch( Dali::DaliException &e)
  {
    tet_printf("8. Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_EQUALS(e.mCondition, "columnIndex < mRelativeWidths.size()", TEST_LOCATION);
  }
  END_TEST;
}
