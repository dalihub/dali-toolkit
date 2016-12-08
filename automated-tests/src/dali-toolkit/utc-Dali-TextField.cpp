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
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/tap-gesture-event.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/pan-gesture-event.h>
#include <dali/integration-api/events/long-press-gesture-event.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
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
const char* const PROPERTY_NAME_SHADOW_OFFSET                        = "shadowOffset";
const char* const PROPERTY_NAME_SHADOW_COLOR                         = "shadowColor";
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

const char* const PROPERTY_NAME_UNDERLINE                            = "underline";
const char* const PROPERTY_NAME_INPUT_UNDERLINE                      = "inputUnderline";
const char* const PROPERTY_NAME_SHADOW                               = "shadow";
const char* const PROPERTY_NAME_INPUT_SHADOW                         = "inputShadow";
const char* const PROPERTY_NAME_EMBOSS                               = "emboss";
const char* const PROPERTY_NAME_INPUT_EMBOSS                         = "inputEmboss";
const char* const PROPERTY_NAME_OUTLINE                              = "outline";
const char* const PROPERTY_NAME_INPUT_OUTLINE                        = "inputOutline";

const int DEFAULT_RENDERING_BACKEND = Dali::Toolkit::Text::DEFAULT_RENDERING_BACKEND;

const Vector4 PLACEHOLDER_TEXT_COLOR( 0.8f, 0.8f, 0.8f, 0.8f );
const Dali::Vector4 LIGHT_BLUE( 0.75f, 0.96f, 1.f, 1.f ); // The text highlight color.

const unsigned int CURSOR_BLINK_INTERVAL = 500u; // Cursor blink interval
const float RENDER_FRAME_INTERVAL = 16.66f;

const float TO_MILLISECONDS = 1000.f;
const float TO_SECONDS = 1.f / TO_MILLISECONDS;

const float SCROLL_THRESHOLD = 10.f;
const float SCROLL_SPEED = 300.f;

const unsigned int DEFAULT_FONT_SIZE = 1152u;
const std::string DEFAULT_FONT_DIR( "/resources/fonts" );

static bool gTextChangedCallBackCalled;
static bool gMaxCharactersCallBackCalled;
static bool gInputStyleChangedCallbackCalled;
static Dali::Toolkit::TextField::InputStyle::Mask gInputStyleMask;

static void LoadBitmapResource(TestPlatformAbstraction& platform, int width, int height)
{
  Integration::ResourceRequest* request = platform.GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD );
  Integration::ResourcePointer resource(bitmap);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, width, height, width, height);

  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
  }
}

static void LoadMarkerImages(ToolkitTestApplication& app, TextField textField)
{
  int width(40);
  int height(40);
  LoadBitmapResource( app.GetPlatform(), width, height );

  Property::Map propertyMap;
  propertyMap["filename"] = "image.png";
  propertyMap["width"] = width;
  propertyMap["height"] = height;
  textField.SetProperty( Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_LEFT, propertyMap );
  textField.SetProperty( Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT, propertyMap );
  textField.SetProperty( Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT, propertyMap );
  textField.SetProperty( Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT, propertyMap );
  textField.SetProperty( Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT, propertyMap );
  textField.SetProperty( Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT, propertyMap );
  textField.SetProperty( Toolkit::TextField::Property::GRAB_HANDLE_IMAGE, propertyMap );
  textField.SetProperty( Toolkit::TextField::Property::GRAB_HANDLE_PRESSED_IMAGE, propertyMap );
}

// Generate a PanGestureEvent to send to Core
static Integration::PanGestureEvent GeneratePan(
    Gesture::State state,
    const Vector2& previousPosition,
    const Vector2& currentPosition,
    unsigned long timeDelta,
    unsigned int numberOfTouches = 1)
{
  Integration::PanGestureEvent pan(state);

  pan.previousPosition = previousPosition;
  pan.currentPosition = currentPosition;
  pan.timeDelta = timeDelta;
  pan.numberOfTouches = numberOfTouches;

  return pan;
}

/**
 * Helper to generate PanGestureEvent
 *
 * @param[in] application Application instance
 * @param[in] state The Gesture State
 * @param[in] pos The current position of touch.
 */
static void SendPan(ToolkitTestApplication& application, Gesture::State state, const Vector2& pos)
{
  static Vector2 last;

  if( (state == Gesture::Started) ||
      (state == Gesture::Possible) )
  {
    last.x = pos.x;
    last.y = pos.y;
  }

  application.ProcessEvent(GeneratePan(state, last, pos, 16));

  last.x = pos.x;
  last.y = pos.y;
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

  for(int i = 0; i <= ( duration / RENDER_FRAME_INTERVAL); i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
    time += RENDER_FRAME_INTERVAL;
  }

  return time;
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

static void TestTextChangedCallback( TextField control )
{
  tet_infoline(" TestTextChangedCallback");

  gTextChangedCallBackCalled = true;
}

static void TestMaxLengthReachedCallback( TextField control )
{
  tet_infoline(" TestMaxLengthReachedCallback");

  gMaxCharactersCallBackCalled = true;
}

static void TestInputStyleChangedCallback( TextField control, TextField::InputStyle::Mask mask )
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

Integration::LongPressGestureEvent GenerateLongPress(
    Gesture::State state,
    unsigned int numberOfTouches,
    Vector2 point)
{
  Integration::LongPressGestureEvent longPress( state );

  longPress.numberOfTouches = numberOfTouches;
  longPress.point = point;
  return longPress;
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

bool DaliTestCheckMaps( const Property::Map& fontStyleMapGet, const Property::Map& fontStyleMapSet )
{
  if( fontStyleMapGet.Count() == fontStyleMapSet.Count() )
  {
    for( unsigned int index = 0u; index < fontStyleMapGet.Count(); ++index )
    {
      const KeyValuePair& valueGet = fontStyleMapGet.GetKeyValue( index );

      Property::Value* valueSet = fontStyleMapSet.Find( valueGet.first.stringKey );
      if( NULL != valueSet )
      {
        if( valueGet.second.Get<std::string>() != valueSet->Get<std::string>() )
        {
          tet_printf( "  Value got : [%s], expected : [%s]", valueGet.second.Get<std::string>().c_str(), valueSet->Get<std::string>().c_str() );
          return false;
        }
      }
      else
      {
        tet_printf( "  The key %s doesn't exist.", valueGet.first.stringKey.c_str() );
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
  DALI_TEST_CHECK( !textField );
  END_TEST;
}

int UtcDaliToolkitTextFieldNewP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldNewP");
  TextField textField = TextField::New();
  DALI_TEST_CHECK( textField );
  END_TEST;
}

int UtcDaliToolkitTextFieldDownCastP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldDownCastP");
  TextField textField1 = TextField::New();
  BaseHandle object( textField1 );

  TextField textField2 = TextField::DownCast( object );
  DALI_TEST_CHECK( textField2 );

  TextField textField3 = DownCast< TextField >( object );
  DALI_TEST_CHECK( textField3 );
  END_TEST;
}

int UtcDaliToolkitTextFieldDownCastN(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldDownCastN");
  BaseHandle uninitializedObject;
  TextField textField1 = TextField::DownCast( uninitializedObject );
  DALI_TEST_CHECK( !textField1 );

  TextField textField2 = DownCast< TextField >( uninitializedObject );
  DALI_TEST_CHECK( !textField2 );
  END_TEST;
}

int UtcDaliToolkitTextFieldCopyConstructorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldCopyConstructorP");
  TextField textField = TextField::New();
  textField.SetProperty( TextField::Property::TEXT, "Test" );

  TextField copy( textField );
  DALI_TEST_CHECK( copy );
  DALI_TEST_CHECK( copy.GetProperty<std::string>( TextLabel::Property::TEXT ) == textField.GetProperty<std::string>( TextLabel::Property::TEXT ) );
  END_TEST;
}

int UtcDaliToolkitTextFieldAssignmentOperatorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldAssignmentOperatorP");
  TextField textField = TextField::New();
  textField.SetProperty( TextField::Property::TEXT, "Test" );

  TextField copy = textField;
  DALI_TEST_CHECK( copy );
  DALI_TEST_CHECK( copy.GetProperty<std::string>( TextField::Property::TEXT ) == textField.GetProperty<std::string>( TextField::Property::TEXT ) );
  END_TEST;
}

int UtcDaliTextFieldNewP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldNewP");
  TextField textField = TextField::New();
  DALI_TEST_CHECK( textField );
  END_TEST;
}

