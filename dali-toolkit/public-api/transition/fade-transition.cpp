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
#include <dali-toolkit/public-api/transition/fade-transition.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/transition/fade-transition-impl.h>

namespace Dali
{
namespace Toolkit
{
FadeTransition::FadeTransition() = default;

FadeTransition::FadeTransition(Internal::FadeTransition* fadeTransition)
: TransitionBase(fadeTransition)
{
}

FadeTransition FadeTransition::New(Dali::Toolkit::Control control, float opacity, TimePeriod timePeriod)
{
  Internal::FadeTransitionPtr internal = Dali::Toolkit::Internal::FadeTransition::New(control, opacity, timePeriod);

  return FadeTransition(internal.Get());
}

FadeTransition FadeTransition::DownCast(BaseHandle handle)
{
  return FadeTransition(dynamic_cast<Dali::Toolkit::Internal::FadeTransition*>(handle.GetObjectPtr()));
}

FadeTransition::~FadeTransition() = default;

FadeTransition::FadeTransition(const FadeTransition& handle) = default;

FadeTransition& FadeTransition::operator=(const FadeTransition& rhs) = default;

FadeTransition::FadeTransition(FadeTransition&& rhs) = default;

FadeTransition& FadeTransition::operator=(FadeTransition&& rhs) = default;

} // namespace Toolkit

} // namespace Dali
