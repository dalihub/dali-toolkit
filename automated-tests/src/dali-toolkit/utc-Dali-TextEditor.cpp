/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/devel-api/adaptor-framework/clipboard.h>
#include <dali/devel-api/adaptor-framework/key-devel.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-editor-devel.h>
#include <dali-toolkit/devel-api/text/rendering-backend.h>
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include "test-text-geometry-utils.h"

using namespace Dali;
using namespace Toolkit;

void dali_texteditor_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_texteditor_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

const char* const PROPERTY_NAME_RENDERING_BACKEND                    = "renderingBackend";
const char* const PROPERTY_NAME_TEXT                                 = "text";
const char* const PROPERTY_NAME_TEXT_COLOR                           = "textColor";
const char* const PROPERTY_NAME_FONT_FAMILY                          = "fontFamily";
const char* const PROPERTY_NAME_FONT_STYLE                           = "fontStyle";
const char* const PROPERTY_NAME_POINT_SIZE                           = "pointSize";
const char* const PROPERTY_NAME_HORIZONTAL_ALIGNMENT                 = "horizontalAlignment";
const char* const PROPERTY_NAME_SCROLL_THRESHOLD                     = "scrollThreshold";
const char* const PROPERTY_NAME_SCROLL_SPEED                         = "scrollSpeed";
const char* const PROPERTY_NAME_PRIMARY_CURSOR_COLOR                 = "primaryCursorColor";
const char* const PROPERTY_NAME_SECONDARY_CURSOR_COLOR               = "secondaryCursorColor";
const char* const PROPERTY_NAME_ENABLE_CURSOR_BLINK                  = "enableCursorBlink";
const char* const PROPERTY_NAME_CURSOR_BLINK_INTERVAL                = "cursorBlinkInterval";
const char* const PROPERTY_NAME_CURSOR_BLINK_DURATION                = "cursorBlinkDuration";
const char* const PROPERTY_NAME_CURSOR_WIDTH                         = "cursorWidth";
const char* const PROPERTY_NAME_GRAB_HANDLE_IMAGE                    = "grabHandleImage";
const char* const PROPERTY_NAME_GRAB_HANDLE_PRESSED_IMAGE            = "grabHandlePressedImage";
const char* const PROPERTY_NAME_SELECTION_HANDLE_IMAGE_LEFT          = "selectionHandleImageLeft";
const char* const PROPERTY_NAME_SELECTION_HANDLE_IMAGE_RIGHT         = "selectionHandleImageRight";
const char* const PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_LEFT  = "selectionHandlePressedImageLeft";
const char* const PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_RIGHT = "selectionHandlePressedImageRight";
const char* const PROPERTY_NAME_SELECTION_HANDLE_MARKER_IMAGE_LEFT   = "selectionHandleMarkerImageLeft";
const char* const PROPERTY_NAME_SELECTION_HANDLE_MARKER_IMAGE_RIGHT  = "selectionHandleMarkerImageRight";
const char* const PROPERTY_NAME_SELECTION_HIGHLIGHT_COLOR            = "selectionHighlightColor";
const char* const PROPERTY_NAME_DECORATION_BOUNDING_BOX              = "decorationBoundingBox";
const char* const PROPERTY_NAME_ENABLE_MARKUP                        = "enableMarkup";
const char* const PROPERTY_NAME_INPUT_COLOR                          = "inputColor";
const char* const PROPERTY_NAME_INPUT_FONT_FAMILY                    = "inputFontFamily";
const char* const PROPERTY_NAME_INPUT_FONT_STYLE                     = "inputFontStyle";
const char* const PROPERTY_NAME_INPUT_POINT_SIZE                     = "inputPointSize";

const char* const PROPERTY_NAME_LINE_SPACING                         = "lineSpacing";
const char* const PROPERTY_NAME_INPUT_LINE_SPACING                   = "inputLineSpacing";
const char* const PROPERTY_NAME_UNDERLINE                            = "underline";
const char* const PROPERTY_NAME_INPUT_UNDERLINE                      = "inputUnderline";
const char* const PROPERTY_NAME_SHADOW                               = "shadow";
const char* const PROPERTY_NAME_INPUT_SHADOW                         = "inputShadow";
const char* const PROPERTY_NAME_EMBOSS                               = "emboss";
const char* const PROPERTY_NAME_INPUT_EMBOSS                         = "inputEmboss";
const char* const PROPERTY_NAME_OUTLINE                              = "outline";
const char* const PROPERTY_NAME_INPUT_OUTLINE                        = "inputOutline";
const char* const PROPERTY_NAME_STRIKETHROUGH                        = "strikethrough";
const char* const PROPERTY_NAME_INPUT_STRIKETHROUGH                  = "inputStrikethrough";

const char* const PROPERTY_NAME_SMOOTH_SCROLL                        = "smoothScroll";
const char* const PROPERTY_NAME_SMOOTH_SCROLL_DURATION               = "smoothScrollDuration";
const char* const PROPERTY_NAME_ENABLE_SCROLL_BAR                    = "enableScrollBar";
const char* const PROPERTY_NAME_SCROLL_BAR_SHOW_DURATION             = "scrollBarShowDuration";
const char* const PROPERTY_NAME_SCROLL_BAR_FADE_DURATION             = "scrollBarFadeDuration";
const char* const PROPERTY_NAME_PIXEL_SIZE                           = "pixelSize";
const char* const PROPERTY_NAME_LINE_COUNT                           = "lineCount";
const char* const PROPERTY_NAME_PLACEHOLDER_TEXT                     = "placeholderText";
const char* const PROPERTY_NAME_PLACEHOLDER_TEXT_COLOR               = "placeholderTextColor";
const char* const PROPERTY_NAME_ENABLE_SELECTION                     = "enableSelection";
const char* const PROPERTY_NAME_PLACEHOLDER                          = "placeholder";
const char* const PROPERTY_NAME_ENABLE_SHIFT_SELECTION               = "enableShiftSelection";
const char* const PROPERTY_NAME_ENABLE_GRAB_HANDLE                   = "enableGrabHandle";
const char* const PROPERTY_NAME_MATCH_SYSTEM_LANGUAGE_DIRECTION      = "matchSystemLanguageDirection";
const char* const PROPERTY_NAME_MAX_LENGTH                           = "maxLength";
const char* const PROPERTY_NAME_FONT_SIZE_SCALE                      = "fontSizeScale";
const char* const PROPERTY_NAME_ENABLE_FONT_SIZE_SCALE               = "enableFontSizeScale";
const char* const PROPERTY_NAME_GRAB_HANDLE_COLOR                    = "grabHandleColor";
const char* const PROPERTY_NAME_ENABLE_GRAB_HANDLE_POPUP             = "enableGrabHandlePopup";
const char* const PROPERTY_NAME_INPUT_METHOD_SETTINGS                = "inputMethodSettings";
const char* const PROPERTY_NAME_INPUT_FILTER                         = "inputFilter";

const Vector4 PLACEHOLDER_TEXT_COLOR( 0.8f, 0.8f, 0.8f, 0.8f );
const Dali::Vector4 LIGHT_BLUE( 0.75f, 0.96f, 1.f, 1.f ); // The text highlight color.

const float RENDER_FRAME_INTERVAL = 16.66f;

const unsigned int DEFAULT_FONT_SIZE = 1152u;
const std::string DEFAULT_FONT_DIR( "/resources/fonts" );

const int KEY_A_CODE = 38;
const int KEY_D_CODE = 40;
const int KEY_C_CODE = 54;
const int KEY_V_CODE = 55;
const int KEY_X_CODE = 53;
const int KEY_WHITE_SPACE_CODE = 65;

const int KEY_SHIFT_MODIFIER = 257;
const int KEY_CONTROL_MODIFIER = 258;

const char* HANDLE_IMAGE_FILE_NAME = TEST_RESOURCE_DIR "/insertpoint-icon.png";
const char* HANDLE_LEFT_SELECTION_FILE_NAME = TEST_RESOURCE_DIR "/selection_handle_drop_left.png";
const char* HANDLE_RIGHT_SELECTION_FILE_NAME = TEST_RESOURCE_DIR "/selection_handle_drop_right.png";

const std::string DEFAULT_DEVICE_NAME("hwKeyboard");

static bool gSelectionChangedCallbackCalled;
static uint32_t oldSelectionStart;
static uint32_t oldSelectionEnd;
static bool gSelectionClearedCallbackCalled;
static bool gAnchorClickedCallBackCalled;
static bool gAnchorClickedCallBackNotCalled;
static bool gTextChangedCallBackCalled;
static bool gInputFilteredAcceptedCallbackCalled;
static bool gInputFilteredRejectedCallbackCalled;
static bool gInputStyleChangedCallbackCalled;
static bool gMaxCharactersCallBackCalled;
static bool gCursorPositionChangedCallbackCalled;
static uint32_t oldCursorPos;
static Dali::Toolkit::TextEditor::InputStyle::Mask gInputStyleMask;

struct CallbackFunctor
{
  CallbackFunctor(bool* callbackFlag)
  : mCallbackFlag( callbackFlag )
  {
  }

  void operator()()
  {
    *mCallbackFlag = true;
  }
  bool* mCallbackFlag;
};

static void TestSelectionClearedCallback(TextEditor control)
{
  tet_infoline(" TestSelectionClearedCallback");

  gSelectionClearedCallbackCalled = true;
}

static void TestSelectionChangedCallback(TextEditor control, uint32_t oldStart, uint32_t oldEnd)
{
  tet_infoline(" TestSelectionChangedCallback");

  gSelectionChangedCallbackCalled = true;
  oldSelectionStart = oldStart;
  oldSelectionEnd   = oldEnd;
}

static void TestAnchorClickedCallback(TextEditor control, const char* href, unsigned int hrefLength)
{
  tet_infoline(" TestAnchorClickedCallback");

  gAnchorClickedCallBackNotCalled = false;

  if (!strcmp(href, "https://www.tizen.org") && hrefLength == strlen(href))
  {
    gAnchorClickedCallBackCalled = true;
  }
}

static void TestCursorPositionChangedCallback( TextEditor control, unsigned int oldPos )
{
  tet_infoline(" TestCursorPositionChangedCallback");

  gCursorPositionChangedCallbackCalled = true;
  oldCursorPos = oldPos;
}

static void TestTextChangedCallback( TextEditor control )
{
  tet_infoline(" TestTextChangedCallback");

  gTextChangedCallBackCalled = true;
}

static void TestInputStyleChangedCallback( TextEditor control, TextEditor::InputStyle::Mask mask )
{
  tet_infoline(" TestInputStyleChangedCallback");

  gInputStyleChangedCallbackCalled = true;
  gInputStyleMask = mask;
}

static void TestMaxLengthReachedCallback( TextEditor control )
{
  tet_infoline(" TestMaxLengthReachedCallback");

  gMaxCharactersCallBackCalled = true;
}

static void TestInputFilteredCallback(TextEditor control, Toolkit::InputFilter::Property::Type type)
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

// Generate a KeyEvent to send to Core.
Integration::KeyEvent GenerateKey( const std::string& keyName,
                                   const std::string& logicalKey,
                                   const std::string& keyString,
                                   int keyCode,
                                   int keyModifier,
                                   unsigned long timeStamp,
                                   const Integration::KeyEvent::State& keyState,
                                   const std::string& compose = "",
                                   const std::string& deviceName = DEFAULT_DEVICE_NAME,
                                   const Device::Class::Type& deviceClass = Device::Class::NONE,
                                   const Device::Subclass::Type& deviceSubclass = Device::Subclass::NONE )
{
  return Integration::KeyEvent( keyName,
                                logicalKey,
                                keyString,
                                keyCode,
                                keyModifier,
                                timeStamp,
                                keyState,
                                compose,
                                deviceName,
                                deviceClass,
                                deviceSubclass );
}

Dali::Integration::Point GetPointDownInside( Vector2& pos )
{
  Dali::Integration::Point point;
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( pos );
  return point;
}

Dali::Integration::Point GetPointUpInside( Vector2& pos )
{
  Dali::Integration::Point point;
  point.SetState( PointState::UP );
  point.SetScreenPosition( pos );
  return point;
}

bool DaliTestCheckMaps( const Property::Map& fontStyleMapGet, const Property::Map& fontStyleMapSet )
{
  if( fontStyleMapGet.Count() == fontStyleMapSet.Count() )
  {
    for( unsigned int index = 0u; index < fontStyleMapGet.Count(); ++index )
    {
      const KeyValuePair& valueGet = fontStyleMapGet.GetKeyValue( index );

      Property::Value* valueSet = NULL;
      if ( valueGet.first.type == Property::Key::INDEX )
      {
        valueSet = fontStyleMapSet.Find( valueGet.first.indexKey );
      }
      else
      {
        // Get Key is a string so searching Set Map for a string key
        valueSet = fontStyleMapSet.Find( valueGet.first.stringKey );
      }

      if( NULL != valueSet )
      {
        if( valueSet->GetType() == Dali::Property::STRING && ( valueGet.second.Get<std::string>() != valueSet->Get<std::string>() ) )
        {
          tet_printf( "Value got : [%s], expected : [%s]", valueGet.second.Get<std::string>().c_str(), valueSet->Get<std::string>().c_str() );
          return false;
        }
        else if( valueSet->GetType() == Dali::Property::BOOLEAN && ( valueGet.second.Get<bool>() != valueSet->Get<bool>() ) )
        {
          tet_printf( "Value got : [%d], expected : [%d]", valueGet.second.Get<bool>(), valueSet->Get<bool>() );
          return false;
        }
        else if( valueSet->GetType() == Dali::Property::INTEGER && ( valueGet.second.Get<int>() != valueSet->Get<int>() ) )
        {
          tet_printf( "Value got : [%d], expected : [%d]", valueGet.second.Get<int>(), valueSet->Get<int>() );
          return false;
        }
        else if( valueSet->GetType() == Dali::Property::FLOAT && ( valueGet.second.Get<float>() != valueSet->Get<float>() ) )
        {
          tet_printf( "Value got : [%f], expected : [%f]", valueGet.second.Get<float>(), valueSet->Get<float>() );
          return false;
        }
        else if( valueSet->GetType() == Dali::Property::VECTOR2 && ( valueGet.second.Get<Vector2>() != valueSet->Get<Vector2>() ) )
        {
          Vector2 vector2Get = valueGet.second.Get<Vector2>();
          Vector2 vector2Set = valueSet->Get<Vector2>();
          tet_printf( "Value got : [%f, %f], expected : [%f, %f]", vector2Get.x, vector2Get.y, vector2Set.x, vector2Set.y );
          return false;
        }
        else if( valueSet->GetType() == Dali::Property::VECTOR4 && ( valueGet.second.Get<Vector4>() != valueSet->Get<Vector4>() ) )
        {
          Vector4 vector4Get = valueGet.second.Get<Vector4>();
          Vector4 vector4Set = valueSet->Get<Vector4>();
          tet_printf( "Value got : [%f, %f, %f, %f], expected : [%f, %f, %f, %f]", vector4Get.r, vector4Get.g, vector4Get.b, vector4Get.a, vector4Set.r, vector4Set.g, vector4Set.b, vector4Set.a );
          return false;
        }
      }
      else
      {
        if ( valueGet.first.type == Property::Key::INDEX )
        {
          tet_printf( "  The key %d doesn't exist.", valueGet.first.indexKey );
        }
        else
        {
          tet_printf( "  The key %s doesn't exist.", valueGet.first.stringKey.c_str() );
        }
        return false;
      }
    }
  }

  return true;
}

class ScrollStateChangeCallback : public Dali::ConnectionTracker
{
public:
  ScrollStateChangeCallback(bool& startedCalled, bool& finishedCalled)
  : mStartedCalled( startedCalled ),
    mFinishedCalled( finishedCalled )
  {
  }

  void Callback( TextEditor editor, TextEditor::Scroll::Type type )
  {
    if( type == TextEditor::Scroll::STARTED )
    {
      mStartedCalled = true;
    }
    else if( type == TextEditor::Scroll::FINISHED )
    {
      mFinishedCalled = true;
    }
  }

  bool& mStartedCalled;
  bool& mFinishedCalled;
};

} // namespace

int UtcDaliToolkitTextEditorConstructorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextEditorConstructorP");
  TextEditor textEditor;
  DALI_TEST_CHECK( !textEditor );
  END_TEST;
}

int UtcDaliToolkitTextEditorNewP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextEditorNewP");
  TextEditor textEditor = TextEditor::New();
  DALI_TEST_CHECK( textEditor );
  END_TEST;
}

int UtcDaliToolkitTextEditorDownCastP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextEditorDownCastP");
  TextEditor textEditor1 = TextEditor::New();
  BaseHandle object( textEditor1 );

  TextEditor textEditor2 = TextEditor::DownCast( object );
  DALI_TEST_CHECK( textEditor2 );

  TextEditor textEditor3 = DownCast< TextEditor >( object );
  DALI_TEST_CHECK( textEditor3 );
  END_TEST;
}

int UtcDaliToolkitTextEditorDownCastN(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextEditorDownCastN");
  BaseHandle uninitializedObject;
  TextEditor textEditor1 = TextEditor::DownCast( uninitializedObject );
  DALI_TEST_CHECK( !textEditor1 );

  TextEditor textEditor2 = DownCast< TextEditor >( uninitializedObject );
  DALI_TEST_CHECK( !textEditor2 );
  END_TEST;
}

int UtcDaliToolkitTextEditorCopyConstructorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextEditorCopyConstructorP");
  TextEditor textEditor = TextEditor::New();
  textEditor.SetProperty( TextEditor::Property::TEXT, "Test" );

  TextEditor copy( textEditor );
  DALI_TEST_CHECK( copy );
  DALI_TEST_CHECK( copy.GetProperty<std::string>( TextEditor::Property::TEXT ) == textEditor.GetProperty<std::string>( TextEditor::Property::TEXT ) );
  END_TEST;
}

int UtcDaliTextEditorMoveConstructor(void)
{
  ToolkitTestApplication application;

  TextEditor textEditor = TextEditor::New();
  textEditor.SetProperty( TextEditor::Property::TEXT, "Test" );
  DALI_TEST_CHECK( textEditor.GetProperty<std::string>( TextEditor::Property::TEXT ) == "Test" );

  TextEditor moved = std::move( textEditor );
  DALI_TEST_CHECK( moved );
  DALI_TEST_EQUALS( 1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION );
  DALI_TEST_CHECK( moved.GetProperty<std::string>( TextEditor::Property::TEXT ) == "Test" );
  DALI_TEST_CHECK( !textEditor );

  END_TEST;
}

int UtcDaliToolkitTextEditorAssignmentOperatorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextEditorAssignmentOperatorP");
  TextEditor textEditor = TextEditor::New();
  textEditor.SetProperty( TextEditor::Property::TEXT, "Test" );

  TextEditor copy = textEditor;
  DALI_TEST_CHECK( copy );
  DALI_TEST_CHECK( copy.GetProperty<std::string>( TextEditor::Property::TEXT ) == textEditor.GetProperty<std::string>( TextEditor::Property::TEXT ) );
  END_TEST;
}

int UtcDaliTextEditorMoveAssignment(void)
{
  ToolkitTestApplication application;

  TextEditor textEditor = TextEditor::New();
  textEditor.SetProperty( TextEditor::Property::TEXT, "Test" );
  DALI_TEST_CHECK( textEditor.GetProperty<std::string>( TextEditor::Property::TEXT ) == "Test" );

  TextEditor moved;
  moved = std::move( textEditor );
  DALI_TEST_CHECK( moved );
  DALI_TEST_EQUALS( 1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION );
  DALI_TEST_CHECK( moved.GetProperty<std::string>( TextEditor::Property::TEXT ) == "Test" );
  DALI_TEST_CHECK( !textEditor );

  END_TEST;
}

int UtcDaliTextEditorNewP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextEditorNewP");
  TextEditor textEditor = TextEditor::New();
  DALI_TEST_CHECK( textEditor );
  END_TEST;
}

