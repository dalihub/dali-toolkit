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
#include <unistd.h>

#include <dali-toolkit-test-suite-utils.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/rendering/shader.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/visual-factory/devel-visual-properties.h>
#include <dali-toolkit/devel-api/align-enums.h>
#include <dali-toolkit/dali-toolkit.h>
#include "dummy-control.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* TEST_IMAGE_FILE_NAME =  TEST_RESOURCE_DIR "/gallery-small-1.jpg";
const char* TEST_NPATCH_FILE_NAME =  "gallery_image_01.9.jpg";
const char* TEST_SVG_FILE_NAME = TEST_RESOURCE_DIR "/svg1.svg";
const char* TEST_OBJ_FILE_NAME = TEST_RESOURCE_DIR "/Cube.obj";
const char* TEST_MTL_FILE_NAME = TEST_RESOURCE_DIR "/ToyRobot-Metal.mtl";
const char* TEST_RESOURCE_LOCATION = TEST_RESOURCE_DIR "/";

const std::string DEFAULT_FONT_DIR( "/resources/fonts" );

bool DaliTestCheckMaps( const Property::Map& fontStyleMapGet, const Property::Map& fontStyleMapSet )
{
  if( fontStyleMapGet.Count() == fontStyleMapSet.Count() )
  {
    for( unsigned int index = 0u; index < fontStyleMapGet.Count(); ++index )
    {
      const KeyValuePair& valueGet = fontStyleMapGet.GetKeyValue( index );

      Property::Value* valueSet = fontStyleMapSet.Find( valueGet.first.stringKey );
      if( NULL != valueSet )
      {
        if( valueGet.second.Get<std::string>() != valueSet->Get<std::string>() )
        {
          tet_printf( "  Value got : [%s], expected : [%s]", valueGet.second.Get<std::string>().c_str(), valueSet->Get<std::string>().c_str() );
          return false;
        }
      }
      else
      {
        tet_printf( "  The key %s doesn't exist.", valueGet.first.stringKey.c_str() );
        return false;
      }
    }
  }

  return true;
}

} //namespace

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
  propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR,  Color::BLUE);
  Visual::Base visual = factory.CreateVisual( propertyMap );

  Visual::Base visualCopy( visual );
  DALI_TEST_CHECK(visual == visualCopy);

  Visual::Base emptyVisual;
  Visual::Base emptyVisualCopy( emptyVisual );
  DALI_TEST_CHECK(emptyVisual == emptyVisualCopy);

  Visual::Base visualEquals;
  visualEquals = visual;
  DALI_TEST_CHECK(visual == visualEquals);

  Visual::Base emptyVisualEquals;
  emptyVisualEquals = emptyVisual;
  DALI_TEST_CHECK( emptyVisual == emptyVisualEquals );

  //self assignment
  visual = visual;
  DALI_TEST_CHECK( visual = visualCopy );

  END_TEST;
}

int UtcDaliVisualSetName01(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualSetName" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR,  Color::BLUE);
  Visual::Base visual = factory.CreateVisual( propertyMap );

  const char* visualName = "backgroundVisual";
  visual.SetName( visualName );

  DALI_TEST_EQUALS( visual.GetName(), visualName, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualSetGetDepthIndex(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualSetDepthIndex" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR,  Color::BLUE);
  Visual::Base visual = factory.CreateVisual( propertyMap );

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
  map[ Visual::Property::TYPE ] = Visual::COLOR;
  map[ ColorVisual::Property::MIX_COLOR ] = Color::MAGENTA;
  Visual::Base colorVisual = factory.CreateVisual( map );
  colorVisual.SetSize( visualSize );
  DALI_TEST_EQUALS( colorVisual.GetSize(), visualSize, TEST_LOCATION );
  colorVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS( naturalSize, Vector2::ZERO, TEST_LOCATION );

  // image visual
  Image image = ResourceImage::New(TEST_IMAGE_FILE_NAME, ImageDimensions(100, 200));
  Visual::Base imageVisual = factory.CreateVisual( image );
  imageVisual.SetSize( visualSize );
  DALI_TEST_EQUALS( imageVisual.GetSize(), visualSize, TEST_LOCATION );
  imageVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS( naturalSize, Vector2(100.f, 200.f), TEST_LOCATION );

  // n patch visual
  TestPlatformAbstraction& platform = application.GetPlatform();
  Vector2 testSize(80.f, 160.f);
  platform.SetClosestImageSize(testSize);
  image = ResourceImage::New(TEST_NPATCH_FILE_NAME);
  Visual::Base nPatchVisual = factory.CreateVisual( image );
  nPatchVisual.SetSize( visualSize );
  DALI_TEST_EQUALS( nPatchVisual.GetSize(), visualSize, TEST_LOCATION );
  nPatchVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS( naturalSize, testSize, TEST_LOCATION );

  // border visual
  float borderSize = 5.f;
  map.Clear();
  map[ Visual::Property::TYPE ] = Visual::BORDER;
  map[ BorderVisual::Property::COLOR  ] = Color::RED;
  map[ BorderVisual::Property::SIZE   ] = borderSize;
  Visual::Base borderVisual = factory.CreateVisual( map );
  borderVisual.SetSize( visualSize );
  DALI_TEST_EQUALS( borderVisual.GetSize(), visualSize, TEST_LOCATION );
  borderVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS( naturalSize, Vector2::ZERO, TEST_LOCATION );

  // gradient gradientVisual
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::GRADIENT);
  Vector2 start(-1.f, -1.f);
  Vector2 end(1.f, 1.f);
  propertyMap.Insert(GradientVisual::Property::START_POSITION,   start);
  propertyMap.Insert(GradientVisual::Property::END_POSITION,   end);
  propertyMap.Insert(GradientVisual::Property::STOP_OFFSET,   Vector2(0.f, 1.f));
  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert(GradientVisual::Property::STOP_COLOR,   stopColors);
  Visual::Base gradientVisual = factory.CreateVisual(propertyMap);
  gradientVisual.SetSize( visualSize );
  DALI_TEST_EQUALS( gradientVisual.GetSize(), visualSize, TEST_LOCATION );
  gradientVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS( naturalSize, Vector2::ZERO,TEST_LOCATION );

  // svg visual
  Visual::Base svgVisual = factory.CreateVisual( TEST_SVG_FILE_NAME, ImageDimensions() );
  svgVisual.SetSize( visualSize );
  DALI_TEST_EQUALS( svgVisual.GetSize(), visualSize, TEST_LOCATION );
  svgVisual.GetNaturalSize(naturalSize);
  // TEST_SVG_FILE:
  //  <svg width="100" height="100">
  //  <circle cx="50" cy="50" r="40" stroke="green" stroke-width="4" fill="yellow" />
  //  </svg>
  DALI_TEST_EQUALS( naturalSize, Vector2(100.f, 100.f), TEST_LOCATION );

  // svg visual with a size
  Visual::Base svgVisual2 = factory.CreateVisual( TEST_SVG_FILE_NAME, ImageDimensions(200, 200) );
  DALI_TEST_EQUALS( svgVisual2.GetSize(), Vector2( 200.f, 200.f ), TEST_LOCATION );
  svgVisual2.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS( naturalSize, Vector2(100.f, 100.f), TEST_LOCATION ); // Natural size should still be 100, 100

  // Batch Image visual
  propertyMap.Clear();
  propertyMap.Insert( Visual::Property::TYPE, Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME );
  propertyMap.Insert( ImageVisual::Property::BATCHING_ENABLED, true );
  Visual::Base batchImageVisual = factory.CreateVisual( propertyMap );
  batchImageVisual.SetSize( visualSize );
  DALI_TEST_EQUALS( batchImageVisual.GetSize(), visualSize, TEST_LOCATION );
  batchImageVisual.GetNaturalSize( naturalSize );
  DALI_TEST_EQUALS( naturalSize, Vector2( 80.0f, 160.0f ), TEST_LOCATION );

  // Text visual.

  // Load some fonts to get the same metrics on different platforms.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi( 96u, 96u );

  char* pathNamePtr = get_current_dir_name();
  const std::string pathName( pathNamePtr );
  free( pathNamePtr );

  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf" );

  propertyMap.Clear();
  propertyMap.Insert( Visual::Property::TYPE, Visual::TEXT );
  propertyMap.Insert( TextVisual::Property::ENABLE_MARKUP, true );
  propertyMap.Insert( TextVisual::Property::TEXT, "<font family='TizenSans' size='12'>Hello world</font>" );
  propertyMap.Insert( TextVisual::Property::MULTI_LINE, true );

  Visual::Base textVisual = factory.CreateVisual( propertyMap );
  textVisual.GetNaturalSize( naturalSize );
  DALI_TEST_EQUALS( naturalSize, Size( 80.f, 20.f ), TEST_LOCATION );

  const float height = textVisual.GetHeightForWidth( 40.f );
  DALI_TEST_EQUALS( height, 40.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualSetOnOffStage(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualSetDepthIndex" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR,  Color::BLUE);
  Visual::Base visual = factory.CreateVisual( propertyMap );

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

  Visual::Base imageVisual;
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
  propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR,  Color::BLUE);
  Visual::Base colorVisual = factory.CreateVisual( propertyMap );

  Property::Map resultMap;
  colorVisual.CreatePropertyMap( resultMap );

  Property::Value* typeValue = resultMap.Find( Visual::Property::TYPE,  Property::INTEGER );
  DALI_TEST_CHECK( typeValue );
  DALI_TEST_CHECK( typeValue->Get<int>() == Visual::COLOR );

  Property::Value* colorValue = resultMap.Find( ColorVisual::Property::MIX_COLOR,  Property::VECTOR4 );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get<Vector4>() == Color::BLUE );

  // change the blend color
  Actor actor;
  colorVisual.RemoveAndReset( actor );
  propertyMap[ColorVisual::Property::MIX_COLOR] = Color::CYAN;
  colorVisual = factory.CreateVisual( propertyMap  );
  colorVisual.CreatePropertyMap( resultMap );

  colorValue = resultMap.Find( ColorVisual::Property::MIX_COLOR,  Property::VECTOR4 );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get<Vector4>() == Color::CYAN );

  // Test the properties. TODO: to be completed.
  colorVisual.SetProperty( ColorVisual::Property::MIX_COLOR, Color::RED );
  Property::Value value = colorVisual.GetProperty( ColorVisual::Property::MIX_COLOR );

  END_TEST;
}

