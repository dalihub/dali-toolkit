/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-style-properties-devel.h>
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>

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

const char* const PROPERTY_NAME_RENDERING_BACKEND = "renderingBackend";
const char* const PROPERTY_NAME_TEXT = "text";
const char* const PROPERTY_NAME_FONT_FAMILY = "fontFamily";
const char* const PROPERTY_NAME_FONT_STYLE = "fontStyle";
const char* const PROPERTY_NAME_POINT_SIZE = "pointSize";
const char* const PROPERTY_NAME_MULTI_LINE =  "multiLine";
const char* const PROPERTY_NAME_HORIZONTAL_ALIGNMENT = "horizontalAlignment";
const char* const PROPERTY_NAME_VERTICAL_ALIGNMENT = "verticalAlignment";
const char* const PROPERTY_NAME_TEXT_COLOR = "textColor";
const char* const PROPERTY_NAME_ENABLE_MARKUP = "enableMarkup";
const char* const PROPERTY_NAME_ENABLE_AUTO_SCROLL = "enableAutoScroll";
const char* const PROPERTY_NAME_ENABLE_AUTO_SCROLL_SPEED = "autoScrollSpeed";
const char* const PROPERTY_NAME_ENABLE_AUTO_SCROLL_LOOPS = "autoScrollLoopCount";
const char* const PROPERTY_NAME_ENABLE_AUTO_SCROLL_GAP = "autoScrollGap";

const char* const PROPERTY_NAME_LINE_SPACING = "lineSpacing";
const char* const PROPERTY_NAME_UNDERLINE = "underline";
const char* const PROPERTY_NAME_SHADOW = "shadow";
const char* const PROPERTY_NAME_EMBOSS = "emboss";
const char* const PROPERTY_NAME_OUTLINE = "outline";
const char* const PROPERTY_NAME_BACKGROUND = "textBackground";

const char* const PROPERTY_NAME_PIXEL_SIZE = "pixelSize";
const char* const PROPERTY_NAME_ELLIPSIS = "ellipsis";
const char* const PROPERTY_NAME_AUTO_SCROLL_LOOP_DELAY = "autoScrollLoopDelay";

const std::string DEFAULT_FONT_DIR( "/resources/fonts" );
const unsigned int EMOJI_FONT_SIZE = 3840u; // 60 * 64

bool DaliTestCheckMaps( const Property::Map& mapGet, const Property::Map& mapSet, const std::vector<std::string>& indexConversionTable = std::vector<std::string>() )
{
  const Property::Map::SizeType size = mapGet.Count();

  if( size == mapSet.Count() )
  {
    for( unsigned int index = 0u; index < size; ++index )
    {
      const KeyValuePair& valueGet = mapGet.GetKeyValue( index );

      // Find the keys of the 'get' map
      Property::Index indexKey = valueGet.first.indexKey;
      std::string stringKey = valueGet.first.stringKey;

      if( !indexConversionTable.empty() )
      {
        if( stringKey.empty() )
        {
          stringKey = indexConversionTable[ indexKey ];
        }

        if( ( indexKey == Property::INVALID_INDEX ) && !stringKey.empty() )
        {
          Property::Index index = 0u;
          for( auto key : indexConversionTable )
          {
            if( key == stringKey )
            {
              indexKey = index;
              break;
            }
            ++index;
          }
        }
      }

      const Property::Value* const valueSet = mapSet.Find( indexKey, stringKey );

      if( nullptr != valueSet )
      {
        if( ( valueSet->GetType() == Dali::Property::STRING ) && ( valueGet.second.Get<std::string>() != valueSet->Get<std::string>() ) )
        {
          tet_printf( "Value got : [%s], expected : [%s]", valueGet.second.Get<std::string>().c_str(), valueSet->Get<std::string>().c_str() );
          return false;
        }
        else if( ( valueSet->GetType() == Dali::Property::BOOLEAN ) && ( valueGet.second.Get<bool>() != valueSet->Get<bool>() ) )
        {
          tet_printf( "Value got : [%d], expected : [%d]", valueGet.second.Get<bool>(), valueSet->Get<bool>() );
          return false;
        }
        else if( ( valueSet->GetType() == Dali::Property::INTEGER ) && ( valueGet.second.Get<int>() != valueSet->Get<int>() ) )
        {
          tet_printf( "Value got : [%d], expected : [%d]", valueGet.second.Get<int>(), valueSet->Get<int>() );
          return false;
        }
        else if( ( valueSet->GetType() == Dali::Property::FLOAT ) && ( valueGet.second.Get<float>() != valueSet->Get<float>() ) )
        {
          tet_printf( "Value got : [%f], expected : [%f]", valueGet.second.Get<float>(), valueSet->Get<float>() );
          return false;
        }
        else if( ( valueSet->GetType() == Dali::Property::VECTOR2 ) && ( valueGet.second.Get<Vector2>() != valueSet->Get<Vector2>() ) )
        {
          Vector2 vector2Get = valueGet.second.Get<Vector2>();
          Vector2 vector2Set = valueSet->Get<Vector2>();
          tet_printf( "Value got : [%f, %f], expected : [%f, %f]", vector2Get.x, vector2Get.y, vector2Set.x, vector2Set.y );
          return false;
        }
        else if( ( valueSet->GetType() == Dali::Property::VECTOR4 ) && ( valueGet.second.Get<Vector4>() != valueSet->Get<Vector4>() ) )
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
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_ENABLE_MARKUP) == TextLabel::Property::ENABLE_MARKUP );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_ENABLE_AUTO_SCROLL ) == TextLabel::Property::ENABLE_AUTO_SCROLL );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_ENABLE_AUTO_SCROLL_SPEED ) == TextLabel::Property::AUTO_SCROLL_SPEED );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_ENABLE_AUTO_SCROLL_LOOPS ) == TextLabel::Property::AUTO_SCROLL_LOOP_COUNT );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_ENABLE_AUTO_SCROLL_GAP ) == TextLabel::Property::AUTO_SCROLL_GAP );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_LINE_SPACING ) == TextLabel::Property::LINE_SPACING );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_UNDERLINE ) == TextLabel::Property::UNDERLINE );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_SHADOW ) == TextLabel::Property::SHADOW );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_EMBOSS ) == TextLabel::Property::EMBOSS );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_OUTLINE ) == TextLabel::Property::OUTLINE );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_BACKGROUND ) == DevelTextLabel::Property::BACKGROUND );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_PIXEL_SIZE ) == TextLabel::Property::PIXEL_SIZE );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_ELLIPSIS ) == TextLabel::Property::ELLIPSIS );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_AUTO_SCROLL_LOOP_DELAY ) == TextLabel::Property::AUTO_SCROLL_LOOP_DELAY );

  END_TEST;
}

int UtcDaliToolkitTextLabelSetPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelSetPropertyP");
  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK( label );

  Stage::GetCurrent().Add( label );

  // Note - we can't check the defaults since the stylesheets are platform-specific
  label.SetProperty( TextLabel::Property::RENDERING_BACKEND, Text::RENDERING_SHARED_ATLAS );
  DALI_TEST_EQUALS( (Text::RenderingType)label.GetProperty<int>( TextLabel::Property::RENDERING_BACKEND ), Text::RENDERING_SHARED_ATLAS, TEST_LOCATION );

  // Check that text can be correctly reset
  label.SetProperty( TextLabel::Property::TEXT, "Setting Text" );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::TEXT ), std::string("Setting Text"), TEST_LOCATION );

  // Check font properties.
  label.SetProperty( TextLabel::Property::FONT_FAMILY, "Setting font family" );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::FONT_FAMILY ), std::string("Setting font family"), TEST_LOCATION );

  Property::Map fontStyleMapSet;
  Property::Map fontStyleMapGet;

  fontStyleMapSet.Insert( "weight", "bold" );
  fontStyleMapSet.Insert( "width", "condensed" );
  fontStyleMapSet.Insert( "slant", "italic" );
  label.SetProperty( TextLabel::Property::FONT_STYLE, fontStyleMapSet );

  fontStyleMapGet = label.GetProperty<Property::Map>( TextLabel::Property::FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  // Check the old font style format.
  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "weight", "thin" );
  fontStyleMapSet.Insert( "width", "expanded" );
  fontStyleMapSet.Insert( "slant", "oblique" );
  const std::string strFontStyle = "{\"weight\":\"thin\",\"width\":\"expanded\",\"slant\":\"oblique\"}";

  label.SetProperty( TextLabel::Property::FONT_STYLE, "{\"weight\":\"thin\",\"width\":\"expanded\",\"slant\":\"oblique\"}" );
  std::string getFontStyle = label.GetProperty<std::string>( TextLabel::Property::FONT_STYLE );
  DALI_TEST_EQUALS( getFontStyle, strFontStyle, TEST_LOCATION );

  label.SetProperty( TextLabel::Property::POINT_SIZE, 10.f );
  DALI_TEST_EQUALS( label.GetProperty<float>( TextLabel::Property::POINT_SIZE ), 10.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Reset font style.
  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "weight", "normal" );
  fontStyleMapSet.Insert( "slant", "oblique" );

  label.SetProperty( TextLabel::Property::FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = label.GetProperty<Property::Map>( TextLabel::Property::FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  fontStyleMapSet.Clear();
  fontStyleMapSet.Insert( "slant", "roman" );

  label.SetProperty( TextLabel::Property::FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = label.GetProperty<Property::Map>( TextLabel::Property::FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );

  // Replace 'roman' for 'normal'.
  Property::Value* slantValue = fontStyleMapGet.Find( "slant" );
  if( NULL != slantValue )
  {
    if( "normal" == slantValue->Get<std::string>() )
    {
      fontStyleMapGet["slant"] = "roman";
    }
  }
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

  fontStyleMapSet.Clear();

  label.SetProperty( TextLabel::Property::FONT_STYLE, fontStyleMapSet );
  fontStyleMapGet = label.GetProperty<Property::Map>( TextLabel::Property::FONT_STYLE );
  DALI_TEST_EQUALS( fontStyleMapGet.Count(), fontStyleMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( fontStyleMapGet, fontStyleMapSet ), true, TEST_LOCATION );

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

  Property::Map underlineMapSet;
  Property::Map underlineMapGet;

  underlineMapSet.Insert( "enable", false );
  underlineMapSet.Insert( "color", Color::BLUE );
  underlineMapSet.Insert( "height", 0 );

  underlineMapGet = label.GetProperty<Property::Map>( TextLabel::Property::UNDERLINE );
  DALI_TEST_EQUALS( underlineMapGet.Count(), underlineMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( underlineMapGet, underlineMapSet ), true, TEST_LOCATION );

  TextLabel label2 = TextLabel::New( "New text" );
  DALI_TEST_CHECK( label2 );
  DALI_TEST_EQUALS( label2.GetProperty<std::string>( TextLabel::Property::TEXT ), std::string("New text"), TEST_LOCATION );

  // Check the enable markup property.
  DALI_TEST_CHECK( !label.GetProperty<bool>( TextLabel::Property::ENABLE_MARKUP ) );
  label.SetProperty( TextLabel::Property::ENABLE_MARKUP, true );
  DALI_TEST_CHECK( label.GetProperty<bool>( TextLabel::Property::ENABLE_MARKUP ) );

  // Check the text property when markup is enabled
  label.SetProperty( TextLabel::Property::TEXT, "<color value='white'>Markup</color><color value='cyan'>Text</color>" );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::TEXT ), std::string("MarkupText"), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  // Check autoscroll properties
  const int SCROLL_SPEED = 80;
  const int SCROLL_LOOPS = 4;
  const float SCROLL_GAP = 50.0f;
  const float SCROLL_LOOP_DELAY = 0.3f;
  const std::string STOP_IMMEDIATE = std::string( "IMMEDIATE" );
  const std::string STOP_FINISH_LOOP = std::string( "FINISH_LOOP" );

  label.SetProperty( TextLabel::Property::MULTI_LINE, false ); // Autoscroll only supported in single line
  DALI_TEST_CHECK( !label.GetProperty<bool>( TextLabel::Property::ENABLE_AUTO_SCROLL ) );
  label.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true );
  DALI_TEST_CHECK( label.GetProperty<bool>( TextLabel::Property::ENABLE_AUTO_SCROLL ) );
  label.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, SCROLL_SPEED );
  DALI_TEST_EQUALS( SCROLL_SPEED, label.GetProperty<int>( TextLabel::Property::AUTO_SCROLL_SPEED ), TEST_LOCATION );
  label.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, SCROLL_LOOPS );
  DALI_TEST_EQUALS( SCROLL_LOOPS, label.GetProperty<int>( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT ), TEST_LOCATION );
  label.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, SCROLL_GAP );
  DALI_TEST_EQUALS( SCROLL_GAP, label.GetProperty<float>( TextLabel::Property::AUTO_SCROLL_GAP ), TEST_LOCATION );
  label.SetProperty(TextLabel::Property::AUTO_SCROLL_LOOP_DELAY, SCROLL_LOOP_DELAY );
  DALI_TEST_EQUALS( SCROLL_LOOP_DELAY, label.GetProperty<float>( TextLabel::Property::AUTO_SCROLL_LOOP_DELAY ), TEST_LOCATION );

  //Check autoscroll stop type property
  label.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE );
  DALI_TEST_EQUALS( STOP_IMMEDIATE, label.GetProperty<std::string>( TextLabel::Property::AUTO_SCROLL_STOP_MODE ), TEST_LOCATION );

  label.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::FINISH_LOOP );
  DALI_TEST_EQUALS( STOP_FINISH_LOOP, label.GetProperty<std::string>( TextLabel::Property::AUTO_SCROLL_STOP_MODE ), TEST_LOCATION );

  // test natural size with multi-line and line spacing
  {
    TextLabel label3 = TextLabel::New("Some text here\nend there\nend here");
    Vector3 oneLineNaturalSize = label3.GetNaturalSize();
    label3.SetProperty(TextLabel::Property::MULTI_LINE, true);
    label3.SetProperty(TextLabel::Property::LINE_SPACING, 0);
    Vector3 multiLineNaturalSize = label3.GetNaturalSize();

    // The width of the text when multi-line is enabled will be smaller (lines separated on '\n')
    // The height of the text when multi-line is enabled will be larger
    DALI_TEST_CHECK( oneLineNaturalSize.width > multiLineNaturalSize.width );
    DALI_TEST_CHECK( oneLineNaturalSize.height < multiLineNaturalSize.height );

    // Change line spacing, meaning height will increase by 3 times the amount specified as we have three lines
    // Everything else will remain the same
    int lineSpacing = 20;
    label3.SetProperty( TextLabel::Property::LINE_SPACING, lineSpacing );
    Vector3 expectedAfterLineSpacingApplied( multiLineNaturalSize );
    expectedAfterLineSpacingApplied.height += 3 * lineSpacing;
    DALI_TEST_EQUALS( expectedAfterLineSpacingApplied, label3.GetNaturalSize(), TEST_LOCATION );
  }

  // single line, line spacing must not affect natural size of the text, only add the spacing to the height
  {
    TextLabel label3 = TextLabel::New("Some text here end there end here");
    label3.SetProperty(TextLabel::Property::MULTI_LINE, false);
    label3.SetProperty(TextLabel::Property::LINE_SPACING, 0);
    Vector3 textNaturalSize = label3.GetNaturalSize();
    int lineSpacing = 20;
    label3.SetProperty( TextLabel::Property::LINE_SPACING, lineSpacing );
    Vector3 expectedNaturalSizeWithLineSpacing( textNaturalSize );
    expectedNaturalSizeWithLineSpacing.height += lineSpacing;
    DALI_TEST_EQUALS( expectedNaturalSizeWithLineSpacing, label3.GetNaturalSize(), TEST_LOCATION );
  }
  // Check the line spacing property
  DALI_TEST_EQUALS( label.GetProperty<float>( TextLabel::Property::LINE_SPACING ), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  label.SetProperty( TextLabel::Property::LINE_SPACING, 10.f );
  DALI_TEST_EQUALS( label.GetProperty<float>( TextLabel::Property::LINE_SPACING ), 10.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Check the underline property
  underlineMapSet.Clear();
  underlineMapSet.Insert( "enable", true );
  underlineMapSet.Insert( "color", Color::RED );
  underlineMapSet.Insert( "height", 1 );

  label.SetProperty( TextLabel::Property::UNDERLINE, underlineMapSet );

  application.SendNotification();
  application.Render();

  underlineMapGet = label.GetProperty<Property::Map>( TextLabel::Property::UNDERLINE );
  DALI_TEST_EQUALS( underlineMapGet.Count(), underlineMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( underlineMapGet, underlineMapSet ), true, TEST_LOCATION );

  underlineMapSet.Clear();
  underlineMapSet.Insert( Toolkit::DevelText::Underline::Property::ENABLE, true );
  underlineMapSet.Insert( Toolkit::DevelText::Underline::Property::COLOR, Color::GREEN );
  underlineMapSet.Insert( Toolkit::DevelText::Underline::Property::HEIGHT, 2 );

  label.SetProperty( TextLabel::Property::UNDERLINE, underlineMapSet );

  application.SendNotification();
  application.Render();

  underlineMapGet = label.GetProperty<Property::Map>( TextLabel::Property::UNDERLINE );
  DALI_TEST_EQUALS( underlineMapGet.Count(), underlineMapSet.Count(), TEST_LOCATION );
  std::vector<std::string> underlineIndicesConversionTable = { "enable", "color", "height" };
  DALI_TEST_EQUALS( DaliTestCheckMaps( underlineMapGet, underlineMapSet, underlineIndicesConversionTable ), true, TEST_LOCATION );

  underlineMapSet.Clear();

  Property::Map underlineDisabledMapGet;
  underlineDisabledMapGet.Insert( "enable", false );
  underlineDisabledMapGet.Insert( "color", Color::GREEN );
  underlineDisabledMapGet.Insert( "height", 2 );

  label.SetProperty( TextLabel::Property::UNDERLINE, underlineMapSet );

  application.SendNotification();
  application.Render();

  underlineMapGet = label.GetProperty<Property::Map>( TextLabel::Property::UNDERLINE );
  DALI_TEST_EQUALS( underlineMapGet.Count(), underlineDisabledMapGet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( underlineMapGet, underlineDisabledMapGet ), true, TEST_LOCATION );

  // Check the shadow property

  Property::Map shadowMapSet;
  Property::Map shadowMapGet;

  shadowMapSet.Insert( "color", Color::GREEN );
  shadowMapSet.Insert( "offset", Vector2(2.0f, 2.0f) );
  shadowMapSet.Insert( "blurRadius", 5.0f );

  label.SetProperty( TextLabel::Property::SHADOW, shadowMapSet );

  shadowMapGet = label.GetProperty<Property::Map>( TextLabel::Property::SHADOW );
  DALI_TEST_EQUALS( shadowMapGet.Count(), shadowMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( shadowMapGet, shadowMapSet ), true, TEST_LOCATION );

  shadowMapSet.Clear();

  shadowMapSet.Insert( Toolkit::DevelText::Shadow::Property::COLOR, Color::BLUE );
  shadowMapSet.Insert( Toolkit::DevelText::Shadow::Property::OFFSET, "3.0 3.0" );
  shadowMapSet.Insert( Toolkit::DevelText::Shadow::Property::BLUR_RADIUS, 3.0f );

  label.SetProperty( TextLabel::Property::SHADOW, shadowMapSet );

  // Replace the offset (string) by a vector2
  shadowMapSet.Clear();
  shadowMapSet.Insert( Toolkit::DevelText::Shadow::Property::COLOR, Color::BLUE );
  shadowMapSet.Insert( Toolkit::DevelText::Shadow::Property::OFFSET, Vector2( 3.0, 3.0 ) );
  shadowMapSet.Insert( Toolkit::DevelText::Shadow::Property::BLUR_RADIUS, 3.0f );

  shadowMapGet = label.GetProperty<Property::Map>( TextLabel::Property::SHADOW );
  DALI_TEST_EQUALS( shadowMapGet.Count(), shadowMapSet.Count(), TEST_LOCATION );
  std::vector<std::string> shadowIndicesConversionTable = { "color", "offset", "blurRadius" };
  DALI_TEST_EQUALS( DaliTestCheckMaps( shadowMapGet, shadowMapSet, shadowIndicesConversionTable ), true, TEST_LOCATION );

  shadowMapSet.Clear();
  Property::Map shadowDisabledMapGet;
  shadowDisabledMapGet.Insert( "color", Color::BLUE );
  shadowDisabledMapGet.Insert( "offset", Vector2(0.0f, 0.0f) );
  shadowDisabledMapGet.Insert( "blurRadius", 3.0f );

  label.SetProperty( TextLabel::Property::SHADOW, shadowMapSet );

  shadowMapGet = label.GetProperty<Property::Map>( TextLabel::Property::SHADOW );
  DALI_TEST_EQUALS( shadowMapGet.Count(), shadowDisabledMapGet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( shadowMapGet, shadowDisabledMapGet ), true, TEST_LOCATION );

  // Check the emboss property
  label.SetProperty( TextLabel::Property::EMBOSS, "Emboss properties" );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::EMBOSS ), std::string("Emboss properties"), TEST_LOCATION );

  // Check the outline property

  // Test string type first
  // This is purely to maintain backward compatibility, but we don't support string as the outline property type.
  label.SetProperty( TextLabel::Property::OUTLINE, "Outline properties" );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::OUTLINE ), std::string("Outline properties"), TEST_LOCATION );

  // Then test the property map type
  Property::Map outlineMapSet;
  Property::Map outlineMapGet;

  outlineMapSet["color"] = Color::RED;
  outlineMapSet["width"] = 2.0f;
  label.SetProperty( TextLabel::Property::OUTLINE, outlineMapSet );

  outlineMapGet = label.GetProperty<Property::Map>( TextLabel::Property::OUTLINE );
  DALI_TEST_EQUALS( outlineMapGet.Count(), outlineMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( outlineMapGet, outlineMapSet ), true, TEST_LOCATION );

  outlineMapSet.Clear();
  outlineMapSet[Toolkit::DevelText::Outline::Property::COLOR] = Color::BLUE;
  outlineMapSet[Toolkit::DevelText::Outline::Property::WIDTH] = 3.0f;
  label.SetProperty( TextLabel::Property::OUTLINE, outlineMapSet );

  outlineMapGet = label.GetProperty<Property::Map>( TextLabel::Property::OUTLINE );
  DALI_TEST_EQUALS( outlineMapGet.Count(), outlineMapSet.Count(), TEST_LOCATION );
  std::vector<std::string> outlineIndicesConversionTable = { "color", "width" };
  DALI_TEST_EQUALS( DaliTestCheckMaps( outlineMapGet, outlineMapSet, outlineIndicesConversionTable ), true, TEST_LOCATION );

  // Check the background property
  Property::Map backgroundMapSet;
  Property::Map backgroundMapGet;

  backgroundMapSet["enable"] = true;
  backgroundMapSet["color"] = Color::RED;
  label.SetProperty( DevelTextLabel::Property::BACKGROUND, backgroundMapSet );

  backgroundMapGet = label.GetProperty<Property::Map>( DevelTextLabel::Property::BACKGROUND );
  DALI_TEST_EQUALS( backgroundMapGet.Count(), backgroundMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( backgroundMapGet, backgroundMapSet ), true, TEST_LOCATION );

  backgroundMapSet.Clear();
  backgroundMapSet[Toolkit::DevelText::Background::Property::ENABLE] = true;
  backgroundMapSet[Toolkit::DevelText::Background::Property::COLOR] = Color::GREEN;
  label.SetProperty( DevelTextLabel::Property::BACKGROUND, backgroundMapSet );

  backgroundMapGet = label.GetProperty<Property::Map>( DevelTextLabel::Property::BACKGROUND );
  DALI_TEST_EQUALS( backgroundMapGet.Count(), backgroundMapSet.Count(), TEST_LOCATION );
  std::vector<std::string> backgroundIndicesConversionTable = { "enable", "color" };
  DALI_TEST_EQUALS( DaliTestCheckMaps( backgroundMapGet, backgroundMapSet, backgroundIndicesConversionTable ), true, TEST_LOCATION );

  // Check the pixel size of font
  label.SetProperty( TextLabel::Property::PIXEL_SIZE, 20.f );
  DALI_TEST_EQUALS( label.GetProperty<float>( TextLabel::Property::PIXEL_SIZE ), 20.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Check the ellipsis property
  DALI_TEST_CHECK( label.GetProperty<bool>( TextLabel::Property::ELLIPSIS ) );
  label.SetProperty( TextLabel::Property::ELLIPSIS, false );
  DALI_TEST_CHECK( !label.GetProperty<bool>( TextLabel::Property::ELLIPSIS ) );

  // Check the layout direction property
  label.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT );
  DALI_TEST_EQUALS( label.GetProperty< int >( Actor::Property::LAYOUT_DIRECTION ), static_cast< int >( LayoutDirection::RIGHT_TO_LEFT ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

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

  Property::Map underlineMap;
  underlineMap.Insert( "enable", true );
  underlineMap.Insert( "color", Color::RED );
  label.SetProperty( TextLabel::Property::UNDERLINE, underlineMap );

  Property::Map shadowMap;
  shadowMap.Insert( "color", Color::BLUE );
  shadowMap.Insert( "offset", Vector2( 1.0f, 1.0f ) );
  label.SetProperty( TextLabel::Property::SHADOW, shadowMap );

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

  try
  {
    // Render some text with the shared atlas backend
    label.SetProperty( TextLabel::Property::RENDERING_BACKEND, Text::RENDERING_VECTOR_BASED );
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

int UtcDaliToolkitTextLabelEmojisP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelLanguagesP");
  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK( label );

  Stage::GetCurrent().Add( label );

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  char* pathNamePtr = get_current_dir_name();
  const std::string pathName( pathNamePtr );
  free( pathNamePtr );

  TextAbstraction::FontDescription fontDescription;
  fontDescription.path = pathName + DEFAULT_FONT_DIR + "/tizen/BreezeColorEmoji.ttf";
  fontDescription.family = "BreezeColorEmoji";
  fontDescription.width = TextAbstraction::FontWidth::NONE;
  fontDescription.weight = TextAbstraction::FontWeight::NORMAL;
  fontDescription.slant = TextAbstraction::FontSlant::NONE;

  fontClient.GetFontId( fontDescription, EMOJI_FONT_SIZE );

  const std::string emojis = "<font family='BreezeColorEmoji' size='60'>\xF0\x9F\x98\x81 \xF0\x9F\x98\x82 \xF0\x9F\x98\x83 \xF0\x9F\x98\x84</font>";
  label.SetProperty( TextLabel::Property::ENABLE_MARKUP, true );
  label.SetProperty( TextLabel::Property::TEXT, emojis );

  Property::Map shadowMap;
  shadowMap.Insert( "color", "green" );
  shadowMap.Insert( "offset", "2 2" );
  label.SetProperty( TextLabel::Property::SHADOW, shadowMap );

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextlabelScrollingP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelScrollingP");
  TextLabel labelImmediate = TextLabel::New("Some text to scroll");
  TextLabel labelFinished = TextLabel::New("مرحبا بالعالم");

  DALI_TEST_CHECK( labelImmediate );
  DALI_TEST_CHECK( labelFinished );
  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  Stage::GetCurrent().Add( labelImmediate );
  // Turn on all the effects
  labelImmediate.SetProperty( TextLabel::Property::MULTI_LINE, false );
  labelImmediate.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  labelImmediate.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  labelImmediate.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );
  labelImmediate.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE );

  Stage::GetCurrent().Add( labelFinished );
  // Turn on all the effects
  labelFinished.SetProperty( TextLabel::Property::MULTI_LINE, false );
  labelFinished.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  labelFinished.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  labelFinished.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );
  labelFinished.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::FINISH_LOOP );



  try
  {
    // Render some text with the shared atlas backend
    labelImmediate.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true );
    labelFinished.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true );

    labelImmediate.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false );
    labelFinished.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false );

    labelImmediate.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true );
    labelFinished.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true );
    application.SendNotification();
    application.Render();

    labelImmediate.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false );
    labelFinished.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false );
    application.SendNotification();
    application.Render();

  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliToolkitTextlabelScrollingCenterAlignP(void)
{
  ToolkitTestApplication application;
  TextLabel labelShort = TextLabel::New("Some text to scroll");
  TextLabel labelLong = TextLabel::New("Some text to scroll that is greater than the width of the text. The quick brown fox jumps over the lazy dog. Hello World, we meet again!");

  DALI_TEST_CHECK( labelShort );
  DALI_TEST_CHECK( labelLong );
  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  Stage::GetCurrent().Add( labelShort );
  // Turn on all the effects
  labelShort.SetProperty( TextLabel::Property::MULTI_LINE, false );
  labelShort.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE );

  Stage::GetCurrent().Add( labelLong );
  // Turn on all the effects
  labelLong.SetProperty( TextLabel::Property::MULTI_LINE, false );
  labelLong.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  labelLong.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  labelLong.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  labelLong.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );
  labelLong.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::FINISH_LOOP );

  try
  {
    // Render some text with the shared atlas backend
    labelShort.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true );
    labelLong.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true );
    application.SendNotification();
    application.Render();

    labelShort.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false );
    labelLong.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false );
    application.SendNotification();
    application.Render();

  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliToolkitTextlabelScrollingCenterAlignRTLP(void)
{
  ToolkitTestApplication application;
  TextLabel labelShort = TextLabel::New("مرحبا بالعالم");
  TextLabel labelLong = TextLabel::New("لكن لا بد أن أوضح لك أن كل هذه الأفكار المغلوطة حول استنكار  النشوة وتمجيد الألم نشأت بالفعل، وسأعرض لك التفاصيل لتكتشف حقيقة وأساس تلك السعادة البشرية، فلا أحد يرفض أو يكره أو يتجنب الشعور بالسعادة، ولكن بفضل هؤلاء الأشخاص الذين لا يدركون بأن السعادة لا بد أن نستشعرها بصورة أكثر عقلانية ومنطقية فيعرضهم هذا لمواجهة الظروف الأليمة، وأكرر بأنه لا يوجد من يرغب في الحب ونيل المنال ويتلذذ بالآلام، الألم هو الألم ولكن نتيجة لظروف ما قد تكمن السعاده فيما نتحمله من كد وأسي");

  DALI_TEST_CHECK( labelShort );
  DALI_TEST_CHECK( labelLong );
  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  Stage::GetCurrent().Add( labelShort );
  // Turn on all the effects
  labelShort.SetProperty( TextLabel::Property::MULTI_LINE, false );
  labelShort.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE );

  Stage::GetCurrent().Add( labelLong );
  // Turn on all the effects
  labelLong.SetProperty( TextLabel::Property::MULTI_LINE, false );
  labelLong.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  labelLong.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  labelLong.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  labelLong.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );
  labelLong.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::FINISH_LOOP );

  try
  {
    // Render some text with the shared atlas backend
    labelShort.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true );
    labelLong.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true );
    application.SendNotification();
    application.Render();

    labelShort.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false );
    labelLong.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false );
    application.SendNotification();
    application.Render();

  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliToolkitTextlabelScrollingEndAlignP(void)
{
  ToolkitTestApplication application;
  TextLabel labelShort = TextLabel::New("Some text to scroll");
  TextLabel labelLong = TextLabel::New("Some text to scroll that is greater than the width of the text. The quick brown fox jumps over the lazy dog. Hello World, we meet again!");

  DALI_TEST_CHECK( labelShort );
  DALI_TEST_CHECK( labelLong );
  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  Stage::GetCurrent().Add( labelShort );
  // Turn on all the effects
  labelShort.SetProperty( TextLabel::Property::MULTI_LINE, false );
  labelShort.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "END" );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE );

  Stage::GetCurrent().Add( labelLong );
  // Turn on all the effects
  labelLong.SetProperty( TextLabel::Property::MULTI_LINE, false );
  labelLong.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "END" );
  labelLong.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  labelLong.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  labelLong.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );
  labelLong.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::FINISH_LOOP );

  try
  {
    // Render some text with the shared atlas backend
    labelShort.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true );
    labelLong.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true );
    application.SendNotification();
    application.Render();

    labelShort.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false );
    labelLong.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false );
    application.SendNotification();
    application.Render();

  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliToolkitTextlabelScrollingEndAlignRTLP(void)
{
  ToolkitTestApplication application;
  TextLabel labelShort = TextLabel::New("مرحبا بالعالم");
  TextLabel labelLong = TextLabel::New("لكن لا بد أن أوضح لك أن كل هذه الأفكار المغلوطة حول استنكار  النشوة وتمجيد الألم نشأت بالفعل، وسأعرض لك التفاصيل لتكتشف حقيقة وأساس تلك السعادة البشرية، فلا أحد يرفض أو يكره أو يتجنب الشعور بالسعادة، ولكن بفضل هؤلاء الأشخاص الذين لا يدركون بأن السعادة لا بد أن نستشعرها بصورة أكثر عقلانية ومنطقية فيعرضهم هذا لمواجهة الظروف الأليمة، وأكرر بأنه لا يوجد من يرغب في الحب ونيل المنال ويتلذذ بالآلام، الألم هو الألم ولكن نتيجة لظروف ما قد تكمن السعاده فيما نتحمله من كد وأسي");

  DALI_TEST_CHECK( labelShort );
  DALI_TEST_CHECK( labelLong );
  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  Stage::GetCurrent().Add( labelShort );
  // Turn on all the effects
  labelShort.SetProperty( TextLabel::Property::MULTI_LINE, false );
  labelShort.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "END" );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE );

  Stage::GetCurrent().Add( labelLong );
  // Turn on all the effects
  labelLong.SetProperty( TextLabel::Property::MULTI_LINE, false );
  labelLong.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "END" );
  labelLong.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  labelLong.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  labelLong.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );
  labelLong.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::FINISH_LOOP );

  try
  {
    // Render some text with the shared atlas backend
    labelShort.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true );
    labelLong.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true );
    application.SendNotification();
    application.Render();

    labelShort.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false );
    labelLong.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false );
    application.SendNotification();
    application.Render();

  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliToolkitTextlabelScrollingInterruptedP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelScrollingInterruptedP");
  TextLabel label = TextLabel::New("Some text to scroll");
  DALI_TEST_CHECK( label );
  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  Stage::GetCurrent().Add( label );
  label.SetSize( 360.0f, 20.f );
  // Turn on all the effects
  label.SetProperty( TextLabel::Property::MULTI_LINE, false );
  label.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  label.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  label.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );

  // Render the text.
  application.SendNotification();
  application.Render();

  unsigned int actorCount1 = label.GetChildCount();
  tet_printf("Initial actor count is(%d)\n", actorCount1 );

  try
  {
    // Render some text with the shared atlas backend
    label.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true );
    application.SendNotification();
    application.Render(2000);

    unsigned int actorCount1 = label.GetChildCount();
    tet_printf("Actor count after scrolling is(%d)\n", actorCount1 );

    label.SetProperty( TextLabel::Property::TEXT_COLOR, Color::RED );

    // Render the text.
    application.SendNotification();
    application.Render();

    unsigned int actorCount2 = label.GetChildCount();
    tet_printf("After changing color the actor count is(%d)\n", actorCount2 );

    DALI_TEST_EQUALS( actorCount1, actorCount2, TEST_LOCATION );

  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliToolkitTextlabelScrollingN(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelScrollingN");

  TextLabel label = TextLabel::New("Some text to scroll");
  DALI_TEST_CHECK( label );

  Stage::GetCurrent().Add( label );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // The text scrolling works only on single line text.
  label.SetProperty( TextLabel::Property::MULTI_LINE, true );

  // Turn on all the effects.
  label.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  label.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  label.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );

  // Enable the auto scrolling effect.
  label.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true );

  // The auto scrolling shouldn't be enabled.
  const bool enabled = label.GetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL ).Get<bool>();
  DALI_TEST_CHECK( !enabled );

  END_TEST;
}

