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
#include <dali-toolkit/internal/text/markup-processor-helper-functions.h>

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
const std::string FAMILY_KEY( "family" );
const std::string TYPE_KEY( "type" );

const std::string SYSTEM_TOKEN( "system" );

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_CONTROLS");
#endif

} // namespace

void SetFontFamilyProperty( ControllerPtr controller, const Property::Value& value )
{
  if( controller )
  {
    const std::string fontFamilyValue = value.Get<std::string>();

    if( fontFamilyValue.empty() )
    {
      // Resets the default's font family name.
      controller->SetDefaultFontFamily( "" );
      return;
    }

    Property::Map map;
    ParsePropertyString( fontFamilyValue, map );

    if( map.Empty() )
    {
      // There is no map. The font has been passed as a font's family name with no format.
      controller->SetDefaultFontFamily( fontFamilyValue );
    }
    else
    {
      /// Family key
      Property::Value* familyValue = map.Find( FAMILY_KEY );

      std::string fontFamilyName;
      if( NULL != familyValue )
      {
        fontFamilyName = familyValue->Get<std::string>();
      }

      /// Type key
      Property::Value* typeValue = map.Find( TYPE_KEY );

      std::string typeStr;
      if( NULL != typeValue )
      {
        typeStr = typeValue->Get<std::string>();
      }

      if( TokenComparison( SYSTEM_TOKEN, typeStr.c_str(), typeStr.size() ) )
      {
        controller->UpdateAfterFontChange( fontFamilyName );
      }
      else
      {
        controller->SetDefaultFontFamily( fontFamilyName );
      }
    }
  }
}

void SetFontStyleProperty( ControllerPtr controller, const Property::Value& value, FontStyle::Type type )
{
  if( controller )
  {
    Property::Map map;
    if( Property::STRING == value.GetType() )
    {
      const std::string& fontStyleProperties = value.Get<std::string>();

      ParsePropertyString( fontStyleProperties, map );
    }
    else
    {
      map = value.Get<Property::Map>();
    }

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
      } // switch
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
      } // switch
    } // map.Empty()
  } // controller
}

void GetFontStyleProperty( ControllerPtr controller, Property::Value& value, FontStyle::Type type )
{
  if( controller )
  {
    const bool isDefaultStyle = FontStyle::DEFAULT == type;

    bool weightDefined = false;
    bool widthDefined = false;
    bool slantDefined = false;
    FontWeight weight = TextAbstraction::FontWeight::NONE;
    FontWidth width = TextAbstraction::FontWidth::NONE;
    FontSlant slant = TextAbstraction::FontSlant::NONE;

    if( isDefaultStyle )
    {
      weightDefined = controller->IsDefaultFontWeightDefined();
      widthDefined = controller->IsDefaultFontWidthDefined();
      slantDefined = controller->IsDefaultFontSlantDefined();

      if( weightDefined )
      {
        weight = controller->GetDefaultFontWeight();
      }

      if( widthDefined )
      {
        width = controller->GetDefaultFontWidth();
      }

      if( slantDefined )
      {
        slant = controller->GetDefaultFontSlant();
      }
    }
    else
    {
      weightDefined = controller->IsInputFontWeightDefined();
      widthDefined = controller->IsInputFontWidthDefined();
      slantDefined = controller->IsInputFontSlantDefined();

      if( weightDefined )
      {
        weight = controller->GetInputFontWeight();
      }

      if( widthDefined )
      {
        width = controller->GetInputFontWidth();
      }

      if( slantDefined )
      {
        slant = controller->GetInputFontSlant();
      }
    }

    Property::Map map;

    if( weightDefined )
    {
      if( TextAbstraction::FontWeight::NONE != weight )
      {
        const std::string weightStr( GetEnumerationName( weight,
                                                         FONT_WEIGHT_STRING_TABLE,
                                                         FONT_WEIGHT_STRING_TABLE_COUNT ) );

        map.Insert( WEIGHT_KEY, weightStr );
      }
    }

    if( widthDefined )
    {
      if( TextAbstraction::FontWidth::NONE != width )
      {
        const std::string widthStr( GetEnumerationName( width,
                                                        FONT_WIDTH_STRING_TABLE,
                                                        FONT_WIDTH_STRING_TABLE_COUNT ) );

        map.Insert( WIDTH_KEY, widthStr );
      }
    }

    if( slantDefined )
    {
      if( TextAbstraction::FontSlant::NONE != slant )
      {
        const std::string slantStr( GetEnumerationName( slant,
                                                        FONT_SLANT_STRING_TABLE,
                                                        FONT_SLANT_STRING_TABLE_COUNT ) );

        map.Insert( SLANT_KEY, slantStr );
      }
    }

    value = map;
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
