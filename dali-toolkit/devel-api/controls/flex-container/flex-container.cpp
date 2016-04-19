/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
 *
 */

// CLASS HEADER
#include <dali-toolkit/devel-api/controls/flex-container/flex-container.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/flex-container/flex-container-impl.h>

namespace Dali
{

namespace Toolkit
{

FlexContainer FlexContainer::New()
{
  return Internal::FlexContainer::New();
}

FlexContainer::FlexContainer()
{
}

FlexContainer::FlexContainer( const FlexContainer& handle )
: Control( handle )
{
}

FlexContainer& FlexContainer::operator=( const FlexContainer& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

FlexContainer::~FlexContainer()
{
}

FlexContainer FlexContainer::DownCast( BaseHandle handle )
{
  return Control::DownCast<FlexContainer, Internal::FlexContainer>(handle);
}

FlexContainer::FlexContainer( Internal::FlexContainer& implementation )
: Control(implementation)
{
}

FlexContainer::FlexContainer( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::FlexContainer>( internal );
}

} // namespace Toolkit

} // namespace Dali
