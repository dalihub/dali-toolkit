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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-style-properties-devel.h>
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali/devel-api/text-abstraction/bitmap-font.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali-toolkit/devel-api/text/bitmap-font.h>
#include <dali-toolkit/devel-api/text/rendering-backend.h>
#include <dali-toolkit/devel-api/text/text-utils-devel.h>
#include "test-text-geometry-utils.h"

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
const char* const PROPERTY_NAME_STRIKETHROUGH = "strikethrough";

const char* const PROPERTY_NAME_PIXEL_SIZE = "pixelSize";
const char* const PROPERTY_NAME_ELLIPSIS = "ellipsis";
const char* const PROPERTY_NAME_AUTO_SCROLL_LOOP_DELAY = "autoScrollLoopDelay";
const char* const PROPERTY_NAME_FONT_SIZE_SCALE = "fontSizeScale";

const char* const PROPERTY_NAME_ELLIPSIS_POSITION = "ellipsisPosition";

const std::string DEFAULT_FONT_DIR( "/resources/fonts" );
const unsigned int EMOJI_FONT_SIZE = 3840u; // 60 * 64

static bool gAnchorClickedCallBackCalled;
static bool gAnchorClickedCallBackNotCalled;

static bool gTextFitChangedCallBackCalled;

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

static void TestAnchorClickedCallback(TextLabel control, const char* href, unsigned int hrefLength)
{
  tet_infoline(" TestAnchorClickedCallback");

  gAnchorClickedCallBackNotCalled = false;

  if (!strcmp(href, "https://www.tizen.org") && hrefLength == strlen(href))
  {
    gAnchorClickedCallBackCalled = true;
  }
}

static void TestTextFitChangedCallback(TextLabel control)
{
  tet_infoline(" TestTextFitChangedCallback");
  gTextFitChangedCallBackCalled = true;
}

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

int UtcDaliTextLabelMoveConstructor(void)
{
  ToolkitTestApplication application;

  TextLabel textLabel = TextLabel::New();
  textLabel.SetProperty( TextLabel::Property::TEXT, "Test" );
  DALI_TEST_CHECK( textLabel.GetProperty<std::string>( TextLabel::Property::TEXT ) == "Test" );

  TextLabel moved = std::move( textLabel );
  DALI_TEST_CHECK( moved );
  DALI_TEST_EQUALS( 1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION );
  DALI_TEST_CHECK( moved.GetProperty<std::string>( TextLabel::Property::TEXT ) == "Test" );
  DALI_TEST_CHECK( !textLabel );

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

int UtcDaliTextLabelMoveAssignment(void)
{
  ToolkitTestApplication application;

  TextLabel textLabel = TextLabel::New();
  textLabel.SetProperty( TextLabel::Property::TEXT, "Test" );
  DALI_TEST_CHECK( textLabel.GetProperty<std::string>( TextLabel::Property::TEXT ) == "Test" );

  TextLabel moved;
  moved = std::move( textLabel );
  DALI_TEST_CHECK( moved );
  DALI_TEST_EQUALS( 1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION );
  DALI_TEST_CHECK( moved.GetProperty<std::string>( TextLabel::Property::TEXT ) == "Test" );
  DALI_TEST_CHECK( !textLabel );

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
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_RENDERING_BACKEND ) == DevelTextLabel::Property::RENDERING_BACKEND );
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
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_FONT_SIZE_SCALE ) == DevelTextLabel::Property::FONT_SIZE_SCALE );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_ELLIPSIS_POSITION ) == DevelTextLabel::Property::ELLIPSIS_POSITION );
  DALI_TEST_CHECK( label.GetPropertyIndex( PROPERTY_NAME_STRIKETHROUGH ) == DevelTextLabel::Property::STRIKETHROUGH );

  END_TEST;
}

int UtcDaliToolkitTextLabelSetPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelSetPropertyP");
  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK( label );

  application.GetScene().Add( label );

  // Note - we can't check the defaults since the stylesheets are platform-specific
  label.SetProperty( DevelTextLabel::Property::RENDERING_BACKEND, DevelText::RENDERING_SHARED_ATLAS );
  DALI_TEST_EQUALS( (DevelText::RenderingType)label.GetProperty<int>( DevelTextLabel::Property::RENDERING_BACKEND ), DevelText::RENDERING_SHARED_ATLAS, TEST_LOCATION );

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

  label.SetProperty( DevelTextLabel::Property::FONT_SIZE_SCALE, 2.5f );
  DALI_TEST_EQUALS( label.GetProperty<float>( DevelTextLabel::Property::FONT_SIZE_SCALE ), 2.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  label.SetProperty( DevelTextLabel::Property::FONT_SIZE_SCALE, 1.0f );

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

  Property::Map strikethroughMapSet;
  Property::Map strikethroughMapGet;

  strikethroughMapSet.Insert( "enable", false );
  strikethroughMapSet.Insert( "color", Color::BLUE );
  strikethroughMapSet.Insert( "height", 2.0f );

  label.SetProperty( DevelTextLabel::Property::STRIKETHROUGH, strikethroughMapSet );

  strikethroughMapGet = label.GetProperty<Property::Map>( DevelTextLabel::Property::STRIKETHROUGH );
  DALI_TEST_EQUALS( strikethroughMapGet.Count(), strikethroughMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( strikethroughMapGet, strikethroughMapSet ), true, TEST_LOCATION );

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

  // Check for incomplete marks.
  label.SetProperty( TextLabel::Property::TEXT, "<color='white'><i>Markup</i><b>Text</b></color>" );
  DALI_TEST_EQUALS( label.GetProperty<std::string>( TextLabel::Property::TEXT ), std::string("MarkupText"), TEST_LOCATION );
  try
  {
    application.SendNotification();
    application.Render();
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

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

  // Check the strikethrough property
  strikethroughMapSet.Clear();
  strikethroughMapSet.Insert( "enable", true );
  strikethroughMapSet.Insert( "color", Color::RED );
  strikethroughMapSet.Insert( "height", 2.0f );

  label.SetProperty( DevelTextLabel::Property::STRIKETHROUGH, strikethroughMapSet );

  application.SendNotification();
  application.Render();

  strikethroughMapGet = label.GetProperty<Property::Map>( DevelTextLabel::Property::STRIKETHROUGH );
  DALI_TEST_EQUALS( strikethroughMapGet.Count(), strikethroughMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( strikethroughMapGet, strikethroughMapSet ), true, TEST_LOCATION );

  strikethroughMapSet.Clear();
  strikethroughMapSet.Insert( Toolkit::DevelText::Strikethrough::Property::ENABLE, true );
  strikethroughMapSet.Insert( Toolkit::DevelText::Strikethrough::Property::COLOR, Color::RED );
  strikethroughMapSet.Insert( Toolkit::DevelText::Strikethrough::Property::HEIGHT, 2.0f );

  label.SetProperty( DevelTextLabel::Property::STRIKETHROUGH, strikethroughMapSet );

  application.SendNotification();
  application.Render();

  strikethroughMapGet = label.GetProperty<Property::Map>( DevelTextLabel::Property::STRIKETHROUGH );
  DALI_TEST_EQUALS( strikethroughMapGet.Count(), strikethroughMapSet.Count(), TEST_LOCATION );
  std::vector<std::string> strikethroughIndicesConversionTable = { "enable", "color","height"};
  DALI_TEST_EQUALS( DaliTestCheckMaps( strikethroughMapGet, strikethroughMapSet, strikethroughIndicesConversionTable ), true, TEST_LOCATION );

  strikethroughMapSet.Clear();

  Property::Map strikethroughDisabledMapGet;
  strikethroughDisabledMapGet.Insert( "enable", false );
  strikethroughDisabledMapGet.Insert( "color", Color::RED );
  strikethroughDisabledMapGet.Insert( "height", 2.0f );

  label.SetProperty( DevelTextLabel::Property::STRIKETHROUGH, strikethroughMapSet );

  application.SendNotification();
  application.Render();

  strikethroughMapGet = label.GetProperty<Property::Map>( DevelTextLabel::Property::STRIKETHROUGH );
  DALI_TEST_EQUALS( strikethroughMapGet.Count(), strikethroughDisabledMapGet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( strikethroughMapGet, strikethroughDisabledMapGet ), true, TEST_LOCATION );

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

  // Check the line size property
  DALI_TEST_EQUALS( label.GetProperty<float>( DevelTextLabel::Property::MIN_LINE_SIZE ), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  label.SetProperty( DevelTextLabel::Property::MIN_LINE_SIZE, 50.f );
  DALI_TEST_EQUALS( label.GetProperty<float>( DevelTextLabel::Property::MIN_LINE_SIZE ), 50.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

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

  application.GetScene().Add( label );

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

  Property::Map strikethroughMap;
  strikethroughMap.Insert( "enable", true );
  strikethroughMap.Insert( "color", Color::GREEN );
  strikethroughMap.Insert( "height", 2.0f );
  label.SetProperty( DevelTextLabel::Property::STRIKETHROUGH, strikethroughMap );

  try
  {
    // Render some text with the shared atlas backend
    label.SetProperty( DevelTextLabel::Property::RENDERING_BACKEND, DevelText::RENDERING_SHARED_ATLAS );
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
    label.SetProperty( DevelTextLabel::Property::RENDERING_BACKEND, DevelText::RENDERING_VECTOR_BASED );
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

  application.GetScene().Add( label );

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

  application.GetScene().Add( label );

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

  // EMOJI + ZWJ + EMOJI case for coverage.
  const std::string emojiWithZWJ = "&#x1f469;&#x200d;&#x1f52c;";
  label.SetProperty( TextLabel::Property::TEXT, emojiWithZWJ );

  application.SendNotification();
  application.Render();

  // EMOJI Sequences case for coverage.
  std::string emojiSequences =
       "Text VS15 &#x262a;&#xfe0e;\n"                                                         //text presentation sequence and selector
      "Color VS16 &#x262a;&#xfe0f;\n"                                                        //emoji presentation sequence and selector
      "Default &#x262a; \n"                                                                  //default presentation
      "FamilyManWomanGirlBoy &#x1F468;&#x200D;&#x1F469;&#x200D;&#x1F467;&#x200D;&#x1F466;\n" // emoji multi zwj sequence
      "WomanScientist &#x1f469;&#x200d;&#x1f52c;\n"                                          // emoji zwj sequence
      "WomanScientistLightSkinTone&#x1F469;&#x1F3FB;&#x200D;&#x1F52C; \n"                    //emoji modifier sequence: skin tone & JWZ
      "LeftRightArrowText&#x2194;&#xfe0e;\n"                                                 //text presentation sequence and selector
      "LeftRightArrowEmoji&#x2194;&#xfe0f;\n"                                                //emoji presentation sequence and selector
      "SouthKoreaFlag&#x1f1f0;&#x1f1f7;\n"                                                   //emoji flag sequence
      "JordanFlag&#x1f1ef;&#x1f1f4;\n"                                                       // emoji flag sequence
      "EnglandFlag&#x1F3F4;&#xE0067;&#xE0062;&#xE0065;&#xE006E;&#xE0067;&#xE007F;\n"         //emoji tag sequence like England flag
      "Runner &#x1f3c3;&#x200d;&#x27a1;&#xfe0f; \n"
      "VictoryHandMediumLightSkinTone:&#x270C;&#xFE0F;&#x1F3FC;\n"               //emoji modifier sequence: skin tone
      "RainbowFlag:&#x1F3F3;&#xFE0F;&#x200D;&#x1F308; \n"                        //emoji zwj sequence: Rainbow Flag
      "keycap# &#x0023;&#xFE0F;&#x20E3; \n"                                      // fully-qualified  emoji keycap sequence
      "keycap#_text &#x0023;&#x20E3; \n"                                         // unqualified emoji keycap sequence
      "keycap3 &#x0033;&#xfe0f;&#x20e3; \n"                                      // fully-qualified  emoji keycap sequence
      "keycap3_text &#x0033;&#x20e3; \n"                                         // unqualified emoji keycap sequence
      "two adjacent glyphs &#x262a;&#xfe0f;&#xfe0f;&#xfe0f;&#x262a;&#xfe0f;\n"   //This line should be rendered as two adjacent glyphs
      "Digit 8&#xfe0f; 8&#xfe0e; 8\n"                                            // should be rendered according to selector
      "Surfing Medium Skin Female:  &#x1f3c4;&#x1f3fc;&#x200d;&#x2640;&#xfe0f;"; // Person Surfing + Medium Skin Tone +? Zero Width Joiner + Female Sign

  label.SetProperty( TextLabel::Property::TEXT, emojiSequences );
  label.SetProperty( TextLabel::Property::ENABLE_MARKUP, true );
  label.SetProperty( TextLabel::Property::MULTI_LINE, true);
  label.SetProperty( TextLabel::Property::ELLIPSIS, false);

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
  application.GetScene().Add( labelImmediate );
  // Turn on all the effects
  labelImmediate.SetProperty( TextLabel::Property::MULTI_LINE, false );
  labelImmediate.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  labelImmediate.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  labelImmediate.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );
  labelImmediate.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE );

  application.GetScene().Add( labelFinished );
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
  application.GetScene().Add( labelShort );
  // Turn on all the effects
  labelShort.SetProperty( TextLabel::Property::MULTI_LINE, false );
  labelShort.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE );

  application.GetScene().Add( labelLong );
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
  application.GetScene().Add( labelShort );
  // Turn on all the effects
  labelShort.SetProperty( TextLabel::Property::MULTI_LINE, false );
  labelShort.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE );

  application.GetScene().Add( labelLong );
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
  application.GetScene().Add( labelShort );
  // Turn on all the effects
  labelShort.SetProperty( TextLabel::Property::MULTI_LINE, false );
  labelShort.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "END" );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE );

  application.GetScene().Add( labelLong );
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
  application.GetScene().Add( labelShort );
  // Turn on all the effects
  labelShort.SetProperty( TextLabel::Property::MULTI_LINE, false );
  labelShort.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "END" );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );
  labelShort.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE );

  application.GetScene().Add( labelLong );
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
  application.GetScene().Add( label );
  label.SetProperty( Actor::Property::SIZE, Vector2( 360.0f, 20.f ) );
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

  application.GetScene().Add( label );

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

