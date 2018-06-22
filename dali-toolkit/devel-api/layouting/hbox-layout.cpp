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
#include <dali-toolkit/devel-api/layouting/hbox-layout.h>

//EXTERNAL HEADERS
//INTERNAL HEADERS
#include <dali-toolkit/internal/layouting/hbox-layout-impl.h>

namespace Dali
{
namespace Toolkit
{

HboxLayout::HboxLayout()
{
}

HboxLayout HboxLayout::New()
{
  Internal::HboxLayoutPtr internal = Internal::HboxLayout::New();
  return HboxLayout( internal.Get() );
}

HboxLayout HboxLayout::DownCast( BaseHandle handle )
{
  return HboxLayout( dynamic_cast< Dali::Toolkit::Internal::HboxLayout*>( handle.GetObjectPtr() ) );
}

HboxLayout::HboxLayout( const HboxLayout& other )
: LayoutGroup( other )
{
}

HboxLayout& HboxLayout::operator=( const HboxLayout& other )
{
  if( &other != this )
  {
    LayoutGroup::operator=( other );
  }
  return *this;
}

void HboxLayout::SetCellPadding( LayoutSize size )
{
  GetImplementation(*this).SetCellPadding( size );
}

LayoutSize HboxLayout::GetCellPadding()
{
  return GetImplementation(*this).GetCellPadding();
}

HboxLayout::HboxLayout( Dali::Toolkit::Internal::HboxLayout* object )
: LayoutGroup( object )
{
}

} // namespace Toolkit
} // namespace Dali