// Positive test case for a method
int UtcDaliTextEditorGetPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextEditorGetPropertyP");
  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  // Check Property Indices are correct
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_RENDERING_BACKEND ) == DevelTextEditor::Property::RENDERING_BACKEND );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_TEXT ) == TextEditor::Property::TEXT );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_TEXT_COLOR ) == TextEditor::Property::TEXT_COLOR );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_FONT_FAMILY ) == TextEditor::Property::FONT_FAMILY );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_FONT_STYLE ) == TextEditor::Property::FONT_STYLE );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_POINT_SIZE ) == TextEditor::Property::POINT_SIZE );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_HORIZONTAL_ALIGNMENT ) == TextEditor::Property::HORIZONTAL_ALIGNMENT );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_SCROLL_THRESHOLD ) == TextEditor::Property::SCROLL_THRESHOLD );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_SCROLL_SPEED ) == TextEditor::Property::SCROLL_SPEED );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_PRIMARY_CURSOR_COLOR ) == TextEditor::Property::PRIMARY_CURSOR_COLOR );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_SECONDARY_CURSOR_COLOR ) == TextEditor::Property::SECONDARY_CURSOR_COLOR );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_ENABLE_CURSOR_BLINK ) == TextEditor::Property::ENABLE_CURSOR_BLINK );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_CURSOR_BLINK_INTERVAL ) == TextEditor::Property::CURSOR_BLINK_INTERVAL );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_CURSOR_BLINK_DURATION ) == TextEditor::Property::CURSOR_BLINK_DURATION );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_CURSOR_WIDTH ) == TextEditor::Property::CURSOR_WIDTH );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_GRAB_HANDLE_IMAGE ) == TextEditor::Property::GRAB_HANDLE_IMAGE );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_GRAB_HANDLE_PRESSED_IMAGE ) == TextEditor::Property::GRAB_HANDLE_PRESSED_IMAGE );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_SELECTION_HANDLE_IMAGE_LEFT ) == TextEditor::Property::SELECTION_HANDLE_IMAGE_LEFT );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_SELECTION_HANDLE_IMAGE_RIGHT ) == TextEditor::Property::SELECTION_HANDLE_IMAGE_RIGHT );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_LEFT ) == TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_RIGHT ) == TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_SELECTION_HANDLE_MARKER_IMAGE_LEFT ) == TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_SELECTION_HANDLE_MARKER_IMAGE_RIGHT ) == TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_SELECTION_HIGHLIGHT_COLOR ) == TextEditor::Property::SELECTION_HIGHLIGHT_COLOR );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_DECORATION_BOUNDING_BOX ) == TextEditor::Property::DECORATION_BOUNDING_BOX );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_ENABLE_MARKUP ) == TextEditor::Property::ENABLE_MARKUP );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_INPUT_COLOR ) == TextEditor::Property::INPUT_COLOR );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_INPUT_FONT_FAMILY ) == TextEditor::Property::INPUT_FONT_FAMILY );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_INPUT_FONT_STYLE ) == TextEditor::Property::INPUT_FONT_STYLE );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_INPUT_POINT_SIZE ) == TextEditor::Property::INPUT_POINT_SIZE );

  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_LINE_SPACING ) == TextEditor::Property::LINE_SPACING );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_INPUT_LINE_SPACING ) == TextEditor::Property::INPUT_LINE_SPACING );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_UNDERLINE ) == TextEditor::Property::UNDERLINE );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_INPUT_UNDERLINE ) == TextEditor::Property::INPUT_UNDERLINE );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_SHADOW ) == TextEditor::Property::SHADOW );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_INPUT_SHADOW ) == TextEditor::Property::INPUT_SHADOW );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_EMBOSS ) == TextEditor::Property::EMBOSS );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_INPUT_EMBOSS ) == TextEditor::Property::INPUT_EMBOSS );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_OUTLINE ) == TextEditor::Property::OUTLINE );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_INPUT_OUTLINE ) == TextEditor::Property::INPUT_OUTLINE );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_STRIKETHROUGH ) == DevelTextEditor::Property::STRIKETHROUGH );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_INPUT_STRIKETHROUGH ) == DevelTextEditor::Property::INPUT_STRIKETHROUGH );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_SMOOTH_SCROLL ) == TextEditor::Property::SMOOTH_SCROLL );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_SMOOTH_SCROLL_DURATION ) == TextEditor::Property::SMOOTH_SCROLL_DURATION );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_ENABLE_SCROLL_BAR ) == TextEditor::Property::ENABLE_SCROLL_BAR );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_SCROLL_BAR_SHOW_DURATION ) == TextEditor::Property::SCROLL_BAR_SHOW_DURATION );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_SCROLL_BAR_FADE_DURATION ) == TextEditor::Property::SCROLL_BAR_FADE_DURATION );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_PIXEL_SIZE ) == TextEditor::Property::PIXEL_SIZE );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_LINE_COUNT) == TextEditor::Property::LINE_COUNT );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_ENABLE_SELECTION ) == TextEditor::Property::ENABLE_SELECTION );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_PLACEHOLDER ) == TextEditor::Property::PLACEHOLDER );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_FONT_SIZE_SCALE ) == DevelTextEditor::Property::FONT_SIZE_SCALE );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_ENABLE_FONT_SIZE_SCALE ) == DevelTextEditor::Property::ENABLE_FONT_SIZE_SCALE );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_PLACEHOLDER_TEXT ) == DevelTextEditor::Property::PLACEHOLDER_TEXT );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_PLACEHOLDER_TEXT_COLOR ) == DevelTextEditor::Property::PLACEHOLDER_TEXT_COLOR );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_ENABLE_SHIFT_SELECTION ) == DevelTextEditor::Property::ENABLE_SHIFT_SELECTION );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_ENABLE_GRAB_HANDLE ) == DevelTextEditor::Property::ENABLE_GRAB_HANDLE );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_MATCH_SYSTEM_LANGUAGE_DIRECTION ) == DevelTextEditor::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_MAX_LENGTH ) == DevelTextEditor::Property::MAX_LENGTH );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_GRAB_HANDLE_COLOR ) == DevelTextEditor::Property::GRAB_HANDLE_COLOR );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_ENABLE_GRAB_HANDLE_POPUP ) == DevelTextEditor::Property::ENABLE_GRAB_HANDLE_POPUP );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_INPUT_METHOD_SETTINGS ) == DevelTextEditor::Property::INPUT_METHOD_SETTINGS );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_INPUT_FILTER ) == DevelTextEditor::Property::INPUT_FILTER );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_STRIKETHROUGH ) == DevelTextEditor::Property::STRIKETHROUGH );
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_INPUT_STRIKETHROUGH ) == DevelTextEditor::Property::INPUT_STRIKETHROUGH );

  END_TEST;
}

bool SetPropertyMapRetrieved( TextEditor& editor, const Property::Index property, const std::string mapKey, const std::string mapValue )
{
  bool result = false;
  Property::Map imageMap;
  imageMap[mapKey] =mapValue;

  editor.SetProperty( property , imageMap );
  Property::Value propValue = editor.GetProperty( property );
  Property::Map* resultMap = propValue.GetMap();

  if ( resultMap->Find( mapKey )->Get< std::string>() == mapValue )
  {
    result = true;
  }

  return result;
}

