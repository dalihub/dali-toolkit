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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/geometry.h>

#include <toolkit-environment-variable.h> // for setting environment variable: DALI_DEBUG_RENDERING

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* TEST_IMAGE_FILE_NAME =  "image_01.jpg";
const char* TEST_NPATCH_FILE_NAME =  "image_01.9.jpg";

bool IsDebugVisual( Visual::Base& visual )
{
  bool isDebugVisualType = false;
  bool isGeometryLineType = false;

  Property::Map propertyMap;
  visual.CreatePropertyMap( propertyMap );
  Property::Value* typeValue = propertyMap.Find( "rendererType",  Property::STRING );
  if ( typeValue )
  {
    isDebugVisualType = ( typeValue->Get<std::string>() == "DEBUG" );
  }

  Actor actor = Actor::New();
  visual.SetOnStage( actor );
  Geometry geometry = actor.GetRendererAt( 0 ).GetGeometry();
  isGeometryLineType = ( geometry.GetGeometryType() == Geometry::LINES );

  return isDebugVisualType && isGeometryLineType;
}
}

void dali_debug_renderer_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_debug_renderer_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliDebugVisualGetRenderer1(void)
{
  EnvironmentVariable::SetTestingEnvironmentVariable(true);
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliDebugVisualGetRenderer1:  Request renderer with a Property::Map" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  // Test that color renderer is replaced with debug renderer
  Property::Map propertyMap1;
  propertyMap1.Insert("rendererType",  "COLOR");
  propertyMap1.Insert("mixColor",  Color::BLUE);
  Visual::Base colorVisual = factory.CreateVisual(propertyMap1);
  DALI_TEST_CHECK( colorVisual );
  DALI_TEST_CHECK( IsDebugVisual( colorVisual ) );

  // Test that border renderer is replaced with debug renderer
  Property::Map propertyMap2;
  propertyMap2.Insert("rendererType",  "BORDER");
  propertyMap2.Insert("borderColor",  Color::BLUE);
  propertyMap2.Insert("borderSize",  2.f);
  Visual::Base borderRenderer = factory.CreateVisual(propertyMap2);
  DALI_TEST_CHECK( borderRenderer );
  DALI_TEST_CHECK( IsDebugVisual( borderRenderer ) );

  // Test that gradient renderer is replaced with debug renderer
  Property::Map propertyMap3;
  propertyMap3.Insert("rendererType",  "GRADIENT");
  Vector2 start(-1.f, -1.f);
  Vector2 end(1.f, 1.f);
  propertyMap3.Insert("startPosition", start);
  propertyMap3.Insert("endPosition", end);
  propertyMap3.Insert("spreadMethod", "REPEAT");
  Property::Array stopOffsets;
  stopOffsets.PushBack( 0.2f );
  stopOffsets.PushBack( 0.8f );
  propertyMap3.Insert("stopOffset", stopOffsets);
  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap3.Insert("stopColor", stopColors);
  Visual::Base gradientRenderer = factory.CreateVisual(propertyMap3);
  DALI_TEST_CHECK( gradientRenderer );
  DALI_TEST_CHECK( IsDebugVisual( gradientRenderer ) );

  // Test that image renderer is replaced with debug renderer
  Property::Map propertyMap4;
  propertyMap4.Insert( "rendererType",  "IMAGE" );
  propertyMap4.Insert( "url",  TEST_IMAGE_FILE_NAME );
  Visual::Base imageRenderer = factory.CreateVisual( propertyMap4 );
  DALI_TEST_CHECK( imageRenderer );
  DALI_TEST_CHECK( IsDebugVisual( imageRenderer ) );

  // Test that n patch renderer is replaced with debug renderer
  Property::Map propertyMap5;
  propertyMap5.Insert( "rendererType",  "IMAGE" );
  propertyMap5.Insert( "url",  TEST_NPATCH_FILE_NAME );
  Visual::Base nPatchRenderer = factory.CreateVisual( propertyMap4 );
  DALI_TEST_CHECK( nPatchRenderer );
  DALI_TEST_CHECK( IsDebugVisual( nPatchRenderer ) );

  EnvironmentVariable::SetTestingEnvironmentVariable(false);
  END_TEST;
}

int UtcDaliDebugVisualGetRenderer2(void)
{
  EnvironmentVariable::SetTestingEnvironmentVariable(true);
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliDebugVisualGetRenderer2: Request renderer with various parameters" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  // Test that color renderer is replaced with debug renderer
  Dali::Property::Map map;
  map[ "rendererType" ] = "COLOR";
  map[ "mixColor" ] = Color::CYAN;

  Visual::Base colorVisual = factory.CreateVisual( map);
  DALI_TEST_CHECK( colorVisual );
  DALI_TEST_CHECK( IsDebugVisual( colorVisual ) );

  // Test that border renderer is replaced with debug renderer
  map.Clear();
  map[ "rendererType" ] = "BORDER";
  map[ "borderColor"  ] = Color::GREEN;
  map[ "borderSize"   ] = 2.f;
  Visual::Base borderRenderer = factory.CreateVisual( map );
  DALI_TEST_CHECK( borderRenderer );
  DALI_TEST_CHECK( IsDebugVisual( borderRenderer ) );

  // Test that image renderer is replaced with debug renderer
  Image image = ResourceImage::New(TEST_IMAGE_FILE_NAME);
  Visual::Base imageRenderer = factory.CreateVisual( image );
  DALI_TEST_CHECK( imageRenderer );
  DALI_TEST_CHECK( IsDebugVisual( imageRenderer ) );

  // Test that n patch renderer is replaced with debug renderer
  Visual::Base nPatchRenderer = factory.CreateVisual( TEST_NPATCH_FILE_NAME, ImageDimensions() );
  DALI_TEST_CHECK( nPatchRenderer );
  DALI_TEST_CHECK( IsDebugVisual( nPatchRenderer ) );

  EnvironmentVariable::SetTestingEnvironmentVariable(false);
  END_TEST;
}
