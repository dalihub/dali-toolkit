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

#include <dali-toolkit/devel-api/layouting/layout-item.h>
#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-transition-data.h>
#include <dali-toolkit/internal/layouting/layout-transition-data-impl.h>

namespace Dali
{
namespace Toolkit
{

LayoutItem::LayoutItem()
: BaseHandle()
{
}

LayoutItem LayoutItem::New( Handle& handle )
{
  Internal::LayoutItemPtr layout = Internal::LayoutItem::New( handle );
  return LayoutItem( layout.Get() );
}

LayoutItem::LayoutItem( Internal::LayoutItem* LayoutItem )
: BaseHandle( LayoutItem )
{
}

Handle LayoutItem::GetOwner() const
{
  return GetImplementation( *this ).GetOwner();
}

void LayoutItem::SetAnimateLayout( bool animateLayout )
{
  GetImplementation( *this ).SetAnimateLayout( animateLayout );
}

bool LayoutItem::IsLayoutAnimated() const
{
  return GetImplementation( *this ).IsLayoutAnimated();
}

void LayoutItem::SetTransitionData( LayoutTransitionData::Type layoutTransitionType, LayoutTransitionData layoutTransitionData )
{
  Toolkit::Internal::LayoutTransitionDataPtr layoutTransitionDataPtr = Toolkit::Internal::LayoutTransitionDataPtr();
  if ( layoutTransitionData )
  {
    layoutTransitionDataPtr = Toolkit::Internal::LayoutTransitionDataPtr( &Toolkit::GetImplementation( layoutTransitionData ) );
  }
  GetImplementation( *this ).SetTransitionData( layoutTransitionType, layoutTransitionDataPtr );
}

LayoutTransitionData LayoutItem::GetTransitionData( LayoutTransitionData::Type layoutTransitionType ) const
{
  return LayoutTransitionData( GetImplementation( *this ).GetTransitionData( layoutTransitionType ).Get() );
}

} // namespace Toolkit

} // namespace Dali
