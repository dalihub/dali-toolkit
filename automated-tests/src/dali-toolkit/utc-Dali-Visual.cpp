/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/text/rendering-backend.h>
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/visuals/animated-gradient-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/arc-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/color-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/text-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/handle-devel.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <toolkit-event-thread-callback.h>

#include "dummy-control.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* TEST_GIF_FILE_NAME          = TEST_RESOURCE_DIR "/anim.gif";
const char* TEST_IMAGE_FILE_NAME        = TEST_RESOURCE_DIR "/gallery-small-1.jpg";
const char* TEST_NPATCH_FILE_NAME       = TEST_RESOURCE_DIR "/button-up.9.png";
const char* TEST_SVG_FILE_NAME          = TEST_RESOURCE_DIR "/svg1.svg";
const char* TEST_OBJ_FILE_NAME          = TEST_RESOURCE_DIR "/Cube.obj";
const char* TEST_MTL_FILE_NAME          = TEST_RESOURCE_DIR "/ToyRobot-Metal.mtl";
const char* TEST_VECTOR_IMAGE_FILE_NAME = TEST_RESOURCE_DIR "/insta_camera.json";
const char* TEST_RESOURCE_LOCATION      = TEST_RESOURCE_DIR "/";

const std::string DEFAULT_FONT_DIR("/resources/fonts");

Property::Map DefaultTransform()
{
  Property::Map transformMap;
  transformMap
    .Add(Toolkit::Visual::Transform::Property::OFFSET, Vector2(0.0f, 0.0f))
    .Add(Toolkit::Visual::Transform::Property::SIZE, Vector2(1.0f, 1.0f))
    .Add(Toolkit::Visual::Transform::Property::ORIGIN, Toolkit::Align::TOP_BEGIN)
    .Add(Toolkit::Visual::Transform::Property::ANCHOR_POINT, Toolkit::Align::TOP_BEGIN)
    .Add(Toolkit::Visual::Transform::Property::OFFSET_POLICY, Vector2(Toolkit::Visual::Transform::Policy::RELATIVE, Toolkit::Visual::Transform::Policy::RELATIVE))
    .Add(Toolkit::Visual::Transform::Property::SIZE_POLICY, Vector2(Toolkit::Visual::Transform::Policy::RELATIVE, Toolkit::Visual::Transform::Policy::RELATIVE));
  return transformMap;
}

bool DaliTestCheckMaps(const Property::Map& fontStyleMapGet, const Property::Map& fontStyleMapSet)
{
  if(fontStyleMapGet.Count() == fontStyleMapSet.Count())
  {
    for(unsigned int index = 0u; index < fontStyleMapGet.Count(); ++index)
    {
      const KeyValuePair& valueGet = fontStyleMapGet.GetKeyValue(index);

      Property::Value* valueSet = NULL;
      if(valueGet.first.type == Property::Key::INDEX)
      {
        valueSet = fontStyleMapSet.Find(valueGet.first.indexKey);
      }
      else
      {
        // Get Key is a string so searching Set Map for a string key
        valueSet = fontStyleMapSet.Find(valueGet.first.stringKey);
      }

      if(NULL != valueSet)
      {
        if(valueSet->GetType() == Dali::Property::STRING && (valueGet.second.Get<std::string>() != valueSet->Get<std::string>()))
        {
          tet_printf("Value got : [%s], expected : [%s]", valueGet.second.Get<std::string>().c_str(), valueSet->Get<std::string>().c_str());
          return false;
        }
        else if(valueSet->GetType() == Dali::Property::BOOLEAN && (valueGet.second.Get<bool>() != valueSet->Get<bool>()))
        {
          tet_printf("Value got : [%d], expected : [%d]", valueGet.second.Get<bool>(), valueSet->Get<bool>());
          return false;
        }
        else if(valueSet->GetType() == Dali::Property::INTEGER && (valueGet.second.Get<int>() != valueSet->Get<int>()))
        {
          tet_printf("Value got : [%d], expected : [%d]", valueGet.second.Get<int>(), valueSet->Get<int>());
          return false;
        }
        else if(valueSet->GetType() == Dali::Property::FLOAT && (valueGet.second.Get<float>() != valueSet->Get<float>()))
        {
          tet_printf("Value got : [%f], expected : [%f]", valueGet.second.Get<float>(), valueSet->Get<float>());
          return false;
        }
        else if(valueSet->GetType() == Dali::Property::VECTOR2 && (valueGet.second.Get<Vector2>() != valueSet->Get<Vector2>()))
        {
          Vector2 vector2Get = valueGet.second.Get<Vector2>();
          Vector2 vector2Set = valueSet->Get<Vector2>();
          tet_printf("Value got : [%f, %f], expected : [%f, %f]", vector2Get.x, vector2Get.y, vector2Set.x, vector2Set.y);
          return false;
        }
        else if(valueSet->GetType() == Dali::Property::VECTOR4 && (valueGet.second.Get<Vector4>() != valueSet->Get<Vector4>()))
        {
          Vector4 vector4Get = valueGet.second.Get<Vector4>();
          Vector4 vector4Set = valueSet->Get<Vector4>();
          tet_printf("Value got : [%f, %f, %f, %f], expected : [%f, %f, %f, %f]", vector4Get.r, vector4Get.g, vector4Get.b, vector4Get.a, vector4Set.r, vector4Set.g, vector4Set.b, vector4Set.a);
          return false;
        }
      }
      else
      {
        if(valueGet.first.type == Property::Key::INDEX)
        {
          tet_printf("  The key %d doesn't exist.", valueGet.first.indexKey);
        }
        else
        {
          tet_printf("  The key %s doesn't exist.", valueGet.first.stringKey.c_str());
        }
        return false;
      }
    }
  }

  return true;
}
void TestShaderCodeContainSubstrings(Control control, std::vector<std::pair<std::string, bool>> substringCheckList, const char* location)
{
  Renderer        renderer = control.GetRendererAt(0);
  Shader          shader   = renderer.GetShader();
  Property::Value value    = shader.GetProperty(Shader::Property::PROGRAM);
  Property::Map*  map      = value.GetMap();
  DALI_TEST_CHECK(map);

  Property::Value* vertex = map->Find("vertex"); // vertex key name from shader-impl.cpp
  std::string      vertexShader;
  DALI_TEST_CHECK(vertex->Get(vertexShader));
  for(const auto& checkPair : substringCheckList)
  {
    const auto& keyword = checkPair.first;
    const auto& expect  = checkPair.second;
    tet_printf("check [%s] %s exist in vertex shader\n", keyword.c_str(), expect ? "is" : "is not");
    DALI_TEST_EQUALS((vertexShader.find(keyword.c_str()) != std::string::npos), expect, location);
  }

  Property::Value* fragment = map->Find("fragment"); // fragment key name from shader-impl.cpp
  DALI_TEST_CHECK(fragment);
  std::string fragmentShader;
  DALI_TEST_CHECK(fragment->Get(fragmentShader));
  for(const auto& checkPair : substringCheckList)
  {
    const auto& keyword = checkPair.first;
    const auto& expect  = checkPair.second;
    tet_printf("check [%s] %s exist in fragment shader\n", keyword.c_str(), expect ? "is" : "is not");
    DALI_TEST_EQUALS((fragmentShader.find(keyword.c_str()) != std::string::npos), expect, location);
  }
}

void TestShaderCodeContainSubstringsForEachShader(Control control, std::vector<std::pair<std::string, std::pair<bool, bool>>> substringCheckList, const char* location)
{
  Renderer        renderer = control.GetRendererAt(0);
  Shader          shader   = renderer.GetShader();
  Property::Value value    = shader.GetProperty(Shader::Property::PROGRAM);
  Property::Map*  map      = value.GetMap();
  DALI_TEST_CHECK(map);

  Property::Value* vertex = map->Find("vertex"); // vertex key name from shader-impl.cpp
  std::string      vertexShader;
  DALI_TEST_CHECK(vertex->Get(vertexShader));
  for(const auto& checkPair : substringCheckList)
  {
    const auto& keyword = checkPair.first;
    const auto& expect  = checkPair.second.first;
    tet_printf("check [%s] %s exist in vertex shader\n", keyword.c_str(), expect ? "is" : "is not");
    DALI_TEST_EQUALS((vertexShader.find(keyword.c_str()) != std::string::npos), expect, location);
  }

  Property::Value* fragment = map->Find("fragment"); // fragment key name from shader-impl.cpp
  DALI_TEST_CHECK(fragment);
  std::string fragmentShader;
  DALI_TEST_CHECK(fragment->Get(fragmentShader));
  for(const auto& checkPair : substringCheckList)
  {
    const auto& keyword = checkPair.first;
    const auto& expect  = checkPair.second.second;
    tet_printf("check [%s] %s exist in fragment shader\n", keyword.c_str(), expect ? "is" : "is not");
    DALI_TEST_EQUALS((fragmentShader.find(keyword.c_str()) != std::string::npos), expect, location);
  }
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

static void TestMixColor(Visual::Base visual, Property::Index mixColorIndex, const Vector4& testColor)
{
  Property::Map map;
  visual.CreatePropertyMap(map);
  Property::Value* value = map.Find(mixColorIndex);
  DALI_TEST_CHECK(value);
  Vector3 mixColor1;
  DALI_TEST_CHECK(value->Get(mixColor1));
  DALI_TEST_EQUALS(mixColor1, Vector3(testColor), 0.001, TEST_LOCATION);

  value = map.Find(Visual::Property::MIX_COLOR);
  DALI_TEST_CHECK(value);
  Vector4 mixColor2;
  DALI_TEST_CHECK(value->Get(mixColor2));
  DALI_TEST_EQUALS(mixColor2, testColor, 0.001, TEST_LOCATION);

  value = map.Find(Visual::Property::OPACITY);
  DALI_TEST_CHECK(value);
  float opacity;
  DALI_TEST_CHECK(value->Get(opacity));
  DALI_TEST_EQUALS(opacity, testColor.a, 0.001, TEST_LOCATION);
}

int UtcDaliVisualCopyAndAssignment(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualCopyAndAssignment");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
  Visual::Base visual = factory.CreateVisual(propertyMap);

  Visual::Base visualCopy(visual);
  DALI_TEST_CHECK(visual == visualCopy);

  Visual::Base emptyVisual;
  Visual::Base emptyVisualCopy(emptyVisual);
  DALI_TEST_CHECK(emptyVisual == emptyVisualCopy);

  Visual::Base visualEquals;
  visualEquals = visual;
  DALI_TEST_CHECK(visual == visualEquals);

  Visual::Base emptyVisualEquals;
  emptyVisualEquals = emptyVisual;
  DALI_TEST_CHECK(emptyVisual == emptyVisualEquals);

  //self assignment
  visual = visual;
  DALI_TEST_CHECK(visual = visualCopy);

  END_TEST;
}

int UtcDaliVisualSetName01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualSetName");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
  Visual::Base visual = factory.CreateVisual(propertyMap);

  const char* visualName = "backgroundVisual";
  visual.SetName(visualName);

  DALI_TEST_EQUALS(visual.GetName(), visualName, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualSetGetDepthIndex(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualSetDepthIndex");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
  Visual::Base visual = factory.CreateVisual(propertyMap);

  visual.SetDepthIndex(1);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  dummyControl.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
  application.GetScene().Add(dummyControl);

  int depthIndex = dummyControl.GetRendererAt(0u).GetProperty<int>(Renderer::Property::DEPTH_INDEX);
  DALI_TEST_EQUALS(depthIndex, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetDepthIndex(), 1, TEST_LOCATION);

  visual.SetDepthIndex(-1);
  depthIndex = dummyControl.GetRendererAt(0u).GetProperty<int>(Renderer::Property::DEPTH_INDEX);
  DALI_TEST_EQUALS(depthIndex, -1, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetDepthIndex(), -1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualSize(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualSize");

  VisualFactory factory = VisualFactory::Get();
  Vector2       controlSize(20.f, 30.f);
  Vector2       naturalSize;

  // color colorVisual
  Dali::Property::Map map;
  map[Toolkit::Visual::Property::TYPE]  = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::MAGENTA;

  Visual::Base colorVisual = factory.CreateVisual(map);
  colorVisual.SetTransformAndSize(DefaultTransform(), controlSize);

  colorVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2::ZERO, TEST_LOCATION);

  // image visual
  map.Clear();
  map[Toolkit::Visual::Property::TYPE]                = Toolkit::Visual::IMAGE;
  map[Toolkit::ImageVisual::Property::URL]            = TEST_IMAGE_FILE_NAME;
  map[Toolkit::ImageVisual::Property::DESIRED_WIDTH]  = 100.0f;
  map[Toolkit::ImageVisual::Property::DESIRED_HEIGHT] = 200.0f;
  Visual::Base imageVisual                            = factory.CreateVisual(map);
  imageVisual.SetTransformAndSize(DefaultTransform(), controlSize);

  imageVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(100.f, 200.f), TEST_LOCATION);

  // n patch visual is tested in the utc-Dali-VisualFactory.cpp

  // border visual
  float borderSize = 5.f;
  map.Clear();
  map[Toolkit::Visual::Property::TYPE] = Visual::BORDER;
  map[BorderVisual::Property::COLOR]   = Color::RED;
  map[BorderVisual::Property::SIZE]    = borderSize;
  Visual::Base borderVisual            = factory.CreateVisual(map);
  borderVisual.SetTransformAndSize(DefaultTransform(), controlSize);
  borderVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2::ZERO, TEST_LOCATION);

  // gradient gradientVisual
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::GRADIENT);
  Vector2 start(-1.f, -1.f);
  Vector2 end(1.f, 1.f);
  propertyMap.Insert("mixColor", Color::MAGENTA);
  propertyMap.Insert(GradientVisual::Property::START_POSITION, start);
  propertyMap.Insert(GradientVisual::Property::END_POSITION, end);
  propertyMap.Insert(GradientVisual::Property::STOP_OFFSET, Vector2(0.f, 1.f));
  propertyMap.Insert(GradientVisual::Property::SPREAD_METHOD, GradientVisual::SpreadMethod::REPEAT);
  Property::Array stopColors;
  stopColors.PushBack(Color::RED);
  stopColors.PushBack(Color::GREEN);
  propertyMap.Insert(GradientVisual::Property::STOP_COLOR, stopColors);
  Visual::Base gradientVisual = factory.CreateVisual(propertyMap);
  gradientVisual.SetTransformAndSize(DefaultTransform(), controlSize);
  gradientVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2::ZERO, TEST_LOCATION);

  // animated gradient visual
  propertyMap.Clear();
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_GRADIENT);
  Visual::Base animatedGradientVisual = factory.CreateVisual(propertyMap);
  animatedGradientVisual.GetNaturalSize(naturalSize);
  animatedGradientVisual.SetTransformAndSize(DefaultTransform(), controlSize);
  DALI_TEST_EQUALS(naturalSize, Vector2::ZERO, TEST_LOCATION);

  // Text visual.

  // Load some fonts to get the same metrics on different platforms.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(96u, 96u);

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf");

  // Create a TextVisual with a font size of 12 first
  propertyMap.Clear();
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::TEXT);
  propertyMap.Insert(TextVisual::Property::ENABLE_MARKUP, true);
  propertyMap.Insert(TextVisual::Property::TEXT, "<font family='TizenSans' size='12'>Hello world</font>");
  propertyMap.Insert(TextVisual::Property::MULTI_LINE, true);

  Visual::Base smallTextVisual = factory.CreateVisual(propertyMap);
  Vector2      smallTextVisualNaturalSize;
  smallTextVisual.GetNaturalSize(smallTextVisualNaturalSize);

  // Then create a TextVisual with a font size of 20
  propertyMap[TextVisual::Property::TEXT] = "<font family='TizenSans' size='20'>Hello world</font>";
  Visual::Base largeTextVisual            = factory.CreateVisual(propertyMap);
  Vector2      largeTextVisualNaturalSize;
  largeTextVisual.GetNaturalSize(largeTextVisualNaturalSize);

  // Compare the sizes of the two text visuals, the second one should be bigger as it has a larger point size in the markup.
  DALI_TEST_CHECK(smallTextVisualNaturalSize.width < largeTextVisualNaturalSize.width &&
                  smallTextVisualNaturalSize.height < largeTextVisualNaturalSize.height);

  // The height returned for a particular width should also be greater for the large text visual
  DALI_TEST_CHECK(smallTextVisual.GetHeightForWidth(40.f) < largeTextVisual.GetHeightForWidth(40.f));

  //AnimatedImageVisual
  Visual::Base animatedImageVisual = factory.CreateVisual(TEST_GIF_FILE_NAME, ImageDimensions());
  animatedImageVisual.SetTransformAndSize(DefaultTransform(), controlSize);
  animatedImageVisual.GetNaturalSize(naturalSize);
  // TEST_GIF_FILE: anim.gif
  // resolution: 50*50, frame count: 4, frame delay: 0.2 second for each frame
  DALI_TEST_EQUALS(naturalSize, Vector2(50.f, 50.f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualSetOnOffScene(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualSetOnOffScene");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
  Visual::Base visual = factory.CreateVisual(propertyMap);

  DummyControl        actor     = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  actor.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(actor.GetRendererCount() == 0u);

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(actor.GetRendererCount() == 1u);

  application.GetScene().Remove(actor);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(actor.GetRendererCount() == 0u);

  END_TEST;
}

int UtcDaliVisualSetOnOffScene2(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualSetOnOffScene2");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::SVG);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_SVG_FILE_NAME);
  Visual::Base visual = factory.CreateVisual(propertyMap);

  DummyControl        actor     = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  actor.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(actor.GetRendererCount() == 0u);

  // First on/off
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(0);

  // Wait for loading & rasterization
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  DALI_TEST_CHECK(actor.GetRendererCount() == 1u);
  Renderer renderer = actor.GetRendererAt(0);
  auto     textures = renderer.GetTextures();
  DALI_TEST_CHECK(textures.GetTextureCount() != 0u);

  application.GetScene().Remove(actor);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(actor.GetRendererCount() == 0u);

  // Second on/off
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  DALI_TEST_CHECK(actor.GetRendererCount() == 1u);
  renderer = actor.GetRendererAt(0);
  textures = renderer.GetTextures();
  DALI_TEST_CHECK(textures.GetTextureCount() != 0u);

  application.GetScene().Remove(actor);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(actor.GetRendererCount() == 0u);

  END_TEST;
}

int UtcDaliVisualGetPropertyMap1(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetPropertyMap1: ColorVisual (With base MixColor");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(Visual::Property::MIX_COLOR, Color::BLUE);
  propertyMap.Insert(DevelVisual::Property::CORNER_RADIUS, 10.0f);
  propertyMap.Insert(DevelVisual::Property::CORNER_RADIUS_POLICY, Toolkit::Visual::Transform::Policy::RELATIVE);
  propertyMap.Insert(DevelVisual::Property::BORDERLINE_WIDTH, 20.0f);
  propertyMap.Insert(DevelVisual::Property::BORDERLINE_COLOR, Color::RED);
  propertyMap.Insert(DevelVisual::Property::BORDERLINE_OFFSET, -1.0f);
  propertyMap.Insert(DevelColorVisual::Property::BLUR_RADIUS, 20.0f);
  Visual::Base colorVisual = factory.CreateVisual(propertyMap);

  Property::Map resultMap;
  colorVisual.CreatePropertyMap(resultMap);

  Property::Value* typeValue = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(typeValue);
  DALI_TEST_CHECK(typeValue->Get<int>() == Visual::COLOR);

  Property::Value* colorValue = resultMap.Find(ColorVisual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(colorValue);
  DALI_TEST_CHECK(colorValue->Get<Vector4>() == Color::BLUE);

  Property::Value* cornerRadiusValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_CHECK(cornerRadiusValue->Get<Vector4>() == Vector4(10.0f, 10.0f, 10.0f, 10.0f));

  Property::Value* cornerRadiusPolicyValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS_POLICY, Property::INTEGER);
  DALI_TEST_CHECK(cornerRadiusPolicyValue);
  DALI_TEST_CHECK(cornerRadiusPolicyValue->Get<int>() == Toolkit::Visual::Transform::Policy::RELATIVE);

  Property::Value* borderlineWidthValue = resultMap.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(borderlineWidthValue);
  DALI_TEST_CHECK(borderlineWidthValue->Get<float>() == 20.0f);

  Property::Value* borderlineColorValue = resultMap.Find(DevelVisual::Property::BORDERLINE_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(borderlineColorValue);
  DALI_TEST_CHECK(borderlineColorValue->Get<Vector4>() == Color::RED);

  Property::Value* borderlineOffsetValue = resultMap.Find(DevelVisual::Property::BORDERLINE_OFFSET, Property::FLOAT);
  DALI_TEST_CHECK(borderlineOffsetValue);
  DALI_TEST_CHECK(borderlineOffsetValue->Get<float>() == -1.0f);

  Property::Value* blurRadiusValue = resultMap.Find(DevelColorVisual::Property::BLUR_RADIUS, Property::FLOAT);
  DALI_TEST_CHECK(blurRadiusValue);
  DALI_TEST_CHECK(blurRadiusValue->Get<float>() == 20.0f);

  // change the blend color
  propertyMap[ColorVisual::Property::MIX_COLOR] = Color::CYAN;
  colorVisual                                   = factory.CreateVisual(propertyMap);
  colorVisual.CreatePropertyMap(resultMap);

  colorValue = resultMap.Find(ColorVisual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(colorValue);
  DALI_TEST_CHECK(colorValue->Get<Vector4>() == Color::CYAN);

  // Test wrong values
  propertyMap[DevelColorVisual::Property::BLUR_RADIUS] = "3.0f";

  colorVisual = factory.CreateVisual(propertyMap);
  colorVisual.CreatePropertyMap(resultMap);

  blurRadiusValue = resultMap.Find(DevelColorVisual::Property::BLUR_RADIUS, Property::FLOAT);
  DALI_TEST_CHECK(blurRadiusValue);
  DALI_TEST_CHECK(blurRadiusValue->Get<float>() == 0.0f);

  END_TEST;
}

int UtcDaliVisualGetPropertyMap2(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetPropertyMap2: BorderVisual");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::BORDER);
  propertyMap.Insert("mixColor", Vector4(1.0f, 0.0f, 1.0f, 0.5f));
  propertyMap.Insert("borderColor", Color::BLUE);
  propertyMap.Insert("borderSize", 5.f);
  propertyMap.Insert("antiAliasing", true);
  Visual::Base borderVisual = factory.CreateVisual(propertyMap);

  Property::Map resultMap;
  borderVisual.CreatePropertyMap(resultMap);

  // check the property values from the returned map from visual
  Property::Value* typeValue = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(typeValue);
  DALI_TEST_CHECK(typeValue->Get<int>() == Visual::BORDER);

  Property::Value* colorValue = resultMap.Find(BorderVisual::Property::COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(colorValue);
  DALI_TEST_CHECK(colorValue->Get<Vector4>() == Color::BLUE);

  Property::Value* sizeValue = resultMap.Find(BorderVisual::Property::SIZE, Property::FLOAT);
  DALI_TEST_CHECK(sizeValue);
  DALI_TEST_CHECK(sizeValue->Get<float>() == 5.f);

  Property::Value* AAValue = resultMap.Find(BorderVisual::Property::ANTI_ALIASING, Property::BOOLEAN);
  DALI_TEST_CHECK(AAValue);
  DALI_TEST_CHECK(AAValue->Get<bool>() == true);

  Property::Map propertyMap1;
  propertyMap1[Toolkit::Visual::Property::TYPE] = Visual::BORDER;
  propertyMap1[BorderVisual::Property::COLOR]   = Color::CYAN;
  propertyMap1[BorderVisual::Property::SIZE]    = 10.0f;
  borderVisual                                  = factory.CreateVisual(propertyMap1);
  borderVisual.CreatePropertyMap(resultMap);

  typeValue = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(typeValue);
  DALI_TEST_CHECK(typeValue->Get<int>() == Visual::BORDER);

  colorValue = resultMap.Find(BorderVisual::Property::COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(colorValue);
  DALI_TEST_CHECK(colorValue->Get<Vector4>() == Color::CYAN);

  sizeValue = resultMap.Find(BorderVisual::Property::SIZE, Property::FLOAT);
  DALI_TEST_CHECK(sizeValue);
  DALI_TEST_CHECK(sizeValue->Get<float>() == 10.f);

  END_TEST;
}

int UtcDaliVisualGetPropertyMap2N(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetPropertyMap2N: BorderVisual with no setup properties");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::BORDER);
  Visual::Base borderVisual = factory.CreateVisual(propertyMap);

  tet_infoline("Test that the visual is created, with a default renderer");
  DALI_TEST_CHECK(borderVisual);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, borderVisual);
  application.GetScene().Add(dummyControl);

  DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualGetPropertyMap3(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetPropertyMap3: linear GradientVisual");

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK(factory);

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::GRADIENT);

  Vector2 start(-1.f, -1.f);
  Vector2 end(1.f, 1.f);
  propertyMap.Insert("startPosition", start);
  propertyMap.Insert("endPosition", end);
  propertyMap.Insert("spreadMethod", GradientVisual::SpreadMethod::REPEAT);

  propertyMap.Insert(GradientVisual::Property::STOP_OFFSET, Vector2(0.2f, 0.8f));

  Property::Array stopColors;
  stopColors.PushBack(Color::RED);
  stopColors.PushBack(Color::GREEN);
  propertyMap.Insert(GradientVisual::Property::STOP_COLOR, stopColors);

  float   borderlineWidth = 4.0f;
  Vector4 cornerRadius(7.0f, 10.0f, 13.0f, 16.0f);
  propertyMap.Insert(DevelVisual::Property::BORDERLINE_WIDTH, borderlineWidth);
  propertyMap.Insert(DevelVisual::Property::CORNER_RADIUS, cornerRadius);

  Visual::Base gradientVisual = factory.CreateVisual(propertyMap);

  Property::Map resultMap;
  gradientVisual.CreatePropertyMap(resultMap);

  // check the property values from the returned map from visual
  Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::GRADIENT);

  value = resultMap.Find(GradientVisual::Property::UNITS, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == GradientVisual::Units::OBJECT_BOUNDING_BOX);

  value = resultMap.Find(GradientVisual::Property::SPREAD_METHOD, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == GradientVisual::SpreadMethod::REPEAT);

  value = resultMap.Find(GradientVisual::Property::START_POSITION, Property::VECTOR2);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), start, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(GradientVisual::Property::END_POSITION, Property::VECTOR2);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), end, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), borderlineWidth, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), cornerRadius, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(GradientVisual::Property::STOP_OFFSET, Property::ARRAY);
  DALI_TEST_CHECK(value);
  Property::Array* offsetArray = value->GetArray();
  DALI_TEST_CHECK(offsetArray->Count() == 2);
  DALI_TEST_EQUALS(offsetArray->GetElementAt(0).Get<float>(), 0.2f, Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_EQUALS(offsetArray->GetElementAt(1).Get<float>(), 0.8f, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(GradientVisual::Property::STOP_COLOR, Property::ARRAY);
  DALI_TEST_CHECK(value);
  Property::Array* colorArray = value->GetArray();
  DALI_TEST_CHECK(colorArray->Count() == 2);
  DALI_TEST_EQUALS(colorArray->GetElementAt(0).Get<Vector4>(), Color::RED, Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_EQUALS(colorArray->GetElementAt(1).Get<Vector4>(), Color::GREEN, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualGetPropertyMap4(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetPropertyMap4: radial GradientVisual");

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK(factory);

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::GRADIENT);

  Vector2 center(100.f, 100.f);
  float   radius = 100.f;
  propertyMap.Insert(GradientVisual::Property::UNITS, GradientVisual::Units::USER_SPACE);
  propertyMap.Insert(GradientVisual::Property::CENTER, center);
  propertyMap.Insert(GradientVisual::Property::RADIUS, radius);
  propertyMap.Insert(GradientVisual::Property::STOP_OFFSET, Vector3(0.1f, 0.3f, 1.1f));

  Property::Array stopColors;
  stopColors.PushBack(Color::RED);
  stopColors.PushBack(Color::BLACK);
  stopColors.PushBack(Color::GREEN);
  propertyMap.Insert(GradientVisual::Property::STOP_COLOR, stopColors);

  float   borderlineWidth = 8.0f;
  Vector4 cornerRadius(1.0f, 2.0f, 4.0f, 8.0f);
  propertyMap.Insert(DevelVisual::Property::BORDERLINE_WIDTH, borderlineWidth);
  propertyMap.Insert(DevelVisual::Property::CORNER_RADIUS, cornerRadius);

  Visual::Base gradientVisual = factory.CreateVisual(propertyMap);
  DALI_TEST_CHECK(gradientVisual);

  Property::Map resultMap;
  gradientVisual.CreatePropertyMap(resultMap);

  // check the property values from the returned map from visual
  Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::GRADIENT);

  value = resultMap.Find(GradientVisual::Property::UNITS, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == GradientVisual::Units::USER_SPACE);

  value = resultMap.Find(GradientVisual::Property::SPREAD_METHOD, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == GradientVisual::SpreadMethod::PAD);

  value = resultMap.Find(GradientVisual::Property::CENTER, Property::VECTOR2);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), center, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(GradientVisual::Property::RADIUS, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), radius, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), borderlineWidth, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), cornerRadius, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(GradientVisual::Property::STOP_OFFSET, Property::ARRAY);
  DALI_TEST_CHECK(value);
  Property::Array* offsetArray = value->GetArray();
  DALI_TEST_CHECK(offsetArray->Count() == 3);
  DALI_TEST_EQUALS(offsetArray->GetElementAt(0).Get<float>(), 0.1f, Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_EQUALS(offsetArray->GetElementAt(1).Get<float>(), 0.3f, Math::MACHINE_EPSILON_100, TEST_LOCATION);
  // any stop value will be clamped to [0.0, 1.0];
  DALI_TEST_EQUALS(offsetArray->GetElementAt(2).Get<float>(), 1.0f, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(GradientVisual::Property::STOP_COLOR, Property::ARRAY);
  DALI_TEST_CHECK(value);
  Property::Array* colorArray = value->GetArray();
  DALI_TEST_CHECK(colorArray->Count() == 3);
  DALI_TEST_EQUALS(colorArray->GetElementAt(0).Get<Vector4>(), Color::RED, Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_EQUALS(colorArray->GetElementAt(1).Get<Vector4>(), Color::BLACK, Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_EQUALS(colorArray->GetElementAt(2).Get<Vector4>(), Color::GREEN, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualGetPropertyMap5(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetPropertyMap5: ImageVisual");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::IMAGE);
  propertyMap.Insert(Visual::Property::MIX_COLOR, Color::MAGENTA);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME);
  propertyMap.Insert(ImageVisual::Property::DESIRED_WIDTH, 20);
  propertyMap.Insert(ImageVisual::Property::DESIRED_HEIGHT, 30);
  propertyMap.Insert("fittingMode", FittingMode::FIT_HEIGHT);
  propertyMap.Insert("samplingMode", SamplingMode::BOX_THEN_NEAREST);
  propertyMap.Insert("pixelArea", Vector4(0.25f, 0.25f, 0.5f, 0.5f));
  propertyMap.Insert("wrapModeU", WrapMode::REPEAT);
  propertyMap.Insert("wrapModeV", WrapMode::MIRRORED_REPEAT);
  propertyMap.Insert("synchronousLoading", true);

  Visual::Base imageVisual = factory.CreateVisual(propertyMap);
  DALI_TEST_CHECK(imageVisual);

  Property::Map resultMap;
  imageVisual.CreatePropertyMap(resultMap);

  // check the property values from the returned map from visual
  Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::IMAGE);

  value = resultMap.Find(ImageVisual::Property::URL, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<std::string>() == TEST_IMAGE_FILE_NAME);

  value = resultMap.Find(Visual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<Vector4>() == Color::MAGENTA);

  value = resultMap.Find(ImageVisual::Property::FITTING_MODE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == FittingMode::FIT_HEIGHT);

  value = resultMap.Find(ImageVisual::Property::SAMPLING_MODE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == SamplingMode::BOX_THEN_NEAREST);

  value = resultMap.Find(ImageVisual::Property::DESIRED_WIDTH, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == 20);

  value = resultMap.Find(ImageVisual::Property::DESIRED_HEIGHT, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == 30);

  value = resultMap.Find(ImageVisual::Property::PIXEL_AREA, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), Vector4(0.25f, 0.25f, 0.5f, 0.5f), Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(ImageVisual::Property::WRAP_MODE_U, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == WrapMode::REPEAT);

  value = resultMap.Find(ImageVisual::Property::WRAP_MODE_V, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == WrapMode::MIRRORED_REPEAT);

  value = resultMap.Find(ImageVisual::Property::SYNCHRONOUS_LOADING, Property::BOOLEAN);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<bool>() == true);

  END_TEST;
}

int UtcDaliVisualGetPropertyMap6(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetPropertyMap6: NPatchVisual");

  Rect<int> border(1, 1, 1, 1);

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::N_PATCH);
  propertyMap.Insert("mixColor", Color::MAGENTA);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_NPATCH_FILE_NAME);
  propertyMap.Insert(ImageVisual::Property::BORDER_ONLY, true);
  propertyMap.Insert(ImageVisual::Property::BORDER, border);
  propertyMap.Insert(DevelImageVisual::Property::AUXILIARY_IMAGE, "application-icon-30.png");
  propertyMap.Insert(DevelImageVisual::Property::AUXILIARY_IMAGE_ALPHA, 0.9f);
  Visual::Base nPatchVisual = factory.CreateVisual(propertyMap);

  Property::Map resultMap;
  nPatchVisual.CreatePropertyMap(resultMap);

  // check the property values from the returned map from visual
  Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::N_PATCH);

  value = resultMap.Find(Visual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<Vector4>() == Color::MAGENTA);

  value = resultMap.Find(ImageVisual::Property::URL, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<std::string>() == TEST_NPATCH_FILE_NAME);

  value = resultMap.Find(ImageVisual::Property::BORDER_ONLY, Property::BOOLEAN);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<bool>());

  value = resultMap.Find(ImageVisual::Property::BORDER, Property::RECTANGLE);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<Rect<int>>() == border);

  value = resultMap.Find(DevelImageVisual::Property::AUXILIARY_IMAGE, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<std::string>() == "application-icon-30.png");

  value = resultMap.Find(DevelImageVisual::Property::AUXILIARY_IMAGE_ALPHA, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<float>() == 0.9f);

  Vector4 border1(1.0f, 1.0f, 1.0f, 1.0f);

  Property::Map propertyMap1;
  propertyMap1.Insert(Toolkit::Visual::Property::TYPE, Visual::N_PATCH);
  propertyMap1.Insert("mixColor", Color::MAGENTA);
  propertyMap1.Insert(ImageVisual::Property::URL, TEST_NPATCH_FILE_NAME);
  propertyMap1.Insert(ImageVisual::Property::BORDER_ONLY, true);
  propertyMap1.Insert(ImageVisual::Property::BORDER, border1);
  nPatchVisual = factory.CreateVisual(propertyMap1);

  nPatchVisual.CreatePropertyMap(resultMap);

  // check the property values from the returned map from visual
  value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::N_PATCH);

  value = resultMap.Find(Visual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<Vector4>() == Color::MAGENTA);

  value = resultMap.Find(ImageVisual::Property::URL, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<std::string>() == TEST_NPATCH_FILE_NAME);

  value = resultMap.Find(ImageVisual::Property::BORDER_ONLY, Property::BOOLEAN);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<bool>());

  value = resultMap.Find(ImageVisual::Property::BORDER, Property::RECTANGLE);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<Rect<int>>() == border);

  END_TEST;
}

