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
 */

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/rendering/shader.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* TEST_IMAGE_FILE_NAME =  "gallery_image_01.jpg";
const char* TEST_NPATCH_FILE_NAME =  "gallery_image_01.9.jpg";
const char* TEST_SVG_FILE_NAME = TEST_RESOURCE_DIR "/svg1.svg";
const char* TEST_OBJ_FILE_NAME = TEST_RESOURCE_DIR "/Cube.obj";
const char* TEST_MTL_FILE_NAME = TEST_RESOURCE_DIR "/ToyRobot-Metal.mtl";
const char* TEST_RESOURCE_LOCATION = TEST_RESOURCE_DIR "/";
}

void dali_visual_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_visual_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliVisualCopyAndAssignment(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualCopyAndAssignment" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert("rendererType",  "COLOR");
  propertyMap.Insert("mixColor",  Color::BLUE);
  Visual visual = factory.CreateVisual( propertyMap );

  Visual visualCopy( visual );
  DALI_TEST_CHECK(visual == visualCopy);

  Visual emptyVisual;
  Visual emptyVisualCopy( emptyVisual );
  DALI_TEST_CHECK(emptyVisual == emptyVisualCopy);

  Visual visualEquals;
  visualEquals = visual;
  DALI_TEST_CHECK(visual == visualEquals);

  Visual emptyVisualEquals;
  emptyVisualEquals = emptyVisual;
  DALI_TEST_CHECK( emptyVisual == emptyVisualEquals );

  //self assignment
  visual = visual;
  DALI_TEST_CHECK( visual = visualCopy );

  END_TEST;
}

