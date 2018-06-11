#ifndef DALI_TOOLKIT_LAYOUTING_VBOX_LAYOUT_H
#define DALI_TOOLKIT_LAYOUTING_VBOX_LAYOUT_H

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

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-handle.h>
#include <dali-toolkit/devel-api/layouting/layout-group.h>
#include <dali-toolkit/devel-api/layouting/layout-size.h>

namespace Dali
{
namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class VboxLayout;
}

/**
 * This class implements a vertical box layout, automatically handling
 * right to left or left to right direction change.
 */
class DALI_TOOLKIT_API VboxLayout : public LayoutGroup
{
public:

  enum PropertyRange
  {
    CHILD_PROPERTY_START_INDEX = LINEAR_LAYOUT_CHILD_PROPERTY_START_INDEX,
    CHILD_PROPERTY_END_INDEX   = LINEAR_LAYOUT_CHILD_PROPERTY_END_INDEX
  };

  struct ChildProperty
  {
    enum
    {
      WEIGHT = CHILD_PROPERTY_START_INDEX,
    };
  };

  /**
   * @brief Creates an uninitialized VboxLayout handle.
   *
   * Initialize it using VboxLayout::New().
   * Calling member functions with an uninitialized handle is not allowed.
   */
  VboxLayout();

  /**
   * @brief Creates a VboxLayout object.
   */
  static VboxLayout New();

  /**
   * @brief Downcasts a handle to a VboxLayout handle.
   *
   * If handle points to a VboxLayout, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.

   * @param[in] handle to an object
   * @return Handle to a VboxLayout or an uninitialized handle
   */
  static VboxLayout DownCast( BaseHandle handle );

  /**
   * @brief Copy constructor
   */
  VboxLayout( const VboxLayout& other );

  /**
   * @brief Assigment operator
   */
  VboxLayout& operator=( const VboxLayout& other );

  /**
   * @brief Default destructor.
   *
   * This is non-virtual, since derived Handle types must not contain data or virtual methods
   */
  ~VboxLayout()=default;

  /**
   * @brief Set the padding between cells in the layout
   *
   * @param[in] size The padding between cells.
   */
  void SetCellPadding( LayoutSize size );

  /**
   * @brief Get the padding between cells in the layout
   *
   * @return The padding between cells.
   */
  LayoutSize GetCellPadding();

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief This constructor is used by VboxLayout::New() methods.
   *
   * @param[in] actor A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL VboxLayout( Internal::VboxLayout* body );
  /// @endcond
};

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_VBOX_LAYOUT_H