int UtcDaliToolkitTextlabelScrollingWithEllipsis(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelScrollingWithEllipsis");

  TextLabel label = TextLabel::New("Some text to scroll");
  DALI_TEST_CHECK( label );

  application.GetScene().Add( label );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Turn on all the effects.
  label.SetProperty( TextLabel::Property::AUTO_SCROLL_GAP, 50.0f );
  label.SetProperty( TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3 );
  label.SetProperty( TextLabel::Property::AUTO_SCROLL_SPEED, 80.0f );

  try
  {
    // Enable the auto scrolling effect.
    label.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true );
    label.SetProperty( TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE );

    // Disable the ellipsis
    label.SetProperty( TextLabel::Property::ELLIPSIS, false );

    // Render the text.
    application.SendNotification();
    application.Render();

    // Stop auto scrolling
    label.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false );

    // Check the ellipsis property
    DALI_TEST_CHECK( !label.GetProperty<bool>( TextLabel::Property::ELLIPSIS ) );
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

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

  application.GetScene().Add( label );

  // Turn on all the effects
  label.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
  label.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  label.SetProperty( Actor::Property::SIZE, Vector2( 360.0f, 10.f ) );

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
  label.SetProperty( Actor::Property::SIZE, Vector2( 400.0f, 10.f ) );

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
  label.SetProperty( Actor::Property::SIZE, Vector2( 400.0f, 10.f ) );

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
  label.SetProperty( Actor::Property::SIZE, Vector2( 300.0f, 300.f ) );
  label.SetProperty( TextLabel::Property::TEXT, "Hello world Hello world" );
  label.SetProperty( TextLabel::Property::MULTI_LINE, true );



  //label.SetProperty( TextLabel::Property::POINT_SIZE, 18 );
  application.GetScene().Add( label );

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

  // Test a transparent text - Rendering should be skipped.
  label.SetProperty( TextLabel::Property::TEXT, "Hello world Hello world" );
  label.SetProperty( TextLabel::Property::TEXT_COLOR, Color::BLUE );

  application.GetScene().Add( label );

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
  label.SetProperty( Actor::Property::SIZE, Vector2( 300.0f, 300.f ) );
  label.SetProperty( TextLabel::Property::TEXT, "Hello world Hello world" );
  label.SetProperty( TextLabel::Property::POINT_SIZE, 12 );
  application.GetScene().Add( label );

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
  application.GetScene().Add( label );

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
  label.SetProperty( DevelTextLabel::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION, false );
  application.GetScene().Add( label );

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
  application.GetScene().Add( label );
  DALI_TEST_EQUALS( label.GetProperty< int >( DevelTextLabel::Property::VERTICAL_LINE_ALIGNMENT ), static_cast< int >( Toolkit::DevelText::VerticalLineAlignment::TOP ), TEST_LOCATION );

  label.SetProperty( DevelTextLabel::Property::VERTICAL_LINE_ALIGNMENT, DevelText::VerticalLineAlignment::MIDDLE  );
  DALI_TEST_EQUALS( label.GetProperty< int >( DevelTextLabel::Property::VERTICAL_LINE_ALIGNMENT ), static_cast< int >( Toolkit::DevelText::VerticalLineAlignment::MIDDLE ), TEST_LOCATION );

  label.SetProperty( DevelTextLabel::Property::VERTICAL_LINE_ALIGNMENT, DevelText::VerticalLineAlignment::BOTTOM  );
  DALI_TEST_EQUALS( label.GetProperty< int >( DevelTextLabel::Property::VERTICAL_LINE_ALIGNMENT ), static_cast< int >( Toolkit::DevelText::VerticalLineAlignment::BOTTOM ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitTextLabelBitmapFont(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelBitmapFont");

  DevelText::BitmapFontDescription fontDescription;
  fontDescription.name = "Digits";
  fontDescription.underlinePosition = 0.f;
  fontDescription.underlineThickness = 0.f;

  fontDescription.glyphs.push_back( { TEST_RESOURCE_DIR "/fonts/bitmap/u0030.png", ":", 34.f, 0.f } );
  fontDescription.glyphs.push_back( { TEST_RESOURCE_DIR "/fonts/bitmap/u0031.png", "0", 34.f, 0.f } );
  fontDescription.glyphs.push_back( { TEST_RESOURCE_DIR "/fonts/bitmap/u0032.png", "1", 34.f, 0.f } );
  fontDescription.glyphs.push_back( { TEST_RESOURCE_DIR "/fonts/bitmap/u0033.png", "2", 34.f, 0.f } );
  fontDescription.glyphs.push_back( { TEST_RESOURCE_DIR "/fonts/bitmap/u0034.png", "3", 34.f, 0.f } );
  fontDescription.glyphs.push_back( { TEST_RESOURCE_DIR "/fonts/bitmap/u0035.png", "4", 34.f, 0.f } );
  fontDescription.glyphs.push_back( { TEST_RESOURCE_DIR "/fonts/bitmap/u0036.png", "5", 34.f, 0.f } );
  fontDescription.glyphs.push_back( { TEST_RESOURCE_DIR "/fonts/bitmap/u0037.png", "6", 34.f, 0.f } );
  fontDescription.glyphs.push_back( { TEST_RESOURCE_DIR "/fonts/bitmap/u0038.png", "7", 34.f, 0.f } );
  fontDescription.glyphs.push_back( { TEST_RESOURCE_DIR "/fonts/bitmap/u0039.png", "8", 34.f, 0.f } );
  fontDescription.glyphs.push_back( { TEST_RESOURCE_DIR "/fonts/bitmap/u003a.png", "9", 34.f, 0.f } );

  TextAbstraction::BitmapFont bitmapFont;
  DevelText::CreateBitmapFont( fontDescription, bitmapFont );

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.GetFontId( bitmapFont );

  TextLabel label = TextLabel::New();

  label.SetProperty( TextLabel::Property::TEXT, "0123456789:" );
  label.SetProperty( TextLabel::Property::FONT_FAMILY, "Digits" );

  // The text has been laid out with the bitmap font if the natural size is the sum of all the width (322) and 34 height.
  DALI_TEST_EQUALS( label.GetNaturalSize(), Vector3(322.f, 34.f, 0.f), Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  application.GetScene().Add( label );

  application.SendNotification();
  application.Render();

  // The text has been rendered if the height of the text-label is the height of the line.
  DALI_TEST_EQUALS( label.GetCurrentProperty< Vector3 >( Actor::Property::SIZE ).height, 34.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  END_TEST;
}

int ConvertPointToPixel( float point )
{
  unsigned int horizontalDpi = 0u;
  unsigned int verticalDpi = 0u;
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.GetDpi( horizontalDpi, verticalDpi );

  return ( point * 72.f ) / static_cast< float >( horizontalDpi );
}

int UtcDaliToolkitTextlabelTextFit(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelTextFit");
  TextLabel label = TextLabel::New();
  Vector2 size( 460.0f, 100.0f );
  label.SetProperty( Actor::Property::SIZE, size );
  label.SetProperty( TextLabel::Property::TEXT, "Hello world" );

   // connect to the text git changed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::TextFitChangedSignal(label).Connect(&TestTextFitChangedCallback);
  bool textFitChangedSignal = false;
  label.ConnectSignal(testTracker, "textFitChanged", CallbackFunctor(&textFitChangedSignal));
  gTextFitChangedCallBackCalled = false;

  // check point size
  Property::Map textFitMapSet;
  textFitMapSet["enable"] = true;
  textFitMapSet["minSize"] = 10.f;
  textFitMapSet["maxSize"] = 100.f;
  textFitMapSet["stepSize"] = -1.f;
  textFitMapSet["fontSizeType"] = "pointSize";

  label.SetProperty( Toolkit::DevelTextLabel::Property::TEXT_FIT, textFitMapSet );
  label.SetProperty( TextLabel::Property::POINT_SIZE, 120.f);

  application.GetScene().Add( label );

  application.SendNotification();
  application.Render();

  const Vector3 EXPECTED_NATURAL_SIZE( 450.0f, 96.0f, 0.0f );
  DALI_TEST_EQUALS( EXPECTED_NATURAL_SIZE, label.GetNaturalSize(), TEST_LOCATION );

  DALI_TEST_CHECK(gTextFitChangedCallBackCalled);
  DALI_TEST_CHECK(textFitChangedSignal);

  // check pixel size
  textFitMapSet.Clear();
  textFitMapSet["enable"] = true;
  textFitMapSet["minSize"] = ConvertPointToPixel( 10.f );
  textFitMapSet["maxSize"] = ConvertPointToPixel( 100.f );
  textFitMapSet["stepSize"] = ConvertPointToPixel ( 1.f );
  textFitMapSet["fontSizeType"] = "pixelSize";

  label.SetProperty( Toolkit::DevelTextLabel::Property::TEXT_FIT, textFitMapSet );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( EXPECTED_NATURAL_SIZE, label.GetNaturalSize(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitTextlabelMaxTextureSet(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelMaxTextureSet");

  DevelText::BitmapFontDescription fontDescription;
  fontDescription.name = "Digits";
  fontDescription.glyphs.push_back( { TEST_RESOURCE_DIR "/fonts/bitmap/u0030.png", ":", 200.f, 0.f } );

  TextAbstraction::BitmapFont bitmapFont;
  DevelText::CreateBitmapFont( fontDescription, bitmapFont );

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.GetFontId( bitmapFont );

  TextLabel label = TextLabel::New();
  label.SetProperty( TextLabel::Property::FONT_FAMILY, "Digits" );
  label.SetProperty( TextLabel::Property::ENABLE_MARKUP, true );
  label.SetProperty( TextLabel::Property::TEXT, ":This is a long sample text made to allow max texture size to be exceeded." );
  label.SetProperty( TextLabel::Property::POINT_SIZE, 200.f );
  label.SetProperty( TextLabel::Property::MULTI_LINE, true );

  Property::Map underlineMapSet;
  underlineMapSet.Clear();
  underlineMapSet.Insert( "enable", true );
  underlineMapSet.Insert( "color", Color::RED );
  underlineMapSet.Insert( "height", 1 );
  label.SetProperty( TextLabel::Property::UNDERLINE, underlineMapSet );
  label.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::BLUE );

  Property::Map strikethroughMapSet;
  strikethroughMapSet.Clear();
  strikethroughMapSet.Insert( "enable", true );
  strikethroughMapSet.Insert( "color", Color::RED );
  strikethroughMapSet.Insert( "height", 2.0f );
  label.SetProperty( DevelTextLabel::Property::STRIKETHROUGH, strikethroughMapSet );
  label.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::BLUE );

  application.GetScene().Add( label );

  application.SendNotification();
  application.Render();

  const int maxTextureSize = Dali::GetMaxTextureSize();
  // Whether the rendered text is greater than maxTextureSize
  DALI_TEST_CHECK( label.GetCurrentProperty< Vector3 >( Actor::Property::SIZE ).height > maxTextureSize );

  // Check if the number of renderers is greater than 1.
  DALI_TEST_CHECK( label.GetRendererCount() > 1u );

  END_TEST;
}

int UtcDaliToolkitTextlabelStrikethroughExceedsWidthAndHeight(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelStrikethroughExceedsWidthAndHeight");

  TextLabel label = TextLabel::New();
  label.SetProperty( TextLabel::Property::TEXT, "Test" );
  label.SetProperty( TextLabel::Property::FONT_FAMILY, "DejaVu Sans");

  //Exeeding BufferWidth
  label.SetProperty(Actor::Property::SIZE, Vector2(200.f, 400.0f));
  label.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, HorizontalAlignment::RIGHT);
  label.SetProperty(TextLabel::Property::POINT_SIZE, 200.f);

  application.GetScene().Add( label );
  application.SendNotification();
  application.Render();

  Property::Map strikethroughMapSet;
  strikethroughMapSet.Clear();
  strikethroughMapSet.Insert( "enable", true );
  strikethroughMapSet.Insert( "color", Color::BLUE);
  strikethroughMapSet.Insert( "height", 2.0f);
  label.SetProperty( TextLabel::Property::TEXT, "Test1" );
  label.SetProperty( DevelTextLabel::Property::STRIKETHROUGH, strikethroughMapSet );
  label.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::BLUE );
  application.GetScene().Add( label );
  application.SendNotification();
  application.Render();
  // Check if the number of renderers is 1.
  DALI_TEST_EQUALS( 1, label.GetRendererCount(), TEST_LOCATION);


  label = TextLabel::New();
  label.SetProperty( TextLabel::Property::TEXT, "Test" );
  label.SetProperty( TextLabel::Property::FONT_FAMILY, "DejaVu Sans");

  //Exeeding BufferHeight
  label.SetProperty(Actor::Property::SIZE, Vector2(200.f, 100.0f));
  label.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, HorizontalAlignment::RIGHT);
  label.SetProperty(TextLabel::Property::POINT_SIZE, 200.f);

  application.GetScene().Add( label );
  application.SendNotification();
  application.Render();

  strikethroughMapSet.Clear();
  strikethroughMapSet.Insert( "enable", true );
  strikethroughMapSet.Insert( "color", Color::BLUE);
  strikethroughMapSet.Insert( "height", 2.0f);
  label.SetProperty( TextLabel::Property::TEXT, "Test2" );
  label.SetProperty( DevelTextLabel::Property::STRIKETHROUGH, strikethroughMapSet );
  label.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::BLUE );
  application.GetScene().Add( label );
  application.SendNotification();
  application.Render();
  // Check if the number of renderers is 1.
  DALI_TEST_EQUALS( 1, label.GetRendererCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextlabelLastCharacterIndex(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelLastCharacterIndex");

  Vector2 size( 300.0f, 100.0f );

  Dali::Toolkit::DevelText::RendererParameters textParameters;
  textParameters.text = "This is a sample text to get the last index.";
  textParameters.layout = "multiLine";
  textParameters.fontSize = 20.f;
  textParameters.textWidth = 300u;
  textParameters.textHeight = 100u;
  textParameters.ellipsisEnabled = true;
  Dali::Property::Array indexArray = Dali::Toolkit::DevelText::GetLastCharacterIndex( textParameters );

  DALI_TEST_CHECK( !indexArray.Empty() );
  DALI_TEST_EQUALS( indexArray.GetElementAt(0).Get<int>(), 10, TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitTextlabelFontSizeScale(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelFontSizeScale");

  TextLabel label = TextLabel::New();
  label.SetProperty( TextLabel::Property::POINT_SIZE, 30.f );
  label.SetProperty( TextLabel::Property::TEXT, "Test" );
  Vector3 nonScaledSize = label.GetNaturalSize();

  TextLabel labelScaled = TextLabel::New();
  labelScaled.SetProperty( TextLabel::Property::POINT_SIZE, 15.f );
  labelScaled.SetProperty( Toolkit::DevelTextLabel::Property::FONT_SIZE_SCALE, 2.f );
  labelScaled.SetProperty( TextLabel::Property::TEXT, "Test" );
  Vector3 scaledSize = labelScaled.GetNaturalSize();

  DALI_TEST_EQUALS( nonScaledSize, scaledSize, TEST_LOCATION );

  label.SetProperty( TextLabel::Property::PIXEL_SIZE, 30.f );
  label.SetProperty( TextLabel::Property::TEXT, "Test" );
  nonScaledSize = label.GetNaturalSize();

  labelScaled.SetProperty( TextLabel::Property::PIXEL_SIZE, 15.f );
  labelScaled.SetProperty( Toolkit::DevelTextLabel::Property::FONT_SIZE_SCALE, 2.f );
  labelScaled.SetProperty( TextLabel::Property::TEXT, "Test" );
  scaledSize = labelScaled.GetNaturalSize();

  DALI_TEST_EQUALS( nonScaledSize, scaledSize, TEST_LOCATION );

  END_TEST;
}

// Positive test for the anchorClicked signal.
int UtcDaliToolkitTextlabelAnchorClicked(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelAnchorClicked");
  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  application.GetScene().Add(label);

  // connect to the anchor clicked signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AnchorClickedSignal(label).Connect(&TestAnchorClickedCallback);
  bool anchorClickedSignal = false;
  label.ConnectSignal(testTracker, "anchorClicked", CallbackFunctor(&anchorClickedSignal));

  gAnchorClickedCallBackCalled = false;
  label.SetProperty(TextLabel::Property::TEXT, "<a href='https://www.tizen.org'>TIZEN</a>");
  label.SetProperty(TextLabel::Property::ENABLE_MARKUP, true);
  label.SetProperty(Actor::Property::SIZE, Vector2(100.f, 50.f));
  label.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  label.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  application.SendNotification();
  application.Render();

  // Create a tap event to touch the text label.
  TestGenerateTap(application, 5.0f, 25.0f, 100);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAnchorClickedCallBackCalled);
  DALI_TEST_CHECK(anchorClickedSignal);

  // reset
  gAnchorClickedCallBackCalled = false;
  anchorClickedSignal = false;
  label.SetProperty(TextLabel::Property::TEXT, "");
  label.SetProperty(TextLabel::Property::ENABLE_MARKUP, false);

  application.SendNotification();
  application.Render();

  // sets anchor text
  label.SetProperty(TextLabel::Property::ENABLE_MARKUP, true);
  label.SetProperty(TextLabel::Property::TEXT, "<a href='https://www.tizen.org'>TIZEN</a>");
  label.SetProperty(Actor::Property::SIZE, Vector2(100.f, 50.f));
  label.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  label.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  application.SendNotification();
  application.Render();

  // Create a tap event to touch the text label.
  TestGenerateTap(application, 5.0f, 25.0f, 200);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAnchorClickedCallBackCalled);
  DALI_TEST_CHECK(anchorClickedSignal);


  gAnchorClickedCallBackNotCalled = true;
  // Tap the outside of anchor, callback should not be called.
  TestGenerateTap(application, 150.f, 100.f, 300);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAnchorClickedCallBackNotCalled);

  END_TEST;
}

int UtcDaliTextLabelAtlasLimitationIsEnabledForLargeFontPointSize(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLabelAtlasLimitationIsEnabledForLargeFontPointSize ");

  //TextLabel is not using Atlas but this is to unify font-size on text-controllers

  // +2: First one to handle the equal case. Second one to handle odd to even case of GetNaturalSize
  const uint32_t lessThanWidth = TextAbstraction::FontClient::MAX_TEXT_ATLAS_WIDTH - TextAbstraction::FontClient::PADDING_TEXT_ATLAS_BLOCK + 2;
  const uint32_t lessThanHeight = TextAbstraction::FontClient::MAX_TEXT_ATLAS_HEIGHT - TextAbstraction::FontClient::PADDING_TEXT_ATLAS_BLOCK + 2;

  // Create a text editor
  TextLabel textLabel = TextLabel::New();
  //Set size to avoid automatic eliding
  textLabel.SetProperty( Actor::Property::SIZE, Vector2(1025, 1025));
  //Set very large font-size using point-size
  textLabel.SetProperty( TextLabel::Property::POINT_SIZE, 1000);
  //Specify font-family
  textLabel.SetProperty( TextLabel::Property::FONT_FAMILY, "DejaVu Sans");
  //Set text to check if appear or not
  textLabel.SetProperty( TextLabel::Property::TEXT, "A");

  application.GetScene().Add( textLabel );

  application.SendNotification();
  application.Render();
  //Use GetNaturalSize to verify that size of block does not exceed Atlas size
  Vector3 naturalSize = textLabel.GetNaturalSize();

  DALI_TEST_GREATER( lessThanWidth, static_cast<uint32_t>(naturalSize.width), TEST_LOCATION );
  DALI_TEST_GREATER( lessThanHeight, static_cast<uint32_t>(naturalSize.height), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextLabelHyphenWrapMode(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLabelHyphenWrapMode ");

  int lineCount =0;
  TextLabel label = TextLabel::New();
  label.SetProperty( Actor::Property::SIZE, Vector2( 150.0f, 300.f ));
  label.SetProperty( TextLabel::Property::POINT_SIZE, 12.f );
  label.SetProperty( TextLabel::Property::MULTI_LINE, true);
  application.GetScene().Add( label );
  application.SendNotification();
  application.Render();

  label.SetProperty( TextLabel::Property::TEXT, "Hi Experimen" );
  label.SetProperty(TextLabel::Property::LINE_WRAP_MODE,DevelText::LineWrap::HYPHENATION);
  DALI_TEST_EQUALS( label.GetProperty< int >( TextLabel::Property::LINE_WRAP_MODE ), static_cast< int >( DevelText::LineWrap::HYPHENATION ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  lineCount = label.GetProperty<int>( TextLabel::Property::LINE_COUNT );
  /*
    text will be :
    Hi Exp-
    erimen
  */
  DALI_TEST_EQUALS( lineCount, 2, TEST_LOCATION );

  label.SetProperty( TextLabel::Property::TEXT, "Hi Experimen" );
  label.SetProperty(TextLabel::Property::LINE_WRAP_MODE,DevelText::LineWrap::MIXED);
  DALI_TEST_EQUALS( label.GetProperty< int >( TextLabel::Property::LINE_WRAP_MODE ), static_cast< int >( DevelText::LineWrap::MIXED ), TEST_LOCATION );

  application.SendNotification();
  application.Render();

  lineCount = label.GetProperty<int>( TextLabel::Property::LINE_COUNT );
  /*
    text will be :
    Hi
    Experi-
    men
  */
  DALI_TEST_EQUALS( lineCount, 3, TEST_LOCATION );

  END_TEST;
}

int utcDaliTextLabelGetHeightForWidthChangeLineCountWhenTextChanged(void)
{
  ToolkitTestApplication application;

  tet_infoline(" utcDaliTextLabelGetHeightForWidthChangeLineCountWhenTextChanged ");

  int lineCountBefore =0 ;
  int lineCountAfter =0 ;

  // Create a text editor
  TextLabel textLabel = TextLabel::New();
  //Set very large font-size using point-size
  textLabel.SetProperty( TextLabel::Property::POINT_SIZE, 10) ;
  //Specify font-family
  textLabel.SetProperty( TextLabel::Property::FONT_FAMILY, "DejaVu Sans");
  //Specify size
  textLabel.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 100.f ) );
  //Set text longer than width of textLabel
  textLabel.SetProperty( TextLabel::Property::TEXT, "Short text");
  //Set line wrap mode Character
  textLabel.SetProperty(TextLabel::Property::LINE_WRAP_MODE, "CHARACTER");
  textLabel.SetProperty(TextLabel::Property::MULTI_LINE, true);

  application.GetScene().Add( textLabel );

  application.SendNotification();
  application.Render();


  lineCountBefore =  textLabel.GetProperty<int>( TextLabel::Property::LINE_COUNT );

  textLabel.SetProperty( TextLabel::Property::TEXT, "This is very loooooooooooooooooooooooooooooooooooong text for test");
  lineCountAfter =  textLabel.GetProperty<int>( TextLabel::Property::LINE_COUNT );

  // When the text changed, the Line-count should be updated according to new text.
  // Because the GetHeightForWidth is called in Controller::GetLineCount(float width)
  DALI_TEST_EQUALS( lineCountBefore ,1, TEST_LOCATION );
  DALI_TEST_GREATER( lineCountAfter,1, TEST_LOCATION );


  END_TEST;
}

int utcDaliTextLabelGeometryRTL(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextLabelGeometryRTL");

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK( label );

  application.GetScene().Add( label );

  label.SetProperty( TextLabel::Property::POINT_SIZE, 7.f );
  label.SetProperty( Actor::Property::SIZE, Vector2( 150.f, 100.f ) );
  label.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  label.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  label.SetProperty( TextLabel::Property::ENABLE_MARKUP, true );
  label.SetProperty( TextLabel::Property::MULTI_LINE, true);
  label.SetProperty( TextLabel::Property::TEXT, "line1 \nline2\nline 3\nالاخيرالسطر" );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  unsigned int expectedCount = 4;
  unsigned int startIndex = 3;
  unsigned int endIndex = 24;

  Vector<Vector2> positionsList = DevelTextLabel::GetTextPosition(label, startIndex, endIndex);
  Vector<Vector2> sizeList = DevelTextLabel::GetTextSize(label, startIndex, endIndex);

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

  expectedPositions.PushBack(Vector2(73, 75));
  expectedSizes.PushBack(Vector2(37, 25));

  TestTextGeometryUtils::CheckGeometryResult(positionsList, sizeList, expectedPositions, expectedSizes);

  END_TEST;
}

int utcDaliTextLabelGeometryGlyphMiddle(void)
{
  ToolkitTestApplication application;
  tet_infoline(" utcDaliTextLabelGeometryGlyphMiddle");

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK( label );

  application.GetScene().Add( label );

  label.SetProperty( TextLabel::Property::POINT_SIZE, 7.f );
  label.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  label.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  label.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  label.SetProperty( TextLabel::Property::ENABLE_MARKUP, true );
  label.SetProperty( TextLabel::Property::TEXT, "لا تحتوي على لا" );

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  // Render and notify
  application.SendNotification();
  application.Render();

  unsigned int expectedCount = 1;
  unsigned int startIndex = 1;
  unsigned int endIndex = 13;

  Vector<Vector2> positionsList = DevelTextLabel::GetTextPosition(label, startIndex, endIndex);
  Vector<Vector2> sizeList = DevelTextLabel::GetTextSize(label, startIndex, endIndex);

  DALI_TEST_EQUALS(positionsList.Size(), expectedCount, TEST_LOCATION);
  DALI_TEST_EQUALS(sizeList.Size(), expectedCount, TEST_LOCATION);

  Vector<Vector2> expectedSizes;
  Vector<Vector2> expectedPositions;

  expectedPositions.PushBack(Vector2(12, 0));
  expectedSizes.PushBack(Vector2(118, 25));

  TestTextGeometryUtils::CheckGeometryResult(positionsList, sizeList, expectedPositions, expectedSizes);

  END_TEST;
}

int UtcDaliToolkitTextlabelEllipsisPositionProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty ");
  TextLabel textLabel = TextLabel::New();

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Default is END");
  DALI_TEST_EQUALS( textLabel.GetProperty< int >( DevelTextLabel::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::END ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to START");
  textLabel.SetProperty(DevelTextLabel::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::START);
  DALI_TEST_EQUALS( textLabel.GetProperty< int >( DevelTextLabel::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::START ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to MIDDLE");
  textLabel.SetProperty(DevelTextLabel::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::MIDDLE);
  DALI_TEST_EQUALS( textLabel.GetProperty< int >( DevelTextLabel::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::MIDDLE ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to END");
  textLabel.SetProperty(DevelTextLabel::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::END);
  DALI_TEST_EQUALS( textLabel.GetProperty< int >( DevelTextLabel::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::END ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to START using integer");
  textLabel.SetProperty(DevelTextLabel::Property::ELLIPSIS_POSITION, 1);
  DALI_TEST_EQUALS( textLabel.GetProperty< int >( DevelTextLabel::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::START ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to MIDDLE using integer");
  textLabel.SetProperty(DevelTextLabel::Property::ELLIPSIS_POSITION, 2);
  DALI_TEST_EQUALS( textLabel.GetProperty< int >( DevelTextLabel::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::MIDDLE ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to END using integer");
  textLabel.SetProperty(DevelTextLabel::Property::ELLIPSIS_POSITION, 0);
  DALI_TEST_EQUALS( textLabel.GetProperty< int >( DevelTextLabel::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::END ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to START using string - uppercase");
  textLabel.SetProperty(DevelTextLabel::Property::ELLIPSIS_POSITION, "START");
  DALI_TEST_EQUALS( textLabel.GetProperty< int >( DevelTextLabel::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::START ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to MIDDLE using string - uppercase");
  textLabel.SetProperty(DevelTextLabel::Property::ELLIPSIS_POSITION, "MIDDLE");
  DALI_TEST_EQUALS( textLabel.GetProperty< int >( DevelTextLabel::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::MIDDLE ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to END using string - uppercase");
  textLabel.SetProperty(DevelTextLabel::Property::ELLIPSIS_POSITION, "END");
  DALI_TEST_EQUALS( textLabel.GetProperty< int >( DevelTextLabel::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::END ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to START using string - lowercase");
  textLabel.SetProperty(DevelTextLabel::Property::ELLIPSIS_POSITION, "start");
  DALI_TEST_EQUALS( textLabel.GetProperty< int >( DevelTextLabel::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::START ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to MIDDLE using string - lowercase");
  textLabel.SetProperty(DevelTextLabel::Property::ELLIPSIS_POSITION, "middle");
  DALI_TEST_EQUALS( textLabel.GetProperty< int >( DevelTextLabel::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::MIDDLE ), TEST_LOCATION );

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisPositionProperty - Change to END using string - lowercase");
  textLabel.SetProperty(DevelTextLabel::Property::ELLIPSIS_POSITION, "end");
  DALI_TEST_EQUALS( textLabel.GetProperty< int >( DevelTextLabel::Property::ELLIPSIS_POSITION ), static_cast< int >( Toolkit::DevelText::EllipsisPosition::END ), TEST_LOCATION );


  END_TEST;
}

int UtcDaliToolkitTextLabelStrikethroughGeneration(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelStrikethroughGeneration");

  TextLabel textLabel = TextLabel::New();
  textLabel.SetProperty( TextLabel::Property::TEXT, "Test" );
  textLabel.SetProperty( Actor::Property::SIZE, Vector2( 200.0f, 100.f ) );
  textLabel.SetProperty( TextLabel::Property::POINT_SIZE, 10) ;
  textLabel.SetProperty( TextLabel::Property::FONT_FAMILY, "DejaVu Sans");

  application.GetScene().Add( textLabel );
  application.SendNotification();
  application.Render();

  Property::Map strikethroughMapSet;
  Property::Map strikethroughMapGet;

  strikethroughMapSet.Insert( "enable", true );
  strikethroughMapSet.Insert( "color", Color::RED );
  strikethroughMapSet.Insert( "height", 2.0f );

  // Check the strikethrough property
  textLabel.SetProperty( DevelTextLabel::Property::STRIKETHROUGH, strikethroughMapSet );
  strikethroughMapGet = textLabel.GetProperty<Property::Map>( DevelTextLabel::Property::STRIKETHROUGH );
  textLabel.SetProperty( TextLabel::Property::TEXT, "Test1" );
  DALI_TEST_EQUALS( strikethroughMapGet.Count(), strikethroughMapSet.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( DaliTestCheckMaps( strikethroughMapGet, strikethroughMapSet ), true, TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  strikethroughMapSet.Clear();
  strikethroughMapGet.Clear();

  END_TEST;
}