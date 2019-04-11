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
#include <dali-toolkit/devel-api/layouting/linear-layout.h>
#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>
#include <dali/devel-api/actors/actor-devel.h>

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

} // unnamed namespace

void utc_dali_toolkit_layouting_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_layouting_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliLayouting_LayoutLength_Types(void)
{
  tet_infoline("UtcDaliLayouting_LayoutLength Types");

  // testing that operators return correct type
  // operator+
  {
    auto value = LayoutLength( 10 ) + LayoutLength();
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }
  {
    auto value = LayoutLength( 10 ) + 20;
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }
  {
    auto value = 22 + LayoutLength( 10 );
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }
  {
    auto value = LayoutLength( 10 ) + 2.1f;
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }
  {
    auto value = 2.2f + LayoutLength( 10 );
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }
  // operator-
  {
    auto value = LayoutLength() - LayoutLength( 10 );
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }
  {
    auto value = LayoutLength( 10 ) - 99;
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }
  {
    auto value = 20 - LayoutLength( 10 );
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }
  {
    auto value = LayoutLength( 10 ) - 9.9f;
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }
  {
    auto value = 2.0f - LayoutLength( 10 );
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }

  // operator*
  {
    auto value = LayoutLength() * LayoutLength( 10 );
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }
  {
    auto value = LayoutLength( 10 ) * 2;
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }
  {
    auto value = 10 * LayoutLength( 10 );
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }
  {
    auto value = LayoutLength( 10 ) * 2.1f;
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }
  {
    auto value = 1.0f * LayoutLength( 10 );
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }
  // operator/
  {
    auto value = LayoutLength( 10 ) / 2.0f;
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }

  // combinations
  {
    auto value = ( LayoutLength( 10 ) * 2.0f ) / 1.5f;
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }
  {
    auto value = 20 + ( LayoutLength( 10 ) * 2.0f ) / 1.5f;
    DALI_TEST_EQUALS( typeid(value).name(), typeid(LayoutLength).name(), TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliLayouting_LayoutLength_Values(void)
{
  tet_infoline("UtcDaliLayouting_LayoutLength Values");

  // operator+
  {
    LayoutLength value = LayoutLength( 10 ) + LayoutLength();
    DALI_TEST_EQUALS( value.AsInteger(), 10.0f, 0.0001f, TEST_LOCATION );
  }
  {
    LayoutLength value = LayoutLength( 10 ) + 20;
    DALI_TEST_EQUALS( value.AsInteger(), 30.0f, 0.0001f, TEST_LOCATION );
  }
  {
    LayoutLength value = 22 - LayoutLength( 10 );
    DALI_TEST_EQUALS( value.AsInteger(), 12.0f, 0.0001f, TEST_LOCATION );
  }
  {
    LayoutLength value = LayoutLength( 10 ) + 2.1f;
    DALI_TEST_EQUALS( value.AsDecimal(), 12.1f, 0.0001f, TEST_LOCATION );
    DALI_TEST_EQUALS( value.AsTruncated(), 12.0f, 0.0001f, TEST_LOCATION );
  }
  {
    LayoutLength value = 2.3f - LayoutLength( 10 );
    DALI_TEST_EQUALS( value.AsDecimal(), -7.7f, 0.0001f, TEST_LOCATION );
    DALI_TEST_EQUALS( value.AsInteger(), -8.0f, 0.0001f, TEST_LOCATION );
    DALI_TEST_EQUALS( value.AsTruncated(), -7.0f, 0.0001f, TEST_LOCATION );
  }
  // operator-
  {
    LayoutLength value = LayoutLength() - LayoutLength( 10 );
    DALI_TEST_EQUALS( value.AsInteger(), -10.0f, 0.0001f, TEST_LOCATION );
  }
  {
    LayoutLength value = LayoutLength( 10 ) - 99;
    DALI_TEST_EQUALS( value.AsInteger(), -89.0f, 0.0001f, TEST_LOCATION );
  }
  {
    LayoutLength value = 20 - LayoutLength( 10 );
    DALI_TEST_EQUALS( value.AsInteger(), 10.0f, 0.0001f, TEST_LOCATION );
    DALI_TEST_EQUALS( value.AsTruncated(), 10.f, 0.0001f, TEST_LOCATION );
  }
  {
    LayoutLength value = LayoutLength( 10 ) - 9.9f;
    DALI_TEST_EQUALS( value.AsInteger(), 0.0f, 0.0001f, TEST_LOCATION );
    DALI_TEST_EQUALS( value.AsDecimal(), 0.1f, 0.0001f, TEST_LOCATION );
    DALI_TEST_EQUALS( value.AsTruncated(), 0.0f, 0.0001f, TEST_LOCATION );
  }
  {
    LayoutLength value = 10.9f - LayoutLength( 10 );
    DALI_TEST_EQUALS( value.AsInteger(), 1.0f, 0.0001f, TEST_LOCATION );
    DALI_TEST_EQUALS( value.AsDecimal(), 0.9f, 0.0001f, TEST_LOCATION );
    DALI_TEST_EQUALS( value.AsTruncated(), 0.0f, 0.0001f, TEST_LOCATION );
  }
  // operator*
  {
    LayoutLength value = LayoutLength() * LayoutLength( 10 );
    DALI_TEST_EQUALS( value.AsInteger(), 0.0f, 0.0001f, TEST_LOCATION );
  }
  {
    LayoutLength value = LayoutLength(1) * 10;
    DALI_TEST_EQUALS( value.AsInteger(), 10.0f, 0.0001f, TEST_LOCATION );
  }
  {
    LayoutLength value = 11 * LayoutLength( 10 );
    DALI_TEST_EQUALS( value.AsInteger(), 110.0f, 0.0001f, TEST_LOCATION );
  }
  {
    LayoutLength value = LayoutLength( 10 ) * 2.12f;
    DALI_TEST_EQUALS( value.AsDecimal(), 21.2f, 0.0001f, TEST_LOCATION );
    DALI_TEST_EQUALS( value.AsInteger(), 21.0f, 0.0001f, TEST_LOCATION );
    DALI_TEST_EQUALS( value.AsTruncated(), 21.0f, 0.0001f, TEST_LOCATION );
  }
  {
    LayoutLength value = 2.189f * LayoutLength( 10 );
    DALI_TEST_EQUALS( value.AsDecimal(), 21.89f, 0.0001f, TEST_LOCATION );
    DALI_TEST_EQUALS( value.AsInteger(), 22.0f, 0.0001f, TEST_LOCATION );
    DALI_TEST_EQUALS( value.AsTruncated(), 21.0f, 0.0001f, TEST_LOCATION );
  }
  // operator/
  {
    LayoutLength value = LayoutLength( 11 ) / 2.0f;
    DALI_TEST_EQUALS( value.AsDecimal(), 5.5f, 0.0001f, TEST_LOCATION );
    DALI_TEST_EQUALS( value.AsInteger(), 6.0f, 0.0001f, TEST_LOCATION );
  }

  // combinations
  {
    LayoutLength value;
    value = 20 + ( LayoutLength( 10 ) * 2.0f ) / 1.5f;
    DALI_TEST_EQUALS( value.AsInteger(), 33.0f, 0.0001f, TEST_LOCATION );
    DALI_TEST_EQUALS( value.AsDecimal(), 33.3333f, 0.0001f, TEST_LOCATION );
  }
  {
    uint16_t padding( 1 );
    LayoutLength right(35), left(10), mTotalLength(2);
    LayoutLength value;
    value = padding + ( right - left - mTotalLength ) / 2.0f;
    //    = 1 + ( 35 - 10 - 2 ) / 2
    DALI_TEST_EQUALS( value.AsInteger(), 13.0f, 0.0001f, TEST_LOCATION );
    DALI_TEST_EQUALS( value.AsDecimal(), 12.5f, 0.0001f, TEST_LOCATION );
  }
  {
    uint16_t padding = 1, top = 2, bottom = 3;
    LayoutLength childSpace( 44 ), childHeight( 23 );
    LayoutLength value;
    value = padding + ( ( childSpace - childHeight ) / 2.0f ) + top - bottom;
    //    = 1       + ( ( 44 - 23                  ) / 2 ) + 2 - 3
    DALI_TEST_EQUALS( value.AsInteger(), 11.0f, 0.0001f, TEST_LOCATION );
    DALI_TEST_EQUALS( value.AsDecimal(), 10.5f, 0.0001f, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliLayouting_LayoutLength_Operators(void)
{
  tet_infoline("UtcDaliLayouting_LayoutLength operators");

  {
    LayoutLength value = 10;
    DALI_TEST_EQUALS( (int)value.AsInteger(), 10, TEST_LOCATION );
    value += 1;
    DALI_TEST_EQUALS( (int)value.AsInteger(), 11, TEST_LOCATION );
    value -= 12;
    DALI_TEST_EQUALS( (int)value.AsInteger(), -1, TEST_LOCATION );
    LayoutLength value2 = value;
    DALI_TEST_EQUALS( value == value2, true, TEST_LOCATION );
    value2 = 123;
    DALI_TEST_EQUALS( value != value2, true, TEST_LOCATION );
    DALI_TEST_EQUALS( value < value2, true, TEST_LOCATION );
    DALI_TEST_EQUALS( value <= value2, true, TEST_LOCATION );
    value = 456;
    DALI_TEST_EQUALS( value > value2, true, TEST_LOCATION );
    DALI_TEST_EQUALS( value >= value2, true, TEST_LOCATION );
  }

  {
    LayoutLength value( 123 );
    std::stringstream ss;
    ss << value;
    DALI_TEST_EQUALS( ss.str(), "123", TEST_LOCATION );
  }
  {
    LayoutLength value( 0.123f );
    std::stringstream ss;
    ss << value;
    DALI_TEST_EQUALS( ss.str(), "0.123", TEST_LOCATION );
  }
  END_TEST;
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

  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Change size specification
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Use WRAP_CONTENT again
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_HboxLayout07(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_HboxLayout07 - Set LTR/RTL direction");

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

  hbox.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::LEFT_TO_RIGHT );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // hbox centers elements vertically, it fills test harness stage, which is 480x800 from left to right.
  // hbox left justifies elements
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 50.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 120.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 210.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hbox.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 470.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 400.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 310.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 200.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_HboxLayout08(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_HboxLayout08 - Test layout animation");

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  auto absoluteLayout = AbsoluteLayout::New();
  absoluteLayout.SetAnimateLayout( true );
  DevelControl::SetLayout( rootControl, absoluteLayout );
  rootControl.SetName( "AbsoluteLayout" );
  stage.Add( rootControl );

  Control control1 = CreateLeafControl( 40, 40 );
  rootControl.Add( control1 );

  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  hboxLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox" );

  Control control2 = CreateLeafControl( 40, 40 );
  hbox.Add( control2 );

  hbox.SetParentOrigin( ParentOrigin::CENTER );
  hbox.SetAnchorPoint( AnchorPoint::CENTER );
  rootControl.Add( hbox );

  DALI_TEST_EQUALS( absoluteLayout.IsLayoutAnimated(), true, TEST_LOCATION );
  DALI_TEST_EQUALS( DevelControl::GetLayout( control1 ).IsLayoutAnimated(), true, TEST_LOCATION );
  DALI_TEST_EQUALS( hboxLayout.IsLayoutAnimated(), false, TEST_LOCATION );
  DALI_TEST_EQUALS( DevelControl::GetLayout( control2 ).IsLayoutAnimated(), false, TEST_LOCATION );

  tet_infoline(" Set hBoxLayout to animate");
  hboxLayout.SetAnimateLayout( true );
  tet_infoline(" Set absoluteLayout not to animate");
  absoluteLayout.SetAnimateLayout( false );

  DALI_TEST_EQUALS( absoluteLayout.IsLayoutAnimated(), false, TEST_LOCATION );
  DALI_TEST_EQUALS( DevelControl::GetLayout( control1 ).IsLayoutAnimated(), false, TEST_LOCATION );
  DALI_TEST_EQUALS( hboxLayout.IsLayoutAnimated(), true, TEST_LOCATION );
  DALI_TEST_EQUALS( DevelControl::GetLayout( control2 ).IsLayoutAnimated(), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_HboxLayout09(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_HboxLayout07 - Set alignment");

  Stage stage = Stage::GetCurrent();
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 60 ) );

  for( auto&& iter : controls )
  {
    hbox.Add( iter );
  }
  hbox.SetParentOrigin( ParentOrigin::CENTER );
  hbox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( hbox );

  // Check default orientation
  DALI_TEST_EQUALS( static_cast<unsigned int>( hboxLayout.GetOrientation() ), static_cast<unsigned int>( LinearLayout::Orientation::HORIZONTAL ), TEST_LOCATION );
  // Check default alignment
  DALI_TEST_EQUALS( hboxLayout.GetAlignment(), ( Dali::Toolkit::LinearLayout::Alignment::BEGIN | Dali::Toolkit::LinearLayout::Alignment::CENTER_VERTICAL ), TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::BEGIN );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 370.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::END );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 380.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 420.0f, 370.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::CENTER_HORIZONTAL );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 190.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 230.0f, 370.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::BEGIN | Dali::Toolkit::LinearLayout::Alignment::TOP );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::END | Dali::Toolkit::LinearLayout::Alignment::TOP );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 380.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 420.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::CENTER_HORIZONTAL | Dali::Toolkit::LinearLayout::Alignment::TOP );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 190.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 230.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::BEGIN | Dali::Toolkit::LinearLayout::Alignment::BOTTOM );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 760.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 740.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::END | Dali::Toolkit::LinearLayout::Alignment::BOTTOM );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 380.0f, 760.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 420.0f, 740.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::CENTER_HORIZONTAL | Dali::Toolkit::LinearLayout::Alignment::BOTTOM );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 190.0f, 760.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 230.0f, 740.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::CENTER_HORIZONTAL | Dali::Toolkit::LinearLayout::Alignment::CENTER_VERTICAL );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 190.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 230.0f, 370.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetOrientation( LinearLayout::Orientation::VERTICAL );
  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::BEGIN );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 390.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::END );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 440.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 420.0f, 390.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::CENTER_HORIZONTAL );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 220.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 210.0f, 390.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::BEGIN | Dali::Toolkit::LinearLayout::Alignment::TOP );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::END | Dali::Toolkit::LinearLayout::Alignment::TOP );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 440.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 420.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::CENTER_HORIZONTAL | Dali::Toolkit::LinearLayout::Alignment::TOP );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 220.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 210.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::BEGIN | Dali::Toolkit::LinearLayout::Alignment::BOTTOM );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 700.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 740.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::END | Dali::Toolkit::LinearLayout::Alignment::BOTTOM );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 440.0f, 700.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 420.0f, 740.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::CENTER_HORIZONTAL | Dali::Toolkit::LinearLayout::Alignment::BOTTOM );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 220.0f, 700.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 210.0f, 740.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::CENTER_HORIZONTAL | Dali::Toolkit::LinearLayout::Alignment::CENTER_VERTICAL );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 220.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 210.0f, 390.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_HboxLayout_Weight(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliLayouting_HboxLayout_Weight - Test LinearLayout weight horizontally" );

  Stage stage = Stage::GetCurrent();
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox" );

  std::vector<Control> controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 40 ) );
  controls.push_back( CreateLeafControl( 80, 40 ) );
  controls.push_back( CreateLeafControl( 100, 40 ) );

  // Set weight for each leaf to use quarter of available space
  // 480 * 0.25, 480 * 0.25, 480 * 0.25, 480 * 0.25,
  // width spec has to be set to 0 so not to consume any extra space.
  for( auto&& iter : controls )
  {
    iter.SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.25f );
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 0 );
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
    hbox.Add( iter );
  }

  hbox.SetParentOrigin( ParentOrigin::CENTER );
  hbox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 120.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 240.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 360.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 120.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 120.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 120.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 120.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Set weight for 3, 4 leafs to use all remaining available space after 1, 2 content sized leafs.
  // 40, 60, (480 - 40 - 60) * 0.5, (480 - 40 - 60) * 0.5
  controls[0].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  controls[0].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );
  controls[1].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  controls[1].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );
  controls[2].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.5f );
  controls[3].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.5f );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 290.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 190.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 190.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Add intrinsic width for 3rd leaf so now we should get
  // 40, 60, (480 - 40 - 60 - 80) * 0.5 + 80, (480 - 40 - 60 - 80) * 0.5
  controls[2].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 80 );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 330.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 230.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 150.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  controls[0].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 0 );
  controls[0].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.25f );
  controls[1].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 0 );
  controls[1].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.5f );
  controls[2].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 0 );
  controls[2].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.25f );
  controls[3].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 40 );
  controls[3].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 110.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 330.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 440.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 110.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 220.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 110.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // WRAP_CONTENT doesn't affect weight
  controls[0].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  controls[0].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );
  controls[1].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  controls[1].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );
  controls[2].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  controls[2].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );
  controls[3].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  controls[3].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 1 );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 180.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 300.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // MATCH_PARENT doesn't affect weight
  for( auto&& iter : controls )
  {
    iter.SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.25f );
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  }

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 120.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 240.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 360.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 120.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 120.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 120.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 120.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  const Extents CONTROL_MARGIN = Extents( 10, 10, 0, 0 );
  for( auto&& iter : controls )
  {
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
    iter.SetProperty( Toolkit::Control::Property::MARGIN, CONTROL_MARGIN );
  }

  controls[0].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );
  controls[1].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );
  controls[2].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );
  controls[3].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 1 );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 10.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 70.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 150.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 250.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 220.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_VboxLayout_Weight(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliLayouting_VboxLayout_Weight - Test LinearLayout weight vertically" );

  Stage stage = Stage::GetCurrent();
  auto vbox = Control::New();
  auto vboxLayout = LinearLayout::New();
  vboxLayout.SetOrientation( Dali::Toolkit::LinearLayout::Orientation::VERTICAL );
  DevelControl::SetLayout( vbox, vboxLayout );
  vbox.SetName( "VBox" );

  std::vector<Control> controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 60 ) );
  controls.push_back( CreateLeafControl( 80, 80 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );

  // Set weight for each leaf to use quarter of available space
  // 800 * 0.25, 800 * 0.25, 800 * 0.25, 800 * 0.25,
  // width spec has to be set to 0 so not to consume any extra space.
  for( auto&& iter : controls )
  {
    iter.SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.25f );
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, 0 );
    vbox.Add( iter );
  }

  vbox.SetParentOrigin( ParentOrigin::CENTER );
  vbox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( vbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 600.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Set weight for 3, 4 leafs to use all remaining available space after 1, 2 content sized leafs.
  // 40, 60, (800 - 40 - 60) * 0.5, (800 - 40 - 60) * 0.5
  controls[0].SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  controls[0].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );
  controls[1].SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  controls[1].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );
  controls[2].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.5f );
  controls[3].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.5f );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 450.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Add intrinsic width for 3rd leaf so now we should get
  // 40, 60, (800 - 40 - 60 - 100) * 0.5 + 100, (800 - 40 - 60 - 100) * 0.5
  controls[2].SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, 100 );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 500.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 300.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  controls[0].SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, 0 );
  controls[0].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.25f );
  controls[1].SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, 0 );
  controls[1].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.5f );
  controls[2].SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, 0 );
  controls[2].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.25f );
  controls[3].SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, 100 );
  controls[3].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 175.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 525.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 700.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 175.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 175.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // WRAP_CONTENT doesn't affect weight
  controls[0].SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  controls[0].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );
  controls[1].SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  controls[1].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );
  controls[2].SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  controls[2].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );
  controls[3].SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  controls[3].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 1 );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 180.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 620.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // MATCH_PARENT doesn't affect weight
  for( auto&& iter : controls )
  {
    iter.SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.25f );
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  }

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 600.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  const Extents CONTROL_MARGIN = Extents( 0, 0, 10, 10 );
  for( auto&& iter : controls )
  {
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
    iter.SetProperty( Toolkit::Control::Property::MARGIN, CONTROL_MARGIN );
  }

  controls[0].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );
  controls[1].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );
  controls[2].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0 );
  controls[3].SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 1 );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 10.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 70.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 150.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 250.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 540.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_NestedLinearLayout_Weight(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_NestedLinearLayout_Weight - test weighted children with wrapped parent");

  Stage stage = Stage::GetCurrent();
  auto rootControl = Control::New();
  rootControl.SetName( "AbsoluteLayout");
  auto rootLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootControl, rootLayout );
  rootControl.SetAnchorPoint( AnchorPoint::CENTER );
  rootControl.SetParentOrigin( ParentOrigin::CENTER );
  stage.Add( rootControl );

  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 60 ) );

  // set equal share of wrapped content width (40 + 60 = 100) for both children 100 * 0.5, 100 * 0.5
  for( auto&& iter : controls )
  {
    iter.SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.5f );
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 0 );
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
    hbox.Add( iter );
  }

  hbox.SetParentOrigin( ParentOrigin::CENTER );
  hbox.SetAnchorPoint( AnchorPoint::CENTER );
  rootControl.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 10.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 50.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 50.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 50.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hboxLayout.SetOrientation(Dali::Toolkit::LinearLayout::Orientation::VERTICAL);

  // set equal share of wrapped content height (40 + 60 = 100) for both children 100 * 0.5, 100 * 0.5
  for( auto&& iter : controls )
  {
    iter.SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.5f );
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
    iter.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, 0 );
  }

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}


