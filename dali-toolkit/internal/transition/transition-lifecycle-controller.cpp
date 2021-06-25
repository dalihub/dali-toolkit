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
#include <dali-toolkit/internal/transition/transition-lifecycle-controller.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/adaptor.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/transition/transition-set-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
std::unique_ptr<TransitionLifecycleController> TransitionLifecycleController::mInstance = nullptr;
std::once_flag                                 TransitionLifecycleController::mOnceFlag;

TransitionLifecycleController& TransitionLifecycleController::GetInstance()
{
  std::call_once(mOnceFlag, []()
                 { mInstance.reset(new TransitionLifecycleController); });
  return *(mInstance.get());
}

void TransitionLifecycleController::AddTransitions(Dali::Toolkit::TransitionSet transitions)
{
  mTransitionList.push_back(transitions);
  transitions.FinishedSignal().Connect(this, &TransitionLifecycleController::RemoveTransitions);
}

void TransitionLifecycleController::RemoveTransitions(Dali::Toolkit::TransitionSet& transitions)
{
  mTransitionList.erase(std::remove(mTransitionList.begin(), mTransitionList.end(), transitions));
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
