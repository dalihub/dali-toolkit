#ifndef DALI_TOOLKIT_INTERNAL_FADE_TRANSITION_H
#define DALI_TOOLKIT_INTERNAL_FADE_TRANSITION_H

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
#include <dali-toolkit/public-api/transition/fade-transition.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/weak-handle.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
using FadeTransitionPtr = IntrusivePtr<FadeTransition>;

class FadeTransition : public TransitionBase
{
public:
  /**
   * @brief Create a new FadeTransition object.
   * @param[in] control A control of this transition.
   * @param[in] opacity opacity value the control Opacity property will be changed from/to.
   * @param[in] timePeriod The duration of the animation.
   * @return A smart-pointer to the newly allocated FadeTransition.
   */
  static FadeTransitionPtr New(Dali::Toolkit::Control control, float opacity, TimePeriod timePeriod);

protected:
  /**
   * @copydoc Dali::Toolkit::FadeTransition::OnPlay()
   */
  void OnPlay() override;

protected:
  /**
   * @brief Construct a new FadeTransition.
   */
  FadeTransition(Dali::Toolkit::Control control,
                 float                  opacity,
                 TimePeriod             timePeriod);

  /**
   * @brief A reference counted object may only be deleted by calling Unreference()
   */
  ~FadeTransition() override;

private:
  // Undefined
  FadeTransition(const FadeTransition&);

  // Undefined
  FadeTransition& operator=(const FadeTransition& rhs);

private:
  WeakHandle<Dali::Toolkit::Control> mTargetControl;
  float                              mOpacity;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::FadeTransition& GetImplementation(Dali::Toolkit::FadeTransition& fade)
{
  DALI_ASSERT_ALWAYS(fade && "FadeTransition handle is empty");

  BaseObject& handle = fade.GetBaseObject();

  return static_cast<Internal::FadeTransition&>(handle);
}

inline const Internal::FadeTransition& GetImplementation(const Dali::Toolkit::FadeTransition& fade)
{
  DALI_ASSERT_ALWAYS(fade && "FadeTransition handle is empty");

  const BaseObject& handle = fade.GetBaseObject();

  return static_cast<const Internal::FadeTransition&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_FADE_TRANSITION_H