namespace
{
const char* TEST_IMAGE_FILE_NAME =  TEST_RESOURCE_DIR  "/broken.png";
}

int UtcDaliLayouting_HboxLayout_ImageView(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_HboxLayout - Use image view for leaf");

  Stage stage = Stage::GetCurrent();
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox" );

  std::string url = CreateImageURL( Vector4( 0, 255, 0, 255), ImageDimensions( 100, 100 ) );
  ImageView imageView = CreateImageView( url, ImageDimensions() );

  hbox.SetParentOrigin( ParentOrigin::CENTER );
  hbox.SetAnchorPoint( AnchorPoint::CENTER );
  hbox.Add( imageView );
  stage.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  tet_infoline("UtcDaliLayouting_HboxLayout - Change image (new size)");
  url = CreateImageURL( Vector4( 0, 255, 0, 255), ImageDimensions( 200, 200 ) );
  imageView.SetImage( url );

  // Ensure layouting happenss
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 300.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 200.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  imageView.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  imageView.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  // Ensure layouting happenss
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 200.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  imageView.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  imageView.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );

  // Ensure layouting happenss
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 300.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  imageView.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  imageView.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );

  Image image = FrameBufferImage::New( 50, 50, Pixel::RGBA8888 );
  imageView.SetImage( image );

  // Ensure layouting happenss
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 375.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 50.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  Property::Map imagePropertyMap;
  imagePropertyMap[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imagePropertyMap[ ImageVisual::Property::URL ] = TEST_IMAGE_FILE_NAME;
  imagePropertyMap[ ImageVisual::Property::DESIRED_WIDTH ] = 150;
  imagePropertyMap[ ImageVisual::Property::DESIRED_HEIGHT ] = 150;
  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imagePropertyMap );

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );
  // Ensure layouting happenss
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 325.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 150.0f, 150.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_HboxLayout_TextLabel(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_HboxLayout - Use text label for leaf");

  Stage stage = Stage::GetCurrent();

  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox" );
  hbox.SetParentOrigin( ParentOrigin::CENTER );
  hbox.SetAnchorPoint( AnchorPoint::CENTER );

  std::vector< Control > controls;
  TextLabel textLabel = CreateTextLabel( "W" );
  controls.push_back( textLabel );
  hbox.Add( textLabel );
  stage.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( textLabel.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 368.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( textLabel.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 52.0f, 64.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  textLabel.SetProperty( TextLabel::Property::TEXT, "WWWW" );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 368.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 214.0f, 64.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  textLabel.SetProperty( TextLabel::Property::POINT_SIZE, 10.0f );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 382.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 120.0f, 36.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  controls[0].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  controls[0].SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 120.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  controls[0].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  controls[0].SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 382.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 36.0f, 0.0f ), 0.0001f, TEST_LOCATION );

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
  tet_infoline("UtcDaliLayouting_HboxLayout_Padding03 - Changing padding on a single child");

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

int UtcDaliLayouting_HboxLayout_Padding04(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_HboxLayout_Padding04 - Adding Padding to the hbox");

  // Adding padding to the layout should offset the positioning of the children.

  const Extents LAYOUT_PADDING = Extents(5, 10, 20, 2 );
  const Size CONTROL_SIZE = Size( 40, 40 );

  Stage stage = Stage::GetCurrent();
  // Create a root layout, ideally Dali would have a default layout in the root layer.
  // Without this root layer the LinearLayout (or any other layout) will not
  // honour WIDTH_SPECIFICATION or HEIGHT_SPECIFICATION settings.
  // It uses the default stage size and ideally should have a layout added to it.
  auto rootLayoutControl = Control::New();
  rootLayoutControl.SetName( "AbsoluteLayout");
  auto rootLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootLayoutControl, rootLayout );
  rootLayoutControl.SetAnchorPoint( AnchorPoint::CENTER );
  rootLayoutControl.SetParentOrigin( ParentOrigin::CENTER );
  stage.Add( rootLayoutControl );

  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  hboxLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox");
  hbox.SetProperty(Toolkit::Control::Property::PADDING, LAYOUT_PADDING );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( CONTROL_SIZE.width, CONTROL_SIZE.height ) );
  controls.push_back( CreateLeafControl( CONTROL_SIZE.width, CONTROL_SIZE.height ) );
  controls.push_back( CreateLeafControl( CONTROL_SIZE.width, CONTROL_SIZE.height ) );
  controls.push_back( CreateLeafControl( CONTROL_SIZE.width, CONTROL_SIZE.height ) );

  for( auto&& iter : controls )
  {
    hbox.Add( iter );
  }

  hbox.SetParentOrigin( ParentOrigin::CENTER );
  hbox.SetAnchorPoint( AnchorPoint::CENTER );
  rootLayoutControl.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // Extra update needed to Relayout one more time. Catches any position updates, false positive without this seen.
  application.SendNotification();

  // hbox centers elements vertically, it fills test harness stage, which is 480x800.
  // hbox left justifies elements
  tet_infoline("Test Child Actor Position");

  auto controlXPosition=0.0f;

  controlXPosition = LAYOUT_PADDING.start;  // First child positioned at offset defined by the padding
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( LAYOUT_PADDING.start,
                                                                                            LAYOUT_PADDING.top,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );

  controlXPosition+=CONTROL_SIZE.width; // Second child positioned is the position of the first child + the first child's width.
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( controlXPosition,
                                                                                            LAYOUT_PADDING.top,
                                                                                            0.0f ),
                                                                                            0.0001f, TEST_LOCATION );

  controlXPosition+=CONTROL_SIZE.width; // Third child positioned adjacent to second
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( controlXPosition,
                                                                                            LAYOUT_PADDING.top,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );

  controlXPosition+=CONTROL_SIZE.width; // Forth passed adjacent to the third
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( controlXPosition,
                                                                                            LAYOUT_PADDING.top,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );

  auto totalControlsWidth = CONTROL_SIZE.width * controls.size();
  auto totalControlsHeight = CONTROL_SIZE.height;

  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( totalControlsWidth + LAYOUT_PADDING.start + LAYOUT_PADDING.end,
                                                                                 totalControlsHeight + LAYOUT_PADDING.top + LAYOUT_PADDING.bottom,
                                                                                 0.0f ), 0.0001f, TEST_LOCATION );


  END_TEST;
}

