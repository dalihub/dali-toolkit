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
const char* TEST_NPATCH_FILE_NAME =  "gallery_image_01.9.jpg";
}

void dali_control_renderer_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_control_renderer_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliControlRendererCopyAndAssignment(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliControlRendererCopyAndAssignment" );

  RendererFactory factory = RendererFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert("renderer-type", "color-renderer");
  propertyMap.Insert("blend-color", Color::BLUE);
  ControlRenderer controlRenderer = factory.GetControlRenderer( propertyMap );

  ControlRenderer controlRendererCopy( controlRenderer );
  DALI_TEST_CHECK(controlRenderer == controlRendererCopy);

  ControlRenderer emptyControlRenderer;
  ControlRenderer emptyControlRendererCopy( emptyControlRenderer );
  DALI_TEST_CHECK(emptyControlRenderer == emptyControlRendererCopy);

  ControlRenderer controlRendererEquals;
  controlRendererEquals = controlRenderer;
  DALI_TEST_CHECK(controlRenderer == controlRendererEquals);

  ControlRenderer emptyControlRendererEquals;
  emptyControlRendererEquals = emptyControlRenderer;
  DALI_TEST_CHECK( emptyControlRenderer == emptyControlRendererEquals );

  //self assignment
  controlRenderer = controlRenderer;
  DALI_TEST_CHECK( controlRenderer = controlRendererCopy );

  END_TEST;
}

int UtcDaliControlRendererSetDepthIndex(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliControlRendererSetDepthIndex" );

  RendererFactory factory = RendererFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert("renderer-type", "color-renderer");
  propertyMap.Insert("blend-color", Color::BLUE);
  ControlRenderer controlRenderer = factory.GetControlRenderer( propertyMap );

  controlRenderer.SetDepthIndex( 1.f );

  Actor actor = Actor::New();
  actor.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetOnStage( actor );

  DALI_TEST_EQUALS( actor.GetRendererAt(0u).GetDepthIndex(), 1.f, TEST_LOCATION );

  controlRenderer.SetDepthIndex( -1.f );
  DALI_TEST_EQUALS( actor.GetRendererAt(0u).GetDepthIndex(), -1.f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlRendererSetOnOffStage(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliControlRendererSetDepthIndex" );

  RendererFactory factory = RendererFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert("renderer-type", "color-renderer");
  propertyMap.Insert("blend-color", Color::BLUE);
  ControlRenderer controlRenderer = factory.GetControlRenderer( propertyMap );

  Actor actor = Actor::New();
  actor.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  controlRenderer.SetOnStage( actor );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  controlRenderer.SetOffStage( actor );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliControlRendererGetPropertyMap1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliControlRendererGetPropertyMap1: ColorRenderer" );

  RendererFactory factory = RendererFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert("renderer-type", "color-renderer");
  propertyMap.Insert("blend-color", Color::BLUE);
  ControlRenderer colorRenderer = factory.GetControlRenderer( propertyMap );

  Property::Map resultMap;
  colorRenderer.CreatePropertyMap( resultMap );

  Property::Value* typeValue = resultMap.Find( "renderer-type", Property::STRING );
  DALI_TEST_CHECK( typeValue );
  DALI_TEST_CHECK( typeValue->Get<std::string>() == "color-renderer" );

  Property::Value* colorValue = resultMap.Find( "blend-color", Property::VECTOR4 );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get<Vector4>() == Color::BLUE );

  // change the blend color
  factory.ResetRenderer( colorRenderer, Color::CYAN );
  colorRenderer.CreatePropertyMap( resultMap );

  colorValue = resultMap.Find( "blend-color", Property::VECTOR4 );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get<Vector4>() == Color::CYAN );

  END_TEST;
}

