/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/text/text-visual.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/wireframe/wireframe-visual.h>

#include <dali-toolkit/dali-toolkit.h>

#include <toolkit-environment-variable.h> // for setting environment variable: DALI_DEBUG_RENDERING

#if defined(ELDBUS_ENABLED)
#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/dbus-wrapper.h>
#endif

#include "dummy-control.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* TEST_IMAGE_FILE_NAME  = "image_01.jpg";
const char* TEST_NPATCH_FILE_NAME = "image_01.9.jpg";
const char* TEST_SVG_FILE_NAME    = TEST_RESOURCE_DIR "/svg1.svg";
const char* TEST_GIF_FILE_NAME    = TEST_RESOURCE_DIR "/anim.gif";

const std::string DEFAULT_FONT_DIR("/resources/fonts");
const char* const DALI_DEBUG_RENDERING("DALI_DEBUG_RENDERING");

void TestDebugVisual(Integration::Scene scene, Visual::Base& visual, DevelVisual::Type actualType, Vector2 expectedNaturalSize)
{
  {
    auto& impl = GetImplementation(visual);
    DALI_TEST_CHECK(&typeid(Toolkit::Internal::WireframeVisual) == &typeid(impl));
  }

  Vector2 naturalSize;
  visual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, expectedNaturalSize, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  Property::Map propertyMap;
  visual.CreatePropertyMap(propertyMap);
  Property::Value* typeValue = propertyMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  if(typeValue)
  {
    DALI_TEST_CHECK(typeValue->Get<int>() == actualType);
  }

  DummyControl      actor     = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(Control::CONTROL_PROPERTY_END_INDEX + 1, visual);
  scene.Add(actor);

  DALI_TEST_EQUALS(actor.GetRendererCount(), 1, TEST_LOCATION);
  if(actor.GetRendererCount() > 0)
  {
    Geometry geometry = actor.GetRendererAt(0).GetGeometry();
    DALI_TEST_CHECK(geometry.GetType() == Geometry::LINES);
  }
}
void TestDebugVisual(Integration::Scene scene, Visual::Base& visual, Visual::Type actualType, Vector2 expectedNaturalSize)
{
  TestDebugVisual(scene, visual, (DevelVisual::Type)actualType, expectedNaturalSize);
}
} // namespace

void dali_debug_rendering_startup(void)
{
  test_return_value = TET_UNDEF;
#if defined(ELDBUS_ENABLED)
  DBusWrapper::Install(std::unique_ptr<DBusWrapper>(new TestDBusWrapper));
#endif
}

