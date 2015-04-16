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
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

void dali_textlabel_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_textlabel_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

const char* const PROPERTY_NAME_RENDERING_BACKEND = "rendering-backend";
const char* const PROPERTY_NAME_TEXT = "text";
const char* const PROPERTY_NAME_FONT_FAMILY = "font-family";
const char* const PROPERTY_NAME_FONT_STYLE = "font-style";
const char* const PROPERTY_NAME_POINT_SIZE = "point-size";
const char* const PROPERTY_NAME_MULTI_LINE =  "multi-line";
const char* const PROPERTY_NAME_HORIZONTAL_ALIGNMENT = "horizontal-alignment";
const char* const PROPERTY_NAME_VERTICAL_ALIGNMENT = "vertical-alignment";
const char* const PROPERTY_NAME_TEXT_COLOR = "text-color";
const char* const PROPERTY_NAME_SHADOW_OFFSET = "shadow-offset";
const char* const PROPERTY_NAME_SHADOW_COLOR = "shadow-color";
const char* const PROPERTY_NAME_UNDERLINE_ENABLED = "underline-enabled";
const char* const PROPERTY_NAME_UNDERLINE_COLOR = "underline-color";

static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

} // namespace

int UtcDaliTextLabelNew(void)
{
  ToolkitTestApplication application;
  TextLabel label = TextLabel::New("Test Text");
  DALI_TEST_CHECK( label );

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect(&TestCallback);
  {
    TextLabel label = TextLabel::New("Test Text");
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
  END_TEST;
}

int UtcDaliTextLabelGetSetProperty(void)
{
  ToolkitTestApplication application;
  TextLabel label = TextLabel::New("Test Text");
  DALI_TEST_CHECK( label );

  // Check Property Indices are correct
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_RENDERING_BACKEND ) == TextLabel::Property::RENDERING_BACKEND );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_TEXT ) == TextLabel::Property::TEXT );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_FONT_FAMILY ) == TextLabel::Property::FONT_FAMILY );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_FONT_STYLE ) == TextLabel::Property::FONT_STYLE );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_POINT_SIZE ) == TextLabel::Property::POINT_SIZE );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_MULTI_LINE ) == TextLabel::Property::MULTI_LINE );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_HORIZONTAL_ALIGNMENT ) == TextLabel::Property::HORIZONTAL_ALIGNMENT );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_VERTICAL_ALIGNMENT ) == TextLabel::Property::VERTICAL_ALIGNMENT );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_TEXT_COLOR ) == TextLabel::Property::TEXT_COLOR );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_SHADOW_OFFSET ) == TextLabel::Property::SHADOW_OFFSET );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_SHADOW_COLOR ) == TextLabel::Property::SHADOW_COLOR );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_UNDERLINE_ENABLED ) == TextLabel::Property::UNDERLINE_ENABLED );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_UNDERLINE_COLOR ) == TextLabel::Property::UNDERLINE_COLOR );

  // Check label defaults are correct
  DALI_TEST_EQUALS( label.GetProperty<int>( TextLabel::Property::RENDERING_BACKEND ), Text::RENDERING_SHARED_ATLAS, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<Vector4>( TextLabel::Property::TEXT_COLOR ), Color::WHITE, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<Vector2>( TextLabel::Property::SHADOW_OFFSET ), Vector2::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<Vector4>( TextLabel::Property::SHADOW_COLOR ), Color::BLACK, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<bool>( TextLabel::Property::UNDERLINE_ENABLED ), false, TEST_LOCATION );

  // Check that text can be correctly reset
  label.SetProperty( TextLabel::Property::TEXT, "Setting Text" );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::TEXT ), "Setting Text", TEST_LOCATION );

  // Check that the Alignment properties can be correctly set
  label.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "BEGIN" );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::HORIZONTAL_ALIGNMENT ), "BEGIN", TEST_LOCATION );
  label.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "TOP" );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::VERTICAL_ALIGNMENT ), "TOP", TEST_LOCATION );

  // Check that shadow parameters can be correctly set
  label.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 3.0f, 3.0f ) );
  DALI_TEST_EQUALS( label.GetProperty<Vector2>( TextLabel::Property::SHADOW_OFFSET ), Vector2( 3.0f, 3.0f ), TEST_LOCATION );
  label.SetProperty( TextLabel::Property::SHADOW_COLOR, Color::BLUE );
  DALI_TEST_EQUALS( label.GetProperty<Vector4>( TextLabel::Property::SHADOW_COLOR ), Color::BLUE, TEST_LOCATION );

  // Check that underline parameters can be correctly set
  label.SetProperty( TextLabel::Property::UNDERLINE_ENABLED, true );
  DALI_TEST_EQUALS( label.GetProperty<bool>( TextLabel::Property::UNDERLINE_ENABLED ), true, TEST_LOCATION );
  label.SetProperty( TextLabel::Property::UNDERLINE_COLOR, Color::RED );
  DALI_TEST_EQUALS( label.GetProperty<Vector4>( TextLabel::Property::UNDERLINE_COLOR ), Color::RED, TEST_LOCATION );

  // Toggle multi-line
  label.SetProperty( TextLabel::Property::MULTI_LINE, true );
  DALI_TEST_EQUALS( label.GetProperty<bool>( TextLabel::Property::MULTI_LINE ), true, TEST_LOCATION );
  END_TEST;
}

int utcDaliTextlabelBasicRender(void)
{
  ToolkitTestApplication application;
  TextLabel label = TextLabel::New("Test Text");
  DALI_TEST_CHECK( label );

  label.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "BEGIN" );
  label.SetProperty( TextLabel::Property::MULTI_LINE, true );

  try
  {
    // Render some text with the basic backend
    label.SetProperty( TextLabel::Property::RENDERING_BACKEND, Text::RENDERING_BASIC );
    application.SendNotification();
    application.Render();
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int utcDaliTextlabelAtlasRender(void)
{
  ToolkitTestApplication application;
  TextLabel label = TextLabel::New("Test Text");
  DALI_TEST_CHECK( label );

  // Turn on all the effects
  label.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  label.SetProperty( TextLabel::Property::MULTI_LINE, true );
  label.SetProperty( TextLabel::Property::UNDERLINE_ENABLED, true );
  label.SetProperty( TextLabel::Property::UNDERLINE_COLOR, Color::RED );
  label.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 1.0f, 1.0f ) );
  label.SetProperty( TextLabel::Property::SHADOW_COLOR, Color::BLUE );
  try
  {
    // Render some text with the shared atlas backend
    label.SetProperty( TextLabel::Property::RENDERING_BACKEND, Text::RENDERING_SHARED_ATLAS );
    application.SendNotification();
    application.Render();
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }
  END_TEST;
}

