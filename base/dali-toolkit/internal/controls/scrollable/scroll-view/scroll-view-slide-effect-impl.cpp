
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

// CLASS HEADER
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-slide-effect-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/common/stage.h>

// INTERNAL INCLUDES

using namespace Dali;

typedef Dali::Toolkit::Internal::ScrollSlideInfo ScrollSlideInfo;
typedef Dali::Toolkit::Internal::ScrollSlideInfoPtr ScrollSlideInfoPtr;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * ScrollSlideInfo structure contains
 * common info that is shared amongst the constraints applied to Actors.
 * The constraints + effect all share ownership of this info struct.
 * The info is written to by the ScrollSlideInfoUpdate constraint. While
 * the other constraints read from by the other constraints. Due to the order
 * in which the constraints are applied, all constraints will get the current
 * property values for these properties.
 * The advantage of doing this is that:
 * A) Constraints are not restricted by the 6 property limit. to function.
 * B) Properties which rarely change or only change when another property changes
 * (e.g. time), such as scroll position, scroll domain, size, wrap mode don't need
 * to be checked for each constraint to apply.
 */
class ScrollSlideInfo : public RefObject
{

public:

  /**
   * Constructor
   */
  ScrollSlideInfo()
  : mScrollWrap(false),
    mVertical(false)
  {
  }

  /**
   * Destructor
   */
  virtual ~ScrollSlideInfo()
  {
  }

public:

  Vector3 mScrollPosition;
  Vector3 mEffectReference;
  Vector3 mScrollSize;
  Vector3 mScrollPositionMin;
  Vector3 mScrollPositionMax;
  bool mScrollWrap;
  bool mVertical;

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali


namespace // unnamed namespace
{

const float SLIDEEFFECT_ANIMATION_MAX_TIME = 60.0f;      ///< Animation time (every time finishes, checks if it needs to go again)
const float COMPLETION_START_DURATION = 0.25f;           ///< Maximum time for completion of effect after scroll-view initially completes (due to delay effect)
const float COMPLETION_END_DURATION = 5.0f;              ///< Maximum time for completion of effect after scroll-view initially completes (due to delay effect)
const float ANIMATION_BLEND_COEFFICIENT = 0.05f;         ///< Animation blending coefficient (blends between target value e.g. 5% and current value 9%)
const float INV_ANIMATION_BLEND_COEFFICIENT = (1.0f - ANIMATION_BLEND_COEFFICIENT);
const float DEFAULT_MAX_DELAY_DURATION = 0.25f;          ///< Default maximum delay duration of the effect after scroll completes is 0.25f
const float EFFECT_SNAP_GROW_DURATION = 0.33f;           ///< Take 1/3rd of a second for the snap effect property to grow
const float EFFECT_SNAP_DECAY_DURATION = 0.667f;         ///< Take 2/3rds of a second for the snap effect property to decay

/**
 * Gets a property index. If the property doesn't already exist, then
 * it will create the property.
 * @param[in] handle The handle that owns or will own the property
 * @param[in] name The name for this property
 * @param[in] propertyValue The initial value for this property
 * @return The property index for this property is returned.
 */
Property::Index SafeRegisterProperty( Handle& handle, const std::string& name, Property::Value propertyValue )
{
  Property::Index index = handle.GetPropertyIndex( name );

  if(index == Property::INVALID_INDEX)
  {
    index = handle.RegisterProperty( name, propertyValue );
  }

  return index;
}

/**
 * Re-scales input value x from x0 - x1, to linearly map
 * over the values y0 - y1. Values outside of this range
 * will also conform to the trend (gradient) set.
 * @param[in] y0 output minimum bound
 * @param[in] y1 output maximum bound
 * @param[in] x input X value
 * @param[in] x0 (optional) input minimum bound (default 0.0)
 * @param[in] x1 (optional) input maximum bound (default 1.0)
 * @return The result of the mapping is returned.
 */
float Mix(float y0, float y1, float x, float x0 = 0.0f, float x1 = 1.0f)
{
  return y0 + (y1 - y0) * (x - x0) / (x1-x0);
}

/**
 * Returns the value of x chasing target.
 * returns a value of x which is closer to target.
 * but limited by maxDelta. #
 * For example:
 * x = 10.0f
 * target = 50.0f
 * maxDelta = 20.0f
 * result is 30.0f (x is 20.0f units closer to target)
 * However, if x is already within maxDelta units
 * of target, x will equal target.
 * For example:
 * x = 55.0f
 * target = 50.0f
 * maxDelta = 20.0f
 * result is 50.0f (x was already within 20.0f units of target)
 */
float Chase( float x, float target, float maxDelta )
{
  float delta = target - x;

  if(delta > 0.0f)
  {
    x = std::min( x + maxDelta, target );
  }
  else
  {
    x = std::max( x - maxDelta, target );
  }

  return x;
}

// constraints ////////////////////////////////////////////////////////////////

/**
 * ScrollSlideInfoUpdate
 *
 * Info constraint updates an info struct with property info,
 * so that constraints can use this instead of having it passed through
 * as parameters.
 */
struct ScrollSlideInfoUpdate
{
  /**
   * Constraint constructor
   */
  ScrollSlideInfoUpdate(ScrollSlideInfoPtr scrollInfo)
  : mScrollSlideInfo(scrollInfo)
  {
  }

