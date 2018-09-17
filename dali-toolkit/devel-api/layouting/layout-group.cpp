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

#include <dali-toolkit/devel-api/layouting/layout-group.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>

namespace Dali
{
namespace Toolkit
{

LayoutGroup::LayoutGroup()
: LayoutItem()
{
}

LayoutGroup LayoutGroup::New( Handle& handle )
{
  Internal::LayoutGroupPtr layout = Internal::LayoutGroup::New( handle );
  return LayoutGroup( layout.Get() );
}

LayoutGroup LayoutGroup::DownCast( BaseHandle handle )
{
  return LayoutGroup( dynamic_cast< Dali::Toolkit::Internal::LayoutGroup* >( handle.GetObjectPtr() ) );
}

LayoutGroup::LayoutId LayoutGroup::Add( LayoutItem& child )
{
  return GetImplementation( *this ).Add( GetImplementation(child) );
}

void LayoutGroup::Remove( LayoutGroup::LayoutId childId )
{
  GetImplementation( *this ).Remove( childId );
}

void LayoutGroup::Remove( LayoutItem& child )
{
  GetImplementation( *this ).Remove( GetImplementation(child) );
}

LayoutItem LayoutGroup::GetChild( LayoutGroup::LayoutId childId ) const
{
  Internal::LayoutItemPtr child = GetImplementation( *this ).GetChild( childId );
  return LayoutItem( child.Get() );
}

LayoutItem LayoutGroup::GetChildAt( unsigned int childId ) const
{
  Internal::LayoutItemPtr child = GetImplementation( *this ).GetChildAt( childId );
  return LayoutItem( child.Get() );
}

unsigned int LayoutGroup::GetChildCount() const
{
  return GetImplementation( *this ).GetChildCount();
}

LayoutGroup::LayoutGroup( Internal::LayoutGroup* layoutGroup )
: LayoutItem( layoutGroup )
{
}

} // namespace Toolkit
} // namespace Dali
