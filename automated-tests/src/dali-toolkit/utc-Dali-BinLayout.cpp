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
#include <dali-toolkit/devel-api/layouting/bin-layout.h>
#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>
#include <dali-toolkit/devel-api/layouting/linear-layout.h>

#include <../custom-layout.h>

#include <layout-utils.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
// Turns the given control into a Root layout control and adds it to the stage.
void SetupRootLayoutControl( Control& rootControl )
{
  rootControl = Control::New();
  auto absoluteLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootControl, absoluteLayout );
  rootControl.SetName( "RootAbsoluteLayout" );
  Stage stage = Stage::GetCurrent();
  stage.Add( rootControl );
}

void CreateDefaultBinContainer( Control& binContainer )
{
  binContainer = Control::New();
  binContainer.SetName( "binContainer" );
  DevelControl::SetLayout( binContainer, BinLayout::New() );
  binContainer.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  binContainer.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
}

} // namespace

void utc_dali_toolkit_bin_layouting_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_bin_layouting_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliLayouting_BinLayoutDownCast(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliLayouting_BinLayoutDownCast - Testing Downcast");

  BinLayout binLayout = BinLayout::New();

  LayoutGroup layoutGroup( binLayout );

  BinLayout binLayoutCandidate = BinLayout::DownCast( layoutGroup );
  DALI_TEST_CHECK( binLayoutCandidate );

  END_TEST;
}