void dali_debug_rendering_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliDebugRenderingGetVisual1(void)
{
  EnvironmentVariable::SetTestEnvironmentVariable(DALI_DEBUG_RENDERING, "1");
  ToolkitTestApplication application;
  tet_infoline("UtcDaliDebugRenderingGetVisual1:  Request visual with a Property::Map");

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK(factory);

  // Test that color visual is replaced with debug visual
  Property::Map propertyMap1;
  propertyMap1.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap1.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
  Visual::Base colorVisual = factory.CreateVisual(propertyMap1);
  DALI_TEST_CHECK(colorVisual);
  TestDebugVisual(application.GetScene(), colorVisual, Visual::COLOR, Vector2::ZERO);

  // Test that border visual is replaced with debug visual
  Property::Map propertyMap2;
  propertyMap2.Insert(Visual::Property::TYPE, Visual::BORDER);
  propertyMap2.Insert(BorderVisual::Property::COLOR, Color::BLUE);
  propertyMap2.Insert(BorderVisual::Property::SIZE, 2.f);
  Visual::Base borderVisual = factory.CreateVisual(propertyMap2);
  DALI_TEST_CHECK(borderVisual);
  TestDebugVisual(application.GetScene(), borderVisual, Visual::BORDER, Vector2::ZERO);

  // Test that gradient visual is replaced with debug visual
  Property::Map propertyMap3;
  propertyMap3.Insert(Visual::Property::TYPE, Visual::GRADIENT);
  Vector2 start(-1.f, -1.f);
  Vector2 end(1.f, 1.f);
  propertyMap3.Insert(GradientVisual::Property::START_POSITION, start);
  propertyMap3.Insert(GradientVisual::Property::END_POSITION, end);
  propertyMap3.Insert(GradientVisual::Property::SPREAD_METHOD, GradientVisual::SpreadMethod::REPEAT);
  Property::Array stopOffsets;
  stopOffsets.PushBack(0.2f);
  stopOffsets.PushBack(0.8f);
  propertyMap3.Insert(GradientVisual::Property::STOP_OFFSET, stopOffsets);
  Property::Array stopColors;
  stopColors.PushBack(Color::RED);
  stopColors.PushBack(Color::GREEN);
  propertyMap3.Insert(GradientVisual::Property::STOP_COLOR, stopColors);
  Visual::Base gradientVisual = factory.CreateVisual(propertyMap3);
  DALI_TEST_CHECK(gradientVisual);
  TestDebugVisual(application.GetScene(), gradientVisual, Visual::GRADIENT, Vector2::ZERO);

  // Test that image visual is replaced with debug visual
  Property::Map propertyMap4;
  propertyMap4.Insert(Toolkit::Visual::Property::TYPE, Visual::IMAGE);
  propertyMap4.Insert(ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME);
  propertyMap4.Insert(ImageVisual::Property::DESIRED_WIDTH, 50.f);
  propertyMap4.Insert(ImageVisual::Property::DESIRED_HEIGHT, 100.f);
  Visual::Base imageVisual = factory.CreateVisual(propertyMap4);
  DALI_TEST_CHECK(imageVisual);
  TestDebugVisual(application.GetScene(), imageVisual, Visual::IMAGE, Vector2(50.f, 100.f));

  // Test that SVG visual is replaced with debug visual
  // TEST_SVG_FILE:
  //  <svg width="100" height="100">
  //  <circle cx="50" cy="50" r="40" stroke="green" stroke-width="4" fill="yellow" />
  //  </svg>
  Property::Map propertyMap5;
  propertyMap5.Insert(Toolkit::Visual::Property::TYPE, Visual::SVG);
  propertyMap5.Insert(ImageVisual::Property::URL, TEST_SVG_FILE_NAME);
  Visual::Base svgVisual = factory.CreateVisual(propertyMap5);
  DALI_TEST_CHECK(svgVisual);
  TestDebugVisual(application.GetScene(), svgVisual, Visual::SVG, Vector2::ZERO);

  // Test that AnimatedImageVisual is replaced with debug visual
  // TEST_GIF_FILE: anim.gif
  // resolution: 50*50, frame count: 4, frame delay: 0.2 second for each frame
  Property::Map propertyMap6;
  propertyMap6.Insert(Toolkit::Visual::Property::TYPE, Visual::ANIMATED_IMAGE);
  propertyMap6.Insert(ImageVisual::Property::URL, TEST_GIF_FILE_NAME);
  Visual::Base animatedImageVisual = factory.CreateVisual(propertyMap6);
  DALI_TEST_CHECK(animatedImageVisual);
  TestDebugVisual(application.GetScene(), animatedImageVisual, Visual::ANIMATED_IMAGE, Vector2(50.f, 50.f));

  // Test that text visual is replaced with debug visual

  // Load some fonts to get the same metrics on different platforms.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(96u, 96u);

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf");

  Property::Map propertyMap7;
  propertyMap7.Insert(Toolkit::Visual::Property::TYPE, Visual::TEXT);
  propertyMap7.Insert(TextVisual::Property::ENABLE_MARKUP, true);
  propertyMap7.Insert(TextVisual::Property::TEXT, "<font family='TizenSans' size='12'>Hello world</font>");
  propertyMap7.Insert(TextVisual::Property::MULTI_LINE, true);

  Visual::Base textVisual = factory.CreateVisual(propertyMap7);
  DALI_TEST_CHECK(textVisual);
  {
    auto&& impl = GetImplementation(textVisual);
    DALI_TEST_CHECK(&typeid(Toolkit::Internal::WireframeVisual) == &typeid(impl));
  }

  Vector2 naturalSize;
  textVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(82.f, 20.f), Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  const float height = textVisual.GetHeightForWidth(40.f);
  DALI_TEST_EQUALS(height, 60.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Test that NPatchVisual is replaced with debug visual
  // TEST_NPATCH_FILE_NAME: image_01.9.jpg
  Property::Map propertyMap8;
  propertyMap8.Insert(Toolkit::Visual::Property::TYPE, Visual::N_PATCH);
  propertyMap8.Insert(ImageVisual::Property::URL, TEST_NPATCH_FILE_NAME);
  Visual::Base nPatchVisual = factory.CreateVisual(propertyMap8);
  DALI_TEST_CHECK(nPatchVisual);
  TestDebugVisual(application.GetScene(), nPatchVisual, Visual::N_PATCH, Vector2::ZERO);

  EnvironmentVariable::SetTestEnvironmentVariable(DALI_DEBUG_RENDERING, "0");
  END_TEST;
}

int UtcDaliDebugRenderingGetVisual2(void)
{
  EnvironmentVariable::SetTestEnvironmentVariable(DALI_DEBUG_RENDERING, "1");
  ToolkitTestApplication application;
  tet_infoline("UtcDaliDebugRenderingGetVisual2: Request visual with various parameters");

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK(factory);

  // Test that color visual is replaced with debug visual
  Dali::Property::Map map;
  map[Toolkit::Visual::Property::TYPE]  = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::CYAN;

  Visual::Base colorVisual = factory.CreateVisual(map);
  DALI_TEST_CHECK(colorVisual);
  TestDebugVisual(application.GetScene(), colorVisual, Visual::COLOR, Vector2::ZERO);

  // Test that border visual is replaced with debug visual
  map.Clear();
  map[Toolkit::Visual::Property::TYPE] = Visual::BORDER;
  map[BorderVisual::Property::COLOR]   = Color::GREEN;
  map[BorderVisual::Property::SIZE]    = 2.f;
  Visual::Base borderVisual            = factory.CreateVisual(map);
  DALI_TEST_CHECK(borderVisual);
  TestDebugVisual(application.GetScene(), borderVisual, Visual::BORDER, Vector2::ZERO);

  // Test that image visual is replaced with debug visual
  map.Clear();
  map[Toolkit::Visual::Property::TYPE] = Visual::IMAGE;
  map[ImageVisual::Property::URL]      = TEST_IMAGE_FILE_NAME;
  Visual::Base imageVisual             = factory.CreateVisual(map);
  DALI_TEST_CHECK(imageVisual);
  TestDebugVisual(application.GetScene(), imageVisual, Visual::IMAGE, Vector2(64.0f, 64.0f /* Broken Image Size */));

  // Test that n patch visual is replaced with debug visual
  Visual::Base nPatchVisual = factory.CreateVisual(TEST_NPATCH_FILE_NAME, ImageDimensions());
  DALI_TEST_CHECK(nPatchVisual);
  TestDebugVisual(application.GetScene(), nPatchVisual, Visual::N_PATCH, Vector2::ZERO);

  EnvironmentVariable::SetTestEnvironmentVariable(DALI_DEBUG_RENDERING, "0");
  END_TEST;
}

int UtcDaliDebugRenderingGetVisual3(void)
{
  EnvironmentVariable::SetTestEnvironmentVariable(DALI_DEBUG_RENDERING, "1");
  ToolkitTestApplication application;
  tet_infoline("UtcDaliDebugRenderingGetVisual3: Request visual with various parameters");

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK(factory);

  // Test that image visual is replaced with debug visual
  Dali::Property::Map map;
  map[Toolkit::Visual::Property::TYPE] = Visual::IMAGE;
  map[ImageVisual::Property::URL]      = TEST_IMAGE_FILE_NAME;
  Visual::Base imageVisual             = factory.CreateVisual(map);
  DALI_TEST_CHECK(imageVisual);
  TestDebugVisual(application.GetScene(), imageVisual, Visual::IMAGE, Vector2(64.0f, 64.0f /* Broken Image Size */));

  // Test that image visual with null string don't make visual
  map.Clear();
  map[Toolkit::Visual::Property::TYPE] = Visual::IMAGE;
  map[ImageVisual::Property::URL]      = "";
  Visual::Base emptyVisual             = factory.CreateVisual(map);
  DALI_TEST_CHECK(emptyVisual);
  TestDebugVisual(application.GetScene(), emptyVisual, Visual::WIREFRAME, Vector2::ZERO);

  tet_infoline("Check that GetVisualObject returns the actual WireframeVisual");
  Toolkit::Internal::Visual::Base& visualImpl = GetImplementation(emptyVisual).GetVisualObject();
  DALI_TEST_CHECK(dynamic_cast<Toolkit::Internal::WireframeVisual*>(&visualImpl));

  tet_infoline("Compare the returned emptyVisual with the visual implementation, should be the same");
  DALI_TEST_CHECK(emptyVisual.GetObjectPtr() == &visualImpl);

  // Test that image view with empty property map don't make visual even DebugRendering is enabled.
  map.Clear();
  ImageView imageView1 = ImageView::New();
  imageView1.SetProperty(Control::Property::BACKGROUND, map);
  imageView1.SetProperty(ImageView::Property::IMAGE, map);

  // Test that image view with empty property value don't make visual even DebugRendering is enabled.
  Property::Value emptyValue;
  ImageView       imageView2 = ImageView::New();
  imageView2.SetProperty(Control::Property::BACKGROUND, emptyValue);
  imageView2.SetProperty(ImageView::Property::IMAGE, emptyValue);

  // Test that image view with invalid property value don't make visual even DebugRendering is enabled.
  Property::Value invalidValue(static_cast<int>(3));
  ImageView       imageView3 = ImageView::New();
  imageView3.SetProperty(Control::Property::BACKGROUND, invalidValue);
  imageView3.SetProperty(ImageView::Property::IMAGE, invalidValue);

  application.GetScene().Add(imageView1);
  application.GetScene().Add(imageView2);
  application.GetScene().Add(imageView3);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(imageView1.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(imageView2.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(imageView3.GetRendererCount(), 0u, TEST_LOCATION);

  EnvironmentVariable::SetTestEnvironmentVariable(DALI_DEBUG_RENDERING, "0");
  END_TEST;
}

int UtcDaliDebugRenderingGetVisual4(void)
{
  EnvironmentVariable::SetTestEnvironmentVariable(DALI_DEBUG_RENDERING, "1");
  ToolkitTestApplication application;
  tet_infoline("UtcDaliDebugRenderingGetVisual4: Request visual with empty URL and empty Array");

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK(factory);

  // Test that image visual with null string don't make visual
  for(auto type : {DevelVisual::IMAGE, DevelVisual::ANIMATED_IMAGE, DevelVisual::SVG, DevelVisual::N_PATCH, DevelVisual::ANIMATED_VECTOR_IMAGE})
  {
    Dali::Property::Map map;
    map[Toolkit::Visual::Property::TYPE] = type;
    map[ImageVisual::Property::URL]      = "";
    Visual::Base emptyVisual             = factory.CreateVisual(map);
    DALI_TEST_CHECK(emptyVisual);
    TestDebugVisual(application.GetScene(), emptyVisual, Visual::WIREFRAME, Vector2::ZERO);

    tet_infoline("Check that GetVisualObject returns the actual WireframeVisual");
    Toolkit::Internal::Visual::Base& visualImpl = GetImplementation(emptyVisual).GetVisualObject();
    DALI_TEST_CHECK(dynamic_cast<Toolkit::Internal::WireframeVisual*>(&visualImpl));

    tet_infoline("Compare the returned emptyVisual with the visual implementation, should be the same");
    DALI_TEST_CHECK(emptyVisual.GetObjectPtr() == &visualImpl);
  }

  // Test that image visual with empty array don't make visual
  for(auto type : {Visual::IMAGE, Visual::ANIMATED_IMAGE})
  {
    Dali::Property::Map   map;
    Dali::Property::Array emptyArray;
    map[Toolkit::Visual::Property::TYPE] = type;
    map[ImageVisual::Property::URL]      = emptyArray;
    Visual::Base emptyVisual             = factory.CreateVisual(map);
    DALI_TEST_CHECK(emptyVisual);
    TestDebugVisual(application.GetScene(), emptyVisual, Visual::WIREFRAME, Vector2::ZERO);

    tet_infoline("Check that GetVisualObject returns the actual WireframeVisual");
    Toolkit::Internal::Visual::Base& visualImpl = GetImplementation(emptyVisual).GetVisualObject();
    DALI_TEST_CHECK(dynamic_cast<Toolkit::Internal::WireframeVisual*>(&visualImpl));

    tet_infoline("Compare the returned emptyVisual with the visual implementation, should be the same");
    DALI_TEST_CHECK(emptyVisual.GetObjectPtr() == &visualImpl);
  }

  EnvironmentVariable::SetTestEnvironmentVariable(DALI_DEBUG_RENDERING, "0");
  END_TEST;
}

int UtcDaliDebugRenderingGetVisualObject01(void)
{
  EnvironmentVariable::SetTestEnvironmentVariable(DALI_DEBUG_RENDERING, "1");
  ToolkitTestApplication application;

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK(factory);

  tet_infoline("Create a TextVisual when debugging is enabled, thus creating a proxy Wireframe Visual");

  Dali::Property::Map map;
  map[Toolkit::Visual::Property::TYPE] = Visual::TEXT;
  map[TextVisual::Property::TEXT]      = "Hello";

  Visual::Base textVisual = factory.CreateVisual(map);
  DALI_TEST_CHECK(textVisual);

  tet_infoline("Check that GetVisualObject returns the actual TextVisual");
  Toolkit::Internal::Visual::Base& visualImpl = GetImplementation(textVisual).GetVisualObject();
  DALI_TEST_CHECK(dynamic_cast<Toolkit::Internal::TextVisual*>(&visualImpl));

  tet_infoline("Compare the returned TextVisual with the visual implementation, should differ");
  DALI_TEST_CHECK(textVisual.GetObjectPtr() != &visualImpl);

  EnvironmentVariable::SetTestEnvironmentVariable(DALI_DEBUG_RENDERING, "0");
  END_TEST;
}

int UtcDaliDebugRenderingGetVisualObject02(void)
{
  ToolkitTestApplication application;

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK(factory);

  tet_infoline("Create a TextVisual without debugging enabled, thus no proxy Wireframe Visual");

  Dali::Property::Map map;
  map[Toolkit::Visual::Property::TYPE] = Visual::TEXT;
  map[TextVisual::Property::TEXT]      = "Hello";

  Visual::Base textVisual = factory.CreateVisual(map);
  DALI_TEST_CHECK(textVisual);

  tet_infoline("Check that GetVisualObject returns the actual TextVisual");
  Toolkit::Internal::Visual::Base& visualImpl = GetImplementation(textVisual).GetVisualObject();
  DALI_TEST_CHECK(dynamic_cast<Toolkit::Internal::TextVisual*>(&visualImpl));

  tet_infoline("Compare the returned TextVisual with the visual implementation, should be the same");
  DALI_TEST_CHECK(textVisual.GetObjectPtr() == &visualImpl);

  END_TEST;
}

int UtcDaliDebugRenderingGetVisualObject03(void)
{
  ToolkitTestApplication application;

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK(factory);

  tet_infoline("Create a WireframeVisual without debugging enabled, thus no proxy Wireframe Visual either");

  Dali::Property::Map map;
  map[Toolkit::Visual::Property::TYPE] = Visual::WIREFRAME;

  Visual::Base textVisual = factory.CreateVisual(map);
  DALI_TEST_CHECK(textVisual);

  tet_infoline("Check that GetVisualObject returns the WireframeVisual");
  Toolkit::Internal::Visual::Base& visualImpl = GetImplementation(textVisual).GetVisualObject();
  DALI_TEST_CHECK(dynamic_cast<Toolkit::Internal::WireframeVisual*>(&visualImpl));

  tet_infoline("Compare the returned Visual with the visual implementation, should be the same");
  DALI_TEST_CHECK(textVisual.GetObjectPtr() == &visualImpl);

  END_TEST;
}

int UtcDaliDebugRenderingRenderText(void)
{
  EnvironmentVariable::SetTestEnvironmentVariable(DALI_DEBUG_RENDERING, "1");
  ToolkitTestApplication application;
  tet_infoline("Ensure we can render text when in debug mode");

  try
  {
    Toolkit::TextLabel label = TextLabel::New("Hello");
    application.GetScene().Add(label);
    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false);
  }

  END_TEST;
}
