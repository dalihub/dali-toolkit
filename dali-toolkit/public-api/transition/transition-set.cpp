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
#include <dali-toolkit/public-api/transition/transition-set.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/transition/transition-set-impl.h>

namespace Dali
{
namespace Toolkit
{
TransitionSet::TransitionSet() = default;

TransitionSet::TransitionSet(Internal::TransitionSet* transition)
: BaseHandle(transition)
{
}

TransitionSet TransitionSet::New()
{
  Internal::TransitionSetPtr internal = Dali::Toolkit::Internal::TransitionSet::New();

  return TransitionSet(internal.Get());
}

TransitionSet TransitionSet::DownCast(BaseHandle handle)
{
  return TransitionSet(dynamic_cast<Dali::Toolkit::Internal::TransitionSet*>(handle.GetObjectPtr()));
}

TransitionSet::~TransitionSet() = default;

TransitionSet::TransitionSet(const TransitionSet& handle) = default;

TransitionSet& TransitionSet::operator=(const TransitionSet& rhs) = default;

TransitionSet::TransitionSet(TransitionSet&& rhs) = default;

TransitionSet& TransitionSet::operator=(TransitionSet&& rhs) = default;

void TransitionSet::AddTransition(TransitionBase transition)
{
  if(transition)
  {
    Internal::TransitionBasePtr transitionBasePtr(&GetImplementation(transition));
    GetImplementation(*this).AddTransition(transitionBasePtr);
  }
}

TransitionBase TransitionSet::GetTransitionAt(uint32_t index) const
{
  Internal::TransitionBase* transitionBasePtr = GetImplementation(*this).GetTransitionAt(static_cast<uint32_t>(index));
  return Dali::Toolkit::TransitionBase(transitionBasePtr);
}

uint32_t TransitionSet::GetTransitionCount() const
{
  return GetImplementation(*this).GetTransitionCount();
}

void TransitionSet::Play()
{
  GetImplementation(*this).Play();
}

TransitionSet::TransitionSetSignalType& TransitionSet::FinishedSignal()
{
  return GetImplementation(*this).FinishedSignal();
}

} // namespace Toolkit

} // namespace Dali
