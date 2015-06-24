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
 *
 */

#include <iostream>
#include <stdlib.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/tap-gesture-event.h>
#include <dali/devel-api/actors/mesh-actor.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/styling/style-manager.h>

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

const char* const PROPERTY_NAME_RENDERING_BACKEND                    = "rendering-backend";
const char* const PROPERTY_NAME_TEXT                                 = "text";
const char* const PROPERTY_NAME_PLACEHOLDER_TEXT                     = "placeholder-text";
const char* const PROPERTY_NAME_PLACEHOLDER_TEXT_FOCUSED             = "placeholder-text-focused";
const char* const PROPERTY_NAME_FONT_FAMILY                          = "font-family";
const char* const PROPERTY_NAME_FONT_STYLE                           = "font-style";
const char* const PROPERTY_NAME_POINT_SIZE                           = "point-size";
const char* const PROPERTY_NAME_MAX_LENGTH                           = "max-length";
const char* const PROPERTY_NAME_EXCEED_POLICY                        = "exceed-policy";
const char* const PROPERTY_NAME_HORIZONTAL_ALIGNMENT                 = "horizontal-alignment";
const char* const PROPERTY_NAME_VERTICAL_ALIGNMENT                   = "vertical-alignment";
const char* const PROPERTY_NAME_TEXT_COLOR                           = "text-color";
const char* const PROPERTY_NAME_PLACEHOLDER_TEXT_COLOR               = "placeholder-text-color";
const char* const PROPERTY_NAME_SHADOW_OFFSET                        = "shadow-offset";
const char* const PROPERTY_NAME_SHADOW_COLOR                         = "shadow-color";
const char* const PROPERTY_NAME_PRIMARY_CURSOR_COLOR                 = "primary-cursor-color";
const char* const PROPERTY_NAME_SECONDARY_CURSOR_COLOR               = "secondary-cursor-color";
const char* const PROPERTY_NAME_ENABLE_CURSOR_BLINK                  = "enable-cursor-blink";
const char* const PROPERTY_NAME_CURSOR_BLINK_INTERVAL                = "cursor-blink-interval";
const char* const PROPERTY_NAME_CURSOR_BLINK_DURATION                = "cursor-blink-duration";
const char* const PROPERTY_NAME_GRAB_HANDLE_IMAGE                    = "grab-handle-image";
const char* const PROPERTY_NAME_GRAB_HANDLE_PRESSED_IMAGE            = "grab-handle-pressed-image";
const char* const PROPERTY_NAME_SCROLL_THRESHOLD                     = "scroll-threshold";
const char* const PROPERTY_NAME_SCROLL_SPEED                         = "scroll-speed";
const char* const PROPERTY_NAME_SELECTION_HANDLE_IMAGE_LEFT          = "selection-handle-image-left";
const char* const PROPERTY_NAME_SELECTION_HANDLE_IMAGE_RIGHT         = "selection-handle-image-right";
const char* const PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_LEFT  = "selection-handle-pressed-image-left";
const char* const PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_RIGHT = "selection-handle-pressed-image-right";
const char* const PROPERTY_NAME_SELECTION_HIGHLIGHT_COLOR            = "selection-highlight-color";
const char* const PROPERTY_NAME_DECORATION_BOUNDING_BOX              = "decoration-bounding-box";
const char* const PROPERTY_NAME_INPUT_METHOD_SETTINGS                = "input-method-settings";

const unsigned int DEFAULT_RENDERING_BACKEND = Dali::Toolkit::Text::DEFAULT_RENDERING_BACKEND;

const Vector4 PLACEHOLDER_TEXT_COLOR( 0.8f, 0.8f, 0.8f, 0.8f );
const Dali::Vector4 LIGHT_BLUE( (0xb2 / 255.0f), (0xeb / 255.0f), (0xf2 / 255.0f), 0.5f ); // The text highlight color.

const unsigned int CURSOR_BLINK_INTERVAL = 500u; // Cursor blink interval
const float TO_MILLISECONDS = 1000.f;
const float TO_SECONDS = 1.f / TO_MILLISECONDS;

