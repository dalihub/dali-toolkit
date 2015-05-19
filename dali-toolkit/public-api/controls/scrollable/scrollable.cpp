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

#include <dali-toolkit/public-api/controls/scrollable/scrollable.h>
#include <dali-toolkit/internal/controls/scrollable/scrollable-impl.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

Scrollable::Scrollable()
{
}

Scrollable::Scrollable(Internal::Scrollable& implementation)
: Control(implementation)
{
}

Scrollable::Scrollable( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::Scrollable>(internal);
}

Scrollable::Scrollable( const Scrollable& handle )
: Control( handle )
{
}

Scrollable& Scrollable::operator=( const Scrollable& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

Scrollable::~Scrollable()
{
}

Scrollable Scrollable::DownCast( BaseHandle handle )
{
  return Control::DownCast<Scrollable, Internal::Scrollable>(handle);
}

Scrollable::ScrollStartedSignalType& Scrollable::ScrollStartedSignal()
{
  return GetImpl(*this).ScrollStartedSignal();
}

Scrollable::ScrollUpdatedSignalType& Scrollable::ScrollUpdatedSignal()
{
  return GetImpl(*this).ScrollUpdatedSignal();
}

Scrollable::ScrollCompletedSignalType& Scrollable::ScrollCompletedSignal()
{
  return GetImpl(*this).ScrollCompletedSignal();
}

bool Scrollable::IsOvershootEnabled() const
{
  return GetImpl(*this).IsOvershootEnabled();
}

void Scrollable::SetOvershootEnabled(bool enable)
{
  GetImpl(*this).SetOvershootEnabled(enable);
}

void Scrollable::SetOvershootEffectColor( const Vector4& color )
{
  GetImpl(*this).SetOvershootEffectColor(color);
}

Vector4 Scrollable::GetOvershootEffectColor() const
{
  return GetImpl(*this).GetOvershootEffectColor();
}

void Scrollable::SetOvershootAnimationSpeed( float pixelsPerSecond )
{
  GetImpl(*this).SetOvershootAnimationSpeed(pixelsPerSecond);
}

float Scrollable::GetOvershootAnimationSpeed() const
{
  return GetImpl(*this).GetOvershootAnimationSpeed();
}

} // namespace Toolkit

} // namespace Dali