int UtcDaliVisualSetGetDepthIndex(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualSetDepthIndex" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert("rendererType",  "COLOR");
  propertyMap.Insert("mixColor",  Color::BLUE);
  Visual visual = factory.CreateVisual( propertyMap );

  visual.SetDepthIndex( 1.f );

  Actor actor = Actor::New();
  actor.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor );
  visual.SetOnStage( actor );

  int depthIndex = actor.GetRendererAt(0u).GetProperty<int>( Renderer::Property::DEPTH_INDEX );
  DALI_TEST_EQUALS( depthIndex, 1, TEST_LOCATION );
  DALI_TEST_EQUALS( visual.GetDepthIndex(), 1.f, TEST_LOCATION );

  visual.SetDepthIndex( -1.f );
  depthIndex = actor.GetRendererAt(0u).GetProperty<int>( Renderer::Property::DEPTH_INDEX );
  DALI_TEST_EQUALS( depthIndex, -1, TEST_LOCATION );
  DALI_TEST_EQUALS( visual.GetDepthIndex(), -1.f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualSize(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetNaturalSize" );

  VisualFactory factory = VisualFactory::Get();
  Vector2 visualSize( 20.f, 30.f );
  Vector2 naturalSize;

  // color colorVisual 
  Dali::Property::Map map;
  map[ "rendererType" ] = "COLOR";
  map[ "mixColor" ] = Color::MAGENTA;
  Visual colorVisual = factory.CreateVisual( map );
  colorVisual.SetSize( visualSize );
  DALI_TEST_EQUALS( colorVisual.GetSize(), visualSize, TEST_LOCATION );
  colorVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS( naturalSize, Vector2::ZERO, TEST_LOCATION );

  // image visual
  Image image = ResourceImage::New(TEST_IMAGE_FILE_NAME, ImageDimensions(100, 200));
  Visual imageVisual = factory.CreateVisual( image );
  imageVisual.SetSize( visualSize );
  DALI_TEST_EQUALS( imageVisual.GetSize(), visualSize, TEST_LOCATION );
  imageVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS( naturalSize, Vector2(100.f, 200.f), TEST_LOCATION );

  // n patch visual
  TestPlatformAbstraction& platform = application.GetPlatform();
  Vector2 testSize(80.f, 160.f);
  platform.SetClosestImageSize(testSize);
  image = ResourceImage::New(TEST_NPATCH_FILE_NAME);
  Visual nPatchVisual = factory.CreateVisual( image );
  nPatchVisual.SetSize( visualSize );
  DALI_TEST_EQUALS( nPatchVisual.GetSize(), visualSize, TEST_LOCATION );
  nPatchVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS( naturalSize, testSize, TEST_LOCATION );

  // border visual
  float borderSize = 5.f;
  map.Clear();
  map[ "rendererType" ] = "BORDER";
  map[ "borderColor"  ] = Color::RED;
  map[ "borderSize"   ] = borderSize;
  Visual borderVisual = factory.CreateVisual( map );
  borderVisual.SetSize( visualSize );
  DALI_TEST_EQUALS( borderVisual.GetSize(), visualSize, TEST_LOCATION );
  borderVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS( naturalSize, Vector2::ZERO, TEST_LOCATION );

  // gradient gradientVisual
  Property::Map propertyMap;
  propertyMap.Insert("rendererType",  "GRADIENT");
  Vector2 start(-1.f, -1.f);
  Vector2 end(1.f, 1.f);
  propertyMap.Insert("startPosition",   start);
  propertyMap.Insert("endPosition",   end);
  propertyMap.Insert("stopOffset",   Vector2(0.f, 1.f));
  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert("stopColor",   stopColors);
  Visual gradientVisual = factory.CreateVisual(propertyMap);
  gradientVisual.SetSize( visualSize );
  DALI_TEST_EQUALS( gradientVisual.GetSize(), visualSize, TEST_LOCATION );
  gradientVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS( naturalSize, Vector2::ZERO,TEST_LOCATION );

  //svg visual
  Visual svgVisual = factory.CreateVisual( TEST_SVG_FILE_NAME, ImageDimensions() );
  svgVisual.SetSize( visualSize );
  DALI_TEST_EQUALS( svgVisual.GetSize(), visualSize, TEST_LOCATION );
  svgVisual.GetNaturalSize(naturalSize);
  // TEST_SVG_FILE:
  //  <svg width="100" height="100">
  //  <circle cx="50" cy="50" r="40" stroke="green" stroke-width="4" fill="yellow" />
  //  </svg>
  DALI_TEST_EQUALS( naturalSize, Vector2(100.f, 100.f), TEST_LOCATION );
  END_TEST;
}

int UtcDaliVisualSetOnOffStage(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualSetDepthIndex" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert("rendererType",  "COLOR");
  propertyMap.Insert("mixColor",  Color::BLUE);
  Visual visual = factory.CreateVisual( propertyMap );

  Actor actor = Actor::New();
  actor.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  visual.SetOnStage( actor );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  visual.SetOffStage( actor );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliVisualRemoveAndReset(void)
{
  ToolkitTestApplication application;
  tet_infoline( "intUtcDaliVisualRemoveAndReset" );

  VisualFactory factory = VisualFactory::Get();

  Actor actor = Actor::New();
  actor.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor );

  Visual imageVisual;
  // test calling RemoveAndReset with an empty handle
  try
  {
    imageVisual.RemoveAndReset( actor );
    tet_result(TET_PASS);
  }
  catch (DaliException& exception)
  {
    tet_result(TET_FAIL);
  }

  Image image = ResourceImage::New(TEST_IMAGE_FILE_NAME, ImageDimensions(100, 200));
  imageVisual = factory.CreateVisual(image);
  DALI_TEST_CHECK( imageVisual );

  imageVisual.SetOnStage( actor );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  imageVisual.RemoveAndReset( actor );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u ); // visual is removed from actor
  DALI_TEST_CHECK( !imageVisual ); // visual is reset

  END_TEST;
}

