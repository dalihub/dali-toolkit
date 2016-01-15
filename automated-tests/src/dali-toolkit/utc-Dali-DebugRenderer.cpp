/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>
#include <dali/devel-api/rendering/renderer.h>
#include <dali/devel-api/rendering/geometry.h>

#include <toolkit-environment-variable.h> // for setting environment variable: DALI_DEBUG_RENDERING

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* TEST_IMAGE_FILE_NAME =  "image_01.jpg";
const char* TEST_NPATCH_FILE_NAME =  "image_01.9.jpg";

bool IsDebugRenderer( ControlRenderer& renderer )
{
  bool isDebugRendererType = false;
  bool isGeometryLineType = false;

  Property::Map propertyMap;
  renderer.CreatePropertyMap( propertyMap );
  Property::Value* typeValue = propertyMap.Find( "rendererType",  Property::STRING );
  if ( typeValue )
  {
    isDebugRendererType = ( typeValue->Get<std::string>() == "debugRenderer" );
  }

  Actor actor = Actor::New();
  renderer.SetOnStage( actor );
  Geometry geometry = actor.GetRendererAt( 0 ).GetGeometry();
  isGeometryLineType = ( geometry.GetGeometryType() == Geometry::LINES );

  return isDebugRendererType && isGeometryLineType;
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

int UtcDaliDebugRendererGetRenderer1(void)
{
  EnvironmentVariable::SetTestingEnvironmentVariable(true);
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliDebugRendererGetRenderer1:  Request renderer with a Property::Map" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  // Test that color renderer is replaced with debug renderer
  Property::Map propertyMap1;
  propertyMap1.Insert("rendererType",  "colorRenderer");
  propertyMap1.Insert("blendColor",  Color::BLUE);
  ControlRenderer colorRenderer = factory.GetControlRenderer(propertyMap1);
  DALI_TEST_CHECK( colorRenderer );
  DALI_TEST_CHECK( IsDebugRenderer( colorRenderer ) );

  // Test that border renderer is replaced with debug renderer
  Property::Map propertyMap2;
  propertyMap2.Insert("rendererType",  "borderRenderer");
  propertyMap2.Insert("borderColor",  Color::BLUE);
  propertyMap2.Insert("borderSize",  2.f);
  ControlRenderer borderRenderer = factory.GetControlRenderer(propertyMap2);
  DALI_TEST_CHECK( borderRenderer );
  DALI_TEST_CHECK( IsDebugRenderer( borderRenderer ) );

  // Test that gradient renderer is replaced with debug renderer
  Property::Map propertyMap3;
  propertyMap3.Insert("rendererType",  "gradientRenderer");
  Vector2 start(-1.f, -1.f);
  Vector2 end(1.f, 1.f);
  propertyMap3.Insert("gradientStartPosition",   start);
  propertyMap3.Insert("gradientEndPosition",   end);
  propertyMap3.Insert("gradientSpreadMethod",   "repeat");
  Property::Array stopOffsets;
  stopOffsets.PushBack( 0.2f );
  stopOffsets.PushBack( 0.8f );
  propertyMap3.Insert("gradientStopOffset",   stopOffsets);
  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap3.Insert("gradientStopColor",   stopColors);
  ControlRenderer gradientRenderer = factory.GetControlRenderer(propertyMap3);
  DALI_TEST_CHECK( gradientRenderer );
  DALI_TEST_CHECK( IsDebugRenderer( gradientRenderer ) );

  // Test that image renderer is replaced with debug renderer
  Property::Map propertyMap4;
  propertyMap4.Insert( "rendererType",  "imageRenderer" );
  propertyMap4.Insert( "imageUrl",  TEST_IMAGE_FILE_NAME );
  ControlRenderer imageRenderer = factory.GetControlRenderer( propertyMap4 );
  DALI_TEST_CHECK( imageRenderer );
  DALI_TEST_CHECK( IsDebugRenderer( imageRenderer ) );

  // Test that n patch renderer is replaced with debug renderer
  Property::Map propertyMap5;
  propertyMap5.Insert( "rendererType",  "nPatchRenderer" );
  propertyMap5.Insert( "imageUrl",  TEST_NPATCH_FILE_NAME );
  ControlRenderer nPatchRenderer = factory.GetControlRenderer( propertyMap4 );
  DALI_TEST_CHECK( nPatchRenderer );
  DALI_TEST_CHECK( IsDebugRenderer( nPatchRenderer ) );

  EnvironmentVariable::SetTestingEnvironmentVariable(false);
  END_TEST;
}

int UtcDaliDebugRendererGetRenderer2(void)
{
  EnvironmentVariable::SetTestingEnvironmentVariable(true);
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliDebugRendererGetRenderer2: Request renderer with various parameters" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  // Test that color renderer is replaced with debug renderer
  ControlRenderer colorRenderer = factory.GetControlRenderer(Color::CYAN);
  DALI_TEST_CHECK( colorRenderer );
  DALI_TEST_CHECK( IsDebugRenderer( colorRenderer ) );

  // Test that border renderer is replaced with debug renderer
  ControlRenderer borderRenderer = factory.GetControlRenderer(2.f, Color::GREEN);
  DALI_TEST_CHECK( borderRenderer );
  DALI_TEST_CHECK( IsDebugRenderer( borderRenderer ) );

  // Test that image renderer is replaced with debug renderer
  Image image = ResourceImage::New(TEST_IMAGE_FILE_NAME);
  ControlRenderer imageRenderer = factory.GetControlRenderer( image );
  DALI_TEST_CHECK( imageRenderer );
  DALI_TEST_CHECK( IsDebugRenderer( imageRenderer ) );

  // Test that n patch renderer is replaced with debug renderer
  ControlRenderer nPatchRenderer = factory.GetControlRenderer( TEST_NPATCH_FILE_NAME );
  DALI_TEST_CHECK( nPatchRenderer );
  DALI_TEST_CHECK( IsDebugRenderer( nPatchRenderer ) );

  EnvironmentVariable::SetTestingEnvironmentVariable(false);
  END_TEST;
}

int UtcDaliDebugRendererResetRenderer(void)
{
  EnvironmentVariable::SetTestingEnvironmentVariable(true);
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliDebugRendererResetRenderer: Reset renderer with various parameters" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  // Test that color renderer is replaced with debug renderer
  ControlRenderer controlRenderer = factory.GetControlRenderer(Color::CYAN);
  DALI_TEST_CHECK( controlRenderer );
  DALI_TEST_CHECK( IsDebugRenderer( controlRenderer ) );

  Actor actor;
  // Reset to render another color
  // Test that color renderer is replaced with debug renderer
  factory.ResetRenderer( controlRenderer, actor, Color::BLUE );
  DALI_TEST_CHECK( IsDebugRenderer( controlRenderer ) );

  // Reset to render an image
  // Test that image renderer is replaced with debug renderer
  Image image = ResourceImage::New(TEST_IMAGE_FILE_NAME);
  factory.ResetRenderer( controlRenderer, actor, image );
  DALI_TEST_CHECK( IsDebugRenderer( controlRenderer ) );

  // Reset with a property map of border renderer
  // Test that border renderer is replaced with debug renderer
  Property::Map propertyMap;
  propertyMap.Insert("rendererType",  "borderRenderer");
  propertyMap.Insert("borderColor",  Color::BLUE);
  propertyMap.Insert("borderSize",  2.f);
  factory.ResetRenderer( controlRenderer, actor, propertyMap );
  DALI_TEST_CHECK( IsDebugRenderer( controlRenderer ) );

  EnvironmentVariable::SetTestingEnvironmentVariable(false);
  END_TEST;
}
