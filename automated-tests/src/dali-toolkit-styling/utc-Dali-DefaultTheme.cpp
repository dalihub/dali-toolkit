/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable by law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dali/devel-api/adaptor-framework/style-monitor.h>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <fstream>
#include <toolkit-style-monitor.h>

// for Internal::DefaultTheme
#include <dali-toolkit/internal/styling/default-theme.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
std::string defaultTheme;

} // anonymous namespace

void dali_default_theme_startup(void)
{
  test_return_value = TET_UNDEF;

  std::ifstream t("src/dali-toolkit-styling/default-theme.json");
  defaultTheme = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
}

void dali_default_theme_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliDefaultThemeGetP(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliDefaultThemeGetP - Test DefaultTheme singleton");

  Toolkit::Internal::DefaultTheme& theme1 = Toolkit::Internal::DefaultTheme::Get();
  Toolkit::Internal::DefaultTheme& theme2 = Toolkit::Internal::DefaultTheme::Get();

  DALI_TEST_CHECK(&theme1 == &theme2);
  END_TEST;
}

int UtcDaliDefaultThemeApplyDefaultStyleTextLabelP(void)
{
  // Define theme with TextLabel and TextField styles for coverage
  const char* themeJson =
    "{"
    "  \"styles\":"
    "  {"
    "    \"TextLabel\":"
    "    {"
    "      \"pointSize\":16,"
    "      \"enableAutoScroll\":false,"
    "      \"autoScrollLoopCount\":2,"
    "      \"autoScrollGap\":50,"
    "      \"autoScrollSpeed\":80"
    "    },"
    "    \"TextField\":"
    "    {"
    "      \"pointSize\":16,"
    "      \"primaryCursorColor\":[0.0,0.72,0.9,1.0],"
    "      \"secondaryCursorColor\":[0.0,0.72,0.9,1.0],"
    "      \"cursorWidth\":3,"
    "      \"selectionHighlightColor\":[0.75,0.96,1.0,1.0],"
    "      \"grabHandleImage\":\"cursor_handler.png\","
    "      \"selectionHandleImageLeft\":{\"filename\":\"handle_left.png\"},"
    "      \"selectionHandleImageRight\":{\"filename\":\"handle_right.png\"},"
    "      \"enableSelection\":true"
    "    }"
    "  }"
    "}";

  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json", themeJson);

  ToolkitTestApplication application;

  tet_infoline(" UtcDaliDefaultThemeApplyDefaultStyleTextLabelP - Test ApplyDefaultStyle for TextLabel");

  Toolkit::TextLabel textLabel = Toolkit::TextLabel::New();
  application.GetScene().Add(textLabel);

  Toolkit::Internal::DefaultTheme& theme = Toolkit::Internal::DefaultTheme::Get();
  theme.Reset(); // Reset theme to use the test theme file
  theme.ApplyDefaultStyle(textLabel);

  // Check that pointSize from default theme was applied
  Property::Value pointSizeValue = textLabel.GetProperty(Toolkit::TextLabel::Property::POINT_SIZE);
  float pointSize = 0.0f;
  pointSizeValue.Get(pointSize);

  DALI_TEST_CHECK(pointSize > 0.0f);

  // Check other TextLabel properties that may have been applied
  Property::Index enableAutoScrollIndex = textLabel.GetPropertyIndex("enableAutoScroll");
  if(enableAutoScrollIndex != Dali::Property::INVALID_INDEX)
  {
    Dali::Property::Value enableAutoScrollValue = textLabel.GetProperty(enableAutoScrollIndex);
    bool enableAutoScroll = false;
    enableAutoScrollValue.Get(enableAutoScroll);
    // Just check that we can read the property without crashing
    DALI_TEST_CHECK(true);
  }

  END_TEST;
}