// Positive test case for a method
int UtcDaliTextFieldGetPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldGetPropertyP");
  TextField field = TextField::New();
  DALI_TEST_CHECK( field );

  // Check Property Indices are correct
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_RENDERING_BACKEND ) == TextField::Property::RENDERING_BACKEND );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_TEXT ) == TextField::Property::TEXT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_PLACEHOLDER_TEXT ) == TextField::Property::PLACEHOLDER_TEXT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_PLACEHOLDER_TEXT_FOCUSED ) == TextField::Property::PLACEHOLDER_TEXT_FOCUSED );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_FONT_FAMILY ) == TextField::Property::FONT_FAMILY );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_FONT_STYLE ) == TextField::Property::FONT_STYLE );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_POINT_SIZE ) == TextField::Property::POINT_SIZE );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_MAX_LENGTH ) == TextField::Property::MAX_LENGTH );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_EXCEED_POLICY ) == TextField::Property::EXCEED_POLICY );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_HORIZONTAL_ALIGNMENT ) == TextField::Property::HORIZONTAL_ALIGNMENT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_VERTICAL_ALIGNMENT ) == TextField::Property::VERTICAL_ALIGNMENT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_TEXT_COLOR ) == TextField::Property::TEXT_COLOR );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_PLACEHOLDER_TEXT_COLOR ) == TextField::Property::PLACEHOLDER_TEXT_COLOR );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SHADOW_OFFSET ) == TextField::Property::SHADOW_OFFSET );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SHADOW_COLOR ) == TextField::Property::SHADOW_COLOR );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_PRIMARY_CURSOR_COLOR ) == TextField::Property::PRIMARY_CURSOR_COLOR );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SECONDARY_CURSOR_COLOR ) == TextField::Property::SECONDARY_CURSOR_COLOR );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_ENABLE_CURSOR_BLINK ) == TextField::Property::ENABLE_CURSOR_BLINK );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_CURSOR_BLINK_INTERVAL ) == TextField::Property::CURSOR_BLINK_INTERVAL );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_CURSOR_BLINK_DURATION ) == TextField::Property::CURSOR_BLINK_DURATION );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_CURSOR_WIDTH ) == TextField::Property::CURSOR_WIDTH );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_GRAB_HANDLE_IMAGE ) == TextField::Property::GRAB_HANDLE_IMAGE );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_GRAB_HANDLE_PRESSED_IMAGE ) == TextField::Property::GRAB_HANDLE_PRESSED_IMAGE );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SCROLL_THRESHOLD ) == TextField::Property::SCROLL_THRESHOLD );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SCROLL_SPEED ) == TextField::Property::SCROLL_SPEED );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SELECTION_HANDLE_IMAGE_LEFT ) == TextField::Property::SELECTION_HANDLE_IMAGE_LEFT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SELECTION_HANDLE_IMAGE_RIGHT ) == TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_LEFT ) == TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_RIGHT ) == TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SELECTION_HANDLE_MARKER_IMAGE_LEFT ) == TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SELECTION_HANDLE_MARKER_IMAGE_RIGHT ) == TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SELECTION_HIGHLIGHT_COLOR ) == TextField::Property::SELECTION_HIGHLIGHT_COLOR );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_DECORATION_BOUNDING_BOX ) == TextField::Property::DECORATION_BOUNDING_BOX );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_INPUT_METHOD_SETTINGS ) == TextField::Property::INPUT_METHOD_SETTINGS );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_INPUT_COLOR ) == TextField::Property::INPUT_COLOR );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_ENABLE_MARKUP ) == TextField::Property::ENABLE_MARKUP );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_INPUT_FONT_FAMILY ) == TextField::Property::INPUT_FONT_FAMILY );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_INPUT_FONT_STYLE ) == TextField::Property::INPUT_FONT_STYLE );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_INPUT_POINT_SIZE ) == TextField::Property::INPUT_POINT_SIZE );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_UNDERLINE ) == TextField::Property::UNDERLINE );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_INPUT_UNDERLINE ) == TextField::Property::INPUT_UNDERLINE );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SHADOW ) == TextField::Property::SHADOW );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_INPUT_SHADOW ) == TextField::Property::INPUT_SHADOW );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_EMBOSS ) == TextField::Property::EMBOSS );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_INPUT_EMBOSS ) == TextField::Property::INPUT_EMBOSS );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_OUTLINE ) == TextField::Property::OUTLINE );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_INPUT_OUTLINE ) == TextField::Property::INPUT_OUTLINE );

  END_TEST;
}

