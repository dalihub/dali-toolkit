#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_GRID_IMPL_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_GRID_IMPL_H

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

// EXTERNAL_HEADERS
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL_HEADERS
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>
#include <dali-toolkit/devel-api/layouting/grid.h>
#include <dali-toolkit/internal/layouting/grid-locations.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class Grid;
using GridPtr = IntrusivePtr<Grid>;

class Grid final : public LayoutGroup
{
public:

  /**
   * Creates a pointer to an Internal Grid
   */
  static GridPtr New();

  /**
   * Set grid's the number of columns
   * @param[in] columns number of columns
   */
  void SetNumberOfColumns( int columns );

  /**
   * Get number of columns, -1 would indicate AUTO_FIT
   * which means the number has not been set but will be calculated.
   * @return the number of columns set
   */
  int GetNumberOfColumns() const;

  // Movable but not copyable
  Grid( const Grid& other ) = delete;
  Grid& operator=( const Grid& other ) = delete;
  Grid( Grid && ) = default;
  Grid& operator=( Grid && ) = default;

protected:

  /**
   * Default Constructor
   */
  Grid();

  /**
   * Destructor
   */
  ~Grid();

  /**
   * @copydoc LayoutItem::OnMeasure
   */
  virtual void OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec ) override;

  /**
   * @copydoc LayoutItem::OnLayout
   */
  virtual void OnLayout( bool changed, LayoutLength l, LayoutLength t, LayoutLength r, LayoutLength b ) override;

private:

  /**
   * @brief
   * For the given availableSpace, calculate or retreive the number of required columns.
   * @param[in] availableSpace the space available for a child in each column.
   */
  void DetermineNumberOfColumns( LayoutLength availableSpace );

private:

  GridLocationsPtr mLocations;

  LayoutLength mTotalHeight;
  LayoutLength mTotalWidth;

  const int AUTO_FIT = -1;

  int mNumColumns;
  int mNumRows;

  int mRequestedColumnWidth;
  int mRequestedNumColumns;
};

} // namespace Internal

inline Internal::Grid& GetImplementation( Dali::Toolkit::Grid& handle )
{
  DALI_ASSERT_ALWAYS( handle && "Grid handle is empty" );
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Internal::Grid&>( object );
}

inline const Internal::Grid& GetImplementation( const Dali::Toolkit::Grid& handle )
{
  DALI_ASSERT_ALWAYS( handle && "Grid handle is empty" );
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const Internal::Grid&>( object );
}

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_LAYOUTINGInner
