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
#include "flex-layout.h"

//INTERNAL INCLUDES
#include <dali-toolkit/internal/layouting/flex-layout-impl.h>

namespace Dali
{
namespace Toolkit
{

FlexLayout::FlexLayout()
{
}

FlexLayout FlexLayout::New()
{
  Internal::FlexLayoutPtr internal = Internal::FlexLayout::New();
  return FlexLayout( internal.Get() );
}

FlexLayout FlexLayout::DownCast( BaseHandle handle )
{
  return FlexLayout( dynamic_cast< Dali::Toolkit::Internal::FlexLayout*>( handle.GetObjectPtr() ) );
}

FlexLayout::FlexLayout( const FlexLayout& other )
: LayoutGroup( other )
{
}

FlexLayout& FlexLayout::operator=( const FlexLayout& other )
{
  if( &other != this )
  {
    LayoutGroup::operator=( other );
  }
  return *this;
}

void FlexLayout::SetFlexDirection( Dali::Toolkit::FlexLayout::FlexDirection flexDirection )
{
  if( GetImplementation(*this).GetFlexDirection() != flexDirection )
  {
    GetImplementation(*this).SetFlexDirection( flexDirection );
    GetImplementation(*this).RequestLayout();
  }
}

Dali::Toolkit::FlexLayout::FlexDirection FlexLayout::GetFlexDirection() const
{
  return GetImplementation(*this).GetFlexDirection();
}

void FlexLayout::SetFlexJustification( Dali::Toolkit::FlexLayout::Justification flexJustification )
{
  if( GetImplementation(*this).GetFlexJustification() != flexJustification )
  {
    GetImplementation(*this).SetFlexJustification( flexJustification );
    GetImplementation(*this).RequestLayout();
  }
}

Dali::Toolkit::FlexLayout::Justification FlexLayout::GetFlexJustification() const
{
  return GetImplementation(*this).GetFlexJustification();
}

void FlexLayout::SetFlexWrap( Dali::Toolkit::FlexLayout::WrapType flexWrap )
{
  if( GetImplementation(*this).GetFlexWrap() != flexWrap )
  {
    GetImplementation(*this).SetFlexWrap( flexWrap );
    GetImplementation(*this).RequestLayout();
  }
}

Dali::Toolkit::FlexLayout::WrapType FlexLayout::GetFlexWrap() const
{
  return GetImplementation(*this).GetFlexWrap();
}

void FlexLayout::SetFlexAlignment( Dali::Toolkit::FlexLayout::Alignment::Type flexAlignment )
{
  if( GetImplementation(*this).GetFlexAlignment() != flexAlignment )
  {
    GetImplementation(*this).SetFlexAlignment( flexAlignment );
    GetImplementation(*this).RequestLayout();
  }
}

Dali::Toolkit::FlexLayout::Alignment::Type FlexLayout::GetFlexAlignment() const
{
  return GetImplementation(*this).GetFlexAlignment();
}

void FlexLayout::SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::Alignment::Type flexAlignment )
{
  if( GetImplementation(*this).GetFlexItemsAlignment() != flexAlignment )
  {
    GetImplementation(*this).SetFlexItemsAlignment( flexAlignment );
    GetImplementation(*this).RequestLayout();
  }
}

Dali::Toolkit::FlexLayout::Alignment::Type FlexLayout::GetFlexItemsAlignment() const
{
  return GetImplementation(*this).GetFlexItemsAlignment();
}

FlexLayout::FlexLayout( Dali::Toolkit::Internal::FlexLayout* object )
: LayoutGroup( object )
{
}

} // namespace Toolkit
} // namespace Dali
