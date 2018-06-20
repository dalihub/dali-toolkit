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

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/absolute-layout.h>
#include <dali-toolkit/devel-api/layouting/linear-layout.h>

#include <layout-utils.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_layouting_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_layouting_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliLayouting_HboxLayout01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_HboxLayout01");

  Stage stage = Stage::GetCurrent();
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  hboxLayout.SetCellPadding( LayoutSize( 10, 0 ) );
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 40 ) );
  controls.push_back( CreateLeafControl( 80, 40 ) );
  controls.push_back( CreateLeafControl( 100, 40 ) );

  for( auto&& iter : controls )
  {
    hbox.Add( iter );
  }
  hbox.SetParentOrigin( ParentOrigin::CENTER );
  hbox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // hbox centers elements vertically, it fills test harness stage, which is 480x800.
  // hbox left justifies elements
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 50.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 120.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 210.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Change a layout
  auto newHBoxLayout = LinearLayout::New();
  newHBoxLayout.SetCellPadding( LayoutSize( 10, 0 ) );
  DevelControl::SetLayout( hbox, newHBoxLayout );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 50.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 120.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 210.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_HboxLayout02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_HboxLayout02 Test nested hboxes with default spec of WRAP_CONTENT");

  Stage stage = Stage::GetCurrent();

  auto hbox1 = Control::New();
  auto hboxLayout1 = LinearLayout::New();
  DevelControl::SetLayout( hbox1, hboxLayout1 );

  auto hbox2 = Control::New();
  auto hboxLayout2 = LinearLayout::New();
  DevelControl::SetLayout( hbox2, hboxLayout2 );

  hbox1.SetName( "HBox1");
  hbox2.SetName( "HBox2");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 20, 40 ) );
  controls.push_back( CreateLeafControl( 30, 50 ) );
  controls.push_back( CreateLeafControl( 40, 60 ) );
  controls.push_back( CreateLeafControl( 50, 70 ) );

  controls.push_back( CreateLeafControl( 25, 40 ) );
  controls.push_back( CreateLeafControl( 35, 50 ) );
  controls.push_back( CreateLeafControl( 45, 60 ) );
  controls.push_back( CreateLeafControl( 55, 70 ) );

  int counter=0;
  for( auto&& iter : controls )
  {
    if( counter < 4 )
    {
      hbox1.Add( iter );
    }
    else
    {
      hbox2.Add( iter );
    }
    ++counter;
  }
  hbox1.SetParentOrigin( ParentOrigin::CENTER );
  hbox1.SetAnchorPoint( AnchorPoint::CENTER );
  hbox2.SetParentOrigin( ParentOrigin::CENTER );
  hbox2.SetAnchorPoint( AnchorPoint::CENTER );

  auto hbox3 = Control::New();
  auto hboxLayout3 = LinearLayout::New();
  DevelControl::SetLayout( hbox3, hboxLayout3 );

  hbox3.SetParentOrigin( ParentOrigin::CENTER );
  hbox3.SetName( "HBox3");
  hbox3.Add( hbox1 );
  hbox3.Add( hbox2 );

  stage.Add( hbox3 );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();


  // hbox centers elements vertically, it fills test harness stage, which is 480x800.
  // hbox left justifies elements
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3(  0.0f, 15.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 20.0f, 10.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 50.0f,  5.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 90.0f,  0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 20.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 30.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 50.0f, 70.0f, 0.0f ), 0.0001f, TEST_LOCATION );


  DALI_TEST_EQUALS( controls[4].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3(  0.0f, 15.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[5].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 25.0f, 10.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[6].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 60.0f,  5.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[7].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 105.0f,  0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[4].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 25.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[5].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 35.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[6].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 45.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[7].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 55.0f, 70.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Test hbox1 and 2 are sized to wrap their content
  DALI_TEST_EQUALS( hbox1.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 140.0f, 70.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( hbox2.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 160.0f, 70.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( hbox1.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 365.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( hbox2.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 140.0f, 365.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Test hbox3 matches parent (root layer)
  DALI_TEST_EQUALS( hbox3.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( hbox3.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}


int UtcDaliLayouting_HboxLayout03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_HboxLayout03 Test nested hboxes with MATCH_PARENT");

  Stage stage = Stage::GetCurrent();

  auto hbox1 = Control::New();
  auto hboxLayout1 = LinearLayout::New();
  DevelControl::SetLayout( hbox1, hboxLayout1 );

  auto hbox2 = Control::New();
  auto hboxLayout2 = LinearLayout::New();
  DevelControl::SetLayout( hbox2, hboxLayout2 );

  hbox1.SetName( "HBox1");
  hbox2.SetName( "HBox2");
  hbox1.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox1.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox2.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox2.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 20, 40 ) );
  controls.push_back( CreateLeafControl( 30, 50 ) );
  controls.push_back( CreateLeafControl( 40, 60 ) );
  controls.push_back( CreateLeafControl( 50, 70 ) );

  controls.push_back( CreateLeafControl( 25, 40 ) );
  controls.push_back( CreateLeafControl( 35, 50 ) );
  controls.push_back( CreateLeafControl( 45, 60 ) );
  controls.push_back( CreateLeafControl( 55, 70 ) );

  int counter=0;
  for( auto&& iter : controls )
  {
    if( counter < 4 )
    {
      hbox1.Add( iter );
    }
    else
    {
      hbox2.Add( iter );
    }
    ++counter;
  }
  hbox1.SetParentOrigin( ParentOrigin::CENTER );
  hbox1.SetAnchorPoint( AnchorPoint::CENTER );
  hbox2.SetParentOrigin( ParentOrigin::CENTER );
  hbox2.SetAnchorPoint( AnchorPoint::CENTER );

  auto hbox3 = Control::New();
  auto hboxLayout3 = LinearLayout::New();
  DevelControl::SetLayout( hbox3, hboxLayout3);

  hbox3.SetParentOrigin( ParentOrigin::CENTER );
  hbox3.SetName( "HBox3");
  hbox3.Add( hbox1 );
  hbox3.Add( hbox2 );

  stage.Add( hbox3 );

  //std::ostringstream oss;
  //DumpControlHierarchy( oss, Stage::GetCurrent().GetRootLayer() );
  //printf("Control hierarchy: \n%s\n", oss.str().c_str() );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();


  // hbox centers elements vertically, it fills test harness stage, which is 480x800.
  // hbox left justifies elements
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 20.0f,375.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 50.0f,370.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 90.0f,365.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 20.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 30.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 50.0f, 70.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[4].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3(  0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[5].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 25.0f, 375.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[6].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 60.0f, 370.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[7].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 105.0f,365.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[4].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 25.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[5].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 35.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[6].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 45.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[7].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 55.0f, 70.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( hbox1.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( hbox2.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( hbox1.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( hbox2.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 480.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Test hbox3 matches parent (root layer)
  DALI_TEST_EQUALS( hbox3.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( hbox3.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_HboxLayout04(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_HboxLayout04 Test nested hboxes with explicit WRAP_CONTENT");

  Stage stage = Stage::GetCurrent();

  auto hbox1 = Control::New();
  auto hboxLayout1 = LinearLayout::New();
  DevelControl::SetLayout( hbox1, hboxLayout1 );

  auto hbox2 = Control::New();
  auto hboxLayout2 = LinearLayout::New();
  DevelControl::SetLayout( hbox2, hboxLayout2 );

  hbox1.SetName( "HBox1"); // Default spec is to wrap content
  hbox2.SetName( "HBox2");
  hbox1.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  hbox1.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  hbox2.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  hbox2.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 80, 40 ) );
  controls.push_back( CreateLeafControl( 80, 50 ) );
  controls.push_back( CreateLeafControl( 80, 60 ) );
  controls.push_back( CreateLeafControl( 80, 70 ) );

  controls.push_back( CreateLeafControl( 80, 40 ) );
  controls.push_back( CreateLeafControl( 80, 50 ) );
  controls.push_back( CreateLeafControl( 80, 60 ) );
  controls.push_back( CreateLeafControl( 80, 70 ) );

  int counter=0;
  for( auto&& iter : controls )
  {
    if( counter < 4 )
    {
      hbox1.Add( iter );
    }
    else
    {
      hbox2.Add( iter );
    }
    ++counter;
  }

  controls[6].SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  auto hbox3 = Control::New();
  auto hboxLayout3 = LinearLayout::New();
  DevelControl::SetLayout( hbox3, hboxLayout3 );

  hbox3.SetParentOrigin( ParentOrigin::CENTER );
  hbox3.SetName( "HBox3");
  hbox3.Add( hbox1 );
  hbox3.Add( hbox2 );
  stage.Add( hbox3 );

  //std::ostringstream oss;
  //DumpControlHierarchy( oss, Stage::GetCurrent().GetRootLayer() );
  //printf("Control hierarchy: \n%s\n", oss.str().c_str() );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();


  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 15.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 80.0f,10.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 160.0f, 5.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 240.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 70.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[4].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3(  0.0f, 15.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[5].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 80.0f, 10.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[6].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 160.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[7].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 240.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[4].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[5].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[6].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 70.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[7].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 70.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( hbox1.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 320.0f, 70.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( hbox2.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 320.0f, 70.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( hbox1.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 365.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( hbox2.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 320.0f, 365.0f, 0.0f ), 0.0001f, TEST_LOCATION );


  // Test hbox3 matches parent (root layer)
  DALI_TEST_EQUALS( hbox3.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( hbox3.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_HboxLayout05(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_HboxLayout05 - Set children size explicitly via width & height specification");

  Stage stage = Stage::GetCurrent();
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  hboxLayout.SetCellPadding( LayoutSize( 10, 0 ) );
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 40 ) );
  controls.push_back( CreateLeafControl( 80, 40 ) );
  controls.push_back( CreateLeafControl( 100, 40 ) );

  for( auto&& iter : controls )
  {
    hbox.Add( iter );
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 100 );
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, 50 );
  }

  hbox.SetParentOrigin( ParentOrigin::CENTER );
  hbox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // hbox centers elements vertically, it fills test harness stage, which is 480x800.
  // hbox left justifies elements
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 375.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 110.0f, 375.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 220.0f, 375.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 330.0f, 375.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_HboxLayout06(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_HboxLayout06 - Test nested layouts");

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  auto absoluteLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootControl, absoluteLayout );
  rootControl.SetName( "AbsoluteLayout" );
  stage.Add( rootControl );

  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  hboxLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox" );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 40 ) );

  for( auto&& iter : controls )
  {
    hbox.Add( iter );
  }
  hbox.SetParentOrigin( ParentOrigin::CENTER );
  hbox.SetAnchorPoint( AnchorPoint::CENTER );
  rootControl.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // hbox centers elements vertically, it fills test harness stage, which is 480x800.
  // hbox left justifies elements
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Change a layout
  auto newHBoxLayout = LinearLayout::New();
  newHBoxLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );
  DevelControl::SetLayout( hbox, newHBoxLayout );

  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

// Padding tests

int UtcDaliLayouting_HboxLayout_Padding01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_HboxLayout_Padding01 - Adding Padding to a single child");

  Stage stage = Stage::GetCurrent();
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 40 ) );
  controls.push_back( CreateLeafControl( 80, 40 ) );
  controls.push_back( CreateLeafControl( 100, 40 ) );

  const Extents CONTROL_PADDING = Extents(5, 10, 20, 2 );
  tet_printf( "\nAdding Padding to control at index %u \n", 1 );
  controls[1].SetProperty(Toolkit::Control::Property::PADDING, CONTROL_PADDING );

  for( auto&& iter : controls )
  {
    hbox.Add( iter );
  }
  hbox.SetParentOrigin( ParentOrigin::CENTER );
  hbox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // hbox centers elements vertically, it fills test harness stage, which is 480x800.
  // hbox left justifies elements
  tet_infoline("Test Child Actor Position");
  float xPositionOfControlBeingTested = 0.0f;
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested,
                                                                                            380.0f,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );
  xPositionOfControlBeingTested += 40.0f;

  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested,
                                                                                            380.0f - ( ( CONTROL_PADDING.top + CONTROL_PADDING.bottom) * 0.5f ),                                                                                            0.0f ),
                                                                                            0.0001f, TEST_LOCATION );

  xPositionOfControlBeingTested += 60.0f + CONTROL_PADDING.start + CONTROL_PADDING.end;
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  xPositionOfControlBeingTested += 80.0f;
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested,380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  tet_infoline("Test Child Actor Size");
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f + CONTROL_PADDING.start + CONTROL_PADDING.end,
                                                                                        40.0f + CONTROL_PADDING.top + CONTROL_PADDING.bottom , 0.0f ),
                                                                                        0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_HboxLayout_Padding02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_HboxLayout_Padding02 - Adding Padding to a all children");

  Stage stage = Stage::GetCurrent();
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 40 ) );
  controls.push_back( CreateLeafControl( 80, 40 ) );
  controls.push_back( CreateLeafControl( 100, 40 ) );

  const Extents CONTROL_PADDING = Extents(5, 10, 20, 2 );

  for( auto&& iter : controls )
  {
    iter.SetProperty(Toolkit::Control::Property::PADDING, CONTROL_PADDING );
    hbox.Add( iter );
  }
  hbox.SetParentOrigin( ParentOrigin::CENTER );
  hbox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // hbox centers elements vertically, it fills test harness stage, which is 480x800.
  // hbox left justifies elements
  tet_infoline("Test Child Actor Position");
  float xPositionOfControlBeingTested = 0.0f;
  float yPositionOfControlBeingTested = ( 800.0f * 0.5) - ( 0.5 * ( 40.0f + CONTROL_PADDING.top + CONTROL_PADDING.bottom ) );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested,
                                                                                            yPositionOfControlBeingTested,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );
  xPositionOfControlBeingTested += 40.0f + CONTROL_PADDING.start + CONTROL_PADDING.end;

  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested,
                                                                                            yPositionOfControlBeingTested,
                                                                                            0.0f ),
                                                                                            0.0001f, TEST_LOCATION );

  xPositionOfControlBeingTested += 60.0f + CONTROL_PADDING.start + CONTROL_PADDING.end;
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested,
                                                                                            yPositionOfControlBeingTested,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );

  xPositionOfControlBeingTested += 80.0f + CONTROL_PADDING.start + CONTROL_PADDING.end;
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested,
                                                                                            yPositionOfControlBeingTested,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );

  tet_infoline("Test Child Actor Size");
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f + CONTROL_PADDING.start + CONTROL_PADDING.end,
                                                                                        40.0f + CONTROL_PADDING.top + CONTROL_PADDING.bottom,
                                                                                        0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f + CONTROL_PADDING.start + CONTROL_PADDING.end,
                                                                                        40.0f + CONTROL_PADDING.top + CONTROL_PADDING.bottom,
                                                                                        0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f + CONTROL_PADDING.start + CONTROL_PADDING.end ,
                                                                                        40.0f + CONTROL_PADDING.top + CONTROL_PADDING.bottom,
                                                                                        0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f + CONTROL_PADDING.start + CONTROL_PADDING.end,
                                                                                        40.0f + CONTROL_PADDING.top + CONTROL_PADDING.bottom,
                                                                                        0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}


