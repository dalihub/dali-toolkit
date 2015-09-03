/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/text-controls/text-font-style.h>

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
const std::string WIDTH_KEY( "width" );
const std::string WEIGHT_KEY( "weight" );
const std::string SLANT_KEY( "slant" );
const std::string EMPTY_STRING( "" );

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

    CreateFontStyleMap( node, map );
  }
}

void SetFontStyleProperty( ControllerPtr controller, const Property::Value& value )
{
  if( controller )
  {
    const std::string style = value.Get< std::string >();

    // Stores the string to be recovered by the GetFontStyleProperty() function.
    controller->SetDefaultFontStyle( style );

    // Parses and applies the style.
    Property::Map map;
    ParseFontStyleString( style, map );

    if( !map.Empty() )
    {
      /// Width key
      Property::Value* widthValue = map.Find( WIDTH_KEY );

      if( widthValue )
      {
        const std::string widthStr = widthValue->Get<std::string>();

        FontWidth width = TextAbstraction::FontWidth::NORMAL;
        if( Scripting::GetEnumeration< FontWidth >( widthStr.c_str(),
                                                    FONT_WIDTH_STRING_TABLE,
                                                    FONT_WIDTH_STRING_TABLE_COUNT,
                                                    width ) )
        {
          if( controller->GetDefaultFontWidth() != width )
          {
            controller->SetDefaultFontWidth( width );
          }
        }
      }
      else
      {
        controller->SetDefaultFontWidth( TextAbstraction::FontWidth::NORMAL );
      }

      /// Weight key
      Property::Value* weightValue = map.Find( WEIGHT_KEY );

      if( weightValue )
      {
        const std::string weightStr = weightValue->Get<std::string>();

        FontWeight weight = TextAbstraction::FontWeight::NORMAL;
        if( Scripting::GetEnumeration< FontWeight >( weightStr.c_str(),
                                                     FONT_WEIGHT_STRING_TABLE,
                                                     FONT_WEIGHT_STRING_TABLE_COUNT,
                                                     weight ) )
        {
          if( controller->GetDefaultFontWeight() != weight )
          {
            controller->SetDefaultFontWeight( weight );
          }
        }
      }
      else
      {
        controller->SetDefaultFontWeight( TextAbstraction::FontWeight::NORMAL );
      }

      /// Slant key
      Property::Value* slantValue = map.Find( SLANT_KEY );

      if( slantValue )
      {
        const std::string slantStr = slantValue->Get<std::string>();

        FontSlant slant = TextAbstraction::FontSlant::NORMAL;
        if( Scripting::GetEnumeration< FontSlant >( slantStr.c_str(),
                                                    FONT_SLANT_STRING_TABLE,
                                                    FONT_SLANT_STRING_TABLE_COUNT,
                                                    slant ) )
        {
          if( controller->GetDefaultFontSlant() != slant )
          {
            controller->SetDefaultFontSlant( slant );
          }
        }
      }
      else
      {
        controller->SetDefaultFontSlant( TextAbstraction::FontSlant::NORMAL );
      }
    }
  }
}

void GetFontStyleProperty( ControllerPtr controller, Property::Value& value )
{
  if( controller )
  {
    value = controller->GetDefaultFontStyle();
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
