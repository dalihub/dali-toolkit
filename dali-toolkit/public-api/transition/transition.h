#ifndef DALI_TOOLKIT_TRANSITION_H
#define DALI_TOOLKIT_TRANSITION_H

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
class Transition;
}

/**
 * @brief
 *
 * Transition provides continuous and seamless motions between two Controls.
 * This Transition generates property animation for transforms(position, scale, orientation), size, color, and opacity.
 * And, if there are common renderer properties of source and destination Control, they also animated.
 */
class DALI_TOOLKIT_API Transition : public TransitionBase
{
public:
  /**
   * @brief Creates an uninitialized Transition; this can be initialized with Transition::New().
   *
   * Calling member functions with an uninitialized Transition handle is not allowed.
   */
  Transition();

  /**
   * @brief Creates an initialized Transition.
   *
   * @param[in] source A source control of this transition.
   * @param[in] destination A destination control of this transition.
   * @param[in] useDestinationTarget True if this transition uses destination control as target.
   * @param[in] timePeriod The duration in seconds
   * @return A handle to a newly allocated Dali resource
   * @note durationSeconds can not be negative.
   */
  static Transition New(Dali::Toolkit::Control source, Dali::Toolkit::Control destination, bool useDestinationTarget, TimePeriod timePeriod);

  /**
   * @brief Downcasts a handle to Transition handle.
   *
   * If handle points to an Transition object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Handle to an Transition object or an uninitialized handle
   */
  static Transition DownCast(BaseHandle handle);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Transition();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the copied handle
   */
  Transition(const Transition& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  Transition& operator=(const Transition& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs A reference to the moved handle
   */
  Transition(Transition&& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  Transition& operator=(Transition&& rhs);

public: // Not intended for use by Application developers
  /// @cond internal
  /**
   * @brief This constructor is used by Transition::New() methods.
   * @param[in] transition A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Transition(Internal::Transition* transition);
  /// @endcond
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TRANSITION_H
