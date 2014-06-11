/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

const std::string Scrollable::SCROLL_RELATIVE_POSITION_PROPERTY_NAME( "scroll-relative-position" );
const std::string Scrollable::SCROLL_POSITION_MIN_PROPERTY_NAME( "scroll-position-min" );
const std::string Scrollable::SCROLL_POSITION_MAX_PROPERTY_NAME( "scroll-position-max" );
const std::string Scrollable::SCROLL_DIRECTION_PROPERTY_NAME( "scroll-direction" );

const char* const Scrollable::SIGNAL_SCROLL_STARTED = "scroll-started";
const char* const Scrollable::SIGNAL_SCROLL_COMPLETED = "scroll-completed";
const char* const Scrollable::SIGNAL_SCROLL_UPDATED = "scroll-updated";
const char* const Scrollable::SIGNAL_SCROLL_CLAMPED = "scroll-clamped";

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

Scrollable::ScrollStartedSignalV2& Scrollable::ScrollStartedSignal()
{
  return GetImpl(*this).ScrollStartedSignal();
}

Scrollable::ScrollUpdatedSignalV2& Scrollable::ScrollUpdatedSignal()
{
  return GetImpl(*this).ScrollUpdatedSignal();
}

Scrollable::ScrollCompletedSignalV2& Scrollable::ScrollCompletedSignal()
{
  return GetImpl(*this).ScrollCompletedSignal();
}

Scrollable::ScrollClampedSignalV2& Scrollable::ScrollClampedSignal()
{
  return GetImpl(*this).ScrollClampedSignal();
}

bool Scrollable::IsScrollComponentEnabled(Scrollable::ScrollComponentType indicator) const
{
  return GetImpl(*this).IsScrollComponentEnabled(indicator);
}

void Scrollable::EnableScrollComponent(Scrollable::ScrollComponentType indicator)
{
  GetImpl(*this).EnableScrollComponent(indicator);
}

void Scrollable::DisableScrollComponent(Scrollable::ScrollComponentType indicator)
{
  GetImpl(*this).DisableScrollComponent(indicator);
}

void Scrollable::SetOvershootEffectColor( const Vector4& color )
{
  GetImpl(*this).SetOvershootEffectColor(color);
}

Vector4 Scrollable::GetOvershootEffectColor() const
{
  return GetImpl(*this).GetOvershootEffectColor();
}

} // namespace Toolkit

} // namespace Dali