  /**
   * @param[in] current The current value
   * @param[in] scrollPositionProperty The scroll-view's position property (SCROLL_POSITION_PROPERTY_NAME)
   * @param[in] effectReferenceProperty The point in the scroll-view where the user touched the screen
   * @param[in] effectTimeProperty The current timer. Starting from 0.0 when scroll animation/dragging
   * commences. Ensures that constraint is applied and processed every frame (to achieve the delay effect)
   * @param[in] sizeProperty The size of the ScrollView.
   * @return The new position of this Actor.
   */
  float operator()(const float& current,
                     const PropertyInput& scrollPositionProperty,
                     const PropertyInput& effectReferenceProperty,
                     const PropertyInput& scrollSizeProperty,
                     const PropertyInput& scrollPositionMinProperty,
                     const PropertyInput& scrollPositionMaxProperty,
                     const PropertyInput& scrollWrapProperty)
  {
    mScrollSlideInfo->mScrollPosition = scrollPositionProperty.GetVector3();
    mScrollSlideInfo->mEffectReference = effectReferenceProperty.GetVector3();
    mScrollSlideInfo->mScrollSize = scrollSizeProperty.GetVector3();
    mScrollSlideInfo->mScrollPositionMin = scrollPositionMinProperty.GetVector3();
    mScrollSlideInfo->mScrollPositionMax = scrollPositionMaxProperty.GetVector3();
    mScrollSlideInfo->mScrollWrap = scrollWrapProperty.GetBoolean();

    return current;
  }

private:

  ScrollSlideInfoPtr mScrollSlideInfo;

};


/**
 * ScrollSlidePositionConstraint
 *
 * Position constraint adjusts the position of the Actors
 * based on their parent page's position relative to the middle of the screen.
 * When at middle of the screen the position is not altered.
 * When one screen away from middle the position is rotated about it's origin + mAnchor
 */
struct ScrollSlidePositionConstraint
{
  /**
   * Constraint constructor
   */
  ScrollSlidePositionConstraint(ScrollSlideInfoPtr scrollInfo, float delayMin, float delayMax)
  : mScrollSlideInfo(scrollInfo),
    mScrollPosition(scrollInfo->mScrollPosition),
    mDelayMin(delayMin),
    mDelayMax(delayMax),
    mAverageSpeed(0.0f)
  {
  }