bool SetPropertyMapRetrieved( TextField& field, const Property::Index property, const std::string mapKey, const std::string mapValue )
{
  bool result = false;
  Property::Map imageMap;
  imageMap[mapKey] =mapValue;

  field.SetProperty( property , imageMap );
  Property::Value propValue = field.GetProperty( property );
  Property::Map* resultMap = propValue.GetMap();

  if ( resultMap->Find( mapKey )->Get< std::string>() == mapValue )
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
  DALI_TEST_CHECK( field );
  Stage::GetCurrent().Add( field );

  // Note - we can't check the defaults since the stylesheets are platform-specific

  // Check the render backend property.
  field.SetProperty( TextField::Property::RENDERING_BACKEND, Text::RENDERING_SHARED_ATLAS );
  DALI_TEST_EQUALS( (Text::RenderingType)field.GetProperty<int>( TextField::Property::RENDERING_BACKEND ), Text::RENDERING_SHARED_ATLAS, TEST_LOCATION );

  // Check text property.
  field.SetProperty( TextField::Property::TEXT, "Setting Text" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::TEXT ), std::string("Setting Text"), TEST_LOCATION );

  // Check placeholder text properties.
  field.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "Setting Placeholder Text" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::PLACEHOLDER_TEXT ), std::string("Setting Placeholder Text"), TEST_LOCATION );

  field.SetProperty( TextField::Property::PLACEHOLDER_TEXT_FOCUSED, "Setting Placeholder Text Focused" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::PLACEHOLDER_TEXT_FOCUSED ), std::string("Setting Placeholder Text Focused"), TEST_LOCATION );

  // Check font properties.
  field.SetProperty( TextField::Property::FONT_FAMILY, "Setting font family" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::FONT_FAMILY ), std::string("Setting font family"), TEST_LOCATION );

  Property::Map fontStyleMapSet;
  Property::Map fontStyleMapGet;
  Property::Value* slantValue = NULL;

  fontStyleMapSet.Insert( "weight", "bold" );
  fontStyleMapSet.Insert( "width", "condensed" );
  fontStyleMapSet.Insert( "slant", "italic" );
  field.SetProperty( TextField::Property::FONT_STYLE, fontStyleMapSet );

  fontStyleMapGet = field.GetProperty<Property::Map>( TextField::Property::FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  field.SetProperty( TextField::Property::POINT_SIZE, 10.f );
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::POINT_SIZE ), 10.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Reset font style.
  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "weight", "normal" );
  fontStyleMapSet.Insert( "slant", "oblique" );
  field.SetProperty( TextField::Property::FONT_STYLE, fontStyleMapSet );

  fontStyleMapGet = field.GetProperty<Property::Map>( TextField::Property::FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "slant", "roman" );
  field.SetProperty( TextField::Property::FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = field.GetProperty<Property::Map>( TextField::Property::FONT_STYLE );

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

  field.SetProperty( TextField::Property::FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = field.GetProperty<Property::Map>( TextField::Property::FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  // Check that the MAX_LENGTH property can be correctly set
  const int maxNumberOfCharacters = 20;
  field.SetProperty( TextField::Property::MAX_LENGTH, maxNumberOfCharacters );
  DALI_TEST_EQUALS( field.GetProperty<int>( TextField::Property::MAX_LENGTH ), maxNumberOfCharacters, TEST_LOCATION );

  // Check exceed policy
  field.SetProperty( TextField::Property::EXCEED_POLICY, Dali::Toolkit::TextField::EXCEED_POLICY_CLIP );
  DALI_TEST_EQUALS( field.GetProperty<int>( TextField::Property::EXCEED_POLICY ), static_cast<int>( Dali::Toolkit::TextField::EXCEED_POLICY_CLIP ), TEST_LOCATION );
  field.SetProperty( TextField::Property::EXCEED_POLICY, Dali::Toolkit::TextField::EXCEED_POLICY_ORIGINAL );
  DALI_TEST_EQUALS( field.GetProperty<int>( TextField::Property::EXCEED_POLICY ), static_cast<int>( Dali::Toolkit::TextField::EXCEED_POLICY_ORIGINAL ), TEST_LOCATION );

  // Check that the Alignment properties can be correctly set
  field.SetProperty( TextField::Property::HORIZONTAL_ALIGNMENT, "END" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::HORIZONTAL_ALIGNMENT ), "END", TEST_LOCATION );
  field.SetProperty( TextField::Property::VERTICAL_ALIGNMENT, "CENTER" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::VERTICAL_ALIGNMENT ), "CENTER", TEST_LOCATION );

  // Check text's color property
  field.SetProperty( TextField::Property::TEXT_COLOR, Color::WHITE );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::TEXT_COLOR ), Color::WHITE, TEST_LOCATION );

  // Check placeholder text's color property.
  field.SetProperty( TextField::Property::PLACEHOLDER_TEXT_COLOR, Color::RED );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::PLACEHOLDER_TEXT_COLOR ), Color::RED, TEST_LOCATION );

  // Check shadow properties.
  field.SetProperty( TextField::Property::SHADOW_OFFSET, Vector2( 1.f, 1.f ) );
  DALI_TEST_EQUALS( field.GetProperty<Vector2>( TextField::Property::SHADOW_OFFSET ), Vector2( 1.f, 1.f ), TEST_LOCATION );
  field.SetProperty( TextField::Property::SHADOW_COLOR, Color::GREEN );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::SHADOW_COLOR ), Color::GREEN, TEST_LOCATION );

  // Check cursor properties
  field.SetProperty( TextField::Property::PRIMARY_CURSOR_COLOR, Color::RED );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::PRIMARY_CURSOR_COLOR ), Color::RED, TEST_LOCATION );
  field.SetProperty( TextField::Property::SECONDARY_CURSOR_COLOR, Color::BLUE );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::SECONDARY_CURSOR_COLOR ), Color::BLUE, TEST_LOCATION );

  field.SetProperty( TextField::Property::ENABLE_CURSOR_BLINK, false );
  DALI_TEST_EQUALS( field.GetProperty<bool>( TextField::Property::ENABLE_CURSOR_BLINK ), false, TEST_LOCATION );
  field.SetProperty( TextField::Property::CURSOR_BLINK_INTERVAL, 1.f );
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::CURSOR_BLINK_INTERVAL ), 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  field.SetProperty( TextField::Property::CURSOR_BLINK_DURATION, 10.f );
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::CURSOR_BLINK_DURATION ), 10.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  field.SetProperty( TextField::Property::CURSOR_WIDTH, 1 );
  DALI_TEST_EQUALS( field.GetProperty<int>( TextField::Property::CURSOR_WIDTH ), 1, TEST_LOCATION );

  // Check scroll properties.
  field.SetProperty( TextField::Property::SCROLL_THRESHOLD, 1.f );
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::SCROLL_THRESHOLD ), 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  field.SetProperty( TextField::Property::SCROLL_SPEED, 100.f );
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::SCROLL_SPEED ), 100.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Check handle images
  field.SetProperty( TextField::Property::GRAB_HANDLE_IMAGE, "image1" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::GRAB_HANDLE_IMAGE ), "image1", TEST_LOCATION );
  field.SetProperty( TextField::Property::GRAB_HANDLE_PRESSED_IMAGE, "image2" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::GRAB_HANDLE_PRESSED_IMAGE ), "image2", TEST_LOCATION );
  field.SetProperty( TextField::Property::SELECTION_HANDLE_IMAGE_LEFT, "image3" );

  // Check handle images
  DALI_TEST_CHECK( SetPropertyMapRetrieved( field, TextField::Property::SELECTION_HANDLE_IMAGE_LEFT, "filename", "leftHandleImage" )  );
  DALI_TEST_CHECK( SetPropertyMapRetrieved( field, TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT, "filename", "rightHandleImage" )  );
  DALI_TEST_CHECK( SetPropertyMapRetrieved( field, TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT, "filename", "leftHandleImagePressed" )  );
  DALI_TEST_CHECK( SetPropertyMapRetrieved( field, TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT, "filename", "rightHandleImagePressed" )  );
  DALI_TEST_CHECK( SetPropertyMapRetrieved( field, TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT, "filename", "leftHandleMarkerImage" )  );
  DALI_TEST_CHECK( SetPropertyMapRetrieved( field, TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT, "filename", "rightHandleMarkerImage" )  );

  // Check the highlight color
  field.SetProperty( TextField::Property::SELECTION_HIGHLIGHT_COLOR, Color::GREEN );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::SELECTION_HIGHLIGHT_COLOR ), Color::GREEN, TEST_LOCATION );

  // Decoration bounding box
  field.SetProperty( TextField::Property::DECORATION_BOUNDING_BOX, Rect<int>( 0, 0, 1, 1 ) );
  DALI_TEST_EQUALS( field.GetProperty<Rect <int > >( TextField::Property::DECORATION_BOUNDING_BOX ), Rect<int>( 0, 0, 1, 1 ), TEST_LOCATION );

  // Check input color property.
  field.SetProperty( TextField::Property::INPUT_COLOR, Color::YELLOW );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::INPUT_COLOR ), Color::YELLOW, TEST_LOCATION );

  // Check the enable markup property.
  DALI_TEST_CHECK( !field.GetProperty<bool>( TextField::Property::ENABLE_MARKUP ) );
  field.SetProperty( TextField::Property::ENABLE_MARKUP, true );
  DALI_TEST_CHECK( field.GetProperty<bool>( TextField::Property::ENABLE_MARKUP ) );

  // Check input font properties.
  field.SetProperty( TextField::Property::INPUT_FONT_FAMILY, "Setting input font family" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::INPUT_FONT_FAMILY ), "Setting input font family", TEST_LOCATION );

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "weight", "bold" );
  fontStyleMapSet.Insert( "width", "condensed" );
  fontStyleMapSet.Insert( "slant", "italic" );

  field.SetProperty( TextField::Property::INPUT_FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = field.GetProperty<Property::Map>( TextField::Property::INPUT_FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  field.SetProperty( TextField::Property::INPUT_POINT_SIZE, 12.f );
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::INPUT_POINT_SIZE ), 12.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Reset input font style.
  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "weight", "normal" );
  fontStyleMapSet.Insert( "slant", "oblique" );

  field.SetProperty( TextField::Property::INPUT_FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = field.GetProperty<Property::Map>( TextField::Property::INPUT_FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "slant", "roman" );

  field.SetProperty( TextField::Property::INPUT_FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = field.GetProperty<Property::Map>( TextField::Property::INPUT_FONT_STYLE );

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

  field.SetProperty( TextField::Property::INPUT_FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = field.GetProperty<Property::Map>( TextField::Property::INPUT_FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  Property::Map underlineMapSet;
  Property::Map underlineMapGet;

  underlineMapSet.Insert( "enable", "true" );
  underlineMapSet.Insert( "color", "red" );
  underlineMapSet.Insert( "height", "1" );

  // Check the underline property
  field.SetProperty( TextField::Property::UNDERLINE, underlineMapSet );

  underlineMapGet = field.GetProperty<Property::Map>( TextField::Property::UNDERLINE );
  DALI_TEST_EQUALS( underlineMapGet.Count(), underlineMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( underlineMapGet, underlineMapSet ), true, TEST_LOCATION );

  // Check the input underline property
  field.SetProperty( TextField::Property::INPUT_UNDERLINE, "Underline input properties" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::INPUT_UNDERLINE ), std::string("Underline input properties"), TEST_LOCATION );

  // Check the shadow property
  Property::Map shadowMapSet;
  Property::Map shadowMapGet;

  shadowMapSet.Insert( "color", "green" );
  shadowMapSet.Insert( "offset", "2 2" );

  field.SetProperty( TextField::Property::SHADOW, shadowMapSet );

  shadowMapGet = field.GetProperty<Property::Map>( TextField::Property::SHADOW );
  DALI_TEST_EQUALS( shadowMapGet.Count(), shadowMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( shadowMapGet, shadowMapSet ), true, TEST_LOCATION );

  // Check the input shadow property
  field.SetProperty( TextField::Property::INPUT_SHADOW, "Shadow input properties" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::INPUT_SHADOW ), std::string("Shadow input properties"), TEST_LOCATION );

  // Check the emboss property
  field.SetProperty( TextField::Property::EMBOSS, "Emboss properties" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::EMBOSS ), std::string("Emboss properties"), TEST_LOCATION );

  // Check the input emboss property
  field.SetProperty( TextField::Property::INPUT_EMBOSS, "Emboss input properties" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::INPUT_EMBOSS ), std::string("Emboss input properties"), TEST_LOCATION );

  // Check the outline property
  field.SetProperty( TextField::Property::OUTLINE, "Outline properties" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::OUTLINE ), std::string("Outline properties"), TEST_LOCATION );

  // Check the input outline property
  field.SetProperty( TextField::Property::INPUT_OUTLINE, "Outline input properties" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::INPUT_OUTLINE ), std::string("Outline input properties"), TEST_LOCATION );

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
  DALI_TEST_CHECK( field );

  field.SetProperty( TextField::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  Stage::GetCurrent().Add( field );

  try
  {
    // Render some text with the shared atlas backend
    field.SetProperty( TextField::Property::RENDERING_BACKEND, Text::RENDERING_SHARED_ATLAS );
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
int utcDaliTextFieldTextChangedP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldTextChangedP");
  TextField field = TextField::New();
  DALI_TEST_CHECK( field );

  Stage::GetCurrent().Add( field );

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  field.TextChangedSignal().Connect(&TestTextChangedCallback);
  bool textChangedSignal = false;
  field.ConnectSignal( testTracker, "textChanged",   CallbackFunctor(&textChangedSignal) );

  gTextChangedCallBackCalled = false;
  field.SetProperty( TextField::Property::TEXT, "ABC" );
  DALI_TEST_CHECK( gTextChangedCallBackCalled );
  DALI_TEST_CHECK( textChangedSignal );

  application.SendNotification();

  field.SetKeyInputFocus();

  gTextChangedCallBackCalled = false;
  application.ProcessEvent( GenerateKey( "D", "D", 0, 0, 0, Integration::KeyEvent::Down ) );
  DALI_TEST_CHECK( gTextChangedCallBackCalled );

  END_TEST;
}

// Negative test for the textChanged signal.
int utcDaliTextFieldTextChangedN(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldTextChangedN");
  TextField field = TextField::New();
  DALI_TEST_CHECK( field );

  Stage::GetCurrent().Add( field );

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  field.TextChangedSignal().Connect(&TestTextChangedCallback);
  bool textChangedSignal = false;
  field.ConnectSignal( testTracker, "textChanged",   CallbackFunctor(&textChangedSignal) );

  gTextChangedCallBackCalled = false;
  field.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "ABC" ); // Setting placeholder, not TEXT
  DALI_TEST_CHECK( !gTextChangedCallBackCalled );
  DALI_TEST_CHECK( !textChangedSignal );

  END_TEST;
}

