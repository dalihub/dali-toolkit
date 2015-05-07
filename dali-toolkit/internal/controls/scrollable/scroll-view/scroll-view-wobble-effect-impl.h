#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_WOBBLE_EFFECT_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_WOBBLE_EFFECT_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/actors/custom-actor.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-effect.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-wobble-effect.h>

namespace Dali
{

class Animation;

namespace Toolkit
{

class ScrollView;

namespace Internal
{

class ScrollViewEffect;

/**
 * @copydoc Toolkit::ScrollViewWobbleEffect
 */
class ScrollViewWobbleEffect : public ScrollViewEffect
{

public:

  /**
   * Constructor
   */
  ScrollViewWobbleEffect();

  /**
   * Increases the Stable Count (when this reaches mStableTotal)
   * Then all constraints are stable, and the animation can stop.
   */
  void IncrementStableCount();

  /**
   * Returns animation cycle id. Every time a new animation starts
   * this cycle id is increased.
   *
   * @return The Animation Cycle id is returned.
   */
  unsigned int GetAnimationCycleId() const;

public:

  /**
   * @copydoc ScrollViewEffect::OnAttach
   */
  virtual void OnAttach(Toolkit::ScrollView& scrollView);

  /**
   * @copydoc ScrollViewEffect::OnDetach
   */
  virtual void OnDetach(Toolkit::ScrollView& scrollView);

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ScrollViewWobbleEffect();

private:

  /**
   * Signal handler, called when the ScrollView starts to move
   *
   * @param[in] position The current scroll position
   */
  void OnScrollStart( const Vector2& position );

  /**
   * Signal handler, called when the ScrollView is moving
   *
   * @param[in] position The current scroll position
   */
  void OnScrollUpdate( const Vector2& position );

  /**
   * Signal handler, called when the ScrollView has completed movement
   *
   * @param[in] position The current scroll position
   */
  void OnScrollComplete( const Vector2& position );

  /**
   * Signal handler, called when the Wobble Effect animation has completed.
   *
   * @param[in] animation The animation.
   */
  void OnAnimationFinished( Animation& animation );

  /**
   * Attaches effect to Scroll Actor (ScrollView)
   *
   * Applies the same wobble effect to each Scroll Actor.
   *
   * @param[in] actor The attached Actor
   */
  void AttachActor(Actor actor);

  /**
   * Detaches effect from Scroll Actor (ScrollView)
   *
   * @param[in] actor The attached Actor
   */
  void DetachActor(Actor actor);

  /**
   * Continues Animation to time reaches endTime
   *
   * @param[in] endTime the target time to reach.
   */
  void ContinueAnimation(float endTime);

private:

  Animation mAnimation;                         ///< Animation Timer to drive the wobble effect constraint.
  Property::Index mPropertyTime;                ///< Time property used by wobble effect constraint to calculate timePassed.

  int mStableCurrent;                           ///< Stability current - how many wobble constraints are not wobbling (or have neglible wobble).
  unsigned int mAnimationCycleId;               ///< The start of each new animation cycle is a unique number.

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_WOBBLE_EFFECT_H__