int UtcDaliLayouting_HboxLayout_Padding03(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_HboxLayout_Padding03 - Adding Changing padding on a single child");

  Stage stage = Stage::GetCurrent();
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 40, 40 ) );

  const Extents CONTROL_PADDING = Extents(5, 10, 20, 2 );
  tet_printf( "\nAdding Padding to control at index 1 \n" );
  controls[1].SetProperty(Toolkit::Control::Property::PADDING, CONTROL_PADDING );

  for( auto&& iter : controls )
  {
    hbox.Add( iter );
  }
  hbox.SetParentOrigin( ParentOrigin::CENTER );
  hbox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // hbox centers elements vertically, it fills test harness stage, which is 480x800.
  // hbox left justifies elements
  tet_infoline("Test Child Actor Position");
  float xPositionOfControlBeingTested = 0.0f;
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested,
                                                                                            380.0f,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );
  xPositionOfControlBeingTested += 40.0f;

  DALI_TEST_EQUALS( controls[ 1 ].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested,
                                                                                            380.0f - ( ( CONTROL_PADDING.top + CONTROL_PADDING.bottom) * 0.5f ),                                                                                            0.0f ),
                                                                                            0.0001f, TEST_LOCATION );

  xPositionOfControlBeingTested += 40.0f + CONTROL_PADDING.start + CONTROL_PADDING.end;
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  const Extents NEW_CONTROL_PADDING = Extents(10, 10, 20, 2 );
  tet_printf( "\nChanging Padding to control at index 1 \n" );
  controls[1].SetProperty(Toolkit::Control::Property::PADDING, NEW_CONTROL_PADDING );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  xPositionOfControlBeingTested = 0.0f; // reset

  tet_infoline("Test Child Actor Position");
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested,
                                                                                            380.0f,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );
  xPositionOfControlBeingTested += 40.0f;

  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested,
                                                                                            380.0f - ( ( NEW_CONTROL_PADDING.top + NEW_CONTROL_PADDING.bottom) * 0.5f ),                                                                                            0.0f ),
                                                                                            0.0001f, TEST_LOCATION );

  xPositionOfControlBeingTested += 40.0f + NEW_CONTROL_PADDING.start + NEW_CONTROL_PADDING.end;
  tet_printf( "\nIf x position %u then change has not been processed \n", 40 + 40 + CONTROL_PADDING.start + CONTROL_PADDING.end );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  tet_infoline("Test Child Actor Size");
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ) , Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ) , Vector3( 40.0f + NEW_CONTROL_PADDING.start + NEW_CONTROL_PADDING.end,
                                                                                        40.0f + NEW_CONTROL_PADDING.top + NEW_CONTROL_PADDING.bottom , 0.0f ),
                                                                                        0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ) , Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