// Positive test for Max Characters reached signal.
int utcDaliTextFieldMaxCharactersReachedP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldMaxCharactersReachedP");
  TextField field = TextField::New();
  DALI_TEST_CHECK( field );

  Stage::GetCurrent().Add( field );

  const int maxNumberOfCharacters = 1;
  field.SetProperty( TextField::Property::MAX_LENGTH, maxNumberOfCharacters );

  field.SetKeyInputFocus();

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  field.MaxLengthReachedSignal().Connect(&TestMaxLengthReachedCallback);
  bool maxLengthReachedSignal = false;
  field.ConnectSignal( testTracker, "maxLengthReached",   CallbackFunctor(&maxLengthReachedSignal) );

  gMaxCharactersCallBackCalled = false;

  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );
  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );

  DALI_TEST_CHECK( gMaxCharactersCallBackCalled );
  DALI_TEST_CHECK( maxLengthReachedSignal );

  END_TEST;
}

// Negative test for Max Characters reached signal.
int utcDaliTextFieldMaxCharactersReachedN(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldMaxCharactersReachedN");
  TextField field = TextField::New();
  DALI_TEST_CHECK( field );

  Stage::GetCurrent().Add( field );

  const int maxNumberOfCharacters = 3;
  field.SetProperty( TextField::Property::MAX_LENGTH, maxNumberOfCharacters );

  field.SetKeyInputFocus();

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  field.MaxLengthReachedSignal().Connect(&TestMaxLengthReachedCallback);
  bool maxLengthReachedSignal = false;
  field.ConnectSignal( testTracker, "maxLengthReached",   CallbackFunctor(&maxLengthReachedSignal) );

  gMaxCharactersCallBackCalled = false;

  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );
  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );

  DALI_TEST_CHECK( !gMaxCharactersCallBackCalled );
  DALI_TEST_CHECK( !maxLengthReachedSignal );

  END_TEST;
}

