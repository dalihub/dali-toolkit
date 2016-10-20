#ifndef __DALI_TOOLKIT_PROPERTY_STRING_PARSER_H__
#define __DALI_TOOLKIT_PROPERTY_STRING_PARSER_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES

namespace Dali
{

// Forward declaration
struct Vector2;

namespace Toolkit
{

// Forward declaration
class TreeNode;

namespace Text
{

/**
 * @brief Creates a map with pairs 'key,value' with the property's parameters.
 *
 * @param[in] node Data structure with the property's parameters.
 * @param[out] map A map with the property's parameters.
 *
 */
void CreatePropertyMap( const TreeNode* const node, Property::Map& map );

/**
 * @brief Parses a property string.
 *
 * @param[in] property A property string.
 * @param[out] map A map with the property's parameters.
 *
 */
void ParsePropertyString( const std::string& property, Property::Map& map );

} //namespace Text

} //namespace Toolkit

} //namespace Dali

#endif //__DALI_TOOLKIT_PROPERTY_STRING_PARSER_H__