int UtcDaliLayouting_HboxLayout_Padding05(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_HboxLayout_Padding05 - Changing the hbox Padding");

  // Adding padding to the layout should offset the positioning of the children.

  const Extents LAYOUT_PADDING = Extents(5, 10, 20, 2 );
  const Size CONTROL_SIZE = Size( 40, 40 );

  Stage stage = Stage::GetCurrent();
  // Create a root layout, ideally Dali would have a default layout in the root layer.
  // Without this root layer the LinearLayout (or any other layout) will not
  // honour WIDTH_SPECIFICATION or HEIGHT_SPECIFICATION settings.
  // It uses the default stage size and ideally should have a layout added to it.
  auto rootLayoutControl = Control::New();
  rootLayoutControl.SetName( "AbsoluteLayout");
  auto rootLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootLayoutControl, rootLayout );
  rootLayoutControl.SetAnchorPoint( AnchorPoint::CENTER );
  rootLayoutControl.SetParentOrigin( ParentOrigin::CENTER );
  stage.Add( rootLayoutControl );

  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  hboxLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox");
  hbox.SetProperty(Toolkit::Control::Property::PADDING, LAYOUT_PADDING );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( CONTROL_SIZE.width, CONTROL_SIZE.height ) );
  controls.push_back( CreateLeafControl( CONTROL_SIZE.width, CONTROL_SIZE.height ) );
  controls.push_back( CreateLeafControl( CONTROL_SIZE.width, CONTROL_SIZE.height ) );
  controls.push_back( CreateLeafControl( CONTROL_SIZE.width, CONTROL_SIZE.height ) );

  for( auto&& iter : controls )
  {
    hbox.Add( iter );
  }

  hbox.SetParentOrigin( ParentOrigin::CENTER );
  hbox.SetAnchorPoint( AnchorPoint::CENTER );
  rootLayoutControl.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // Extra update needed to Relayout one more time. Catches any position updates, false positive without this seen.
  application.SendNotification();

  // hbox centers elements vertically, it fills test harness stage, which is 480x800.
  // hbox left justifies elements
  tet_infoline("Test Child Actor Position");

  auto controlXPosition=0.0f;

  controlXPosition = LAYOUT_PADDING.start;  // First child positioned at offset defined by the padding
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( LAYOUT_PADDING.start,
                                                                                            LAYOUT_PADDING.top,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );

  controlXPosition+=CONTROL_SIZE.width; // Second child positioned is the position of the first child + the first child's width.
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( controlXPosition,
                                                                                            LAYOUT_PADDING.top,
                                                                                            0.0f ),
                                                                                            0.0001f, TEST_LOCATION );

  controlXPosition+=CONTROL_SIZE.width; // Third child positioned adjacent to second
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( controlXPosition,
                                                                                            LAYOUT_PADDING.top,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );

  controlXPosition+=CONTROL_SIZE.width; // Forth passed adjacent to the third
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( controlXPosition,
                                                                                            LAYOUT_PADDING.top,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );

  auto totalControlsWidth = CONTROL_SIZE.width * controls.size();
  auto totalControlsHeight = CONTROL_SIZE.height;

  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( totalControlsWidth + LAYOUT_PADDING.start + LAYOUT_PADDING.end,
                                                                                 totalControlsHeight + LAYOUT_PADDING.top + LAYOUT_PADDING.bottom,
                                                                                 0.0f ), 0.0001f, TEST_LOCATION );

  // Change layout padding
  const Extents NEW_LAYOUT_PADDING = Extents(5, 20, 10, 2 );
  tet_printf( "\nChanging Padding to control at index 1 \n" );
  hbox.SetProperty(Toolkit::Control::Property::PADDING, NEW_LAYOUT_PADDING );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // Extra update needed to Relayout one more time. Catches any position updates, false positive without this seen.
  application.SendNotification();

  controlXPosition = NEW_LAYOUT_PADDING.start;  // First child positioned at offset defined by the padding
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( NEW_LAYOUT_PADDING.start,
                                                                                            NEW_LAYOUT_PADDING.top,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );

  controlXPosition+=CONTROL_SIZE.width; // Second child positioned is the position of the first child + the first child's width.
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( controlXPosition,
                                                                                            NEW_LAYOUT_PADDING.top,
                                                                                            0.0f ),
                                                                                            0.0001f, TEST_LOCATION );

  controlXPosition+=CONTROL_SIZE.width; // Third child positioned adjacent to second
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( controlXPosition,
                                                                                            NEW_LAYOUT_PADDING.top,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );

  controlXPosition+=CONTROL_SIZE.width; // Forth passed adjacent to the third
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( controlXPosition,
                                                                                            NEW_LAYOUT_PADDING.top,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );
  totalControlsWidth = CONTROL_SIZE.width * controls.size();
  totalControlsHeight = CONTROL_SIZE.height;

  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( totalControlsWidth + NEW_LAYOUT_PADDING.start + NEW_LAYOUT_PADDING.end,
                                                                                 totalControlsHeight + NEW_LAYOUT_PADDING.top + NEW_LAYOUT_PADDING.bottom,
                                                                                 0.0f ), 0.0001f, TEST_LOCATION );
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
  vboxLayout.SetAlignment( LinearLayout::Alignment::TOP | LinearLayout::Alignment::CENTER_HORIZONTAL );
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
  vboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::TOP | Dali::Toolkit::LinearLayout::Alignment::CENTER_HORIZONTAL );
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
  vboxLayout.SetAlignment( Dali::Toolkit::LinearLayout::Alignment::TOP | Dali::Toolkit::LinearLayout::Alignment::CENTER_HORIZONTAL );

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