int UtcDaliVisualGetPropertyMap1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMap1: ColorVisual" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert("rendererType",  "COLOR");
  propertyMap.Insert("mixColor",  Color::BLUE);
  Visual colorVisual = factory.CreateVisual( propertyMap );

  Property::Map resultMap;
  colorVisual.CreatePropertyMap( resultMap );

  Property::Value* typeValue = resultMap.Find( "rendererType",  Property::STRING );
  DALI_TEST_CHECK( typeValue );
  DALI_TEST_CHECK( typeValue->Get<std::string>() == "COLOR" );

  Property::Value* colorValue = resultMap.Find( "mixColor",  Property::VECTOR4 );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get<Vector4>() == Color::BLUE );

  // change the blend color
  Actor actor;
  colorVisual.RemoveAndReset( actor );
  propertyMap["mixColor"] = Color::CYAN;
  colorVisual = factory.CreateVisual( propertyMap  );
  colorVisual.CreatePropertyMap( resultMap );

  colorValue = resultMap.Find( "mixColor",  Property::VECTOR4 );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get<Vector4>() == Color::CYAN );

  END_TEST;
}

int UtcDaliVisualGetPropertyMap2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMap2: BorderVisual" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert("rendererType",  "BORDER");
  propertyMap.Insert("borderColor",  Color::BLUE);
  propertyMap.Insert("borderSize",  5.f);
  Visual borderVisual = factory.CreateVisual( propertyMap );

  Property::Map resultMap;
  borderVisual.CreatePropertyMap( resultMap );

  // check the property values from the returned map from visual
  Property::Value* typeValue = resultMap.Find( "rendererType",  Property::STRING );
  DALI_TEST_CHECK( typeValue );
  DALI_TEST_CHECK( typeValue->Get<std::string>() == "BORDER" );

  Property::Value* colorValue = resultMap.Find( "borderColor",  Property::VECTOR4 );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get<Vector4>() == Color::BLUE );

  Property::Value* sizeValue = resultMap.Find( "borderSize",  Property::FLOAT );
  DALI_TEST_CHECK( sizeValue );
  DALI_TEST_CHECK( sizeValue->Get<float>() == 5.f );

  Property::Map propertyMap1;
  propertyMap1[ "rendererType" ] = "BORDER";
  propertyMap1[ "borderColor"  ] = Color::CYAN;
  propertyMap1[ "borderSize"   ] = 10.0f;
  borderVisual = factory.CreateVisual( propertyMap1 );
  borderVisual.CreatePropertyMap( resultMap );

  typeValue = resultMap.Find( "rendererType",  Property::STRING );
  DALI_TEST_CHECK( typeValue );
  DALI_TEST_CHECK( typeValue->Get<std::string>() == "BORDER" );

  colorValue = resultMap.Find( "borderColor",  Property::VECTOR4 );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get<Vector4>() == Color::CYAN );

  colorValue = resultMap.Find( "borderSize",  Property::FLOAT );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get<float>() == 10.f );

  END_TEST;
}

