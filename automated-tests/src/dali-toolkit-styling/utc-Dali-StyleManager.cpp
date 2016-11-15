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

#include <dali/devel-api/adaptor-framework/style-monitor.h>

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-toolkit/devel-api/builder/builder.h>
#include <test-button.h>
#include <test-animation-data.h>
#include <toolkit-style-monitor.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* defaultTheme =
"{\n"
"  \"styles\":\n"
"  {\n"
"    \"textlabel\":\n"
"    {\n"
"      \"pointSize\":18\n"
"    },\n"
"\n"
"    \"textlabelFontSize0\":\n"
"    {\n"
"      \"pointSize\":8\n"
"    },\n"
"    \"textlabelFontSize1\":\n"
"    {\n"
"      \"pointSize\":10\n"
"    },\n"
"    \"textlabelFontSize2\":\n"
"    {\n"
"      \"pointSize\":15\n"
"    },\n"
"    \"textlabelFontSize3\":\n"
"    {\n"
"      \"pointSize\":19\n"
"    },\n"
"    \"textlabelFontSize4\":\n"
"    {\n"
"      \"pointSize\":25\n"
"    },\n"
"\n"
"    \"textfield\":\n"
"    {\n"
"      \"pointSize\":18,\n"
"      \"primaryCursorColor\":[0.0,0.72,0.9,1.0],\n"
"      \"secondaryCursorColor\":[0.0,0.72,0.9,1.0],\n"
"      \"cursorWidth\":3,\n"
"      \"selectionHighlightColor\":[0.75,0.96,1.0,1.0],\n"
"      \"grabHandleImage\" : \"{DALI_STYLE_IMAGE_DIR}cursor_handler_drop_center.png\",\n"
"      \"selectionHandleImageLeft\" : {\"filename\":\"{DALI_STYLE_IMAGE_DIR}selection_handle_drop_left.png\" },\n"
"      \"selectionHandleImageRight\": {\"filename\":\"{DALI_STYLE_IMAGE_DIR}selection_handle_drop_right.png\" }\n"
"    },\n"
"\n"
"    \"textfieldFontSize0\":\n"
"    {\n"
"      \"pointSize\":10\n"
"    },\n"
"    \"textfieldFontSize1\":\n"
"    {\n"
"      \"pointSize\":10\n"
"    },\n"
"    \"textfieldFontSize2\":\n"
"    {\n"
"      \"pointSize\":10\n"
"    },\n"
"    \"textfieldFontSize3\":\n"
"    {\n"
"      \"pointSize\":10\n"
"    },\n"
"    \"textfieldFontSize4\":\n"
"    {\n"
"      \"pointSize\":10\n"
"    },\n"
"    \"textselectionpopup\":\n"
"    {\n"
"      \"popupMaxSize\":[656,72],\n"
"      \"optionDividerSize\":[2,0],\n"
"      \"popupDividerColor\":[0.23,0.72,0.8,0.11],\n"
"      \"popupIconColor\":[1.0,1.0,1.0,1.0],\n"
"      \"popupPressedColor\":[0.24,0.72,0.8,0.11],\n"
"      \"background\": {\n"
"        \"rendererType\": \"nPatch\",\n"
"        \"imageUrl\": \"{DALI_IMAGE_DIR}selection-popup-bg.9.png\"\n"
"        },\n"
"      \"popupFadeInDuration\":0.25,\n"
"      \"popupFadeOutDuration\":0.25\n"
"    },\n"
"    \"textselectionpopupbutton\":\n"
"    {\n"
"      \"label\":\n"
"      {\n"
"        \"pointSize\":8,\n"
"        \"fontStyle\":\"{\\\\""weight\\\\"":\\\\""light\\\\""}\"\n"
"      }\n"
"    },\n"
"    \"textselectiontoolbar\":\n"
"    {\n"
"      \"enableOvershoot\":true,\n"
"      \"scrollView\":\n"
"      {\n"
"        \"overshootAnimationSpeed\":360.0,\n"
"        \"overshootSize\":[720.0,130.0]\n"
"      }\n"
"    },\n"
"    \"scrollview\":\n"
"    {\n"
"      \"overshootEffectColor\":\"B018\",\n"
"      \"overshootAnimationSpeed\":360.0,\n"
"      \"overshootSize\":[720.0,130.0]\n"
"    },\n"
"    \"itemview\":\n"
"    {\n"
"      \"overshootEffectColor\":\"B018\",\n"
"      \"overshootAnimationSpeed\":360.0,\n"
"      \"overshootSize\":[720.0,130.0]\n"
"    },\n"
"    \"texteditor\":\n"
"    {\n"
"      \"pointSize\":18,\n"
"      \"primaryCursorColor\":[0.0,0.72,0.9,1.0],\n"
"      \"secondaryCursorColor\":[0.0,0.72,0.9,1.0],\n"
"      \"cursorWidth\":3,\n"
"      \"selectionHighlightColor\":[0.75,0.96,1.0,1.0],\n"
"      \"grabHandleImage\" : \"{DALI_STYLE_IMAGE_DIR}cursor_handler_drop_center.png\",\n"
"      \"selectionHandleImageLeft\" : {\"filename\":\"{DALI_STYLE_IMAGE_DIR}selection_handle_drop_left.png\" },\n"
"      \"selectionHandleImageRight\": {\"filename\":\"{DALI_STYLE_IMAGE_DIR}selection_handle_drop_right.png\" }\n"
"    }\n"
"  }\n"
"}\n";

} // anonymous namespace