int UtcDaliLayouting_VboxLayout_Padding(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_VboxLayout_Padding - Adding Padding to the vbox");

  // Adding padding to the layout should offset the positioning of the children.

  const Extents LAYOUT_PADDING = Extents(5, 10, 20, 2 );
  const Size CONTROL_SIZE = Size( 40, 40 );

  Stage stage = Stage::GetCurrent();
  // Create a root layout, ideally Dali would have a default layout in the root layer.
  // Without this root layer the LinearLayout (or any other layout) will not
  // honour WIDTH_SPECIFICATION or HEIGHT_SPECIFICATION settings.
  // It uses the default stage size and ideally should have a layout added to it.
  auto rootLayoutControl = Control::New();
  rootLayoutControl.SetName( "AbsoluteLayout");
  auto rootLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootLayoutControl, rootLayout );
  rootLayoutControl.SetAnchorPoint( AnchorPoint::CENTER );
  rootLayoutControl.SetParentOrigin( ParentOrigin::CENTER );
  stage.Add( rootLayoutControl );

  auto vbox = Control::New();
  auto vboxLayout = LinearLayout::New();
  vboxLayout.SetOrientation( LinearLayout::Orientation::VERTICAL );
  DevelControl::SetLayout( vbox, vboxLayout );
  vbox.SetName( "VBox");
  vbox.SetProperty( Toolkit::Control::Property::PADDING, LAYOUT_PADDING );
  vbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  vbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( CONTROL_SIZE.width, CONTROL_SIZE.height ) );
  controls.push_back( CreateLeafControl( CONTROL_SIZE.width, CONTROL_SIZE.height ) );
  controls.push_back( CreateLeafControl( CONTROL_SIZE.width, CONTROL_SIZE.height ) );
  controls.push_back( CreateLeafControl( CONTROL_SIZE.width, CONTROL_SIZE.height ) );

  for( auto&& iter : controls )
  {
    vbox.Add( iter );
  }

  vbox.SetParentOrigin( ParentOrigin::CENTER );
  vbox.SetAnchorPoint( AnchorPoint::CENTER );
  rootLayoutControl.Add( vbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // Extra update needed to Relayout one more time. Catches any position updates, false positive without this seen.
  application.SendNotification();

  // vbox centers elements horizontally, it fills test harness stage, which is 480x800.
  tet_infoline("Test Child Actor Position");

  auto controlYPosition = 0.0f;

  controlYPosition = LAYOUT_PADDING.top;  // First child positioned at offset defined by the padding
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( LAYOUT_PADDING.start,
                                                                                            LAYOUT_PADDING.top,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );

  controlYPosition += CONTROL_SIZE.height; // Second child positioned is the position of the first child + the first child's height.
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( LAYOUT_PADDING.start,
                                                                                            controlYPosition,
                                                                                            0.0f ),
                                                                                            0.0001f, TEST_LOCATION );

  controlYPosition += CONTROL_SIZE.height; // Third child positioned adjacent to second
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( LAYOUT_PADDING.start,
                                                                                            controlYPosition,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );

  controlYPosition += CONTROL_SIZE.height; // Forth passed adjacent to the third
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( LAYOUT_PADDING.start,
                                                                                            controlYPosition,
                                                                                            0.0f ), 0.0001f, TEST_LOCATION );

  auto totalControlsWidth = CONTROL_SIZE.width;
  auto totalControlsHeight = CONTROL_SIZE.height * controls.size();

  DALI_TEST_EQUALS( vbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( totalControlsWidth + LAYOUT_PADDING.start + LAYOUT_PADDING.end,
                                                                                 totalControlsHeight + LAYOUT_PADDING.top + LAYOUT_PADDING.bottom,
                                                                                 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}


int UtcDaliLayouting_HboxLayout_TargetSize(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_HboxLayout07 - Set target size on leaf");

  Stage stage = Stage::GetCurrent();
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  for( auto&& iter : controls )
  {
    iter.SetSize( 100, 100 );
    hbox.Add( iter );
  }
  hbox.SetParentOrigin( ParentOrigin::CENTER );
  hbox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // hbox centers elements vertically, it fills test harness stage, which is 480x800 from left to right.
  // hbox left justifies elements
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_LayoutChildren01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_LayoutChildren01");

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  auto absoluteLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootControl, absoluteLayout );
  stage.Add( rootControl );

  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  rootControl.Add( hbox );

  DALI_TEST_EQUALS( absoluteLayout.GetChildCount(), 1, TEST_LOCATION );

  tet_infoline("Test removal by removing child actor from parent container" );
  hbox.Unparent();

  DALI_TEST_EQUALS( absoluteLayout.GetChildCount(), 0, TEST_LOCATION );

  auto& hboxImpl = GetImplementation( hboxLayout );
  tet_infoline("Test child actor still has hbox layout " );
  DALI_TEST_EQUALS( (bool)hboxLayout.GetOwner(), true, TEST_LOCATION );

  tet_infoline("Test hbox layout has no parent " );
  DALI_TEST_EQUALS( (void*)hboxImpl.GetParent(), (void*)nullptr, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_LayoutChildren02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_LayoutChildren02");

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  auto absoluteLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootControl, absoluteLayout );
  stage.Add( rootControl );

  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  rootControl.Add( hbox );

  DALI_TEST_EQUALS( absoluteLayout.GetChildCount(), 1, TEST_LOCATION );

  tet_infoline("Test removal by removing child layout from parent layout" );
  absoluteLayout.Remove( hboxLayout );

  DALI_TEST_EQUALS( absoluteLayout.GetChildCount(), 0, TEST_LOCATION );

  auto& hboxImpl = GetImplementation( hboxLayout );

  tet_infoline("Check child actor has orphaned layout (Moving child keeps old layout)");
  DALI_TEST_EQUALS( hboxLayout.GetOwner(), hbox, TEST_LOCATION );
  DALI_TEST_EQUALS( DevelControl::GetLayout(hbox), hboxLayout, TEST_LOCATION );

  tet_infoline("Check orphaned layout has no parent");
  DALI_TEST_EQUALS( (void*)hboxImpl.GetParent(), (void*)nullptr, TEST_LOCATION );

  END_TEST;
}


