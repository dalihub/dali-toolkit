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

#include <dali-toolkit/public-api/controls/slider/slider.h>
#include <dali-toolkit/internal/controls/slider/slider-impl.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// Slider
///////////////////////////////////////////////////////////////////////////////////////////////////

// Signals
const char* const Slider::SIGNAL_VALUE_CHANGED = "value-changed";
const char* const Slider::SIGNAL_MARK = "mark";

Slider::Slider()
{
}

Slider::Slider( const Slider& handle )
: Control( handle )
{
}

Slider& Slider::operator=( const Slider& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

Slider::Slider(Internal::Slider& implementation)
: Control(implementation)
{
}

Slider::Slider( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::Slider>(internal);
}

Slider Slider::New()
{
  return Internal::Slider::New();
}

Slider::~Slider()
{
}

Slider::ValueChangedSignalType& Slider::ValueChangedSignal()
{
  return GetImpl( *this ).ValueChangedSignal();
}

Slider::ValueChangedSignalType& Slider::SlidingFinishedSignal()
{
  return GetImpl( *this ).SlidingFinishedSignal();
}

Slider::MarkSignalType& Slider::MarkSignal()
{
  return GetImpl( *this ).MarkSignal();
}

Slider Slider::DownCast( BaseHandle handle )
{
  return Control::DownCast<Slider, Internal::Slider>(handle);
}



} // namespace Toolkit

} // namespace Dali