const float SCROLL_THRESHOLD = 10.f;
const float SCROLL_SPEED = 300.f;

static bool gTextChangedCallBackCalled;
static bool gMaxCharactersCallBackCalled;

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
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_GRAB_HANDLE_IMAGE ) == TextField::Property::GRAB_HANDLE_IMAGE );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_GRAB_HANDLE_PRESSED_IMAGE ) == TextField::Property::GRAB_HANDLE_PRESSED_IMAGE );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SCROLL_THRESHOLD ) == TextField::Property::SCROLL_THRESHOLD );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SCROLL_SPEED ) == TextField::Property::SCROLL_SPEED );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SELECTION_HANDLE_IMAGE_LEFT ) == TextField::Property::SELECTION_HANDLE_IMAGE_LEFT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SELECTION_HANDLE_IMAGE_RIGHT ) == TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_LEFT ) == TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SELECTION_HANDLE_PRESSED_IMAGE_RIGHT ) == TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SELECTION_HIGHLIGHT_COLOR ) == TextField::Property::SELECTION_HIGHLIGHT_COLOR );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_DECORATION_BOUNDING_BOX ) == TextField::Property::DECORATION_BOUNDING_BOX );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_INPUT_METHOD_SETTINGS ) == TextField::Property::INPUT_METHOD_SETTINGS );
  END_TEST;
}

// Positive test case for a method
int UtcDaliTextFieldSetPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldSetPropertyP");
  TextField field = TextField::New();
  DALI_TEST_CHECK( field );

  // Check defaults.
  DALI_TEST_EQUALS( field.GetProperty<unsigned int>( TextField::Property::RENDERING_BACKEND ), DEFAULT_RENDERING_BACKEND, TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::TEXT ), std::string(""), TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::PLACEHOLDER_TEXT ), std::string(""), TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::PLACEHOLDER_TEXT_FOCUSED ), std::string(""), TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::FONT_FAMILY ), std::string(""), TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::FONT_STYLE ), std::string(""), TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::POINT_SIZE ), 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<int>( TextField::Property::MAX_LENGTH ), 50u, TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<int>( TextField::Property::EXCEED_POLICY ), TextField::EXCEED_POLICY_CLIP, TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::HORIZONTAL_ALIGNMENT ), "BEGIN", TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::VERTICAL_ALIGNMENT ), "TOP", TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::TEXT_COLOR ), Color::BLACK, TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<Vector2>( TextField::Property::SHADOW_OFFSET ), Vector2::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::PLACEHOLDER_TEXT_COLOR ), PLACEHOLDER_TEXT_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::SHADOW_COLOR ), Color::BLACK, TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::PRIMARY_CURSOR_COLOR ), Color::BLACK, TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::SECONDARY_CURSOR_COLOR ), Color::BLACK, TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<bool>( TextField::Property::ENABLE_CURSOR_BLINK ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::CURSOR_BLINK_INTERVAL ), CURSOR_BLINK_INTERVAL * TO_SECONDS, TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::CURSOR_BLINK_DURATION ), 0.f, TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::SCROLL_THRESHOLD ), SCROLL_THRESHOLD, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::SCROLL_SPEED ), SCROLL_SPEED, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::SELECTION_HIGHLIGHT_COLOR ), LIGHT_BLUE, TEST_LOCATION );

  // Check the render backend property.
  field.SetProperty( TextField::Property::RENDERING_BACKEND, Text::RENDERING_BASIC );
  DALI_TEST_EQUALS( field.GetProperty<unsigned int>( TextField::Property::RENDERING_BACKEND ), Text::RENDERING_BASIC, TEST_LOCATION );

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
  field.SetProperty( TextField::Property::FONT_STYLE, "Setting font style" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::FONT_STYLE ), std::string("Setting font style"), TEST_LOCATION );
  field.SetProperty( TextField::Property::POINT_SIZE, 10.f );
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::POINT_SIZE ), 10.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Check that the MAX_LENGTH property can be correctly set
  const int maxNumberOfCharacters = 20;
  field.SetProperty( TextField::Property::MAX_LENGTH, maxNumberOfCharacters );
  DALI_TEST_EQUALS( field.GetProperty<int>( TextField::Property::MAX_LENGTH ), maxNumberOfCharacters, TEST_LOCATION );

  // Check exceed policy
  // Set a different exceed policy is not implemented.

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
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::CURSOR_BLINK_INTERVAL ), 1.f, TEST_LOCATION );
  field.SetProperty( TextField::Property::CURSOR_BLINK_DURATION, 10.f );
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::CURSOR_BLINK_DURATION ), 10.f, TEST_LOCATION );

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
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::SELECTION_HANDLE_IMAGE_LEFT ), "image3", TEST_LOCATION );
  field.SetProperty( TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT, "image4" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT ), "image4", TEST_LOCATION );
  field.SetProperty( TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT, "image5" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT ), "image5", TEST_LOCATION );
  field.SetProperty( TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT, "image6" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT ), "image6", TEST_LOCATION );

  // Check the highlight color
  field.SetProperty( TextField::Property::SELECTION_HIGHLIGHT_COLOR, Color::GREEN );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::SELECTION_HIGHLIGHT_COLOR ), Color::GREEN, TEST_LOCATION );

  // Decoration bounding box
  field.SetProperty( TextField::Property::DECORATION_BOUNDING_BOX, Rect<int>( 0, 0, 1, 1 ) );
  DALI_TEST_EQUALS( field.GetProperty<Rect <int > >( TextField::Property::DECORATION_BOUNDING_BOX ), Rect<int>( 0, 0, 1, 1 ), TEST_LOCATION );

  END_TEST;
}