int UtcDaliVisualGetPropertyMap2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMap2: BorderVisual" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::BORDER);
  propertyMap.Insert(BorderVisual::Property::COLOR,  Color::BLUE);
  propertyMap.Insert(BorderVisual::Property::SIZE,  5.f);
  Visual::Base borderVisual = factory.CreateVisual( propertyMap );

  Property::Map resultMap;
  borderVisual.CreatePropertyMap( resultMap );

  // check the property values from the returned map from visual
  Property::Value* typeValue = resultMap.Find( Visual::Property::TYPE,  Property::INTEGER );
  DALI_TEST_CHECK( typeValue );
  DALI_TEST_CHECK( typeValue->Get<int>() == Visual::BORDER );

  Property::Value* colorValue = resultMap.Find( BorderVisual::Property::COLOR,  Property::VECTOR4 );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get<Vector4>() == Color::BLUE );

  Property::Value* sizeValue = resultMap.Find( BorderVisual::Property::SIZE,  Property::FLOAT );
  DALI_TEST_CHECK( sizeValue );
  DALI_TEST_CHECK( sizeValue->Get<float>() == 5.f );

  Property::Map propertyMap1;
  propertyMap1[ Visual::Property::TYPE ] = Visual::BORDER;
  propertyMap1[ BorderVisual::Property::COLOR  ] = Color::CYAN;
  propertyMap1[ BorderVisual::Property::SIZE   ] = 10.0f;
  borderVisual = factory.CreateVisual( propertyMap1 );
  borderVisual.CreatePropertyMap( resultMap );

  typeValue = resultMap.Find( Visual::Property::TYPE,  Property::INTEGER );
  DALI_TEST_CHECK( typeValue );
  DALI_TEST_CHECK( typeValue->Get<int>() == Visual::BORDER );

  colorValue = resultMap.Find( BorderVisual::Property::COLOR,  Property::VECTOR4 );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get<Vector4>() == Color::CYAN );

  colorValue = resultMap.Find( BorderVisual::Property::SIZE,  Property::FLOAT );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get<float>() == 10.f );

  // Test the properties. TODO: to be completed.
  borderVisual.SetProperty( BorderVisual::Property::COLOR, Color::RED );
  Property::Value value = borderVisual.GetProperty( BorderVisual::Property::COLOR );

  END_TEST;
}

