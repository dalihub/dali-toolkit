#ifndef DALI_TOOLKIT_INTERNAL_SLIDE_TRANSITION_H
#define DALI_TOOLKIT_INTERNAL_SLIDE_TRANSITION_H

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
#include <dali-toolkit/public-api/transition/slide-transition.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/weak-handle.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
using SlideTransitionPtr = IntrusivePtr<SlideTransition>;

class SlideTransition : public TransitionBase
{
public:
  /**
   * @brief Create a new SlideTransition object.
   * @param[in] control A control of this transition.
   * @param[in] direction Relative direction that the control is comming from or going to.
   * @param[in] timePeriod The duration of the animation.
   * @return A smart-pointer to the newly allocated SlideTransition.
   */
  static SlideTransitionPtr New(Dali::Toolkit::Control control, const Vector2& direction, TimePeriod timePeriod);

  /**
   * @copydoc Dali::Toolkit::SlideTransition::SetDirection()
   */
  void SetDirection(const Vector2& direction);

  /**
   * @copydoc Dali::Toolkit::SlideTransition::GetDirection()
   */
  Vector2 GetDirection() const;

protected:
  /**
   * @copydoc Dali::Toolkit::SlideTransition::OnPlay()
   */
  void OnPlay() override;

protected:
  /**
   * @brief Construct a new SlideTransition.
   */
  SlideTransition(Dali::Toolkit::Control control,
                  const Vector2&         direction,
                  TimePeriod             timePeriod);

  /**
   * @brief A reference counted object may only be deleted by calling Unreference()
   */
  ~SlideTransition() override;

private:
  // Undefined
  SlideTransition(const SlideTransition&);

  // Undefined
  SlideTransition& operator=(const SlideTransition& rhs);

private:
  WeakHandle<Dali::Toolkit::Control> mTargetControl;
  Vector2                            mDirection;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::SlideTransition& GetImplementation(Dali::Toolkit::SlideTransition& slide)
{
  DALI_ASSERT_ALWAYS(slide && "SlideTransition handle is empty");

  BaseObject& handle = slide.GetBaseObject();

  return static_cast<Internal::SlideTransition&>(handle);
}

inline const Internal::SlideTransition& GetImplementation(const Dali::Toolkit::SlideTransition& slide)
{
  DALI_ASSERT_ALWAYS(slide && "SlideTransition handle is empty");

  const BaseObject& handle = slide.GetBaseObject();

  return static_cast<const Internal::SlideTransition&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_SLIDE_TRANSITION_H
