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

#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>
#include <dali-toolkit/devel-api/layouting/grid.h>

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
  static GridPtr New();

  void SetNumberOfRows( unsigned int rows );

  void SetNumberOfColumns( unsigned int columns );

  int GetNumberOfColumns();

  void SetCellPadding( LayoutSize size );
  LayoutSize GetCellPadding();

protected:

  Grid();

  ~Grid();

  /**
   * @copydoc LayoutItem::DoInitialize
   */
  virtual void DoInitialize() override;

  /**
   * @copydoc LayoutItem::DoRegisterChildProperties()
   */
  virtual void DoRegisterChildProperties( const std::string& containerType ) override;

  /**
   * @copydoc LayoutItem::OnChildAdd
   */
  virtual void OnChildAdd( LayoutItem& child ) override;

  /**
   * @copydoc LayoutItem::OnMeasure
   */
  virtual void OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec ) override;

  /**
   * @copydoc LayoutItem::OnLayout
   */
  virtual void OnLayout( bool changed, LayoutLength l, LayoutLength t, LayoutLength r, LayoutLength b ) override;

private:
  Grid( const Grid& other ) = delete;
  Grid& operator=( const Grid& other ) = delete;

  /**
   * @brief
   * For the given availableSpace, calculate or retreive the number of required columns.
   * @param[in] availableSpace the space available for a child in each column.
   * @param[out] numberOfColumns the updated number of columns required
   */
  void DetermineNumberOfColumns( int availableSpace, int numberOfColumns );

  void CalculateLocations( int numberOfColumns, unsigned int columnWidth, unsigned int rowHeight, unsigned int numberOfCells );

  Dali::Vector<int> GetLocations( unsigned int axis );

private:

  class Locations;
  
  struct LocationsDeleter
  {
   void operator()(Locations *p);
  }
  //std::unique_ptr<Locations> mLocations;
  std::unique_ptr<Locations, LocationsDeleter> mLocations;

  LayoutSize mCellPadding;
  LayoutLength mTotalLength;

  // todo make enums?
  const int AUTO_FIT = -1;
  const int STRETCH_COLUMN_WIDTH = 2;

  const unsigned int DEFAULT_COLUMN_COUNT = 2;
  const unsigned int DEFAULT_ROW_COUNT = 3;

  int mNumColumns;
  int mNumRows;

  int mHorizontalSpacing = 0;
  int mVerticalSpacing = 0;
  int mRequestedHorizontalSpacing;
  int mStretchMode = STRETCH_COLUMN_WIDTH;
  int mColumnWidth;
  int mRequestedColumnWidth;
  int mRequestedNumColumns;
  int mRequestedNumRows;
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