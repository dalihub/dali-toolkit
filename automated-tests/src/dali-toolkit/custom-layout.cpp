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

// CLASS HEADER
#include "custom-layout.h"

// INTERNAL HEADERS
#include "custom-layout-impl.h"

namespace Test
{

CustomLayout CustomLayout::New()
{
  Internal::CustomLayoutPtr internal = Internal::CustomLayout::New();
  return CustomLayout( internal.Get() );
}

CustomLayout CustomLayout::DownCast( BaseHandle handle )
{
  return CustomLayout( dynamic_cast< Test::Internal::CustomLayout* >( handle.GetObjectPtr() ) );
}

CustomLayout::CustomLayout( Internal::CustomLayout* object )
: Dali::Toolkit::LayoutGroup( object )
{
}

void CustomLayout::RequestLayout()
{
  GetImplementation( *this ).RequestLayout();
}

void CustomLayout::SetCustomBehaviourFlag( int flag )
{
  GetImplementation(*this).SetCustomBehaviourFlag( flag );
}

bool CustomLayout::GetCustomBehaviourFlags( int flagToCheck )
{
  return GetImplementation(*this).GetCustomBehaviourFlags( flagToCheck );
}

void CustomLayout::ClearPrivateFlag( int flag )
{
  return GetImplementation(*this).ClearPrivateFlag( flag );
}

} // namespace Test