// Positive test case for a method
int UtcDaliTextEditorSetPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextEditorSetPropertyP");
  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );
  application.GetScene().Add( editor );

  // Note - we can't check the defaults since the stylesheets are platform-specific

  // Check the render backend property.
  editor.SetProperty( DevelTextEditor::Property::RENDERING_BACKEND, DevelText::RENDERING_SHARED_ATLAS );
  DALI_TEST_EQUALS( (DevelText::RenderingType)editor.GetProperty<int>( DevelTextEditor::Property::RENDERING_BACKEND ), DevelText::RENDERING_SHARED_ATLAS, TEST_LOCATION );

  // Check text property.
  editor.SetProperty( TextEditor::Property::TEXT, "Setting Text" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::TEXT ), std::string("Setting Text"), TEST_LOCATION );

  // Check text's color property
  editor.SetProperty( TextEditor::Property::TEXT_COLOR, Color::WHITE );
  DALI_TEST_EQUALS( editor.GetProperty<Vector4>( TextEditor::Property::TEXT_COLOR ), Color::WHITE, TEST_LOCATION );

  // Check font properties.
  editor.SetProperty( TextEditor::Property::FONT_FAMILY, "Setting font family" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::FONT_FAMILY ), std::string("Setting font family"), TEST_LOCATION );

  Property::Map fontStyleMapSet;
  Property::Map fontStyleMapGet;
  Property::Value* slantValue = NULL;

  fontStyleMapSet.Insert( "weight", "bold" );
  fontStyleMapSet.Insert( "width", "condensed" );
  fontStyleMapSet.Insert( "slant", "italic" );

  editor.SetProperty( TextEditor::Property::FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10.f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::POINT_SIZE ), 10.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  editor.SetProperty( DevelTextEditor::Property::FONT_SIZE_SCALE, 2.5f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( DevelTextEditor::Property::FONT_SIZE_SCALE ), 2.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  editor.SetProperty( DevelTextEditor::Property::FONT_SIZE_SCALE, 1.0f );

  editor.SetProperty( DevelTextEditor::Property::ENABLE_FONT_SIZE_SCALE, false );
  DALI_TEST_EQUALS( editor.GetProperty<bool>( DevelTextEditor::Property::ENABLE_FONT_SIZE_SCALE ), false, TEST_LOCATION );
  editor.SetProperty( DevelTextEditor::Property::ENABLE_FONT_SIZE_SCALE, true );

  // Reset font style.
  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "weight", "normal" );
  fontStyleMapSet.Insert( "slant", "oblique" );
  editor.SetProperty( TextEditor::Property::FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "slant", "roman" );
  editor.SetProperty( TextEditor::Property::FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::FONT_STYLE );

  // Replace 'roman' for 'normal'.
  slantValue = fontStyleMapGet.Find( "slant" );
  if( NULL != slantValue )
  {
    if( "normal" == slantValue->Get<std::string>() )
    {
      fontStyleMapGet["slant"] = "roman";
    }
  }
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  fontStyleMapSet.Clear();

  editor.SetProperty( TextEditor::Property::FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  // Check that the Alignment properties can be correctly set
  editor.SetProperty( TextEditor::Property::HORIZONTAL_ALIGNMENT, "END" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::HORIZONTAL_ALIGNMENT ), "END", TEST_LOCATION );

  // Check scroll properties.
  editor.SetProperty( TextEditor::Property::SCROLL_THRESHOLD, 1.f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::SCROLL_THRESHOLD ), 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::SCROLL_SPEED, 100.f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::SCROLL_SPEED ), 100.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Check cursor properties
  editor.SetProperty( TextEditor::Property::PRIMARY_CURSOR_COLOR, Color::RED );
  DALI_TEST_EQUALS( editor.GetProperty<Vector4>( TextEditor::Property::PRIMARY_CURSOR_COLOR ), Color::RED, TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::SECONDARY_CURSOR_COLOR, Color::BLUE );
  DALI_TEST_EQUALS( editor.GetProperty<Vector4>( TextEditor::Property::SECONDARY_CURSOR_COLOR ), Color::BLUE, TEST_LOCATION );

  editor.SetProperty( TextEditor::Property::ENABLE_CURSOR_BLINK, false );
  DALI_TEST_EQUALS( editor.GetProperty<bool>( TextEditor::Property::ENABLE_CURSOR_BLINK ), false, TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::CURSOR_BLINK_INTERVAL, 1.f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::CURSOR_BLINK_INTERVAL ), 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::CURSOR_BLINK_DURATION, 10.f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::CURSOR_BLINK_DURATION ), 10.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::CURSOR_WIDTH, 1 );
  DALI_TEST_EQUALS( editor.GetProperty<int>( TextEditor::Property::CURSOR_WIDTH ), 1, TEST_LOCATION );

  // Check handle images
  editor.SetProperty( TextEditor::Property::GRAB_HANDLE_IMAGE, "image1" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::GRAB_HANDLE_IMAGE ), "image1", TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::GRAB_HANDLE_PRESSED_IMAGE, "image2" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::GRAB_HANDLE_PRESSED_IMAGE ), "image2", TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::SELECTION_HANDLE_IMAGE_LEFT, "image3" );

  // Check handle images
  DALI_TEST_CHECK( SetPropertyMapRetrieved( editor, TextEditor::Property::SELECTION_HANDLE_IMAGE_LEFT, "filename", "leftHandleImage" )  );
  DALI_TEST_CHECK( SetPropertyMapRetrieved( editor, TextEditor::Property::SELECTION_HANDLE_IMAGE_RIGHT, "filename", "rightHandleImage" )  );
  DALI_TEST_CHECK( SetPropertyMapRetrieved( editor, TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT, "filename", "leftHandleImagePressed" )  );
  DALI_TEST_CHECK( SetPropertyMapRetrieved( editor, TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT, "filename", "rightHandleImagePressed" )  );
  DALI_TEST_CHECK( SetPropertyMapRetrieved( editor, TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT, "filename", "leftHandleMarkerImage" )  );
  DALI_TEST_CHECK( SetPropertyMapRetrieved( editor, TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT, "filename", "rightHandleMarkerImage" )  );

  // Check the highlight color
  editor.SetProperty( TextEditor::Property::SELECTION_HIGHLIGHT_COLOR, Color::GREEN );
  DALI_TEST_EQUALS( editor.GetProperty<Vector4>( TextEditor::Property::SELECTION_HIGHLIGHT_COLOR ), Color::GREEN, TEST_LOCATION );

  // Decoration bounding box
  editor.SetProperty( TextEditor::Property::DECORATION_BOUNDING_BOX, Rect<int>( 0, 0, 1, 1 ) );
  DALI_TEST_EQUALS( editor.GetProperty<Rect <int > >( TextEditor::Property::DECORATION_BOUNDING_BOX ), Rect<int>( 0, 0, 1, 1 ), TEST_LOCATION );

  // Check the enable markup property.
  DALI_TEST_CHECK( !editor.GetProperty<bool>( TextEditor::Property::ENABLE_MARKUP ) );
  editor.SetProperty( TextEditor::Property::ENABLE_MARKUP, true );
  DALI_TEST_CHECK( editor.GetProperty<bool>( TextEditor::Property::ENABLE_MARKUP ) );

  // Check input color property.
  editor.SetProperty( TextEditor::Property::INPUT_COLOR, Color::YELLOW );
  DALI_TEST_EQUALS( editor.GetProperty<Vector4>( TextEditor::Property::INPUT_COLOR ), Color::YELLOW, TEST_LOCATION );

  // Check input font properties.
  editor.SetProperty( TextEditor::Property::INPUT_FONT_FAMILY, "Setting input font family" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::INPUT_FONT_FAMILY ), "Setting input font family", TEST_LOCATION );

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "weight", "bold" );
  fontStyleMapSet.Insert( "width", "condensed" );
  fontStyleMapSet.Insert( "slant", "italic" );

  editor.SetProperty( TextEditor::Property::INPUT_FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::INPUT_FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  editor.SetProperty( TextEditor::Property::INPUT_POINT_SIZE, 12.f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::INPUT_POINT_SIZE ), 12.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Reset input font style.
  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "weight", "normal" );
  fontStyleMapSet.Insert( "slant", "oblique" );

  editor.SetProperty( TextEditor::Property::INPUT_FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::INPUT_FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "slant", "roman" );

  editor.SetProperty( TextEditor::Property::INPUT_FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::INPUT_FONT_STYLE );

  // Replace 'roman' for 'normal'.
  slantValue = fontStyleMapGet.Find( "slant" );
  if( NULL != slantValue )
  {
    if( "normal" == slantValue->Get<std::string>() )
    {
      fontStyleMapGet["slant"] = "roman";
    }
  }
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  fontStyleMapSet.Clear();

  editor.SetProperty( TextEditor::Property::INPUT_FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::INPUT_FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  // Check the line spacing property
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::LINE_SPACING ), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::LINE_SPACING, 10.f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::LINE_SPACING ), 10.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Check the input line spacing property
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::INPUT_LINE_SPACING ), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::INPUT_LINE_SPACING, 20.f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::INPUT_LINE_SPACING ), 20.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Check the underline property

  Property::Map underlineMapSet;
  Property::Map underlineMapGet;

  underlineMapSet.Insert( "enable", true );
  underlineMapSet.Insert( "color", Color::RED );
  underlineMapSet.Insert( "height", 1 );

  editor.SetProperty( TextEditor::Property::UNDERLINE, underlineMapSet );

  underlineMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::UNDERLINE );
  DALI_TEST_EQUALS( underlineMapGet.Count(), underlineMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( underlineMapGet, underlineMapSet ), true, TEST_LOCATION );

  // Check the input underline property
  editor.SetProperty( TextEditor::Property::INPUT_UNDERLINE, "Underline input properties" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::INPUT_UNDERLINE ), std::string("Underline input properties"), TEST_LOCATION );

  // Check the shadow property
  Property::Map shadowMapSet;
  Property::Map shadowMapGet;

  shadowMapSet.Insert( "color", Color::GREEN );
  shadowMapSet.Insert( "offset", Vector2(2.0f, 2.0f) );
  shadowMapSet.Insert( "blurRadius", 3.0f );

  editor.SetProperty( TextEditor::Property::SHADOW, shadowMapSet );

  shadowMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::SHADOW );
  DALI_TEST_EQUALS( shadowMapGet.Count(), shadowMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( shadowMapGet, shadowMapSet ), true, TEST_LOCATION );

  // Check the input shadow property
  editor.SetProperty( TextEditor::Property::INPUT_SHADOW, "Shadow input properties" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::INPUT_SHADOW ), std::string("Shadow input properties"), TEST_LOCATION );

  // Check the emboss property
  editor.SetProperty( TextEditor::Property::EMBOSS, "Emboss properties" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::EMBOSS ), std::string("Emboss properties"), TEST_LOCATION );

  // Check the input emboss property
  editor.SetProperty( TextEditor::Property::INPUT_EMBOSS, "Emboss input properties" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::INPUT_EMBOSS ), std::string("Emboss input properties"), TEST_LOCATION );

  // Check the outline property

  // Test string type first
  // This is purely to maintain backward compatibility, but we don't support string as the outline property type.
  editor.SetProperty( TextEditor::Property::OUTLINE, "Outline properties" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::OUTLINE ), std::string("Outline properties"), TEST_LOCATION );

  // Then test the property map type
  Property::Map outlineMapSet;
  Property::Map outlineMapGet;

  outlineMapSet["color"] = Color::RED;
  outlineMapSet["width"] = 2.0f;

  editor.SetProperty( TextEditor::Property::OUTLINE, outlineMapSet );

  outlineMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::OUTLINE );
  DALI_TEST_EQUALS( outlineMapGet.Count(), outlineMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( outlineMapGet, outlineMapSet ), true, TEST_LOCATION );

  // Check the input outline property
  editor.SetProperty( TextEditor::Property::INPUT_OUTLINE, "Outline input properties" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::INPUT_OUTLINE ), std::string("Outline input properties"), TEST_LOCATION );

  // Check the smooth scroll property
  DALI_TEST_EQUALS( editor.GetProperty<bool>( TextEditor::Property::SMOOTH_SCROLL ), false, TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::SMOOTH_SCROLL, true );
  DALI_TEST_EQUALS( editor.GetProperty<bool>( TextEditor::Property::SMOOTH_SCROLL ), true, TEST_LOCATION );

  // Check the smooth scroll duration property
  editor.SetProperty( TextEditor::Property::SMOOTH_SCROLL_DURATION, 0.2f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::SMOOTH_SCROLL_DURATION ), 0.2f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Check the scroll bar property
  DALI_TEST_EQUALS( editor.GetProperty<bool>( TextEditor::Property::ENABLE_SCROLL_BAR ), false, TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::ENABLE_SCROLL_BAR, true );
  DALI_TEST_EQUALS( editor.GetProperty<bool>( TextEditor::Property::ENABLE_SCROLL_BAR ), true, TEST_LOCATION );

  editor.SetProperty( TextEditor::Property::SCROLL_BAR_SHOW_DURATION, 0.3f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::SCROLL_BAR_SHOW_DURATION ), 0.3f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::SCROLL_BAR_FADE_DURATION, 0.2f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::SCROLL_BAR_FADE_DURATION ), 0.2f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Check the pixel size of font
  editor.SetProperty( TextEditor::Property::PIXEL_SIZE, 20.f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::PIXEL_SIZE ), 20.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Check placeholder text properties.
  editor.SetProperty( DevelTextEditor::Property::PLACEHOLDER_TEXT, "Setting Placeholder Text" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( DevelTextEditor::Property::PLACEHOLDER_TEXT ), std::string("Setting Placeholder Text"), TEST_LOCATION );

  // Check placeholder text's color property.
  editor.SetProperty( DevelTextEditor::Property::PLACEHOLDER_TEXT_COLOR, Color::RED );
  DALI_TEST_EQUALS( editor.GetProperty<Vector4>( DevelTextEditor::Property::PLACEHOLDER_TEXT_COLOR ), Color::RED, TEST_LOCATION );

  // Check the enable selection property
  editor.SetProperty( TextEditor::Property::ENABLE_SELECTION, false );
  DALI_TEST_EQUALS( editor.GetProperty<bool>( TextEditor::Property::ENABLE_SELECTION ), false, TEST_LOCATION );

  // Check the placeholder property with pixel size
  Property::Map placeholderPixelSizeMapSet;
  Property::Map placeholderPixelSizeMapGet;
  Property::Map placeholderFontstyleMap;
  placeholderPixelSizeMapSet["text"] = "Setting Placeholder Text";
  placeholderPixelSizeMapSet["textFocused"] = "Setting Placeholder Text Focused";
  placeholderPixelSizeMapSet["color"] = Color::BLUE;
  placeholderPixelSizeMapSet["fontFamily"] = "Arial";
  placeholderPixelSizeMapSet["pixelSize"] = 15.0f;

  placeholderFontstyleMap.Insert( "weight", "bold" );
  placeholderPixelSizeMapSet["fontStyle"] = placeholderFontstyleMap;
  editor.SetProperty( TextEditor::Property::PLACEHOLDER, placeholderPixelSizeMapSet );

  placeholderPixelSizeMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::PLACEHOLDER );
  DALI_TEST_EQUALS( placeholderPixelSizeMapGet.Count(), placeholderPixelSizeMapSet.Count(), TEST_LOCATION );

  tet_infoline("Test Placeholder settings set as strings is converted correctly to Property Index key and holds set value");
  Property::Map placeholderConversionMap;
  placeholderConversionMap[ Text::PlaceHolder::Property::TEXT ] = placeholderPixelSizeMapSet["text"];
  placeholderConversionMap[ Text::PlaceHolder::Property::TEXT_FOCUSED ] = placeholderPixelSizeMapSet["textFocused"] ;
  placeholderConversionMap[ Text::PlaceHolder::Property::COLOR ] = placeholderPixelSizeMapSet["color"];
  placeholderConversionMap[ Text::PlaceHolder::Property::FONT_STYLE ] = placeholderPixelSizeMapSet["fontStyle"];
  placeholderConversionMap[ Text::PlaceHolder::Property::FONT_FAMILY ] = placeholderPixelSizeMapSet["fontFamily"];
  placeholderConversionMap[ Text::PlaceHolder::Property::PIXEL_SIZE ] = placeholderPixelSizeMapSet["pixelSize"];

  DALI_TEST_EQUALS( DaliTestCheckMaps( placeholderPixelSizeMapGet, placeholderConversionMap ), true, TEST_LOCATION );

  // Check the placeholder property with point size
  Property::Map placeholderMapSet;
  Property::Map placeholderMapGet;
  placeholderMapSet["text"] = "Setting Placeholder Text";
  placeholderMapSet["textFocused"] = "Setting Placeholder Text Focused";
  placeholderMapSet["color"] = Color::RED;
  placeholderMapSet["fontFamily"] = "Arial";
  placeholderMapSet["pointSize"] = 12.0f;
  // Check the placeholder font style property
  placeholderFontstyleMap.Clear();

  placeholderFontstyleMap.Insert( "weight", "bold" );
  placeholderFontstyleMap.Insert( "width", "condensed" );
  placeholderFontstyleMap.Insert( "slant", "italic" );
  placeholderMapSet["fontStyle"] = placeholderFontstyleMap;
  editor.SetProperty( TextEditor::Property::PLACEHOLDER, placeholderMapSet );

  placeholderMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::PLACEHOLDER );
  DALI_TEST_EQUALS( placeholderMapGet.Count(), placeholderMapSet.Count(), TEST_LOCATION );

  tet_infoline("Test Placeholder settings set as strings is converted correctly to Property Index key and holds set value");
  placeholderConversionMap.Clear();
  placeholderConversionMap[ Text::PlaceHolder::Property::TEXT ] = placeholderMapSet["text"];
  placeholderConversionMap[ Text::PlaceHolder::Property::TEXT_FOCUSED ] = placeholderMapSet["textFocused"] ;
  placeholderConversionMap[ Text::PlaceHolder::Property::COLOR ] = placeholderMapSet["color"];
  placeholderConversionMap[ Text::PlaceHolder::Property::FONT_STYLE ] = placeholderPixelSizeMapSet["fontStyle"];
  placeholderConversionMap[ Text::PlaceHolder::Property::FONT_FAMILY ] = placeholderMapSet["fontFamily"];
  placeholderConversionMap[ Text::PlaceHolder::Property::POINT_SIZE ] = placeholderMapSet["pointSize"];
  DALI_TEST_EQUALS( DaliTestCheckMaps( placeholderMapGet, placeholderConversionMap ), true, TEST_LOCATION );

  // Reset font style.
  placeholderFontstyleMap.Clear();
  placeholderFontstyleMap.Insert( "weight", "normal" );
  placeholderFontstyleMap.Insert( "slant", "oblique" );
  placeholderMapSet["fontStyle"] = placeholderFontstyleMap;
  editor.SetProperty( TextEditor::Property::PLACEHOLDER, placeholderMapSet );

  placeholderMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::PLACEHOLDER );
  DALI_TEST_EQUALS( placeholderMapGet.Count(), placeholderMapSet.Count(), TEST_LOCATION );
  placeholderConversionMap[ Text::PlaceHolder::Property::FONT_STYLE ] = placeholderMapSet["fontStyle"];
  DALI_TEST_EQUALS( DaliTestCheckMaps( placeholderMapGet, placeholderConversionMap ), true, TEST_LOCATION );

  placeholderFontstyleMap.Clear();
  placeholderFontstyleMap.Insert( "slant", "roman" );
  placeholderMapSet["fontStyle"] = placeholderFontstyleMap;
  editor.SetProperty( TextEditor::Property::PLACEHOLDER, placeholderMapSet );

  placeholderMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::PLACEHOLDER );

  placeholderFontstyleMap.Clear();
  placeholderMapSet["fontStyle"] = placeholderFontstyleMap;

  editor.SetProperty( TextEditor::Property::PLACEHOLDER, placeholderMapSet );
  placeholderMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::PLACEHOLDER );
  DALI_TEST_EQUALS( placeholderMapGet.Count(), placeholderMapSet.Count(), TEST_LOCATION );
  placeholderConversionMap[ Text::PlaceHolder::Property::FONT_STYLE ] = placeholderMapSet["fontStyle"];
  DALI_TEST_EQUALS( DaliTestCheckMaps( placeholderMapGet, placeholderConversionMap ), true, TEST_LOCATION );

  editor.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT );
  DALI_TEST_EQUALS( editor.GetProperty<int>( Actor::Property::LAYOUT_DIRECTION ), static_cast<int>( LayoutDirection::RIGHT_TO_LEFT ), TEST_LOCATION );

  // Check handle color
  editor.SetProperty( DevelTextEditor::Property::GRAB_HANDLE_COLOR, Color::GREEN );
  DALI_TEST_EQUALS( editor.GetProperty<Vector4>( DevelTextEditor::Property::GRAB_HANDLE_COLOR ), Color::GREEN, TEST_LOCATION );

  // Test the ENABLE_GRAB_HANDLE_POPUP property
  editor.SetProperty( DevelTextEditor::Property::ENABLE_GRAB_HANDLE_POPUP, false );
  DALI_TEST_EQUALS( editor.GetProperty<bool>( DevelTextEditor::Property::ENABLE_GRAB_HANDLE_POPUP ), false, TEST_LOCATION);

  // Check the input method setting
  Property::Map propertyMap;
  InputMethod::PanelLayout::Type panelLayout = InputMethod::PanelLayout::NUMBER;
  InputMethod::AutoCapital::Type autoCapital = InputMethod::AutoCapital::WORD;
  InputMethod::ButtonAction::Type buttonAction = InputMethod::ButtonAction::GO;
  int inputVariation = 1;
  propertyMap["PANEL_LAYOUT"] = panelLayout;
  propertyMap["AUTO_CAPITALIZE"] = autoCapital;
  propertyMap["BUTTON_ACTION"] = buttonAction;
  propertyMap["VARIATION"] = inputVariation;
  editor.SetProperty( DevelTextEditor::Property::INPUT_METHOD_SETTINGS, propertyMap );

  Property::Value value = editor.GetProperty( DevelTextEditor::Property::INPUT_METHOD_SETTINGS );
  Property::Map map;
  DALI_TEST_CHECK( value.Get( map ) );

  int layout = 0;
  DALI_TEST_CHECK( map[ "PANEL_LAYOUT" ].Get( layout ) );
  DALI_TEST_EQUALS( static_cast<int>(panelLayout), layout, TEST_LOCATION );

  int capital = 0;
  DALI_TEST_CHECK( map[ "AUTO_CAPITALIZE" ].Get( capital ) );
  DALI_TEST_EQUALS( static_cast<int>(autoCapital), capital, TEST_LOCATION );

  int action = 0;
  DALI_TEST_CHECK( map[ "BUTTON_ACTION" ].Get( action ) );
  DALI_TEST_EQUALS( static_cast<int>(buttonAction), action, TEST_LOCATION );

  int variation = 0;
  DALI_TEST_CHECK( map[ "VARIATION" ].Get( variation ) );
  DALI_TEST_EQUALS( inputVariation, variation, TEST_LOCATION );

  // Check the input filter property
  Property::Map inputFilterMapSet;
  Property::Map inputFilterMapGet;
  inputFilterMapSet[InputFilter::Property::ACCEPTED] = "[\\w]";
  inputFilterMapSet[InputFilter::Property::REJECTED] = "[\\d]";

  editor.SetProperty(DevelTextEditor::Property::INPUT_FILTER, inputFilterMapSet);

  inputFilterMapGet = editor.GetProperty<Property::Map>(DevelTextEditor::Property::INPUT_FILTER);
  DALI_TEST_EQUALS(inputFilterMapGet.Count(), inputFilterMapSet.Count(), TEST_LOCATION);

  // Clear
  inputFilterMapSet.Clear();
  editor.SetProperty(DevelTextEditor::Property::INPUT_FILTER, inputFilterMapSet);

  // Check the strikethrough property

  Property::Map strikethroughMapSet;
  Property::Map strikethroughMapGet;

  application.SendNotification();
  application.Render();

  // Check the input strikethrough property

  strikethroughMapSet.Clear();
  strikethroughMapGet.Clear();
  strikethroughMapSet.Insert( "enable", true );
  strikethroughMapSet.Insert( "color", Color::BLUE );
  strikethroughMapSet.Insert( "height", 2.0f );

  editor.SetProperty( DevelTextEditor::Property::STRIKETHROUGH, strikethroughMapSet );

  application.SendNotification();
  application.Render();

  strikethroughMapGet = editor.GetProperty<Property::Map>( DevelTextEditor::Property::STRIKETHROUGH );

  DALI_TEST_EQUALS( strikethroughMapGet.Count(), strikethroughMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( strikethroughMapSet, strikethroughMapGet ), true,  TEST_LOCATION );

  // Check the input strikethrough property
  editor.SetProperty( DevelTextEditor::Property::INPUT_STRIKETHROUGH, "Strikethrough input properties" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( DevelTextEditor::Property::INPUT_STRIKETHROUGH ), std::string("Strikethrough input properties"), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  // Check the line size property
  DALI_TEST_EQUALS( editor.GetProperty<float>( DevelTextEditor::Property::MIN_LINE_SIZE ), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  editor.SetProperty( DevelTextEditor::Property::MIN_LINE_SIZE, 50.f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( DevelTextEditor::Property::MIN_LINE_SIZE ), 50.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  END_TEST;
}

// Positive Atlas Text Renderer test
int utcDaliTextEditorAtlasRenderP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextEditorAtlasRenderP");
  StyleManager styleManager = StyleManager::Get();
  styleManager.ApplyDefaultTheme();
  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  editor.SetProperty( TextEditor::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  application.GetScene().Add( editor );

  try
  {
    // Render some text with the shared atlas backend
    editor.SetProperty( DevelTextEditor::Property::RENDERING_BACKEND, DevelText::RENDERING_SHARED_ATLAS );
    application.SendNotification();
    application.Render();
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }
  END_TEST;
}

// Positive test for the anchorClicked signal.
int utcDaliTextEditorAnchorClickedP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorAnchorClickedP");
  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK(editor);

  application.GetScene().Add(editor);

  // connect to the anchor clicked signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextEditor::AnchorClickedSignal(editor).Connect(&TestAnchorClickedCallback);
  bool anchorClickedSignal = false;
  editor.ConnectSignal(testTracker, "anchorClicked", CallbackFunctor(&anchorClickedSignal));

  gAnchorClickedCallBackCalled = false;
  editor.SetProperty(TextEditor::Property::TEXT, "<a href='https://www.tizen.org'>TIZEN</a>");
  editor.SetProperty(TextEditor::Property::ENABLE_MARKUP, true);
  editor.SetProperty(Actor::Property::SIZE, Vector2(100.f, 50.f));
  editor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  editor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  application.SendNotification();
  application.Render();
  editor.SetKeyInputFocus();

  // Create a tap event to touch the text editor.
  TestGenerateTap(application, 5.0f, 5.0f);
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

// Positive test for the textChanged signal.
int utcDaliTextEditorTextChangedP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorTextChangedP");
  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  editor.TextChangedSignal().Connect( &TestTextChangedCallback );
  bool textChangedSignal = false;
  editor.ConnectSignal( testTracker, "textChanged",   CallbackFunctor(&textChangedSignal) );

  gTextChangedCallBackCalled = false;
  editor.SetProperty( TextEditor::Property::TEXT, "ABC" );
  DALI_TEST_CHECK( gTextChangedCallBackCalled );
  DALI_TEST_CHECK( textChangedSignal );

  application.SendNotification();
  editor.SetKeyInputFocus();

  gTextChangedCallBackCalled = false;
  application.ProcessEvent( GenerateKey( "D", "", "D", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "D", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  DALI_TEST_CHECK( gTextChangedCallBackCalled );

  // Remove all text
  editor.SetProperty( TextField::Property::TEXT, "" );

  // Pressing backspace key: TextChangedCallback should not be called when there is no text in texteditor.
  gTextChangedCallBackCalled = false;
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  DALI_TEST_CHECK( !gTextChangedCallBackCalled );

  // Pressing delete key: TextChangedCallback should not be called when there is no text in texteditor.
  gTextChangedCallBackCalled = false;
  application.ProcessEvent( GenerateKey( "", "", "", Dali::DevelKey::DALI_KEY_DELETE, 0, 0, Integration::KeyEvent::DOWN, "Delete", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  DALI_TEST_CHECK( !gTextChangedCallBackCalled );

  END_TEST;
}

int utcDaliTextEditorTextChangedWithInputMethodContext(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorTextChangedWithInputMethodContext");
  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );


  application.GetScene().Add( editor );

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  editor.TextChangedSignal().Connect(&TestTextChangedCallback);
  bool textChangedSignal = false;
  editor.ConnectSignal( testTracker, "textChanged",   CallbackFunctor(&textChangedSignal) );


  // get InputMethodContext
  std::string text;
  InputMethodContext::EventData imfEvent;
  InputMethodContext inputMethodContext = DevelTextEditor::GetInputMethodContext( editor );

  editor.SetKeyInputFocus();
  editor.SetProperty( DevelTextEditor::Property::ENABLE_EDITING, true );

  // input text
  gTextChangedCallBackCalled = false;
  imfEvent = InputMethodContext::EventData( InputMethodContext::PRE_EDIT, "ㅎ", 0, 1 );
  inputMethodContext.EventReceivedSignal().Emit(inputMethodContext, imfEvent);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( gTextChangedCallBackCalled );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::TEXT ), std::string("ㅎ"), TEST_LOCATION );

  gTextChangedCallBackCalled = false;
  imfEvent = InputMethodContext::EventData( InputMethodContext::PRE_EDIT, "호", 0, 1 );
  inputMethodContext.EventReceivedSignal().Emit(inputMethodContext, imfEvent);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( gTextChangedCallBackCalled );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::TEXT ), std::string("호"), TEST_LOCATION );

  gTextChangedCallBackCalled = false;
  imfEvent = InputMethodContext::EventData( InputMethodContext::PRE_EDIT, "혿", 0, 1 );
  inputMethodContext.EventReceivedSignal().Emit(inputMethodContext, imfEvent);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( gTextChangedCallBackCalled );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::TEXT ), std::string("혿"), TEST_LOCATION );

  gTextChangedCallBackCalled = false;
  imfEvent = InputMethodContext::EventData( InputMethodContext::PRE_EDIT, "", 0, 1 );
  inputMethodContext.EventReceivedSignal().Emit(inputMethodContext, imfEvent);
  DALI_TEST_CHECK( !gTextChangedCallBackCalled );

  imfEvent = InputMethodContext::EventData( InputMethodContext::COMMIT, "호", 0, 1 );
  inputMethodContext.EventReceivedSignal().Emit(inputMethodContext, imfEvent);
  DALI_TEST_CHECK( !gTextChangedCallBackCalled );

  imfEvent = InputMethodContext::EventData( InputMethodContext::PRE_EDIT, "두", 1, 2 );
  inputMethodContext.EventReceivedSignal().Emit(inputMethodContext, imfEvent);
  DALI_TEST_CHECK( !gTextChangedCallBackCalled );

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( gTextChangedCallBackCalled );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::TEXT ), std::string("호두"), TEST_LOCATION );

  END_TEST;
}