int UtcDaliControlRendererGetPropertyMap2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliControlRendererGetPropertyMap2: BorderRenderer" );

  RendererFactory factory = RendererFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert("renderer-type", "border-renderer");
  propertyMap.Insert("border-color", Color::BLUE);
  propertyMap.Insert("border-size", 5.f);
  ControlRenderer borderRenderer = factory.GetControlRenderer( propertyMap );

  Property::Map resultMap;
  borderRenderer.CreatePropertyMap( resultMap );

  // check the property values from the returned map from control renderer
  Property::Value* typeValue = resultMap.Find( "renderer-type", Property::STRING );
  DALI_TEST_CHECK( typeValue );
  DALI_TEST_CHECK( typeValue->Get<std::string>() == "border-renderer" );

  Property::Value* colorValue = resultMap.Find( "border-color", Property::VECTOR4 );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get<Vector4>() == Color::BLUE );

  Property::Value* sizeValue = resultMap.Find( "border-size", Property::FLOAT );
  DALI_TEST_CHECK( sizeValue );
  DALI_TEST_CHECK( sizeValue->Get<float>() == 5.f );

  borderRenderer = factory.GetControlRenderer( 10.f, Color::CYAN );
  borderRenderer.CreatePropertyMap( resultMap );

  typeValue = resultMap.Find( "renderer-type", Property::STRING );
  DALI_TEST_CHECK( typeValue );
  DALI_TEST_CHECK( typeValue->Get<std::string>() == "border-renderer" );

   colorValue = resultMap.Find( "border-color", Property::VECTOR4 );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get<Vector4>() == Color::CYAN );

  colorValue = resultMap.Find( "border-size", Property::FLOAT );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get<float>() == 10.f );

  END_TEST;
}

