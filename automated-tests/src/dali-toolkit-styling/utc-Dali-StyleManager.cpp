/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/adaptor-framework/style-monitor.h>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/builder/builder.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/styling/style-manager-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dummy-control.h>
#include <stdlib.h>
#include <test-animation-data.h>
#include <test-button.h>
#include <toolkit-style-monitor.h>
#include <fstream>
#include <iostream>
#include <sstream>

// for Internal::StyleManager
#include <dali-toolkit/internal/styling/style-manager-impl.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
std::string defaultTheme;

} // anonymous namespace

void dali_style_manager_startup(void)
{
  test_return_value = TET_UNDEF;

  std::ifstream t("src/dali-toolkit-styling/default-theme.json");
  defaultTheme = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
}

void dali_style_manager_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
Visual::Base CheckVisual(Impl::DummyControl& dummyImpl, Property::Index visualId, int type, const char* location)
{
  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(visualId), true, location);
  Visual::Base visual = dummyImpl.GetVisual(visualId);
  DALI_TEST_EQUALS((bool)visual, true, location);
  Property::Map map;
  visual.CreatePropertyMap(map);
  Property::Value* value = map.Find(Toolkit::Visual::Property::TYPE);
  DALI_TEST_EQUALS(value != NULL, true, location);

  int visualType;
  value->Get(visualType);
  DALI_TEST_EQUALS(visualType, type, location);
  return visual;
}

Integration::Bitmap* CreateBitmap(unsigned int imageWidth, unsigned int imageHeight, unsigned int initialColor, Pixel::Format pixelFormat)
{
  Integration::Bitmap*      bitmap        = Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_RETAIN);
  Integration::PixelBuffer* pixbuffer     = bitmap->GetPackedPixelsProfile()->ReserveBuffer(pixelFormat, imageWidth, imageHeight, imageWidth, imageHeight);
  unsigned int              bytesPerPixel = GetBytesPerPixel(pixelFormat);

  memset(pixbuffer, initialColor, imageHeight * imageWidth * bytesPerPixel);

  return bitmap;
}

Integration::ResourcePointer CustomizeNinePatch(ToolkitTestApplication& application,
                                                unsigned int            ninePatchImageWidth,
                                                unsigned int            ninePatchImageHeight)
{
  TestPlatformAbstraction& platform = application.GetPlatform();

  Pixel::Format pixelFormat = Pixel::RGBA8888;

  tet_infoline("Create Bitmap");
  platform.SetClosestImageSize(Vector2(ninePatchImageWidth, ninePatchImageHeight));
  Integration::Bitmap* bitmap = CreateBitmap(ninePatchImageWidth, ninePatchImageHeight, 0xFF, pixelFormat);

  tet_infoline("Getting resource");
  Integration::ResourcePointer resourcePtr(bitmap);
  platform.SetSynchronouslyLoadedResource(resourcePtr);

  return resourcePtr;
}
} // anonymous namespace

int UtcDaliStyleManagerConstructorP(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliStyleManagerConstructorP");
  StyleManager styleManager;
  DALI_TEST_CHECK(!styleManager);
  END_TEST;
}

int UtcDaliStyleManagerCopyConstructorP(void)
{
  ToolkitTestApplication application;

  StyleManager styleManager = StyleManager::Get();
  StyleManager copyOfStyleManager(styleManager);

  DALI_TEST_CHECK(copyOfStyleManager);
  END_TEST;
}

int UtcDaliStyleManagerAssignmentOperatorP(void)
{
  ToolkitTestApplication application;

  StyleManager styleManager       = StyleManager::Get();
  StyleManager copyOfStyleManager = styleManager;

  DALI_TEST_CHECK(copyOfStyleManager);
  DALI_TEST_CHECK(copyOfStyleManager == styleManager);
  END_TEST;
}

int UtcDaliStyleManagerGet(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliStyleManagerGet");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo("StyleManager");
  DALI_TEST_CHECK(type);
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK(handle);

  StyleManager manager;

  manager = StyleManager::Get();
  DALI_TEST_CHECK(manager);

  StyleManager newManager = StyleManager::Get();
  DALI_TEST_CHECK(newManager);

  // Check that focus manager is a singleton
  DALI_TEST_CHECK(manager == newManager);
  END_TEST;
}

namespace
{
class StyleChangedSignalChecker : public ConnectionTracker
{
public:
  StyleChangedSignalChecker()
  : signalCount(0)
  {
  }

  void OnStyleChanged(StyleManager styleManager, StyleChange::Type type)
  {
    signalCount++;
  }

  void Reset()
  {
    signalCount = 0;
  }

public:
  int signalCount;
};

} // anonymous namespace

