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
#include <dali-toolkit/devel-api/layouting/vbox-layout.h>

//EXTERNAL HEADERS
//INTERNAL HEADERS
#include <dali-toolkit/internal/layouting/vbox-layout-impl.h>

namespace Dali
{
namespace Toolkit
{

VboxLayout::VboxLayout()
{
}

VboxLayout VboxLayout::New()
{
  Internal::VboxLayoutPtr internal = Internal::VboxLayout::New();
  return VboxLayout( internal.Get() );
}

VboxLayout VboxLayout::DownCast( BaseHandle handle )
{
  return VboxLayout( dynamic_cast< Dali::Toolkit::Internal::VboxLayout*>( handle.GetObjectPtr() ) );
}

VboxLayout::VboxLayout( const VboxLayout& other )
: LayoutGroup( other )
{
}

VboxLayout& VboxLayout::operator=( const VboxLayout& other )
{
  if( &other != this )
  {
    LayoutGroup::operator=( other );
  }
  return *this;
}

void VboxLayout::SetCellPadding( LayoutSize size )
{
  GetImplementation(*this).SetCellPadding( size );
}

LayoutSize VboxLayout::GetCellPadding()
{
  return GetImplementation(*this).GetCellPadding();
}

VboxLayout::VboxLayout( Dali::Toolkit::Internal::VboxLayout* object )
: LayoutGroup( object )
{
}

} // namespace Toolkit
} // namespace Dali
