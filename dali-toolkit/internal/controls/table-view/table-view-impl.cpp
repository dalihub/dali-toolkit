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

// CLASS HEADER
#include <dali-toolkit/internal/controls/table-view/table-view-impl.h>

// EXTERNAL INCLUDES
#include <sstream>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <dali/integration-api/debug.h>

using namespace Dali;

namespace
{

/**
 * @brief Should the tableview fit around the given actor
 *
 * @param[in] actor The child actor to test against
 * @param[dimension] The dimnesion to test against
 */
bool FitToChild( Actor actor, Dimension::Type dimension )
{
  return actor.GetResizePolicy( dimension ) != ResizePolicy::FILL_TO_PARENT && actor.GetRelayoutSize( dimension ) > 0.0f;
}

#if defined(DEBUG_ENABLED)
// debugging support, very useful when new features are added or bugs are hunted down
// currently not called from code so compiler will optimize these away, kept here for future debugging

#define TABLEVIEW_TAG "DALI Toolkit::TableView "
#define TV_LOG(fmt, args...) Debug::LogMessage(Debug::DebugInfo, TABLEVIEW_TAG fmt, ## args)

void PrintArray( Array2d<Dali::Toolkit::Internal::TableView::CellData>& array )
{
  TV_LOG( "Array2d<CellData> size [%d,%d] \n", array.GetRows(), array.GetColumns() );
  // print values
  for( unsigned int i = 0; i < array.GetRows(); ++i )
  {
    for( unsigned int j = 0; j < array.GetColumns(); ++j )
    {
      Dali::Toolkit::Internal::TableView::CellData data = array[i][j];
      char actor = ' ';
      std::string actorName;
      if( data.actor )
      {
        actor = 'A';
        actorName = data.actor.GetName();
      }
      TV_LOG("Array[%d,%d]=%c %s %d,%d,%d,%d  ", i, j, actor, actorName.c_str(),
          data.position.rowIndex, data.position.columnIndex,
          data.position.rowSpan, data.position.columnSpan );
    }
    TV_LOG( "\n" );
  }
}

// debugging support, very useful when new features are added or bugs are hunted down
// currently not called from code so compiler will optimize these away, kept here for future debugging
void PrintArray( Array2d<Size>& array )
{
  TV_LOG( "Array2d<Size> size [%d,%d] \n", array.GetRows(), array.GetColumns() );
  // print values
  for( unsigned int i = 0; i < array.GetRows(); ++i )
  {
    for( unsigned int j = 0; j < array.GetColumns(); ++j )
    {
      TV_LOG( "Array[%d,%d]=%.2f,%.2f ", i, j, array[i][j].width, array[i][j].height );
    }
    TV_LOG( "\n" );
  }
}
// debugging support, very useful when new features are added or bugs are hunted down
// currently not called from code so compiler will optimize these away, kept here for future debugging
void PrintVector( std::vector<float>& array )
{
  TV_LOG( "vector, size [%d]\n", array.size() );
  // print values
  for( unsigned int i = 0; i < array.size(); ++i )
  {
    TV_LOG( "vector[%d]=%.2f ", i, array[i] );
  }
  TV_LOG( "\n" );
}
#endif // defined(DEBUG_ENABLED)

} // namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

// Type registration
BaseHandle Create()
{
  return Toolkit::TableView::New( 0, 0 );
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::TableView, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, TableView, "rows",           UNSIGNED_INTEGER, ROWS           )
DALI_PROPERTY_REGISTRATION( Toolkit, TableView, "columns",        UNSIGNED_INTEGER, COLUMNS        )
DALI_PROPERTY_REGISTRATION( Toolkit, TableView, "cell-padding",   VECTOR2,          CELL_PADDING   )
DALI_PROPERTY_REGISTRATION( Toolkit, TableView, "layout-rows",    MAP,              LAYOUT_ROWS    )
DALI_PROPERTY_REGISTRATION( Toolkit, TableView, "layout-columns", MAP,              LAYOUT_COLUMNS )

DALI_TYPE_REGISTRATION_END()

const Scripting::StringEnum< Toolkit::TableView::LayoutPolicy > LAYOUT_POLICY_STRING_TABLE[] =
{
 { "fixed",    Toolkit::TableView::FIXED    },
 { "relative", Toolkit::TableView::RELATIVE },
 { "fill",     Toolkit::TableView::FILL     }
};

const unsigned int LAYOUT_POLICY_STRING_TABLE_COUNT = sizeof(LAYOUT_POLICY_STRING_TABLE) / sizeof( LAYOUT_POLICY_STRING_TABLE[0] );

} // Unnamed namespace

Toolkit::TableView TableView::New( unsigned int initialRows, unsigned int initialColumns )
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< TableView > impl = new TableView( initialRows, initialColumns );

  // Pass ownership to CustomActor handle
  Toolkit::TableView handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

