#ifndef DALI_TOOLKIT_INTERNAL_SCALE_TRANSITION_H
#define DALI_TOOLKIT_INTERNAL_SCALE_TRANSITION_H

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
 *Fade
 */

// INTERNAL INCLUDES
#include <dali-toolkit/internal/transition/transition-base-impl.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/transition/scale-transition.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/weak-handle.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
using ScaleTransitionPtr = IntrusivePtr<ScaleTransition>;

class ScaleTransition : public TransitionBase
{
public:
  /**
   * @brief Create a new ScaleTransition object.
   * @param[in] control A control of this transition.
   * @param[in] scaleFactor A scale value to be applied on control during transition
   * @param[in] timePeriod The duration of the animation.
   * @return A smart-pointer to the newly allocated ScaleTransition.
   */
  static ScaleTransitionPtr New(Dali::Toolkit::Control control, const Vector2& scaleFactor, TimePeriod timePeriod);

  /**
   * @copydoc Dali::Toolkit::ScaleTransition::SetScaleFactor()
   */
  void SetScaleFactor(const Vector2& scaleFactor);

  /**
   * @copydoc Dali::Toolkit::ScaleTransition::GetDirection()
   */
  Vector2 GetScaleFactor() const;

protected:
  /**
   * @copydoc Dali::Toolkit::ScaleTransition::OnPlay()
   */
  void OnPlay() override;

protected:
  /**
   * @brief Construct a new ScaleTransition.
   */
  ScaleTransition(Dali::Toolkit::Control control,
                  const Vector2&         scaleFactor,
                  TimePeriod             timePeriod);

  /**
   * @brief A reference counted object may only be deleted by calling Unreference()
   */
  ~ScaleTransition() override;

private:
  // Undefined
  ScaleTransition(const ScaleTransition&);

  // Undefined
  ScaleTransition& operator=(const ScaleTransition& rhs);

private:
  WeakHandle<Dali::Toolkit::Control> mTargetControl;
  Vector2                            mScaleFactor;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ScaleTransition& GetImplementation(Dali::Toolkit::ScaleTransition& scale)
{
  DALI_ASSERT_ALWAYS(scale && "ScaleTransition handle is empty");

  BaseObject& handle = scale.GetBaseObject();

  return static_cast<Internal::ScaleTransition&>(handle);
}

inline const Internal::ScaleTransition& GetImplementation(const Dali::Toolkit::ScaleTransition& scale)
{
  DALI_ASSERT_ALWAYS(scale && "ScaleTransition handle is empty");

  const BaseObject& handle = scale.GetBaseObject();

  return static_cast<const Internal::ScaleTransition&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_FADE_H