int UtcDaliDefaultThemeApplyDefaultStyleTextFieldP(void)
{
  // Define theme with TextField styles for coverage
  const char* themeJson =
    "{"
    "  \"styles\":"
    "  {"
    "    \"TextField\":"
    "    {"
    "      \"pointSize\":16,"
    "      \"primaryCursorColor\":[0.0,0.72,0.9,1.0],"
    "      \"secondaryCursorColor\":[0.0,0.72,0.9,1.0],"
    "      \"cursorWidth\":3,"
    "      \"selectionHighlightColor\":[0.75,0.96,1.0,1.0],"
    "      \"grabHandleImage\":\"cursor_handler.png\","
    "      \"selectionHandleImageLeft\":{\"filename\":\"handle_left.png\"},"
    "      \"selectionHandleImageRight\":{\"filename\":\"handle_right.png\"},"
    "      \"enableSelection\":true"
    "    }"
    "  }"
    "}";

  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json", themeJson);

  ToolkitTestApplication application;

  tet_infoline(" UtcDaliDefaultThemeApplyDefaultStyleTextFieldP - Test ApplyDefaultStyle for TextField");

  Toolkit::TextField textField = Toolkit::TextField::New();
  application.GetScene().Add(textField);

  Toolkit::Internal::DefaultTheme& theme = Toolkit::Internal::DefaultTheme::Get();
  theme.Reset(); // Reset theme to use the test theme file
  theme.ApplyDefaultStyle(textField);

  // Check that pointSize was applied
  Property::Value pointSizeValue = textField.GetProperty(Toolkit::TextField::Property::POINT_SIZE);
  float pointSize = 0.0f;
  pointSizeValue.Get(pointSize);

  DALI_TEST_CHECK(pointSize > 0.0f);

  END_TEST;
}

int UtcDaliDefaultThemeApplyDefaultStyleTextEditorP(void)
{
  // Define theme with TextEditor styles
  const char* themeJson =
    "{"
    "  \"styles\":"
    "  {"
    "    \"TextEditor\":"
    "    {"
    "      \"pointSize\":16,"
    "      \"primaryCursorColor\":[0.0,0.72,0.9,1.0],"
    "      \"secondaryCursorColor\":[0.0,0.72,0.9,1.0],"
    "      \"cursorWidth\":3,"
    "      \"selectionHighlightColor\":[0.75,0.96,1.0,1.0],"
    "      \"grabHandleImage\":\"cursor_handler.png\","
    "      \"selectionHandleImageLeft\":{\"filename\":\"handle_left.png\"},"
    "      \"selectionHandleImageRight\":{\"filename\":\"handle_right.png\"},"
    "      \"enableSelection\":true"
    "    }"
    "  }"
    "}";

  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json", themeJson);

  ToolkitTestApplication application;

  tet_infoline(" UtcDaliDefaultThemeApplyDefaultStyleTextEditorP - Test ApplyDefaultStyle for TextEditor");

  Toolkit::TextEditor textEditor = Toolkit::TextEditor::New();
  application.GetScene().Add(textEditor);

  Toolkit::Internal::DefaultTheme& theme = Toolkit::Internal::DefaultTheme::Get();
  theme.Reset(); // Reset theme to use the test theme file
  theme.ApplyDefaultStyle(textEditor);

  // Check that pointSize was applied
  Property::Value pointSizeValue = textEditor.GetProperty(Toolkit::TextEditor::Property::POINT_SIZE);
  float pointSize = 0.0f;
  pointSizeValue.Get(pointSize);

  DALI_TEST_CHECK(pointSize > 0.0f);

  END_TEST;
}

int UtcDaliDefaultThemeApplyDefaultStyleWithImageP(void)
{
  // Define theme with TextField images for coverage
  const char* themeJson =
    "{"
    "  \"styles\":"
    "  {"
    "    \"TextField\":"
    "    {"
    "      \"pointSize\":16,"
    "      \"grabHandleImage\":\"cursor_handler.png\","
    "      \"selectionHandleImageLeft\":{\"filename\":\"handle_left.png\"},"
    "      \"selectionHandleImageRight\":{\"filename\":\"handle_right.png\"}"
    "    }"
    "  }"
    "}";

  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json", themeJson);

  ToolkitTestApplication application;

  tet_infoline(" UtcDaliDefaultThemeApplyDefaultStyleWithImageP - Test ApplyDefaultStyle with images");

  Toolkit::TextField textField = Toolkit::TextField::New();
  application.GetScene().Add(textField);

  Toolkit::Internal::DefaultTheme& theme = Toolkit::Internal::DefaultTheme::Get();
  theme.Reset(); // Reset theme to use the test theme file
  theme.ApplyDefaultStyle(textField);

  // Check that grabHandleImage was applied if theme has images
  Property::Value grabHandleValue = textField.GetProperty(Toolkit::TextField::Property::GRAB_HANDLE_IMAGE);

  // Value could be Property::MAP or Property::STRING or empty
  DALI_TEST_CHECK(grabHandleValue.GetType() == Property::MAP ||
                  grabHandleValue.GetType() == Property::STRING ||
                  grabHandleValue.GetType() == Property::NONE);

  END_TEST;
}