bool TableView::AddChild( Actor& child, const Toolkit::TableView::CellPosition& position )
{
  // check that the child is valid
  DALI_ASSERT_ALWAYS( child );

  // if child is already parented, we adopt it
  if( child.GetParent() )
  {
    child.GetParent().Remove( child );
  }

  // check if we need to expand our data array
  if( position.rowIndex >= mCellData.GetRows() )
  {
    // only adding new rows
    ResizeContainers( position.rowIndex + 1, mCellData.GetColumns() );
  }

  if( position.columnIndex >= mCellData.GetColumns() )
  {
    // only adding new columns
    ResizeContainers( mCellData.GetRows(), position.columnIndex + 1 );
  }

  // check if there already is something in this cell
  if( mCellData[ position.rowIndex ][ position.columnIndex ].actor )
  {
    return false; // cannot share a cell, it would complicate all logic and not bring much benefit
  }

  RelayoutingLock lock( *this );
  // adopt the child
  Self().Add( child );

  // if child spans multiple rows of columns
  if( ( position.rowSpan > 1 ) && ( position.rowIndex + position.rowSpan > mCellData.GetRows() ) )
  {
    // increase table size for the full span, only increasing rows
    ResizeContainers( position.rowIndex + position.rowSpan, mCellData.GetColumns() );
  }

  if( ( position.columnSpan > 1 ) && ( position.columnIndex + position.columnSpan > mCellData.GetColumns() ) )
  {
    // increase table size for the full span, only increasing columns
    ResizeContainers( mCellData.GetRows(), position.columnIndex + position.columnSpan );
  }

  // Fill in all cells that need the data
  CellData data;
  data.actor = child;
  data.position = position;

  for( unsigned int row = position.rowIndex; row < ( position.rowIndex + position.rowSpan ); ++row )
  {
    // store same information to all cells, this way we can identify
    // if a cell is the prime location of an actor or a spanned one
    for( unsigned int column = position.columnIndex; column < ( position.columnIndex + position.columnSpan ); ++column )
    {
      // store same information to all cells, this way we can identify
      // if a cell is the prime location of an actor or a spanned one
      mCellData[ row ][ column ] = data;
    }
  }

  // Relayout the whole table
  RelayoutRequest();

  return true;    // Addition successful
}

Actor TableView::GetChildAt( const Toolkit::TableView::CellPosition& position )
{
  if( ( position.rowIndex < mCellData.GetRows() ) && ( position.columnIndex < mCellData.GetColumns() ) )
  {
    return mCellData[ position.rowIndex ][ position.columnIndex ].actor;
  }

  // Return an empty handle
  return Actor();
}

Actor TableView::RemoveChildAt( const Toolkit::TableView::CellPosition& position )
{
  // get the child handle
  Actor child = GetChildAt( position );
  // if no real actor there, nothing else to be done
  if( child )
  {
    RelayoutingLock lock( *this );
    // Remove the child, this will trigger a call to OnControlChildRemove
    Self().Remove( child );

    // relayout the table only if instances were found
    if( RemoveAllInstances( child ) )
    {
      RelayoutRequest();
    }
  }
  // return the child back to caller
  return child;
}

bool TableView::FindChildPosition( const Actor& child, Toolkit::TableView::CellPosition& positionOut )
{
  // Only find valid child actors
  if( child )
  {
    // Walk through the layout data
    const unsigned int rowCount = mCellData.GetRows();
    const unsigned int columnCount = mCellData.GetColumns();

    for( unsigned int row = 0; row < rowCount; ++row )
    {
      for( unsigned int column = 0; column < columnCount; ++column )
      {
        if( mCellData[ row ][ column ].actor == child )
        {
          positionOut = mCellData[ row ][ column ].position;
          return true;
        }
      }
    }
  }

  return false;
}

void TableView::InsertRow( unsigned int rowIndex )
{
  RelayoutingLock lock( *this );

  mCellData.InsertRow( rowIndex );

  // Need to update the cell infos for the items that moved
  const unsigned int rowCount = mCellData.GetRows();
  const unsigned int columnCount = mCellData.GetColumns();

  for( unsigned int row = 0; row < rowCount; ++row )
  {
    for( unsigned int column = 0; column < columnCount; ++column )
    {
      Toolkit::TableView::CellPosition& position = mCellData[ row ][ column ].position;

      // If cell is spanning and above and spans to inserted row
      if( ( position.rowSpan > 1 ) && ( position.rowIndex <= rowIndex ) &&
          ( position.rowIndex + position.rowSpan > rowIndex ) )
      {
        // Increment span
        position.rowSpan++;

        // Copy cell to occupy the new column
        mCellData[ rowIndex ][ column ] = mCellData[ row ][ column ];
      }
      else if( row > rowIndex )   // If below of inserted row, increase row index
      {
        // Increment index
        position.rowIndex++;
      }
    }
  }

  // Expand row data array
  mRowData.Insert( mRowData.Begin() + rowIndex, RowColumnData() );

  // Sizes may have changed, so relayout
  mRowColumnDirty = true;
  RelayoutRequest();
}

void TableView::DeleteRow( unsigned int rowIndex )
{
  std::vector< Actor > ignored;
  DeleteRow( rowIndex, ignored );
}

void TableView::DeleteRow( unsigned int rowIndex, std::vector<Actor>& removed )
{
  RelayoutingLock lock( *this );

  // Delete the row
  std::vector< CellData > lost;
  mCellData.DeleteRow( rowIndex, lost );

  // Need to update the cell infos for the items that moved
  const unsigned int rowCount = mCellData.GetRows();
  const unsigned int columnCount = mCellData.GetColumns();

  for( unsigned int row = 0; row < rowCount; ++row )
  {
    for( unsigned int column = 0; column < columnCount; ++column )
    {
      Toolkit::TableView::CellPosition& position = mCellData[ row ][ column ].position;

      // If cell is spanning and above and spans to deleted row
      if( ( position.rowSpan > 1 ) && ( position.rowIndex <= rowIndex ) &&
          ( position.rowIndex + position.rowSpan > rowIndex ) )
      {
        // Decrement span
        if( position.rowSpan > 1 )
        {
          position.rowSpan--;
        }
      }
      else if( row >= rowIndex )    // If below of or at the inserted row, decrease row index
      {
        // Decrement index
        if( position.rowIndex > 1 )
        {
          position.rowIndex--;
        }
      }
    }
  }

  // 1 row removed, 0 columns
  RemoveAndGetLostActors( lost, removed, 1u, 0u );

  // Contract row data array
  mRowData.Erase( mRowData.Begin() + rowIndex );

  // Sizes may have changed, so relayout
  mRowColumnDirty = true;
  RelayoutRequest();
}

