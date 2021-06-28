#ifndef DALI_TOOLKIT_INTERNAL_TRANSITION_PLAYLIST_H
#define DALI_TOOLKIT_INTERNAL_TRANSITION_PLAYLIST_H

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

// EXTERNAL INCLUDES
#include <cstdio>
#include <memory>
#include <mutex>
#include <vector>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/transition/transition-set.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class TransitionLifecycleController : public ConnectionTracker
{
public:
  static TransitionLifecycleController& GetInstance();

  void AddTransitions(Dali::Toolkit::TransitionSet transitions);

private:

  void RemoveTransitions(Dali::Toolkit::TransitionSet &transitions);

  /**
   * Construct a new TransitionLifecycleController.
   */
  TransitionLifecycleController() = default;

  // Undefined
  TransitionLifecycleController(const TransitionLifecycleController&) = delete;

  // Undefined
  TransitionLifecycleController& operator=(const TransitionLifecycleController& rhs) = delete;

private:
  std::vector<Dali::Toolkit::TransitionSet>             mTransitionList;
  static std::unique_ptr<TransitionLifecycleController> mInstance;
  static std::once_flag                                 mOnceFlag;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TRANSITION_PLAYLIST_H
;