  /**
   * @param[in] current The current position
   * @param[in] scrollPositionProperty The scroll-view's position property (SCROLL_POSITION_PROPERTY_NAME)
   * @param[in] effectReferenceProperty The point in the scroll-view where the user touched the screen
   * @param[in] effectTimeProperty The current timer. Starting from 0.0 when scroll animation/dragging
   * commences. Ensures that constraint is applied and processed every frame (to achieve the delay effect)
   * @param[in] sizeProperty The size of the ScrollView.
   * @return The new position of this Actor.
   */
  Vector3 operator()(const Vector3& current,
                     const PropertyInput& pagePositionProperty,
                     const PropertyInput& effectTimeProperty,
                     const PropertyInput& deltaPositionProperty,
                     const PropertyInput& snapProperty)
  {
    const float complete = snapProperty.GetFloat();
    bool activate = (complete > Math::MACHINE_EPSILON_1);
    const Vector3& pagePosition = pagePositionProperty.GetVector3();
    const Vector3& scrollPosition = mScrollSlideInfo->mScrollPosition;

    // Get position of page.
    Vector2 relativePosition(pagePosition + scrollPosition);

    // short circuit: for orthognal view and when the blending has been deactivated.
    if( (!activate) &&
        (fabsf(relativePosition.x) < Math::MACHINE_EPSILON_1) &&
        (fabsf(relativePosition.y) < Math::MACHINE_EPSILON_1) )
    {
      Vector3 actorPosition(current + scrollPosition);
      return actorPosition;
    }

    const Vector3& referencePoint = mScrollSlideInfo->mEffectReference;
    const Vector3& scrollSize = mScrollSlideInfo->mScrollSize;
    const Vector3& deltaPosition = deltaPositionProperty.GetVector3();

    // 1. Determine the relative position of the actor from the scrolling reference point.
    // (the further away from the reference, the longer the delay should be)
    const Vector3& min = mScrollSlideInfo->mScrollPositionMin;
    const Vector3& max = mScrollSlideInfo->mScrollPositionMax;

    relativePosition.y = (pagePosition.y + current.y - referencePoint.y) / scrollSize.height;

    // Smoothen the relativePosition value by averaging with mRelativePosition (avoids sudden jerk when
    // user touche different points)
    float shortestDirection = ShortestDistanceInDomain(mRelativePosition.y, relativePosition.y, min.y, max.y);
    mRelativePosition.y += activate ? shortestDirection * ANIMATION_BLEND_COEFFICIENT : shortestDirection;

    // f represents this absolute distance. Get as a relative distance and inverse exponential
    // (as delay equation has an exponential effect i.e. the closer delayFactor to 1.0f,
    // the longer the delay would appear exponentially)
    float f = fabsf(mRelativePosition.y) * complete;
    f = std::min(f, 1.0f);
    f = 1.0f - powf(ANIMATION_BLEND_COEFFICIENT, f);

    // at center delay factor is mDelayMin, at maximum (1.0) it is mDelayMax
    f = Mix(mDelayMin, mDelayMax, f);

    // 2. Now that f (delay factor) has been determined for this Actor,
    // move mScrollPosition towards the actual scroll position, at rate determined by f.
    float shortest = ShortestDistanceInDomain(mScrollPosition.x, WrapInDomain(scrollPosition.x, -min.x, -max.x), min.x, max.x);
    mScrollPosition.x += activate ? shortest * (1.0f-f) : shortest;
    mScrollPosition.x = WrapInDomain(mScrollPosition.x, -min.x, -max.x);
    mScrollPosition.y = scrollPosition.y;

    Vector3 actorPosition(current + pagePosition + mScrollPosition);

    // Get position of actor.
    bool wrap = mScrollSlideInfo->mScrollWrap;

    if(wrap)
    {
      if(fabsf(min.x - max.x) > Math::MACHINE_EPSILON_1)
      {
        // WRAP X (based on the position of the right side)
        actorPosition.x = WrapInDomain(actorPosition.x + scrollSize.x, min.x, max.x) - scrollSize.width;
      }
    }

    const float targetRelativePositionX = (referencePoint.x + deltaPosition.x);

    float blend(Mix(1.0f, ANIMATION_BLEND_COEFFICIENT, 1.0f - (1.0f - complete) * (1.0f - complete) ));
    float invBlend(1.0f - blend);

    mRelativePosition.x = activate ? mRelativePosition.x * invBlend + targetRelativePositionX * blend : targetRelativePositionX;
    mRelativePosition.x = Chase( mRelativePosition.x, targetRelativePositionX, 1.0f );

    relativePosition.x = (actorPosition.x - mRelativePosition.x) / scrollSize.width;

    float difference = fabsf(ShortestDistanceInDomain(mScrollPosition.x, scrollPosition.x, -max.x, -min.x));
    mAverageSpeed = activate ? mAverageSpeed * invBlend + difference * blend : 0.0f;

    actorPosition.x += relativePosition.x * mAverageSpeed;

    return actorPosition - pagePosition;
  }

private:

  ScrollSlideInfoPtr mScrollSlideInfo;
  Vector3 mScrollPosition;              ///< The current scroll position
  float mDelayMin;                      ///< Minimum delay rate (at closest position to touch)
  float mDelayMax;                      ///< Maximum delay rate (at furthest position from touch - 1 page away)
  Vector2 mRelativePosition;
  float mAverageSpeed;                  ///< The Average speed of the Actor (proportional to mScrollPosition - scrollPosition)

};

/**
 * ScrollSlideScaleConstraint
 *
 */
struct ScrollSlideScaleConstraint
{
  /**
   * Constraint constructor
   */
  ScrollSlideScaleConstraint()
  {
  }

  /**
   * @param[in] current The current position
   * @param[in] scrollPositionProperty The scroll-view's position property (SCROLL_POSITION_PROPERTY_NAME)
   * @param[in] effectReferenceProperty The point in the scroll-view where the user touched the screen
   * @param[in] effectTimeProperty The current timer. Starting from 0.0 when scroll animation/dragging
   * commences. Ensures that constraint is applied and processed every frame (to achieve the delay effect)
   * @param[in] sizeProperty The size of the ScrollView.
   * @return The new position of this Actor.
   */
  Vector3 operator()(const Vector3& current,
                     const PropertyInput& snapProperty)
  {
    float scale = 1.0f + snapProperty.GetFloat() * 0.008f;
    return Vector3( current.x * scale, current.y * scale, current.z);
  }

};

/**
 * Applies the slide constraints to the child actor for overshoot effect.
 *
 * @param[in] scrollView The ScrollView containing the pages.
 * @param[in] child The child to be affected with the slide effect.
 * @param[in] angleSwing The maximum amount the child actor should
 * rotate in radians for each axis (X and Y) as the page is scrolled.
 * move for each axis (X and Y) as the page is scrolled.
 */
void ApplyScrollSlideConstraints(ScrollSlideInfoPtr scrollSlideInfo,
                                Toolkit::ScrollView scrollView,
                                Actor child,
                                float delayMin,
                                float delayMax)
{
  // Apply constraints to these actors //
  Constraint constraint = Constraint::New<Vector3>( Actor::POSITION,
                                         ParentSource(Actor::POSITION),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollViewSlideEffect::EFFECT_TIME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_DELTA_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollViewSlideEffect::EFFECT_ACTIVE ) ),
                                         ScrollSlidePositionConstraint(scrollSlideInfo, delayMin, delayMax) );
  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector3>( Actor::SCALE,
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollViewSlideEffect::EFFECT_ACTIVE ) ),
                                         ScrollSlideScaleConstraint() );
  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );
}

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

ScrollViewSlideEffect::ScrollViewSlideEffect()
: mScrollSlideInfo(new ScrollSlideInfo()),
  mPropertyTime(Property::INVALID_INDEX),
  mPropertyReference(Property::INVALID_INDEX),
  mPropertyActive(Property::INVALID_INDEX),
  mDelayReferenceOffset(Vector3::ZERO),
  mMaxDelayDuration(DEFAULT_MAX_DELAY_DURATION)
{
}

ScrollViewSlideEffect::~ScrollViewSlideEffect()
{
}

bool ScrollViewSlideEffect::GetSlideDirection() const
{
  return mScrollSlideInfo->mVertical;
}

void ScrollViewSlideEffect::SetSlideDirection(bool vertical)
{
  mScrollSlideInfo->mVertical = vertical;
}

const Vector3& ScrollViewSlideEffect::GetDelayReferenceOffset() const
{
  return mDelayReferenceOffset;
}

void ScrollViewSlideEffect::SetDelayReferenceOffset(const Vector3& offset)
{
  mDelayReferenceOffset = offset;
}

float ScrollViewSlideEffect::GetMaxDelayDuration() const
{
  return mMaxDelayDuration;
}