int UtcDaliVisualGetPropertyMap3(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMap3: linear GradientVisual" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::GRADIENT);

  Vector2 start(-1.f, -1.f);
  Vector2 end(1.f, 1.f);
  propertyMap.Insert(GradientVisual::Property::START_POSITION, start);
  propertyMap.Insert(GradientVisual::Property::END_POSITION, end);
  propertyMap.Insert(GradientVisual::Property::SPREAD_METHOD, GradientVisual::SpreadMethod::REPEAT);

  propertyMap.Insert(GradientVisual::Property::STOP_OFFSET,   Vector2(0.2f, 0.8f));

  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert(GradientVisual::Property::STOP_COLOR,   stopColors);

  Visual::Base gradientVisual = factory.CreateVisual(propertyMap);

  Property::Map resultMap;
  gradientVisual.CreatePropertyMap( resultMap );

  // check the property values from the returned map from visual
  Property::Value* value = resultMap.Find( Visual::Property::TYPE,  Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == Visual::GRADIENT );

  value = resultMap.Find( GradientVisual::Property::UNITS,  Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == GradientVisual::Units::OBJECT_BOUNDING_BOX );

  value = resultMap.Find( GradientVisual::Property::SPREAD_METHOD,   Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == GradientVisual::SpreadMethod::REPEAT );

  value = resultMap.Find( GradientVisual::Property::START_POSITION,   Property::VECTOR2 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector2>(), start , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( GradientVisual::Property::END_POSITION,   Property::VECTOR2 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector2>(), end , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( GradientVisual::Property::STOP_OFFSET,   Property::ARRAY );
  DALI_TEST_CHECK( value );
  Property::Array* offsetArray = value->GetArray();
  DALI_TEST_CHECK( offsetArray->Count() == 2 );
  DALI_TEST_EQUALS( offsetArray->GetElementAt(0).Get<float>(), 0.2f , Math::MACHINE_EPSILON_100, TEST_LOCATION );
  DALI_TEST_EQUALS( offsetArray->GetElementAt(1).Get<float>(), 0.8f , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( GradientVisual::Property::STOP_COLOR,   Property::ARRAY );
  DALI_TEST_CHECK( value );
  Property::Array* colorArray = value->GetArray();
  DALI_TEST_CHECK( colorArray->Count() == 2 );
  DALI_TEST_EQUALS( colorArray->GetElementAt(0).Get<Vector4>(), Color::RED , Math::MACHINE_EPSILON_100, TEST_LOCATION );
  DALI_TEST_EQUALS( colorArray->GetElementAt(1).Get<Vector4>(), Color::GREEN , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  // Test the properties. TODO: to be completed.
  gradientVisual.SetProperty( GradientVisual::Property::STOP_COLOR, Color::RED );
  Property::Value gradientValue = gradientVisual.GetProperty( GradientVisual::Property::STOP_COLOR );

  END_TEST;
}

int UtcDaliVisualGetPropertyMap4(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMap4: radial GradientVisual" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::GRADIENT);

  Vector2 center(100.f, 100.f);
  float radius = 100.f;
  propertyMap.Insert(GradientVisual::Property::UNITS,  GradientVisual::Units::USER_SPACE);
  propertyMap.Insert(GradientVisual::Property::CENTER,  center);
  propertyMap.Insert(GradientVisual::Property::RADIUS,  radius);
  propertyMap.Insert(GradientVisual::Property::STOP_OFFSET,   Vector3(0.1f, 0.3f, 1.1f));

  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::BLACK );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert(GradientVisual::Property::STOP_COLOR,   stopColors);

  Visual::Base gradientVisual = factory.CreateVisual(propertyMap);
  DALI_TEST_CHECK( gradientVisual );

  Property::Map resultMap;
  gradientVisual.CreatePropertyMap( resultMap );

  // check the property values from the returned map from visual
  Property::Value* value = resultMap.Find( Visual::Property::TYPE,  Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == Visual::GRADIENT );

  value = resultMap.Find( GradientVisual::Property::UNITS,  Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == GradientVisual::Units::USER_SPACE );

  value = resultMap.Find( GradientVisual::Property::SPREAD_METHOD,   Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == GradientVisual::SpreadMethod::PAD );

  value = resultMap.Find( GradientVisual::Property::CENTER,  Property::VECTOR2 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector2>(), center , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( GradientVisual::Property::RADIUS,  Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), radius , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( GradientVisual::Property::STOP_OFFSET,   Property::ARRAY );
  DALI_TEST_CHECK( value );
  Property::Array* offsetArray = value->GetArray();
  DALI_TEST_CHECK( offsetArray->Count() == 3 );
  DALI_TEST_EQUALS( offsetArray->GetElementAt(0).Get<float>(), 0.1f , Math::MACHINE_EPSILON_100, TEST_LOCATION );
  DALI_TEST_EQUALS( offsetArray->GetElementAt(1).Get<float>(), 0.3f , Math::MACHINE_EPSILON_100, TEST_LOCATION );
  // any stop value will be clamped to [0.0, 1.0];
  DALI_TEST_EQUALS( offsetArray->GetElementAt(2).Get<float>(), 1.0f , Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( GradientVisual::Property::STOP_COLOR,   Property::ARRAY );
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
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_IMAGE_FILE_NAME );
  propertyMap.Insert( ImageVisual::Property::DESIRED_WIDTH,   20 );
  propertyMap.Insert( ImageVisual::Property::DESIRED_HEIGHT,   30 );
  propertyMap.Insert( ImageVisual::Property::FITTING_MODE,   FittingMode::FIT_HEIGHT );
  propertyMap.Insert( ImageVisual::Property::SAMPLING_MODE,   SamplingMode::BOX_THEN_NEAREST );
  propertyMap.Insert( ImageVisual::Property::PIXEL_AREA, Vector4( 0.25f, 0.25f, 0.5f, 0.5f ) );
  propertyMap.Insert( ImageVisual::Property::WRAP_MODE_U, WrapMode::REPEAT );
  propertyMap.Insert( ImageVisual::Property::WRAP_MODE_V, WrapMode::MIRRORED_REPEAT );
  propertyMap.Insert( "synchronousLoading",   true );

  Visual::Base imageVisual = factory.CreateVisual(propertyMap);
  DALI_TEST_CHECK( imageVisual );

  Property::Map resultMap;
  imageVisual.CreatePropertyMap( resultMap );

  // check the property values from the returned map from visual
  Property::Value* value = resultMap.Find( Visual::Property::TYPE,  Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == Visual::IMAGE );

  value = resultMap.Find( ImageVisual::Property::URL,  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_IMAGE_FILE_NAME );

  value = resultMap.Find( ImageVisual::Property::FITTING_MODE,   Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == FittingMode::FIT_HEIGHT );

  value = resultMap.Find( ImageVisual::Property::SAMPLING_MODE,   Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == SamplingMode::BOX_THEN_NEAREST );

  value = resultMap.Find( ImageVisual::Property::DESIRED_WIDTH,   Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 20 );

  value = resultMap.Find( ImageVisual::Property::DESIRED_HEIGHT,   Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 30 );

  value = resultMap.Find( ImageVisual::Property::PIXEL_AREA, Property::VECTOR4 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector4>(), Vector4( 0.25f, 0.25f, 0.5f, 0.5f ), Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( ImageVisual::Property::WRAP_MODE_U, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK(  value->Get<int>() == WrapMode::REPEAT);

  value = resultMap.Find( ImageVisual::Property::WRAP_MODE_V, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK(  value->Get<int>() == WrapMode::MIRRORED_REPEAT);

  value = resultMap.Find( "synchronousLoading",   Property::BOOLEAN );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<bool>() == true );

  // Test the properties..
  Property::Value imageValue = imageVisual.GetProperty( ImageVisual::Property::URL );
  DALI_TEST_CHECK( imageValue.Get<std::string>() == TEST_IMAGE_FILE_NAME );

  // Other Properties for Image Visual can be tested here once implemented in the Visual

  tet_infoline( "UtcDaliVisualGetPropertyMap5 Ensuring Property without a Getter is not problematic" );
  imageValue = imageVisual.GetProperty( ImageVisual::Property::DESIRED_HEIGHT );
  DALI_TEST_CHECK( imageValue.Get<int>()  != 30 );


  // Rest of test uses Image not an url ( legacy )

  // Get an image visual with an image handle, and test the default property values
  Image image = ResourceImage::New(TEST_IMAGE_FILE_NAME, ImageDimensions(100, 200));
  imageVisual = factory.CreateVisual(image);
  imageVisual.CreatePropertyMap( resultMap );

  value = resultMap.Find( Visual::Property::TYPE,  Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == Visual::IMAGE );

  value = resultMap.Find( ImageVisual::Property::URL,  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_IMAGE_FILE_NAME );

  value = resultMap.Find( ImageVisual::Property::FITTING_MODE,   Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == FittingMode::SHRINK_TO_FIT );

  value = resultMap.Find( ImageVisual::Property::SAMPLING_MODE,   Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == SamplingMode::BOX );

  value = resultMap.Find( ImageVisual::Property::DESIRED_WIDTH,   Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 100 );

  value = resultMap.Find( ImageVisual::Property::DESIRED_HEIGHT,   Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 200 );

  value = resultMap.Find( ImageVisual::Property::PIXEL_AREA, Property::VECTOR4 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector4>(), Vector4( 0.f, 0.f, 1.f, 1.f ), Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( ImageVisual::Property::WRAP_MODE_U, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK(  value->Get<int>() == WrapMode::DEFAULT);

  value = resultMap.Find( ImageVisual::Property::WRAP_MODE_V, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK(  value->Get<int>() == WrapMode::DEFAULT);

  value = resultMap.Find( "synchronousLoading",   Property::BOOLEAN );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<bool>() == false );

  END_TEST;
}

int UtcDaliVisualGetPropertyMap6(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMap6: NPatchVisual" );

  bool BORDER_ONLY_SETTING = true;

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_NPATCH_FILE_NAME );
  propertyMap.Insert( ImageVisual::Property::BORDER_ONLY,  BORDER_ONLY_SETTING );
  Visual::Base nPatchVisual = factory.CreateVisual( propertyMap );

  Property::Map resultMap;
  nPatchVisual.CreatePropertyMap( resultMap );

  // check the property values from the returned map from visual
  Property::Value* value = resultMap.Find( Visual::Property::TYPE,  Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == Visual::IMAGE );

  value = resultMap.Find( ImageVisual::Property::URL,  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_NPATCH_FILE_NAME );

  value = resultMap.Find( ImageVisual::Property::BORDER_ONLY,  Property::BOOLEAN );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<bool>() );

  // Test the properties via Index.
  nPatchVisual.SetProperty( ImageVisual::Property::URL, TEST_NPATCH_FILE_NAME ); // May not able to change URL in future using SetProperty
  Property::Value nPatchValue = nPatchVisual.GetProperty( ImageVisual::Property::URL );
  DALI_TEST_CHECK( nPatchValue.Get<std::string>() == TEST_NPATCH_FILE_NAME );

  nPatchValue = nPatchVisual.GetProperty( ImageVisual::Property::BORDER_ONLY );
  DALI_TEST_CHECK( nPatchValue.Get<bool>() == BORDER_ONLY_SETTING );

  // Other Properties for N-Patch Visual can be tested here once implemented in the Visual

  tet_infoline( "UtcDaliVisualGetPropertyMap6 Ensuring Property without a Getter is not problematic" );
  nPatchValue = nPatchVisual.GetProperty( ImageVisual::Property::DESIRED_HEIGHT );
  DALI_TEST_CHECK( nPatchValue.Get<int>()  != 30 );

  END_TEST;
}

int UtcDaliVisualGetPropertyMap7(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMap7: SvgVisual" );

  // request SvgVisual with a property map
  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_SVG_FILE_NAME );
  Visual::Base svgVisual = factory.CreateVisual( propertyMap );

  Property::Map resultMap;
  svgVisual.CreatePropertyMap( resultMap );
  // check the property values from the returned map from a visual
  Property::Value* value = resultMap.Find( Visual::Property::TYPE,  Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == Visual::IMAGE );

  value = resultMap.Find( ImageVisual::Property::URL,  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_SVG_FILE_NAME );

  // request SvgVisual with an URL
  Visual::Base svgVisual2 = factory.CreateVisual( TEST_SVG_FILE_NAME, ImageDimensions() );
  resultMap.Clear();
  svgVisual2.CreatePropertyMap( resultMap );
  // check the property values from the returned map from a visual
  value = resultMap.Find( Visual::Property::TYPE,  Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == Visual::IMAGE );

  value = resultMap.Find( ImageVisual::Property::URL,  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_SVG_FILE_NAME );

  // Test the properties. TODO: to be completed.
  svgVisual.SetProperty( ImageVisual::Property::URL, TEST_SVG_FILE_NAME );
  Property::Value svgValue = svgVisual.GetProperty( ImageVisual::Property::URL );

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
  propertyMap.Insert( Visual::Property::TYPE, Visual::MESH );
  propertyMap.Insert( MeshVisual::Property::OBJECT_URL, TEST_OBJ_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::MATERIAL_URL, TEST_MTL_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::TEXTURES_PATH, TEST_RESOURCE_LOCATION );
  propertyMap.Insert( MeshVisual::Property::SHADING_MODE, MeshVisual::ShadingMode::TEXTURELESS_WITH_DIFFUSE_LIGHTING );
  propertyMap.Insert( MeshVisual::Property::LIGHT_POSITION, Vector3( 5.0f, 10.0f, 15.0f) );
  Visual::Base meshVisual = factory.CreateVisual( propertyMap );

  Property::Map resultMap;
  meshVisual.CreatePropertyMap( resultMap );

  //Check values in the result map are identical to the initial map's values.
  Property::Value* value = resultMap.Find( Visual::Property::TYPE, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<int>(), (int)Visual::MESH, TEST_LOCATION );

  value = resultMap.Find( MeshVisual::Property::OBJECT_URL, Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<std::string>(), TEST_OBJ_FILE_NAME, TEST_LOCATION );

  value = resultMap.Find( MeshVisual::Property::MATERIAL_URL, Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<std::string>(), TEST_MTL_FILE_NAME, TEST_LOCATION );

  value = resultMap.Find( MeshVisual::Property::TEXTURES_PATH, Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<std::string>(), TEST_RESOURCE_LOCATION, TEST_LOCATION );

  value = resultMap.Find( MeshVisual::Property::SHADING_MODE, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<int>(), (int)MeshVisual::ShadingMode::TEXTURELESS_WITH_DIFFUSE_LIGHTING, TEST_LOCATION );

  value = resultMap.Find( MeshVisual::Property::LIGHT_POSITION, Property::VECTOR3 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector3>(), Vector3( 5.0f, 10.0f, 15.0f), Math::MACHINE_EPSILON_100, TEST_LOCATION );

  // Test the properties. TODO: to be completed.
  meshVisual.SetProperty( MeshVisual::Property::OBJECT_URL, TEST_OBJ_FILE_NAME );
  Property::Value meshValue = meshVisual.GetProperty( MeshVisual::Property::OBJECT_URL );

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
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::CUBE );
  propertyMap.Insert( PrimitiveVisual::Property::MIX_COLOR, color );
  propertyMap.Insert( PrimitiveVisual::Property::SLICES, 10 );
  propertyMap.Insert( PrimitiveVisual::Property::STACKS, 20 );
  propertyMap.Insert( PrimitiveVisual::Property::SCALE_TOP_RADIUS, 30.0f );
  propertyMap.Insert( PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS, 40.0f );
  propertyMap.Insert( PrimitiveVisual::Property::SCALE_HEIGHT, 50.0f );
  propertyMap.Insert( PrimitiveVisual::Property::SCALE_RADIUS, 60.0f );
  propertyMap.Insert( PrimitiveVisual::Property::SCALE_DIMENSIONS, dimensions );
  propertyMap.Insert( PrimitiveVisual::Property::BEVEL_PERCENTAGE, 0.3f );
  propertyMap.Insert( PrimitiveVisual::Property::BEVEL_SMOOTHNESS, 0.6f );
  propertyMap.Insert( PrimitiveVisual::Property::LIGHT_POSITION, Vector3( 5.0f, 10.0f, 15.0f) );
  Visual::Base primitiveVisual = factory.CreateVisual( propertyMap );

  Property::Map resultMap;
  primitiveVisual.CreatePropertyMap( resultMap );

  //Check values in the result map are identical to the initial map's values.
  Property::Value* value = resultMap.Find( Visual::Property::TYPE, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<int>(), (int)Visual::PRIMITIVE, TEST_LOCATION );

  value = resultMap.Find( PrimitiveVisual::Property::SHAPE, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<int>(), (int)PrimitiveVisual::Shape::CUBE, TEST_LOCATION );

  value = resultMap.Find( PrimitiveVisual::Property::MIX_COLOR, Property::VECTOR4 );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<Vector4>() == color );
  DALI_TEST_EQUALS( value->Get<Vector4>(), color, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( PrimitiveVisual::Property::SLICES, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<int>(), 10, TEST_LOCATION );

  value = resultMap.Find( PrimitiveVisual::Property::STACKS, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<int>(), 20, TEST_LOCATION );

  value = resultMap.Find( PrimitiveVisual::Property::SCALE_TOP_RADIUS, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), 30.0f, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), 40.0f, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( PrimitiveVisual::Property::SCALE_HEIGHT, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), 50.0f, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( PrimitiveVisual::Property::SCALE_RADIUS, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), 60.0f, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( PrimitiveVisual::Property::SCALE_DIMENSIONS, Property::VECTOR3 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector3>(), dimensions, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( PrimitiveVisual::Property::BEVEL_PERCENTAGE, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), 0.3f, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( PrimitiveVisual::Property::BEVEL_SMOOTHNESS, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), 0.6f, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  value = resultMap.Find( PrimitiveVisual::Property::LIGHT_POSITION, Property::VECTOR3 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector3>(), Vector3( 5.0f, 10.0f, 15.0f), Math::MACHINE_EPSILON_100, TEST_LOCATION );

  // Test the properties. TODO: to be completed.
  primitiveVisual.SetProperty( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::CUBE );
  Property::Value primitiveValue = primitiveVisual.GetProperty( PrimitiveVisual::Property::SHAPE );

  END_TEST;
}

int UtcDaliVisualGetPropertyMapBatchImageVisual(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMapBatchImageVisual:" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::BATCHING_ENABLED, true );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME );
  propertyMap.Insert( ImageVisual::Property::DESIRED_WIDTH, 20 );
  propertyMap.Insert( ImageVisual::Property::DESIRED_HEIGHT, 30 );

  Visual::Base batchImageVisual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( batchImageVisual );

  Property::Map resultMap;
  batchImageVisual.CreatePropertyMap( resultMap );

  // Check the property values from the returned map from visual
  Property::Value* value = resultMap.Find( Visual::Property::TYPE, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == Visual::IMAGE );

  value = resultMap.Find( ImageVisual::Property::URL, Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_IMAGE_FILE_NAME );

  value = resultMap.Find( ImageVisual::Property::DESIRED_WIDTH, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 20 );

  value = resultMap.Find( ImageVisual::Property::DESIRED_HEIGHT, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 30 );

  // Test the properties. TODO: to be completed.
  batchImageVisual.SetProperty( ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME );
  Property::Value primitiveValue = batchImageVisual.GetProperty( ImageVisual::Property::URL );

  END_TEST;
}

//Text shape visual
int UtcDaliVisualGetPropertyMap10(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMap10: TextVisual" );

  //Request PrimitiveVisual using a property map.
  VisualFactory factory = VisualFactory::Get();

  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::TEXT );
  propertyMap.Insert( "renderingBackend", static_cast<int>( Toolkit::Text::DEFAULT_RENDERING_BACKEND ) );
  propertyMap.Insert( "text", "Hello world" );
  propertyMap.Insert( "fontFamily", "TizenSans" );

  Property::Map fontStyleMapSet;
  fontStyleMapSet.Insert( "weight", "bold" );
  propertyMap.Insert( "fontStyle", fontStyleMapSet );

  propertyMap.Insert( "pointSize", 12.f );
  propertyMap.Insert( "multiLine", true );
  propertyMap.Insert( "horizontalAlignment", "CENTER" );
  propertyMap.Insert( "verticalAlignment", "CENTER" );
  propertyMap.Insert( "textColor", Color::RED );
  propertyMap.Insert( "enableMarkup", false );
  propertyMap.Insert( "enableAutoScroll", false );
  propertyMap.Insert( "lineSpacing", 0.f );
  propertyMap.Insert( "batchingEnabled", false );
  Visual::Base textVisual = factory.CreateVisual( propertyMap );

  Property::Map resultMap;
  textVisual.CreatePropertyMap( resultMap );

  //Check values in the result map are identical to the initial map's values.
  Property::Value* value = resultMap.Find( Visual::Property::TYPE, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<int>(), (int)Visual::TEXT, TEST_LOCATION );

  value = resultMap.Find( TextVisual::Property::RENDERING_BACKEND, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<int>(), Toolkit::Text::DEFAULT_RENDERING_BACKEND, TEST_LOCATION );

  value = resultMap.Find( TextVisual::Property::TEXT, Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<std::string>(), "Hello world", TEST_LOCATION );

  value = resultMap.Find( TextVisual::Property::FONT_FAMILY, Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<std::string>(), "TizenSans", TEST_LOCATION );

  value = resultMap.Find( TextVisual::Property::FONT_STYLE, Property::MAP );
  DALI_TEST_CHECK( value );

  Property::Map fontStyleMapGet = value->Get<Property::Map>();
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  value = resultMap.Find( TextVisual::Property::POINT_SIZE, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), 12.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  value = resultMap.Find( TextVisual::Property::MULTI_LINE, Property::BOOLEAN );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<bool>() );

  value = resultMap.Find( TextVisual::Property::HORIZONTAL_ALIGNMENT, Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<std::string>(), "CENTER", TEST_LOCATION );

  value = resultMap.Find( TextVisual::Property::VERTICAL_ALIGNMENT, Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<std::string>(), "CENTER", TEST_LOCATION );

  value = resultMap.Find( TextVisual::Property::TEXT_COLOR, Property::VECTOR4 );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<Vector4>(), Color::RED, TEST_LOCATION );

  value = resultMap.Find( TextVisual::Property::ENABLE_MARKUP, Property::BOOLEAN );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( !value->Get<bool>() );

  value = resultMap.Find( TextVisual::Property::ENABLE_AUTO_SCROLL, Property::BOOLEAN );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( !value->Get<bool>() );

  value = resultMap.Find( TextVisual::Property::LINE_SPACING, Property::FLOAT );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<float>(), 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  value = resultMap.Find( TextVisual::Property::BATCHING_ENABLED, Property::BOOLEAN );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( !value->Get<bool>() );

  END_TEST;
}

int UtcDaliVisualGetPropertyMapBatchImageVisualNoAtlas(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetPropertyMapBatchImageVisualNoAtlas:" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::BATCHING_ENABLED, true );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME );

  // Set the desired size to be larger than the atlas limit of 1024x1024.
  propertyMap.Insert( ImageVisual::Property::DESIRED_WIDTH, 2048 );
  propertyMap.Insert( ImageVisual::Property::DESIRED_HEIGHT, 2048 );

  // Create the visual.
  Visual::Base batchImageVisual = factory.CreateVisual( propertyMap );

  DALI_TEST_CHECK( batchImageVisual );

  Actor actor = Actor::New();
  batchImageVisual.SetOnStage( actor );

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  END_TEST;
}

int UtcDaliVisualAnimateBorderVisual01(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimateBorderVisual Color" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::BORDER);
  propertyMap.Insert(BorderVisual::Property::COLOR,  Color::BLUE);
  propertyMap.Insert(BorderVisual::Property::SIZE,  5.f);
  Visual::Base borderVisual = factory.CreateVisual( propertyMap );

  Actor actor = Actor::New();
  actor.SetSize(2000, 2000);
  actor.SetParentOrigin(ParentOrigin::CENTER);
  Stage::GetCurrent().Add(actor);
  borderVisual.SetOnStage( actor );

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION);

  Renderer renderer = actor.GetRendererAt(0);
  Property::Index index = renderer.GetPropertyIndex( BorderVisual::Property::COLOR );

  Animation animation = Animation::New(4.0f);
  animation.AnimateTo( Property(renderer, index), Color::WHITE );
  animation.Play();

  application.SendNotification();
  application.Render(0);
  application.Render(2000u); // halfway point between blue and white

  Vector4 color = renderer.GetProperty<Vector4>( index );
  Vector4 testColor = (Color::BLUE + Color::WHITE)*0.5f;
  DALI_TEST_EQUALS( color, testColor, TEST_LOCATION );
  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<Vector4>("borderColor", testColor ), true, TEST_LOCATION );

  application.Render(2000u); // halfway point between blue and white

  color = renderer.GetProperty<Vector4>( index );
  DALI_TEST_EQUALS( color, Color::WHITE, TEST_LOCATION );
  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<Vector4>("borderColor", Color::WHITE ), true, TEST_LOCATION );

  END_TEST;
}


