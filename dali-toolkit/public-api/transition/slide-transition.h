#ifndef DALI_TOOLKIT_SLIDE_TRANSITION_H
#define DALI_TOOLKIT_SLIDE_TRANSITION_H

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
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/transition/transition-base.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class SlideTransition;
}

/**
 * @brief Pre-defined directions to define where the control is comming from or going to.
 */
namespace SlideTransitionDirection
{
  static constexpr Vector2 TOP(0, -1); // TOP
  static constexpr Vector2 BOTTOM(0,  1); // BOTTOM
  static constexpr Vector2 LEFT(-1, 0); // LEFT
  static constexpr Vector2 RIGHT(1,  0); // RIGHT
}

/**
 * @brief SlideTransition provides smoothly appearing/disappearing effects for target Control.
 * The direction the target Control is comming from or going to can be selected in the pre-defined directions. {UP, DOWN, LEFT, RIGHT}
 * And, to use custom direction, the direction can be set CUSTOM and use SetDirection(Vector2) method.
 */
class DALI_TOOLKIT_API SlideTransition : public TransitionBase
{
public:

  /**
   * @brief Creates an uninitialized SlideTransition; this can be initialized with SlideTransition::New().
   *
   * Calling member functions with an uninitialized SlideTransition handle is not allowed.
   */
  SlideTransition();

  /**
   * @brief Creates an initialized SlideTransition.
   *
   * @param[in] control A control of this transition.
   * @param[in] direction Relative direction that the control is comming from or going to.
   * @param[in] timePeriod The duration of the animation.
   * @return A handle to a newly allocated Dali resource
   */
  static SlideTransition New(Dali::Toolkit::Control control, const Vector2& direction, TimePeriod timePeriod);

  /**
   * @brief Downcasts a handle to SlideTransition handle.
   *
   * If handle points to an SlideTransition object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Handle to an SlideTransition object or an uninitialized handle
   */
  static SlideTransition DownCast(BaseHandle handle);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~SlideTransition();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the copied handle
   */
  SlideTransition(const SlideTransition& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  SlideTransition& operator=(const SlideTransition& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs A reference to the moved handle
   */
  SlideTransition(SlideTransition&& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  SlideTransition& operator=(SlideTransition&& rhs);

  /**
   * @brief Sets direction to be used to move target Control
   *
   * @param[in] direction Relative direction that the control is comming from or going to.
   */
  void SetDirection(const Vector2& direction);

  /**
   * @brief Retrieves direction to be used to move target Control
   *
   * @note The direction is normalized Vector.
   */
  Vector2 GetDirection() const;

public: // Not intended for use by Application developers
  /// @cond internal
  /**
   * @brief This constructor is used by SlideTransition::New() methods.
   * @param[in] slideTransition A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL SlideTransition(Internal::SlideTransition* slideTransition);
  /// @endcond
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SLIDE_TRANSITION_H