int UtcDaliVisualGetPropertyMap3(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMap3: linear GradientVisual" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert("rendererType",  "GRADIENT");

  Vector2 start(-1.f, -1.f);
  Vector2 end(1.f, 1.f);
  propertyMap.Insert("startPosition", start);
  propertyMap.Insert("endPosition", end);
  propertyMap.Insert("spreadMethod", "REPEAT");

  propertyMap.Insert("stopOffset",   Vector2(0.2f, 0.8f));

  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert("stopColor",   stopColors);

  Visual gradientVisual = factory.CreateVisual(propertyMap);

  Property::Map resultMap;
  gradientVisual.CreatePropertyMap( resultMap );

  // check the property values from the returned map from visual
  Property::Value* value = resultMap.Find( "rendererType",  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "GRADIENT" );

  value = resultMap.Find( "units",  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "OBJECT_BOUNDING_BOX" );

  value = resultMap.Find( "spreadMethod",   Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "REPEAT" );

  value = resultMap.Find( "startPosition",   Property::VECTOR2 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector2>(), start , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "endPosition",   Property::VECTOR2 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector2>(), end , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "stopOffset",   Property::ARRAY );
  DALI_TEST_CHECK( value );
  Property::Array* offsetArray = value->GetArray();
  DALI_TEST_CHECK( offsetArray->Count() == 2 );
  DALI_TEST_EQUALS( offsetArray->GetElementAt(0).Get<float>(), 0.2f , Math::MACHINE_EPSILON_100, TEST_LOCATION );
  DALI_TEST_EQUALS( offsetArray->GetElementAt(1).Get<float>(), 0.8f , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "stopColor",   Property::ARRAY );
  DALI_TEST_CHECK( value );
  Property::Array* colorArray = value->GetArray();
  DALI_TEST_CHECK( colorArray->Count() == 2 );
  DALI_TEST_EQUALS( colorArray->GetElementAt(0).Get<Vector4>(), Color::RED , Math::MACHINE_EPSILON_100, TEST_LOCATION );
  DALI_TEST_EQUALS( colorArray->GetElementAt(1).Get<Vector4>(), Color::GREEN , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualGetPropertyMap4(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMap4: radial GradientVisual" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert("rendererType",  "GRADIENT");

  Vector2 center(100.f, 100.f);
  float radius = 100.f;
  propertyMap.Insert("units",  "USER_SPACE");
  propertyMap.Insert("center",  center);
  propertyMap.Insert("radius",  radius);
  propertyMap.Insert("stopOffset",   Vector3(0.1f, 0.3f, 1.1f));

  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::BLACK );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert("stopColor",   stopColors);

  Visual gradientVisual = factory.CreateVisual(propertyMap);
  DALI_TEST_CHECK( gradientVisual );

  Property::Map resultMap;
  gradientVisual.CreatePropertyMap( resultMap );

  // check the property values from the returned map from visual
  Property::Value* value = resultMap.Find( "rendererType",  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "GRADIENT" );

  value = resultMap.Find( "units",  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "USER_SPACE" );

  value = resultMap.Find( "spreadMethod",   Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "PAD" );

  value = resultMap.Find( "center",  Property::VECTOR2 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector2>(), center , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "radius",  Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), radius , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "stopOffset",   Property::ARRAY );
  DALI_TEST_CHECK( value );
  Property::Array* offsetArray = value->GetArray();
  DALI_TEST_CHECK( offsetArray->Count() == 3 );
  DALI_TEST_EQUALS( offsetArray->GetElementAt(0).Get<float>(), 0.1f , Math::MACHINE_EPSILON_100, TEST_LOCATION );
  DALI_TEST_EQUALS( offsetArray->GetElementAt(1).Get<float>(), 0.3f , Math::MACHINE_EPSILON_100, TEST_LOCATION );
  // any stop value will be clamped to [0.0, 1.0];
  DALI_TEST_EQUALS( offsetArray->GetElementAt(2).Get<float>(), 1.0f , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "stopColor",   Property::ARRAY );
  DALI_TEST_CHECK( value );
  Property::Array* colorArray = value->GetArray();
  DALI_TEST_CHECK( colorArray->Count() == 3 );
  DALI_TEST_EQUALS( colorArray->GetElementAt(0).Get<Vector4>(), Color::RED , Math::MACHINE_EPSILON_100, TEST_LOCATION );
  DALI_TEST_EQUALS( colorArray->GetElementAt(1).Get<Vector4>(), Color::BLACK , Math::MACHINE_EPSILON_100, TEST_LOCATION );
  DALI_TEST_EQUALS( colorArray->GetElementAt(2).Get<Vector4>(), Color::GREEN , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualGetPropertyMap5(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMap5: ImageVisual" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert( "rendererType",  "IMAGE" );
  propertyMap.Insert( "url",  TEST_IMAGE_FILE_NAME );
  propertyMap.Insert( "desiredWidth",   20 );
  propertyMap.Insert( "desiredHeight",   30 );
  propertyMap.Insert( "fittingMode",   "FIT_HEIGHT" );
  propertyMap.Insert( "samplingMode",   "BOX_THEN_NEAREST" );
  propertyMap.Insert( "synchronousLoading",   true );

  Visual imageVisual = factory.CreateVisual(propertyMap);
  DALI_TEST_CHECK( imageVisual );

  Property::Map resultMap;
  imageVisual.CreatePropertyMap( resultMap );

  // check the property values from the returned map from visual
  Property::Value* value = resultMap.Find( "rendererType",  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "IMAGE" );

  value = resultMap.Find( "url",  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_IMAGE_FILE_NAME );

  value = resultMap.Find( "fittingMode",   Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "FIT_HEIGHT" );

  value = resultMap.Find( "samplingMode",   Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "BOX_THEN_NEAREST" );

  value = resultMap.Find( "desiredWidth",   Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 20 );

  value = resultMap.Find( "desiredHeight",   Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 30 );

  value = resultMap.Find( "synchronousLoading",   Property::BOOLEAN );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<bool>() == true );

  // Get an image visual with an image handle, and test the default property values
  Image image = ResourceImage::New(TEST_IMAGE_FILE_NAME, ImageDimensions(100, 200));
  imageVisual = factory.CreateVisual(image);
  imageVisual.CreatePropertyMap( resultMap );

  value = resultMap.Find( "rendererType",  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "IMAGE" );

  value = resultMap.Find( "url",  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_IMAGE_FILE_NAME );

  value = resultMap.Find( "fittingMode",   Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "SHRINK_TO_FIT" );

  value = resultMap.Find( "samplingMode",   Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "BOX" );

  value = resultMap.Find( "desiredWidth",   Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 100 );

  value = resultMap.Find( "desiredHeight",   Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 200 );

  value = resultMap.Find( "synchronousLoading",   Property::BOOLEAN );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<bool>() == false );

  END_TEST;
}