int UtcDaliVisualAnimateBorderVisual02(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimateBorderVisual Size" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::BORDER);
  propertyMap.Insert(BorderVisual::Property::COLOR,  Color::BLUE);
  propertyMap.Insert(BorderVisual::Property::SIZE,  5.f);
  Visual::Base borderVisual = factory.CreateVisual( propertyMap );

  Actor actor = Actor::New();
  actor.SetSize(2000, 2000);
  actor.SetParentOrigin(ParentOrigin::CENTER);
  Stage::GetCurrent().Add(actor);
  borderVisual.SetOnStage( actor );

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION);

  Renderer renderer = actor.GetRendererAt(0);
  Property::Index index = renderer.GetPropertyIndex( BorderVisual::Property::SIZE );

  Animation animation = Animation::New(4.0f);
  animation.AnimateTo( Property(renderer, index), 9.0f );
  animation.Play();

  application.SendNotification();
  application.Render(0);
  application.Render(2000u); // halfway point

  float size = renderer.GetProperty<float>( index );
  DALI_TEST_EQUALS( size, 7.0f, 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<float>("borderSize", 7.0f ), true, TEST_LOCATION );

  application.Render(2000u); // halfway point between blue and white

  size = renderer.GetProperty<float>( index );
  DALI_TEST_EQUALS( size, 9.0f, 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<float>("borderSize", 9.0f ), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualAnimateColorVisual(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimateColorVisual mixColor" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
  Visual::Base borderVisual = factory.CreateVisual( propertyMap );

  Actor actor = Actor::New();
  actor.SetSize(2000, 2000);
  actor.SetParentOrigin(ParentOrigin::CENTER);
  Stage::GetCurrent().Add(actor);
  borderVisual.SetOnStage( actor );

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION);

  Renderer renderer = actor.GetRendererAt(0);
  Property::Index index = renderer.GetPropertyIndex( ColorVisual::Property::MIX_COLOR );

  Animation animation = Animation::New(4.0f);
  animation.AnimateTo( Property(renderer, index), Color::WHITE );
  animation.Play();

  application.SendNotification();
  application.Render(0);
  application.Render(2000u); // halfway point

  Vector4 color = renderer.GetProperty<Vector4>( index );
  Vector4 testColor = (Color::BLUE + Color::WHITE)*0.5f;
  DALI_TEST_EQUALS( color, testColor, TEST_LOCATION );

  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<Vector4>("mixColor", testColor ), true, TEST_LOCATION );

  application.Render(2000u); // halfway point between blue and white

  color = renderer.GetProperty<Vector4>( index );
  DALI_TEST_EQUALS( color, Color::WHITE, TEST_LOCATION );

  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<Vector4>("mixColor", Color::WHITE ), true, TEST_LOCATION );


  END_TEST;
}