int UtcDaliVisualGetPropertyMap7(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetPropertyMap7: SvgVisual");

  // request SvgVisual with a property map
  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::SVG);
  propertyMap.Insert(Visual::Property::MIX_COLOR, Color::WHITE);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_SVG_FILE_NAME);
  propertyMap.Insert(ImageVisual::Property::ATLASING, false);
  Visual::Base svgVisual = factory.CreateVisual(propertyMap);

  Property::Map resultMap;
  svgVisual.CreatePropertyMap(resultMap);
  // check the property values from the returned map from a visual
  Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::SVG);

  value = resultMap.Find(ImageVisual::Property::URL, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<std::string>() == TEST_SVG_FILE_NAME);

  value = resultMap.Find(ImageVisual::Property::ATLASING, Property::BOOLEAN);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<bool>() == false);

  // request SvgVisual with a property map 2
  propertyMap.Clear();
  propertyMap["visualType"] = Visual::SVG;
  propertyMap["mixColor"]   = Color::WHITE;
  propertyMap["url"]        = TEST_SVG_FILE_NAME;
  propertyMap["atlasing"]   = true;
  Visual::Base svgVisual1   = factory.CreateVisual(propertyMap);

  resultMap.Clear();
  svgVisual1.CreatePropertyMap(resultMap);
  // check the property values from the returned map from a visual
  value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::SVG);

  value = resultMap.Find(ImageVisual::Property::URL, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<std::string>() == TEST_SVG_FILE_NAME);

  value = resultMap.Find(ImageVisual::Property::ATLASING, Property::BOOLEAN);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<bool>() == true);

  // request SvgVisual with an URL
  Visual::Base svgVisual2 = factory.CreateVisual(TEST_SVG_FILE_NAME, ImageDimensions());
  resultMap.Clear();
  svgVisual2.CreatePropertyMap(resultMap);
  // check the property values from the returned map from a visual
  value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::SVG);

  value = resultMap.Find(ImageVisual::Property::URL, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<std::string>() == TEST_SVG_FILE_NAME);

  END_TEST;
}

//Mesh visual
int UtcDaliVisualGetPropertyMap8(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetPropertyMap8: MeshVisual");

  //Request MeshVisual using a property map.
  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::MESH);
  propertyMap.Insert(Visual::Property::MIX_COLOR, Color::BLUE);
  propertyMap.Insert(MeshVisual::Property::OBJECT_URL, TEST_OBJ_FILE_NAME);
  propertyMap.Insert(MeshVisual::Property::MATERIAL_URL, TEST_MTL_FILE_NAME);
  propertyMap.Insert(MeshVisual::Property::TEXTURES_PATH, TEST_RESOURCE_LOCATION);
  propertyMap.Insert(MeshVisual::Property::SHADING_MODE, MeshVisual::ShadingMode::TEXTURELESS_WITH_DIFFUSE_LIGHTING);
  propertyMap.Insert(MeshVisual::Property::LIGHT_POSITION, Vector3(5.0f, 10.0f, 15.0f));
  Visual::Base meshVisual = factory.CreateVisual(propertyMap);

  Property::Map resultMap;
  meshVisual.CreatePropertyMap(resultMap);
  TestMixColor(meshVisual, Visual::Property::MIX_COLOR, Color::BLUE);

  //Check values in the result map are identical to the initial map's values.
  Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), (int)Visual::MESH, TEST_LOCATION);

  value = resultMap.Find(MeshVisual::Property::OBJECT_URL, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<std::string>(), TEST_OBJ_FILE_NAME, TEST_LOCATION);

  value = resultMap.Find(MeshVisual::Property::MATERIAL_URL, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<std::string>(), TEST_MTL_FILE_NAME, TEST_LOCATION);

  value = resultMap.Find(MeshVisual::Property::TEXTURES_PATH, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<std::string>(), TEST_RESOURCE_LOCATION, TEST_LOCATION);

  value = resultMap.Find(MeshVisual::Property::SHADING_MODE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), (int)MeshVisual::ShadingMode::TEXTURELESS_WITH_DIFFUSE_LIGHTING, TEST_LOCATION);

  value = resultMap.Find(MeshVisual::Property::LIGHT_POSITION, Property::VECTOR3);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector3>(), Vector3(5.0f, 10.0f, 15.0f), Math::MACHINE_EPSILON_100, TEST_LOCATION);

  END_TEST;
}

//Primitive shape visual
int UtcDaliVisualGetPropertyMap9(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetPropertyMap9: PrimitiveVisual");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("mixColor", Property::Type::VECTOR3),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  Vector4 color      = Vector4(1.0, 0.8, 0.6, 1.0);
  Vector3 dimensions = Vector3(1.0, 2.0, 3.0);

  //Request PrimitiveVisual using a property map.
  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::PRIMITIVE);
  propertyMap.Insert(PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::CUBE);
  propertyMap.Insert(PrimitiveVisual::Property::MIX_COLOR, color);
  propertyMap.Insert(PrimitiveVisual::Property::SLICES, 10);
  propertyMap.Insert(PrimitiveVisual::Property::STACKS, 20);
  propertyMap.Insert(PrimitiveVisual::Property::SCALE_TOP_RADIUS, 30.0f);
  propertyMap.Insert(PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS, 40.0f);
  propertyMap.Insert(PrimitiveVisual::Property::SCALE_HEIGHT, 50.0f);
  propertyMap.Insert(PrimitiveVisual::Property::SCALE_RADIUS, 60.0f);
  propertyMap.Insert(PrimitiveVisual::Property::SCALE_DIMENSIONS, dimensions);
  propertyMap.Insert(PrimitiveVisual::Property::BEVEL_PERCENTAGE, 0.3f);
  propertyMap.Insert(PrimitiveVisual::Property::BEVEL_SMOOTHNESS, 0.6f);
  propertyMap.Insert(PrimitiveVisual::Property::LIGHT_POSITION, Vector3(5.0f, 10.0f, 15.0f));
  Visual::Base primitiveVisual = factory.CreateVisual(propertyMap);

  Property::Map resultMap;
  primitiveVisual.CreatePropertyMap(resultMap);

  //Check values in the result map are identical to the initial map's values.
  Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), (int)Visual::PRIMITIVE, TEST_LOCATION);

  value = resultMap.Find(PrimitiveVisual::Property::SHAPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), (int)PrimitiveVisual::Shape::CUBE, TEST_LOCATION);

  value = resultMap.Find(PrimitiveVisual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<Vector4>() == color);
  DALI_TEST_EQUALS(value->Get<Vector4>(), color, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(PrimitiveVisual::Property::SLICES, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), 10, TEST_LOCATION);

  value = resultMap.Find(PrimitiveVisual::Property::STACKS, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), 20, TEST_LOCATION);

  value = resultMap.Find(PrimitiveVisual::Property::SCALE_TOP_RADIUS, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 30.0f, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 40.0f, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(PrimitiveVisual::Property::SCALE_HEIGHT, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 50.0f, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(PrimitiveVisual::Property::SCALE_RADIUS, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 60.0f, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(PrimitiveVisual::Property::SCALE_DIMENSIONS, Property::VECTOR3);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector3>(), dimensions, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(PrimitiveVisual::Property::BEVEL_PERCENTAGE, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 0.3f, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(PrimitiveVisual::Property::BEVEL_SMOOTHNESS, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 0.6f, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(PrimitiveVisual::Property::LIGHT_POSITION, Property::VECTOR3);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector3>(), Vector3(5.0f, 10.0f, 15.0f), Math::MACHINE_EPSILON_100, TEST_LOCATION);

  DummyControl        actor     = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, primitiveVisual);
  actor.SetProperty(Actor::Property::SIZE, Vector2(2000.f, 2000.f));
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  application.GetScene().Add(actor);

  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(DevelControl::GetVisualProperty(actor, DummyControl::Property::TEST_VISUAL, PrimitiveVisual::Property::MIX_COLOR), Vector3(Color::MAGENTA));
  animation.Play();
  application.SendNotification();
  application.Render(0);
  application.Render(1000);
  application.SendNotification();

  auto& gl = application.GetGlAbstraction();
  DALI_TEST_EQUALS(gl.CheckUniformValue<Vector3>("mixColor", Vector3(Color::MAGENTA)), true, TEST_LOCATION);

  tet_infoline("Check property map after animation");

  primitiveVisual.CreatePropertyMap(resultMap);
  value = resultMap.Find(PrimitiveVisual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  color = value->Get<Vector4>();
  // Ignore alpha part
  DALI_TEST_EQUALS(Vector3(color), Vector3(Color::MAGENTA), 0.001f, TEST_LOCATION);

  END_TEST;
}

//Text shape visual
int UtcDaliVisualGetPropertyMap10(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetPropertyMap10: TextVisual");

  //Request PrimitiveVisual using a property map.
  VisualFactory factory = VisualFactory::Get();

  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::TEXT);
  propertyMap.Insert(Visual::Property::MIX_COLOR, Color::BLACK);
  propertyMap.Insert("renderingBackend", static_cast<int>(Toolkit::DevelText::DEFAULT_RENDERING_BACKEND));
  propertyMap.Insert("enableMarkup", false);
  propertyMap.Insert("text", "Hello world");
  propertyMap.Insert("fontFamily", "TizenSans");

  Property::Map fontStyleMapSet;
  fontStyleMapSet.Insert("weight", "bold");
  propertyMap.Insert("fontStyle", fontStyleMapSet);

  propertyMap.Insert("pointSize", 12.f);
  propertyMap.Insert("multiLine", true);
  propertyMap.Insert("horizontalAlignment", "CENTER");
  propertyMap.Insert("verticalAlignment", "CENTER");
  propertyMap.Insert("textColor", Color::RED);

  Property::Map shadowMapSet;
  propertyMap.Insert("shadow", shadowMapSet.Add("color", Color::RED).Add("offset", Vector2(2.0f, 2.0f)).Add("blurRadius", 3.0f));

  Property::Map underlineMapSet;
  propertyMap.Insert("underline", underlineMapSet.Add("enable", true).Add("color", Color::GREEN).Add("height", 1).Add("type", Text::Underline::Type::SOLID).Add("dashWidth", 2).Add("dashGap", 1));

  Property::Map outlineMapSet;
  propertyMap.Insert("outline", outlineMapSet.Add("color", Color::YELLOW).Add("width", 1).Add("offset", Vector2(2.0f, 2.0f)));

  Property::Map backgroundMapSet;
  propertyMap.Insert("textBackground", backgroundMapSet.Add("enable", true).Add("color", Color::CYAN));

  Visual::Base textVisual = factory.CreateVisual(propertyMap);

  Property::Map resultMap;
  textVisual.CreatePropertyMap(resultMap);

  //Check values in the result map are identical to the initial map's values.
  Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), (int)Visual::TEXT, TEST_LOCATION);

  value = resultMap.Find(Visual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), Color::BLACK, 0.001f, TEST_LOCATION);

  value = resultMap.Find(TextVisual::Property::TEXT, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<std::string>(), "Hello world", TEST_LOCATION);

  value = resultMap.Find(TextVisual::Property::FONT_FAMILY, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<std::string>(), "TizenSans", TEST_LOCATION);

  value = resultMap.Find(TextVisual::Property::FONT_STYLE, Property::MAP);
  DALI_TEST_CHECK(value);

  Property::Map fontStyleMapGet = value->Get<Property::Map>();
  DALI_TEST_EQUALS(fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(fontStyleMapGet, fontStyleMapSet), true, TEST_LOCATION);

  value = resultMap.Find(TextVisual::Property::POINT_SIZE, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 12.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  value = resultMap.Find(TextVisual::Property::MULTI_LINE, Property::BOOLEAN);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<bool>());

  value = resultMap.Find(TextVisual::Property::HORIZONTAL_ALIGNMENT, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), (int)Text::HorizontalAlignment::CENTER, TEST_LOCATION);

  value = resultMap.Find(TextVisual::Property::VERTICAL_ALIGNMENT, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), (int)Text::VerticalAlignment::CENTER, TEST_LOCATION);

  value = resultMap.Find(TextVisual::Property::TEXT_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), Color::RED, TEST_LOCATION);

  value = resultMap.Find(TextVisual::Property::ENABLE_MARKUP, Property::BOOLEAN);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(!value->Get<bool>());

  value = resultMap.Find(TextVisual::Property::SHADOW, Property::MAP);
  DALI_TEST_CHECK(value);

  Property::Map shadowMapGet = value->Get<Property::Map>();
  DALI_TEST_EQUALS(shadowMapGet.Count(), shadowMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(shadowMapGet, shadowMapSet), true, TEST_LOCATION);

  value = resultMap.Find(TextVisual::Property::UNDERLINE, Property::MAP);
  DALI_TEST_CHECK(value);

  Property::Map underlineMapGet = value->Get<Property::Map>();
  DALI_TEST_EQUALS(underlineMapGet.Count(), underlineMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(underlineMapGet, underlineMapSet), true, TEST_LOCATION);

  value = resultMap.Find(DevelTextVisual::Property::OUTLINE, Property::MAP);
  DALI_TEST_CHECK(value);

  Property::Map outlineMapGet = value->Get<Property::Map>();
  DALI_TEST_EQUALS(outlineMapGet.Count(), outlineMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(outlineMapGet, outlineMapSet), true, TEST_LOCATION);

  value = resultMap.Find(DevelTextVisual::Property::BACKGROUND, Property::MAP);
  DALI_TEST_CHECK(value);

  Property::Map backgroundMapGet = value->Get<Property::Map>();
  DALI_TEST_EQUALS(backgroundMapGet.Count(), backgroundMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(backgroundMapGet, backgroundMapSet), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualGetPropertyMap11(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetPropertyMap11: AnimatedGradientVisual");

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK(factory);

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, DevelVisual::ANIMATED_GRADIENT);

  Vector2 start(-0.5f, 0.5f);
  Vector2 end(0.5f, -0.0f);
  Vector4 start_color(1.0f, 0.7f, 0.5f, 1.0f);
  Vector4 end_color(0.7f, 0.5f, 1.0f, 1.0f);
  Vector2 rotate_center(0.0f, 0.4f);
  float   rotate_amount = 1.57f;
  float   offset        = 100.f;

  propertyMap.Insert(DevelAnimatedGradientVisual::Property::GRADIENT_TYPE, DevelAnimatedGradientVisual::GradientType::RADIAL);
  propertyMap.Insert(DevelAnimatedGradientVisual::Property::UNIT_TYPE, DevelAnimatedGradientVisual::UnitType::USER_SPACE);
  propertyMap.Insert(DevelAnimatedGradientVisual::Property::SPREAD_TYPE, DevelAnimatedGradientVisual::SpreadType::CLAMP);

  propertyMap.Insert(DevelAnimatedGradientVisual::Property::START_POSITION, start);
  propertyMap.Insert(DevelAnimatedGradientVisual::Property::END_POSITION, end);
  propertyMap.Insert(DevelAnimatedGradientVisual::Property::START_COLOR, start_color);
  propertyMap.Insert(DevelAnimatedGradientVisual::Property::END_COLOR, end_color);
  propertyMap.Insert(DevelAnimatedGradientVisual::Property::ROTATE_CENTER, rotate_center);
  propertyMap.Insert(DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT, rotate_amount);
  propertyMap.Insert(DevelAnimatedGradientVisual::Property::OFFSET, offset);

  Visual::Base animatedGradientVisual = factory.CreateVisual(propertyMap);
  DALI_TEST_CHECK(animatedGradientVisual);

  Property::Map resultMap;
  animatedGradientVisual.CreatePropertyMap(resultMap);

  // check the property values from the returned map from visual
  Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == DevelVisual::ANIMATED_GRADIENT);

  value = resultMap.Find(DevelAnimatedGradientVisual::Property::GRADIENT_TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == DevelAnimatedGradientVisual::GradientType::RADIAL);

  value = resultMap.Find(DevelAnimatedGradientVisual::Property::UNIT_TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == DevelAnimatedGradientVisual::UnitType::USER_SPACE);

  value = resultMap.Find(DevelAnimatedGradientVisual::Property::SPREAD_TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == DevelAnimatedGradientVisual::SpreadType::CLAMP);

  value = resultMap.Find(DevelAnimatedGradientVisual::Property::START_POSITION, Property::VECTOR2);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), start, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(DevelAnimatedGradientVisual::Property::END_POSITION, Property::VECTOR2);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), end, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(DevelAnimatedGradientVisual::Property::START_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), start_color, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(DevelAnimatedGradientVisual::Property::END_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), end_color, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(DevelAnimatedGradientVisual::Property::ROTATE_CENTER, Property::VECTOR2);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), rotate_center, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), rotate_amount, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  value = resultMap.Find(DevelAnimatedGradientVisual::Property::OFFSET, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), offset, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualGetPropertyMap12(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetPropertyMap12: AnimatedGradientVisual with animation param");

  // Case 1 : Set values by index
  {
    tet_printf(" - Set Values by Index\n");
    // NOTE : PropertyMap doesn't optimized even delay < -loop_count * (duration + repeat_delay) so this animation will not run
    // _delay = -10.0f is this case. It will progress (10.0f / 1.5f) amount. and 10.0f / 1.5f > 5.
    for(float _delay = -10.0f; _delay <= 5.0f; _delay += 5.0f)
    {
      tet_printf("test with delay [%f]\n", _delay);
      VisualFactory factory = VisualFactory::Get();
      DALI_TEST_CHECK(factory);

      Property::Map propertyMap;
      Property::Map animationMap;
      propertyMap.Insert(Visual::Property::TYPE, DevelVisual::ANIMATED_GRADIENT);

      float duration     = 1.1f;
      float delay        = _delay;
      float repeat_delay = 0.4f;

      int direction  = DevelAnimatedGradientVisual::AnimationParameter::DirectionType::BACKWARD;
      int loop_count = 5;
      int motion     = DevelAnimatedGradientVisual::AnimationParameter::MotionType::MIRROR;
      int easing     = DevelAnimatedGradientVisual::AnimationParameter::EasingType::OUT;

      auto buildAnimatedMap = [&animationMap, &direction, &duration, &delay, &loop_count, &repeat_delay, &motion, &easing](const Property::Value& start, const Property::Value& target) -> Property::Map& {
        animationMap.Clear();
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::START, start);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::TARGET, target);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::DIRECTION, direction);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::DURATION, duration);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::DELAY, delay);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT, loop_count);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT_DELAY, repeat_delay);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::MOTION_TYPE, motion);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::EASING_TYPE, easing);

        return animationMap;
      };

      Vector2 start1(-0.5f, 0.5f);
      Vector2 end1(0.5f, -0.5f);
      Vector4 start_color1(1.0f, 0.7f, 0.5f, 1.0f);
      Vector4 end_color1(0.7f, 0.5f, 1.0f, 1.0f);
      Vector2 rotate_center1(0.0f, 0.4f);
      float   rotate_amount1 = 0.0f;
      float   offset1        = 0.f;

      Vector2 start2(-0.5f, -0.5f);
      Vector2 end2(0.5f, 0.5f);
      Vector4 start_color2(0.0f, 0.1f, 0.8f, 1.0f);
      Vector4 end_color2(0.3f, 1.0f, 0.1f, 0.0f);
      Vector2 rotate_center2(0.0f, -0.4f);
      float   rotate_amount2 = 6.2832f;
      float   offset2        = 2.f;

      propertyMap.Insert(DevelAnimatedGradientVisual::Property::GRADIENT_TYPE, DevelAnimatedGradientVisual::GradientType::LINEAR);
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::UNIT_TYPE, DevelAnimatedGradientVisual::UnitType::OBJECT_BOUNDING_BOX);
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::SPREAD_TYPE, DevelAnimatedGradientVisual::SpreadType::REPEAT);

      propertyMap.Insert(DevelAnimatedGradientVisual::Property::START_POSITION, buildAnimatedMap(start1, start2));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::END_POSITION, buildAnimatedMap(end1, end2));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::START_COLOR, buildAnimatedMap(start_color1, start_color2));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::END_COLOR, buildAnimatedMap(end_color1, end_color2));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::ROTATE_CENTER, buildAnimatedMap(rotate_center1, rotate_center2));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT, buildAnimatedMap(rotate_amount1, rotate_amount2));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::OFFSET, buildAnimatedMap(offset1, offset2));

      Visual::Base animatedGradientVisual = factory.CreateVisual(propertyMap);
      DALI_TEST_CHECK(animatedGradientVisual);

      Property::Map resultMap;
      animatedGradientVisual.CreatePropertyMap(resultMap);

      // check the property values from the returned map from visual
      Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
      DALI_TEST_CHECK(value);
      DALI_TEST_CHECK(value->Get<int>() == DevelVisual::ANIMATED_GRADIENT);

      value = resultMap.Find(DevelAnimatedGradientVisual::Property::GRADIENT_TYPE, Property::INTEGER);
      DALI_TEST_CHECK(value);
      DALI_TEST_CHECK(value->Get<int>() == DevelAnimatedGradientVisual::GradientType::LINEAR);

      value = resultMap.Find(DevelAnimatedGradientVisual::Property::UNIT_TYPE, Property::INTEGER);
      DALI_TEST_CHECK(value);
      DALI_TEST_CHECK(value->Get<int>() == DevelAnimatedGradientVisual::UnitType::OBJECT_BOUNDING_BOX);

      value = resultMap.Find(DevelAnimatedGradientVisual::Property::SPREAD_TYPE, Property::INTEGER);
      DALI_TEST_CHECK(value);
      DALI_TEST_CHECK(value->Get<int>() == DevelAnimatedGradientVisual::SpreadType::REPEAT);

      auto checkAnimatedMap = [&value, &resultMap, &direction, &duration, &delay, &loop_count, &repeat_delay, &motion, &easing](const Property::Index& index, const Property::Value& start, const Property::Value& target, int line_num) -> void {
        tet_printf("Check value at %d\n", line_num);
        value = resultMap.Find(index, Property::MAP);
        DALI_TEST_CHECK(value);
        DALI_TEST_CHECK(value->GetType() == Property::MAP);
        Property::Map* temp_map = value->GetMap();
        DALI_TEST_CHECK(temp_map);

        auto checkMapValue = [&temp_map](const Property::Index index) -> Property::Value {
          Property::Value* res = temp_map->Find(index);
          DALI_TEST_CHECK(res);
          return *res;
        };

        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::START), start, Math::MACHINE_EPSILON_100, TEST_LOCATION);
        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::TARGET), target, Math::MACHINE_EPSILON_100, TEST_LOCATION);
        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::DIRECTION), Property::Value(direction), Math::MACHINE_EPSILON_100, TEST_LOCATION);
        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::DURATION), Property::Value(duration), Math::MACHINE_EPSILON_100, TEST_LOCATION);
        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::DELAY), Property::Value(delay), Math::MACHINE_EPSILON_100, TEST_LOCATION);
        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT), Property::Value(loop_count), Math::MACHINE_EPSILON_100, TEST_LOCATION);
        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT_DELAY), Property::Value(repeat_delay), Math::MACHINE_EPSILON_100, TEST_LOCATION);
        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::MOTION_TYPE), Property::Value(motion), Math::MACHINE_EPSILON_100, TEST_LOCATION);
        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::EASING_TYPE), Property::Value(easing), Math::MACHINE_EPSILON_100, TEST_LOCATION);
      };

      // check the animation map data is good
      checkAnimatedMap(DevelAnimatedGradientVisual::Property::START_POSITION, start1, start2, __LINE__);
      checkAnimatedMap(DevelAnimatedGradientVisual::Property::END_POSITION, end1, end2, __LINE__);
      checkAnimatedMap(DevelAnimatedGradientVisual::Property::START_COLOR, start_color1, start_color2, __LINE__);
      checkAnimatedMap(DevelAnimatedGradientVisual::Property::END_COLOR, end_color1, end_color2, __LINE__);
      checkAnimatedMap(DevelAnimatedGradientVisual::Property::ROTATE_CENTER, rotate_center1, rotate_center2, __LINE__);
      checkAnimatedMap(DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT, rotate_amount1, rotate_amount2, __LINE__);
      checkAnimatedMap(DevelAnimatedGradientVisual::Property::OFFSET, offset1, offset2, __LINE__);
    }
  }

  // Case 2 : Set values by string
  {
    tet_printf(" - Set Values by String\n");
    // NOTE : PropertyMap doesn't optimized even delay < -loop_count * (duration + repeat_delay) so this animation will not run
    // _delay = -10.0f is this case. It will progress (10.0f / 1.5f) amount. and 10.0f / 1.5f > 5.
    for(float _delay = -10.0f; _delay <= 5.0f; _delay += 5.0f)
    {
      tet_printf("test with delay [%f]\n", _delay);
      VisualFactory factory = VisualFactory::Get();
      DALI_TEST_CHECK(factory);

      Property::Map propertyMap;
      Property::Map animationMap;
      propertyMap.Insert("visualType", "ANIMATED_GRADIENT");

      float duration     = 1.1f;
      float delay        = _delay;
      float repeat_delay = 0.4f;

      int direction  = DevelAnimatedGradientVisual::AnimationParameter::DirectionType::BACKWARD;
      int loop_count = 5;
      int motion     = DevelAnimatedGradientVisual::AnimationParameter::MotionType::MIRROR;
      int easing     = DevelAnimatedGradientVisual::AnimationParameter::EasingType::IN_OUT;

      auto buildAnimatedMap = [&animationMap, &duration, &delay, &loop_count, &repeat_delay](const Property::Value& start, const Property::Value& target) -> Property::Map& {
        animationMap.Clear();
        animationMap.Insert("startValue", start);
        animationMap.Insert("targetValue", target);
        animationMap.Insert("directionType", "BACKWARD");
        animationMap.Insert("duration", duration);
        animationMap.Insert("delay", delay);
        animationMap.Insert("repeat", loop_count);
        animationMap.Insert("repeatDelay", repeat_delay);
        animationMap.Insert("motionType", "MIRROR");
        animationMap.Insert("easingType", "IN_OUT");

        return animationMap;
      };

      Vector2 start1(-0.5f, 0.5f);
      Vector2 end1(0.5f, -0.5f);
      Vector4 start_color1(1.0f, 0.7f, 0.5f, 1.0f);
      Vector4 end_color1(0.7f, 0.5f, 1.0f, 1.0f);
      Vector2 rotate_center1(0.0f, 0.4f);
      float   rotate_amount1 = 0.0f;
      float   offset1        = 0.f;

      Vector2 start2(-0.5f, -0.5f);
      Vector2 end2(0.5f, 0.5f);
      Vector4 start_color2(0.0f, 0.1f, 0.8f, 1.0f);
      Vector4 end_color2(0.3f, 1.0f, 0.1f, 0.0f);
      Vector2 rotate_center2(0.0f, -0.4f);
      float   rotate_amount2 = 6.2832f;
      float   offset2        = 2.f;

      // For test mix the type string/index key and string/index value works well.
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::GRADIENT_TYPE, "RADIAL");
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::UNIT_TYPE, DevelAnimatedGradientVisual::UnitType::USER_SPACE);
      propertyMap.Insert("spreadType", DevelAnimatedGradientVisual::SpreadType::REFLECT);

      propertyMap.Insert("startPosition", buildAnimatedMap(start1, start2));
      propertyMap.Insert("endPosition", buildAnimatedMap(end1, end2));
      propertyMap.Insert("startColor", buildAnimatedMap(start_color1, start_color2));
      propertyMap.Insert("endColor", buildAnimatedMap(end_color1, end_color2));
      propertyMap.Insert("rotateCenter", buildAnimatedMap(rotate_center1, rotate_center2));
      propertyMap.Insert("rotateAmount", buildAnimatedMap(rotate_amount1, rotate_amount2));
      propertyMap.Insert("offset", buildAnimatedMap(offset1, offset2));

      Visual::Base animatedGradientVisual = factory.CreateVisual(propertyMap);
      DALI_TEST_CHECK(animatedGradientVisual);

      Property::Map resultMap;
      animatedGradientVisual.CreatePropertyMap(resultMap);

      // check the property values from the returned map from visual
      // Note : resultMap from CreatePropertyMap only contain indexKey
      Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
      DALI_TEST_CHECK(value);
      DALI_TEST_CHECK(value->Get<int>() == DevelVisual::ANIMATED_GRADIENT);

      value = resultMap.Find(DevelAnimatedGradientVisual::Property::GRADIENT_TYPE, Property::INTEGER);
      DALI_TEST_CHECK(value);
      DALI_TEST_CHECK(value->Get<int>() == DevelAnimatedGradientVisual::GradientType::RADIAL);

      value = resultMap.Find(DevelAnimatedGradientVisual::Property::UNIT_TYPE, Property::INTEGER);
      DALI_TEST_CHECK(value);
      DALI_TEST_CHECK(value->Get<int>() == DevelAnimatedGradientVisual::UnitType::USER_SPACE);

      value = resultMap.Find(DevelAnimatedGradientVisual::Property::SPREAD_TYPE, Property::INTEGER);
      DALI_TEST_CHECK(value);
      DALI_TEST_CHECK(value->Get<int>() == DevelAnimatedGradientVisual::SpreadType::REFLECT);

      auto checkAnimatedMap = [&value, &resultMap, &direction, &duration, &delay, &loop_count, &repeat_delay, &motion, &easing](const Property::Index& index, const Property::Value& start, const Property::Value& target, int line_num) -> void {
        tet_printf("Check value at %d\n", line_num);
        value = resultMap.Find(index, Property::MAP);
        DALI_TEST_CHECK(value);
        DALI_TEST_CHECK(value->GetType() == Property::MAP);
        Property::Map* temp_map = value->GetMap();
        DALI_TEST_CHECK(temp_map);

        auto checkMapValue = [&temp_map](const Property::Index index) -> Property::Value {
          Property::Value* res = temp_map->Find(index);
          DALI_TEST_CHECK(res);
          return *res;
        };

        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::START), start, Math::MACHINE_EPSILON_100, TEST_LOCATION);
        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::TARGET), target, Math::MACHINE_EPSILON_100, TEST_LOCATION);
        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::DIRECTION), Property::Value(direction), Math::MACHINE_EPSILON_100, TEST_LOCATION);
        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::DURATION), Property::Value(duration), Math::MACHINE_EPSILON_100, TEST_LOCATION);
        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::DELAY), Property::Value(delay), Math::MACHINE_EPSILON_100, TEST_LOCATION);
        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT), Property::Value(loop_count), Math::MACHINE_EPSILON_100, TEST_LOCATION);
        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT_DELAY), Property::Value(repeat_delay), Math::MACHINE_EPSILON_100, TEST_LOCATION);
        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::MOTION_TYPE), Property::Value(motion), Math::MACHINE_EPSILON_100, TEST_LOCATION);
        DALI_TEST_EQUALS(checkMapValue(DevelAnimatedGradientVisual::AnimationParameter::Property::EASING_TYPE), Property::Value(easing), Math::MACHINE_EPSILON_100, TEST_LOCATION);
      };

      // check the animation map data is good
      checkAnimatedMap(DevelAnimatedGradientVisual::Property::START_POSITION, start1, start2, __LINE__);
      checkAnimatedMap(DevelAnimatedGradientVisual::Property::END_POSITION, end1, end2, __LINE__);
      checkAnimatedMap(DevelAnimatedGradientVisual::Property::START_COLOR, start_color1, start_color2, __LINE__);
      checkAnimatedMap(DevelAnimatedGradientVisual::Property::END_COLOR, end_color1, end_color2, __LINE__);
      checkAnimatedMap(DevelAnimatedGradientVisual::Property::ROTATE_CENTER, rotate_center1, rotate_center2, __LINE__);
      checkAnimatedMap(DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT, rotate_amount1, rotate_amount2, __LINE__);
      checkAnimatedMap(DevelAnimatedGradientVisual::Property::OFFSET, offset1, offset2, __LINE__);
    }
  }

  END_TEST;
}
int UtcDaliVisualGetPropertyMap13(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetPropertyMap13: AnimatedGradientVisual when repeat = 0");

  for(int _direction = 0; _direction <= 1; ++_direction)
  {
    for(float _delay = -10.0f; _delay <= 10.0f; _delay += 10.0f)
    {
      tet_printf(((_direction == 0) ? "Forward test with delay [%f]\n" : "Backward test with delay [%f]\n"), _delay);
      VisualFactory factory = VisualFactory::Get();
      DALI_TEST_CHECK(factory);

      Property::Map propertyMap;
      Property::Map animationMap;
      propertyMap.Insert(Visual::Property::TYPE, DevelVisual::ANIMATED_GRADIENT);

      float duration     = 1.0f;
      float delay        = _delay;
      float repeat_delay = 0.5f;

      int direction  = _direction;
      int loop_count = 0; // When loop_count is 0, Animation will not be created.
      int motion     = DevelAnimatedGradientVisual::AnimationParameter::MotionType::LOOP;
      int easing     = DevelAnimatedGradientVisual::AnimationParameter::EasingType::IN;

      auto buildAnimatedMap = [&animationMap, &direction, &duration, &delay, &loop_count, &repeat_delay, &motion, &easing](const Property::Value& start, const Property::Value& target) -> Property::Map& {
        animationMap.Clear();
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::START, start);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::TARGET, target);
        if(direction == 0)
          animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::DIRECTION, DevelAnimatedGradientVisual::AnimationParameter::DirectionType::FORWARD);
        else
          animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::DIRECTION, DevelAnimatedGradientVisual::AnimationParameter::DirectionType::BACKWARD);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::DIRECTION, direction);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::DURATION, duration);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::DELAY, delay);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT, loop_count);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT_DELAY, repeat_delay);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::MOTION_TYPE, motion);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::EASING_TYPE, easing);

        return animationMap;
      };

      Vector2 start1(-0.5f, 0.5f);
      Vector2 end1(0.5f, -0.5f);
      Vector4 start_color1(1.0f, 0.7f, 0.5f, 1.0f);
      Vector4 end_color1(0.7f, 0.5f, 1.0f, 1.0f);
      Vector2 rotate_center1(1.0f, 0.4f);
      float   rotate_amount1 = 2.0f;
      float   offset1        = 1.f;

      Vector2 start2(-0.5f, -0.5f);
      Vector2 end2(0.5f, 0.5f);
      Vector4 start_color2(0.0f, 0.1f, 0.8f, 1.0f);
      Vector4 end_color2(0.3f, 1.0f, 0.1f, 0.0f);
      Vector2 rotate_center2(1.0f, -0.4f);
      float   rotate_amount2 = 1.0f;
      float   offset2        = 3.f;

      propertyMap.Insert(DevelAnimatedGradientVisual::Property::GRADIENT_TYPE, DevelAnimatedGradientVisual::GradientType::LINEAR);
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::UNIT_TYPE, DevelAnimatedGradientVisual::UnitType::OBJECT_BOUNDING_BOX);
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::SPREAD_TYPE, DevelAnimatedGradientVisual::SpreadType::REFLECT);

      propertyMap.Insert(DevelAnimatedGradientVisual::Property::START_POSITION, buildAnimatedMap(start1, start2));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::END_POSITION, buildAnimatedMap(end1, end2));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::START_COLOR, buildAnimatedMap(start_color1, start_color2));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::END_COLOR, buildAnimatedMap(end_color1, end_color2));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::ROTATE_CENTER, buildAnimatedMap(rotate_center1, rotate_center2));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT, buildAnimatedMap(rotate_amount1, rotate_amount2));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::OFFSET, buildAnimatedMap(offset1, offset2));

      Visual::Base animatedGradientVisual = factory.CreateVisual(propertyMap);
      DALI_TEST_CHECK(animatedGradientVisual);

      Property::Map resultMap;
      animatedGradientVisual.CreatePropertyMap(resultMap);

      // check the property values from the returned map from visual
      Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
      DALI_TEST_CHECK(value);
      DALI_TEST_CHECK(value->Get<int>() == DevelVisual::ANIMATED_GRADIENT);

      value = resultMap.Find(DevelAnimatedGradientVisual::Property::GRADIENT_TYPE, Property::INTEGER);
      DALI_TEST_CHECK(value);
      DALI_TEST_CHECK(value->Get<int>() == DevelAnimatedGradientVisual::GradientType::LINEAR);

      value = resultMap.Find(DevelAnimatedGradientVisual::Property::UNIT_TYPE, Property::INTEGER);
      DALI_TEST_CHECK(value);
      DALI_TEST_CHECK(value->Get<int>() == DevelAnimatedGradientVisual::UnitType::OBJECT_BOUNDING_BOX);

      value = resultMap.Find(DevelAnimatedGradientVisual::Property::SPREAD_TYPE, Property::INTEGER);
      DALI_TEST_CHECK(value);
      DALI_TEST_CHECK(value->Get<int>() == DevelAnimatedGradientVisual::SpreadType::REFLECT);

      // If loop_count = 0, Animation doesn't created.
      // Optimized resultMap only have one value, which is target value
      // Note: target value will be changed by direction option.
      value = resultMap.Find(DevelAnimatedGradientVisual::Property::START_POSITION, Property::VECTOR2);
      DALI_TEST_CHECK(value);
      DALI_TEST_EQUALS(value->Get<Vector2>(), direction ? start1 : start2, Math::MACHINE_EPSILON_100, TEST_LOCATION);

      value = resultMap.Find(DevelAnimatedGradientVisual::Property::END_POSITION, Property::VECTOR2);
      DALI_TEST_CHECK(value);
      DALI_TEST_EQUALS(value->Get<Vector2>(), direction ? end1 : end2, Math::MACHINE_EPSILON_100, TEST_LOCATION);

      value = resultMap.Find(DevelAnimatedGradientVisual::Property::START_COLOR, Property::VECTOR4);
      DALI_TEST_CHECK(value);
      DALI_TEST_EQUALS(value->Get<Vector4>(), direction ? start_color1 : start_color2, Math::MACHINE_EPSILON_100, TEST_LOCATION);

      value = resultMap.Find(DevelAnimatedGradientVisual::Property::END_COLOR, Property::VECTOR4);
      DALI_TEST_CHECK(value);
      DALI_TEST_EQUALS(value->Get<Vector4>(), direction ? end_color1 : end_color2, Math::MACHINE_EPSILON_100, TEST_LOCATION);

      value = resultMap.Find(DevelAnimatedGradientVisual::Property::ROTATE_CENTER, Property::VECTOR2);
      DALI_TEST_CHECK(value);
      DALI_TEST_EQUALS(value->Get<Vector2>(), direction ? rotate_center1 : rotate_center2, Math::MACHINE_EPSILON_100, TEST_LOCATION);

      value = resultMap.Find(DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT, Property::FLOAT);
      DALI_TEST_CHECK(value);
      DALI_TEST_EQUALS(value->Get<float>(), direction ? rotate_amount1 : rotate_amount2, Math::MACHINE_EPSILON_100, TEST_LOCATION);

      value = resultMap.Find(DevelAnimatedGradientVisual::Property::OFFSET, Property::FLOAT);
      DALI_TEST_CHECK(value);
      DALI_TEST_EQUALS(value->Get<float>(), direction ? offset1 : offset2, Math::MACHINE_EPSILON_100, TEST_LOCATION);
    }
  }

  END_TEST;
}

