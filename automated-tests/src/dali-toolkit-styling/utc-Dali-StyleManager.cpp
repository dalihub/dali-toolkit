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
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-toolkit/devel-api/builder/builder.h>
#include <test-button.h>
#include <test-animation-data.h>
#include <toolkit-style-monitor.h>
#include <dummy-control.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>

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


int UtcDaliStyleManagerIncludeStyleP(void)
{
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliStyleManagerIncludeStyle - test that style sheet inclusion works" );

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

  DALI_TEST_EQUALS( themedBgColor, Property::Value(Color::YELLOW), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( themedFgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION );

  END_TEST;
}


int UtcDaliStyleManagerIncludeStyleN(void)
{
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliStyleManagerIncludeStyle - test that style sheet inclusion works, but included stylesheet is bad json" );

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

  try
  {
    styleManager.ApplyTheme(themeFile);
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "!\"Cannot parse JSON\"", TEST_LOCATION );
  }

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


int UtcDaliStyleManagerSetState01(void)
{
  tet_infoline("Instantiate dummy control and test state/visual/transition capture" );
  Test::StyleMonitor::SetThemeFileOutput( DALI_STYLE_DIR "dali-toolkit-default-theme.json",
                                          defaultTheme );

  ToolkitTestApplication application;

  StyleChangedSignalChecker styleChangedSignalHandler;
  Dali::StyleMonitor styleMonitor = Dali::StyleMonitor::Get();
  StyleManager styleManager = StyleManager::Get();

  DummyControl actor = DummyControl::New(true);
  actor.SetStyleName("BasicControl");
  Stage::GetCurrent().Add(actor);

  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());

  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);
  Visual::Base visual1 = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);

  actor.SetProperty( DevelControl::Property::STATE, DevelControl::FOCUSED );

  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);
  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOCUS_VISUAL), true, TEST_LOCATION);

  Visual::Base visual2 = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);
  DALI_TEST_CHECK( visual1 != visual2 );

  actor.SetProperty( DevelControl::Property::STATE, DevelControl::DISABLED );

  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);

  Visual::Base visual3 = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);
  Visual::Base focusVisual = dummyImpl.GetVisual(DummyControl::Property::FOCUS_VISUAL);
  DALI_TEST_CHECK( !focusVisual );
  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOCUS_VISUAL), false, TEST_LOCATION);

  DALI_TEST_CHECK( visual1 != visual3 );
  DALI_TEST_CHECK( visual2 != visual3 );

  END_TEST;
}

int UtcDaliStyleManagerSetState02(void)
{
  tet_infoline("Instantiate dummy control and test state/visual/transition capture" );
  Test::StyleMonitor::SetThemeFileOutput( DALI_STYLE_DIR "dali-toolkit-default-theme.json",
                                          defaultTheme );

  ToolkitTestApplication application;

  StyleChangedSignalChecker styleChangedSignalHandler;
  Dali::StyleMonitor styleMonitor = Dali::StyleMonitor::Get();
  StyleManager styleManager = StyleManager::Get();

  DummyControl actor = DummyControl::New(true);
  actor.SetStyleName("BasicControl");
  Stage::GetCurrent().Add(actor);

  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());

  int state = actor.GetProperty<int>( DevelControl::Property::STATE );
  DALI_TEST_EQUALS( state, (int) DevelControl::NORMAL, TEST_LOCATION );

  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);
  Visual::Base visual1 = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);

  actor.SetProperty( DevelControl::Property::STATE,
                     Property::Map().Add( "state", "FOCUSED" ).Add("withTransitions", false));

  state = actor.GetProperty<int>( DevelControl::Property::STATE );
  DALI_TEST_EQUALS( state, (int) DevelControl::FOCUSED, TEST_LOCATION );

  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);
  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOCUS_VISUAL), true, TEST_LOCATION);

  Visual::Base visual2 = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);
  DALI_TEST_CHECK( visual1 != visual2 );

  actor.SetProperty( DevelControl::Property::STATE,
                     Property::Map().Add( "state", "DISABLED" ).Add("withTransitions", false));

  state = actor.GetProperty<int>( DevelControl::Property::STATE );
  DALI_TEST_EQUALS( state, (int) DevelControl::DISABLED, TEST_LOCATION );

  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);

  Visual::Base visual3 = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);
  Visual::Base focusVisual = dummyImpl.GetVisual(DummyControl::Property::FOCUS_VISUAL);
  DALI_TEST_CHECK( !focusVisual );
  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOCUS_VISUAL), false, TEST_LOCATION);

  DALI_TEST_CHECK( visual1 != visual3 );
  DALI_TEST_CHECK( visual2 != visual3 );

  actor.SetProperty( DevelControl::Property::STATE,
                     Property::Map().Add( "state", "NORMAL" ).Add("withTransitions", false));

  state = actor.GetProperty<int>( DevelControl::Property::STATE );
  DALI_TEST_EQUALS( state, (int) DevelControl::NORMAL, TEST_LOCATION );

  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);

  visual1 = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);
  DALI_TEST_CHECK( visual1 );

  focusVisual = dummyImpl.GetVisual(DummyControl::Property::FOCUS_VISUAL);
  DALI_TEST_CHECK( !focusVisual );
  DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOCUS_VISUAL), false, TEST_LOCATION);


  END_TEST;
}


