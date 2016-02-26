#ifndef DALI_TOOLKIT_INTERNAL_BUILDER_IMPL_DEBUG_H
#define DALI_TOOLKIT_INTERNAL_BUILDER_IMPL_DEBUG_H

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
 */

#include <dali/integration-api/debug.h>
#include <dali-toolkit/devel-api/builder/json-parser.h>

#if defined( DEBUG_ENABLED )

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

#define DUMP_PARSE_TREE(parser)  LogTree(parser)
#define DUMP_TEST_MAPPINGS(parser)                                      \
  OptionalChild mappings = IsChild( parser.GetRoot(), KEYNAME_MAPPINGS ); \
  if( mappings )                                                        \
  {                                                                     \
    std::ostringstream oss;                                             \
    oss << "Mappings: {" << std::endl;                                  \
    for( TreeNode::ConstIterator iter = (*mappings).CBegin(); iter != (*mappings).CEnd(); ++iter ) \
    {                                                                   \
      Property::Value value;                                            \
      bool converted = GetPropertyMap(*mappings, (*iter).first, Property::NONE, value ); \
      if( converted )                                                   \
      {                                                                 \
        oss << "  " << (*iter).first << ":" << value << std::endl;      \
      }                                                                 \
    }                                                                   \
    oss << "}" << std::endl;                                            \
    DALI_LOG_INFO( gFilterScript, Debug::Verbose, oss.str().c_str() );  \
  }


void LogTree( const Toolkit::JsonParser& mParser );

std::string PropertyValueToString( const Property::Value& value );


} // Internal
} // Toolkit
} // Dali

#else

#define DUMP_PARSE_TREE(parser)
#define DUMP_TEST_MAPPINGS(parser)

#endif // DEBUG_ENABLED
#endif // DALI_TOOLKIT_INTERNAL_BUILDER_IMPL_DEBUG_H