int UtcDaliVisualAnimateArcVisual(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualAnimateArcVisual color");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("startAngle", Property::Type::FLOAT),
      UniformData("sweepAngle", Property::Type::FLOAT),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, DevelVisual::ARC);
  propertyMap.Insert(Visual::Property::MIX_COLOR, Color::BLUE);
  propertyMap.Insert(DevelArcVisual::Property::START_ANGLE, 0.0f);
  propertyMap.Insert(DevelArcVisual::Property::SWEEP_ANGLE, 90.0f);
  propertyMap.Insert(DevelArcVisual::Property::CAP, DevelArcVisual::Cap::ROUND);
  propertyMap.Insert(DevelArcVisual::Property::THICKNESS, 20.0f);
  Visual::Base arcVisual = factory.CreateVisual(propertyMap);

  DummyControl        actor     = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, arcVisual);
  actor.SetProperty(Actor::Property::SIZE, Vector2(2000.f, 2000.f));
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  application.GetScene().Add(actor);

  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);

  Renderer        renderer = actor.GetRendererAt(0);
  Property::Index index    = renderer.GetPropertyIndex(DevelArcVisual::Property::SWEEP_ANGLE);

  Animation animation = Animation::New(4.0f);
  animation.AnimateTo(Property(renderer, index), 50.0f);
  animation.AnimateTo(DevelControl::GetVisualProperty(actor, DummyControl::Property::TEST_VISUAL, DevelArcVisual::Property::START_ANGLE), 40.0f);
  animation.Play();

  application.SendNotification();
  application.Render(0);
  application.Render(2000u); // halfway point

  float sweepAngle = renderer.GetCurrentProperty<float>(index);
  DALI_TEST_EQUALS(sweepAngle, 70.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("startAngle", 20.0f), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("sweepAngle", 70.0f), true, TEST_LOCATION);

  application.Render(2000u); // another halfway point

  sweepAngle = renderer.GetCurrentProperty<float>(index);
  DALI_TEST_EQUALS(sweepAngle, 50.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("startAngle", 40.0f), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("sweepAngle", 50.0f), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualAnimateBorderVisual01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimateBorderVisual Color");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("borderColor", Property::Type::VECTOR4),
      UniformData("mixColor", Property::Type::VECTOR3),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::BORDER);
  propertyMap.Insert(Visual::Property::MIX_COLOR, Vector4(1, 1, 1, 0.8f));
  propertyMap.Insert(BorderVisual::Property::COLOR, Color::BLUE);
  propertyMap.Insert(BorderVisual::Property::SIZE, 5.f);
  Visual::Base borderVisual = factory.CreateVisual(propertyMap);

  Property::Map map;
  map["target"]      = "testVisual";
  map["property"]    = "mixColor";
  map["targetValue"] = Vector4(1, 1, 1, 0.1);
  map["animator"]    = Property::Map()
                      .Add("alphaFunction", "LINEAR")
                      .Add("timePeriod", Property::Map().Add("delay", 0.0f).Add("duration", 4.0f));

  Dali::Toolkit::TransitionData transition = TransitionData::New(map);

  DummyControl        actor     = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, borderVisual);
  actor.SetProperty(Actor::Property::SIZE, Vector2(2000.f, 2000.f));
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  application.GetScene().Add(actor);

  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);

  Renderer        renderer         = actor.GetRendererAt(0);
  Property::Index borderColorIndex = renderer.GetPropertyIndex(BorderVisual::Property::COLOR);
  Property::Index mixColorIndex    = VisualRenderer::Property::VISUAL_MIX_COLOR; //renderer.GetPropertyIndex( Visual::Property::MIX_COLOR );

  Animation animation = dummyImpl.CreateTransition(transition);

  // Animate the mix color through the transition, and the border color through
  // programmatic method.
  animation.AnimateTo(Property(renderer, borderColorIndex), Color::WHITE);
  animation.Play();

  application.SendNotification();
  application.Render(0);
  application.Render(2000u); // halfway point between blue and white

  Vector4 color     = renderer.GetCurrentProperty<Vector4>(borderColorIndex);
  Vector4 testColor = (Color::BLUE + Color::WHITE) * 0.5f;
  DALI_TEST_EQUALS(color, testColor, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("borderColor", testColor), true, TEST_LOCATION);

  color     = renderer.GetCurrentProperty<Vector3>(mixColorIndex);
  testColor = Vector4(1, 1, 1, 0.45f);
  DALI_TEST_EQUALS(Vector3(color), Vector3(testColor), 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector3>("mixColor", Vector3(testColor)), true, TEST_LOCATION);

  Vector4 uColor;
  DALI_TEST_CHECK(application.GetGlAbstraction().GetUniformValue<Vector4>("uColor", uColor));
  DALI_TEST_EQUALS(uColor.a, testColor.a, TEST_LOCATION);

  application.Render(2000u);

  color = renderer.GetCurrentProperty<Vector4>(borderColorIndex);
  DALI_TEST_EQUALS(color, Color::WHITE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("borderColor", Color::WHITE), true, TEST_LOCATION);

  color     = renderer.GetCurrentProperty<Vector3>(mixColorIndex);
  testColor = Vector4(1, 1, 1, 0.1);
  DALI_TEST_EQUALS(Vector3(color), Vector3(testColor), TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector3>("mixColor", Vector3(testColor)), true, TEST_LOCATION);

  DALI_TEST_CHECK(application.GetGlAbstraction().GetUniformValue<Vector4>("uColor", uColor));
  DALI_TEST_EQUALS(uColor.a, testColor.a, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualAnimateBorderVisual02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimateBorderVisual Size");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("borderSize", Property::Type::FLOAT),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::BORDER);
  propertyMap.Insert(BorderVisual::Property::COLOR, Color::BLUE);
  propertyMap.Insert(BorderVisual::Property::SIZE, 5.f);
  Visual::Base borderVisual = factory.CreateVisual(propertyMap);

  DummyControl        actor     = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, borderVisual);
  actor.SetProperty(Actor::Property::SIZE, Vector2(2000.f, 2000.f));
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  application.GetScene().Add(actor);

  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);

  Renderer        renderer = actor.GetRendererAt(0);
  Property::Index index    = renderer.GetPropertyIndex(BorderVisual::Property::SIZE);

  Animation animation = Animation::New(4.0f);
  animation.AnimateTo(Property(renderer, index), 9.0f);
  animation.Play();

  application.SendNotification();
  application.Render(0);
  application.Render(2000u); // halfway point

  float size = renderer.GetCurrentProperty<float>(index);
  DALI_TEST_EQUALS(size, 7.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderSize", 7.0f), true, TEST_LOCATION);

  application.Render(2000u); // halfway point between blue and white

  size = renderer.GetCurrentProperty<float>(index);
  DALI_TEST_EQUALS(size, 9.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderSize", 9.0f), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualAnimateColorVisual(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimateColorVisual mixColor");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("mixColor", Property::Type::VECTOR3),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
  Visual::Base borderVisual = factory.CreateVisual(propertyMap);

  DummyControl        actor     = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, borderVisual);
  actor.SetProperty(Actor::Property::SIZE, Vector2(2000.f, 2000.f));
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  application.GetScene().Add(actor);

  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);

  Renderer        renderer      = actor.GetRendererAt(0);
  Property::Index mixColorIndex = VisualRenderer::Property::VISUAL_MIX_COLOR; //renderer.GetPropertyIndex( ColorVisual::Property::MIX_COLOR );

  Property::Value blendModeValue = renderer.GetProperty(Renderer::Property::BLEND_MODE);
  DALI_TEST_EQUALS(blendModeValue.Get<int>(), (int)BlendMode::AUTO, TEST_LOCATION);

  Animation animation = Animation::New(4.0f);
  animation.AnimateTo(Property(renderer, mixColorIndex), Vector3(Color::WHITE));
  animation.Play();

  application.SendNotification();
  application.Render(0);
  application.Render(2000u); // halfway point

  Vector3 color     = renderer.GetCurrentProperty<Vector3>(mixColorIndex);
  Vector3 testColor = Vector3(Color::BLUE + Color::WHITE) * 0.5f;
  DALI_TEST_EQUALS(color, testColor, TEST_LOCATION);

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector3>("mixColor", testColor), true, TEST_LOCATION);

  application.Render(2000u); // halfway point between blue and white

  color = renderer.GetCurrentProperty<Vector3>(mixColorIndex);
  DALI_TEST_EQUALS(color, Vector3(Color::WHITE), TEST_LOCATION);

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector3>("mixColor", Vector3(Color::WHITE)), true, TEST_LOCATION);

  blendModeValue = renderer.GetCurrentProperty(Renderer::Property::BLEND_MODE);
  DALI_TEST_EQUALS(blendModeValue.Get<int>(), (int)BlendMode::AUTO, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualAnimatePrimitiveVisual(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatePrimitiveVisual color");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("mixColor", Property::Type::VECTOR3),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  {
    VisualFactory factory = VisualFactory::Get();
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::PRIMITIVE);
    propertyMap.Insert(PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::CUBE);
    propertyMap.Insert(PrimitiveVisual::Property::MIX_COLOR, Color::BLUE);
    Visual::Base visual = factory.CreateVisual(propertyMap);

    DummyControl        actor     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
    actor.SetProperty(Actor::Property::SIZE, Vector2(2000.f, 2000.f));
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    actor.SetProperty(Actor::Property::COLOR, Color::BLACK);
    application.GetScene().Add(actor);

    DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);

    Renderer renderer = actor.GetRendererAt(0);

    const Vector4 INITIAL_MIX_COLOR(1.0f, 0.0f, 1.0f, 0.5f); // Magenta with half alpha
    const Vector4 TARGET_MIX_COLOR(Color::RED);

    Property::Map map;
    map["target"]       = "testVisual";
    map["property"]     = "mixColor";
    map["initialValue"] = INITIAL_MIX_COLOR;
    map["targetValue"]  = TARGET_MIX_COLOR;
    map["animator"]     = Property::Map()
                        .Add("alphaFunction", "LINEAR")
                        .Add("timePeriod", Property::Map().Add("delay", 0.0f).Add("duration", 4.0f));

    Dali::Toolkit::TransitionData transition = TransitionData::New(map);

    Animation animation = dummyImpl.CreateTransition(transition);
    animation.AnimateTo(Property(actor, Actor::Property::COLOR), Color::WHITE);
    animation.Play();

    TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
    glAbstraction.EnableEnableDisableCallTrace(true);
    TraceCallStack&    glEnableStack = glAbstraction.GetEnableDisableTrace();
    std::ostringstream blendStr;
    blendStr << std::hex << GL_BLEND;

    application.SendNotification();
    application.Render(0);
    application.Render(2000u); // halfway point
    application.SendNotification();

    Vector4 halfwayColor = (INITIAL_MIX_COLOR + TARGET_MIX_COLOR) * 0.5;
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("uColor", Vector4(0.5f, 0.5f, 0.5f, halfwayColor.a)), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector3>("mixColor", Vector3(halfwayColor)), true, TEST_LOCATION);

    DALI_TEST_CHECK(glEnableStack.FindMethodAndParams("Enable", blendStr.str()));

    glEnableStack.Reset();

    application.Render(2001u);      // go past end
    application.SendNotification(); // Trigger signals

    DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR), Color::WHITE, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("uColor", Vector4(1.0f, 1.0f, 1.0f, TARGET_MIX_COLOR.a)), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector3>("mixColor", Vector3(TARGET_MIX_COLOR)), true, TEST_LOCATION);

    DALI_TEST_CHECK(glEnableStack.FindMethodAndParams("Disable", blendStr.str()));

    actor.Unparent();
  }

  END_TEST;
}

int UtcDaliVisualAnimatedGradientVisual01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedGradientVisual with default");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("start_point", Property::Type::VECTOR2),
      UniformData("end_point", Property::Type::VECTOR2),
      UniformData("start_color", Property::Type::VECTOR4),
      UniformData("end_color", Property::Type::VECTOR4),
      UniformData("rotate_center", Property::Type::VECTOR2),
      UniformData("rotate_angle", Property::Type::FLOAT),
      UniformData("gradient_offset", Property::Type::FLOAT),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  {
    VisualFactory factory = VisualFactory::Get();
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, DevelVisual::ANIMATED_GRADIENT);
    Visual::Base visual = factory.CreateVisual(propertyMap);

    DummyControl        actor     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
    actor.SetProperty(Actor::Property::SIZE, Vector2(2000.f, 2000.f));
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    actor.SetProperty(Actor::Property::COLOR, Color::BLACK);
    application.GetScene().Add(actor);

    application.SendNotification();
    application.Render(0);
    application.SendNotification();

    DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);

    for(int step_iter = 0; step_iter < 3; step_iter++)
    {
      application.SendNotification();
      application.Render(0);
      application.Render(750u); // step i/4
      application.SendNotification();

      DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector2>("start_point", Vector2(-0.5f, 0.0f)), true, TEST_LOCATION);
      DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector2>("end_point", Vector2(0.5f, 0.0f)), true, TEST_LOCATION);
      DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("start_color", Vector4(143.0f, 170.0f, 220.0f, 255.0f) / 255.0f), true, TEST_LOCATION);
      DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("end_color", Vector4(255.0f, 163.0f, 163.0f, 255.0f) / 255.0f), true, TEST_LOCATION);
      DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector2>("rotate_center", Vector2(0.0f, 0.0f)), true, TEST_LOCATION);
      DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("rotate_angle", 0.0f), true, TEST_LOCATION);
      DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("gradient_offset", 0.5f * step_iter + 0.5f), true, TEST_LOCATION);
    }

    //Not check here. cause gradient_offset value can be 2.0f or 0.0f
    application.Render(750u); // go to end
    application.SendNotification();

    application.Render(10u); // finish
    application.SendNotification();

    actor.Unparent();
    application.SendNotification();
    application.Render(0u);
    application.SendNotification();
  }

  END_TEST;
}

int UtcDaliVisualAnimatedGradientVisual02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedGradientVisual with full-option");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("start_point", Property::Type::VECTOR2),
      UniformData("end_point", Property::Type::VECTOR2),
      UniformData("start_color", Property::Type::VECTOR4),
      UniformData("end_color", Property::Type::VECTOR4),
      UniformData("rotate_center", Property::Type::VECTOR2),
      UniformData("rotate_angle", Property::Type::FLOAT),
      UniformData("gradient_offset", Property::Type::FLOAT),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  {
    float _delay[4]      = {0.0f, -1.35f, 0.15f, -0.4f}; // fract(_delay) must NOT be 1/4, 2/4, 3/4. cause we don't know progress is 1.0f or 0.0f
    int   _direction[2]  = {0, 1};
    int   _loop_count[3] = {-1, 0, 1};
    int   _motion[2]     = {0, 1};
    int   _easing[4]     = {0, 1, 2, 3};

    int test_case_max = 4 * 2 * 3 * 2 * 4;
    int test_case     = 0;
    int test_case_d   = 7; // 7 is the number of animated properties.

    float _duration     = 0.4f;
    float _repeat_delay = _duration * 0.25f; // < _duration. cause real_duration = _duration - _repeat_delay;
    float noise_maker   = 0.0f;
    // total testing time = ceil((4*2*3*2*4) / 7) * (_duration(=0.4) * 2 + 0.01) = 22.68 seconds
    for(test_case = 0; test_case < test_case_max + test_case_d; test_case += test_case_d)
    {
      tet_printf("test [%d ~ %d / %d]\n", test_case, test_case + test_case_d - 1, test_case_max);

      VisualFactory factory = VisualFactory::Get();
      Property::Map propertyMap;
      Property::Map animationMap;
      propertyMap.Insert(Visual::Property::TYPE, DevelVisual::ANIMATED_GRADIENT);

      int gradient_type = DevelAnimatedGradientVisual::GradientType::LINEAR;
      int unit_type     = DevelAnimatedGradientVisual::UnitType::USER_SPACE;
      int spread_type   = DevelAnimatedGradientVisual::SpreadType::REPEAT;

      auto buildAnimatedMap = [&animationMap, &_direction, &_duration, &_delay, &_loop_count, &_repeat_delay, &_motion, &_easing, &test_case](const Property::Value& start, const Property::Value& target, int tc_offset) -> Property::Map& {
        int tc         = (test_case + tc_offset);
        int idx_easing = tc % 4;
        tc /= 4;
        int idx_motion = tc % 2;
        tc /= 2;
        int idx_loop_count = tc % 3;
        tc /= 3;
        int idx_direction = tc % 2;
        tc /= 2;
        int idx_delay = tc % 4;
        tc /= 4;

        float duration     = _duration - _repeat_delay;
        float repeat_delay = _repeat_delay;
        float delay        = _delay[idx_delay] * _duration;
        int   direction    = _direction[idx_direction];
        int   loop_count   = _loop_count[idx_loop_count];
        int   motion       = _motion[idx_motion];
        int   easing       = _easing[idx_easing];

        animationMap.Clear();
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::START, start);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::TARGET, target);
        if(direction == 0)
        {
          animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::DIRECTION, DevelAnimatedGradientVisual::AnimationParameter::DirectionType::FORWARD);
        }
        else
        {
          animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::DIRECTION, DevelAnimatedGradientVisual::AnimationParameter::DirectionType::BACKWARD);
        }
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::DURATION, duration);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::DELAY, delay);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT, loop_count);
        animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT_DELAY, repeat_delay);
        if(motion == 0)
        {
          animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::MOTION_TYPE, DevelAnimatedGradientVisual::AnimationParameter::MotionType::LOOP);
        }
        else
        {
          animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::MOTION_TYPE, DevelAnimatedGradientVisual::AnimationParameter::MotionType::MIRROR);
        }
        if(easing == 0)
        {
          animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::EASING_TYPE, DevelAnimatedGradientVisual::AnimationParameter::EasingType::LINEAR);
        }
        else if(easing == 1)
        {
          animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::EASING_TYPE, DevelAnimatedGradientVisual::AnimationParameter::EasingType::IN);
        }
        else if(easing == 2)
        {
          animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::EASING_TYPE, DevelAnimatedGradientVisual::AnimationParameter::EasingType::OUT);
        }
        else
        {
          animationMap.Insert(DevelAnimatedGradientVisual::AnimationParameter::Property::EASING_TYPE, DevelAnimatedGradientVisual::AnimationParameter::EasingType::IN_OUT);
        }

        return animationMap;
      };

      // Give different values for debuging
      noise_maker += 1.0f;
      Vector2 start1(-0.5f + noise_maker * 0.1f, 0.5f + noise_maker * 0.1f);
      Vector2 end1(0.5f + noise_maker * 0.1f, -0.5f + noise_maker * 0.1f);
      Vector4 start_color1(1.0f, 0.7f, 0.5f, 1.0f);
      Vector4 end_color1(0.7f, 0.5f, 1.0f, 1.0f);
      Vector2 rotate_center1(0.0f + noise_maker * 0.1f, 0.4f + noise_maker * 0.1f);
      float   rotate_amount1 = 0.0f + noise_maker * 0.1f;
      float   offset1        = 0.f + noise_maker * 0.1f;

      Vector2 start2(0.2f + noise_maker * 0.1f, -0.7f + noise_maker * 0.1f);
      Vector2 end2(0.5f + noise_maker * 0.1f, 0.5f + noise_maker * 0.1f);
      Vector4 start_color2(0.0f, 0.1f, 0.8f, 1.0f);
      Vector4 end_color2(0.3f, 1.0f, 0.1f, 0.0f);
      Vector2 rotate_center2(0.0f + noise_maker * 0.1f, -0.4f + noise_maker * 0.1f);
      float   rotate_amount2 = 7.0f + noise_maker * 0.1f;
      float   offset2        = 2.f + noise_maker * 0.1f;

      propertyMap.Insert(DevelAnimatedGradientVisual::Property::GRADIENT_TYPE, gradient_type);
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::UNIT_TYPE, unit_type);
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::SPREAD_TYPE, spread_type);

      propertyMap.Insert(DevelAnimatedGradientVisual::Property::START_POSITION, buildAnimatedMap(start1, start2, 0));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::END_POSITION, buildAnimatedMap(end1, end2, 1));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::START_COLOR, buildAnimatedMap(start_color1, start_color2, 2));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::END_COLOR, buildAnimatedMap(end_color1, end_color2, 3));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::ROTATE_CENTER, buildAnimatedMap(rotate_center1, rotate_center2, 4));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT, buildAnimatedMap(rotate_amount1, rotate_amount2, 5));
      propertyMap.Insert(DevelAnimatedGradientVisual::Property::OFFSET, buildAnimatedMap(offset1, offset2, 6));

      Visual::Base visual = factory.CreateVisual(propertyMap);

      DummyControl        actor     = DummyControl::New(true);
      Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
      dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
      actor.SetProperty(Actor::Property::SIZE, Vector2(2000.f, 2000.f));
      actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      actor.SetProperty(Actor::Property::COLOR, Color::BLACK);
      application.GetScene().Add(actor);

      application.SendNotification();
      application.Render(0);
      application.SendNotification();

      DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);

      application.SendNotification();

      //Compare between CPU calculated value and Shader Visual calculated value
      auto testProperty = [&application, &_direction, &_duration, &_delay, &_loop_count, &_repeat_delay, &_motion, &_easing, &test_case](const char* name, const Property::Value& start, const Property::Value& target, int tc_offset, int value_type, float progress) -> void {
        int tc         = (test_case + tc_offset);
        int idx_easing = tc % 4;
        tc /= 4;
        int idx_motion = tc % 2;
        tc /= 2;
        int idx_loop_count = tc % 3;
        tc /= 3;
        int idx_direction = tc % 2;
        tc /= 2;
        int idx_delay = tc % 4;
        tc /= 4;

        float duration     = _duration - _repeat_delay;
        float repeat_delay = _repeat_delay;
        float delay        = _delay[idx_delay] * _duration;
        int   direction    = _direction[idx_direction];
        int   loop_count   = _loop_count[idx_loop_count];
        int   motion       = _motion[idx_motion];
        int   easing       = _easing[idx_easing];

        progress -= delay / _duration;

        Property::Value s = start;
        Property::Value t = target;
        if(direction == 1)
        {
          s = target;
          t = start;
        }
        float x; ///< Animator progress value
        if(loop_count == 0)
        {
          x = 1.0f;
        }
        else if(loop_count > 0 && progress + 0.01f > loop_count)
        {
          x = (motion == 0) ? 1.0f : 0.0f;
        }
        else
        {
          if(progress < 0.0f)
          {
            progress = 0.0f;
          }
          progress = fmodf(progress, 1.0f);
          progress = Dali::Clamp((progress * (duration + repeat_delay) - repeat_delay) / duration, 0.0f, 1.0f);

          x = progress;
          if(motion == 1)
          {
            x = progress * 2.0f;
            if(x > 1.0f)
            {
              x = 2.0f - x;
            }
          }

          if(easing == 1) // EASE_IN
          {
            x = x * x;
          }
          else if(easing == 2) // EASE_OUT
          {
            x = 2.0f * x - x * x;
          }
          else if(easing == 3) // EASE_IN_OUT
          {
            x = x * x * (3.0f - 2.0f * x);
          }
        }
        if(value_type == 0) // result type is Float
        {
          float res;
          float cur;
          res = s.Get<float>() * (1.0f - x) + t.Get<float>() * (x);
          DALI_TEST_EQUALS(application.GetGlAbstraction().GetUniformValue<float>(name, cur), true, TEST_LOCATION);
          DALI_TEST_EQUALS(res, cur, Math::MACHINE_EPSILON_100, TEST_LOCATION);
        }
        else if(value_type == 1) // result type is Vector2
        {
          Vector2 res;
          Vector2 cur;
          res = s.Get<Vector2>() * (1.0f - x) + t.Get<Vector2>() * (x);
          DALI_TEST_EQUALS(application.GetGlAbstraction().GetUniformValue<Vector2>(name, cur), true, TEST_LOCATION);
          DALI_TEST_EQUALS(res, cur, Math::MACHINE_EPSILON_100, TEST_LOCATION);
        }
        else if(value_type == 2) // result type is Vector3
        {
          Vector3 res;
          Vector3 cur;
          res = s.Get<Vector3>() * (1.0f - x) + t.Get<Vector3>() * (x);
          DALI_TEST_EQUALS(application.GetGlAbstraction().GetUniformValue<Vector3>(name, cur), true, TEST_LOCATION);
          DALI_TEST_EQUALS(res, cur, Math::MACHINE_EPSILON_100, TEST_LOCATION);
        }
        else // result type is Vector4
        {
          Vector4 res;
          Vector4 cur;
          res = s.Get<Vector4>() * (1.0f - x) + t.Get<Vector4>() * (x);
          DALI_TEST_EQUALS(application.GetGlAbstraction().GetUniformValue<Vector4>(name, cur), true, TEST_LOCATION);
          DALI_TEST_EQUALS(res, cur, Math::MACHINE_EPSILON_100, TEST_LOCATION);
        }
      };

      float step = 0.0f;
      for(int iter = 0; iter < 2; iter++) // test 2*duration seconds
      {
        for(int step_iter = 0; step_iter < 3; step_iter++)
        {
          application.SendNotification();
          application.Render(_duration * 250.f); // step i/4
          application.SendNotification();
          step += 0.25f;

          testProperty("start_point", Property::Value(start1), Property::Value(start2), 0, 1, step);
          testProperty("end_point", Property::Value(end1), Property::Value(end2), 1, 1, step);
          testProperty("start_color", Property::Value(start_color1), Property::Value(start_color2), 2, 3, step);
          testProperty("end_color", Property::Value(end_color1), Property::Value(end_color2), 3, 3, step);
          testProperty("rotate_center", Property::Value(rotate_center1), Property::Value(rotate_center2), 4, 1, step);
          testProperty("rotate_angle", Property::Value(rotate_amount1), Property::Value(rotate_amount2), 5, 0, step);
          testProperty("gradient_offset", Property::Value(offset1), Property::Value(offset2), 6, 0, step);
        }
        application.SendNotification();
        application.Render(_duration * 250.f); // step 4/4 will not test
        application.SendNotification();
        step += 0.25f;
      }

      application.SendNotification();
      actor.Unparent();
      application.SendNotification();
      application.Render(10.f); // tempral time
      application.SendNotification();
    }
  }

  END_TEST;
}

