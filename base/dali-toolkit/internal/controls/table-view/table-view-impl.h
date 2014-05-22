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
class TableView : public ControlImpl
{
public:

  /**
   * Structure for the layout data
   */
  struct CellData
  {
    // data members
    Actor actor;
    Toolkit::TableView::CellPosition position;
  };

  /**
   * Create a new TableView.
   * @return A smart-pointer to the newly allocated TableView.
   */
  static Toolkit::TableView New( unsigned int initialRows, unsigned int initialColumns );

  /**
   * @copydoc Toolkit::TableView::AddChild
   */
  bool AddChild( Actor child, Toolkit::TableView::CellPosition position );

  /**
   * @copydoc Toolkit::TableView::GetChildAt
   */
  Actor GetChildAt( Toolkit::TableView::CellPosition position );

  /**
   * @copydoc Toolkit::TableView::RemoveChildAt
   */
  Actor RemoveChildAt( Toolkit::TableView::CellPosition position );

  /**
   * @copydoc Toolkit::TableView::FindChildPosition
   */
  bool FindChildPosition( Actor child, Toolkit::TableView::CellPosition& position );

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
   * @copydoc Toolkit::TableView::SetFixedWidth
   */
  void SetFixedWidth( unsigned int columnIndex, float width );

  /**
   * @copydoc Toolkit::TableView::GetFixedWidth
   */
  float GetFixedWidth( unsigned int columnIndex ) const;

  /**
   * @copydoc Toolkit::TableView::SetRelativeWidth
   */
  void SetRelativeWidth( unsigned int columnIndex, float widthPercentage );

  /**
   * @copydoc Toolkit::TableView::GetRelativeWidth
   */
  float GetRelativeWidth( unsigned int columnIndex ) const;

  /**
   * @copydoc Toolkit::TableView::SetLayoutAnimationDuration
   */
  void SetLayoutAnimationDuration( float duration );

  /**
   * @copydoc Toolkit::TableView::GetLayoutAnimationDuration
   */
  float GetLayoutAnimationDuration();


  /**
   * @copydoc Toolkit::TableView::GetRows
   */
  unsigned int GetRows();

  /**
   * @copydoc Toolkit::TableView::GetColumns
   */
  unsigned int GetColumns();

private: // From ControlImpl

  /**
   * @copydoc Toolkit::ControlImpl::OnControlChildAdd(Actor& child)
   */
  virtual void OnControlChildAdd(Actor& child);

  /**
   * @copydoc Toolkit::ControlImpl::OnControlChildRemove(Actor& child)
   */
  virtual void OnControlChildRemove(Actor& child);

  /**
   * @copydoc Toolkit::ControlImpl::OnRelaidOut
   */
  virtual void OnRelaidOut( Vector2 size, ActorSizeContainer& container );

  /**
   * @copydoc Toolkit::ControlImpl::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::ControlImpl::GetNextKeyboardFocusableActor
   */
  virtual Actor GetNextKeyboardFocusableActor(Actor currentFocusedActor, Control::KeyboardFocusNavigationDirection direction, bool loopEnabled);

private: // Implementation

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
  bool RemoveAllInstances( Actor child );

  /**
   * Helper to update relative sizes
   * @param fixedHeightsTotal sum of the fixed height rows
   * @param fixedWidthsTotal sum of the fixed width columns
   */
  void UpdateRelativeSizes( float& fixedHeightsTotal, float& fixedWidthsTotal );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TableView();

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

  Array2d<CellData> mCellData;
  Array2d<Size> mRelativeSizes;
  std::vector<float> mFixedHeights;
  std::vector<float> mRelativeHeights;
  std::vector<float> mFixedWidths;
  std::vector<float> mRelativeWidths;
  Size mPadding;
  bool mLayoutingChild;
  float mConstraintDuration;

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
