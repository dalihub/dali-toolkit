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
#include <dali-toolkit/public-api/transition/slide-transition.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/transition/slide-transition-impl.h>

namespace Dali
{
namespace Toolkit
{
SlideTransition::SlideTransition() = default;

SlideTransition::SlideTransition(Internal::SlideTransition* slideTransition)
: TransitionBase(slideTransition)
{
}

SlideTransition SlideTransition::New(Dali::Toolkit::Control control, const Vector2& direction, TimePeriod timePeriod)
{
  Internal::SlideTransitionPtr internal = Dali::Toolkit::Internal::SlideTransition::New(control, direction, timePeriod);

  return SlideTransition(internal.Get());
}

SlideTransition SlideTransition::DownCast(BaseHandle handle)
{
  return SlideTransition(dynamic_cast<Dali::Toolkit::Internal::SlideTransition*>(handle.GetObjectPtr()));
}

SlideTransition::~SlideTransition() = default;

SlideTransition::SlideTransition(const SlideTransition& handle) = default;

SlideTransition& SlideTransition::operator=(const SlideTransition& rhs) = default;

SlideTransition::SlideTransition(SlideTransition&& rhs) noexcept = default;

SlideTransition& SlideTransition::operator=(SlideTransition&& rhs) noexcept = default;

void SlideTransition::SetDirection(const Vector2& direction)
{
  GetImplementation(*this).SetDirection(direction);
}

Vector2 SlideTransition::GetDirection() const
{
  return GetImplementation(*this).GetDirection();
}

} // namespace Toolkit

} // namespace Dali