int UtcDaliLayouting_BinLayoutAssignment(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliLayouting_BinLayoutAssignment - Testing operator=");

  BinLayout binLayout = BinLayout::New();
  BinLayout binLayout2;

  binLayout2 = binLayout;

  DALI_TEST_EQUALS( binLayout2, binLayout, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_BinLayoutCopyConstructor(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliLayouting_BinLayoutCopyConstructor - Testing copy constructor");

  BinLayout binLayout1 = BinLayout::New();
  BinLayout binLayout2( binLayout1 );

  DALI_TEST_EQUALS( binLayout1, binLayout2, TEST_LOCATION );
  END_TEST;
}

int UtcDaliLayouting_BinLayout01(void)
{
  const auto NUMBER_OF_ITEMS = 4;

  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_BinLayout01 BinLayout WRAP_CONTENT");

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  auto absoluteLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootControl, absoluteLayout );
  rootControl.SetName( "AbsoluteLayout" );
  stage.Add( rootControl );

  auto binContainer = Control::New();
  auto binLayout = BinLayout::New();
  binContainer.SetName( "BinLayout");
  DevelControl::SetLayout( binContainer, binLayout );
  binContainer.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  binContainer.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );

  std::vector< Control > controls;
  for( auto i=0; i < NUMBER_OF_ITEMS; i++ )
  {
    controls.push_back( CreateLeafControl( 100, 100 ) );
  }

  for( auto&& iter : controls )
  {
    binContainer.Add( iter );
  }

  rootControl.Add( binContainer );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // Items will be laid out at the same position
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Item sizes will not be changed
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // BinLayout size to be that of greatest child dimensions
  DALI_TEST_EQUALS( binContainer.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_BinLayout02(void)
{
  const auto NUMBER_OF_ITEMS = 4;

  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_BinLayout02 BinLayout MATCH_PARENT");

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  auto absoluteLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootControl, absoluteLayout );
  rootControl.SetName( "AbsoluteLayout" );
  stage.Add( rootControl );

  auto binContainer = Control::New();
  auto binLayout = BinLayout::New();
  binContainer.SetName( "BinLayout");
  DevelControl::SetLayout( binContainer, binLayout );
  binContainer.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  binContainer.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::MATCH_PARENT );

  std::vector< Control > controls;
  for( auto i=0; i < NUMBER_OF_ITEMS; i++ )
  {
    controls.push_back( CreateLeafControl( 100, 100 ) );
  }

  for( auto&& iter : controls )
  {
    binContainer.Add( iter );
  }

  rootControl.Add( binContainer );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // Items will be laid out at the same position
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Item sizes will not be changed
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // BinLayout size to be that of greatest child dimensions
  DALI_TEST_EQUALS( binContainer.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_BinLayout03(void)
{
  const auto NUMBER_OF_ITEMS = 4;

  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_BinLayout03 Explicit child Positioning when Bin layout MATCH_PARENT");

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  auto absoluteLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootControl, absoluteLayout );
  rootControl.SetName( "AbsoluteLayout" );
  stage.Add( rootControl );

  auto binContainer = Control::New();
  auto binLayout = BinLayout::New();
  binContainer.SetName( "BinLayout");
  DevelControl::SetLayout( binContainer, binLayout );
  binContainer.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  binContainer.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::MATCH_PARENT );

  std::vector< Control > controls;
  for( auto i=0; i < NUMBER_OF_ITEMS; i++ )
  {
    controls.push_back( CreateLeafControl( 100, 100 ) );
  }

  for( auto&& iter : controls )
  {
    binContainer.Add( iter );
  }

  tet_infoline("Position child explicitly from top left");
  controls[0].SetProperty(Actor::Property::POSITION_X, 50.0f );

  tet_infoline("Position child explicitly from top left");
  controls[2].SetProperty(Actor::Property::POSITION_Y, 50.0f );

  rootControl.Add( binContainer );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // Items will be laid out at the same position
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 50.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Item sizes will not be changed
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // BinLayout size to be that of parent
  DALI_TEST_EQUALS( binContainer.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayoutingBinLayoutResizePolicy_01(void)
{
  /*
  Root
    |
  Control (LinearLayout Horizontal)
    |
  Control (BinLayout)
    |
  Control (ResizePolicy::FILL_TO_PARENT)
    |
  LeafControl
  */

  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayoutingBinLayoutResizePolicy_01 - Test ResizePolicy mapping with FILL_TO_PARENT on BinLayout child");

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  SetupRootLayoutControl( rootControl );

  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  hboxLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "hBox" );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  auto binContainer = Control::New();
  CreateDefaultBinContainer( binContainer );
  hbox.Add( binContainer );

  auto control = Control::New();
  control.SetName( "fillToParentControl" );
  DevelControl::SetLayoutingRequired( control, true );
  control.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  binContainer.Add( control );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );

  for( auto&& iter : controls )
  {
    control.Add( iter );
  }

  rootControl.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  tet_infoline("Testing child of BinLayout is size of parent");
  DALI_TEST_EQUALS( control.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  tet_infoline("Testing BinLayout's child control has not altered it's children's sizes ");
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  tet_infoline("Testing BinLayout is size of parent");
  DALI_TEST_EQUALS( binContainer.GetProperty<Vector3>( Actor::Property::SIZE ), hbox.GetProperty<Vector3>( Actor::Property::SIZE ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayoutingBinLayoutResizePolicy_02(void)
{
  /*
  Root
    |
  Control (LinearLayout Horizontal)
    |
  Control (BinLayout)
    |
  Control (ResizePolicy::SIZE_RELATIVE_TO_PARENT)
    |
  LeafControl
  */

  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayoutingBinLayoutResizePolicy_02 - Set ResizePolicy SIZE_RELATIVE_TO_PARENT on BinLayout child");

  const auto NUMBER_OF_ITEMS = 4;

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  SetupRootLayoutControl( rootControl );

  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  hboxLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "hBox" );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  auto binContainer = Control::New();
  CreateDefaultBinContainer( binContainer );
  hbox.Add( binContainer );

  auto control = Control::New();
  control.SetName( "fitToChildrenControl" );
  DevelControl::SetLayoutingRequired( control, true );
  control.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
  control.SetSizeModeFactor( Vector3( 0.50f, 1.0f, 1.0f ) );
  binContainer.Add( control );

  std::vector< Control > controls;
  for( auto i=0; i < NUMBER_OF_ITEMS; i++ )
  {
    controls.push_back( CreateLeafControl( 40, 40 ) );
  }

  for( auto&& iter : controls )
  {
    control.Add( iter );
  }

  rootControl.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  tet_infoline("Testing child of BinLayout is the defined relative size of parent");
  DALI_TEST_EQUALS( control.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 240.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  tet_infoline("Testing BinLayout's child control has not altered it's children's sizes ");
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  tet_infoline("Testing BinLayout is size of parent");
  DALI_TEST_EQUALS( binContainer.GetProperty<Vector3>( Actor::Property::SIZE ), hbox.GetProperty<Vector3>( Actor::Property::SIZE ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliBinLayoutResizePolicy_03(void)
{
  /*
  Root
    |
  Control (LinearLayout Horizontal)
    |
  Control (LayoutingRequired)
    |
  Control (ResizePolicy::SIZE_RELATIVE_TO_PARENT)
    |
  LeafControl
  */

  ToolkitTestApplication application;
  tet_infoline("UtcDaliBinLayoutResizePolicy_03 - Set ResizePolicy SIZE_FIXED_OFFSET_FROM_PARENT on BinLayout child");

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  SetupRootLayoutControl( rootControl );

  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  hboxLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "hBox" );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  auto binContainer = Control::New();
  CreateDefaultBinContainer( binContainer );
  hbox.Add( binContainer );

  hbox.Add( binContainer );

  auto control = Control::New();
  control.SetName( "fitToChildrenControl" );
  DevelControl::SetLayoutingRequired( control, true );
  control.SetResizePolicy( ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::ALL_DIMENSIONS );
  control.SetSizeModeFactor( Vector3( -100.0f, 10.0f, 0.0f ) );
  binContainer.Add( control );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );

  for( auto&& iter : controls )
  {
    control.Add( iter );
  }

  rootControl.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( DevelControl::IsLayoutingRequired( control ), true , 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( control.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 380.0f, 810.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}