int UtcDaliToolkitTextlabelEllipsis(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelEllipsis");

  TextLabel label = TextLabel::New("Hello world");
  DALI_TEST_CHECK( label );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  Stage::GetCurrent().Add( label );

  // Turn on all the effects
  label.SetAnchorPoint( AnchorPoint::CENTER );
  label.SetParentOrigin( ParentOrigin::CENTER );
  label.SetSize( 360.0f, 10.f );

  try
  {
    // Render the text.
    application.SendNotification();
    application.Render();
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  label.SetProperty( TextLabel::Property::TEXT, "Hello world                                        " );
  label.SetProperty( DevelTextLabel::Property::IGNORE_SPACES_AFTER_TEXT, false );
  label.SetSize( 400.0f, 10.f );

  try
  {
    // Render the text.
    application.SendNotification();
    application.Render();
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }


  label.SetProperty( TextLabel::Property::TEXT, "Hello world" );
  label.SetProperty( DevelTextLabel::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION, true );
  label.SetSize( 400.0f, 10.f );

  try
  {
    // Render the text.
    application.SendNotification();
    application.Render();
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliToolkitTextlabelTextWrapMode(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelTextWarpMode");

  int lineCount =0 ;

  TextLabel label = TextLabel::New();
  label.SetSize( 300.0f, 300.f );
  label.SetProperty( TextLabel::Property::TEXT, "Hello world Hello world" );
  label.SetProperty( TextLabel::Property::MULTI_LINE, true );



  //label.SetProperty( TextLabel::Property::POINT_SIZE, 18 );
  Stage::GetCurrent().Add( label );

  label.SetProperty( TextLabel::Property::LINE_WRAP_MODE, "WORD" );
  DALI_TEST_EQUALS( label.GetProperty< int >( TextLabel::Property::LINE_WRAP_MODE ), static_cast< int >( Text::LineWrap::WORD ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  lineCount =  label.GetProperty<int>( TextLabel::Property::LINE_COUNT );
  DALI_TEST_EQUALS( lineCount, 2, TEST_LOCATION );

  label.SetProperty( TextLabel::Property::LINE_WRAP_MODE, "CHARACTER" );
  DALI_TEST_EQUALS( label.GetProperty< int >( TextLabel::Property::LINE_WRAP_MODE ), static_cast< int >( Text::LineWrap::CHARACTER ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  label.SetProperty( TextLabel::Property::LINE_WRAP_MODE, Text::LineWrap::WORD );
  DALI_TEST_EQUALS( label.GetProperty< int >( TextLabel::Property::LINE_WRAP_MODE ), static_cast< int >( Text::LineWrap::WORD ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  lineCount =  label.GetProperty<int>( TextLabel::Property::LINE_COUNT );
  DALI_TEST_EQUALS( lineCount, 2, TEST_LOCATION );

  label.SetProperty( TextLabel::Property::LINE_WRAP_MODE, Text::LineWrap::CHARACTER );
  DALI_TEST_EQUALS( label.GetProperty< int >( TextLabel::Property::LINE_WRAP_MODE ), static_cast< int >( Text::LineWrap::CHARACTER ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  lineCount =  label.GetProperty<int>( TextLabel::Property::LINE_COUNT );
  DALI_TEST_EQUALS( lineCount, 2, TEST_LOCATION );

  tet_infoline( "Ensure invalid string does not change wrapping mode" );
  label.SetProperty( TextLabel::Property::LINE_WRAP_MODE, "InvalidWrapMode" );
  DALI_TEST_EQUALS( label.GetProperty< int >( TextLabel::Property::LINE_WRAP_MODE ), static_cast< int >( Text::LineWrap::CHARACTER ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitTextLabelColorComponents(void)
{
  ToolkitTestApplication application;

  TextLabel label = TextLabel::New();
  label.SetProperty( TextLabel::Property::TEXT_COLOR, Color::RED );
  DALI_TEST_EQUALS( label.GetProperty< float >( TextLabel::Property::TEXT_COLOR_RED ),   1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty< float >( TextLabel::Property::TEXT_COLOR_GREEN ), 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty< float >( TextLabel::Property::TEXT_COLOR_BLUE ),  0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty< float >( TextLabel::Property::TEXT_COLOR_ALPHA ), 1.0f, TEST_LOCATION );

  label.SetProperty( TextLabel::Property::TEXT_COLOR, Color::GREEN );
  DALI_TEST_EQUALS( label.GetProperty< float >( TextLabel::Property::TEXT_COLOR_RED ),   0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty< float >( TextLabel::Property::TEXT_COLOR_GREEN ), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty< float >( TextLabel::Property::TEXT_COLOR_BLUE ),  0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty< float >( TextLabel::Property::TEXT_COLOR_ALPHA ), 1.0f, TEST_LOCATION );

  label.SetProperty( TextLabel::Property::TEXT_COLOR, Color::BLUE );
  DALI_TEST_EQUALS( label.GetProperty< float >( TextLabel::Property::TEXT_COLOR_RED ),   0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty< float >( TextLabel::Property::TEXT_COLOR_GREEN ), 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty< float >( TextLabel::Property::TEXT_COLOR_BLUE ),  1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty< float >( TextLabel::Property::TEXT_COLOR_ALPHA ), 1.0f, TEST_LOCATION );

  label.SetProperty( TextLabel::Property::TEXT_COLOR_ALPHA, 0.6f );
  DALI_TEST_EQUALS( label.GetProperty< float >( TextLabel::Property::TEXT_COLOR_ALPHA ), 0.6f, TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty< Vector4 >( TextLabel::Property::TEXT_COLOR ), Vector4( 0.0f, 0.0f, 1.0f, 0.6f ), TEST_LOCATION );
  DALI_TEST_EQUALS( label.GetProperty< Vector4 >( TextLabel::Property::UNUSED_PROPERTY_TEXT_COLOR ), Vector4( 0.0f, 0.0f, 1.0f, 0.6f ), TEST_LOCATION );

  // Test a transparent text - Rendering should be skipped.
  label.SetProperty( TextLabel::Property::TEXT, "Hello world Hello world" );
  label.SetProperty( TextLabel::Property::TEXT_COLOR, Color::BLUE );

  Stage::GetCurrent().Add( label );

  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable( true );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( drawTrace.FindMethod( "DrawArrays" ), true, TEST_LOCATION );  // Should be rendered

  label.SetProperty( TextLabel::Property::TEXT_COLOR, Color::TRANSPARENT );

  drawTrace.Reset();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( drawTrace.FindMethod( "DrawArrays" ), false, TEST_LOCATION ); // Rendering should be skipped

  label.SetProperty( TextLabel::Property::TEXT_COLOR, Color::RED );

  drawTrace.Reset();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( drawTrace.FindMethod( "DrawArrays" ), true, TEST_LOCATION ); // Should be rendered again

  END_TEST;
}

int UtcDaliToolkitTextlabelTextStyle01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelTextStyle Setting Outline after Shadow");

  TextLabel label = TextLabel::New();
  label.SetSize( 300.0f, 300.f );
  label.SetProperty( TextLabel::Property::TEXT, "Hello world Hello world" );
  label.SetProperty( TextLabel::Property::POINT_SIZE, 12 );
  Stage::GetCurrent().Add( label );

  Property::Map outlineMapSet;
  Property::Map outlineMapGet;

  outlineMapSet["color"] = Color::BLUE;
  outlineMapSet["width"] = 2.0f;
  label.SetProperty( TextLabel::Property::OUTLINE, outlineMapSet );

  application.SendNotification();
  application.Render();

  Property::Map shadowMapSet;
  shadowMapSet.Insert( "color", "green" );
  shadowMapSet.Insert( "offset", "2 2" );
  shadowMapSet.Insert( "blurRadius", "3" );
  label.SetProperty( TextLabel::Property::SHADOW, shadowMapSet );

  outlineMapSet["color"] = Color::RED;
  outlineMapSet["width"] = 0.0f;
  label.SetProperty( TextLabel::Property::OUTLINE, outlineMapSet );

  application.SendNotification();
  application.Render();

  outlineMapGet = label.GetProperty<Property::Map>( TextLabel::Property::OUTLINE );

  Property::Value* colorValue = outlineMapGet.Find("color");

  bool colorMatched( false );

  if ( colorValue )
  {
     Property::Type valueType = colorValue->GetType();

     if ( Property::STRING == valueType )
     {
       std::string stringValue;
       colorValue->Get( stringValue );
       if (  stringValue == "red" )
       {
         colorMatched = true;
       }
     }
     else if ( Property::VECTOR4 == valueType )
     {
       Vector4 colorVector4;
       colorValue->Get( colorVector4 );
       if (  colorVector4 == Color::RED )
       {
         colorMatched = true;
       }
     }
  }

  DALI_TEST_EQUALS( colorMatched, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitTextlabelMultiline(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelMultiline");

  TextLabel label = TextLabel::New();
  label.SetProperty( TextLabel::Property::TEXT, "Hello world Hello world Hello world Hello world Hello world Hello world" );
  label.SetProperty( TextLabel::Property::POINT_SIZE, 20 );
  label.SetProperty( TextLabel::Property::MULTI_LINE, false );
  Stage::GetCurrent().Add( label );

  application.SendNotification();
  application.Render();

  int lineCount =  label.GetProperty<int>( TextLabel::Property::LINE_COUNT );
  DALI_TEST_EQUALS( lineCount, 1, TEST_LOCATION );

  label.SetProperty( TextLabel::Property::MULTI_LINE, true );

  application.SendNotification();
  application.Render();

  lineCount =  label.GetProperty<int>( TextLabel::Property::LINE_COUNT );
  DALI_TEST_EQUALS( true, (lineCount > 1) , TEST_LOCATION );


  END_TEST;
}

int UtcDaliToolkitTextlabelTextDirection(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelTextDirection");

  TextLabel label = TextLabel::New();
  DALI_TEST_EQUALS( label.GetProperty< int >( DevelTextLabel::Property::TEXT_DIRECTION ), static_cast< int >( Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT ), TEST_LOCATION );

  label.SetProperty( TextLabel::Property::TEXT, "Hello world" );
  label.SetProperty( TextLabel::Property::POINT_SIZE, 20 );
  Stage::GetCurrent().Add( label );

  // Test LTR text
  DALI_TEST_EQUALS( label.GetProperty< int >( DevelTextLabel::Property::TEXT_DIRECTION ), static_cast< int >( Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT ), TEST_LOCATION );

  // Test RTL text
  label.SetProperty( TextLabel::Property::TEXT, "ﻡﺮﺤﺑﺍ ﺏﺎﻠﻋﺎﻠﻣ ﻡﺮﺤﺑﺍ" );
  DALI_TEST_EQUALS( label.GetProperty< int >( DevelTextLabel::Property::TEXT_DIRECTION ), static_cast< int >( Toolkit::DevelText::TextDirection::RIGHT_TO_LEFT ), TEST_LOCATION );

  // Test RTL text starting with weak character
  label.SetProperty( TextLabel::Property::TEXT, "()ﻡﺮﺤﺑﺍ ﺏﺎﻠﻋﺎﻠﻣ ﻡﺮﺤﺑﺍ" );
  DALI_TEST_EQUALS( label.GetProperty< int >( DevelTextLabel::Property::TEXT_DIRECTION ), static_cast< int >( Toolkit::DevelText::TextDirection::RIGHT_TO_LEFT ), TEST_LOCATION );

  // Test RTL text string with emoji and weak character
  label.SetProperty( TextLabel::Property::TEXT, "\xF0\x9F\x98\x81 () ﻡﺮﺤﺑﺍ ﺏﺎﻠﻋﺎﻠﻣ ﻡﺮﺤﺑﺍ" );
  DALI_TEST_EQUALS( label.GetProperty< int >( DevelTextLabel::Property::TEXT_DIRECTION ), static_cast< int >( Toolkit::DevelText::TextDirection::RIGHT_TO_LEFT ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitTextlabelVerticalLineAlignment(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelVerticalLineAlignment");

  TextLabel label = TextLabel::New();

  label.SetProperty( DevelTextLabel::Property::VERTICAL_LINE_ALIGNMENT, DevelText::VerticalLineAlignment::TOP  );
  label.SetProperty( TextLabel::Property::TEXT, "Hello world" );
  label.SetProperty( TextLabel::Property::POINT_SIZE, 15 );
  label.SetProperty( TextLabel::Property::LINE_SPACING, 12 );
  Stage::GetCurrent().Add( label );
  DALI_TEST_EQUALS( label.GetProperty< int >( DevelTextLabel::Property::VERTICAL_LINE_ALIGNMENT ), static_cast< int >( Toolkit::DevelText::VerticalLineAlignment::TOP ), TEST_LOCATION );

  label.SetProperty( DevelTextLabel::Property::VERTICAL_LINE_ALIGNMENT, DevelText::VerticalLineAlignment::MIDDLE  );
  DALI_TEST_EQUALS( label.GetProperty< int >( DevelTextLabel::Property::VERTICAL_LINE_ALIGNMENT ), static_cast< int >( Toolkit::DevelText::VerticalLineAlignment::MIDDLE ), TEST_LOCATION );

  label.SetProperty( DevelTextLabel::Property::VERTICAL_LINE_ALIGNMENT, DevelText::VerticalLineAlignment::BOTTOM  );
  DALI_TEST_EQUALS( label.GetProperty< int >( DevelTextLabel::Property::VERTICAL_LINE_ALIGNMENT ), static_cast< int >( Toolkit::DevelText::VerticalLineAlignment::BOTTOM ), TEST_LOCATION );

  END_TEST;
}
