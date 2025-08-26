#ifndef DALI_TOOLKIT_BACKGROUND_BLUR_EFFECT_H
#define DALI_TOOLKIT_BACKGROUND_BLUR_EFFECT_H
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
class BackgroundBlurEffectImpl;
} // namespace Internal DALI_INTERNAL

/**
 * @brief BackgroundBlurEffect is a visual effect that blurs owner control's background.
 *
 * @code
 * BackgroundBlurEffect effect = BackgroundBlurEffect::New();
 * control.SetRenderEffect(effect); // Activate
 * effect.Deactivate();
 * effect.Activate();
 * control.ClearRenderEffect(); // Deactivate
 * @endcode
 *
 * @note The owner control owns at most one render effect.
 * @note Tree hierarchy matters for BackgroundBlurEffect. You should determine "what is the background".
 *
 * @SINCE_2_3.28
 */
class DALI_TOOLKIT_API BackgroundBlurEffect : public RenderEffect
{
public:
  /**
   * @brief Typedef for finished signals sent by this class.
   *
   * @SINCE_2_4.15
   */
  typedef Signal<void()> FinishedSignalType;

  /**
   * @brief Creates an initialized BackgroundBlurEffect, using default settings. As default, blur radius is set to 10u.
   * @SINCE_2_3.28
   * @return A handle to a newly allocated Dali resource
   */
  static BackgroundBlurEffect New();

  /**
   * @brief Creates an initialized BackgroundBlurEffect.
   * @param[in] blurRadius The radius of Gaussian kernel.
   * @SINCE_2_3.28
   * @return A handle to a newly allocated Dali resource
   * @note Due to the downscale factor and kernel optimizations, the actual applied blur changes only in fixed increments.
   * @note The increment size is (2 / downscaleFactor). For example, with downscaleFactor = 0.25, the effective blur radius changes in steps of 8 pixels, so small adjustments below that threshold will have no visual effect.
   */
  static BackgroundBlurEffect New(uint32_t blurRadius);

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

  /**
   * @brief Set whether to refresh effect once or every frame.
   * @param[in] blurOnce If true, renders effect once, else updates effect rendering every frame.
   * @SINCE_2_4.15
   */
  void SetBlurOnce(bool blurOnce);

  /**
   * @brief Retrives whether effect rendering is done once(true) or every frame(false)
   * @SINCE_2_4.15
   * @return Whether blur is rendered once or every frame.
   */
  bool GetBlurOnce() const;

  /**
   * @brief Set blur radius value.
   * @param[in] blurRadius The radius of Gaussian kernel.
   * @SINCE_2_4.15
   * @note Due to the downscale factor and kernel optimizations, the actual applied blur changes only in fixed increments.
   * @note The increment size is (2 / downscaleFactor). For example, with downscaleFactor = 0.25, the effective blur radius changes in steps of 8 pixels, so small adjustments below that threshold will have no visual effect.
   */
  void SetBlurRadius(uint32_t blurRadius);

  /**
   * @brief Retrieves blur radius value.
   * @SINCE_2_4.15
   * @return The blur radius.
   */
  uint32_t GetBlurRadius() const;

  /**
   * @brief Set blur downscale factor. The value resides between 0.0f to 1.0f.
   * @param[in] downscaleFactor The downscale factor for input texture.
   * @SINCE_2_4.23
   */
  void SetBlurDownscaleFactor(float downscaleFactor);

  /**
   * @brief Retrives blur downscale factor.
   * @SINCE_2_4.23
   * @return The blur downscale factor.
   */
  float GetBlurDownscaleFactor() const;

  /**
   * @brief Adds blur strength animation. Blurifies clear texture within given animation parameters.
   * @param[in] animation Animation instance to which we add blur strength animation.
   * @param[in] alphaFunction AlphaFunction of blur strength animation.
   * @param[in] timePeriod TimePeriod of blur strength animation. Default value is animation's duration.
   * @param[in] fromValue Starting value of blur strength. Must be in range of [0.0f, 1.0f]
   * @param[in] toValue End value of blur strength. Must be in range of [0.0f, 1.0f]
   * @note If toValue is smaller than fromValue, animation would show reversed(blurred->clarified) animation.
   * @note When choosing alpha function, note that gaussian curve itself is innately non-linear.
   * @SINCE_2_4.15
   */
  void AddBlurStrengthAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod, float fromValue, float toValue);

  /**
   * @brief Adds blur opacity animation. Blurifies clear texture within given animation parameters.
   * @param[in] animation Animation instance to which we add blur opacity animation.
   * @param[in] alphaFunction AlphaFunction of blur opacity animation.
   * @param[in] timePeriod TimePeriod of blur opacity animation. Default value is animation's duration.
   * @param[in] fromValue Starting value of blur opacity. Must be in range of [0.0f, 1.0f]
   * @param[in] toValue End value of blur opacity. Must be in range of [0.0f, 1.0f]
   * @note If toValue is smaller than fromValue, animation would show reversed(blurred->clarified) animation.
   * @note When choosing alpha function, note that gaussian curve itself is innately non-linear.
   * @SINCE_2_4.15
   */
  void AddBlurOpacityAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod, float fromValue, float toValue);

  /**
   * @brief Set specific source actor of background blur effects. If empty handle, works same as normal.
   * @note If given source actor is not a parent of source control, it has no efforts.
   * @warning RenderEffect didn't hold source actor reference.
   * @param[in] sourceActor The source actor of background blur effects.
   * @SINCE_2_4.33
   */
  void SetSourceActor(Dali::Actor sourceActor);

  /**
   * @brief Set specific stopper actor of background blur effects. If empty handle, works same as normal.
   * @warning RenderEffect didn't hold stopper actor reference.
   * @param[in] stopperActor The stopper actor of background blur effects.
   * @SINCE_2_4.33
   */
  void SetStopperActor(Dali::Actor stopperActor);

public: // Signals
  /**
   * @brief If blurOnce is true and effect is activated, then connect to this signal to be notified when the
   * target actor has been rendered.
   * @SINCE_2_4.15
   * @return The finished signal
   */
  FinishedSignalType& FinishedSignal();

public: // Not intended for use by Application developers
  ///@cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   * @SINCE_2_3.28
   * @param[in]  blurEffectImpl The UI Control implementation.
   */
  explicit DALI_INTERNAL BackgroundBlurEffect(Internal::BackgroundBlurEffectImpl* backgroundBlurEffectImpl);
  ///@endcond
};
} // namespace Toolkit
} // namespace Dali
#endif // DALI_TOOLKIT_BACKGROUND_BLUR_EFFECT_H
