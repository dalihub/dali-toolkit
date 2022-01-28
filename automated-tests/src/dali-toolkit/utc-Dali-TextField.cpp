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

#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/public-api/rendering/renderer.h>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-field-devel.h>
#include <dali-toolkit/devel-api/text/rendering-backend.h>
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali/devel-api/adaptor-framework/key-devel.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include "test-text-geometry-utils.h"
#include "toolkit-clipboard.h"

using namespace Dali;
using namespace Toolkit;

void dali_textfield_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_textfield_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* const PROPERTY_NAME_RENDERING_BACKEND                    = "renderingBackend";
const char* const PROPERTY_NAME_TEXT                                 = "text";
const char* const PROPERTY_NAME_PLACEHOLDER_TEXT                     = "placeholderText";
const char* const PROPERTY_NAME_PLACEHOLDER_TEXT_FOCUSED             = "placeholderTextFocused";
const char* const PROPERTY_NAME_FONT_FAMILY                          = "fontFamily";
const char* const PROPERTY_NAME_FONT_STYLE                           = "fontStyle";
const char* const PROPERTY_NAME_POINT_SIZE                           = "pointSize";
const char* const PROPERTY_NAME_MAX_LENGTH                           = "maxLength";
const char* const PROPERTY_NAME_EXCEED_POLICY                        = "exceedPolicy";
const char* const PROPERTY_NAME_HORIZONTAL_ALIGNMENT                 = "horizontalAlignment";
const char* const PROPERTY_NAME_VERTICAL_ALIGNMENT                   = "verticalAlignment";
const char* const PROPERTY_NAME_TEXT_COLOR                           = "textColor";
const char* const PROPERTY_NAME_PLACEHOLDER_TEXT_COLOR               = "placeholderTextColor";
const char* const PROPERTY_NAME_PRIMARY_CURSOR_COLOR                 = "primaryCursorColor";
const char* const PROPERTY_NAME_SECONDARY_CURSOR_COLOR               = "secondaryCursorColor";
const char* const PROPERTY_NAME_ENABLE_CURSOR_BLINK                  = "enableCursorBlink";
const char* const PROPERTY_NAME_CURSOR_BLINK_INTERVAL                = "cursorBlinkInterval";
const char* const PROPERTY_NAME_CURSOR_BLINK_DURATION                = "cursorBlinkDuration";
const char* const PROPERTY_NAME_CURSOR_WIDTH                         = "cursorWidth";
const char* const PROPERTY_NAME_GRAB_HANDLE_IMAGE                    = "grabHandleImage";
const char* const PROPERTY_NAME_GRAB_HANDLE_PRESSED_IMAGE            = "grabHandlePressedImage";
const char* const PROPERTY_NAME_SCROLL_THRESHOLD                     = "scrollThreshold";
const char* const PROPERTY_NAME_SCROLL_SPEED                         = "scrollSpeed";
const char* const PROPERTY_NAME_SELECTION_HANDLE_IMAGE_LEFT          = "selectionHandleImageLeft";
const char* const PROPERTY_NAME_SELECTION_HANDLE_IMAGE_RIGHT         = "selectionHandleImageRight";
const char* const PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_LEFT  = "selectionHandlePressedImageLeft";
const char* const PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_RIGHT = "selectionHandlePressedImageRight";
const char* const PROPERTY_NAME_SELECTION_HANDLE_MARKER_IMAGE_LEFT   = "selectionHandleMarkerImageLeft";
const char* const PROPERTY_NAME_SELECTION_HANDLE_MARKER_IMAGE_RIGHT  = "selectionHandleMarkerImageRight";
const char* const PROPERTY_NAME_SELECTION_HIGHLIGHT_COLOR            = "selectionHighlightColor";
const char* const PROPERTY_NAME_DECORATION_BOUNDING_BOX              = "decorationBoundingBox";
const char* const PROPERTY_NAME_INPUT_METHOD_SETTINGS                = "inputMethodSettings";
const char* const PROPERTY_NAME_INPUT_COLOR                          = "inputColor";
const char* const PROPERTY_NAME_ENABLE_MARKUP                        = "enableMarkup";
const char* const PROPERTY_NAME_INPUT_FONT_FAMILY                    = "inputFontFamily";
const char* const PROPERTY_NAME_INPUT_FONT_STYLE                     = "inputFontStyle";
const char* const PROPERTY_NAME_INPUT_POINT_SIZE                     = "inputPointSize";

const char* const PROPERTY_NAME_UNDERLINE           = "underline";
const char* const PROPERTY_NAME_INPUT_UNDERLINE     = "inputUnderline";
const char* const PROPERTY_NAME_SHADOW              = "shadow";
const char* const PROPERTY_NAME_INPUT_SHADOW        = "inputShadow";
const char* const PROPERTY_NAME_EMBOSS              = "emboss";
const char* const PROPERTY_NAME_INPUT_EMBOSS        = "inputEmboss";
const char* const PROPERTY_NAME_OUTLINE             = "outline";
const char* const PROPERTY_NAME_INPUT_OUTLINE       = "inputOutline";
const char* const PROPERTY_NAME_STRIKETHROUGH       = "strikethrough";
const char* const PROPERTY_NAME_INPUT_STRIKETHROUGH = "inputStrikethrough";

const char* const PROPERTY_NAME_HIDDEN_INPUT_SETTINGS           = "hiddenInputSettings";
const char* const PROPERTY_NAME_PIXEL_SIZE                      = "pixelSize";
const char* const PROPERTY_NAME_ENABLE_SELECTION                = "enableSelection";
const char* const PROPERTY_NAME_PLACEHOLDER                     = "placeholder";
const char* const PROPERTY_NAME_ELLIPSIS                        = "ellipsis";
const char* const PROPERTY_NAME_ENABLE_SHIFT_SELECTION          = "enableShiftSelection";
const char* const PROPERTY_NAME_ENABLE_GRAB_HANDLE              = "enableGrabHandle";
const char* const PROPERTY_NAME_MATCH_SYSTEM_LANGUAGE_DIRECTION = "matchSystemLanguageDirection";
const char* const PROPERTY_NAME_ENABLE_GRAB_HANDLE_POPUP        = "enableGrabHandlePopup";
const char* const PROPERTY_NAME_BACKGROUND                      = "textBackground";
const char* const PROPERTY_NAME_FONT_SIZE_SCALE                 = "fontSizeScale";
const char* const PROPERTY_NAME_ENABLE_FONT_SIZE_SCALE          = "enableFontSizeScale";
const char* const PROPERTY_NAME_GRAB_HANDLE_COLOR               = "grabHandleColor";
const char* const PROPERTY_NAME_INPUT_FILTER                    = "inputFilter";

const Vector4       PLACEHOLDER_TEXT_COLOR(0.8f, 0.8f, 0.8f, 0.8f);
const Dali::Vector4 LIGHT_BLUE(0.75f, 0.96f, 1.f, 1.f); // The text highlight color.

const float RENDER_FRAME_INTERVAL = 16.66f;

const unsigned int DEFAULT_FONT_SIZE = 1152u;
const std::string  DEFAULT_FONT_DIR("/resources/fonts");

const int KEY_RETURN_CODE    = 36;
const int KEY_A_CODE         = 38;
const int KEY_D_CODE         = 40;
const int KEY_SHIFT_MODIFIER = 257;

const std::string DEFAULT_DEVICE_NAME("hwKeyboard");

static bool                                       gSelectionChangedCallbackCalled;
static uint32_t                                   oldSelectionStart;
static uint32_t                                   oldSelectionEnd;
static bool                                       gSelectionClearedCallbackCalled;
static bool                                       gSelectionStartedCallbackCalled;
static bool                                       gAnchorClickedCallBackCalled;
static bool                                       gAnchorClickedCallBackNotCalled;
static bool                                       gTextChangedCallBackCalled;
static bool                                       gMaxCharactersCallBackCalled;
static bool                                       gInputFilteredAcceptedCallbackCalled;
static bool                                       gInputFilteredRejectedCallbackCalled;
static bool                                       gInputStyleChangedCallbackCalled;
static bool                                       gCursorPositionChangedCallbackCalled;
static uint32_t                                   oldCursorPos;
static Dali::Toolkit::TextField::InputStyle::Mask gInputStyleMask;

static void LoadBitmapResource(TestPlatformAbstraction& platform, int width, int height)
{
  Integration::Bitmap*         bitmap = Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD);
  Integration::ResourcePointer resource(bitmap);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, width, height, width, height);
}

static void LoadMarkerImages(ToolkitTestApplication& app, TextField textField)
{
  int width(40);
  int height(40);
  LoadBitmapResource(app.GetPlatform(), width, height);

  Property::Map propertyMap;
  propertyMap["filename"] = "image.png";
  propertyMap["width"]    = width;
  propertyMap["height"]   = height;
  textField.SetProperty(Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_LEFT, propertyMap);
  textField.SetProperty(Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT, propertyMap);
  textField.SetProperty(Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT, propertyMap);
  textField.SetProperty(Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT, propertyMap);
  textField.SetProperty(Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT, propertyMap);
  textField.SetProperty(Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT, propertyMap);
  textField.SetProperty(Toolkit::TextField::Property::GRAB_HANDLE_IMAGE, propertyMap);
  textField.SetProperty(Toolkit::TextField::Property::GRAB_HANDLE_PRESSED_IMAGE, propertyMap);
}

/*
 * Simulate time passed by.
 *
 * @note this will always process at least 1 frame (1/60 sec)
 *
 * @param application Test application instance
 * @param duration Time to pass in milliseconds.
 * @return The actual time passed in milliseconds
 */
static int Wait(ToolkitTestApplication& application, int duration = 0)
{
  int time = 0;

  for(int i = 0; i <= (duration / RENDER_FRAME_INTERVAL); i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
    time += RENDER_FRAME_INTERVAL;
  }

  return time;
}

Dali::Integration::Point GetPointDownInside(Vector2& pos)
{
  Dali::Integration::Point point;
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(pos);
  return point;
}

Dali::Integration::Point GetPointUpInside(Vector2& pos)
{
  Dali::Integration::Point point;
  point.SetState(PointState::UP);
  point.SetScreenPosition(pos);
  return point;
}

struct CallbackFunctor
{
  CallbackFunctor(bool* callbackFlag)
  : mCallbackFlag(callbackFlag)
  {
  }

  void operator()()
  {
    *mCallbackFlag = true;
  }
  bool* mCallbackFlag;
};

static void TestSelectionClearedCallback(TextField control)
{
  tet_infoline(" TestSelectionClearedCallback");

  gSelectionClearedCallbackCalled = true;
}

static void TestSelectionStartedCallback(TextField control)
{
  tet_infoline(" TestSelectionStartedCallback");

  gSelectionStartedCallbackCalled = true;
}

static void TestSelectionChangedCallback(TextField control, uint32_t oldStart, uint32_t oldEnd)
{
  tet_infoline(" TestSelectionChangedCallback");

  gSelectionChangedCallbackCalled = true;
  oldSelectionStart               = oldStart;
  oldSelectionEnd                 = oldEnd;
}

static void TestAnchorClickedCallback(TextField control, const char* href, unsigned int hrefLength)
{
  tet_infoline(" TestAnchorClickedCallback");

  gAnchorClickedCallBackNotCalled = false;

  if(!strcmp(href, "https://www.tizen.org") && hrefLength == strlen(href))
  {
    gAnchorClickedCallBackCalled = true;
  }
}

static void TestCursorPositionChangedCallback(TextField control, unsigned int oldPos)
{
  tet_infoline(" TestCursorPositionChangedCallback");

  gCursorPositionChangedCallbackCalled = true;
  oldCursorPos                         = oldPos;
}

static void TestTextChangedCallback(TextField control)
{
  tet_infoline(" TestTextChangedCallback");

  gTextChangedCallBackCalled = true;
}

static void TestMaxLengthReachedCallback(TextField control)
{
  tet_infoline(" TestMaxLengthReachedCallback");

  gMaxCharactersCallBackCalled = true;
}

static void TestInputFilteredCallback(TextField control, Toolkit::InputFilter::Property::Type type)
{
  tet_infoline(" TestInputFilteredCallback");

  if(type == Toolkit::InputFilter::Property::ACCEPTED)
  {
    gInputFilteredAcceptedCallbackCalled = true;
  }
  else if(type == Toolkit::InputFilter::Property::REJECTED)
  {
    gInputFilteredRejectedCallbackCalled = true;
  }
}

static void TestInputStyleChangedCallback(TextField control, TextField::InputStyle::Mask mask)
{
  tet_infoline(" TestInputStyleChangedCallback");

  gInputStyleChangedCallbackCalled = true;
  gInputStyleMask                  = mask;
}

// Generate a KeyEvent to send to Core.
Integration::KeyEvent GenerateKey(const std::string&                  keyName,
                                  const std::string&                  logicalKey,
                                  const std::string&                  keyString,
                                  int                                 keyCode,
                                  int                                 keyModifier,
                                  unsigned long                       timeStamp,
                                  const Integration::KeyEvent::State& keyState,
                                  const std::string&                  compose        = "",
                                  const std::string&                  deviceName     = DEFAULT_DEVICE_NAME,
                                  const Device::Class::Type&          deviceClass    = Device::Class::NONE,
                                  const Device::Subclass::Type&       deviceSubclass = Device::Subclass::NONE)
{
  return Integration::KeyEvent(keyName,
                               logicalKey,
                               keyString,
                               keyCode,
                               keyModifier,
                               timeStamp,
                               keyState,
                               compose,
                               deviceName,
                               deviceClass,
                               deviceSubclass);
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

} // namespace

int UtcDaliToolkitTextFieldConstructorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldConstructorP");
  TextField textField;
  DALI_TEST_CHECK(!textField);
  END_TEST;
}

int UtcDaliToolkitTextFieldNewP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldNewP");
  TextField textField = TextField::New();
  DALI_TEST_CHECK(textField);
  END_TEST;
}

int UtcDaliToolkitTextFieldDownCastP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldDownCastP");
  TextField  textField1 = TextField::New();
  BaseHandle object(textField1);

  TextField textField2 = TextField::DownCast(object);
  DALI_TEST_CHECK(textField2);

  TextField textField3 = DownCast<TextField>(object);
  DALI_TEST_CHECK(textField3);
  END_TEST;
}

int UtcDaliToolkitTextFieldDownCastN(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldDownCastN");
  BaseHandle uninitializedObject;
  TextField  textField1 = TextField::DownCast(uninitializedObject);
  DALI_TEST_CHECK(!textField1);

  TextField textField2 = DownCast<TextField>(uninitializedObject);
  DALI_TEST_CHECK(!textField2);
  END_TEST;
}

int UtcDaliToolkitTextFieldCopyConstructorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldCopyConstructorP");
  TextField textField = TextField::New();
  textField.SetProperty(TextField::Property::TEXT, "Test");

  TextField copy(textField);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy.GetProperty<std::string>(TextLabel::Property::TEXT) == textField.GetProperty<std::string>(TextLabel::Property::TEXT));
  END_TEST;
}

int UtcDaliTextFieldMoveConstructor(void)
{
  ToolkitTestApplication application;

  TextField textField = TextField::New();
  textField.SetProperty(TextEditor::Property::TEXT, "Test");
  DALI_TEST_CHECK(textField.GetProperty<std::string>(TextField::Property::TEXT) == "Test");

  TextField moved = std::move(textField);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(moved.GetProperty<std::string>(TextField::Property::TEXT) == "Test");
  DALI_TEST_CHECK(!textField);

  END_TEST;
}

int UtcDaliToolkitTextFieldAssignmentOperatorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldAssignmentOperatorP");
  TextField textField = TextField::New();
  textField.SetProperty(TextField::Property::TEXT, "Test");

  TextField copy = textField;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy.GetProperty<std::string>(TextField::Property::TEXT) == textField.GetProperty<std::string>(TextField::Property::TEXT));
  END_TEST;
}

int UtcDaliTextFieldMoveAssignment(void)
{
  ToolkitTestApplication application;

  TextField textField = TextField::New();
  textField.SetProperty(TextEditor::Property::TEXT, "Test");
  DALI_TEST_CHECK(textField.GetProperty<std::string>(TextField::Property::TEXT) == "Test");

  TextField moved;
  moved = std::move(textField);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(moved.GetProperty<std::string>(TextField::Property::TEXT) == "Test");
  DALI_TEST_CHECK(!textField);

  END_TEST;
}

int UtcDaliTextFieldNewP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldNewP");
  TextField textField = TextField::New();
  DALI_TEST_CHECK(textField);
  END_TEST;
}

// Positive test case for a method
int UtcDaliTextFieldGetPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldGetPropertyP");
  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  // Check Property Indices are correct
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_RENDERING_BACKEND) == DevelTextField::Property::RENDERING_BACKEND);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_TEXT) == TextField::Property::TEXT);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_PLACEHOLDER_TEXT) == TextField::Property::PLACEHOLDER_TEXT);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_PLACEHOLDER_TEXT_FOCUSED) == TextField::Property::PLACEHOLDER_TEXT_FOCUSED);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_FONT_FAMILY) == TextField::Property::FONT_FAMILY);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_FONT_STYLE) == TextField::Property::FONT_STYLE);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_POINT_SIZE) == TextField::Property::POINT_SIZE);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_MAX_LENGTH) == TextField::Property::MAX_LENGTH);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_EXCEED_POLICY) == TextField::Property::EXCEED_POLICY);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_HORIZONTAL_ALIGNMENT) == TextField::Property::HORIZONTAL_ALIGNMENT);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_VERTICAL_ALIGNMENT) == TextField::Property::VERTICAL_ALIGNMENT);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_TEXT_COLOR) == TextField::Property::TEXT_COLOR);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_PLACEHOLDER_TEXT_COLOR) == TextField::Property::PLACEHOLDER_TEXT_COLOR);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_PRIMARY_CURSOR_COLOR) == TextField::Property::PRIMARY_CURSOR_COLOR);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_SECONDARY_CURSOR_COLOR) == TextField::Property::SECONDARY_CURSOR_COLOR);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_ENABLE_CURSOR_BLINK) == TextField::Property::ENABLE_CURSOR_BLINK);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_CURSOR_BLINK_INTERVAL) == TextField::Property::CURSOR_BLINK_INTERVAL);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_CURSOR_BLINK_DURATION) == TextField::Property::CURSOR_BLINK_DURATION);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_CURSOR_WIDTH) == TextField::Property::CURSOR_WIDTH);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_GRAB_HANDLE_IMAGE) == TextField::Property::GRAB_HANDLE_IMAGE);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_GRAB_HANDLE_PRESSED_IMAGE) == TextField::Property::GRAB_HANDLE_PRESSED_IMAGE);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_SCROLL_THRESHOLD) == TextField::Property::SCROLL_THRESHOLD);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_SCROLL_SPEED) == TextField::Property::SCROLL_SPEED);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_SELECTION_HANDLE_IMAGE_LEFT) == TextField::Property::SELECTION_HANDLE_IMAGE_LEFT);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_SELECTION_HANDLE_IMAGE_RIGHT) == TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_LEFT) == TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_RIGHT) == TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_SELECTION_HANDLE_MARKER_IMAGE_LEFT) == TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_SELECTION_HANDLE_MARKER_IMAGE_RIGHT) == TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_SELECTION_HIGHLIGHT_COLOR) == TextField::Property::SELECTION_HIGHLIGHT_COLOR);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_DECORATION_BOUNDING_BOX) == TextField::Property::DECORATION_BOUNDING_BOX);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_INPUT_METHOD_SETTINGS) == TextField::Property::INPUT_METHOD_SETTINGS);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_INPUT_COLOR) == TextField::Property::INPUT_COLOR);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_ENABLE_MARKUP) == TextField::Property::ENABLE_MARKUP);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_INPUT_FONT_FAMILY) == TextField::Property::INPUT_FONT_FAMILY);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_INPUT_FONT_STYLE) == TextField::Property::INPUT_FONT_STYLE);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_INPUT_POINT_SIZE) == TextField::Property::INPUT_POINT_SIZE);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_UNDERLINE) == TextField::Property::UNDERLINE);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_INPUT_UNDERLINE) == TextField::Property::INPUT_UNDERLINE);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_SHADOW) == TextField::Property::SHADOW);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_INPUT_SHADOW) == TextField::Property::INPUT_SHADOW);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_EMBOSS) == TextField::Property::EMBOSS);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_INPUT_EMBOSS) == TextField::Property::INPUT_EMBOSS);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_OUTLINE) == TextField::Property::OUTLINE);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_INPUT_OUTLINE) == TextField::Property::INPUT_OUTLINE);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_HIDDEN_INPUT_SETTINGS) == TextField::Property::HIDDEN_INPUT_SETTINGS);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_PIXEL_SIZE) == TextField::Property::PIXEL_SIZE);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_ENABLE_SELECTION) == TextField::Property::ENABLE_SELECTION);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_PLACEHOLDER) == TextField::Property::PLACEHOLDER);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_ELLIPSIS) == TextField::Property::ELLIPSIS);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_FONT_SIZE_SCALE) == DevelTextField::Property::FONT_SIZE_SCALE);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_ENABLE_FONT_SIZE_SCALE) == DevelTextField::Property::ENABLE_FONT_SIZE_SCALE);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_ENABLE_SHIFT_SELECTION) == DevelTextField::Property::ENABLE_SHIFT_SELECTION);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_ENABLE_GRAB_HANDLE) == DevelTextField::Property::ENABLE_GRAB_HANDLE);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_MATCH_SYSTEM_LANGUAGE_DIRECTION) == DevelTextField::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_ENABLE_GRAB_HANDLE_POPUP) == DevelTextField::Property::ENABLE_GRAB_HANDLE_POPUP);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_BACKGROUND) == DevelTextField::Property::BACKGROUND);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_GRAB_HANDLE_COLOR) == DevelTextField::Property::GRAB_HANDLE_COLOR);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_INPUT_FILTER) == DevelTextField::Property::INPUT_FILTER);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_STRIKETHROUGH) == DevelTextField::Property::STRIKETHROUGH);
  DALI_TEST_CHECK(field.GetPropertyIndex(PROPERTY_NAME_INPUT_STRIKETHROUGH) == DevelTextField::Property::INPUT_STRIKETHROUGH);

  END_TEST;
}

