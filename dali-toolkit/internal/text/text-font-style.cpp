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

// FILE HEADER
#include <dali-toolkit/internal/text/text-font-style.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/property-string-parser.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

namespace
{
const std::string STYLE_KEY( "style" );
const std::string WEIGHT_KEY( "weight" );
const std::string WIDTH_KEY( "width" );
const std::string SLANT_KEY( "slant" );

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_CONTROLS");
#endif

} // namespace

void SetFontStyleProperty( ControllerPtr controller, const Property::Value& value, FontStyle::Type type )
{
  if( controller )
  {
    const std::string style = value.Get< std::string >();
    DALI_LOG_INFO( gLogFilter, Debug::General, "Text Control %p FONT_STYLE %s\n", controller.Get(), style.c_str() );

    switch( type )
    {
      case FontStyle::DEFAULT:
      {
        // Stores the default font's style string to be recovered by the GetFontStyleProperty() function.
        controller->SetDefaultFontStyle( style );
        break;
      }
      case FontStyle::INPUT:
      {
        // Stores the input font's style string to be recovered by the GetFontStyleProperty() function.
        controller->SetInputFontStyle( style );
        break;
      }
    }

    // Parses and applies the style.
    Property::Map map;
    ParsePropertyString( style, map );

    if( !map.Empty() )
    {
      /// Weight key
      Property::Value* weightValue = map.Find( WEIGHT_KEY );

      FontWeight weight = TextAbstraction::FontWeight::NONE;
      const bool weightDefined = weightValue != NULL;
      if( weightDefined )
      {
        const std::string weightStr = weightValue->Get<std::string>();

        Scripting::GetEnumeration< FontWeight >( weightStr.c_str(),
                                                 FONT_WEIGHT_STRING_TABLE,
                                                 FONT_WEIGHT_STRING_TABLE_COUNT,
                                                 weight );
      }

      /// Width key
      Property::Value* widthValue = map.Find( WIDTH_KEY );

      FontWidth width = TextAbstraction::FontWidth::NONE;
      const bool widthDefined = widthValue != NULL;
      if( widthDefined )
      {
        const std::string widthStr = widthValue->Get<std::string>();

        Scripting::GetEnumeration< FontWidth >( widthStr.c_str(),
                                                FONT_WIDTH_STRING_TABLE,
                                                FONT_WIDTH_STRING_TABLE_COUNT,
                                                width );
      }

      /// Slant key
      Property::Value* slantValue = map.Find( SLANT_KEY );

      FontSlant slant = TextAbstraction::FontSlant::NONE;
      const bool slantDefined = slantValue != NULL;
      if( slantDefined )
      {
        const std::string slantStr = slantValue->Get<std::string>();

        Scripting::GetEnumeration< FontSlant >( slantStr.c_str(),
                                                FONT_SLANT_STRING_TABLE,
                                                FONT_SLANT_STRING_TABLE_COUNT,
                                                slant );
      }

      switch( type )
      {
        case FontStyle::DEFAULT:
        {
          // Sets the default font's style values.
          if( !weightDefined ||
              ( weightDefined && ( controller->GetDefaultFontWeight() != weight ) ) )
          {
            controller->SetDefaultFontWeight( weight );
          }

          if( !widthDefined ||
              ( widthDefined && ( controller->GetDefaultFontWidth() != width ) ) )
          {
            controller->SetDefaultFontWidth( width );
          }

          if( !slantDefined ||
              ( slantDefined && ( controller->GetDefaultFontSlant() != slant ) ) )
          {
            controller->SetDefaultFontSlant( slant );
          }
          break;
        }
        case FontStyle::INPUT:
        {
          // Sets the input font's style values.
          if( !weightDefined ||
              ( weightDefined && ( controller->GetInputFontWeight() != weight ) ) )
          {
            controller->SetInputFontWeight( weight );
          }

          if( !widthDefined ||
              ( widthDefined && ( controller->GetInputFontWidth() != width ) ) )
          {
            controller->SetInputFontWidth( width );
          }

          if( !slantDefined ||
              ( slantDefined && ( controller->GetInputFontSlant() != slant ) ) )
          {
            controller->SetInputFontSlant( slant );
          }
          break;
        }
      }
    } // map not empty
    else
    {
      switch( type )
      {
        case FontStyle::DEFAULT:
        {
          controller->SetDefaultFontWeight( TextAbstraction::FontWeight::NONE );
          controller->SetDefaultFontWidth( TextAbstraction::FontWidth::NONE );
          controller->SetDefaultFontSlant( TextAbstraction::FontSlant::NONE );
          break;
        }
        case FontStyle::INPUT:
        {
          controller->SetInputFontWeight( TextAbstraction::FontWeight::NONE );
          controller->SetInputFontWidth( TextAbstraction::FontWidth::NONE );
          controller->SetInputFontSlant( TextAbstraction::FontSlant::NONE );
          break;
        }
      }
    }
  }
}

void GetFontStyleProperty( ControllerPtr controller, Property::Value& value, FontStyle::Type type )
{
  if( controller )
  {
    switch( type )
    {
      case FontStyle::DEFAULT:
      {
        value = controller->GetDefaultFontStyle();
        break;
      }
      case FontStyle::INPUT:
      {
        value = controller->GetInputFontStyle();
        break;
      }
    }
  }
}

FontWeight StringToWeight( const char* const weightStr )
{
  FontWeight weight = TextAbstraction::FontWeight::NORMAL;
  Scripting::GetEnumeration<FontWeight>( weightStr,
                                         FONT_WEIGHT_STRING_TABLE,
                                         FONT_WEIGHT_STRING_TABLE_COUNT,
                                         weight );

  return weight;
}

FontWidth StringToWidth( const char* const widthStr )
{
  FontWidth width = TextAbstraction::FontWidth::NORMAL;
  Scripting::GetEnumeration<FontWidth>( widthStr,
                                        FONT_WIDTH_STRING_TABLE,
                                        FONT_WIDTH_STRING_TABLE_COUNT,
                                        width );

  return width;
}

FontSlant StringToSlant( const char* const slantStr )
{
  FontSlant slant = TextAbstraction::FontSlant::NORMAL;
  Scripting::GetEnumeration<FontSlant>( slantStr,
                                        FONT_SLANT_STRING_TABLE,
                                        FONT_SLANT_STRING_TABLE_COUNT,
                                        slant );

  return slant;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
