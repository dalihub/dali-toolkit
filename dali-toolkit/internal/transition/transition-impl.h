#ifndef DALI_TOOLKIT_INTERNAL_TRANSITION_H
#define DALI_TOOLKIT_INTERNAL_TRANSITION_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/transition/transition-base-impl.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/transition/transition.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
using TransitionPtr = IntrusivePtr<Transition>;

class Transition : public TransitionBase
{
public:
  /**
   * @brief Create a new Transition object.
   * @param[in] source A source control of this transition.
   * @param[in] destination A destination control of this transition.
   * @param[in] durationSeconds The duration of the animation.
   * @return A smart-pointer to the newly allocated Transition.
   */
  static TransitionPtr New(Dali::Toolkit::Control source, Dali::Toolkit::Control destination, TimePeriod timePeriod);

protected:
  /**
   * @copydoc Dali::Toolkit::Transition::OnPlay()
   */
  void OnPlay() override;

  /**
   * @brief Emit the Finished signal
   */
  void OnFinished() override;

protected:
  /**
   * @brief Construct a new Transition.
   */
  Transition(Dali::Toolkit::Control source,
             Dali::Toolkit::Control destination,
             TimePeriod             timePeriod);

  /**
   * @brief A reference counted object may only be deleted by calling Unreference()
   */
  ~Transition() override;

private:
  // Undefined
  Transition(const Transition&);

  // Undefined
  Transition& operator=(const Transition& rhs);

private:
  Dali::Toolkit::Control mSourceControl;
  Dali::Toolkit::Control mDestinationControl;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Transition& GetImplementation(Dali::Toolkit::Transition& transition)
{
  DALI_ASSERT_ALWAYS(transition && "Transition handle is empty");

  BaseObject& handle = transition.GetBaseObject();

  return static_cast<Internal::Transition&>(handle);
}

inline const Internal::Transition& GetImplementation(const Dali::Toolkit::Transition& transition)
{
  DALI_ASSERT_ALWAYS(transition && "Transition handle is empty");

  const BaseObject& handle = transition.GetBaseObject();

  return static_cast<const Internal::Transition&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TRANSITION_H
