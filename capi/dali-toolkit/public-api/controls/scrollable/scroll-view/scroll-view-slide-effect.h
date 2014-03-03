#ifndef __DALI_TOOLKIT_SCROLL_VIEW_SLIDE_EFFECT_H__
#define __DALI_TOOLKIT_SCROLL_VIEW_SLIDE_EFFECT_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// EXTERNAL INCLUDES

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
class ScrollViewSlideEffect;
}

/**
 * ScrollView Twist-Effect.
 */
class ScrollViewSlideEffect : public ScrollViewEffect
{

public:

  static const std::string EFFECT_TIME;
  static const std::string EFFECT_REFERENCE;
  static const std::string EFFECT_ACTIVE;

public:

  /**
   * Create an initialized ScrollViewSlideEffect.
   * @return A handle to a newly allocated Dali resource.
   */
  static ScrollViewSlideEffect New();

  /**
   * Create an uninitialized ScrollViewSlideEffect; this can be initialized with ScrollViewSlideEffect::New()
   * Calling member functions with an uninitialized Toolkit::ScrollViewSlideEffect is not allowed.
   */
  ScrollViewSlideEffect();

  /**
   * Downcast an Object handle to ScrollViewSlideEffect. If handle points to a ScrollViewSlideEffect the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ScrollViewSlideEffect or an uninitialized handle
   */
  static ScrollViewSlideEffect DownCast( BaseHandle handle );

  /**
   * Gets the slide direction for this effect.
   * @return The slide direction (true = vertical, false = horizontal)
   */
  bool GetSlideDirection() const;

  /**
   * Sets the slide direction for this effect.
   * If the direction has been set to horizontal (false), then
   * the user will see the Actors have a delay in horizontal movement
   * based on the vertical distance the actor is away from the initial drag point.
   * If the direction has been set to vertical (true), then the
   * user will experience the opposite effect (i.e. delay in the vertical movement).
   * @param[in] vertical The slide direction (true = vertical, false = horizontal)
   * (default is horizontal i.e. false)
   */
  void SetSlideDirection(bool vertical);

  /**
   * Gets the delay reference offset for this effect.
   * @return The delay reference offset (Vector3::ZERO - indicates no offset)
   */
  Vector3 GetDelayReferenceOffset() const;

  /**
   * Sets an offset for where the central delay point on the scroll-view should be
   * when dragging.
   * By default the offset is 0. Which means that the point where the user drags
   * the scroll-view content should have no delay, and the further away from this
   * point, the delay should increase. Adjusting this offset to for example
   * 0.0f, -stageSize.height * 0.5f, will mean that dragging the center of the stage
   * will result in the content at the top of the stage moving with no delay, and
   * the further away from this point (top of stage), the delay should increase.
   * @param[in] offset The offset in local coordinates, relative to the ScrollView.
   */
  void SetDelayReferenceOffset(const Vector3& offset);

  /**
   * Gets the maximum duration of the effect after scrolling completes
   * @return The duration in seconds
   */
  float GetMaxDelayDuration() const;

  /**
   * Sets the maximum duration of the effect after scrolling completes
   * @param[in] duration The duration in seconds (>= 0.0f, default is 0.25 seconds)
   */
  void SetMaxDelayDuration(float duration);

  /**
   * Manually apply effect to an Actor.
   * @param[in] child The child Actor to be affected by this effect.
   * @param[in] delayMin The minimum delay coefficient for Actors at the
   * scroll-view touch point. Set to 0 for instantaneous, and 1 for infinite delay.
   * Default is 0.5f
   * @param[in] delayMax The maximum delay coefficient for Actors at the
   * scroll-view approx 1 ScrollView size from the touch point. Set to 0 for
   * instantaneous, and 1 for infinite delay. Default is 0.99f (a noticable delay)
   */
  void ApplyToActor( Actor child,
                     float delayMin = 0.5f,
                     float delayMax = 0.95f );

protected:

  /**
   * This constructor is used by Dali New() methods.
   * @param [in] impl A pointer to a newly allocated Dali resource
   */
  ScrollViewSlideEffect(Internal::ScrollViewSlideEffect *impl);

};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_SCROLL_VIEW_SLIDE_EFFECT_H__
