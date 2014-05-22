#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_TWIST_EFFECT_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_TWIST_EFFECT_H__

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
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/ref-object.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-effect.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-twist-effect.h>

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
 * @copydoc Toolkit::ScrollViewTwistEffect
 */
class ScrollViewTwistEffect : public ScrollViewEffect
{

public:
  enum Flag
  {
    FlagTwist = 0x01,
    FlagScale = 0x02,
    FlagDropOff = 0x04,
    FlagDefaultDropOff = 0x08,
    DefaultFlags = FlagTwist | FlagDropOff | FlagDefaultDropOff
  };

  /**
   * Constructor
   */
  ScrollViewTwistEffect();

public:

  /**
   * @copydoc ScrollViewEffect::GetMinimumDistanceForShrink
   */
  float GetMinimumDistanceForShrink() const;

  /**
   * @copydoc ScrollViewEffect::SetMinimumDistanceForShrink
   */
  void SetMinimumDistanceForShrink(float distance);

  /**
   * enable or disable this effect
   * @param[in] enable when enableFlag is true, disable when enableFlag is false.
   */
  void EnableEffect(bool enableFlag);

  /**
   * @copydoc ScrollViewEffect::ApplyToActor
   */
  void ApplyToActor( Actor child,
                     bool additionalEffects,
                     const Vector2& angleSwing,
                     float scaleAmount,
                     float delayMin,
                     float delayMax );

  /**
   * @copydoc Toolkit::ScrollViewEffect::Apply
   */
  void Apply(Actor child);

  /**
   * @copydoc Toolkit::ScrollViewEffect::SetMaxSwingAngle
   */
  void SetMaxSwingAngle(const Vector2& maxSwingAngle) { mMaxSwingAngle = maxSwingAngle; }

  /**
   * @copydoc Toolkit::ScrollViewEffect::SetSwingDropOff
   */
  void SetSwingDropOff(const Vector2& dropOff, const Vector2& distance, AlphaFunction function = NULL);

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
  virtual ~ScrollViewTwistEffect();

private:

  /**
   * Signal handler, called when the ScrollView starts to move
   *
   * @param[in] position The current scroll position
   */
  void OnScrollStart( const Vector3& position );

  /**
   * Signal handler, called when the ScrollView is moving
   *
   * @param[in] position The current scroll positionApply
   */
  void OnScrollUpdate( const Vector3& position );

  /**
   * Signal handler, called when the ScrollView has completed movement
   *
   * @param[in] position The current scroll position
   */
  void OnScrollComplete( const Vector3& position );

  void OnScrollSnap( const Toolkit::ScrollView::SnapEvent& event );

  /**
   * Signal handler, called when the Wobble Effect animation has completed.
   *
   * @param[in] animation The animation.
   */
  void OnAnimationFinished( Animation& animation );

  void OnActivateAnimationFinished( Animation& animation );
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

  ushort mFlags;
  Animation mAnimation;                         ///< Animation Timer to drive the twist effect constraint.
  Animation mActivateAnimation;
  Property::Index mPropertyTime;                ///< Time property used by twist effect constraint to calculate timePassed.
  bool mEnableEffect;                           ///< flag that decide whether enable or disable the twist effect.
  bool mAdditionalEffects;                      ///< whether we want to apply extra effects
  Property::Index mPropertyReference;           ///< Reference point in scroll-contents, this point has no delay.
  Property::Index mPropertyActivate;                                              ///::< The further out from this point, the further the delay.
  Property::Index mPropertyDepth;               ///< Depth property used by twist effect constraint to shrink Actors.
  float mMinimumDistanceForShrink;              ///< Scrolling animation must last for longer than this amount to do shrink effect.s
  Vector2 mMaxSwingAngle;                       ///< Maximum swing angle to be applied to x and y axes
  Vector2 mDropOff;                             ///< Amount to drop off the angle on each axis of drop off distance
  Vector2 mDropOffDistance;                     ///< Distance to apply the full drop off over
  AlphaFunction mDropOffFunction;                      ///< Function to adjust how drop off is aplied over the distance, NULL for linear
  float mScaleFactor;                           ///< Amount to scale by during translation
  float mDelayMin;
  float mDelayMax;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ScrollViewTwistEffect& GetImpl(Dali::Toolkit::ScrollViewTwistEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::RefObject& handle = obj.GetBaseObject();

  return static_cast<Internal::ScrollViewTwistEffect&>(handle);
}

inline const Internal::ScrollViewTwistEffect& GetImpl(const Dali::Toolkit::ScrollViewTwistEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::RefObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::ScrollViewTwistEffect&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_TWIST_EFFECT_H__