int UtcDaliVisualAnimatedGradientVisual03(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedGradientVisual with full-option use string key");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("start_point", Property::Type::VECTOR2),
      UniformData("end_point", Property::Type::VECTOR2),
      UniformData("start_color", Property::Type::VECTOR4),
      UniformData("end_color", Property::Type::VECTOR4),
      UniformData("rotate_center", Property::Type::VECTOR2),
      UniformData("rotate_angle", Property::Type::FLOAT),
      UniformData("gradient_offset", Property::Type::FLOAT),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  {
    float _delay[4]      = {0.0f, -1.35f, 0.15f, -0.4f}; // fract(_delay) must NOT be 1/4, 2/4, 3/4. cause we don't know progress is 1.0f or 0.0f
    int   _direction[2]  = {0, 1};
    int   _loop_count[3] = {-1, 0, 1};
    int   _motion[2]     = {0, 1};
    int   _easing[4]     = {0, 1, 2, 3};

    int test_case_max = 4 * 2 * 3 * 2 * 4;
    int test_case     = 0;
    int test_case_d   = 7; // 7 is the number of animated properties.

    float _duration     = 0.4f;
    float _repeat_delay = _duration * 0.25f; // < _duration. cause real_duration = _duration - _repeat_delay;
    float noise_maker   = 0.2f;
    // total testing time = ceil((4*2*3*2*4) / 7) * (_duration(=0.4) * 2 + 0.01) = 22.68 seconds
    for(test_case = 0; test_case < test_case_max + test_case_d; test_case += test_case_d)
    {
      tet_printf("test [%d ~ %d / %d]\n", test_case, test_case + test_case_d - 1, test_case_max);

      VisualFactory factory = VisualFactory::Get();
      Property::Map propertyMap;
      Property::Map animationMap;
      propertyMap.Insert(Visual::Property::TYPE, DevelVisual::ANIMATED_GRADIENT);

      auto buildAnimatedMap = [&animationMap, &_direction, &_duration, &_delay, &_loop_count, &_repeat_delay, &_motion, &_easing, &test_case](const Property::Value& start, const Property::Value& target, int tc_offset) -> Property::Map& {
        int tc         = (test_case + tc_offset);
        int idx_easing = tc % 4;
        tc /= 4;
        int idx_motion = tc % 2;
        tc /= 2;
        int idx_loop_count = tc % 3;
        tc /= 3;
        int idx_direction = tc % 2;
        tc /= 2;
        int idx_delay = tc % 4;
        tc /= 4;

        float duration     = _duration - _repeat_delay;
        float repeat_delay = _repeat_delay;
        float delay        = _delay[idx_delay] * _duration;
        int   direction    = _direction[idx_direction];
        int   loop_count   = _loop_count[idx_loop_count];
        int   motion       = _motion[idx_motion];
        int   easing       = _easing[idx_easing];

        animationMap.Clear();
        animationMap.Insert("startValue", start);
        animationMap.Insert("targetValue", target);
        if(direction == 0)
        {
          animationMap.Insert("directionType", "FORWARD");
        }
        else
        {
          animationMap.Insert("directionType", "BACKWARD");
        }
        animationMap.Insert("duration", duration);
        animationMap.Insert("delay", delay);
        animationMap.Insert("repeat", loop_count);
        animationMap.Insert("repeatDelay", repeat_delay);
        if(motion == 0)
        {
          animationMap.Insert("motionType", "LOOP");
        }
        else
        {
          animationMap.Insert("motionType", "MIRROR");
        }
        if(easing == 0)
        {
          animationMap.Insert("easingType", "LINEAR");
        }
        else if(easing == 1)
        {
          animationMap.Insert("easingType", "IN");
        }
        else if(easing == 2)
        {
          animationMap.Insert("easingType", "OUT");
        }
        else
        {
          animationMap.Insert("easingType", "IN_OUT");
        }

        return animationMap;
      };

      // Give different values for debuging
      noise_maker += 0.8f;
      Vector2 start1(-0.5f + noise_maker * 0.1f, 0.5f + noise_maker * 0.1f);
      Vector2 end1(0.5f + noise_maker * 0.1f, -0.5f + noise_maker * 0.1f);
      Vector4 start_color1(1.0f, 0.7f, 0.5f, 1.0f);
      Vector4 end_color1(0.7f, 0.5f, 1.0f, 1.0f);
      Vector2 rotate_center1(0.0f + noise_maker * 0.1f, 0.4f + noise_maker * 0.1f);
      float   rotate_amount1 = 0.0f + noise_maker * 0.1f;
      float   offset1        = 0.f + noise_maker * 0.1f;

      Vector2 start2(0.2f + noise_maker * 0.1f, -0.7f + noise_maker * 0.1f);
      Vector2 end2(0.5f + noise_maker * 0.1f, 0.5f + noise_maker * 0.1f);
      Vector4 start_color2(0.0f, 0.1f, 0.8f, 1.0f);
      Vector4 end_color2(0.3f, 1.0f, 0.1f, 0.0f);
      Vector2 rotate_center2(0.0f + noise_maker * 0.1f, -0.4f + noise_maker * 0.1f);
      float   rotate_amount2 = 7.0f + noise_maker * 0.1f;
      float   offset2        = 2.f + noise_maker * 0.1f;

      propertyMap.Insert("gradientType", "LINEAR");
      propertyMap.Insert("unitType", "USER_SPACE");
      propertyMap.Insert("spreadType", "CLAMP");

      propertyMap.Insert("startPosition", buildAnimatedMap(start1, start2, 0));
      propertyMap.Insert("endPosition", buildAnimatedMap(end1, end2, 1));
      propertyMap.Insert("startColor", buildAnimatedMap(start_color1, start_color2, 2));
      propertyMap.Insert("endColor", buildAnimatedMap(end_color1, end_color2, 3));
      propertyMap.Insert("rotateCenter", buildAnimatedMap(rotate_center1, rotate_center2, 4));
      propertyMap.Insert("rotateAmount", buildAnimatedMap(rotate_amount1, rotate_amount2, 5));
      propertyMap.Insert("offset", buildAnimatedMap(offset1, offset2, 6));

      Visual::Base visual = factory.CreateVisual(propertyMap);

      DummyControl        actor     = DummyControl::New(true);
      Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
      dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
      actor.SetProperty(Actor::Property::SIZE, Vector2(2000.f, 2000.f));
      actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      actor.SetProperty(Actor::Property::COLOR, Color::BLACK);
      application.GetScene().Add(actor);

      application.SendNotification();
      application.Render(0);
      application.SendNotification();

      DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);

      application.SendNotification();

      //Compare between CPU calculated value and Shader Visual calculated value
      auto testProperty = [&application, &_direction, &_duration, &_delay, &_loop_count, &_repeat_delay, &_motion, &_easing, &test_case](const char* name, const Property::Value& start, const Property::Value& target, int tc_offset, int value_type, float progress) -> void {
        int tc         = (test_case + tc_offset);
        int idx_easing = tc % 4;
        tc /= 4;
        int idx_motion = tc % 2;
        tc /= 2;
        int idx_loop_count = tc % 3;
        tc /= 3;
        int idx_direction = tc % 2;
        tc /= 2;
        int idx_delay = tc % 4;
        tc /= 4;

        float duration     = _duration - _repeat_delay;
        float repeat_delay = _repeat_delay;
        float delay        = _delay[idx_delay] * _duration;
        int   direction    = _direction[idx_direction];
        int   loop_count   = _loop_count[idx_loop_count];
        int   motion       = _motion[idx_motion];
        int   easing       = _easing[idx_easing];

        progress -= delay / _duration;

        Property::Value s = start;
        Property::Value t = target;
        if(direction == 1)
        {
          s = target;
          t = start;
        }
        float x; ///< Animator progress value
        if(loop_count == 0)
        {
          x = 1.0f;
        }
        else if(loop_count > 0 && progress + 0.01f > loop_count)
        {
          x = (motion == 0) ? 1.0f : 0.0f;
        }
        else
        {
          if(progress < 0.0f)
          {
            progress = 0.0f;
          }
          progress = fmodf(progress, 1.0f);
          progress = Dali::Clamp((progress * (duration + repeat_delay) - repeat_delay) / duration, 0.0f, 1.0f);

          x = progress;
          if(motion == 1)
          {
            x = progress * 2.0f;
            if(x > 1.0f)
            {
              x = 2.0f - x;
            }
          }

          if(easing == 1) // EASE_IN
          {
            x = x * x;
          }
          else if(easing == 2) // EASE_OUT
          {
            x = 2.0f * x - x * x;
          }
          else if(easing == 3) // EASE_IN_OUT
          {
            x = x * x * (3.0f - 2.0f * x);
          }
        }
        if(value_type == 0) // result type is Float
        {
          float res;
          float cur;
          res = s.Get<float>() * (1.0f - x) + t.Get<float>() * (x);
          DALI_TEST_EQUALS(application.GetGlAbstraction().GetUniformValue<float>(name, cur), true, TEST_LOCATION);
          DALI_TEST_EQUALS(res, cur, Math::MACHINE_EPSILON_100, TEST_LOCATION);
        }
        else if(value_type == 1) // result type is Vector2
        {
          Vector2 res;
          Vector2 cur;
          res = s.Get<Vector2>() * (1.0f - x) + t.Get<Vector2>() * (x);
          DALI_TEST_EQUALS(application.GetGlAbstraction().GetUniformValue<Vector2>(name, cur), true, TEST_LOCATION);
          DALI_TEST_EQUALS(res, cur, Math::MACHINE_EPSILON_100, TEST_LOCATION);
        }
        else if(value_type == 2) // result type is Vector3
        {
          Vector3 res;
          Vector3 cur;
          res = s.Get<Vector3>() * (1.0f - x) + t.Get<Vector3>() * (x);
          DALI_TEST_EQUALS(application.GetGlAbstraction().GetUniformValue<Vector3>(name, cur), true, TEST_LOCATION);
          DALI_TEST_EQUALS(res, cur, Math::MACHINE_EPSILON_100, TEST_LOCATION);
        }
        else // result type is Vector4
        {
          Vector4 res;
          Vector4 cur;
          res = s.Get<Vector4>() * (1.0f - x) + t.Get<Vector4>() * (x);
          DALI_TEST_EQUALS(application.GetGlAbstraction().GetUniformValue<Vector4>(name, cur), true, TEST_LOCATION);
          DALI_TEST_EQUALS(res, cur, Math::MACHINE_EPSILON_100, TEST_LOCATION);
        }
      };

      float step = 0.0f;
      for(int iter = 0; iter < 2; iter++) // test 2*duration seconds
      {
        for(int step_iter = 0; step_iter < 3; step_iter++)
        {
          application.SendNotification();
          application.Render(_duration * 250.f); // step i/4
          application.SendNotification();
          step += 0.25f;

          testProperty("start_point", Property::Value(start1), Property::Value(start2), 0, 1, step);
          testProperty("end_point", Property::Value(end1), Property::Value(end2), 1, 1, step);
          testProperty("start_color", Property::Value(start_color1), Property::Value(start_color2), 2, 3, step);
          testProperty("end_color", Property::Value(end_color1), Property::Value(end_color2), 3, 3, step);
          testProperty("rotate_center", Property::Value(rotate_center1), Property::Value(rotate_center2), 4, 1, step);
          testProperty("rotate_angle", Property::Value(rotate_amount1), Property::Value(rotate_amount2), 5, 0, step);
          testProperty("gradient_offset", Property::Value(offset1), Property::Value(offset2), 6, 0, step);
        }
        application.SendNotification();
        application.Render(_duration * 250.f); // step 4/4 will not test
        application.SendNotification();
        step += 0.25f;
      }

      application.SendNotification();
      actor.Unparent();
      application.SendNotification();
      application.Render(10.f); // tempral time
      application.SendNotification();
    }
  }

  END_TEST;
}

int UtcDaliVisualWireframeVisual(void)
{
  ToolkitTestApplication application;

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::WIREFRAME);

  // Create the visual.
  Visual::Base visual = factory.CreateVisual(propertyMap);

  DALI_TEST_CHECK(visual);

  Property::Map resultMap;
  visual.CreatePropertyMap(resultMap);

  // Check the property values from the returned map from visual
  Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::WIREFRAME);

  END_TEST;
}

int UtcDaliVisualGetTransform(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetTransform: ColorVisual");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
  Visual::Base colorVisual = factory.CreateVisual(propertyMap);

  Dali::Property::Map visualMap;
  colorVisual.CreatePropertyMap(visualMap);
  Property::Value*     value = visualMap.Find(Dali::Toolkit::Visual::Property::TRANSFORM);
  Dali::Property::Map* map   = value->GetMap();
  DALI_TEST_CHECK(map);

  //Test default values
  {
    Property::Value* typeValue = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
    DALI_TEST_CHECK(typeValue);
    DALI_TEST_CHECK(typeValue->Get<Vector2>() == Vector2(0.0f, 0.0f));
  }
  {
    Property::Value* typeValue = map->Find(Toolkit::Visual::Transform::Property::SIZE);
    DALI_TEST_CHECK(typeValue);
    DALI_TEST_CHECK(typeValue->Get<Vector2>() == Vector2(1.0f, 1.0f));
  }
  {
    Property::Value* typeValue = map->Find(Toolkit::Visual::Transform::Property::OFFSET_POLICY);
    DALI_TEST_CHECK(typeValue);
    DALI_TEST_CHECK(typeValue->Get<Vector2>() == Vector2(Toolkit::Visual::Transform::Policy::RELATIVE, Toolkit::Visual::Transform::Policy::RELATIVE));
  }
  {
    Property::Value* typeValue = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
    DALI_TEST_CHECK(typeValue);
    DALI_TEST_CHECK(typeValue->Get<Vector2>() == Vector2(Toolkit::Visual::Transform::Policy::RELATIVE, Toolkit::Visual::Transform::Policy::RELATIVE));
  }
  {
    Property::Value* typeValue = map->Find(Toolkit::Visual::Transform::Property::ORIGIN);
    DALI_TEST_CHECK(typeValue);
    DALI_TEST_CHECK((Toolkit::Align::Type)typeValue->Get<int>() == Toolkit::Align::TOP_BEGIN);
  }
  {
    Property::Value* typeValue = map->Find(Toolkit::Visual::Transform::Property::ANCHOR_POINT);
    DALI_TEST_CHECK(typeValue);
    DALI_TEST_CHECK((Toolkit::Align::Type)typeValue->Get<int>() == Toolkit::Align::TOP_BEGIN);
  }
  {
    Property::Value* typeValue = map->Find(Toolkit::DevelVisual::Transform::Property::EXTRA_SIZE);
    DALI_TEST_CHECK(typeValue);
    DALI_TEST_CHECK(typeValue->Get<Vector2>() == Vector2(0.0f, 0.0f));
  }

  END_TEST;
}

static void TestTransform(ToolkitTestApplication& application, Visual::Base visual)
{
  Property::Map transform;
  transform.Insert(Visual::Transform::Property::OFFSET, Vector2(10.0f, 10.0f));
  transform.Insert(Visual::Transform::Property::SIZE, Vector2(0.2f, 0.2f));
  transform.Insert(Visual::Transform::Property::OFFSET_POLICY, Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE));
  transform.Insert(Visual::Transform::Property::ORIGIN, "CENTER");
  transform.Insert(Visual::Transform::Property::ANCHOR_POINT, Toolkit::Align::BOTTOM_END);
  transform.Insert(DevelVisual::Transform::Property::EXTRA_SIZE, Vector2(50.0f, 50.0f));

  visual.SetTransformAndSize(transform, Vector2(100, 100));

  Dali::Property::Map visualMap;
  visual.CreatePropertyMap(visualMap);
  Property::Value*     value = visualMap.Find(Dali::Toolkit::Visual::Property::TRANSFORM);
  Dali::Property::Map* map   = value->GetMap();
  DALI_TEST_CHECK(map);

  {
    Property::Value* typeValue = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
    DALI_TEST_CHECK(typeValue);
    DALI_TEST_EQUALS(typeValue->Get<Vector2>(), Vector2(10.0f, 10.0f), TEST_LOCATION);
  }
  {
    Property::Value* typeValue = map->Find(Toolkit::Visual::Transform::Property::SIZE);
    DALI_TEST_CHECK(typeValue);
    DALI_TEST_EQUALS(typeValue->Get<Vector2>(), Vector2(0.2f, 0.2f), TEST_LOCATION);
  }
  {
    Property::Value* typeValue = map->Find(Toolkit::Visual::Transform::Property::OFFSET_POLICY);
    DALI_TEST_CHECK(typeValue);
    DALI_TEST_EQUALS(typeValue->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);
  }
  {
    Property::Value* typeValue = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
    DALI_TEST_CHECK(typeValue);
    DALI_TEST_EQUALS(typeValue->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::RELATIVE, Toolkit::Visual::Transform::Policy::RELATIVE), TEST_LOCATION);
  }
  {
    Property::Value* typeValue = map->Find(Toolkit::Visual::Transform::Property::ORIGIN);
    DALI_TEST_CHECK(typeValue);
    DALI_TEST_EQUALS((Toolkit::Align::Type)typeValue->Get<int>(), Toolkit::Align::CENTER, TEST_LOCATION);
  }
  {
    Property::Value* typeValue = map->Find(Toolkit::Visual::Transform::Property::ANCHOR_POINT);
    DALI_TEST_CHECK(typeValue);
    DALI_TEST_EQUALS((Toolkit::Align::Type)typeValue->Get<int>(), Toolkit::Align::BOTTOM_END, TEST_LOCATION);
  }
  {
    Property::Value* typeValue = map->Find(Toolkit::DevelVisual::Transform::Property::EXTRA_SIZE);
    DALI_TEST_CHECK(typeValue);
    DALI_TEST_EQUALS(typeValue->Get<Vector2>(), Vector2(50.0f, 50.0f), TEST_LOCATION);
  }

  //Put the visual on the stage
  DummyControl        actor     = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  actor.SetProperty(Actor::Property::SIZE, Vector2(2000.f, 2000.f));
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  application.GetScene().Add(actor);

  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
  dummyImpl.SetLayout(DummyControl::Property::TEST_VISUAL, transform);

  application.SendNotification();
  application.Render(0);
  Renderer renderer(actor.GetRendererAt(0));

  Vector2 offset = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET);
  DALI_TEST_EQUALS(offset, Vector2(10.0f, 10.0f), TEST_LOCATION);

  Vector2 size = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE);
  DALI_TEST_EQUALS(size, Vector2(0.2f, 0.2f), TEST_LOCATION);

  Vector4 offsetSizeMode = renderer.GetProperty<Vector4>(VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE);
  DALI_TEST_EQUALS(offsetSizeMode, Vector4(1.0f, 1.0f, 0.0f, 0.0f), TEST_LOCATION);

  Vector2 parentOrigin = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_ORIGIN);
  DALI_TEST_EQUALS(parentOrigin, Vector2(0.0f, 0.0f), TEST_LOCATION);

  Vector2 anchorPoint = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_ANCHOR_POINT);
  DALI_TEST_EQUALS(anchorPoint, Vector2(-0.5f, -0.5f), TEST_LOCATION);

  Vector2 extraSize = renderer.GetProperty<Vector2>(VisualRenderer::Property::EXTRA_SIZE);
  DALI_TEST_EQUALS(extraSize, Vector2(50.0f, 50.0f), TEST_LOCATION);

  //Set a new transform
  transform.Clear();
  transform = DefaultTransform();
  transform.Insert(Visual::Transform::Property::OFFSET, Vector2(20.0f, 20.0f));
  transform.Insert(Visual::Transform::Property::SIZE, Vector2(100.0f, 100.0f));
  transform.Insert(Visual::Transform::Property::SIZE_POLICY, Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE));
  transform.Insert(DevelVisual::Transform::Property::EXTRA_SIZE, Vector2(0.5f, 0.5f));
  visual.SetTransformAndSize(transform, Vector2(100, 100));
  application.SendNotification();
  application.Render(0);

  //Check that the values have changed in the renderer
  offset = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET);
  DALI_TEST_EQUALS(offset, Vector2(20.0f, 20.0f), TEST_LOCATION);

  size = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE);
  DALI_TEST_EQUALS(size, Vector2(100.0f, 100.0f), TEST_LOCATION);

  offsetSizeMode = renderer.GetProperty<Vector4>(VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE);
  DALI_TEST_EQUALS(offsetSizeMode, Vector4(0.0f, 0.0f, 1.0f, 1.0f), TEST_LOCATION);

  //Parent origin and anchor point should have the default values
  parentOrigin = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_ORIGIN);
  DALI_TEST_EQUALS(parentOrigin, Vector2(-0.5f, -0.5f), TEST_LOCATION);

  anchorPoint = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_ANCHOR_POINT);
  DALI_TEST_EQUALS(anchorPoint, Vector2(0.5f, 0.5f), TEST_LOCATION);

  extraSize = renderer.GetProperty<Vector2>(VisualRenderer::Property::EXTRA_SIZE);
  DALI_TEST_EQUALS(extraSize, Vector2(0.5f, 0.5f), TEST_LOCATION);
}

int UtcDaliVisualSetTransform01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualSetTransform: ColorVisual");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(Visual::Property::OPACITY, 0.5f);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
  Visual::Base visual = factory.CreateVisual(propertyMap);
  TestTransform(application, visual);
  TestMixColor(visual, ColorVisual::Property::MIX_COLOR, Color::BLUE);

  END_TEST;
}

int UtcDaliVisualSetTransform0(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualSetTransform: ColorVisual");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
  Visual::Base visual = factory.CreateVisual(propertyMap);
  TestTransform(application, visual);
  TestMixColor(visual, ColorVisual::Property::MIX_COLOR, Color::BLUE);

  END_TEST;
}

int UtcDaliVisualSetTransform1(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualSetTransform: PrimitiveVisual");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap[Toolkit::Visual::Property::TYPE]      = Visual::PRIMITIVE;
  propertyMap[PrimitiveVisual::Property::MIX_COLOR] = Color::WHITE;
  propertyMap[PrimitiveVisual::Property::SHAPE]     = PrimitiveVisual::Shape::SPHERE;
  propertyMap[PrimitiveVisual::Property::SLICES]    = 10;
  propertyMap[PrimitiveVisual::Property::STACKS]    = 10;
  Visual::Base visual                               = factory.CreateVisual(propertyMap);
  TestTransform(application, visual);
  TestMixColor(visual, PrimitiveVisual::Property::MIX_COLOR, Color::WHITE);

  END_TEST;
}

int UtcDaliVisualSetTransform2(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualSetTransform: GradientVisual");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::GRADIENT);
  propertyMap.Insert(Visual::Property::MIX_COLOR, Color::GREEN);

  Property::Array stopOffsets;
  stopOffsets.PushBack(0.0f);
  stopOffsets.PushBack(0.3f);
  stopOffsets.PushBack(0.6f);
  stopOffsets.PushBack(0.8f);
  stopOffsets.PushBack(1.0f);
  propertyMap.Insert(GradientVisual::Property::STOP_OFFSET, stopOffsets);

  Property::Array stopColors;
  stopColors.PushBack(Vector4(129.f, 198.f, 193.f, 255.f) / 255.f);
  stopColors.PushBack(Vector4(196.f, 198.f, 71.f, 122.f) / 255.f);
  stopColors.PushBack(Vector4(214.f, 37.f, 139.f, 191.f) / 255.f);
  stopColors.PushBack(Vector4(129.f, 198.f, 193.f, 150.f) / 255.f);
  stopColors.PushBack(Color::YELLOW);
  propertyMap.Insert(GradientVisual::Property::STOP_COLOR, stopColors);
  propertyMap.Insert(GradientVisual::Property::CENTER, Vector2(0.5f, 0.5f));
  propertyMap.Insert(GradientVisual::Property::RADIUS, 1.414f);
  Visual::Base visual = factory.CreateVisual(propertyMap);
  TestTransform(application, visual);
  TestMixColor(visual, Visual::Property::MIX_COLOR, Color::GREEN);

  END_TEST;
}

int UtcDaliVisualSetTransform3(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualSetTransform: BorderVisual");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::BORDER);
  propertyMap.Insert(Visual::Property::MIX_COLOR, Color::MAGENTA);
  propertyMap.Insert(BorderVisual::Property::COLOR, Vector4(0.f, 1.f, 0.f, 0.6f));
  propertyMap.Insert(BorderVisual::Property::SIZE, 3.0f);
  Visual::Base visual = factory.CreateVisual(propertyMap);
  TestTransform(application, visual);
  TestMixColor(visual, Visual::Property::MIX_COLOR, Color::MAGENTA);

  END_TEST;
}

int UtcDaliVisualSetTransform4(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualSetTransform: MeshVisual");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::MESH);
  propertyMap.Insert(Visual::Property::MIX_COLOR, Color::CYAN);

  propertyMap.Insert("objectUrl", TEST_OBJ_FILE_NAME);
  propertyMap.Insert("materialUrl", TEST_MTL_FILE_NAME);
  propertyMap.Insert("texturesPath", TEST_RESOURCE_LOCATION);
  propertyMap.Insert("shadingMode", MeshVisual::ShadingMode::TEXTURELESS_WITH_DIFFUSE_LIGHTING);
  propertyMap.Insert("lightPosition", Vector3(5.0f, 10.0f, 15.0f));
  Visual::Base visual = factory.CreateVisual(propertyMap);
  TestTransform(application, visual);
  TestMixColor(visual, Visual::Property::MIX_COLOR, Color::CYAN);

  END_TEST;
}

