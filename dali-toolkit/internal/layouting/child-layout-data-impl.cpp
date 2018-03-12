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
#include <dali-toolkit/internal/layouting/child-layout-data-impl.h>

//EXTERNAL HEADERS
//INTERNAL HEADERS

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

ChildLayoutDataPtr ChildLayoutData::New( uint16_t width, uint16_t height )
{
  ChildLayoutDataPtr object( new ChildLayoutData( width, height ) );
  return object;
}

ChildLayoutDataPtr ChildLayoutData::Clone()
{
  ChildLayoutDataPtr object( new ChildLayoutData( mSize.GetWidth(), mSize.GetHeight() ) );
  return object;
}

uint16_t ChildLayoutData::GetWidth()
{
  return mSize.GetWidth();
}

uint16_t ChildLayoutData::GetHeight()
{
  return mSize.GetHeight();
}

void ChildLayoutData::SetWidth( uint16_t width )
{
}

void ChildLayoutData::SetHeight( uint16_t height )
{
}

ChildLayoutData::ChildLayoutData( uint16_t width, uint16_t height )
: BaseObject(),
  mSize( width, height )
{
}

ChildLayoutData::~ChildLayoutData()
{
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
