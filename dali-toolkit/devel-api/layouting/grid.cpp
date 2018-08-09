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
#include <dali-toolkit/devel-api/layouting/grid.h>

//EXTERNAL HEADERS
//INTERNAL HEADERS
#include <dali-toolkit/internal/layouting/grid-impl.h>

namespace Dali
{
namespace Toolkit
{

Grid::Grid()
{
}

Grid Grid::New()
{
  Internal::GridPtr internal = Internal::Grid::New();
  return Grid( internal.Get() );
}

Grid Grid::DownCast( BaseHandle handle )
{
  return Grid( dynamic_cast< Dali::Toolkit::Internal::Grid*>( handle.GetObjectPtr() ) );
}

Grid::Grid( const Grid& other )
: LayoutGroup( other )
{
}

Grid& Grid::operator=( const Grid& other )
{
  if( &other != this )
  {
    LayoutGroup::operator=( other );
  }
  return *this;
}

void Grid::SetNumberOfColumns( int columns )
{
  GetImplementation(*this).SetNumberOfColumns( columns );
}

int Grid::GetNumberOfColumns() const
{
  return GetImplementation(*this).GetNumberOfColumns();
}

Grid::Grid( Dali::Toolkit::Internal::Grid* object )
: LayoutGroup( object )
{
}

} // namespace Toolkit
} // namespace Dali