int UtcDaliVisualAnimatePrimitiveVisual(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatePrimitiveVisual color" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
  Visual::Base borderVisual = factory.CreateVisual( propertyMap );

  Actor actor = Actor::New();
  actor.SetSize(2000, 2000);
  actor.SetParentOrigin(ParentOrigin::CENTER);
  actor.SetColor(Color::BLACK);
  Stage::GetCurrent().Add(actor);
  borderVisual.SetOnStage( actor );

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION);

  Renderer renderer = actor.GetRendererAt(0);
  Property::Index index = renderer.GetPropertyIndex( PrimitiveVisual::Property::MIX_COLOR );

  // The property isn't registered on the renderer, it's instead registered on the shader.
  DALI_TEST_EQUALS( index, Property::INVALID_INDEX, TEST_LOCATION );

  Animation animation = Animation::New(4.0f);
  animation.AnimateTo( Property(actor, Actor::Property::COLOR), Color::WHITE );
  animation.Play();

  application.SendNotification();
  application.Render(0);
  application.Render(2000u); // halfway point

  // Actor color overrides renderer color.
  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<Vector4>("uColor", Vector4(0.5f, 0.5f, 0.5f, 1.0f )), true, TEST_LOCATION );

  application.Render(2000u); // halfway point between blue and white

  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::WHITE, TEST_LOCATION );
  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<Vector4>("uColor", Color::WHITE ), true, TEST_LOCATION );


  END_TEST;
}

