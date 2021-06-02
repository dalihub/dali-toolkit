#ifndef DALI_TOOLKIT_FADE_H
#define DALI_TOOLKIT_FADE_H

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
class Fade;
}

/**
 * @brief Fade provides smoothly appearing/disappearing effects for target Control.
 */
class DALI_TOOLKIT_API Fade : public TransitionBase
{
public:
  /**
   * @brief Creates an uninitialized Fade; this can be initialized with Fade::New().
   *
   * Calling member functions with an uninitialized Fade handle is not allowed.
   */
  Fade();

  /**
   * @brief Creates an initialized Fade.
   *
   * @param[in] control A control of this transition.
   * @param[in] opacity opacity value the control Opacity property will be changed from/to. Opacity must be between [0, 1].
   * @param[in] timePeriod The duration of the animation.
   * @return A handle to a newly allocated Dali resource
   */
  static Fade New(Dali::Toolkit::Control control, float opacity, TimePeriod timePeriod);

  /**
   * @brief Downcasts a handle to Fade handle.
   *
   * If handle points to an Fade object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Handle to an Fade object or an uninitialized handle
   */
  static Fade DownCast(BaseHandle handle);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Fade();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the copied handle
   */
  Fade(const Fade& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  Fade& operator=(const Fade& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs A reference to the moved handle
   */
  Fade(Fade&& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  Fade& operator=(Fade&& rhs);

public: // Not intended for use by Application developers
  /// @cond internal
  /**
   * @brief This constructor is used by Fade::New() methods.
   * @param[in] fade A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Fade(Internal::Fade* fade);
  /// @endcond
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_FADE_H
