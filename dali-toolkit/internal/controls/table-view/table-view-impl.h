#ifndef __DALI_TOOLKIT_INTERNAL_TABLE_VIEW_H__
#define __DALI_TOOLKIT_INTERNAL_TABLE_VIEW_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/table-view/table-view.h>
#include "array-2d.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * TableView is a custom control for laying out actors in a table layout
 * @see Dali::Toolkit:TableView for more details
 */
class TableView : public Control
{
public:

  /**
   * Enum for the size policies of rows and columns
   */
  enum CellSizePolicy
  {
    FILL,       ///< Fill up available space, may have a ratio associated with it
    FIXED,      ///< A specific fixed width or height
    FIT         ///< Fit around actors in the row or column
  };

  /**
   * Create a new TableView.
   * @return A smart-pointer to the newly allocated TableView.
   */
  static Toolkit::TableView New( unsigned int initialRows, unsigned int initialColumns );

  /**
   * @copydoc Toolkit::TableView::AddChild
   */
  bool AddChild( Actor& child, const Toolkit::TableView::CellPosition& position );

  /**
   * @copydoc Toolkit::TableView::GetChildAt
   */
  Actor GetChildAt( const Toolkit::TableView::CellPosition& position );

  /**
   * @copydoc Toolkit::TableView::RemoveChildAt
   */
  Actor RemoveChildAt( const Toolkit::TableView::CellPosition& position );

  /**
   * @copydoc Toolkit::TableView::FindChildPosition
   */
  bool FindChildPosition( const Actor& child, Toolkit::TableView::CellPosition& positionOut );

  /**
   * @copydoc Toolkit::TableView::InsertRow
   */
  void InsertRow( unsigned int rowIndex );

  /**
   * @copydoc Toolkit::TableView::DeleteRow( unsigned int rowIndex )
   */
  void DeleteRow( unsigned int rowIndex );

  /**
   * @copydoc Toolkit::TableView::DeleteRow( unsigned int rowIndex, std::vector<Actor>& removed )
   */
  void DeleteRow( unsigned int rowIndex, std::vector<Actor>& removed );

  /**
   * @copydoc Toolkit::TableView::InsertColumn
   */
  void InsertColumn( unsigned int columnIndex );

  /**
   * @copydoc Toolkit::TableView::DeleteColumn( unsigned int columnIndex )
   */
  void DeleteColumn( unsigned int columnIndex );

  /**
   * @copydoc Toolkit::TableView::DeleteColumn( unsigned int columnIndex, std::vector<Actor>& removed )
   */
  void DeleteColumn( unsigned int columnIndex, std::vector<Actor>& removed );

  /**
   * @copydoc Toolkit::TableView::Resize( unsigned int rows, unsigned int columns )
   */
  void Resize( unsigned int rows, unsigned int columns );

  /**
   * @copydoc Toolkit::TableView::Resize( unsigned int rows, unsigned int columns, std::vector<Actor>& removed )
   */
  void Resize( unsigned int rows, unsigned int columns, std::vector<Actor>& removed );

  /**
   * @copydoc Toolkit::TableView::SetCellPadding
   */
  void SetCellPadding( Size padding );

  /**
   * @copydoc Toolkit::TableView::GetCellPadding
   */
  Size GetCellPadding();

  /**
   * @brief Set a row policy
   *
   * @param[in] rowIndex The row to set the policy for
   * @param[in] policy The policy to set
   */
  void SetRowPolicy( unsigned int rowIndex, CellSizePolicy policy );

  /**
   * @brief Querry a row policy
   *
   * @param[in] rowIndex The row to querry
   * @return Return the policy
   */
  CellSizePolicy GetRowPolicy( unsigned int rowIndex ) const;

  /**
   * @brief Set a column policy
   *
   * @param[in] columnIndex The column to set the policy for
   * @param[in] policy The policy to set
   */
  void SetColumnPolicy( unsigned int columnIndex, CellSizePolicy policy );

  /**
   * @brief Querry a column policy
   *
   * @param[in] columnIndex The column to querry
   * @return Return the policy
   */
  CellSizePolicy GetColumnPolicy( unsigned int columnIndex ) const;

  /**
   * @copydoc Toolkit::TableView::SetFixedWidth
   */
  void SetFixedWidth( unsigned int columnIndex, float width );

  /**
   * @copydoc Toolkit::TableView::GetFixedWidth
   */
  float GetFixedWidth( unsigned int columnIndex ) const;

  /**
   * @copydoc Toolkit::TableView::SetFixedHeight
   */
  void SetFixedHeight( unsigned int rowIndex, float height );

  /**
   * @copydoc Toolkit::TableView::GetFixedHeight
   */
  float GetFixedHeight( unsigned int rowIndex ) const;

  /**
   * @copydoc Toolkit::TableView::SetRelativeHeight
   */
  void SetRelativeHeight( unsigned int rowIndex, float heightPercentage );