int UtcDaliVisualWireframeVisual(void)
{
  ToolkitTestApplication application;

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::WIREFRAME );

  // Create the visual.
  Visual::Base visual = factory.CreateVisual( propertyMap );

  DALI_TEST_CHECK( visual );

  Property::Map resultMap;
  visual.CreatePropertyMap( resultMap );

  // Check the property values from the returned map from visual
  Property::Value* value = resultMap.Find( Visual::Property::TYPE, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == Visual::WIREFRAME );

  // Test the properties. TODO: to be completed.
  Property::Value primitiveValue = visual.GetProperty( Visual::Property::TYPE );

  END_TEST;
}

int UtcDaliVisualGetTransform(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualGetTransform: ColorVisual" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR,  Color::BLUE);
  Visual::Base colorVisual = factory.CreateVisual( propertyMap );

  Dali::Property::Map map;
  Dali::Property::Value transformProperty = colorVisual.GetProperty( Dali::Toolkit::Visual::DevelProperty::TRANSFORM );
  transformProperty.Get(map);

  //Test default values
  {
    Property::Value* typeValue = map.Find( Toolkit::Visual::DevelProperty::Transform::Property::OFFSET );
    DALI_TEST_CHECK( typeValue );
    DALI_TEST_CHECK( typeValue->Get<Vector2>() == Vector2(0.0f,0.0f) );
  }
  {
    Property::Value* typeValue = map.Find( Toolkit::Visual::DevelProperty::Transform::Property::SIZE );
    DALI_TEST_CHECK( typeValue );
    DALI_TEST_CHECK( typeValue->Get<Vector2>() == Vector2(1.0f,1.0f) );
  }
  {
    Property::Value* typeValue = map.Find( Toolkit::Visual::DevelProperty::Transform::Property::OFFSET_SIZE_MODE );
    DALI_TEST_CHECK( typeValue );
    DALI_TEST_CHECK( typeValue->Get<Vector4>() == Vector4(0.0f,0.0f,0.0f,0.0f) );
  }
  {
    Property::Value* typeValue = map.Find( Toolkit::Visual::DevelProperty::Transform::Property::ORIGIN );
    DALI_TEST_CHECK( typeValue );
    DALI_TEST_CHECK( (Toolkit::Align::Type)typeValue->Get<int>() == Toolkit::Align::CENTER );
  }
  {
    Property::Value* typeValue = map.Find( Toolkit::Visual::DevelProperty::Transform::Property::ANCHOR_POINT );
    DALI_TEST_CHECK( typeValue );
    DALI_TEST_CHECK( (Toolkit::Align::Type)typeValue->Get<int>() == Toolkit::Align::CENTER );
  }

  END_TEST;
}

