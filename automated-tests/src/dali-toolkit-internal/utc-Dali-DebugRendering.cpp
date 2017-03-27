/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <unistd.h>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/visuals/wireframe/wireframe-visual.h>
#include <dali-toolkit/devel-api/visuals/text-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/geometry.h>

#include <dali-toolkit/dali-toolkit.h>

#include <toolkit-environment-variable.h> // for setting environment variable: DALI_DEBUG_RENDERING
#include "dummy-control.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* TEST_IMAGE_FILE_NAME =  "image_01.jpg";
const char* TEST_NPATCH_FILE_NAME =  "image_01.9.jpg";
const char* TEST_SVG_FILE_NAME = TEST_RESOURCE_DIR "/svg1.svg";
const char* TEST_GIF_FILE_NAME = TEST_RESOURCE_DIR "/anim.gif";

const std::string DEFAULT_FONT_DIR( "/resources/fonts" );

void TestDebugVisual( Visual::Base& visual, DevelVisual::Type actualType, Vector2 expectedNaturalSize )
{
  DALI_TEST_CHECK( &typeid( Toolkit::Internal::WireframeVisual ) == &typeid( GetImplementation(visual) ) );

  Vector2 naturalSize;
  visual.GetNaturalSize( naturalSize );
  DALI_TEST_EQUALS( naturalSize, expectedNaturalSize, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  Property::Map propertyMap;
  visual.CreatePropertyMap( propertyMap );
  Property::Value* typeValue = propertyMap.Find( Visual::Property::TYPE,  Property::INTEGER );
  if ( typeValue )
  {
    DALI_TEST_CHECK( typeValue->Get<int>() == actualType );
  }

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );
  Stage::GetCurrent().Add( actor );

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1, TEST_LOCATION );
  if( actor.GetRendererCount() > 0 )
  {
    Geometry geometry = actor.GetRendererAt( 0 ).GetGeometry();
    DALI_TEST_CHECK( geometry.GetType() == Geometry::LINES );
  }
}
}