int utcDaliTextEditorInputStyleChanged01(void)
{
  // The text-editor emits signals when the input style changes. These changes of style are
  // detected during the relayout process (size negotiation), i.e after the cursor has been moved. Signals
  // can't be emitted during the size negotiation as the callbacks may update the UI.
  // The text-editor adds an idle callback to the adaptor to emit the signals after the size negotiation.
  // The ToolkitTestApplication creates an implementation of the adaptor stub and a queue of idle callbacks.
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorInputStyleChanged01");

  // Load some fonts.

  char* pathNamePtr = get_current_dir_name();
  const std::string pathName( pathNamePtr );
  free( pathNamePtr );

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi( 93u, 93u );

  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/dejavu/DejaVuSerif.ttf", DEFAULT_FONT_SIZE );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/dejavu/DejaVuSerif-Bold.ttf", DEFAULT_FONT_SIZE );

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );


  editor.SetProperty( Actor::Property::SIZE, Vector2( 300.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  editor.SetProperty( TextEditor::Property::ENABLE_MARKUP, true );
  editor.SetProperty( TextEditor::Property::TEXT, "<font family='DejaVuSerif' size='18'>He<color value='green'>llo</color> <font weight='bold'>world</font> demo</font>" );

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  editor.InputStyleChangedSignal().Connect( &TestInputStyleChangedCallback );
  bool inputStyleChangedSignal = false;
  editor.ConnectSignal( testTracker, "inputStyleChanged",   CallbackFunctor(&inputStyleChangedSignal) );

  application.GetScene().Add( editor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text editor.
  TestGenerateTap( application, 18.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( gInputStyleChangedCallbackCalled );
  if( gInputStyleChangedCallbackCalled )
  {
    DALI_TEST_EQUALS( static_cast<unsigned int>( gInputStyleMask ), static_cast<unsigned int>( TextEditor::InputStyle::FONT_FAMILY | TextEditor::InputStyle::POINT_SIZE ), TEST_LOCATION );

    const std::string fontFamily = editor.GetProperty( TextEditor::Property::INPUT_FONT_FAMILY ).Get<std::string>();
    DALI_TEST_EQUALS( fontFamily, "DejaVuSerif", TEST_LOCATION );

    const float pointSize = editor.GetProperty( TextEditor::Property::INPUT_POINT_SIZE ).Get<float>();
    DALI_TEST_EQUALS( pointSize, 18.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text editor.
  TestGenerateTap( application, 30.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( !gInputStyleChangedCallbackCalled );
  DALI_TEST_CHECK( !inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text editor.
  TestGenerateTap( application, 43.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( gInputStyleChangedCallbackCalled );
  if( gInputStyleChangedCallbackCalled )
  {
    DALI_TEST_EQUALS( static_cast<unsigned int>( gInputStyleMask ), static_cast<unsigned int>( TextEditor::InputStyle::COLOR ), TEST_LOCATION );

    const Vector4 color = editor.GetProperty( TextEditor::Property::INPUT_COLOR ).Get<Vector4>();
    DALI_TEST_EQUALS( color, Color::GREEN, TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text editor.
  TestGenerateTap( application, 88.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( gInputStyleChangedCallbackCalled );
  if( gInputStyleChangedCallbackCalled )
  {
    DALI_TEST_EQUALS( static_cast<unsigned int>( gInputStyleMask ), static_cast<unsigned int>( TextEditor::InputStyle::COLOR | TextEditor::InputStyle::FONT_STYLE ), TEST_LOCATION );

    const Vector4 color = editor.GetProperty( TextEditor::Property::INPUT_COLOR ).Get<Vector4>();
    DALI_TEST_EQUALS( color, Color::BLACK, TEST_LOCATION );

    Property::Map fontStyleMapSet;
    Property::Map fontStyleMapGet;

    fontStyleMapSet.Insert( "weight", "bold" );

    fontStyleMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::INPUT_FONT_STYLE );
    DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
    DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text editor.
  TestGenerateTap( application, 115.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( !gInputStyleChangedCallbackCalled );
  DALI_TEST_CHECK( !inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text editor.
  TestGenerateTap( application, 164.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( gInputStyleChangedCallbackCalled );
  if( gInputStyleChangedCallbackCalled )
  {
    DALI_TEST_EQUALS( static_cast<unsigned int>( gInputStyleMask ), static_cast<unsigned int>( TextEditor::InputStyle::FONT_STYLE ), TEST_LOCATION );

    Property::Map fontStyleMapSet;
    Property::Map fontStyleMapGet;

    fontStyleMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::INPUT_FONT_STYLE );
    DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
    DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text editor.
  TestGenerateTap( application, 191.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( !gInputStyleChangedCallbackCalled );
  DALI_TEST_CHECK( !inputStyleChangedSignal );

  END_TEST;
}

int utcDaliTextEditorInputStyleChanged02(void)
{
  // The text-editor emits signals when the input style changes. These changes of style are
  // detected during the relayout process (size negotiation), i.e after the cursor has been moved. Signals
  // can't be emitted during the size negotiation as the callbacks may update the UI.
  // The text-editor adds an idle callback to the adaptor to emit the signals after the size negotiation.
  // The ToolkitTestApplication creates an implementation of the adaptor stub and a queue of idle callbacks.
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorInputStyleChanged02");

  // Load some fonts.

  char* pathNamePtr = get_current_dir_name();
  const std::string pathName( pathNamePtr );
  free( pathNamePtr );

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi( 93u, 93u );

  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/dejavu/DejaVuSerif.ttf", DEFAULT_FONT_SIZE );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/dejavu/DejaVuSerif-Bold.ttf", DEFAULT_FONT_SIZE );

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );


  editor.SetProperty( Actor::Property::SIZE, Vector2( 300.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  editor.SetProperty( TextEditor::Property::ENABLE_MARKUP, true );
  editor.SetProperty( TextEditor::Property::TEXT, "<font family='DejaVuSerif' size='18'>He<color value='blue'> l</color><color value='green'>lo</color> <font weight='bold'>world</font> demo</font>" );

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  editor.InputStyleChangedSignal().Connect( &TestInputStyleChangedCallback );
  bool inputStyleChangedSignal = false;
  editor.ConnectSignal( testTracker, "inputStyleChanged",   CallbackFunctor(&inputStyleChangedSignal) );

  application.GetScene().Add( editor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text editor.
  TestGenerateTap( application, 53.0f, 25.0f, 100 );
  TestGenerateTap( application, 53.0f, 25.0f, 200 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( gInputStyleChangedCallbackCalled );
  if( gInputStyleChangedCallbackCalled )
  {
    DALI_TEST_EQUALS( static_cast<unsigned int>( gInputStyleMask ),
                      static_cast<unsigned int>( TextEditor::InputStyle::FONT_FAMILY |
                                                 TextEditor::InputStyle::POINT_SIZE  |
                                                 TextEditor::InputStyle::COLOR ),
                      TEST_LOCATION );

    const Vector4 color = editor.GetProperty( TextEditor::Property::INPUT_COLOR ).Get<Vector4>();
    DALI_TEST_EQUALS( color, Color::GREEN, TEST_LOCATION );

    const std::string fontFamily = editor.GetProperty( TextEditor::Property::INPUT_FONT_FAMILY ).Get<std::string>();
    DALI_TEST_EQUALS( fontFamily, "DejaVuSerif", TEST_LOCATION );

    const float pointSize = editor.GetProperty( TextEditor::Property::INPUT_POINT_SIZE ).Get<float>();
    DALI_TEST_EQUALS( pointSize, 18.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( gInputStyleChangedCallbackCalled );
  if( gInputStyleChangedCallbackCalled )
  {
    DALI_TEST_EQUALS( static_cast<unsigned int>( gInputStyleMask ),
                      static_cast<unsigned int>( TextEditor::InputStyle::COLOR ),
                      TEST_LOCATION );

    const Vector4 color = editor.GetProperty( TextEditor::Property::INPUT_COLOR ).Get<Vector4>();
    DALI_TEST_EQUALS( color, Color::BLUE, TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( !gInputStyleChangedCallbackCalled );
  DALI_TEST_CHECK( !inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( gInputStyleChangedCallbackCalled );
  if( gInputStyleChangedCallbackCalled )
  {
    DALI_TEST_EQUALS( static_cast<unsigned int>( gInputStyleMask ),
                      static_cast<unsigned int>( TextEditor::InputStyle::COLOR ),
                      TEST_LOCATION );

    const Vector4 color = editor.GetProperty( TextEditor::Property::INPUT_COLOR ).Get<Vector4>();
    DALI_TEST_EQUALS( color, Color::BLACK, TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  editor.SetProperty( TextEditor::Property::INPUT_COLOR, Color::YELLOW );

  Property::Map fontStyleMapSet;
  fontStyleMapSet.Insert( "weight", "thin" );
  fontStyleMapSet.Insert( "width", "condensed" );
  fontStyleMapSet.Insert( "slant", "italic" );

  editor.SetProperty( TextEditor::Property::INPUT_FONT_STYLE, fontStyleMapSet );
  editor.SetProperty( TextEditor::Property::INPUT_POINT_SIZE, 20.f );
  editor.SetProperty( TextEditor::Property::INPUT_LINE_SPACING, 5.f );

  editor.SetProperty( TextEditor::Property::INPUT_UNDERLINE, "underline" );
  editor.SetProperty( TextEditor::Property::INPUT_SHADOW, "shadow" );
  editor.SetProperty( TextEditor::Property::INPUT_EMBOSS, "emboss" );
  editor.SetProperty( TextEditor::Property::INPUT_OUTLINE, "outline" );
  editor.SetProperty( DevelTextEditor::Property::INPUT_STRIKETHROUGH, "strikethrough" );

  application.ProcessEvent( GenerateKey( "a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( !gInputStyleChangedCallbackCalled );
  DALI_TEST_CHECK( !inputStyleChangedSignal );

  // Create a tap event to touch the text editor.
  TestGenerateTap( application, 63.0f, 25.0f, 900 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( gInputStyleChangedCallbackCalled );
  if( gInputStyleChangedCallbackCalled )
  {
    DALI_TEST_EQUALS( static_cast<unsigned int>( gInputStyleMask ),
                      static_cast<unsigned int>( TextEditor::InputStyle::COLOR |
                                                 TextEditor::InputStyle::POINT_SIZE |
                                                 TextEditor::InputStyle::FONT_STYLE |
                                                 TextEditor::InputStyle::LINE_SPACING |
                                                 TextEditor::InputStyle::UNDERLINE |
                                                 TextEditor::InputStyle::SHADOW |
                                                 TextEditor::InputStyle::EMBOSS |
                                                 TextEditor::InputStyle::OUTLINE ),
                      TEST_LOCATION );

    const Vector4 color = editor.GetProperty( TextEditor::Property::INPUT_COLOR ).Get<Vector4>();
    DALI_TEST_EQUALS( color, Color::BLACK, TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  editor.SetProperty( TextEditor::Property::FONT_FAMILY, "DejaVuSerif" );

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "weight", "black" );
  fontStyleMapSet.Insert( "width", "expanded" );
  fontStyleMapSet.Insert( "slant", "oblique" );

  editor.SetProperty( TextEditor::Property::FONT_STYLE, fontStyleMapSet );

  // Create a tap event to touch the text editor.
  TestGenerateTap( application, 30.0f, 25.0f, 1500 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( gInputStyleChangedCallbackCalled );
  if( gInputStyleChangedCallbackCalled )
  {
    DALI_TEST_EQUALS( static_cast<unsigned int>( gInputStyleMask ),
                      static_cast<unsigned int>( TextEditor::InputStyle::COLOR |
                                                 TextEditor::InputStyle::POINT_SIZE |
                                                 TextEditor::InputStyle::FONT_STYLE ),
                      TEST_LOCATION );

    const Vector4 color = editor.GetProperty( TextEditor::Property::INPUT_COLOR ).Get<Vector4>();
    DALI_TEST_EQUALS( color, Color::YELLOW, TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  END_TEST;
}

int utcDaliTextEditorEvent01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorEvent01");

  // Creates a tap event. After creating a tap event the text editor should
  // have the focus and add text with key events should be possible.

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  editor.SetProperty( Actor::Property::SIZE, Vector2( 300.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Add a key event but as the text editor has not the focus it should do nothing.
  application.ProcessEvent( GenerateKey( "a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::TEXT ), std::string(""), TEST_LOCATION );

  // Create a tap event to touch the text editor.
  TestGenerateTap( application, 150.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Now the text editor has the focus, so it can handle the key events.
  application.ProcessEvent( GenerateKey( "a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::TEXT ), std::string("aa"), TEST_LOCATION );

  // Create a second text editor and send key events to it.
  TextEditor editor2 = TextEditor::New();

  editor2.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor2.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  editor2.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 100.f ) );
  editor2.SetProperty( Actor::Property::POSITION, Vector2( 100.f, 100.f ));

  application.GetScene().Add( editor2 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Create a tap event on the second text editor.
  TestGenerateTap( application, 150.0f, 125.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // The second text editor has the focus. It should handle the key events.
  application.ProcessEvent( GenerateKey( "a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check the text has been added to the second text editor.
  DALI_TEST_EQUALS( editor2.GetProperty<std::string>( TextEditor::Property::TEXT ), std::string("aa"), TEST_LOCATION );

  END_TEST;
}

int utcDaliTextEditorEvent02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorEvent02");

  // Checks if the right number of actors are created.

  TextEditor editor = TextEditor::New();
  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10.f );
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  editor.SetProperty( Actor::Property::SIZE, Vector2( 300.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check there are the expected number of children (the stencil).
  DALI_TEST_EQUALS( editor.GetChildCount(), 1u, TEST_LOCATION );

  Actor stencil = editor.GetChildAt( 0u );

  // Create a tap event to touch the text editor.
  TestGenerateTap( application, 150.0f, 25.0f, 100 );

  // Render and notify
  application.SendNotification();
  application.Render();

  Actor layer = editor.GetChildAt( 1u );
  DALI_TEST_EQUALS( layer.GetChildCount(), 1u, TEST_LOCATION ); // The cursor.
  DALI_TEST_EQUALS( stencil.GetChildCount(), 0u, TEST_LOCATION );

  // Now the text editor has the focus, so it can handle the key events.
  application.ProcessEvent( GenerateKey( "a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Checks the cursor and the renderer have been created.
  DALI_TEST_EQUALS( layer.GetChildCount(), 1u, TEST_LOCATION ); // The cursor.
  DALI_TEST_EQUALS( stencil.GetChildCount(), 1u, TEST_LOCATION ); // The renderer

  Control cursor = Control::DownCast( layer.GetChildAt( 0u ) );
  DALI_TEST_CHECK( cursor );

  // The stencil actor has a container with all the actors which contain the text renderers.
  Actor container = stencil.GetChildAt( 0u );
  for( unsigned int index = 0; index < container.GetChildCount(); ++index )
  {
    Renderer renderer = container.GetChildAt( index ).GetRendererAt( 0u );
    DALI_TEST_CHECK( renderer );
  }

  // Move the cursor and check the position changes.
  Vector3 position1 = cursor.GetCurrentProperty< Vector3 >( Actor::Property::POSITION );

  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector3 position2 = cursor.GetCurrentProperty< Vector3 >( Actor::Property::POSITION );

  DALI_TEST_CHECK( position2.x < position1.x );

  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector3 position3 = cursor.GetCurrentProperty< Vector3 >( Actor::Property::POSITION );

  DALI_TEST_EQUALS( position1, position3, TEST_LOCATION ); // Should be in the same position1.

  // Send some taps and check the cursor positions.

  // Try to tap at the beginning.
  TestGenerateTap( application, 1.0f, 25.0f, 700 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Cursor position should be the same than position1.
  Vector3 position4 = cursor.GetCurrentProperty< Vector3 >( Actor::Property::POSITION );

  DALI_TEST_EQUALS( position2, position4, TEST_LOCATION ); // Should be in the same position2.

  // Tap away from the start position.
  TestGenerateTap( application, 16.0f, 25.0f, 1400 );

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector3 position5 = cursor.GetCurrentProperty< Vector3 >( Actor::Property::POSITION );

  DALI_TEST_CHECK( position5.x > position4.x );

  // Remove all the text.
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  editor.SetProperty( TextEditor::Property::TEXT, "" );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Cursor position should be the same than position2.
  Vector3 position6 = cursor.GetCurrentProperty< Vector3 >( Actor::Property::POSITION );

  DALI_TEST_EQUALS( position2, position6, TEST_LOCATION );// Should be in the same position2.

  // Should not be a renderer.
  DALI_TEST_EQUALS( stencil.GetChildCount(), 0u, TEST_LOCATION );

  END_TEST;
}

int utcDaliTextEditorEvent03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorEvent03");

  // Checks if the highlight actor is created.

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::TEXT, "This is a long text for the size of the text-editor." );
  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 30.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Send some taps and check text controller with clipboard window
  Dali::Clipboard clipboard = Clipboard::Get();
  clipboard.ShowClipboard();
  TestGenerateTap( application, 3.0f, 25.0f, 100 );
  clipboard.HideClipboard();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap first to get the focus.
  TestGenerateTap( application, 3.0f, 25.0f, 1000 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Double tap to select a word.
  TestGenerateTap( application, 3.0f, 25.0f, 1100 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // The stencil actor should have two actors: the renderer and the highlight actor.
  Actor stencil = editor.GetChildAt( 0u );

  // Highlight needs to be drawn before text, so should come first in child order
  Renderer highlight = stencil.GetChildAt( 0u ).GetRendererAt( 0u );
  DALI_TEST_CHECK( highlight );

  // The stencil actor has a container with all the actors which contain the text renderers.
  Actor container = stencil.GetChildAt( 1u );
  for( unsigned int index = 0; index < container.GetChildCount(); ++index )
  {
    Renderer renderer = container.GetChildAt( index ).GetRendererAt( 0u );
    DALI_TEST_CHECK( renderer );
  }

  // Double tap out of bounds
  TestGenerateTap( application, 29.0f, 25.0f, 1700 );
  TestGenerateTap( application, 29.0f, 25.0f, 1800 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // The stencil actor should have one actors: the renderer actor.
  stencil = editor.GetChildAt( 0u );

  // The stencil actor has a container with all the actors which contain the text renderers.
  container = stencil.GetChildAt( 0u );
  for( unsigned int index = 0; index < container.GetChildCount(); ++index )
  {
    Renderer renderer = container.GetChildAt( index ).GetRendererAt( 0u );
    DALI_TEST_CHECK( renderer );
  }

  // Long Press
  TestGenerateLongPress(application, 1.0f, 25.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Pan Press
  TestGenerateMiniPan(application);

  // Render and notify
  application.SendNotification();
  application.Render();

  END_TEST;
}

int utcDaliTextEditorEvent04(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorEvent04");

  // Checks if the highlight actor is created.

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::TEXT, "Hello\nworl" );
  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap on the text editor
  TestGenerateTap( application, 3.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Move at the end of the text.
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  for( unsigned int index = 0u; index < 10u; ++index )
  {
    application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
    application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

    // Render and notify
    application.SendNotification();
    application.Render();
  }

  // Add a character
  application.ProcessEvent( GenerateKey( "d", "", "d", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( "Hello\nworld", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );

  // Add some key events
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_UP, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_UP, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  for( unsigned int index = 0u; index < 10u; ++index )
  {
    application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
    application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

    // Render and notify
    application.SendNotification();
    application.Render();
  }

  // Add a character
  application.ProcessEvent( GenerateKey( " ", "", " ", KEY_WHITE_SPACE_CODE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( " Hello\nworld", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );

  END_TEST;
}

int utcDaliTextEditorEvent05(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorEvent05");

  // Checks if the highlight actor is created.

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::TEXT, "Hello\nworl" );
  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 50.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  editor.SetProperty( TextEditor::Property::SMOOTH_SCROLL, true );
  editor.SetProperty( TextEditor::Property::SMOOTH_SCROLL_DURATION, 0.2f );
  editor.SetProperty( TextEditor::Property::ENABLE_SCROLL_BAR, true );
  editor.SetProperty( TextEditor::Property::SCROLL_BAR_SHOW_DURATION, 0.3f );
  editor.SetProperty( TextEditor::Property::SCROLL_BAR_FADE_DURATION, 0.2f );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap on the text editor
  TestGenerateTap( application, 3.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Move at the end of the text.
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  for( unsigned int index = 0u; index < 10u; ++index )
  {
    // Add a character
    application.ProcessEvent( GenerateKey( "d", "", "d", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

    // Render and notify
    application.SendNotification();
    application.Render();
  }
  // Modify duration after scroll is enabled
  editor.SetProperty( TextEditor::Property::SMOOTH_SCROLL_DURATION, 0.1f );

  // Continuous scroll left to increase coverage
  for( unsigned int index = 0u; index < 10u; ++index )
  {
    application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

    // Render and notify
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::SMOOTH_SCROLL_DURATION ), 0.1f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( editor.GetProperty<bool>( TextEditor::Property::SMOOTH_SCROLL ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( editor.GetProperty<bool>( TextEditor::Property::ENABLE_SCROLL_BAR ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::SCROLL_BAR_SHOW_DURATION ), 0.3f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::SCROLL_BAR_FADE_DURATION ), 0.2f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Press Escape to increase coverage
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_ESCAPE, 0, 0, Integration::KeyEvent::UP, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( !editor.HasKeyInputFocus() );

  END_TEST;
}

int utcDaliTextEditorEvent06(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorEvent06");

  // Checks if the highlight actor is created.

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::TEXT, "Hello\nworld\nHello world" );
  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap on the text editor
  TestGenerateTap( application, 3.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Move to seconds line of the text.
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  float layoutHeight = editor.GetHeightForWidth( 100.f );


  // Add  another script characters ( glyph height is defferent )
  application.ProcessEvent( GenerateKey( "d", "", "ㅁ", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "ㅁ", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "d", "", "ኢ", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "ኢ", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Delete characters
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  DALI_TEST_EQUALS( layoutHeight, editor.GetHeightForWidth( 100.f ), TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( "Hello\nworld\nHello world", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );

  // For coverage
  application.ProcessEvent( GenerateKey( "", "", "", 0, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.SendNotification();
  application.Render();

  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_SHIFT_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.SendNotification();
  application.Render();

  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_VOLUME_UP, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.SendNotification();
  application.Render();

  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_VOLUME_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.SendNotification();
  application.Render();

  application.ProcessEvent( GenerateKey( "", "", "", Dali::DevelKey::DALI_KEY_DELETE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.SendNotification();
  application.Render();

  application.ProcessEvent( GenerateKey( "", "", "", Dali::DevelKey::DALI_KEY_CONTROL_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.SendNotification();
  application.Render();

  application.ProcessEvent( GenerateKey( "", "", "", Dali::DevelKey::DALI_KEY_CONTROL_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.SendNotification();
  application.Render();

  END_TEST;
}

int utcDaliTextEditorEvent07(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorEvent07");

  // Checks if the highlight actor is created.

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::TEXT, "Hello\nworld\nHello world" );
  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap on the text editor
  TestGenerateTap( application, 3.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Move to second line of the text.
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Select some text in the right of the current cursor position
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_SHIFT_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Cut the selected text
  application.ProcessEvent( GenerateKey( "", "", "", Dali::DevelKey::DALI_KEY_CONTROL_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "x", "", "x", KEY_X_CODE, KEY_CONTROL_MODIFIER, 0, Integration::KeyEvent::DOWN, "x", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( "Hello\nld\nHello world", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );

  // Select some text in the left of the current cursor position
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_SHIFT_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN,  "",DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Copy the selected text
  application.ProcessEvent( GenerateKey( "", "", "", Dali::DevelKey::DALI_KEY_CONTROL_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "c", "", "c", KEY_C_CODE, KEY_CONTROL_MODIFIER, 0, Integration::KeyEvent::DOWN, "c", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Move the cursor to the third line
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Paste the selected text at the current cursor position
  application.ProcessEvent( GenerateKey( "", "", "", Dali::DevelKey::DALI_KEY_CONTROL_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "v", "", "v", KEY_V_CODE, KEY_CONTROL_MODIFIER, 0, Integration::KeyEvent::DOWN, "v", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( "Hello\nld\nHello lo\nworld", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );


  // Disable Shift Selection
  editor.SetProperty( DevelTextEditor::Property::ENABLE_SHIFT_SELECTION, false );

  // Test to select some text in the right of the current cursor position
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_SHIFT_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Cut the selected text
  application.ProcessEvent( GenerateKey( "", "", "", Dali::DevelKey::DALI_KEY_CONTROL_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "x", "", "x", KEY_X_CODE, KEY_CONTROL_MODIFIER, 0, Integration::KeyEvent::DOWN, "x", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // The text isn't selected and not changed because of 'SetProperty( DevelTextEditor::Property::ENABLE_SHIFT_SELECTION, false )'
  DALI_TEST_EQUALS( "Hello\nld\nHello lo\nworld", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );

  // Test to select some text in the left of the current cursor position
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_SHIFT_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Copy the selected text
  application.ProcessEvent( GenerateKey( "", "", "", Dali::DevelKey::DALI_KEY_CONTROL_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "c", "", "c", KEY_C_CODE, KEY_CONTROL_MODIFIER, 0, Integration::KeyEvent::DOWN, "c", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // The text is not selected and not changed because of 'SetProperty( DevelTextEditor::Property::ENABLE_SHIFT_SELECTION, false )'
  DALI_TEST_EQUALS( "Hello\nld\nHello lo\nworld", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );

  // Select all Text
  application.ProcessEvent( GenerateKey( "a", "", "a", KEY_A_CODE, KEY_CONTROL_MODIFIER, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // replace text with "c"
  application.ProcessEvent( GenerateKey( "c", "", "c", KEY_C_CODE, 0, 0, Integration::KeyEvent::DOWN, "c", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  //text is "c"
  DALI_TEST_EQUALS( "c", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );

  // select all text
  DevelTextEditor::SelectWholeText(editor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Copy the selected text using logical keys
  application.ProcessEvent( GenerateKey( "", "", "", Dali::DevelKey::DALI_KEY_CONTROL_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "ؤ", "c", "ؤ", KEY_C_CODE, KEY_CONTROL_MODIFIER, 0, Integration::KeyEvent::DOWN, "c", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // select none
  DevelTextEditor::SelectNone(editor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Paste the selected using logical keys
  application.ProcessEvent( GenerateKey( "", "", "", Dali::DevelKey::DALI_KEY_CONTROL_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "ر", "v", "ر", KEY_V_CODE, KEY_CONTROL_MODIFIER, 0, Integration::KeyEvent::DOWN, "v", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  //text is "cc"
  DALI_TEST_EQUALS( "cc", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );

  // select all using logical keys
  application.ProcessEvent( GenerateKey( "", "", "", Dali::DevelKey::DALI_KEY_CONTROL_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "ش", "a", "ش", KEY_A_CODE, KEY_CONTROL_MODIFIER, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // cut text using logical keys
  application.ProcessEvent( GenerateKey( "", "", "", Dali::DevelKey::DALI_KEY_CONTROL_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "ء", "x", "ء", KEY_X_CODE, KEY_CONTROL_MODIFIER, 0, Integration::KeyEvent::DOWN, "x", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  //text is ""
  DALI_TEST_EQUALS( "", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );

  END_TEST;
}

int utcDaliTextEditorEvent08(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorEvent08");

  // Checks if the highlight actor is released correctly.

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::TEXT, "DALi" );
  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap on the text editor
  TestGenerateTap( application, 3.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // When the left selection handle and the right selection handle are at the same position, the highlight box should be deactivated.
  // Test to select some text in the left of the current cursor position
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_SHIFT_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Test to the left selection handle position and the right selection handle position
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_SHIFT_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Test to select full text in the left of the current cursor position
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_SHIFT_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Test to release the current full text selection
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Test to move the current cursor position correctly
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Add a character
  application.ProcessEvent( GenerateKey( "d", "", "d", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( "DdALi", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );

  // Test to select some text in the right of the current cursor position
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_SHIFT_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Test the cursor position with right arrow key
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Add a character
  application.ProcessEvent( GenerateKey( "c", "", "c", KEY_C_CODE, 0, 0, Integration::KeyEvent::DOWN, "c", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( "DdALci", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );

  // Test to select some text in the left of the current cursor position
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_SHIFT_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Test the cursor position with left arrow key
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Add a character
  application.ProcessEvent( GenerateKey( "c", "", "c", KEY_C_CODE, 0, 0, Integration::KeyEvent::DOWN, "c", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( "DcdALci", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );

  // Test to select some text in the right of the current cursor position
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_SHIFT_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Test the cursor position with left arrow key
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Add a character
  application.ProcessEvent( GenerateKey( "x", "", "x", KEY_X_CODE, 0, 0, Integration::KeyEvent::DOWN, "x", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( "DcxdALci", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );

  // Test to select some text in the left of the current cursor position
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_SHIFT_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Test the cursor position with right arrow key
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Add a character
  application.ProcessEvent( GenerateKey( "c", "", "c", KEY_C_CODE, 0, 0, Integration::KeyEvent::DOWN, "c", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( "DcxcdALci", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );

  END_TEST;
}

int utcDaliTextEditorHandles(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorHandles");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::TEXT, "This is a long text for the size of the text-editor." );
  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10.f );
  editor.SetProperty( TextEditor::Property::GRAB_HANDLE_IMAGE, HANDLE_IMAGE_FILE_NAME );
  editor.SetProperty( TextEditor::Property::SMOOTH_SCROLL, true );

  editor.SetProperty( TextEditor::Property::SELECTION_HANDLE_IMAGE_LEFT, Property::Map{ {"filename", HANDLE_LEFT_SELECTION_FILE_NAME } } );
  editor.SetProperty( TextEditor::Property::SELECTION_HANDLE_IMAGE_RIGHT, Property::Map{ {"filename", HANDLE_LEFT_SELECTION_FILE_NAME } } );
  editor.SetProperty( TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT, Property::Map{ {"filename", HANDLE_RIGHT_SELECTION_FILE_NAME } } );
  editor.SetProperty( TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT, Property::Map{ {"filename", HANDLE_RIGHT_SELECTION_FILE_NAME } } );

  editor.SetProperty( Actor::Property::SIZE, Vector2( 30.f, 500.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap first to get the focus.
  TestGenerateTap( application, 3.0f, 25.0f, 100 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap to create the grab handle.
  TestGenerateTap( application, 3.0f, 25.0f, 700 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Get the active layer where the text's decoration is added.
  Actor activeLayer = editor.GetChildAt( 1u );

  // Get the handle's actor.
  Actor handle = activeLayer.GetChildAt( 1u );
  handle.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 100.f ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Touch the grab handle to set it as pressed.
  Vector2 touchPos( 10.0f, 50.0f );
  Dali::Integration::TouchEvent event;
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownInside( touchPos ) );
  application.ProcessEvent( event );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Pan the grab handle
  uint32_t time = 100;
  TestStartPan( application, Vector2(10.0f, 50.0f), Vector2(10.0f, 50.0f), time );
  TestMovePan( application, Vector2(10.0f, 30.0f), time );
  TestEndPan( application, Vector2(10.0f, 50.0f), time);
  application.SendNotification();
  application.Render();


  // Release the grab handle.
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpInside( touchPos ) );
  application.ProcessEvent( event );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap first to get the focus.
  TestGenerateTap( application, 3.0f, 25.0f, 1400 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Double tap to select a word and create the selection handles.
  TestGenerateTap( application, 3.0f, 25.0f, 1500 );

  // Render and notify
  application.SendNotification();
  application.Render();

  touchPos = Vector2( 10.0f, 50.0f );

  // Touch the left selection handle to set it as pressed.
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointDownInside( touchPos ) );
  application.ProcessEvent( event );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Release the left selection handle.
  event = Dali::Integration::TouchEvent();
  event.AddPoint( GetPointUpInside( touchPos ) );
  application.ProcessEvent( event );

  // Render and notify
  application.SendNotification();
  application.Render();

  END_TEST;
}

int utcDaliTextEditorUnderPropertyStringP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorUnderPropertyStringP");
  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  std::string underlineSettings1( "{\"enable\":\"true\",\"color\":\"red\",\"height\":\"1\"}" );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::UNDERLINE, underlineSettings1 );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::UNDERLINE ), underlineSettings1, TEST_LOCATION );

  tet_infoline("Set underline settings with a map");
  // Check the input underline property
  Property::Map underlineMapSet;
  Property::Map underlineMapGet;
  underlineMapSet.Insert( "enable", true );
  underlineMapSet.Insert( "color", Color::BLUE );
  underlineMapSet.Insert( "height", 2 );

  editor.SetProperty( TextEditor::Property::UNDERLINE, underlineMapSet );
  underlineMapGet = editor.GetProperty<Property::Map>( TextEditor::Property::UNDERLINE );
  DALI_TEST_EQUALS( underlineMapGet.Count(), underlineMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( underlineMapSet, underlineMapGet ), true,  TEST_LOCATION );

  tet_infoline("Set underline settings with a string");
  editor.SetProperty( TextEditor::Property::UNDERLINE, underlineSettings1 );
  Property::Value value = editor.GetProperty( TextEditor::Property::UNDERLINE );
  std::string result;
  value.Get(result);
  DALI_TEST_EQUALS( result , underlineSettings1, TEST_LOCATION  );

  tet_infoline("Trying to set invalid underline settings, should not update and stay at previous settings");
  std::string underlineSettingsVoid( "{\"enable\":\"true\",\"coooolor\":\"blue\",\"heeeight\":\"4\"}" );
  editor.SetProperty( TextEditor::Property::UNDERLINE, underlineSettingsVoid );
  value = editor.GetProperty( TextEditor::Property::UNDERLINE );
  value.Get(result);
  DALI_TEST_EQUALS( result , underlineSettings1, TEST_LOCATION  );

  END_TEST;
}

int utcDaliTextEditorStrikethroughPropertyStringP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorStrikethroughPropertyStringP");
  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  std::string strikethroughSettings1( "{\"enable\":\"true\",\"color\":\"red\",\"height\":\"2\"}" );

  application.GetScene().Add( editor );

  editor.SetProperty( DevelTextEditor::Property::STRIKETHROUGH, strikethroughSettings1 );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( DevelTextEditor::Property::STRIKETHROUGH ), strikethroughSettings1, TEST_LOCATION );

  tet_infoline("Set strikethrough settings with a map");
  // Check the input strikethrough property
  Property::Map strikethroughMapSet;
  Property::Map strikethroughMapGet;
  strikethroughMapSet.Insert( "enable", true );
  strikethroughMapSet.Insert( "color", Color::BLUE );
  strikethroughMapSet.Insert( "height", 2.0f );

  editor.SetProperty( DevelTextEditor::Property::STRIKETHROUGH, strikethroughMapSet );
  strikethroughMapGet = editor.GetProperty<Property::Map>( DevelTextEditor::Property::STRIKETHROUGH );
  DALI_TEST_EQUALS( strikethroughMapGet.Count(), strikethroughMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( strikethroughMapSet, strikethroughMapGet ), true,  TEST_LOCATION );

  tet_infoline("Set strikethrough settings with a string");
  editor.SetProperty( DevelTextEditor::Property::STRIKETHROUGH, strikethroughSettings1 );
  Property::Value value = editor.GetProperty( DevelTextEditor::Property::STRIKETHROUGH );
  std::string result;
  value.Get(result);
  DALI_TEST_EQUALS( result , strikethroughSettings1, TEST_LOCATION  );

  tet_infoline("Trying to set invalid strikethrough settings, should not update and stay at previous settings");
  std::string strikethroughSettingsVoid( "{\"enable\":\"true\",\"coooolor\":\"blue\",\"height\":\"2\"}" );
  editor.SetProperty( DevelTextEditor::Property::STRIKETHROUGH, strikethroughSettingsVoid );
  value = editor.GetProperty( TextEditor::Property::UNDERLINE );
  value.Get(result);
  DALI_TEST_EQUALS( result , strikethroughSettings1, TEST_LOCATION  );

  END_TEST;
}

int utcDaliTextEditorShadowPropertyStringP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorUnderPropertyStringP Setting Shadow propeties by string");

  TextEditor editor = TextEditor::New();

  std::string shadowSettings( "{\"color\":\"green\",\"offset\":\"2 2\",\"blurRadius\":\"0\"}" );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::SHADOW, "{\"color\":\"green\",\"offset\":\"2 2\",\"blurRadius\":\"0\"}" );

  Property::Value value = editor.GetProperty<std::string>( TextEditor::Property::SHADOW );
  std::string result;
  value.Get(result);

  DALI_TEST_EQUALS( result, shadowSettings, TEST_LOCATION );

  END_TEST;
}

int utcDaliTextEditorFontStylePropertyStringP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorFontStylePropertyStringP Setting FontStyle propeties by string");

  TextEditor editor = TextEditor::New();

  std::string fontStyleSettings( "{\"weight\":\"bold\",\"width\":\"condensed\",\"slant\":\"italic\"}" );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::FONT_STYLE, "{\"weight\":\"bold\",\"width\":\"condensed\",\"slant\":\"italic\"}" );

  Property::Value value = editor.GetProperty<std::string>( TextEditor::Property::FONT_STYLE );
  std::string result;
  value.Get(result);

  DALI_TEST_EQUALS( result, fontStyleSettings, TEST_LOCATION );

  END_TEST;
}

int utcDaliTextEditorGetPropertyLinecountP(void)
{
  ToolkitTestApplication application;

  tet_infoline(" utcDaliTextEditorGetPropertyLinecount getting line count property");

  int lineCount =0 ;

  TextEditor editor = TextEditor::New();
  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10) ;
  editor.SetProperty( TextEditor::Property::TEXT,
                       "TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST ");

  application.GetScene().Add( editor );

  editor.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 100.f ) );
  lineCount =  editor.GetProperty<int>( TextEditor::Property::LINE_COUNT );
  DALI_TEST_EQUALS( lineCount, 14, TEST_LOCATION );

  editor.SetProperty( Actor::Property::SIZE, Vector2( 50.f, 100.f ) );
  lineCount =  editor.GetProperty<int>( TextEditor::Property::LINE_COUNT );
  DALI_TEST_EQUALS( lineCount, 28, TEST_LOCATION );

  END_TEST;
}

int utcDaliTextEditorScrollStateChangedSignalTest(void)
{

  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorScrollStateChangedSignalTest");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 50.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  editor.SetProperty( TextEditor::Property::ENABLE_SCROLL_BAR, true );
  editor.SetProperty( Actor::Property::KEYBOARD_FOCUSABLE,true);

  bool startedCalled = false;
  bool finishedCalled = false;

  ScrollStateChangeCallback callback( startedCalled, finishedCalled );
  editor.ScrollStateChangedSignal().Connect( &callback, &ScrollStateChangeCallback::Callback );

  KeyboardFocusManager::Get().SetCurrentFocusActor( editor );

  // Render and notify
  application.SendNotification();
  application.Render();

  editor.SetProperty( TextEditor::Property::TEXT, "Long enough message for TextEditor!");
  application.SendNotification();
  application.Render(6000);

  application.SendNotification();
  DALI_TEST_EQUALS( startedCalled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( finishedCalled, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitTextEditorTextWrapMode(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextEditorTextWarpMode");

  int lineCount =0 ;

  TextEditor editor = TextEditor::New();
  editor.SetProperty( Actor::Property::SIZE, Vector2( 150.0f, 300.f ) );
  editor.SetProperty( TextEditor::Property::TEXT, "Hello world Hello world" );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::LINE_WRAP_MODE, "WORD" );
  DALI_TEST_EQUALS( editor.GetProperty< int >( TextEditor::Property::LINE_WRAP_MODE ), static_cast< int >( Text::LineWrap::WORD ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  lineCount =  editor.GetProperty<int>( TextEditor::Property::LINE_COUNT );
  DALI_TEST_EQUALS( lineCount, 4, TEST_LOCATION );

  editor.SetProperty( TextEditor::Property::LINE_WRAP_MODE, "CHARACTER" );
  DALI_TEST_EQUALS( editor.GetProperty< int >( TextEditor::Property::LINE_WRAP_MODE ), static_cast< int >( Text::LineWrap::CHARACTER ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  lineCount =  editor.GetProperty<int>( TextEditor::Property::LINE_COUNT );
  DALI_TEST_EQUALS( lineCount, 3, TEST_LOCATION );

  editor.SetProperty( TextEditor::Property::LINE_WRAP_MODE, Text::LineWrap::WORD );
  DALI_TEST_EQUALS( editor.GetProperty< int >( TextEditor::Property::LINE_WRAP_MODE ), static_cast< int >( Text::LineWrap::WORD ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  lineCount =  editor.GetProperty<int>( TextEditor::Property::LINE_COUNT );
  DALI_TEST_EQUALS( lineCount, 4, TEST_LOCATION );

  editor.SetProperty( TextEditor::Property::LINE_WRAP_MODE, Text::LineWrap::CHARACTER );
  DALI_TEST_EQUALS( editor.GetProperty< int >( TextEditor::Property::LINE_WRAP_MODE ), static_cast< int >( Text::LineWrap::CHARACTER ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  lineCount =  editor.GetProperty<int>( TextEditor::Property::LINE_COUNT );
  DALI_TEST_EQUALS( lineCount, 3, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextEditorSetPaddingProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextEditorSetPaddingProperty\n");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 300.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  application.GetScene().Add( editor );

  application.SendNotification();
  application.Render();

  Vector3 originalSize = editor.GetNaturalSize();

  editor.SetProperty( Toolkit::Control::Property::PADDING, Extents( 10, 10, 10, 10 ) );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( editor.GetProperty<Extents>( Toolkit::Control::Property::PADDING ), Extents( 10, 10, 10, 10 ), TEST_LOCATION );

  Vector3 paddingAddedSize = editor.GetNaturalSize();

  DALI_TEST_EQUALS( originalSize.width + 10 + 10 , paddingAddedSize.width, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_EQUALS( originalSize.height + 10 + 10 , paddingAddedSize.height, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextEditorEnableShiftSelectionProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextEditorEnableShiftSelectionProperty");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 300.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  application.GetScene().Add( editor );

  application.SendNotification();
  application.Render();

  // The default value of ENABLE_SHIFT_SELECTION is 'true'.
  DALI_TEST_EQUALS( editor.GetProperty<bool>( DevelTextEditor::Property::ENABLE_SHIFT_SELECTION ), true, TEST_LOCATION );

  // Check the enable shift selection property
  editor.SetProperty( DevelTextEditor::Property::ENABLE_SHIFT_SELECTION, false );
  DALI_TEST_EQUALS( editor.GetProperty<bool>( DevelTextEditor::Property::ENABLE_SHIFT_SELECTION ), false, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliTextEditorEnableGrabHandleProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextEditorEnableGrabHandleProperty");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 300.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  application.GetScene().Add( editor );

  application.SendNotification();
  application.Render();

  // The default value of ENABLE_GRAB_HANDLE is 'true'.
  DALI_TEST_EQUALS( editor.GetProperty<bool>( DevelTextEditor::Property::ENABLE_GRAB_HANDLE ), true, TEST_LOCATION );

  // Check the enable grab handle property
  editor.SetProperty( DevelTextEditor::Property::ENABLE_GRAB_HANDLE, false );
  DALI_TEST_EQUALS( editor.GetProperty<bool>( DevelTextEditor::Property::ENABLE_GRAB_HANDLE ), false, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliTextEditorMatchSystemLanguageDirectionProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextEditorMatchSystemLanguageDirectionProperty");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 300.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  application.GetScene().Add( editor );

  application.SendNotification();
  application.Render();

  // The default value of MATCH_SYSTEM_LANGUAGE_DIRECTION is 'true'.
  DALI_TEST_EQUALS( editor.GetProperty<bool>( DevelTextEditor::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION ), true, TEST_LOCATION );

  // Check the disable match system language direction property
  editor.SetProperty( DevelTextEditor::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION, false );
  DALI_TEST_EQUALS( editor.GetProperty<bool>( DevelTextEditor::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION ), false, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliTextEditorGetInputMethodContext(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextEditorGetInputMethodContext");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( DevelTextEditor::GetInputMethodContext( editor ) );

  END_TEST;
}

int utcDaliTextEditorMaxCharactersReached(void)
{
  ToolkitTestApplication application;
  tet_infoline("utcDaliTextEditorMaxCharactersReached");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  const int maxNumberOfCharacters = 1;
  editor.SetProperty( DevelTextEditor::Property::MAX_LENGTH, maxNumberOfCharacters );
  DALI_TEST_EQUALS( editor.GetProperty<int>( DevelTextEditor::Property::MAX_LENGTH ), maxNumberOfCharacters, TEST_LOCATION );

  editor.SetKeyInputFocus();

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextEditor::MaxLengthReachedSignal( editor ).Connect(&TestMaxLengthReachedCallback);
  bool maxLengthReachedSignal = false;
  editor.ConnectSignal( testTracker, "maxLengthReached", CallbackFunctor(&maxLengthReachedSignal) );

  gMaxCharactersCallBackCalled = false;

  application.ProcessEvent( GenerateKey( "a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  DALI_TEST_CHECK( gMaxCharactersCallBackCalled );
  DALI_TEST_CHECK( maxLengthReachedSignal );

  END_TEST;
}

int utcDaliTextEditorInputFiltered(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorInputFiltered");
  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK(editor);

  application.GetScene().Add(editor);

  Property::Map inputFilter;

  // Only digit is accepted.
  inputFilter[InputFilter::Property::ACCEPTED] = "[\\d]";

  // Set input filter to TextEditor.
  editor.SetProperty(DevelTextEditor::Property::INPUT_FILTER, inputFilter);

  editor.SetKeyInputFocus();

  // connect to the input filtered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextEditor::InputFilteredSignal(editor).Connect(&TestInputFilteredCallback);
  bool inputFilteredSignal = false;
  editor.ConnectSignal(testTracker, "inputFiltered", CallbackFunctor(&inputFilteredSignal));

  gInputFilteredAcceptedCallbackCalled = false;

  application.ProcessEvent(GenerateKey( "a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ));

  DALI_TEST_CHECK(gInputFilteredAcceptedCallbackCalled);
  DALI_TEST_CHECK(inputFilteredSignal);

  // Word is rejected.
  inputFilter[InputFilter::Property::ACCEPTED] = "";
  inputFilter[InputFilter::Property::REJECTED] = "[\\w]";

  // Set input filter to TextEditor.
  editor.SetProperty(DevelTextEditor::Property::INPUT_FILTER, inputFilter);

  editor.SetKeyInputFocus();

  inputFilteredSignal = false;
  gInputFilteredRejectedCallbackCalled = false;

  application.ProcessEvent(GenerateKey( "a", "", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  DALI_TEST_CHECK(gInputFilteredAcceptedCallbackCalled);
  DALI_TEST_CHECK(inputFilteredSignal);

  END_TEST;
}

int UtcDaliTextEditorSelectWholeText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorSelectWholeText ");

  TextEditor textEditor = TextEditor::New();

  application.GetScene().Add( textEditor );

  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 300.f, 50.f ) );
  textEditor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  textEditor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( 1u, textEditor.GetChildCount(), TEST_LOCATION );

  DevelTextEditor::SelectWholeText( textEditor );

  application.SendNotification();
  application.Render();

  // Nothing should have been selected. The number of children is still 1
  DALI_TEST_EQUALS( 1u, textEditor.GetChildCount(), TEST_LOCATION );

  textEditor.SetProperty( TextEditor::Property::TEXT, "Hello world" );

  application.SendNotification();
  application.Render();

  DevelTextEditor::SelectWholeText( textEditor );

  application.SendNotification();
  application.Render();

  // Should be 2 children, the stencil and the layer
  DALI_TEST_EQUALS( 2u, textEditor.GetChildCount(), TEST_LOCATION );

  // The offscreen root actor should have two actors: the renderer and the highlight actor.
  Actor stencil = textEditor.GetChildAt( 0u );

  // The highlight actor is drawn first, so is the first actor in the list
  Renderer highlight = stencil.GetChildAt( 0u ).GetRendererAt( 0u );
  DALI_TEST_CHECK( highlight );

  END_TEST;
}

int UtcDaliTextEditorSelectText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorSelectText ");

  TextEditor textEditor = TextEditor::New();

  application.GetScene().Add( textEditor );

  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 300.f, 50.f ) );
  textEditor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  textEditor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  application.SendNotification();
  application.Render();

  DevelTextEditor::SelectText( textEditor ,0, 5 );

  application.SendNotification();
  application.Render();

  // Nothing is selected
  std::string selectedText = textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT ).Get<std::string>();
  DALI_TEST_EQUALS( "", selectedText, TEST_LOCATION );

  textEditor.SetProperty( TextEditor::Property::TEXT, "Hello world" );

  application.SendNotification();
  application.Render();

  DevelTextEditor::SelectText( textEditor, 0, 5 );

  application.SendNotification();
  application.Render();

  selectedText = textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT ).Get<std::string>();
  DALI_TEST_EQUALS( "Hello", selectedText, TEST_LOCATION );

  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_START ).Get<int>(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_END ).Get<int>(), 5, TEST_LOCATION );

  // world is selected
  DevelTextEditor::SelectText( textEditor, 6, 11 );

  application.SendNotification();
  application.Render();

  selectedText = textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT ).Get<std::string>();
  DALI_TEST_EQUALS( "world", selectedText, TEST_LOCATION );

  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_START ).Get<int>(), 6, TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_END ).Get<int>(), 11, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextEditorSelectNone(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorSelectWholeText ");

  TextEditor textEditor = TextEditor::New();

  application.GetScene().Add( textEditor );

  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 300.f, 50.f ) );
  textEditor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  textEditor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  application.SendNotification();
  application.Render();

  textEditor.SetProperty( TextEditor::Property::TEXT, "Hello world" );

  application.SendNotification();
  application.Render();

  // Nothing is selected
  std::string selectedText = textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT ).Get<std::string>();
  DALI_TEST_EQUALS( "", selectedText, TEST_LOCATION );

  DevelTextEditor::SelectWholeText( textEditor );

  application.SendNotification();
  application.Render();

  // whole text is selected
  selectedText = textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT ).Get<std::string>();
  DALI_TEST_EQUALS( "Hello world", selectedText, TEST_LOCATION );

  DevelTextEditor::SelectNone( textEditor );

  application.SendNotification();
  application.Render();

  // Nothing is selected
  selectedText = textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT ).Get<std::string>();
  DALI_TEST_EQUALS( "", selectedText, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextEditorSelectRange(void)
{
  ToolkitTestApplication application;
  tet_infoline("utcDaliTextEditorSelectRange");

  TextEditor textEditor = TextEditor::New();
  DALI_TEST_CHECK( textEditor );

  application.GetScene().Add( textEditor );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  application.SendNotification();
  application.Render();

  textEditor.SetProperty( TextEditor::Property::TEXT, "Hello world" );

  textEditor.SetProperty( DevelTextEditor::Property::SELECTED_TEXT_START, 0 );
  textEditor.SetProperty( DevelTextEditor::Property::SELECTED_TEXT_END, 5 );

  // Hello is selected
  std::string selectedText = textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT ).Get<std::string>();
  DALI_TEST_EQUALS( "Hello", selectedText, TEST_LOCATION );

  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_START ).Get<int>(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_END ).Get<int>(), 5, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextEditorEnableEditing(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorEnableEditing ");

  TextEditor textEditor = TextEditor::New();

  application.GetScene().Add( textEditor );

  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 300.f, 50.f ) );
  textEditor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  textEditor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  application.SendNotification();
  application.Render();

  textEditor.SetKeyInputFocus();
  textEditor.SetProperty( DevelTextEditor::Property::ENABLE_EDITING, false );
  application.ProcessEvent( GenerateKey( "D", "", "D", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "D", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( textEditor.GetProperty( TextEditor::Property::TEXT ).Get<std::string>(), "", TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::ENABLE_EDITING ).Get<bool>(), false, TEST_LOCATION );

  textEditor.SetKeyInputFocus();
  textEditor.SetProperty( DevelTextEditor::Property::ENABLE_EDITING, true );
  application.ProcessEvent( GenerateKey( "D", "", "D", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "D", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( textEditor.GetProperty( TextEditor::Property::TEXT ).Get<std::string>(), "D", TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::ENABLE_EDITING ).Get<bool>(), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextEditorScrolling(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorScrolling ");

  TextEditor textEditor = TextEditor::New();
  DALI_TEST_CHECK( textEditor );

  application.GetScene().Add( textEditor );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  application.SendNotification();
  application.Render();

  textEditor.SetProperty(TextEditor::Property::TEXT, "Tex1\nTex2\nTex3\nTex4\nTex5\nTex6\nTex7\nTex8");
  textEditor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
  textEditor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
  textEditor.SetProperty(Actor::Property::SIZE, Vector2(60.0f, 160.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::VERTICAL_SCROLL_POSITION ).Get<float>(), 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::HORIZONTAL_SCROLL_POSITION ).Get<float>(), 0.0f, TEST_LOCATION );


  DevelTextEditor::ScrollBy(textEditor, Vector2(1.0f, 1.0f));

  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::VERTICAL_SCROLL_POSITION ).Get<float>(), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::HORIZONTAL_SCROLL_POSITION ).Get<float>(), 0.0f, TEST_LOCATION );

  DevelTextEditor::ScrollBy(textEditor, Vector2(0.0f, 1000.0f));

  DALI_TEST_NOT_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::VERTICAL_SCROLL_POSITION ).Get<float>(), 1.0f, 0.1f, TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::HORIZONTAL_SCROLL_POSITION ).Get<float>(), 0.0f, TEST_LOCATION );

  textEditor.SetProperty(DevelTextEditor::Property::VERTICAL_SCROLL_POSITION , 0.0f);
  textEditor.SetProperty(DevelTextEditor::Property::HORIZONTAL_SCROLL_POSITION , 0.0f);

  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::VERTICAL_SCROLL_POSITION ).Get<float>(), 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::HORIZONTAL_SCROLL_POSITION ).Get<float>(), 0.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitTextEditorFontSizeScale(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextEditorFontSizeScale");

  TextEditor textEditor = TextEditor::New();
  textEditor.SetProperty( TextEditor::Property::POINT_SIZE, 30.f );
  textEditor.SetProperty( TextEditor::Property::TEXT, "Test" );
  Vector3 nonScaledSize = textEditor.GetNaturalSize();

  TextEditor textEditorScaled = TextEditor::New();
  textEditorScaled.SetProperty( TextEditor::Property::POINT_SIZE, 15.f );
  textEditorScaled.SetProperty( Toolkit::DevelTextEditor::Property::FONT_SIZE_SCALE, 2.f );
  textEditorScaled.SetProperty( TextEditor::Property::TEXT, "Test" );
  Vector3 scaledSize = textEditorScaled.GetNaturalSize();

  DALI_TEST_EQUALS( nonScaledSize, scaledSize, TEST_LOCATION );

  textEditor.SetProperty( TextEditor::Property::PIXEL_SIZE, 30.f );
  textEditor.SetProperty( TextEditor::Property::TEXT, "Test" );
  nonScaledSize = textEditor.GetNaturalSize();

  textEditorScaled.SetProperty( TextEditor::Property::PIXEL_SIZE, 15.f );
  textEditorScaled.SetProperty( Toolkit::DevelTextEditor::Property::FONT_SIZE_SCALE, 2.f );
  textEditorScaled.SetProperty( TextEditor::Property::TEXT, "Test" );
  scaledSize = textEditorScaled.GetNaturalSize();

  DALI_TEST_EQUALS( nonScaledSize, scaledSize, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextEditorPrimaryCursorPosition(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextPrimaryCursorPosition ");

  TextEditor textEditor = TextEditor::New();

  application.GetScene().Add( textEditor );

  textEditor.SetProperty( TextEditor::Property::TEXT, "ABCEF");
  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 300.f, 50.f ) );
  textEditor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  textEditor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  textEditor.SetProperty( DevelTextEditor::Property::PRIMARY_CURSOR_POSITION, 3);
  application.SendNotification();
  application.Render();
  textEditor.SetKeyInputFocus();

  application.ProcessEvent( GenerateKey( "D", "", "D", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "D", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( textEditor.GetProperty( TextEditor::Property::TEXT ).Get<std::string>(), "ABCDEF", TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::PRIMARY_CURSOR_POSITION ).Get<int>(), 4, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextEditorLineCountAfterGetNaturalSize(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorLineCountAfterGetNaturalSize ");

  TextEditor textEditor = TextEditor::New();
  textEditor.SetProperty(TextEditor::Property::TEXT, "A\nB\nC\nD\nE\nF\n");
  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 300.f, 50.f ) );
  textEditor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  textEditor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  application.GetScene().Add( textEditor );

  application.SendNotification();
  application.Render();

  int lineCount = 0;
  lineCount =  textEditor.GetProperty<int>( TextEditor::Property::LINE_COUNT );
  DALI_TEST_EQUALS( lineCount, 7, TEST_LOCATION );

  textEditor.GetNaturalSize();

  // Create a tap event to touch the text editor.
  TestGenerateTap( application, 18.0f, 25.0f );

  application.SendNotification();
  application.Render();

  lineCount =  textEditor.GetProperty<int>( TextEditor::Property::LINE_COUNT );
  DALI_TEST_EQUALS( lineCount, 7, TEST_LOCATION );

  END_TEST;
}


int utcDaliTextEditorGetHeightForWidthDoesNotChangeLineCountScrollingCase(void)
{
  ToolkitTestApplication application;

  tet_infoline(" utcDaliTextEditorGetHeightForWidthDoesNotChangeLineCountScrollingCase ");

  int lineCountBefore =0 ;
  int lineCountAfter =0 ;

  // Create a text editor
  TextEditor textEditor = TextEditor::New();
  //Set very large font-size using point-size
  textEditor.SetProperty( TextEditor::Property::POINT_SIZE, 10) ;
  //Specify font-family
  textEditor.SetProperty( TextEditor::Property::FONT_FAMILY, "DejaVu Sans");
  //Specify size
  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 100.f ) );
  //Set text longer than width of textEditor
  textEditor.SetProperty( TextEditor::Property::TEXT, "TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST ");

  application.GetScene().Add( textEditor );

  application.SendNotification();
  application.Render();

  //Failed case is the GetHeightForWidth change LineCount then the scrollor will not arrive to latest line
  //GetHeightForWidth is a retrieval method which should not modify object
  lineCountBefore =  textEditor.GetProperty<int>( TextEditor::Property::LINE_COUNT );
  textEditor.GetHeightForWidth(200.f);

  //This is to simulate focus into text editor after calling GetHeightForWidth
  //Create a tap event to touch the text editor.
  TestGenerateTap( application, 18.0f, 25.0f );

  application.SendNotification();
  application.Render();

  lineCountAfter =  textEditor.GetProperty<int>( TextEditor::Property::LINE_COUNT );

  //The LineCount must not be changed when calling GetHeightForWidth.
  DALI_TEST_EQUALS( lineCountAfter , lineCountBefore, TEST_LOCATION );

  END_TEST;
}

int utcDaliTextEditorGetHeightForWidthDoesNotChangeLineCountLineWrapCharCase(void)
{
  ToolkitTestApplication application;

  tet_infoline(" utcDaliTextEditorGetHeightForWidthDoesNotChangeLineCountLineWrapCharCase ");

  int lineCountBefore =0 ;
  int lineCountAfter =0 ;

  // Create a text editor
  TextEditor textEditor = TextEditor::New();
  //Set very large font-size using point-size
  textEditor.SetProperty( TextEditor::Property::POINT_SIZE, 10) ;
  //Specify font-family
  textEditor.SetProperty( TextEditor::Property::FONT_FAMILY, "DejaVu Sans");
  //Specify size
  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 50.f, 100.f ) );
  //Set text longer than width of textEditor
  textEditor.SetProperty( TextEditor::Property::TEXT, "qwertyuiopasdfghjklzxcvbnm\n");
  //Set line wrap mode Character
  textEditor.SetProperty(TextEditor::Property::LINE_WRAP_MODE, "CHARACTER");

  application.GetScene().Add( textEditor );

  application.SendNotification();
  application.Render();

  //Failed case is the GetHeightForWidth change LineCount which make position of cursor invalid in TextEditor
  //GetHeightForWidth is a retrieval method which should not modify object
  lineCountBefore =  textEditor.GetProperty<int>( TextEditor::Property::LINE_COUNT );
  textEditor.GetHeightForWidth(200.f);

  //This is to simulate focus into text editor after calling GetHeightForWidth
  //Create a tap event to touch the text editor.
  TestGenerateTap( application, 18.0f, 25.0f );

  application.SendNotification();
  application.Render();

  lineCountAfter =  textEditor.GetProperty<int>( TextEditor::Property::LINE_COUNT );

  //The LineCount must not be changed when calling GetHeightForWidth.
  DALI_TEST_EQUALS( lineCountAfter , lineCountBefore, TEST_LOCATION );

  END_TEST;
}

int utcDaliTextEditorGetHeightForWidthChangeLineCountWhenTextChanged(void)
{
  ToolkitTestApplication application;

  tet_infoline(" utcDaliTextEditorGetHeightForWidthChangeLineCountWhenTextChanged ");

  int lineCountBefore =0 ;
  int lineCountAfter =0 ;

  // Create a text editor
  TextEditor textEditor = TextEditor::New();
  //Set very large font-size using point-size
  textEditor.SetProperty( TextEditor::Property::POINT_SIZE, 10) ;
  //Specify font-family
  textEditor.SetProperty( TextEditor::Property::FONT_FAMILY, "DejaVu Sans");
  //Specify size
  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 100.f ) );
  //Set text longer than width of textEditor
  textEditor.SetProperty( TextEditor::Property::TEXT, "Short text");
  //Set line wrap mode Character
  textEditor.SetProperty(TextEditor::Property::LINE_WRAP_MODE, "CHARACTER");

  application.GetScene().Add( textEditor );

  application.SendNotification();
  application.Render();


  lineCountBefore =  textEditor.GetProperty<int>( TextEditor::Property::LINE_COUNT );

  textEditor.SetProperty( TextEditor::Property::TEXT, "This is very loooooooooooooooooooooooooooooooooooong text for test");
  lineCountAfter =  textEditor.GetProperty<int>( TextEditor::Property::LINE_COUNT );

  // When the text changed, the Line-count should be updated according to new text.
  // Because the GetHeightForWidth is called in Controller::GetLineCount(float width)
  DALI_TEST_EQUALS( lineCountBefore ,1, TEST_LOCATION );
  DALI_TEST_GREATER( lineCountAfter,1, TEST_LOCATION );


  END_TEST;
}


int utcDaliTextEditorGetNaturalSizeDoesNotChangeLineCountScrollingCase(void)
{
  ToolkitTestApplication application;

  tet_infoline(" utcDaliTextEditorGetNaturalSizeDoesNotChangeLineCountScrollingCase ");

  int lineCountBefore =0 ;
  int lineCountAfter =0 ;

  // Create a text editor
  TextEditor textEditor = TextEditor::New();
  //Set very large font-size using point-size
  textEditor.SetProperty( TextEditor::Property::POINT_SIZE, 10) ;
  //Specify font-family
  textEditor.SetProperty( TextEditor::Property::FONT_FAMILY, "DejaVu Sans");
  //Specify size
  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 100.f ) );
  //Set text longer than width of textEditor
  textEditor.SetProperty( TextEditor::Property::TEXT, "TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST ");

  application.GetScene().Add( textEditor );

  application.SendNotification();
  application.Render();

  //Failed case is the GetNaturalSize change LineCount then the scrollor will not arrive to latest line
  //GetNaturalSize is a retrieval method which should not modify object
  lineCountBefore =  textEditor.GetProperty<int>( TextEditor::Property::LINE_COUNT );
  textEditor.GetNaturalSize();

  //This is to simulate focus into text editor after calling GetNaturalSize
  //Create a tap event to touch the text editor.
  TestGenerateTap( application, 18.0f, 25.0f );

  application.SendNotification();
  application.Render();

  lineCountAfter =  textEditor.GetProperty<int>( TextEditor::Property::LINE_COUNT );

  //The LineCount must not be changed when calling GetNaturalSize.
  DALI_TEST_EQUALS( lineCountAfter , lineCountBefore, TEST_LOCATION );

  END_TEST;
}

int utcDaliTextEditorGetNaturalSizeDoesNotChangeLineCountLineWrapCharCase(void)
{
  ToolkitTestApplication application;

  tet_infoline(" utcDaliTextEditorGetNaturalSizeDoesNotChangeLineCountLineWrapCharCase ");

  int lineCountBefore =0 ;
  int lineCountAfter =0 ;

  // Create a text editor
  TextEditor textEditor = TextEditor::New();
  //Set very large font-size using point-size
  textEditor.SetProperty( TextEditor::Property::POINT_SIZE, 10) ;
  //Specify font-family
  textEditor.SetProperty( TextEditor::Property::FONT_FAMILY, "DejaVu Sans");
  //Specify size
  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 50.f, 100.f ) );
  //Set text longer than width of textEditor
  textEditor.SetProperty( TextEditor::Property::TEXT, "qwertyuiopasdfghjklzxcvbnm\n");
  //Set line wrap mode Character
  textEditor.SetProperty(TextEditor::Property::LINE_WRAP_MODE, "CHARACTER");

  application.GetScene().Add( textEditor );

  application.SendNotification();
  application.Render();

  //Failed case is the GetNaturalSize change LineCount which make position of cursor invalid in TextEditor
  //GetNaturalSize is a retrieval method which should not modify object
  lineCountBefore =  textEditor.GetProperty<int>( TextEditor::Property::LINE_COUNT );
  textEditor.GetNaturalSize( );

  //This is to simulate focus into text editor after calling GetNaturalSize
  //Create a tap event to touch the text editor.
  TestGenerateTap( application, 18.0f, 25.0f );

  application.SendNotification();
  application.Render();

  lineCountAfter =  textEditor.GetProperty<int>( TextEditor::Property::LINE_COUNT );

  //The LineCount must not be changed when calling GetNaturalSize.
  DALI_TEST_EQUALS( lineCountAfter , lineCountBefore, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextEditorAtlasLimitationIsEnabledForLargeFontPointSize(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorAtlasLimitationIsEnabledForLargeFontPointSize ");

  // +2: First one to handle the equal case. Second one to handle odd to even case of GetNaturalSize
  const uint32_t lessThanWidth = TextAbstraction::FontClient::MAX_TEXT_ATLAS_WIDTH - TextAbstraction::FontClient::PADDING_TEXT_ATLAS_BLOCK + 2;
  const uint32_t lessThanHeight = TextAbstraction::FontClient::MAX_TEXT_ATLAS_HEIGHT - TextAbstraction::FontClient::PADDING_TEXT_ATLAS_BLOCK + 2;

  // Create a text editor
  TextEditor textEditor = TextEditor::New();

  //Set size to avoid automatic eliding
  textEditor.SetProperty( Actor::Property::SIZE, Vector2(1025, 1025));
  //Set very large font-size using point-size
  textEditor.SetProperty( TextEditor::Property::POINT_SIZE, 1000) ;
  //Specify font-family
  textEditor.SetProperty( TextEditor::Property::FONT_FAMILY, "DejaVu Sans");
  //Set text to check if appear or not
  textEditor.SetProperty( TextEditor::Property::TEXT, "A");

  application.GetScene().Add( textEditor );

  application.SendNotification();
  application.Render();
  //Use GetNaturalSize to verify that size of block does not exceed Atlas size
  Vector3 naturalSize = textEditor.GetNaturalSize();

  DALI_TEST_GREATER( lessThanWidth, static_cast<uint32_t>(naturalSize.width), TEST_LOCATION );
  DALI_TEST_GREATER( lessThanHeight, static_cast<uint32_t>(naturalSize.height), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextEditorAtlasLimitationIsEnabledPerformanceCases(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorAtlasLimitationIsEnabledPerformanceCases ");

  // +2: First one to handle the equal case. Second one to handle odd to even case of GetNaturalSize
  const uint32_t lessThanWidth = TextAbstraction::FontClient::MAX_TEXT_ATLAS_WIDTH - TextAbstraction::FontClient::PADDING_TEXT_ATLAS_BLOCK + 2;
  const uint32_t lessThanHeight = TextAbstraction::FontClient::MAX_TEXT_ATLAS_HEIGHT - TextAbstraction::FontClient::PADDING_TEXT_ATLAS_BLOCK + 2;

  Vector3 naturalSize; //Use GetNaturalSize to verify that size of block does not exceed Atlas size
  // Create a text editor
  TextEditor textEditor = TextEditor::New();
  //Set size to avoid automatic eliding
  textEditor.SetProperty( Actor::Property::SIZE, Vector2(1025, 1025));
  textEditor.SetProperty( TextEditor::Property::FONT_FAMILY, "DejaVu Sans");
  textEditor.SetProperty( TextEditor::Property::TEXT, "A");

  const int numberOfCases = 6;
  int arrayCases[numberOfCases] = {323, 326, 330, 600, 1630, 2500};

  for (int index=0; index < numberOfCases; index++)
  {
    tet_printf(" UtcDaliTextEditorAtlasLimitationIsEnabledPerformanceCases point-size= %d \n", arrayCases[index]);
    textEditor.SetProperty( TextEditor::Property::POINT_SIZE, arrayCases[index]) ;
    application.GetScene().Add( textEditor );
    application.SendNotification();
    application.Render();
    naturalSize = textEditor.GetNaturalSize();
    DALI_TEST_GREATER( lessThanWidth, static_cast<uint32_t>(naturalSize.width), TEST_LOCATION );
    DALI_TEST_GREATER( lessThanHeight, static_cast<uint32_t>(naturalSize.height), TEST_LOCATION );

  }

  END_TEST;
}

int UtcDaliTextEditorHyphenWrapMode(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorHyphenWrapMode ");

  int lineCount =0;
  TextEditor textEditor = TextEditor::New();

  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 150.0f, 300.f ) );

  application.GetScene().Add( textEditor );
  application.SendNotification();
  application.Render();

  textEditor.SetProperty( TextEditor::Property::TEXT, "Hi Experimen" );
  textEditor.SetProperty(TextEditor::Property::LINE_WRAP_MODE, DevelText::LineWrap::HYPHENATION);
  DALI_TEST_EQUALS( textEditor.GetProperty< int >( TextEditor::Property::LINE_WRAP_MODE ), static_cast< int >( DevelText::LineWrap::HYPHENATION ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  lineCount = textEditor.GetProperty<int>( TextEditor::Property::LINE_COUNT );
  /*
    text will be :
    Hi Exp-
    erimen
  */
  DALI_TEST_EQUALS( lineCount, 2, TEST_LOCATION );

  textEditor.SetProperty( TextEditor::Property::TEXT, "Hi Experimen" );
  textEditor.SetProperty(TextEditor::Property::LINE_WRAP_MODE, DevelText::LineWrap::MIXED);
  DALI_TEST_EQUALS( textEditor.GetProperty< int >( TextEditor::Property::LINE_WRAP_MODE ), static_cast< int >( DevelText::LineWrap::MIXED ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  lineCount = textEditor.GetProperty<int>( TextEditor::Property::LINE_COUNT );
  /*
    text will be :
    Hi
    Experi-
    men
  */
  DALI_TEST_EQUALS( lineCount, 3, TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitTextEditorEllipsisPositionProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextEditorEllipsisPositionProperty ");
  TextEditor textEditor = TextEditor::New();

  tet_infoline(" UtcDaliToolkitTextEditorEllipsisPositionProperty - Default is END");
  DALI_TEST_EQUALS( textEditor.GetProperty< int >( DevelTextEditor::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::END ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextEditorEllipsisPositionProperty - Change to START");
  textEditor.SetProperty(DevelTextEditor::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::START);
  DALI_TEST_EQUALS( textEditor.GetProperty< int >( DevelTextEditor::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::START ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextEditorEllipsisPositionProperty - Change to MIDDLE");
  textEditor.SetProperty(DevelTextEditor::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::MIDDLE);
  DALI_TEST_EQUALS( textEditor.GetProperty< int >( DevelTextEditor::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::MIDDLE ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextEditorEllipsisPositionProperty - Change to END");
  textEditor.SetProperty(DevelTextEditor::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::END);
  DALI_TEST_EQUALS( textEditor.GetProperty< int >( DevelTextEditor::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::END ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextEditorEllipsisPositionProperty - Change to START using integer");
  textEditor.SetProperty(DevelTextEditor::Property::ELLIPSIS_POSITION, 1);
  DALI_TEST_EQUALS( textEditor.GetProperty< int >( DevelTextEditor::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::START ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextEditorEllipsisPositionProperty - Change to MIDDLE using integer");
  textEditor.SetProperty(DevelTextEditor::Property::ELLIPSIS_POSITION, 2);
  DALI_TEST_EQUALS( textEditor.GetProperty< int >( DevelTextEditor::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::MIDDLE ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextEditorEllipsisPositionProperty - Change to END using integer");
  textEditor.SetProperty(DevelTextEditor::Property::ELLIPSIS_POSITION, 0);
  DALI_TEST_EQUALS( textEditor.GetProperty< int >( DevelTextEditor::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::END ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to START using string - uppercase");
  textEditor.SetProperty(DevelTextEditor::Property::ELLIPSIS_POSITION, "START");
  DALI_TEST_EQUALS( textEditor.GetProperty< int >( DevelTextEditor::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::START ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to MIDDLE using string - uppercase");
  textEditor.SetProperty(DevelTextEditor::Property::ELLIPSIS_POSITION, "MIDDLE");
  DALI_TEST_EQUALS( textEditor.GetProperty< int >( DevelTextEditor::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::MIDDLE ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to END using string - uppercase");
  textEditor.SetProperty(DevelTextEditor::Property::ELLIPSIS_POSITION, "END");
  DALI_TEST_EQUALS( textEditor.GetProperty< int >( DevelTextEditor::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::END ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to START using string - lowercase");
  textEditor.SetProperty(DevelTextEditor::Property::ELLIPSIS_POSITION, "start");
  DALI_TEST_EQUALS( textEditor.GetProperty< int >( DevelTextEditor::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::START ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to MIDDLE using string - lowercase");
  textEditor.SetProperty(DevelTextEditor::Property::ELLIPSIS_POSITION, "middle");
  DALI_TEST_EQUALS( textEditor.GetProperty< int >( DevelTextEditor::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::MIDDLE ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to END using string - lowercase");
  textEditor.SetProperty(DevelTextEditor::Property::ELLIPSIS_POSITION, "end");
  DALI_TEST_EQUALS( textEditor.GetProperty< int >( DevelTextEditor::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::END ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextEditorCopyText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorCopyText ");

  TextEditor textEditor = TextEditor::New();

  std::string selectedText = "";
  std::string copiedText = "";

  application.GetScene().Add( textEditor );

  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 300.f, 50.f ) );
  textEditor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  textEditor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  application.SendNotification();
  application.Render();

  textEditor.SetProperty( TextEditor::Property::TEXT, "Hello world" );

  application.SendNotification();
  application.Render();

  // Hello is selected
  DevelTextEditor::SelectText( textEditor, 0, 5 );

  application.SendNotification();
  application.Render();

  selectedText = textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT ).Get<std::string>();
  DALI_TEST_EQUALS( "Hello", selectedText, TEST_LOCATION );

  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_START ).Get<int>(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_END ).Get<int>(), 5, TEST_LOCATION );

  // Hello is copied
  copiedText = DevelTextEditor::CopyText( textEditor );
  DALI_TEST_EQUALS( "Hello", copiedText, TEST_LOCATION );

  // world is selected
  DevelTextEditor::SelectText( textEditor, 6, 11 );

  application.SendNotification();
  application.Render();

  selectedText = textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT ).Get<std::string>();
  DALI_TEST_EQUALS( "world", selectedText, TEST_LOCATION );

  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_START ).Get<int>(), 6, TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_END ).Get<int>(), 11, TEST_LOCATION );

  // world is copied
  copiedText = DevelTextEditor::CopyText( textEditor );
  DALI_TEST_EQUALS( "world", copiedText, TEST_LOCATION );

  // "lo wo" is selected
  DevelTextEditor::SelectText( textEditor, 3, 8 );

  application.SendNotification();
  application.Render();

  selectedText = textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT ).Get<std::string>();
  DALI_TEST_EQUALS( "lo wo", selectedText, TEST_LOCATION );

  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_START ).Get<int>(), 3, TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_END ).Get<int>(), 8, TEST_LOCATION );

  // "lo wo" is copied
  copiedText = DevelTextEditor::CopyText( textEditor );
  DALI_TEST_EQUALS( "lo wo", copiedText, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextEditorCutText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorCutText ");

  TextEditor textEditor = TextEditor::New();

  std::string selectedText = "";

  application.GetScene().Add( textEditor );

  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 300.f, 50.f ) );
  textEditor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  textEditor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  application.SendNotification();
  application.Render();

  textEditor.SetProperty( TextEditor::Property::TEXT, "Hello world" );

  application.SendNotification();
  application.Render();

  // Hello is selected
  DevelTextEditor::SelectText( textEditor, 0, 5 );

  application.SendNotification();
  application.Render();

  selectedText = textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT ).Get<std::string>();
  DALI_TEST_EQUALS( "Hello", selectedText, TEST_LOCATION );

  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_START ).Get<int>(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_END ).Get<int>(), 5, TEST_LOCATION );

  // Hello is cut
  DALI_TEST_EQUALS( "Hello", DevelTextEditor::CutText( textEditor ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( textEditor.GetProperty( TextEditor::Property::TEXT ).Get<std::string>(), " world", TEST_LOCATION );

  // " w" is selected
  DevelTextEditor::SelectText( textEditor, 0, 2 );

  application.SendNotification();
  application.Render();

  selectedText = textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT ).Get<std::string>();
  DALI_TEST_EQUALS( " w", selectedText, TEST_LOCATION );

  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_START ).Get<int>(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_END ).Get<int>(), 2, TEST_LOCATION );

  // " w" is cut
  DALI_TEST_EQUALS( " w", DevelTextEditor::CutText( textEditor ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( textEditor.GetProperty( TextEditor::Property::TEXT ).Get<std::string>(), "orld", TEST_LOCATION );

  // Test Cut from the middle

  // "rl" is selected
  DevelTextEditor::SelectText( textEditor, 1, 3 );

  application.SendNotification();
  application.Render();

  selectedText = textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT ).Get<std::string>();
  DALI_TEST_EQUALS( "rl", selectedText, TEST_LOCATION );

  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_START ).Get<int>(), 1, TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_END ).Get<int>(), 3, TEST_LOCATION );

  // "rl" is cut
  DALI_TEST_EQUALS( "rl", DevelTextEditor::CutText( textEditor ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( textEditor.GetProperty( TextEditor::Property::TEXT ).Get<std::string>(), "od", TEST_LOCATION );

  // Test Cut from the end

  // "d" is selected
  DevelTextEditor::SelectText( textEditor, 1, 2 );

  application.SendNotification();
  application.Render();

  selectedText = textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT ).Get<std::string>();
  DALI_TEST_EQUALS( "d", selectedText, TEST_LOCATION );

  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_START ).Get<int>(), 1, TEST_LOCATION );
  DALI_TEST_EQUALS( textEditor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT_END ).Get<int>(), 2, TEST_LOCATION );

  // "d" is cut
  DALI_TEST_EQUALS( "d", DevelTextEditor::CutText( textEditor ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( textEditor.GetProperty( TextEditor::Property::TEXT ).Get<std::string>(), "o", TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextEditorPasteText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorPasteText ");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  std::string cutText = "";
  std::string copiedText = "";

  editor.SetProperty( TextEditor::Property::TEXT, "Hello\nworld\nHello world" );
  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap on the text editor
  TestGenerateTap( application, 3.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Move to second line of the text.
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Select some text in the right of the current cursor position
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_SHIFT_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Cut the selected text
  cutText = DevelTextEditor::CutText(editor);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( "wor", cutText, TEST_LOCATION );
  DALI_TEST_EQUALS( "Hello\nld\nHello world", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );

  // Select some text in the left of the current cursor position
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_SHIFT_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN,  "",DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Copy the selected text
  copiedText = DevelTextEditor::CopyText(editor);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( "lo\n", copiedText, TEST_LOCATION );
  DALI_TEST_EQUALS( "Hello\nld\nHello world", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );


  // Move the cursor to the third line
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Paste the selected text at the current cursor position
  DevelTextEditor::PasteText(editor);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( "Hello\nld\nHello lo\nworld", editor.GetProperty<std::string>( TextEditor::Property::TEXT ), TEST_LOCATION );

  END_TEST;
}
int UtcDaliTextEditorLineSpacing(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorLineSpacing ");

  TextEditor textEditor = TextEditor::New();
  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 400.0f, 400.f ) );
  application.GetScene().Add( textEditor );
  application.SendNotification();
  application.Render();

  textEditor.SetProperty( TextEditor::Property::TEXT, "Line #1\nLine #2\nLine #3" );
  textEditor.SetProperty( DevelTextEditor::Property::LINE_SPACING, 0 );

  Vector3 sizeBefore = textEditor.GetNaturalSize();

  textEditor.SetProperty( DevelTextEditor::Property::LINE_SPACING, 20 );

  //add 20 for each line  20 * 3
  DALI_TEST_EQUALS(sizeBefore.height + 60.0f, textEditor.GetNaturalSize().height, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextEditorMinLineSize(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorMinLineSize ");

  TextEditor textEditor = TextEditor::New();
  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 400.0f, 400.f ) );
  application.GetScene().Add( textEditor );
  application.SendNotification();
  application.Render();

  textEditor.SetProperty( TextEditor::Property::TEXT, "Line #1\nLine #2\nLine #3" );
  textEditor.SetProperty( DevelTextEditor::Property::MIN_LINE_SIZE, 0 );

  Vector3 sizeBefore = textEditor.GetNaturalSize();

  textEditor.SetProperty( DevelTextEditor::Property::MIN_LINE_SIZE, 60 );

  DALI_TEST_NOT_EQUALS( sizeBefore, textEditor.GetNaturalSize(), 0.0f, TEST_LOCATION);

  //60 * 3 lines
  DALI_TEST_EQUALS(180.0f, textEditor.GetNaturalSize().height, TEST_LOCATION);

  END_TEST;
}

int utcDaliTextEditorCursorPositionChangedSignal(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorCursorPositionChangedSignal");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  // connect to the selection changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextEditor::CursorPositionChangedSignal(editor).Connect(&TestCursorPositionChangedCallback);
  bool cursorPositionChangedSignal = false;
  editor.ConnectSignal( testTracker, "cursorPositionChanged",   CallbackFunctor(&cursorPositionChangedSignal) );

  editor.SetProperty( TextEditor::Property::TEXT, "Hello\nworld\nHello world" );
  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  editor.SetKeyInputFocus();

  // Tap on the text editor
  TestGenerateTap( application, 3.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gCursorPositionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldCursorPos, 23, TEST_LOCATION);

  gCursorPositionChangedCallbackCalled = false;

  // Move to left.
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gCursorPositionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldCursorPos, 18, TEST_LOCATION);

  gCursorPositionChangedCallbackCalled = false;

  // Insert C
  application.ProcessEvent( GenerateKey( "c", "", "c", KEY_C_CODE, 0, 0, Integration::KeyEvent::DOWN, "c", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gCursorPositionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldCursorPos, 17, TEST_LOCATION);

  gCursorPositionChangedCallbackCalled = false;

  //delete one character
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gCursorPositionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldCursorPos, 18, TEST_LOCATION);

  gCursorPositionChangedCallbackCalled = false;

  editor.SetProperty( TextEditor::Property::TEXT, "Hello" );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gCursorPositionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldCursorPos, 17, TEST_LOCATION);

  gCursorPositionChangedCallbackCalled = false;

  editor.SetProperty( DevelTextEditor::Property::PRIMARY_CURSOR_POSITION, 3);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gCursorPositionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldCursorPos, 5, TEST_LOCATION);

  END_TEST;
}

int utcDaliTextEditorGeometryEllipsisStart(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorGeometryEllipsisStart");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::POINT_SIZE, 7.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  editor.SetProperty( TextEditor::Property::ENABLE_MARKUP, true );
  editor.SetProperty( DevelTextEditor::Property::ENABLE_SCROLL_BAR, false );
  editor.SetProperty( DevelTextEditor::Property::ELLIPSIS, true );
  editor.SetProperty( DevelTextEditor::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::START );
  editor.SetProperty( TextEditor::Property::TEXT, "line1 \nline2\nline 3\nline4" );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  unsigned int expectedCount = 2;
  unsigned int startIndex = 0;
  unsigned int endIndex = 24;

  Vector<Vector2> positionsList = DevelTextEditor::GetTextPosition(editor, startIndex, endIndex);
  Vector<Vector2> sizeList = DevelTextEditor::GetTextSize(editor, startIndex, endIndex);

  DALI_TEST_EQUALS(positionsList.Size(), expectedCount, TEST_LOCATION);
  DALI_TEST_EQUALS(sizeList.Size(), expectedCount, TEST_LOCATION);

  Vector<Vector2> expectedSizes;
  Vector<Vector2> expectedPositions;

  expectedPositions.PushBack(Vector2(37, 0));
  expectedSizes.PushBack(Vector2(20, 25));

  expectedPositions.PushBack(Vector2(-1, 25));
  expectedSizes.PushBack(Vector2(52, 25));

  TestTextGeometryUtils::CheckGeometryResult(positionsList, sizeList, expectedPositions, expectedSizes);

  END_TEST;
}

int utcDaliTextEditorGeometryEllipsisMiddle(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorGeometryEllipsisMiddle");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::POINT_SIZE, 7.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  editor.SetProperty( TextEditor::Property::ENABLE_MARKUP, true );
  editor.SetProperty( DevelTextEditor::Property::ENABLE_SCROLL_BAR, false );
  editor.SetProperty( DevelTextEditor::Property::ELLIPSIS, true );
  editor.SetProperty( DevelTextEditor::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::MIDDLE );
  editor.SetProperty( TextEditor::Property::TEXT, "line1 \nline2\nline 3\nline4" );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  unsigned int expectedCount = 2;
  unsigned int startIndex = 0;
  unsigned int endIndex = 24;

  Vector<Vector2> positionsList = DevelTextEditor::GetTextPosition(editor, startIndex, endIndex);
  Vector<Vector2> sizeList = DevelTextEditor::GetTextSize(editor, startIndex, endIndex);

  DALI_TEST_EQUALS(positionsList.Size(), expectedCount, TEST_LOCATION);
  DALI_TEST_EQUALS(sizeList.Size(), expectedCount, TEST_LOCATION);

  Vector<Vector2> expectedSizes;
  Vector<Vector2> expectedPositions;

  expectedPositions.PushBack(Vector2(-1, 0));
  expectedSizes.PushBack(Vector2(25, 25));

  expectedPositions.PushBack(Vector2(-1, 25));
  expectedSizes.PushBack(Vector2(52, 25));

  TestTextGeometryUtils::CheckGeometryResult(positionsList, sizeList, expectedPositions, expectedSizes);

  END_TEST;
}

int utcDaliTextEditorGeometryEllipsisEnd(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorGeometryEllipsisEnd");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::POINT_SIZE, 7.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  editor.SetProperty( TextEditor::Property::ENABLE_MARKUP, true );
  editor.SetProperty( DevelTextEditor::Property::ENABLE_SCROLL_BAR, false );
  editor.SetProperty( DevelTextEditor::Property::ELLIPSIS, true );
  editor.SetProperty( DevelTextEditor::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::END );
  editor.SetProperty( TextEditor::Property::TEXT, "line1 \nline2\nline 3\nline4" );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  unsigned int expectedCount = 2;
  unsigned int startIndex = 0;
  unsigned int endIndex = 24;

  Vector<Vector2> positionsList = DevelTextEditor::GetTextPosition(editor, startIndex, endIndex);
  Vector<Vector2> sizeList = DevelTextEditor::GetTextSize(editor, startIndex, endIndex);

  DALI_TEST_EQUALS(positionsList.Size(), expectedCount, TEST_LOCATION);
  DALI_TEST_EQUALS(sizeList.Size(), expectedCount, TEST_LOCATION);

  Vector<Vector2> expectedSizes;
  Vector<Vector2> expectedPositions;

  expectedPositions.PushBack(Vector2(-1, 0));
  expectedSizes.PushBack(Vector2(59, 25));

  expectedPositions.PushBack(Vector2(-1, 25));
  expectedSizes.PushBack(Vector2(25, 25));

  TestTextGeometryUtils::CheckGeometryResult(positionsList, sizeList, expectedPositions, expectedSizes);

  END_TEST;
}

int utcDaliTextEditorGeometryRTL(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorGeometryRTL");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::POINT_SIZE, 7.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  editor.SetProperty( TextEditor::Property::ENABLE_MARKUP, true );
  editor.SetProperty( TextEditor::Property::TEXT, "line1 \nline2\nline 3\nالاخيرالسطر" );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  unsigned int expectedCount = 4;
  unsigned int startIndex = 3;
  unsigned int endIndex = 24;

  Vector<Vector2> positionsList = DevelTextEditor::GetTextPosition(editor, startIndex, endIndex);
  Vector<Vector2> sizeList = DevelTextEditor::GetTextSize(editor, startIndex, endIndex);

  DALI_TEST_EQUALS(positionsList.Size(), expectedCount, TEST_LOCATION);
  DALI_TEST_EQUALS(sizeList.Size(), expectedCount, TEST_LOCATION);

  Vector<Vector2> expectedSizes;
  Vector<Vector2> expectedPositions;

  expectedPositions.PushBack(Vector2(24, 0));
  expectedSizes.PushBack(Vector2(33, 25));

  expectedPositions.PushBack(Vector2(-1, 25));
  expectedSizes.PushBack(Vector2(52, 25));

  expectedPositions.PushBack(Vector2(-1, 50));
  expectedSizes.PushBack(Vector2(59, 25));

  expectedPositions.PushBack(Vector2(61, 75));
  expectedSizes.PushBack(Vector2(37, 25));

  TestTextGeometryUtils::CheckGeometryResult(positionsList, sizeList, expectedPositions, expectedSizes);

  END_TEST;
}

int utcDaliTextEditorGeometryGlyphMiddle(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorGeometryGlyphMiddle");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::POINT_SIZE, 7.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 150.f, 200.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  editor.SetProperty( TextEditor::Property::ENABLE_MARKUP, true );
  editor.SetProperty( TextEditor::Property::TEXT, "لا تحتوي على لا" );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  unsigned int expectedCount = 1;
  unsigned int startIndex = 1;
  unsigned int endIndex = 13;

  Vector<Vector2> positionsList = DevelTextEditor::GetTextPosition(editor, startIndex, endIndex);
  Vector<Vector2> sizeList = DevelTextEditor::GetTextSize(editor, startIndex, endIndex);

  DALI_TEST_EQUALS(positionsList.Size(), expectedCount, TEST_LOCATION);
  DALI_TEST_EQUALS(sizeList.Size(), expectedCount, TEST_LOCATION);

  Vector<Vector2> expectedSizes;
  Vector<Vector2> expectedPositions;

  expectedPositions.PushBack(Vector2(6, 0));
  expectedSizes.PushBack(Vector2(124, 25));

  TestTextGeometryUtils::CheckGeometryResult(positionsList, sizeList, expectedPositions, expectedSizes);

  END_TEST;
}

int utcDaliTextEditorSelectionClearedSignal(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorSelectionClearedSignal");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  // connect to the selection changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextEditor::SelectionClearedSignal(editor).Connect(&TestSelectionClearedCallback);
  bool selectionClearedSignal = false;
  editor.ConnectSignal( testTracker, "selectionCleared",   CallbackFunctor(&selectionClearedSignal) );

  editor.SetProperty( TextEditor::Property::TEXT, "Hello\nworld\nHello world" );
  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap on the text editor
  TestGenerateTap( application, 3.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Move to second line of the text & Select some text in the right of the current cursor position
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // remove selection
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_ESCAPE, 0, 0, Integration::KeyEvent::UP, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionClearedCallbackCalled);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap on the text editor
  TestGenerateTap( application, 3.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  gSelectionClearedCallbackCalled = false;

  // Move to second line of the text & select.
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  //remove selection
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionClearedCallbackCalled);

  gSelectionClearedCallbackCalled = false;

  // Render and notify
  application.SendNotification();
  application.Render();

  // Move to second line of the text & select.
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // replace C with selected text
  application.ProcessEvent( GenerateKey( "c", "", "c", KEY_C_CODE, 0, 0, Integration::KeyEvent::DOWN, "c", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionClearedCallbackCalled);

  gSelectionClearedCallbackCalled = false;

  // Render and notify
  application.SendNotification();
  application.Render();

  DevelTextEditor::SelectText( editor ,1, 3 );

  // Render and notify
  application.SendNotification();
  application.Render();

  editor.SetProperty( DevelTextEditor::Property::PRIMARY_CURSOR_POSITION, 3);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionClearedCallbackCalled);

  gSelectionClearedCallbackCalled = false;

  DevelTextEditor::SelectText( editor ,1, 3 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // select none
  DevelTextEditor::SelectNone(editor);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionClearedCallbackCalled);

  END_TEST;
}

int utcDaliTextEditorSelectionWithSecondaryCursor(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorSelectionWithSecondaryCursor");

  // Checks if the actor is created.

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  editor.SetProperty( TextEditor::Property::ENABLE_MARKUP, true );
  editor.SetProperty( TextEditor::Property::TEXT, "اللغة العربية\nمرحبا بالجميع\nالسلام عليكم <span font-size='12' font-family='DejaVu Sans' >Hello world</span>" );
  editor.SetProperty( TextEditor::Property::POINT_SIZE, 12.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  editor.SetProperty( DevelTextEditor::Property::MIN_LINE_SIZE, 50.f );
  editor.SetProperty( DevelTextEditor::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION, false );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap on the text editor
  TestGenerateTap( application, 3.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  //Select the last Arabic word (RTL) & some the space before the English (LTR) letters.
  DevelTextEditor::SelectText( editor, 35, 41 );// This will activate the alternative cursor position and thus 'cursorInfo.isSecondaryCursor' will be true.

  application.SendNotification();
  application.Render();

  std::string selectedText = editor.GetProperty( DevelTextEditor::Property::SELECTED_TEXT ).Get<std::string>();
  DALI_TEST_EQUALS( "عليكم ", selectedText, TEST_LOCATION );

  END_TEST;
}

int utcDaliTextEditorSelectionChangedSignal(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorSelectionChangedSignal");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  // connect to the selection changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextEditor::SelectionChangedSignal(editor).Connect(&TestSelectionChangedCallback);
  bool selectionChangedSignal = false;
  editor.ConnectSignal( testTracker, "selectionChanged",   CallbackFunctor(&selectionChangedSignal) );

  editor.SetProperty( TextEditor::Property::TEXT, "Hello\nworld\nHello world" );
  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10.f );
  editor.SetProperty( Actor::Property::SIZE, Vector2( 100.f, 50.f ) );
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap on the text editor
  TestGenerateTap( application, 3.0f, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Move to second line of the text.
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_DOWN, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Select some text in the right of the current cursor position
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_SHIFT_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldSelectionStart, oldSelectionEnd, TEST_LOCATION);

  gSelectionChangedCallbackCalled = false;

  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_CURSOR_RIGHT, KEY_SHIFT_MODIFIER, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldSelectionStart, 6, TEST_LOCATION);
  DALI_TEST_EQUALS(oldSelectionEnd, 7, TEST_LOCATION);

  gSelectionChangedCallbackCalled = false;

  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_ESCAPE, 0, 0, Integration::KeyEvent::UP, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldSelectionStart, 6, TEST_LOCATION);
  DALI_TEST_EQUALS(oldSelectionEnd, 8, TEST_LOCATION);

  gSelectionChangedCallbackCalled = false;
  editor.SetKeyInputFocus();

  // Render and notify
  application.SendNotification();
  application.Render();

  DevelTextEditor::SelectText( editor ,0, 5 );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldSelectionStart, oldSelectionEnd, TEST_LOCATION);

  gSelectionChangedCallbackCalled = false;

  editor.SetProperty( DevelTextEditor::Property::PRIMARY_CURSOR_POSITION, 3);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldSelectionStart, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(oldSelectionEnd, 5, TEST_LOCATION);

  gSelectionChangedCallbackCalled = false;

  // select all text
  DevelTextEditor::SelectWholeText(editor);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldSelectionStart, oldSelectionEnd, TEST_LOCATION);

  gSelectionChangedCallbackCalled = false;

  // select none
  DevelTextEditor::SelectNone(editor);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gSelectionChangedCallbackCalled);
  DALI_TEST_EQUALS(oldSelectionStart, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(oldSelectionEnd, 23, TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextEditorStrikethroughGeneration(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextEditorStrikethroughGeneration");

  TextEditor textEditor = TextEditor::New();
  textEditor.SetProperty( TextEditor::Property::TEXT, "Test" );
  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 200.0f, 100.f ) );
  textEditor.SetProperty( TextEditor::Property::POINT_SIZE, 10) ;
  textEditor.SetProperty( TextEditor::Property::FONT_FAMILY, "DejaVu Sans");

  application.GetScene().Add( textEditor );
  application.SendNotification();
  application.Render();

  Property::Map strikethroughMapSet;
  Property::Map strikethroughMapGet;

  strikethroughMapSet.Insert( "enable", true );
  strikethroughMapSet.Insert( "color", Color::RED );
  strikethroughMapSet.Insert( "height", 2.0f );

  // Check the strikethrough property
  textEditor.SetProperty( DevelTextEditor::Property::STRIKETHROUGH, strikethroughMapSet );
  strikethroughMapGet = textEditor.GetProperty<Property::Map>( DevelTextEditor::Property::STRIKETHROUGH );
  textEditor.SetProperty( TextEditor::Property::TEXT, "Test1" );
  DALI_TEST_EQUALS( strikethroughMapGet.Count(), strikethroughMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( strikethroughMapGet, strikethroughMapSet ), true, TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  strikethroughMapSet.Clear();
  strikethroughMapGet.Clear();

  END_TEST;
}

int utcDaliTextEditorInsertCharacterAfterInitWithResizePolicyNaturalSize(void)
{

  //This is to test a crash when used Resize Policy equals USE_NATURAL_SIZE
  //DaliException on vector: "Iterator not inside vector"

  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorInsertCharacterAfterInitWithResizePolicyNaturalSize");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  //Set multilines text
  editor.SetProperty(Dali::Toolkit::TextEditor::Property::TEXT, "Hello \n World");
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  //Set ResizePolicy to NaturalSize
  editor.SetProperty(Dali::Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::USE_NATURAL_SIZE);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Create a tap event to touch the text editor.
  TestGenerateTap( application, 5.0f, 5.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Set currsor and add character (in first line)
  editor.SetProperty( DevelTextEditor::Property::PRIMARY_CURSOR_POSITION, 5);
  application.ProcessEvent( GenerateKey( "d", "", "d", KEY_D_CODE, 0, 0, Integration::KeyEvent::DOWN, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "d", "", "d", KEY_D_CODE, 0, 0, Integration::KeyEvent::UP, "d", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  //Check the changed text and cursor position
  DALI_TEST_EQUALS( editor.GetProperty( TextEditor::Property::TEXT ).Get<std::string>(), "Hellod \n World", TEST_LOCATION );
  DALI_TEST_EQUALS( editor.GetProperty( DevelTextEditor::Property::PRIMARY_CURSOR_POSITION ).Get<int>(), 6, TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  END_TEST;
}

int utcDaliTextEditorRemoveCharacterAfterInitWithResizePolicyNaturalSize(void)
{

  //This is to test a crash when used Resize Policy equals USE_NATURAL_SIZE
  //DaliException on vector: "Iterator not inside vector"

  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorRemoveCharacterAfterInitWithResizePolicyNaturalSize");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  //Set multilines text
  editor.SetProperty(Dali::Toolkit::TextEditor::Property::TEXT, "Hello \n World");
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  //Set ResizePolicy to NaturalSize
  editor.SetProperty(Dali::Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::USE_NATURAL_SIZE);

  // Set currsor
  editor.SetProperty( DevelTextEditor::Property::PRIMARY_CURSOR_POSITION, 5);
  application.SendNotification();
  application.Render();

  // Set focus and remove character
  editor.SetKeyInputFocus();
  application.ProcessEvent( GenerateKey( "", "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  //Check the changed text and cursor position
  DALI_TEST_EQUALS( editor.GetProperty( TextEditor::Property::TEXT ).Get<std::string>(), "Hell \n World", TEST_LOCATION );
  DALI_TEST_EQUALS( editor.GetProperty( DevelTextEditor::Property::PRIMARY_CURSOR_POSITION ).Get<int>(), 4, TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  END_TEST;
}

int utcDaliTextEditorCutSelectedTextAfterInitWithResizePolicyNaturalSize(void)
{

  //This is to test a crash when used Resize Policy equals USE_NATURAL_SIZE
  //DaliException on vector: "Iterator not inside vector"

  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorCutSelectedTextAfterInitWithResizePolicyNaturalSize");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  //Set multilines text
  editor.SetProperty(Dali::Toolkit::TextEditor::Property::TEXT, "Hello \n World");
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  //Set ResizePolicy to NaturalSize
  editor.SetProperty(Dali::Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::USE_NATURAL_SIZE);

  //Select text at initialization (before the first render)
  DevelTextEditor::SelectText( editor ,3, 5 );

  // Render and notify
  application.SendNotification();
  application.Render();

  //Cut text
  application.ProcessEvent( GenerateKey( "", "", "", Dali::DevelKey::DALI_KEY_CONTROL_LEFT, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );
  application.ProcessEvent( GenerateKey( "x", "x", "x", KEY_X_CODE, KEY_CONTROL_MODIFIER, 0, Integration::KeyEvent::DOWN, "x", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  //Check the changed text and cursor position
  DALI_TEST_EQUALS( editor.GetProperty( TextEditor::Property::TEXT ).Get<std::string>(), "Hel \n World", TEST_LOCATION );
  DALI_TEST_EQUALS( editor.GetProperty( DevelTextEditor::Property::PRIMARY_CURSOR_POSITION ).Get<int>(), 3, TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  END_TEST;
}


int utcDaliTextEditorDoubleEnterAfterInitWithResizePolicyNaturalSize(void)
{

  //This is to test a crash when used Resize Policy equals USE_NATURAL_SIZE
  //DaliException on vector: "Iterator not inside vector"

  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorDoubleEnterAfterInitWithResizePolicyNaturalSize");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  application.GetScene().Add( editor );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  //Set multilines text
  editor.SetProperty(Dali::Toolkit::TextEditor::Property::TEXT, "Hello \n World");
  editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  //Set ResizePolicy to NaturalSize
  editor.SetProperty(Dali::Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::USE_NATURAL_SIZE);

  // Set currsor
  editor.SetProperty( DevelTextEditor::Property::PRIMARY_CURSOR_POSITION, 5);
  application.SendNotification();
  application.Render();

  // Set focus and double enter (new line)
  editor.SetKeyInputFocus();
  application.ProcessEvent(GenerateKey("Enter", "", "\n", 13, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));
  application.ProcessEvent(GenerateKey("Enter", "", "\n", 13, 0, 0, Integration::KeyEvent::DOWN, "", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE));

  // Render and notify
  application.SendNotification();
  application.Render();

  //Check the changed text and cursor position
  DALI_TEST_EQUALS( editor.GetProperty( TextEditor::Property::TEXT ).Get<std::string>(), "Hello\n\n \n World", TEST_LOCATION );
  DALI_TEST_EQUALS( editor.GetProperty( DevelTextEditor::Property::PRIMARY_CURSOR_POSITION ).Get<int>(), 7, TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  END_TEST;
}