static void TestTransform( ToolkitTestApplication& application, Visual::Base visual )
{
  Property::Map transform;
  transform.Insert( Visual::DevelProperty::Transform::Property::OFFSET, Vector2(10.0f, 10.0f) );
  transform.Insert( Visual::DevelProperty::Transform::Property::SIZE, Vector2(0.2f, 0.2f) );
  transform.Insert( Visual::DevelProperty::Transform::Property::OFFSET_SIZE_MODE, Vector4(1.0f, 1.0f, 0.0f,0.0f) );
  transform.Insert( Visual::DevelProperty::Transform::Property::ORIGIN, "TOP_BEGIN" );
  transform.Insert( Visual::DevelProperty::Transform::Property::ANCHOR_POINT, Toolkit::Align::BOTTOM_END );

  visual.SetProperty( Dali::Toolkit::Visual::DevelProperty::TRANSFORM, transform );

  Dali::Property::Map map;
  Dali::Property::Value transformProperty = visual.GetProperty( Dali::Toolkit::Visual::DevelProperty::TRANSFORM );
  transformProperty.Get(map);

  {
    Property::Value* typeValue = map.Find( Toolkit::Visual::DevelProperty::Transform::Property::OFFSET );
    DALI_TEST_CHECK( typeValue );
    DALI_TEST_EQUALS( typeValue->Get<Vector2>(),Vector2(10.0f,10.0f), TEST_LOCATION );
  }
  {
    Property::Value* typeValue = map.Find( Toolkit::Visual::DevelProperty::Transform::Property::SIZE );
    DALI_TEST_CHECK( typeValue );
    DALI_TEST_EQUALS( typeValue->Get<Vector2>(), Vector2(0.2f,0.2f), TEST_LOCATION );
  }
  {
    Property::Value* typeValue = map.Find( Toolkit::Visual::DevelProperty::Transform::Property::OFFSET_SIZE_MODE );
    DALI_TEST_CHECK( typeValue );
    DALI_TEST_EQUALS( typeValue->Get<Vector4>(), Vector4(1.0f,1.0f,0.0f,0.0f), TEST_LOCATION );
  }
  {
    Property::Value* typeValue = map.Find( Toolkit::Visual::DevelProperty::Transform::Property::ORIGIN );
    DALI_TEST_CHECK( typeValue );
    DALI_TEST_EQUALS( (Toolkit::Align::Type)typeValue->Get<int>(), Toolkit::Align::TOP_BEGIN, TEST_LOCATION );
  }
  {
    Property::Value* typeValue = map.Find( Toolkit::Visual::DevelProperty::Transform::Property::ANCHOR_POINT );
    DALI_TEST_CHECK( typeValue );
    DALI_TEST_EQUALS( (Toolkit::Align::Type)typeValue->Get<int>(), Toolkit::Align::BOTTOM_END, TEST_LOCATION );
  }

  //Put the visual on the stage
  Actor actor = Actor::New();
  actor.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor );
  visual.SetOnStage( actor );
  application.SendNotification();
  application.Render(0);
  Renderer renderer( actor.GetRendererAt(0) );

  //Check that the properties have been registered on the Renderer
  Vector2 offset = renderer.GetProperty<Vector2>( renderer.GetPropertyIndex( "offset" ) );
  DALI_TEST_EQUALS( offset, Vector2(10.0f,10.0f), TEST_LOCATION );

  Vector2 size = renderer.GetProperty<Vector2>( renderer.GetPropertyIndex( "size" ) );
  DALI_TEST_EQUALS( size, Vector2(0.2f,0.2f), TEST_LOCATION );

  Vector4 offsetSizeMode = renderer.GetProperty<Vector4>( renderer.GetPropertyIndex( "offsetSizeMode" ) );
  DALI_TEST_EQUALS( offsetSizeMode, Vector4(1.0f,1.0f,0.0f,0.0f), TEST_LOCATION );

  Vector2 parentOrigin = renderer.GetProperty<Vector2>( renderer.GetPropertyIndex( "origin" ) );
  DALI_TEST_EQUALS( parentOrigin, Vector2(-0.5f,-0.5f), TEST_LOCATION );

  Vector2 anchorPoint = renderer.GetProperty<Vector2>( renderer.GetPropertyIndex( "anchorPoint" ) );
  DALI_TEST_EQUALS( anchorPoint, Vector2(-0.5f,-0.5f), TEST_LOCATION );

  //Set a new transform
  transform.Clear();
  transform.Insert( Visual::DevelProperty::Transform::Property::OFFSET, Vector2(20.0f, 20.0f) );
  transform.Insert( Visual::DevelProperty::Transform::Property::SIZE, Vector2(100.0f, 100.0f) );
  transform.Insert( Visual::DevelProperty::Transform::Property::OFFSET_SIZE_MODE, Vector4(0.0f, 0.0f, 1.0f,1.0f) );
  visual.SetProperty( Dali::Toolkit::Visual::DevelProperty::TRANSFORM, transform );
  application.SendNotification();
  application.Render(0);

  //Check that the values have changed in the renderer
  offset = renderer.GetProperty<Vector2>( renderer.GetPropertyIndex( "offset" ) );
  DALI_TEST_EQUALS( offset, Vector2(20.0f,20.0f), TEST_LOCATION );

  size = renderer.GetProperty<Vector2>( renderer.GetPropertyIndex( "size" ) );
  DALI_TEST_EQUALS( size, Vector2(100.0f,100.0f), TEST_LOCATION );

  offsetSizeMode = renderer.GetProperty<Vector4>( renderer.GetPropertyIndex( "offsetSizeMode" ) );
  DALI_TEST_EQUALS( offsetSizeMode, Vector4(0.0f,0.0f,1.0f,1.0f), TEST_LOCATION );

  //Parent origin and anchor point should have default values
  parentOrigin = renderer.GetProperty<Vector2>( renderer.GetPropertyIndex( "origin" ) );
  DALI_TEST_EQUALS( parentOrigin, Vector2(0.0f,0.0f), TEST_LOCATION );

  anchorPoint = renderer.GetProperty<Vector2>( renderer.GetPropertyIndex( "anchorPoint" ) );
  DALI_TEST_EQUALS( anchorPoint, Vector2(0.0f,0.0f), TEST_LOCATION );
}

