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

#include <layout-utils.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_absolute_layout_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_absolute_layoutg_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliLayouting_AbsoluteLayoutDownCast(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliLayouting_AbsoluteLayoutDownCast - Testing Downcast");

  AbsoluteLayout absoluteLayout = AbsoluteLayout::New();

  LayoutGroup layoutGroup( absoluteLayout );

  AbsoluteLayout absoluteLayoutCandidate = AbsoluteLayout::DownCast( layoutGroup );
  DALI_TEST_CHECK( absoluteLayoutCandidate );

  END_TEST;
}

int UtcDaliLayouting_AbsoluteLayoutAssignment(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliLayouting_AbsoluteLayoutAssignment - Testing operator=");

  AbsoluteLayout absoluteLayout = AbsoluteLayout::New();
  AbsoluteLayout absoluteLayout2;

  absoluteLayout2 = absoluteLayout;

  DALI_TEST_CHECK( absoluteLayout2 == absoluteLayout );

  END_TEST;
}

int UtcDaliLayouting_AbsoluteLayout01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_AbsoluteLayout01 - Position an item with Actor::Property::POSITION");

  Stage stage = Stage::GetCurrent();
  auto absoluteLayout = Control::New();
  auto layout = AbsoluteLayout::New();
  layout.SetAnimateLayout( true );
  DevelControl::SetLayout( absoluteLayout, layout );
  absoluteLayout.SetName( "AsoluteLayout");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );

  // Position one of the  controls using the actor property.
  controls[1].SetProperty(Actor::Property::POSITION, Vector3( 100.0f, 0.0f, 0.0f) );

  for( auto&& iter : controls )
  {
    absoluteLayout.Add( iter );
  }
  absoluteLayout.SetParentOrigin( ParentOrigin::CENTER );
  absoluteLayout.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( absoluteLayout );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // AbsoluteLayout renders items at the positions given by their Actor::Property::POSITION relative to the top left of the container.
  // Items can overlap or spill out of their parent container.
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // The controls[1] was the only control to have a defiend position
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Items size should not change regardless of parent's size.
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_AbsoluteLayout_SetPosition(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_AbsoluteLayout_GetWorldPosition - Testing WorldPosition");

  Stage stage = Stage::GetCurrent();

  Dali::Toolkit::Control layoutControl = Dali::Toolkit::Control::New();
  layoutControl.SetName("AsoluteLayout");
  layoutControl.SetAnchorPoint( Dali::AnchorPoint::CENTER );
  layoutControl.SetParentOrigin( Dali::ParentOrigin::CENTER );

  Dali::Toolkit::AbsoluteLayout absoluteLayout = Dali::Toolkit::AbsoluteLayout::New();
  Dali::Toolkit::DevelControl::SetLayout( layoutControl, absoluteLayout );

  stage.GetRootLayer().Add( layoutControl );

  // Ensure layouting happens
  application.SendNotification();
  application.Render(0);

  Dali::Toolkit::Control control = Dali::Toolkit::Control::New();
  control.SetSize( 100.0f, 100.0f );
  control.SetParentOrigin( ParentOrigin::CENTER );
  control.SetAnchorPoint( AnchorPoint::CENTER );
  Vector3 parentPosition( 1.0f, 2.0f, 3.0f );
  control.SetPosition( parentPosition );

  layoutControl.Add( control );

  // Ensure layouting happens
  application.SendNotification();
  application.Render(0);

  // The value of z should not be zero
  DALI_TEST_EQUALS( control.GetCurrentPosition(), parentPosition, TEST_LOCATION );

  END_TEST;
}