int UtcDaliLayouting_LayoutChildren03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_LayoutChildren03");

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  auto absoluteLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootControl, absoluteLayout );
  stage.Add( rootControl );

  auto hbox = Control::New();
  tet_infoline("Test unparenting by adding child with no layout to parent (should auto-generate LayoutItem) ");
  auto hboxLayout = LinearLayout::New();
  rootControl.Add( hbox );

  DALI_TEST_EQUALS( absoluteLayout.GetChildCount(), 1, TEST_LOCATION );

  tet_infoline("Then setting a layout on the child container");
  DevelControl::SetLayout( hbox, hboxLayout );

  DALI_TEST_EQUALS( absoluteLayout.GetChildCount(), 1, TEST_LOCATION );

  auto& hboxImpl = GetImplementation( hboxLayout );
  auto& absImpl = GetImplementation( absoluteLayout );
  DALI_TEST_EQUALS( hboxLayout.GetOwner(), Handle(hbox), TEST_LOCATION );
  DALI_TEST_EQUALS( hboxImpl.GetParent(), (Dali::Toolkit::Internal::LayoutParent*)&absImpl, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_SetLayoutOrder(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_SetLayoutOrder01 - Call SetLayout after adding the control to the root layout");

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  auto absoluteLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( rootControl, absoluteLayout );
  rootControl.SetName( "AbsoluteLayout" );
  stage.Add( rootControl );

  tet_infoline(" UtcDaliLayouting_SetLayoutOrder - Creating control");
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  hbox.SetName( "HBox");

  tet_infoline(" UtcDaliLayouting_SetLayoutOrder - Add control to root layout");
  rootControl.Add( hbox );

  tet_infoline(" UtcDaliLayouting_SetLayoutOrder - Set layout to control AFTER control added to root");
  DevelControl::SetLayout( hbox, hboxLayout );

  // Add a Child control
  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) ); // Single control
  for( auto&& iter : controls )
  {
    hbox.Add( iter );
  }

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}


