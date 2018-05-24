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
#include <dali-toolkit/devel-api/layouting/fbox-layout.h>

//EXTERNAL HEADERS
//INTERNAL HEADERS
#include <dali-toolkit/internal/layouting/fbox-layout-impl.h>

namespace Dali
{
namespace Toolkit
{

FboxLayout::FboxLayout()
{
}

FboxLayout FboxLayout::New()
{
  Internal::FboxLayoutPtr internal = Internal::FboxLayout::New();
  return FboxLayout( internal.Get() );
}

FboxLayout FboxLayout::DownCast( BaseHandle handle )
{
  return FboxLayout( dynamic_cast< Dali::Toolkit::Internal::FboxLayout*>( handle.GetObjectPtr() ) );
}

FboxLayout::FboxLayout( const FboxLayout& other )
: LayoutGroup( other )
{
}

FboxLayout& FboxLayout::operator=( const FboxLayout& other )
{
  if( &other != this )
  {
    LayoutGroup::operator=( other );
  }
  return *this;
}

void FboxLayout::SetFlexDirection( Dali::Toolkit::FboxLayout::FlexDirection flexDirection )
{
  if (GetImplementation(*this).GetFlexDirection() != flexDirection)
  {
    GetImplementation(*this).SetFlexDirection( flexDirection );
    GetImplementation(*this).RequestLayout();
  }
}

Dali::Toolkit::FboxLayout::FlexDirection FboxLayout::GetFlexDirection()
{
  return GetImplementation(*this).GetFlexDirection();
}

void FboxLayout::SetFlexJustification( Dali::Toolkit::FboxLayout::Justification flexJustification )
{
  if (GetImplementation(*this).GetFlexJustification() != flexJustification)
  {
    GetImplementation(*this).SetFlexJustification( flexJustification );
    GetImplementation(*this).RequestLayout();
  }
}

Dali::Toolkit::FboxLayout::Justification FboxLayout::GetFlexJustification()
{
  return GetImplementation(*this).GetFlexJustification();
}

void FboxLayout::SetFlexWrap( Dali::Toolkit::FboxLayout::WrapType flexWrap )
{
  if (GetImplementation(*this).GetFlexWrap() != flexWrap)
  {
    GetImplementation(*this).SetFlexWrap( flexWrap );
    GetImplementation(*this).RequestLayout();
  }
}

Dali::Toolkit::FboxLayout::WrapType FboxLayout::GetFlexWrap()
{
  return GetImplementation(*this).GetFlexWrap();
}

FboxLayout::FboxLayout( Dali::Toolkit::Internal::FboxLayout* object )
: LayoutGroup( object )
{
}

} // namespace Toolkit
} // namespace Dali