bool SetPropertyMapRetrieved(TextField& field, const Property::Index property, const std::string mapKey, const std::string mapValue)
{
  bool          result = false;
  Property::Map imageMap;
  imageMap[mapKey] = mapValue;

  field.SetProperty(property, imageMap);
  Property::Value propValue = field.GetProperty(property);
  Property::Map*  resultMap = propValue.GetMap();

  if(resultMap->Find(mapKey)->Get<std::string>() == mapValue)
  {
    result = true;
  }

  return result;
}

// Positive test case for a method
int UtcDaliTextFieldSetPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldSetPropertyP");
  TextField field = TextField::New();
  DALI_TEST_CHECK(field);
  application.GetScene().Add(field);

  // Note - we can't check the defaults since the stylesheets are platform-specific

  // Check the render backend property.
  field.SetProperty(DevelTextField::Property::RENDERING_BACKEND, DevelText::RENDERING_SHARED_ATLAS);
  DALI_TEST_EQUALS((DevelText::RenderingType)field.GetProperty<int>(DevelTextField::Property::RENDERING_BACKEND), DevelText::RENDERING_SHARED_ATLAS, TEST_LOCATION);

  field.SetProperty(DevelTextField::Property::RENDERING_BACKEND, DevelText::RENDERING_VECTOR_BASED);
  DALI_TEST_EQUALS((DevelText::RenderingType)field.GetProperty<int>(DevelTextField::Property::RENDERING_BACKEND), DevelText::RENDERING_VECTOR_BASED, TEST_LOCATION);

  // Check text property.
  field.SetProperty(TextField::Property::TEXT, "Setting Text");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::TEXT), std::string("Setting Text"), TEST_LOCATION);

  // Check placeholder text properties.
  field.SetProperty(TextField::Property::PLACEHOLDER_TEXT, "Setting Placeholder Text");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::PLACEHOLDER_TEXT), std::string("Setting Placeholder Text"), TEST_LOCATION);

  field.SetProperty(TextField::Property::PLACEHOLDER_TEXT_FOCUSED, "Setting Placeholder Text Focused");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::PLACEHOLDER_TEXT_FOCUSED), std::string("Setting Placeholder Text Focused"), TEST_LOCATION);

  // Check font properties.
  field.SetProperty(TextField::Property::FONT_FAMILY, "Setting font family");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::FONT_FAMILY), std::string("Setting font family"), TEST_LOCATION);

  Property::Map    fontStyleMapSet;
  Property::Map    fontStyleMapGet;
  Property::Value* slantValue = NULL;

  fontStyleMapSet.Insert("weight", "bold");
  fontStyleMapSet.Insert("width", "condensed");
  fontStyleMapSet.Insert("slant", "italic");
  field.SetProperty(TextField::Property::FONT_STYLE, fontStyleMapSet);

  fontStyleMapGet = field.GetProperty<Property::Map>(TextField::Property::FONT_STYLE);
  DALI_TEST_EQUALS(fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(fontStyleMapGet, fontStyleMapSet), true, TEST_LOCATION);

  field.SetProperty(TextField::Property::POINT_SIZE, 10.f);
  DALI_TEST_EQUALS(field.GetProperty<float>(TextField::Property::POINT_SIZE), 10.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  field.SetProperty(DevelTextField::Property::FONT_SIZE_SCALE, 2.5f);
  DALI_TEST_EQUALS(field.GetProperty<float>(DevelTextField::Property::FONT_SIZE_SCALE), 2.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  field.SetProperty(DevelTextField::Property::FONT_SIZE_SCALE, 1.0f);

  field.SetProperty(DevelTextField::Property::ENABLE_FONT_SIZE_SCALE, false);
  DALI_TEST_EQUALS(field.GetProperty<bool>(DevelTextField::Property::ENABLE_FONT_SIZE_SCALE), false, TEST_LOCATION);
  field.SetProperty(DevelTextField::Property::ENABLE_FONT_SIZE_SCALE, true);

  // Reset font style.
  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert("weight", "normal");
  fontStyleMapSet.Insert("slant", "oblique");
  field.SetProperty(TextField::Property::FONT_STYLE, fontStyleMapSet);

  fontStyleMapGet = field.GetProperty<Property::Map>(TextField::Property::FONT_STYLE);
  DALI_TEST_EQUALS(fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(fontStyleMapGet, fontStyleMapSet), true, TEST_LOCATION);

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert("slant", "roman");
  field.SetProperty(TextField::Property::FONT_STYLE, fontStyleMapSet);
  fontStyleMapGet = field.GetProperty<Property::Map>(TextField::Property::FONT_STYLE);

  // Replace 'roman' for 'normal'.
  slantValue = fontStyleMapGet.Find("slant");
  if(NULL != slantValue)
  {
    if("normal" == slantValue->Get<std::string>())
    {
      fontStyleMapGet["slant"] = "roman";
    }
  }
  DALI_TEST_EQUALS(fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(fontStyleMapGet, fontStyleMapSet), true, TEST_LOCATION);

  fontStyleMapSet.Clear();

  field.SetProperty(TextField::Property::FONT_STYLE, fontStyleMapSet);
  fontStyleMapGet = field.GetProperty<Property::Map>(TextField::Property::FONT_STYLE);
  DALI_TEST_EQUALS(fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(fontStyleMapGet, fontStyleMapSet), true, TEST_LOCATION);

  // Check that the MAX_LENGTH property can be correctly set
  const int maxNumberOfCharacters = 20;
  field.SetProperty(TextField::Property::MAX_LENGTH, maxNumberOfCharacters);
  DALI_TEST_EQUALS(field.GetProperty<int>(TextField::Property::MAX_LENGTH), maxNumberOfCharacters, TEST_LOCATION);

  // Check exceed policy
  field.SetProperty(TextField::Property::EXCEED_POLICY, Dali::Toolkit::TextField::EXCEED_POLICY_CLIP);
  DALI_TEST_EQUALS(field.GetProperty<int>(TextField::Property::EXCEED_POLICY), static_cast<int>(Dali::Toolkit::TextField::EXCEED_POLICY_CLIP), TEST_LOCATION);
  field.SetProperty(TextField::Property::EXCEED_POLICY, Dali::Toolkit::TextField::EXCEED_POLICY_ORIGINAL);
  DALI_TEST_EQUALS(field.GetProperty<int>(TextField::Property::EXCEED_POLICY), static_cast<int>(Dali::Toolkit::TextField::EXCEED_POLICY_ORIGINAL), TEST_LOCATION);

  // Check that the Alignment properties can be correctly set
  field.SetProperty(TextField::Property::HORIZONTAL_ALIGNMENT, "END");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::HORIZONTAL_ALIGNMENT), "END", TEST_LOCATION);
  field.SetProperty(TextField::Property::VERTICAL_ALIGNMENT, "CENTER");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::VERTICAL_ALIGNMENT), "CENTER", TEST_LOCATION);

  // Check text's color property
  field.SetProperty(TextField::Property::TEXT_COLOR, Color::WHITE);
  DALI_TEST_EQUALS(field.GetProperty<Vector4>(TextField::Property::TEXT_COLOR), Color::WHITE, TEST_LOCATION);

  // Check placeholder text's color property.
  field.SetProperty(TextField::Property::PLACEHOLDER_TEXT_COLOR, Color::RED);
  DALI_TEST_EQUALS(field.GetProperty<Vector4>(TextField::Property::PLACEHOLDER_TEXT_COLOR), Color::RED, TEST_LOCATION);

  // Check cursor properties
  field.SetProperty(TextField::Property::PRIMARY_CURSOR_COLOR, Color::RED);
  DALI_TEST_EQUALS(field.GetProperty<Vector4>(TextField::Property::PRIMARY_CURSOR_COLOR), Color::RED, TEST_LOCATION);
  field.SetProperty(TextField::Property::SECONDARY_CURSOR_COLOR, Color::BLUE);
  DALI_TEST_EQUALS(field.GetProperty<Vector4>(TextField::Property::SECONDARY_CURSOR_COLOR), Color::BLUE, TEST_LOCATION);

  field.SetProperty(TextField::Property::ENABLE_CURSOR_BLINK, false);
  DALI_TEST_EQUALS(field.GetProperty<bool>(TextField::Property::ENABLE_CURSOR_BLINK), false, TEST_LOCATION);
  field.SetProperty(TextField::Property::CURSOR_BLINK_INTERVAL, 1.f);
  DALI_TEST_EQUALS(field.GetProperty<float>(TextField::Property::CURSOR_BLINK_INTERVAL), 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  field.SetProperty(TextField::Property::CURSOR_BLINK_DURATION, 10.f);
  DALI_TEST_EQUALS(field.GetProperty<float>(TextField::Property::CURSOR_BLINK_DURATION), 10.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  field.SetProperty(TextField::Property::CURSOR_WIDTH, 1);
  DALI_TEST_EQUALS(field.GetProperty<int>(TextField::Property::CURSOR_WIDTH), 1, TEST_LOCATION);

  // Check scroll properties.
  field.SetProperty(TextField::Property::SCROLL_THRESHOLD, 1.f);
  DALI_TEST_EQUALS(field.GetProperty<float>(TextField::Property::SCROLL_THRESHOLD), 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  field.SetProperty(TextField::Property::SCROLL_SPEED, 100.f);
  DALI_TEST_EQUALS(field.GetProperty<float>(TextField::Property::SCROLL_SPEED), 100.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Check handle images
  field.SetProperty(TextField::Property::GRAB_HANDLE_IMAGE, "image1");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::GRAB_HANDLE_IMAGE), "image1", TEST_LOCATION);
  field.SetProperty(TextField::Property::GRAB_HANDLE_PRESSED_IMAGE, "image2");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::GRAB_HANDLE_PRESSED_IMAGE), "image2", TEST_LOCATION);
  field.SetProperty(TextField::Property::SELECTION_HANDLE_IMAGE_LEFT, "image3");

  // Check handle images
  DALI_TEST_CHECK(SetPropertyMapRetrieved(field, TextField::Property::SELECTION_HANDLE_IMAGE_LEFT, "filename", "leftHandleImage"));
  DALI_TEST_CHECK(SetPropertyMapRetrieved(field, TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT, "filename", "rightHandleImage"));
  DALI_TEST_CHECK(SetPropertyMapRetrieved(field, TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT, "filename", "leftHandleImagePressed"));
  DALI_TEST_CHECK(SetPropertyMapRetrieved(field, TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT, "filename", "rightHandleImagePressed"));
  DALI_TEST_CHECK(SetPropertyMapRetrieved(field, TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT, "filename", "leftHandleMarkerImage"));
  DALI_TEST_CHECK(SetPropertyMapRetrieved(field, TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT, "filename", "rightHandleMarkerImage"));

  // Check the highlight color
  field.SetProperty(TextField::Property::SELECTION_HIGHLIGHT_COLOR, Color::GREEN);
  DALI_TEST_EQUALS(field.GetProperty<Vector4>(TextField::Property::SELECTION_HIGHLIGHT_COLOR), Color::GREEN, TEST_LOCATION);

  // Decoration bounding box
  field.SetProperty(TextField::Property::DECORATION_BOUNDING_BOX, Rect<int>(0, 0, 1, 1));
  DALI_TEST_EQUALS(field.GetProperty<Rect<int> >(TextField::Property::DECORATION_BOUNDING_BOX), Rect<int>(0, 0, 1, 1), TEST_LOCATION);

  // Check the input method setting
  Property::Map                   propertyMap;
  InputMethod::PanelLayout::Type  panelLayout    = InputMethod::PanelLayout::NUMBER;
  InputMethod::AutoCapital::Type  autoCapital    = InputMethod::AutoCapital::WORD;
  InputMethod::ButtonAction::Type buttonAction   = InputMethod::ButtonAction::GO;
  int                             inputVariation = 1;
  propertyMap["PANEL_LAYOUT"]                    = panelLayout;
  propertyMap["AUTO_CAPITALIZE"]                 = autoCapital;
  propertyMap["BUTTON_ACTION"]                   = buttonAction;
  propertyMap["VARIATION"]                       = inputVariation;
  field.SetProperty(TextField::Property::INPUT_METHOD_SETTINGS, propertyMap);

  Property::Value value = field.GetProperty(TextField::Property::INPUT_METHOD_SETTINGS);
  Property::Map   map;
  DALI_TEST_CHECK(value.Get(map));

  int layout = 0;
  DALI_TEST_CHECK(map["PANEL_LAYOUT"].Get(layout));
  DALI_TEST_EQUALS(static_cast<int>(panelLayout), layout, TEST_LOCATION);

  int capital = 0;
  DALI_TEST_CHECK(map["AUTO_CAPITALIZE"].Get(capital));
  DALI_TEST_EQUALS(static_cast<int>(autoCapital), capital, TEST_LOCATION);

  int action = 0;
  DALI_TEST_CHECK(map["BUTTON_ACTION"].Get(action));
  DALI_TEST_EQUALS(static_cast<int>(buttonAction), action, TEST_LOCATION);

  int variation = 0;
  DALI_TEST_CHECK(map["VARIATION"].Get(variation));
  DALI_TEST_EQUALS(inputVariation, variation, TEST_LOCATION);

  // Check input color property.
  field.SetProperty(TextField::Property::INPUT_COLOR, Color::YELLOW);
  DALI_TEST_EQUALS(field.GetProperty<Vector4>(TextField::Property::INPUT_COLOR), Color::YELLOW, TEST_LOCATION);

  // Check the enable markup property.
  DALI_TEST_CHECK(!field.GetProperty<bool>(TextField::Property::ENABLE_MARKUP));
  field.SetProperty(TextField::Property::ENABLE_MARKUP, true);
  DALI_TEST_CHECK(field.GetProperty<bool>(TextField::Property::ENABLE_MARKUP));

  // Check input font properties.
  field.SetProperty(TextField::Property::INPUT_FONT_FAMILY, "Setting input font family");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::INPUT_FONT_FAMILY), "Setting input font family", TEST_LOCATION);

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert("weight", "bold");
  fontStyleMapSet.Insert("width", "condensed");
  fontStyleMapSet.Insert("slant", "italic");

  field.SetProperty(TextField::Property::INPUT_FONT_STYLE, fontStyleMapSet);
  fontStyleMapGet = field.GetProperty<Property::Map>(TextField::Property::INPUT_FONT_STYLE);
  DALI_TEST_EQUALS(fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(fontStyleMapGet, fontStyleMapSet), true, TEST_LOCATION);

  field.SetProperty(TextField::Property::INPUT_POINT_SIZE, 12.f);
  DALI_TEST_EQUALS(field.GetProperty<float>(TextField::Property::INPUT_POINT_SIZE), 12.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Reset input font style.
  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert("weight", "normal");
  fontStyleMapSet.Insert("slant", "oblique");

  field.SetProperty(TextField::Property::INPUT_FONT_STYLE, fontStyleMapSet);
  fontStyleMapGet = field.GetProperty<Property::Map>(TextField::Property::INPUT_FONT_STYLE);
  DALI_TEST_EQUALS(fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(fontStyleMapGet, fontStyleMapSet), true, TEST_LOCATION);

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert("slant", "roman");

  field.SetProperty(TextField::Property::INPUT_FONT_STYLE, fontStyleMapSet);
  fontStyleMapGet = field.GetProperty<Property::Map>(TextField::Property::INPUT_FONT_STYLE);

  // Replace 'roman' for 'normal'.
  slantValue = fontStyleMapGet.Find("slant");
  if(NULL != slantValue)
  {
    if("normal" == slantValue->Get<std::string>())
    {
      fontStyleMapGet["slant"] = "roman";
    }
  }
  DALI_TEST_EQUALS(fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(fontStyleMapGet, fontStyleMapSet), true, TEST_LOCATION);

  fontStyleMapSet.Clear();

  field.SetProperty(TextField::Property::INPUT_FONT_STYLE, fontStyleMapSet);
  fontStyleMapGet = field.GetProperty<Property::Map>(TextField::Property::INPUT_FONT_STYLE);
  DALI_TEST_EQUALS(fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(fontStyleMapGet, fontStyleMapSet), true, TEST_LOCATION);

  Property::Map strikethroughMapSet;
  Property::Map strikethroughMapGet;

  strikethroughMapSet.Insert("enable", true);
  strikethroughMapSet.Insert("color", Color::RED);
  strikethroughMapSet.Insert("height", 2.0f);

  // Check the strikethrough property
  field.SetProperty(DevelTextField::Property::STRIKETHROUGH, strikethroughMapSet);

  strikethroughMapGet = field.GetProperty<Property::Map>(DevelTextField::Property::STRIKETHROUGH);
  DALI_TEST_EQUALS(strikethroughMapGet.Count(), strikethroughMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(strikethroughMapGet, strikethroughMapSet), true, TEST_LOCATION);

  // Check the input strikethrough property
  field.SetProperty(DevelTextField::Property::INPUT_STRIKETHROUGH, "Strikethrough input properties");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(DevelTextField::Property::INPUT_STRIKETHROUGH), std::string("Strikethrough input properties"), TEST_LOCATION);

  Property::Map underlineMapSet;
  Property::Map underlineMapGet;

  underlineMapSet.Insert("enable", true);
  underlineMapSet.Insert("color", Color::RED);
  underlineMapSet.Insert("height", 1);
  underlineMapSet.Insert("type", Text::Underline::SOLID);
  underlineMapSet.Insert("dashWidth", 2);
  underlineMapSet.Insert("dashGap", 1);

  // Check the underline property
  field.SetProperty(TextField::Property::UNDERLINE, underlineMapSet);

  underlineMapGet = field.GetProperty<Property::Map>(TextField::Property::UNDERLINE);
  DALI_TEST_EQUALS(underlineMapGet.Count(), underlineMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(underlineMapGet, underlineMapSet), true, TEST_LOCATION);

  // Render and notify
  application.SendNotification();
  application.Render();

  underlineMapSet.Clear();
  underlineMapGet.Clear();

  underlineMapSet.Insert("enable", true);
  underlineMapSet.Insert("color", Color::BLUE);
  underlineMapSet.Insert("height", 1);
  underlineMapSet.Insert("type", Text::Underline::DASHED);
  underlineMapSet.Insert("dashWidth", 4);
  underlineMapSet.Insert("dashGap", 2);

  // Check the dashed underline property
  field.SetProperty(TextField::Property::UNDERLINE, underlineMapSet);

  underlineMapGet = field.GetProperty<Property::Map>(TextField::Property::UNDERLINE);
  DALI_TEST_EQUALS(underlineMapGet.Count(), underlineMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(underlineMapGet, underlineMapSet), true, TEST_LOCATION);

  // Render and notify
  application.SendNotification();
  application.Render();

  underlineMapSet.Clear();
  underlineMapGet.Clear();

  underlineMapSet.Insert("enable", true);
  underlineMapSet.Insert("color", Color::BLUE);
  underlineMapSet.Insert("height", 4);
  underlineMapSet.Insert("type", Text::Underline::DOUBLE);
  underlineMapSet.Insert("dashWidth", 4);
  underlineMapSet.Insert("dashGap", 2);

  // Check the dashed underline property
  field.SetProperty(TextField::Property::UNDERLINE, underlineMapSet);

  underlineMapGet = field.GetProperty<Property::Map>(TextField::Property::UNDERLINE);
  DALI_TEST_EQUALS(underlineMapGet.Count(), underlineMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(underlineMapGet, underlineMapSet), true, TEST_LOCATION);

  // Render and notify
  application.SendNotification();
  application.Render();

  underlineMapSet.Clear();
  underlineMapGet.Clear();

  // Check the input underline property
  field.SetProperty(TextField::Property::INPUT_UNDERLINE, "Underline input properties");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::INPUT_UNDERLINE), std::string("Underline input properties"), TEST_LOCATION);

  // Check the shadow property
  Property::Map shadowMapSet;
  Property::Map shadowMapGet;

  shadowMapSet.Insert("color", Color::GREEN);
  shadowMapSet.Insert("offset", Vector2(2.0f, 2.0f));
  shadowMapSet.Insert("blurRadius", 3.0f);

  field.SetProperty(TextField::Property::SHADOW, shadowMapSet);

  shadowMapGet = field.GetProperty<Property::Map>(TextField::Property::SHADOW);
  DALI_TEST_EQUALS(shadowMapGet.Count(), shadowMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(shadowMapGet, shadowMapSet), true, TEST_LOCATION);

  // Check the input shadow property
  field.SetProperty(TextField::Property::INPUT_SHADOW, "Shadow input properties");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::INPUT_SHADOW), std::string("Shadow input properties"), TEST_LOCATION);

  // Check the emboss property
  field.SetProperty(TextField::Property::EMBOSS, "Emboss properties");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::EMBOSS), std::string("Emboss properties"), TEST_LOCATION);

  // Check the input emboss property
  field.SetProperty(TextField::Property::INPUT_EMBOSS, "Emboss input properties");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::INPUT_EMBOSS), std::string("Emboss input properties"), TEST_LOCATION);

  // Check the outline property

  // Test string type first
  // This is purely to maintain backward compatibility, but we don't support string as the outline property type.
  field.SetProperty(TextField::Property::OUTLINE, "Outline properties");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::OUTLINE), std::string("Outline properties"), TEST_LOCATION);

  // Then test the property map type
  Property::Map outlineMapSet;
  Property::Map outlineMapGet;

  outlineMapSet["color"] = Color::RED;
  outlineMapSet["width"] = 2.0f;

  field.SetProperty(TextField::Property::OUTLINE, outlineMapSet);

  outlineMapGet = field.GetProperty<Property::Map>(TextField::Property::OUTLINE);
  DALI_TEST_EQUALS(outlineMapGet.Count(), outlineMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(outlineMapGet, outlineMapSet), true, TEST_LOCATION);

  // Check the input outline property
  field.SetProperty(TextField::Property::INPUT_OUTLINE, "Outline input properties");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::INPUT_OUTLINE), std::string("Outline input properties"), TEST_LOCATION);

  // Check the hidden input settings property
  Property::Map hiddenMapSet;
  Property::Map hiddenMapGet;
  hiddenMapSet[HiddenInput::Property::MODE]                         = HiddenInput::Mode::HIDE_ALL;
  hiddenMapSet[HiddenInput::Property::SHOW_LAST_CHARACTER_DURATION] = 2;
  hiddenMapSet[HiddenInput::Property::SUBSTITUTE_COUNT]             = 4;
  hiddenMapSet[HiddenInput::Property::SUBSTITUTE_CHARACTER]         = 0x23;
  field.SetProperty(TextField::Property::HIDDEN_INPUT_SETTINGS, hiddenMapSet);

  hiddenMapGet = field.GetProperty<Property::Map>(TextField::Property::HIDDEN_INPUT_SETTINGS);
  DALI_TEST_EQUALS(hiddenMapSet.Count(), hiddenMapGet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(hiddenMapSet, hiddenMapGet), true, TEST_LOCATION);

  // Check the pixel size of font
  field.SetProperty(TextField::Property::PIXEL_SIZE, 20.f);
  DALI_TEST_EQUALS(field.GetProperty<float>(TextField::Property::PIXEL_SIZE), 20.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  // Check the enable selection property
  field.SetProperty(TextField::Property::ENABLE_SELECTION, false);
  DALI_TEST_EQUALS(field.GetProperty<bool>(TextField::Property::ENABLE_SELECTION), false, TEST_LOCATION);

  // Check the placeholder property with pixel size
  Property::Map placeholderPixelSizeMapSet;
  Property::Map placeholderPixelSizeMapGet;
  Property::Map placeholderFontstyleMap;
  placeholderPixelSizeMapSet["text"]        = "Setting Placeholder Text";
  placeholderPixelSizeMapSet["textFocused"] = "Setting Placeholder Text Focused";
  placeholderPixelSizeMapSet["color"]       = Color::BLUE;
  placeholderPixelSizeMapSet["fontFamily"]  = "Arial";
  placeholderPixelSizeMapSet["pixelSize"]   = 15.0f;
  placeholderPixelSizeMapSet["ellipsis"]    = true;

  placeholderFontstyleMap.Insert("weight", "bold");
  placeholderPixelSizeMapSet["placeholderFontStyle"] = placeholderFontstyleMap;
  field.SetProperty(TextField::Property::PLACEHOLDER, placeholderPixelSizeMapSet);

  placeholderPixelSizeMapGet = field.GetProperty<Property::Map>(TextField::Property::PLACEHOLDER);
  DALI_TEST_EQUALS(placeholderPixelSizeMapGet.Count(), placeholderPixelSizeMapSet.Count(), TEST_LOCATION);

  tet_infoline("Test Placeholder settings set as strings is converted correctly to Property Index key and holds set value");
  Property::Map placeholderConversionMap;
  placeholderConversionMap[Text::PlaceHolder::Property::TEXT]         = placeholderPixelSizeMapSet["text"];
  placeholderConversionMap[Text::PlaceHolder::Property::TEXT_FOCUSED] = placeholderPixelSizeMapSet["textFocused"];
  placeholderConversionMap[Text::PlaceHolder::Property::COLOR]        = placeholderPixelSizeMapSet["color"];
  placeholderConversionMap[Text::PlaceHolder::Property::FONT_STYLE]   = placeholderPixelSizeMapSet["fontStyle"];
  placeholderConversionMap[Text::PlaceHolder::Property::FONT_FAMILY]  = placeholderPixelSizeMapSet["fontFamily"];
  placeholderConversionMap[Text::PlaceHolder::Property::PIXEL_SIZE]   = placeholderPixelSizeMapSet["pixelSize"];

  DALI_TEST_EQUALS(DaliTestCheckMaps(placeholderPixelSizeMapGet, placeholderConversionMap), true, TEST_LOCATION);

  // Check the placeholder property with point size
  Property::Map placeholderMapSet;
  Property::Map placeholderMapGet;
  placeholderMapSet["text"]        = "Setting Placeholder Text";
  placeholderMapSet["textFocused"] = "Setting Placeholder Text Focused";
  placeholderMapSet["color"]       = Color::RED;
  placeholderMapSet["fontFamily"]  = "Arial";
  placeholderMapSet["pointSize"]   = 12.0f;
  placeholderMapSet["ellipsis"]    = false;

  // Check the placeholder font style property
  placeholderFontstyleMap.Clear();

  placeholderFontstyleMap.Insert("weight", "bold");
  placeholderFontstyleMap.Insert("width", "condensed");
  placeholderFontstyleMap.Insert("slant", "italic");
  placeholderMapSet["fontStyle"] = placeholderFontstyleMap;
  field.SetProperty(TextField::Property::PLACEHOLDER, placeholderMapSet);

  placeholderMapGet = field.GetProperty<Property::Map>(TextField::Property::PLACEHOLDER);
  DALI_TEST_EQUALS(placeholderMapGet.Count(), placeholderMapSet.Count(), TEST_LOCATION);

  placeholderConversionMap.Clear();
  placeholderConversionMap[Text::PlaceHolder::Property::TEXT]         = placeholderPixelSizeMapSet["text"];
  placeholderConversionMap[Text::PlaceHolder::Property::TEXT_FOCUSED] = placeholderPixelSizeMapSet["textFocused"];
  placeholderConversionMap[Text::PlaceHolder::Property::COLOR]        = placeholderPixelSizeMapSet["color"];
  placeholderConversionMap[Text::PlaceHolder::Property::FONT_STYLE]   = placeholderPixelSizeMapSet["fontStyle"];
  placeholderConversionMap[Text::PlaceHolder::Property::FONT_FAMILY]  = placeholderPixelSizeMapSet["fontFamily"];
  placeholderConversionMap[Text::PlaceHolder::Property::POINT_SIZE]   = placeholderPixelSizeMapSet["pointSize"];

  DALI_TEST_EQUALS(DaliTestCheckMaps(placeholderMapGet, placeholderConversionMap), true, TEST_LOCATION);

  // Reset font style.
  placeholderFontstyleMap.Clear();
  placeholderFontstyleMap.Insert("weight", "normal");
  placeholderFontstyleMap.Insert("slant", "oblique");
  placeholderMapSet["fontStyle"] = placeholderFontstyleMap;
  field.SetProperty(TextField::Property::PLACEHOLDER, placeholderMapSet);

  placeholderMapGet = field.GetProperty<Property::Map>(TextField::Property::PLACEHOLDER);
  DALI_TEST_EQUALS(placeholderMapGet.Count(), placeholderMapSet.Count(), TEST_LOCATION);

  placeholderConversionMap[Text::PlaceHolder::Property::FONT_STYLE] = placeholderPixelSizeMapSet["fontStyle"];
  DALI_TEST_EQUALS(DaliTestCheckMaps(placeholderMapGet, placeholderConversionMap), true, TEST_LOCATION);

  placeholderFontstyleMap.Clear();
  placeholderFontstyleMap.Insert("slant", "roman");
  placeholderMapSet["fontStyle"]                                    = placeholderFontstyleMap;
  placeholderConversionMap[Text::PlaceHolder::Property::FONT_STYLE] = placeholderPixelSizeMapSet["fontStyle"];

  field.SetProperty(TextField::Property::PLACEHOLDER, placeholderMapSet);

  placeholderMapGet = field.GetProperty<Property::Map>(TextField::Property::PLACEHOLDER);

  placeholderFontstyleMap.Clear();
  placeholderMapSet["fontStyle"]                                    = placeholderFontstyleMap;
  placeholderConversionMap[Text::PlaceHolder::Property::FONT_STYLE] = placeholderPixelSizeMapSet["fontStyle"];

  field.SetProperty(TextField::Property::PLACEHOLDER, placeholderMapSet);
  placeholderMapGet = field.GetProperty<Property::Map>(TextField::Property::PLACEHOLDER);
  DALI_TEST_EQUALS(placeholderMapGet.Count(), placeholderMapSet.Count(), TEST_LOCATION);

  DALI_TEST_EQUALS(DaliTestCheckMaps(placeholderMapGet, placeholderConversionMap), true, TEST_LOCATION);

  // Check the ellipsis property
  DALI_TEST_CHECK(!field.GetProperty<bool>(TextField::Property::ELLIPSIS));
  field.SetProperty(TextField::Property::ELLIPSIS, true);
  DALI_TEST_CHECK(field.GetProperty<bool>(TextField::Property::ELLIPSIS));

  field.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_EQUALS(field.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::RIGHT_TO_LEFT), TEST_LOCATION);

  // Test the ENABLE_GRAB_HANDLE_POPUP property
  DALI_TEST_CHECK(field.GetProperty<bool>(DevelTextField::Property::ENABLE_GRAB_HANDLE_POPUP));
  field.SetProperty(DevelTextField::Property::ENABLE_GRAB_HANDLE_POPUP, false);
  DALI_TEST_CHECK(!field.GetProperty<bool>(DevelTextField::Property::ENABLE_GRAB_HANDLE_POPUP));

  // Check the background property
  field.SetProperty(DevelTextField::Property::BACKGROUND, Color::RED);
  DALI_TEST_EQUALS(field.GetProperty<Vector4>(DevelTextField::Property::BACKGROUND), Color::RED, TEST_LOCATION);

  //Check handle color
  field.SetProperty(DevelTextField::Property::GRAB_HANDLE_COLOR, Color::GREEN);
  DALI_TEST_EQUALS(field.GetProperty<Vector4>(DevelTextField::Property::GRAB_HANDLE_COLOR), Color::GREEN, TEST_LOCATION);

  // Check the input filter property
  Property::Map inputFilterMapSet;
  Property::Map inputFilterMapGet;
  inputFilterMapSet[InputFilter::Property::ACCEPTED] = "[\\w]";
  inputFilterMapSet[InputFilter::Property::REJECTED] = "[\\d]";

  field.SetProperty(DevelTextField::Property::INPUT_FILTER, inputFilterMapSet);

  inputFilterMapGet = field.GetProperty<Property::Map>(DevelTextField::Property::INPUT_FILTER);
  DALI_TEST_EQUALS(inputFilterMapGet.Count(), inputFilterMapSet.Count(), TEST_LOCATION);

  // Clear
  inputFilterMapSet.Clear();
  field.SetProperty(DevelTextField::Property::INPUT_FILTER, inputFilterMapSet);

  application.SendNotification();
  application.Render();

  END_TEST;
}

