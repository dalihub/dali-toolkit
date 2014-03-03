//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali-toolkit/public-api/controls/magnifier/magnifier.h>
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

const std::string Magnifier::SOURCE_POSITION_PROPERTY_NAME( "source-position" );

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

void Magnifier::SetSourceActor(Actor actor)
{
  GetImpl(*this).SetSourceActor( actor );
}

void Magnifier::SetSourcePosition(Vector3 position)
{
  GetImpl(*this).SetSourcePosition( position );
}

bool Magnifier::GetFrameVisibility() const
{
  return GetImpl(*this).GetFrameVisibility();
}

void Magnifier::SetFrameVisibility(bool visible)
{
  GetImpl(*this).SetFrameVisibility(visible);
}

float Magnifier::GetMagnificationFactor() const
{
  return GetImpl(*this).GetMagnificationFactor();
}

void Magnifier::SetMagnificationFactor(float value)
{
  GetImpl(*this).SetMagnificationFactor( value );
}


} // namespace Toolkit

} // namespace Dali