int UtcDaliVisualSetTransform5(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualSetTransform: ImageVisual for URL ");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap[Toolkit::Visual::Property::TYPE]                     = Toolkit::Visual::IMAGE;
  propertyMap[Visual::Property::MIX_COLOR]                         = Color::YELLOW;
  propertyMap[Toolkit::ImageVisual::Property::URL]                 = TEST_IMAGE_FILE_NAME;
  propertyMap[Toolkit::ImageVisual::Property::DESIRED_WIDTH]       = 100.0f;
  propertyMap[Toolkit::ImageVisual::Property::DESIRED_HEIGHT]      = 100.0f;
  propertyMap[Toolkit::ImageVisual::Property::FITTING_MODE]        = FittingMode::SCALE_TO_FILL;
  propertyMap[Toolkit::ImageVisual::Property::SAMPLING_MODE]       = SamplingMode::BOX_THEN_LINEAR;
  propertyMap[Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING] = true;
  Visual::Base visual                                              = factory.CreateVisual(propertyMap);
  TestTransform(application, visual);
  TestMixColor(visual, Visual::Property::MIX_COLOR, Color::YELLOW);

  END_TEST;
}

int UtcDaliVisualSetTransform6(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualSetTransform: NPatch visual");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap[Toolkit::Visual::Property::TYPE]                     = Toolkit::Visual::IMAGE;
  propertyMap[Toolkit::ImageVisual::Property::URL]                 = TEST_NPATCH_FILE_NAME;
  propertyMap[Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING] = true;
  Visual::Base visual                                              = factory.CreateVisual(propertyMap);
  TestTransform(application, visual);
  TestMixColor(visual, Visual::Property::MIX_COLOR, Color::WHITE);

  END_TEST;
}

int UtcDaliVisualTestTransformPoliciesAsStrings(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualTestTransformPoliciesAsStrings: Use a ColorVisual and test the offset and size policies as strings");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
  Visual::Base visual = factory.CreateVisual(propertyMap);

  Property::Map transform;
  transform["offsetPolicy"] = Property::Array().Add("ABSOLUTE").Add("RELATIVE");
  transform["sizePolicy"]   = Property::Array().Add("RELATIVE").Add("ABSOLUTE");
  visual.SetTransformAndSize(transform, Vector2(100, 100));

  Dali::Property::Map visualMap;
  visual.CreatePropertyMap(visualMap);
  Property::Value*     value = visualMap.Find(Dali::Toolkit::Visual::Property::TRANSFORM);
  Dali::Property::Map* map   = value->GetMap();
  DALI_TEST_CHECK(map);

  {
    Property::Value* typeValue = map->Find(Toolkit::Visual::Transform::Property::OFFSET_POLICY);
    DALI_TEST_CHECK(typeValue);
    DALI_TEST_EQUALS(typeValue->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::RELATIVE), TEST_LOCATION);
  }
  {
    Property::Value* typeValue = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
    DALI_TEST_CHECK(typeValue);
    DALI_TEST_EQUALS(typeValue->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::RELATIVE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliNPatchVisualCustomShader(void)
{
  ToolkitTestApplication application;
  tet_infoline("NPatchVisual with custom shader");

  VisualFactory     factory = VisualFactory::Get();
  Property::Map     properties;
  Property::Map     shader;
  const std::string vertexShader                                   = "Foobar";
  const std::string fragmentShader                                 = "Foobar";
  shader[Dali::Toolkit::Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShader;
  shader[Dali::Toolkit::Visual::Shader::Property::VERTEX_SHADER]   = vertexShader;

  Property::Map transformMap;
  transformMap["size"]                    = Vector2(0.5f, 0.5f);
  transformMap["offset"]                  = Vector2(20.0f, 0.0f);
  transformMap["offsetPolicy"]            = Vector2(Visual::Transform::Policy::ABSOLUTE, Visual::Transform::Policy::ABSOLUTE);
  transformMap["anchorPoint"]             = Align::CENTER;
  transformMap["origin"]                  = Align::CENTER;
  transformMap["extraSize"]               = Vector2(0.0f, 50.0f);
  properties[Visual::Property::TRANSFORM] = transformMap;

  properties[Visual::Property::TYPE]                     = Visual::IMAGE;
  properties[Visual::Property::MIX_COLOR]                = Color::BLUE;
  properties[Visual::Property::SHADER]                   = shader;
  properties[ImageVisual::Property::URL]                 = TEST_NPATCH_FILE_NAME;
  properties[ImageVisual::Property::SYNCHRONOUS_LOADING] = true;

  Visual::Base visual = factory.CreateVisual(properties);
  TestMixColor(visual, Visual::Property::MIX_COLOR, Color::BLUE);

  // trigger creation through setting on stage
  DummyControl        dummy     = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
  dummyImpl.SetLayout(DummyControl::Property::TEST_VISUAL, transformMap);
  dummy.SetProperty(Actor::Property::SIZE, Vector2(2000.f, 2000.f));
  dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  application.GetScene().Add(dummy);
  application.SendNotification();

  Renderer        renderer = dummy.GetRendererAt(0);
  Shader          shader2  = renderer.GetShader();
  Property::Value value    = shader2.GetProperty(Shader::Property::PROGRAM);
  Property::Map*  map      = value.GetMap();
  DALI_TEST_CHECK(map);

  Property::Index index = VisualRenderer::Property::TRANSFORM_SIZE;
  DALI_TEST_EQUALS(renderer.GetProperty(index), Property::Value(Vector2(0.5, 0.5)), 0.001, TEST_LOCATION);

  Property::Value* fragment = map->Find("fragment"); // fragment key name from shader-impl.cpp
  DALI_TEST_EQUALS(fragmentShader, fragment->Get<std::string>(), TEST_LOCATION);

  Property::Value* vertex = map->Find("vertex"); // vertex key name from shader-impl.cpp
  DALI_TEST_EQUALS(vertexShader, vertex->Get<std::string>(), TEST_LOCATION);

  Vector2 extraSize = renderer.GetProperty<Vector2>(VisualRenderer::Property::EXTRA_SIZE);
  DALI_TEST_EQUALS(extraSize, Vector2(0.0f, 50.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualBlendMode(void)
{
  ToolkitTestApplication application;
  VisualFactory          factory = VisualFactory::Get();

  Visual::Base opaqueGradientVisual = factory.CreateVisual(
    Property::Map().Add(Toolkit::Visual::Property::TYPE, Visual::GRADIENT).Add(GradientVisual::Property::START_POSITION, Vector2(-0.5f, -0.5f)).Add(GradientVisual::Property::END_POSITION, Vector2(0.5f, 0.5f)).Add(GradientVisual::Property::STOP_COLOR, Property::Array().Add(Color::RED).Add(Color::GREEN)));

  Visual::Base alphaGradientVisual = factory.CreateVisual(
    Property::Map().Add(Toolkit::Visual::Property::TYPE, Visual::GRADIENT).Add(GradientVisual::Property::START_POSITION, Vector2(-0.5f, -0.5f)).Add(GradientVisual::Property::END_POSITION, Vector2(0.5f, 0.5f)).Add(GradientVisual::Property::STOP_COLOR, Property::Array().Add(Color::RED).Add(Vector4(1.0f, 1.0f, 1.0f, 0.5f))));

  DummyControl control = DummyControl::New(true);
  control.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(control);

  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(control.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, opaqueGradientVisual);
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL2, alphaGradientVisual);

  application.SendNotification();
  application.Render();

  // Control should have two renderers, the first one is opaque so our blending mode should be off, the second one has some alpha so should be set to automatic
  DALI_TEST_EQUALS(2u, control.GetRendererCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetRendererAt(0).GetProperty(Renderer::Property::BLEND_MODE).Get<int>(), (int)BlendMode::OFF, TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetRendererAt(1).GetProperty(Renderer::Property::BLEND_MODE).Get<int>(), (int)BlendMode::AUTO, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualRendererRemovalAndReAddition(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualRendererRemoval");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
  Visual::Base visual = factory.CreateVisual(propertyMap);

  visual.SetDepthIndex(1);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
  DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 0, TEST_LOCATION);

  dummyControl.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
  tet_infoline("Add control with visual to stage and check renderer count is 1");

  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 1, TEST_LOCATION);

  tet_infoline("Remove control with visual from stage and check renderer count is 0");
  application.GetScene().Remove(dummyControl);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 0, TEST_LOCATION);

  tet_infoline("Re-add control with visual to stage and check renderer count is still 1");

  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualTextVisualRender(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualTextVisualRender");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::TEXT);
  propertyMap.Insert("mixColor", Color::WHITE);
  propertyMap.Insert("renderingBackend", static_cast<int>(Toolkit::DevelText::DEFAULT_RENDERING_BACKEND));
  propertyMap.Insert("enableMarkup", false);
  propertyMap.Insert("text", "Hello world");
  propertyMap.Insert("fontFamily", "TizenSans");

  Property::Map fontStyleMapSet;
  fontStyleMapSet.Insert("weight", "bold");
  propertyMap.Insert("fontStyle", fontStyleMapSet);

  propertyMap.Insert("pointSize", 12.f);
  propertyMap.Insert("multiLine", true);
  propertyMap.Insert("horizontalAlignment", "CENTER");
  propertyMap.Insert("verticalAlignment", "CENTER");
  propertyMap.Insert("textColor", Color::RED);
  Visual::Base textVisual = factory.CreateVisual(propertyMap);
  textVisual.SetDepthIndex(1);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, textVisual);
  DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 0, TEST_LOCATION);

  dummyControl.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
  dummyControl.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  application.GetScene().Add(dummyControl);
  application.SendNotification();
  application.Render();

  // Create a texture bigger than the maximum allowed by the image atlas. Used to increase coverage.
  propertyMap.Clear();
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::TEXT);
  propertyMap.Insert(TextVisual::Property::ENABLE_MARKUP, true);
  propertyMap.Insert(TextVisual::Property::TEXT, "<font family='TizenSans' size='12'>Hello world</font>");
  propertyMap.Insert(TextVisual::Property::MULTI_LINE, true);

  Property::Map transformMap;
  transformMap.Insert("size", Vector2(0.5f, 0.5f));
  propertyMap.Insert(Visual::Property::TRANSFORM, transformMap);

  textVisual = factory.CreateVisual(propertyMap);
  textVisual.SetDepthIndex(1);

  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, textVisual);
  dummyControl.SetProperty(Actor::Property::SIZE, Vector2(720.f, 640.f));

  application.SendNotification(); // force process events to ensure text visual
  // adds renderer to the dummy control in OnRelayout
  application.Render();

  Renderer        renderer = dummyControl.GetRendererAt(0u);
  Property::Index index    = renderer.GetPropertyIndex("transformSize");

  tet_infoline("Test that the TextVisual has NOT overridden what was set by developer");
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(index), Vector2(0.5f, 0.5f), 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualTextVisualDisableEnable(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualTextVisualDisableEnable Ensure Text visible can be re-enabled");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::TEXT);
  propertyMap.Insert("mixColor", Color::WHITE);
  propertyMap.Insert("renderingBackend", static_cast<int>(Toolkit::DevelText::DEFAULT_RENDERING_BACKEND));
  propertyMap.Insert("enableMarkup", false);
  propertyMap.Insert("text", "Hello world");
  propertyMap.Insert("fontFamily", "TizenSans");

  Property::Map fontStyleMapSet;
  fontStyleMapSet.Insert("weight", "bold");
  propertyMap.Insert("fontStyle", fontStyleMapSet);

  propertyMap.Insert("pointSize", 12.f);
  propertyMap.Insert("multiLine", true);
  propertyMap.Insert("horizontalAlignment", "CENTER");
  propertyMap.Insert("verticalAlignment", "CENTER");
  propertyMap.Insert("textColor", Color::RED);
  Visual::Base textVisual = factory.CreateVisual(propertyMap);
  textVisual.SetDepthIndex(1);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, textVisual);
  DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 0, TEST_LOCATION);

  dummyControl.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
  dummyControl.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  application.GetScene().Add(dummyControl);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 1, TEST_LOCATION);

  dummyImpl.EnableVisual(DummyControl::Property::TEST_VISUAL, false);

  DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 0, TEST_LOCATION);

  dummyImpl.EnableVisual(DummyControl::Property::TEST_VISUAL, true);

  DALI_TEST_EQUALS(dummyControl.GetRendererCount(), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualPremultipliedAlpha(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualPremultipliedAlpha");

  VisualFactory factory = VisualFactory::Get();

  // image visual, test default value ( true )
  {
    Visual::Base imageVisual = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, Visual::IMAGE)
        .Add(ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME));

    Dali::Property::Map visualMap;
    imageVisual.CreatePropertyMap(visualMap);
    Property::Value* value = visualMap.Find(Visual::Property::PREMULTIPLIED_ALPHA);

    // test values
    DALI_TEST_CHECK(value);
    DALI_TEST_EQUALS(value->Get<bool>(), true, TEST_LOCATION);
  }

  // image visual, override premultiplied
  {
    Visual::Base imageVisual = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, Visual::IMAGE)
        .Add(ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME)
        .Add(Visual::Property::PREMULTIPLIED_ALPHA, false));

    Dali::Property::Map visualMap;
    imageVisual.CreatePropertyMap(visualMap);
    Property::Value* value = visualMap.Find(Visual::Property::PREMULTIPLIED_ALPHA);

    // test values
    DALI_TEST_CHECK(value);
    DALI_TEST_EQUALS(value->Get<bool>(), false, TEST_LOCATION);
  }

  // svg visual ( premultiplied alpha by default is true, and cannot change value )
  {
    Visual::Base imageVisual = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, Visual::IMAGE)
        .Add(ImageVisual::Property::URL, TEST_SVG_FILE_NAME));

    Dali::Property::Map visualMap;
    imageVisual.CreatePropertyMap(visualMap);
    Property::Value* value = visualMap.Find(Visual::Property::PREMULTIPLIED_ALPHA);

    // test values
    DALI_TEST_CHECK(value);
    DALI_TEST_EQUALS(value->Get<bool>(), true, TEST_LOCATION);
  }
  {
    Visual::Base imageVisual = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, Visual::IMAGE)
        .Add(ImageVisual::Property::URL, TEST_SVG_FILE_NAME)
        .Add(Visual::Property::PREMULTIPLIED_ALPHA, false));

    Dali::Property::Map visualMap;
    imageVisual.CreatePropertyMap(visualMap);
    Property::Value* value = visualMap.Find(Visual::Property::PREMULTIPLIED_ALPHA);

    // test values
    DALI_TEST_CHECK(value);
    DALI_TEST_EQUALS(value->Get<bool>(), true, TEST_LOCATION);
  }

  // animated vector visual ( premultiplied alpha by default is true, and cannot change value )
  {
    Visual::Base imageVisual = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, Visual::IMAGE)
        .Add(ImageVisual::Property::URL, "something.json"));

    Dali::Property::Map visualMap;
    imageVisual.CreatePropertyMap(visualMap);
    Property::Value* value = visualMap.Find(Visual::Property::PREMULTIPLIED_ALPHA);

    // test values
    DALI_TEST_CHECK(value);
    DALI_TEST_EQUALS(value->Get<bool>(), true, TEST_LOCATION);
  }
  {
    Visual::Base imageVisual = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, Visual::IMAGE)
        .Add(ImageVisual::Property::URL, "something.json")
        .Add(Visual::Property::PREMULTIPLIED_ALPHA, false));

    Dali::Property::Map visualMap;
    imageVisual.CreatePropertyMap(visualMap);
    Property::Value* value = visualMap.Find(Visual::Property::PREMULTIPLIED_ALPHA);

    // test values
    DALI_TEST_CHECK(value);
    DALI_TEST_EQUALS(value->Get<bool>(), true, TEST_LOCATION);
  }

  // text visual ( premultiplied alpha by default is true, and cannot change value )
  {
    Visual::Base textVisual = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, Visual::TEXT)
        .Add(TextVisual::Property::TEXT, "Text"));

    Dali::Property::Map visualMap;
    textVisual.CreatePropertyMap(visualMap);
    Property::Value* value = visualMap.Find(Visual::Property::PREMULTIPLIED_ALPHA);

    // test values
    DALI_TEST_CHECK(value);
    DALI_TEST_EQUALS(value->Get<bool>(), true, TEST_LOCATION);
  }
  {
    Visual::Base textVisual = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, Visual::TEXT)
        .Add(TextVisual::Property::TEXT, "Text")
        .Add(Visual::Property::PREMULTIPLIED_ALPHA, false));

    Dali::Property::Map visualMap;
    textVisual.CreatePropertyMap(visualMap);
    Property::Value* value = visualMap.Find(Visual::Property::PREMULTIPLIED_ALPHA);

    // test values
    DALI_TEST_CHECK(value);
    DALI_TEST_EQUALS(value->Get<bool>(), true, TEST_LOCATION);
  }

  // gradient visual ( premultiplied alpha by default is true, and cannot change value )
  {
    Visual::Base gradientVisual = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, Visual::GRADIENT)
        .Add(GradientVisual::Property::START_POSITION, Vector2(-0.5f, -0.5f))
        .Add(GradientVisual::Property::END_POSITION, Vector2(0.5f, 0.5f))
        .Add(GradientVisual::Property::STOP_COLOR, Property::Array().Add(Color::RED).Add(Vector4(1.0f, 1.0f, 1.0f, 0.5f))));

    Dali::Property::Map visualMap;
    gradientVisual.CreatePropertyMap(visualMap);
    Property::Value* value = visualMap.Find(Visual::Property::PREMULTIPLIED_ALPHA);

    // test values
    DALI_TEST_CHECK(value);
    DALI_TEST_EQUALS(value->Get<bool>(), true, TEST_LOCATION);
  }
  {
    Visual::Base gradientVisual = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, Visual::GRADIENT)
        .Add(GradientVisual::Property::START_POSITION, Vector2(-0.5f, -0.5f))
        .Add(GradientVisual::Property::END_POSITION, Vector2(0.5f, 0.5f))
        .Add(GradientVisual::Property::STOP_COLOR, Property::Array().Add(Color::RED).Add(Vector4(1.0f, 1.0f, 1.0f, 0.5f)))
        .Add(Visual::Property::PREMULTIPLIED_ALPHA, false));

    Dali::Property::Map visualMap;
    gradientVisual.CreatePropertyMap(visualMap);
    Property::Value* value = visualMap.Find(Visual::Property::PREMULTIPLIED_ALPHA);

    // test values
    DALI_TEST_CHECK(value);
    DALI_TEST_EQUALS(value->Get<bool>(), true, TEST_LOCATION);
  }

  // color visual ( premultiplied alpha by default is false, and cannot change value )
  {
    Visual::Base colorVisual = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, Visual::COLOR)
        .Add(ColorVisual::Property::MIX_COLOR, Color::AQUA));

    Dali::Property::Map visualMap;
    colorVisual.CreatePropertyMap(visualMap);
    Property::Value* value = visualMap.Find(Visual::Property::PREMULTIPLIED_ALPHA);

    // test values
    DALI_TEST_CHECK(value);
    DALI_TEST_EQUALS(value->Get<bool>(), false, TEST_LOCATION);
  }
  {
    Visual::Base colorVisual = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, Visual::COLOR)
        .Add(ColorVisual::Property::MIX_COLOR, Color::AQUA)
        .Add(Visual::Property::PREMULTIPLIED_ALPHA, true));

    Dali::Property::Map visualMap;
    colorVisual.CreatePropertyMap(visualMap);
    Property::Value* value = visualMap.Find(Visual::Property::PREMULTIPLIED_ALPHA);

    // test values
    DALI_TEST_CHECK(value);
    DALI_TEST_EQUALS(value->Get<bool>(), false, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliRegisterVisualOrder(void)
{
  ToolkitTestApplication application;
  tet_infoline("Register Visual Order");

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);

  tet_infoline("Register visual, should have depth index of 0.0f");
  Visual::Base testVisual = factory.CreateVisual(propertyMap);
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, testVisual);
  DALI_TEST_EQUALS(testVisual.GetDepthIndex(), 0, TEST_LOCATION);

  tet_infoline("Register more visuals, each added one should have a depth index greater than previous");

  Visual::Base testVisual2 = factory.CreateVisual(propertyMap);
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL2, testVisual2);
  DALI_TEST_CHECK(testVisual2.GetDepthIndex() > testVisual.GetDepthIndex());

  Visual::Base foregroundVisual = factory.CreateVisual(propertyMap);
  dummyImpl.RegisterVisual(DummyControl::Property::FOREGROUND_VISUAL, foregroundVisual);
  DALI_TEST_CHECK(foregroundVisual.GetDepthIndex() > testVisual2.GetDepthIndex());

  Visual::Base focusVisual = factory.CreateVisual(propertyMap);
  dummyImpl.RegisterVisual(DummyControl::Property::FOCUS_VISUAL, focusVisual);
  DALI_TEST_CHECK(focusVisual.GetDepthIndex() > foregroundVisual.GetDepthIndex());

  tet_infoline("Set depth index on a new visual before registering, the depth index should not have been changed");
  Visual::Base labelVisual = factory.CreateVisual(propertyMap);
  labelVisual.SetDepthIndex(-2000);
  dummyImpl.RegisterVisual(DummyControl::Property::LABEL_VISUAL, labelVisual);
  DALI_TEST_EQUALS(labelVisual.GetDepthIndex(), -2000, TEST_LOCATION);

  tet_infoline("Replace visual, the depth index should be the same as what was previously set");
  const int    testVisual2DepthIndex  = testVisual2.GetDepthIndex();
  Visual::Base testVisual2Replacement = factory.CreateVisual(propertyMap);
  DALI_TEST_CHECK(testVisual2Replacement.GetDepthIndex() != testVisual2DepthIndex);
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL2, testVisual2Replacement);
  DALI_TEST_EQUALS(testVisual2Replacement.GetDepthIndex(), testVisual2DepthIndex, TEST_LOCATION);

  tet_infoline("Replace visual and set a depth index on the replacement, the depth index of the replacement should be honoured");
  Visual::Base anotherTestVisual2Replacement = factory.CreateVisual(propertyMap);
  anotherTestVisual2Replacement.SetDepthIndex(2000);
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL2, anotherTestVisual2Replacement);
  DALI_TEST_EQUALS(anotherTestVisual2Replacement.GetDepthIndex(), 2000, TEST_LOCATION);

  dummyControl.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
  application.GetScene().Add(dummyControl);

  END_TEST;
}

int UtcDaliRegisterVisualOrder02(void)
{
  ToolkitTestApplication application;
  tet_infoline("Register Visual Order with Background Set");

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());

  const int backgroundDepthIndex = Toolkit::DepthIndex::BACKGROUND;

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);

  tet_printf("Register a control background visual, should have depth index of %d\n", backgroundDepthIndex);

  dummyControl.SetProperty(Control::Property::BACKGROUND, propertyMap);

  const int TEST_VISUAL_1_DEPTH_INDEX = 0;
  tet_printf("Register visual, should have depth index of %d\n", TEST_VISUAL_1_DEPTH_INDEX);
  Visual::Base testVisual1 = factory.CreateVisual(propertyMap);
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, testVisual1);
  DALI_TEST_EQUALS(testVisual1.GetDepthIndex(), TEST_VISUAL_1_DEPTH_INDEX, TEST_LOCATION);

  tet_printf("Register another visual, should have a depth index greater than previous(%d)\n", TEST_VISUAL_1_DEPTH_INDEX);
  Visual::Base testVisual2 = factory.CreateVisual(propertyMap);
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL2, testVisual2);
  DALI_TEST_CHECK(testVisual2.GetDepthIndex() > testVisual1.GetDepthIndex());

  dummyControl.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
  application.GetScene().Add(dummyControl);

  END_TEST;
}

int UtcDaliRegisterVisualWithDepthIndex(void)
{
  ToolkitTestApplication application;
  tet_infoline("Register a Visual With Depth Index");

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);

  tet_infoline("Register a visual with a depth index, it should be enabled by default too");
  Visual::Base testVisual = factory.CreateVisual(propertyMap);
  DevelControl::RegisterVisual(dummyImpl, DummyControl::Property::TEST_VISUAL, testVisual, 203);
  DALI_TEST_EQUALS(testVisual.GetDepthIndex(), 203, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelControl::IsVisualEnabled(dummyImpl, DummyControl::Property::TEST_VISUAL), true, TEST_LOCATION);

  tet_infoline("Register another visual with a depth index and it disabled");
  Visual::Base testVisual2 = factory.CreateVisual(propertyMap);
  DevelControl::RegisterVisual(dummyImpl, DummyControl::Property::TEST_VISUAL2, testVisual2, false, 450);
  DALI_TEST_EQUALS(testVisual2.GetDepthIndex(), 450, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelControl::IsVisualEnabled(dummyImpl, DummyControl::Property::TEST_VISUAL2), false, TEST_LOCATION);

  tet_infoline("Register another visual with a depth index and it enabled using the enabled API");
  Visual::Base testVisual3 = factory.CreateVisual(propertyMap);
  DevelControl::RegisterVisual(dummyImpl, DummyControl::Property::TEST_VISUAL2, testVisual3, true, 300);
  DALI_TEST_EQUALS(testVisual3.GetDepthIndex(), 300, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelControl::IsVisualEnabled(dummyImpl, DummyControl::Property::TEST_VISUAL2), true, TEST_LOCATION);

  dummyControl.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
  application.GetScene().Add(dummyControl);

  END_TEST;
}

