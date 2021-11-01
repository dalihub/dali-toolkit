/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
 */

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/arc-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-actions-devel.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include "dummy-control.h"

using namespace Dali;
using namespace Dali::Toolkit;

void dali_arc_visual_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_arc_visual_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliVisualFactoryGetArcVisual01(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetArcVisual01: Request arc visual with a Property::Map" );

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE, DevelVisual::ARC )
             .Add( Visual::Property::MIX_COLOR, Color::RED )
             .Add( DevelArcVisual::Property::THICKNESS, 20.0f );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );
  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.0f, 200.0f ) );
  application.GetScene().Add( actor );

  application.SendNotification();
  application.Render();

  // renderer is added to actor
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  Renderer renderer = actor.GetRendererAt( 0u );
  DALI_TEST_CHECK( renderer );

  actor.Unparent( );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliVisualFactoryGetArcVisual02(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetArcVisual02: Request arc visual with a Property::Map" );

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE, DevelVisual::ARC )
             .Add( Visual::Property::MIX_COLOR, Color::RED )
             .Add( DevelArcVisual::Property::THICKNESS, 20.0f )
             .Add( DevelArcVisual::Property::START_ANGLE, 0.0f )
             .Add( DevelArcVisual::Property::SWEEP_ANGLE, 90.0f )
             .Add( DevelArcVisual::Property::CAP, DevelArcVisual::Cap::ROUND );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );
  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.0f, 200.0f ) );
  application.GetScene().Add( actor );

  application.SendNotification();
  application.Render();

  // renderer is added to actor
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  Renderer renderer = actor.GetRendererAt( 0u );
  DALI_TEST_CHECK( renderer );

  actor.Unparent( );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliArcVisualGetPropertyMap01(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliArcVisualGetPropertyMap01" );

  float thickness = 20.0f;
  float startAngle = 0.0f, sweepAngle = 90.0f;

  Property::Map propertyMap;
  propertyMap.Add( "visualType", DevelVisual::ARC )
             .Add( "mixColor", Color::RED )
             .Add( "thickness", thickness )
             .Add( "startAngle", startAngle )
             .Add( "sweepAngle", sweepAngle )
             .Add( "cap", DevelArcVisual::Cap::ROUND );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );
  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.0f, 200.0f ) );
  application.GetScene().Add( actor );

  application.SendNotification();
  application.Render();

  // renderer is added to actor
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  Renderer renderer = actor.GetRendererAt( 0u );
  DALI_TEST_CHECK( renderer );

  Property::Map resultMap;
  visual.CreatePropertyMap( resultMap );

  // check the property values from the returned map from a visual
  Property::Value* value = resultMap.Find( Visual::Property::MIX_COLOR, Property::VECTOR4 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector4 >(), Color::RED, TEST_LOCATION );

  value = resultMap.Find( DevelArcVisual::Property::THICKNESS, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< float >(), thickness, TEST_LOCATION );

  value = resultMap.Find( DevelArcVisual::Property::START_ANGLE, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< float >(), startAngle, TEST_LOCATION );

  value = resultMap.Find( DevelArcVisual::Property::SWEEP_ANGLE, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< float >(), sweepAngle, TEST_LOCATION );

  value = resultMap.Find( DevelArcVisual::Property::CAP, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< int >() == DevelArcVisual::Cap::ROUND );

  // Test wrong values
  propertyMap[DevelArcVisual::Property::THICKNESS] = "3.0f";
  propertyMap[DevelArcVisual::Property::START_ANGLE] = "0.0f";
  propertyMap[DevelArcVisual::Property::SWEEP_ANGLE] = "90.0f";
  propertyMap[DevelArcVisual::Property::CAP] = "1";

  visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  visual.CreatePropertyMap( resultMap );

  value = resultMap.Find( DevelArcVisual::Property::THICKNESS, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< float >(), 0.0f, TEST_LOCATION );

  value = resultMap.Find( DevelArcVisual::Property::START_ANGLE, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< float >(), 0.0f, TEST_LOCATION );

  value = resultMap.Find( DevelArcVisual::Property::SWEEP_ANGLE, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< float >(), 360.0f, TEST_LOCATION );

  value = resultMap.Find( DevelArcVisual::Property::CAP, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< int >() == DevelArcVisual::Cap::BUTT );

  actor.Unparent( );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliArcVisualGetPropertyMap02(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliArcVisualGetPropertyMap02" );

  float thickness = 20.0f;
  float startAngle = 0.0f, sweepAngle = 90.0f;

  Property::Map propertyMap;
  propertyMap.Add(Visual::Property::TYPE, DevelVisual::ARC)
             .Add(Visual::Property::MIX_COLOR, Color::RED)
             .Add(DevelArcVisual::Property::THICKNESS, thickness )
             .Add(DevelArcVisual::Property::START_ANGLE, startAngle )
             .Add(DevelArcVisual::Property::SWEEP_ANGLE, sweepAngle )
             .Add(DevelArcVisual::Property::CAP, DevelArcVisual::Cap::ROUND );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  Property::Map resultMap;
  visual.CreatePropertyMap( resultMap );

  // check the property values from the returned map from a visual
  Property::Value* value = resultMap.Find( Visual::Property::MIX_COLOR, Property::VECTOR4 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector4 >(), Color::RED, TEST_LOCATION );

  value = resultMap.Find( DevelArcVisual::Property::THICKNESS, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< float >(), thickness, TEST_LOCATION );

  value = resultMap.Find( DevelArcVisual::Property::START_ANGLE, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< float >(), startAngle, TEST_LOCATION );

  value = resultMap.Find( DevelArcVisual::Property::SWEEP_ANGLE, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< float >(), sweepAngle, TEST_LOCATION );

  value = resultMap.Find( DevelArcVisual::Property::CAP, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< int >() == DevelArcVisual::Cap::ROUND );

  END_TEST;
}

int UtcDaliArcVisualUpdateProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliArcVisualUpdateProperty" );

  static std::vector<UniformData> customUniforms =
  {
    UniformData("thickness", Property::Type::FLOAT),
    UniformData("startAngle", Property::Type::FLOAT),
    UniformData("sweepAngle", Property::Type::FLOAT),
  };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  float thickness = 20.0f;
  float startAngle = 0.0f;
  float sweepAngle = 90.0f;

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE, DevelVisual::ARC )
             .Add( Visual::Property::MIX_COLOR, Color::RED )
             .Add( DevelArcVisual::Property::THICKNESS, thickness )
             .Add( DevelArcVisual::Property::START_ANGLE, startAngle )
             .Add( DevelArcVisual::Property::SWEEP_ANGLE, sweepAngle );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.0f, 200.0f ) );
  application.GetScene().Add( actor );

  application.SendNotification();
  application.Render();

  Property::Map map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  Property::Value* value = map.Find( DevelArcVisual::Property::THICKNESS, Property::FLOAT );
  DALI_TEST_EQUALS( value->Get< float >(), thickness, TEST_LOCATION );
  DALI_TEST_EQUALS( gl.CheckUniformValue< float >( "thickness", thickness ), true, TEST_LOCATION );

  value = map.Find( DevelArcVisual::Property::START_ANGLE, Property::FLOAT );
  DALI_TEST_EQUALS( value->Get< float >(), startAngle, TEST_LOCATION );
  DALI_TEST_EQUALS( gl.CheckUniformValue< float >( "startAngle", startAngle ), true, TEST_LOCATION );

  value = map.Find( DevelArcVisual::Property::SWEEP_ANGLE, Property::FLOAT );
  DALI_TEST_EQUALS( value->Get< float >(), sweepAngle, TEST_LOCATION );
  DALI_TEST_EQUALS( gl.CheckUniformValue< float >( "sweepAngle", sweepAngle ), true, TEST_LOCATION );

  thickness = 10.0f;
  startAngle = 90.0f;
  sweepAngle = 180.0f;

  Property::Map attributes;
  attributes.Add( DevelArcVisual::Property::THICKNESS, thickness )
            .Add( DevelArcVisual::Property::START_ANGLE, startAngle )
            .Add( DevelArcVisual::Property::SWEEP_ANGLE, sweepAngle );

  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelVisual::Action::UPDATE_PROPERTY, attributes );

  application.SendNotification();
  application.Render();

  map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  value = map.Find( DevelArcVisual::Property::THICKNESS, Property::FLOAT );
  DALI_TEST_EQUALS( value->Get< float >(), thickness, TEST_LOCATION );
  DALI_TEST_EQUALS( gl.CheckUniformValue< float >( "thickness", thickness ), true, TEST_LOCATION );

  value = map.Find( DevelArcVisual::Property::START_ANGLE, Property::FLOAT );
  DALI_TEST_EQUALS( value->Get< float >(), startAngle, TEST_LOCATION );
  DALI_TEST_EQUALS( gl.CheckUniformValue< float >( "startAngle", startAngle ), true, TEST_LOCATION );

  value = map.Find( DevelArcVisual::Property::SWEEP_ANGLE, Property::FLOAT );
  DALI_TEST_EQUALS( value->Get< float >(), sweepAngle, TEST_LOCATION );
  DALI_TEST_EQUALS( gl.CheckUniformValue< float >( "sweepAngle", sweepAngle ), true, TEST_LOCATION );

  END_TEST;
}
