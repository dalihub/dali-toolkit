//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali-toolkit/public-api/controls/table-view/table-view.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/table-view/table-view-impl.h>

using std::vector;

namespace Dali
{

namespace Toolkit
{

TableView::TableView()
{
}

TableView::TableView( const TableView& handle )
: Control( handle )
{
}

TableView& TableView::operator=( const TableView& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

TableView TableView::New( unsigned int initialRows, unsigned int initialColumns )
{
  return Internal::TableView::New( initialRows, initialColumns );
}

TableView::~TableView()
{
}

TableView TableView::DownCast( BaseHandle handle )
{
  return Control::DownCast<TableView, Internal::TableView>(handle);
}

bool TableView::AddChild( Actor child, CellPosition position )
{
  return GetImpl(*this).AddChild( child, position );
}

Actor TableView::GetChildAt( CellPosition position )
{
  return GetImpl(*this).GetChildAt( position );
}

Actor TableView::RemoveChildAt( CellPosition position )
{
  return GetImpl(*this).RemoveChildAt( position );
}

bool TableView::FindChildPosition( Actor child, CellPosition& position )
{
  return GetImpl(*this).FindChildPosition( child, position );
}

void TableView::InsertRow( unsigned int rowIndex )
{
  GetImpl(*this).InsertRow( rowIndex );
}

void TableView::DeleteRow( unsigned int rowIndex )
{
  GetImpl(*this).DeleteRow( rowIndex );
}

void TableView::DeleteRow( unsigned int rowIndex, vector<Actor>& removed )
{
  GetImpl(*this).DeleteRow( rowIndex, removed );
}

void TableView::InsertColumn( unsigned int columnIndex )
{
  GetImpl(*this).InsertColumn( columnIndex );
}

void TableView::DeleteColumn( unsigned int columnIndex )
{
  GetImpl(*this).DeleteColumn( columnIndex );
}

void TableView::DeleteColumn( unsigned int columnIndex, vector<Actor>& removed )
{
  GetImpl(*this).DeleteColumn( columnIndex, removed );
}

void TableView::Resize( unsigned int rows, unsigned int columns )
{
  GetImpl(*this).Resize( rows, columns );
}

void TableView::Resize( unsigned int rows, unsigned int columns, vector<Actor>& removed )
{
  GetImpl(*this).Resize( rows, columns, removed );
}

void TableView::SetCellPadding( Size padding )
{
  GetImpl(*this).SetCellPadding( padding );
}

Size TableView::GetCellPadding()
{
  return GetImpl(*this).GetCellPadding();
}

void TableView::SetFixedHeight( unsigned int rowIndex, float height )
{
  GetImpl(*this).SetFixedHeight( rowIndex, height );
}

float TableView::GetFixedHeight( unsigned int rowIndex ) const
{
  return GetImpl(*this).GetFixedHeight( rowIndex );
}

void TableView::SetRelativeHeight( unsigned int rowIndex, float heightPercentage )
{
  GetImpl(*this).SetRelativeHeight( rowIndex, heightPercentage );
}

float TableView::GetRelativeHeight( unsigned int rowIndex ) const
{
  return GetImpl(*this).GetRelativeHeight( rowIndex );
}

void TableView::SetFixedWidth( unsigned int columnIndex, float width )
{
  GetImpl(*this).SetFixedWidth( columnIndex, width );
}

float TableView::GetFixedWidth( unsigned int columnIndex ) const
{
  return GetImpl(*this).GetFixedWidth( columnIndex );
}

void TableView::SetRelativeWidth( unsigned int columnIndex, float widthPercentage )
{
  GetImpl(*this).SetRelativeWidth( columnIndex, widthPercentage );
}

float TableView::GetRelativeWidth( unsigned int columnIndex ) const
{
  return GetImpl(*this).GetRelativeWidth( columnIndex );
}

void TableView::SetLayoutAnimationDuration( float duration )
{
  GetImpl(*this).SetLayoutAnimationDuration( duration );
}

float TableView::GetLayoutAnimationDuration()
{
  return GetImpl(*this).GetLayoutAnimationDuration();
}

unsigned int TableView::GetRows()
{
  return GetImpl(*this).GetRows();
}

unsigned int TableView::GetColumns()
{
  return GetImpl(*this).GetColumns();
}

TableView::TableView(Internal::TableView& implementation)
: Control(implementation)
{
}

TableView::TableView( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::TableView>(internal);
}

} // namespace Toolkit

} // namespace Dali
