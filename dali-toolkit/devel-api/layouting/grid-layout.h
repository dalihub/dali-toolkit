#ifndef DALI_TOOLKIT_LAYOUTING_GRID_LAYOUT_H
#define DALI_TOOLKIT_LAYOUTING_GRID_LAYOUT_H

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
#include <dali-toolkit/public-api/toolkit-property-index-ranges.h>

namespace Dali
{
namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class GridLayout;
}

/**
 * This class implements a horizontal box layout, automatically handling
 * right to left or left to right direction change.
 */
class DALI_IMPORT_API GridLayout : public LayoutGroup
{
public:


  /**
   * @brief Creates an uninitialized GridLayout handle.
   *
   * Initialize it using GridLayout::New().
   * Calling member functions with an uninitialized handle is not allowed.
   */
  GridLayout();

  /**
   * @brief Creates a GridLayout object.
   */
  static GridLayout New();

  /**
   * @brief Downcasts a handle to a GridLayout handle.
   *
   * If handle points to a GridLayout, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.

   * @param[in] handle to an object
   * @return Handle to a GridLayout or an uninitialized handle
   */
  static GridLayout DownCast( BaseHandle handle );

  /**
   * @brief Copy constructor
   */
  GridLayout( const GridLayout& other );

  /**
   * @brief Assigment operator
   */
  GridLayout& operator=( const GridLayout& other );

  /**
   * @brief Default destructor.
   *
   * This is non-virtual, since derived Handle types must not contain data or virtual methods
   */
  ~GridLayout()=default;

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
   * @brief This constructor is used by GridLayout::New() methods.
   *
   * @param[in] actor A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL GridLayout( Internal::GridLayout* body );
  /// @endcond
};

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_GRID_LAYOUT_H