void ScrollViewSlideEffect::SetMaxDelayDuration(float duration)
{
  mMaxDelayDuration = duration;
}

void ScrollViewSlideEffect::ApplyToActor(Actor child,
                                         float delayMin,
                                         float delayMax)
{
  ApplyScrollSlideConstraints( mScrollSlideInfo,
                               GetScrollView(),
                               child,
                               delayMin,
                               delayMax );
}

void ScrollViewSlideEffect::OnAttach(Toolkit::ScrollView& scrollView)
{
  mScrollSlideInfo->mScrollPosition = scrollView.GetProperty<Vector3>( scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_PROPERTY_NAME ) );
  mScrollSlideInfo->mScrollSize = scrollView.GetProperty<Vector3>( Actor::SIZE );
  mScrollSlideInfo->mScrollPositionMin = scrollView.GetProperty<Vector3>( scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MIN_PROPERTY_NAME ) );
  mScrollSlideInfo->mScrollPositionMax = scrollView.GetProperty<Vector3>( scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MAX_PROPERTY_NAME ) );
  mScrollSlideInfo->mScrollWrap = scrollView.GetProperty<bool>( scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_WRAP_PROPERTY_NAME ) );
  mScrollSlideInfo->mVertical = false;

  // Create effect-time property if not already created.
  if(mPropertyTime == Property::INVALID_INDEX)
  {
    mPropertyTime = SafeRegisterProperty( scrollView, Toolkit::ScrollViewSlideEffect::EFFECT_TIME, 0.0f );
    mPropertyReference = SafeRegisterProperty( scrollView, Toolkit::ScrollViewSlideEffect::EFFECT_REFERENCE, Vector3::ZERO );
    mPropertyActive = SafeRegisterProperty( scrollView, Toolkit::ScrollViewSlideEffect::EFFECT_ACTIVE, 0.0f );
  }

  // Create constraint to update ScrollSlideInfo
  // Doesn't matter what this is applied to and on what property.
  // Just needs to update mScrollSlideInfo values as properties change.
  // The minor constraints (applied to the Actors) can use this mScrollSlideInfo.
  Constraint constraint = Constraint::New<float>( mPropertyTime,
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollViewSlideEffect::EFFECT_REFERENCE ) ),
                                         Source(scrollView, Actor::SIZE),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_WRAP_PROPERTY_NAME ) ),
                                         ScrollSlideInfoUpdate(mScrollSlideInfo) );
  constraint.SetRemoveAction( Constraint::Discard );
  mInfoUpdateConstraint = scrollView.ApplyConstraint( constraint );

  // Connect to the scroll view signals
  scrollView.ScrollStartedSignal().Connect(this, &ScrollViewSlideEffect::OnScrollStart);
  scrollView.SnapStartedSignal().Connect(this, &ScrollViewSlideEffect::OnScrollSnapStarted);
  scrollView.TouchedSignal().Connect(this, &ScrollViewSlideEffect::OnScrollTouched);

  AttachActor(scrollView);
}

bool ScrollViewSlideEffect::OnScrollTouched(Actor actor, const TouchEvent& event)
{
  // Ignore events with multiple-touch points
  if (event.GetPointCount() != 1)
  {
    return false;
  }

  if (event.GetPoint(0).state == TouchPoint::Down)
  {
    const TouchPoint& point = event.GetPoint(0);
    Vector3 touchPosition(point.local - Stage::GetCurrent().GetSize() * 0.5f);

    Vector3 scrollPosition = GetScrollView().GetCurrentScrollPosition();
    GetScrollView().SetProperty(mPropertyReference, scrollPosition + touchPosition + mDelayReferenceOffset);
  }

  return false;
}

