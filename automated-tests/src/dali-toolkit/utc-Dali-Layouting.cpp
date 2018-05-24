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
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/hbox-layout.h>
#include <dali-toolkit/devel-api/layouting/vbox-layout.h>
#include <dali-toolkit/devel-api/layouting/flex-layout.h>
//#include <dali-toolkit/internal/controls/control/control-data-impl-debug.h>

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


Control CreateLeafControl( int width, int height )
{
  auto control = Control::New();
  control.SetName( "Leaf" );

  auto pixelBuffer = Devel::PixelBuffer::New( 1, 1, Pixel::RGB888 );
  unsigned char* pixels = pixelBuffer.GetBuffer();
  pixels[0] = 0xff;
  pixels[1] = 0x00;
  pixels[2] = 0x00;
  auto texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGB888, 1, 1 );
  auto pixelData = Devel::PixelBuffer::Convert( pixelBuffer );
  texture.Upload( pixelData );
  std::string url = TextureManager::AddTexture( texture );

  Property::Map map;
  map[ Visual::Property::TYPE ] = Visual::IMAGE;
  map[ ImageVisual::Property::URL ] = url;
  map[ ImageVisual::Property::DESIRED_WIDTH ] = (float) width;
  map[ ImageVisual::Property::DESIRED_HEIGHT ] = (float) height;
  control.SetProperty( Control::Property::BACKGROUND, map );
  return control;
}

int UtcDaliLayouting_HboxLayout01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_HboxLayout01");

  Stage stage = Stage::GetCurrent();
  auto hbox = Control::New();
  auto hboxLayout = HboxLayout::New();
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

  END_TEST;
}

int UtcDaliLayouting_HboxLayout02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_HboxLayout02 Test nested hboxes with default spec of WRAP_CONTENT");

  Stage stage = Stage::GetCurrent();

  auto hbox1 = Control::New();
  auto hboxLayout1 = HboxLayout::New();
  DevelControl::SetLayout( hbox1, hboxLayout1 );

  auto hbox2 = Control::New();
  auto hboxLayout2 = HboxLayout::New();
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
  auto hboxLayout3 = HboxLayout::New();
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
  auto hboxLayout1 = HboxLayout::New();
  DevelControl::SetLayout( hbox1, hboxLayout1 );

  auto hbox2 = Control::New();
  auto hboxLayout2 = HboxLayout::New();
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
  auto hboxLayout3 = HboxLayout::New();
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
  auto hboxLayout1 = HboxLayout::New();
  DevelControl::SetLayout( hbox1, hboxLayout1 );

  auto hbox2 = Control::New();
  auto hboxLayout2 = HboxLayout::New();
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
  auto hboxLayout3 = HboxLayout::New();
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

// Padding tests

int UtcDaliLayouting_HboxLayout_Padding01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayouting_HboxLayout_Padding01 - Adding Padding to a single child");

  Stage stage = Stage::GetCurrent();
  auto hbox = Control::New();
  auto hboxLayout = HboxLayout::New();
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
  auto hboxLayout = HboxLayout::New();
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
  auto hboxLayout = HboxLayout::New();
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
  auto hboxLayout = HboxLayout::New();
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
  auto vboxLayout = VboxLayout::New();
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
  auto hbox = Control::New();
  auto hboxLayout = HboxLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "Hbox");
  stage.Add( hbox );

  auto vbox = Control::New();
  auto vboxLayout = VboxLayout::New();
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
  auto hboxLayout = HboxLayout::New();
  DevelControl::SetLayout( hbox, hboxLayout );
  hbox.SetName( "Hbox");
  stage.Add( hbox );

  auto vbox = Control::New();
  auto vboxLayout = VboxLayout::New();
  vboxLayout.SetCellPadding( LayoutSize( 0, 10 ) );

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

