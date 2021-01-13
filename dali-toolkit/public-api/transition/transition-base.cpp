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
#include <dali-toolkit/public-api/transition/transition-base.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/transition/transition-base-impl.h>

namespace Dali
{
namespace Toolkit
{
TransitionBase::TransitionBase() = default;

TransitionBase::TransitionBase(Internal::TransitionBase* transitionBase)
: BaseHandle(transitionBase)
{
}

TransitionBase TransitionBase::New()
{
  Internal::TransitionBasePtr internal = Dali::Toolkit::Internal::TransitionBase::New();

  return TransitionBase(internal.Get());
}

TransitionBase TransitionBase::DownCast(BaseHandle handle)
{
  return TransitionBase(dynamic_cast<Dali::Toolkit::Internal::TransitionBase*>(handle.GetObjectPtr()));
}

TransitionBase::~TransitionBase() = default;

TransitionBase::TransitionBase(const TransitionBase& handle) = default;

TransitionBase& TransitionBase::operator=(const TransitionBase& rhs) = default;

TransitionBase::TransitionBase(TransitionBase&& rhs) = default;

TransitionBase& TransitionBase::operator=(TransitionBase&& rhs) = default;

void TransitionBase::SetTimePeriod(TimePeriod timePeriod)
{
  GetImplementation(*this).SetTimePeriod(timePeriod);
}

TimePeriod TransitionBase::GetTimePeriod() const
{
  return GetImplementation(*this).GetTimePeriod();
}

void TransitionBase::SetAlphaFunction(AlphaFunction alphaFunction)
{
  GetImplementation(*this).SetAlphaFunction(alphaFunction);
}

AlphaFunction TransitionBase::GetAlphaFunction() const
{
  return GetImplementation(*this).GetAlphaFunction();
}

void TransitionBase::TransitionWithChild(bool transitionWithChild)
{
  return GetImplementation(*this).TransitionWithChild(transitionWithChild);
}

} // namespace Toolkit

} // namespace Dali