int UtcDaliControlRendererGetPropertyMap3(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliControlRendererGetPropertyMap3: linear GradientRenderer" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert("renderer-type", "gradient-renderer");

  Vector2 start(-1.f, -1.f);
  Vector2 end(1.f, 1.f);
  propertyMap.Insert("gradient-start-position", start);
  propertyMap.Insert("gradient-end-position", end);
  propertyMap.Insert("gradient-spread-method", "repeat");

  propertyMap.Insert("gradient-stop-offset", Vector2(0.2f, 0.8f));

  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert("gradient-stop-color", stopColors);

  ControlRenderer gradientRenderer = factory.GetControlRenderer(propertyMap);

  Property::Map resultMap;
  gradientRenderer.CreatePropertyMap( resultMap );

  // check the property values from the returned map from control renderer
  Property::Value* value = resultMap.Find( "renderer-type", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "gradient-renderer" );

  value = resultMap.Find( "gradient-units", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "object-bounding-box" );

  value = resultMap.Find( "gradient-spread-method", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "repeat" );

  value = resultMap.Find( "gradient-start-position", Property::VECTOR2 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector2>(), start , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "gradient-end-position", Property::VECTOR2 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector2>(), end , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "gradient-stop-offset", Property::ARRAY );
  DALI_TEST_CHECK( value );
  Property::Array* offsetArray = value->GetArray();
  DALI_TEST_CHECK( offsetArray->Count() == 2 );
  DALI_TEST_EQUALS( offsetArray->GetElementAt(0).Get<float>(), 0.2f , Math::MACHINE_EPSILON_100, TEST_LOCATION );
  DALI_TEST_EQUALS( offsetArray->GetElementAt(1).Get<float>(), 0.8f , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "gradient-stop-color", Property::ARRAY );
  DALI_TEST_CHECK( value );
  Property::Array* colorArray = value->GetArray();
  DALI_TEST_CHECK( colorArray->Count() == 2 );
  DALI_TEST_EQUALS( colorArray->GetElementAt(0).Get<Vector4>(), Color::RED , Math::MACHINE_EPSILON_100, TEST_LOCATION );
  DALI_TEST_EQUALS( colorArray->GetElementAt(1).Get<Vector4>(), Color::GREEN , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlRendererGetPropertyMap4(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliControlRendererGetPropertyMap4: radial GradientRenderer" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert("renderer-type", "gradient-renderer");

  Vector2 center(100.f, 100.f);
  float radius = 100.f;
  propertyMap.Insert("gradient-units", "user-space");
  propertyMap.Insert("gradient-center", center);
  propertyMap.Insert("gradient-radius", radius);
  propertyMap.Insert("gradient-stop-offset", Vector3(0.1f, 0.3f, 1.1f));

  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::BLACK );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert("gradient-stop-color", stopColors);

  ControlRenderer gradientRenderer = factory.GetControlRenderer(propertyMap);
  DALI_TEST_CHECK( gradientRenderer );

  Property::Map resultMap;
  gradientRenderer.CreatePropertyMap( resultMap );

  // check the property values from the returned map from control renderer
  Property::Value* value = resultMap.Find( "renderer-type", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "gradient-renderer" );

  value = resultMap.Find( "gradient-units", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "user-space" );

  value = resultMap.Find( "gradient-spread-method", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "pad" );

  value = resultMap.Find( "gradient-center", Property::VECTOR2 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector2>(), center , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "gradient-radius", Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), radius , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "gradient-stop-offset", Property::ARRAY );
  DALI_TEST_CHECK( value );
  Property::Array* offsetArray = value->GetArray();
  DALI_TEST_CHECK( offsetArray->Count() == 3 );
  DALI_TEST_EQUALS( offsetArray->GetElementAt(0).Get<float>(), 0.1f , Math::MACHINE_EPSILON_100, TEST_LOCATION );
  DALI_TEST_EQUALS( offsetArray->GetElementAt(1).Get<float>(), 0.3f , Math::MACHINE_EPSILON_100, TEST_LOCATION );
  // any stop value will be clamped to [0.0, 1.0];
  DALI_TEST_EQUALS( offsetArray->GetElementAt(2).Get<float>(), 1.0f , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "gradient-stop-color", Property::ARRAY );
  DALI_TEST_CHECK( value );
  Property::Array* colorArray = value->GetArray();
  DALI_TEST_CHECK( colorArray->Count() == 3 );
  DALI_TEST_EQUALS( colorArray->GetElementAt(0).Get<Vector4>(), Color::RED , Math::MACHINE_EPSILON_100, TEST_LOCATION );
  DALI_TEST_EQUALS( colorArray->GetElementAt(1).Get<Vector4>(), Color::BLACK , Math::MACHINE_EPSILON_100, TEST_LOCATION );
  DALI_TEST_EQUALS( colorArray->GetElementAt(2).Get<Vector4>(), Color::GREEN , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlRendererGetPropertyMap5(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliControlRendererGetPropertyMap5: ImageRenderer" );

  RendererFactory factory = RendererFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert( "renderer-type", "image-renderer" );
  propertyMap.Insert( "image-url", TEST_IMAGE_FILE_NAME );
  propertyMap.Insert( "image-desired-width", 20 );
  propertyMap.Insert( "image-desired-height", 30 );
  propertyMap.Insert( "image-fitting-mode", "fit-height" );
  propertyMap.Insert( "image-sampling-mode", "box-then-nearest" );

  ControlRenderer imageRenderer = factory.GetControlRenderer(propertyMap);
  DALI_TEST_CHECK( imageRenderer );

  Property::Map resultMap;
  imageRenderer.CreatePropertyMap( resultMap );

  // check the property values from the returned map from control renderer
  Property::Value* value = resultMap.Find( "renderer-type", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "image-renderer" );

  value = resultMap.Find( "image-url", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_IMAGE_FILE_NAME );

  value = resultMap.Find( "image-fitting-mode", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "fit-height" );

  value = resultMap.Find( "image-sampling-mode", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "box-then-nearest" );

  value = resultMap.Find( "image-desired-width", Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 20 );

  value = resultMap.Find( "image-desired-height", Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 30 );

  // Get an image renderer with an image handle, and test the default property values
  Image image = ResourceImage::New(TEST_IMAGE_FILE_NAME, ImageDimensions(100, 200));
  imageRenderer = factory.GetControlRenderer(image);
  imageRenderer.CreatePropertyMap( resultMap );

  value = resultMap.Find( "renderer-type", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "image-renderer" );

  value = resultMap.Find( "image-url", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_IMAGE_FILE_NAME );

  value = resultMap.Find( "image-fitting-mode", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "shrink-to-fit" );

  value = resultMap.Find( "image-sampling-mode", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "box" );

  value = resultMap.Find( "image-desired-width", Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 100 );

  value = resultMap.Find( "image-desired-height", Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 200 );

  END_TEST;
}

int UtcDaliControlRendererGetPropertyMap6(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliControlRendererGetPropertyMap6: NPatchRenderer" );

  RendererFactory factory = RendererFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert( "renderer-type", "n-patch-renderer" );
  propertyMap.Insert( "image-url", TEST_NPATCH_FILE_NAME );
  propertyMap.Insert( "border-only", true );
  ControlRenderer nPatchRenderer = factory.GetControlRenderer( propertyMap );

  Property::Map resultMap;
  nPatchRenderer.CreatePropertyMap( resultMap );

  // check the property values from the returned map from control renderer
  Property::Value* value = resultMap.Find( "renderer-type", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "n-patch-renderer" );

  value = resultMap.Find( "image-url", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_NPATCH_FILE_NAME );

  value = resultMap.Find( "border-only", Property::BOOLEAN );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<bool>() );

  END_TEST;
}