// Positive Atlas Text Renderer test
int utcDaliTextFieldAtlasRenderP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldAtlasRenderP");
  StyleManager styleManager = StyleManager::Get();
  styleManager.ApplyDefaultTheme();
  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  field.SetProperty(TextField::Property::HORIZONTAL_ALIGNMENT, "CENTER");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  application.GetScene().Add(field);

  try
  {
    // Render some text with the shared atlas backend
    field.SetProperty(DevelTextField::Property::RENDERING_BACKEND, DevelText::RENDERING_SHARED_ATLAS);
    application.SendNotification();
    application.Render();
  }
  catch(...)
  {
    tet_result(TET_FAIL);
  }
  END_TEST;
}

// Positive test for the anchorClicked signal.
int utcDaliTextFieldAnchorClicked01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldAnchorClicked01");
  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  // connect to the anchor clicked signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextField::AnchorClickedSignal(field).Connect(&TestAnchorClickedCallback);
  bool anchorClickedSignal = false;
  field.ConnectSignal(testTracker, "anchorClicked", CallbackFunctor(&anchorClickedSignal));

  gAnchorClickedCallBackCalled = false;
  field.SetProperty(TextField::Property::TEXT, "<a href='https://www.tizen.org'>TIZEN</a>");
  field.SetProperty(TextField::Property::ENABLE_MARKUP, true);
  field.SetProperty(Actor::Property::SIZE, Vector2(100.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  application.SendNotification();
  application.Render();
  field.SetKeyInputFocus();

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 5.0f, 25.0f);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAnchorClickedCallBackCalled);
  DALI_TEST_CHECK(anchorClickedSignal);

  gAnchorClickedCallBackNotCalled = true;
  // Tap the outside of anchor, callback should not be called.
  TestGenerateTap(application, 150.f, 100.f);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAnchorClickedCallBackNotCalled);

  END_TEST;
}

// Positive test for the anchorClicked signal.
int utcDaliTextFieldAnchorClicked02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldAnchorClicked02");
  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  // connect to the anchor clicked signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextField::AnchorClickedSignal(field).Connect(&TestAnchorClickedCallback);
  bool anchorClickedSignal = false;
  field.ConnectSignal(testTracker, "anchorClicked", CallbackFunctor(&anchorClickedSignal));

  gAnchorClickedCallBackCalled = false;
  field.SetProperty(TextField::Property::TEXT, "<a href='https://www.tizen.org'>TIZEN</a>");
  field.SetProperty(TextField::Property::ENABLE_MARKUP, true);
  field.SetProperty(Actor::Property::SIZE, Vector2(100.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  application.SendNotification();
  application.Render();
  field.SetKeyInputFocus();

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 30.0f, 25.0f, 100);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAnchorClickedCallBackCalled);
  DALI_TEST_CHECK(anchorClickedSignal);

  // For coverage InsertTextAnchor, RemoveTextAnchor
  // first index insert
  field.SetProperty(TextField::Property::TEXT, "<a href='https://www.tizen.org'>TIZEN</a>");
  field.SetProperty(DevelTextField::Property::PRIMARY_CURSOR_POSITION, 0);
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateKey("D", "", "D", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "D", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  gAnchorClickedCallBackCalled = false;
  // Create a tap event to touch the text field.
  TestGenerateTap(application, 30.0f, 25.0f, 700);
  application.SendNotification();
  field.SetKeyInputFocus();

  DALI_TEST_CHECK(gAnchorClickedCallBackCalled);

  // last index insert
  field.SetProperty(TextField::Property::TEXT, "<a href='https://www.tizen.org'>TIZEN</a>");
  field.SetProperty(DevelTextField::Property::PRIMARY_CURSOR_POSITION, 5);
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateKey("D", "", "D", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "D", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  gAnchorClickedCallBackCalled = false;
  // Create a tap event to touch the text field.
  TestGenerateTap(application, 30.0f, 25.0f, 1300);
  application.SendNotification();
  field.SetKeyInputFocus();

  DALI_TEST_CHECK(gAnchorClickedCallBackCalled);

  // mid index insert
  field.SetProperty(TextField::Property::TEXT, "<a href='https://www.tizen.org'>TIZEN</a>");
  field.SetProperty(DevelTextField::Property::PRIMARY_CURSOR_POSITION, 2);
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateKey("D", "", "D", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "D", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  gAnchorClickedCallBackCalled = false;
  // Create a tap event to touch the text field.
  TestGenerateTap(application, 30.0f, 25.0f, 1900);
  application.SendNotification();
  field.SetKeyInputFocus();

  DALI_TEST_CHECK(gAnchorClickedCallBackCalled);

  // first index remove
  field.SetProperty(TextField::Property::TEXT, "<a href='https://www.tizen.org'>TIZEN</a>");
  field.SetProperty(DevelTextField::Property::PRIMARY_CURSOR_POSITION, 0);
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("Delete", "", "Delete", Dali::DevelKey::DALI_KEY_DELETE, 0, 0, Integration::KeyEvent::DOWN, "Delete", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  gAnchorClickedCallBackCalled = false;
  // Create a tap event to touch the text field.
  TestGenerateTap(application, 30.0f, 25.0f, 2500);
  application.SendNotification();
  field.SetKeyInputFocus();

  DALI_TEST_CHECK(gAnchorClickedCallBackCalled);

  // last index remove
  field.SetProperty(TextField::Property::TEXT, "<a href='https://www.tizen.org'>TIZEN</a>");
  field.SetProperty(DevelTextField::Property::PRIMARY_CURSOR_POSITION, 5);
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("Delete", "", "Delete", Dali::DevelKey::DALI_KEY_DELETE, 0, 0, Integration::KeyEvent::DOWN, "Delete", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  gAnchorClickedCallBackCalled = false;
  // Create a tap event to touch the text field.
  TestGenerateTap(application, 30.0f, 25.0f, 3100);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAnchorClickedCallBackCalled);

  // middle index
  field.SetProperty(TextField::Property::TEXT, "<a href='https://www.tizen.org'>TIZEN</a>");
  field.SetProperty(DevelTextField::Property::PRIMARY_CURSOR_POSITION, 2);
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("Delete", "", "Delete", Dali::DevelKey::DALI_KEY_DELETE, 0, 0, Integration::KeyEvent::DOWN, "Delete", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  gAnchorClickedCallBackCalled = false;
  // Create a tap event to touch the text field.
  TestGenerateTap(application, 30.0f, 25.0f, 3700);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAnchorClickedCallBackCalled);

  // 0 ~ 1 index remove
  field.SetProperty(TextField::Property::TEXT, "<a href='https://www.tizen.org'>TIZEN</a>");
  field.SetProperty(DevelTextField::Property::SELECTED_TEXT_START, 0);
  field.SetProperty(DevelTextField::Property::SELECTED_TEXT_END, 1);
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  gAnchorClickedCallBackCalled = false;
  // Create a tap event to touch the text field.
  TestGenerateTap(application, 30.0f, 25.0f, 4300);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAnchorClickedCallBackCalled);

  // 1 ~ 3 index remove
  field.SetProperty(TextField::Property::TEXT, "<a href='https://www.tizen.org'>TIZEN</a>");
  field.SetProperty(DevelTextField::Property::SELECTED_TEXT_START, 1);
  field.SetProperty(DevelTextField::Property::SELECTED_TEXT_END, 3);
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  gAnchorClickedCallBackCalled = false;
  // Create a tap event to touch the text field.
  TestGenerateTap(application, 30.0f, 25.0f, 4900);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAnchorClickedCallBackCalled);

  // 3 ~ 4 index remove
  field.SetProperty(TextField::Property::TEXT, "<a href='https://www.tizen.org'>TIZEN</a>");
  field.SetProperty(DevelTextField::Property::SELECTED_TEXT_START, 3);
  field.SetProperty(DevelTextField::Property::SELECTED_TEXT_END, 4);
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  gAnchorClickedCallBackCalled = false;
  // Create a tap event to touch the text field.
  TestGenerateTap(application, 30.0f, 25.0f, 5500);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAnchorClickedCallBackCalled);

  // Remove front of anchor
  field.SetProperty(TextField::Property::TEXT, "TIZEN<a href='https://www.tizen.org'>TIZEN</a>");
  field.SetProperty(DevelTextField::Property::PRIMARY_CURSOR_POSITION, 3);
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  // Remove whole text
  field.SetProperty(TextField::Property::TEXT, "<a href='https://www.tizen.org'>TIZEN</a>");
  DevelTextField::SelectWholeText(field);
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  // Remove all with backspace
  field.SetProperty(TextField::Property::TEXT, "<a href='https://www.tizen.org'>T</a>");
  field.SetProperty(DevelTextField::Property::PRIMARY_CURSOR_POSITION, 1);
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  // Remove all with delete
  field.SetProperty(TextField::Property::TEXT, "<a href='https://www.tizen.org'>T</a>");
  field.SetProperty(DevelTextField::Property::PRIMARY_CURSOR_POSITION, 0);
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateKey("Delete", "", "Delete", Dali::DevelKey::DALI_KEY_DELETE, 0, 0, Integration::KeyEvent::DOWN, "Delete", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  END_TEST;
}

// Positive test for the textChanged signal.
int utcDaliTextFieldTextChangedP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldTextChangedP");
  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  field.TextChangedSignal().Connect(&TestTextChangedCallback);
  bool textChangedSignal = false;
  field.ConnectSignal(testTracker, "textChanged", CallbackFunctor(&textChangedSignal));

  gTextChangedCallBackCalled = false;
  field.SetProperty(TextField::Property::TEXT, "ABC");
  DALI_TEST_CHECK(gTextChangedCallBackCalled);
  DALI_TEST_CHECK(textChangedSignal);

  application.SendNotification();
  field.SetKeyInputFocus();

  gTextChangedCallBackCalled = false;
  application.ProcessEvent(GenerateKey("D", "", "D", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "D", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  DALI_TEST_CHECK(gTextChangedCallBackCalled);

  // Remove all text
  field.SetProperty(TextField::Property::TEXT, "");

  // Pressing backspace key: TextChangedCallback should not be called when there is no text in textfield.
  gTextChangedCallBackCalled = false;
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  DALI_TEST_CHECK(!gTextChangedCallBackCalled);

  // Pressing delete key: TextChangedCallback should not be called when there is no text in textfield.
  gTextChangedCallBackCalled = false;
  application.ProcessEvent(GenerateKey("", "", "", Dali::DevelKey::DALI_KEY_DELETE, 0, 0, Integration::KeyEvent::DOWN, "Delete", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  DALI_TEST_CHECK(!gTextChangedCallBackCalled);

  END_TEST;
}

int utcDaliTextFieldTextChangedWithInputMethodContext(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldTextChangedWithInputMethodContext");
  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  field.TextChangedSignal().Connect(&TestTextChangedCallback);
  bool textChangedSignal = false;
  field.ConnectSignal(testTracker, "textChanged", CallbackFunctor(&textChangedSignal));

  // get InputMethodContext
  std::string                   text;
  InputMethodContext::EventData imfEvent;
  InputMethodContext            inputMethodContext = DevelTextField::GetInputMethodContext(field);

  field.SetKeyInputFocus();
  field.SetProperty(DevelTextField::Property::ENABLE_EDITING, true);

  // input text
  gTextChangedCallBackCalled = false;
  imfEvent                   = InputMethodContext::EventData(InputMethodContext::PRE_EDIT, "ㅎ", 0, 1);
  inputMethodContext.EventReceivedSignal().Emit(inputMethodContext, imfEvent);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(gTextChangedCallBackCalled);
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::TEXT), std::string("ㅎ"), TEST_LOCATION);

  gTextChangedCallBackCalled = false;
  imfEvent                   = InputMethodContext::EventData(InputMethodContext::PRE_EDIT, "호", 0, 1);
  inputMethodContext.EventReceivedSignal().Emit(inputMethodContext, imfEvent);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(gTextChangedCallBackCalled);
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::TEXT), std::string("호"), TEST_LOCATION);

  gTextChangedCallBackCalled = false;
  imfEvent                   = InputMethodContext::EventData(InputMethodContext::PRE_EDIT, "혿", 0, 1);
  inputMethodContext.EventReceivedSignal().Emit(inputMethodContext, imfEvent);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(gTextChangedCallBackCalled);
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::TEXT), std::string("혿"), TEST_LOCATION);

  gTextChangedCallBackCalled = false;
  imfEvent                   = InputMethodContext::EventData(InputMethodContext::PRE_EDIT, "", 0, 1);
  inputMethodContext.EventReceivedSignal().Emit(inputMethodContext, imfEvent);
  DALI_TEST_CHECK(!gTextChangedCallBackCalled);

  imfEvent = InputMethodContext::EventData(InputMethodContext::COMMIT, "호", 0, 1);
  inputMethodContext.EventReceivedSignal().Emit(inputMethodContext, imfEvent);
  DALI_TEST_CHECK(!gTextChangedCallBackCalled);

  imfEvent = InputMethodContext::EventData(InputMethodContext::PRE_EDIT, "두", 1, 2);
  inputMethodContext.EventReceivedSignal().Emit(inputMethodContext, imfEvent);
  DALI_TEST_CHECK(!gTextChangedCallBackCalled);

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(gTextChangedCallBackCalled);
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::TEXT), std::string("호두"), TEST_LOCATION);

  END_TEST;
}

