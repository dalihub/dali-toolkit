#ifndef DALI_TOOLKIT_LAYOUTING_LAYOUT_GROUP_H
#define DALI_TOOLKIT_LAYOUTING_LAYOUT_GROUP_H
/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <memory>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali-toolkit/devel-api/toolkit-property-index-ranges.h>
#include <dali-toolkit/devel-api/layouting/layout-item.h>
#include <dali-toolkit/devel-api/layouting/measure-spec.h>

namespace Dali
{
namespace Toolkit
{

namespace Internal
{
class LayoutGroup;
}


/**
 * A layout that has layout children. Implements LayoutItem.
 * It can both layout it's children, and be laid out by a parent container.
 *
 * A layout group automatically handles adding a Control container's children to itself,
 * both on startup and on child add/remove. If this functionality is not desired, this needs
 * splitting into two classes; one that manages just the layout part, and the other which
 * handles Actor hierarchy. (@todo Consider doing this anyway...)
 *
 * This handle class allows the application to set up layout properties for the layout group;
 * it doesn't access measure/layout directly.
 *
 * To write a new layout, inherit from both LayoutGroup handle and Internal::LayoutGroup body.
 *
 */
class DALI_TOOLKIT_API LayoutGroup : public LayoutItem
{
public:
  using LayoutId = unsigned int;
  static const unsigned int UNKNOWN_ID = 0;

  struct ChildProperty
  {
    enum
    {
    };
  };


  /**
   * @brief Creates an uninitialized LayoutGroup handle.
   *
   * LayoutGroup is intended as a base class, and as such, does not have a New method.
   *
   * Calling member functions with an uninitialized handle is not allowed.
   */
  LayoutGroup();

  /**
   * @brief Default Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~LayoutGroup() = default;

  /**
   * @brief Copy constructor
   */
  LayoutGroup(const LayoutGroup& copy) = default;

  /**
   * @brief Assigment operator
   */
  LayoutGroup& operator=(const LayoutGroup& rhs) = default;

  /**
   * @brief Downcasts a handle to a LayoutGroup handle.
   *
   * If handle points to a LayoutGroup, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.

   * @param[in] handle to an object
   * @return Handle to a LayoutGroup or an uninitialized handle
   */
  static LayoutGroup DownCast( BaseHandle handle );

  /**
   * @brief Add a child layout to the layout group
   *
   * @param[in] childLayout The layout to add.
   * @return an Id of the child.
   */
  LayoutId Add( LayoutItem& childLayout );

  /**
   * @brief Add a child layout to the layout group
   *
   * @param[in] childId the id of the child to remove
   */
  void Remove( LayoutId childId );

  /**
   * @brief Remove a child layout from the layout group
   *
   * @param[in] childLayout The layout to remove.
   */
  void Remove( LayoutItem& childLayout );

  /**
   * @brief Get the child at the given index.
   *
   * @param[in] index The index of the child.
   */
  LayoutItem GetChildAt( unsigned int index ) const;

  /**
   * @brief Get the count of the children of the layout
   *
   * @return the count of the children of the layout
   */
  unsigned int GetChildCount() const ;

  /**
   * @brief Get the child referenced by childId.
   *
   * @param[in] childId The id of the child to get
   * @return A handle to the child layout, or empty if not found
   */
  LayoutItem GetChild( LayoutId childId ) const ;

  /**
   * Delete template method to remove implicit casting to integer types.
   */
  template <typename T>
    LayoutItem GetChild( T childId ) = delete;

public:
  /// @cond internal
  /**
   * @brief This constructor is used by LayoutGroup::New() methods.
   *
   * @param[in] actor A pointer to a newly allocated Dali resource
   */
  explicit LayoutGroup( Internal::LayoutGroup* layoutGroup );
  /// @endcond
};


} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_LAYOUT_GROUP_H