int UtcDaliStyleManagerApplyTheme(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing StyleManager ApplyTheme");

  const char* json1 =
    "{\n"
    "  \"constants\":\n"
    "  {\n"
    "    \"CONFIG_SCRIPT_LOG_LEVEL\":\"Verbose\"\n"
    "  },\n"
    "  \"styles\":\n"
    "  {\n"
    "    \"testbutton\":\n"
    "    {\n"
    "      \"backgroundColor\":[1.0,1.0,0.0,1.0],\n"
    "      \"foregroundColor\":[0.0,0.0,1.0,1.0]\n"
    "    }\n"
    "  }\n"
    "}\n";

  const char* json2 =
    "{\n"
    "  \"styles\":\n"
    "  {\n"
    "    \"testbutton\":\n"
    "    {\n"
    "      \"backgroundColor\":[1.0,0.0,0.0,1.0],\n"
    "      \"foregroundColor\":[0.0,1.0,1.0,1.0]\n"
    "    }\n"
    "  }\n"
    "}\n";

  // Add 2 buttons to test how many times the signal is sent
  Test::TestButton testButton  = Test::TestButton::New();
  Test::TestButton testButton2 = Test::TestButton::New();
  application.GetScene().Add(testButton);
  application.GetScene().Add(testButton2);
  StyleChangedSignalChecker styleChangedSignalHandler;
  StyleChangedSignalChecker styleChangedSignalHandler2;
  StyleManager              styleManager = StyleManager::Get();

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  // To ensure we make VisualFactory
  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::TEXT);
  Visual::Base textVisual = factory.CreateVisual(propertyMap);

  // Render and notify
  application.SendNotification();
  application.Render();

  tet_infoline("Apply the style");

  std::string themeFile("ThemeOne");
  Test::StyleMonitor::SetThemeFileOutput(themeFile, json1);
  StyleManager::Get().ApplyTheme(themeFile);

  Property::Value bgColor(testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR));
  Property::Value fgColor(testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR));

  DALI_TEST_EQUALS(bgColor, Property::Value(Color::YELLOW), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(fgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION);

  tet_infoline("Testing that the signal handler is called only once");
  DALI_TEST_EQUALS(styleChangedSignalHandler.signalCount, 1, TEST_LOCATION);

  tet_infoline("Override the background property");
  testButton.SetProperty(Test::TestButton::Property::BACKGROUND_COLOR, Color::GREEN);
  bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);
  DALI_TEST_EQUALS(bgColor, Property::Value(Color::GREEN), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(fgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION);

  // Render and notify
  application.SendNotification();
  application.Render();

  tet_infoline("Apply the style again");

  styleChangedSignalHandler.signalCount = 0;
  StyleManager::Get().ApplyTheme(themeFile);

  bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);

  tet_infoline("Check that the property is changed");
  DALI_TEST_EQUALS(bgColor, Property::Value(Color::YELLOW), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(fgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION);
  tet_infoline("Testing that the signal handler is called only once");
  DALI_TEST_EQUALS(styleChangedSignalHandler.signalCount, 1, TEST_LOCATION);

  tet_infoline("Load a different stylesheet");

  tet_infoline("Apply the new style");
  std::string themeFile2("ThemeTwo");
  Test::StyleMonitor::SetThemeFileOutput(themeFile2, json2);

  styleChangedSignalHandler.signalCount = 0;
  StyleManager::Get().ApplyTheme(themeFile2);

  bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);

  tet_infoline("Check that the properties change, but the signal gets sent only once");
  DALI_TEST_EQUALS(bgColor, Property::Value(Color::RED), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(fgColor, Property::Value(Color::CYAN), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(styleChangedSignalHandler.signalCount, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerApplyThemeN(void)
{
  ToolkitTestApplication application;

  tet_infoline("Testing StyleManager ApplyTheme with invalid json");

  const char* json1 =
    "{\n"
    "  \"constants\":\n"
    "  {\n"
    "    \"CONFIG_SCRIPT_LOG_LEVEL\":\"Verbose\"\n"
    "  },\n"
    "  \"styles\":\n"
    "  {\n"
    "    \"testbutton\":\n"
    "    {\n"
    "      \"backgroundColor\":[1.0,1.0,0.0,1.0],\n"
    "      \"foregroundColor\":[0.0,0.0,1.0,1.0]\n"
    "    }\n"
    "  }\n"
    "}\n";

  const char* jsonInvalid =
    "{\n"
    "  \"styles\":\n"
    "  {\n"
    "    \"testbutton\":\n"
    "    {\n"
    "      \"backgroundColor\":[1.0,0.0,0.0,1.0],\n"
    "      \"foregroundColor\":[0.0,1.0,1.0,1.0],\n" /// Deliberate Error: trailing comma
    "    }\n"
    "  }\n"
    "}\n";

  // Add 2 buttons to test how many times the signal is sent
  Test::TestButton testButton  = Test::TestButton::New();
  Test::TestButton testButton2 = Test::TestButton::New();
  application.GetScene().Add(testButton);
  application.GetScene().Add(testButton2);
  StyleChangedSignalChecker styleChangedSignalHandler;
  StyleChangedSignalChecker styleChangedSignalHandler2;
  StyleManager              styleManager = StyleManager::Get();

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  // To ensure we make VisualFactory
  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::TEXT);
  Visual::Base textVisual = factory.CreateVisual(propertyMap);

  // Render and notify
  application.SendNotification();
  application.Render();

  tet_infoline("Apply the style");

  std::string themeFile("ThemeOne");
  Test::StyleMonitor::SetThemeFileOutput(themeFile, json1);
  StyleManager::Get().ApplyTheme(themeFile);

  Property::Value bgColor(testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR));
  Property::Value fgColor(testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR));

  DALI_TEST_EQUALS(bgColor, Property::Value(Color::YELLOW), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(fgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION);

  tet_infoline("Testing that the signal handler is called only once");
  DALI_TEST_EQUALS(styleChangedSignalHandler.signalCount, 1, TEST_LOCATION);

  tet_infoline("Override the background property");
  testButton.SetProperty(Test::TestButton::Property::BACKGROUND_COLOR, Color::GREEN);
  bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);
  DALI_TEST_EQUALS(bgColor, Property::Value(Color::GREEN), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(fgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION);

  // Render and notify
  application.SendNotification();
  application.Render();

  tet_infoline("Apply the style again");

  styleChangedSignalHandler.signalCount = 0;
  StyleManager::Get().ApplyTheme(themeFile);

  bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);

  tet_infoline("Check that the property is changed");
  DALI_TEST_EQUALS(bgColor, Property::Value(Color::YELLOW), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(fgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION);
  tet_infoline("Testing that the signal handler is called only once");
  DALI_TEST_EQUALS(styleChangedSignalHandler.signalCount, 1, TEST_LOCATION);

  tet_infoline("Load a different stylesheet, with broken json file");

  tet_infoline("Apply the new style");
  std::string themeFile2("ThemeTwo");
  Test::StyleMonitor::SetThemeFileOutput(themeFile2, jsonInvalid);

  styleChangedSignalHandler.signalCount = 0;
  StyleManager::Get().ApplyTheme(themeFile2);

  bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);

  tet_infoline("Check that the properties not be change, but the signal gets sent only once (due to the default theme applied)");
  DALI_TEST_EQUALS(bgColor, Property::Value(Color::YELLOW), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(fgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(styleChangedSignalHandler.signalCount, 1, TEST_LOCATION);

  tet_infoline("Override the background property");
  testButton.SetProperty(Test::TestButton::Property::BACKGROUND_COLOR, Color::GREEN);
  bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);
  DALI_TEST_EQUALS(bgColor, Property::Value(Color::GREEN), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(fgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION);

  tet_infoline("Apply the broken style again");

  styleChangedSignalHandler.signalCount = 0;
  StyleManager::Get().ApplyTheme(themeFile2);

  bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);

  tet_infoline("Check that the property is not be changed");
  DALI_TEST_EQUALS(bgColor, Property::Value(Color::GREEN), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(fgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION);
  tet_infoline("Testing that the signal handler is called only once");
  DALI_TEST_EQUALS(styleChangedSignalHandler.signalCount, 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerApplyDefaultTheme(void)
{
  tet_infoline("Testing StyleManager ApplyTheme");

  const char* defaultTheme =
    "{\n"
    "  \"constants\":\n"
    "  {\n"
    "    \"CONFIG_SCRIPT_LOG_LEVEL\":\"Concise\"\n"
    "  },\n"
    "  \"styles\":\n"
    "  {\n"
    "    \"testbutton\":\n"
    "    {\n"
    "      \"backgroundColor\":[1.0,1.0,0.0,1.0],\n"
    "      \"foregroundColor\":[0.0,0.0,1.0,1.0]\n"
    "    }\n"
    "  }\n"
    "}\n";
  // Bg: Yellow, Fg: Blue

  const char* appTheme =
    "{\n"
    "  \"styles\":\n"
    "  {\n"
    "    \"testbutton\":\n"
    "    {\n"
    "      \"backgroundColor\":[1.0,0.0,1.0,1.0],\n"
    "      \"foregroundColor\":[0.0,1.0,0.0,1.0]\n"
    "    }\n"
    "  }\n"
    "}\n";
  // Bg::Magenta, Fg:Green

  std::string filepath(TEST_RESOURCE_DIR "");

  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json",
                                         defaultTheme);
  ToolkitTestApplication application;

  Test::TestButton testButton = Test::TestButton::New();
  application.GetScene().Add(testButton);
  StyleChangedSignalChecker styleChangedSignalHandler;
  StyleManager              styleManager = StyleManager::Get();

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Get the default:
  Property::Value defaultBgColor(testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR));
  Property::Value defaultFgColor(testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR));

  tet_infoline("Apply the style");

  std::string themeFile("ThemeOne");
  Test::StyleMonitor::SetThemeFileOutput(themeFile, appTheme);
  StyleManager::Get().ApplyTheme(themeFile);

  Property::Value bgColor(testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR));
  Property::Value fgColor(testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR));

  DALI_TEST_EQUALS(bgColor, Property::Value(Color::MAGENTA), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(fgColor, Property::Value(Color::GREEN), 0.001, TEST_LOCATION);

  tet_infoline("Testing that the signal handler is called only once");
  DALI_TEST_EQUALS(styleChangedSignalHandler.signalCount, 1, TEST_LOCATION);
  tet_infoline("Revert the style");

  styleChangedSignalHandler.signalCount = 0;
  StyleManager::Get().ApplyDefaultTheme();

  bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);

  tet_infoline("Check that the property is reverted");
  DALI_TEST_EQUALS(bgColor, defaultBgColor, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(fgColor, defaultFgColor, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(bgColor, Property::Value(Color::YELLOW), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(fgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION);
  tet_infoline("Testing that the signal handler is called only once");
  DALI_TEST_EQUALS(styleChangedSignalHandler.signalCount, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerSetStyleConstantP(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliStyleManagerSetStyleConstantP");

  StyleManager manager = StyleManager::Get();

  std::string     key("key");
  Property::Value value(100);

  manager.SetStyleConstant(key, value);

  Property::Value returnedValue;
  manager.GetStyleConstant(key, returnedValue);

  DALI_TEST_CHECK(value.Get<int>() == returnedValue.Get<int>());
  END_TEST;
}

int UtcDaliStyleManagerGetStyleConstantP(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliStyleManagerGetStyleConstantP");

  StyleManager manager = StyleManager::Get();

  std::string     key("key");
  Property::Value value(100);

  manager.SetStyleConstant(key, value);

  Property::Value returnedValue;
  manager.GetStyleConstant(key, returnedValue);

  DALI_TEST_CHECK(value.Get<int>() == returnedValue.Get<int>());
  END_TEST;
}

int UtcDaliStyleManagerGetStyleConstantN(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliStyleManagerGetStyleConstantN");

  StyleManager manager = StyleManager::Get();

  std::string     key2("key2");
  Property::Value returnedValue2;
  DALI_TEST_CHECK(!manager.GetStyleConstant(key2, returnedValue2));

  END_TEST;
}

int UtcDaliStyleManagerApplyStyle(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliStyleManagerApplyStyle - test that a style can be applied to a single button");

  const char* json1 =
    "{\n"
    "  \"constants\":\n"
    "  {\n"
    "    \"CONFIG_SCRIPT_LOG_LEVEL\":\"General\"\n"
    "  },\n"
    "  \"styles\":\n"
    "  {\n"
    "    \"testbutton\":\n"
    "    {\n"
    "      \"backgroundColor\":[1.0,1.0,0.0,1.0],\n"
    "      \"foregroundColor\":[0.0,0.0,1.0,1.0]\n"
    "    }\n"
    "  }\n"
    "}\n";

  const char* json2 =
    "{\n"
    "  \"styles\":\n"
    "  {\n"
    "    \"testbutton\":\n"
    "    {\n"
    "      \"backgroundColor\":[1.0,0.0,0.0,1.0],\n"
    "      \"foregroundColor\":[0.0,1.0,1.0,1.0]\n"
    "    }\n"
    "  }\n"
    "}\n";

  // Add 2 buttons
  Test::TestButton testButton  = Test::TestButton::New();
  Test::TestButton testButton2 = Test::TestButton::New();
  application.GetScene().Add(testButton);
  application.GetScene().Add(testButton2);
  StyleChangedSignalChecker styleChangedSignalHandler;
  StyleManager              styleManager = StyleManager::Get();

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  tet_infoline("Apply the style");

  std::string themeFile("ThemeOne");
  Test::StyleMonitor::SetThemeFileOutput(themeFile, json1);
  styleManager.ApplyTheme(themeFile);

  // Render and notify
  application.SendNotification();
  application.Render();

  Property::Value themedBgColor(testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR));
  Property::Value themedFgColor(testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR));

  // Apply the style to the test button:
  std::string themeFile2("ThemeTwo");
  Test::StyleMonitor::SetThemeFileOutput(themeFile2, json2);
  styleManager.ApplyStyle(testButton, themeFile2, "testbutton");

  tet_infoline("Check that the properties change for the first button");
  Property::Value bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  Property::Value fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);
  DALI_TEST_EQUALS(bgColor, Property::Value(Color::RED), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(fgColor, Property::Value(Color::CYAN), 0.001, TEST_LOCATION);

  DALI_TEST_NOT_EQUALS(bgColor, themedBgColor, 0.001, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(fgColor, themedFgColor, 0.001, TEST_LOCATION);

  tet_infoline("Check that the properties remain the same for the second button");
  bgColor = testButton2.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton2.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);
  DALI_TEST_EQUALS(bgColor, themedBgColor, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(fgColor, themedFgColor, 0.001, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerApplyStyleBeforeAdaptorInitialized(void)
{
  tet_infoline("UtcDaliStyleManagerApplyStyleBeforeAdaptorInitialized - test that a style can be applied to a single button before application initialized");

  const char* json1 =
    "{\n"
    "  \"constants\":\n"
    "  {\n"
    "    \"CONFIG_SCRIPT_LOG_LEVEL\":\"General\"\n"
    "  },\n"
    "  \"styles\":\n"
    "  {\n"
    "    \"testbutton\":\n"
    "    {\n"
    "      \"backgroundColor\":[1.0,1.0,0.0,1.0],\n"
    "      \"foregroundColor\":[0.0,0.0,1.0,1.0]\n"
    "    }\n"
    "  }\n"
    "}\n";

  Dali::StyleMonitor styleMonitor = Dali::StyleMonitor::Get();

  tet_infoline("Apply the style before application initialized");

  std::string themeFile("ThemeOne");
  Test::StyleMonitor::SetThemeFileOutput(themeFile, json1);
  styleMonitor.SetTheme(themeFile);

  // Set global variable to change style at style manager.
  Test::StyleMonitor::SetThemeChangedBeforeAdaptorInit(true);

  ToolkitTestApplication application;

  // Reset global variable for next test
  Test::StyleMonitor::SetThemeChangedBeforeAdaptorInit(false);

  // Add 2 buttons
  Test::TestButton testButton = Test::TestButton::New();
  application.GetScene().Add(testButton);

  // Render and notify
  application.SendNotification();
  application.Render();

  Property::Value themedBgColor(testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR));
  Property::Value themedFgColor(testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR));

  tet_infoline("Check that the properties change for the button");
  DALI_TEST_EQUALS(themedBgColor, Property::Value(Color::YELLOW), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(themedFgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerIncludeStyleP(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliStyleManagerIncludeStyle - test that style sheet inclusion works");

  const char* json1 =
    "{\n"
    "  \"includes\":\n"
    "  [\n"
    "     \"src/dali-toolkit-styling/theme2.json\"\n"
    "  ],\n"
    "  \"styles\":\n"
    "  {\n"
    "    \"testbutton\":\n"
    "    {\n"
    "      \"foregroundColor\":[0.0,0.0,1.0,1.0]\n"
    "    }\n"
    "  }\n"
    "}\n";

  // Add 2 buttons
  Test::TestButton testButton  = Test::TestButton::New();
  Test::TestButton testButton2 = Test::TestButton::New();
  application.GetScene().Add(testButton);
  application.GetScene().Add(testButton2);
  StyleChangedSignalChecker styleChangedSignalHandler;
  StyleManager              styleManager = StyleManager::Get();

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  tet_infoline("Apply the style");

  std::string themeFile("ThemeOne");
  Test::StyleMonitor::SetThemeFileOutput(themeFile, json1);

  styleManager.ApplyTheme(themeFile);

  // Render and notify
  application.SendNotification();
  application.Render();

  Property::Value themedBgColor(testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR));
  Property::Value themedFgColor(testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR));

  DALI_TEST_EQUALS(themedBgColor, Property::Value(Color::YELLOW), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(themedFgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerIncludeStyleN(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliStyleManagerIncludeStyle - test that style sheet inclusion works, but included stylesheet is bad json");

  const char* json1 =
    "{\n"
    "  \"includes\":\n"
    "  [\n"
    "     \"src/dali-toolkit-styling/theme3.json\"\n"
    "  ],\n"
    "  \"styles\":\n"
    "  {\n"
    "    \"testbutton\":\n"
    "    {\n"
    "      \"foregroundColor\":[0.0,0.0,1.0,1.0]\n"
    "    }\n"
    "  }\n"
    "}\n";

  // Add 2 buttons
  Test::TestButton testButton  = Test::TestButton::New();
  Test::TestButton testButton2 = Test::TestButton::New();

  // Set some property, to avoid random value returns.
  testButton.SetProperty(Test::TestButton::Property::BACKGROUND_COLOR, Color::WHITE);
  testButton.SetProperty(Test::TestButton::Property::FOREGROUND_COLOR, Color::BLACK);

  application.GetScene().Add(testButton);
  application.GetScene().Add(testButton2);
  StyleChangedSignalChecker styleChangedSignalHandler;
  StyleManager              styleManager = StyleManager::Get();

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  tet_infoline("Apply the style");

  std::string themeFile("ThemeOne");
  Test::StyleMonitor::SetThemeFileOutput(themeFile, json1);

  styleManager.ApplyTheme(themeFile);

  // Render and notify
  application.SendNotification();
  application.Render();

  Property::Value themedBgColor(testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR));
  Property::Value themedFgColor(testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR));

  tet_infoline("Test that broken json didnt' give any effort");

  DALI_TEST_EQUALS(themedBgColor, Property::Value(Color::WHITE), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(themedFgColor, Property::Value(Color::BLACK), 0.001, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerStyleChangedSignalFontFamily(void)
{
  tet_infoline("Test that the StyleChange signal is fired when the font family is altered");
  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json",
                                         defaultTheme);

  ToolkitTestApplication application;

  std::string        labelStr("Label");
  Toolkit::TextLabel label = Toolkit::TextLabel::New(labelStr);
  application.GetScene().Add(label);

  Toolkit::TextField field = Toolkit::TextField::New();
  application.GetScene().Add(field);

  Toolkit::TextEditor editor = Toolkit::TextEditor::New();
  application.GetScene().Add(editor);

  StyleChangedSignalChecker styleChangedSignalHandler;
  Dali::StyleMonitor        styleMonitor = Dali::StyleMonitor::Get();
  StyleManager              styleManager = StyleManager::Get();

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  Test::StyleMonitor::SetDefaultFontFamily("Times New Roman");

  styleMonitor.StyleChangeSignal().Emit(styleMonitor, StyleChange::DEFAULT_FONT_CHANGE);

  tet_infoline("Test that the StyleChanged signal is received only once");

  DALI_TEST_EQUALS(styleChangedSignalHandler.signalCount, 1, TEST_LOCATION);

  // Check that the label's font style has been altered
  Property::Value family = label.GetProperty(TextLabel::Property::FONT_FAMILY);
  std::string     familyStr;
  family.Get(familyStr);

  DALI_TEST_EQUALS(familyStr, "Times New Roman", TEST_LOCATION);

  // Check that the field's font style has been altered
  family = field.GetProperty(TextField::Property::FONT_FAMILY);
  family.Get(familyStr);

  DALI_TEST_EQUALS(familyStr, "Times New Roman", TEST_LOCATION);

  // Check that the editor's font style has been altered
  family = editor.GetProperty(TextEditor::Property::FONT_FAMILY);
  family.Get(familyStr);

  DALI_TEST_EQUALS(familyStr, "Times New Roman", TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerStyleChangedSignalFontSize(void)
{
  tet_infoline("Test that the StyleChange signal is fired when the font size is altered");

  const char* defaultTheme =
    "{\n"
    "  \"styles\":\n"
    "  {\n"
    "    \"textlabelFontSize0\":\n"
    "    {\n"
    "      \"pointSize\":10\n"
    "    },\n"
    "    \"textlabelFontSize1\":\n"
    "    {\n"
    "      \"pointSize\":10\n"
    "    },\n"
    "    \"textlabelFontSize2\":\n"
    "    {\n"
    "      \"pointSize\":12\n"
    "    },\n"
    "    \"textlabelFontSize3\":\n"
    "    {\n"
    "      \"pointSize\":14\n"
    "    },\n"
    "    \"textlabelFontSize4\":\n"
    "    {\n"
    "      \"pointSize\":16\n"
    "    }\n"
    "  }\n"
    "}\n";

  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json", defaultTheme);

  ToolkitTestApplication application;

  std::string        labelStr("Label");
  Toolkit::TextLabel label = Toolkit::TextLabel::New(labelStr);
  application.GetScene().Add(label);

  Toolkit::TextLabel label2 = Toolkit::TextLabel::New(labelStr);
  application.GetScene().Add(label2);

  StyleChangedSignalChecker styleChangedSignalHandler;
  StyleMonitor              styleMonitor = StyleMonitor::Get();
  StyleManager              styleManager = StyleManager::Get();

  label.SetProperty(TextLabel::Property::POINT_SIZE, 10.0f);

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  Test::StyleMonitor::SetDefaultFontSize(2);
  styleMonitor.StyleChangeSignal().Emit(styleMonitor, StyleChange::DEFAULT_FONT_SIZE_CHANGE);

  tet_infoline("Test that the StyleChanged signal is received only once");
  DALI_TEST_EQUALS(styleChangedSignalHandler.signalCount, 1, TEST_LOCATION);

  tet_infoline("Test that the label's font size has been altered\n");
  Property::Value pointSizeValue = label.GetProperty(TextLabel::Property::POINT_SIZE);
  float           pointSize;
  pointSizeValue.Get(pointSize);

  DALI_TEST_EQUALS(pointSize, 12.0f, 0.001, TEST_LOCATION);

  styleChangedSignalHandler.signalCount = 0;

  Test::StyleMonitor::SetDefaultFontSize(4);
  styleMonitor.StyleChangeSignal().Emit(styleMonitor, StyleChange::DEFAULT_FONT_SIZE_CHANGE);

  tet_infoline("Test that the StyleChanged signal is received only once");
  DALI_TEST_EQUALS(styleChangedSignalHandler.signalCount, 1, TEST_LOCATION);

  // Check that the label's font style has been altered
  pointSizeValue = label.GetProperty(TextLabel::Property::POINT_SIZE);
  pointSizeValue.Get(pointSize);

  DALI_TEST_EQUALS(pointSize, 16.0f, 0.001, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerStyleChangedSignalFontSizeTextField(void)
{
  tet_infoline("Test that the StyleChange signal is fired when the font size is altered");

  const char* defaultTheme =
    "{\n"
    "  \"styles\":\n"
    "  {\n"
    "    \"textfieldFontSize0\":\n"
    "    {\n"
    "      \"pointSize\":8\n"
    "    },\n"
    "    \"textfieldFontSize1\":\n"
    "    {\n"
    "      \"pointSize\":10\n"
    "    },\n"
    "    \"textfieldFontSize2\":\n"
    "    {\n"
    "      \"pointSize\":12\n"
    "    },\n"
    "    \"textfieldFontSize3\":\n"
    "    {\n"
    "      \"pointSize\":14\n"
    "    },\n"
    "    \"textfieldFontSize4\":\n"
    "    {\n"
    "      \"pointSize\":16\n"
    "    }\n"
    "  }\n"
    "}\n";

  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json", defaultTheme);

  ToolkitTestApplication application;

  std::string        fieldStr("Field");
  Toolkit::TextField field = Toolkit::TextField::New();
  field.SetProperty(Toolkit::TextField::Property::TEXT, fieldStr);
  application.GetScene().Add(field);

  Toolkit::TextField field2 = Toolkit::TextField::New();
  application.GetScene().Add(field2);
  field2.SetProperty(Toolkit::TextField::Property::TEXT, fieldStr);

  StyleChangedSignalChecker styleChangedSignalHandler;
  StyleMonitor              styleMonitor = StyleMonitor::Get();
  StyleManager              styleManager = StyleManager::Get();

  field.SetProperty(TextField::Property::POINT_SIZE, 10.0f);

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  Test::StyleMonitor::SetDefaultFontSize(2);
  styleMonitor.StyleChangeSignal().Emit(styleMonitor, StyleChange::DEFAULT_FONT_SIZE_CHANGE);

  tet_infoline("Test that the StyleChanged signal is received only once");
  DALI_TEST_EQUALS(styleChangedSignalHandler.signalCount, 1, TEST_LOCATION);

  tet_infoline("Test that the field's font size has been altered\n");
  Property::Value pointSizeValue = field.GetProperty(TextField::Property::POINT_SIZE);
  float           pointSize;
  pointSizeValue.Get(pointSize);

  DALI_TEST_EQUALS(pointSize, 12.0f, 0.001, TEST_LOCATION);

  styleChangedSignalHandler.signalCount = 0;

  Test::StyleMonitor::SetDefaultFontSize(4);
  styleMonitor.StyleChangeSignal().Emit(styleMonitor, StyleChange::DEFAULT_FONT_SIZE_CHANGE);

  tet_infoline("Test that the StyleChanged signal is received only once");
  DALI_TEST_EQUALS(styleChangedSignalHandler.signalCount, 1, TEST_LOCATION);

  // Check that the field's font style has been altered
  pointSizeValue = field.GetProperty(TextField::Property::POINT_SIZE);
  pointSizeValue.Get(pointSize);

  DALI_TEST_EQUALS(pointSize, 16.0f, 0.001, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerStyleChangedSignalFontSizeTextEditor(void)
{
  tet_infoline("Test that the StyleChange signal is fired when the font size is altered");

  const char* defaultTheme =
    "{\n"
    "  \"styles\":\n"
    "  {\n"
    "    \"texteditorFontSize0\":\n"
    "    {\n"
    "      \"pointSize\":10\n"
    "    },\n"
    "    \"texteditorFontSize1\":\n"
    "    {\n"
    "      \"pointSize\":12\n"
    "    },\n"
    "    \"texteditorFontSize2\":\n"
    "    {\n"
    "      \"pointSize\":14\n"
    "    },\n"
    "    \"texteditorFontSize3\":\n"
    "    {\n"
    "      \"pointSize\":18\n"
    "    },\n"
    "    \"texteditorFontSize4\":\n"
    "    {\n"
    "      \"pointSize\":25\n"
    "    }\n"
    "  }\n"
    "}\n";

  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json", defaultTheme);

  ToolkitTestApplication application;

  std::string         editorStr("Editor");
  Toolkit::TextEditor editor = Toolkit::TextEditor::New();
  editor.SetProperty(Toolkit::TextEditor::Property::TEXT, editorStr);
  application.GetScene().Add(editor);

  Toolkit::TextEditor editor2 = Toolkit::TextEditor::New();
  application.GetScene().Add(editor2);
  editor2.SetProperty(Toolkit::TextEditor::Property::TEXT, editorStr);

  StyleChangedSignalChecker styleChangedSignalHandler;
  StyleMonitor              styleMonitor = StyleMonitor::Get();
  StyleManager              styleManager = StyleManager::Get();

  editor.SetProperty(TextEditor::Property::POINT_SIZE, 10.0f);

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  Test::StyleMonitor::SetDefaultFontSize(2);
  styleMonitor.StyleChangeSignal().Emit(styleMonitor, StyleChange::DEFAULT_FONT_SIZE_CHANGE);

  tet_infoline("Test that the StyleChanged signal is received only once");
  DALI_TEST_EQUALS(styleChangedSignalHandler.signalCount, 1, TEST_LOCATION);

  tet_infoline("Test that the editor's font size has been altered\n");
  Property::Value pointSizeValue = editor.GetProperty(TextEditor::Property::POINT_SIZE);
  float           pointSize;
  pointSizeValue.Get(pointSize);

  DALI_TEST_EQUALS(pointSize, 14.0f, 0.001, TEST_LOCATION);

  styleChangedSignalHandler.signalCount = 0;

  Test::StyleMonitor::SetDefaultFontSize(4);
  styleMonitor.StyleChangeSignal().Emit(styleMonitor, StyleChange::DEFAULT_FONT_SIZE_CHANGE);

  tet_infoline("Test that the StyleChanged signal is received only once");
  DALI_TEST_EQUALS(styleChangedSignalHandler.signalCount, 1, TEST_LOCATION);

  // Check that the editor's font style has been altered
  pointSizeValue = editor.GetProperty(TextEditor::Property::POINT_SIZE);
  pointSizeValue.Get(pointSize);

  DALI_TEST_EQUALS(pointSize, 25.0f, 0.001, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerSetState01(void)
{
  tet_infoline("Instantiate dummy control and test state/visual/transition capture");
  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json",
                                         defaultTheme);

  ToolkitTestApplication application;

  StyleChangedSignalChecker styleChangedSignalHandler;
  Dali::StyleMonitor        styleMonitor = Dali::StyleMonitor::Get();
  StyleManager              styleManager = StyleManager::Get();

  DummyControl actor = DummyControl::New(true);
  actor.SetStyleName("BasicControl");
  application.GetScene().Add(actor);

  Impl::DummyControl&          dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  Integration::ResourcePointer ninePatch = CustomizeNinePatch(application, 30, 30);

  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);
  Visual::Base  visual1      = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);
  Visual::Base  labelVisual1 = dummyImpl.GetVisual(DummyControl::Property::LABEL_VISUAL);
  Property::Map labelMap;
  labelVisual1.CreatePropertyMap(labelMap);
  labelMap[TextVisual::Property::TEXT] = "New text";
  VisualFactory factory                = VisualFactory::Get();
  labelVisual1                         = factory.CreateVisual(labelMap);
  dummyImpl.UnregisterVisual(DummyControl::Property::LABEL_VISUAL);
  dummyImpl.RegisterVisual(DummyControl::Property::LABEL_VISUAL, labelVisual1);

  actor.SetProperty(DevelControl::Property::STATE, DevelControl::FOCUSED);

  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);
  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOCUS_VISUAL), true, TEST_LOCATION);
  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::LABEL_VISUAL), true, TEST_LOCATION);

  Visual::Base visual2      = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);
  Visual::Base labelVisual2 = dummyImpl.GetVisual(DummyControl::Property::LABEL_VISUAL);
  DALI_TEST_CHECK(visual1 != visual2);
  DALI_TEST_CHECK(labelVisual1 != labelVisual2);
  labelMap.Clear();
  labelVisual2.CreatePropertyMap(labelMap);
  Property::Value* textValue = labelMap.Find(Toolkit::TextVisual::Property::TEXT, "text");
  DALI_TEST_CHECK(textValue);
  Property::Value* pointSizeValue = labelMap.Find(Toolkit::TextVisual::Property::POINT_SIZE, "pointSize");
  tet_infoline("Check that the instance data has been copied to the new text visual\n");
  DALI_TEST_EQUALS(textValue->Get<std::string>(), "New text", TEST_LOCATION);
  DALI_TEST_EQUALS(pointSizeValue->Get<int>(), 10, TEST_LOCATION);

  actor.SetProperty(DevelControl::Property::STATE, DevelControl::DISABLED);

  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);

  Visual::Base visual3     = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);
  Visual::Base focusVisual = dummyImpl.GetVisual(DummyControl::Property::FOCUS_VISUAL);
  DALI_TEST_CHECK(!focusVisual);
  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOCUS_VISUAL), false, TEST_LOCATION);

  DALI_TEST_CHECK(visual1 != visual3);
  DALI_TEST_CHECK(visual2 != visual3);

  Visual::Base labelVisual3 = dummyImpl.GetVisual(DummyControl::Property::LABEL_VISUAL);
  DALI_TEST_CHECK(labelVisual2 != labelVisual3);

  labelVisual2.CreatePropertyMap(labelMap);
  textValue = labelMap.Find(Toolkit::TextVisual::Property::TEXT, "text");
  DALI_TEST_CHECK(textValue);
  pointSizeValue = labelMap.Find(Toolkit::TextVisual::Property::POINT_SIZE, "pointSize");
  tet_infoline("Check that the instance data has been copied to the new text visual\n");
  DALI_TEST_EQUALS(textValue->Get<std::string>(), "New text", TEST_LOCATION);
  DALI_TEST_EQUALS(pointSizeValue->Get<int>(), 10, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerSetState02(void)
{
  tet_infoline("Instantiate dummy control and test state/visual/transition capture");
  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json",
                                         defaultTheme);

  ToolkitTestApplication application;

  StyleChangedSignalChecker styleChangedSignalHandler;
  Dali::StyleMonitor        styleMonitor = Dali::StyleMonitor::Get();
  StyleManager              styleManager = StyleManager::Get();

  DummyControl actor = DummyControl::New(true);
  actor.SetStyleName("BasicControl");
  application.GetScene().Add(actor);

  Impl::DummyControl&          dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  Integration::ResourcePointer ninePatch = CustomizeNinePatch(application, 30, 30);

  int state = actor.GetProperty<int>(DevelControl::Property::STATE);
  DALI_TEST_EQUALS(state, (int)DevelControl::NORMAL, TEST_LOCATION);

  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);
  Visual::Base visual1 = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);

  actor.SetProperty(DevelControl::Property::STATE,
                    Property::Map().Add("state", "FOCUSED").Add("withTransitions", false));

  state = actor.GetProperty<int>(DevelControl::Property::STATE);
  DALI_TEST_EQUALS(state, (int)DevelControl::FOCUSED, TEST_LOCATION);

  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);
  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOCUS_VISUAL), true, TEST_LOCATION);

  Visual::Base visual2 = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);
  DALI_TEST_CHECK(visual1 != visual2);

  actor.SetProperty(DevelControl::Property::STATE,
                    Property::Map().Add("state", "DISABLED").Add("withTransitions", false));

  state = actor.GetProperty<int>(DevelControl::Property::STATE);
  DALI_TEST_EQUALS(state, (int)DevelControl::DISABLED, TEST_LOCATION);

  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);

  Visual::Base visual3 = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);

  Visual::Base testVisual = dummyImpl.GetVisual(DummyControl::Property::FOCUS_VISUAL);
  DALI_TEST_CHECK(!testVisual);
  testVisual = dummyImpl.GetVisual(DummyControl::Property::TEST_VISUAL);
  DALI_TEST_CHECK(!testVisual);
  testVisual = dummyImpl.GetVisual(DummyControl::Property::TEST_VISUAL2);
  DALI_TEST_CHECK(!testVisual);
  testVisual = dummyImpl.GetVisual(DummyControl::Property::LABEL_VISUAL);
  DALI_TEST_CHECK(testVisual);

  DALI_TEST_CHECK(visual1 != visual3);
  DALI_TEST_CHECK(visual2 != visual3);

  actor.SetProperty(DevelControl::Property::STATE,
                    Property::Map().Add("state", "NORMAL").Add("withTransitions", false));

  state = actor.GetProperty<int>(DevelControl::Property::STATE);
  DALI_TEST_EQUALS(state, (int)DevelControl::NORMAL, TEST_LOCATION);

  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);

  visual1 = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);
  DALI_TEST_CHECK(visual1);

  Visual::Base focusVisual = dummyImpl.GetVisual(DummyControl::Property::FOCUS_VISUAL);
  DALI_TEST_CHECK(!focusVisual);
  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOCUS_VISUAL), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerSetState03N(void)
{
  tet_infoline("Instantiate dummy control and test state transition without state style");
  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json",
                                         defaultTheme);

  ToolkitTestApplication application;

  StyleChangedSignalChecker styleChangedSignalHandler;
  Dali::StyleMonitor        styleMonitor = Dali::StyleMonitor::Get();
  StyleManager              styleManager = StyleManager::Get();

  DummyControl actor = DummyControl::New(true);
  actor.SetStyleName("NoStyles");
  application.GetScene().Add(actor);

  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  Property::Map       propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
  VisualFactory factory = VisualFactory::Get();
  Visual::Base  visual  = factory.CreateVisual(propertyMap);
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  int state = actor.GetProperty<int>(DevelControl::Property::STATE);
  DALI_TEST_EQUALS(state, (int)DevelControl::NORMAL, TEST_LOCATION);

  actor.SetProperty(DevelControl::Property::STATE,
                    Property::Map().Add("state", "FOCUSED").Add("withTransitions", false));

  Visual::Base testVisual = dummyImpl.GetVisual(DummyControl::Property::TEST_VISUAL);
  DALI_TEST_CHECK(testVisual = visual);

  state = actor.GetProperty<int>(DevelControl::Property::STATE);
  DALI_TEST_EQUALS(state, (int)DevelControl::FOCUSED, TEST_LOCATION);

  actor.SetProperty(DevelControl::Property::STATE,
                    Property::Map().Add("state", "DISABLED").Add("withTransitions", false));

  testVisual = dummyImpl.GetVisual(DummyControl::Property::TEST_VISUAL);
  DALI_TEST_CHECK(testVisual = visual);

  state = actor.GetProperty<int>(DevelControl::Property::STATE);
  DALI_TEST_EQUALS(state, (int)DevelControl::DISABLED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerSetState04N(void)
{
  tet_infoline("Instantiate dummy control and test state transition with style without state");
  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json",
                                         defaultTheme);

  ToolkitTestApplication application;

  StyleChangedSignalChecker styleChangedSignalHandler;
  Dali::StyleMonitor        styleMonitor = Dali::StyleMonitor::Get();
  StyleManager              styleManager = StyleManager::Get();

  DummyControl actor = DummyControl::New(true);
  actor.SetStyleName("NoStateStyle");
  application.GetScene().Add(actor);

  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  Property::Map       propertyMap;
  propertyMap.Insert(Visual::Property::TYPE, Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
  VisualFactory factory = VisualFactory::Get();
  Visual::Base  visual  = factory.CreateVisual(propertyMap);
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  int state = actor.GetProperty<int>(DevelControl::Property::STATE);
  DALI_TEST_EQUALS(state, (int)DevelControl::NORMAL, TEST_LOCATION);

  actor.SetProperty(DevelControl::Property::STATE,
                    Property::Map().Add("state", "FOCUSED").Add("withTransitions", false));

  Visual::Base testVisual = dummyImpl.GetVisual(DummyControl::Property::TEST_VISUAL);
  DALI_TEST_CHECK(testVisual = visual);

  state = actor.GetProperty<int>(DevelControl::Property::STATE);
  DALI_TEST_EQUALS(state, (int)DevelControl::FOCUSED, TEST_LOCATION);

  actor.SetProperty(DevelControl::Property::STATE,
                    Property::Map().Add("state", "DISABLED").Add("withTransitions", false));

  testVisual = dummyImpl.GetVisual(DummyControl::Property::TEST_VISUAL);
  DALI_TEST_CHECK(testVisual = visual);

  state = actor.GetProperty<int>(DevelControl::Property::STATE);
  DALI_TEST_EQUALS(state, (int)DevelControl::DISABLED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerSetSubState01(void)
{
  tet_infoline("Instantiate dummy control and test state/visual/transition capture");
  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json",
                                         defaultTheme);

  ToolkitTestApplication application;

  StyleChangedSignalChecker styleChangedSignalHandler;
  Dali::StyleMonitor        styleMonitor = Dali::StyleMonitor::Get();
  StyleManager              styleManager = StyleManager::Get();

  DummyControl actor = DummyControl::New(true);
  actor.SetProperty(DevelControl::Property::STATE, "NORMAL");
  actor.SetProperty(DevelControl::Property::SUB_STATE, "SELECTED");
  actor.SetStyleName("ComplexControl");
  application.GetScene().Add(actor);

  Integration::ResourcePointer ninePatch = CustomizeNinePatch(application, 30, 30);

  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());

  CheckVisual(dummyImpl, DummyControl::Property::FOREGROUND_VISUAL, Toolkit::Visual::IMAGE, TEST_LOCATION);
  CheckVisual(dummyImpl, DummyControl::Property::TEST_VISUAL, Toolkit::Visual::IMAGE, TEST_LOCATION);
  CheckVisual(dummyImpl, DummyControl::Property::TEST_VISUAL2, Toolkit::Visual::GRADIENT, TEST_LOCATION);

  actor.SetProperty(DevelControl::Property::SUB_STATE, "UNSELECTED");

  CheckVisual(dummyImpl, DummyControl::Property::FOREGROUND_VISUAL, Toolkit::Visual::IMAGE, TEST_LOCATION);
  CheckVisual(dummyImpl, DummyControl::Property::TEST_VISUAL, Toolkit::Visual::IMAGE, TEST_LOCATION);
  CheckVisual(dummyImpl, DummyControl::Property::TEST_VISUAL2, Toolkit::Visual::COLOR, TEST_LOCATION);

  actor.SetProperty(DevelControl::Property::SUB_STATE, "SELECTED");

  CheckVisual(dummyImpl, DummyControl::Property::FOREGROUND_VISUAL, Toolkit::Visual::IMAGE, TEST_LOCATION);
  CheckVisual(dummyImpl, DummyControl::Property::TEST_VISUAL, Toolkit::Visual::IMAGE, TEST_LOCATION);
  CheckVisual(dummyImpl, DummyControl::Property::TEST_VISUAL2, Toolkit::Visual::GRADIENT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerSetSubState02(void)
{
  tet_infoline("Instantiate complex control and test state/substate change");
  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json",
                                         defaultTheme);

  ToolkitTestApplication application;

  StyleChangedSignalChecker styleChangedSignalHandler;
  Dali::StyleMonitor        styleMonitor = Dali::StyleMonitor::Get();
  StyleManager              styleManager = StyleManager::Get();

  DummyControl actor = DummyControl::New(true);
  actor.SetProperty(DevelControl::Property::STATE, "NORMAL");
  actor.SetProperty(DevelControl::Property::SUB_STATE, "SELECTED");
  tet_infoline("Setting state to NORMAL/SELECTED before re-styling\n");

  actor.SetStyleName("ComplexControl");
  application.GetScene().Add(actor);

  Integration::ResourcePointer ninePatch = CustomizeNinePatch(application, 30, 30);

  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());

  CheckVisual(dummyImpl, DummyControl::Property::FOREGROUND_VISUAL, Toolkit::Visual::IMAGE, TEST_LOCATION);
  CheckVisual(dummyImpl, DummyControl::Property::TEST_VISUAL2, Toolkit::Visual::GRADIENT, TEST_LOCATION);

  actor.SetProperty(DevelControl::Property::SUB_STATE, "UNSELECTED");
  tet_infoline("Changing substate to UNSELECTED - check visual changes\n");

  CheckVisual(dummyImpl, DummyControl::Property::FOREGROUND_VISUAL, Toolkit::Visual::IMAGE, TEST_LOCATION);
  CheckVisual(dummyImpl, DummyControl::Property::TEST_VISUAL2, Toolkit::Visual::COLOR, TEST_LOCATION);

  actor.SetProperty(DevelControl::Property::STATE, "FOCUSED");
  tet_infoline("Changing state to FOCUSED - check visual changes\n");

  Visual::Base fgVisual1    = CheckVisual(dummyImpl, DummyControl::Property::FOREGROUND_VISUAL, Toolkit::Visual::GRADIENT, TEST_LOCATION);
  Visual::Base focusVisual1 = CheckVisual(dummyImpl, DummyControl::Property::FOCUS_VISUAL, Toolkit::Visual::N_PATCH, TEST_LOCATION);

  actor.SetProperty(DevelControl::Property::SUB_STATE, "SELECTED");
  tet_infoline("Changing  substate to SELECTED - Expect no change\n");

  Visual::Base fgVisual2    = CheckVisual(dummyImpl, DummyControl::Property::FOREGROUND_VISUAL, Toolkit::Visual::GRADIENT, TEST_LOCATION);
  Visual::Base focusVisual2 = CheckVisual(dummyImpl, DummyControl::Property::FOCUS_VISUAL, Toolkit::Visual::N_PATCH, TEST_LOCATION);

  DALI_TEST_CHECK(fgVisual1 == fgVisual2);
  DALI_TEST_CHECK(focusVisual1 == focusVisual2);

  actor.SetProperty(DevelControl::Property::STATE, "NORMAL");
  tet_infoline("Changing state to NORMAL - Expect to change to NORMAL/SELECTED \n");

  CheckVisual(dummyImpl, DummyControl::Property::FOREGROUND_VISUAL, Toolkit::Visual::IMAGE, TEST_LOCATION);
  CheckVisual(dummyImpl, DummyControl::Property::TEST_VISUAL2, Toolkit::Visual::GRADIENT, TEST_LOCATION);

  Visual::Base focusVisual = dummyImpl.GetVisual(DummyControl::Property::FOCUS_VISUAL);
  DALI_TEST_CHECK(!focusVisual);

  actor.SetProperty(DevelControl::Property::STATE, "DISABLED");
  tet_infoline("Changing state to DISABLED - Expect to change to DISABLED/SELECTED \n");

  CheckVisual(dummyImpl, DummyControl::Property::FOREGROUND_VISUAL, Toolkit::Visual::COLOR, TEST_LOCATION);
  CheckVisual(dummyImpl, DummyControl::Property::TEST_VISUAL, Toolkit::Visual::IMAGE, TEST_LOCATION);

  Visual::Base testVisual = dummyImpl.GetVisual(DummyControl::Property::FOCUS_VISUAL);
  DALI_TEST_CHECK(!testVisual);
  testVisual = dummyImpl.GetVisual(DummyControl::Property::LABEL_VISUAL);
  DALI_TEST_CHECK(!testVisual);

  END_TEST;
}

int UtcDaliStyleManagerConfigSectionTestP(void)
{
  tet_infoline("Test that the properties in config section are works");

  const char* defaultTheme =
    "{\n"
    "  \"constants\":\n"
    "  {\n"
    "    \"TEST\":\"broken\"\n"
    "  },\n"
    "  \"config\":\n"
    "  {\n"
    "    \"brokenImageUrl\":\"{TEST}|{TEST}|{TEST|TEST.png\",\n"
    "    \"alwaysShowFocus\":false,\n"
    "    \"clearFocusOnEscape\":false\n"
    "  },\n"
    "  \"styles\":\n"
    "  {\n"
    "  }\n"
    "}\n";

  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json", defaultTheme);

  ToolkitTestApplication application;

  Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();

  Property::Map config          = Toolkit::DevelStyleManager::GetConfigurations(styleManager);
  bool          alwaysShowFocus = config["alwaysShowFocus"].Get<bool>();
  tet_printf("alwaysShowFocus : %d\n", alwaysShowFocus);
  DALI_TEST_CHECK(!alwaysShowFocus);
  bool clearFocusOnEscape = config["clearFocusOnEscape"].Get<bool>();
  tet_printf("clearFocusOnEscape : %d\n", clearFocusOnEscape);
  DALI_TEST_CHECK(!clearFocusOnEscape);
  std::string brokenImageUrl = config["brokenImageUrl"].Get<std::string>();
  tet_printf("broken image url : %s\n", brokenImageUrl.c_str());
  DALI_TEST_CHECK(brokenImageUrl.compare("broken|broken|{TEST|TEST.png") == 0);

  // For coverage
  Toolkit::TextEditor editor = Toolkit::TextEditor::New();
  editor.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  application.GetScene().Add(editor);

  Toolkit::KeyboardFocusManager::Get().SetCurrentFocusActor(editor);

  application.ProcessEvent(Integration::KeyEvent("", "", "", DALI_KEY_ESCAPE, 0, 0, Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliStyleManagerConfigSectionTestN(void)
{
  tet_infoline("Test that the properties in config section are works as default if theme is broken");

  const char* brokenTheme = "INVALID";

  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json", brokenTheme);
  try
  {
    ToolkitTestApplication application;

    Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();

    Property::Map config = Toolkit::DevelStyleManager::GetConfigurations(styleManager);
    DALI_TEST_CHECK(config.Empty());

    // For coverage
    Toolkit::TextEditor editor = Toolkit::TextEditor::New();
    editor.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
    application.GetScene().Add(editor);

    Toolkit::KeyboardFocusManager::Get().SetCurrentFocusActor(editor);

    application.ProcessEvent(Integration::KeyEvent("", "", "", DALI_KEY_ESCAPE, 0, 0, Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE));
    application.SendNotification();
    application.Render();

    Toolkit::TextLabel label = Toolkit::TextLabel::New();
    label.SetProperty(Toolkit::TextLabel::Property::TEXT, "Hello, World!");
    application.GetScene().Add(label);

    application.SendNotification();
    application.Render();
  }
  catch(...)
  {
    DALI_TEST_CHECK(false); ///< Should not get here
  }

  // Restore default theme data.
  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json", defaultTheme);

  END_TEST;
}

int UtcDaliStyleManagerNewWithAdditionalBehavior(void)
{
  ToolkitTestApplication application;

  Toolkit::StyleManager            styleManager     = StyleManager::Get();
  Toolkit::Internal::StyleManager& styleManagerImpl = GetImpl(styleManager);

  auto checkup = [&styleManagerImpl](int enableStyleChangeSignal, const Control& control)
  {
    DALI_TEST_EQUALS(enableStyleChangeSignal, styleManagerImpl.ControlStyleChangeSignal().GetConnectionCount(), TEST_LOCATION);
  };

  // Default New
  tet_infoline("Check whether ControlStyleChangeSignal connected in default New\n");
  checkup(1, Control::New());
  checkup(1, ImageView::New());
  checkup(1, ImageView::New("url"));
  checkup(1, ImageView::New("url", Dali::ImageDimensions(32u, 32u)));
  checkup(1, TextLabel::New());
  checkup(1, TextLabel::New("text"));
  checkup(1, TextField::New());
  checkup(1, TextEditor::New());

  // New with additional behaviour, but enable style change signals
  tet_infoline("Check whether ControlStyleChangeSignal connected in non-disable style change signals\n");
  checkup(1, Control::New(Toolkit::Control::ControlBehaviour::CONTROL_BEHAVIOUR_DEFAULT));
  checkup(1, Control::New(Toolkit::Control::ControlBehaviour::DISABLE_SIZE_NEGOTIATION));
  checkup(1, Control::New(Toolkit::Control::ControlBehaviour::REQUIRES_KEYBOARD_NAVIGATION_SUPPORT));
  checkup(1, ImageView::New(Toolkit::Control::ControlBehaviour::CONTROL_BEHAVIOUR_DEFAULT));
  checkup(1, ImageView::New(Toolkit::Control::ControlBehaviour::CONTROL_BEHAVIOUR_DEFAULT, "url"));
  checkup(1, ImageView::New(Toolkit::Control::ControlBehaviour::CONTROL_BEHAVIOUR_DEFAULT, "url", Dali::ImageDimensions(32u, 32u)));
  checkup(1, ImageView::New(Toolkit::Control::ControlBehaviour::DISABLE_SIZE_NEGOTIATION));
  checkup(1, ImageView::New(Toolkit::Control::ControlBehaviour::DISABLE_SIZE_NEGOTIATION, "url"));
  checkup(1, ImageView::New(Toolkit::Control::ControlBehaviour::DISABLE_SIZE_NEGOTIATION, "url", Dali::ImageDimensions(32u, 32u)));
  checkup(1, TextLabel::New(Toolkit::Control::ControlBehaviour::CONTROL_BEHAVIOUR_DEFAULT));
  checkup(1, TextLabel::New(Toolkit::Control::ControlBehaviour::CONTROL_BEHAVIOUR_DEFAULT, "text"));
  checkup(1, TextLabel::New(Toolkit::Control::ControlBehaviour::DISABLE_SIZE_NEGOTIATION));
  checkup(1, TextLabel::New(Toolkit::Control::ControlBehaviour::DISABLE_SIZE_NEGOTIATION, "text"));
  checkup(1, TextField::New(Toolkit::Control::ControlBehaviour::CONTROL_BEHAVIOUR_DEFAULT));
  checkup(1, TextField::New(Toolkit::Control::ControlBehaviour::DISABLE_SIZE_NEGOTIATION));
  checkup(1, TextEditor::New(Toolkit::Control::ControlBehaviour::CONTROL_BEHAVIOUR_DEFAULT));
  checkup(1, TextEditor::New(Toolkit::Control::ControlBehaviour::DISABLE_SIZE_NEGOTIATION));

  // New with additional behaviour, so disable style change signals
  tet_infoline("Check whether ControlStyleChangeSignal did not connected\n");
  checkup(0, Control::New(Toolkit::Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS));
  checkup(0, Control::New(Toolkit::Control::ControlBehaviour(Toolkit::Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS | Toolkit::Control::ControlBehaviour::DISABLE_SIZE_NEGOTIATION)));
  checkup(0, ImageView::New(Toolkit::Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS));
  checkup(0, ImageView::New(Toolkit::Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS, "url"));
  checkup(0, ImageView::New(Toolkit::Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS, "url", Dali::ImageDimensions(32u, 32u)));
  checkup(0, TextLabel::New(Toolkit::Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS));
  checkup(0, TextLabel::New(Toolkit::Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS, "text"));
  checkup(0, TextField::New(Toolkit::Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS));
  checkup(0, TextEditor::New(Toolkit::Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS));

  END_TEST;
}
