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
#include <dali-toolkit/devel-api/builder/json-parser.h>
#include <dali-toolkit/devel-api/builder/tree-node.h>

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
const std::string EMPTY_STRING( "" );

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_CONTROLS");
#endif

} // namespace

/**
 * @brief Creates a map with pairs 'key,value' with the font's style parameters.
 *
 * @param[in] node Data structure with the font's style parameters.
 * @param[out] map A map with the font's style parameters.
 *
 */
void CreateFontStyleMap( const TreeNode* const node, Property::Map& map )
{
  switch( node->GetType() )
  {
    case TreeNode::IS_NULL:
    case TreeNode::OBJECT:
    case TreeNode::ARRAY: // FALL THROUGH
    {
      break;
    }
    case TreeNode::STRING:
    {
      map.Insert( node->GetName(), Property::Value( node->GetString() ) );
      break;
    }
    case TreeNode::INTEGER:
    case TreeNode::FLOAT:
    case TreeNode::BOOLEAN: // FALL THROUGH
    {
      break;
    }
  }

  for( TreeNode::ConstIterator it = node->CBegin(), endIt = node->CEnd(); it != endIt; ++it )
  {
    const TreeNode::KeyNodePair& pair = *it;
    CreateFontStyleMap( &pair.second, map );
  }
}

/**
 * @brief Parses the font's style string.
 *
 * @param[in] style The font's style string.
 * @param[out] map A map with the font's style parameters.
 *
 */
void ParseFontStyleString( const std::string& style, Property::Map& map )
{
  Toolkit::JsonParser parser = Toolkit::JsonParser::New();

  if( parser.Parse( style ) )
  {
    const TreeNode* const node = parser.GetRoot();

    if( NULL != node )
    {
      CreateFontStyleMap( node, map );
    }
  }
}

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
    ParseFontStyleString( style, map );

    if( !map.Empty() )
    {
      /// Weight key
      Property::Value* weightValue = map.Find( WEIGHT_KEY );

      FontWeight weight = TextAbstraction::FontWeight::NORMAL;
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

      FontWidth width = TextAbstraction::FontWidth::NORMAL;
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

      FontSlant slant = TextAbstraction::FontSlant::NORMAL;
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
          if( weightDefined && ( controller->GetDefaultFontWeight() != weight ) )
          {
            controller->SetDefaultFontWeight( weight );
          }

          if( widthDefined && ( controller->GetDefaultFontWidth() != width ) )
          {
            controller->SetDefaultFontWidth( width );
          }

          if( slantDefined && ( controller->GetDefaultFontSlant() != slant ) )
          {
            controller->SetDefaultFontSlant( slant );
          }
          break;
        }
        case FontStyle::INPUT:
        {
          // Sets the input font's style values.
          if( weightDefined && ( controller->GetInputFontWeight() != weight ) )
          {
            controller->SetInputFontWeight( weight );
          }

          if( widthDefined && ( controller->GetInputFontWidth() != width ) )
          {
            controller->SetInputFontWidth( width );
          }

          if( slantDefined && ( controller->GetInputFontSlant() != slant ) )
          {
            controller->SetInputFontSlant( slant );
          }
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
