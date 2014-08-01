#ifndef __DALI_TOOLKIT_SCROLL_VIEW_TWIST_EFFECT_H__
#define __DALI_TOOLKIT_SCROLL_VIEW_TWIST_EFFECT_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-effect.h>

namespace Dali DALI_IMPORT_API
{

class Actor;

namespace Toolkit
{

class ScrollViewEffect;

namespace Internal DALI_INTERNAL
{
class ScrollViewTwistEffect;
}

/**
 * @brief ScrollView effect that twists pages onto screen when transitioning.
 */
class ScrollViewTwistEffect : public ScrollViewEffect
{

public:

  static const float DEFAULT_MINIMUM_DISTANCE_FOR_SHRINK; ///< The min distance for shrink

public:

  /**
   * @brief Create an initialized ScrollViewTwistEffect.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static ScrollViewTwistEffect New();

  /**
   * @brief Create an uninitialized ScrollViewTwistEffect; this can be initialized with ScrollViewTwistEffect::New().
   *
   * Calling member functions with an uninitialized Toolkit::ScrollViewTwistEffect is not allowed.
   */
  ScrollViewTwistEffect();

  /**
   * @brief Downcast an Object handle to ScrollViewTwistEffect.
   *
   * If handle points to a ScrollViewTwistEffect the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ScrollViewTwistEffect or an uninitialized handle
   */
  static ScrollViewTwistEffect DownCast( BaseHandle handle );

  /**
   * @brief Gets the minimum animation distance for the shrink effect to
   * occur.
   *
   * @return The minimum distance in seconds is returned.
   */
  float GetMinimumDistanceForShrink() const;

  /**
   * @brief Sets the minimum animation distance for the shrink effect
   * to occur.
   *
   * @param[in] distance The minimum distance in pixels (default = 0.0)
   * i.e. any flick will result in shrinking.
   */
  void SetMinimumDistanceForShrink(float distance);

  /**
   * @brief Enable or disable this effect.
   *
   * @param[in] enableFlag Set to true if the effect should be enabled.
   */
  void EnableEffect(bool enableFlag);

  /**
   * @brief Manually apply effect to an Actor.
   *
   * @param[in] child The child Actor to be affected by this effect.
   * @param[in] additionalEffects Whether just the basic effect (delay)
   * should be applied. Or all effects (delay, rotation, scaling).
   * For all effects set to true. Default is true i.e. apply all effects.
   * @param[in] angleSwing The maximum amount the child actor should
   * rotate in radians for each axis (X and Y) if the scrollview reaches
   * overshoot. Default is PI/2 i.e. 90 degrees rotation.
   * @param[in] scaleAmount The relative amount to shrink Actors as they
   * are panned fast (flick animation). default is 0.125 (12.5% shrinkage)
   * @param[in] delayMin The minimum delay coefficient for Actors at the
   * scroll-view center. Set to 0 for instantaneous, and 1 for infinite delay.
   * Default is 0.0f
   * @param[in] delayMax The maximum delay coefficient for Actors at the
   * scroll-view approx 1 ScrollView size from the center. Set to 0 for
   * instantaneous, and 1 for infinite delay. Default is 0.9f (a slight delay)
   */
  void ApplyToActor( Actor child,
                     bool additionalEffects = true,
                     const Vector2& angleSwing = Vector2( Math::PI_4, Math::PI_4 ),
                     float scaleAmount = 0.125f,
                     float delayMin = 0.0f,
                     float delayMax = 0.9f );

  /**
   * @brief Set the maximum swing angle when at zero drop off.
   *
   * @param[in] maxSwingAngle maximum swing angle for x and y axes
   */
  void SetMaxSwingAngle(const Vector2& maxSwingAngle);

  /**
   * @brief Set the drop off values to affect the amount of swing
   * angle applied to an actor the further it is from the scroll
   * position.
   *
   * A drop off of 0.0f means no angle drop off while 1.0f will reduce
   * the angle to zero over the distance supplied for that axis.
   *
   * Example maxSwingAngle.x is Pi, dropOff.x is 0.5f and distance.x is 100.0f:
   *    The angle on the x axis will reduce to (0.5f * Pi) over 100 pixels
   *
   * @param[in] dropOff amount to reduce swing angle by in the range [0.0f, 1.0f]. 0.0f
   * @param[in] distance distance to apply dropOff in pixels
   * @param[in] function Alpha Function to affect how drop off is applied over distance, NULL for linear application
   */
  void SetSwingDropOff(const Vector2& dropOff, const Vector2& distance, AlphaFunction function = NULL);

protected:

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] impl A pointer to a newly allocated Dali resource
   */
  ScrollViewTwistEffect(Internal::ScrollViewTwistEffect *impl);

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_VIEW_TWIST_EFFECT_H__
