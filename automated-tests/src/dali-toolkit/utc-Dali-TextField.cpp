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

const char* const PROPERTY_NAME_RENDERING_BACKEND       = "rendering-backend";
const char* const PROPERTY_NAME_PLACEHOLDER_TEXT        = "placeholder-text";
const char* const PROPERTY_NAME_TEXT                    = "text";
const char* const PROPERTY_NAME_FONT_FAMILY             = "font-family";
const char* const PROPERTY_NAME_FONT_STYLE              = "font-style";
const char* const PROPERTY_NAME_POINT_SIZE              = "point-size";
const char* const PROPERTY_NAME_EXCEED_POLICY           = "exceed-policy";
const char* const PROPERTY_NAME_PRIMARY_CURSOR_COLOR    = "primary-cursor-color";
const char* const PROPERTY_NAME_SECONDARY_CURSOR_COLOR  = "secondary-cursor-color";
const char* const PROPERTY_NAME_ENABLE_CURSOR_BLINK     = "enable-cursor-blink";
const char* const PROPERTY_NAME_CURSOR_BLINK_INTERVAL   = "cursor-blink-interval";
const char* const PROPERTY_NAME_CURSOR_BLINK_DURATION   = "cursor-blink-duration";
const char* const PROPERTY_NAME_GRAB_HANDLE_IMAGE       = "grab-handle-image";
const char* const PROPERTY_NAME_DECORATION_BOUNDING_BOX = "decoration-bounding-box";
const char* const PROPERTY_NAME_HORIZONTAL_ALIGNMENT    = "horizontal-alignment";
const char* const PROPERTY_NAME_VERTICAL_ALIGNMENT      = "vertical-alignment";

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
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_PLACEHOLDER_TEXT ) == TextField::Property::PLACEHOLDER_TEXT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_TEXT ) == TextField::Property::TEXT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_FONT_FAMILY ) == TextField::Property::FONT_FAMILY );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_FONT_STYLE ) == TextField::Property::FONT_STYLE );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_POINT_SIZE ) == TextField::Property::POINT_SIZE );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_EXCEED_POLICY ) == TextField::Property::EXCEED_POLICY );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_PRIMARY_CURSOR_COLOR ) == TextField::Property::PRIMARY_CURSOR_COLOR );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_SECONDARY_CURSOR_COLOR ) == TextField::Property::SECONDARY_CURSOR_COLOR );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_ENABLE_CURSOR_BLINK ) == TextField::Property::ENABLE_CURSOR_BLINK );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_CURSOR_BLINK_INTERVAL ) == TextField::Property::CURSOR_BLINK_INTERVAL );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_CURSOR_BLINK_DURATION ) == TextField::Property::CURSOR_BLINK_DURATION );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_GRAB_HANDLE_IMAGE ) == TextField::Property::GRAB_HANDLE_IMAGE );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_DECORATION_BOUNDING_BOX ) == TextField::Property::DECORATION_BOUNDING_BOX );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_HORIZONTAL_ALIGNMENT ) == TextField::Property::HORIZONTAL_ALIGNMENT );
  DALI_TEST_CHECK( field.GetPropertyIndex( PROPERTY_NAME_VERTICAL_ALIGNMENT ) == TextField::Property::VERTICAL_ALIGNMENT );
  END_TEST;
}