void TableView::InsertColumn( unsigned int columnIndex )
{
  RelayoutingLock lock( *this );

  // Insert the new column
  mCellData.InsertColumn( columnIndex );

  // Need to update the cell infos for the items that moved
  const unsigned int rowCount = mCellData.GetRows();
  const unsigned int columnCount = mCellData.GetColumns();

  for( unsigned int row = 0; row < rowCount; ++row )
  {
    for( unsigned int column = 0; column < columnCount; ++column )
    {
      Toolkit::TableView::CellPosition& position = mCellData[ row ][ column ].position;

      // If cell is spanning and left side and spans to inserted column
      if( ( position.columnSpan > 1 ) && ( position.columnIndex <= columnIndex ) &&
          ( position.columnIndex + position.columnSpan > columnIndex ) )
      {
        // Increment span
        position.columnSpan++;

        // Copy cell to occupy the new column
        mCellData[ row ][ columnIndex ] = mCellData[ row ][ column ];
      }
      else if( column > columnIndex )   // If on the right side of inserted column, increase column index
      {
        // Increment index
        position.columnIndex++;
      }
    }
  }

  // Expand column data array
  mColumnData.Insert( mColumnData.Begin() + columnIndex, RowColumnData() );

  // Sizes may have changed so relayout
  mRowColumnDirty = true;
  RelayoutRequest();
}

void TableView::DeleteColumn( unsigned int columnIndex )
{
  std::vector< Actor > ignored;
  DeleteColumn( columnIndex, ignored );
}

void TableView::DeleteColumn( unsigned int columnIndex, std::vector<Actor>& removed )
{
  RelayoutingLock lock( *this );

  // Remove the column
  std::vector< CellData > lost;
  mCellData.DeleteColumn( columnIndex, lost );

  // Need to update the cell infos for the items that moved
  const unsigned int rowCount = mCellData.GetRows();
  const unsigned int columnCount = mCellData.GetColumns();

  for( unsigned int row = 0; row < rowCount; ++row )
  {
    for( unsigned int column = 0; column < columnCount; ++column )
    {
      Toolkit::TableView::CellPosition& position = mCellData[ row ][ column ].position;

      // If cell is spanning and left side and spans to inserted column
      if( ( position.columnSpan > 1 ) && ( position.columnIndex <= columnIndex ) &&
          ( position.columnIndex + position.columnSpan > columnIndex ) )
      {
        // Decrement span
        if( position.columnSpan > 1 )
        {
          position.columnSpan--;
        }
      }
      else if( column >= columnIndex )    // If on the right side of or at the inserted column, decrease column index
      {
        // Decrement index
        if( position.columnIndex > 0 )
        {
          position.columnIndex--;
        }
      }
    }
  }

  // 0 rows, 1 column removed
  RemoveAndGetLostActors( lost, removed, 0u, 1u );

  // Contract column data array
  mColumnData.Erase( mColumnData.Begin() + columnIndex );

  // Size may have changed so relayout
  mRowColumnDirty = true;
  RelayoutRequest();
}

void TableView::Resize( unsigned int rows, unsigned int columns )
{
  std::vector< Actor > ignored;
  Resize( rows, columns, ignored );
}

void TableView::Resize( unsigned int rows, unsigned int columns, std::vector<Actor>& removed )
{
  RelayoutingLock lock( *this );

  unsigned int oldRows = GetRows();
  unsigned int oldColumns = GetColumns();

  // Resize data array
  std::vector< CellData > lost;
  ResizeContainers( rows, columns, lost );

  // Calculate if we lost rows
  unsigned int rowsRemoved = 0;
  unsigned int newRows = GetRows();

  if( oldRows < newRows )
  {
    rowsRemoved = newRows - oldRows;
  }

  // Calculate if we lost columns
  unsigned int columnsRemoved = 0;
  unsigned int newColumns = GetColumns();
  if( oldColumns < newColumns )
  {
    rowsRemoved = newColumns - oldColumns;
  }

  RemoveAndGetLostActors( lost, removed, rowsRemoved, columnsRemoved );

  // Sizes may have changed so request a relayout
  mRowColumnDirty = true;
  RelayoutRequest();
}

void TableView::SetCellPadding( Size padding )
{
  // If padding really changed
  if( padding != mPadding )
  {
    mPadding = padding;

    RelayoutRequest();
  }
}

Size TableView::GetCellPadding()
{
  return mPadding;
}

void TableView::SetRowPolicy( unsigned int rowIndex, CellSizePolicy policy )
{
  DALI_ASSERT_ALWAYS( rowIndex < mRowData.Size() );

  if( mRowData[ rowIndex ].sizePolicy != policy )
  {
    mRowData[ rowIndex ].sizePolicy = policy;

    mRowColumnDirty = true;
    RelayoutRequest();
  }
}

TableView::CellSizePolicy TableView::GetRowPolicy( unsigned int rowIndex ) const
{
  DALI_ASSERT_ALWAYS( rowIndex < mRowData.Size() );

  return mRowData[ rowIndex ].sizePolicy;
}

void TableView::SetColumnPolicy( unsigned int columnIndex, CellSizePolicy policy )
{
  DALI_ASSERT_ALWAYS( columnIndex < mColumnData.Size() );

  if( mColumnData[ columnIndex ].sizePolicy != policy )
  {
    mColumnData[ columnIndex ].sizePolicy = policy;

    mRowColumnDirty = true;
    RelayoutRequest();
  }
}