int UtcDaliSvgVisualCustomShader(void)
{
  ToolkitTestApplication application;
  tet_infoline("SvgVisual with custom shader");

  VisualFactory     factory = VisualFactory::Get();
  Property::Map     properties;
  Property::Map     shader;
  const std::string vertexShader                                   = "Foobar";
  const std::string fragmentShader                                 = "Foobar";
  shader[Dali::Toolkit::Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShader;
  shader[Dali::Toolkit::Visual::Shader::Property::VERTEX_SHADER]   = vertexShader;

  properties[Visual::Property::TYPE]     = Visual::IMAGE;
  properties[Visual::Property::SHADER]   = shader;
  properties[ImageVisual::Property::URL] = TEST_SVG_FILE_NAME;

  Visual::Base visual = factory.CreateVisual(properties);

  // trigger creation through setting on stage
  DummyControl        dummy     = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
  dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  application.GetScene().Add(dummy);

  application.SendNotification();
  application.Render();

  // Wait for loading & rasterization
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  Renderer        renderer = dummy.GetRendererAt(0);
  Shader          shader2  = renderer.GetShader();
  Property::Value value    = shader2.GetProperty(Shader::Property::PROGRAM);
  Property::Map*  map      = value.GetMap();
  DALI_TEST_CHECK(map);

  Property::Value* fragment = map->Find("fragment"); // fragment key name from shader-impl.cpp
  DALI_TEST_EQUALS(fragmentShader, fragment->Get<std::string>(), TEST_LOCATION);

  Property::Value* vertex = map->Find("vertex"); // vertex key name from shader-impl.cpp
  DALI_TEST_EQUALS(vertexShader, vertex->Get<std::string>(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualRoundedCorner(void)
{
  tet_infoline("UtcDaliVisualRoundedCorner");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("cornerRadius", Property::Type::VECTOR4),
      UniformData("cornerRadiusPolicy", Property::Type::FLOAT),
    };

  // image visual
  {
    ToolkitTestApplication  application;
    TestGraphicsController& graphics = application.GetGraphicsController();
    graphics.AddCustomUniforms(customUniforms);

    VisualFactory factory = VisualFactory::Get();
    Property::Map properties;
    float         cornerRadius = 30.0f;

    properties[Visual::Property::TYPE]               = Visual::IMAGE;
    properties[ImageVisual::Property::URL]           = TEST_IMAGE_FILE_NAME;
    properties[DevelVisual::Property::CORNER_RADIUS] = cornerRadius;

    Visual::Base visual = factory.CreateVisual(properties);

    // trigger creation through setting on stage
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(dummy);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", Vector4(cornerRadius, cornerRadius, cornerRadius, cornerRadius)), true, TEST_LOCATION);
    // Default corner radius policy is absolute.
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("cornerRadiusPolicy", Toolkit::Visual::Transform::Policy::ABSOLUTE), true, TEST_LOCATION);
  }

  // color visual 1
  {
    ToolkitTestApplication  application;
    TestGraphicsController& graphics = application.GetGraphicsController();
    graphics.AddCustomUniforms(customUniforms);

    VisualFactory factory = VisualFactory::Get();
    Property::Map properties;
    float         cornerRadius = 30.0f;

    properties[Visual::Property::TYPE]           = Visual::COLOR;
    properties[ColorVisual::Property::MIX_COLOR] = Color::BLUE;
    properties["cornerRadius"]                   = cornerRadius;
    properties["cornerRadiusPolicy"]             = Toolkit::Visual::Transform::Policy::ABSOLUTE;

    Visual::Base visual = factory.CreateVisual(properties);

    // trigger creation through setting on stage
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(dummy);

    application.SendNotification();
    application.Render();

    application.SendNotification();
    application.Render();

    // Currently test with multiple program doesn't work well. will fix another day
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", Vector4(cornerRadius, cornerRadius, cornerRadius, cornerRadius)), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("cornerRadiusPolicy", Toolkit::Visual::Transform::Policy::ABSOLUTE), true, TEST_LOCATION);
  }

  // color visual 2
  {
    ToolkitTestApplication  application;
    TestGraphicsController& graphics = application.GetGraphicsController();
    graphics.AddCustomUniforms(customUniforms);

    VisualFactory factory = VisualFactory::Get();
    Property::Map properties;
    Vector4       cornerRadius(0.5f, 0.5f, 0.5f, 0.3f);

    properties[Visual::Property::TYPE]                      = Visual::COLOR;
    properties[ColorVisual::Property::MIX_COLOR]            = Color::BLUE;
    properties[DevelVisual::Property::CORNER_RADIUS]        = cornerRadius;
    properties[DevelVisual::Property::CORNER_RADIUS_POLICY] = Toolkit::Visual::Transform::Policy::RELATIVE;

    Visual::Base visual = factory.CreateVisual(properties);

    // trigger creation through setting on stage
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(dummy);

    application.SendNotification();
    application.Render();

    application.SendNotification();
    application.Render();

    // Currently test with multiple program doesn't work well. will fix another day
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", cornerRadius), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("cornerRadiusPolicy", Toolkit::Visual::Transform::Policy::RELATIVE), true, TEST_LOCATION);
  }

  // color visual 3 - invalid value
  {
    ToolkitTestApplication  application;
    TestGraphicsController& graphics = application.GetGraphicsController();
    graphics.AddCustomUniforms(customUniforms);

    VisualFactory factory = VisualFactory::Get();
    Property::Map properties;
    Vector4       cornerRadius(30.0f, 30.0f, 30.0f, 20.0f);

    properties[Visual::Property::TYPE]                      = Visual::COLOR;
    properties[ColorVisual::Property::MIX_COLOR]            = Color::BLUE;
    properties[DevelVisual::Property::CORNER_RADIUS]        = cornerRadius;
    properties[DevelVisual::Property::CORNER_RADIUS_POLICY] = -1; // Set an invalid value

    Visual::Base visual = factory.CreateVisual(properties);

    // trigger creation through setting on stage
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(dummy);

    application.SendNotification();
    application.Render();

    application.SendNotification();
    application.Render();

    // Currently test with multiple program doesn't work well. will fix another day
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", cornerRadius), true, TEST_LOCATION);
    // Default corner radius policy is absolute.
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("cornerRadiusPolicy", Toolkit::Visual::Transform::Policy::ABSOLUTE), true, TEST_LOCATION);
  }

  // gradient visual
  {
    ToolkitTestApplication  application;
    TestGraphicsController& graphics = application.GetGraphicsController();
    graphics.AddCustomUniforms(customUniforms);

    VisualFactory factory = VisualFactory::Get();
    Property::Map properties;
    float         cornerRadius = 30.0f;

    properties[Visual::Property::TYPE]                   = Visual::GRADIENT;
    properties[ColorVisual::Property::MIX_COLOR]         = Color::BLUE;
    properties[DevelVisual::Property::CORNER_RADIUS]     = cornerRadius;
    properties[GradientVisual::Property::START_POSITION] = Vector2(0.5f, 0.5f);
    properties[GradientVisual::Property::END_POSITION]   = Vector2(-0.5f, -0.5f);
    properties[GradientVisual::Property::UNITS]          = GradientVisual::Units::USER_SPACE;

    Property::Array stopOffsets;
    stopOffsets.PushBack(0.0f);
    stopOffsets.PushBack(0.6f);
    stopOffsets.PushBack(1.0f);
    properties[GradientVisual::Property::STOP_OFFSET] = stopOffsets;

    Property::Array stopColors;
    stopColors.PushBack(Color::RED);
    stopColors.PushBack(Color::YELLOW);
    stopColors.PushBack(Color::GREEN);
    properties[GradientVisual::Property::STOP_COLOR] = stopColors;

    Visual::Base visual = factory.CreateVisual(properties);

    // trigger creation through setting on stage
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(dummy);

    application.SendNotification();
    application.Render();

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", Vector4(cornerRadius, cornerRadius, cornerRadius, cornerRadius)), true, TEST_LOCATION);
    // Default corner radius policy is absolute.
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("cornerRadiusPolicy", Toolkit::Visual::Transform::Policy::ABSOLUTE), true, TEST_LOCATION);
  }

  // animated image visual
  {
    ToolkitTestApplication  application;
    TestGraphicsController& graphics = application.GetGraphicsController();
    graphics.AddCustomUniforms(customUniforms);

    VisualFactory factory = VisualFactory::Get();
    Property::Map properties;
    Vector4       cornerRadius(24.0f, 23.0f, 22.0f, 21.0f);

    properties[Visual::Property::TYPE]               = Visual::ANIMATED_IMAGE;
    properties[ImageVisual::Property::URL]           = TEST_GIF_FILE_NAME;
    properties[DevelVisual::Property::CORNER_RADIUS] = cornerRadius.x + 10.0f; // Dummy Input
    properties[DevelVisual::Property::CORNER_RADIUS] = cornerRadius;
    properties["cornerRadiusPolicy"]                 = Toolkit::Visual::Transform::Policy::ABSOLUTE;

    Visual::Base visual = factory.CreateVisual(properties);

    // trigger creation through setting on stage
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(dummy);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", cornerRadius), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("cornerRadiusPolicy", Toolkit::Visual::Transform::Policy::ABSOLUTE), true, TEST_LOCATION);
  }

  // vector image visual
  {
    ToolkitTestApplication  application;
    TestGraphicsController& graphics = application.GetGraphicsController();
    graphics.AddCustomUniforms(customUniforms);

    VisualFactory factory = VisualFactory::Get();
    Property::Map properties;
    Vector4       cornerRadius(27.0f, 72.0f, 11.0f, 500.5f);

    properties[Visual::Property::TYPE]               = Visual::SVG;
    properties[ImageVisual::Property::URL]           = TEST_SVG_FILE_NAME;
    properties[DevelVisual::Property::CORNER_RADIUS] = cornerRadius;

    Visual::Base visual = factory.CreateVisual(properties);

    // trigger creation through setting on stage
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(dummy);

    application.SendNotification();
    application.Render();

    // Wait for loading & rasterization
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", cornerRadius), true, TEST_LOCATION);
    // Default corner radius policy is absolute.
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("cornerRadiusPolicy", Toolkit::Visual::Transform::Policy::ABSOLUTE), true, TEST_LOCATION);
  }

  // animated vector image visual
  {
    ToolkitTestApplication  application;
    TestGraphicsController& graphics = application.GetGraphicsController();
    graphics.AddCustomUniforms(customUniforms);

    VisualFactory factory = VisualFactory::Get();
    Property::Map properties;
    float         cornerRadius = 1.3f;

    properties[Visual::Property::TYPE]                      = DevelVisual::ANIMATED_VECTOR_IMAGE;
    properties[ImageVisual::Property::URL]                  = TEST_VECTOR_IMAGE_FILE_NAME;
    properties["cornerRadius"]                              = Vector4(1.0f, 100.0f, 10.0f, 0.1f); // Dummy Input
    properties["cornerRadius"]                              = cornerRadius;
    properties[DevelVisual::Property::CORNER_RADIUS_POLICY] = Toolkit::Visual::Transform::Policy::RELATIVE;
    properties["synchronousLoading"]                        = false;

    Visual::Base visual = factory.CreateVisual(properties);

    // trigger creation through setting on stage
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(dummy);

    application.SendNotification();
    application.Render();

    // Trigger count is 2 - load & render a frame
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", Vector4(cornerRadius, cornerRadius, cornerRadius, cornerRadius)), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("cornerRadiusPolicy", Toolkit::Visual::Transform::Policy::RELATIVE), true, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliVisualBorderline(void)
{
  tet_infoline("UtcDaliVisualBorderline");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("cornerRadius", Property::Type::VECTOR4),
      UniformData("cornerRadiusPolicy", Property::Type::FLOAT),
      UniformData("borderlineWidth", Property::Type::FLOAT),
      UniformData("borderlineColor", Property::Type::VECTOR4),
      UniformData("borderlineOffset", Property::Type::FLOAT),
    };

  // image visual
  {
    ToolkitTestApplication  application;
    TestGraphicsController& graphics = application.GetGraphicsController();
    graphics.AddCustomUniforms(customUniforms);

    VisualFactory factory = VisualFactory::Get();
    Property::Map properties;
    float         cornerRadius    = 5.0f;
    float         borderlineWidth = 30.0f;
    Vector4       borderlineColor(1.0f, 0.0f, 0.0f, 1.0f);
    float         borderlineOffset = 1.0f;

    properties[Visual::Property::TYPE]                   = Visual::IMAGE;
    properties[ImageVisual::Property::URL]               = TEST_IMAGE_FILE_NAME;
    properties[DevelVisual::Property::CORNER_RADIUS]     = cornerRadius;
    properties[DevelVisual::Property::BORDERLINE_WIDTH]  = borderlineWidth;
    properties[DevelVisual::Property::BORDERLINE_COLOR]  = borderlineColor;
    properties[DevelVisual::Property::BORDERLINE_OFFSET] = borderlineOffset;

    Visual::Base visual = factory.CreateVisual(properties);

    // trigger creation through setting on stage
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(dummy);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", Vector4(cornerRadius, cornerRadius, cornerRadius, cornerRadius)), true, TEST_LOCATION);
    // Default corner radius policy is absolute.
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("cornerRadiusPolicy", Toolkit::Visual::Transform::Policy::ABSOLUTE), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineWidth", borderlineWidth), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("borderlineColor", borderlineColor), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineOffset", borderlineOffset), true, TEST_LOCATION);
  }

  // color visual 1
  {
    ToolkitTestApplication  application;
    TestGraphicsController& graphics = application.GetGraphicsController();
    graphics.AddCustomUniforms(customUniforms);

    VisualFactory factory = VisualFactory::Get();
    Property::Map properties;
    Vector4       cornerRadius(23.0f, 2.0f, 3.0f, 2.3f);
    float         borderlineWidth = 30.0f;
    Vector4       borderlineColor(0.5f, 0.4f, 0.3f, 0.2f);
    float         borderlineOffset = -0.4f;

    properties[Visual::Property::TYPE]           = Visual::COLOR;
    properties[ColorVisual::Property::MIX_COLOR] = Color::BLUE;
    properties["cornerRadius"]                   = cornerRadius;
    properties["borderlineWidth"]                = borderlineWidth;
    properties["borderlineColor"]                = borderlineColor;
    properties["borderlineOffset"]               = borderlineOffset;

    Visual::Base visual = factory.CreateVisual(properties);

    // trigger creation through setting on stage
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(dummy);

    application.SendNotification();
    application.Render();

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", cornerRadius), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineWidth", borderlineWidth), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("borderlineColor", borderlineColor), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineOffset", borderlineOffset), true, TEST_LOCATION);
  }

  // color visual 2, default color, default offset
  {
    ToolkitTestApplication  application;
    TestGraphicsController& graphics = application.GetGraphicsController();
    graphics.AddCustomUniforms(customUniforms);

    VisualFactory factory = VisualFactory::Get();
    Property::Map properties;
    float         borderlineWidth = 30.0f;

    properties[Visual::Property::TYPE]                  = Visual::COLOR;
    properties[ColorVisual::Property::MIX_COLOR]        = Color::BLUE;
    properties[DevelVisual::Property::BORDERLINE_WIDTH] = borderlineWidth;

    Visual::Base visual = factory.CreateVisual(properties);

    // trigger creation through setting on stage
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(dummy);

    application.SendNotification();
    application.Render();

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineWidth", borderlineWidth), true, TEST_LOCATION);
    // Default borderline color is BLACK.
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("borderlineColor", Color::BLACK), true, TEST_LOCATION);
    // Default borderline offset is 0.0f.
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineOffset", 0.0f), true, TEST_LOCATION);
  }

  // color visual 3, offset not [-1.0 ~ 1.0], but uniform value is same anyway
  {
    ToolkitTestApplication  application;
    TestGraphicsController& graphics = application.GetGraphicsController();
    graphics.AddCustomUniforms(customUniforms);

    VisualFactory factory = VisualFactory::Get();
    Property::Map properties;
    float         borderlineWidth = 30.0f;
    Vector4       borderlineColor(0.5f, 0.4f, 0.3f, 0.2f);
    float         borderlineOffset = 37.4f;

    properties[Visual::Property::TYPE]           = Visual::COLOR;
    properties[ColorVisual::Property::MIX_COLOR] = Color::BLUE;
    properties["borderlineWidth"]                = borderlineWidth;
    properties["borderlineColor"]                = borderlineColor;
    properties["borderlineOffset"]               = borderlineOffset;

    Visual::Base visual = factory.CreateVisual(properties);

    // trigger creation through setting on stage
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(dummy);

    application.SendNotification();
    application.Render();

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineWidth", borderlineWidth), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("borderlineColor", borderlineColor), true, TEST_LOCATION);
    // NOTE : borderlineOffset will clamp in fragment shader. not visual itself
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineOffset", borderlineOffset), true, TEST_LOCATION);
  }

  // gradient visual
  {
    ToolkitTestApplication  application;
    TestGraphicsController& graphics = application.GetGraphicsController();
    graphics.AddCustomUniforms(customUniforms);

    VisualFactory factory = VisualFactory::Get();
    Property::Map properties;
    float         borderlineWidth = 30.0f;
    float         cornerRadius    = 70.0f;

    properties[Visual::Property::TYPE]                   = Visual::GRADIENT;
    properties[ColorVisual::Property::MIX_COLOR]         = Color::BLUE;
    properties[DevelVisual::Property::CORNER_RADIUS]     = cornerRadius;
    properties[DevelVisual::Property::BORDERLINE_WIDTH]  = borderlineWidth;
    properties[GradientVisual::Property::START_POSITION] = Vector2(0.5f, 0.5f);
    properties[GradientVisual::Property::END_POSITION]   = Vector2(-0.5f, -0.5f);
    properties[GradientVisual::Property::UNITS]          = GradientVisual::Units::USER_SPACE;

    Property::Array stopOffsets;
    stopOffsets.PushBack(0.0f);
    stopOffsets.PushBack(0.6f);
    stopOffsets.PushBack(1.0f);
    properties[GradientVisual::Property::STOP_OFFSET] = stopOffsets;

    Property::Array stopColors;
    stopColors.PushBack(Color::RED);
    stopColors.PushBack(Color::YELLOW);
    stopColors.PushBack(Color::GREEN);
    properties[GradientVisual::Property::STOP_COLOR] = stopColors;

    Visual::Base visual = factory.CreateVisual(properties);

    // trigger creation through setting on stage
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(dummy);

    application.SendNotification();
    application.Render();

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", Vector4(cornerRadius, cornerRadius, cornerRadius, cornerRadius)), true, TEST_LOCATION);
    // Default corner radius policy is absolute.
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("cornerRadiusPolicy", Toolkit::Visual::Transform::Policy::ABSOLUTE), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineWidth", borderlineWidth), true, TEST_LOCATION);
    // Default borderline color is BLACK.
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("borderlineColor", Color::BLACK), true, TEST_LOCATION);
    // Default borderline offset is 0.0f.
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineOffset", 0.0f), true, TEST_LOCATION);
  }

  // animated image visual
  {
    ToolkitTestApplication  application;
    TestGraphicsController& graphics = application.GetGraphicsController();
    graphics.AddCustomUniforms(customUniforms);

    VisualFactory factory = VisualFactory::Get();
    Property::Map properties;
    float         borderlineWidth  = 24.0f;
    float         borderlineOffset = -1.0f;

    properties[Visual::Property::TYPE]                  = Visual::ANIMATED_IMAGE;
    properties[ImageVisual::Property::URL]              = TEST_GIF_FILE_NAME;
    properties[DevelVisual::Property::BORDERLINE_WIDTH] = borderlineWidth + 10.0f; // Dummy Input
    properties[DevelVisual::Property::BORDERLINE_WIDTH] = borderlineWidth;
    properties["borderlineOffset"]                      = borderlineOffset;

    Visual::Base visual = factory.CreateVisual(properties);

    // trigger creation through setting on stage
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(dummy);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineWidth", borderlineWidth), true, TEST_LOCATION);
    // Default borderline color is BLACK.
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("borderlineColor", Color::BLACK), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineOffset", borderlineOffset), true, TEST_LOCATION);
  }

  // vector image visual
  {
    ToolkitTestApplication  application;
    TestGraphicsController& graphics = application.GetGraphicsController();
    graphics.AddCustomUniforms(customUniforms);

    VisualFactory factory = VisualFactory::Get();
    Property::Map properties;
    Vector4       cornerRadius(54.0f, 43.0f, 32.0f, 21.0f);
    float         borderlineWidth = 27.0f;
    Vector4       borderlineColor(0.5f, 0.5f, 0.5f, 0.0f);

    properties[Visual::Property::TYPE]                  = Visual::SVG;
    properties[ImageVisual::Property::URL]              = TEST_SVG_FILE_NAME;
    properties[DevelVisual::Property::CORNER_RADIUS]    = cornerRadius;
    properties[DevelVisual::Property::BORDERLINE_WIDTH] = borderlineWidth;
    properties[DevelVisual::Property::BORDERLINE_COLOR] = borderlineColor;

    Visual::Base visual = factory.CreateVisual(properties);

    // trigger creation through setting on stage
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(dummy);

    application.SendNotification();
    application.Render();

    // Wait for loading & rasterization
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", cornerRadius), true, TEST_LOCATION);
    // Default corner radius policy is absolute.
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("cornerRadiusPolicy", Toolkit::Visual::Transform::Policy::ABSOLUTE), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineWidth", borderlineWidth), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("borderlineColor", borderlineColor), true, TEST_LOCATION);
    // Default borderline offset is 0.0.
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineOffset", 0.0f), true, TEST_LOCATION);
  }

  // animated vector image visual
  {
    ToolkitTestApplication  application;
    TestGraphicsController& graphics = application.GetGraphicsController();
    graphics.AddCustomUniforms(customUniforms);

    VisualFactory factory = VisualFactory::Get();
    Property::Map properties;
    Vector4       cornerRadius(1.3f, 0.0f, 0.4f, 0.2f);
    float         borderlineWidth = 13.0f;
    Vector4       borderlineColor(0.3f, 0.3f, 0.3f, 1.0f);
    float         borderlineOffset = 13.0f;

    properties[Visual::Property::TYPE]                      = DevelVisual::ANIMATED_VECTOR_IMAGE;
    properties[ImageVisual::Property::URL]                  = TEST_VECTOR_IMAGE_FILE_NAME;
    properties["cornerRadius"]                              = cornerRadius;
    properties[DevelVisual::Property::CORNER_RADIUS_POLICY] = Toolkit::Visual::Transform::Policy::RELATIVE;
    properties[DevelVisual::Property::BORDERLINE_WIDTH]     = borderlineWidth;
    properties["borderlineColor"]                           = borderlineColor;
    properties[DevelVisual::Property::BORDERLINE_OFFSET]    = borderlineOffset;
    properties[ImageVisual::Property::SYNCHRONOUS_LOADING]  = false;

    Visual::Base visual = factory.CreateVisual(properties);

    // trigger creation through setting on stage
    DummyControl        dummy     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(dummy);

    application.SendNotification();
    application.Render();

    // Trigger count is 2 - load & render a frame
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", cornerRadius), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("cornerRadiusPolicy", Toolkit::Visual::Transform::Policy::RELATIVE), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineWidth", borderlineWidth), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("borderlineColor", borderlineColor), true, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineOffset", borderlineOffset), true, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliVisualBorderlineBlendModeTest(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualBorderlineBlendModeTest");
  VisualFactory factory = VisualFactory::Get();

  // Case 1 : Test which doesn't support borderline feature.
  {
    tet_printf("Test Unsupported visual type\n");
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::BORDER);
    propertyMap.Insert(BorderVisual::Property::COLOR, Color::BLUE);
    propertyMap.Insert(DevelVisual::Property::BORDERLINE_WIDTH, 1.0f);
    Visual::Base borderVisual = factory.CreateVisual(propertyMap);

    DummyControl        actor     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, borderVisual);
    actor.SetProperty(Actor::Property::SIZE, Vector2(2000.f, 2000.f));
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(actor);

    DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);

    Renderer renderer = actor.GetRendererAt(0);

    Property::Value blendModeValue = renderer.GetProperty(Renderer::Property::BLEND_MODE);
    // Visual::BORDER doesn't support BORDERLINE. BlendMode is AUTO.
    DALI_TEST_EQUALS(blendModeValue.Get<int>(), (int)BlendMode::AUTO, TEST_LOCATION);

    application.GetScene().Remove(actor);
  }

  // Case 2 : Test which support borderline feature.
  {
    tet_printf("Test normal case\n");
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
    propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
    propertyMap.Insert(DevelVisual::Property::BORDERLINE_WIDTH, 1.0f);
    Visual::Base colorVisual = factory.CreateVisual(propertyMap);

    DummyControl        actor     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, colorVisual);
    actor.SetProperty(Actor::Property::SIZE, Vector2(2000.f, 2000.f));
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(actor);

    DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);

    Renderer renderer = actor.GetRendererAt(0);

    Property::Value blendModeValue = renderer.GetProperty(Renderer::Property::BLEND_MODE);
    // Visual::COLOR support BORDERLINE. BlendMode is ON_WITHOUT_CULL.
    DALI_TEST_EQUALS(blendModeValue.Get<int>(), (int)BlendMode::ON_WITHOUT_CULL, TEST_LOCATION);

    application.GetScene().Remove(actor);
  }

  // Case 3 : Test which animated borderline.
  {
    tet_printf("Test borderline animate case\n");
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
    propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
    Visual::Base colorVisual = factory.CreateVisual(propertyMap);

    DummyControl        actor     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, colorVisual);
    actor.SetProperty(Actor::Property::SIZE, Vector2(2000.f, 2000.f));
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(actor);

    DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);

    Renderer renderer = actor.GetRendererAt(0);

    Property::Value blendModeValue = renderer.GetProperty(Renderer::Property::BLEND_MODE);
    // BlendMode is AUTO.
    DALI_TEST_EQUALS(blendModeValue.Get<int>(), (int)BlendMode::AUTO, TEST_LOCATION);

    Animation animation = Animation::New(0.1f);
    animation.AnimateTo(DevelControl::GetVisualProperty(actor, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_WIDTH), 1.0f);
    animation.Play();

    application.SendNotification();
    application.Render();
    application.Render(101u); // End of animation

    blendModeValue = renderer.GetProperty(Renderer::Property::BLEND_MODE);
    // BlendMode is ON_WITHOUT_CULL.
    DALI_TEST_EQUALS(blendModeValue.Get<int>(), (int)BlendMode::ON_WITHOUT_CULL, TEST_LOCATION);

    Animation revanimation = Animation::New(0.1f);
    revanimation.AnimateTo(DevelControl::GetVisualProperty(actor, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_WIDTH), 0.0f);
    revanimation.Play();

    application.SendNotification();
    application.Render();
    application.Render(101u); // End of animation

    blendModeValue = renderer.GetProperty(Renderer::Property::BLEND_MODE);
    // BlendMode is still ON_WITHOUT_CULL.
    DALI_TEST_EQUALS(blendModeValue.Get<int>(), (int)BlendMode::ON_WITHOUT_CULL, TEST_LOCATION);

    application.GetScene().Remove(actor);
  }

  // Case 4 : Test which animated corner radius occur.
  {
    tet_printf("Test borderline animate case\n");
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
    propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
    propertyMap.Insert(DevelVisual::Property::BORDERLINE_WIDTH, 1.0f);
    Visual::Base colorVisual = factory.CreateVisual(propertyMap);

    DummyControl        actor     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, colorVisual);
    actor.SetProperty(Actor::Property::SIZE, Vector2(2000.f, 2000.f));
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(actor);

    DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);

    Renderer renderer = actor.GetRendererAt(0);

    Property::Value blendModeValue = renderer.GetProperty(Renderer::Property::BLEND_MODE);
    // BlendMode is ON_WITHOUT_CULL.
    DALI_TEST_EQUALS(blendModeValue.Get<int>(), (int)BlendMode::ON_WITHOUT_CULL, TEST_LOCATION);

    Animation animation = Animation::New(0.1f);
    animation.AnimateTo(DevelControl::GetVisualProperty(actor, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::CORNER_RADIUS), Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    animation.Play();

    application.SendNotification();
    application.Render();
    application.Render(101u); // End of animation

    blendModeValue = renderer.GetProperty(Renderer::Property::BLEND_MODE);
    // BlendMode is ON_WITHOUT_CULL.
    DALI_TEST_EQUALS(blendModeValue.Get<int>(), (int)BlendMode::ON_WITHOUT_CULL, TEST_LOCATION);

    application.GetScene().Remove(actor);
  }

  END_TEST;
}

int UtcDaliVisualBorderlineColorAnimateTest(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualBorderlineColorAnimateTest color");

  TestGraphicsController&         graphics = application.GetGraphicsController();
  static std::vector<UniformData> customUniforms =
    {
      UniformData("mixColor", Property::Type::VECTOR3),
      UniformData("cornerRadius", Property::Type::VECTOR4),
      UniformData("cornerRadiusPolicy", Property::Type::FLOAT),
      UniformData("borderlineWidth", Property::Type::FLOAT),
      UniformData("borderlineColor", Property::Type::VECTOR4),
      UniformData("borderlineOffset", Property::Type::FLOAT),
    };
  graphics.AddCustomUniforms(customUniforms);

  {
    const Vector3 INITIAL_MIX_COLOR(1.0f, 0.0f, 1.0f);
    const float   INITIAL_MIX_OPACITY(0.5f);
    const Vector4 INITIAL_BORDERLINE_COLOR(0.0f, 1.0f, 0.0f, 1.0f);
    const float   INITIAL_ACTOR_OPACITY(1.0f);
    const Vector3 TARGET_MIX_COLOR(1.0f, 0.0f, 0.0f);
    const float   TARGET_MIX_OPACITY(0.8f);
    const Vector4 TARGET_BORDERLINE_COLOR(1.0f, 0.0f, 1.0f, 0.2f);
    const float   TARGET_ACTOR_OPACITY(0.5f);

    VisualFactory factory = VisualFactory::Get();
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
    propertyMap.Insert(Visual::Property::MIX_COLOR, INITIAL_MIX_COLOR);
    propertyMap.Insert(Visual::Property::OPACITY, INITIAL_MIX_OPACITY);
    propertyMap.Insert(DevelVisual::Property::BORDERLINE_WIDTH, 1.0f);
    propertyMap.Insert(DevelVisual::Property::BORDERLINE_COLOR, INITIAL_BORDERLINE_COLOR);
    Visual::Base visual = factory.CreateVisual(propertyMap);

    DummyControl        actor     = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
    actor.SetProperty(Actor::Property::SIZE, Vector2(2000.f, 2000.f));
    actor.SetProperty(Actor::Property::OPACITY, INITIAL_ACTOR_OPACITY);
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    application.GetScene().Add(actor);

    DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);

    Animation animation = Animation::New(4.0f);
    animation.AnimateTo(DevelControl::GetVisualProperty(actor, DummyControl::Property::TEST_VISUAL, Visual::Property::MIX_COLOR), TARGET_MIX_COLOR);
    animation.AnimateTo(DevelControl::GetVisualProperty(actor, DummyControl::Property::TEST_VISUAL, Visual::Property::OPACITY), TARGET_MIX_OPACITY);
    animation.AnimateTo(DevelControl::GetVisualProperty(actor, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_COLOR), TARGET_BORDERLINE_COLOR);
    animation.AnimateTo(Property(actor, Actor::Property::OPACITY), TARGET_ACTOR_OPACITY);
    animation.Play();

    TestGlAbstraction& glAbstraction = application.GetGlAbstraction();

    application.SendNotification();
    application.Render(0);
    application.Render(2000u); // halfway point
    application.SendNotification();

    Vector3 halfwayMixColor        = (INITIAL_MIX_COLOR + TARGET_MIX_COLOR) * 0.5f;
    float   halfwayMixOpacity      = (INITIAL_MIX_OPACITY + TARGET_MIX_OPACITY) * 0.5f;
    Vector4 halfwayBorderlineColor = (INITIAL_BORDERLINE_COLOR + TARGET_BORDERLINE_COLOR) * 0.5f;
    float   halfwayActorOpacity    = (INITIAL_ACTOR_OPACITY + TARGET_ACTOR_OPACITY) * 0.5f;
    halfwayMixOpacity *= halfwayActorOpacity;
    DALI_TEST_EQUALS(glAbstraction.CheckUniformValue<Vector3>("mixColor", halfwayMixColor), true, TEST_LOCATION);
    DALI_TEST_EQUALS(glAbstraction.CheckUniformValue<Vector4>("uColor", Vector4(1.0f, 1.0f, 1.0f, halfwayMixOpacity)), true, TEST_LOCATION);
    DALI_TEST_EQUALS(glAbstraction.CheckUniformValue<Vector4>("uActorColor", Vector4(1.0f, 1.0f, 1.0f, halfwayActorOpacity)), true, TEST_LOCATION);
    DALI_TEST_EQUALS(glAbstraction.CheckUniformValue<Vector4>("borderlineColor", halfwayBorderlineColor), true, TEST_LOCATION);

    application.Render(2001u);      // go past end
    application.SendNotification(); // Trigger signals

    DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR), Vector4(1.0f, 1.0f, 1.0f, TARGET_ACTOR_OPACITY), TEST_LOCATION);
    DALI_TEST_EQUALS(glAbstraction.CheckUniformValue<Vector3>("mixColor", TARGET_MIX_COLOR), true, TEST_LOCATION);
    DALI_TEST_EQUALS(glAbstraction.CheckUniformValue<Vector4>("uColor", Vector4(1.0f, 1.0f, 1.0f, TARGET_MIX_OPACITY * TARGET_ACTOR_OPACITY)), true, TEST_LOCATION);
    DALI_TEST_EQUALS(glAbstraction.CheckUniformValue<Vector4>("uActorColor", Vector4(1.0f, 1.0f, 1.0f, TARGET_ACTOR_OPACITY)), true, TEST_LOCATION);
    DALI_TEST_EQUALS(glAbstraction.CheckUniformValue<Vector4>("borderlineColor", TARGET_BORDERLINE_COLOR), true, TEST_LOCATION);

    actor.Unparent();
  }

  END_TEST;
}