  /**
   * @copydoc Toolkit::TableView::GetRelativeHeight
   */
  float GetRelativeHeight( unsigned int rowIndex ) const;

  /**
   * @copydoc Toolkit::TableView::SetRelativeWidth
   */
  void SetRelativeWidth( unsigned int columnIndex, float widthPercentage );

  /**
   * @copydoc Toolkit::TableView::GetRelativeWidth
   */
  float GetRelativeWidth( unsigned int columnIndex ) const;

  /**
   * @copydoc Toolkit::TableView::GetRows
   */
  unsigned int GetRows();

  /**
   * @copydoc Toolkit::TableView::GetColumns
   */
  unsigned int GetColumns();

  /**
   * @copydoc Toolkit::TableView::SetCellAlignment
   */
  void SetCellAlignment( Toolkit::TableView::CellPosition position, HorizontalAlignment::Type horizontal, VerticalAlignment::Type vertical );

  // Properties

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index index );

private: // From Control

  /**
   * @copydoc Control::OnControlChildAdd(Actor& child)
   */
  virtual void OnControlChildAdd( Actor& child );

  /**
   * @copydoc Control::OnControlChildRemove(Actor& child)
   */
  virtual void OnControlChildRemove( Actor& child );

  /**
   * @copydoc Control::OnRelayout
   */
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );

  /**
   * @copydoc Control::CalculateChildSize
   */
  virtual float CalculateChildSize( const Actor& child, Dimension::Type dimension );

  /**
   * @copydoc Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Control::GetNextKeyboardFocusableActor
   */
  virtual Actor GetNextKeyboardFocusableActor( Actor currentFocusedActor, Toolkit::Control::KeyboardFocusNavigationDirection direction, bool loopEnabled );

  /**
   * @copydoc Control::GetNaturalSize()
   */
  virtual Vector3 GetNaturalSize();

  /**
   * @copydoc Control::RelayoutDependentOnChildren()
   */
  virtual bool RelayoutDependentOnChildren( Dimension::Type dimension = Dimension::ALL_DIMENSIONS );

  /**
   * @copydoc Control::OnCalculateRelayoutSize
   */
  virtual void OnCalculateRelayoutSize( Dimension::Type dimension );

  /**
   * @copydoc Control::OnLayoutNegotiated
   */
  virtual void OnLayoutNegotiated( float size, Dimension::Type dimension );

private: // Implementation

  /**
   * Struct to hold data for rows and columns
   *
   * If sizePolicy is FIXED then size is the absolute size to use.
   * If sizePolicy is FIT or FILL then size is the calculated value of size.
   */
  struct RowColumnData
  {
    /**
     * Default constructor
     */
    RowColumnData()
    : size( 0.0f ),
      fillRatio( 0.0f ),
      sizePolicy( FILL ),
      userFillRatio( false )
    {
    }

    /**
     * Constructor
     *
     * @param[in] newSize The size to set for this data
     * @param[in] newSizePolicy The policy used to interpret the size value
     */
    RowColumnData( float newSize, float newFillRatio, CellSizePolicy newSizePolicy, bool newUserFillRatio )
    : size( newSize ),
      fillRatio( newFillRatio ),
      sizePolicy( newSizePolicy ),
      userFillRatio( newUserFillRatio )
    {
    }

    float size;                       ///< Set or calculated size
    float fillRatio;                  ///< Ratio to fill remaining space
    CellSizePolicy sizePolicy;        ///< The size policy used to interpret the size value
    bool userFillRatio : 1;           ///< FillRatio was set by user
  };

  typedef Dali::Vector<RowColumnData> RowColumnArray;

public:

  /**
   * Structure for the layout data
   */
  struct CellData
  {
    CellData()
    : horizontalAlignment( HorizontalAlignment::LEFT ),
      verticalAlignment( VerticalAlignment::TOP )
    {
    }

    // data members
    Dali::Actor actor;
    Toolkit::TableView::CellPosition position;
    HorizontalAlignment::Type horizontalAlignment;
    VerticalAlignment::Type verticalAlignment;
  };

