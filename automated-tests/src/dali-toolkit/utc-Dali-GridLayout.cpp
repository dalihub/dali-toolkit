/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <toolkit-event-thread-callback.h>

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/absolute-layout.h>
#include <dali-toolkit/devel-api/layouting/grid.h>
#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>

#include <../custom-layout.h>

#include <layout-utils.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_grid_layouting_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_grid_layouting_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliLayouting_GridLayout01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_GridLayout01 2 Column, 4 Items");

  const auto NUMBER_OF_COLUMNS = 2;
  const auto NUMBER_OF_ITEMS = 4;

  tet_printf( "Testing %d columns with %d items\n", NUMBER_OF_COLUMNS, NUMBER_OF_ITEMS );

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  auto absoluteLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootControl, absoluteLayout );
  rootControl.SetName( "AbsoluteLayout" );
  stage.Add( rootControl );

  auto gridContainer = Control::New();
  auto gridLayout = Grid::New();
  gridLayout.SetNumberOfColumns( NUMBER_OF_COLUMNS );
  gridContainer.SetName( "GridLayout");
  DevelControl::SetLayout( gridContainer, gridLayout );
  gridContainer.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  gridContainer.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );

  std::vector< Control > controls;
  for( auto i=0; i < NUMBER_OF_ITEMS; i++ )
  {
    controls.push_back( CreateLeafControl( 100, 100 ) );
  }

  for( auto&& iter : controls )
  {
    gridContainer.Add( iter );
  }

  rootControl.Add( gridContainer );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

 // Grid will layout first 2 items on first row then last 2 on second row.
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Item sizes will not be changed
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  tet_printf( "Confirm number of columns is as set\n");
  DALI_TEST_EQUALS( gridLayout.GetNumberOfColumns(), NUMBER_OF_COLUMNS, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_GridLayout02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_GridLayout02");

  const auto NUMBER_OF_COLUMNS = 3;
  const auto NUMBER_OF_ITEMS = 7;

  tet_printf( "Testing %d columns with %d items\n", NUMBER_OF_COLUMNS, NUMBER_OF_ITEMS );

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  auto absoluteLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootControl, absoluteLayout );
  rootControl.SetName( "AbsoluteLayout" );
  stage.Add( rootControl );

  auto gridContainer = Control::New();
  auto gridLayout = Grid::New();
  gridLayout.SetNumberOfColumns( NUMBER_OF_COLUMNS );
  gridContainer.SetName( "GridLayout");
  DevelControl::SetLayout( gridContainer, gridLayout );
  gridContainer.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  gridContainer.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );

  std::vector< Control > controls;
  for( auto i=0; i < NUMBER_OF_ITEMS; i++ )
  {
    controls.push_back( CreateLeafControl( 100, 100 ) );
  }

  for( auto&& iter : controls )
  {
    gridContainer.Add( iter );
  }

  rootControl.Add( gridContainer );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // grid  layouts out 3 items per row, which is 480x800.
  // Row 1
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 200.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  // Row 2
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[4].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[5].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 200.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  // Row 3
  DALI_TEST_EQUALS( controls[6].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[4].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[5].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[6].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_GridLayout03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_GridLayout03 Grid Padding");

  const auto NUMBER_OF_COLUMNS = 2;
  const auto NUMBER_OF_ITEMS = 4;

  tet_printf( "Testing %d columns with %d items\n", NUMBER_OF_COLUMNS, NUMBER_OF_ITEMS );

  Extents GRID_PADDING( Extents( 10, 10, 20, 20 ) ); // start,end,top,bottom

  tet_printf( "Testing with Padding 10,10,20,20\n");

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  auto absoluteLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootControl, absoluteLayout );
  rootControl.SetName( "AbsoluteLayout" );
  stage.Add( rootControl );

  auto gridContainer = Control::New();
  auto gridLayout = Grid::New();
  gridLayout.SetNumberOfColumns( NUMBER_OF_COLUMNS );
  gridContainer.SetName( "GridLayout");
  DevelControl::SetLayout( gridContainer, gridLayout );
  gridContainer.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  gridContainer.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );
  gridContainer.SetProperty( Control::Property::PADDING, GRID_PADDING );

  std::vector< Control > controls;
  for( auto i=0; i < NUMBER_OF_ITEMS; i++ )
  {
    controls.push_back( CreateLeafControl( 100, 100 ) );
  }

  for( auto&& iter : controls )
  {
    gridContainer.Add( iter );
  }

  rootControl.Add( gridContainer );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  tet_infoline(" UtcDaliLayouting_GridLayout03 Grid Padding 2 Column, 4 Items");
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f + GRID_PADDING.start , 0.0f + GRID_PADDING.top, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f + GRID_PADDING.start, 0.0f + GRID_PADDING.top, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f + GRID_PADDING.start, 100.0f + GRID_PADDING.top , 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f + GRID_PADDING.start, 100.0f + GRID_PADDING.top, 0.0f ), 0.0001f, TEST_LOCATION );

  tet_infoline(" UtcDaliLayouting_GridLayout03 Size of Grid should include padding");
  DALI_TEST_EQUALS( gridContainer.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f * NUMBER_OF_COLUMNS + GRID_PADDING.start + + GRID_PADDING.end,
                                                                                 100.0f * ( NUMBER_OF_ITEMS / NUMBER_OF_COLUMNS ) +
                                                                                 GRID_PADDING.top + GRID_PADDING.bottom,
                                                                                 0.0f ), 0.0001f, TEST_LOCATION );

  tet_infoline(" UtcDaliLayouting_GridLayout03 Item sizes unchanged");
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_GridLayout04(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_GridLayout04 Child Margin");

  const auto NUMBER_OF_COLUMNS = 2;
  const auto NUMBER_OF_ITEMS = 4;

  tet_printf( "Testing %d columns with %d items\n", NUMBER_OF_COLUMNS, NUMBER_OF_ITEMS );

  Extents GRID_PADDING( Extents( 10, 10, 20, 20 ) ); // start,end,top,bottom
  Extents ITEM_MARGIN( Extents( 10, 10, 5, 5 ) ); // start,end,top,bottom

  tet_printf( "Testing with Margin 10,10,5,5\n");

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  auto absoluteLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootControl, absoluteLayout );
  rootControl.SetName( "AbsoluteLayout" );
  stage.Add( rootControl );

  auto gridContainer = Control::New();
  auto gridLayout = Grid::New();
  gridLayout.SetNumberOfColumns( NUMBER_OF_COLUMNS );
  gridContainer.SetName( "GridLayout");
  DevelControl::SetLayout( gridContainer, gridLayout );
  gridContainer.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  gridContainer.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );
  gridContainer.SetProperty( Control::Property::PADDING, GRID_PADDING );

  std::vector< Control > controls;
  for( auto i=0; i < NUMBER_OF_ITEMS; i++ )
  {
    auto control = CreateLeafControl( 100, 100 );
    control.SetProperty(Toolkit::Control::Property::MARGIN, ITEM_MARGIN );
    controls.push_back( control );
  }

  for( auto&& iter : controls )
  {
    gridContainer.Add( iter );
  }

  rootControl.Add( gridContainer );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ),
                    Vector3( 0.0f + GRID_PADDING.start + ITEM_MARGIN.start,
                    0.0f + GRID_PADDING.top + ITEM_MARGIN.top,
                    0.0f ),
                    0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ),
                    Vector3( 100.0f + GRID_PADDING.start + ITEM_MARGIN.start *2 + ITEM_MARGIN.end,
                    0.0f + GRID_PADDING.top + ITEM_MARGIN.top,
                    0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ),
                    Vector3( 0.0f + GRID_PADDING.start + ITEM_MARGIN.start,
                    100.0f + GRID_PADDING.top + ITEM_MARGIN.top*2 + ITEM_MARGIN.bottom,
                    0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ),
                    Vector3( 100.0f + GRID_PADDING.start + ITEM_MARGIN.start*2 + ITEM_MARGIN.end,
                    100.0f + GRID_PADDING.top + ITEM_MARGIN.top*2 + ITEM_MARGIN.bottom,
                    0.0f ), 0.0001f, TEST_LOCATION );

  tet_infoline(" UtcDaliLayouting_GridLayout03 Size of Grid should include padding and margins");

  const auto NUMBER_OF_ROWS = ( NUMBER_OF_ITEMS / NUMBER_OF_COLUMNS );

  DALI_TEST_EQUALS( gridContainer.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f * NUMBER_OF_COLUMNS + GRID_PADDING.start + GRID_PADDING.end +
                                                                                          ITEM_MARGIN.start *NUMBER_OF_COLUMNS + ITEM_MARGIN.end *NUMBER_OF_COLUMNS,
                                                                                          100.0f * NUMBER_OF_ROWS +
                                                                                          GRID_PADDING.top + GRID_PADDING.bottom +
                                                                                          ITEM_MARGIN.bottom *NUMBER_OF_ROWS + ITEM_MARGIN.bottom *NUMBER_OF_ROWS,
                                                                                          0.0f ), 0.0001f, TEST_LOCATION );

  tet_infoline(" UtcDaliLayouting_GridLayout03 Item sizes unchanged");
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}


int UtcDaliLayouting_GridLayoutDownCast(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliLayouting_GridLayoutDownCast - Testing Downcast");

  Grid gridLayout = Grid::New();

  LayoutGroup layoutGroup( gridLayout );

  Grid gridLayoutCandidate = Grid::DownCast( layoutGroup );
  DALI_TEST_CHECK( gridLayoutCandidate );

  END_TEST;
}