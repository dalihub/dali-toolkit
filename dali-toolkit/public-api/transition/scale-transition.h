#ifndef DALI_TOOLKIT_SCALE_TRANSITION_H
#define DALI_TOOLKIT_SCALE_TRANSITION_H

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
class ScaleTransition;
}

/**
 * @brief ScaleTransition provides smoothly appearing/disappearing effects for target Control.
 * User of this scale transition can set scale factor for this transiton.
 * The scale factor can be a single float value or Vector2 value.
 * If it is a single float value, the value is applied to both x and y direction.
 *
 * If this transition is for appearing, the Control comes out with the scale factor applied
 * and will be animated at its original scale.
 * If this transition is for disappearing, the Control starts at its original size
 * but will become the scale of scale factor and vanished.
 */
class DALI_TOOLKIT_API ScaleTransition : public TransitionBase
{
public:
  /**
   * @brief Creates an uninitialized ScaleTransition; this can be initialized with ScaleTransition::New().
   *
   * Calling member functions with an uninitialized ScaleTransition handle is not allowed.
   */
  ScaleTransition();

  /**
   * @brief Creates an initialized ScaleTransition.
   *
   * @param[in] control A control of this transition.
   * @param[in] scaleFactor A scala scale factor that will be applied on both of width and height of the control
   * @param[in] timePeriod The duration of the animation.
   * @return A handle to a newly allocated Dali resource
   */
  static ScaleTransition New(Dali::Toolkit::Control control, float scaleFactor, TimePeriod timePeriod);

  /**
   * @brief Creates an initialized ScaleTransition.
   *
   * @param[in] control A control of this transition.
   * @param[in] scaleFactor A scale vector to be applied on control during transition
   * @param[in] timePeriod The duration of the animation.
   * @return A handle to a newly allocated Dali resource
   */
  static ScaleTransition New(Dali::Toolkit::Control control, const Vector2& scaleFactor, TimePeriod timePeriod);

  /**
   * @brief Downcasts a handle to ScaleTransition handle.
   *
   * If handle points to an ScaleTransition object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Handle to an ScaleTransition object or an uninitialized handle
   */
  static ScaleTransition DownCast(BaseHandle handle);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ScaleTransition();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the copied handle
   */
  ScaleTransition(const ScaleTransition& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  ScaleTransition& operator=(const ScaleTransition& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs A reference to the moved handle
   */
  ScaleTransition(ScaleTransition&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  ScaleTransition& operator=(ScaleTransition&& rhs) noexcept;

  /**
   * @brief Sets scaleFactor to be used to scale target Control
   *
   * @param[in] scaleFactor Relative scaleFactor that will be used when the Control is appearing or disappearing.
   */
  void SetScaleFactor(const Vector2& scaleFactor);

  /**
   * @brief Retrieves scaleFactor that will be used when the Control is appearing or disappearing.
   */
  Vector2 GetScaleFactor() const;

public: // Not intended for use by Application developers
  /// @cond internal
  /**
   * @brief This constructor is used by ScaleTransition::New() methods.
   * @param[in] scale A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ScaleTransition(Internal::ScaleTransition* scale);
  /// @endcond
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SCALE_TRANSITION_H