private:

  /**
   * Construct a new TableView.
   */
  TableView( unsigned int initialRows, unsigned int initialColumns );

  /**
   * Resizes the data containers to match the new size
   * @param [in] rows in the table
   * @param [in] columns in the table
   */
  void ResizeContainers( unsigned int rows, unsigned int columns );

  /**
   * Resizes the data containers to match the new size
   * @param [in] rows in the table
   * @param [in] columns in the table
   * @param [out] removed celldata
   */
  void ResizeContainers( unsigned int rows, unsigned int columns, std::vector<CellData>& removed );

  /**
   * Helper to get the list of lost actors in the case when table looses cells.
   * Also handles the case when actors span multiple cells
   * @param lost cells
   * @param removed actors
   * @param rowsRemoved from table
   * @param columnsRemoved from table
   */
  void RemoveAndGetLostActors( const std::vector<CellData>& lost, std::vector<Actor>& removed,
      unsigned int rowsRemoved, unsigned int columnsRemoved );

  /**
   * Helper to remove all instances of the actor
   * @param child actor to remove
   * @return true if the actor was found
   */
  bool RemoveAllInstances( const Actor& child );

  /**
   * @brief Compute relative sizes for an array
   *
   * @param[in] data The RowColumn data to compute the relative sizes for
   */
  void ComputeRelativeSizes( RowColumnArray& data );

  /**
   * @brief Calculate the total fixed sizes for a row or column
   *
   * @param[in] data The row or column data to process
   */
  float CalculateTotalFixedSize( const RowColumnArray& data );

  /**
   * @brief Calculate the fixed sizes for a row or column
   *
   * @param[in] data The row or column data to process
   * @param[in] dimension The dimension being calculated: row == Dimension::HEIGHT, column == Dimension::WIDTH
   */
  void CalculateFixedSizes( RowColumnArray& data, Dimension::Type dimension );

  /**
   * @brief Calculate the value of the relative sizes
   *
   * @param[in] data The row or column data to process
   * @param[in] size The size of the table view in that dimension
   */
  void CalculateRelativeSizes( RowColumnArray& data, float size );

  /**
   * @brief Search for a FIT cell in the array
   *
   * @param[in] data The row or column data to process
   * @return Return if a FIT cell was found or not
   */
  bool FindFit( const RowColumnArray& data );

  /**
   * @brief Calculate row and column data when it is dirty
   */
  void CalculateRowColumnData();

  /**
   * @brief Return the cell padding for a given dimension
   *
   * @param[in] dimension The dimension to return the padding for
   * @return Return the padding (x = low, y = high)
   */
  Vector2 GetCellPadding( Dimension::Type dimension );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TableView();

private: // scripting support

  /**
   * Called to set the heights/widths property.
   * @param[in] tableViewImpl The object whose property is set.
   * @param[in] funcFixed The set function to call, it can be SetFixedHeight or SetFixedWidths.
   * @param[in] funcRelative The set function to call, it can be SetRelativeHeight or SetRelativeWidths.
   * @param[in] value The new property value.
   */
  static void SetHeightOrWidthProperty( TableView& tableViewImpl,
                                        void(TableView::*funcFixed)(unsigned int, float),
                                        void(TableView::*funcRelative)(unsigned int, float),
                                        const Property::Value& map );

  /**
   * Called to retrieve the property value of row heights.
   * @return The property value of row heights.
   */
  Property::Value GetRowHeightsPropertyValue();

  /**
   * Called to retrieve the property value of column widths.
   * @return The fixed-widths property value.
   */
  Property::Value GetColumnWidthsPropertyValue();

  /**
   * Generate the map type property value from the size vectors.
   * @param[in] data The array of row or column data
   * @param[out] map The property value.
   */
  void GetMapPropertyValue( const RowColumnArray& data, Property::Map& map );


  /**
   * Helper class to prevent child adds and removes from causing relayout
   * when we're already anyways going to do one in the end
   */
  class RelayoutingLock
  {
  public: // API

    /**
     * Constructor, sets the lock boolean
     */
    RelayoutingLock( TableView& parent )
    : mLock( parent.mLayoutingChild )
    {
      mLock = true;
    }

    /**
     * Destructor, releases lock boolean
     */
    ~RelayoutingLock()
    {
      mLock = false;
      // Note, we could also call Relayout here. This would save one line of code
      // from each method that uses this lock but destructors are not meant to do
      // big processing so better to not do it here. This destructor would also
      // be called in case of an exception and we don't definitely want to do Relayout
      // in that situation
    }

  private:
    bool& mLock;
  };

private:

  // Undefined copy constructor and assignment operators
  TableView(const TableView&);
  TableView& operator=(const TableView& rhs);

private: // Data

  Array2d<CellData> mCellData;                ///< Data for each cell: Actor, alignment settings etc

  RowColumnArray mRowData;       ///< Data for each row
  RowColumnArray mColumnData;    ///< Data for each column
  Size mFixedTotals;             ///< Accumulated totals for fixed width and height

  Size mPadding;                 ///< Padding to apply to each cell
  bool mLayoutingChild;          ///< Can't be a bitfield due to Relayouting lock
  bool mRowColumnDirty : 1;       ///< Flag to indicate the row column data is dirty
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::TableView& GetImpl( Toolkit::TableView& tableView )
{
  DALI_ASSERT_ALWAYS(tableView);

  Dali::RefObject& handle = tableView.GetImplementation();

  return static_cast<Toolkit::Internal::TableView&>(handle);
}

inline const Toolkit::Internal::TableView& GetImpl( const Toolkit::TableView& tableView )
{
  DALI_ASSERT_ALWAYS(tableView);

  const Dali::RefObject& handle = tableView.GetImplementation();

  return static_cast<const Toolkit::Internal::TableView&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TABLE_VIEW_H__