int utcDaliTextFieldInputStyleChanged01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldInputStyleChanged01");

  // The text-field emits signals when the input style changes. These changes of style are
  // detected during the relayout process (size negotiation), i.e after the cursor has been moved. Signals
  // can't be emitted during the size negotiation as the callbacks may update the UI.
  // The text-field adds an idle callback to the adaptor to emit the signals after the size negotiation.
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

  TextField field = TextField::New();
  DALI_TEST_CHECK( field );


  field.SetSize( 300.f, 50.f );
  field.SetParentOrigin( ParentOrigin::TOP_LEFT );
  field.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  field.SetProperty( TextField::Property::ENABLE_MARKUP, true );
  field.SetProperty( TextField::Property::TEXT, "<font family='DejaVuSerif' size='18'>He<color value='green'>llo</color> <font weight='bold'>world</font> demo</font>" );

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  field.InputStyleChangedSignal().Connect( &TestInputStyleChangedCallback );
  bool inputStyleChangedSignal = false;
  field.ConnectSignal( testTracker, "inputStyleChanged",   CallbackFunctor(&inputStyleChangedSignal) );

  Stage::GetCurrent().Add( field );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextField::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text field.
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
    DALI_TEST_EQUALS( static_cast<unsigned int>( gInputStyleMask ), static_cast<unsigned int>( TextField::InputStyle::FONT_FAMILY | TextField::InputStyle::POINT_SIZE ), TEST_LOCATION );

    const std::string fontFamily = field.GetProperty( TextField::Property::INPUT_FONT_FAMILY ).Get<std::string>();
    DALI_TEST_EQUALS( fontFamily, "DejaVuSerif", TEST_LOCATION );

    const float pointSize = field.GetProperty( TextField::Property::INPUT_POINT_SIZE ).Get<float>();
    DALI_TEST_EQUALS( pointSize, 18.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextField::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text field.
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
  gInputStyleMask = TextField::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text field.
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
    DALI_TEST_EQUALS( static_cast<unsigned int>( gInputStyleMask ), static_cast<unsigned int>( TextField::InputStyle::COLOR ), TEST_LOCATION );

    const Vector4 color = field.GetProperty( TextField::Property::INPUT_COLOR ).Get<Vector4>();
    DALI_TEST_EQUALS( color, Color::GREEN, TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextField::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text field.
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
    DALI_TEST_EQUALS( static_cast<unsigned int>( gInputStyleMask ), static_cast<unsigned int>( TextField::InputStyle::COLOR | TextField::InputStyle::FONT_STYLE ), TEST_LOCATION );

    const Vector4 color = field.GetProperty( TextField::Property::INPUT_COLOR ).Get<Vector4>();
    DALI_TEST_EQUALS( color, Color::BLACK, TEST_LOCATION );

    const Property::Map fontStyleMapGet = field.GetProperty( TextField::Property::INPUT_FONT_STYLE ).Get<Property::Map>();

    Property::Map fontStyleMapSet;
    fontStyleMapSet.Insert( "weight", "bold" );

    DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
    DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextField::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text field.
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
  gInputStyleMask = TextField::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text field.
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
    DALI_TEST_EQUALS( static_cast<unsigned int>( gInputStyleMask ), static_cast<unsigned int>( TextField::InputStyle::FONT_STYLE ), TEST_LOCATION );

    const std::string style = field.GetProperty( TextField::Property::INPUT_FONT_STYLE ).Get<std::string>();
    DALI_TEST_CHECK( style.empty() );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextField::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text field.
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

int utcDaliTextFieldInputStyleChanged02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldInputStyleChanged02");

  // The text-field emits signals when the input style changes. These changes of style are
  // detected during the relayout process (size negotiation), i.e after the cursor has been moved. Signals
  // can't be emitted during the size negotiation as the callbacks may update the UI.
  // The text-field adds an idle callback to the adaptor to emit the signals after the size negotiation.
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

  TextField field = TextField::New();
  DALI_TEST_CHECK( field );


  field.SetSize( 300.f, 50.f );
  field.SetParentOrigin( ParentOrigin::TOP_LEFT );
  field.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  field.SetProperty( TextField::Property::ENABLE_MARKUP, true );
  field.SetProperty( TextField::Property::TEXT, "<font family='DejaVuSerif' size='18'>He<color value='blue'> l</color><color value='green'>lo</color> <font weight='bold'>world</font> demo</font>" );

  // connect to the text changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  field.InputStyleChangedSignal().Connect( &TestInputStyleChangedCallback );
  bool inputStyleChangedSignal = false;
  field.ConnectSignal( testTracker, "inputStyleChanged",   CallbackFunctor(&inputStyleChangedSignal) );

  Stage::GetCurrent().Add( field );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextField::InputStyle::NONE;
  inputStyleChangedSignal = false;

  // Create a tap event to touch the text field.
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
                      static_cast<unsigned int>( TextField::InputStyle::FONT_FAMILY |
                                                 TextField::InputStyle::POINT_SIZE  |
                                                 TextField::InputStyle::COLOR ),
                      TEST_LOCATION );

    const Vector4 color = field.GetProperty( TextField::Property::INPUT_COLOR ).Get<Vector4>();
    DALI_TEST_EQUALS( color, Color::GREEN, TEST_LOCATION );

    const std::string fontFamily = field.GetProperty( TextField::Property::INPUT_FONT_FAMILY ).Get<std::string>();
    DALI_TEST_EQUALS( fontFamily, "DejaVuSerif", TEST_LOCATION );

    const float pointSize = field.GetProperty( TextField::Property::INPUT_POINT_SIZE ).Get<float>();
    DALI_TEST_EQUALS( pointSize, 18.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextField::InputStyle::NONE;
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
                      static_cast<unsigned int>( TextField::InputStyle::COLOR ),
                      TEST_LOCATION );

    const Vector4 color = field.GetProperty( TextField::Property::INPUT_COLOR ).Get<Vector4>();
    DALI_TEST_EQUALS( color, Color::BLUE, TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextField::InputStyle::NONE;
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
  gInputStyleMask = TextField::InputStyle::NONE;
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
                      static_cast<unsigned int>( TextField::InputStyle::COLOR ),
                      TEST_LOCATION );

    const Vector4 color = field.GetProperty( TextField::Property::INPUT_COLOR ).Get<Vector4>();
    DALI_TEST_EQUALS( color, Color::BLACK, TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  gInputStyleChangedCallbackCalled = false;
  gInputStyleMask = TextField::InputStyle::NONE;
  inputStyleChangedSignal = false;

  field.SetProperty( TextField::Property::INPUT_COLOR, Color::YELLOW );

  Property::Map fontStyleMapSet;
  fontStyleMapSet.Insert( "weight", "thin" );
  fontStyleMapSet.Insert( "width", "condensed" );
  fontStyleMapSet.Insert( "slant", "italic" );

  field.SetProperty( TextField::Property::INPUT_FONT_STYLE, fontStyleMapSet );
  field.SetProperty( TextField::Property::INPUT_POINT_SIZE, 20.f );

  field.SetProperty( TextField::Property::INPUT_UNDERLINE, "underline" );
  field.SetProperty( TextField::Property::INPUT_SHADOW, "shadow" );
  field.SetProperty( TextField::Property::INPUT_EMBOSS, "emboss" );
  field.SetProperty( TextField::Property::INPUT_OUTLINE, "outline" );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Executes the idle callbacks added by the text control on the change of input style.
  application.RunIdles();

  DALI_TEST_CHECK( !gInputStyleChangedCallbackCalled );
  DALI_TEST_CHECK( !inputStyleChangedSignal );

  // Create a tap event to touch the text field.
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
                      static_cast<unsigned int>( TextField::InputStyle::COLOR |
                                                 TextField::InputStyle::POINT_SIZE |
                                                 TextField::InputStyle::FONT_STYLE |
                                                 TextField::InputStyle::UNDERLINE |
                                                 TextField::InputStyle::SHADOW |
                                                 TextField::InputStyle::EMBOSS |
                                                 TextField::InputStyle::OUTLINE ),
                      TEST_LOCATION );

    const Vector4 color = field.GetProperty( TextField::Property::INPUT_COLOR ).Get<Vector4>();
    DALI_TEST_EQUALS( color, Color::BLACK, TEST_LOCATION );
  }
  DALI_TEST_CHECK( inputStyleChangedSignal );

  END_TEST;
}

