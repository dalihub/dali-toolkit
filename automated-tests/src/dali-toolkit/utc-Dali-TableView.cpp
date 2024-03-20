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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

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
const char* const PROPERTY_NAME_ROWS           = "rows";
const char* const PROPERTY_NAME_COLUMNS        = "columns";
const char* const PROPERTY_NAME_CELL_PADDING   = "cellPadding";
const char* const PROPERTY_NAME_LAYOUT_ROWS    = "layoutRows";
const char* const PROPERTY_NAME_LAYOUT_COLUMNS = "layoutColumns";
const Vector2     CELL_SIZE(10, 10);

static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

struct Constraint100
{
  Constraint100()
  {
  }

  /**
   * function operator to apply the parent size
   */
  void operator()(Dali::Vector3& current, const PropertyInputContainer& /* inputs */)
  {
    current.x = current.y = current.z = 100.0f;
  }
};

// Convenience function to quickly set up a 10x10 table with each cell being 10x10 pixels in size by default.
static void SetupTableViewAndActors(Integration::Scene scene, TableView& tableView, Actor& actor1, Actor& actor2, Actor& actor3)
{
  tableView = TableView::New(10, 10); // 10 by 10 grid.
  DALI_TEST_CHECK(tableView);

  scene.Add(tableView);
  tableView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  actor1 = Actor::New();
  actor2 = Actor::New();
  actor3 = Actor::New();

  actor1.SetProperty(Actor::Property::SIZE, CELL_SIZE);
  actor2.SetProperty(Actor::Property::SIZE, CELL_SIZE);
  actor3.SetProperty(Actor::Property::SIZE, CELL_SIZE);

  tableView.AddChild(actor1, TableView::CellPosition(0, 0));
  tableView.AddChild(actor2, TableView::CellPosition(0, 1));
  tableView.AddChild(actor3, TableView::CellPosition(1, 0));
}

} // namespace