TableView::CellSizePolicy TableView::GetColumnPolicy( unsigned int columnIndex ) const
{
  DALI_ASSERT_ALWAYS( columnIndex < mColumnData.Size() );

  return mColumnData[ columnIndex ].sizePolicy;
}

void TableView::SetFixedHeight( unsigned int rowIndex, float height )
{
  DALI_ASSERT_ALWAYS( rowIndex < mRowData.Size() );

  RowColumnData& data = mRowData[ rowIndex ];
  data.size = height;
  data.sizePolicy = FIXED;

  mRowColumnDirty = true;
  RelayoutRequest();
}

float TableView::GetFixedHeight( unsigned int rowIndex ) const
{
  DALI_ASSERT_ALWAYS( rowIndex < mRowData.Size() );

  return mRowData[ rowIndex ].size;
}

void TableView::SetFixedWidth( unsigned int columnIndex, float width )
{
  DALI_ASSERT_ALWAYS( columnIndex < mColumnData.Size() );

  RowColumnData& data = mColumnData[ columnIndex ];
  data.size = width;
  data.sizePolicy = FIXED;

  mRowColumnDirty = true;
  RelayoutRequest();
}

float TableView::GetFixedWidth( unsigned int columnIndex ) const
{
  DALI_ASSERT_ALWAYS( columnIndex < mColumnData.Size() );

  return mColumnData[ columnIndex ].size;
}

void TableView::SetRelativeHeight( unsigned int rowIndex, float heightPercentage )
{
  DALI_ASSERT_ALWAYS( rowIndex < mRowData.Size() );

  RowColumnData& data = mRowData[ rowIndex ];
  data.fillRatio = heightPercentage;
  data.userFillRatio = true;
  data.sizePolicy = FILL;

  mRowColumnDirty = true;
  RelayoutRequest();
}

float TableView::GetRelativeHeight( unsigned int rowIndex ) const
{
  DALI_ASSERT_ALWAYS( rowIndex < mRowData.Size() );

  return mRowData[ rowIndex ].fillRatio;
}

void TableView::SetRelativeWidth( unsigned int columnIndex, float widthPercentage )
{
  DALI_ASSERT_ALWAYS( columnIndex < mColumnData.Size() );

  RowColumnData& data = mColumnData[ columnIndex ];
  data.fillRatio = widthPercentage;
  data.userFillRatio = true;
  data.sizePolicy = FILL;

  mRowColumnDirty = true;
  RelayoutRequest();
}

float TableView::GetRelativeWidth( unsigned int columnIndex ) const
{
  DALI_ASSERT_ALWAYS( columnIndex < mColumnData.Size() );

  return mColumnData[ columnIndex ].fillRatio;
}

void TableView::CalculateRowColumnData()
{
  // Calculate the relative sizes
  if( mRowColumnDirty )
  {
    ComputeRelativeSizes( mRowData );
    ComputeRelativeSizes( mColumnData );

    mRowColumnDirty = false;
  }
}

void TableView::OnCalculateRelayoutSize( Dimension::Type dimension )
{
  CalculateRowColumnData();

  if( dimension & Dimension::WIDTH )
  {
    CalculateFixedSizes( mColumnData, Dimension::WIDTH );
    mFixedTotals.width = CalculateTotalFixedSize( mColumnData );
  }

  if( dimension & Dimension::HEIGHT )
  {
    CalculateFixedSizes( mRowData, Dimension::HEIGHT );
    mFixedTotals.height = CalculateTotalFixedSize( mRowData );
  }
}

void TableView::OnLayoutNegotiated( float size, Dimension::Type dimension )
{
  CalculateRowColumnData();

  // Calculate the value of all relative sized rows and columns
  if( dimension & Dimension::WIDTH )
  {
    float remainingSize = size - mFixedTotals.width;
    if( remainingSize < 0.0f )
    {
      remainingSize = 0.0f;
    }

    CalculateRelativeSizes( mColumnData, remainingSize );
  }

  if( dimension & Dimension::HEIGHT )
  {
    float remainingSize = size - mFixedTotals.height;
    if( remainingSize < 0.0f )
    {
      remainingSize = 0.0f;
    }

    CalculateRelativeSizes( mRowData, remainingSize );
  }
}

void TableView::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  CalculateRowColumnData();

  // Go through the layout data
  float cumulatedHeight = 0.0f;

  const unsigned int rowCount = mCellData.GetRows();
  const unsigned int columnCount = mCellData.GetColumns();

  for( unsigned int row = 0; row < rowCount; ++row )
  {
    float cumulatedWidth = 0.0f;

    for( unsigned int column = 0; column < columnCount; ++column )
    {
      Actor& actor = mCellData[ row ][ column ].actor;
      const Toolkit::TableView::CellPosition position = mCellData[ row ][ column ].position;

      // If there is an actor and this is the main cell of the actor.
      // An actor can be in multiple cells if its row or columnspan is more than 1.
      // We however must lay out each actor only once.
      if( actor && ( position.rowIndex == row ) && ( position.columnIndex == column ) )
      {
        // Anchor actor to top left of table view
        actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
        actor.SetParentOrigin( ParentOrigin::TOP_LEFT );

        Padding padding;
        actor.GetPadding( padding );

        Vector3 actorPosition( cumulatedWidth + mPadding.width + padding.left,       // Left padding
                               cumulatedHeight + mPadding.height + padding.top,      // Top padding
                               0.0f );
        actor.SetPosition( actorPosition );
      }

      DALI_ASSERT_DEBUG( column < mColumnData.Size() );
      cumulatedWidth += mColumnData[ column ].size;
    }

    DALI_ASSERT_DEBUG( row < mRowData.Size() );
    cumulatedHeight += mRowData[ row ].size;
  }
}