int UtcDaliLayouting_FlexLayout01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_FlexLayout01");

  Stage stage = Stage::GetCurrent();
  auto flexBox = Control::New();
  auto flexLayout = FlexLayout::New();
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::ROW );
  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::ALIGN_CENTER );
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

  // flexbox centers elements vertically, it fills test harness stage, which is 480x800.
  // left aligned elements with right margin 10
  // To see goto https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjIsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJtYXJnaW4iOnsicmlnaHQiOiIxMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fSx7IndpZHRoIjoiNjAiLCJoZWlnaHQiOiI2MCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsIm1hcmdpbiI6eyJyaWdodCI6IjEwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19LHsid2lkdGgiOjEwMCwiaGVpZ2h0IjoxMDAsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsIm1hcmdpbiI6eyJyaWdodCI6IjEwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19XX0=
  DALI_TEST_EQUALS( flexBox.GetProperty<Vector3>(Actor::Property::POSITION), Vector3(0,0,0),TEST_LOCATION);
  DALI_TEST_EQUALS( flexBox.GetProperty<Vector3>(Actor::Property::SIZE), Vector3(480,800,0),TEST_LOCATION);

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 50.0f, 370.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 120.0f, 360.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 210.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_FlexLayout02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_FlexLayout02");

  Stage stage = Stage::GetCurrent();
  auto flexBox = Control::New();
  auto flexLayout = FlexLayout::New();
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::ROW );
  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::ALIGN_CENTER );

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

  flexLayout1.SetFlexDirection( Dali::Toolkit::FlexLayout::ROW );
  flexLayout1.SetFlexWrap(Dali::Toolkit::FlexLayout::NO_WRAP);
  flexLayout1.SetFlexAlignment( Dali::Toolkit::FlexLayout::ALIGN_FLEX_START );
  flexLayout1.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::ALIGN_FLEX_START );
  flexLayout1.SetFlexJustification( Dali::Toolkit::FlexLayout::JUSTIFY_FLEX_START );

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

  flexLayout2.SetFlexDirection( Dali::Toolkit::FlexLayout::COLUMN );
  flexLayout2.SetFlexWrap(Dali::Toolkit::FlexLayout::NO_WRAP);
  flexLayout2.SetFlexAlignment( Dali::Toolkit::FlexLayout::ALIGN_FLEX_START );
  flexLayout2.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::ALIGN_FLEX_START );
  flexLayout2.SetFlexJustification( Dali::Toolkit::FlexLayout::JUSTIFY_FLEX_START );

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
  flexLayout.SetFlexAlignment( Dali::Toolkit::FlexLayout::ALIGN_FLEX_START );
  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::ALIGN_FLEX_START );
  flexLayout.SetFlexWrap(Dali::Toolkit::FlexLayout::NO_WRAP);
  flexLayout.SetFlexJustification( Dali::Toolkit::FlexLayout::JUSTIFY_FLEX_START );
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::COLUMN );
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
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::COLUMN_REVERSE );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MSwiZmxleERpcmVjdGlvbiI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJtYXJnaW4iOnsicmlnaHQiOiIwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19LHsid2lkdGgiOiI2MCIsImhlaWdodCI6IjYwIiwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwibWFyZ2luIjp7InJpZ2h0IjoiMTAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 760.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 700.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Ensure layouting happens
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::ROW );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJtYXJnaW4iOnsicmlnaHQiOiIwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19LHsid2lkdGgiOiI2MCIsImhlaWdodCI6IjYwIiwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwibWFyZ2luIjp7InJpZ2h0IjoiMTAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Ensure layouting happens
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::ROW_REVERSE );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MSwiZmxleERpcmVjdGlvbiI6MywicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJtYXJnaW4iOnsicmlnaHQiOiIwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19LHsid2lkdGgiOiI2MCIsImhlaWdodCI6IjYwIiwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwibWFyZ2luIjp7InJpZ2h0IjoiMTAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 440.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 380.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Ensure layouting happens
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::ROW );
  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::ALIGN_CENTER );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjIsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiY2hpbGRyZW4iOlt7IndpZHRoIjoiNDAiLCJoZWlnaHQiOiI0MCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjYwIiwiaGVpZ2h0IjoiNjAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJtYXJnaW4iOnsicmlnaHQiOiIxMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fV19
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 370.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Ensure layouting happens
  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::ALIGN_FLEX_END );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjMsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiY2hpbGRyZW4iOlt7IndpZHRoIjoiNDAiLCJoZWlnaHQiOiI0MCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjYwIiwiaGVpZ2h0IjoiNjAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJtYXJnaW4iOnsicmlnaHQiOiIxMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fV19
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 760.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 740.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Ensure layouting happens
  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::ALIGN_STRETCH );
  application.SendNotification();
  application.Render();
