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
#include <dali-toolkit/internal/text/property-string-parser.h>

// EXTERNAL HEADERS
#include <sstream>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/builder/json-parser.h>
#include <dali-toolkit/devel-api/builder/tree-node.h>
#include <dali-toolkit/internal/text/markup-processor-helper-functions.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

void CreatePropertyMap( const TreeNode* const node, Property::Map& map )
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
    CreatePropertyMap( &pair.second, map );
  }
}

void ParsePropertyString( const std::string& property, Property::Map& map )
{
  Toolkit::JsonParser parser = Toolkit::JsonParser::New();

  if( parser.Parse( property ) )
  {
    const TreeNode* const node = parser.GetRoot();
    if( node )
    {
      CreatePropertyMap( node, map );
    }
  }
}

void StringOffsetToVector2( const std::string& offsetStr,
                            Vector2& offset )
{
  offset = Vector2::ZERO;

  unsigned int wsIndex = 0u;
  for( std::string::const_iterator it = offsetStr.begin(),
         endIt = offsetStr.end();
       it != endIt;
       ++it, ++wsIndex )
  {
    if( *it == ' ' )
    {
      break;
    }
  }

  offset[0] = StringToFloat( offsetStr.c_str() );
  offset[1] = StringToFloat( offsetStr.c_str() + wsIndex );
}

} //namespace Text

} //namespace Toolkit

} //namespace Dali
