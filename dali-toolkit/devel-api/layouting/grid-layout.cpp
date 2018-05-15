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
#include <dali-toolkit/devel-api/layouting/grid-layout.h>

//EXTERNAL HEADERS
//INTERNAL HEADERS
#include <dali-toolkit/internal/layouting/grid-layout-impl.h>

namespace Dali
{
namespace Toolkit
{

GridLayout::GridLayout()
{
}

GridLayout GridLayout::New()
{
  Internal::GridLayoutPtr internal = Internal::GridLayout::New();
  return GridLayout( internal.Get() );
}

GridLayout GridLayout::DownCast( BaseHandle handle )
{
  return GridLayout( dynamic_cast< Dali::Toolkit::Internal::GridLayout*>( handle.GetObjectPtr() ) );
}

GridLayout::GridLayout( const GridLayout& other )
: LayoutGroup( other )
{
}

GridLayout& GridLayout::operator=( const GridLayout& other )
{
  if( &other != this )
  {
    LayoutGroup::operator=( other );
  }
  return *this;
}

void GridLayout::SetCellPadding( LayoutSize size )
{
  GetImplementation(*this).SetCellPadding( size );
}

LayoutSize GridLayout::GetCellPadding()
{
  return GetImplementation(*this).GetCellPadding();
}

GridLayout::GridLayout( Dali::Toolkit::Internal::GridLayout* object )
: LayoutGroup( object )
{
}

} // namespace Toolkit
} // namespace Dali