int utcDaliTextFieldEvent01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldEvent01");

  // Creates a tap event. After creating a tap event the text field should
  // have the focus and add text with key events should be possible.

  TextField field = TextField::New();
  DALI_TEST_CHECK( field );

  Stage::GetCurrent().Add( field );

  field.SetSize( 300.f, 50.f );
  field.SetParentOrigin( ParentOrigin::TOP_LEFT );
  field.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Add a key event but as the text field has not the focus it should do nothing.
  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::TEXT ), std::string(""), TEST_LOCATION );

  // Create a tap event to touch the text field.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Now the text field has the focus, so it can handle the key events.
  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );
  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::TEXT ), std::string("aa"), TEST_LOCATION );

  // Create a second text field and send key events to it.
  TextField field2 = TextField::New();

  field2.SetParentOrigin( ParentOrigin::TOP_LEFT );
  field2.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  field2.SetSize( 100.f, 100.f );
  field2.SetPosition( 100.f, 100.f );

  Stage::GetCurrent().Add( field2 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Create a tap event on the second text field.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 150.0f, 125.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 150.0f, 125.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // The second text field has the focus. It should handle the key events.
  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );
  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check the text has been added to the second text field.
  DALI_TEST_EQUALS( field2.GetProperty<std::string>( TextField::Property::TEXT ), std::string("aa"), TEST_LOCATION );

  END_TEST;
}

