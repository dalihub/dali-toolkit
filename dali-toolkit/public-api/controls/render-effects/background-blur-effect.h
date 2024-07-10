#ifndef DALI_TOOLKIT_BACKGROUND_BLUR_EFFECT_H
#define DALI_TOOLKIT_BACKGROUND_BLUR_EFFECT_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/render-effects/render-effect.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class BlurEffectImpl;
} // namespace DALI_INTERNAL

/**
 * @brief BackgroundBlurEffect is a visual effect that blurs owner control's background.
 * This class is a concrete class from RenderEffect interface.
 * Add this effect to a control, clear manually to deactivate.
 *
 * Toolkit::Control control = Toolkit::Control::New();
 * parent.Add(control);
 * control.SetRenderEffect(BackgroundBlurEffect::New()); // Activate
 * ...
 * control.ClearRenderEffect(); // Deactivate
 *
 * Note that tree hierarchy matters for BackgroundBlurEffect. You should determine "what is the background".
 * Add() should preceed SetRenderEffect(), and the effect cannot have multiple owner controls.
 *
 * @SINCE_2_3.28
 */
class DALI_TOOLKIT_API BackgroundBlurEffect : public RenderEffect
{
public:
  /**
   * @brief Creates an initialized BackgroundBlurEffect, using default settings. The default settings are:
   *
   * downscaleFactor = 0.4f
   * pixelRadius = 5u
   *
   * @SINCE_2_3.28
   * @return A handle to a newly allocated Dali resource
   */
  static BackgroundBlurEffect New();

  /**
   * @brief Creates an initialized BackgroundBlurEffect.
   * @param[in] downscaleFactor This value should reside in the range [0.0, 1.0].
   * @param[in] blurRadius The radius of Gaussian kernel.
   * @SINCE_2_3.28
   * @return A handle to a newly allocated Dali resource
   */
  static BackgroundBlurEffect New(float downscaleFactor, uint32_t blurRadius);

  /**
   * @brief Creates an uninitialized blur effect.
   * @SINCE_2_3.28
   */
  BackgroundBlurEffect();

  /**
   * @brief Copy constructor.
   * @SINCE_2_3.28
   */
  BackgroundBlurEffect(const BackgroundBlurEffect& handle);

  /**
  * @brief Destructor
  * @SINCE_2_3.28
  */
  ~BackgroundBlurEffect();

public: // Not intended for use by Application developers
  ///@cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   * @SINCE_2_3.28
   * @param[in]  blurEffectImpl The UI Control implementation.
   */
  explicit DALI_INTERNAL BackgroundBlurEffect(Internal::BlurEffectImpl* blurEffectImpl);
  ///@endcond
};

} // namespace Toolkit
} // namespace Dali

#endif //DALI_TOOLKIT_BACKGROUND_BLUR_EFFECT_H
