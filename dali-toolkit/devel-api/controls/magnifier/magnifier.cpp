/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/magnifier/magnifier.h>
#include <dali-toolkit/internal/controls/magnifier/magnifier-impl.h>

using namespace Dali;

namespace
{

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// Magnifier
///////////////////////////////////////////////////////////////////////////////////////////////////

Magnifier::Magnifier()
{
}

Magnifier::Magnifier( const Magnifier& handle )
: Control( handle )
{
}

Magnifier& Magnifier::operator=( const Magnifier& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

Magnifier::Magnifier(Internal::Magnifier& implementation)
: Control(implementation)
{
}

Magnifier::Magnifier( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::Magnifier>(internal);
}

Magnifier Magnifier::New()
{
  return Internal::Magnifier::New();
}

Magnifier::~Magnifier()
{
}

Magnifier Magnifier::DownCast( BaseHandle handle )
{
  return Control::DownCast<Magnifier, Internal::Magnifier>(handle);
}

void Magnifier::SetSourceActor(Actor actor)
{
  GetImpl(*this).SetSourceActor( actor );
}

} // namespace Toolkit

} // namespace Dali
