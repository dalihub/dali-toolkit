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
 *
 */

#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include <dali/public-api/rendering/renderer.h>
#include <dali/devel-api/adaptor-framework/clipboard.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/tap-gesture-event.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

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

const int DEFAULT_RENDERING_BACKEND = Dali::Toolkit::Text::DEFAULT_RENDERING_BACKEND;

const Dali::Vector4 LIGHT_BLUE( 0.75f, 0.96f, 1.f, 1.f ); // The text highlight color.

const unsigned int CURSOR_BLINK_INTERVAL = 500u; // Cursor blink interval
const float TO_MILLISECONDS = 1000.f;
const float TO_SECONDS = 1.f / TO_MILLISECONDS;

const float SCROLL_THRESHOLD = 10.f;
const float SCROLL_SPEED = 300.f;

const unsigned int DEFAULT_FONT_SIZE = 1152u;
const std::string DEFAULT_FONT_DIR( "/resources/fonts" );

const int KEY_A_CODE = 38;
const int KEY_D_CODE = 40;
const int KEY_WHITE_SPACE_CODE = 65;

static bool gTextChangedCallBackCalled;
static bool gInputStyleChangedCallbackCalled;
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

// Generate a TapGestureEvent to send to Core.
Integration::TapGestureEvent GenerateTap(
    Gesture::State state,
    unsigned int numberOfTaps,
    unsigned int numberOfTouches,
    Vector2 point)
{
  Integration::TapGestureEvent tap( state );

  tap.numberOfTaps = numberOfTaps;
  tap.numberOfTouches = numberOfTouches;
  tap.point = point;

  return tap;
}

// Generate a KeyEvent to send to Core.
Integration::KeyEvent GenerateKey( const std::string& keyName,
                                   const std::string& keyString,
                                   int keyCode,
                                   int keyModifier,
                                   unsigned long timeStamp,
                                   const Integration::KeyEvent::State& keyState )
{
  return Integration::KeyEvent( keyName,
                                keyString,
                                keyCode,
                                keyModifier,
                                timeStamp,
                                keyState );
}

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
  DALI_TEST_CHECK( editor.GetPropertyIndex( PROPERTY_NAME_RENDERING_BACKEND ) == TextEditor::Property::RENDERING_BACKEND );
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
  Stage::GetCurrent().Add( editor );

  // Note - we can't check the defaults since the stylesheets are platform-specific

  // Check the render backend property.
  editor.SetProperty( TextEditor::Property::RENDERING_BACKEND, Text::RENDERING_SHARED_ATLAS );
  DALI_TEST_EQUALS( (Text::RenderingType)editor.GetProperty<int>( TextEditor::Property::RENDERING_BACKEND ), Text::RENDERING_SHARED_ATLAS, TEST_LOCATION );

  // Check text property.
  editor.SetProperty( TextEditor::Property::TEXT, "Setting Text" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::TEXT ), std::string("Setting Text"), TEST_LOCATION );

  // Check text's color property
  editor.SetProperty( TextEditor::Property::TEXT_COLOR, Color::WHITE );
  DALI_TEST_EQUALS( editor.GetProperty<Vector4>( TextEditor::Property::TEXT_COLOR ), Color::WHITE, TEST_LOCATION );

  // Check font properties.
  editor.SetProperty( TextEditor::Property::FONT_FAMILY, "Setting font family" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::FONT_FAMILY ), std::string("Setting font family"), TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::FONT_STYLE, "{\"weight\":\"bold\",\"width\":\"condensed\",\"slant\":\"italic\"}" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::FONT_STYLE ), std::string("{\"weight\":\"bold\",\"width\":\"condensed\",\"slant\":\"italic\"}"), TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10.f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::POINT_SIZE ), 10.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Reset font style.
  editor.SetProperty( TextEditor::Property::FONT_STYLE, "{\"weight\":\"normal\",\"slant\":\"oblique\"}" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::FONT_STYLE ), std::string("{\"weight\":\"normal\",\"slant\":\"oblique\"}"), TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::FONT_STYLE, "{\"slant\":\"roman\"}" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::FONT_STYLE ), std::string("{\"slant\":\"normal\"}"), TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::FONT_STYLE, "" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::FONT_STYLE ), std::string(""), TEST_LOCATION );

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
  editor.SetProperty( TextEditor::Property::INPUT_FONT_STYLE, "{\"weight\":\"bold\",\"width\":\"condensed\",\"slant\":\"italic\"}" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::INPUT_FONT_STYLE ), "{\"weight\":\"bold\",\"width\":\"condensed\",\"slant\":\"italic\"}", TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::INPUT_POINT_SIZE, 12.f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::INPUT_POINT_SIZE ), 12.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Reset input font style.
  editor.SetProperty( TextEditor::Property::INPUT_FONT_STYLE, "{\"weight\":\"normal\",\"slant\":\"oblique\"}" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::INPUT_FONT_STYLE ), std::string("{\"weight\":\"normal\",\"slant\":\"oblique\"}"), TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::INPUT_FONT_STYLE, "{\"slant\":\"roman\"}" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::INPUT_FONT_STYLE ), std::string("{\"slant\":\"normal\"}"), TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::INPUT_FONT_STYLE, "" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::INPUT_FONT_STYLE ), std::string(""), TEST_LOCATION );

  // Check the line spacing property
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::LINE_SPACING ), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::LINE_SPACING, 10.f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::LINE_SPACING ), 10.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Check the input line spacing property
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::INPUT_LINE_SPACING ), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  editor.SetProperty( TextEditor::Property::INPUT_LINE_SPACING, 20.f );
  DALI_TEST_EQUALS( editor.GetProperty<float>( TextEditor::Property::INPUT_LINE_SPACING ), 20.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Check the underline property
  editor.SetProperty( TextEditor::Property::UNDERLINE, "Underline properties" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::UNDERLINE ), std::string("Underline properties"), TEST_LOCATION );

  // Check the input underline property
  editor.SetProperty( TextEditor::Property::INPUT_UNDERLINE, "Underline input properties" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::INPUT_UNDERLINE ), std::string("Underline input properties"), TEST_LOCATION );

  // Check the shadow property
  editor.SetProperty( TextEditor::Property::SHADOW, "Shadow properties" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::SHADOW ), std::string("Shadow properties"), TEST_LOCATION );

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
  editor.SetProperty( TextEditor::Property::OUTLINE, "Outline properties" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::OUTLINE ), std::string("Outline properties"), TEST_LOCATION );

  // Check the input outline property
  editor.SetProperty( TextEditor::Property::INPUT_OUTLINE, "Outline input properties" );
  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::INPUT_OUTLINE ), std::string("Outline input properties"), TEST_LOCATION );

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

  Stage::GetCurrent().Add( editor );

  try
  {
    // Render some text with the shared atlas backend
    editor.SetProperty( TextEditor::Property::RENDERING_BACKEND, Text::RENDERING_SHARED_ATLAS );
    application.SendNotification();
    application.Render();
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }
  END_TEST;
}