void ScrollViewSlideEffect::OnDetach(Toolkit::ScrollView& scrollView)
{
  scrollView.ScrollStartedSignal().Disconnect(this, &ScrollViewSlideEffect::OnScrollStart);
  scrollView.SnapStartedSignal().Disconnect(this, &ScrollViewSlideEffect::OnScrollSnapStarted);
  scrollView.TouchedSignal().Disconnect(this, &ScrollViewSlideEffect::OnScrollTouched);
  scrollView.RemoveConstraint( mInfoUpdateConstraint );

  if(mAnimation)
  {
    mAnimation.FinishedSignal().Disconnect(this, &ScrollViewSlideEffect::OnAnimationFinished);
    mAnimation.Clear();
    mAnimation.Reset();
  }

  if(mAnimationSnap)
  {
    mAnimationSnap.FinishedSignal().Disconnect(this, &ScrollViewSlideEffect::OnAnimationSnapFinished);
    mAnimationSnap.Clear();
    mAnimationSnap.Reset();
  }
}

void ScrollViewSlideEffect::AttachActor(Actor actor)
{

}

void ScrollViewSlideEffect::DetachActor(Actor actor)
{
  // TODO: remove the specific constraint defined in AttachActor (and possibly
  // unregister property) - neither functionality exists in Dali.
}

void ScrollViewSlideEffect::ContinueAnimation(float endTime)
{
  // continue animating
  if(mAnimation)
  {
    mAnimation.FinishedSignal().Disconnect(this, &ScrollViewSlideEffect::OnAnimationFinished);
    mAnimation.Clear();
  }

  Actor scrollView = GetScrollView();

  mAnimation = Animation::New(SLIDEEFFECT_ANIMATION_MAX_TIME);
  mAnimation.AnimateTo( Property(scrollView, mPropertyTime), endTime, AlphaFunctions::Linear );
  mAnimation.FinishedSignal().Connect(this, &ScrollViewSlideEffect::OnAnimationFinished);
  mAnimation.Play();
}

void ScrollViewSlideEffect::OnScrollStart( const Vector3& position )
{
  Actor scrollView = GetScrollView();
  GetScrollView().SetProperty(mPropertyTime, 0.0f);

  ContinueAnimation(SLIDEEFFECT_ANIMATION_MAX_TIME);

  if(mAnimationSnap)
  {
    mAnimationSnap.FinishedSignal().Disconnect(this, &ScrollViewSlideEffect::OnAnimationSnapFinished);
    mAnimationSnap.Clear();
  }

  mAnimationSnap = Animation::New( EFFECT_SNAP_GROW_DURATION );
  mAnimationSnap.AnimateTo( Property(scrollView, mPropertyActive), 1.0f, AlphaFunctions::Linear );
  mAnimationSnap.FinishedSignal().Connect(this, &ScrollViewSlideEffect::OnAnimationSnapFinished);
  mAnimationSnap.Play();
}

void ScrollViewSlideEffect::OnScrollSnapStarted(const Toolkit::ScrollView::SnapEvent& event)
{
  if(mAnimationSnap)
  {
    mAnimationSnap.Clear();
  }

  Actor scrollView = GetScrollView();
  mAnimationSnap = Animation::New(EFFECT_SNAP_DECAY_DURATION );
  mAnimationSnap.AnimateTo( Property(scrollView, mPropertyActive), 0.0f, AlphaFunctions::Linear );
  mAnimationSnap.FinishedSignal().Connect(this, &ScrollViewSlideEffect::OnAnimationSnapFinished);
  mAnimationSnap.Play();
}

void ScrollViewSlideEffect::OnAnimationSnapFinished( Animation& animation )
{
  mAnimationSnap.FinishedSignal().Disconnect(this, &ScrollViewSlideEffect::OnAnimationSnapFinished);
  mAnimationSnap.Clear();

  // stop time animation
  if(mAnimation)
  {
    mAnimation.FinishedSignal().Disconnect(this, &ScrollViewSlideEffect::OnAnimationFinished);
    mAnimation.Clear();
  }
}

void ScrollViewSlideEffect::OnAnimationFinished( Animation& animation )
{
  // still unstable, so continue animating.
  // TODO: Requires an instability check to ensure time animation finishes when delay is
  // less noticeable. i.e. all present scrollPositions are approx the same as mScrollPosition in constraints.
  // best solution for this is to switch to a single history vector of scroll position, and compare if
  // position has not deviated >= 0.5 pixel for the past 1 second.
  float endTime = GetScrollView().GetProperty<float>(mPropertyTime) + SLIDEEFFECT_ANIMATION_MAX_TIME;
  ContinueAnimation(endTime);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
