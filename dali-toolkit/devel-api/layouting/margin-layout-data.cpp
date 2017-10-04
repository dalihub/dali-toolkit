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
#include <dali-toolkit/devel-api/layouting/margin-layout-data.h>

//EXTERNAL HEADERS
//INTERNAL HEADERS
#include <dali-toolkit/internal/layouting/margin-layout-data-impl.h>

namespace Dali
{
namespace Toolkit
{

MarginLayoutData::MarginLayoutData()
{
}

MarginLayoutData MarginLayoutData::New( uint16_t width, uint16_t height, uint16_t begin, uint16_t end, uint16_t top, uint16_t bottom )
{
  Internal::MarginLayoutDataPtr internal = Internal::MarginLayoutData::New( width, height, begin, end, top, bottom );
  return MarginLayoutData( internal.Get() );
}

MarginLayoutData MarginLayoutData::DownCast( BaseHandle handle )
{
  return MarginLayoutData( dynamic_cast< Dali::Toolkit::Internal::MarginLayoutData*>( handle.GetObjectPtr() ) );
}

MarginLayoutData::MarginLayoutData( const MarginLayoutData& other )
: ChildLayoutData( other )
{
}

MarginLayoutData& MarginLayoutData::operator=( const MarginLayoutData& other )
{
  ChildLayoutData::operator=( other );
  return *this;
}


MarginLayoutData::MarginLayoutData( Dali::Toolkit::Internal::MarginLayoutData* object )
: ChildLayoutData( object )
{
}

} // namespace Toolkit
} // namespace Dali

