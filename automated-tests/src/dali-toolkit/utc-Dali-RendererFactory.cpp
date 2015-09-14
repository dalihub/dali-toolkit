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
 */

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali/devel-api/rendering/renderer.h>
#include <dali/devel-api/rendering/material.h>
#include <dali/devel-api/rendering/shader.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* TEST_IMAGE_FILE_NAME =  "gallery_image_01.jpg";
} // namespace


void dali_renderer_factory_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_renderer_factory_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliRendererFactoryGet(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactory" );

  //Register type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "RendererFactory" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  RendererFactory factory;
  factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  RendererFactory newFactory = RendererFactory::Get();
  DALI_TEST_CHECK( newFactory );

  // Check that renderer factory is a singleton
  DALI_TEST_CHECK(factory == newFactory);

  END_TEST;
}

int UtcDaliRendererFactoryCopyAndAssignment(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryCopyAndAssignment" );
  RendererFactory factory = RendererFactory::Get();

  RendererFactory factoryCopy( factory );
  DALI_TEST_CHECK(factory == factoryCopy);

  RendererFactory emptyFactory;
  RendererFactory emptyFactoryCopy( emptyFactory );
  DALI_TEST_CHECK(emptyFactory == emptyFactoryCopy);

  RendererFactory factoryEquals;
  factoryEquals = factory;
  DALI_TEST_CHECK(factory == factoryEquals);

  RendererFactory emptyFactoryEquals;
  emptyFactoryEquals = emptyFactory;
  DALI_TEST_CHECK( emptyFactory == emptyFactoryEquals );

  //self assignment
  factory = factory;
  DALI_TEST_CHECK( factory = factoryCopy );

  END_TEST;
}

int UtcDaliRendererFactoryGetColorRenderer(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryGetColorRenderer" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  Vector4 testColor( 1.f, 0.5f, 0.3f, 0.2f );
  propertyMap.Insert("renderer-type", "color-renderer");
  propertyMap.Insert("blend-color", testColor);

  ControlRenderer controlRenderer = factory.GetControlRenderer(propertyMap);
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  actor.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetSize(Vector2(200.f, 200.f));
  controlRenderer.SetOnStage( actor );

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uBlendColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, testColor, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererFactoryGetLinearGradientRenderer(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRendererFactoryGetRadialGradientRenderer");

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert("renderer-type", "gradient-renderer");

  Vector2 start(-1.f, -1.f);
  Vector2 end(1.f, 1.f);
  propertyMap.Insert("gradient-start-position", start);
  propertyMap.Insert("gradient-end-position", end);
  propertyMap.Insert("gradient-spread-method", "repeat");

  Property::Array stopOffsets;
  stopOffsets.PushBack( 0.2f );
  stopOffsets.PushBack( 0.8f );
  propertyMap.Insert("gradient-stop-offset", stopOffsets);

  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert("gradient-stop-color", stopColors);

  ControlRenderer controlRenderer = factory.GetControlRenderer(propertyMap);
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  Vector2 size(200.f, 200.f);
  actor.SetSize(size);
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetOnStage( actor );
  controlRenderer.SetSize(size);

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  // A lookup texture is generated and pass to shader as sampler
  DALI_TEST_CHECK( actor.GetRendererAt(0u).GetMaterial().GetNumberOfSamplers() == 1u );

  application.SendNotification();
  application.Render(0);

  END_TEST;
}

int UtcDaliRendererFactoryGetRadialGradientRenderer(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRendererFactoryGetRadialGradientRenderer");

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert("renderer-type", "gradient-renderer");

  Vector2 center(100.f, 100.f);
  float radius = 100.f;
  propertyMap.Insert("gradient-units", "user-space");
  propertyMap.Insert("gradient-center", center);
  propertyMap.Insert("gradient-radius", radius);

  Property::Array stopOffsets;
  stopOffsets.PushBack( 0.0f );
  stopOffsets.PushBack( 1.f );
  propertyMap.Insert("gradient-stop-offset", stopOffsets);

  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert("gradient-stop-color", stopColors);

  ControlRenderer controlRenderer = factory.GetControlRenderer(propertyMap);
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  Vector2 size(200.f, 200.f);
  actor.SetSize(size);
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetSize(size);
  controlRenderer.SetOnStage( actor );

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  // A lookup texture is generated and pass to shader as sampler
  DALI_TEST_CHECK( actor.GetRendererAt(0u).GetMaterial().GetNumberOfSamplers() == 1u );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  application.SendNotification();
  application.Render(0);

  Matrix3 alignMatrix( radius, 0.f, 0.f, 0.f, radius, 0.f, center.x, center.y, 1.f );
  alignMatrix.Invert();

  Matrix3 actualValue( Matrix3::IDENTITY );
  DALI_TEST_CHECK( gl.GetUniformValue<Matrix3>( "uAlignmentMatrix", actualValue ) );
  DALI_TEST_EQUALS( actualValue, alignMatrix, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererFactoryGetImageRenderer(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryGetImageRenderer" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert( "renderer-type", "image-renderer" );
  propertyMap.Insert( "image-url", TEST_IMAGE_FILE_NAME );

  ControlRenderer controlRenderer = factory.GetControlRenderer( propertyMap );
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  actor.SetSize( 200.f, 200.f );
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetSize( Vector2(200.f, 200.f) );
  controlRenderer.SetOnStage( actor );

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  DALI_TEST_CHECK( actor.GetRendererAt(0u).GetMaterial().GetNumberOfSamplers() == 1u );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  application.SendNotification();
  application.Render();

  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  if(request)
  {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD)));
  }

  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK(application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc));

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  int textureUnit = -1;
  DALI_TEST_CHECK( gl.GetUniformValue< int >( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  END_TEST;
}