// Margin Tests

int UtcDaliLayouting_HboxLayout_Margin01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_HboxLayout_Margin01 - Adding a margin to a single child");

  Stage stage = Stage::GetCurrent();
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 40 ) );
  controls.push_back( CreateLeafControl( 80, 40 ) );
  controls.push_back( CreateLeafControl( 100, 40 ) );

  const Extents CONTROL_MARGIN = Extents(5, 10, 20, 0 );
  tet_printf( "\nAdding Margin to control at index 1 \n" );
  controls[1].SetProperty(Toolkit::Control::Property::MARGIN, CONTROL_MARGIN );

  for( auto&& iter : controls )
  {
    hbox.Add( iter );
  }
  hbox.SetParentOrigin( ParentOrigin::CENTER );
  hbox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // hbox centers elements vertically, it fills test harness stage, which is 480x800.
  // hbox left justifies elements
  tet_infoline("Test Child Actor Position");
  auto xPositionOfControlBeingTested = 0.0f;
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested,
                                                                                            380.0f,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );
  xPositionOfControlBeingTested += 40.0f + CONTROL_MARGIN.start;

  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested,
                                                                                            380.0f + CONTROL_MARGIN.top, 0.0f ),
                                                                                            0.0001f, TEST_LOCATION );

  xPositionOfControlBeingTested += 60.0f + CONTROL_MARGIN.end;
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  xPositionOfControlBeingTested += 80.0f;
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( xPositionOfControlBeingTested,380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  tet_infoline("Test Child Actor Size is the same after Margin added");
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 40.0f , 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}