int UtcDaliColorVisualBlurRadius(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliColorVisualBlurRadius");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("blurRadius", Property::Type::FLOAT),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  VisualFactory factory = VisualFactory::Get();
  Property::Map properties;
  float         blurRadius = 20.0f;

  properties[Visual::Property::TYPE]           = Visual::COLOR;
  properties[ColorVisual::Property::MIX_COLOR] = Color::BLUE;
  properties["blurRadius"]                     = blurRadius;

  Visual::Base visual = factory.CreateVisual(properties);

  // trigger creation through setting on stage
  DummyControl        dummy     = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
  dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  application.GetScene().Add(dummy);

  application.SendNotification();
  application.Render();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("blurRadius", blurRadius), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualGetType(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetType");

  VisualFactory factory = VisualFactory::Get();

  {
    Property::Map properties;
    properties[Visual::Property::TYPE] = Visual::BORDER;
    Visual::Base visual                = factory.CreateVisual(properties);

    DALI_TEST_CHECK(visual.GetType() == Visual::BORDER);
  }

  {
    Property::Map properties;
    properties[Visual::Property::TYPE] = Visual::COLOR;
    Visual::Base visual                = factory.CreateVisual(properties);

    DALI_TEST_CHECK(visual.GetType() == Visual::COLOR);
  }

  {
    Property::Map properties;
    properties[Visual::Property::TYPE]                   = Visual::GRADIENT;
    properties[GradientVisual::Property::START_POSITION] = Vector2(-1.f, -1.f);
    properties[GradientVisual::Property::END_POSITION]   = Vector2(1.f, 1.f);
    properties[GradientVisual::Property::STOP_OFFSET]    = Vector2(0.f, 1.f);
    // propertyMap.Insert( GradientVisual::Property::SPREAD_METHOD, GradientVisual::SpreadMethod::REPEAT) ;
    Property::Array stopColors;
    stopColors.PushBack(Color::RED);
    stopColors.PushBack(Color::GREEN);
    properties[GradientVisual::Property::STOP_COLOR] = stopColors;
    Visual::Base visual                              = factory.CreateVisual(properties);

    DALI_TEST_CHECK(visual.GetType() == Visual::GRADIENT);
  }

  {
    Property::Map properties;
    properties[Visual::Property::TYPE] = Visual::IMAGE;
    properties.Insert(ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME);
    Visual::Base visual = factory.CreateVisual(properties);

    DALI_TEST_CHECK(visual.GetType() == Visual::IMAGE);
  }

  {
    Property::Map properties;
    properties[Visual::Property::TYPE] = Visual::MESH;
    Visual::Base visual                = factory.CreateVisual(properties);

    DALI_TEST_CHECK(visual.GetType() == Visual::MESH);
  }

  {
    Property::Map properties;
    properties[Visual::Property::TYPE]           = Visual::PRIMITIVE;
    properties[PrimitiveVisual::Property::SHAPE] = PrimitiveVisual::Shape::CUBE;
    Visual::Base visual                          = factory.CreateVisual(properties);

    DALI_TEST_CHECK(visual.GetType() == Visual::PRIMITIVE);
  }

  {
    Property::Map properties;
    properties[Visual::Property::TYPE] = Visual::WIREFRAME;
    Visual::Base visual                = factory.CreateVisual(properties);

    DALI_TEST_CHECK(visual.GetType() == Visual::WIREFRAME);
  }

  {
    Property::Map properties;
    properties[Visual::Property::TYPE] = Visual::TEXT;
    Visual::Base visual                = factory.CreateVisual(properties);

    DALI_TEST_CHECK(visual.GetType() == Visual::TEXT);
  }

  {
    Property::Map properties;
    properties[Visual::Property::TYPE]     = Visual::N_PATCH;
    properties[ImageVisual::Property::URL] = TEST_NPATCH_FILE_NAME;
    Visual::Base visual                    = factory.CreateVisual(properties);

    DALI_TEST_CHECK(visual.GetType() == Visual::N_PATCH);
  }

  {
    Property::Map properties;
    properties[Visual::Property::TYPE]     = Visual::SVG;
    properties[ImageVisual::Property::URL] = TEST_SVG_FILE_NAME;
    Visual::Base visual                    = factory.CreateVisual(properties);

    DALI_TEST_CHECK(visual.GetType() == Visual::SVG);
  }

  {
    Property::Map properties;
    properties[Visual::Property::TYPE]     = Visual::ANIMATED_IMAGE;
    properties[ImageVisual::Property::URL] = TEST_GIF_FILE_NAME;
    Visual::Base visual                    = factory.CreateVisual(properties);

    DALI_TEST_CHECK(visual.GetType() == Visual::ANIMATED_IMAGE);
  }

  {
    Property::Map properties;
    properties[Visual::Property::TYPE] = DevelVisual::ANIMATED_GRADIENT;
    Visual::Base visual                = factory.CreateVisual(properties);

    DALI_TEST_CHECK(visual.GetType() == static_cast<Visual::Type>(DevelVisual::ANIMATED_GRADIENT));
  }

  {
    Property::Map properties;
    properties[Visual::Property::TYPE]     = DevelVisual::ANIMATED_VECTOR_IMAGE;
    properties[ImageVisual::Property::URL] = TEST_VECTOR_IMAGE_FILE_NAME;
    Visual::Base visual                    = factory.CreateVisual(properties);

    DALI_TEST_CHECK(visual.GetType() == static_cast<Visual::Type>(DevelVisual::ANIMATED_VECTOR_IMAGE));
  }

  {
    Property::Map properties;
    properties[Visual::Property::TYPE] = DevelVisual::ARC;
    Visual::Base visual                = factory.CreateVisual(properties);

    DALI_TEST_CHECK(visual.GetType() == static_cast<Visual::Type>(DevelVisual::ARC));
  }

  END_TEST;
}

int UtcDaliVisualGetVisualProperty01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetVisualProperty01: Test animatable property, Visual::Base, ColorVisual");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("mixColor", Property::Type::VECTOR3),
      UniformData("offset", Property::Type::VECTOR2),
      UniformData("size", Property::Type::VECTOR2),
      UniformData("cornerRadius", Property::Type::VECTOR4),
      UniformData("blurRadius", Property::Type::FLOAT),
      UniformData("borderlineWidth", Property::Type::FLOAT),
      UniformData("borderlineColor", Property::Type::VECTOR4),
      UniformData("borderlineOffset", Property::Type::FLOAT)};

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(Visual::Property::MIX_COLOR, Color::BLUE);
  propertyMap.Insert(DevelVisual::Property::CORNER_RADIUS, Vector4(10.0f, 0.0f, 2.0f, 4.0f));
  propertyMap.Insert(DevelVisual::Property::CORNER_RADIUS_POLICY, Toolkit::Visual::Transform::Policy::RELATIVE);
  propertyMap.Insert(DevelColorVisual::Property::BLUR_RADIUS, 20.0f);
  propertyMap.Insert(DevelVisual::Property::BORDERLINE_WIDTH, 20.0f);
  propertyMap.Insert(DevelVisual::Property::BORDERLINE_COLOR, Color::RED);
  propertyMap.Insert(DevelVisual::Property::BORDERLINE_OFFSET, 1.0f);
  Visual::Base colorVisual = factory.CreateVisual(propertyMap);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, colorVisual);
  dummyControl[Actor::Property::SIZE] = Vector2(200.f, 200.f);
  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  Vector3 targetColor(1.0f, 1.0f, 1.0f);
  Vector2 targetOffset(0.05f, 0.05f);
  Vector2 targetSize(1.1f, 1.1f);
  float   targetOpacity = 0.5f;
  Vector4 targetCornerRadius(0.0f, 0.0f, 0.0f, 0.0f);
  float   targetBlurRadius      = 10.0f;
  float   targetBorderlineWidth = 25.0f;
  Vector4 targetBorderlineColor(1.0f, 1.0f, 1.0f, 1.0f);
  float   targetBorderlineOffset = -1.0f;

  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, Visual::Property::MIX_COLOR), targetColor);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, Visual::Property::OPACITY), targetOpacity);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, Visual::Transform::Property::OFFSET), targetOffset);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, Visual::Transform::Property::SIZE), targetSize);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::CORNER_RADIUS), targetCornerRadius);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelColorVisual::Property::BLUR_RADIUS), targetBlurRadius);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_WIDTH), targetBorderlineWidth);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_COLOR), targetBorderlineColor);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_OFFSET), targetBorderlineOffset);
  animation.Play();

  application.SendNotification();
  application.Render();
  application.Render(1001u); // End of animation

  Property::Map resultMap;
  colorVisual.CreatePropertyMap(resultMap);

  // Test property values: they should be updated
  Property::Value* colorValue = resultMap.Find(ColorVisual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(colorValue);
  DALI_TEST_EQUALS(colorValue->Get<Vector4>(), Vector4(targetColor.r, targetColor.g, targetColor.b, targetOpacity), TEST_LOCATION);

  Property::Value*     transformValue = resultMap.Find(Dali::Toolkit::Visual::Property::TRANSFORM);
  Dali::Property::Map* transformMap   = transformValue->GetMap();
  DALI_TEST_CHECK(transformMap);

  Property::Value* offsetValue = transformMap->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(offsetValue);
  DALI_TEST_EQUALS(offsetValue->Get<Vector2>(), targetOffset, TEST_LOCATION);

  Property::Value* sizeValue = transformMap->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(sizeValue);
  DALI_TEST_EQUALS(sizeValue->Get<Vector2>(), targetSize, TEST_LOCATION);

  Property::Value* cornerRadiusValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), targetCornerRadius, TEST_LOCATION);

  Property::Value* blurRadiusValue = resultMap.Find(DevelColorVisual::Property::BLUR_RADIUS, Property::FLOAT);
  DALI_TEST_CHECK(blurRadiusValue);
  DALI_TEST_EQUALS(blurRadiusValue->Get<float>(), targetBlurRadius, TEST_LOCATION);

  Property::Value* borderlineWidthValue = resultMap.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(borderlineWidthValue);
  DALI_TEST_EQUALS(borderlineWidthValue->Get<float>(), targetBorderlineWidth, TEST_LOCATION);

  Property::Value* borderlineColorValue = resultMap.Find(DevelVisual::Property::BORDERLINE_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(borderlineColorValue);
  DALI_TEST_EQUALS(borderlineColorValue->Get<Vector4>(), targetBorderlineColor, TEST_LOCATION);

  Property::Value* borderlineOffsetValue = resultMap.Find(DevelVisual::Property::BORDERLINE_OFFSET, Property::FLOAT);
  DALI_TEST_CHECK(borderlineOffsetValue);
  DALI_TEST_EQUALS(borderlineOffsetValue->Get<float>(), targetBorderlineOffset, TEST_LOCATION);

  // Test uniform values
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector3>("mixColor", targetColor), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector2>("offset", targetOffset), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector2>("size", targetSize), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", targetCornerRadius), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("blurRadius", targetBlurRadius), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineWidth", targetBorderlineWidth), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("borderlineColor", targetBorderlineColor), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineOffset", targetBorderlineOffset), true, TEST_LOCATION);

  // Test unregistered visual
  Property property3 = DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL2, Visual::Property::MIX_COLOR);
  DALI_TEST_CHECK(!property3.object);
  DALI_TEST_CHECK(property3.propertyIndex == Property::INVALID_INDEX);

  END_TEST;
}

int UtcDaliVisualGetVisualProperty02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetVisualProperty02: Test animatable property, ColorVisual");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("mixColor", Property::Type::VECTOR3),
      UniformData("offset", Property::Type::VECTOR2),
      UniformData("size", Property::Type::VECTOR2),
      UniformData("cornerRadius", Property::Type::VECTOR4),
      UniformData("borderlineWidth", Property::Type::FLOAT),
      UniformData("borderlineCOlor", Property::Type::VECTOR4),
      UniformData("borderlineOffset", Property::Type::FLOAT),
      UniformData("blurRadius", Property::Type::FLOAT),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  Visual::Base colorVisual = factory.CreateVisual(propertyMap);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, colorVisual);
  dummyControl[Actor::Property::SIZE] = Vector2(200.f, 200.f);
  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  Vector3 targetColor(1.0f, 1.0f, 1.0f);
  Vector2 targetOffset(0.05f, 0.05f);
  Vector2 targetSize(1.1f, 1.1f);
  float   targetOpacity = 0.5f;
  Vector4 targetCornerRadius(20.0f, 0.0f, 20.0f, 0.0f);
  float   targetBorderlineWidth = 77.7f;
  Vector4 targetBorderlineColor(0.4f, 0.2f, 0.3f, 0.9f);
  float   targetBorderlineOffset = 1.0f;
  float   targetBlurRadius       = 10.0f;

  // Should work when the properties are not set before
  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, "mixColor"), targetColor);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, "opacity"), targetOpacity);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, "offset"), targetOffset);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, "size"), targetSize);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, "cornerRadius"), targetCornerRadius);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, "borderlineWidth"), targetBorderlineWidth);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, "borderlineColor"), targetBorderlineColor);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, "borderlineOffset"), targetBorderlineOffset);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, "blurRadius"), targetBlurRadius);
  animation.Play();

  application.SendNotification();
  application.Render();
  application.Render(1001u); // End of animation

  Property::Map resultMap;
  colorVisual.CreatePropertyMap(resultMap);

  // Test property values: they should be updated
  Property::Value* colorValue = resultMap.Find(ColorVisual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(colorValue);
  DALI_TEST_EQUALS(colorValue->Get<Vector4>(), Vector4(targetColor.r, targetColor.g, targetColor.b, targetOpacity), TEST_LOCATION);

  Property::Value*     transformValue = resultMap.Find(Dali::Toolkit::Visual::Property::TRANSFORM);
  Dali::Property::Map* transformMap   = transformValue->GetMap();
  DALI_TEST_CHECK(transformMap);

  Property::Value* offsetValue = transformMap->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(offsetValue);
  DALI_TEST_EQUALS(offsetValue->Get<Vector2>(), targetOffset, TEST_LOCATION);

  Property::Value* sizeValue = transformMap->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(sizeValue);
  DALI_TEST_EQUALS(sizeValue->Get<Vector2>(), targetSize, TEST_LOCATION);

  Property::Value* cornerRadiusValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), targetCornerRadius, TEST_LOCATION);

  Property::Value* borderlineWidthValue = resultMap.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(borderlineWidthValue);
  DALI_TEST_EQUALS(borderlineWidthValue->Get<float>(), targetBorderlineWidth, TEST_LOCATION);

  Property::Value* borderlineColorValue = resultMap.Find(DevelVisual::Property::BORDERLINE_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(borderlineColorValue);
  DALI_TEST_EQUALS(borderlineColorValue->Get<Vector4>(), targetBorderlineColor, TEST_LOCATION);

  Property::Value* borderlineOffsetValue = resultMap.Find(DevelVisual::Property::BORDERLINE_OFFSET, Property::FLOAT);
  DALI_TEST_CHECK(borderlineOffsetValue);
  DALI_TEST_EQUALS(borderlineOffsetValue->Get<float>(), targetBorderlineOffset, TEST_LOCATION);

  Property::Value* blurRadiusValue = resultMap.Find(DevelColorVisual::Property::BLUR_RADIUS, Property::FLOAT);
  DALI_TEST_CHECK(blurRadiusValue);
  DALI_TEST_EQUALS(blurRadiusValue->Get<float>(), targetBlurRadius, TEST_LOCATION);

  // Test uniform values
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector3>("mixColor", targetColor), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector2>("offset", targetOffset), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector2>("size", targetSize), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", targetCornerRadius), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("blurRadius", targetBlurRadius), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualGetVisualProperty03(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetVisualProperty03: Test animatable property, ImageVisual");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("pixelArea", Property::Type::VECTOR4),
      UniformData("cornerRadius", Property::Type::VECTOR4),
      UniformData("borderlineWidth", Property::Type::FLOAT),
      UniformData("borderlineColor", Property::Type::VECTOR4),
      UniformData("borderlineOffset", Property::Type::FLOAT),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME);

  Visual::Base imageVisual = factory.CreateVisual(propertyMap);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual);
  dummyControl[Actor::Property::SIZE] = Vector2(200.f, 200.f);
  application.GetScene().Add(dummyControl);

  // Wait for image loading
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  float   targetOpacity = 0.5f;
  Vector4 targetPixelArea(0.0f, 1.0f, 2.0f, -0.5f);
  Vector4 targetCornerRadius(20.0f, 20.0f, 0.0f, 0.0f);
  float   targetBorderlineWidth = 10.0f;
  Vector4 targetBorderlineColor(1.0f, 0.0f, 1.0f, 0.5f);
  float   targetBorderlineOffset = -1.5f;

  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, Visual::Property::OPACITY), targetOpacity);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, ImageVisual::Property::PIXEL_AREA), targetPixelArea);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::CORNER_RADIUS), targetCornerRadius);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_WIDTH), targetBorderlineWidth);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_COLOR), targetBorderlineColor);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_OFFSET), targetBorderlineOffset);
  animation.Play();

  application.SendNotification();
  application.Render();
  application.Render(1001u); // End of animation

  Property::Map resultMap;
  imageVisual.CreatePropertyMap(resultMap);

  // Test property values: they should be updated
  Property::Value* colorValue = resultMap.Find(Visual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(colorValue);
  DALI_TEST_EQUALS(colorValue->Get<Vector4>(), Vector4(1.0f, 1.0f, 1.0f, targetOpacity), TEST_LOCATION);

  Property::Value* pixelAreaValue = resultMap.Find(ImageVisual::Property::PIXEL_AREA, Property::VECTOR4);
  DALI_TEST_CHECK(pixelAreaValue);
  DALI_TEST_EQUALS(pixelAreaValue->Get<Vector4>(), targetPixelArea, TEST_LOCATION);

  Property::Value* cornerRadiusValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), targetCornerRadius, TEST_LOCATION);

  Property::Value* borderlineWidthValue = resultMap.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(borderlineWidthValue);
  DALI_TEST_EQUALS(borderlineWidthValue->Get<float>(), targetBorderlineWidth, TEST_LOCATION);

  Property::Value* borderlineColorValue = resultMap.Find(DevelVisual::Property::BORDERLINE_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(borderlineColorValue);
  DALI_TEST_EQUALS(borderlineColorValue->Get<Vector4>(), targetBorderlineColor, TEST_LOCATION);

  Property::Value* borderlineOffsetValue = resultMap.Find(DevelVisual::Property::BORDERLINE_OFFSET, Property::FLOAT);
  DALI_TEST_CHECK(borderlineOffsetValue);
  DALI_TEST_EQUALS(borderlineOffsetValue->Get<float>(), targetBorderlineOffset, TEST_LOCATION);

  // Test uniform value
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("pixelArea", targetPixelArea), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", targetCornerRadius), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineWidth", targetBorderlineWidth), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("borderlineColor", targetBorderlineColor), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineOffset", targetBorderlineOffset), true, TEST_LOCATION);

  // Test non-animatable index, for coverage.
  DALI_TEST_EQUALS(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, ImageVisual::Property::URL).propertyIndex, Property::INVALID_INDEX, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualGetVisualProperty04(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetVisualProperty04: Test animatable property, GradientVisual");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("cornerRadius", Property::Type::VECTOR4),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  Vector2         start(-1.f, -1.f);
  Vector2         end(1.f, 1.f);
  Property::Array stopColors;
  stopColors.PushBack(Color::RED);
  stopColors.PushBack(Color::GREEN);

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::GRADIENT);
  propertyMap.Insert(GradientVisual::Property::START_POSITION, start);
  propertyMap.Insert(GradientVisual::Property::END_POSITION, end);
  propertyMap.Insert(GradientVisual::Property::STOP_OFFSET, Vector2(0.f, 1.f));
  propertyMap.Insert(GradientVisual::Property::SPREAD_METHOD, GradientVisual::SpreadMethod::REPEAT);
  propertyMap.Insert(GradientVisual::Property::STOP_COLOR, stopColors);
  Visual::Base gradientVisual = factory.CreateVisual(propertyMap);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, gradientVisual);
  dummyControl[Actor::Property::SIZE] = Vector2(200.f, 200.f);
  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  float   targetOpacity = 0.5f;
  Vector4 targetCornerRadius(20.0f, 30.0f, 10.0f, 20.0f);

  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, Visual::Property::OPACITY), targetOpacity);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::CORNER_RADIUS), targetCornerRadius);
  animation.Play();

  application.SendNotification();
  application.Render();
  application.Render(1001u); // End of animation

  Property::Map resultMap;
  gradientVisual.CreatePropertyMap(resultMap);

  // Test property values: they should be updated
  Property::Value* colorValue = resultMap.Find(Visual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(colorValue);
  DALI_TEST_EQUALS(colorValue->Get<Vector4>(), Vector4(1.0f, 1.0f, 1.0f, targetOpacity), TEST_LOCATION);

  Property::Value* cornerRadiusValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), targetCornerRadius, TEST_LOCATION);

  // Test uniform value
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", targetCornerRadius), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualGetVisualProperty05(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetVisualProperty05: Test animatable property, SvgVisual");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("cornerRadius", Property::Type::VECTOR4),
      UniformData("borderlineWidth", Property::Type::FLOAT),
      UniformData("borderlineColor", Property::Type::VECTOR4),
      UniformData("borderlineOffset", Property::Type::FLOAT),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::SVG);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_SVG_FILE_NAME);

  Visual::Base svgVisual = factory.CreateVisual(propertyMap);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, svgVisual);
  dummyControl[Actor::Property::SIZE] = Vector2(200.f, 200.f);
  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  // Wait for loading & rasterization
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  float   targetOpacity = 0.5f;
  Vector4 targetCornerRadius(20.0f, 20.0f, 0.0f, 0.0f);
  float   targetBorderlineWidth = 10.0f;
  Vector4 targetBorderlineColor(1.0f, 0.0f, 1.0f, 0.5f);
  float   targetBorderlineOffset = -1.5f;

  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, Visual::Property::OPACITY), targetOpacity);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::CORNER_RADIUS), targetCornerRadius);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_WIDTH), targetBorderlineWidth);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_COLOR), targetBorderlineColor);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_OFFSET), targetBorderlineOffset);
  animation.Play();

  application.SendNotification();
  application.Render();
  application.Render(1001u); // End of animation

  Property::Map resultMap;
  svgVisual.CreatePropertyMap(resultMap);

  // Test property values: they should be updated
  Property::Value* colorValue = resultMap.Find(Visual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(colorValue);
  DALI_TEST_EQUALS(colorValue->Get<Vector4>(), Vector4(1.0f, 1.0f, 1.0f, targetOpacity), TEST_LOCATION);

  Property::Value* cornerRadiusValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), targetCornerRadius, TEST_LOCATION);

  Property::Value* borderlineWidthValue = resultMap.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(borderlineWidthValue);
  DALI_TEST_EQUALS(borderlineWidthValue->Get<float>(), targetBorderlineWidth, TEST_LOCATION);

  Property::Value* borderlineColorValue = resultMap.Find(DevelVisual::Property::BORDERLINE_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(borderlineColorValue);
  DALI_TEST_EQUALS(borderlineColorValue->Get<Vector4>(), targetBorderlineColor, TEST_LOCATION);

  Property::Value* borderlineOffsetValue = resultMap.Find(DevelVisual::Property::BORDERLINE_OFFSET, Property::FLOAT);
  DALI_TEST_CHECK(borderlineOffsetValue);
  DALI_TEST_EQUALS(borderlineOffsetValue->Get<float>(), targetBorderlineOffset, TEST_LOCATION);

  // Currently test with multiple program doesn't work well. will fix another day
  // Test uniform value
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", targetCornerRadius), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineWidth", targetBorderlineWidth), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("borderlineColor", targetBorderlineColor), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineOffset", targetBorderlineOffset), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualGetVisualProperty06(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetVisualProperty06: Test animatable property, AnimatedImageVisual");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("cornerRadius", Property::Type::VECTOR4),
      UniformData("borderlineWidth", Property::Type::FLOAT),
      UniformData("borderlineColor", Property::Type::VECTOR4),
      UniformData("borderlineOffset", Property::Type::FLOAT),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::ANIMATED_IMAGE);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_GIF_FILE_NAME);

  Visual::Base animatedImageVisual = factory.CreateVisual(propertyMap);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, animatedImageVisual);
  dummyControl[Actor::Property::SIZE] = Vector2(200.f, 200.f);
  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  // Wait for image loading
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  float   targetOpacity = 0.5f;
  Vector4 targetCornerRadius(20.0f, 20.0f, 0.0f, 0.0f);
  float   targetBorderlineWidth = 10.0f;
  Vector4 targetBorderlineColor(1.0f, 0.0f, 1.0f, 0.5f);
  float   targetBorderlineOffset = -1.5f;

  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, Visual::Property::OPACITY), targetOpacity);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::CORNER_RADIUS), targetCornerRadius);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_WIDTH), targetBorderlineWidth);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_COLOR), targetBorderlineColor);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_OFFSET), targetBorderlineOffset);
  animation.Play();

  application.SendNotification();
  application.Render();
  application.Render(1001u); // End of animation

  Property::Map resultMap;
  animatedImageVisual.CreatePropertyMap(resultMap);

  // Test property values: they should be updated
  Property::Value* colorValue = resultMap.Find(Visual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(colorValue);
  DALI_TEST_EQUALS(colorValue->Get<Vector4>(), Vector4(1.0f, 1.0f, 1.0f, targetOpacity), TEST_LOCATION);

  Property::Value* cornerRadiusValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), targetCornerRadius, TEST_LOCATION);

  Property::Value* borderlineWidthValue = resultMap.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(borderlineWidthValue);
  DALI_TEST_EQUALS(borderlineWidthValue->Get<float>(), targetBorderlineWidth, TEST_LOCATION);

  Property::Value* borderlineColorValue = resultMap.Find(DevelVisual::Property::BORDERLINE_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(borderlineColorValue);
  DALI_TEST_EQUALS(borderlineColorValue->Get<Vector4>(), targetBorderlineColor, TEST_LOCATION);

  Property::Value* borderlineOffsetValue = resultMap.Find(DevelVisual::Property::BORDERLINE_OFFSET, Property::FLOAT);
  DALI_TEST_CHECK(borderlineOffsetValue);
  DALI_TEST_EQUALS(borderlineOffsetValue->Get<float>(), targetBorderlineOffset, TEST_LOCATION);

  // Currently test with multiple program doesn't work well. will fix another day
  // Test uniform value
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", targetCornerRadius), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineWidth", targetBorderlineWidth), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("borderlineColor", targetBorderlineColor), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineOffset", targetBorderlineOffset), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualGetVisualProperty07(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualGetVisualProperty07: Test animatable property, AnimatedVectorVisual");

  static std::vector<UniformData> customUniforms =
    {
      UniformData("cornerRadius", Property::Type::VECTOR4),
      UniformData("borderlineWidth", Property::Type::FLOAT),
      UniformData("borderlineColor", Property::Type::VECTOR4),
      UniformData("borderlineOffset", Property::Type::FLOAT),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, DevelVisual::Type::ANIMATED_VECTOR_IMAGE);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME);
  propertyMap.Insert(ImageVisual::Property::SYNCHRONOUS_LOADING, false);

  Visual::Base animatedVectorVisual = factory.CreateVisual(propertyMap);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, animatedVectorVisual);
  dummyControl[Actor::Property::SIZE] = Vector2(200.f, 200.f);
  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  // Trigger count is 2 - load & render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  float   targetOpacity = 0.5f;
  Vector4 targetCornerRadius(20.0f, 20.0f, 0.0f, 0.0f);
  float   targetBorderlineWidth = 10.0f;
  Vector4 targetBorderlineColor(1.0f, 0.0f, 1.0f, 0.5f);
  float   targetBorderlineOffset = -1.5f;

  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, Visual::Property::OPACITY), targetOpacity);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::CORNER_RADIUS), targetCornerRadius);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_WIDTH), targetBorderlineWidth);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_COLOR), targetBorderlineColor);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::BORDERLINE_OFFSET), targetBorderlineOffset);
  animation.Play();

  application.SendNotification();
  application.Render();
  application.Render(1001u); // End of animation

  Property::Map resultMap;
  animatedVectorVisual.CreatePropertyMap(resultMap);

  // Test property values: they should be updated
  Property::Value* colorValue = resultMap.Find(Visual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(colorValue);
  DALI_TEST_EQUALS(colorValue->Get<Vector4>(), Vector4(1.0f, 1.0f, 1.0f, targetOpacity), TEST_LOCATION);

  Property::Value* cornerRadiusValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), targetCornerRadius, TEST_LOCATION);

  Property::Value* borderlineWidthValue = resultMap.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(borderlineWidthValue);
  DALI_TEST_EQUALS(borderlineWidthValue->Get<float>(), targetBorderlineWidth, TEST_LOCATION);

  Property::Value* borderlineColorValue = resultMap.Find(DevelVisual::Property::BORDERLINE_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(borderlineColorValue);
  DALI_TEST_EQUALS(borderlineColorValue->Get<Vector4>(), targetBorderlineColor, TEST_LOCATION);

  Property::Value* borderlineOffsetValue = resultMap.Find(DevelVisual::Property::BORDERLINE_OFFSET, Property::FLOAT);
  DALI_TEST_CHECK(borderlineOffsetValue);
  DALI_TEST_EQUALS(borderlineOffsetValue->Get<float>(), targetBorderlineOffset, TEST_LOCATION);

  // Currently test with multiple program doesn't work well. will fix another day
  // Test uniform value
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("cornerRadius", targetCornerRadius), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineWidth", targetBorderlineWidth), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<Vector4>("borderlineColor", targetBorderlineColor), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckUniformValue<float>("borderlineOffset", targetBorderlineOffset), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualUpdateProperty01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualUpdateProperty01: Test update property by DoAction. Standard case");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap[Visual::Property::TYPE]                     = Visual::Type::IMAGE;
  propertyMap[ImageVisual::Property::URL]                 = TEST_IMAGE_FILE_NAME;
  propertyMap[Visual::Property::MIX_COLOR]                = Color::BLUE;
  propertyMap[DevelVisual::Property::VISUAL_FITTING_MODE] = DevelVisual::FIT_WIDTH;

  Visual::Base imageVisual = factory.CreateVisual(propertyMap);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual);
  dummyControl[Actor::Property::SIZE] = Vector2(200.f, 200.f);
  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  // Wait for image loading
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  Property::Map originalMap;
  imageVisual.CreatePropertyMap(originalMap);

  float                    targetOpacity            = 0.5f;
  Vector3                  targetMixColor           = Vector3(1.0f, 0.4f, 0.2f);
  bool                     targetPreMultipliedAlpha = originalMap[Visual::Property::PREMULTIPLIED_ALPHA].Get<bool>() ^ true;
  DevelVisual::FittingMode targetVisualFittingMode  = DevelVisual::CENTER;
  Vector4                  targetCornerRadius       = Vector4(10.0f, 0.0f, 1.0f, 2.0f);
  float                    targetBorderlineWidth    = 20.0f;
  Vector4                  targetBorderlineColor    = Color::RED;
  float                    targetBorderlineOffset   = 1.0f;

  Property::Map targetPropertyMap;
  targetPropertyMap[Visual::Property::OPACITY]                  = targetOpacity;
  targetPropertyMap[ImageVisual::Property::URL]                 = "foobar";
  targetPropertyMap[Visual::Property::MIX_COLOR]                = targetMixColor;
  targetPropertyMap[Visual::Property::PREMULTIPLIED_ALPHA]      = targetPreMultipliedAlpha;
  targetPropertyMap[DevelVisual::Property::VISUAL_FITTING_MODE] = targetVisualFittingMode;
  targetPropertyMap[DevelVisual::Property::CORNER_RADIUS]       = targetCornerRadius;
  targetPropertyMap[DevelVisual::Property::BORDERLINE_WIDTH]    = targetBorderlineWidth;
  targetPropertyMap[DevelVisual::Property::BORDERLINE_COLOR]    = targetBorderlineColor;
  targetPropertyMap[DevelVisual::Property::BORDERLINE_OFFSET]   = targetBorderlineOffset;

  // Update Properties
  DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Action::UPDATE_PROPERTY, targetPropertyMap);

  Property::Map resultMap;
  imageVisual.CreatePropertyMap(resultMap);

  // Test property values: they should be updated
  Property::Value* colorValue = resultMap.Find(Visual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(colorValue);
  DALI_TEST_EQUALS(colorValue->Get<Vector4>(), Vector4(targetMixColor.r, targetMixColor.g, targetMixColor.b, targetOpacity), TEST_LOCATION);

  Property::Value* urlValue = resultMap.Find(ImageVisual::Property::URL, Property::STRING);
  DALI_TEST_CHECK(urlValue);
  // NOTE : ImageVisual URL must NOT changed.
  DALI_TEST_EQUALS(urlValue->Get<std::string>(), TEST_IMAGE_FILE_NAME, TEST_LOCATION);

  Property::Value* preMultipliedValue = resultMap.Find(Visual::Property::PREMULTIPLIED_ALPHA, Property::BOOLEAN);
  DALI_TEST_CHECK(preMultipliedValue);
  DALI_TEST_EQUALS(preMultipliedValue->Get<bool>(), targetPreMultipliedAlpha, TEST_LOCATION);

  Property::Value* visualFittingModeValue = resultMap.Find(DevelVisual::Property::VISUAL_FITTING_MODE, Property::STRING);
  DALI_TEST_CHECK(visualFittingModeValue);
  DALI_TEST_EQUALS(visualFittingModeValue->Get<std::string>(), "CENTER", TEST_LOCATION);

  Property::Value* cornerRadiusValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), targetCornerRadius, TEST_LOCATION);

  Property::Value* borderlineWidthValue = resultMap.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(borderlineWidthValue);
  DALI_TEST_EQUALS(borderlineWidthValue->Get<float>(), targetBorderlineWidth, TEST_LOCATION);

  Property::Value* borderlineColorValue = resultMap.Find(DevelVisual::Property::BORDERLINE_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(borderlineColorValue);
  DALI_TEST_EQUALS(borderlineColorValue->Get<Vector4>(), targetBorderlineColor, TEST_LOCATION);

  Property::Value* borderlineOffsetValue = resultMap.Find(DevelVisual::Property::BORDERLINE_OFFSET, Property::FLOAT);
  DALI_TEST_CHECK(borderlineOffsetValue);
  DALI_TEST_EQUALS(borderlineOffsetValue->Get<float>(), targetBorderlineOffset, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualUpdateProperty02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualUpdateProperty02: Test update property by DoAction. Initialize as zero, and update non-zero case");

  Vector4 borderlineColor  = Color::BLUE;
  float   borderlineOffset = 1.0f;

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap[Visual::Property::TYPE]                      = Visual::Type::IMAGE;
  propertyMap[ImageVisual::Property::URL]                  = TEST_IMAGE_FILE_NAME;
  propertyMap[Visual::Property::MIX_COLOR]                 = Color::BLUE;
  propertyMap[DevelVisual::Property::VISUAL_FITTING_MODE]  = DevelVisual::FIT_WIDTH;
  propertyMap[DevelVisual::Property::CORNER_RADIUS]        = 0.0f;
  propertyMap[DevelVisual::Property::CORNER_RADIUS_POLICY] = Toolkit::Visual::Transform::Policy::RELATIVE;
  propertyMap[DevelVisual::Property::BORDERLINE_WIDTH]     = 0.0f;
  propertyMap[DevelVisual::Property::BORDERLINE_COLOR]     = borderlineColor;
  propertyMap[DevelVisual::Property::BORDERLINE_OFFSET]    = borderlineOffset;

  Visual::Base imageVisual = factory.CreateVisual(propertyMap);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual);
  dummyControl[Actor::Property::SIZE] = Vector2(200.f, 200.f);
  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  // Wait for image loading
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  Property::Map originalMap;
  imageVisual.CreatePropertyMap(originalMap);

  Vector4 targetCornerRadius    = Vector4(10.0f, 0.0f, 1.0f, 2.0f);
  float   targetBorderlineWidth = 20.0f;

  Property::Map targetPropertyMap;
  targetPropertyMap[DevelVisual::Property::CORNER_RADIUS]    = targetCornerRadius;
  targetPropertyMap[DevelVisual::Property::BORDERLINE_WIDTH] = targetBorderlineWidth;

  // Update Properties
  DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Action::UPDATE_PROPERTY, targetPropertyMap);

  Property::Map resultMap;
  imageVisual.CreatePropertyMap(resultMap);

  Property::Value* cornerRadiusValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), targetCornerRadius, TEST_LOCATION);

  Property::Value* cornerRadiusPolicyValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS_POLICY, Property::INTEGER);
  DALI_TEST_CHECK(cornerRadiusPolicyValue);
  DALI_TEST_EQUALS(cornerRadiusPolicyValue->Get<int>(), static_cast<int>(Toolkit::Visual::Transform::Policy::RELATIVE), TEST_LOCATION);

  Property::Value* borderlineWidthValue = resultMap.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(borderlineWidthValue);
  DALI_TEST_EQUALS(borderlineWidthValue->Get<float>(), targetBorderlineWidth, TEST_LOCATION);

  Property::Value* borderlineColorValue = resultMap.Find(DevelVisual::Property::BORDERLINE_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(borderlineColorValue);
  DALI_TEST_EQUALS(borderlineColorValue->Get<Vector4>(), borderlineColor, TEST_LOCATION);

  Property::Value* borderlineOffsetValue = resultMap.Find(DevelVisual::Property::BORDERLINE_OFFSET, Property::FLOAT);
  DALI_TEST_CHECK(borderlineOffsetValue);
  DALI_TEST_EQUALS(borderlineOffsetValue->Get<float>(), borderlineOffset, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualUpdatePropertyInvalidType(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualUpdatePropertyInvalidType: Test update property by DoAction. But some value is not invalid to update");

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap[Visual::Property::TYPE]     = Visual::Type::N_PATCH;
  propertyMap[ImageVisual::Property::URL] = TEST_NPATCH_FILE_NAME;

  Visual::Base imageVisual = factory.CreateVisual(propertyMap);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual);
  dummyControl[Actor::Property::SIZE] = Vector2(200.f, 200.f);
  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  // Wait for image loading
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  Property::Map originalMap;
  imageVisual.CreatePropertyMap(originalMap);

  float                    targetOpacity            = 0.5f;
  Vector3                  targetMixColor           = Vector3(1.0f, 0.4f, 0.2f);
  bool                     targetPreMultipliedAlpha = originalMap[Visual::Property::PREMULTIPLIED_ALPHA].Get<bool>() ^ true;
  DevelVisual::FittingMode targetVisualFittingMode  = DevelVisual::CENTER;
  Vector4                  targetCornerRadius       = Vector4(10.0f, 0.0f, 1.0f, 2.0f);
  float                    targetBorderlineWidth    = 20.0f;
  Vector4                  targetBorderlineColor    = Color::RED;
  float                    targetBorderlineOffset   = 1.0f;

  Property::Map targetPropertyMap;
  targetPropertyMap[Visual::Property::OPACITY]                  = targetOpacity;
  targetPropertyMap[ImageVisual::Property::URL]                 = "foobar";
  targetPropertyMap[Visual::Property::MIX_COLOR]                = targetMixColor;
  targetPropertyMap[Visual::Property::PREMULTIPLIED_ALPHA]      = targetPreMultipliedAlpha;
  targetPropertyMap[DevelVisual::Property::VISUAL_FITTING_MODE] = targetVisualFittingMode;

  // Properties that N_PATCH visual could not used.
  targetPropertyMap[DevelVisual::Property::CORNER_RADIUS]     = targetCornerRadius;
  targetPropertyMap[DevelVisual::Property::BORDERLINE_WIDTH]  = targetBorderlineWidth;
  targetPropertyMap[DevelVisual::Property::BORDERLINE_COLOR]  = targetBorderlineColor;
  targetPropertyMap[DevelVisual::Property::BORDERLINE_OFFSET] = targetBorderlineOffset;

  // Update Properties
  DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Action::UPDATE_PROPERTY, targetPropertyMap);

  Property::Map resultMap;
  imageVisual.CreatePropertyMap(resultMap);

  // Test property values: they should be updated
  Property::Value* colorValue = resultMap.Find(Visual::Property::MIX_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(colorValue);
  DALI_TEST_EQUALS(colorValue->Get<Vector4>(), Vector4(targetMixColor.r, targetMixColor.g, targetMixColor.b, targetOpacity), TEST_LOCATION);

  Property::Value* urlValue = resultMap.Find(ImageVisual::Property::URL, Property::STRING);
  DALI_TEST_CHECK(urlValue);
  // NOTE : NPatchVisual URL must NOT changed.
  DALI_TEST_EQUALS(urlValue->Get<std::string>(), TEST_NPATCH_FILE_NAME, TEST_LOCATION);

  Property::Value* preMultipliedValue = resultMap.Find(Visual::Property::PREMULTIPLIED_ALPHA, Property::BOOLEAN);
  DALI_TEST_CHECK(preMultipliedValue);
  DALI_TEST_EQUALS(preMultipliedValue->Get<bool>(), targetPreMultipliedAlpha, TEST_LOCATION);

  Property::Value* visualFittingModeValue = resultMap.Find(DevelVisual::Property::VISUAL_FITTING_MODE, Property::STRING);
  DALI_TEST_CHECK(visualFittingModeValue);
  DALI_TEST_EQUALS(visualFittingModeValue->Get<std::string>(), "CENTER", TEST_LOCATION);

  // We don't check properties value that N_PATCH visual could not used.
  // It is undefined.

  END_TEST;
}

int UtcDaliVisualUpdatePropertyChangeShader01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualUpdatePropertyChangeShader01: Test update property by DoAction. Change the shader case");

  TraceCallStack& callStack = application.GetGraphicsController().mCallStack;

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  // Case ImageVisual
  propertyMap[Visual::Property::TYPE]     = Visual::Type::IMAGE;
  propertyMap[ImageVisual::Property::URL] = TEST_IMAGE_FILE_NAME;

  Visual::Base imageVisual = factory.CreateVisual(propertyMap);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual);
  dummyControl[Actor::Property::SIZE] = Vector2(200.f, 200.f);
  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  // Wait for image loading
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  TestShaderCodeContainSubstrings(
    dummyControl,
    {
      {"#define IS_REQUIRED_BORDERLINE", false},
      {"#define IS_REQUIRED_ROUNDED_CORNER", false},
    },
    TEST_LOCATION);

  callStack.Reset();
  callStack.Enable(true);

  Vector4 targetCornerRadius = Vector4(1.0f, 12.0f, 2.0f, 21.0f);

  Property::Map targetPropertyMap;
  targetPropertyMap[DevelVisual::Property::CORNER_RADIUS]        = targetCornerRadius;
  targetPropertyMap[DevelVisual::Property::CORNER_RADIUS_POLICY] = Toolkit::Visual::Transform::Policy::RELATIVE;

  // Update Properties with CornerRadius
  DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Action::UPDATE_PROPERTY, targetPropertyMap);

  Property::Map resultMap;
  imageVisual.CreatePropertyMap(resultMap);

  // Test property values: they should be updated
  Property::Value* cornerRadiusValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), targetCornerRadius, TEST_LOCATION);

  Property::Value* cornerRadiusPolicyValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS_POLICY, Property::INTEGER);
  DALI_TEST_CHECK(cornerRadiusPolicyValue);
  DALI_TEST_EQUALS(cornerRadiusPolicyValue->Get<int>(), static_cast<int>(Toolkit::Visual::Transform::Policy::RELATIVE), TEST_LOCATION);

  TestShaderCodeContainSubstrings(
    dummyControl,
    {
      {"#define IS_REQUIRED_BORDERLINE", false},
      {"#define IS_REQUIRED_ROUNDED_CORNER", true},
    },
    TEST_LOCATION);

  // Send shader compile signal
  application.SendNotification();
  application.Render();

  callStack.Enable(false);
  // Shader changed
  DALI_TEST_CHECK(callStack.FindMethod("CreateShader"));
  callStack.Reset();
  callStack.Enable(true);

  float   targetBorderlineWidth  = 10.0f;
  Vector4 targetBorderlineColor  = Vector4(1.0f, 0.2f, 0.1f, 0.5f);
  float   targetBorderlineOffset = -0.3f;

  Property::Map targetPropertyMap2;
  targetPropertyMap2[DevelVisual::Property::CORNER_RADIUS]        = Vector4::ZERO;
  targetPropertyMap2[DevelVisual::Property::CORNER_RADIUS_POLICY] = Toolkit::Visual::Transform::Policy::ABSOLUTE;
  targetPropertyMap2[DevelVisual::Property::BORDERLINE_WIDTH]     = targetBorderlineWidth;
  targetPropertyMap2[DevelVisual::Property::BORDERLINE_COLOR]     = targetBorderlineColor;
  targetPropertyMap2[DevelVisual::Property::BORDERLINE_OFFSET]    = targetBorderlineOffset;

  // Update Properties with Borderline
  DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Action::UPDATE_PROPERTY, targetPropertyMap2);

  Property::Map resultMap2;
  imageVisual.CreatePropertyMap(resultMap2);

  // Test property values: they should be updated
  cornerRadiusValue = resultMap2.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), Vector4::ZERO, TEST_LOCATION);

  cornerRadiusPolicyValue = resultMap2.Find(DevelVisual::Property::CORNER_RADIUS_POLICY, Property::INTEGER);
  DALI_TEST_CHECK(cornerRadiusPolicyValue);
  DALI_TEST_EQUALS(cornerRadiusPolicyValue->Get<int>(), static_cast<int>(Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  Property::Value* borderlineWidthValue = resultMap2.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(borderlineWidthValue);
  DALI_TEST_EQUALS(borderlineWidthValue->Get<float>(), targetBorderlineWidth, TEST_LOCATION);

  Property::Value* borderlineColorValue = resultMap2.Find(DevelVisual::Property::BORDERLINE_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(borderlineColorValue);
  DALI_TEST_EQUALS(borderlineColorValue->Get<Vector4>(), targetBorderlineColor, TEST_LOCATION);

  Property::Value* borderlineOffsetValue = resultMap2.Find(DevelVisual::Property::BORDERLINE_OFFSET, Property::FLOAT);
  DALI_TEST_CHECK(borderlineOffsetValue);
  DALI_TEST_EQUALS(borderlineOffsetValue->Get<float>(), targetBorderlineOffset, TEST_LOCATION);

  TestShaderCodeContainSubstrings(
    dummyControl,
    {
      {"#define IS_REQUIRED_BORDERLINE", true},
      {"#define IS_REQUIRED_ROUNDED_CORNER", true}, // Note : mAlwaysUsingCornerRadius is true.
    },
    TEST_LOCATION);

  // Send shader compile signal
  application.SendNotification();
  application.Render();

  callStack.Enable(false);
  // Shader changed
  DALI_TEST_CHECK(callStack.FindMethod("CreateShader"));
  callStack.Reset();
  callStack.Enable(true);

  Property::Map targetPropertyMap3;
  targetPropertyMap3[DevelVisual::Property::CORNER_RADIUS]        = Vector4::ZERO;
  targetPropertyMap3[DevelVisual::Property::CORNER_RADIUS_POLICY] = Toolkit::Visual::Transform::Policy::ABSOLUTE;
  targetPropertyMap3[DevelVisual::Property::BORDERLINE_WIDTH]     = 0.0f;
  targetPropertyMap3[DevelVisual::Property::BORDERLINE_COLOR]     = Vector4::ZERO;
  targetPropertyMap3[DevelVisual::Property::BORDERLINE_OFFSET]    = 0.0f;

  // Update Properties into zero
  DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Action::UPDATE_PROPERTY, targetPropertyMap3);

  Property::Map resultMap3;
  imageVisual.CreatePropertyMap(resultMap3);

  // Test property values: they should be updated
  cornerRadiusValue = resultMap3.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), Vector4::ZERO, TEST_LOCATION);

  cornerRadiusPolicyValue = resultMap3.Find(DevelVisual::Property::CORNER_RADIUS_POLICY, Property::INTEGER);
  DALI_TEST_CHECK(cornerRadiusPolicyValue);
  DALI_TEST_EQUALS(cornerRadiusPolicyValue->Get<int>(), static_cast<int>(Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  borderlineWidthValue = resultMap3.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(borderlineWidthValue);
  DALI_TEST_EQUALS(borderlineWidthValue->Get<float>(), 0.0f, TEST_LOCATION);

  borderlineColorValue = resultMap3.Find(DevelVisual::Property::BORDERLINE_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(borderlineColorValue);
  DALI_TEST_EQUALS(borderlineColorValue->Get<Vector4>(), Vector4::ZERO, TEST_LOCATION);

  borderlineOffsetValue = resultMap3.Find(DevelVisual::Property::BORDERLINE_OFFSET, Property::FLOAT);
  DALI_TEST_CHECK(borderlineOffsetValue);
  DALI_TEST_EQUALS(borderlineOffsetValue->Get<float>(), 0.0f, TEST_LOCATION);

  TestShaderCodeContainSubstrings(
    dummyControl,
    {
      {"#define IS_REQUIRED_BORDERLINE", true},     // Note : mAlwaysUsingBorderline is true.
      {"#define IS_REQUIRED_ROUNDED_CORNER", true}, // Note : mAlwaysUsingCornerRadius is true.
    },
    TEST_LOCATION);

  // Send shader compile signal
  application.SendNotification();
  application.Render();

  callStack.Enable(false);
  // Shader not changed
  DALI_TEST_CHECK(!callStack.FindMethod("CreateShader"));

  END_TEST;
}

int UtcDaliVisualUpdatePropertyChangeShader02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualUpdatePropertyChangeShader02: Test update property by DoAction. Fake update");

  TraceCallStack& callStack = application.GetGraphicsController().mCallStack;

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  // Case ImageVisual
  propertyMap[Visual::Property::TYPE]     = Visual::Type::IMAGE;
  propertyMap[ImageVisual::Property::URL] = TEST_IMAGE_FILE_NAME;

  Visual::Base imageVisual = factory.CreateVisual(propertyMap);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual);
  dummyControl[Actor::Property::SIZE] = Vector2(200.f, 200.f);
  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  // Wait for image loading
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  TestShaderCodeContainSubstrings(
    dummyControl,
    {
      {"#define IS_REQUIRED_BORDERLINE", false},
      {"#define IS_REQUIRED_ROUNDED_CORNER", false},
    },
    TEST_LOCATION);

  Vector4 targetCornerRadius = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

  Property::Map targetPropertyMap;
  targetPropertyMap[DevelVisual::Property::CORNER_RADIUS] = targetCornerRadius;

  callStack.Reset();
  callStack.Enable(true);

  // Update Properties with CornerRadius
  DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Action::UPDATE_PROPERTY, targetPropertyMap);

  Property::Map resultMap;
  imageVisual.CreatePropertyMap(resultMap);

  // Test property values: they should be updated
  Property::Value* cornerRadiusValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), targetCornerRadius, TEST_LOCATION);

  TestShaderCodeContainSubstrings(
    dummyControl,
    {
      {"#define IS_REQUIRED_BORDERLINE", false},
      {"#define IS_REQUIRED_ROUNDED_CORNER", false}, // Note : corner radius is zero. so we don't change shader!
    },
    TEST_LOCATION);

  // Send shader compile signal
  application.SendNotification();
  application.Render();

  callStack.Enable(false);

  // Shader doesn't changed
  DALI_TEST_CHECK(!(callStack.FindMethod("CreateShader")));
  callStack.Reset();
  callStack.Enable(true);

  float   targetBorderlineWidth  = 0.0f;
  Vector4 targetBorderlineColor  = Vector4(1.0f, 1.0f, 0.0f, 0.0f);
  float   targetBorderlineOffset = -1.0f;

  Property::Map targetPropertyMap2;
  targetPropertyMap2[DevelVisual::Property::BORDERLINE_WIDTH]  = targetBorderlineWidth;
  targetPropertyMap2[DevelVisual::Property::BORDERLINE_COLOR]  = targetBorderlineColor;
  targetPropertyMap2[DevelVisual::Property::BORDERLINE_OFFSET] = targetBorderlineOffset;

  // Update Properties with Borderline
  DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Action::UPDATE_PROPERTY, targetPropertyMap2);

  Property::Map resultMap2;
  imageVisual.CreatePropertyMap(resultMap2);

  // Test property values: they should be updated
  Property::Value* borderlineWidthValue = resultMap2.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(borderlineWidthValue);
  DALI_TEST_EQUALS(borderlineWidthValue->Get<float>(), targetBorderlineWidth, TEST_LOCATION);

  Property::Value* borderlineColorValue = resultMap2.Find(DevelVisual::Property::BORDERLINE_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(borderlineColorValue);
  DALI_TEST_EQUALS(borderlineColorValue->Get<Vector4>(), targetBorderlineColor, TEST_LOCATION);

  Property::Value* borderlineOffsetValue = resultMap2.Find(DevelVisual::Property::BORDERLINE_OFFSET, Property::FLOAT);
  DALI_TEST_CHECK(borderlineOffsetValue);
  DALI_TEST_EQUALS(borderlineOffsetValue->Get<float>(), targetBorderlineOffset, TEST_LOCATION);

  TestShaderCodeContainSubstrings(
    dummyControl,
    {
      {"#define IS_REQUIRED_BORDERLINE", false}, // Note : borderline width is zero. so we don't change shader!
      {"#define IS_REQUIRED_ROUNDED_CORNER", false},
    },
    TEST_LOCATION);

  // Send shader compile signal
  application.SendNotification();
  application.Render();

  callStack.Enable(false);

  // Shader doesn't changed
  DALI_TEST_CHECK(!(callStack.FindMethod("CreateShader")));

  END_TEST;
}