// Positive Basic Text Renderer test
int utcDaliTextFieldBasicRenderP(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliToolkitTextFieldBasicRenderP");
  TextField field = TextField::New();
  DALI_TEST_CHECK( field );

  field.SetProperty( TextField::Property::HORIZONTAL_ALIGNMENT, "BEGIN" );

  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  Stage::GetCurrent().Add( field );

  try
  {
    // Render some text with the basic backend
    field.SetProperty( TextField::Property::RENDERING_BACKEND, Text::RENDERING_BASIC );
    application.SendNotification();
    application.Render();
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }
  END_TEST;
}

// Positive Atlas Text Renderer test
int utcDaliTextFieldAtlasRenderP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldAtlasRenderP");
  StyleManager styleManager = StyleManager::Get();
  styleManager.RequestDefaultTheme();
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

// Positive test for the text-changed signal.
int utcDaliTextFieldTextChangedP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldTextChangedP");
  TextField field = TextField::New();
  DALI_TEST_CHECK( field );

  Stage::GetCurrent().Add( field );

  field.TextChangedSignal().Connect(&TestTextChangedCallback);

  gTextChangedCallBackCalled = false;
  field.SetProperty( TextField::Property::TEXT, "ABC" );
  DALI_TEST_CHECK( gTextChangedCallBackCalled );

  application.SendNotification();

  field.SetKeyInputFocus();

  gTextChangedCallBackCalled = false;
  application.ProcessEvent( GenerateKey( "D", "D", 0, 0, 0, Integration::KeyEvent::Down ) );
  DALI_TEST_CHECK( gTextChangedCallBackCalled );

  END_TEST;
}