int UtcDaliVisualGetPropertyMap6(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMap6: NPatchVisual" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert( "rendererType",  "IMAGE" );
  propertyMap.Insert( "url",  TEST_NPATCH_FILE_NAME );
  propertyMap.Insert( "borderOnly",  true );
  Visual nPatchVisual = factory.CreateVisual( propertyMap );

  Property::Map resultMap;
  nPatchVisual.CreatePropertyMap( resultMap );

  // check the property values from the returned map from visual
  Property::Value* value = resultMap.Find( "rendererType",  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "IMAGE" );

  value = resultMap.Find( "url",  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_NPATCH_FILE_NAME );

  value = resultMap.Find( "borderOnly",  Property::BOOLEAN );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<bool>() );

  END_TEST;
}

int UtcDaliVisualGetPropertyMap7(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMap7: SvgVisual" );

  // request SvgVisual with a property map
  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert( "rendererType",  "IMAGE" );
  propertyMap.Insert( "url",  TEST_SVG_FILE_NAME );
  Visual svgVisual = factory.CreateVisual( propertyMap );

  Property::Map resultMap;
  svgVisual.CreatePropertyMap( resultMap );
  // check the property values from the returned map from a visual
  Property::Value* value = resultMap.Find( "rendererType",  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "IMAGE" );

  value = resultMap.Find( "url",  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_SVG_FILE_NAME );

  // request SvgVisual with an URL
  Visual svgVisual2 = factory.CreateVisual( TEST_SVG_FILE_NAME, ImageDimensions() );
  resultMap.Clear();
  svgVisual2.CreatePropertyMap( resultMap );
  // check the property values from the returned map from a visual
  value = resultMap.Find( "rendererType",  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == "IMAGE" );

  value = resultMap.Find( "url",  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_SVG_FILE_NAME );

  END_TEST;
}