unsigned int TableView::GetRows()
{
  return mCellData.GetRows();
}

unsigned int TableView::GetColumns()
{
  return mCellData.GetColumns();
}

void TableView::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::TableView tableView = Toolkit::TableView::DownCast( Dali::BaseHandle( object ) );

  if( tableView )
  {
    TableView& tableViewImpl( GetImpl( tableView ) );
    switch( index )
    {
      case Toolkit::TableView::Property::ROWS:
      {
        if( value.Get<unsigned int>() != tableViewImpl.GetRows() )
        {
          tableViewImpl.Resize( value.Get<unsigned int>(), tableViewImpl.GetColumns() );
        }
        break;
      }
      case Toolkit::TableView::Property::COLUMNS:
      {
        if( value.Get<unsigned int>() != tableViewImpl.GetColumns() )
        {
          tableViewImpl.Resize( tableViewImpl.GetRows(), value.Get<unsigned int>() );
        }
        break;
      }
      case Toolkit::TableView::Property::CELL_PADDING:
      {
        tableViewImpl.SetCellPadding( value.Get<Vector2>() );
        break;
      }
      case Toolkit::TableView::Property::LAYOUT_ROWS:
      {
        SetHeightOrWidthProperty( tableViewImpl, &TableView::SetFixedHeight, &TableView::SetRelativeHeight, value );
        break;
      }
      case Toolkit::TableView::Property::LAYOUT_COLUMNS:
      {
        SetHeightOrWidthProperty( tableViewImpl, &TableView::SetFixedWidth, &TableView::SetRelativeWidth, value );
        break;
      }
    }
  }
}

Property::Value TableView::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::TableView tableView = Toolkit::TableView::DownCast( Dali::BaseHandle( object ) );

  if( tableView )
  {
    TableView& tableViewImpl( GetImpl( tableView ) );
    switch( index )
    {
      case Toolkit::TableView::Property::ROWS:
      {
        value = tableViewImpl.GetRows();
        break;
      }
      case Toolkit::TableView::Property::COLUMNS:
      {
        value = tableViewImpl.GetColumns();
        break;
      }
      case Toolkit::TableView::Property::CELL_PADDING:
      {
        value = tableViewImpl.GetCellPadding();
        break;
      }
      case Toolkit::TableView::Property::LAYOUT_ROWS:
      {
        value = tableViewImpl.GetRowHeightsPropertyValue();
        break;
      }
      case Toolkit::TableView::Property::LAYOUT_COLUMNS:
      {
        value = tableViewImpl.GetColumnWidthsPropertyValue();
        break;
      }
    }
  }

  return value;
}

void TableView::OnControlChildAdd( Actor& child )
{
  if( mLayoutingChild )
  {
    // we're in the middle of laying out children so no point doing anything here
    return;
  }

  RelayoutRequest();

  // Test properties on actor
  Toolkit::TableView::CellPosition cellPosition;
  if( child.GetPropertyIndex(Toolkit::TableView::ROW_SPAN_PROPERTY_NAME) != Property::INVALID_INDEX )
  {
    cellPosition.rowSpan = static_cast<unsigned int>( child.GetProperty( child.GetPropertyIndex(Toolkit::TableView::ROW_SPAN_PROPERTY_NAME) ).Get<float>() );
  }

  if( child.GetPropertyIndex(Toolkit::TableView::COLUMN_SPAN_PROPERTY_NAME) != Property::INVALID_INDEX )
  {
    cellPosition.columnSpan = static_cast<unsigned int>( child.GetProperty( child.GetPropertyIndex(Toolkit::TableView::COLUMN_SPAN_PROPERTY_NAME) ).Get<float>() );
  }

  if( child.GetPropertyIndex(Toolkit::TableView::CELL_INDICES_PROPERTY_NAME) != Property::INVALID_INDEX )
  {
    Vector2 indices = child.GetProperty( child.GetPropertyIndex(Toolkit::TableView::CELL_INDICES_PROPERTY_NAME) ).Get<Vector2 >();
    cellPosition.rowIndex = static_cast<unsigned int>( indices.x );
    cellPosition.columnIndex = static_cast<unsigned int>( indices.y );

    AddChild( child, cellPosition );

    // Do not continue
    return;
  }

  // Find the first available cell to store the actor in
  const unsigned int rowCount = mCellData.GetRows();
  const unsigned int columnCount = mCellData.GetColumns();
  for( unsigned int row = 0; row < rowCount; ++row )
  {
    for( unsigned int column = 0; column < columnCount; ++column )
    {
      if( !(mCellData[ row ][ column ].actor) )
      {
        // Put the actor in the cell
        CellData data;
        data.actor = child;
        data.position.columnIndex = column;
        data.position.rowIndex = row;
        mCellData[ row ][ column ] = data;

        // Don't continue
        return;
      }
    }
  }

  // No empty cells, so increase size of the table
  unsigned int newColumnCount = ( columnCount > 0 ) ? columnCount : 1;
  ResizeContainers( rowCount + 1, newColumnCount );

  // Put the actor in the first cell of the new row
  CellData data;
  data.actor = child;
  data.position.rowIndex = rowCount;
  data.position.columnIndex = 0;
  mCellData[ rowCount ][ 0 ] = data;
}

