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
#include <dali-toolkit/devel-api/layouting/absolute-layout.h>

//INTERNAL HEADERS
#include <dali-toolkit/internal/layouting/absolute-layout-impl.h>

namespace Dali
{
namespace Toolkit
{

AbsoluteLayout::AbsoluteLayout()
{
}

AbsoluteLayout AbsoluteLayout::New()
{
  Internal::AbsoluteLayoutPtr internal = Internal::AbsoluteLayout::New();
  return AbsoluteLayout( internal.Get() );
}

AbsoluteLayout AbsoluteLayout::DownCast( BaseHandle handle )
{
  return AbsoluteLayout( dynamic_cast< Dali::Toolkit::Internal::AbsoluteLayout*>( handle.GetObjectPtr() ) );
}

AbsoluteLayout::AbsoluteLayout( const AbsoluteLayout& other )
: LayoutGroup( other )
{
}

AbsoluteLayout& AbsoluteLayout::operator=( const AbsoluteLayout& other )
{
  if( &other != this )
  {
    LayoutGroup::operator=( other );
  }
  return *this;
}

AbsoluteLayout::AbsoluteLayout( Dali::Toolkit::Internal::AbsoluteLayout* object )
: LayoutGroup( object )
{
}

} // namespace Toolkit
} // namespace Dali
