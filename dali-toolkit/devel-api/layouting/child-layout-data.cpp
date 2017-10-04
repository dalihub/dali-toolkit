/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/layouting/child-layout-data.h>

//EXTERNAL HEADERS
//INTERNAL HEADERS
#include <dali-toolkit/internal/layouting/child-layout-data-impl.h>

namespace Dali
{
namespace Toolkit
{

ChildLayoutData::ChildLayoutData()
{
}

ChildLayoutData ChildLayoutData::New( uint16_t width, uint16_t height )
{
  Internal::ChildLayoutDataPtr internal = Internal::ChildLayoutData::New( width, height );
  return ChildLayoutData( internal.Get() );
}

ChildLayoutData ChildLayoutData::DownCast( BaseHandle handle )
{
  return ChildLayoutData( dynamic_cast< Dali::Toolkit::Internal::ChildLayoutData*>( handle.GetObjectPtr() ) );
}

ChildLayoutData::ChildLayoutData( const ChildLayoutData& other )
: BaseHandle( other )
{
}

ChildLayoutData& ChildLayoutData::operator=( const ChildLayoutData& other )
{
  BaseHandle::operator=( other );
  return *this;
}

uint16_t ChildLayoutData::GetWidth()
{
  return GetImplementation( *this ).GetWidth();
}

uint16_t ChildLayoutData::GetHeight()
{
  return GetImplementation( *this ).GetHeight();
}

void ChildLayoutData::SetWidth( uint16_t width )
{
  GetImplementation( *this ).SetWidth( width );
}

void ChildLayoutData::SetHeight( uint16_t height )
{
  GetImplementation( *this ).SetHeight( height );
}


ChildLayoutData::ChildLayoutData( Dali::Toolkit::Internal::ChildLayoutData* object )
: BaseHandle( object )
{
}

} // namespace Toolkit
} // namespace Dali

