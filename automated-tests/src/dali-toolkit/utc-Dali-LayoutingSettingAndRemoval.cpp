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

void utc_dali_toolkit_layouting_setting_and_removal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_layouting_setting_and_removal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliLayoutingSettingAndRemoval_RemoveLayout(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayoutingSettingAndRemoval_RemoveLayout - Remove a layout from a control");

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  SetupRootLayoutControl( rootControl );

  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox");
  rootControl.Add( hbox );

  // Add child controls
  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );  // 0
  controls.push_back( CreateLeafControl( 100, 100 ) );  // 1
  controls.push_back( CreateLeafControl( 100, 100 ) );  // 2

  for( auto&& iter : controls )
  {
    hbox.Add( iter );
  }

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  tet_infoline("Get number of child in the rootControl layout");
  DALI_TEST_EQUALS( ( LayoutGroup::DownCast( DevelControl::GetLayout( rootControl ) ) ).GetChildCount(), 1 , TEST_LOCATION );

  tet_infoline("SetLayout with empty Layout handle");

  DevelControl::SetLayout( hbox, LayoutItem{} );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  tet_infoline("Get number of children in the rootControl layout");
  DALI_TEST_EQUALS( ( LayoutGroup::DownCast( DevelControl::GetLayout( rootControl ) ) ).GetChildCount(), 1 , TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayoutingSettingAndRemoval_RemoveLayoutFromChild(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayoutingSettingAndRemoval_RemoveLayoutFromChild - Remove a layout from a child of another layout");

  /*

              Hbox
     |          |           |
  control0  control1  control2
  (vbox)     (leaf)     (leaf)

  Test removes layout from control0 but does not remove the control.

  */

  Stage stage = Stage::GetCurrent();

  auto rootControl = Control::New();
  SetupRootLayoutControl( rootControl );

  auto hbox = Control::New();
  auto hboxLayout = LinearLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "HBox");
  rootControl.Add( hbox );

  // Add child controls
  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );  // 0
  controls.push_back( CreateLeafControl( 100, 100 ) );  // 1
  controls.push_back( CreateLeafControl( 100, 100 ) );  // 2

  for( auto&& iter : controls )
  {
    hbox.Add( iter );
  }

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  tet_infoline("Set LinearLayout to child control 0");

  auto vboxLayout = LinearLayout::New();
  DevelControl::SetLayout( controls[0], vboxLayout );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  tet_infoline("Get number of children in the hbox layout");
  DALI_TEST_EQUALS( ( LayoutGroup::DownCast( DevelControl::GetLayout( hbox ) ) ).GetChildCount(), 3 , TEST_LOCATION );

  tet_infoline("SetLayout with empty Layout handle");

  DevelControl::SetLayout( controls[0], LayoutItem{} );

  // If vbox control has no children then should get a LayoutItem.
  // but if still has children then should be a LayoutGroup/BinContainer.

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  tet_infoline("Get number of child in the hbox layout");
  DALI_TEST_EQUALS( ( LayoutGroup::DownCast( DevelControl::GetLayout( hbox ) ) ).GetChildCount(), 3 , TEST_LOCATION );
  // Test should fail as the setting of an empty layout reduces the child count by 1

  END_TEST;
}

int UtcDaliLayoutingSettingAndRemoval_RemoveLayoutFromHbox(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayoutingSettingAndRemoval_RemoveLayoutFromHbox");

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
  hbox.SetName( "Container" );

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

  tet_infoline("Layout as normal");
  application.SendNotification();
  application.Render();

  tet_infoline("Set an empty layout on hbox container");
  LinearLayout emptyLayout;
  DevelControl::SetLayout( hbox, emptyLayout );

  tet_infoline("Run another layout");
  application.SendNotification();
  application.Render();

  tet_infoline("Check leaf controls size and position");

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  tet_infoline("Child keeps position from last layout");
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}