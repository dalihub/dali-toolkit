#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_SLIDE_EFFECT_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_SLIDE_EFFECT_H__

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
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-slide-effect.h>

namespace Dali
{

class Animation;

namespace Toolkit
{

class ScrollView;

namespace Internal
{

class ScrollViewEffect;

class ScrollSlideInfo;

typedef IntrusivePtr<ScrollSlideInfo> ScrollSlideInfoPtr;

/**
 * @copydoc Toolkit::ScrollViewSlideEffect
 */
class ScrollViewSlideEffect : public ScrollViewEffect
{
public:

  /**
   * Constructor
   */
  ScrollViewSlideEffect();

public:

  /**
   * @copydoc ScrollViewEffect::GetSlideDirection
   */
  bool GetSlideDirection() const;

  /**
   * @copydoc ScrollViewEffect::SetSlideDirection
   */
  void SetSlideDirection(bool vertical);

  /**
   * @copydoc ScrollViewEffect::GetDelayReferenceOffset
   */
  const Vector3& GetDelayReferenceOffset() const;

  /**
   * @copydoc ScrollViewEffect::SetDelayReferenceOffset
   */
  void SetDelayReferenceOffset(const Vector3& offset);

  /**
   * @copydoc ScrollViewEffect::GetMaxDelayDuration
   */
  float GetMaxDelayDuration() const;

  /**
   * @copydoc ScrollViewEffect::SetMaxDelayDuration
   */
  void SetMaxDelayDuration(float duration);

  /**
   * @copydoc ScrollViewEffect::ApplyToActor
   */
  void ApplyToActor( Actor child,
                     float delayMin,
                     float delayMax );
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
  virtual ~ScrollViewSlideEffect();

private:

  /**
   * Invoked when user touches the scroll-view
   * We keep track of the touch as this is used to determine
   * the reference point which is used to determine the delay
   * factor for the Actors' movements.
   * @param[in] actor The actor touched
   * @param[in] event The touch Event
   * @return Whether to consume the even or not.
   */
  bool OnScrollTouched(Actor actor, const TouchEvent& event);

  /**
   * Signal handler, called when the ScrollView starts to move
   *
   * @param[in] position The current scroll position
   */
  void OnScrollStart( const Vector3& position );

  /**
   * Signal handler, called when the ScrollView starts to snap
   * @param[in] event The snap event.
   */
  void OnScrollSnapStarted(const Toolkit::ScrollView::SnapEvent& event);

  /**
   * Signal handler, called some time after the ScrollView has completed
   * movement. There is a delay as when the ScrollView has completed
   * movement, there are Actors that have a delay, and take time to arrive
   * at their final destination.
   *
   * @param[in] animation The animation delegate for this delay
   */
  void OnAnimationSnapFinished( Animation& animation );

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

  ScrollSlideInfoPtr mScrollSlideInfo;                  ///< Info structure to keep track of common properties amongst many constraints.
  ActiveConstraint mInfoUpdateConstraint;               ///< Constraint applied to scroll-view to update Info structure.
  Animation mAnimation;                                 ///< Animation Timer to drive the twist effect constraint.
  Animation mAnimationSnap;                             ///< Animation Snap (this animates from from 1.0 to 0.0 when contents snap)
  Property::Index mPropertyTime;                        ///< Time property used by twist effect constraint to calculate timePassed.
  Property::Index mPropertyReference;                   ///< Reference point in scroll-contents, this point has no delay.
                                                        ///< The further out from this point, the further the delay.
  Property::Index mPropertyActive;                      ///< Property indicates the progress of the scrolling from 1.0f (scrolling) to 0.0f (fully snapped)
  Vector3 mDelayReferenceOffset;                        ///< Where to offset the delay reference point when dragging.
  float mMaxDelayDuration;                              ///< Maximum duration of effect after scroll-view completes.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ScrollViewSlideEffect& GetImpl(Dali::Toolkit::ScrollViewSlideEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::RefObject& handle = obj.GetBaseObject();

  return static_cast<Internal::ScrollViewSlideEffect&>(handle);
}

inline const Internal::ScrollViewSlideEffect& GetImpl(const Dali::Toolkit::ScrollViewSlideEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::RefObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::ScrollViewSlideEffect&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_SLIDE_EFFECT_H__
