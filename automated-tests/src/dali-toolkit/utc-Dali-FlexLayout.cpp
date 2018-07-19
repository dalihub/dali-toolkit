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
#include <dali-toolkit/devel-api/layouting/flex-layout.h>

#include <layout-utils.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_flexlayout_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_flexlayout_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliLayouting_FlexLayout01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_FlexLayout01");

  Stage stage = Stage::GetCurrent();
  auto flexBox = Control::New();
  auto flexLayout = FlexLayout::New();
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::FlexDirection::ROW );
  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::Alignment::CENTER );
  DevelControl::SetLayout( flexBox, flexLayout );
  flexBox.SetName( "Flexbox");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 60 ) );
  controls.push_back( CreateLeafControl( 80, 80 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );

  const Extents CONTROL_MARGIN = Extents( 0, 10, 0, 0 );
  for( auto&& iter : controls )
  {
    iter.SetProperty(Toolkit::Control::Property::MARGIN, CONTROL_MARGIN );
    flexBox.Add( iter );
  }
  flexBox.SetParentOrigin( ParentOrigin::CENTER );
  flexBox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( flexBox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // To see goto https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjIsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsInBhZGRpbmciOnsidG9wIjoiIiwicmlnaHQiOiIiLCJib3R0b20iOiIiLCJsZWZ0IjoiIn0sIm1hcmdpbiI6eyJyaWdodCI6IjEwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19LHsid2lkdGgiOiI2MCIsImhlaWdodCI6IjYwIiwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwiYWxpZ25JdGVtcyI6MCwiYWxpZ25Db250ZW50IjowLCJwYWRkaW5nIjp7InRvcCI6IiIsInJpZ2h0IjoiIiwiYm90dG9tIjoiIiwibGVmdCI6IiJ9LCJtYXJnaW4iOnsicmlnaHQiOiIxMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fSx7IndpZHRoIjoiODAiLCJoZWlnaHQiOiI4MCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjAsImFsaWduQ29udGVudCI6MCwicGFkZGluZyI6eyJ0b3AiOiIiLCJyaWdodCI6IiIsImJvdHRvbSI6IiIsImxlZnQiOiIifSwibWFyZ2luIjp7InJpZ2h0IjoiMTAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6MTAwLCJoZWlnaHQiOjEwMCwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwiYWxpZ25JdGVtcyI6MCwiYWxpZ25Db250ZW50IjowLCJwYWRkaW5nIjp7InRvcCI6IiIsInJpZ2h0IjoiIiwiYm90dG9tIjoiIiwibGVmdCI6IiJ9LCJtYXJnaW4iOnsicmlnaHQiOiIxMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fV19
  DALI_TEST_EQUALS( flexBox.GetProperty<Vector3>(Actor::Property::POSITION), Vector3( 0, 0, 0 ),TEST_LOCATION);
  DALI_TEST_EQUALS( flexBox.GetProperty<Vector3>(Actor::Property::SIZE), Vector3( 480, 800, 0 ),TEST_LOCATION);

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 50.0f, 370.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 120.0f, 360.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 210.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  const Extents CONTROL_PADDING = Extents( 5, 10, 15, 15 );
  for( auto&& iter : controls )
  {
    iter.SetProperty(Toolkit::Control::Property::PADDING, CONTROL_PADDING );
    flexBox.Add( iter );
  }

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // To see go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjIsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsInBhZGRpbmciOnsidG9wIjoiMTUiLCJyaWdodCI6IjEwIiwiYm90dG9tIjoiMTUiLCJsZWZ0IjoiNSJ9LCJtYXJnaW4iOnsicmlnaHQiOiIxMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fSx7IndpZHRoIjoiNjAiLCJoZWlnaHQiOiI2MCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjAsImFsaWduQ29udGVudCI6MCwicGFkZGluZyI6eyJ0b3AiOiIxNSIsInJpZ2h0IjoiMTAiLCJib3R0b20iOiIxNSIsImxlZnQiOiI1In0sIm1hcmdpbiI6eyJyaWdodCI6IjEwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19LHsid2lkdGgiOiI4MCIsImhlaWdodCI6IjgwIiwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwiYWxpZ25JdGVtcyI6MCwiYWxpZ25Db250ZW50IjowLCJwYWRkaW5nIjp7InRvcCI6IjE1IiwicmlnaHQiOiIxMCIsImJvdHRvbSI6IjE1IiwibGVmdCI6IjUifSwibWFyZ2luIjp7InJpZ2h0IjoiMTAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6MTAwLCJoZWlnaHQiOjEwMCwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwiYWxpZ25JdGVtcyI6MCwiYWxpZ25Db250ZW50IjowLCJwYWRkaW5nIjp7InRvcCI6IjE1IiwicmlnaHQiOiIxMCIsImJvdHRvbSI6IjE1IiwibGVmdCI6IjUifSwibWFyZ2luIjp7InJpZ2h0IjoiMTAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 365.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 65.0f, 355.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 150.0f, 345.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 255.0f, 335.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 55.0f, 70.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 75.0f, 90.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 95.0f, 110.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 115.0f, 130.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  const Extents FLEXBOX_PADDING = Extents( 25, 25, 25, 25 );
  flexBox.SetProperty(Toolkit::Control::Property::PADDING, FLEXBOX_PADDING );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // To see go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjIsImFsaWduQ29udGVudCI6MSwicGFkZGluZyI6eyJ0b3AiOiIyNSIsInJpZ2h0IjoiMjUiLCJib3R0b20iOiIyNSIsImxlZnQiOiIyNSJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9LCJmbGV4V3JhcCI6MSwiY2hpbGRyZW4iOlt7IndpZHRoIjoiNDAiLCJoZWlnaHQiOiI0MCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjAsImFsaWduQ29udGVudCI6MCwicGFkZGluZyI6eyJ0b3AiOiIxNSIsInJpZ2h0IjoiMTAiLCJib3R0b20iOiIxNSIsImxlZnQiOiI1In0sIm1hcmdpbiI6eyJyaWdodCI6IjEwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19LHsid2lkdGgiOiI2MCIsImhlaWdodCI6IjYwIiwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwiYWxpZ25JdGVtcyI6MCwiYWxpZ25Db250ZW50IjowLCJwYWRkaW5nIjp7InRvcCI6IjE1IiwicmlnaHQiOiIxMCIsImJvdHRvbSI6IjE1IiwibGVmdCI6IjUifSwibWFyZ2luIjp7InJpZ2h0IjoiMTAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjgwIiwiaGVpZ2h0IjoiODAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsInBhZGRpbmciOnsidG9wIjoiMTUiLCJyaWdodCI6IjEwIiwiYm90dG9tIjoiMTUiLCJsZWZ0IjoiNSJ9LCJtYXJnaW4iOnsicmlnaHQiOiIxMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fSx7IndpZHRoIjoxMDAsImhlaWdodCI6MTAwLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsInBhZGRpbmciOnsidG9wIjoiMTUiLCJyaWdodCI6IjEwIiwiYm90dG9tIjoiMTUiLCJsZWZ0IjoiNSJ9LCJtYXJnaW4iOnsicmlnaHQiOiIxMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fV19
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 25.0f, 365.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 90.0f, 355.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 175.0f, 345.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 280.0f, 335.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 55.0f, 70.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 75.0f, 90.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 95.0f, 110.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 115.0f, 130.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_FlexLayout02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_FlexLayout02");

  Stage stage = Stage::GetCurrent();
  auto flexBox = Control::New();
  auto flexLayout = FlexLayout::New();
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::FlexDirection::ROW );
  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::Alignment::CENTER );

  DevelControl::SetLayout( flexBox, flexLayout );
  flexBox.SetName( "Flexbox");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 60 ) );
  controls.push_back( CreateLeafControl( 80, 80 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );

  const Extents CONTROL_MARGIN = Extents( 0, 10, 0, 0 );
  for( auto&& iter : controls )
  {
    iter.SetProperty( Toolkit::Control::Property::MARGIN, CONTROL_MARGIN );
    flexBox.Add( iter );
  }

  controls[1].SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  controls[1].SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  flexBox.SetParentOrigin( ParentOrigin::CENTER );
  flexBox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( flexBox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // flexbox centers elements vertically, it fills test harness stage, which is 480x800.
  DALI_TEST_EQUALS( flexBox.GetProperty<Vector3>(Actor::Property::POSITION), Vector3( 0, 0, 0 ),TEST_LOCATION);
  DALI_TEST_EQUALS( flexBox.GetProperty<Vector3>(Actor::Property::SIZE), Vector3( 480, 800, 0 ),TEST_LOCATION);

  DALI_TEST_EQUALS( controls[1].GetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION ), Property::Value( ChildLayoutData::WRAP_CONTENT ), TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION ), Property::Value( ChildLayoutData::MATCH_PARENT ), TEST_LOCATION );

  // left aligned elements with right margin 10
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 50.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 120.0f, 360.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 210.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_FlexLayout03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_FlexLayout03 Test nested flex boxes");

  Stage stage = Stage::GetCurrent();

  auto flexbox1 = Control::New();
  auto flexLayout1 = FlexLayout::New();
  DevelControl::SetLayout( flexbox1, flexLayout1 );
  flexbox1.SetName( "Flexbox1");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 20, 20 ) );
  controls.push_back( CreateLeafControl( 30, 30 ) );
  flexbox1.Add( controls[0] );
  flexbox1.Add( controls[1] );
  flexbox1.SetParentOrigin( ParentOrigin::CENTER );
  flexbox1.SetAnchorPoint( AnchorPoint::CENTER );

  flexLayout1.SetFlexDirection( Dali::Toolkit::FlexLayout::FlexDirection::ROW );
  flexLayout1.SetFlexWrap(Dali::Toolkit::FlexLayout::WrapType::NO_WRAP);
  flexLayout1.SetFlexAlignment( Dali::Toolkit::FlexLayout::Alignment::FLEX_START );
  flexLayout1.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::Alignment::FLEX_START );
  flexLayout1.SetFlexJustification( Dali::Toolkit::FlexLayout::Justification::FLEX_START );

  auto flexbox2 = Control::New();
  auto flexLayout2 = FlexLayout::New();
  DevelControl::SetLayout( flexbox2, flexLayout2 );
  flexbox2.SetName( "Flexbox2");

  controls.push_back( CreateLeafControl( 25, 25 ) );
  controls.push_back( CreateLeafControl( 35, 35 ) );
  flexbox2.Add( controls[2] );
  flexbox2.Add( controls[3] );
  flexbox2.SetParentOrigin( ParentOrigin::CENTER );
  flexbox2.SetAnchorPoint( AnchorPoint::CENTER );

  flexLayout2.SetFlexDirection( Dali::Toolkit::FlexLayout::FlexDirection::COLUMN );
  flexLayout2.SetFlexWrap(Dali::Toolkit::FlexLayout::WrapType::NO_WRAP);
  flexLayout2.SetFlexAlignment( Dali::Toolkit::FlexLayout::Alignment::FLEX_START );
  flexLayout2.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::Alignment::FLEX_START );
  flexLayout2.SetFlexJustification( Dali::Toolkit::FlexLayout::Justification::FLEX_START );

  auto flexbox3 = Control::New();
  auto flexLayout3 = FlexLayout::New();
  DevelControl::SetLayout( flexbox3, flexLayout3 );
  flexbox3.SetParentOrigin( ParentOrigin::CENTER );
  flexbox3.SetName( "Flexbox3");
  flexbox3.Add( flexbox1 );
  flexbox3.Add( flexbox2 );
  stage.Add( flexbox3 );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 20.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 30.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 55.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 20.0f, 20.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 30.0f, 30.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 25.0f, 25.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 35.0f, 35.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Test flexbox1,2 are sized to wrap their content
  DALI_TEST_EQUALS( flexbox1.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 50.0f, 30.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox2.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 35.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox1.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox2.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 30.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Test flexbox3 matches parent (root layer)
  DALI_TEST_EQUALS( flexbox3.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox3.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_FlexLayout04(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_FlexLayout04");

  Stage stage = Stage::GetCurrent();
  auto flexBox = Control::New();
  auto flexLayout = FlexLayout::New();
  flexLayout.SetFlexAlignment( Dali::Toolkit::FlexLayout::Alignment::FLEX_START );
  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::Alignment::FLEX_START );
  flexLayout.SetFlexWrap(Dali::Toolkit::FlexLayout::WrapType::NO_WRAP);
  flexLayout.SetFlexJustification( Dali::Toolkit::FlexLayout::Justification::FLEX_START );
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::FlexDirection::COLUMN );
  DevelControl::SetLayout( flexBox, flexLayout );
  flexBox.SetName( "Flexbox");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 60 ) );
  for( auto&& iter : controls )
  {
    flexBox.Add( iter );
  }
  flexBox.SetParentOrigin( ParentOrigin::CENTER );
  flexBox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( flexBox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MSwiZmxleERpcmVjdGlvbiI6MCwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJtYXJnaW4iOnsicmlnaHQiOiIwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19LHsid2lkdGgiOiI2MCIsImhlaWdodCI6IjYwIiwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwibWFyZ2luIjp7InJpZ2h0IjoiMTAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Ensure layouting happens
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::FlexDirection::COLUMN_REVERSE );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MSwiZmxleERpcmVjdGlvbiI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJtYXJnaW4iOnsicmlnaHQiOiIwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19LHsid2lkdGgiOiI2MCIsImhlaWdodCI6IjYwIiwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwibWFyZ2luIjp7InJpZ2h0IjoiMTAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 760.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 700.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Ensure layouting happens
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::FlexDirection::ROW );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJtYXJnaW4iOnsicmlnaHQiOiIwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19LHsid2lkdGgiOiI2MCIsImhlaWdodCI6IjYwIiwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwibWFyZ2luIjp7InJpZ2h0IjoiMTAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Ensure layouting happens
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::FlexDirection::ROW_REVERSE );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MSwiZmxleERpcmVjdGlvbiI6MywicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJtYXJnaW4iOnsicmlnaHQiOiIwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19LHsid2lkdGgiOiI2MCIsImhlaWdodCI6IjYwIiwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwibWFyZ2luIjp7InJpZ2h0IjoiMTAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 440.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 380.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Ensure layouting happens
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::FlexDirection::ROW );
  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::Alignment::CENTER );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjIsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiY2hpbGRyZW4iOlt7IndpZHRoIjoiNDAiLCJoZWlnaHQiOiI0MCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjYwIiwiaGVpZ2h0IjoiNjAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJtYXJnaW4iOnsicmlnaHQiOiIxMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fV19
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 370.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Ensure layouting happens
  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::Alignment::FLEX_END );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjMsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiY2hpbGRyZW4iOlt7IndpZHRoIjoiNDAiLCJoZWlnaHQiOiI0MCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjYwIiwiaGVpZ2h0IjoiNjAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJtYXJnaW4iOnsicmlnaHQiOiIxMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fV19
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 760.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 740.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Ensure layouting happens
  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::Alignment::STRETCH );
  controls[0].SetProperty( Actor::Property::MAXIMUM_SIZE, Vector2(40.f, 40.f) );
  controls[1].SetProperty( Actor::Property::MAXIMUM_SIZE, Vector2(60.f, 60.f) );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiY2hpbGRyZW4iOlt7IndpZHRoIjoiNDAiLCJoZWlnaHQiOiI0MCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjAsImFsaWduQ29udGVudCI6MCwibWFyZ2luIjp7InJpZ2h0IjoiMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fSx7IndpZHRoIjoiNjAiLCJoZWlnaHQiOiI2MCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjAsImFsaWduQ29udGVudCI6MCwibWFyZ2luIjp7InJpZ2h0IjoiMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fV19
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::Alignment::FLEX_START );
  flexLayout.SetFlexJustification( Dali::Toolkit::FlexLayout::Justification::FLEX_START );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiY2hpbGRyZW4iOlt7IndpZHRoIjoiNDAiLCJoZWlnaHQiOiI0MCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjAsImFsaWduQ29udGVudCI6MCwibWFyZ2luIjp7InJpZ2h0IjoiMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fSx7IndpZHRoIjoiNjAiLCJoZWlnaHQiOiI2MCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjAsImFsaWduQ29udGVudCI6MCwibWFyZ2luIjp7InJpZ2h0IjoiMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fV19
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexLayout.SetFlexJustification( Dali::Toolkit::FlexLayout::Justification::CENTER );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImp1c3RpZnlDb250ZW50IjoxLCJhbGlnbkl0ZW1zIjoxLCJhbGlnbkNvbnRlbnQiOjEsInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH0sImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjYwIiwiaGVpZ2h0IjoiNjAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 190.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 230.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexLayout.SetFlexJustification( Dali::Toolkit::FlexLayout::Justification::FLEX_END );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImp1c3RpZnlDb250ZW50IjoyLCJhbGlnbkl0ZW1zIjoxLCJhbGlnbkNvbnRlbnQiOjEsInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH0sImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjYwIiwiaGVpZ2h0IjoiNjAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 380.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 420.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexLayout.SetFlexJustification( Dali::Toolkit::FlexLayout::Justification::SPACE_BETWEEN );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImp1c3RpZnlDb250ZW50IjozLCJhbGlnbkl0ZW1zIjoxLCJhbGlnbkNvbnRlbnQiOjEsInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH0sImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjYwIiwiaGVpZ2h0IjoiNjAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 420.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexLayout.SetFlexJustification( Dali::Toolkit::FlexLayout::Justification::SPACE_AROUND );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImp1c3RpZnlDb250ZW50Ijo0LCJhbGlnbkl0ZW1zIjoxLCJhbGlnbkNvbnRlbnQiOjEsInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH0sImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjYwIiwiaGVpZ2h0IjoiNjAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 95.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 325.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_FlexLayout05(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_FlexLayout05");

  Stage stage = Stage::GetCurrent();
  auto flexBox = Control::New();
  auto flexLayout = FlexLayout::New();
  flexLayout.SetFlexAlignment( Dali::Toolkit::FlexLayout::Alignment::FLEX_START );
  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::Alignment::FLEX_START );
  flexLayout.SetFlexWrap(Dali::Toolkit::FlexLayout::WrapType::NO_WRAP);
  flexLayout.SetFlexJustification( Dali::Toolkit::FlexLayout::Justification::FLEX_START );
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::FlexDirection::ROW );
  DevelControl::SetLayout( flexBox, flexLayout );
  flexBox.SetName( "Flexbox");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 200, 200 ) );
  controls.push_back( CreateLeafControl( 400, 400 ) );
  for( auto&& iter : controls )
  {
    flexBox.Add( iter );
  }
  flexBox.SetParentOrigin( ParentOrigin::CENTER );
  flexBox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( flexBox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // Shrink is off
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 200.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 200.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 400.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexLayout.SetFlexWrap(Dali::Toolkit::FlexLayout::WrapType::WRAP);
  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjIwMCIsImhlaWdodCI6IjIwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjQwMCIsImhlaWdodCI6IjQwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 200.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 400.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexLayout.SetFlexAlignment(Dali::Toolkit::FlexLayout::Alignment::CENTER);
  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MiwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjIwMCIsImhlaWdodCI6IjIwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjQwMCIsImhlaWdodCI6IjQwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 300.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 200.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 400.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexLayout.SetFlexAlignment(Dali::Toolkit::FlexLayout::Alignment::FLEX_END);
  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MiwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjIwMCIsImhlaWdodCI6IjIwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjQwMCIsImhlaWdodCI6IjQwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 200.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 400.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexLayout.SetFlexAlignment(Dali::Toolkit::FlexLayout::Alignment::STRETCH);
  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MiwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjIwMCIsImhlaWdodCI6IjIwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjQwMCIsImhlaWdodCI6IjQwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 300.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 200.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 400.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_FlexLayout06(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_FlexLayout06");

  Stage stage = Stage::GetCurrent();
  auto flexBox = Control::New();
  auto flexLayout = FlexLayout::New();
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::FlexDirection::COLUMN );
  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::Alignment::FLEX_START );
  DevelControl::SetLayout( flexBox, flexLayout );
  flexBox.SetName( "Flexbox");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 60 ) );
  controls.push_back( CreateLeafControl( 80, 80 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );
  for( auto&& iter : controls )
  {
    flexBox.Add( iter );
  }
  flexBox.SetParentOrigin( ParentOrigin::CENTER );
  flexBox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( flexBox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // flexbox centers elements vertically, it fills test harness stage, which is 480x800.
  // left aligned elements with right margin 10
  // To see goto https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MSwiZmxleERpcmVjdGlvbiI6MCwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjYwIiwiaGVpZ2h0IjoiNjAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjgwIiwiaGVpZ2h0IjoiODAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6MTAwLCJoZWlnaHQiOjEwMCwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwiYWxpZ25JdGVtcyI6MCwiYWxpZ25Db250ZW50IjowLCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fV19
  DALI_TEST_EQUALS( flexBox.GetProperty<Vector3>(Actor::Property::POSITION), Vector3(0, 0, 0),TEST_LOCATION);
  DALI_TEST_EQUALS( flexBox.GetProperty<Vector3>(Actor::Property::SIZE), Vector3(480, 800, 0),TEST_LOCATION);

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 180.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexBox.Remove( controls[1] );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // To see goto https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MSwiZmxleERpcmVjdGlvbiI6MCwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjgwIiwiaGVpZ2h0IjoiODAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6MTAwLCJoZWlnaHQiOjEwMCwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwiYWxpZ25JdGVtcyI6MCwiYWxpZ25Db250ZW50IjowLCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fV19
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 120.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_FlexLayout07(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_FlexLayout07");

  Stage stage = Stage::GetCurrent();
  auto flexBox = Control::New();
  auto flexLayout = FlexLayout::New();
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::FlexDirection::ROW );
  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::Alignment::CENTER );
  DevelControl::SetLayout( flexBox, flexLayout );
  flexBox.SetName( "Flexbox");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 60 ) );
  controls.push_back( CreateLeafControl( 80, 80 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );

  for( auto&& iter : controls )
  {
    iter.SetProperty( Toolkit::FlexLayout::ChildProperty::FLEX, 0.f );
    iter.SetProperty( Toolkit::FlexLayout::ChildProperty::ALIGN_SELF, Dali::Toolkit::FlexLayout::Alignment::AUTO );
    flexBox.Add( iter );
  }
  flexBox.SetParentOrigin( ParentOrigin::CENTER );
  flexBox.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( flexBox );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // To see goto https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjIsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjYwIiwiaGVpZ2h0IjoiNjAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjgwIiwiaGVpZ2h0IjoiODAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6MTAwLCJoZWlnaHQiOjEwMCwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwiYWxpZ25JdGVtcyI6MCwiYWxpZ25Db250ZW50IjowLCJtYXJnaW4iOnsicmlnaHQiOiIwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19XX0=
  DALI_TEST_EQUALS( flexBox.GetProperty<Vector3>(Actor::Property::POSITION), Vector3( 0, 0, 0 ),TEST_LOCATION);
  DALI_TEST_EQUALS( flexBox.GetProperty<Vector3>(Actor::Property::SIZE), Vector3( 480, 800, 0 ),TEST_LOCATION);

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 370.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 360.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 180.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  for( auto&& iter : controls )
  {
    iter.SetProperty( Toolkit::FlexLayout::ChildProperty::FLEX, 0.25f );
    iter.SetProperty( Toolkit::FlexLayout::ChildProperty::ALIGN_SELF, Toolkit::FlexLayout::Alignment::AUTO );
  }

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // To see go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjIsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleEdyb3ciOiIwLjI1In0seyJ3aWR0aCI6IjYwIiwiaGVpZ2h0IjoiNjAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleEdyb3ciOiIwLjI1In0seyJ3aWR0aCI6IjgwIiwiaGVpZ2h0IjoiODAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleEdyb3ciOiIwLjI1In0seyJ3aWR0aCI6MTAwLCJoZWlnaHQiOjEwMCwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwiYWxpZ25JdGVtcyI6MCwiYWxpZ25Db250ZW50IjowLCJtYXJnaW4iOnsicmlnaHQiOiIwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH0sImZsZXhHcm93IjoiMC4yNSJ9XX0=
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 120.0f, 370.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 240.0f, 360.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 360.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 120.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 120.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 120.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 120.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  for( auto&& iter : controls )
  {
    iter.SetProperty( Toolkit::FlexLayout::ChildProperty::FLEX, 0.f );
  }
  controls[0].SetProperty( Toolkit::FlexLayout::ChildProperty::ALIGN_SELF, Toolkit::FlexLayout::Alignment::CENTER );
  controls[1].SetProperty( Toolkit::FlexLayout::ChildProperty::ALIGN_SELF, Toolkit::FlexLayout::Alignment::FLEX_START );
  controls[2].SetProperty( Toolkit::FlexLayout::ChildProperty::ALIGN_SELF, Toolkit::FlexLayout::Alignment::FLEX_END );
  controls[3].SetProperty( Toolkit::FlexLayout::ChildProperty::ALIGN_SELF, Toolkit::FlexLayout::Alignment::STRETCH );
  controls[3].SetProperty( Actor::Property::MAXIMUM_SIZE, Vector2(100.f, 100.f) );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // To see go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjIsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnbkNvbnRlbnQiOjAsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjYwIiwiaGVpZ2h0IjoiNjAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjowLCJhbGlnblNlbGYiOjEsImFsaWduQ29udGVudCI6MCwibWFyZ2luIjp7InJpZ2h0IjoiMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fSx7IndpZHRoIjoiODAiLCJoZWlnaHQiOiI4MCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjAsImFsaWduU2VsZiI6MywiYWxpZ25Db250ZW50IjowLCJtYXJnaW4iOnsicmlnaHQiOiIwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19LHsid2lkdGgiOjEwMCwiaGVpZ2h0IjoxMDAsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjAsImFsaWduU2VsZiI6NCwiYWxpZ25Db250ZW50IjowLCJtYXJnaW4iOnsicmlnaHQiOiIwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19XX0=
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 720.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 180.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_FlexLayout08(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_FlexLayout08");

  Stage stage = Stage::GetCurrent();

  auto flexbox1 = Control::New();
  auto flexLayout1 = FlexLayout::New();
  DevelControl::SetLayout( flexbox1, flexLayout1 );
  flexbox1.SetName( "Flexbox1");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 20, 20 ) );
  flexbox1.Add( controls[0] );
  flexbox1.SetParentOrigin( ParentOrigin::CENTER );
  flexbox1.SetAnchorPoint( AnchorPoint::CENTER );
  const Extents CONTROL_MARGIN = Extents( 10, 10, 10, 10 );
  const Extents CONTROL_PADDING = Extents( 5, 5, 5, 5 );
  flexbox1.SetProperty( Toolkit::Control::Property::MARGIN, CONTROL_MARGIN );
  flexbox1.SetProperty( Toolkit::Control::Property::PADDING, CONTROL_PADDING );
  flexbox1.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  flexbox1.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );

  auto flexbox2 = Control::New();
  auto flexLayout2 = FlexLayout::New();
  DevelControl::SetLayout( flexbox2, flexLayout2 );
  flexbox2.SetParentOrigin( ParentOrigin::CENTER );
  flexbox2.SetName( "Flexbox2");
  flexbox2.Add( flexbox1 );
  stage.Add( flexbox2 );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 15.0f, 15.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 20.0f, 20.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox1.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 30.0f, 30.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox2.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox1.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 10.0f, 10.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox2.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_FlexLayout_NestedFlexboxWithSpec(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_FlexLayout_NestedFlexboxWithSpec Test nested flex box with wrap content/match parent");

  Stage stage = Stage::GetCurrent();

  auto flexbox1 = Control::New();
  auto flexLayout1 = FlexLayout::New();
  DevelControl::SetLayout( flexbox1, flexLayout1 );
  flexbox1.SetName( "Flexbox1");
  flexbox1.SetParentOrigin( ParentOrigin::CENTER );
  flexbox1.SetAnchorPoint( AnchorPoint::CENTER );

  auto flexbox2 = Control::New();
  auto flexLayout2 = FlexLayout::New();
  DevelControl::SetLayout( flexbox2, flexLayout2 );
  flexbox2.SetParentOrigin( ParentOrigin::CENTER );
  flexbox2.SetAnchorPoint( AnchorPoint::CENTER );
  flexbox2.SetName( "Flexbox2");
  flexbox2.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  flexbox2.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );
  flexbox2.Add( controls[0] );
  flexbox1.Add( flexbox2 );
  stage.Add( flexbox1 );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Test flexbox2 is sized to wrap its content
  DALI_TEST_EQUALS( flexbox1.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox2.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox1.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox2.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexbox2.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  flexbox2.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Test flexbox2 width is sized to match parent
  DALI_TEST_EQUALS( flexbox1.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox2.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox1.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox2.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexbox2.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  flexbox2.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Test flexbox2 height is sized to match parent
  DALI_TEST_EQUALS( flexbox1.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox2.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox1.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox2.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexbox2.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  flexbox2.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Test flexbox2 is sized to match its parent
  DALI_TEST_EQUALS( flexbox1.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox2.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox1.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox2.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_FlexLayout_WithTextLabel(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_FlexLayout_WithTextLabel ");

  Stage stage = Stage::GetCurrent();

  auto flexbox1 = Control::New();
  auto flexLayout1 = FlexLayout::New();
  DevelControl::SetLayout( flexbox1, flexLayout1 );
  flexbox1.SetName( "Flexbox1");
  flexbox1.SetParentOrigin( ParentOrigin::CENTER );
  flexbox1.SetAnchorPoint( AnchorPoint::CENTER );

  std::vector< Control > controls;
  controls.push_back( CreateTextLabel( "W" ) );
  flexbox1.Add( controls[0] );
  stage.Add( flexbox1 );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 54.0f, 64.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Test flexbox2 is sized to wrap its content
  DALI_TEST_EQUALS( flexbox1.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox1.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  TextLabel::DownCast( controls[0] ).SetProperty( TextLabel::Property::TEXT, "WWWW" );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 216.0f, 64.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( flexbox1.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexbox1.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}
