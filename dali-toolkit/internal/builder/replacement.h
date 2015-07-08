#ifndef __DALI_TOOLKIT_INTERNAL_BUILDER_REPLACEMENT__
#define __DALI_TOOLKIT_INTERNAL_BUILDER_REPLACEMENT__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/property-value.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/builder/builder.h>
#include <dali-toolkit/internal/builder/builder-get-is.inl.h>

namespace Dali
{

namespace Toolkit
{
  class TreeNode;
}

namespace Toolkit
{

namespace Internal
{

/*
 * Supports template replacement functionality
 *
 */
class Replacement
{
public:

  /*
   * Constructor
   */
  Replacement();

  /*
   * Constructor with default and overriding map
   *
   * Make a deep copy of the tree.
   * @param overrideMap The user overriding map
   * @param defaultMap The default map to use
   */
  Replacement( const Property::Map& overrideMap, const Property::Map& defaultMap );

  /*
   * Constructor with default map
   *
   * Make a deep copy of the tree.
   * @param overrideMap The user overriding map
   */
  Replacement( const Property::Map& defaultMap );

  /* @brief Check node for a type
   *
   * @param node The TreeNode to check
   * @return Optional value
   */
  OptionalString IsString( const TreeNode& node ) const;

  /* @brief Check node for a type
   *
   * @param node The TreeNode to check
   * @return Optional value
   */
  OptionalBoolean IsBoolean( const TreeNode & node ) const;

  /* @brief Check node for a type
   *
   * @param node The TreeNode to check
   * @return Optional value
   */
  OptionalFloat IsFloat( const TreeNode & node ) const;

  /* @brief Check node for a type
   *
   * @param node The TreeNode to check
   * @return Optional value
   */
  OptionalInteger IsInteger( const TreeNode & node ) const;

  /* @brief Check node for a type
   *
   * @param node The TreeNode to check
   * @return Optional value
   */
  OptionalVector2 IsVector2( const TreeNode & node ) const;

  /* @brief Check node for a type
   *
   * @param node The TreeNode to check
   * @return Optional value
   */
  OptionalVector3 IsVector3( const TreeNode & node ) const;

  /* @brief Check node for a type
   *
   * @param node The TreeNode to check
   * @return Optional value
   */
  OptionalVector4 IsVector4( const TreeNode & node ) const;

  /* @brief Check node for a type
   *
   * @param node The TreeNode to check
   * @return Optional value
   */
  OptionalMatrix IsMatrix( const TreeNode & node ) const;

  /* @brief Check node for a type
   *
   * @param node The TreeNode to check
   * @return Optional value
   */
  OptionalMatrix3 IsMatrix3( const TreeNode & node ) const;

  /* @brief Check node for a type
   *
   * @param node The TreeNode to check
   * @return Optional value
   */
  OptionalRect IsRect( const TreeNode & node ) const;

  /* @brief Check node for a type
   *
   * @param child The optional child TreeNode
   * @return Optional value
   */
  OptionalString IsString( OptionalChild child ) const;

  /* @brief Check node for a type
   *
   * @param child The optional child TreeNode
   * @return Optional value
   */
  OptionalFloat IsFloat( OptionalChild child ) const;

  /* @brief Check node for a type
   *
   * @param child The optional child TreeNode
   * @return Optional value
   */
  OptionalBoolean IsBoolean( OptionalChild child ) const;

  /* @brief Check node for a type
   *
   * @param child The optional child TreeNode
   * @return Optional value
   */
  OptionalInteger IsInteger( OptionalChild child ) const;

  /* @brief Check node for a type
   *
   * @param child The optional child TreeNode
   * @return Optional value
   */
  OptionalVector2 IsVector2( OptionalChild child ) const;

  /* @brief Check node for a type
   *
   * @param child The optional child TreeNode
   * @return Optional value
   */
  OptionalVector3 IsVector3( OptionalChild child ) const;

  /* @brief Check node for a type
   *
   * @param child The optional child TreeNode
   * @return Optional value
   */
  OptionalVector4 IsVector4( OptionalChild child ) const;

  /* @brief Check node for a type
   *
   * @param child The optional child TreeNode
   * @return Optional value
   */
  OptionalMatrix IsMatrix( OptionalChild child ) const;

  /* @brief Check node for a type
   *
   * @param child The optional child TreeNode
   * @return Optional value
   */
  OptionalMatrix3 IsMatrix3( OptionalChild child ) const;

  /* @brief Check node for a type
   *
   * @param child The optional child TreeNode
   * @return Optional value
   */
  OptionalRect IsRect( OptionalChild child ) const;

  /* @brief Check node for a type
   *
   * @param child The optional child TreeNode
   * @return Optional value
   */
  bool IsMap( OptionalChild child, Property::Value& out ) const;

  /* @brief Check node for a type
   *
   * @param child The optional child TreeNode
   * @return Optional value
   */
  bool IsArray( OptionalChild child, Property::Value& out ) const;

private:
  // Overriding map (overrides the default map). The map is not owned.
  const Property::Map* const mOverrideMap;

  // Default map. The map is not owned.
  const Property::Map* const mDefaultMap;

  // compiler
  // Replacement & operation=( Replacement& replacement );
  // Replacement( const Replacement& copy );

  // Returns the string if the node has a full replacement ie IMAGES if node is "{IMAGES}"
  OptionalString HasFullReplacement( const TreeNode & node ) const;

  // Returns the property value for a full replacement from the maps
  Property::Value GetFullReplacement( const std::string& replacementString ) const;

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali


#endif // header