int UtcDaliStyleManagerSetSubState(void)
{
  tet_infoline("Instantiate dummy control and test state/visual/transition capture" );
  Test::StyleMonitor::SetThemeFileOutput( DALI_STYLE_DIR "dali-toolkit-default-theme.json",
                                          defaultTheme );

  ToolkitTestApplication application;

  StyleChangedSignalChecker styleChangedSignalHandler;
  Dali::StyleMonitor styleMonitor = Dali::StyleMonitor::Get();
  StyleManager styleManager = StyleManager::Get();

  DummyControl actor = DummyControl::New(true);
  actor.SetProperty(DevelControl::Property::STATE, "NORMAL");
  actor.SetProperty(DevelControl::Property::SUB_STATE, "SELECTED");
  actor.SetStyleName("ComplexControl");
  Stage::GetCurrent().Add(actor);

  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());

  {
    DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);
    Visual::Base visual = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);
    DALI_TEST_CHECK( visual );
    Property::Map map;
    visual.CreatePropertyMap( map );
    Property::Value* value = map.Find( Visual::Property::TYPE );
    DALI_TEST_CHECK( value );

    int visualType;
    value->Get( visualType );
    DALI_TEST_EQUALS( visualType, (int)Toolkit::Visual::GRADIENT, TEST_LOCATION );
  }

  actor.SetProperty(DevelControl::Property::SUB_STATE, "UNSELECTED");

  {
    DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);
    Visual::Base visual = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);
    DALI_TEST_CHECK( visual );
    Property::Map map;
    visual.CreatePropertyMap( map );
    Property::Value* value = map.Find( Visual::Property::TYPE );
    DALI_TEST_CHECK( value );

    int visualType;
    value->Get( visualType );
    DALI_TEST_EQUALS( visualType, (int)Toolkit::Visual::COLOR, TEST_LOCATION );
  }

  actor.SetProperty(DevelControl::Property::SUB_STATE, "SELECTED");

  {
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
    DALI_TEST_EQUALS(dummyImpl.IsVisualEnabled(DummyControl::Property::FOREGROUND_VISUAL), true, TEST_LOCATION);
    Visual::Base visual = dummyImpl.GetVisual(DummyControl::Property::FOREGROUND_VISUAL);
    DALI_TEST_CHECK( visual );
    Property::Map map;
    visual.CreatePropertyMap( map );
    Property::Value* value = map.Find( Visual::Property::TYPE );
    DALI_TEST_CHECK( value );

    int visualType;
    value->Get( visualType );
    DALI_TEST_EQUALS( visualType, (int)Toolkit::Visual::GRADIENT, TEST_LOCATION );
  }
  END_TEST;
}
