#ifndef DALI_TOOLKIT_BLUR_EFFECT_H
#define DALI_TOOLKIT_BLUR_EFFECT_H
/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
// EXTERNAL INCLUDES
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/time-period.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class GaussianBlurEffectImpl;
} // namespace DALI_INTERNAL

/**
 * @brief GaussianBlurEffect is a visual effect that blurs owner control and its children.
 *
 * @code
 * GaussianBlurEffect effect = GaussianBlurEffect::New();
 * control.SetRenderEffect(effect); // Activate
 * effect.Deactivate();
 * effect.Activate();
 * control.ClearRenderEffect(); // Deactivate
 * @endcode
 *
 * @SINCE_2_4.20
 */
class DALI_TOOLKIT_API GaussianBlurEffect : public RenderEffect
{
public:
  /**
   * @brief Typedef for finished signals sent by this class.
   * @SINCE_2_4.20
   */
  typedef Signal<void()> FinishedSignalType;

  /**
   * @brief Creates an initialized GaussianBlurEffect, with default blur radius 10u.
   * @SINCE_2_4.20
   * @return A handle to a newly allocated Dali resource
   */
  static GaussianBlurEffect New();

  /**
   * @brief Creates an initialized GaussianBlurEffect.
   * @param[in] blurRadius The radius of Gaussian kernel.
   * @SINCE_2_4.20
   * @return A handle to a newly allocated Dali resource
   */
  static GaussianBlurEffect New(uint32_t blurRadius);

  /**
   * @brief Creates an uninitialized blur effect.
   * @SINCE_2_4.20
   */
  GaussianBlurEffect();

  /**
   * @brief Copy constructor
   * @SINCE_2_4.20
   */
  GaussianBlurEffect(const GaussianBlurEffect& handle);

  /**
   * @brief Destructor
   * @SINCE_2_4.20
   */
  ~GaussianBlurEffect();

  /**
   * @copydoc Dali::Toolkit::BackgroundBlurEffect::SetBlurOnce
   */
  void SetBlurOnce(bool blurOnce);

  /**
   * @copydoc Dali::Toolkit::BackgroundBlurEffect::GetBlurOnce
   */
  bool GetBlurOnce() const;

  /**
   * @copydoc Dali::Toolkit::BackgroundBlurEffect::GetBlurOnce
   */
  void SetBlurRadius(uint32_t blurRadius);

  /**
   * @copydoc Dali::Toolkit::BackgroundBlurEffect::GetBlurOnce
   */
  uint32_t GetBlurRadius() const;

  /**
   * @copydoc Dali::Toolkit::BackgroundBlurEffect::AddBlurStrengthAnimation
   */
  void AddBlurStrengthAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod, float fromValue, float toValue);

  /**
   * @copydoc Dali::Toolkit::BackgroundBlurEffect::AddBlurOpacityAnimation
   */
  void AddBlurOpacityAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod, float fromValue, float toValue);

public: // Signals
  /**
   * @copydoc Dali::Toolkit::BackgroundBlurEffect::FinishedSignal
   */
  FinishedSignalType& FinishedSignal();

public: // Not intended for use by Application developers
  ///@cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   * @SINCE_2_4.20
   * @param[in]  blurEffectImpl The UI Control implementation.
   */
  explicit DALI_INTERNAL GaussianBlurEffect(Internal::GaussianBlurEffectImpl* gaussianBlurEffectImpl);
  ///@endcond
};
} // namespace Toolkit
} // namespace Dali
#endif // DALI_TOOLKIT_BLUR_EFFECT_H