// Negative test for the textChanged signal.
int utcDaliTextFieldTextChangedN(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldTextChangedN");
  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  field.TextChangedSignal().Connect(&TestTextChangedCallback);
  bool textChangedSignal = false;
  field.ConnectSignal(testTracker, "textChanged", CallbackFunctor(&textChangedSignal));

  gTextChangedCallBackCalled = false;
  field.SetProperty(TextField::Property::PLACEHOLDER_TEXT, "ABC"); // Setting placeholder, not TEXT
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!gTextChangedCallBackCalled);
  DALI_TEST_CHECK(!textChangedSignal);

  END_TEST;
}

// Positive test for Max Characters reached signal.
int utcDaliTextFieldMaxCharactersReachedP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldMaxCharactersReachedP");
  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  const int maxNumberOfCharacters = 1;
  field.SetProperty(TextField::Property::MAX_LENGTH, maxNumberOfCharacters);

  field.SetKeyInputFocus();

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  field.MaxLengthReachedSignal().Connect(&TestMaxLengthReachedCallback);
  bool maxLengthReachedSignal = false;
  field.ConnectSignal(testTracker, "maxLengthReached", CallbackFunctor(&maxLengthReachedSignal));

  gMaxCharactersCallBackCalled = false;

  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  DALI_TEST_CHECK(gMaxCharactersCallBackCalled);
  DALI_TEST_CHECK(maxLengthReachedSignal);

  END_TEST;
}

