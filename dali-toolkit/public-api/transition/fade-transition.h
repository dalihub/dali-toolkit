#ifndef DALI_TOOLKIT_FADE_TRANSITION_H
#define DALI_TOOLKIT_FADE_TRANSITION_H

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
class FadeTransition;
}

/**
 * @brief Fade provides smoothly appearing/disappearing effects for target Control.
 */
class DALI_TOOLKIT_API FadeTransition : public TransitionBase
{
public:
  /**
   * @brief Creates an uninitialized FadeTransition; this can be initialized with FadeTransition::New().
   *
   * Calling member functions with an uninitialized FadeTransition handle is not allowed.
   */
  FadeTransition();

  /**
   * @brief Creates an initialized FadeTransition.
   *
   * @param[in] control A control of this transition.
   * @param[in] opacity opacity value the control Opacity property will be changed from/to. Opacity must be between [0, 1].
   * @param[in] timePeriod The duration of the animation.
   * @return A handle to a newly allocated Dali resource
   */
  static FadeTransition New(Dali::Toolkit::Control control, float opacity, TimePeriod timePeriod);

  /**
   * @brief Downcasts a handle to FadeTransition handle.
   *
   * If handle points to an FadeTransition object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Handle to an FadeTransition object or an uninitialized handle
   */
  static FadeTransition DownCast(BaseHandle handle);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~FadeTransition();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the copied handle
   */
  FadeTransition(const FadeTransition& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  FadeTransition& operator=(const FadeTransition& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs A reference to the moved handle
   */
  FadeTransition(FadeTransition&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  FadeTransition& operator=(FadeTransition&& rhs) noexcept;

public: // Not intended for use by Application developers
  /// @cond internal
  /**
   * @brief This constructor is used by FadeTransition::New() methods.
   * @param[in] fadeTransition A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL FadeTransition(Internal::FadeTransition* fadeTransition);
  /// @endcond
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_FADE_TRANSITION_H
