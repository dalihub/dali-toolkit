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
#include <dali-toolkit/devel-api/controls/control-wrapper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-wrapper-impl.h>

namespace Dali
{

namespace Toolkit
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// ControlWrapper
///////////////////////////////////////////////////////////////////////////////////////////////////

ControlWrapper ControlWrapper::New( const std::string& typeName, Internal::ControlWrapper& implementation )
{
  return Internal::ControlWrapper::New( typeName, &implementation );
}

ControlWrapper::ControlWrapper()
{
}

ControlWrapper::ControlWrapper( const ControlWrapper& handle )
: Control( handle )
{
}

ControlWrapper& ControlWrapper::operator=( const ControlWrapper& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

ControlWrapper::ControlWrapper( Internal::ControlWrapper& implementation )
: Control( implementation )
{
}

ControlWrapper::ControlWrapper( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::ControlWrapper>( internal );
}

ControlWrapper::~ControlWrapper()
{
}

ControlWrapper ControlWrapper::DownCast( BaseHandle handle )
{
  return Control::DownCast<ControlWrapper, Internal::ControlWrapper>( handle );
}

} // namespace Toolkit

} // namespace Dali