// Negative test for Max Characters reached signal.
int utcDaliTextFieldMaxCharactersReachedN(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldMaxCharactersReachedN");
  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  const int maxNumberOfCharacters = 3;
  field.SetProperty(TextField::Property::MAX_LENGTH, maxNumberOfCharacters);

  field.SetKeyInputFocus();

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  field.MaxLengthReachedSignal().Connect(&TestMaxLengthReachedCallback);
  bool maxLengthReachedSignal = false;
  field.ConnectSignal(testTracker, "maxLengthReached", CallbackFunctor(&maxLengthReachedSignal));

  gMaxCharactersCallBackCalled = false;

  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  DALI_TEST_CHECK(!gMaxCharactersCallBackCalled);
  DALI_TEST_CHECK(!maxLengthReachedSignal);

  application.ProcessEvent(GenerateKey("Return", "", "\r", KEY_RETURN_CODE, 0, 0, Integration::KeyEvent::DOWN, "\r", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  DALI_TEST_CHECK(!gMaxCharactersCallBackCalled);
  DALI_TEST_CHECK(!maxLengthReachedSignal);

  END_TEST;
}

// Positive test for Input Filtered signal.
int utcDaliTextFieldInputFilteredP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldInputFilteredP");
  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  Property::Map inputFilter;

  // Only digit is accepted.
  inputFilter[InputFilter::Property::ACCEPTED] = "[\\d]";

  // Set input filter to TextField.
  field.SetProperty(DevelTextField::Property::INPUT_FILTER, inputFilter);

  field.SetKeyInputFocus();

  // connect to the input filtered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextField::InputFilteredSignal(field).Connect(&TestInputFilteredCallback);
  bool inputFilteredSignal = false;
  field.ConnectSignal(testTracker, "inputFiltered", CallbackFunctor(&inputFilteredSignal));

  gInputFilteredAcceptedCallbackCalled = false;

  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  DALI_TEST_CHECK(gInputFilteredAcceptedCallbackCalled);
  DALI_TEST_CHECK(inputFilteredSignal);

  // Word is rejected.
  inputFilter[InputFilter::Property::ACCEPTED] = "";
  inputFilter[InputFilter::Property::REJECTED] = "[\\w]";

  // Set input filter to TextField.
  field.SetProperty(DevelTextField::Property::INPUT_FILTER, inputFilter);

  field.SetKeyInputFocus();

  inputFilteredSignal                  = false;
  gInputFilteredRejectedCallbackCalled = false;

  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  DALI_TEST_CHECK(gInputFilteredAcceptedCallbackCalled);
  DALI_TEST_CHECK(inputFilteredSignal);

  END_TEST;
}

// Negative test for Input Filtered signal.
int utcDaliTextFieldInputFilteredN(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldInputFilteredP");
  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  Property::Map inputFilter;

  // Only word is accepted.
  inputFilter[InputFilter::Property::ACCEPTED] = "[\\w]";

  // Set input filter to TextField.
  field.SetProperty(DevelTextField::Property::INPUT_FILTER, inputFilter);

  field.SetKeyInputFocus();

  // connect to the input filtered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextField::InputFilteredSignal(field).Connect(&TestInputFilteredCallback);
  bool inputFilteredSignal = false;
  field.ConnectSignal(testTracker, "inputFiltered", CallbackFunctor(&inputFilteredSignal));

  gInputFilteredAcceptedCallbackCalled = false;

  // Key a, d should not be filtered.
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::UP, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("d", "", "d", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("d", "", "d", KEY_D_CODE, 0, 0, Integration::KeyEvent::UP, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Backspace, Delete should not be filtered.
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("Delete", "", "Delete", Dali::DevelKey::DALI_KEY_DELETE, 0, 0, Integration::KeyEvent::DOWN, "Delete", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(!gInputFilteredAcceptedCallbackCalled);
  DALI_TEST_CHECK(!inputFilteredSignal);

  // Digit is rejected.
  inputFilter[InputFilter::Property::ACCEPTED] = "";
  inputFilter[InputFilter::Property::REJECTED] = "[\\d]";

  field.SetProperty(DevelTextField::Property::INPUT_FILTER, inputFilter);

  field.SetKeyInputFocus();

  inputFilteredSignal                  = false;
  gInputFilteredRejectedCallbackCalled = false;

  // Key a, d should not be filtered.
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::UP, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("d", "", "d", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("d", "", "d", KEY_D_CODE, 0, 0, Integration::KeyEvent::UP, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Backspace, Delete should not be filtered.
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("Delete", "", "Delete", Dali::DevelKey::DALI_KEY_DELETE, 0, 0, Integration::KeyEvent::DOWN, "Delete", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(!gInputFilteredAcceptedCallbackCalled);
  DALI_TEST_CHECK(!inputFilteredSignal);

  END_TEST;
}

int utcDaliTextFieldInputStyleChanged01(void)
{
  // The text-field emits signals when the input style changes. These changes of style are
  // detected during the relayout process (size negotiation), i.e after the cursor has been moved. Signals
  // can't be emitted during the size negotiation as the callbacks may update the UI.
  // The text-field adds an idle callback to the adaptor to emit the signals after the size negotiation.
  // The ToolkitTestApplication creates an implementation of the adaptor stub and a queue of idle callbacks.
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldInputStyleChanged01");

  // Load some fonts.

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(93u, 93u);

  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/dejavu/DejaVuSerif.ttf", DEFAULT_FONT_SIZE);
  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/dejavu/DejaVuSerif-Bold.ttf", DEFAULT_FONT_SIZE);

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  field.SetProperty(TextField::Property::ENABLE_MARKUP, true);
  field.SetProperty(TextField::Property::TEXT, "<font family='DejaVuSerif' size='18'>He<color value='green'>llo</color> <font weight='bold'>world</font> demo</font>");

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  field.InputStyleChangedSignal().Connect(&TestInputStyleChangedCallback);
  bool inputStyleChangedSignal = false;
  field.ConnectSignal(testTracker, "inputStyleChanged", CallbackFunctor(&inputStyleChangedSignal));

  application.GetScene().Add(field);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask                  = TextField::InputStyle::NONE;
  inputStyleChangedSignal          = false;

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 18.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK(gInputStyleChangedCallbackCalled);
  if(gInputStyleChangedCallbackCalled)
  {
    DALI_TEST_EQUALS(static_cast<unsigned int>(gInputStyleMask), static_cast<unsigned int>(TextField::InputStyle::FONT_FAMILY | TextField::InputStyle::POINT_SIZE), TEST_LOCATION);

    const std::string fontFamily = field.GetProperty(TextField::Property::INPUT_FONT_FAMILY).Get<std::string>();
    DALI_TEST_EQUALS(fontFamily, "DejaVuSerif", TEST_LOCATION);

    const float pointSize = field.GetProperty(TextField::Property::INPUT_POINT_SIZE).Get<float>();
    DALI_TEST_EQUALS(pointSize, 18.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  }
  DALI_TEST_CHECK(inputStyleChangedSignal);

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask                  = TextField::InputStyle::NONE;
  inputStyleChangedSignal          = false;

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 30.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK(!gInputStyleChangedCallbackCalled);
  DALI_TEST_CHECK(!inputStyleChangedSignal);

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask                  = TextField::InputStyle::NONE;
  inputStyleChangedSignal          = false;

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 43.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK(gInputStyleChangedCallbackCalled);
  if(gInputStyleChangedCallbackCalled)
  {
    DALI_TEST_EQUALS(static_cast<unsigned int>(gInputStyleMask), static_cast<unsigned int>(TextField::InputStyle::COLOR), TEST_LOCATION);

    const Vector4 color = field.GetProperty(TextField::Property::INPUT_COLOR).Get<Vector4>();
    DALI_TEST_EQUALS(color, Color::GREEN, TEST_LOCATION);
  }
  DALI_TEST_CHECK(inputStyleChangedSignal);

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask                  = TextField::InputStyle::NONE;
  inputStyleChangedSignal          = false;

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 88.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK(gInputStyleChangedCallbackCalled);
  if(gInputStyleChangedCallbackCalled)
  {
    DALI_TEST_EQUALS(static_cast<unsigned int>(gInputStyleMask), static_cast<unsigned int>(TextField::InputStyle::COLOR | TextField::InputStyle::FONT_STYLE), TEST_LOCATION);

    const Vector4 color = field.GetProperty(TextField::Property::INPUT_COLOR).Get<Vector4>();
    DALI_TEST_EQUALS(color, Color::BLACK, TEST_LOCATION);

    const Property::Map fontStyleMapGet = field.GetProperty(TextField::Property::INPUT_FONT_STYLE).Get<Property::Map>();

    Property::Map fontStyleMapSet;
    fontStyleMapSet.Insert("weight", "bold");

    DALI_TEST_EQUALS(fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION);
    DALI_TEST_EQUALS(DaliTestCheckMaps(fontStyleMapGet, fontStyleMapSet), true, TEST_LOCATION);
  }
  DALI_TEST_CHECK(inputStyleChangedSignal);

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask                  = TextField::InputStyle::NONE;
  inputStyleChangedSignal          = false;

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 115.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK(!gInputStyleChangedCallbackCalled);
  DALI_TEST_CHECK(!inputStyleChangedSignal);

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask                  = TextField::InputStyle::NONE;
  inputStyleChangedSignal          = false;

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 164.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK(gInputStyleChangedCallbackCalled);
  if(gInputStyleChangedCallbackCalled)
  {
    DALI_TEST_EQUALS(static_cast<unsigned int>(gInputStyleMask), static_cast<unsigned int>(TextField::InputStyle::FONT_STYLE), TEST_LOCATION);

    const std::string style = field.GetProperty(TextField::Property::INPUT_FONT_STYLE).Get<std::string>();
    DALI_TEST_CHECK(style.empty());
  }
  DALI_TEST_CHECK(inputStyleChangedSignal);

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask                  = TextField::InputStyle::NONE;
  inputStyleChangedSignal          = false;

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 191.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK(!gInputStyleChangedCallbackCalled);
  DALI_TEST_CHECK(!inputStyleChangedSignal);

  END_TEST;
}

int utcDaliTextFieldInputStyleChanged02(void)
{
  // The text-field emits signals when the input style changes. These changes of style are
  // detected during the relayout process (size negotiation), i.e after the cursor has been moved. Signals
  // can't be emitted during the size negotiation as the callbacks may update the UI.
  // The text-field adds an idle callback to the adaptor to emit the signals after the size negotiation.
  // The ToolkitTestApplication creates an implementation of the adaptor stub and a queue of idle callbacks.
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldInputStyleChanged02");

  // Load some fonts.

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(93u, 93u);

  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/dejavu/DejaVuSerif.ttf", DEFAULT_FONT_SIZE);
  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/dejavu/DejaVuSerif-Bold.ttf", DEFAULT_FONT_SIZE);

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  field.SetProperty(TextField::Property::ENABLE_MARKUP, true);
  field.SetProperty(TextField::Property::TEXT, "<font family='DejaVuSerif' size='18'>He<color value='blue'> l</color><color value='green'>lo</color> <font weight='bold'>world</font> demo</font>");

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  field.InputStyleChangedSignal().Connect(&TestInputStyleChangedCallback);
  bool inputStyleChangedSignal = false;
  field.ConnectSignal(testTracker, "inputStyleChanged", CallbackFunctor(&inputStyleChangedSignal));

  application.GetScene().Add(field);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask                  = TextField::InputStyle::NONE;
  inputStyleChangedSignal          = false;

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 53.0f, 25.0f, 100);
  TestGenerateTap(application, 53.0f, 25.0f, 200);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK(gInputStyleChangedCallbackCalled);
  if(gInputStyleChangedCallbackCalled)
  {
    DALI_TEST_EQUALS(static_cast<unsigned int>(gInputStyleMask),
                     static_cast<unsigned int>(TextField::InputStyle::FONT_FAMILY |
                                               TextField::InputStyle::POINT_SIZE |
                                               TextField::InputStyle::COLOR),
                     TEST_LOCATION);

    const Vector4 color = field.GetProperty(TextField::Property::INPUT_COLOR).Get<Vector4>();
    DALI_TEST_EQUALS(color, Color::GREEN, TEST_LOCATION);

    const std::string fontFamily = field.GetProperty(TextField::Property::INPUT_FONT_FAMILY).Get<std::string>();
    DALI_TEST_EQUALS(fontFamily, "DejaVuSerif", TEST_LOCATION);

    const float pointSize = field.GetProperty(TextField::Property::INPUT_POINT_SIZE).Get<float>();
    DALI_TEST_EQUALS(pointSize, 18.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  }
  DALI_TEST_CHECK(inputStyleChangedSignal);

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask                  = TextField::InputStyle::NONE;
  inputStyleChangedSignal          = false;

  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK(gInputStyleChangedCallbackCalled);
  if(gInputStyleChangedCallbackCalled)
  {
    DALI_TEST_EQUALS(static_cast<unsigned int>(gInputStyleMask),
                     static_cast<unsigned int>(TextField::InputStyle::COLOR),
                     TEST_LOCATION);

    const Vector4 color = field.GetProperty(TextField::Property::INPUT_COLOR).Get<Vector4>();
    DALI_TEST_EQUALS(color, Color::BLUE, TEST_LOCATION);
  }
  DALI_TEST_CHECK(inputStyleChangedSignal);

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask                  = TextField::InputStyle::NONE;
  inputStyleChangedSignal          = false;

  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK(!gInputStyleChangedCallbackCalled);
  DALI_TEST_CHECK(!inputStyleChangedSignal);

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask                  = TextField::InputStyle::NONE;
  inputStyleChangedSignal          = false;

  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK(gInputStyleChangedCallbackCalled);
  if(gInputStyleChangedCallbackCalled)
  {
    DALI_TEST_EQUALS(static_cast<unsigned int>(gInputStyleMask),
                     static_cast<unsigned int>(TextField::InputStyle::COLOR),
                     TEST_LOCATION);

    const Vector4 color = field.GetProperty(TextField::Property::INPUT_COLOR).Get<Vector4>();
    DALI_TEST_EQUALS(color, Color::BLACK, TEST_LOCATION);
  }
  DALI_TEST_CHECK(inputStyleChangedSignal);

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask                  = TextField::InputStyle::NONE;
  inputStyleChangedSignal          = false;

  field.SetProperty(TextField::Property::INPUT_COLOR, Color::YELLOW);

  Property::Map fontStyleMapSet;
  fontStyleMapSet.Insert("weight", "thin");
  fontStyleMapSet.Insert("width", "condensed");
  fontStyleMapSet.Insert("slant", "italic");

  field.SetProperty(TextField::Property::INPUT_FONT_STYLE, fontStyleMapSet);
  field.SetProperty(TextField::Property::INPUT_POINT_SIZE, 20.f);

  field.SetProperty(TextField::Property::INPUT_UNDERLINE, "underline");
  field.SetProperty(TextField::Property::INPUT_SHADOW, "shadow");
  field.SetProperty(TextField::Property::INPUT_EMBOSS, "emboss");
  field.SetProperty(TextField::Property::INPUT_OUTLINE, "outline");
  field.SetProperty(DevelTextField::Property::INPUT_STRIKETHROUGH, "strikethrough");

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK(!gInputStyleChangedCallbackCalled);
  DALI_TEST_CHECK(!inputStyleChangedSignal);

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 63.0f, 25.0f, 700);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK(gInputStyleChangedCallbackCalled);
  if(gInputStyleChangedCallbackCalled)
  {
    DALI_TEST_EQUALS(static_cast<unsigned int>(gInputStyleMask),
                     static_cast<unsigned int>(TextField::InputStyle::COLOR |
                                               TextField::InputStyle::POINT_SIZE |
                                               TextField::InputStyle::FONT_STYLE |
                                               TextField::InputStyle::UNDERLINE |
                                               TextField::InputStyle::SHADOW |
                                               TextField::InputStyle::EMBOSS |
                                               TextField::InputStyle::OUTLINE),
                     TEST_LOCATION);

    const Vector4 color = field.GetProperty(TextField::Property::INPUT_COLOR).Get<Vector4>();
    DALI_TEST_EQUALS(color, Color::BLACK, TEST_LOCATION);
  }
  DALI_TEST_CHECK(inputStyleChangedSignal);

  END_TEST;
}

int utcDaliTextFieldEvent01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldEvent01");

  // Creates a tap event. After creating a tap event the text field should
  // have the focus and add text with key events should be possible.

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Add a key event but as the text field has not the focus it should do nothing.
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::TEXT), std::string(""), TEST_LOCATION);

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 150.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Pressing delete key should be fine even if there is no text in TextField.
  application.ProcessEvent(GenerateKey("Delete", "", "Delete", Dali::DevelKey::DALI_KEY_DELETE, 0, 0, Integration::KeyEvent::DOWN, "Delete", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  // Render and notify
  application.SendNotification();
  application.Render();

  // Now the text field has the focus, so it can handle the key events.
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::TEXT), std::string("aa"), TEST_LOCATION);

  // Create a second text field and send key events to it.
  TextField field2 = TextField::New();

  field2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  field2.SetProperty(Actor::Property::SIZE, Vector2(100.f, 100.f));
  field2.SetProperty(Actor::Property::POSITION, Vector2(100.0f, 100.0f));

  application.GetScene().Add(field2);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Create a tap event on the second text field.
  TestGenerateTap(application, 150.0f, 125.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // The second text field has the focus. It should handle the key events.
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check the text has been added to the second text field.
  DALI_TEST_EQUALS(field2.GetProperty<std::string>(TextField::Property::TEXT), std::string("aa"), TEST_LOCATION);

  END_TEST;
}

int utcDaliTextFieldEvent02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldEvent02");

  // Checks if the right number of actors are created.

  TextField field = TextField::New();
  field.SetProperty(TextField::Property::POINT_SIZE, 10.f);
  DALI_TEST_CHECK(field);
  LoadMarkerImages(application, field);

  application.GetScene().Add(field);

  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check there are the expected number of children ( stencil ).
  DALI_TEST_EQUALS(field.GetChildCount(), 1u, TEST_LOCATION);

  Actor stencil = field.GetChildAt(0u);
  DALI_TEST_EQUALS(stencil.GetChildCount(), 0u, TEST_LOCATION);

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 150.0f, 25.0f, 300);

  // Render and notify
  application.SendNotification();
  application.Render();

  Actor layer = field.GetChildAt(2u);
  DALI_TEST_EQUALS(layer.GetChildCount(), 1u, TEST_LOCATION); // The cursor.
  DALI_TEST_EQUALS(stencil.GetChildCount(), 0u, TEST_LOCATION);

  // Now the text field has the focus, so it can handle the key events.
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  // Checks the cursor and the renderer have been created.
  DALI_TEST_EQUALS(layer.GetChildCount(), 1u, TEST_LOCATION);   // The cursor.
  DALI_TEST_EQUALS(stencil.GetChildCount(), 2u, TEST_LOCATION); // The renderer, clipped cursor

  Control cursor = Control::DownCast(layer.GetChildAt(0u));
  DALI_TEST_CHECK(cursor);

  // The offscreen root actor has a container with all the actors which contain the text renderers.
  Actor container = stencil.GetChildAt(0u);
  for(unsigned int index = 0; index < container.GetChildCount(); ++index)
  {
    Renderer renderer = container.GetChildAt(index).GetRendererAt(0u);
    DALI_TEST_CHECK(renderer);
  }

  // Move the cursor and check the position changes.
  Vector3 position1 = cursor.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector3 position2 = cursor.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
  DALI_TEST_CHECK(position2.x < position1.x);

  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector3 position3 = cursor.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
  DALI_TEST_EQUALS(position1, position3, TEST_LOCATION); // Should be in the same position1.

  // Move the cursor to the first position.
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector3 position4 = cursor.GetCurrentProperty<Vector3>(Actor::Property::POSITION);

  // Send some taps and check the cursor positions.

  // Try to tap at the beginning.
  TestGenerateTap(application, 1.0f, 25.0f, 900);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Cursor position should be the same than position1.
  Vector3 position5 = cursor.GetCurrentProperty<Vector3>(Actor::Property::POSITION);

  DALI_TEST_EQUALS(position4, position5, TEST_LOCATION); // Should be in the same position2.

  // Tap away from the start position.
  TestGenerateTap(application, 16.0f, 25.0f, 1500);

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector3 position6 = cursor.GetCurrentProperty<Vector3>(Actor::Property::POSITION);

  DALI_TEST_CHECK(position6.x > position5.x);

  // Remove all the text.
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  field.SetProperty(TextField::Property::TEXT, "");

  // Render and notify
  application.SendNotification();
  application.Render();

  // Cursor position should be the same than position2.
  Vector3 position7 = cursor.GetCurrentProperty<Vector3>(Actor::Property::POSITION);

  DALI_TEST_EQUALS(position4, position7, TEST_LOCATION); // Should be in the same position2.

  // Should not be a renderer, there is only a clipped cursor.
  DALI_TEST_EQUALS(stencil.GetChildCount(), 1u, TEST_LOCATION);

  // Chanege exceed policy (EXCEED_POLICY_ORIGINAL doesn't use stencil )
  field.SetProperty(TextField::Property::TEXT, "This is a long text for the size of the text-field.");
  field.SetProperty(TextField::Property::EXCEED_POLICY, Dali::Toolkit::TextField::EXCEED_POLICY_ORIGINAL);

  application.SendNotification();
  application.Render();

  // There are renderer and decorator layer
  DALI_TEST_EQUALS(field.GetChildCount(), 2u, TEST_LOCATION);

  END_TEST;
}

int utcDaliTextFieldEvent03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldEvent03");

  // Checks if the highlight actor is created.

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  field.SetProperty(TextField::Property::TEXT, "This is a long text for the size of the text-field.");
  field.SetProperty(TextField::Property::POINT_SIZE, 10.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(30.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  LoadMarkerImages(application, field);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap first to get the focus.
  TestGenerateTap(application, 3.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Double tap to select a word.
  TestGenerateTap(application, 3.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // The offscreen root actor should have two actors: the renderer and the highlight actor.
  Actor stencil = field.GetChildAt(0u);

  // The highlight actor is drawn first, so is the first actor in the list
  Renderer highlight = stencil.GetChildAt(0u).GetRendererAt(0u);
  DALI_TEST_CHECK(highlight);

  // The offscreen root actor has a container with all the actors which contain the text renderers.
  Actor container = stencil.GetChildAt(1u);
  for(unsigned int index = 0; index < container.GetChildCount(); ++index)
  {
    Renderer renderer = container.GetChildAt(index).GetRendererAt(0u);
    DALI_TEST_CHECK(renderer);
  }

  END_TEST;
}

int utcDaliTextFieldEvent04(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldEvent04");

  // Checks if the highlight actor is created.

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);
  application.GetScene().Add(field);
  LoadMarkerImages(application, field);
  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetProperty(TextField::Property::TEXT, "This is a long text for the size of the text-field.");
  field.SetProperty(TextField::Property::POINT_SIZE, 10.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  // Render and notify
  application.SendNotification();
  application.Render();

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 150.0f, 25.0f);
  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap first to get the focus.
  TestGenerateTap(application, 1.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Double tap to select a word.
  TestGenerateTap(application, 1.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap grab handle
  TestGenerateTap(application, 0.0f, 40.0f);
  END_TEST;
}

int utcDaliTextFieldEvent05(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldEvent05");

  // Checks dragging of cursor/grab handle

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);
  application.GetScene().Add(field);
  LoadMarkerImages(application, field);
  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetProperty(TextField::Property::TEXT, "This is a long text for the size of the text-field.");
  field.SetProperty(TextField::Property::POINT_SIZE, 10.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  // Render and notify
  application.SendNotification();
  application.Render();

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 150.0f, 25.0f);
  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap first to get the focus.
  TestGenerateTap(application, 1.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Double tap to select a word.
  TestGenerateTap(application, 1.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  Actor stencil = field.GetChildAt(1u);
  END_TEST;
}

int utcDaliTextFieldEvent06(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldEvent06");

  // Checks Longpress when in edit mode

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);
  application.GetScene().Add(field);
  LoadMarkerImages(application, field);
  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetProperty(TextField::Property::TEXT, "Thisisalongtextforthesizeofthetextfield.");
  field.SetProperty(TextField::Property::POINT_SIZE, 10.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  // Render and notify
  application.SendNotification();
  application.Render();

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 150.0f, 25.0f);
  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap first to get the focus.
  TestGenerateTap(application, 1.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Long Press
  TestGenerateLongPress(application, 1.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  END_TEST;
}

int utcDaliTextFieldEvent07(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldEvent07");

  // Checks Longpress to start edit mode

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);
  application.GetScene().Add(field);
  LoadMarkerImages(application, field);
  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetProperty(TextField::Property::TEXT, "Thisisalongtextforthesizeofthetextfield.");
  field.SetProperty(TextField::Property::POINT_SIZE, 10.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  Property::Map propertyMap;
  propertyMap["PANEL_LAYOUT"] = InputMethod::PanelLayout::PASSWORD;
  field.SetProperty(TextField::Property::INPUT_METHOD_SETTINGS, propertyMap);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  // Render and notify
  application.SendNotification();
  application.Render();

  // Long Press
  TestGenerateLongPress(application, 1.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  END_TEST;
}

int utcDaliTextFieldEvent08(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldEvent08");

  Dali::Clipboard clipboard = Clipboard::Get();
  clipboard.SetItem("testTextFieldEvent");

  // Checks Longpress when only place holder text

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);
  application.GetScene().Add(field);
  LoadMarkerImages(application, field);
  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetProperty(TextField::Property::PLACEHOLDER_TEXT, "Setting Placeholder Text");
  field.SetProperty(TextField::Property::POINT_SIZE, 10.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  // Render and notify
  application.SendNotification();
  application.Render();

  // Long Press
  TestGenerateLongPress(application, 1.0f, 25.0f, 20);

  // Render and notify
  application.SendNotification();
  application.Render();

  Wait(application, 500);

  TestEndLongPress(application, 1.0f, 25.0f, 520);

  // Long Press
  TestGenerateLongPress(application, 1.0f, 25.0f, 600);

  // Render and notify
  application.Render();

  Wait(application, 500);

  Integration::Scene stage = application.GetScene();
  Layer              layer = stage.GetRootLayer();
  Actor              actor = layer.FindChildByName("optionPaste");

  if(actor)
  {
    Vector3 worldPosition = actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION);
    Vector2 halfStageSize = stage.GetSize() / 2.0f;
    Vector2 position(worldPosition.x + halfStageSize.width, worldPosition.y + halfStageSize.height);

    Dali::Integration::TouchEvent event;
    event = Dali::Integration::TouchEvent();
    event.AddPoint(GetPointDownInside(position));
    application.ProcessEvent(event);

    event = Dali::Integration::TouchEvent();
    event.AddPoint(GetPointUpInside(position));
    application.ProcessEvent(event);
  }
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextEditor::Property::TEXT), std::string("testTextFieldEvent"), TEST_LOCATION);

  END_TEST;
}

int utcDaliTextFieldEvent09(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldEvent09");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);
  application.GetScene().Add(field);
  LoadMarkerImages(application, field);
  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetProperty(TextField::Property::TEXT, "Hello");
  field.SetProperty(TextField::Property::POINT_SIZE, 10.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 150.0f, 25.0f);
  application.SendNotification();
  application.Render();

  Property::Map map;
  map[HiddenInput::Property::MODE] = HiddenInput::Mode::HIDE_NONE;
  field.SetProperty(TextField::Property::HIDDEN_INPUT_SETTINGS, map);
  application.ProcessEvent(GenerateKey("d", "", "d", 0, 0, 0, Integration::KeyEvent::DOWN, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  map[HiddenInput::Property::MODE]                 = HiddenInput::Mode::HIDE_ALL;
  map[HiddenInput::Property::SUBSTITUTE_CHARACTER] = 0x23;
  field.SetProperty(TextField::Property::HIDDEN_INPUT_SETTINGS, map);
  application.ProcessEvent(GenerateKey("d", "", "d", 0, 0, 0, Integration::KeyEvent::DOWN, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  map[HiddenInput::Property::MODE]             = HiddenInput::Mode::HIDE_COUNT;
  map[HiddenInput::Property::SUBSTITUTE_COUNT] = 2;
  field.SetProperty(TextField::Property::HIDDEN_INPUT_SETTINGS, map);
  for(unsigned int index = 0u; index < 5u; ++index)
  {
    application.ProcessEvent(GenerateKey("d", "", "d", 0, 0, 0, Integration::KeyEvent::DOWN, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
    application.SendNotification();
    application.Render();
  }

  map[HiddenInput::Property::MODE]             = HiddenInput::Mode::SHOW_COUNT;
  map[HiddenInput::Property::SUBSTITUTE_COUNT] = 2;
  field.SetProperty(TextField::Property::HIDDEN_INPUT_SETTINGS, map);
  for(unsigned int index = 0u; index < 5u; ++index)
  {
    application.ProcessEvent(GenerateKey("d", "", "d", 0, 0, 0, Integration::KeyEvent::DOWN, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
    application.SendNotification();
    application.Render();
  }

  map[HiddenInput::Property::MODE]                         = HiddenInput::Mode::SHOW_LAST_CHARACTER;
  map[HiddenInput::Property::SHOW_LAST_CHARACTER_DURATION] = 0;
  field.SetProperty(TextField::Property::HIDDEN_INPUT_SETTINGS, map);
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();
  application.ProcessEvent(GenerateKey("d", "", "d", 0, 0, 0, Integration::KeyEvent::DOWN, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  map[HiddenInput::Property::SHOW_LAST_CHARACTER_DURATION] = 100;
  field.SetProperty(TextField::Property::HIDDEN_INPUT_SETTINGS, map);
  application.ProcessEvent(GenerateKey("d", "", "d", 0, 0, 0, Integration::KeyEvent::DOWN, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  Property::Map mapGet;
  mapGet = field.GetProperty<Property::Map>(TextField::Property::HIDDEN_INPUT_SETTINGS);
  DALI_TEST_EQUALS(map.Count(), mapGet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(map, mapGet), true, TEST_LOCATION);
  END_TEST;
}

int utcDaliTextFieldStyleWhilstSelected(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldStyleWhilstSelected");

  // Change font and styles whilst text is selected whilst word selected

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);
  application.GetScene().Add(field);
  LoadMarkerImages(application, field);
  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetProperty(TextField::Property::TEXT, "This is a long text for the size of the text-field.");
  field.SetProperty(TextField::Property::POINT_SIZE, 10.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  // Render and notify
  application.SendNotification();
  application.Render();

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 150.0f, 25.0f);
  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap first to get the focus.
  TestGenerateTap(application, 1.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Double tap to select a word.
  TestGenerateTap(application, 1.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetProperty(TextField::Property::INPUT_FONT_FAMILY, "Setting input font family");
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::INPUT_FONT_FAMILY), "Setting input font family", TEST_LOCATION);

  Property::Map fontStyleMapSet;
  Property::Map fontStyleMapGet;

  fontStyleMapSet.Insert("weight", "bold");
  fontStyleMapSet.Insert("slant", "italic");
  field.SetProperty(TextField::Property::INPUT_FONT_STYLE, fontStyleMapSet);

  fontStyleMapGet = field.GetProperty<Property::Map>(TextField::Property::INPUT_FONT_STYLE);
  DALI_TEST_EQUALS(fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(fontStyleMapGet, fontStyleMapSet), true, TEST_LOCATION);

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert("width", "expanded");
  fontStyleMapSet.Insert("slant", "italic");
  field.SetProperty(TextField::Property::INPUT_FONT_STYLE, fontStyleMapSet);

  fontStyleMapGet = field.GetProperty<Property::Map>(TextField::Property::INPUT_FONT_STYLE);
  DALI_TEST_EQUALS(fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(fontStyleMapGet, fontStyleMapSet), true, TEST_LOCATION);

  field.SetProperty(TextField::Property::INPUT_POINT_SIZE, 12.f);
  DALI_TEST_EQUALS(field.GetProperty<float>(TextField::Property::INPUT_POINT_SIZE), 12.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  field.SetProperty(TextField::Property::TEXT_COLOR, Color::RED);
  DALI_TEST_EQUALS(field.GetProperty<Vector4>(TextField::Property::TEXT_COLOR), Color::RED, TEST_LOCATION);

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert("weight", "bold");
  fontStyleMapSet.Insert("slant", "italic");

  field.SetProperty(TextField::Property::FONT_STYLE, fontStyleMapSet);

  fontStyleMapGet = field.GetProperty<Property::Map>(TextField::Property::FONT_STYLE);
  DALI_TEST_EQUALS(fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(fontStyleMapGet, fontStyleMapSet), true, TEST_LOCATION);

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert("width", "expanded");

  field.SetProperty(TextField::Property::FONT_STYLE, fontStyleMapSet);

  fontStyleMapGet = field.GetProperty<Property::Map>(TextField::Property::FONT_STYLE);
  DALI_TEST_EQUALS(fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(fontStyleMapGet, fontStyleMapSet), true, TEST_LOCATION);

  // Press Escape to increase coverage
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_ESCAPE, 0, 0, Integration::KeyEvent::UP, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(!field.HasKeyInputFocus());

  END_TEST;
}

int utcDaliTextFieldEscKeyLoseFocus(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldEscKeyLoseFocus");

  // Creates a tap event. After creating a tap event the text field should
  // have the focus and add text with key events should be possible.

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Add a key event but as the text field has not the focus it should do nothing.
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::UP, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::TEXT), std::string(""), TEST_LOCATION);

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 150.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Now the text field has the focus, so it can handle the key events.
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::UP, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("d", "", "d", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("d", "", "d", KEY_D_CODE, 0, 0, Integration::KeyEvent::UP, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::TEXT), std::string("ad"), TEST_LOCATION);

  // Generate a Esc key event. The text field should lose the focus.
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_ESCAPE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_ESCAPE, 0, 0, Integration::KeyEvent::UP, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(false, field.HasKeyInputFocus(), TEST_LOCATION);

  // No more text should be introduced
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::UP, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::TEXT), std::string("ad"), TEST_LOCATION);

  END_TEST;
}

int utcDaliTextFieldSomeSpecialKeys(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldSomeSpecialKeys");

  // Checks some special keys when the text is selected.

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);
  application.GetScene().Add(field);
  LoadMarkerImages(application, field);
  // Render and notify
  application.SendNotification();
  application.Render();

  const std::string longText("This is a long text for the size of the text-field.");

  field.SetProperty(TextField::Property::TEXT, longText);
  field.SetProperty(TextField::Property::POINT_SIZE, 10.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  // Render and notify
  application.SendNotification();
  application.Render();

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 150.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap first to get the focus.
  TestGenerateTap(application, 1.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Double tap to select a word.
  TestGenerateTap(application, 1.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Generate a Esc key event. The text field should lose the focus.
  application.ProcessEvent(GenerateKey("XF86PowerOff", "", "XF86PowerOff", DALI_KEY_POWER, 0, 0, Integration::KeyEvent::DOWN, "XF86PowerOff", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("XF86PowerOff", "", "XF86PowerOff", DALI_KEY_POWER, 0, 0, Integration::KeyEvent::UP, "XF86PowerOff", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  // Generate a Esc key event. The text field should lose the focus.
  application.ProcessEvent(GenerateKey("XF86Menu", "", "XF86Menu", DALI_KEY_MENU, 0, 0, Integration::KeyEvent::DOWN, "XF86Menu", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("XF86Menu", "", "XF86Menu", DALI_KEY_MENU, 0, 0, Integration::KeyEvent::UP, "XF86Menu", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  // Generate a Esc key event. The text field should lose the focus.
  application.ProcessEvent(GenerateKey("XF86Home", "", "XF86Home", DALI_KEY_HOME, 0, 0, Integration::KeyEvent::DOWN, "XF86Home", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("XF86Home", "", "XF86Home", DALI_KEY_HOME, 0, 0, Integration::KeyEvent::UP, "XF86Home", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  // The text shouldn't be deleted.
  DALI_TEST_EQUALS(field.GetProperty<std::string>(TextField::Property::TEXT), longText, TEST_LOCATION);

  END_TEST;
}

int utcDaliTextFieldSizeUpdate(void)
{
  ToolkitTestApplication application;
  tet_infoline("utcDaliTextFieldSizeUpdate");

  // Checks some special keys when the text is selected.
  TextField field = TextField::New();
  DALI_TEST_CHECK(field);
  application.GetScene().Add(field);

  float       previousHeight = 0.0f;
  float       currentHeight  = 0.0f;
  const float fieldWidth     = 1920.0f;

  // "ㅁ" is bigger then "ኢ"
  field.SetProperty(Actor::Property::SIZE, Vector2(fieldWidth, 10.0f));
  field.SetResizePolicy(ResizePolicy::FIXED, Dimension::WIDTH);
  field.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);

  field.SetProperty(TextField::Property::TEXT, "ኢ");
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  field.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  KeyboardFocusManager::Get().SetCurrentFocusActor(field);

  application.SendNotification();
  application.Render();

  previousHeight = field.GetHeightForWidth(fieldWidth);
  DALI_TEST_EQUALS(previousHeight, field.GetProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);

  // Add  another script characters ( glyph height is defferent )
  application.ProcessEvent(GenerateKey("ㅁ", "", "ㅁ", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "ㅁ", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("ㅁ", "", "ㅁ", KEY_A_CODE, 0, 0, Integration::KeyEvent::UP, "ㅁ", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  application.SendNotification();
  application.Render();

  currentHeight = field.GetHeightForWidth(fieldWidth);
  DALI_TEST_EQUALS(currentHeight, field.GetProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS((previousHeight < currentHeight), true, TEST_LOCATION);

  END_TEST;
}

int utcDaliTextFieldExtremlyLargePointSize(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldExtremlyLargePointSize");

  TextField field = TextField::New();

  field.SetProperty(TextField::Property::TEXT, "Text");
  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(field);

  try
  {
    field.SetProperty(TextField::Property::POINT_SIZE, 160.0f);
    application.SendNotification();
    DALI_TEST_CHECK(field);
  }
  catch(...)
  {
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliTextFieldDefaultFontStylePropertyCoverage(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextFieldFontStylePorpertyCoverage");
  TextField field = TextField::New();
  DALI_TEST_CHECK(field);
  application.GetScene().Add(field);

  Property::Map fontStyleMapGet;

  fontStyleMapGet = field.GetProperty<Property::Map>(TextField::Property::FONT_STYLE);

  Property::Value* weightValue = NULL;
  Property::Value* widthValue  = NULL;
  Property::Value* slantValue  = NULL;
  weightValue                  = fontStyleMapGet.Find("weight");
  widthValue                   = fontStyleMapGet.Find("width");
  slantValue                   = fontStyleMapGet.Find("slant");
  DALI_TEST_CHECK(!weightValue);
  DALI_TEST_CHECK(!widthValue);
  DALI_TEST_CHECK(!slantValue);

  END_TEST;
}

int UtcDaliTextFieldSettingPlaceholder(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextFieldSettingPlaceholder");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);
  application.GetScene().Add(field);

  // Check the placeholder property with pixel size
  Property::Map placeholderPixelSizeMapSet;
  Property::Map placeholderPixelSizeMapGet;
  Property::Map placeholderFontstyleMap;
  placeholderPixelSizeMapSet[Text::PlaceHolder::Property::TEXT]         = "Setting Placeholder Text";
  placeholderPixelSizeMapSet[Text::PlaceHolder::Property::TEXT_FOCUSED] = "Setting Placeholder Text Focused";
  placeholderPixelSizeMapSet[Text::PlaceHolder::Property::COLOR]        = Color::BLUE;
  placeholderPixelSizeMapSet[Text::PlaceHolder::Property::FONT_FAMILY]  = "Arial";
  placeholderPixelSizeMapSet[Text::PlaceHolder::Property::PIXEL_SIZE]   = 15.0f;
  placeholderPixelSizeMapSet[Text::PlaceHolder::Property::ELLIPSIS]     = true;

  placeholderFontstyleMap.Insert("weight", "bold");
  placeholderPixelSizeMapSet[Text::PlaceHolder::Property::FONT_STYLE] = placeholderFontstyleMap;
  field.SetProperty(TextField::Property::PLACEHOLDER, placeholderPixelSizeMapSet);

  placeholderPixelSizeMapGet = field.GetProperty<Property::Map>(TextField::Property::PLACEHOLDER);
  DALI_TEST_EQUALS(placeholderPixelSizeMapGet.Count(), placeholderPixelSizeMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(placeholderPixelSizeMapGet, placeholderPixelSizeMapSet), true, TEST_LOCATION);

  // Check the placeholder property with point size
  Property::Map placeholderMapSet;
  Property::Map placeholderMapGet;
  placeholderMapSet[Text::PlaceHolder::Property::TEXT]         = "Setting Placeholder Text";
  placeholderMapSet[Text::PlaceHolder::Property::TEXT_FOCUSED] = "Setting Placeholder Text Focused";
  placeholderMapSet[Text::PlaceHolder::Property::COLOR]        = Color::RED;
  placeholderMapSet[Text::PlaceHolder::Property::FONT_FAMILY]  = "Arial";
  placeholderMapSet[Text::PlaceHolder::Property::POINT_SIZE]   = 12.0f;
  placeholderMapSet[Text::PlaceHolder::Property::ELLIPSIS]     = false;

  // Check the placeholder font style property
  placeholderFontstyleMap.Clear();

  placeholderFontstyleMap.Insert("weight", "bold");
  placeholderFontstyleMap.Insert("width", "condensed");
  placeholderFontstyleMap.Insert("slant", "italic");
  placeholderMapSet[Text::PlaceHolder::Property::FONT_STYLE] = placeholderFontstyleMap;
  field.SetProperty(TextField::Property::PLACEHOLDER, placeholderMapSet);

  placeholderMapGet = field.GetProperty<Property::Map>(TextField::Property::PLACEHOLDER);
  DALI_TEST_EQUALS(placeholderMapGet.Count(), placeholderMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(placeholderMapGet, placeholderMapSet), true, TEST_LOCATION);

  // Reset font style.
  placeholderFontstyleMap.Clear();
  placeholderFontstyleMap.Insert("weight", "normal");
  placeholderFontstyleMap.Insert("slant", "oblique");
  placeholderMapSet[Text::PlaceHolder::Property::FONT_STYLE] = placeholderFontstyleMap;
  field.SetProperty(TextField::Property::PLACEHOLDER, placeholderMapSet);

  placeholderMapGet = field.GetProperty<Property::Map>(TextField::Property::PLACEHOLDER);
  DALI_TEST_EQUALS(placeholderMapGet.Count(), placeholderMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(placeholderMapGet, placeholderMapSet), true, TEST_LOCATION);

  placeholderFontstyleMap.Clear();
  placeholderFontstyleMap.Insert("slant", "roman");
  placeholderMapSet[Text::PlaceHolder::Property::FONT_STYLE] = placeholderFontstyleMap;
  field.SetProperty(TextField::Property::PLACEHOLDER, placeholderMapSet);

  placeholderMapGet = field.GetProperty<Property::Map>(TextField::Property::PLACEHOLDER);

  placeholderFontstyleMap.Clear();
  placeholderMapSet[Text::PlaceHolder::Property::FONT_STYLE] = placeholderFontstyleMap;

  field.SetProperty(TextField::Property::PLACEHOLDER, placeholderMapSet);
  placeholderMapGet = field.GetProperty<Property::Map>(TextField::Property::PLACEHOLDER);
  DALI_TEST_EQUALS(placeholderMapGet.Count(), placeholderMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(placeholderMapGet, placeholderMapSet), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFieldPlaceholderCoverage(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextFieldPlaceholderCoverage");

  // mPlaceholderFont is created only once, so create a new control for coverage.
  TextField fieldForCoverage = TextField::New();
  DALI_TEST_CHECK(fieldForCoverage);
  application.GetScene().Add(fieldForCoverage);

  // for SetPlaceholderFontFamily() coverage.
  Property::Map fontFamilyMap;
  fontFamilyMap[Text::PlaceHolder::Property::FONT_FAMILY] = "Arial";
  fieldForCoverage.SetProperty(TextField::Property::PLACEHOLDER, fontFamilyMap);

  // mPlaceholderFont is created only once, so create a new control for coverage.
  fieldForCoverage = TextField::New();
  DALI_TEST_CHECK(fieldForCoverage);
  application.GetScene().Add(fieldForCoverage);

  // for SetPlaceholderTextFontSize coverage.
  Property::Map fontSizeMap;
  fontSizeMap[Text::PlaceHolder::Property::PIXEL_SIZE] = 15.0f;
  fieldForCoverage.SetProperty(TextField::Property::PLACEHOLDER, fontSizeMap);

  // mPlaceholderFont is created only once, so create a new control for coverage.
  fieldForCoverage = TextField::New();
  DALI_TEST_CHECK(fieldForCoverage);
  application.GetScene().Add(fieldForCoverage);

  // for SetPlaceholderTextFontWeight coverage.
  Property::Map fontStyleWeightMap;
  Property::Map fontStyleWeightPropertyMap;
  fontStyleWeightPropertyMap.Insert("weight", "bold");
  fontStyleWeightMap[Text::PlaceHolder::Property::FONT_STYLE] = fontStyleWeightPropertyMap;
  fieldForCoverage.SetProperty(TextField::Property::PLACEHOLDER, fontStyleWeightMap);

  // mPlaceholderFont is created only once, so create a new control for coverage.
  fieldForCoverage = TextField::New();
  DALI_TEST_CHECK(fieldForCoverage);
  application.GetScene().Add(fieldForCoverage);

  // for SetPlaceholderTextFontWidth coverage.
  Property::Map fontStyleWidthMap;
  Property::Map fontStyleWidthPropertyMap;
  fontStyleWidthPropertyMap.Insert("width", "expanded");
  fontStyleWidthMap[Text::PlaceHolder::Property::FONT_STYLE] = fontStyleWidthPropertyMap;
  fieldForCoverage.SetProperty(TextField::Property::PLACEHOLDER, fontStyleWidthMap);

  // mPlaceholderFont is created only once, so create a new control for coverage.
  fieldForCoverage = TextField::New();
  DALI_TEST_CHECK(fieldForCoverage);
  application.GetScene().Add(fieldForCoverage);

  // for SetPlaceholderTextFontSlant coverage.
  Property::Map fontStyleSlantMap;
  Property::Map fontStyleSlantPropertyMap;
  fontStyleSlantPropertyMap.Insert("slant", "italic");
  fontStyleSlantMap[Text::PlaceHolder::Property::FONT_STYLE] = fontStyleSlantPropertyMap;
  fieldForCoverage.SetProperty(TextField::Property::PLACEHOLDER, fontStyleSlantMap);

  END_TEST;
}

int UtcDaliTextFieldSetPaddingProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextFieldSetPaddingProperty\n");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);
  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(field);

  application.SendNotification();
  application.Render();

  Vector3 originalSize = field.GetNaturalSize();

  field.SetProperty(Toolkit::Control::Property::PADDING, Extents(10, 10, 10, 10));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(field.GetProperty<Extents>(Toolkit::Control::Property::PADDING), Extents(10, 10, 10, 10), TEST_LOCATION);

  Vector3 paddingAddedSize = field.GetNaturalSize();

  DALI_TEST_EQUALS(originalSize.width + 10 + 10, paddingAddedSize.width, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  DALI_TEST_EQUALS(originalSize.height + 10 + 10, paddingAddedSize.height, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFieldEnableShiftSelectionProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextFieldEnableShiftSelectionProperty");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);
  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(field);

  application.SendNotification();
  application.Render();

  // The default value of ENABLE_SHIFT_SELECTION is 'true'.
  DALI_TEST_EQUALS(field.GetProperty<bool>(DevelTextField::Property::ENABLE_SHIFT_SELECTION), true, TEST_LOCATION);

  // Check the enable shift selection property
  field.SetProperty(DevelTextField::Property::ENABLE_SHIFT_SELECTION, false);
  DALI_TEST_EQUALS(field.GetProperty<bool>(DevelTextField::Property::ENABLE_SHIFT_SELECTION), false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliTextFieldEnableGrabHandleProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextFieldEnableGrabHandleProperty");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);
  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(field);

  application.SendNotification();
  application.Render();

  // The default value of ENABLE_GRAB_HANDLE is 'true'.
  DALI_TEST_EQUALS(field.GetProperty<bool>(DevelTextField::Property::ENABLE_GRAB_HANDLE), true, TEST_LOCATION);

  // Check the enable grab handle property
  field.SetProperty(DevelTextField::Property::ENABLE_GRAB_HANDLE, false);
  DALI_TEST_EQUALS(field.GetProperty<bool>(DevelTextField::Property::ENABLE_GRAB_HANDLE), false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliTextFieldMatchSystemLanguageDirectionProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextFieldMatchSystemLanguageDirectionProperty");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);
  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(field);

  application.SendNotification();
  application.Render();

  // The default value of MATCH_SYSTEM_LANGUAGE_DIRECTION is 'true'.
  DALI_TEST_EQUALS(field.GetProperty<bool>(DevelTextField::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION), true, TEST_LOCATION);

  // Check the match system language direction property
  field.SetProperty(DevelTextField::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION, false);
  DALI_TEST_EQUALS(field.GetProperty<bool>(DevelTextField::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION), false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int utcDaliTextFieldLayoutDirectionCoverage(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldLayoutDirectionCoverage");

  // Creates a tap event. After creating a tap event the text field should
  // have the focus and add text with key events should be possible.
  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Render and notify
  application.SendNotification();
  application.Render();

  // init direction for coverage
  // Set horizontal alignment END
  field.SetProperty(TextField::Property::HORIZONTAL_ALIGNMENT, "END");

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 150.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Set MATCH_SYSTEM_LANGUAGE_DIRECTION to true to use the layout direction.
  field.SetProperty(DevelTextField::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION, true);
  field.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT);

  // Set horizontal alignment BEGIN
  field.SetProperty(TextField::Property::HORIZONTAL_ALIGNMENT, "BEGIN");

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 150.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Set horizontal alignment CENTER
  field.SetProperty(TextField::Property::HORIZONTAL_ALIGNMENT, "CENTER");

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 150.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Set horizontal alignment END
  field.SetProperty(TextField::Property::HORIZONTAL_ALIGNMENT, "END");

  // Create a tap event to touch the text field.
  TestGenerateTap(application, 150.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Generate a Esc key event. The text field should lose the focus.
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_ESCAPE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_ESCAPE, 0, 0, Integration::KeyEvent::UP, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(false, field.HasKeyInputFocus(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFieldGetInputMethodContext(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextFieldGetInputMethodContext");

  TextField field = TextField::New();
  DALI_TEST_CHECK(DevelTextField::GetInputMethodContext(field));

  END_TEST;
}

int UtcDaliTextFieldSelectWholeText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldSelectWholeText ");

  TextField textField = TextField::New();

  application.GetScene().Add(textField);

  textField.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  textField.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  textField.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(1u, textField.GetChildCount(), TEST_LOCATION);

  DevelTextField::SelectWholeText(textField);

  application.SendNotification();
  application.Render();

  // Nothing should have been selected. The number of children is still 1
  DALI_TEST_EQUALS(1u, textField.GetChildCount(), TEST_LOCATION);

  textField.SetProperty(TextField::Property::TEXT, "Hello world");

  application.SendNotification();
  application.Render();

  DevelTextField::SelectWholeText(textField);

  application.SendNotification();
  application.Render();

  // Even if resize, selection should remain.
  textField.SetProperty(Actor::Property::SIZE, Vector2(150.f, 50.f));

  application.SendNotification();
  application.Render();

  // Should be 2 children, the stencil and the layer
  DALI_TEST_EQUALS(2u, textField.GetChildCount(), TEST_LOCATION);

  // The offscreen root actor should have two actors: the renderer and the highlight actor.
  Actor stencil = textField.GetChildAt(0u);

  // The highlight actor is drawn first, so is the first actor in the list
  Renderer highlight = stencil.GetChildAt(0u).GetRendererAt(0u);
  DALI_TEST_CHECK(highlight);

  END_TEST;
}

int UtcDaliTextFieldSelectText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldSelectText ");

  TextField textField = TextField::New();

  application.GetScene().Add(textField);

  textField.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  textField.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  textField.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  application.SendNotification();
  application.Render();

  DevelTextField::SelectText(textField, 0, 5);

  application.SendNotification();
  application.Render();

  // Nothing is selected
  std::string selectedText = textField.GetProperty(DevelTextField::Property::SELECTED_TEXT).Get<std::string>();
  DALI_TEST_EQUALS("", selectedText, TEST_LOCATION);

  textField.SetProperty(TextField::Property::TEXT, "Hello world");

  application.SendNotification();
  application.Render();

  // Hello is selected
  DevelTextField::SelectText(textField, 0, 5);

  application.SendNotification();
  application.Render();

  selectedText = textField.GetProperty(DevelTextField::Property::SELECTED_TEXT).Get<std::string>();
  DALI_TEST_EQUALS("Hello", selectedText, TEST_LOCATION);

  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_START).Get<int>(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_END).Get<int>(), 5, TEST_LOCATION);

  // world is selected
  DevelTextField::SelectText(textField, 6, 11);

  application.SendNotification();
  application.Render();

  selectedText = textField.GetProperty(DevelTextField::Property::SELECTED_TEXT).Get<std::string>();
  DALI_TEST_EQUALS("world", selectedText, TEST_LOCATION);

  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_START).Get<int>(), 6, TEST_LOCATION);
  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_END).Get<int>(), 11, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFieldSelectNone(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldSelectWholeText ");

  TextField textField = TextField::New();

  application.GetScene().Add(textField);

  textField.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  textField.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  textField.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  application.SendNotification();
  application.Render();

  textField.SetProperty(TextField::Property::TEXT, "Hello world");

  application.SendNotification();
  application.Render();

  // Nothing is selected
  std::string selectedText = textField.GetProperty(DevelTextField::Property::SELECTED_TEXT).Get<std::string>();
  DALI_TEST_EQUALS("", selectedText, TEST_LOCATION);

  DevelTextField::SelectWholeText(textField);

  application.SendNotification();
  application.Render();

  // whole text is selected
  selectedText = textField.GetProperty(DevelTextField::Property::SELECTED_TEXT).Get<std::string>();
  DALI_TEST_EQUALS("Hello world", selectedText, TEST_LOCATION);

  DevelTextField::SelectNone(textField);

  application.SendNotification();
  application.Render();

  // Nothing is selected
  selectedText = textField.GetProperty(DevelTextField::Property::SELECTED_TEXT).Get<std::string>();
  DALI_TEST_EQUALS("", selectedText, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFieldSelectRange(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldSelectRange ");

  TextField textField = TextField::New();

  application.GetScene().Add(textField);

  textField.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  textField.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  textField.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  application.SendNotification();
  application.Render();

  textField.SetProperty(TextField::Property::TEXT, "Hello world");

  application.SendNotification();
  application.Render();

  textField.SetProperty(DevelTextField::Property::SELECTED_TEXT_START, 0);
  textField.SetProperty(DevelTextField::Property::SELECTED_TEXT_END, 5);

  // Hello is selected
  std::string selectedText = textField.GetProperty(DevelTextField::Property::SELECTED_TEXT).Get<std::string>();
  DALI_TEST_EQUALS("Hello", selectedText, TEST_LOCATION);

  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_START).Get<int>(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_END).Get<int>(), 5, TEST_LOCATION);

  textField.SetProperty(DevelTextField::Property::SELECTED_TEXT_START, 6);
  textField.SetProperty(DevelTextField::Property::SELECTED_TEXT_END, 11);

  // world is selected
  selectedText = textField.GetProperty(DevelTextField::Property::SELECTED_TEXT).Get<std::string>();
  DALI_TEST_EQUALS("world", selectedText, TEST_LOCATION);

  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_START).Get<int>(), 6, TEST_LOCATION);
  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_END).Get<int>(), 11, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFieldEnableEditing(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldEnableEditing ");

  TextField textField = TextField::New();

  application.GetScene().Add(textField);

  textField.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  textField.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  textField.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  application.SendNotification();
  application.Render();

  textField.SetKeyInputFocus();
  textField.SetProperty(DevelTextField::Property::ENABLE_EDITING, false);
  application.ProcessEvent(GenerateKey("D", "", "D", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "D", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(textField.GetProperty(TextField::Property::TEXT).Get<std::string>(), "", TEST_LOCATION);
  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::ENABLE_EDITING).Get<bool>(), false, TEST_LOCATION);

  textField.SetKeyInputFocus();
  textField.SetProperty(DevelTextField::Property::ENABLE_EDITING, true);
  application.ProcessEvent(GenerateKey("D", "", "D", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "D", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(textField.GetProperty(TextField::Property::TEXT).Get<std::string>(), "D", TEST_LOCATION);
  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::ENABLE_EDITING).Get<bool>(), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextFieldFontSizeScale(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldFontSizeScale");

  TextField textField = TextField::New();
  textField.SetProperty(TextField::Property::POINT_SIZE, 30.f);
  textField.SetProperty(TextField::Property::TEXT, "Test");
  Vector3 nonScaledSize = textField.GetNaturalSize();

  TextField textFieldScaled = TextField::New();
  textFieldScaled.SetProperty(TextField::Property::POINT_SIZE, 15.f);
  textFieldScaled.SetProperty(Toolkit::DevelTextField::Property::FONT_SIZE_SCALE, 2.f);
  textFieldScaled.SetProperty(TextField::Property::TEXT, "Test");
  Vector3 scaledSize = textFieldScaled.GetNaturalSize();

  DALI_TEST_EQUALS(nonScaledSize, scaledSize, TEST_LOCATION);

  textField.SetProperty(TextField::Property::PIXEL_SIZE, 30.f);
  textField.SetProperty(TextField::Property::TEXT, "Test");
  nonScaledSize = textField.GetNaturalSize();

  textFieldScaled.SetProperty(TextField::Property::PIXEL_SIZE, 15.f);
  textFieldScaled.SetProperty(Toolkit::DevelTextField::Property::FONT_SIZE_SCALE, 2.f);
  textFieldScaled.SetProperty(TextField::Property::TEXT, "Test");
  scaledSize = textFieldScaled.GetNaturalSize();

  DALI_TEST_EQUALS(nonScaledSize, scaledSize, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFieldPrimaryCursorPosition(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldPrimaryCursorPosition ");

  TextField textField = TextField::New();

  application.GetScene().Add(textField);

  textField.SetProperty(TextField::Property::TEXT, "ABCEF");
  textField.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  textField.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  textField.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  textField.SetProperty(DevelTextField::Property::PRIMARY_CURSOR_POSITION, 3);
  application.SendNotification();
  application.Render();
  textField.SetKeyInputFocus();

  application.ProcessEvent(GenerateKey("D", "", "D", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "D", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(textField.GetProperty(TextField::Property::TEXT).Get<std::string>(), "ABCDEF", TEST_LOCATION);
  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::PRIMARY_CURSOR_POSITION).Get<int>(), 4, TEST_LOCATION);

  END_TEST;
}

// test max length when set after setting long text
int utcDaliTextFieldMaxCharactersReachedAfterSetText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldMaxCharactersReachedAfterSetText");
  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  field.SetProperty(TextField::Property::TEXT, "123456789");

  const int maxNumberOfCharacters = 3;
  field.SetProperty(TextField::Property::MAX_LENGTH, maxNumberOfCharacters);

  field.SetKeyInputFocus();

  // connect to the text max lengh reached signal.
  ConnectionTracker* testTracker            = new ConnectionTracker();
  bool               maxLengthReachedSignal = false;
  field.ConnectSignal(testTracker, "maxLengthReached", CallbackFunctor(&maxLengthReachedSignal));

  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  DALI_TEST_CHECK(maxLengthReachedSignal);

  DALI_TEST_EQUALS(field.GetProperty(TextField::Property::TEXT).Get<std::string>(), "123456789", TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFieldAtlasLimitationIsEnabledForLargeFontPointSize(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldAtlasLimitationIsEnabledForLargeFontPointSize ");

  // +2: First one to handle the equal case. Second one to handle odd to even case of GetNaturalSize
  const uint32_t lessThanWidth  = TextAbstraction::FontClient::MAX_TEXT_ATLAS_WIDTH - TextAbstraction::FontClient::PADDING_TEXT_ATLAS_BLOCK + 2;
  const uint32_t lessThanHeight = TextAbstraction::FontClient::MAX_TEXT_ATLAS_HEIGHT - TextAbstraction::FontClient::PADDING_TEXT_ATLAS_BLOCK + 2;

  // Create a text field
  TextField textField = TextField::New();

  //Set size to avoid automatic eliding
  textField.SetProperty(Actor::Property::SIZE, Vector2(1025, 1025));
  //Set very large font-size using point-size
  textField.SetProperty(TextField::Property::POINT_SIZE, 1000);
  //Specify font-family
  textField.SetProperty(TextField::Property::FONT_FAMILY, "DejaVu Sans");
  //Set text to check if appear or not
  textField.SetProperty(TextField::Property::TEXT, "A");

  application.GetScene().Add(textField);

  application.SendNotification();
  application.Render();
  //Use GetNaturalSize to verify that size of block does not exceed Atlas size
  Vector3 naturalSize = textField.GetNaturalSize();

  DALI_TEST_GREATER(lessThanWidth, static_cast<uint32_t>(naturalSize.width), TEST_LOCATION);
  DALI_TEST_GREATER(lessThanHeight, static_cast<uint32_t>(naturalSize.height), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFieldAtlasLimitationIsEnabledPerformanceCases(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldAtlasLimitationIsEnabledPerformanceCases ");

  // +2: First one to handle the equal case. Second one to handle odd to even case of GetNaturalSize
  const uint32_t lessThanWidth  = TextAbstraction::FontClient::MAX_TEXT_ATLAS_WIDTH - TextAbstraction::FontClient::PADDING_TEXT_ATLAS_BLOCK + 2;
  const uint32_t lessThanHeight = TextAbstraction::FontClient::MAX_TEXT_ATLAS_HEIGHT - TextAbstraction::FontClient::PADDING_TEXT_ATLAS_BLOCK + 2;

  Vector3 naturalSize; //Use GetNaturalSize to verify that size of block does not exceed Atlas size
  // Create a text editor
  TextField textField = TextField::New();

  //Set size to avoid automatic eliding
  textField.SetProperty(Actor::Property::SIZE, Vector2(1025, 1025));
  textField.SetProperty(TextField::Property::FONT_FAMILY, "DejaVu Sans");
  textField.SetProperty(TextField::Property::TEXT, "A");

  const int numberOfCases             = 6;
  int       arrayCases[numberOfCases] = {323, 326, 330, 600, 1630, 2500};

  for(int index = 0; index < numberOfCases; index++)
  {
    tet_printf(" UtcDaliTextFieldAtlasLimitationIsEnabledPerformanceCases point-size= %d \n", arrayCases[index]);
    textField.SetProperty(TextField::Property::POINT_SIZE, arrayCases[index]);
    application.GetScene().Add(textField);
    application.SendNotification();
    application.Render();
    naturalSize = textField.GetNaturalSize();
    DALI_TEST_GREATER(lessThanWidth, static_cast<uint32_t>(naturalSize.width), TEST_LOCATION);
    DALI_TEST_GREATER(lessThanHeight, static_cast<uint32_t>(naturalSize.height), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliToolkitTextFieldEllipsisPositionProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldEllipsisPositionProperty ");
  TextField textField = TextField::New();

  tet_infoline(" UtcDaliToolkitTextFieldEllipsisPositionProperty - Default is END");
  DALI_TEST_EQUALS(textField.GetProperty<int>(DevelTextField::Property::ELLIPSIS_POSITION), static_cast<int>(Toolkit::DevelText::EllipsisPosition::END), TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextFieldEllipsisPositionProperty - Change to START");
  textField.SetProperty(DevelTextField::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::START);
  DALI_TEST_EQUALS(textField.GetProperty<int>(DevelTextField::Property::ELLIPSIS_POSITION), static_cast<int>(Toolkit::DevelText::EllipsisPosition::START), TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextFieldEllipsisPositionProperty - Change to MIDDLE");
  textField.SetProperty(DevelTextField::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::MIDDLE);
  DALI_TEST_EQUALS(textField.GetProperty<int>(DevelTextField::Property::ELLIPSIS_POSITION), static_cast<int>(Toolkit::DevelText::EllipsisPosition::MIDDLE), TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextFieldEllipsisPositionProperty - Change to END");
  textField.SetProperty(DevelTextField::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::END);
  DALI_TEST_EQUALS(textField.GetProperty<int>(DevelTextField::Property::ELLIPSIS_POSITION), static_cast<int>(Toolkit::DevelText::EllipsisPosition::END), TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextFieldEllipsisPositionProperty - Change to START using integer");
  textField.SetProperty(DevelTextField::Property::ELLIPSIS_POSITION, 1);
  DALI_TEST_EQUALS(textField.GetProperty<int>(DevelTextField::Property::ELLIPSIS_POSITION), static_cast<int>(Toolkit::DevelText::EllipsisPosition::START), TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextFieldEllipsisPositionProperty - Change to MIDDLE using integer");
  textField.SetProperty(DevelTextField::Property::ELLIPSIS_POSITION, 2);
  DALI_TEST_EQUALS(textField.GetProperty<int>(DevelTextField::Property::ELLIPSIS_POSITION), static_cast<int>(Toolkit::DevelText::EllipsisPosition::MIDDLE), TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextFieldEllipsisPositionProperty - Change to END using integer");
  textField.SetProperty(DevelTextField::Property::ELLIPSIS_POSITION, 0);
  DALI_TEST_EQUALS(textField.GetProperty<int>(DevelTextField::Property::ELLIPSIS_POSITION), static_cast<int>(Toolkit::DevelText::EllipsisPosition::END), TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to START using string - uppercase");
  textField.SetProperty(DevelTextField::Property::ELLIPSIS_POSITION, "START");
  DALI_TEST_EQUALS(textField.GetProperty<int>(DevelTextField::Property::ELLIPSIS_POSITION), static_cast<int>(Toolkit::DevelText::EllipsisPosition::START), TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to MIDDLE using string - uppercase");
  textField.SetProperty(DevelTextField::Property::ELLIPSIS_POSITION, "MIDDLE");
  DALI_TEST_EQUALS(textField.GetProperty<int>(DevelTextField::Property::ELLIPSIS_POSITION), static_cast<int>(Toolkit::DevelText::EllipsisPosition::MIDDLE), TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to END using string - uppercase");
  textField.SetProperty(DevelTextField::Property::ELLIPSIS_POSITION, "END");
  DALI_TEST_EQUALS(textField.GetProperty<int>(DevelTextField::Property::ELLIPSIS_POSITION), static_cast<int>(Toolkit::DevelText::EllipsisPosition::END), TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to START using string - lowercase");
  textField.SetProperty(DevelTextField::Property::ELLIPSIS_POSITION, "start");
  DALI_TEST_EQUALS(textField.GetProperty<int>(DevelTextField::Property::ELLIPSIS_POSITION), static_cast<int>(Toolkit::DevelText::EllipsisPosition::START), TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to MIDDLE using string - lowercase");
  textField.SetProperty(DevelTextField::Property::ELLIPSIS_POSITION, "middle");
  DALI_TEST_EQUALS(textField.GetProperty<int>(DevelTextField::Property::ELLIPSIS_POSITION), static_cast<int>(Toolkit::DevelText::EllipsisPosition::MIDDLE), TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to END using string - lowercase");
  textField.SetProperty(DevelTextField::Property::ELLIPSIS_POSITION, "end");
  DALI_TEST_EQUALS(textField.GetProperty<int>(DevelTextField::Property::ELLIPSIS_POSITION), static_cast<int>(Toolkit::DevelText::EllipsisPosition::END), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFieldCopyText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldCopyText ");

  TextField textField = TextField::New();

  std::string selectedText = "";
  std::string copiedText   = "";

  application.GetScene().Add(textField);

  textField.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  textField.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  textField.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  application.SendNotification();
  application.Render();

  textField.SetProperty(TextField::Property::TEXT, "Hello world");

  application.SendNotification();
  application.Render();

  // Hello is selected
  DevelTextField::SelectText(textField, 0, 5);

  application.SendNotification();
  application.Render();

  selectedText = textField.GetProperty(DevelTextField::Property::SELECTED_TEXT).Get<std::string>();
  DALI_TEST_EQUALS("Hello", selectedText, TEST_LOCATION);

  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_START).Get<int>(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_END).Get<int>(), 5, TEST_LOCATION);

  // Hello is copied
  copiedText = DevelTextField::CopyText(textField);
  DALI_TEST_EQUALS("Hello", copiedText, TEST_LOCATION);

  // world is selected
  DevelTextField::SelectText(textField, 6, 11);

  application.SendNotification();
  application.Render();

  selectedText = textField.GetProperty(DevelTextField::Property::SELECTED_TEXT).Get<std::string>();
  DALI_TEST_EQUALS("world", selectedText, TEST_LOCATION);

  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_START).Get<int>(), 6, TEST_LOCATION);
  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_END).Get<int>(), 11, TEST_LOCATION);

  // world is copied
  copiedText = DevelTextField::CopyText(textField);
  DALI_TEST_EQUALS("world", copiedText, TEST_LOCATION);

  // "lo wo" is selected
  DevelTextField::SelectText(textField, 3, 8);

  application.SendNotification();
  application.Render();

  selectedText = textField.GetProperty(DevelTextField::Property::SELECTED_TEXT).Get<std::string>();
  DALI_TEST_EQUALS("lo wo", selectedText, TEST_LOCATION);

  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_START).Get<int>(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_END).Get<int>(), 8, TEST_LOCATION);

  // "lo wo" is copied
  copiedText = DevelTextField::CopyText(textField);
  DALI_TEST_EQUALS("lo wo", copiedText, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFieldCutText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldCutText ");

  TextField textField = TextField::New();

  std::string selectedText = "";

  application.GetScene().Add(textField);

  textField.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  textField.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  textField.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  application.SendNotification();
  application.Render();

  textField.SetProperty(TextField::Property::TEXT, "Hello world");

  application.SendNotification();
  application.Render();

  // Hello is selected
  DevelTextField::SelectText(textField, 0, 5);

  application.SendNotification();
  application.Render();

  selectedText = textField.GetProperty(DevelTextField::Property::SELECTED_TEXT).Get<std::string>();
  DALI_TEST_EQUALS("Hello", selectedText, TEST_LOCATION);

  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_START).Get<int>(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_END).Get<int>(), 5, TEST_LOCATION);

  // Hello is cut
  DALI_TEST_EQUALS("Hello", DevelTextField::CutText(textField), TEST_LOCATION);

  DALI_TEST_EQUALS(textField.GetProperty(TextField::Property::TEXT).Get<std::string>(), " world", TEST_LOCATION);

  // " w" is selected
  DevelTextField::SelectText(textField, 0, 2);

  application.SendNotification();
  application.Render();

  selectedText = textField.GetProperty(DevelTextField::Property::SELECTED_TEXT).Get<std::string>();
  DALI_TEST_EQUALS(" w", selectedText, TEST_LOCATION);

  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_START).Get<int>(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_END).Get<int>(), 2, TEST_LOCATION);

  // " w" is cut
  DALI_TEST_EQUALS(" w", DevelTextField::CutText(textField), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(textField.GetProperty(TextField::Property::TEXT).Get<std::string>(), "orld", TEST_LOCATION);

  // Test Cut from the middle

  // "rl" is selected
  DevelTextField::SelectText(textField, 1, 3);

  application.SendNotification();
  application.Render();

  selectedText = textField.GetProperty(DevelTextField::Property::SELECTED_TEXT).Get<std::string>();
  DALI_TEST_EQUALS("rl", selectedText, TEST_LOCATION);

  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_START).Get<int>(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_END).Get<int>(), 3, TEST_LOCATION);

  // "rl" is cut
  DALI_TEST_EQUALS("rl", DevelTextField::CutText(textField), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(textField.GetProperty(TextField::Property::TEXT).Get<std::string>(), "od", TEST_LOCATION);

  // Test Cut from the end

  // "d" is selected
  DevelTextField::SelectText(textField, 1, 2);

  application.SendNotification();
  application.Render();

  selectedText = textField.GetProperty(DevelTextField::Property::SELECTED_TEXT).Get<std::string>();
  DALI_TEST_EQUALS("d", selectedText, TEST_LOCATION);

  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_START).Get<int>(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(textField.GetProperty(DevelTextField::Property::SELECTED_TEXT_END).Get<int>(), 2, TEST_LOCATION);

  // "d" is cut
  DALI_TEST_EQUALS("d", DevelTextField::CutText(textField), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(textField.GetProperty(TextField::Property::TEXT).Get<std::string>(), "o", TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFieldPasteText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldPasteText ");

  TextField textField = TextField::New();

  application.GetScene().Add(textField);

  std::string cutText    = "";
  std::string copiedText = "";

  textField.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  textField.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  textField.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  application.SendNotification();
  application.Render();

  textField.SetProperty(TextField::Property::TEXT, "Hello World");

  application.SendNotification();
  application.Render();

  // Tap on the text editor
  TestGenerateTap(application, 3.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Select some text in the right of the current cursor position
  DevelTextField::SelectText(textField, 0, 3);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Cut the selected text
  cutText = DevelTextField::CutText(textField);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS("Hel", cutText, TEST_LOCATION);
  DALI_TEST_EQUALS(textField.GetProperty(TextField::Property::TEXT).Get<std::string>(), "lo World", TEST_LOCATION);

  DevelTextField::SelectText(textField, 0, 3);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Copy the selected text
  copiedText = DevelTextField::CopyText(textField);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS("lo ", copiedText, TEST_LOCATION);
  DALI_TEST_EQUALS("lo World", textField.GetProperty<std::string>(TextField::Property::TEXT), TEST_LOCATION);

  // Move the cursor to the end of the line
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  // Paste the selected text at the current cursor position
  DevelTextField::PasteText(textField);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(textField.GetProperty(TextField::Property::TEXT).Get<std::string>(), "lo Worldlo ", TEST_LOCATION);

  END_TEST;
}

int utcDaliTextFieldCursorPositionChangedSignal(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldCursorPositionChangedSignal");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  // connect to the selection changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextField::CursorPositionChangedSignal(field).Connect(&TestCursorPositionChangedCallback);
  bool cursorPositionChangedSignal = false;
  field.ConnectSignal(testTracker, "cursorPositionChanged", CallbackFunctor(&cursorPositionChangedSignal));

  field.SetProperty(TextField::Property::TEXT, "Hello world Hello world");
  field.SetProperty(TextField::Property::POINT_SIZE, 10.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(100.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetKeyInputFocus();

  // Tap on the text field
  TestGenerateTap(application, 3.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gCursorPositionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldCursorPos, 23, TEST_LOCATION);

  gCursorPositionChangedCallbackCalled = false;

  // Move to left.
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gCursorPositionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldCursorPos, 17, TEST_LOCATION);

  gCursorPositionChangedCallbackCalled = false;

  // Insert D
  application.ProcessEvent(GenerateKey("D", "", "D", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "D", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gCursorPositionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldCursorPos, 16, TEST_LOCATION);

  gCursorPositionChangedCallbackCalled = false;

  //delete one character
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gCursorPositionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldCursorPos, 17, TEST_LOCATION);

  gCursorPositionChangedCallbackCalled = false;

  field.SetProperty(TextField::Property::TEXT, "Hello");

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gCursorPositionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldCursorPos, 16, TEST_LOCATION);

  gCursorPositionChangedCallbackCalled = false;

  field.SetProperty(DevelTextField::Property::PRIMARY_CURSOR_POSITION, 3);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gCursorPositionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldCursorPos, 5, TEST_LOCATION);

  END_TEST;
}

int utcDaliTextFieldGeometryEllipsisStart(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldGeometryEllipsisStart");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  field.SetProperty(TextField::Property::POINT_SIZE, 7.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(250.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  field.SetProperty(TextField::Property::ENABLE_MARKUP, true);
  field.SetProperty(DevelTextField::Property::ELLIPSIS, true);
  field.SetProperty(DevelTextField::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::START);
  field.SetProperty(TextField::Property::TEXT, "Hello World");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Render and notify
  application.SendNotification();
  application.Render();

  unsigned int expectedCount = 1;
  unsigned int startIndex    = 0;
  unsigned int endIndex      = 10;

  Vector<Vector2> positionsList = DevelTextField::GetTextPosition(field, startIndex, endIndex);
  Vector<Vector2> sizeList      = DevelTextField::GetTextSize(field, startIndex, endIndex);

  DALI_TEST_EQUALS(positionsList.Size(), expectedCount, TEST_LOCATION);
  DALI_TEST_EQUALS(sizeList.Size(), expectedCount, TEST_LOCATION);

  Vector<Vector2> expectedSizes;
  Vector<Vector2> expectedPositions;

  expectedPositions.PushBack(Vector2(14, 0));
  expectedSizes.PushBack(Vector2(106, 25));

  TestTextGeometryUtils::CheckGeometryResult(positionsList, sizeList, expectedPositions, expectedSizes);

  END_TEST;
}

int utcDaliTextFieldGeometryEllipsisEnd(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldGeometryEllipsisEnd");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  field.SetProperty(TextField::Property::POINT_SIZE, 7.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(250.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  field.SetProperty(TextField::Property::ENABLE_MARKUP, true);
  field.SetProperty(DevelTextField::Property::ELLIPSIS, true);
  field.SetProperty(DevelTextField::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::END);
  field.SetProperty(TextField::Property::TEXT, "Hello World");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Render and notify
  application.SendNotification();
  application.Render();

  unsigned int expectedCount = 1;
  unsigned int startIndex    = 0;
  unsigned int endIndex      = 10;

  Vector<Vector2> positionsList = DevelTextField::GetTextPosition(field, startIndex, endIndex);
  Vector<Vector2> sizeList      = DevelTextField::GetTextSize(field, startIndex, endIndex);

  DALI_TEST_EQUALS(positionsList.Size(), expectedCount, TEST_LOCATION);
  DALI_TEST_EQUALS(sizeList.Size(), expectedCount, TEST_LOCATION);

  Vector<Vector2> expectedSizes;
  Vector<Vector2> expectedPositions;

  expectedPositions.PushBack(Vector2(-2, 0));
  expectedSizes.PushBack(Vector2(122, 25));

  TestTextGeometryUtils::CheckGeometryResult(positionsList, sizeList, expectedPositions, expectedSizes);

  END_TEST;
}

int utcDaliTextFieldGeometryRTL(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldGeometryRTL");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  field.SetProperty(TextField::Property::POINT_SIZE, 7.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(300.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  field.SetProperty(TextField::Property::ENABLE_MARKUP, true);
  field.SetProperty(TextField::Property::TEXT, "السطر الاخير");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Render and notify
  application.SendNotification();
  application.Render();

  unsigned int expectedCount = 1;
  unsigned int startIndex    = 1;
  unsigned int endIndex      = 7;

  Vector<Vector2> positionsList = DevelTextField::GetTextPosition(field, startIndex, endIndex);
  Vector<Vector2> sizeList      = DevelTextField::GetTextSize(field, startIndex, endIndex);

  DALI_TEST_EQUALS(positionsList.Size(), expectedCount, TEST_LOCATION);
  DALI_TEST_EQUALS(sizeList.Size(), expectedCount, TEST_LOCATION);

  Vector<Vector2> expectedSizes;
  Vector<Vector2> expectedPositions;

  expectedPositions.PushBack(Vector2(38, 0));
  expectedSizes.PushBack(Vector2(73, 25));

  TestTextGeometryUtils::CheckGeometryResult(positionsList, sizeList, expectedPositions, expectedSizes);

  END_TEST;
}

int utcDaliTextFieldGeometryGlyphMiddle(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldGeometryGlyphMiddle");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  field.SetProperty(TextField::Property::POINT_SIZE, 7.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(150.f, 200.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  field.SetProperty(TextField::Property::ENABLE_MARKUP, true);
  field.SetProperty(TextField::Property::TEXT, "لا تحتوي على لا");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Render and notify
  application.SendNotification();
  application.Render();

  unsigned int expectedCount = 1;
  unsigned int startIndex    = 1;
  unsigned int endIndex      = 13;

  Vector<Vector2> positionsList = DevelTextField::GetTextPosition(field, startIndex, endIndex);
  Vector<Vector2> sizeList      = DevelTextField::GetTextSize(field, startIndex, endIndex);

  DALI_TEST_EQUALS(positionsList.Size(), expectedCount, TEST_LOCATION);
  DALI_TEST_EQUALS(sizeList.Size(), expectedCount, TEST_LOCATION);

  Vector<Vector2> expectedSizes;
  Vector<Vector2> expectedPositions;

  expectedPositions.PushBack(Vector2(6, 0));
  expectedSizes.PushBack(Vector2(124, 25));

  TestTextGeometryUtils::CheckGeometryResult(positionsList, sizeList, expectedPositions, expectedSizes);

  END_TEST;
}

int utcDaliTextFieldSelectionClearedSignal(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldSelectionClearedSignal");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  // connect to the selection changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextField::SelectionClearedSignal(field).Connect(&TestSelectionClearedCallback);
  bool selectionClearedSignal = false;
  field.ConnectSignal(testTracker, "selectionCleared", CallbackFunctor(&selectionClearedSignal));

  field.SetProperty(TextField::Property::TEXT, "Hello\nworld\nHello world");
  field.SetProperty(TextField::Property::POINT_SIZE, 10.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(100.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap on the text editor
  TestGenerateTap(application, 3.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Move to second line of the text & Select some text in the right of the current cursor position
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // remove selection
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_ESCAPE, 0, 0, Integration::KeyEvent::UP, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionClearedCallbackCalled);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap on the text editor
  TestGenerateTap(application, 3.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  gSelectionClearedCallbackCalled = false;

  // Move to second line of the text & select.
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  //remove selection
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionClearedCallbackCalled);

  gSelectionClearedCallbackCalled = false;

  // Render and notify
  application.SendNotification();
  application.Render();

  // Move to second line of the text & select.
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // replace D with selected text
  application.ProcessEvent(GenerateKey("D", "", "D", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "D", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionClearedCallbackCalled);

  gSelectionClearedCallbackCalled = false;

  // Render and notify
  application.SendNotification();
  application.Render();

  DevelTextField::SelectText(field, 1, 3);

  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetProperty(DevelTextField::Property::PRIMARY_CURSOR_POSITION, 3);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionClearedCallbackCalled);

  gSelectionClearedCallbackCalled = false;

  DevelTextField::SelectText(field, 1, 3);

  // Render and notify
  application.SendNotification();
  application.Render();

  // select none
  DevelTextField::SelectNone(field);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionClearedCallbackCalled);

  END_TEST;
}

int utcDaliTextFieldSelectionStartedSignal(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldSelectionStartedSignal");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  // connect to the selection changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextField::SelectionStartedSignal(field).Connect(&TestSelectionStartedCallback);
  bool selectionStartedSignal = false;
  field.ConnectSignal(testTracker, "selectionStarted", CallbackFunctor(&selectionStartedSignal));

  field.SetProperty(TextField::Property::TEXT, "Hello\nworld\nHello world");
  field.SetProperty(TextField::Property::POINT_SIZE, 10.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(100.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap on the text field
  TestGenerateTap(application, 3.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Move to second line of the text & Select some text in the right of the current cursor position
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionStartedCallbackCalled);

  // remove selection
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_ESCAPE, 0, 0, Integration::KeyEvent::UP, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  gSelectionStartedCallbackCalled = false;

  DevelTextField::SelectText(field, 1, 3);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionStartedCallbackCalled);

  END_TEST;
}

int utcDaliTextFieldSelectionChangedSignal(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldSelectionChangedSignal");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  // connect to the selection changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextField::SelectionChangedSignal(field).Connect(&TestSelectionChangedCallback);
  bool selectionChangedSignal = false;
  field.ConnectSignal(testTracker, "selectionChanged", CallbackFunctor(&selectionChangedSignal));

  field.SetProperty(TextField::Property::TEXT, "Hello world Hello world");
  field.SetProperty(TextField::Property::POINT_SIZE, 10.f);
  field.SetProperty(Actor::Property::SIZE, Vector2(100.f, 50.f));
  field.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap on the text field
  TestGenerateTap(application, 3.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Select some text in the right of the current cursor position
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_SHIFT_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldSelectionStart, oldSelectionEnd, TEST_LOCATION);

  gSelectionChangedCallbackCalled = false;

  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldSelectionStart, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(oldSelectionEnd, 1, TEST_LOCATION);

  gSelectionChangedCallbackCalled = false;

  application.ProcessEvent(GenerateKey("", "", "", DALI_KEY_ESCAPE, 0, 0, Integration::KeyEvent::UP, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldSelectionStart, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(oldSelectionEnd, 2, TEST_LOCATION);

  gSelectionChangedCallbackCalled = false;
  field.SetKeyInputFocus();

  // Render and notify
  application.SendNotification();
  application.Render();

  DevelTextField::SelectText(field, 0, 5);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldSelectionStart, oldSelectionEnd, TEST_LOCATION);

  gSelectionChangedCallbackCalled = false;

  field.SetProperty(DevelTextField::Property::PRIMARY_CURSOR_POSITION, 3);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldSelectionStart, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(oldSelectionEnd, 5, TEST_LOCATION);

  gSelectionChangedCallbackCalled = false;

  // select all text
  DevelTextField::SelectWholeText(field);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldSelectionStart, oldSelectionEnd, TEST_LOCATION);

  gSelectionChangedCallbackCalled = false;

  // select none
  DevelTextField::SelectNone(field);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldSelectionStart, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(oldSelectionEnd, 23, TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextFieldStrikethroughGeneration(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldStrikethroughGeneration");

  TextField textField = TextField::New();
  textField.SetProperty(TextField::Property::TEXT, "Test");
  textField.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 100.f));
  textField.SetProperty(TextField::Property::POINT_SIZE, 10);
  textField.SetProperty(TextField::Property::FONT_FAMILY, "DejaVu Sans");

  application.GetScene().Add(textField);
  application.SendNotification();
  application.Render();

  Property::Map strikethroughMapSet;
  Property::Map strikethroughMapGet;

  strikethroughMapSet.Insert("enable", true);
  strikethroughMapSet.Insert("color", Color::RED);
  strikethroughMapSet.Insert("height", 2.0f);

  // Check the strikethrough property
  textField.SetProperty(DevelTextField::Property::STRIKETHROUGH, strikethroughMapSet);
  strikethroughMapGet = textField.GetProperty<Property::Map>(DevelTextField::Property::STRIKETHROUGH);
  textField.SetProperty(TextField::Property::TEXT, "Test1");
  DALI_TEST_EQUALS(strikethroughMapGet.Count(), strikethroughMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(strikethroughMapGet, strikethroughMapSet), true, TEST_LOCATION);

  // Render and notify
  application.SendNotification();
  application.Render();

  strikethroughMapSet.Clear();
  strikethroughMapGet.Clear();

  END_TEST;
}

int UtcDaliToolkitTextFieldInputStrikethroughGeneration(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldInputStrikethroughGeneration");

  TextField textField = TextField::New();
  textField.SetProperty(TextField::Property::TEXT, "Test");
  textField.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 100.f));
  textField.SetProperty(TextField::Property::POINT_SIZE, 10);
  textField.SetProperty(TextField::Property::FONT_FAMILY, "DejaVu Sans");

  application.GetScene().Add(textField);
  application.SendNotification();
  application.Render();

  std::string strikethroughSettings1("{\"enable\":\"true\",\"color\":\"red\",\"height\":\"2\"}");

  // Check the strikethrough property
  textField.SetProperty(DevelTextField::Property::INPUT_STRIKETHROUGH, strikethroughSettings1);
  textField.SetProperty(TextField::Property::TEXT, "Test1");
  DALI_TEST_EQUALS(textField.GetProperty<std::string>(DevelTextField::Property::INPUT_STRIKETHROUGH), strikethroughSettings1, TEST_LOCATION);

  // Render and notify
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextFieldUnderlineTypesGeneration1(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldUnderlineTypesGeneration1");
  TextField field = TextField::New();
  field.SetProperty(TextField::Property::TEXT, "Test");
  field.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 100.f));
  field.SetProperty(TextField::Property::POINT_SIZE, 10);
  field.SetProperty(TextField::Property::FONT_FAMILY, "DejaVu Sans");

  application.GetScene().Add(field);
  application.SendNotification();
  application.Render();

  Property::Map underlineMapSet;
  Property::Map underlineMapGet;

  underlineMapSet.Insert("enable", true);
  underlineMapSet.Insert("color", Color::RED);
  underlineMapSet.Insert("height", 1);
  underlineMapSet.Insert("type", Text::Underline::SOLID);
  underlineMapSet.Insert("dashWidth", 2);
  underlineMapSet.Insert("dashGap", 1);

  // Check the underline property
  field.SetProperty(TextField::Property::UNDERLINE, underlineMapSet);

  underlineMapGet = field.GetProperty<Property::Map>(TextField::Property::UNDERLINE);
  DALI_TEST_EQUALS(underlineMapGet.Count(), underlineMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(underlineMapGet, underlineMapSet), true, TEST_LOCATION);

  // Render and notify
  application.SendNotification();
  application.Render();

  underlineMapSet.Clear();
  underlineMapGet.Clear();

  underlineMapSet.Insert("enable", true);
  underlineMapSet.Insert("color", Color::BLUE);
  underlineMapSet.Insert("height", 1);
  underlineMapSet.Insert("type", Text::Underline::DASHED);
  underlineMapSet.Insert("dashWidth", 4);
  underlineMapSet.Insert("dashGap", 2);

  // Check the dashed underline property
  field.SetProperty(TextField::Property::UNDERLINE, underlineMapSet);

  underlineMapGet = field.GetProperty<Property::Map>(TextField::Property::UNDERLINE);
  DALI_TEST_EQUALS(underlineMapGet.Count(), underlineMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(underlineMapGet, underlineMapSet), true, TEST_LOCATION);

  // Render and notify
  application.SendNotification();
  application.Render();

  underlineMapSet.Clear();
  underlineMapGet.Clear();

  underlineMapSet.Insert("enable", true);
  underlineMapSet.Insert("color", Color::BLUE);
  underlineMapSet.Insert("height", 1);
  underlineMapSet.Insert("type", Text::Underline::DOUBLE);
  underlineMapSet.Insert("dashWidth", 4);
  underlineMapSet.Insert("dashGap", 2);

  // Check the dashed underline property
  field.SetProperty(TextField::Property::UNDERLINE, underlineMapSet);

  underlineMapGet = field.GetProperty<Property::Map>(TextField::Property::UNDERLINE);
  DALI_TEST_EQUALS(underlineMapGet.Count(), underlineMapSet.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(underlineMapGet, underlineMapSet), true, TEST_LOCATION);

  // Render and notify
  application.SendNotification();
  application.Render();

  underlineMapSet.Clear();
  underlineMapGet.Clear();

  END_TEST;
}

int UtcDaliToolkitTextFieldUnderlineTypesGeneration2(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldUnderlineTypesGeneration2");

  TextField field1 = TextField::New();
  field1.SetProperty(TextField::Property::TEXT, "Test");
  field1.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 100.f));
  field1.SetProperty(TextField::Property::POINT_SIZE, 10);
  field1.SetProperty(TextField::Property::FONT_FAMILY, "DejaVu Sans");

  Property::Map underlineMapSet1;
  Property::Map underlineMapGet1;

  underlineMapSet1.Insert("enable", true);
  underlineMapSet1.Insert("color", Color::RED);
  underlineMapSet1.Insert("height", 1);
  underlineMapSet1.Insert("type", Text::Underline::SOLID);
  underlineMapSet1.Insert("dashWidth", 2);
  underlineMapSet1.Insert("dashGap", 1);

  // Check the underline property
  field1.SetProperty(TextField::Property::UNDERLINE, underlineMapSet1);

  underlineMapGet1 = field1.GetProperty<Property::Map>(TextField::Property::UNDERLINE);
  DALI_TEST_EQUALS(underlineMapGet1.Count(), underlineMapSet1.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(underlineMapGet1, underlineMapSet1), true, TEST_LOCATION);

  application.GetScene().Add(field1);
  application.SendNotification();
  application.Render();

  TextField field2 = TextField::New();
  field2.SetProperty(TextField::Property::TEXT, "Test");
  field2.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 100.f));
  field2.SetProperty(TextField::Property::POINT_SIZE, 10);
  field2.SetProperty(TextField::Property::FONT_FAMILY, "DejaVu Sans");

  Property::Map underlineMapSet2;
  Property::Map underlineMapGet2;

  underlineMapSet2.Insert("enable", true);
  underlineMapSet2.Insert("color", Color::BLUE);
  underlineMapSet2.Insert("height", 1);
  underlineMapSet2.Insert("type", Text::Underline::DASHED);
  underlineMapSet2.Insert("dashWidth", 4);
  underlineMapSet2.Insert("dashGap", 2);

  // Check the dashed underline property
  field2.SetProperty(TextField::Property::UNDERLINE, underlineMapSet2);

  underlineMapGet2 = field2.GetProperty<Property::Map>(TextField::Property::UNDERLINE);
  DALI_TEST_EQUALS(underlineMapGet2.Count(), underlineMapSet2.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(underlineMapGet2, underlineMapSet2), true, TEST_LOCATION);

  application.GetScene().Add(field2);
  application.SendNotification();
  application.Render();

  TextField field3 = TextField::New();
  field3.SetProperty(TextField::Property::TEXT, "Test");
  field3.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 100.f));
  field3.SetProperty(TextField::Property::POINT_SIZE, 10);
  field3.SetProperty(TextField::Property::FONT_FAMILY, "DejaVu Sans");

  Property::Map underlineMapSet3;
  Property::Map underlineMapGet3;

  underlineMapSet3.Insert("enable", true);
  underlineMapSet3.Insert("color", Color::BLUE);
  underlineMapSet3.Insert("height", 1);
  underlineMapSet3.Insert("type", Text::Underline::DOUBLE);
  underlineMapSet3.Insert("dashWidth", 4);
  underlineMapSet3.Insert("dashGap", 2);

  // Check the dashed underline property
  field3.SetProperty(TextField::Property::UNDERLINE, underlineMapSet3);

  underlineMapGet3 = field3.GetProperty<Property::Map>(TextField::Property::UNDERLINE);
  DALI_TEST_EQUALS(underlineMapGet3.Count(), underlineMapSet3.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(underlineMapGet3, underlineMapSet3), true, TEST_LOCATION);

  application.GetScene().Add(field3);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliTextFieldCharacterSpacing(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldCharacterSpacing ");

  TextField textField = TextField::New();

  textField.SetProperty(Actor::Property::SIZE, Vector2(150.0f, 300.f));

  application.GetScene().Add(textField);
  application.SendNotification();
  application.Render();

  textField.SetProperty(TextField::Property::TEXT, "Hi Experiment");
  textField.SetProperty(DevelTextField::Property::CHARACTER_SPACING, 10.f);
  DALI_TEST_EQUALS(textField.GetProperty<float>(DevelTextField::Property::CHARACTER_SPACING), 10.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextFieldUnderlineTypesGeneration3(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldUnderlineTypesGeneration3");

  TextField field1 = TextField::New();
  field1.SetProperty(TextField::Property::TEXT, "Test1");
  field1.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 100.f));
  field1.SetProperty(TextField::Property::POINT_SIZE, 10);
  field1.SetProperty(TextField::Property::FONT_FAMILY, "DejaVu Sans");

  Property::Map underlineMapSet1;
  Property::Map underlineMapGet1;

  underlineMapSet1.Insert("enable", true);
  underlineMapSet1.Insert("color", Color::RED);
  underlineMapSet1.Insert("height", 1);
  underlineMapSet1.Insert("type", Text::Underline::SOLID);
  underlineMapSet1.Insert("dashWidth", 2);
  underlineMapSet1.Insert("dashGap", 1);

  // Check the underline property
  field1.SetProperty(TextField::Property::UNDERLINE, underlineMapSet1);
  //field1.SetProperty( TextField::Property::TEXT, "Test2" );

  underlineMapGet1 = field1.GetProperty<Property::Map>(TextField::Property::UNDERLINE);
  DALI_TEST_EQUALS(underlineMapGet1.Count(), underlineMapSet1.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(DaliTestCheckMaps(underlineMapGet1, underlineMapSet1), true, TEST_LOCATION);

  application.GetScene().Add(field1);
  application.SendNotification();
  application.Render();

  END_TEST;
}