int UtcDaliLayouting_LayoutGroup01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_LayoutGroup01 - Test adding a control to a layout then adding a TextLabel to that control");

  Control rootControl;
  SetupRootLayoutControl( rootControl );

  // Create a parent layout
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  hbox.SetName( "HBox");
  rootControl.Add( hbox );
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 600 );
  hbox.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );
  DevelControl::SetLayoutingRequired( hbox, true );
  hbox.SetAnchorPoint( AnchorPoint::TOP_LEFT );  // LinearLayout will eventually do this internally.

  tet_infoline("Add a control without SetLayout being called but with layout required set true");

  auto control = Control::New();
  control.SetName("Control1");
  DevelControl::SetLayoutingRequired( control, true );
  hbox.Add( control );
  control.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  control.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );
  tet_infoline("Add a Textlabel to the control");
  auto textLabel = TextLabel::New("Test text");
  textLabel.SetName("TextLabel");

  control.Add( textLabel );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  tet_infoline("Test text is it's natural size");
  DALI_TEST_EQUALS( textLabel.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 228.0f, 64.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  tet_infoline("Test control is width of it's parent and height of it's child");
  DALI_TEST_EQUALS( control.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 600.0f, 64.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_LayoutGroup02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_LayoutGroup02 - Test control is the size of it's largest child");

  Control rootControl;
  SetupRootLayoutControl( rootControl );

  // Create a parent layout
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox");
  rootControl.Add( hbox );
  hbox.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 600 );
  hbox.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );
  hbox.SetAnchorPoint( AnchorPoint::TOP_LEFT );  // LinearLayout will eventually do this internally.

  tet_infoline("Add a control without SetLayout being called but with layout required set true");

  auto control = Control::New();
  control.SetName("Control1");
  DevelControl::SetLayoutingRequired( control, true );
  hbox.Add( control );
  control.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  control.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );

  tet_infoline("Add a Textlabel to the control");
  auto textLabel = TextLabel::New("Test text");
  textLabel.SetName("TextLabel");
  control.Add( textLabel );

  tet_infoline("Add another  Textlabel to the control");
  auto largeTextLabel = TextLabel::New("Test large text");
  largeTextLabel.SetName("TextLabel-Large");
  control.Add( largeTextLabel );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  tet_infoline("Test text is it's natural size");
  DALI_TEST_EQUALS( textLabel.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 228.0f, 64.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  tet_infoline("Test text is centered in the control, the control is the size of the largest child");
  DALI_TEST_EQUALS( textLabel.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  tet_infoline("Test large text is it's natural size");
  DALI_TEST_EQUALS( largeTextLabel.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 382.0f, 64.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  tet_infoline("Test text is aligned to start as is the size of the control");
  DALI_TEST_EQUALS( largeTextLabel.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  tet_infoline("Test control is width of it's parent and height of it's largest child");
  DALI_TEST_EQUALS( control.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 382.0f, 64.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_LayoutGroup03(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_LayoutGroup03 - Test control with a LayoutGroup as a leaf");

  Control rootControl;
  SetupRootLayoutControl( rootControl );

  // Create a parent layout
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox");
  rootControl.Add( hbox );
  hbox.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 600 );
  hbox.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );
  hbox.SetAnchorPoint( AnchorPoint::TOP_LEFT );  // LinearLayout will eventually do this internally.

  tet_infoline("Add a control without SetLayout being called");

  auto control = Control::New();
  control.SetName("Control1");
  hbox.Add( control );
  control.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  control.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  100 );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  tet_infoline("Test control is width of it's parent and exact given height");
  DALI_TEST_EQUALS( control.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 600.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( control.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}


int UtcDaliLayouting_LayoutGroup04(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_LayoutGroup04 - Test control with a LayoutGroup as a leaf and with SetLayotRequired = true");

  Control rootControl;
  SetupRootLayoutControl( rootControl );

  // Create a parent layout
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox");
  rootControl.Add( hbox );
  hbox.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 600 );
  hbox.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );
  hbox.SetAnchorPoint( AnchorPoint::TOP_LEFT );  // LinearLayout will eventually do this internally.

  tet_infoline("Add a control without SetLayout being called");

  auto control = Control::New();
  control.SetName("Control1");
  DevelControl::SetLayoutingRequired( control, true );
  hbox.Add( control );
  control.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  control.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  100 );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  tet_infoline("Test control is width of it's parent and exact given height");
  DALI_TEST_EQUALS( control.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 600.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( control.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_IsLayoutingRequired(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_IsLayoutingRequired - Test setting the SetLayoutRequired and then check if flag was set");

  Control rootControl;
  SetupRootLayoutControl( rootControl );

  // Create a parent layout
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox");
  rootControl.Add( hbox );
  hbox.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 600 );
  hbox.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );
  hbox.SetAnchorPoint( AnchorPoint::TOP_LEFT );  // LinearLayout will eventually do this internally.

  tet_infoline("Add a control without SetLayout being called");

  auto control = Control::New();
  control.SetName("Control1");
  DALI_TEST_EQUALS( DevelControl::IsLayoutingRequired( control ), false, TEST_LOCATION );
  DevelControl::SetLayoutingRequired( control, true );
  hbox.Add( control );
  control.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  control.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  100 );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  tet_infoline("Test control is width of it's parent and exact given height");
  DALI_TEST_EQUALS( DevelControl::IsLayoutingRequired( control ), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_LayoutGroupWithPadding01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_LayoutGroupWithPadding01 - Test adding a control to a layout that has padding");

  Control rootControl;
  SetupRootLayoutControl( rootControl );

  // Create a parent layout
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  hbox.SetName( "HBox");
  rootControl.Add( hbox );
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 600 );
  hbox.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );
  hbox.SetAnchorPoint( AnchorPoint::TOP_LEFT );  // LinearLayout will eventually do this internally.

  tet_infoline("Add a control without SetLayout being called");

  auto control = Control::New();
  control.SetName("Control1");
  DevelControl::SetLayoutingRequired( control, true );
  hbox.Add( control );
  control.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  control.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );

  const Extents CONTROL_PADDING = Extents(5, 10, 20, 2 );
  tet_printf( "Adding Padding to control");
  control.SetProperty( Toolkit::Control::Property::PADDING, CONTROL_PADDING );

  tet_infoline("Add a Textlabel to the control");
  auto textLabel = TextLabel::New("Test text");
  textLabel.SetName("TextLabel");
  control.Add( textLabel );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  tet_infoline("Test text is it's natural size");
  DALI_TEST_EQUALS( textLabel.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 228.0f, 64.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  tet_infoline("Test control is size of it's child and control it's own padding");
  DALI_TEST_EQUALS( control.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 243.0f, 86.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_LayoutGroupWithChildMargin01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_LayoutGroupWithChildMargin01 - Test adding a control with padding to a layout that has padding");

  Control rootControl;
  SetupRootLayoutControl( rootControl );

  // Create a parent layout
  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  hbox.SetName( "HBox");
  rootControl.Add( hbox );
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 600 );
  hbox.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );
  hbox.SetAnchorPoint( AnchorPoint::TOP_LEFT );  // LinearLayout will eventually do this internally.

  tet_infoline("Add a control without SetLayout being called");

  auto control = Control::New();
  control.SetName("Control1");
  DevelControl::SetLayoutingRequired( control, true );
  hbox.Add( control );
  control.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  control.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );

  const Extents CONTROL_PADDING = Extents(5, 10, 20, 2 );
  tet_printf( "Adding Padding to control");
  control.SetProperty( Toolkit::Control::Property::PADDING, CONTROL_PADDING );

  tet_infoline("Add a Textlabel to the control");
  auto textLabel = TextLabel::New("Test text");
  const Extents CHILD_MARGIN = Extents( 10, 0, 5, 0 );
  textLabel.SetProperty( Toolkit::Control::Property::MARGIN, CHILD_MARGIN );
  textLabel.SetName("TextLabel");
  control.Add( textLabel );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  tet_infoline("Test text is it's natural size");
  DALI_TEST_EQUALS( textLabel.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 228.0f, 64.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  tet_infoline("Test control is width of it's parent and height of it's child");
  DALI_TEST_EQUALS( control.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 253.0f, 91.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_SetLayout(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_SetLayout - Test reusing layouts");

  Control rootControl;
  SetupRootLayoutControl( rootControl );

  auto container = Control::New();
  auto horizontalLayout = LinearLayout::New();
  horizontalLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );
  DevelControl::SetLayout( container, horizontalLayout );
  container.SetName( "Container" );
  rootControl.Add( container );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 60 ) );

  for( auto&& iter : controls )
  {
    container.Add( iter );
  }

  container.SetParentOrigin( ParentOrigin::CENTER );
  container.SetAnchorPoint( AnchorPoint::CENTER );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 10.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Change a layout
  auto verticalLayout = LinearLayout::New();
  verticalLayout.SetOrientation( LinearLayout::Orientation::VERTICAL );
  DevelControl::SetLayout( container, verticalLayout );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Second round
  DevelControl::SetLayout( container, horizontalLayout );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 10.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Change a layout
  DevelControl::SetLayout( container, verticalLayout );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_StageAdd(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_StageAdd");

  Stage stage = Stage::GetCurrent();

  AbsoluteLayout absoluteLayout = AbsoluteLayout::New();
  Control container = Control::New();
  container.SetName( "Container" );
  DevelControl::SetLayout( container, absoluteLayout );
  container.SetAnchorPoint( Dali::AnchorPoint::CENTER );
  container.SetParentOrigin( Dali::ParentOrigin::CENTER );

  Control child = Control::New();
  child.SetAnchorPoint( Dali::AnchorPoint::TOP_LEFT );
  child.SetPosition( 0.0f, 0.0f );
  child.SetSize( 480.0f, 180.0f );
  child.SetName( "Child Control" );
  container.Add( child );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION ); // Not re-laid out
  DALI_TEST_EQUALS( child.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 180.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  // Add container to stage here
  // Should call RequestLayout() to measure and layout
  stage.Add( container );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION ); // Stage Size
  DALI_TEST_EQUALS( child.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 180.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}
