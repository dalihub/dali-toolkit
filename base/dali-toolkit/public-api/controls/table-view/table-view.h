#ifndef __DALI_TOOLKIT_TABLE_VIEW_H__
#define __DALI_TOOLKIT_TABLE_VIEW_H__

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
#include <dali/public-api/common/dali-vector.h>
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class TableView;
}

/**
 * TableView is a layout container for aligning child actors in a grid like layout.
 * TableView constrains the x and y position and width and height of the child actors.
 * z position and depth are left intact so that 3D model actors can also be laid out
 * in a grid without loosing their depth scaling.
 */
class TableView : public Control
{
public:

  /**
   * Structure to specify layout position for child actor
   */
  struct CellPosition
  {
    /**
     * Constructor to initialise values to defaults for convenience
     */
    CellPosition( unsigned int rowIndex = 0, unsigned int columnIndex = 0,
                    unsigned int rowSpan = 1, unsigned int columnSpan = 1 )
    : rowIndex( rowIndex ), columnIndex( columnIndex ),
      rowSpan( rowSpan ), columnSpan( columnSpan )
    { }

    unsigned int rowIndex;
    unsigned int columnIndex;
    unsigned int rowSpan;
    unsigned int columnSpan;
  };

  /**
   * Create a TableView handle; this can be initialised with TableView::New()
   * Calling member functions with an uninitialised handle is not allowed.
   */
  TableView();

  /**
   * Copy constructor. Creates another handle that points to the same real object
   * @param handle to copy from
   */
  TableView( const TableView& handle );

  /**
   * Assignment operator. Changes this handle to point to another real object
   */
  TableView& operator=( const TableView& handle );

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~TableView();

  /**
   * Create the TableView control.
   * @param[in] initialRows for the table
   * @param[in] initialColumns for the table
   * @return A handle to the TableView control.
   */
  static TableView New( unsigned int initialRows, unsigned int initialColumns );

  /**
   * Downcast an Object handle to TableView. If handle points to a TableView the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a TableView or an uninitialized handle
   */
  static TableView DownCast( BaseHandle handle );

  /**
   * Adds a child to the table
   * If the row or column index is outside the table, the table gets resized bigger
   * @pre The child actor has been initialized.
   * @param[in] child to add
   * @param[in] position for the child
   * @return true if the addition succeeded, false if the cell is already occupied
   */
  bool AddChild( Actor child, CellPosition position );

  /**
   * Returns a child from the given layout position
   * Note! if there is no child in this position this method returns an uninitialized
   * Actor handle
   * @param[in] position in the table
   * @return child that was in the cell or an uninitialized handle
   */
  Actor GetChildAt( CellPosition position );

  /**
   * Removes a child from the given layout position
   * Note! if there is no child in this position this method does nothing
   * @param[in] position for the child to remove
   * @return child that was removed or an uninitialized handle
   */
  Actor RemoveChildAt( CellPosition position );

  /**
   * Finds the childs layout position
   * @param[in] child to search for
   * @param[out] position for the child
   * @return true if the child was included in this TableView
   */
  bool FindChildPosition( Actor child, CellPosition& position );

  /**
   * Insert a new row to given index
   * @param [in] rowIndex of the new row
   */
  void InsertRow( unsigned int rowIndex );

  /**
   * Delete a row from given index
   * Removed elements are deleted
   * @param [in] rowIndex of the row to delete
   */
  void DeleteRow( unsigned int rowIndex );

  /**
   * Delete a row from given index
   * @param [in] rowIndex of the row to delete
   * @param [out] removed elements
   */
  void DeleteRow( unsigned int rowIndex, std::vector<Actor>& removed );

  /**
   * Insert a new column to given index
   * @param [in] columnIndex of the new column
   */
  void InsertColumn( unsigned int columnIndex );

  /**
   * Delete a column from given index.
   * Removed elements are deleted
   * @param [in] columnIndex of the column to delete
   */
  void DeleteColumn( unsigned int columnIndex );

  /**
   * Delete a column from given index
   * @param [in] columnIndex of the column to delete
   * @param [out] removed elements
   */
  void DeleteColumn( unsigned int columnIndex, std::vector<Actor>& removed );

