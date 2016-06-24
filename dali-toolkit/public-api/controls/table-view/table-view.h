#ifndef __DALI_TOOLKIT_TABLE_VIEW_H__
#define __DALI_TOOLKIT_TABLE_VIEW_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/actors/actor-enumerations.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class TableView;
}
/**
 * @addtogroup dali_toolkit_controls_table_view
 * @{
 */

/**
 * @brief TableView is a layout container for aligning child actors in a grid like layout.
 *
 * TableView constrains the x and y position and width and height of the child actors.
 * z position and depth are left intact so that 3D model actors can also be laid out
 * in a grid without loosing their depth scaling.
 *
 * @nosubgrouping
 * <h3>Per-child Custom properties for script supporting:</h3>
 *
 * When an actor is add to the tableView through Actor::Add() instead of TableView::AddChild,
 * the following custom properties of the actor are checked to decide the actor position inside the table.
 *
 * These properties are registered dynamically to the child and is non-animatable.
 *
 * | %Property Name          | Type        |
 * |-------------------------|-------------|
 * | cellIndex               | Vector2     |
 * | rowSpan                 | float       |
 * | columnSpan              | float       |
 * | cellHorizontalAlignment | string      |
 * | cellVerticalAlignment   | string      |
 *
 * The row-span or column span has integer value, but its type is float here due to the limitation of the builder's ability to differentiate integer and float from Json string.
 * The available values for cellHorizontalAlignment are: left, center, right.
 * The available values for cellVerticalAlignment are: top, center, bottom.
 *
 * @code
 * "name":"gallery1",
 * "type":"ImageView",
 * "image": {
 *    "url": "{DALI_IMAGE_DIR}gallery-small-1.jpg"
 *  },
 *  "properties": {
 *     "cellIndex":[1,1],  // property to specify the top-left cell this child occupies, if not set, the first available cell is used
 *     "rowSpan":3,        // property to specify how many rows this child occupies, if not set, default value is 1
 *     "columnSpan": 2,    // property to specify how many columns this child occupies, if nor set, default value is 1
 *     "cellHorizontalAlignment": "left", // property to specify how to align horizontally inside the cells, if not set, default value is 'left'
 *     "cellVerticalAlignment": "center"  // property to specify how to align vertically inside the cells, if not set, default value is 'top'
 *   }
 * @endcode
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API TableView : public Control
{
public:

  /**
   * @brief The start and end property ranges for this control.
   * @SINCE_1_0.0
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1, ///< @SINCE_1_0.0
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000,             ///< Reserve property indices @SINCE_1_0.0

    CHILD_PROPERTY_START_INDEX = CHILD_PROPERTY_REGISTRATION_START_INDEX,         ///< @SINCE_1_1.36
    CHILD_PROPERTY_END_INDEX =   CHILD_PROPERTY_REGISTRATION_START_INDEX + 1000   ///< Reserve child property indices @SINCE_1_1.36
  };

  /**
   * @brief An enumeration of properties belonging to the TableView class.
   *
   * LayoutRows: set the height of the rows.
   * It has the format as follows in script:
   * @code
   * "layoutRows":
   *       {
   *         "0": { "policy": "fixed", "value": 40 },       //@see SetFixedHight
   *         "2": { "policy": "relative", "value": 0.33 },  //@see SetRelativeHeight
   *         "3": { "policy": "fit", "value":0.0 }          //@see SetFitHeight, the value is not used, its height is decided by the children in this row
   *       }
   * @endcode
   *
   * LayoutColumns: set the height of the rows.
   * It has the format as follows in script:
   * @code
   * "layoutColumns":
   *       {
   *         "0": { "policy": "fixed", "value": 40 },       //@see SetFixedWidth
   *         "1": { "policy": "fit", "value":0.0 }          //@see SetFitHeight, the value is not used, its width is decided by the children in this column
   *         "2": { "policy": "relative", "value": 0.33 }   //@see SetRelativeWidth
   *       }
   * @endcode
   * @SINCE_1_0.0
   */
  struct Property
  {
    enum
    {
      ROWS = PROPERTY_START_INDEX, ///< name "rows",           type unsigned int @SINCE_1_0.0
      COLUMNS,                     ///< name "columns",        type unsigned int @SINCE_1_0.0
      CELL_PADDING,                ///< name "cellPadding",    type Vector2 @SINCE_1_0.0
      LAYOUT_ROWS,                 ///< name "layoutRows",     type Map @SINCE_1_0.0
      LAYOUT_COLUMNS,              ///< name "layoutColumns",  type Map @SINCE_1_0.0
    };
  };

  /**
   * @brief An enumeration of child properties belonging to the TableView class.
   * @SINCE_1_1.36
   */
  struct ChildProperty
  {
    enum
    {
      CELL_INDEX = CHILD_PROPERTY_START_INDEX,  ///< name "cellIndex",                The top-left cell this child occupies, if not set, the first available cell is used,           type VECTOR2 @SINCE_1_1.36
      ROW_SPAN,                                 ///< name "rowSpan",                  The number of rows this child occupies, if not set, default value is 1,                        type FLOAT @SINCE_1_1.36
      COLUMN_SPAN,                              ///< name "columnSpan",               The number of columns this child occupies, if not set, default value is 1,                     type FLOAT @SINCE_1_1.36
      CELL_HORIZONTAL_ALIGNMENT,                ///< name "cellHorizontalAlignment",  The horizontal alignment of this child inside the cells, if not set, default value is 'left',  type STRING @SINCE_1_1.36
      CELL_VERTICAL_ALIGNMENT                   ///< name "cellVerticalAlignment",    The vertical alignment of this child inside the cells, if not set, default value is 'top',     type STRING @SINCE_1_1.36
    };
  };

  /**
   * @brief Describes how the size of a row / column been set
   * @SINCE_1_0.0
   */
  enum LayoutPolicy
  {
    FIXED,      ///< Fixed with the given value. @SINCE_1_0.0
    RELATIVE,   ///< Calculated as percentage of the remainder after subtracting Padding and Fixed height/width @SINCE_1_0.0
    FILL,       ///< Default policy, get the remainder of the 100% (after subtracting Fixed, Fit and Relative height/ width) divided evenly between 'fill' rows/columns @SINCE_1_0.0
    FIT         ///< Fit around its children. @SINCE_1_0.0
  };

  /**
   * @brief Structure to specify layout position for child actor
   * @SINCE_1_0.0
   */
  struct CellPosition
  {
    /**
     * @brief Constructor to initialise values to defaults for convenience
     * @SINCE_1_0.0
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
   * @brief Create a TableView handle; this can be initialised with TableView::New()
   * Calling member functions with an uninitialised handle is not allowed.
   * @SINCE_1_0.0
   */
  TableView();

  /**
   * @brief Copy constructor. Creates another handle that points to the same real object
   * @SINCE_1_0.0
   * @param handle to copy from
   */
  TableView( const TableView& handle );

  /**
   * @brief Assignment operator. Changes this handle to point to another real object
   * @SINCE_1_0.0
   */
  TableView& operator=( const TableView& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~TableView();

  /**
   * @brief Create the TableView control.
   * @SINCE_1_0.0
   * @param[in] initialRows for the table
   * @param[in] initialColumns for the table
   * @return A handle to the TableView control.
   */
  static TableView New( unsigned int initialRows, unsigned int initialColumns );

  /**
   * @brief Downcast a handle to TableView handle.
   *
   * If handle points to a TableView the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return handle to a TableView or an uninitialized handle
   */
  static TableView DownCast( BaseHandle handle );

  /**
   * @brief Adds a child to the table
   * If the row or column index is outside the table, the table gets resized bigger
   * @SINCE_1_0.0
   * @param[in] child to add
   * @param[in] position for the child
   * @return true if the addition succeeded, false if the cell is already occupied
   * @pre The child actor has been initialized.
   */
  bool AddChild( Actor child, CellPosition position );

  /**
   * @brief Returns a child from the given layout position
   * @SINCE_1_0.0
   * @param[in] position in the table
   * @return child that was in the cell or an uninitialized handle
   * @note If there is no child in this position this method returns an uninitialized.
   * Actor handle
   */
  Actor GetChildAt( CellPosition position );

  /**
   * @brief Removes a child from the given layout position
   * @SINCE_1_0.0
   * @param[in] position for the child to remove
   * @return child that was removed or an uninitialized handle
   * @note If there is no child in this position this method does nothing.
   */
  Actor RemoveChildAt( CellPosition position );

  /**
   * @brief Finds the childs layout position
   * @SINCE_1_0.0
   * @param[in] child to search for
   * @param[out] position for the child
   * @return true if the child was included in this TableView
   */
  bool FindChildPosition( Actor child, CellPosition& position );

  /**
   * @brief Insert a new row to given index
   * @SINCE_1_0.0
   * @param [in] rowIndex of the new row
   */
  void InsertRow( unsigned int rowIndex );

  /**
   * @brief Delete a row from given index
   * Removed elements are deleted
   * @SINCE_1_0.0
   * @param [in] rowIndex of the row to delete
   */
  void DeleteRow( unsigned int rowIndex );

  /**
   * @brief Delete a row from given index
   * @SINCE_1_0.0
   * @param [in] rowIndex of the row to delete
   * @param [out] removed elements
   */
  void DeleteRow( unsigned int rowIndex, std::vector<Actor>& removed );

  /**
   * @brief Insert a new column to given index
   * @SINCE_1_0.0
   * @param [in] columnIndex of the new column
   */
  void InsertColumn( unsigned int columnIndex );

  /**
   * @brief Delete a column from given index.
   * Removed elements are deleted
   * @SINCE_1_0.0
   * @param [in] columnIndex of the column to delete
   */
  void DeleteColumn( unsigned int columnIndex );

  /**
   * @brief Delete a column from given index
   * @SINCE_1_0.0
   * @param [in] columnIndex of the column to delete
   * @param [out] removed elements
   */
  void DeleteColumn( unsigned int columnIndex, std::vector<Actor>& removed );

  /**
   * @brief Resize the TableView.
   * @SINCE_1_0.0
   * @param[in] rows for the table
   * @param[in] columns for the table
   * @note If the new size is smaller than old,
   * superfluous actors get removed. If you want to relayout removed children,
   * use the variant that returns the removed Actors and reinsert them into the table.
   * If an actor spans to a removed row or column it gets removed from the table.
   */
  void Resize( unsigned int rows, unsigned int columns );

  /**
   * @brief Resize the TableView.
   * @SINCE_1_0.0
   * @param[in] rows for the table
   * @param[in] columns for the table
   * @param[out] removed actor handles
   * @note If the new size is smaller than old,
   * superfluous actors get removed.
   * If an actor spans to a removed row or column it gets removed from the table.
   */
  void Resize( unsigned int rows, unsigned int columns, std::vector<Actor>& removed );

  /**
   * @brief Set horizontal and vertical padding between cells
   * @SINCE_1_0.0
   * @param[in] padding width and height
   */
  void SetCellPadding( Size padding );

  /**
   * @brief Get the current padding as width and height.
   * @SINCE_1_0.0
   * @return the current padding as width and height
   */
  Size GetCellPadding();

  /**
   * @brief Specify this row as fitting its height to its children
   *
   * @SINCE_1_0.0
   * @param[in] rowIndex The row to set
   */
  void SetFitHeight( unsigned int rowIndex );

  /**
   * @brief Is the row a fit row
   *
   * @SINCE_1_0.0
   * @param[in] rowIndex The row to check
   * @return Return true if the row is fit
   */
  bool IsFitHeight( unsigned int rowIndex ) const;

  /**
   * @brief Specify this column as fitting its width to its children
   *
   * @SINCE_1_0.0
   * @param[in] columnIndex The column to set
   */
  void SetFitWidth( unsigned int columnIndex );

  /**
   * @brief Is the column a fit column
   *
   * @SINCE_1_0.0
   * @param[in] columnIndex The column to check
   * @return Return true if the column is fit
   */
  bool IsFitWidth( unsigned int columnIndex ) const;

  /**
   * @brief Sets a row to have fixed height
   * Setting a fixed height of 0 has no effect
   * @SINCE_1_0.0
   * @param rowIndex for row with fixed height
   * @param height in world coordinate units
   * @pre The row rowIndex must exist.
   */
  void SetFixedHeight( unsigned int rowIndex, float height );

  /**
   * @brief Gets a row's fixed height.
   * @SINCE_1_0.0
   * @return height in world coordinate units.
   * @pre The row rowIndex must exist.
   * @note The returned value is valid if it has been set before.
   */
  float GetFixedHeight( unsigned int rowIndex ) const;

  /**
   * @brief Sets a row to have relative height. Relative height means percentage of
   * the remainder of the table height after subtracting Padding and Fixed height rows
   * Setting a relative height of 0 has no effect
   * @SINCE_1_0.0
   * @param rowIndex for row with relative height
   * @param heightPercentage between 0.0f and 1.0f
   * @pre The row rowIndex must exist.
   */
  void SetRelativeHeight( unsigned int rowIndex, float heightPercentage );

  /**
   * @brief Gets a row's relative height.
   * @SINCE_1_0.0
   * @return height in percentage units, between 0.0f and 1.0f.
   * @pre The row rowIndex must exist.
   * @note The returned value is valid if it has been set before.
   */
  float GetRelativeHeight( unsigned int rowIndex ) const;

  /**
   * @brief Sets a column to have fixed width
   * Setting a fixed width of 0 has no effect
   * @SINCE_1_0.0
   * @param columnIndex for column with fixed width
   * @param width in world coordinate units
   * @pre The column columnIndex must exist.
   */
  void SetFixedWidth( unsigned int columnIndex, float width );

  /**
   * @brief Gets a column's fixed width.
   * @SINCE_1_0.0
   * @return width in world coordinate units.
   * @pre The column columnIndex must exist.
   * @note The returned value is valid if it has been set before.
   */
  float GetFixedWidth( unsigned int columnIndex ) const;

  /**
   * @brief Sets a column to have relative width. Relative width means percentage of
   * the remainder of table width after subtracting Padding and Fixed width columns
   * Setting a relative width of 0 has no effect
   * @SINCE_1_0.0
   * @param columnIndex for column with fixed width
   * @param widthPercentage between 0.0f and 1.0f
   * @pre The column columnIndex must exist.
   */
  void SetRelativeWidth( unsigned int columnIndex, float widthPercentage );

  /**
   * @brief Gets a column's relative width.
   * @SINCE_1_0.0
   * @return width in percentage units, between 0.0f and 1.0f.
   * @pre The column columnIndex must exist.
   * @note The returned value is valid if it has been set before.
   */
  float GetRelativeWidth( unsigned int columnIndex ) const;

  /**
   * @brief Gets the amount of rows in the table.
   * @SINCE_1_0.0
   * @return the amount of rows in the table
   */
  unsigned int GetRows();

  /**
   * @brief Gets the amount of columns in the table.
   * @SINCE_1_0.0
   * @return the amount of columns in the table
   */
  unsigned int GetColumns();

  /**
   * @brief Set the alignment on a cell.
   *
   * Cells without calling this function have the default values of LEFT and TOP respectively.
   *
   * @SINCE_1_0.0
   * @param[in] position The cell to set alignment on.
   * @param[in] horizontal The horizontal alignment.
   * @param[in] vertical The vertical alignment.
   */
  void SetCellAlignment( CellPosition position, HorizontalAlignment::Type horizontal, VerticalAlignment::Type vertical );

public: // Not intended for application developers

  /**
   * @internal
   * @brief Creates a handle using the Toolkit::Internal implementation.
   * @SINCE_1_0.0
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL TableView(Internal::TableView& implementation);

  /**
   * @internal
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   * @SINCE_1_0.0
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL TableView( Dali::Internal::CustomActor* internal );
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TABLE_VIEW_H__