/* TODO: fix
  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiY2hpbGRyZW4iOlt7IndpZHRoIjoiNDAiLCJoZWlnaHQiOiI0MCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjYwIiwiaGVpZ2h0IjoiNjAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJtYXJnaW4iOnsicmlnaHQiOiIxMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fV19
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
*/

  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::ALIGN_FLEX_START );
  flexLayout.SetFlexJustification( Dali::Toolkit::FlexLayout::JUSTIFY_FLEX_START );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiY2hpbGRyZW4iOlt7IndpZHRoIjoiNDAiLCJoZWlnaHQiOiI0MCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjYwIiwiaGVpZ2h0IjoiNjAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJtYXJnaW4iOnsicmlnaHQiOiIxMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fV19
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexLayout.SetFlexJustification( Dali::Toolkit::FlexLayout::JUSTIFY_CENTER );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImp1c3RpZnlDb250ZW50IjoxLCJhbGlnbkl0ZW1zIjoxLCJhbGlnbkNvbnRlbnQiOjEsInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH0sImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJtYXJnaW4iOnsicmlnaHQiOiIwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19LHsid2lkdGgiOiI2MCIsImhlaWdodCI6IjYwIiwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwibWFyZ2luIjp7InJpZ2h0IjoiMTAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 190.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 230.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexLayout.SetFlexJustification( Dali::Toolkit::FlexLayout::JUSTIFY_FLEX_END );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImp1c3RpZnlDb250ZW50IjoyLCJhbGlnbkl0ZW1zIjoxLCJhbGlnbkNvbnRlbnQiOjEsInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH0sImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJtYXJnaW4iOnsicmlnaHQiOiIwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19LHsid2lkdGgiOiI2MCIsImhlaWdodCI6IjYwIiwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwibWFyZ2luIjp7InJpZ2h0IjoiMTAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 190.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 380.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 420.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexLayout.SetFlexJustification( Dali::Toolkit::FlexLayout::JUSTIFY_SPACE_BETWEEN );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImp1c3RpZnlDb250ZW50IjozLCJhbGlnbkl0ZW1zIjoxLCJhbGlnbkNvbnRlbnQiOjEsInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH0sImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjoxLCJtYXJnaW4iOnsicmlnaHQiOiIwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19LHsid2lkdGgiOiI2MCIsImhlaWdodCI6IjYwIiwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwiYWxpZ25JdGVtcyI6MSwibWFyZ2luIjp7InJpZ2h0IjoiMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fV19  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 420.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 40.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 60.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexLayout.SetFlexJustification( Dali::Toolkit::FlexLayout::JUSTIFY_SPACE_AROUND );
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImp1c3RpZnlDb250ZW50Ijo0LCJhbGlnbkl0ZW1zIjoxLCJhbGlnbkNvbnRlbnQiOjEsInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH0sImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjQwIiwiaGVpZ2h0IjoiNDAiLCJtaW5XaWR0aCI6bnVsbCwibWluSGVpZ2h0IjpudWxsLCJtYXhXaWR0aCI6bnVsbCwibWF4SGVpZ2h0IjpudWxsLCJhbGlnbkl0ZW1zIjoxLCJtYXJnaW4iOnsicmlnaHQiOiIwIn0sInBvc2l0aW9uIjp7InRvcCI6bnVsbCwicmlnaHQiOm51bGwsImJvdHRvbSI6bnVsbCwibGVmdCI6bnVsbH19LHsid2lkdGgiOiI2MCIsImhlaWdodCI6IjYwIiwibWluV2lkdGgiOm51bGwsIm1pbkhlaWdodCI6bnVsbCwibWF4V2lkdGgiOm51bGwsIm1heEhlaWdodCI6bnVsbCwiYWxpZ25JdGVtcyI6MSwibWFyZ2luIjp7InJpZ2h0IjoiMCJ9LCJwb3NpdGlvbiI6eyJ0b3AiOm51bGwsInJpZ2h0IjpudWxsLCJib3R0b20iOm51bGwsImxlZnQiOm51bGx9fV19  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 95.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
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
  flexLayout.SetFlexAlignment( Dali::Toolkit::FlexLayout::ALIGN_FLEX_START );
  flexLayout.SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::ALIGN_FLEX_START );
  flexLayout.SetFlexWrap(Dali::Toolkit::FlexLayout::NO_WRAP);
  flexLayout.SetFlexJustification( Dali::Toolkit::FlexLayout::JUSTIFY_FLEX_START );
  flexLayout.SetFlexDirection( Dali::Toolkit::FlexLayout::ROW );
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

  flexLayout.SetFlexWrap(Dali::Toolkit::FlexLayout::WRAP);
  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjIwMCIsImhlaWdodCI6IjIwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjQwMCIsImhlaWdodCI6IjQwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 200.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 400.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexLayout.SetFlexAlignment(Dali::Toolkit::FlexLayout::ALIGN_CENTER);
  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MiwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjIwMCIsImhlaWdodCI6IjIwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjQwMCIsImhlaWdodCI6IjQwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 300.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 200.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 400.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexLayout.SetFlexAlignment(Dali::Toolkit::FlexLayout::ALIGN_FLEX_END);
  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // To see the test go to https://yogalayout.com/playground/#eyJ3aWR0aCI6IjQ4MCIsImhlaWdodCI6IjgwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsImFsaWduQ29udGVudCI6MiwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfSwiZmxleFdyYXAiOjEsImNoaWxkcmVuIjpbeyJ3aWR0aCI6IjIwMCIsImhlaWdodCI6IjIwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX0seyJ3aWR0aCI6IjQwMCIsImhlaWdodCI6IjQwMCIsIm1pbldpZHRoIjpudWxsLCJtaW5IZWlnaHQiOm51bGwsIm1heFdpZHRoIjpudWxsLCJtYXhIZWlnaHQiOm51bGwsImFsaWduSXRlbXMiOjEsIm1hcmdpbiI6eyJyaWdodCI6IjAifSwicG9zaXRpb24iOnsidG9wIjpudWxsLCJyaWdodCI6bnVsbCwiYm90dG9tIjpudWxsLCJsZWZ0IjpudWxsfX1dfQ==
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 200.0f, 200.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 400.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  flexLayout.SetFlexAlignment(Dali::Toolkit::FlexLayout::ALIGN_STRETCH);
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
