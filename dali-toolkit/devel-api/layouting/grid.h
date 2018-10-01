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

#include <dali/public-api/object/base-handle.h>
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali-toolkit/devel-api/layouting/layout-group.h>
#include <dali-toolkit/devel-api/layouting/layout-size.h>
#include <dali-toolkit/public-api/toolkit-property-index-ranges.h>

namespace Dali
{
namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class Grid;
}

/**
 * @brief
 * This class implements a horizontal box layout, automatically handling
 * right to left or left to right direction change.
 */
class DALI_TOOLKIT_API Grid : public LayoutGroup
{
public:

  /**
   * @brief Creates an uninitialized Grid handle.
   *
   * Initialize it using Grid::New().
   * Calling member functions with an uninitialized handle is not allowed.
   */
  Grid();

  /**
   * @brief Creates a Grid object.
   */
  static Grid New();

  /**
   * @brief Downcasts a handle to a Grid handle.
   *
   * If handle points to a Grid, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.

   * @param[in] handle to an object
   * @return Handle to a Grid or an uninitialized handle
   */
  static Grid DownCast( BaseHandle handle );

  /**
   * @brief Copy constructor
   */
  Grid( const Grid& other );

  /**
   * @brief Assigment operator
   */
  Grid& operator=( const Grid& other );

  /**
   * @brief Default destructor.
   *
   * This is non-virtual, since derived Handle types must not contain data or virtual methods
   */
  ~Grid()=default;

  /**
   * @brief Set the number of columns in the Grid.
   * @param[in] columns number of columns,
   */
  void SetNumberOfColumns( int columns );

  /**
   *  @brief Get the number of columns in the grid.
   *  @return the number of columns.
   */
  int GetNumberOfColumns() const;

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief This constructor is used by Grid::New() methods.
   *
   * @param[in] actor A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Grid( Internal::Grid* body );
  /// @endcond
};

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_GRID_LAYOUT_H
