/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-toolkit/devel-api/styling/style-manager.h>
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

int UtcDaliStyleManagerSetOrientationValue(void)
{
  ToolkitTestApplication application;

  tet_infoline( " UtcDaliStyleManagerSetOrientationValue" );

  StyleManager manager = StyleManager::Get();

  int orientation1 = 0;
  manager.SetOrientationValue( orientation1 );
  DALI_TEST_CHECK( manager.GetOrientationValue() == orientation1 );

  int orientation2 = 180;
  manager.SetOrientationValue( orientation2 );
  DALI_TEST_CHECK( manager.GetOrientationValue() == orientation2 );

  END_TEST;
}

int UtcDaliStyleManagerSetOrientation(void)
{
  ToolkitTestApplication application;

  tet_infoline( " UtcDaliStyleManagerSetOrientation" );

  StyleManager manager = StyleManager::Get();

  Orientation orientation;

  manager.SetOrientation( orientation );

  DALI_TEST_CHECK( manager.GetOrientation() == orientation );

  END_TEST;
}

int UtcDaliStyleManagerSetStyleConstant(void)
{
  ToolkitTestApplication application;

  tet_infoline( " UtcDaliStyleManagerSetStyleConstant" );

  StyleManager manager = StyleManager::Get();

  std::string key( "key" );
  Property::Value value( 100 );

  manager.SetStyleConstant( key, value );

  Property::Value returnedValue;
  manager.GetStyleConstant( key, returnedValue );

  DALI_TEST_CHECK( value.Get<int>() == returnedValue.Get<int>() );

  std::string key2( "key2" );
  Property::Value returnedValue2;
  DALI_TEST_CHECK( !manager.GetStyleConstant( key2, returnedValue2 ) );

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

int UtcDaliStyleManagerPropertyOverride(void)
{
  ToolkitTestApplication application;

  tet_infoline( "Testing StyleManager property overrides" );

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

  Test::StyleMonitor::SetThemeFileOutput(json1);
  std::string themeFile("ThemeOne");
  StyleManager::Get().RequestThemeChange(themeFile);

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
  StyleManager::Get().RequestThemeChange(themeFile);

  bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);

  tet_infoline("Check that the property is changed");
  DALI_TEST_EQUALS( bgColor, Property::Value(Color::YELLOW), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( fgColor, Property::Value(Color::BLUE), 0.001, TEST_LOCATION );
  tet_infoline("Testing that the signal handler is called only once");
  DALI_TEST_EQUALS( styleChangedSignalHandler.signalCount, 1, TEST_LOCATION );

  tet_infoline( "Load a different stylesheet");

  tet_infoline("Apply the new style");
  Test::StyleMonitor::SetThemeFileOutput(json2);

  styleChangedSignalHandler.signalCount = 0;
  std::string themeFile2("ThemeTwo");
  StyleManager::Get().RequestThemeChange(themeFile2);

  bgColor = testButton.GetProperty(Test::TestButton::Property::BACKGROUND_COLOR);
  fgColor = testButton.GetProperty(Test::TestButton::Property::FOREGROUND_COLOR);

  tet_infoline("Check that the properties change, but the signal gets sent only once");
  DALI_TEST_EQUALS( bgColor, Property::Value(Color::RED), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( fgColor, Property::Value(Color::CYAN), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( styleChangedSignalHandler.signalCount, 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliStyleManagerFontSizeChange(void)
{
  tet_infoline("Test that the StyleChange signal is fired when the font size is altered" );
  Test::StyleMonitor::SetThemeFileOutput(defaultTheme);

  ToolkitTestApplication application;

  std::string labelStr("Label");
  Toolkit::TextLabel label = Toolkit::TextLabel::New(labelStr);
  Stage::GetCurrent().Add( label );

  Toolkit::TextLabel label2 = Toolkit::TextLabel::New(labelStr);
  Stage::GetCurrent().Add( label2 );

  StyleChangedSignalChecker styleChangedSignalHandler;
  StyleMonitor styleMonitor = StyleMonitor::Get();
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