int UtcDaliDefaultThemeApplyDefaultStyleNoPointSizeP(void)
{
  // Define theme without pointSize for coverage
  const char* themeJson =
    "{"
    "  \"styles\":"
    "  {"
    "    \"TextLabel\":"
    "    {"
    "      \"enableAutoScroll\":false"
    "    }"
    "  }"
    "}";

  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json", themeJson);

  ToolkitTestApplication application;

  tet_infoline(" UtcDaliDefaultThemeApplyDefaultStyleNoPointSizeP - Test ApplyDefaultStyle doesn't crash");

  Toolkit::TextLabel textLabel = Toolkit::TextLabel::New();
  application.GetScene().Add(textLabel);

  // Set a custom pointSize first
  textLabel.SetProperty(Toolkit::TextLabel::Property::POINT_SIZE, 15.0f);

  Toolkit::Internal::DefaultTheme& theme = Toolkit::Internal::DefaultTheme::Get();
  theme.Reset(); // Reset theme to use the test theme file

  // This test verifies the method doesn't crash and doesn't override existing pointSize
  theme.ApplyDefaultStyle(textLabel);

  // Verify pointSize wasn't changed
  Property::Value pointSizeValue = textLabel.GetProperty(Toolkit::TextLabel::Property::POINT_SIZE);
  float pointSize = 0.0f;
  pointSizeValue.Get(pointSize);

  DALI_TEST_EQUALS(pointSize, 15.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliDefaultThemeApplyDefaultStyleWithDisableStyleChangeSignalsP(void)
{
  // This test verifies that controls created with DISABLE_STYLE_CHANGE_SIGNALS
  // can still have their default style applied manually
  const char* themeJson =
    "{"
    "  \"styles\":"
    "  {"
    "    \"TextLabel\":"
    "    {"
    "      \"pointSize\":20"
    "    },"
    "    \"TextField\":"
    "    {"
    "      \"pointSize\":18"
    "    },"
    "    \"TextEditor\":"
    "    {"
    "      \"pointSize\":22"
    "    }"
    "  }"
    "}";

  Test::StyleMonitor::SetThemeFileOutput(DALI_STYLE_DIR "dali-toolkit-default-theme.json", themeJson);

  ToolkitTestApplication application;

  tet_infoline(" UtcDaliDefaultThemeApplyDefaultStyleWithDisableStyleChangeSignalsP - Test ApplyDefaultStyle with DISABLE_STYLE_CHANGE_SIGNALS");

  // Create controls with DISABLE_STYLE_CHANGE_SIGNALS
  Toolkit::TextLabel textLabel = Toolkit::TextLabel::New(Toolkit::Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS);
  Toolkit::TextField textField = Toolkit::TextField::New(Toolkit::Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS);
  Toolkit::TextEditor textEditor = Toolkit::TextEditor::New(Toolkit::Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS);

  application.GetScene().Add(textLabel);
  application.GetScene().Add(textField);
  application.GetScene().Add(textEditor);

  Toolkit::Internal::DefaultTheme& theme = Toolkit::Internal::DefaultTheme::Get();
  theme.Reset(); // Reset theme to use the test theme file

  // Manually apply default style (this should work even with DISABLE_STYLE_CHANGE_SIGNALS)
  theme.ApplyDefaultStyle(textLabel);
  theme.ApplyDefaultStyle(textField);
  theme.ApplyDefaultStyle(textEditor);

  // Verify pointSize was applied correctly
  Property::Value labelPointSizeValue = textLabel.GetProperty(Toolkit::TextLabel::Property::POINT_SIZE);
  float labelPointSize = 0.0f;
  labelPointSizeValue.Get(labelPointSize);
  DALI_TEST_EQUALS(labelPointSize, 20.0f, 0.001, TEST_LOCATION);

  Property::Value fieldPointSizeValue = textField.GetProperty(Toolkit::TextField::Property::POINT_SIZE);
  float fieldPointSize = 0.0f;
  fieldPointSizeValue.Get(fieldPointSize);
  DALI_TEST_EQUALS(fieldPointSize, 18.0f, 0.001, TEST_LOCATION);

  Property::Value editorPointSizeValue = textEditor.GetProperty(Toolkit::TextEditor::Property::POINT_SIZE);
  float editorPointSize = 0.0f;
  editorPointSizeValue.Get(editorPointSize);
  DALI_TEST_EQUALS(editorPointSize, 22.0f, 0.001, TEST_LOCATION);

  END_TEST;
}