void dali_debug_rendering_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_debug_rendering_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliDebugRenderingGetVisual1(void)
{
  EnvironmentVariable::SetTestingEnvironmentVariable(true);
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliDebugRenderingGetVisual1:  Request visual with a Property::Map" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  // Test that color visual is replaced with debug visual
  Property::Map propertyMap1;
  propertyMap1.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap1.Insert(ColorVisual::Property::MIX_COLOR,  Color::BLUE);
  Visual::Base colorVisual = factory.CreateVisual(propertyMap1);
  DALI_TEST_CHECK( colorVisual );
  TestDebugVisual( colorVisual, DevelVisual::COLOR, Vector2::ZERO );

  // Test that border visual is replaced with debug visual
  Property::Map propertyMap2;
  propertyMap2.Insert(Visual::Property::TYPE,  Visual::BORDER);
  propertyMap2.Insert(BorderVisual::Property::COLOR,  Color::BLUE);
  propertyMap2.Insert(BorderVisual::Property::SIZE,  2.f);
  Visual::Base borderVisual = factory.CreateVisual(propertyMap2);
  DALI_TEST_CHECK( borderVisual );
  TestDebugVisual( borderVisual, DevelVisual::BORDER, Vector2::ZERO );

  // Test that gradient visual is replaced with debug visual
  Property::Map propertyMap3;
  propertyMap3.Insert(Visual::Property::TYPE,  Visual::GRADIENT);
  Vector2 start(-1.f, -1.f);
  Vector2 end(1.f, 1.f);
  propertyMap3.Insert(GradientVisual::Property::START_POSITION, start);
  propertyMap3.Insert(GradientVisual::Property::END_POSITION, end);
  propertyMap3.Insert(GradientVisual::Property::SPREAD_METHOD, GradientVisual::SpreadMethod::REPEAT);
  Property::Array stopOffsets;
  stopOffsets.PushBack( 0.2f );
  stopOffsets.PushBack( 0.8f );
  propertyMap3.Insert(GradientVisual::Property::STOP_OFFSET, stopOffsets);
  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap3.Insert(GradientVisual::Property::STOP_COLOR, stopColors);
  Visual::Base gradientVisual = factory.CreateVisual(propertyMap3);
  DALI_TEST_CHECK( gradientVisual );
  TestDebugVisual( gradientVisual, DevelVisual::GRADIENT, Vector2::ZERO );

  // Test that image visual is replaced with debug visual
  Property::Map propertyMap4;
  propertyMap4.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap4.Insert( ImageVisual::Property::URL,  TEST_IMAGE_FILE_NAME );
  propertyMap4.Insert( ImageVisual::Property::DESIRED_WIDTH,  50.f );
  propertyMap4.Insert( ImageVisual::Property::DESIRED_HEIGHT,  100.f );
  Visual::Base imageVisual = factory.CreateVisual( propertyMap4 );
  DALI_TEST_CHECK( imageVisual );
  TestDebugVisual( imageVisual, DevelVisual::IMAGE, Vector2( 50.f, 100.f ) );

  // Test that SVG visual is replaced with debug visual
  // TEST_SVG_FILE:
  //  <svg width="100" height="100">
  //  <circle cx="50" cy="50" r="40" stroke="green" stroke-width="4" fill="yellow" />
  //  </svg>
  Property::Map propertyMap5;
  propertyMap5.Insert( Visual::Property::TYPE, DevelVisual::SVG );
  propertyMap5.Insert( ImageVisual::Property::URL,  TEST_SVG_FILE_NAME );
  Visual::Base svgVisual = factory.CreateVisual( propertyMap5 );
  DALI_TEST_CHECK( svgVisual );
  TestDebugVisual( svgVisual, DevelVisual::SVG, Vector2(100.f, 100.f) );

  // Test that AnimatedImageVisual is replaced with debug visual
  // TEST_GIF_FILE: anim.gif
  // resolution: 50*50, frame count: 4, frame delay: 0.2 second for each frame
  Property::Map propertyMap6;
  propertyMap6.Insert( Visual::Property::TYPE, DevelVisual::ANIMATED_IMAGE );
  propertyMap6.Insert( ImageVisual::Property::URL,  TEST_GIF_FILE_NAME );
  Visual::Base animatedImageVisual = factory.CreateVisual( propertyMap6 );
  DALI_TEST_CHECK( animatedImageVisual );
  TestDebugVisual( animatedImageVisual, DevelVisual::ANIMATED_IMAGE, Vector2(50.f, 50.f) );

  // Test that text visual is replaced with debug visual

  // Load some fonts to get the same metrics on different platforms.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi( 96u, 96u );

  char* pathNamePtr = get_current_dir_name();
  const std::string pathName( pathNamePtr );
  free( pathNamePtr );

  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf" );

  Property::Map propertyMap7;
  propertyMap7.Insert( Visual::Property::TYPE, DevelVisual::TEXT );
  propertyMap7.Insert( TextVisual::Property::ENABLE_MARKUP, true );
  propertyMap7.Insert( TextVisual::Property::TEXT, "<font family='TizenSans' size='12'>Hello world</font>" );
  propertyMap7.Insert( TextVisual::Property::MULTI_LINE, true );

  Visual::Base textVisual = factory.CreateVisual( propertyMap7 );
  DALI_TEST_CHECK( textVisual );
  DALI_TEST_CHECK( &typeid( Toolkit::Internal::WireframeVisual ) == &typeid( GetImplementation(textVisual) ) );

  Vector2 naturalSize;
  textVisual.GetNaturalSize( naturalSize );
  DALI_TEST_EQUALS( naturalSize, Vector2( 80.f, 20.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  const float height = textVisual.GetHeightForWidth( 40.f );
  DALI_TEST_EQUALS( height, 40.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Test that NPatchVisual is replaced with debug visual
  // TEST_NPATCH_FILE_NAME: image_01.9.jpg
  Property::Map propertyMap8;
  propertyMap8.Insert( Visual::Property::TYPE, DevelVisual::N_PATCH );
  propertyMap8.Insert( ImageVisual::Property::URL,  TEST_NPATCH_FILE_NAME );
  Visual::Base nPatchVisual = factory.CreateVisual( propertyMap8 );
  DALI_TEST_CHECK( nPatchVisual );
  TestDebugVisual( nPatchVisual, DevelVisual::N_PATCH, Vector2::ZERO );

  EnvironmentVariable::SetTestingEnvironmentVariable(false);
  END_TEST;
}

int UtcDaliDebugRenderingGetVisual2(void)
{
  EnvironmentVariable::SetTestingEnvironmentVariable(true);
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliDebugRenderingGetVisual2: Request visual with various parameters" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  // Test that color visual is replaced with debug visual
  Dali::Property::Map map;
  map[ Visual::Property::TYPE ] = Visual::COLOR;
  map[ ColorVisual::Property::MIX_COLOR ] = Color::CYAN;

  Visual::Base colorVisual = factory.CreateVisual( map);
  DALI_TEST_CHECK( colorVisual );
  TestDebugVisual( colorVisual, DevelVisual::COLOR, Vector2::ZERO );

  // Test that border visual is replaced with debug visual
  map.Clear();
  map[ Visual::Property::TYPE ] = Visual::BORDER;
  map[ BorderVisual::Property::COLOR  ] = Color::GREEN;
  map[ BorderVisual::Property::SIZE   ] = 2.f;
  Visual::Base borderVisual = factory.CreateVisual( map );
  DALI_TEST_CHECK( borderVisual );
  TestDebugVisual( borderVisual, DevelVisual::BORDER, Vector2::ZERO );

  // Test that image visual is replaced with debug visual
  Image image = ResourceImage::New(TEST_IMAGE_FILE_NAME);
  Visual::Base imageVisual = factory.CreateVisual( image );
  DALI_TEST_CHECK( imageVisual );
  TestDebugVisual( imageVisual, DevelVisual::IMAGE, Vector2::ZERO);

  // Test that n patch visual is replaced with debug visual
  Visual::Base nPatchVisual = factory.CreateVisual( TEST_NPATCH_FILE_NAME, ImageDimensions() );
  DALI_TEST_CHECK( nPatchVisual );
  TestDebugVisual( nPatchVisual, DevelVisual::N_PATCH, Vector2::ZERO );

  EnvironmentVariable::SetTestingEnvironmentVariable(false);
  END_TEST;
}
