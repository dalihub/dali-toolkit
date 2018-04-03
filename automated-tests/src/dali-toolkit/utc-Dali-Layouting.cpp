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
#include <dali-toolkit/devel-api/controls/layouting/hbox-view.h>
#include <dali-toolkit/internal/controls/control/control-data-impl-debug.h>

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

std::string DumpActor( Actor actor )
{
  std::ostringstream oss;
  oss << "{\n  ";
  const std::string& name = actor.GetName();
  if( ! name.empty() )
  {
    oss << "\"name\":\"" << name << "\",\n";
  }
  oss << "\"id\":\"" << actor.GetId() << "\",\n";
  oss << "\"rendererCount\":" << actor.GetRendererCount() << ",\n";
  oss << "\"properties\":\n{\n";
  Toolkit::Internal::DumpProperties( oss, actor ) << "}\n";
  oss << "}\n";
  return oss.str();
}

void DumpControlHierarchy( std::ostream& o, Actor actor )
{
  auto control = Control::DownCast( actor );
  o << "{\n";
  if( control )
  {
    o << "\"Control\":" << Toolkit::Internal::DumpControl( Toolkit::Internal::GetImplementation( control ) );
  }
  else
  {
    o << "\"Actor\":" << DumpActor( actor );
  }
  o << ",\n\"children\":[\n";
  bool first=true;
  for( auto count=actor.GetChildCount(), i=0u; i<count; ++i )
  {
    if( !first )
    {
      o << ",";
    }
    first = false;
    o << "\n";
    DumpControlHierarchy( o, actor.GetChildAt( i ) );
  }
  o << "]}\n";
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
  auto hbox = HboxView::New();
  hbox.SetName( "HBox");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 40, 40 ) );
  controls.push_back( CreateLeafControl( 60, 40 ) );
  controls.push_back( CreateLeafControl( 80, 40 ) );
  controls.push_back( CreateLeafControl( 100, 40 ) );

  for( auto&& iter : controls )
  {
    hbox.AddChild( iter );
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
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 40.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 180.0f, 380.0f, 0.0f ), 0.0001f, TEST_LOCATION );

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
  auto hbox1 = HboxView::New();
  auto hbox2 = HboxView::New();
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
      hbox1.AddChild( iter );
    }
    else
    {
      hbox2.AddChild( iter );
    }
    ++counter;
  }
  hbox1.SetParentOrigin( ParentOrigin::CENTER );
  hbox1.SetAnchorPoint( AnchorPoint::CENTER );
  hbox2.SetParentOrigin( ParentOrigin::CENTER );
  hbox2.SetAnchorPoint( AnchorPoint::CENTER );

  auto hbox3 = HboxView::New();
  hbox3.SetParentOrigin( ParentOrigin::CENTER );
  hbox3.SetName( "HBox3");
  hbox3.AddChild( hbox1 );
  hbox3.AddChild( hbox2 );

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
  auto hbox1 = HboxView::New();
  auto hbox2 = HboxView::New();
  hbox1.SetName( "HBox1");
  hbox2.SetName( "HBox2");
  hbox1.SetProperty( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox1.SetProperty( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox2.SetProperty( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  hbox2.SetProperty( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  // hbox1.SetLayoutData( MarginLayoutData::New( ChildLayoutData::MATCH_PARENT, , 0,0,0,0 ));
  // hbox2.SetLayoutData( MarginLayoutData::New( ChildLayoutData::MATCH_PARENT, ChildLayoutData::MATCH_PARENT, 0,0,0,0 ));

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
      hbox1.AddChild( iter );
    }
    else
    {
      hbox2.AddChild( iter );
    }
    ++counter;
  }
  hbox1.SetParentOrigin( ParentOrigin::CENTER );
  hbox1.SetAnchorPoint( AnchorPoint::CENTER );
  hbox2.SetParentOrigin( ParentOrigin::CENTER );
  hbox2.SetAnchorPoint( AnchorPoint::CENTER );

  auto hbox3 = HboxView::New();
  hbox3.SetParentOrigin( ParentOrigin::CENTER );
  hbox3.SetName( "HBox3");
  hbox3.AddChild( hbox1 );
  hbox3.AddChild( hbox2 );

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
  auto hbox1 = HboxView::New();
  auto hbox2 = HboxView::New();
  hbox1.SetName( "HBox1"); // Default spec is to wrap content
  hbox2.SetName( "HBox2");
  hbox1.SetProperty( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  hbox1.SetProperty( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  hbox2.SetProperty( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  hbox2.SetProperty( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );

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
      hbox1.AddChild( iter );
    }
    else
    {
      hbox2.AddChild( iter );
    }
    ++counter;
  }


  auto hbox3 = HboxView::New();
  hbox3.SetParentOrigin( ParentOrigin::CENTER );
  hbox3.SetName( "HBox3");
  hbox3.AddChild( hbox1 );
  hbox3.AddChild( hbox2 );
  stage.Add( hbox3 );

  std::ostringstream oss;
  DumpControlHierarchy( oss, Stage::GetCurrent().GetRootLayer() );
  printf("Control hierarchy: \n%s\n", oss.str().c_str() );

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
  DALI_TEST_EQUALS( controls[6].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 160.0f,  5.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[7].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 240.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( controls[4].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 40.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[5].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 50.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[6].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 60.0f, 0.0f ), 0.0001f, TEST_LOCATION );
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