  /**
   * Resize the TableView. Note! if the new size is smaller than old,
   * superfluous actors get removed. If you want to relayout removed children,
   * use the variant that returns the removed Actors and reinsert them into the table
   * If an actor spans to a removed row or column it gets removed from the table
   * @param[in] rows for the table
   * @param[in] columns for the table
   */
  void Resize( unsigned int rows, unsigned int columns );

  /**
   * Resize the TableView. Note! if the new size is smaller than old,
   * superfluous actors get removed.
   * If an actor spans to a removed row or column it gets removed from the table
   * @param[in] rows for the table
   * @param[in] columns for the table
   * @param[out] removed actor handles
   */
  void Resize( unsigned int rows, unsigned int columns, std::vector<Actor>& removed );

  /**
   * Set horizontal and vertical padding between cells
   * @param[in] padding width and height
   */
  void SetCellPadding( Size padding );

  /**
   * @return the current padding as width and height
   */
  Size GetCellPadding();

  /**
   * Sets a row to have fixed height
   * Setting a fixed height of 0 has no effect
   * @pre The row rowIndex must exist.
   * @param rowIndex for row with fixed height
   * @param height in world coordinate units
   */
  void SetFixedHeight( unsigned int rowIndex, float height );

  /**
   * Gets a row's fixed height.
   * Note! The returned value is valid if it has been set before.
   * @pre The row rowIndex must exist.
   * @return height in world coordinate units.
   */
  float GetFixedHeight( unsigned int rowIndex ) const;

  /**
   * Sets a row to have relative height. Relative height means percentage of
   * the remainder of the table height after subtracting Padding and Fixed height rows
   * Setting a relative height of 0 has no effect
   * @pre The row rowIndex must exist.
   * @param rowIndex for row with relative height
   * @param heightPercentage between 0.0f and 1.0f
   */
  void SetRelativeHeight( unsigned int rowIndex, float heightPercentage );

  /**
   * Gets a row's relative height.
   * Note! The returned value is valid if it has been set before.
   * @pre The row rowIndex must exist.
   * @return height in percentage units, between 0.0f and 1.0f.
   */
  float GetRelativeHeight( unsigned int rowIndex ) const;

  /**
   * Sets a column to have fixed width
   * Setting a fixed width of 0 has no effect
   * @pre The column columnIndex must exist.
   * @param columnIndex for column with fixed width
   * @param width in world coordinate units
   */
  void SetFixedWidth( unsigned int columnIndex, float width );

  /**
   * Gets a column's fixed width.
   * Note! The returned value is valid if it has been set before.
   * @pre The column columnIndex must exist.
   * @return width in world coordinate units.
   */
  float GetFixedWidth( unsigned int columnIndex ) const;

  /**
   * Sets a column to have relative width. Relative width means percentage of
   * the remainder of table width after subtracting Padding and Fixed width columns
   * Setting a relative width of 0 has no effect
   * @pre The column columnIndex must exist.
   * @param columnIndex for column with fixed width
   * @param widthPercentage between 0.0f and 1.0f
   */
  void SetRelativeWidth( unsigned int columnIndex, float widthPercentage );

  /**
   * Gets a column's relative width.
   * Note! The returned value is valid if it has been set before.
   * @pre The column columnIndex must exist.
   * @return width in percentage units, between 0.0f and 1.0f.
   */
  float GetRelativeWidth( unsigned int columnIndex ) const;

  /**
   * Sets the layout animation duration for add, remove and relayout
   * @param duration for the layout animations
   * @note The default duration is 0.0f.
   */
  void SetLayoutAnimationDuration( float duration );

  /**
   * Gets the layout animation duration for add, remove and relayout
   * @return duration for the layout animations
   */
  float GetLayoutAnimationDuration();

  /**
   * @return the amount of rows in the table
   */
  unsigned int GetRows();

  /**
   * @return the amount of columns in the table
   */
  unsigned int GetColumns();

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  TableView(Internal::TableView& implementation);

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  TableView( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TABLE_VIEW_H__
