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

//CLASS HEADER
#include <dali-toolkit/devel-api/controls/layouting/hbox-view.h>

//EXTERNAL HEADERS
//INTERNAL HEADERS
#include <dali-toolkit/internal/controls/layouting/hbox-view-impl.h>

namespace Dali
{
namespace Toolkit
{

HboxView::HboxView()
{
}

HboxView HboxView::New()
{
  return Internal::HboxView::New();
}

HboxView HboxView::DownCast( BaseHandle handle )
{
  return Control::DownCast< HboxView, Internal::HboxView >( handle );
}

HboxView::HboxView( const HboxView& other )
: Control( other )
{
}

HboxView& HboxView::operator=( const HboxView& other )
{
  if( &other != this )
  {
    Control::operator=( other );
  }
  return *this;
}

void HboxView::AddChild( Actor child )
{
  GetImpl( *this ).AddChild( child );
}

Actor HboxView::GetChildAt( CellPosition position )
{
  return GetImpl( *this ).GetChildAt( position );
}

Actor HboxView::RemoveChildAt( CellPosition position )
{
  return GetImpl( *this ).RemoveChildAt( position );
}

void HboxView::SetCellPadding( LayoutSize size )
{
  GetImpl( *this ).SetCellPadding( size );
}

LayoutSize HboxView::GetCellPadding()
{
  return GetImpl( *this ).GetCellPadding();
}

void HboxView::SetMode( Mode mode )
{
  GetImpl( *this ).SetMode( mode );
}

void HboxView::SetCellWidth( CellPosition cellPosition, LayoutLength width )
{
  GetImpl( *this ).SetCellWidth( cellPosition, width );
}

void HboxView::SetCellHeight( CellPosition cellPosition, LayoutLength height )
{
  GetImpl( *this ).SetCellHeight( cellPosition, height );
}

void HboxView::SetLayoutData( MarginLayoutData handle )
{
  GetImpl( *this ).SetLayoutData( handle );
}

MarginLayoutData HboxView::GetLayoutData()
{
  return GetImpl( *this ).GetLayoutData();
}

HboxView::HboxView( Dali::Toolkit::Internal::HboxView& object )
: Control( object )
{
}

HboxView::HboxView( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer< Dali::Toolkit::Internal::HboxView >( internal );
}

} // namespace Toolkit
} // namespace Dali