int utcDaliTextFieldEvent02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldEvent02");

  // Checks if the right number of actors are created.

  TextField field = TextField::New();
  field.SetProperty( TextField::Property::POINT_SIZE, 10.f );
  DALI_TEST_CHECK( field );
  LoadMarkerImages(application, field);

  Stage::GetCurrent().Add( field );

  field.SetSize( 300.f, 50.f );
  field.SetParentOrigin( ParentOrigin::TOP_LEFT );
  field.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check there are the expected number of children ( stencil ).
  DALI_TEST_EQUALS( field.GetChildCount(), 1u, TEST_LOCATION );

  Actor stencil = field.GetChildAt( 0u );
  DALI_TEST_EQUALS( stencil.GetChildCount(), 0u, TEST_LOCATION );

  // Create a tap event to touch the text field.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  Actor layer = field.GetChildAt( 1u );
  DALI_TEST_CHECK( layer.IsLayer() );

  DALI_TEST_EQUALS( layer.GetChildCount(), 1u, TEST_LOCATION ); // The cursor.
  DALI_TEST_EQUALS( stencil.GetChildCount(), 0u, TEST_LOCATION );

  // Now the text field has the focus, so it can handle the key events.
  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );
  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Checks the cursor and the renderer have been created.
  DALI_TEST_EQUALS( layer.GetChildCount(), 1u, TEST_LOCATION ); // The cursor.
  DALI_TEST_EQUALS( stencil.GetChildCount(), 1u, TEST_LOCATION ); // The renderer

  Control cursor = Control::DownCast( layer.GetChildAt( 0u ) );
  DALI_TEST_CHECK( cursor );

  // The offscreen root actor has a container with all the actors which contain the text renderers.
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
  field.SetProperty( TextField::Property::TEXT, "" );

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

int utcDaliTextFieldEvent03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldEvent03");

  // Checks if the highlight actor is created.

  TextField field = TextField::New();
  DALI_TEST_CHECK( field );

  Stage::GetCurrent().Add( field );

  field.SetProperty( TextField::Property::TEXT, "This is a long text for the size of the text-field." );
  field.SetProperty( TextField::Property::POINT_SIZE, 10.f );
  field.SetSize( 30.f, 50.f );
  field.SetParentOrigin( ParentOrigin::TOP_LEFT );
  field.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  LoadMarkerImages(application, field);

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

  // The offscreen root actor should have two actors: the renderer and the highlight actor.
  Actor stencil = field.GetChildAt( 0u );

  // The offscreen root actor has a container with all the actors which contain the text renderers.
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

int utcDaliTextFieldEvent04(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldEvent04");

  // Checks if the highlight actor is created.

  TextField field = TextField::New();
  DALI_TEST_CHECK( field );
  Stage::GetCurrent().Add( field );
  LoadMarkerImages(application, field);
  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetProperty( TextField::Property::TEXT, "This is a long text for the size of the text-field." );
  field.SetProperty( TextField::Property::POINT_SIZE, 10.f );
  field.SetSize( 300.f, 50.f );
  field.SetParentOrigin( ParentOrigin::TOP_LEFT );
  field.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  // Render and notify
  application.SendNotification();
  application.Render();

  // Create a tap event to touch the text field.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );
  // Render and notify
  application.SendNotification();
  application.Render();


  // Tap first to get the focus.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 1.f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 1.f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Double tap to select a word.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 2u, 1u, Vector2( 1.f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 2u, 1u, Vector2( 1.f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap grab handle
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 0.f, 40.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 0.f, 40.0f ) ) );
  END_TEST;
}

int utcDaliTextFieldEvent05(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldEvent05");

  // Checks dragging of cursor/grab handle

  TextField field = TextField::New();
  DALI_TEST_CHECK( field );
  Stage::GetCurrent().Add( field );
  LoadMarkerImages(application, field);
  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetProperty( TextField::Property::TEXT, "This is a long text for the size of the text-field." );
  field.SetProperty( TextField::Property::POINT_SIZE, 10.f );
  field.SetSize( 300.f, 50.f );
  field.SetParentOrigin( ParentOrigin::TOP_LEFT );
  field.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  // Render and notify
  application.SendNotification();
  application.Render();

  // Create a tap event to touch the text field.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );
  // Render and notify
  application.SendNotification();
  application.Render();


  // Tap first to get the focus.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 1.f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 1.f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Double tap to select a word.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 2u, 1u, Vector2( 1.f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 2u, 1u, Vector2( 1.f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // drag grab handle right
  Vector2 pos(0.0f, 40.0f);
  SendPan(application, Gesture::Possible, pos);
  SendPan(application, Gesture::Started, pos);
  pos.x += 5.0f;
  Wait(application, 100);

  for(int i = 0;i<20;i++)
  {
    SendPan(application, Gesture::Continuing, pos);
    pos.x += 5.0f;
    Wait(application);
  }

  SendPan(application, Gesture::Finished, pos);
  Wait(application, RENDER_FRAME_INTERVAL);

  Actor stencil = field.GetChildAt( 1u );
  END_TEST;
}

