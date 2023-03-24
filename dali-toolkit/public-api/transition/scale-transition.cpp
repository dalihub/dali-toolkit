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
#include <dali-toolkit/public-api/transition/scale-transition.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/transition/scale-transition-impl.h>

namespace Dali
{
namespace Toolkit
{
ScaleTransition::ScaleTransition() = default;

ScaleTransition::ScaleTransition(Internal::ScaleTransition* scaleTransition)
: TransitionBase(scaleTransition)
{
}

ScaleTransition ScaleTransition::New(Dali::Toolkit::Control control, float scaleFactor, TimePeriod timePeriod)
{
  Internal::ScaleTransitionPtr internal = Dali::Toolkit::Internal::ScaleTransition::New(control, Vector2(scaleFactor, scaleFactor), timePeriod);

  return ScaleTransition(internal.Get());
}

ScaleTransition ScaleTransition::New(Dali::Toolkit::Control control, const Vector2& scaleFactor, TimePeriod timePeriod)
{
  Internal::ScaleTransitionPtr internal = Dali::Toolkit::Internal::ScaleTransition::New(control, scaleFactor, timePeriod);

  return ScaleTransition(internal.Get());
}

ScaleTransition ScaleTransition::DownCast(BaseHandle handle)
{
  return ScaleTransition(dynamic_cast<Dali::Toolkit::Internal::ScaleTransition*>(handle.GetObjectPtr()));
}

ScaleTransition::~ScaleTransition() = default;

ScaleTransition::ScaleTransition(const ScaleTransition& handle) = default;

ScaleTransition& ScaleTransition::operator=(const ScaleTransition& rhs) = default;

ScaleTransition::ScaleTransition(ScaleTransition&& rhs) noexcept = default;

ScaleTransition& ScaleTransition::operator=(ScaleTransition&& rhs) noexcept = default;

void ScaleTransition::SetScaleFactor(const Vector2& scaleFactor)
{
  GetImplementation(*this).SetScaleFactor(scaleFactor);
}

Vector2 ScaleTransition::GetScaleFactor() const
{
  return GetImplementation(*this).GetScaleFactor();
}

} // namespace Toolkit

} // namespace Dali