void dali_style_manager_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_style_manager_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliStyleManagerConstructorP(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliStyleManagerConstructorP");
  StyleManager styleManager;
  DALI_TEST_CHECK( !styleManager);
  END_TEST;
}

int UtcDaliStyleManagerCopyConstructorP(void)
{
  TestApplication application;

  StyleManager styleManager = StyleManager::Get();
  StyleManager copyOfStyleManager( styleManager );

  DALI_TEST_CHECK( copyOfStyleManager );
  END_TEST;
}

int UtcDaliStyleManagerAssignmentOperatorP(void)
{
  TestApplication application;

  StyleManager styleManager = StyleManager::Get();
  StyleManager copyOfStyleManager = styleManager;

  DALI_TEST_CHECK( copyOfStyleManager );
  DALI_TEST_CHECK( copyOfStyleManager == styleManager );
  END_TEST;
}

int UtcDaliStyleManagerGet(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliStyleManagerGet");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "StyleManager" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

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
    signalCount =0;
  }

public:
  int signalCount;
};

} // anonymous namespace

int UtcDaliStyleManagerApplyTheme(void)
{
  ToolkitTestApplication application;

  tet_infoline( "Testing StyleManager ApplyTheme" );

  const char* json1 =
    "{\n"
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
  Test::TestButton testButton = Test::TestButton::New();
  Test::TestButton testButton2 = Test::TestButton::New();
  Stage::GetCurrent().Add( testButton );
  Stage::GetCurrent().Add( testButton2 );
  StyleChangedSignalChecker styleChangedSignalHandler;
  StyleChangedSignalChecker styleChangedSignalHandler2;
  StyleManager styleManager = StyleManager::Get();

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  // Render and notify
  application.SendNotification();
  application.Render();

  tet_infoline("Apply the style");

  std::string themeFile("ThemeOne");
  Test::StyleMonitor::SetThemeFileOutput(themeFile, json1);
  StyleManager::Get().ApplyTheme(themeFile);

  Property::Value bgColor( testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR) );
  Property::Value fgColor( testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR) );

  DALI_TEST_EQUALS( bgColor, Property::Value(Color::YELLOW), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( fgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION );

  tet_infoline("Testing that the signal handler is called only once");
  DALI_TEST_EQUALS( styleChangedSignalHandler.signalCount, 1, TEST_LOCATION );

  tet_infoline("Override the background property");
  testButton.SetProperty( Test::TestButton::Property::BACKGROUND_COLOR, Color::GREEN );
  bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);
  DALI_TEST_EQUALS( bgColor, Property::Value(Color::GREEN), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( fgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  tet_infoline("Apply the style again");

  styleChangedSignalHandler.signalCount = 0;
  StyleManager::Get().ApplyTheme(themeFile);

  bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);

  tet_infoline("Check that the property is changed");
  DALI_TEST_EQUALS( bgColor, Property::Value(Color::YELLOW), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( fgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION );
  tet_infoline("Testing that the signal handler is called only once");
  DALI_TEST_EQUALS( styleChangedSignalHandler.signalCount, 1, TEST_LOCATION );

  tet_infoline( "Load a different stylesheet");

  tet_infoline("Apply the new style");
  std::string themeFile2("ThemeTwo");
  Test::StyleMonitor::SetThemeFileOutput(themeFile2, json2);

  styleChangedSignalHandler.signalCount = 0;
  StyleManager::Get().ApplyTheme(themeFile2);

  bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);

  tet_infoline("Check that the properties change, but the signal gets sent only once");
  DALI_TEST_EQUALS( bgColor, Property::Value(Color::RED), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( fgColor, Property::Value(Color::CYAN), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( styleChangedSignalHandler.signalCount, 1, TEST_LOCATION );

  END_TEST;
}


int UtcDaliStyleManagerApplyDefaultTheme(void)
{
  tet_infoline( "Testing StyleManager ApplyTheme" );

  const char* defaultTheme =
    "{\n"
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

  Test::StyleMonitor::SetThemeFileOutput( DALI_STYLE_DIR "dali-toolkit-default-theme.json",
                                          defaultTheme);
  ToolkitTestApplication application;

  Test::TestButton testButton = Test::TestButton::New();
  Stage::GetCurrent().Add( testButton );
  StyleChangedSignalChecker styleChangedSignalHandler;
  StyleManager styleManager = StyleManager::Get();

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Get the default:
  Property::Value defaultBgColor( testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR) );
  Property::Value defaultFgColor( testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR) );

  tet_infoline("Apply the style");

  std::string themeFile("ThemeOne");
  Test::StyleMonitor::SetThemeFileOutput(themeFile, appTheme);
  StyleManager::Get().ApplyTheme(themeFile);

  Property::Value bgColor( testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR) );
  Property::Value fgColor( testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR) );

  DALI_TEST_EQUALS( bgColor, Property::Value(Color::MAGENTA), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( fgColor, Property::Value(Color::GREEN), 0.001, TEST_LOCATION );

  tet_infoline("Testing that the signal handler is called only once");
  DALI_TEST_EQUALS( styleChangedSignalHandler.signalCount, 1, TEST_LOCATION );
  tet_infoline("Revert the style");

  styleChangedSignalHandler.signalCount = 0;
  StyleManager::Get().ApplyDefaultTheme();

  bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);

  tet_infoline("Check that the property is reverted");
  DALI_TEST_EQUALS( bgColor, defaultBgColor, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( fgColor, defaultFgColor, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( bgColor, Property::Value(Color::YELLOW), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( fgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION );
  tet_infoline("Testing that the signal handler is called only once");
  DALI_TEST_EQUALS( styleChangedSignalHandler.signalCount, 1, TEST_LOCATION );

  END_TEST;
}


int UtcDaliStyleManagerSetStyleConstantP(void)
{
  ToolkitTestApplication application;

  tet_infoline( " UtcDaliStyleManagerSetStyleConstantP" );

  StyleManager manager = StyleManager::Get();

  std::string key( "key" );
  Property::Value value( 100 );

  manager.SetStyleConstant( key, value );

  Property::Value returnedValue;
  manager.GetStyleConstant( key, returnedValue );

  DALI_TEST_CHECK( value.Get<int>() == returnedValue.Get<int>() );
  END_TEST;
}


int UtcDaliStyleManagerGetStyleConstantP(void)
{
  ToolkitTestApplication application;

  tet_infoline( " UtcDaliStyleManagerGetStyleConstantP" );

  StyleManager manager = StyleManager::Get();

  std::string key( "key" );
  Property::Value value( 100 );

  manager.SetStyleConstant( key, value );

  Property::Value returnedValue;
  manager.GetStyleConstant( key, returnedValue );

  DALI_TEST_CHECK( value.Get<int>() == returnedValue.Get<int>() );
  END_TEST;
}

int UtcDaliStyleManagerGetStyleConstantN(void)
{
  ToolkitTestApplication application;

  tet_infoline( " UtcDaliStyleManagerGetStyleConstantN" );

  StyleManager manager = StyleManager::Get();

  std::string key2( "key2" );
  Property::Value returnedValue2;
  DALI_TEST_CHECK( !manager.GetStyleConstant( key2, returnedValue2 ) );

  END_TEST;
}

int UtcDaliStyleManagerApplyStyle(void)
{
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliStyleManagerApplyStyle - test that a style can be applied to a single button" );

  const char* json1 =
    "{\n"
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
  Test::TestButton testButton = Test::TestButton::New();
  Test::TestButton testButton2 = Test::TestButton::New();
  Stage::GetCurrent().Add( testButton );
  Stage::GetCurrent().Add( testButton2 );
  StyleChangedSignalChecker styleChangedSignalHandler;
  StyleManager styleManager = StyleManager::Get();

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  tet_infoline("Apply the style");

  std::string themeFile("ThemeOne");
  Test::StyleMonitor::SetThemeFileOutput(themeFile, json1);
  styleManager.ApplyTheme(themeFile);

  // Render and notify
  application.SendNotification();
  application.Render();

  Property::Value themedBgColor( testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR) );
  Property::Value themedFgColor( testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR) );

  // Apply the style to the test button:
  std::string themeFile2("ThemeTwo");
  Test::StyleMonitor::SetThemeFileOutput(themeFile2, json2);
  styleManager.ApplyStyle( testButton, themeFile2, "testbutton" );

  tet_infoline("Check that the properties change for the first button");
  Property::Value bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  Property::Value fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);
  DALI_TEST_EQUALS( bgColor, Property::Value(Color::RED), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( fgColor, Property::Value(Color::CYAN), 0.001, TEST_LOCATION );

  DALI_TEST_NOT_EQUALS( bgColor, themedBgColor, 0.001, TEST_LOCATION );
  DALI_TEST_NOT_EQUALS( fgColor, themedFgColor, 0.001, TEST_LOCATION );

  tet_infoline("Check that the properties remain the same for the second button");
  bgColor = testButton2.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton2.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);
  DALI_TEST_EQUALS( bgColor, themedBgColor, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( fgColor, themedFgColor, 0.001, TEST_LOCATION );

  END_TEST;
}


