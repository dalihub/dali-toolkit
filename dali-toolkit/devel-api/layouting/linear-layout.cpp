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
#include "linear-layout.h"

//INTERNAL INCLUDES
#include <dali-toolkit/internal/layouting/linear-layout-impl.h>

namespace Dali
{
namespace Toolkit
{

LinearLayout::LinearLayout()
{
}

LinearLayout LinearLayout::New()
{
  Internal::LinearLayoutPtr internal = Internal::LinearLayout::New();
  return LinearLayout( internal.Get() );
}

LinearLayout LinearLayout::DownCast( BaseHandle handle )
{
  return LinearLayout( dynamic_cast< Dali::Toolkit::Internal::LinearLayout*>( handle.GetObjectPtr() ) );
}

LinearLayout::LinearLayout( const LinearLayout& other )
: LayoutGroup( other )
{
}

LinearLayout& LinearLayout::operator=( const LinearLayout& other )
{
  if( &other != this )
  {
    LayoutGroup::operator=( other );
  }
  return *this;
}

void LinearLayout::SetCellPadding( LayoutSize size )
{
  GetImplementation(*this).SetCellPadding( size );
}

LayoutSize LinearLayout::GetCellPadding() const
{
  return GetImplementation(*this).GetCellPadding();
}

void LinearLayout::SetOrientation( LinearLayout::Orientation orientation )
{
  GetImplementation(*this).SetOrientation( orientation );
}

LinearLayout::Orientation LinearLayout::GetOrientation() const
{
  return GetImplementation(*this).GetOrientation();
}

void LinearLayout::SetAlignment( unsigned int alignment )
{
  GetImplementation(*this).SetAlignment( alignment );
}

unsigned int LinearLayout::GetAlignment() const
{
  return GetImplementation(*this).GetAlignment();
}

LinearLayout::LinearLayout( Dali::Toolkit::Internal::LinearLayout* object )
: LayoutGroup( object )
{
}

} // namespace Toolkit
} // namespace Dali