int UtcDaliTableViewCtorCopyP(void)
{
  ToolkitTestApplication application;

  TableView actor1 = TableView::New(10, 10);
  TableView actor2(actor1);

  DALI_TEST_EQUALS(actor1, actor2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTableViewNew(void)
{
  ToolkitTestApplication application;

  TableView tableView = TableView::New(10, 10);
  DALI_TEST_CHECK(tableView);

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = application.GetCore().GetObjectRegistry();
  DALI_TEST_CHECK(registry);

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect(&TestCallback);
  {
    TableView tableView = TableView::New(10, 10);
  }
  DALI_TEST_CHECK(gObjectCreatedCallBackCalled);
  END_TEST;
}

// Test adjusting the metric values for the cell.
int UtcDaliTableViewMetricsPadding(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTableViewMetricsPadding");

  TableView tableView;
  Actor     actor1;
  Actor     actor2;
  Actor     actor3;

  SetupTableViewAndActors(application.GetScene(), tableView, actor1, actor2, actor3);

  // 1. check that padding works. no padding:
  tableView.SetCellPadding(Size(0.0f, 0.0f));
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(tableView.GetCellPadding(), Size(0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor1.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(10.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor3.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(0.0f, 10.0f, 0.0f), TEST_LOCATION);

  // 1. check that padding works. some padding:
  tableView.SetCellPadding(Size(5.0f, 10.0f));
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(tableView.GetCellPadding(), Size(5.0f, 10.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor1.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(5.0f, 10.0f, 0.0f), TEST_LOCATION);
  END_TEST;
}

// Test adjusting the metric values for the cell.
int UtcDaliTableViewMetricsFit(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTableViewMetricsFit");

  TableView tableView;
  Actor     actor1;
  Actor     actor2;
  Actor     actor3;

  SetupTableViewAndActors(application.GetScene(), tableView, actor1, actor2, actor3);
  application.SendNotification();
  application.Render();

  // 1. check that with no fixed width/heights, actors are in default position.
  DALI_TEST_EQUALS(actor1.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(10.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor3.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(0.0f, 10.0f, 0.0f), TEST_LOCATION);

  // 2. check that with a fixed width & height, actors to the right and below are offsetted.
  tableView.SetFitHeight(0);
  tableView.SetFitWidth(0);
  DALI_TEST_EQUALS(tableView.IsFitHeight(0), true, TEST_LOCATION);
  DALI_TEST_EQUALS(tableView.IsFitWidth(0), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor1.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(10.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor3.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(0.0f, 10.0f, 0.0f), TEST_LOCATION);

  tableView.SetProperty(Dali::Actor::Property::LAYOUT_DIRECTION, Dali::LayoutDirection::RIGHT_TO_LEFT);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor1.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(90.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(80.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor3.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(90.0f, 10.0f, 0.0f), TEST_LOCATION);

  tableView.SetProperty(Dali::Actor::Property::LAYOUT_DIRECTION, Dali::LayoutDirection::LEFT_TO_RIGHT);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor1.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(10.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor3.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(0.0f, 10.0f, 0.0f), TEST_LOCATION);

  END_TEST;
}

// Test adjusting the metric values for the cell.
int UtcDaliTableViewMetricsFixed(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTableViewMetricsFixed");

  TableView tableView;
  Actor     actor1;
  Actor     actor2;
  Actor     actor3;

  SetupTableViewAndActors(application.GetScene(), tableView, actor1, actor2, actor3);
  application.SendNotification();
  application.Render();

  // 1. check that with no fixed width/heights, actors are in default position.
  DALI_TEST_EQUALS(actor1.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(10.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor3.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(0.0f, 10.0f, 0.0f), TEST_LOCATION);

  // 2. check that with a fixed width & height, actors to the right and below are offsetted.
  tableView.SetFixedWidth(0, 20.0f);
  tableView.SetFixedHeight(0, 50.0f);
  DALI_TEST_EQUALS(tableView.GetFixedWidth(0), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tableView.GetFixedHeight(0), 50.0f, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor1.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(20.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor3.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(0.0f, 50.0f, 0.0f), TEST_LOCATION);
  END_TEST;
}

// Test adjusting the metric values for the cell.
int UtcDaliTableViewMetricsRelative(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTableViewMetricsRelative");

  TableView tableView;
  Actor     actor1;
  Actor     actor2;
  Actor     actor3;

  SetupTableViewAndActors(application.GetScene(), tableView, actor1, actor2, actor3);
  application.SendNotification();
  application.Render();

  // 1. check that with no relative width/heights, actors are in default position.
  DALI_TEST_EQUALS(actor1.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(10.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor3.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(0.0f, 10.0f, 0.0f), TEST_LOCATION);

  // 2. check that with a relative width & height, actors to the right and below are offsetted.
  tableView.SetRelativeWidth(0, 0.3f); // cell 0,0 occupies 30%x50% of the grid (i.e. 30x50 pixels)
  tableView.SetRelativeHeight(0, 0.5f);
  DALI_TEST_EQUALS(tableView.GetRelativeWidth(0), 0.3f, TEST_LOCATION);
  DALI_TEST_EQUALS(tableView.GetRelativeHeight(0), 0.5f, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor1.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor2.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(30.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor3.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(0.0f, 50.0f, 0.0f), TEST_LOCATION);
  END_TEST;
}

// Test Adding/Removing/Finding Children.
int UtcDaliTableViewChild(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTableViewChild");

  // Create a 10x10 table-view
  TableView tableView = TableView::New(10, 10);
  DALI_TEST_CHECK(tableView);

  // Check if actor doesn't exist.
  DALI_TEST_CHECK(!tableView.GetChildAt(TableView::CellPosition(0, 0)));

  // Add an actor to it at 0,0
  Actor actor = Actor::New();
  tableView.AddChild(actor, TableView::CellPosition());

  // Check if exists.
  DALI_TEST_CHECK(tableView.GetChildAt(TableView::CellPosition(0, 0)));

  // Remove this actor
  tableView.RemoveChildAt(TableView::CellPosition());

  // Check if actor no longer exists.
  DALI_TEST_CHECK(!tableView.GetChildAt(TableView::CellPosition(0, 0)));

  // Add actor to it again, but at 2,5
  tableView.AddChild(actor, TableView::CellPosition(2, 5));

  // Add another actor somewhere else 7,8
  Actor actor2 = Actor::New();
  tableView.AddChild(actor2, TableView::CellPosition(7, 8));

  Actor searchActor;

  // Check that no actor exists in a few random places.
  DALI_TEST_CHECK(!tableView.GetChildAt(TableView::CellPosition(0, 0)));
  DALI_TEST_CHECK(!tableView.GetChildAt(TableView::CellPosition(2, 1)));
  DALI_TEST_CHECK(!tableView.GetChildAt(TableView::CellPosition(6, 3)));
  DALI_TEST_CHECK(!tableView.GetChildAt(TableView::CellPosition(9, 5)));

  // Check for actors at actual positions.
  searchActor = tableView.GetChildAt(TableView::CellPosition(2, 5));
  DALI_TEST_CHECK(searchActor == actor);

  searchActor = tableView.GetChildAt(TableView::CellPosition(7, 8));
  DALI_TEST_CHECK(searchActor == actor2);

  // Create a second table, and add already added Child to new one.
  TableView tableView2 = TableView::New(5, 5);
  tableView2.AddChild(actor, TableView::CellPosition(2, 2));
  DALI_TEST_CHECK(tableView2.GetChildAt(TableView::CellPosition(2, 2)));
  END_TEST;
}

// Test calling Add on it's own (to invoke the OnChildAdd)
int UtcDaliTableViewAdd(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTableViewAdd");

  // Create a 4x1 table-view, and just keep adding.
  TableView tableView = TableView::New(1, 4);
  DALI_TEST_CHECK(tableView);

  for(unsigned int i = 0; i < 16; i++)
  {
    Actor                   currentActor = Actor::New();
    TableView::CellPosition position     = TableView::CellPosition();
    tableView.Add(currentActor);
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
  TableView tableView = TableView::New(10, 10);
  DALI_TEST_CHECK(tableView);

  // Add a few actors to the table.
  Actor actor1 = Actor::New();
  Actor actor2 = Actor::New();
  Actor actor3 = Actor::New();
  actor1.SetProperty(Dali::Actor::Property::NAME, "Actor1");
  actor2.SetProperty(Dali::Actor::Property::NAME, "Actor2");
  actor3.SetProperty(Dali::Actor::Property::NAME, "Actor3");

  // note: positions are specified in reversed cartesian coords - row,col (i.e. y,x)
  tableView.AddChild(actor1, TableView::CellPosition(0, 0));
  tableView.AddChild(actor2, TableView::CellPosition(5, 5));
  tableView.AddChild(actor3, TableView::CellPosition(7, 2));

  DALI_TEST_CHECK(tableView.GetRows() == 10 && tableView.GetColumns() == 10);

  // Add a row between actor1 and actor2 | insert column on actor1 and see what happens...
  tableView.InsertRow(3);
  tableView.InsertColumn(0);
  DALI_TEST_CHECK(tableView.GetRows() == 11 && tableView.GetColumns() == 11);

  TableView::CellPosition cellPosition;
  bool                    result;

  result = tableView.FindChildPosition(actor1, cellPosition);
  DALI_TEST_CHECK(result && cellPosition.rowIndex == 0 && cellPosition.columnIndex == 1);
  result = tableView.FindChildPosition(actor2, cellPosition);
  DALI_TEST_CHECK(result && cellPosition.rowIndex == 6 && cellPosition.columnIndex == 6);
  result = tableView.FindChildPosition(actor3, cellPosition);
  DALI_TEST_CHECK(result && cellPosition.rowIndex == 8 && cellPosition.columnIndex == 3);

  // Delete a row between actor2 and actor3 | delete column on actor2 and see what happens...
  tableView.DeleteRow(7);
  tableView.DeleteColumn(6);
  DALI_TEST_CHECK(tableView.GetRows() == 10 && tableView.GetColumns() == 10);

  result = tableView.FindChildPosition(actor1, cellPosition);
  DALI_TEST_CHECK(result && cellPosition.rowIndex == 0 && cellPosition.columnIndex == 1);
  result = tableView.FindChildPosition(actor2, cellPosition);
  DALI_TEST_CHECK(!result);
  result = tableView.FindChildPosition(actor3, cellPosition);
  DALI_TEST_CHECK(result && cellPosition.rowIndex == 7 && cellPosition.columnIndex == 3);

  // Delete the other two remaining actors by a row delete and a column delete.
  std::vector<Actor> actorsRemoved;
  tableView.DeleteRow(0, actorsRemoved);
  tet_printf("Row Delete >> Actors Removed: %d {", actorsRemoved.size());
  for(size_t i = 0; i < actorsRemoved.size(); i++) tet_printf("%d => %s, ", i, actorsRemoved[i].GetProperty<std::string>(Dali::Actor::Property::NAME).c_str());
  tet_printf("}\n");
  DALI_TEST_EQUALS(static_cast<int>(actorsRemoved.size()), 1, TEST_LOCATION);
  DALI_TEST_CHECK(actorsRemoved[0] == actor1);

  actorsRemoved.clear();
  tableView.DeleteColumn(3, actorsRemoved);
  tet_printf("Column Delete >> Actors Removed: %d {", actorsRemoved.size());
  for(size_t i = 0; i < actorsRemoved.size(); i++) tet_printf("%d => %s, ", i, actorsRemoved[i].GetProperty<std::string>(Dali::Actor::Property::NAME).c_str());
  tet_printf("}\n");
  DALI_TEST_EQUALS(static_cast<int>(actorsRemoved.size()), 1, TEST_LOCATION);
  DALI_TEST_CHECK(actorsRemoved[0] == actor3);

  DALI_TEST_CHECK(tableView.GetRows() == 9 && tableView.GetColumns() == 9);

  tableView.AddChild(actor1, TableView::CellPosition(5, 8));
  tableView.Resize(100, 100);
  DALI_TEST_CHECK(tableView.GetRows() == 100 && tableView.GetColumns() == 100);

  tableView.AddChild(actor2, TableView::CellPosition(69, 57));
  DALI_TEST_CHECK(tableView.FindChildPosition(actor1, cellPosition) && tableView.FindChildPosition(actor2, cellPosition));

  tableView.Resize(20, 20);
  DALI_TEST_CHECK(tableView.FindChildPosition(actor1, cellPosition) && !tableView.FindChildPosition(actor2, cellPosition));

  actorsRemoved.clear();
  tableView.Resize(1, 1, actorsRemoved);
  DALI_TEST_CHECK(!tableView.FindChildPosition(actor1, cellPosition) && !tableView.FindChildPosition(actor2, cellPosition));
  DALI_TEST_EQUALS(static_cast<int>(actorsRemoved.size()), 1, TEST_LOCATION);
  DALI_TEST_CHECK(actorsRemoved[0] == actor1);

  // Add child outside table size, forcing a resize.
  tableView.AddChild(actor1, TableView::CellPosition(100, 100, 1, 1));
  DALI_TEST_CHECK(tableView.GetRows() == 101 && tableView.GetColumns() == 101);

  // Add child outside table size, forcing a resize.
  tableView.AddChild(actor1, TableView::CellPosition(110, 110, 5, 5));
  DALI_TEST_CHECK(tableView.GetRows() == 115 && tableView.GetColumns() == 115);

  // Set the alignment of the cell
  tableView.SetCellAlignment(TableView::CellPosition(100, 100, 1, 1), HorizontalAlignment::CENTER, VerticalAlignment::CENTER);
  tableView.SetCellAlignment(TableView::CellPosition(110, 110, 5, 5), HorizontalAlignment::LEFT, VerticalAlignment::TOP);

  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliTableViewChildAssert(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTableViewChildAssert");

  // Create a 10x10 table-view
  TableView tableView = TableView::New(10, 10);
  DALI_TEST_CHECK(tableView);
  Actor childActor;

  try
  {
    tableView.AddChild(childActor, TableView::CellPosition(0, 0, 5, 5));
    // should assert
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_EQUALS(e.condition, "child", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTableViewMetricsAssert(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTableViewChildAssert");

  // Create a 10x10 table-view
  TableView tableView = TableView::New(10, 10);
  DALI_TEST_CHECK(tableView);

  // fixeds...

  try
  {
    tableView.SetFixedHeight(10, 1.0f);

    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_EQUALS(e.condition, "rowIndex < mRowData.Size()", TEST_LOCATION);
  }

  try
  {
    tableView.GetFixedHeight(10);

    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_EQUALS(e.condition, "rowIndex < mRowData.Size()", TEST_LOCATION);
  }

  try
  {
    tableView.SetFixedWidth(10, 1.0f);

    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_EQUALS(e.condition, "columnIndex < mColumnData.Size()", TEST_LOCATION);
  }

  try
  {
    tableView.GetFixedWidth(10);

    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_EQUALS(e.condition, "columnIndex < mColumnData.Size()", TEST_LOCATION);
  }

  // relatives...

  try
  {
    tableView.SetRelativeHeight(10, 0.1f);

    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_EQUALS(e.condition, "rowIndex < mRowData.Size()", TEST_LOCATION);
  }

  try
  {
    tableView.GetRelativeHeight(10);

    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_EQUALS(e.condition, "rowIndex < mRowData.Size()", TEST_LOCATION);
  }

  try
  {
    tableView.SetRelativeWidth(10, 0.1f);

    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_EQUALS(e.condition, "columnIndex < mColumnData.Size()", TEST_LOCATION);
  }

  try
  {
    tableView.GetRelativeWidth(10);

    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_EQUALS(e.condition, "columnIndex < mColumnData.Size()", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTableViewSetGetProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTableViewSetGetProperty");

  // Create a 1x1 table-view
  TableView tableView = TableView::New(1, 1);
  tableView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  DALI_TEST_CHECK(tableView);

  // Test "rows" property
  DALI_TEST_CHECK(tableView.GetPropertyIndex(PROPERTY_NAME_ROWS) == TableView::Property::ROWS);

  tableView.SetProperty(TableView::Property::ROWS, 7);

  DALI_TEST_CHECK(tableView.GetRows() == 7u);
  DALI_TEST_CHECK(tableView.GetProperty(TableView::Property::ROWS).Get<int>() == 7);

  // Test "columns" property
  DALI_TEST_CHECK(tableView.GetPropertyIndex(PROPERTY_NAME_COLUMNS) == TableView::Property::COLUMNS);

  tableView.SetProperty(TableView::Property::COLUMNS, 8);

  DALI_TEST_CHECK(tableView.GetColumns() == 8u);
  DALI_TEST_CHECK(tableView.GetProperty(TableView::Property::COLUMNS).Get<int>() == 8);

  // Test "cellPadding" property
  DALI_TEST_CHECK(tableView.GetPropertyIndex(PROPERTY_NAME_CELL_PADDING) == TableView::Property::CELL_PADDING);

  tableView.SetProperty(TableView::Property::CELL_PADDING, Size(6.f, 8.f));

  DALI_TEST_EQUALS(tableView.GetCellPadding(), Size(6.f, 8.f), TEST_LOCATION);
  DALI_TEST_EQUALS(tableView.GetProperty(TableView::Property::CELL_PADDING).Get<Vector2>(), Vector2(6.f, 8.f), TEST_LOCATION);

  //{ "policy": "fixed", "value": 30.0 },
  Property::Map item1;
  item1["policy"] = "fixed";
  item1["value"]  = 30.f;
  //{ "policy": "relative", "value": 0.2 },
  Property::Map item2;
  item2["policy"] = "relative";
  item2["value"]  = 0.2f;
  //{ "policy": "fill"},
  Property::Map item3;
  item3["policy"] = "fill";
  //{ "policy": "fit"},
  Property::Map item4;
  item4["policy"] = "fit";

  // Test "layoutRows" property
  DALI_TEST_CHECK(tableView.GetPropertyIndex(PROPERTY_NAME_LAYOUT_ROWS) == TableView::Property::LAYOUT_ROWS);

  /*
   * "layoutRows":
   *  {
   *    "1": { "policy": "fixed", "value": 30 },
   *    "3": { "policy": "relative", "value": 0.2 },
   *    "4": { "policy": "fill" },
   *    "6": { "policy": "fit" },
   *   }
   */
  Property::Map layoutRows;
  layoutRows["1"] = item1;
  layoutRows["3"] = item2;
  layoutRows["4"] = item3;
  layoutRows["6"] = item4;
  tableView.SetProperty(TableView::Property::LAYOUT_ROWS, layoutRows);

  DALI_TEST_EQUALS(tableView.GetFixedHeight(1u), 30.f, TEST_LOCATION);
  DALI_TEST_EQUALS(tableView.GetRelativeHeight(3u), 0.2f, TEST_LOCATION);

  Property::Map layoutRowsGet = tableView.GetProperty(TableView::Property::LAYOUT_ROWS).Get<Property::Map>();

  DALI_TEST_EQUALS(layoutRowsGet.GetKey(1).compare(layoutRows.GetKey(0)), 0, TEST_LOCATION);
  Property::Map* childMap = layoutRowsGet.GetValue(1).GetMap();
  DALI_TEST_CHECK(childMap->Find("policy")->Get<std::string>().compare("fixed") == 0);
  DALI_TEST_EQUALS(childMap->Find("value")->Get<float>(), 30.f, TEST_LOCATION);

  childMap = layoutRowsGet.GetValue(3).GetMap();
  DALI_TEST_CHECK(layoutRowsGet.GetKey(3).compare(layoutRows.GetKey(1)) == 0);
  DALI_TEST_CHECK(childMap->Find("policy")->Get<std::string>().compare("relative") == 0);
  DALI_TEST_EQUALS(childMap->Find("value")->Get<float>(), 0.2f, TEST_LOCATION);

  childMap = layoutRowsGet.GetValue(4).GetMap();
  DALI_TEST_CHECK(layoutRowsGet.GetKey(4).compare(layoutRows.GetKey(2)) == 0);
  DALI_TEST_CHECK(childMap->Find("policy")->Get<std::string>().compare("fill") == 0);

  childMap = layoutRowsGet.GetValue(6).GetMap();
  DALI_TEST_CHECK(layoutRowsGet.GetKey(6).compare(layoutRows.GetKey(3)) == 0);
  DALI_TEST_CHECK(childMap->Find("policy")->Get<std::string>().compare("fit") == 0);

  // Test "layoutColumns" property
  DALI_TEST_CHECK(tableView.GetPropertyIndex(PROPERTY_NAME_LAYOUT_COLUMNS) == TableView::Property::LAYOUT_COLUMNS);

  /*
   * "layoutColumns":
   *  {
   *    "2": { "policy": "relative", "value": 0.2 },
   *    "3": { "policy": "fixed", "value": 30 },
   *    "5": { "policy": "fit" },
   *    "7": { "policy": "fill" }
   *   }
   */
  Property::Map layoutColumns;
  layoutColumns["2"] = item2;
  layoutColumns["3"] = item1;
  layoutColumns["5"] = item4;
  layoutColumns["7"] = item3;
  tableView.SetProperty(TableView::Property::LAYOUT_COLUMNS, layoutColumns);

  DALI_TEST_EQUALS(tableView.GetRelativeWidth(2u), 0.2f, TEST_LOCATION);
  DALI_TEST_EQUALS(tableView.GetFixedWidth(3u), 30.f, TEST_LOCATION);

  Property::Map layoutColumnsGet = tableView.GetProperty(TableView::Property::LAYOUT_COLUMNS).Get<Property::Map>();

  DALI_TEST_CHECK(layoutColumnsGet.GetKey(2).compare(layoutColumns.GetKey(0)) == 0);
  childMap = layoutColumnsGet.GetValue(2).GetMap();
  DALI_TEST_CHECK(childMap->Find("policy")->Get<std::string>().compare("relative") == 0);
  DALI_TEST_EQUALS(childMap->Find("value")->Get<float>(), 0.2f, TEST_LOCATION);

  childMap = layoutColumnsGet.GetValue(3).GetMap();
  DALI_TEST_CHECK(layoutColumnsGet.GetKey(3).compare(layoutColumns.GetKey(1)) == 0);
  DALI_TEST_CHECK(childMap->Find("policy")->Get<std::string>().compare("fixed") == 0);
  DALI_TEST_EQUALS(childMap->Find("value")->Get<float>(), 30.f, TEST_LOCATION);

  childMap = layoutColumnsGet.GetValue(5).GetMap();
  DALI_TEST_CHECK(layoutColumnsGet.GetKey(5).compare(layoutColumns.GetKey(2)) == 0);
  DALI_TEST_CHECK(childMap->Find("policy")->Get<std::string>().compare("fit") == 0);

  childMap = layoutColumnsGet.GetValue(7).GetMap();
  DALI_TEST_CHECK(layoutColumnsGet.GetKey(7).compare(layoutColumns.GetKey(3)) == 0);
  DALI_TEST_CHECK(childMap->Find("policy")->Get<std::string>().compare("fill") == 0);

  END_TEST;
}

int UtcDaliTableViewChildProperties(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTableViewChildProperties");

  // Create a 10x10 table-view
  TableView tableView = TableView::New(10, 10);
  application.GetScene().Add(tableView);
  tableView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  DALI_TEST_CHECK(tableView);

  // Create a child actor with the custom properties
  Actor child1 = Actor::New();
  child1.SetProperty(TableView::ChildProperty::CELL_INDEX, Vector2(3, 4));
  tableView.Add(child1);
  // Check for actors at actual positions.
  DALI_TEST_CHECK(tableView.GetChildAt(TableView::CellPosition(3, 4)) == child1);

  // Create a second child actor with the custom properties
  Actor child2     = Actor::New();
  float rowSpan    = 3.f;
  float columnSpan = 2.f;
  child2.SetProperty(TableView::ChildProperty::CELL_INDEX, Vector2(6, 1));
  child2.SetProperty(TableView::ChildProperty::ROW_SPAN, rowSpan);
  child2.SetProperty(TableView::ChildProperty::COLUMN_SPAN, columnSpan);
  tableView.Add(child2);
  // Check for actors at actual positions.
  for(int i = 0; i < rowSpan; i++)
  {
    for(int j = 0; j < columnSpan; j++)
    {
      DALI_TEST_CHECK(tableView.GetChildAt(TableView::CellPosition(6 + i, 1 + j)) == child2);
    }
  }

  // Create a third child actor with the cell alignment properties
  Actor child3 = Actor::New();
  child3.SetProperty(Actor::Property::SIZE, Vector2(5.f, 5.f));
  child3.SetProperty(TableView::ChildProperty::CELL_HORIZONTAL_ALIGNMENT, "center");
  child3.SetProperty(TableView::ChildProperty::CELL_VERTICAL_ALIGNMENT, "bottom");
  tableView.Add(child3);

  // store the actor in the first available cell
  DALI_TEST_CHECK(tableView.GetChildAt(TableView::CellPosition(0, 0)) == child3)
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(child3.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT), AnchorPoint::TOP_LEFT, TEST_LOCATION);
  DALI_TEST_EQUALS(child3.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN), ParentOrigin::TOP_LEFT, TEST_LOCATION);
  DALI_TEST_EQUALS(child3.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(2.5f, 5.0f, 0.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTableViewGetChildAtN(void)
{
  ToolkitTestApplication application;

  TableView tableView = TableView::New(10, 10);

  Actor actor = tableView.GetChildAt(TableView::CellPosition(200, 200));
  DALI_TEST_CHECK(!actor);

  END_TEST;
}

int UtcDaliTableViewAddChildN(void)
{
  ToolkitTestApplication application;

  TableView tableView = TableView::New(10, 10);
  DALI_TEST_CHECK(tableView.AddChild(Actor::New(), TableView::CellPosition(0, 0)));
  DALI_TEST_CHECK(!tableView.AddChild(Actor::New(), TableView::CellPosition(0, 0)));

  END_TEST;
}

int UtcDaliTableViewInsertRowAtZero(void)
{
  ToolkitTestApplication application;

  TableView tableView = TableView::New(10, 10);
  DALI_TEST_CHECK(tableView.AddChild(Actor::New(), TableView::CellPosition(0, 0, 10, 10)));
  tableView.InsertRow(0);

  DALI_TEST_CHECK(tableView.GetRows() == 11);

  END_TEST;
}

int UtcDaliTableViewDeleteRowAtZero(void)
{
  ToolkitTestApplication application;

  TableView tableView = TableView::New(10, 10);
  DALI_TEST_CHECK(tableView.AddChild(Actor::New(), TableView::CellPosition(0, 0, 10, 10)));
  tableView.DeleteRow(0);

  DALI_TEST_CHECK(tableView.GetRows() == 9);

  END_TEST;
}

int UtcDaliTableViewInsertColumnAtZero(void)
{
  ToolkitTestApplication application;

  TableView tableView = TableView::New(10, 10);
  DALI_TEST_CHECK(tableView.AddChild(Actor::New(), TableView::CellPosition(0, 0, 10, 10)));
  tableView.InsertColumn(0);

  DALI_TEST_CHECK(tableView.GetColumns() == 11);

  END_TEST;
}

int UtcDaliTableViewDeleteColumnAtZero(void)
{
  ToolkitTestApplication application;

  TableView tableView = TableView::New(10, 10);
  DALI_TEST_CHECK(tableView.AddChild(Actor::New(), TableView::CellPosition(0, 0, 10, 10)));
  tableView.DeleteColumn(0);

  DALI_TEST_CHECK(tableView.GetColumns() == 9);

  END_TEST;
}

int UtcDaliTableViewTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK(typeRegistry);

  TypeInfo typeInfo = typeRegistry.GetTypeInfo("TableView");
  DALI_TEST_CHECK(typeInfo);

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK(handle);

  TableView view = TableView::DownCast(handle);
  DALI_TEST_CHECK(view);

  END_TEST;
}

int UtcDaliTableViewKeyboardFocus(void)
{
  ToolkitTestApplication application;

  TableView tableView = TableView::New(4, 4);
  tableView.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  tableView.SetProperty(Dali::Actor::Property::NAME, "TableView");

  for(int row = 0; row < 4; ++row)
  {
    for(int col = 0; col < 4; ++col)
    {
      Control            control = Control::New();
      std::ostringstream str;
      str << row << "-" << col;
      control.SetProperty(Dali::Actor::Property::NAME, str.str());
      control.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
      tableView.AddChild(control, TableView::CellPosition(row, col));
    }
  }

  application.GetScene().Add(tableView);

  application.SendNotification();
  application.Render();

  Actor firstFocusActor = Toolkit::Internal::GetImplementation(tableView).GetNextKeyboardFocusableActor(Actor(), Control::KeyboardFocus::RIGHT, true);
  DALI_TEST_CHECK(firstFocusActor);
  DALI_TEST_CHECK(firstFocusActor.GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-0");

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  manager.SetFocusGroupLoop(true);
  manager.SetCurrentFocusActor(firstFocusActor);

  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-0");
  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-1");
  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-2");
  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-3");
  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-0");

  manager.MoveFocus(Control::KeyboardFocus::LEFT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-3");
  manager.MoveFocus(Control::KeyboardFocus::LEFT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-2");
  manager.MoveFocus(Control::KeyboardFocus::LEFT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-1");
  manager.MoveFocus(Control::KeyboardFocus::LEFT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-0");
  manager.MoveFocus(Control::KeyboardFocus::LEFT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "3-3");

  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-0");
  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-1");

  manager.MoveFocus(Control::KeyboardFocus::DOWN);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-1");
  manager.MoveFocus(Control::KeyboardFocus::DOWN);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "2-1");
  manager.MoveFocus(Control::KeyboardFocus::DOWN);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "3-1");
  manager.MoveFocus(Control::KeyboardFocus::DOWN);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-1");

  manager.MoveFocus(Control::KeyboardFocus::UP);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "3-1");
  manager.MoveFocus(Control::KeyboardFocus::UP);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "2-1");
  manager.MoveFocus(Control::KeyboardFocus::UP);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-1");
  manager.MoveFocus(Control::KeyboardFocus::UP);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-1");

  END_TEST;
}

int UtcDaliTableViewKeyboardFocusInNestedTableView(void)
{
  ToolkitTestApplication application;

  TableView tableView = TableView::New(3, 3);
  tableView.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  tableView.SetProperty(Dali::Actor::Property::NAME, "TableView");

  for(int row = 0; row < 3; ++row)
  {
    for(int col = 0; col < 3; ++col)
    {
      std::ostringstream str;
      str << row << "-" << col;

      if(row == 1 && col == 1)
      {
        // Add a nested 2x2 table view in the middle cell of the parent table view
        TableView childTableView = TableView::New(2, 2);
        childTableView.SetProperty(Dali::Actor::Property::NAME, str.str());

        for(int childRow = 0; childRow < 2; childRow++)
        {
          for(int childCol = 0; childCol < 2; childCol++)
          {
            Control            control = Control::New();
            std::ostringstream nameStr;
            nameStr << row << "-" << col << "-" << childRow << "-" << childCol;
            control.SetProperty(Dali::Actor::Property::NAME, nameStr.str());
            control.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
            childTableView.AddChild(control, TableView::CellPosition(childRow, childCol));
          }
        }
        tableView.AddChild(childTableView, TableView::CellPosition(row, col));
      }
      else
      {
        Control control = Control::New();
        control.SetProperty(Dali::Actor::Property::NAME, str.str());
        control.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
        tableView.AddChild(control, TableView::CellPosition(row, col));
      }
    }
  }

  application.GetScene().Add(tableView);

  application.SendNotification();
  application.Render();

  Actor firstFocusActor = Toolkit::Internal::GetImplementation(tableView).GetNextKeyboardFocusableActor(Actor(), Control::KeyboardFocus::RIGHT, true);
  DALI_TEST_CHECK(firstFocusActor);
  DALI_TEST_CHECK(firstFocusActor.GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-0");

  KeyboardFocusManager manager = KeyboardFocusManager::Get();
  manager.SetFocusGroupLoop(false);
  manager.SetCurrentFocusActor(firstFocusActor);

  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-0");
  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-1");
  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-2");
  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-0");
  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-1-0-0");
  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-1-0-1");
  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-1-1-0");
  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-1-1-1");
  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-2");
  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "2-0");
  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "2-1");
  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "2-2");

  manager.MoveFocus(Control::KeyboardFocus::LEFT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "2-1");
  manager.MoveFocus(Control::KeyboardFocus::LEFT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "2-0");
  manager.MoveFocus(Control::KeyboardFocus::LEFT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-2");
  manager.MoveFocus(Control::KeyboardFocus::LEFT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-1-1-1");
  manager.MoveFocus(Control::KeyboardFocus::LEFT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-1-1-0");
  manager.MoveFocus(Control::KeyboardFocus::LEFT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-1-0-1");
  manager.MoveFocus(Control::KeyboardFocus::LEFT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-1-0-0");
  manager.MoveFocus(Control::KeyboardFocus::LEFT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-0");
  manager.MoveFocus(Control::KeyboardFocus::LEFT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-2");
  manager.MoveFocus(Control::KeyboardFocus::LEFT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-1");
  manager.MoveFocus(Control::KeyboardFocus::LEFT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-0");

  manager.MoveFocus(Control::KeyboardFocus::RIGHT);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-1");
  manager.MoveFocus(Control::KeyboardFocus::DOWN);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-1-0-0");
  manager.MoveFocus(Control::KeyboardFocus::DOWN);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-1-1-0");
  manager.MoveFocus(Control::KeyboardFocus::DOWN);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "2-1");

  manager.MoveFocus(Control::KeyboardFocus::UP);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-1-1-1");
  manager.MoveFocus(Control::KeyboardFocus::UP);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "1-1-0-1");
  manager.MoveFocus(Control::KeyboardFocus::UP);
  DALI_TEST_CHECK(manager.GetCurrentFocusActor().GetProperty<std::string>(Dali::Actor::Property::NAME) == "0-1");

  END_TEST;
}
