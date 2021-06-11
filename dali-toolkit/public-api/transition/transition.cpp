/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/transition/transition.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/transition/transition-impl.h>

namespace Dali
{
namespace Toolkit
{
Transition::Transition() = default;

Transition::Transition(Internal::Transition* transition)
: TransitionBase(transition)
{
}

Transition Transition::New(Dali::Toolkit::Control source, Dali::Toolkit::Control destination, TimePeriod timePeriod)
{
  Internal::TransitionPtr internal = Dali::Toolkit::Internal::Transition::New(source, destination, timePeriod);

  return Transition(internal.Get());
}

Transition Transition::DownCast(BaseHandle handle)
{
  return Transition(dynamic_cast<Dali::Toolkit::Internal::Transition*>(handle.GetObjectPtr()));
}

Transition::~Transition() = default;

Transition::Transition(const Transition& handle) = default;

Transition& Transition::operator=(const Transition& rhs) = default;

Transition::Transition(Transition&& rhs) = default;

Transition& Transition::operator=(Transition&& rhs) = default;

} // namespace Toolkit

} // namespace Dali