// Positive test case for a method
int UtcDaliTextFieldSetPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldSetPropertyP");
  TextField field = TextField::New();
  DALI_TEST_CHECK( field );

  // Check exceed policy
  field.SetProperty( TextField::Property::EXCEED_POLICY, TextField::EXCEED_POLICY_CLIP );
  DALI_TEST_EQUALS( field.GetProperty<int>( TextField::Property::EXCEED_POLICY ), TextField::EXCEED_POLICY_CLIP, TEST_LOCATION );

  // Check cursor properties
  field.SetProperty( TextField::Property::ENABLE_CURSOR_BLINK, true );
  DALI_TEST_EQUALS( field.GetProperty<bool>( TextField::Property::ENABLE_CURSOR_BLINK ), true, TEST_LOCATION );
  field.SetProperty( TextField::Property::PRIMARY_CURSOR_COLOR, Color::RED );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::PRIMARY_CURSOR_COLOR ), Color::RED, TEST_LOCATION );
  field.SetProperty( TextField::Property::SECONDARY_CURSOR_COLOR, Color::BLUE );
  DALI_TEST_EQUALS( field.GetProperty<Vector4>( TextField::Property::SECONDARY_CURSOR_COLOR ), Color::BLUE, TEST_LOCATION );
  field.SetProperty( TextField::Property::CURSOR_BLINK_DURATION, 10.0f );
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::CURSOR_BLINK_DURATION ), 10.0f, TEST_LOCATION );

  // Blink interval
  field.SetProperty( TextField::Property::CURSOR_BLINK_INTERVAL, 1.0f );
  DALI_TEST_EQUALS( field.GetProperty<float>( TextField::Property::CURSOR_BLINK_INTERVAL ), 1.0f, TEST_LOCATION );

  // Decoration bounding box
  field.SetProperty( TextField::Property::DECORATION_BOUNDING_BOX, Rect<int>( 0, 0, 1, 1 ) );
  DALI_TEST_EQUALS( field.GetProperty<Rect <int > >( TextField::Property::DECORATION_BOUNDING_BOX ), Rect<int>( 0, 0, 1, 1 ), TEST_LOCATION );

  // Check that the Alignment properties can be correctly set
  field.SetProperty( TextField::Property::HORIZONTAL_ALIGNMENT, "BEGIN" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::HORIZONTAL_ALIGNMENT ), "BEGIN", TEST_LOCATION );
  field.SetProperty( TextField::Property::VERTICAL_ALIGNMENT, "CENTER" );
  DALI_TEST_EQUALS( field.GetProperty<std::string>( TextField::Property::VERTICAL_ALIGNMENT ), "CENTER", TEST_LOCATION );

  // Set text
  field.SetProperty( TextField::Property::TEXT, "Setting Text" );

  // Set placeholder text (currently not implemented)
  field.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "Setting Text" );

  // Set Grab Handle image
  field.SetProperty( TextField::Property::GRAB_HANDLE_IMAGE, "" );

  // Check that the MAX_LENGTH property can be correctly set
  const int maxNumberOfCharacters = 20;
  field.SetProperty( TextField::Property::MAX_LENGTH, maxNumberOfCharacters );
  DALI_TEST_EQUALS( field.GetProperty<int>( TextField::Property::MAX_LENGTH ), maxNumberOfCharacters, TEST_LOCATION );

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

  Stage::GetCurrent().Add(field);

  field.TextChangedSignal().Connect(&TestTextChangedCallback);

  gTextChangedCallBackCalled = false;
  field.SetProperty( TextField::Property::TEXT, "ABC" );
  DALI_TEST_CHECK( gTextChangedCallBackCalled );

  application.SendNotification();

  field.SetKeyInputFocus();

  Dali::Integration::KeyEvent keyevent;
  keyevent.keyName = "D";
  keyevent.keyString = "D";
  keyevent.keyCode = 0;
  keyevent.keyModifier = 0;
  keyevent.time = 0;
  keyevent.state = Integration::KeyEvent::Down;

  gTextChangedCallBackCalled = false;
  application.ProcessEvent( keyevent );
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

  Stage::GetCurrent().Add(field);

  field.TextChangedSignal().Connect(&TestTextChangedCallback);

  gTextChangedCallBackCalled = false;
  field.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "ABC" ); // Setting placeholder, not TEXT
  DALI_TEST_CHECK( ! gTextChangedCallBackCalled );

  END_TEST;
}

// Positive test for Max Characters reached signal.
int utcDaliTextFieldMaxCharactersReachedP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextFieldMaxCharactersReachedP");
  TextField field = TextField::New();
  DALI_TEST_CHECK( field );

  Stage::GetCurrent().Add(field);

  const int maxNumberOfCharacters = 1;
  field.SetProperty( TextField::Property::MAX_LENGTH, maxNumberOfCharacters );

  field.SetKeyInputFocus();

  gMaxCharactersCallBackCalled = false;
  field.MaxLengthReachedSignal().Connect(&TestMaxLengthReachedCallback);

  Dali::Integration::KeyEvent keyevent;
  keyevent.keyName = "a";
  keyevent.keyString = "a";
  keyevent.keyCode = 0;
  keyevent.keyModifier = 0;
  keyevent.time = 0;
  keyevent.state = Integration::KeyEvent::Down;

  application.ProcessEvent( keyevent );

  application.ProcessEvent( keyevent );

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

  Stage::GetCurrent().Add(field);

  const int maxNumberOfCharacters = 3;
  field.SetProperty( TextField::Property::MAX_LENGTH, maxNumberOfCharacters );

  field.SetKeyInputFocus();

  gMaxCharactersCallBackCalled = false;
  field.MaxLengthReachedSignal().Connect(&TestMaxLengthReachedCallback);

  Dali::Integration::KeyEvent keyevent;
  keyevent.keyName = "a";
  keyevent.keyString = "a";
  keyevent.keyCode = 0;
  keyevent.keyModifier = 0;
  keyevent.time = 0;
  keyevent.state = Integration::KeyEvent::Down;

  application.ProcessEvent( keyevent );
  application.ProcessEvent( keyevent );

  DALI_TEST_CHECK( !gMaxCharactersCallBackCalled );

  END_TEST;
}