int UtcDaliVisualUpdatePropertyChangeShader03(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualUpdatePropertyChangeShader03: Test update property by DoAction. Blur Radius");

  TraceCallStack& callStack = application.GetGraphicsController().mCallStack;

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  // Case ImageVisual
  propertyMap[Visual::Property::TYPE]           = Visual::Type::COLOR;
  propertyMap[ColorVisual::Property::MIX_COLOR] = Color::BLUE;

  Visual::Base imageVisual = factory.CreateVisual(propertyMap);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual);
  dummyControl[Actor::Property::SIZE] = Vector2(200.f, 200.f);
  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  application.SendNotification();
  application.Render();

  TestShaderCodeContainSubstrings(
    dummyControl,
    {
      {"#define IS_REQUIRED_BLUR", false},
      {"#define IS_REQUIRED_BORDERLINE", false},
      {"#define IS_REQUIRED_ROUNDED_CORNER", false},
    },
    TEST_LOCATION);

  float   targetBlurRadius   = 15.0f;
  Vector4 targetCornerRadius = Vector4(1.0f, 0.1f, 1.1f, 0.0f);

  Property::Map targetPropertyMap;
  targetPropertyMap[DevelColorVisual::Property::BLUR_RADIUS] = targetBlurRadius;
  targetPropertyMap[DevelVisual::Property::CORNER_RADIUS]    = targetCornerRadius;
  targetPropertyMap[DevelVisual::Property::BORDERLINE_WIDTH] = 10.0f; // Don't care. just dummy

  callStack.Reset();
  callStack.Enable(true);

  // Update Properties with CornerRadius
  DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Action::UPDATE_PROPERTY, targetPropertyMap);

  Property::Map resultMap;
  imageVisual.CreatePropertyMap(resultMap);

  // Test property values: they should be updated
  Property::Value* blurRadiusValue = resultMap.Find(DevelColorVisual::Property::BLUR_RADIUS, Property::FLOAT);
  DALI_TEST_CHECK(blurRadiusValue);
  DALI_TEST_EQUALS(blurRadiusValue->Get<float>(), targetBlurRadius, TEST_LOCATION);

  Property::Value* cornerRadiusValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), targetCornerRadius, TEST_LOCATION);

  TestShaderCodeContainSubstrings(
    dummyControl,
    {
      {"#define IS_REQUIRED_BLUR", true},
      {"#define IS_REQUIRED_BORDERLINE", false}, // Note : We ignore borderline when blur radius occured
      {"#define IS_REQUIRED_ROUNDED_CORNER", true},
    },
    TEST_LOCATION);

  // Send shader compile signal
  application.SendNotification();
  application.Render();

  callStack.Enable(false);

  // Shader changed
  DALI_TEST_CHECK((callStack.FindMethod("CreateShader")));
  callStack.Reset();
  callStack.Enable(true);

  Property::Map targetPropertyMap2;
  targetPropertyMap2[DevelColorVisual::Property::BLUR_RADIUS] = 0.0f;
  targetPropertyMap2[DevelVisual::Property::CORNER_RADIUS]    = Vector4::ZERO;
  targetPropertyMap2[DevelVisual::Property::BORDERLINE_WIDTH] = 15.0f; // Don't care. just dummy

  // Update Properties with CornerRadius
  DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Action::UPDATE_PROPERTY, targetPropertyMap2);

  Property::Map resultMap2;
  imageVisual.CreatePropertyMap(resultMap2);

  // Test property values: they should be updated
  blurRadiusValue = resultMap2.Find(DevelColorVisual::Property::BLUR_RADIUS, Property::FLOAT);
  DALI_TEST_CHECK(blurRadiusValue);
  DALI_TEST_EQUALS(blurRadiusValue->Get<float>(), 0.0f, TEST_LOCATION);

  cornerRadiusValue = resultMap2.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), Vector4::ZERO, TEST_LOCATION);

  TestShaderCodeContainSubstrings(
    dummyControl,
    {
      {"#define IS_REQUIRED_BLUR", true},           // Note : mAlwaysUsingBlurRadius is true.
      {"#define IS_REQUIRED_BORDERLINE", false},    // Note : We ignore borderline when blur radius occured
      {"#define IS_REQUIRED_ROUNDED_CORNER", true}, // Note : mAlwaysUsingCornerRadius is true.
    },
    TEST_LOCATION);

  // Send shader compile signal
  application.SendNotification();
  application.Render();

  callStack.Enable(false);

  // Shader not changed
  DALI_TEST_CHECK(!(callStack.FindMethod("CreateShader")));

  END_TEST;
}

int UtcDaliVisualUpdatePropertyChangeShader04(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualUpdatePropertyChangeShader04: Test update property by DoAction during Animation. Change the shader case");

  TraceCallStack& callStack = application.GetGraphicsController().mCallStack;

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  // Case ImageVisual
  propertyMap[Visual::Property::TYPE]               = Visual::Type::IMAGE;
  propertyMap[ImageVisual::Property::URL]           = TEST_IMAGE_FILE_NAME;
  propertyMap[DevelVisual::Property::CORNER_RADIUS] = 10.0f;

  Visual::Base imageVisual = factory.CreateVisual(propertyMap);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual);
  dummyControl[Actor::Property::SIZE] = Vector2(200.f, 200.f);
  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  // Wait for image loading
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  TestShaderCodeContainSubstrings(
    dummyControl,
    {
      {"#define IS_REQUIRED_BORDERLINE", false},
      {"#define IS_REQUIRED_ROUNDED_CORNER", true},
    },
    TEST_LOCATION);

  callStack.Reset();
  callStack.Enable(true);

  Vector4 targetCornerRadius = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(DevelControl::GetVisualProperty(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Property::CORNER_RADIUS), targetCornerRadius);
  animation.Play();

  application.SendNotification();
  application.Render();
  application.Render(1001u); // End of animation

  TestShaderCodeContainSubstrings(
    dummyControl,
    {
      {"#define IS_REQUIRED_BORDERLINE", false},
      {"#define IS_REQUIRED_ROUNDED_CORNER", true}, // Note : mAlwaysUsingCornerRadius is true.
    },
    TEST_LOCATION);

  callStack.Enable(false);
  // Shader not changed
  DALI_TEST_CHECK(!callStack.FindMethod("CreateShader"));
  callStack.Reset();
  callStack.Enable(true);

  float         targetBorderlineWidth = 10.0f;
  Property::Map targetPropertyMap;
  targetPropertyMap[DevelVisual::Property::BORDERLINE_WIDTH] = targetBorderlineWidth;

  // Update Properties with CornerRadius
  DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Action::UPDATE_PROPERTY, targetPropertyMap);

  Property::Map resultMap;
  imageVisual.CreatePropertyMap(resultMap);

  // Test property values: they should be updated
  Property::Value* cornerRadiusValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(cornerRadiusValue);
  DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), targetCornerRadius, TEST_LOCATION);

  Property::Value* cornerRadiusPolicyValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS_POLICY, Property::INTEGER);
  DALI_TEST_CHECK(cornerRadiusPolicyValue);
  DALI_TEST_EQUALS(cornerRadiusPolicyValue->Get<int>(), static_cast<int>(Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  Property::Value* borderlineWidthValue = resultMap.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(borderlineWidthValue);
  DALI_TEST_EQUALS(borderlineWidthValue->Get<float>(), targetBorderlineWidth, TEST_LOCATION);

  TestShaderCodeContainSubstrings(
    dummyControl,
    {
      {"#define IS_REQUIRED_BORDERLINE", true},
      {"#define IS_REQUIRED_ROUNDED_CORNER", true}, // Note : mAlwaysUsingCornerRadius is true.
    },
    TEST_LOCATION);

  // Send shader compile signal
  application.SendNotification();
  application.Render();

  callStack.Enable(false);
  // Shader changed
  DALI_TEST_CHECK(callStack.FindMethod("CreateShader"));

  END_TEST;
}

int UtcDaliVisualUpdatePropertyChangeShader05(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualUpdatePropertyChangeShader05: Test update property under glsl version is under 300");

  auto originalShaderVersion = application.GetGlAbstraction().GetShaderLanguageVersion();

  // Change the shader language version forcely!
  application.GetGlAbstraction().mShaderLanguageVersion = 200;

  try
  {
    TraceCallStack& callStack = application.GetGraphicsController().mCallStack;

    VisualFactory factory = VisualFactory::Get();
    Property::Map propertyMap;
    // Case ImageVisual
    propertyMap[Visual::Property::TYPE]           = Visual::Type::COLOR;
    propertyMap[ColorVisual::Property::MIX_COLOR] = Color::BLUE;

    Visual::Base imageVisual = factory.CreateVisual(propertyMap);

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, imageVisual);
    dummyControl[Actor::Property::SIZE] = Vector2(200.f, 200.f);
    application.GetScene().Add(dummyControl);

    application.SendNotification();
    application.Render();

    application.SendNotification();
    application.Render();

    TestShaderCodeContainSubstrings(
      dummyControl,
      {
        {"#define IS_REQUIRED_BLUR", false},
        {"#define IS_REQUIRED_BORDERLINE", false},
        {"#define IS_REQUIRED_ROUNDED_CORNER", false},
      },
      TEST_LOCATION);

    float   targetBlurRadius   = 15.0f;
    Vector4 targetCornerRadius = Vector4(1.0f, 0.1f, 1.1f, 0.0f);

    Property::Map targetPropertyMap;
    targetPropertyMap[DevelColorVisual::Property::BLUR_RADIUS] = targetBlurRadius;
    targetPropertyMap[DevelVisual::Property::CORNER_RADIUS]    = targetCornerRadius;
    targetPropertyMap[DevelVisual::Property::BORDERLINE_WIDTH] = 10.0f; // Don't care. just dummy

    callStack.Reset();
    callStack.Enable(true);

    // Update Properties with CornerRadius
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Action::UPDATE_PROPERTY, targetPropertyMap);

    Property::Map resultMap;
    imageVisual.CreatePropertyMap(resultMap);

    // Test property values: they should be updated
    Property::Value* blurRadiusValue = resultMap.Find(DevelColorVisual::Property::BLUR_RADIUS, Property::FLOAT);
    DALI_TEST_CHECK(blurRadiusValue);
    DALI_TEST_EQUALS(blurRadiusValue->Get<float>(), targetBlurRadius, TEST_LOCATION);

    Property::Value* cornerRadiusValue = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
    DALI_TEST_CHECK(cornerRadiusValue);
    DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), targetCornerRadius, TEST_LOCATION);

    TestShaderCodeContainSubstringsForEachShader(
      dummyControl,
      {
        {"#define IS_REQUIRED_BLUR", {true, true}},
        {"#define IS_REQUIRED_BORDERLINE", {false, false}}, // Note : We ignore borderline when blur radius occured
        {"#define IS_REQUIRED_ROUNDED_CORNER", {true, true}},
        {"#define SL_VERSION_LOW", {false, true}},
      },
      TEST_LOCATION);

    // Send shader compile signal
    application.SendNotification();
    application.Render();

    callStack.Enable(false);

    // Shader changed
    DALI_TEST_CHECK((callStack.FindMethod("CreateShader")));
    callStack.Reset();
    callStack.Enable(true);

    Property::Map targetPropertyMap2;
    targetPropertyMap2[DevelColorVisual::Property::BLUR_RADIUS] = 0.0f;
    targetPropertyMap2[DevelVisual::Property::CORNER_RADIUS]    = Vector4::ZERO;
    targetPropertyMap2[DevelVisual::Property::BORDERLINE_WIDTH] = 15.0f; // Don't care. just dummy

    // Update Properties with CornerRadius
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, DevelVisual::Action::UPDATE_PROPERTY, targetPropertyMap2);

    Property::Map resultMap2;
    imageVisual.CreatePropertyMap(resultMap2);

    // Test property values: they should be updated
    blurRadiusValue = resultMap2.Find(DevelColorVisual::Property::BLUR_RADIUS, Property::FLOAT);
    DALI_TEST_CHECK(blurRadiusValue);
    DALI_TEST_EQUALS(blurRadiusValue->Get<float>(), 0.0f, TEST_LOCATION);

    cornerRadiusValue = resultMap2.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
    DALI_TEST_CHECK(cornerRadiusValue);
    DALI_TEST_EQUALS(cornerRadiusValue->Get<Vector4>(), Vector4::ZERO, TEST_LOCATION);

    TestShaderCodeContainSubstringsForEachShader(
      dummyControl,
      {
        {"#define IS_REQUIRED_BLUR", {true, true}},           // Note : mAlwaysUsingBlurRadius is true.
        {"#define IS_REQUIRED_BORDERLINE", {false, false}},   // Note : We ignore borderline when blur radius occured
        {"#define IS_REQUIRED_ROUNDED_CORNER", {true, true}}, // Note : mAlwaysUsingCornerRadius is true.
        {"#define SL_VERSION_LOW", {false, true}},
      },
      TEST_LOCATION);

    // Send shader compile signal
    application.SendNotification();
    application.Render();

    callStack.Enable(false);

    // Shader not changed
    DALI_TEST_CHECK(!(callStack.FindMethod("CreateShader")));
  }
  catch(...)
  {
    DALI_TEST_CHECK(false);
  }

  // Revert shader version. We should revert it even if UTC failed.
  application.GetGlAbstraction().mShaderLanguageVersion = originalShaderVersion;

  END_TEST;
}