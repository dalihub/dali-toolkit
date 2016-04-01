#ifndef DALI_TOOLKIT_INTERNAL_BUILDER_SET_PROPERTY_H
#define DALI_TOOLKIT_INTERNAL_BUILDER_SET_PROPERTY_H

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

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

/**
 * Set a property value from a tree node.
 * This function determines the type of the property from the format of the string in the node.
 * This is not always possible and if the type cannot be determined then then the type will default to Array.
 * @param node  The node string to convert from
 * @param value The property value to set
 */
void DeterminePropertyFromNode( const TreeNode&  node,
                                Property::Value& value );

/**
 * Set a property value from a tree node as DeterminePropertyFromNode() above
 * This function determines the type of the property from the format of the string in the node.
 * This is not always possible and if the type cannot be determined then then the type will default to Array.
 * @param node  The node string to convert from
 * @param value The property value to set
 * @param replacement The overriding replacement map (if any)
 */
void DeterminePropertyFromNode( const TreeNode&    node,
                                Property::Value&   value,
                                const Replacement& replacement );

/**
 * Set a property value as the given type from a tree node.
 * @param node The node string to convert from
 * @param type The property type to convert to.
 * @param value The property value to set
 * @return true if the string could be converted to the correct type.
 */
bool DeterminePropertyFromNode( const TreeNode&  node,
                                Property::Type   type,
                                Property::Value& value );

/**
 * Set a property value as the given type from a tree node as DeterminePropertyFromNode() above
 * @param node The node string to convert from
 * @param type The property type to convert to.
 * @param value The property value to set
 * @param replacement The overriding replacement map (if any)
 * @return true if the string could be converted to the correct type.
 */
bool DeterminePropertyFromNode( const TreeNode&    node,
                                Property::Type     type,
                                Property::Value&   value,
                                const Replacement& replacement );


} // Internal namespace
} // Toolkit namespace
} // Dali namespace

#endif //DALI_TOOLKIT_INTERNAL_BUILDER_SET_PROPERTY_H