void TableView::OnControlChildRemove( Actor& child )
{
  // dont process if we're in the middle of bigger operation like delete row, column or resize
  if( !mLayoutingChild )
  {
    // relayout the table only if instances were found
    if( RemoveAllInstances( child ) )
    {
      RelayoutRequest();
    }
  }
}

TableView::TableView( unsigned int initialRows, unsigned int initialColumns )
: Control( ControlBehaviour( REQUIRES_TOUCH_EVENTS | REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mCellData( initialRows, initialColumns ),
  mLayoutingChild( false ),
  mRowColumnDirty( true )     // Force recalculation first time
{
  SetKeyboardNavigationSupport( true );
  ResizeContainers( initialRows, initialColumns );
}

void TableView::OnInitialize()
{
  // Make self as keyboard focusable and focus group
  Actor self = Self();
  self.SetKeyboardFocusable(true);
  SetAsKeyboardFocusGroup(true);
}

void TableView::ResizeContainers( unsigned int rows, unsigned int columns )
{
  std::vector<CellData> ignored;
  ResizeContainers( rows, columns, ignored );
}

void TableView::ResizeContainers( unsigned int rows, unsigned int columns, std::vector<CellData>& removed )
{
  // Resize cell data
  mCellData.Resize( rows, columns, removed );

  // We don't care if these go smaller, data will be regenerated or is not needed anymore
  mRowData.Resize( rows );
  mColumnData.Resize( columns );
}

void TableView::RemoveAndGetLostActors( const std::vector<CellData>& lost, std::vector<Actor>& removed,
                                        unsigned int rowsRemoved, unsigned int columnsRemoved )
{
  // iterate through all lost cells
  std::vector< CellData >::const_iterator iter = lost.begin();
  for( ; iter != lost.end(); ++iter )
  {
    // if it is a valid actor
    if( (*iter).actor )
    {
      // is this actor still somewhere else in the table
      Toolkit::TableView::CellPosition position;
      if( FindChildPosition( (*iter).actor, position ) )
      {
        // it must be spanning multiple cells, position contains the top left most one
        // check if position is left of the removed location
        if( position.columnIndex < (*iter).position.columnIndex )
        {
          // if column span is greater than 1
          if( mCellData[ position.rowIndex ][ position.columnIndex ].position.columnSpan > 1 )
          {
            // decrease column span
            mCellData[ position.rowIndex ][ position.columnIndex ].position.columnSpan -= columnsRemoved;
          }
        }
        // check if position is left of the removed location
        if( position.rowIndex < (*iter).position.rowIndex )
        {
          // if row span is greater than 1
          if( mCellData[ position.rowIndex ][ position.columnIndex ].position.rowSpan > 1 )
          {
            // decrease row span
            mCellData[ position.rowIndex ][ position.columnIndex ].position.rowSpan -= rowsRemoved;
          }
        }
      }
      else
      {
        // this actor is gone for good
        // add actor to removed container
        removed.push_back( (*iter).actor );
        // we dont want the child actor anymore
        Self().Remove( (*iter).actor );
      }
    }
  }
}

bool TableView::RemoveAllInstances( const Actor& child )
{
  bool found = false;
  // walk through the layout data
  const unsigned int rowCount = mCellData.GetRows();
  const unsigned int columnCount = mCellData.GetColumns();
  for( unsigned int row = 0; row < rowCount; ++row )
  {
    for( unsigned int column = 0; column < columnCount; ++column )
    {
      if( mCellData[ row ][ column ].actor == child )
      {
        // clear the cell, NOTE that the cell might be spanning multiple cells
        mCellData[ row ][ column ] = CellData();
        found = true;
      }
    }
  }
  return found;
}

void TableView::SetHeightOrWidthProperty(TableView& tableViewImpl,
                                         void(TableView::*funcFixed)(unsigned int, float),
                                         void(TableView::*funcRelative)(unsigned int, float),
                                         const Property::Value& value )
{
  if( Property::MAP == value.GetType() )
  {
    Property::Map map = value.Get<Property::Map>();
    unsigned int rowIndex(0);
    for ( unsigned int i = 0, count = map.Count(); i < count; ++i )
    {
      Property::Value& item = map.GetValue(i);

      if( std::istringstream(map.GetKey(i)) >> rowIndex  // the key is a number
          && Property::MAP == item.GetType())
      {
        if( item.HasKey( "policy" ) && item.HasKey( "value" ) )
        {
          Toolkit::TableView::LayoutPolicy policy = Scripting::GetEnumeration< Toolkit::TableView::LayoutPolicy >( item.GetValue("policy").Get<std::string>().c_str(), LAYOUT_POLICY_STRING_TABLE, LAYOUT_POLICY_STRING_TABLE_COUNT );
          if( policy == Toolkit::TableView::FIXED )
          {
            (tableViewImpl.*funcFixed)( rowIndex, item.GetValue("value").Get<float>() );
          }
          else if( policy == Toolkit::TableView::RELATIVE )
          {
            (tableViewImpl.*funcRelative)( rowIndex, item.GetValue("value").Get<float>() );
          }
        }
      }
    }
  }
}

Property::Value TableView::GetRowHeightsPropertyValue()
{
  Property::Map map;
  GetMapPropertyValue( mRowData, map);
  return Property::Value(map);
}

Property::Value TableView::GetColumnWidthsPropertyValue()
{
  Property::Map map;
  GetMapPropertyValue( mColumnData, map);
  return Property::Value(map);
}

void TableView::GetMapPropertyValue( const RowColumnArray& data, Property::Map& map )
{
  std::string fixedPolicy( Scripting::GetEnumerationName< Toolkit::TableView::LayoutPolicy >( Toolkit::TableView::FIXED, LAYOUT_POLICY_STRING_TABLE, LAYOUT_POLICY_STRING_TABLE_COUNT ) );
  std::string relativePolicy( Scripting::GetEnumerationName< Toolkit::TableView::LayoutPolicy >( Toolkit::TableView::RELATIVE, LAYOUT_POLICY_STRING_TABLE, LAYOUT_POLICY_STRING_TABLE_COUNT ) );

  const RowColumnArray::SizeType count = data.Size();
  for( RowColumnArray::SizeType i = 0; i < count; i++ )
  {
    const RowColumnData& dataInstance = data[ i ];

    switch( dataInstance.sizePolicy )
    {
      case FIXED:
      {
        Property::Map item;
        item[ "policy" ] = fixedPolicy;
        item[ "value" ] = dataInstance.size;

        std::ostringstream ss;
        ss << i;

        map[ ss.str() ] = item;

        break;
      }

      case FILL:
      {
        Property::Map item;
        item[ "policy" ] = relativePolicy;
        item[ "value" ] = dataInstance.fillRatio;

        std::ostringstream ss;
        ss << i;

        map[ ss.str() ] = item;

        break;
      }

      default:
      {
        break;
      }
    }
  }
}

TableView::~TableView()
{
  // nothing to do
}

Actor TableView::GetNextKeyboardFocusableActor(Actor currentFocusedActor, Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled)
{
  Actor nextFocusableActor;

  if ( !currentFocusedActor )
  {
    // Nothing is currently focused, so the child in the first cell should be focused.
    nextFocusableActor = GetChildAt(Toolkit::TableView::CellPosition(0, 0));
  }
  else
  {
    Toolkit::TableView::CellPosition position;
    if( FindChildPosition( currentFocusedActor, position ) )
    {
      // The current focused actor is a child of TableView
      bool focusLost = false;
      int currentRow = position.rowIndex;
      int currentColumn = position.columnIndex;
      int numberOfColumns = GetColumns();
      int numberOfRows = GetRows();

      switch ( direction )
      {
        case Toolkit::Control::KeyboardFocus::LEFT:
        {
          if(--currentColumn < 0)
          {
            currentColumn = numberOfColumns - 1;
            if(--currentRow < 0)
            {
              currentRow = loopEnabled ? numberOfRows - 1 : 0;
              focusLost = (currentRow == 0);
            }
          }
          break;
        }
        case Toolkit::Control::KeyboardFocus::RIGHT:
        {
          if(++currentColumn > numberOfColumns - 1)
          {
            currentColumn = 0;
            if(++currentRow > numberOfRows - 1)
            {
              currentRow = loopEnabled ? 0 : numberOfRows - 1;
              focusLost = (currentRow == numberOfRows - 1);
            }
          }
          break;
        }
        case Toolkit::Control::KeyboardFocus::UP:
        {
          if(--currentRow < 0)
          {
            currentRow = loopEnabled ? numberOfRows - 1 : 0;
            focusLost = (currentRow == 0);
          }
          break;
        }
        case Toolkit::Control::KeyboardFocus::DOWN:

        {
          if(++currentRow > numberOfRows - 1)
          {
            currentRow = loopEnabled ? 0 : numberOfRows - 1;
            focusLost = (currentRow == numberOfRows - 1);
          }
          break;
        }
      }

      // Move the focus if we haven't lost it.
      if(!focusLost)
      {
        nextFocusableActor = GetChildAt(Toolkit::TableView::CellPosition(currentRow, currentColumn));
      }
    }
    else
    {
      // The current focused actor is not within table view, so the child in the first cell should be focused.
      nextFocusableActor = GetChildAt(Toolkit::TableView::CellPosition(0, 0));
    }
  }

  return nextFocusableActor;
}

Vector3 TableView::GetNaturalSize()
{
  // Natural size is the size of all fixed cell widths or heights. This ignores cells with relative heights.
  return Vector3( mFixedTotals.width, mFixedTotals.height, 1.0f );
}

float TableView::CalculateChildSize( const Actor& child, Dimension::Type dimension )
{
  CalculateRowColumnData();

  const unsigned int rowCount = mCellData.GetRows();
  const unsigned int columnCount = mCellData.GetColumns();

  for( unsigned int row = 0; row < rowCount; ++row )
  {
    for( unsigned int column = 0; column < columnCount; ++column )
    {
      // check if this cell has an actor
      Actor& actor = mCellData[ row ][ column ].actor;

      if( actor && ( actor == child ) )
      {
        const Toolkit::TableView::CellPosition position = mCellData[ row ][ column ].position;

        // If there is an actor and this is the main cell of the actor.
        // An actor can be in multiple cells if its row or columnspan is more than 1.
        if ( ( position.rowIndex == row ) && ( position.columnIndex == column ) )
        {
          switch( dimension )
          {
            case Dimension::WIDTH:
            {
              float cellSize = 0.0f;

              // Accumulate the width
              for( unsigned int i = 0; i < position.columnSpan; ++i )
              {
                DALI_ASSERT_DEBUG( column + i < mColumnData.Size() );
                cellSize += mColumnData[ column + i ].size;
              }

              // Apply padding
              cellSize -= mPadding.width * 2.0f;
              if( cellSize < 0.0f )
              {
                cellSize = 0.0f;
              }

              return cellSize;
            }

            case Dimension::HEIGHT:
            {
              float cellSize = 0.0f;

              // Accumulate the height
              for( unsigned int i = 0; i < position.rowSpan; ++i )
              {
                DALI_ASSERT_DEBUG( row + i < mRowData.Size() );
                cellSize += mRowData[ row + i ].size;
              }

              // Apply padding
              cellSize -= mPadding.width * 2.0f;
              if( cellSize < 0.0f )
              {
                cellSize = 0.0f;
              }

              return cellSize;
            }

            default:
            {
              return 0.0f;
            }
          }
        }
      }
    }
  }

  return 0.0f;    // Child not found
}

bool TableView::RelayoutDependentOnChildren( Dimension::Type dimension )
{
  if ( Control::RelayoutDependentOnChildren( dimension ) )
  {
    return true;
  }

  return FindFit( mRowData ) || FindFit( mColumnData );
}

void TableView::SetCellAlignment( Toolkit::TableView::CellPosition position, HorizontalAlignment::Type horizontal, VerticalAlignment::Type vertical )
{
  // Check if we need to expand our data array
  if( position.rowIndex >= mCellData.GetRows() )
  {
    // Only adding new rows
    ResizeContainers( position.rowIndex + 1, mCellData.GetColumns() );
  }

  if( position.columnIndex >= mCellData.GetColumns() )
  {
    // Only adding new columns
    ResizeContainers( mCellData.GetRows(), position.columnIndex + 1 );
  }

  // Set the alignment of the cell
  CellData& data = mCellData[ position.rowIndex ][ position.columnIndex ];
  data.horizontalAlignment = horizontal;
  data.verticalAlignment = vertical;
}

void TableView::ComputeRelativeSizes( RowColumnArray& data )
{
  // First pass: Count number of fill entries and calculate used relative space
  Dali::Vector< RowColumnData* > fillData;
  float relativeTotal = 0.0f;

  const unsigned int dataCount = data.Size();

  for( unsigned int i = 0; i < dataCount; ++i )
  {
    RowColumnData& dataInstance = data[ i ];

    if( dataInstance.sizePolicy == FILL )
    {
      if( dataInstance.userFillRatio )
      {
        relativeTotal += dataInstance.fillRatio;
      }
      else
      {
        fillData.PushBack( &dataInstance );
      }
    }
  }

  // Second pass: Distribute remaining relative space
  const unsigned int fillCount = fillData.Size();
  if( fillCount > 0 )
  {
    if( relativeTotal > 1.0f )
    {
      relativeTotal = 1.0f;
    }

    const float evenFillRatio = (1.0f - relativeTotal ) / fillCount;

    for( unsigned int i = 0; i < fillCount; ++i )
    {
      fillData[ i ]->fillRatio = evenFillRatio;
    }
  }
}

float TableView::CalculateTotalFixedSize( const RowColumnArray& data )
{
  float totalSize = 0.0f;

  const unsigned int dataCount = data.Size();

  for( unsigned int i = 0; i < dataCount; ++i )
  {
    const RowColumnData& dataInstance = data[ i ];

    switch( dataInstance.sizePolicy )
    {
      case FIXED:
      case FIT:
      {
        totalSize += dataInstance.size;
        break;
      }

      default:
      {
        break;
      }
    }
  }

  return totalSize;
}

Vector2 TableView::GetCellPadding( Dimension::Type dimension )
{
  switch( dimension )
  {
    case Dimension::WIDTH:
    {
      return Vector2( mPadding.x, mPadding.x );
    }
    case Dimension::HEIGHT:
    {
      return Vector2( mPadding.y, mPadding.y );
    }
    default:
    {
      break;
    }
  }

  return Vector2();
}

void TableView::CalculateFixedSizes( RowColumnArray& data, Dimension::Type dimension )
{
  Vector2 cellPadding = GetCellPadding( dimension );

  const unsigned int dataCount = data.Size();

  for( unsigned int i = 0; i < dataCount; ++i )
  {
    RowColumnData& dataInstance = data[ i ];

    if( dataInstance.sizePolicy == FIT )
    {
      // Find the size of the biggest actor in the row or column
      float maxActorHeight = 0.0f;

      unsigned int fitCount = ( dimension == Dimension::WIDTH ) ? mCellData.GetRows() : mCellData.GetColumns();

      for( unsigned int j = 0; j < fitCount; ++j )
      {
        unsigned int row = ( dimension == Dimension::WIDTH ) ? j : i;
        unsigned int column = ( dimension == Dimension::WIDTH ) ? i : j;
        DALI_ASSERT_DEBUG( row < mCellData.GetRows() );
        DALI_ASSERT_DEBUG( column < mCellData.GetColumns() );

        const CellData& cellData = mCellData[ row ][ column ];
        const Actor& actor = cellData.actor;
        if( actor )
        {
          if( FitToChild( actor, dimension ) && ( dimension == Dimension::WIDTH ) ? ( cellData.position.columnSpan == 1 ) : ( cellData.position.rowSpan == 1 )  )
          {
            maxActorHeight = std::max( maxActorHeight, actor.GetRelayoutSize( dimension ) + cellPadding.x + cellPadding.y );
          }
        }
      }

      dataInstance.size = maxActorHeight;
    }
  }
}

void TableView::CalculateRelativeSizes( RowColumnArray& data, float size )
{
  const unsigned int dataCount = data.Size();

  for( unsigned int i = 0; i < dataCount; ++i )
  {
    RowColumnData& dataInstance = data[ i ];

    if( dataInstance.sizePolicy == FILL )
    {
      dataInstance.size = dataInstance.fillRatio * size;
    }
  }
}

bool TableView::FindFit( const RowColumnArray& data )
{
  for( unsigned int i = 0, count = data.Size(); i < count; ++i )
  {
    if( data[ i ].sizePolicy == FIT )
    {
      return true;
    }
  }

  return false;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
