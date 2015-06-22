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
const char* const PROPERTY_NAME_UNDERLINE_HEIGHT = "underline-height";

const unsigned int DEFAULT_RENDERING_BACKEND = Dali::Toolkit::Text::DEFAULT_RENDERING_BACKEND;

} // namespace

int UtcDaliToolkitTextLabelConstructorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelConstructorP");
  TextLabel textLabel;
  DALI_TEST_CHECK( !textLabel );
  END_TEST;
}

int UtcDaliToolkitTextLabelNewP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelNewP");
  TextLabel textLabel = TextLabel::New( "Test Text" );
  DALI_TEST_CHECK( textLabel );
  END_TEST;
}

int UtcDaliToolkitTextLabelDownCastP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelDownCastP");
  TextLabel textLabel1 = TextLabel::New();
  BaseHandle object( textLabel1 );

  TextLabel textLabel2 = TextLabel::DownCast( object );
  DALI_TEST_CHECK( textLabel2 );

  TextLabel textLabel3 = DownCast< TextLabel >( object );
  DALI_TEST_CHECK( textLabel3 );
  END_TEST;
}

int UtcDaliToolkitTextLabelDownCastN(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelDownCastN");
  BaseHandle uninitializedObject;
  TextLabel textLabel1 = TextLabel::DownCast( uninitializedObject );
  DALI_TEST_CHECK( !textLabel1 );

  TextLabel textLabel2 = DownCast< TextLabel >( uninitializedObject );
  DALI_TEST_CHECK( !textLabel2 );
  END_TEST;
}

int UtcDaliToolkitTextLabelCopyConstructorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelCopyConstructorP");
  TextLabel textLabel = TextLabel::New();
  textLabel.SetProperty( TextLabel::Property::TEXT_COLOR, Color::RED );

  TextLabel copy( textLabel );
  DALI_TEST_CHECK( copy );
  DALI_TEST_CHECK( copy.GetProperty<Vector4>( TextLabel::Property::TEXT_COLOR ) == textLabel.GetProperty<Vector4>( TextLabel::Property::TEXT_COLOR ) );
  END_TEST;
}

int UtcDaliToolkitTextLabelAssignmentOperatorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAssingmentOperatorP");
  TextLabel textLabel = TextLabel::New();
  textLabel.SetProperty( TextLabel::Property::TEXT_COLOR, Color::RED );

  TextLabel copy = textLabel;
  DALI_TEST_CHECK( copy );
  DALI_TEST_CHECK( copy.GetProperty<Vector4>( TextLabel::Property::TEXT_COLOR ) == textLabel.GetProperty<Vector4>( TextLabel::Property::TEXT_COLOR ) );
  END_TEST;
}

// Positive test case for a method
int UtcDaliToolkitTextLabelGetPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelGetPropertyP");
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
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_UNDERLINE_HEIGHT) == TextLabel::Property::UNDERLINE_HEIGHT );

  END_TEST;
}

int UtcDaliToolkitTextLabelSetPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelSetPropertyP");
  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK( label );

  // Check defaults
  DALI_TEST_EQUALS( label.GetProperty<unsigned int>( TextLabel::Property::RENDERING_BACKEND ), DEFAULT_RENDERING_BACKEND, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::TEXT ), std::string(""), TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::FONT_FAMILY ), std::string(""), TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::FONT_STYLE ), std::string(""), TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<float>( TextLabel::Property::POINT_SIZE ), 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<bool>( TextLabel::Property::MULTI_LINE ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::HORIZONTAL_ALIGNMENT ), "BEGIN", TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::VERTICAL_ALIGNMENT ), "TOP", TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<Vector4>( TextLabel::Property::TEXT_COLOR ), Color::BLACK, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<Vector2>( TextLabel::Property::SHADOW_OFFSET ), Vector2::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<Vector4>( TextLabel::Property::SHADOW_COLOR ), Color::BLACK, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<bool>( TextLabel::Property::UNDERLINE_ENABLED ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<Vector4>( TextLabel::Property::UNDERLINE_COLOR ), Color::BLACK, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty<float>( TextLabel::Property::UNDERLINE_HEIGHT ), 0.0f, TEST_LOCATION );

  label.SetProperty( TextLabel::Property::RENDERING_BACKEND, Text::RENDERING_BASIC );
  DALI_TEST_EQUALS( label.GetProperty<unsigned int>( TextLabel::Property::RENDERING_BACKEND ), Text::RENDERING_BASIC, TEST_LOCATION );

  // Check that text can be correctly reset
  label.SetProperty( TextLabel::Property::TEXT, "Setting Text" );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::TEXT ), std::string("Setting Text"), TEST_LOCATION );

  // Check font properties.
  label.SetProperty( TextLabel::Property::FONT_FAMILY, "Setting font family" );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::FONT_FAMILY ), std::string("Setting font family"), TEST_LOCATION );
  label.SetProperty( TextLabel::Property::FONT_STYLE, "Setting font style" );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::FONT_STYLE ), std::string("Setting font style"), TEST_LOCATION );
  label.SetProperty( TextLabel::Property::POINT_SIZE, 10.f );
  DALI_TEST_EQUALS( label.GetProperty<float>( TextLabel::Property::POINT_SIZE ), 10.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Toggle multi-line
  label.SetProperty( TextLabel::Property::MULTI_LINE, true );
  DALI_TEST_EQUALS( label.GetProperty<bool>( TextLabel::Property::MULTI_LINE ), true, TEST_LOCATION );

  // Check that the Alignment properties can be correctly set
  label.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::HORIZONTAL_ALIGNMENT ), "CENTER", TEST_LOCATION );
  label.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::VERTICAL_ALIGNMENT ), "CENTER", TEST_LOCATION );

  // Check that text color can be properly set
  label.SetProperty( TextLabel::Property::TEXT_COLOR, Color::BLUE );
  DALI_TEST_EQUALS( label.GetProperty<Vector4>( TextLabel::Property::TEXT_COLOR ), Color::BLUE, TEST_LOCATION );
  // The underline color is changed as well.
  DALI_TEST_EQUALS( label.GetProperty<Vector4>( TextLabel::Property::UNDERLINE_COLOR ), Color::BLUE, TEST_LOCATION );

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
  label.SetProperty( TextLabel::Property::UNDERLINE_HEIGHT, 1.0f );
  DALI_TEST_EQUALS( label.GetProperty<float>( TextLabel::Property::UNDERLINE_HEIGHT ), 1.0f, TEST_LOCATION );

  TextLabel label2 = TextLabel::New( "New text" );
  DALI_TEST_CHECK( label2 );
  DALI_TEST_EQUALS( label2.GetProperty<std::string>( TextLabel::Property::TEXT ), std::string("New text"), TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitTextlabelBasicRenderP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelBasicRenderP");
  TextLabel label = TextLabel::New("Test Text");
  DALI_TEST_CHECK( label );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  Stage::GetCurrent().Add( label );

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

int UtcDaliToolkitTextlabelAtlasRenderP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAtlasRenderP");
  TextLabel label = TextLabel::New("Test Text");
  DALI_TEST_CHECK( label );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  Stage::GetCurrent().Add( label );

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

int UtcDaliToolkitTextLabelLanguagesP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelLanguagesP");
  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK( label );

  Stage::GetCurrent().Add( label );

  const std::string scripts( " привет мир, γειά σου Κόσμε, Hello world, مرحبا بالعالم, שלום עולם, "
                             "բարեւ աշխարհը, მსოფლიოში, 안녕하세요, 你好世界, ひらがな, カタカナ, "
                             "ওহে বিশ্ব, မင်္ဂလာပါကမ္ဘာလောက, हैलो वर्ल्ड, હેલો વર્લ્ડ, ਸਤਿ ਸ੍ਰੀ ਅਕਾਲ ਦੁਨਿਆ, ಹಲೋ ವರ್ಲ್ಡ್, "
                             "ഹലോ വേൾഡ്, ଓଡ଼ିଆ, හෙලෝ වර්ල්ඩ්, ஹலோ உலகம், హలో వరల్డ్, "
                             "ສະບາຍດີໂລກ, สวัสดีโลก, ជំរាបសួរពិភពលោក, "
                             "\xF0\x9F\x98\x81 \xF0\x9F\x98\x82 \xF0\x9F\x98\x83 \xF0\x9F\x98\x84." ); // these characters on the last line are emojis.

  label.SetProperty( TextLabel::Property::TEXT, scripts );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::TEXT ), scripts, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  END_TEST;
}