int UtcDaliStyleManagerStyleChangedSignalFontFamily(void)
{
  tet_infoline("Test that the StyleChange signal is fired when the font family is altered" );
  Test::StyleMonitor::SetThemeFileOutput( DALI_STYLE_DIR "dali-toolkit-default-theme.json",
                                          defaultTheme );

  ToolkitTestApplication application;

  std::string labelStr("Label");
  Toolkit::TextLabel label = Toolkit::TextLabel::New(labelStr);
  Stage::GetCurrent().Add( label );

  Toolkit::TextLabel label2 = Toolkit::TextLabel::New(labelStr);
  Stage::GetCurrent().Add( label2 );

  StyleChangedSignalChecker styleChangedSignalHandler;
  Dali::StyleMonitor styleMonitor = Dali::StyleMonitor::Get();
  StyleManager styleManager = StyleManager::Get();

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  Test::StyleMonitor::SetDefaultFontFamily("Times New Roman");

  styleMonitor.StyleChangeSignal().Emit( styleMonitor,  StyleChange::DEFAULT_FONT_CHANGE);

  tet_infoline("Test that the StyleChanged signal is received only once");

  DALI_TEST_EQUALS( styleChangedSignalHandler.signalCount, 1, TEST_LOCATION );

  // Check that the label's font style has been altered
  Property::Value family = label.GetProperty(TextLabel::Property::FONT_FAMILY);
  std::string familyStr;
  family.Get( familyStr );

  DALI_TEST_EQUALS( familyStr, "Times New Roman", TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyleManagerStyleChangedSignalFontSize(void)
{
  tet_infoline("Test that the StyleChange signal is fired when the font size is altered" );

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

  Test::StyleMonitor::SetThemeFileOutput( DALI_STYLE_DIR "dali-toolkit-default-theme.json", defaultTheme );

  ToolkitTestApplication application;

  std::string labelStr("Label");
  Toolkit::TextLabel label = Toolkit::TextLabel::New(labelStr);
  Stage::GetCurrent().Add( label );

  Toolkit::TextLabel label2 = Toolkit::TextLabel::New(labelStr);
  Stage::GetCurrent().Add( label2 );

  StyleChangedSignalChecker styleChangedSignalHandler;
  StyleMonitor styleMonitor = StyleMonitor::Get();
  StyleManager styleManager = StyleManager::Get();

  label.SetProperty(TextLabel::Property::POINT_SIZE, 10.0f);

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  Test::StyleMonitor::SetDefaultFontSize(2);
  styleMonitor.StyleChangeSignal().Emit( styleMonitor,  StyleChange::DEFAULT_FONT_SIZE_CHANGE);

  tet_infoline("Test that the StyleChanged signal is received only once");
  DALI_TEST_EQUALS( styleChangedSignalHandler.signalCount, 1, TEST_LOCATION );

  tet_infoline("Test that the label's font size has been altered\n");
  Property::Value pointSizeValue = label.GetProperty(TextLabel::Property::POINT_SIZE);
  float pointSize;
  pointSizeValue.Get( pointSize );

  DALI_TEST_EQUALS( pointSize, 12.0f, 0.001, TEST_LOCATION );

  styleChangedSignalHandler.signalCount = 0;

  Test::StyleMonitor::SetDefaultFontSize(4);
  styleMonitor.StyleChangeSignal().Emit( styleMonitor, StyleChange::DEFAULT_FONT_SIZE_CHANGE);

  tet_infoline("Test that the StyleChanged signal is received only once");
  DALI_TEST_EQUALS( styleChangedSignalHandler.signalCount, 1, TEST_LOCATION );

  // Check that the label's font style has been altered
  pointSizeValue = label.GetProperty(TextLabel::Property::POINT_SIZE);
  pointSizeValue.Get( pointSize );

  DALI_TEST_EQUALS( pointSize, 16.0f, 0.001, TEST_LOCATION );


  END_TEST;
}


int UtcDaliStyleManagerStyleChangedSignalFontSizeTextField(void)
{
  tet_infoline("Test that the StyleChange signal is fired when the font size is altered" );

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

  Test::StyleMonitor::SetThemeFileOutput( DALI_STYLE_DIR "dali-toolkit-default-theme.json", defaultTheme );

  ToolkitTestApplication application;

  std::string fieldStr("Field");
  Toolkit::TextField field = Toolkit::TextField::New();
  field.SetProperty( Toolkit::TextField::Property::TEXT, fieldStr );
  Stage::GetCurrent().Add( field );

  Toolkit::TextField field2 = Toolkit::TextField::New();
  Stage::GetCurrent().Add( field2 );
  field2.SetProperty( Toolkit::TextField::Property::TEXT, fieldStr );

  StyleChangedSignalChecker styleChangedSignalHandler;
  StyleMonitor styleMonitor = StyleMonitor::Get();
  StyleManager styleManager = StyleManager::Get();

  field.SetProperty(TextField::Property::POINT_SIZE, 10.0f);

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  Test::StyleMonitor::SetDefaultFontSize(2);
  styleMonitor.StyleChangeSignal().Emit( styleMonitor,  StyleChange::DEFAULT_FONT_SIZE_CHANGE);

  tet_infoline("Test that the StyleChanged signal is received only once");
  DALI_TEST_EQUALS( styleChangedSignalHandler.signalCount, 1, TEST_LOCATION );

  tet_infoline("Test that the field's font size has been altered\n");
  Property::Value pointSizeValue = field.GetProperty(TextField::Property::POINT_SIZE);
  float pointSize;
  pointSizeValue.Get( pointSize );

  DALI_TEST_EQUALS( pointSize, 12.0f, 0.001, TEST_LOCATION );

  styleChangedSignalHandler.signalCount = 0;

  Test::StyleMonitor::SetDefaultFontSize(4);
  styleMonitor.StyleChangeSignal().Emit( styleMonitor, StyleChange::DEFAULT_FONT_SIZE_CHANGE);

  tet_infoline("Test that the StyleChanged signal is received only once");
  DALI_TEST_EQUALS( styleChangedSignalHandler.signalCount, 1, TEST_LOCATION );

  // Check that the field's font style has been altered
  pointSizeValue = field.GetProperty(TextField::Property::POINT_SIZE);
  pointSizeValue.Get( pointSize );

  DALI_TEST_EQUALS( pointSize, 16.0f, 0.001, TEST_LOCATION );


  END_TEST;
}

int UtcDaliStyleManagerStyleChangedSignalFontSizeTextEditor(void)
{
  tet_infoline("Test that the StyleChange signal is fired when the font size is altered" );

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

  Test::StyleMonitor::SetThemeFileOutput( DALI_STYLE_DIR "dali-toolkit-default-theme.json", defaultTheme );

  ToolkitTestApplication application;

  std::string editorStr("Editor");
  Toolkit::TextEditor editor = Toolkit::TextEditor::New();
  editor.SetProperty( Toolkit::TextEditor::Property::TEXT, editorStr );
  Stage::GetCurrent().Add( editor );

  Toolkit::TextEditor editor2 = Toolkit::TextEditor::New();
  Stage::GetCurrent().Add( editor2 );
  editor2.SetProperty( Toolkit::TextEditor::Property::TEXT, editorStr );

  StyleChangedSignalChecker styleChangedSignalHandler;
  StyleMonitor styleMonitor = StyleMonitor::Get();
  StyleManager styleManager = StyleManager::Get();

  editor.SetProperty(TextEditor::Property::POINT_SIZE, 10.0f);

  styleManager.StyleChangedSignal().Connect(&styleChangedSignalHandler, &StyleChangedSignalChecker::OnStyleChanged);

  Test::StyleMonitor::SetDefaultFontSize(2);
  styleMonitor.StyleChangeSignal().Emit( styleMonitor,  StyleChange::DEFAULT_FONT_SIZE_CHANGE);

  tet_infoline("Test that the StyleChanged signal is received only once");
  DALI_TEST_EQUALS( styleChangedSignalHandler.signalCount, 1, TEST_LOCATION );

  tet_infoline("Test that the editor's font size has been altered\n");
  Property::Value pointSizeValue = editor.GetProperty(TextEditor::Property::POINT_SIZE);
  float pointSize;
  pointSizeValue.Get( pointSize );

  DALI_TEST_EQUALS( pointSize, 14.0f, 0.001, TEST_LOCATION );

  styleChangedSignalHandler.signalCount = 0;

  Test::StyleMonitor::SetDefaultFontSize(4);
  styleMonitor.StyleChangeSignal().Emit( styleMonitor, StyleChange::DEFAULT_FONT_SIZE_CHANGE);

  tet_infoline("Test that the StyleChanged signal is received only once");
  DALI_TEST_EQUALS( styleChangedSignalHandler.signalCount, 1, TEST_LOCATION );

  // Check that the editor's font style has been altered
  pointSizeValue = editor.GetProperty(TextEditor::Property::POINT_SIZE);
  pointSizeValue.Get( pointSize );

  DALI_TEST_EQUALS( pointSize, 25.0f, 0.001, TEST_LOCATION );


  END_TEST;
}