// Negative test for the text-changed signal.
int utcDaliTextFieldTextChangedN(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldTextChangedN");
  TextField field = TextField::New();
  DALI_TEST_CHECK( field );

  Stage::GetCurrent().Add( field );

  field.TextChangedSignal().Connect(&TestTextChangedCallback);

  gTextChangedCallBackCalled = false;
  field.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "ABC" ); // Setting placeholder, not TEXT
  DALI_TEST_CHECK( !gTextChangedCallBackCalled );

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

  gMaxCharactersCallBackCalled = false;
  field.MaxLengthReachedSignal().Connect(&TestMaxLengthReachedCallback);

  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );
  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );

  DALI_TEST_CHECK( gMaxCharactersCallBackCalled );

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

  gMaxCharactersCallBackCalled = false;
  field.MaxLengthReachedSignal().Connect(&TestMaxLengthReachedCallback);

  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );
  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );

  DALI_TEST_CHECK( !gMaxCharactersCallBackCalled );

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

  // Check there are the expected number of children ( active layer, offscreen root actor, and the offscreen image actor
  DALI_TEST_EQUALS( field.GetChildCount(), 3u, TEST_LOCATION );

  Actor layer = field.GetChildAt( 0u );
  DALI_TEST_CHECK( layer.IsLayer() );

  Actor offscreenRoot = field.GetChildAt( 1u );
  DALI_TEST_CHECK( offscreenRoot.IsLayer() );
  DALI_TEST_EQUALS( offscreenRoot.GetChildCount(), 1u, TEST_LOCATION ); // The camera actor.

  Actor offscreenImage = field.GetChildAt( 2u );
  ImageActor imageActor = ImageActor::DownCast( offscreenImage );
  DALI_TEST_CHECK( imageActor );

  // Create a tap event to touch the text field.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 150.0f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( layer.GetChildCount(), 1u, TEST_LOCATION ); // The cursor.
  DALI_TEST_EQUALS( offscreenRoot.GetChildCount(), 1u, TEST_LOCATION ); // The camera actor.

  // Now the text field has the focus, so it can handle the key events.
  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );
  application.ProcessEvent( GenerateKey( "a", "a", 0, 0, 0, Integration::KeyEvent::Down ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Checks the cursor and the renderer have been created.
  DALI_TEST_EQUALS( layer.GetChildCount(), 1u, TEST_LOCATION ); // The cursor.
  DALI_TEST_EQUALS( offscreenRoot.GetChildCount(), 2u, TEST_LOCATION ); // The camera actor and the renderer

  ImageActor cursor = ImageActor::DownCast( layer.GetChildAt( 0u ) );
  DALI_TEST_CHECK( cursor );

  CameraActor camera = CameraActor::DownCast( offscreenRoot.GetChildAt( 0u ) );
  DALI_TEST_CHECK( camera );

  RenderableActor renderer = RenderableActor::DownCast( offscreenRoot.GetChildAt( 1u ) );
  DALI_TEST_CHECK( renderer );

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
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 3.f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 3.f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Cursor position should be the same than position1.
  Vector3 position4 = cursor.GetCurrentPosition();

  DALI_TEST_EQUALS( position2, position4, TEST_LOCATION ); // Should be in the same position2.

  // Try to tap at the end.
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, Vector2( 13.f, 25.0f ) ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, Vector2( 13.0f, 25.0f ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Cursor position should be the same than position1.
  Vector3 position5 = cursor.GetCurrentPosition();

  DALI_TEST_EQUALS( position1, position5, TEST_LOCATION ); // Should be in the same position1.

  // Remove some text.
  application.ProcessEvent( GenerateKey( "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::Down ) );
  application.ProcessEvent( GenerateKey( "", "", DALI_KEY_BACKSPACE, 0, 0, Integration::KeyEvent::Down ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Cursor position should be the same than position2.
  Vector3 position6 = cursor.GetCurrentPosition();

  DALI_TEST_EQUALS( Vector3( 0.f, position2.y, position2.z ), position6, TEST_LOCATION ); // TODO Should be in the same position2.

  // Should not be renderer.
  DALI_TEST_EQUALS( offscreenRoot.GetChildCount(), 1u, TEST_LOCATION ); // The camera actor only.

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
  field.SetSize( 30.f, 50.f );
  field.SetParentOrigin( ParentOrigin::TOP_LEFT );
  field.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

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

  // The offscreen root actor should have three actors: the camera, a renderer and the highlight actor.
  Actor offscreenRoot = field.GetChildAt( 1u );
  DALI_TEST_CHECK( offscreenRoot.IsLayer() );

  CameraActor camera = CameraActor::DownCast( offscreenRoot.GetChildAt( 0u ) );
  DALI_TEST_CHECK( camera );

  RenderableActor renderer = RenderableActor::DownCast( offscreenRoot.GetChildAt( 1u ) );
  DALI_TEST_CHECK( renderer );

  MeshActor highlight = MeshActor::DownCast( offscreenRoot.GetChildAt( 2u ) );
  DALI_TEST_CHECK( highlight );

  END_TEST;
}
