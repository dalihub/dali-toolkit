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

void utc_dali_toolkit_layouting_nesting_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_layouting_nesting_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Test nesting of Layouts and Controls

int UtcDaliLayoutingNesting_01(void)
{
  /*

  Root
    |
  Control (LinearLayout Horizontal)
    |
  Control (LayoutingRequired)
    |
  Control (LinearLayout Horizontal)
    |
  LeafControl

  */

  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayoutingNesting_01 - Nesting a LinearLayout (MATCH_PARENT) containing a leaf control within a LinearLayout");

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  SetupRootLayoutControl( rootControl );

  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  hboxLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "hBox" );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 480 );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, 800 );


  auto vbox = Control::New();
  auto vboxLayout = LinearLayout::New();
  vboxLayout.SetOrientation( LinearLayout::Orientation::VERTICAL );
  DevelControl::SetLayout( vbox, vboxLayout );
  vbox.SetName( "vBox" );
  vbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  vbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox.Add( vbox );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );

  for( auto&& iter : controls )
  {
    hbox.Add( iter );
  }

  rootControl.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( vbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayoutingNesting_02(void)
{
  /*

  Root
    |
  Control (LinearLayout Horizontal)
    |
  Control (LayoutingRequired)
    |
  Control (LinearLayout Horizontal)
    |    |
  LeafControl

  */

  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayoutingNesting_02 - Nesting a LinearLayout (MATCH_PARENT) containing a leaf control within a LinearLayout");
  tet_infoline("Then change the parent's size and test child responded correctly");

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  SetupRootLayoutControl( rootControl );

  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  hboxLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "hBox" );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 480 );
  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, 800 );


  auto vbox = Control::New();
  auto vboxLayout = LinearLayout::New();
  vboxLayout.SetOrientation( LinearLayout::Orientation::VERTICAL );
  DevelControl::SetLayout( vbox, vboxLayout );
  vbox.SetName( "vBox" );
  vbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  vbox.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox.Add( vbox );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );

  for( auto&& iter : controls )
  {
    hbox.Add( iter );
  }

  rootControl.Add( hbox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( vbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  hbox.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 400 );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();


  DALI_TEST_EQUALS( hbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 400.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( vbox.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 400.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}