int UtcDaliVisualSetTransform0(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualSetTransform: ColorVisual" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR,  Color::BLUE);
  Visual::Base visual = factory.CreateVisual( propertyMap );
  TestTransform( application, visual );

  END_TEST;
}

int UtcDaliVisualSetTransform1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualSetTransform: PrimitiveVisual" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap[ Visual::Property::TYPE           ] = Visual::PRIMITIVE;
  propertyMap[ PrimitiveVisual::Property::MIX_COLOR ] = Vector4(1.0f,1.0f,1.0f,1.0f);
  propertyMap[ PrimitiveVisual::Property::SHAPE  ] = PrimitiveVisual::Shape::SPHERE;
  propertyMap[ PrimitiveVisual::Property::SLICES ] = 10;
  propertyMap[ PrimitiveVisual::Property::STACKS ] = 10;
  Visual::Base visual = factory.CreateVisual( propertyMap );
  TestTransform( application, visual );

  END_TEST;
}

int UtcDaliVisualSetTransform2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualSetTransform: GradientVisual" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE,  Visual::GRADIENT );
  Property::Array stopOffsets;
  stopOffsets.PushBack( 0.0f );
  stopOffsets.PushBack( 0.3f );
  stopOffsets.PushBack( 0.6f );
  stopOffsets.PushBack( 0.8f );
  stopOffsets.PushBack( 1.0f );
  propertyMap.Insert( GradientVisual::Property::STOP_OFFSET, stopOffsets );

  Property::Array stopColors;
  stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 255.f )/255.f );
  stopColors.PushBack( Vector4( 196.f, 198.f, 71.f, 122.f )/255.f );
  stopColors.PushBack( Vector4( 214.f, 37.f, 139.f, 191.f )/255.f );
  stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 150.f )/255.f );
  stopColors.PushBack( Color::YELLOW );
  propertyMap.Insert( GradientVisual::Property::STOP_COLOR, stopColors );
  propertyMap.Insert( GradientVisual::Property::CENTER, Vector2( 0.5f, 0.5f ) );
  propertyMap.Insert( GradientVisual::Property::RADIUS, 1.414f );
  Visual::Base visual = factory.CreateVisual( propertyMap );
  TestTransform( application, visual );

  END_TEST;
}

int UtcDaliVisualSetTransform3(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualSetTransform: BorderVisual" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::BORDER );
  propertyMap.Insert( BorderVisual::Property::COLOR, Vector4(0.f, 1.f, 0.f, 0.6f) );
  propertyMap.Insert( BorderVisual::Property::SIZE, 3.0f );
  Visual::Base visual = factory.CreateVisual( propertyMap );
  TestTransform( application, visual );

  END_TEST;
}

int UtcDaliVisualSetTransform4(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualSetTransform: MeshVisual" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::MESH );
  propertyMap.Insert( MeshVisual::Property::OBJECT_URL, TEST_OBJ_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::MATERIAL_URL, TEST_MTL_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::TEXTURES_PATH, TEST_RESOURCE_LOCATION );
  propertyMap.Insert( MeshVisual::Property::SHADING_MODE, MeshVisual::ShadingMode::TEXTURELESS_WITH_DIFFUSE_LIGHTING );
  propertyMap.Insert( MeshVisual::Property::LIGHT_POSITION, Vector3( 5.0f, 10.0f, 15.0f) );
  Visual::Base visual = factory.CreateVisual( propertyMap );
  TestTransform( application, visual );

  END_TEST;
}

int UtcDaliVisualSetTransform5(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualSetTransform: ImageVisual from Image" );

  VisualFactory factory = VisualFactory::Get();
  Image image = ResourceImage::New(TEST_IMAGE_FILE_NAME, ImageDimensions(100, 200));
  Visual::Base visual = factory.CreateVisual(image);
  TestTransform( application, visual );

  END_TEST;
}

int UtcDaliVisualSetTransform6(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualSetTransform: ImageVisual for URL " );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap[Toolkit::Visual::Property::TYPE] = Toolkit::Visual::IMAGE;
  propertyMap[Toolkit::ImageVisual::Property::URL] = TEST_IMAGE_FILE_NAME;
  propertyMap[Toolkit::ImageVisual::Property::DESIRED_WIDTH] = 100.0f;
  propertyMap[Toolkit::ImageVisual::Property::DESIRED_HEIGHT] = 100.0f;
  propertyMap[Toolkit::ImageVisual::Property::FITTING_MODE] = FittingMode::SCALE_TO_FILL;
  propertyMap[Toolkit::ImageVisual::Property::SAMPLING_MODE] = SamplingMode::BOX_THEN_LINEAR;
  propertyMap[Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING] = true;
  Visual::Base visual = factory.CreateVisual(propertyMap);
  TestTransform( application, visual );

  END_TEST;
}

int UtcDaliVisualSetTransform7(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualSetTransform: NPatch visual" );

  VisualFactory factory = VisualFactory::Get();
  Image image = ResourceImage::New(TEST_NPATCH_FILE_NAME, ImageDimensions(100, 200));
  Visual::Base visual = factory.CreateVisual(image);
  TestTransform( application, visual );

  END_TEST;
}