// Positive test for the textChanged signal.
int utcDaliTextEditorTextChangedP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorTextChangedP");
  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK( editor );

  Stage::GetCurrent().Add( editor );

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
  application.ProcessEvent( GenerateKey( "D", "D", KEY_D_CODE, 0, 0, Integration::KeyEvent::Down ) );
  DALI_TEST_CHECK( gTextChangedCallBackCalled );

  END_TEST;
}

int utcDaliTextEditorInputStyleChanged01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorInputStyleChanged01");

  // The text-editor emits signals when the input style changes. These changes of style are
  // detected during the relayout process (size negotiation), i.e after the cursor has been moved. Signals
  // can't be emitted during the size negotiation as the callbacks may update the UI.
  // The text-editor adds an idle callback to the adaptor to emit the signals after the size negotiation.
  // This creates an implementation of the adaptor stub and a queue of idle callbacks.
  application.CreateAdaptor();

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


  editor.SetSize( 300.f, 50.f );
  editor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  editor.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  editor.SetProperty( TextEditor::Property::ENABLE_MARKUP, true );
  editor.SetProperty( TextEditor::Property::TEXT, "<font family='DejaVuSerif' size='18'>He<color value='green'>llo</color> <font weight='bold'>world</font> demo</font>" );

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  editor.InputStyleChangedSignal().Connect( &TestInputStyleChangedCallback );
  bool inputStyleChangedSignal = false;
  editor.ConnectSignal( testTracker, "inputStyleChanged",   CallbackFunctor(&inputStyleChangedSignal) );

  Stage::GetCurrent().Add( editor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text editor.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 18.f, 25.f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 18.f, 25.f ) ) );

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
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 30.f, 25.f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 30.f, 25.f ) ) );

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
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 43.f, 25.f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 43.f, 25.f ) ) );

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
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 88.f, 25.f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 88.f, 25.f ) ) );

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

    const std::string style = editor.GetProperty( TextEditor::Property::INPUT_FONT_STYLE ).Get<std::string>();
    DALI_TEST_EQUALS( style, "{\"weight\":\"bold\"}", TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text editor.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 115.f, 25.f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 115.f, 25.f ) ) );

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
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 164.f, 25.f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 164.f, 25.f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( gInputStyleChangedCallbackCalled );
  if( gInputStyleChangedCallbackCalled )
  {
    DALI_TEST_EQUALS( static_cast<unsigned int>( gInputStyleMask ), static_cast<unsigned int>( TextEditor::InputStyle::FONT_STYLE ), TEST_LOCATION );

    const std::string style = editor.GetProperty( TextEditor::Property::INPUT_FONT_STYLE ).Get<std::string>();
    DALI_TEST_CHECK( style.empty() );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text editor.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 191.f, 25.f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 191.f, 25.f ) ) );

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
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextEditorInputStyleChanged02");

  // The text-editor emits signals when the input style changes. These changes of style are
  // detected during the relayout process (size negotiation), i.e after the cursor has been moved. Signals
  // can't be emitted during the size negotiation as the callbacks may update the UI.
  // The text-editor adds an idle callback to the adaptor to emit the signals after the size negotiation.
  // This creates an implementation of the adaptor stub and a queue of idle callbacks.
  application.CreateAdaptor();

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


  editor.SetSize( 300.f, 50.f );
  editor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  editor.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  editor.SetProperty( TextEditor::Property::ENABLE_MARKUP, true );
  editor.SetProperty( TextEditor::Property::TEXT, "<font family='DejaVuSerif' size='18'>He<color value='blue'> l</color><color value='green'>lo</color> <font weight='bold'>world</font> demo</font>" );

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  editor.InputStyleChangedSignal().Connect( &TestInputStyleChangedCallback );
  bool inputStyleChangedSignal = false;
  editor.ConnectSignal( testTracker, "inputStyleChanged",   CallbackFunctor(&inputStyleChangedSignal) );

  Stage::GetCurrent().Add( editor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextEditor::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text editor.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 53.f, 25.f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 53.f, 25.f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Possible, 2u, 1u, Vector2( 53.f, 25.f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 2u, 1u, Vector2( 53.f, 25.f ) ) );

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

  application.ProcessEvent( GenerateKey( "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::Down ) );

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

  application.ProcessEvent( GenerateKey( "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::Down ) );

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

  application.ProcessEvent( GenerateKey( "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::Down ) );

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

  editor.SetProperty( TextEditor::Property::INPUT_FONT_STYLE, "{\"weight\":\"thin\",\"width\":\"condensed\",\"slant\":\"italic\"}" );
  editor.SetProperty( TextEditor::Property::INPUT_POINT_SIZE, 20.f );
  editor.SetProperty( TextEditor::Property::INPUT_LINE_SPACING, 5.f );

  editor.SetProperty( TextEditor::Property::INPUT_UNDERLINE, "underline" );
  editor.SetProperty( TextEditor::Property::INPUT_SHADOW, "shadow" );
  editor.SetProperty( TextEditor::Property::INPUT_EMBOSS, "emboss" );
  editor.SetProperty( TextEditor::Property::INPUT_OUTLINE, "outline" );

  application.ProcessEvent( GenerateKey( "a", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::Down ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( !gInputStyleChangedCallbackCalled );
  DALI_TEST_CHECK( !inputStyleChangedSignal );

  // Create a tap event to touch the text editor.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 63.f, 25.f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 63.f, 25.f ) ) );

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
  editor.SetProperty( TextEditor::Property::FONT_STYLE, "{\"weight\":\"black\",\"width\":\"expanded\",\"slant\":\"oblique\"}" );

  // Create a tap event to touch the text editor.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 30.f, 25.f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 30.f, 25.f ) ) );

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

  Stage::GetCurrent().Add( editor );

  editor.SetSize( 300.f, 50.f );
  editor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  editor.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Add a key event but as the text editor has not the focus it should do nothing.
  application.ProcessEvent( GenerateKey( "a", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::Down ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::TEXT ), std::string(""), TEST_LOCATION );

  // Create a tap event to touch the text editor.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Now the text editor has the focus, so it can handle the key events.
  application.ProcessEvent( GenerateKey( "a", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::Down ) );
  application.ProcessEvent( GenerateKey( "a", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::Down ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( editor.GetProperty<std::string>( TextEditor::Property::TEXT ), std::string("aa"), TEST_LOCATION );

  // Create a second text editor and send key events to it.
  TextEditor editor2 = TextEditor::New();

  editor2.SetParentOrigin( ParentOrigin::TOP_LEFT );
  editor2.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  editor2.SetSize( 100.f, 100.f );
  editor2.SetPosition( 100.f, 100.f );

  Stage::GetCurrent().Add( editor2 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Create a tap event on the second text editor.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 150.0f, 125.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 150.0f, 125.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // The second text editor has the focus. It should handle the key events.
  application.ProcessEvent( GenerateKey( "a", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::Down ) );
  application.ProcessEvent( GenerateKey( "a", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::Down ) );

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

  Stage::GetCurrent().Add( editor );

  editor.SetSize( 300.f, 50.f );
  editor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  editor.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check there are the expected number of children (the stencil).
  DALI_TEST_EQUALS( editor.GetChildCount(), 1u, TEST_LOCATION );

  Actor stencil = editor.GetChildAt( 0u );

  // Create a tap event to touch the text editor.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  Actor layer = editor.GetChildAt( 1u );
  DALI_TEST_CHECK( layer.IsLayer() );

  DALI_TEST_EQUALS( layer.GetChildCount(), 1u, TEST_LOCATION ); // The cursor.
  DALI_TEST_EQUALS( stencil.GetChildCount(), 0u, TEST_LOCATION );

  // Now the text editor has the focus, so it can handle the key events.
  application.ProcessEvent( GenerateKey( "a", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::Down ) );
  application.ProcessEvent( GenerateKey( "a", "a", KEY_A_CODE, 0, 0, Integration::KeyEvent::Down ) );

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
  Vector3 position1 = cursor.GetCurrentPosition();

  application.ProcessEvent( GenerateKey( "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Integration::KeyEvent::Down ) );
  application.ProcessEvent( GenerateKey( "", "", DALI_KEY_CURSOR_LEFT, 0, 0, Integration::KeyEvent::Down ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector3 position2 = cursor.GetCurrentPosition();

  DALI_TEST_CHECK( position2.x < position1.x );

  application.ProcessEvent( GenerateKey( "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::Down ) );
  application.ProcessEvent( GenerateKey( "", "", DALI_KEY_CURSOR_RIGHT, 0, 0, Integration::KeyEvent::Down ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector3 position3 = cursor.GetCurrentPosition();

  DALI_TEST_EQUALS( position1, position3, TEST_LOCATION ); // Should be in the same position1.

  // Send some taps and check the cursor positions.

  // Try to tap at the beginning.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 1.f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 1.f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Cursor position should be the same than position1.
  Vector3 position4 = cursor.GetCurrentPosition();

  DALI_TEST_EQUALS( position2, position4, TEST_LOCATION ); // Should be in the same position2.

  // Tap away from the start position.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 16.f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 16.0f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector3 position5 = cursor.GetCurrentPosition();

  DALI_TEST_CHECK( position5.x > position4.x );

  // Remove all the text.
  application.ProcessEvent( GenerateKey( "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::Down ) );
  application.ProcessEvent( GenerateKey( "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::Down ) );
  editor.SetProperty( TextEditor::Property::TEXT, "" );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Cursor position should be the same than position2.
  Vector3 position6 = cursor.GetCurrentPosition();

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

  Stage::GetCurrent().Add( editor );

  editor.SetProperty( TextEditor::Property::TEXT, "This is a long text for the size of the text-editor." );
  editor.SetProperty( TextEditor::Property::POINT_SIZE, 10.f );
  editor.SetSize( 30.f, 50.f );
  editor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  editor.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Send some taps and check text controller with clipboard window
  Dali::Clipboard clipboard = Clipboard::Get();
  clipboard.ShowClipboard();
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 3.f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 3.f, 25.0f ) ) );
  clipboard.HideClipboard();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap first to get the focus.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 3.f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 3.f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Double tap to select a word.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 2u, 1u, Vector2( 3.f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 2u, 1u, Vector2( 3.f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // The stencil actor should have two actors: the renderer and the highlight actor.
  Actor stencil = editor.GetChildAt( 0u );

  // The stencil actor has a container with all the actors which contain the text renderers.
  Actor container = stencil.GetChildAt( 0u );
  for( unsigned int index = 0; index < container.GetChildCount(); ++index )
  {
    Renderer renderer = container.GetChildAt( index ).GetRendererAt( 0u );
    DALI_TEST_CHECK( renderer );
  }

  Renderer highlight = stencil.GetChildAt( 1u ).GetRendererAt( 0u );
  DALI_TEST_CHECK( highlight );

  END_TEST;
}
