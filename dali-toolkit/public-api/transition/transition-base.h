#ifndef DALI_TRANSITION_BASE_H
#define DALI_TRANSITION_BASE_H

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
#include <dali-toolkit/public-api/dali-toolkit-common.h>

// EXTERNAL INCLUDES
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/base-handle.h>
#include <cstdint> // uint32_t, uint8_t

namespace Dali
{
namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class TransitionBase;
}

/**
 * @brief
 *
 * Transition provides continuous and seamless motions for the visually plausible scene change.
 * And, TransitionBase is a base class for every Transition cases.
 */
class DALI_TOOLKIT_API TransitionBase : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized TransitionBase; this can be initialized with TransitionBase::New().
   *
   * Calling member functions with an uninitialized TransitionBase handle is not allowed.
   */
  TransitionBase();

  /**
   * @brief Creates an initialized TransitionBase.
   */
  static TransitionBase New();

  /**
   * @brief Downcasts a handle to TransitionBase handle.
   *
   * If handle points to an TransitionBase object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Handle to an TransitionBase object or an uninitialized handle
   */
  static TransitionBase DownCast(BaseHandle handle);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~TransitionBase();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the copied handle
   */
  TransitionBase(const TransitionBase& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  TransitionBase& operator=(const TransitionBase& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs A reference to the moved handle
   */
  TransitionBase(TransitionBase&& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  TransitionBase& operator=(TransitionBase&& rhs);

  /**
   * @brief Sets the duration of an transition.
   *
   * @param[in] seconds The duration in seconds
   * @pre DurationSeconds must be greater than zero.
   */
  void SetDuration(float seconds);

  /**
   * @brief Retrieves the duration of an transition.
   *
   * @return The duration in seconds
   */
  float GetDuration() const;

  /**
   * @brief Sets the delay of an transition.
   *
   * @param[in] seconds The delay in seconds
   * @pre DelaySeconds must be greater than zero.
   */
  void SetDelay(float seconds);

  /**
   * @brief Retrieves the delay of an transition.
   *
   * @return The delay in seconds
   */
  float GetDelay() const;

  /**
   * Set time period that contains delay and duration
   * @param[in] timePeriod The time period for an animator.
   */
  void SetTimePeriod(TimePeriod timePeriod);

  /**
   * @brief Sets the alpha function for an transition.
   *
   * This is applied to individual property transitions, if no further alpha functions are supplied.
   * @param[in] alpha The alpha function
   */
  void SetAlphaFunction(AlphaFunction alpha);

  /**
   * @brief Retrieves the alpha function for an transition.
   *
   * @return The alpha function
   */
  AlphaFunction GetAlphaFunction() const;

  /**
   * @brief A View could be transition with its child Views or without them.
   */
  void TransitionWithChild(bool transitionWithChild);

public: // Not intended for use by Application developers
  /// @cond internal
  /**
   * @brief This constructor is used by TransitionBase::New() methods.
   * @param[in] transitionBase A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL TransitionBase(Internal::TransitionBase* transitionBase);
  /// @endcond
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TRANSITION_BASE_H
