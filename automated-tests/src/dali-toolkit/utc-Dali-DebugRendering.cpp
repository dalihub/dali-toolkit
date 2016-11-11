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

#include <dali-toolkit/dali-toolkit.h>

#include <toolkit-environment-variable.h> // for setting environment variable: DALI_DEBUG_RENDERING
#include "dummy-control.h"

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
  Property::Value* typeValue = propertyMap.Find( Visual::Property::TYPE,  Property::INTEGER );
  if ( typeValue )
  {
    isDebugVisualType = ( typeValue->Get<int>() == Visual::WIREFRAME ); // Debug Rendering uses the WireframeVisual
  }

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );
  Stage::GetCurrent().Add( actor );

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1, TEST_LOCATION );
  if( actor.GetRendererCount() > 0 )
  {
    Geometry geometry = actor.GetRendererAt( 0 ).GetGeometry();
    isGeometryLineType = ( geometry.GetType() == Geometry::LINES );
  }

  return isDebugVisualType && isGeometryLineType;
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
  DALI_TEST_CHECK( IsDebugVisual( colorVisual ) );

  // Test that border visual is replaced with debug visual
  Property::Map propertyMap2;
  propertyMap2.Insert(Visual::Property::TYPE,  Visual::BORDER);
  propertyMap2.Insert(BorderVisual::Property::COLOR,  Color::BLUE);
  propertyMap2.Insert(BorderVisual::Property::SIZE,  2.f);
  Visual::Base borderVisual = factory.CreateVisual(propertyMap2);
  DALI_TEST_CHECK( borderVisual );
  DALI_TEST_CHECK( IsDebugVisual( borderVisual ) );

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
  DALI_TEST_CHECK( IsDebugVisual( gradientVisual ) );

  // Test that image visual is replaced with debug visual
  Property::Map propertyMap4;
  propertyMap4.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap4.Insert( ImageVisual::Property::URL,  TEST_IMAGE_FILE_NAME );
  Visual::Base imageVisual = factory.CreateVisual( propertyMap4 );
  DALI_TEST_CHECK( imageVisual );
  DALI_TEST_CHECK( IsDebugVisual( imageVisual ) );

  // Test that n patch visual is replaced with debug visual
  Property::Map propertyMap5;
  propertyMap5.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap5.Insert( ImageVisual::Property::URL,  TEST_NPATCH_FILE_NAME );
  Visual::Base nPatchVisual = factory.CreateVisual( propertyMap4 );
  DALI_TEST_CHECK( nPatchVisual );
  DALI_TEST_CHECK( IsDebugVisual( nPatchVisual ) );

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
  DALI_TEST_CHECK( IsDebugVisual( colorVisual ) );

  // Test that border visual is replaced with debug visual
  map.Clear();
  map[ Visual::Property::TYPE ] = Visual::BORDER;
  map[ BorderVisual::Property::COLOR  ] = Color::GREEN;
  map[ BorderVisual::Property::SIZE   ] = 2.f;
  Visual::Base borderVisual = factory.CreateVisual( map );
  DALI_TEST_CHECK( borderVisual );
  DALI_TEST_CHECK( IsDebugVisual( borderVisual ) );

  // Test that image visual is replaced with debug visual
  Image image = ResourceImage::New(TEST_IMAGE_FILE_NAME);
  Visual::Base imageVisual = factory.CreateVisual( image );
  DALI_TEST_CHECK( imageVisual );
  DALI_TEST_CHECK( IsDebugVisual( imageVisual ) );

  // Test that n patch visual is replaced with debug visual
  Visual::Base nPatchVisual = factory.CreateVisual( TEST_NPATCH_FILE_NAME, ImageDimensions() );
  DALI_TEST_CHECK( nPatchVisual );
  DALI_TEST_CHECK( IsDebugVisual( nPatchVisual ) );

  EnvironmentVariable::SetTestingEnvironmentVariable(false);
  END_TEST;
}