int UtcDaliLayouting_VboxLayout01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_VboxLayout01");

  Stage stage = Stage::GetCurrent();
  auto vbox = Control::New();
  auto vboxLayout = LinearLayout::New();
  vboxLayout.SetOrientation( LinearLayout::Orientation::VERTICAL );
  DevelControl::SetLayout( vbox, vboxLayout );
  vbox.SetName( "Vbox");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 60 ) );
  controls.push_back( CreateLeafControl( 80, 80 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );

  for( auto&& iter : controls )
  {
    vbox.Add( iter );
  }
  vbox.SetParentOrigin( ParentOrigin::CENTER );
  vbox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( vbox );

  controls[2].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  // Check it.
  DALI_TEST_EQUALS( controls[2].GetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION ), Property::Value( ChildLayoutData::MATCH_PARENT ), TEST_LOCATION );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // vbox centers elements horizontally, it fills test harness stage, which is 480x800.
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 220.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 210.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 190.0f, 180.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_VboxLayout02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_VboxLayout01");

  Stage stage = Stage::GetCurrent();

  // @todo Can't set specification properties on root control. Really need to make LayoutController a root
  // LayoutGroup for this to happen automatically.
  //
  // For this test, add an hbox instead
  auto rootControl = Control::New();
  auto absoluteLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootControl, absoluteLayout );
  rootControl.SetName( "AbsoluteLayout");
  stage.Add( rootControl );

  auto vbox = Control::New();
  auto vboxLayout = LinearLayout::New();
  vboxLayout.SetOrientation( LinearLayout::Orientation::VERTICAL );
  DevelControl::SetLayout( vbox, vboxLayout );
  vbox.SetName( "Vbox");
  rootControl.Add( vbox );

  vbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  vbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 60 ) );
  controls.push_back( CreateLeafControl( 80, 80 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );

  for( auto&& iter : controls )
  {
    vbox.Add( iter );
  }
  vbox.SetParentOrigin( ParentOrigin::CENTER );
  vbox.SetAnchorPoint( AnchorPoint::CENTER );

  controls[2].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  // Check it.
  DALI_TEST_EQUALS( controls[2].GetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION ), Property::Value( ChildLayoutData::MATCH_PARENT ), TEST_LOCATION );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( rootControl.GetProperty<Vector3>(Actor::Property::POSITION), Vector3(0,0,0),TEST_LOCATION);
  DALI_TEST_EQUALS( rootControl.GetProperty<Vector3>(Actor::Property::SIZE), Vector3(480,800,0),TEST_LOCATION);

  // vbox centers elements horizontally, it should wrap it's content horizontally, i.e. it should take the width of the largest element (100)
  DALI_TEST_EQUALS( vbox.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( vbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // 3rd control is set to match parent - this should also be 100 wide
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 30.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 20.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 180.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}


int UtcDaliLayouting_VboxLayout03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_VboxLayout03 test with cell padding set");

  Stage stage = Stage::GetCurrent();

  // @todo Can't set specification properties on root control. Really need to make LayoutController a root
  // LayoutGroup for this to happen automatically.
  //
  // For this test, add an hbox instead
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "Hbox");
  stage.Add( hbox );

  auto vbox = Control::New();
  auto vboxLayout = LinearLayout::New();
  vboxLayout.SetCellPadding( LayoutSize( 0, 10 ) );
  vboxLayout.SetOrientation( LinearLayout::Orientation::VERTICAL );

  DALI_TEST_EQUALS( vboxLayout.GetCellPadding(), LayoutSize( 0, 10 ), TEST_LOCATION );

  DevelControl::SetLayout( vbox, vboxLayout );
  vbox.SetName( "Vbox");
  hbox.Add( vbox );

  vbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  vbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 60 ) );
  controls.push_back( CreateLeafControl( 80, 80 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );

  for( auto&& iter : controls )
  {
    vbox.Add( iter );
  }
  vbox.SetParentOrigin( ParentOrigin::CENTER );
  vbox.SetAnchorPoint( AnchorPoint::CENTER );

  controls[2].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  // Check it.
  DALI_TEST_EQUALS( controls[2].GetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION ), Property::Value( ChildLayoutData::MATCH_PARENT ), TEST_LOCATION );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>(Actor::Property::POSITION), Vector3(0,0,0),TEST_LOCATION);
  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>(Actor::Property::SIZE), Vector3(480,800,0),TEST_LOCATION);

  // vbox centers elements horizontally, it should wrap it's content horizontally, i.e. it should take the width of the largest element (100)
  DALI_TEST_EQUALS( vbox.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( vbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // 3rd control is set to match parent - this should also be 100 wide
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 30.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 20.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 120.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 210.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}