//Mesh visual
int UtcDaliVisualGetPropertyMap8(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMap8: MeshVisual" );

  //Request MeshVisual using a property map.
  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert( "rendererType", "MESH" );
  propertyMap.Insert( "objectUrl", TEST_OBJ_FILE_NAME );
  propertyMap.Insert( "materialUrl", TEST_MTL_FILE_NAME );
  propertyMap.Insert( "texturesPath", TEST_RESOURCE_LOCATION );
  propertyMap.Insert( "shaderType", "TEXTURELESS" );
  propertyMap.Insert( "lightPosition", Vector3( 5.0f, 10.0f, 15.0f) );
  Visual meshVisual = factory.CreateVisual( propertyMap );

  Property::Map resultMap;
  meshVisual.CreatePropertyMap( resultMap );

  //Check values in the result map are identical to the initial map's values.
  Property::Value* value = resultMap.Find( "rendererType", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<std::string>(), "MESH", TEST_LOCATION );

  value = resultMap.Find( "objectUrl", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<std::string>(), TEST_OBJ_FILE_NAME, TEST_LOCATION );

  value = resultMap.Find( "materialUrl", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<std::string>(), TEST_MTL_FILE_NAME, TEST_LOCATION );

  value = resultMap.Find( "texturesPath", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<std::string>(), TEST_RESOURCE_LOCATION, TEST_LOCATION );

  value = resultMap.Find( "shaderType", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<std::string>(), "TEXTURELESS", TEST_LOCATION );

  value = resultMap.Find( "lightPosition", Property::VECTOR3 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector3>(), Vector3( 5.0f, 10.0f, 15.0f), Math::MACHINE_EPSILON_100, TEST_LOCATION );

  END_TEST;
}

//Primitive shape visual
int UtcDaliVisualGetPropertyMap9(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMap9: PrimitiveVisual" );

  Vector4 color = Vector4( 1.0, 0.8, 0.6, 1.0);
  Vector3 dimensions = Vector3( 1.0, 2.0, 3.0 );

  //Request PrimitiveVisual using a property map.
  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert( "rendererType", "PRIMITIVE" );
  propertyMap.Insert( "shape", "CUBE" );
  propertyMap.Insert( "shapeColor", color );
  propertyMap.Insert( "slices", 10 );
  propertyMap.Insert( "stacks", 20 );
  propertyMap.Insert( "scaleTopRadius", 30.0f );
  propertyMap.Insert( "scaleBottomRadius", 40.0f );
  propertyMap.Insert( "scaleHeight", 50.0f );
  propertyMap.Insert( "scaleRadius", 60.0f );
  propertyMap.Insert( "scaleDimensions", dimensions );
  propertyMap.Insert( "bevelPercentage", 0.3f );
  propertyMap.Insert( "bevelSmoothness", 0.6f );
  propertyMap.Insert( "lightPosition", Vector3( 5.0f, 10.0f, 15.0f) );
  Visual primitiveVisual = factory.CreateVisual( propertyMap );

  Property::Map resultMap;
  primitiveVisual.CreatePropertyMap( resultMap );

  //Check values in the result map are identical to the initial map's values.
  Property::Value* value = resultMap.Find( "rendererType", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<std::string>(), "PRIMITIVE", TEST_LOCATION );

  value = resultMap.Find( "shape", Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<std::string>(), "CUBE", TEST_LOCATION );

  value = resultMap.Find( "shapeColor", Property::VECTOR4 );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<Vector4>() == color );
  DALI_TEST_EQUALS( value->Get<Vector4>(), color, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "slices", Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<int>(), 10, TEST_LOCATION );

  value = resultMap.Find( "stacks", Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<int>(), 20, TEST_LOCATION );

  value = resultMap.Find( "scaleTopRadius", Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), 30.0f, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "scaleBottomRadius", Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), 40.0f, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "scaleHeight", Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), 50.0f, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "scaleRadius", Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), 60.0f, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "scaleDimensions", Property::VECTOR3 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector3>(), dimensions, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "bevelPercentage", Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), 0.3f, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "bevelSmoothness", Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), 0.6f, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( "lightPosition", Property::VECTOR3 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector3>(), Vector3( 5.0f, 10.0f, 15.0f), Math::MACHINE_EPSILON_100, TEST_LOCATION );

  END_TEST;
}