int utcDaliTextFieldEvent06(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldEvent06");

  // Checks Longpress when in edit mode

  TextField field = TextField::New();
  DALI_TEST_CHECK( field );
  Stage::GetCurrent().Add( field );
  LoadMarkerImages(application, field);
  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetProperty( TextField::Property::TEXT, "Thisisalongtextforthesizeofthetextfield." );
  field.SetProperty( TextField::Property::POINT_SIZE, 10.f );
  field.SetSize( 300.f, 50.f );
  field.SetParentOrigin( ParentOrigin::TOP_LEFT );
  field.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  // Render and notify
  application.SendNotification();
  application.Render();

  // Create a tap event to touch the text field.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );
  // Render and notify
  application.SendNotification();
  application.Render();


  // Tap first to get the focus.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 1.f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 1.f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Long Press
  application.ProcessEvent( GenerateLongPress( Gesture::Possible, 1u, Vector2( 1.f, 25.0f ) ) );
  application.ProcessEvent( GenerateLongPress( Gesture::Started,  1u, Vector2( 1.f, 25.0f ) ) );

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
  DALI_TEST_CHECK( field );
  Stage::GetCurrent().Add( field );
  LoadMarkerImages(application, field);
  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetProperty( TextField::Property::TEXT, "Thisisalongtextforthesizeofthetextfield." );
  field.SetProperty( TextField::Property::POINT_SIZE, 10.f );
  field.SetSize( 300.f, 50.f );
  field.SetParentOrigin( ParentOrigin::TOP_LEFT );
  field.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  // Render and notify
  application.SendNotification();
  application.Render();

  // Long Press
  application.ProcessEvent( GenerateLongPress( Gesture::Possible, 1u, Vector2( 1.f, 25.0f ) ) );
  application.ProcessEvent( GenerateLongPress( Gesture::Started,  1u, Vector2( 1.f, 25.0f ) ) );

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
  DALI_TEST_CHECK( field );
  Stage::GetCurrent().Add( field );
  LoadMarkerImages(application, field);
  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "Setting Placeholder Text" );
  field.SetProperty( TextField::Property::POINT_SIZE, 10.f );
  field.SetSize( 300.f, 50.f );
  field.SetParentOrigin( ParentOrigin::TOP_LEFT );
  field.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  // Render and notify
  application.SendNotification();
  application.Render();

  // Long Press
  application.ProcessEvent( GenerateLongPress( Gesture::Possible, 1u, Vector2( 1.f, 25.0f ) ) );
  application.ProcessEvent( GenerateLongPress( Gesture::Started,  1u, Vector2( 1.f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  Wait(application, 500);

  Stage stage = Stage::GetCurrent();
  Layer layer = stage.GetRootLayer();
  Actor actor = layer.FindChildByName("optionPaste");

  if (actor)
  {
    Vector3 worldPosition = actor.GetCurrentWorldPosition();
    Vector2 halfStageSize = stage.GetSize() / 2.0f;
    Vector2 position(worldPosition.x + halfStageSize.width, worldPosition.y + halfStageSize.height);

    Dali::Integration::TouchEvent event;
    event = Dali::Integration::TouchEvent();
    event.AddPoint( GetPointDownInside( position ) );
    application.ProcessEvent( event );

    event = Dali::Integration::TouchEvent();
    event.AddPoint( GetPointUpInside( position ) );
    application.ProcessEvent( event );
  }
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextEditor::Property::TEXT ), std::string("testTextFieldEvent"), TEST_LOCATION );
  END_TEST;
}

int utcDaliTextFieldStyleWhilstSelected(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldStyleWhilstSelected");

  // Change font and styles whilst text is selected whilst word selected

  TextField field = TextField::New();
  DALI_TEST_CHECK( field );
  Stage::GetCurrent().Add( field );
  LoadMarkerImages(application, field);
  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetProperty( TextField::Property::TEXT, "This is a long text for the size of the text-field." );
  field.SetProperty( TextField::Property::POINT_SIZE, 10.f );
  field.SetSize( 300.f, 50.f );
  field.SetParentOrigin( ParentOrigin::TOP_LEFT );
  field.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  // Render and notify
  application.SendNotification();
  application.Render();

  // Create a tap event to touch the text field.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );
  // Render and notify
  application.SendNotification();
  application.Render();


  // Tap first to get the focus.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 1.f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 1.f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Double tap to select a word.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 2u, 1u, Vector2( 1.f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 2u, 1u, Vector2( 1.f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  field.SetProperty( TextField::Property::INPUT_FONT_FAMILY, "Setting input font family" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::INPUT_FONT_FAMILY ), "Setting input font family", TEST_LOCATION );

  Property::Map fontStyleMapSet;
  Property::Map fontStyleMapGet;

  fontStyleMapSet.Insert( "weight", "bold" );
  fontStyleMapSet.Insert( "slant", "italic" );
  field.SetProperty( TextField::Property::INPUT_FONT_STYLE, fontStyleMapSet );

  fontStyleMapGet = field.GetProperty<Property::Map>( TextField::Property::INPUT_FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "width", "expanded" );
  fontStyleMapSet.Insert( "slant", "italic" );
  field.SetProperty( TextField::Property::INPUT_FONT_STYLE, fontStyleMapSet );

  fontStyleMapGet = field.GetProperty<Property::Map>( TextField::Property::INPUT_FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  field.SetProperty( TextField::Property::INPUT_POINT_SIZE, 12.f );
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::INPUT_POINT_SIZE ), 12.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  field.SetProperty( TextField::Property::TEXT_COLOR, Color::RED );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::TEXT_COLOR ), Color::RED, TEST_LOCATION );

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "weight", "bold" );
  fontStyleMapSet.Insert( "slant", "italic" );

  field.SetProperty( TextField::Property::FONT_STYLE, fontStyleMapSet );

  fontStyleMapGet = field.GetProperty<Property::Map>( TextField::Property::FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "width", "expanded" );

  field.SetProperty( TextField::Property::FONT_STYLE, fontStyleMapSet );

  fontStyleMapGet = field.GetProperty<Property::Map>( TextField::Property::FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  END_TEST;
}
