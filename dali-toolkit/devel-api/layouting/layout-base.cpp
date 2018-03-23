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

#include <dali-toolkit/devel-api/layouting/layout-base.h>
#include <dali-toolkit/devel-api/layouting/layout-base-impl.h>

namespace Dali
{
namespace Toolkit
{

LayoutBase::LayoutBase()
: BaseHandle()
{
}

LayoutBase LayoutBase::New( Handle& handle )
{
  Internal::LayoutBasePtr layout = Internal::LayoutBase::New( handle );
  return LayoutBase( layout.Get() );
}

LayoutBase::LayoutBase( const LayoutBase& rhs )
: BaseHandle( rhs )
{
}

LayoutBase& LayoutBase::operator=( const LayoutBase& rhs )
{
  BaseHandle::operator=(rhs);
  return *this;
}

LayoutBase::LayoutBase( Internal::LayoutBase* layoutBase )
: BaseHandle( layoutBase )
{
}

Handle LayoutBase::GetOwner()
{
  return GetImplementation( *this ).GetOwner();
}

} // namespace Toolkit

} // namespace Dali
