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

void utc_dali_toolkit_layouting_resize_policy_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_layouting_resize_policy_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Test setting of the MeasureSpecs with the legacy ResizePolicies

int UtcDaliLayoutingResizePolicy_01(void)
{
  /*
  Root
    |
  Control (LinearLayout Horizontal)
    |
  Control (LayoutingRequired)
    |
  Control (ResizePolicy::FILL_TO_PARENT)
    |
  LeafControl
  */

  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayoutingResizePolicy_01 - Set ResizePolicy FILL_TO_PARENT on Control");

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

  auto layoutingControl = Control::New();
  layoutingControl.SetName( "layoutingRequiredControl" );
  DevelControl::SetLayoutingRequired( layoutingControl, true );
  layoutingControl.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  layoutingControl.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox.Add( layoutingControl );

  auto control = Control::New();
  control.SetName( "fillToParentControl" );
  DevelControl::SetLayoutingRequired( control, true );
  control.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  layoutingControl.Add( control );

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

  DALI_TEST_EQUALS( control.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}


int UtcDaliLayoutingResizePolicy_02(void)
{
  /*
  Root
    |
  Control (LinearLayout Horizontal)
    |
  Control (LayoutingRequired)
    |
  Control (ResizePolicy::FIT_TO_CHILDREN)
    |
  LeafControl
  */

  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayoutingResizePolicy_02 - Set ResizePolicy FIT_TO_CHILDREN on Control");

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

  auto layoutingControl = Control::New();
  layoutingControl.SetName( "layoutingRequiredControl" );
  DevelControl::SetLayoutingRequired( layoutingControl, true );
  layoutingControl.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  layoutingControl.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox.Add( layoutingControl );

  auto control = Control::New();
  control.SetName( "fitToChildrenControl" );
  DevelControl::SetLayoutingRequired( control, true );
  control.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );
  layoutingControl.Add( control );

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

  DALI_TEST_EQUALS( control.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayoutingResizePolicy_03(void)
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
  tet_infoline("UtcDaliLayoutingResizePolicy_03 - Set ResizePolicy SIZE_RELATIVE_TO_PARENT on Control");

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

  auto layoutingControl = Control::New();
  layoutingControl.SetName( "layoutingRequiredControl" );
  DevelControl::SetLayoutingRequired( layoutingControl, true );
  layoutingControl.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  layoutingControl.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox.Add( layoutingControl );

  auto control = Control::New();
  control.SetName( "fitToChildrenControl" );
  DevelControl::SetLayoutingRequired( control, true );
  control.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
  control.SetSizeModeFactor( Vector3( 0.50f, 1.0f, 1.0f ) );
  layoutingControl.Add( control );

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

  DALI_TEST_EQUALS( control.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 240.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayoutingResizePolicy_04(void)
{
  /*
  Root
    |
  Control (LinearLayout Horizontal)
    |
  Control (LayoutingRequired)
    |
  Control (ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT)
    |
  LeafControl
  */

  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayoutingResizePolicy_04 - Set ResizePolicy SIZE_FIXED_OFFSET_FROM_PARENT on Control");

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

  auto layoutingControl = Control::New();
  layoutingControl.SetName( "layoutingRequiredControl" );
  DevelControl::SetLayoutingRequired( layoutingControl, true );
  layoutingControl.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  layoutingControl.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox.Add( layoutingControl );

  auto control = Control::New();
  control.SetName( "fitToChildrenControl" );
  DevelControl::SetLayoutingRequired( control, true );
  control.SetResizePolicy( ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::ALL_DIMENSIONS );
  control.SetSizeModeFactor( Vector3( -100.0f, 10.0f, 0.0f ) );
  layoutingControl.Add( control );

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

int UtcDaliLayoutingResizePolicy_05(void)
{
  /*
  Root
    |
  Control (LinearLayout Horizontal)
    |
  Control (LayoutingRequired)
    |
  Control (ResizePolicy::FIXED)
    |
  LeafControl
  */

  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayoutingResizePolicy_05 - Set ResizePolicy FIXED on Control");

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

  auto layoutingControl = Control::New();
  layoutingControl.SetName( "layoutingRequiredControl" );
  DevelControl::SetLayoutingRequired( layoutingControl, true );
  layoutingControl.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  layoutingControl.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox.Add( layoutingControl );

  auto control = Control::New();
  control.SetName( "fitToChildrenControl" );
  DevelControl::SetLayoutingRequired( control, true );
  control.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
  control.SetSize( Vector3( 300.0f, 300.0f, 0.0f ) );
  layoutingControl.Add( control );

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

  DALI_TEST_EQUALS( control.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 300.0f, 300.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}