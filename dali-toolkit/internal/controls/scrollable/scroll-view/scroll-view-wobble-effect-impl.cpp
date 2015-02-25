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
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-effect-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-wobble-effect-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-impl.h>

using namespace Dali;

namespace // unnamed namespace
{

const float WOBBLEEFFECT_FRICTION_COEFFICIENT = 0.8f;     ///< (deacc) - velocity multiplier per unit time (80%)
const float WOBBLEEFFECT_IMPULSE_DISTANCE_FACTOR = 0.1f;  ///< (acc) - move by 10% of distance-delta per unit time
const float WOBBLEEFFECT_TIME_FACTOR = 30.0f;             ///< (T) - 30 times faster (unit time = 1/30th sec)
const float WOBBLEEFFECT_ANIMATION_MAX_TIME = 60.0f;      ///< Animation time (every time finishes, checks if it needs to go again)
const float WOBBLEEFFECT_STABLE_TIME_THRESHOLD = 0.5f;    ///< Must be stable for more than half a second to stop animating.
const float STABILITY_DELTA_THRESHOLD = Math::MACHINE_EPSILON_10000;  ///< When velocity delta is greater than this threshold it is considered in motion.

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

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * ScrollView WobbleEffect constraint
 * This constraint has a chase position and velocity, that chases
 * a target position (scroll-position + scroll-offset). As it has a
 * velocity. It will eventually pass it's target position, and chase back
 * in the opposite direction. As it has a friction coefficient, it will
 * gradually slow, and reach it's target position (stabilized).
 */
struct ScrollViewWobbleEffectConstraint
{
  /**
   * @param[in,out] wobbleEffect Reference to wobbleEffect instance
   */
  ScrollViewWobbleEffectConstraint(ScrollViewWobbleEffect& wobbleEffect)
  : mChase(Vector3::ZERO),
    mVelocity(Vector3::ZERO),
    mTime(0.0f),
    mStabilityTimeCounter(0),
    mStabilized(true),
    mWobbleEffect(wobbleEffect),
    mAnimationCycleId(0)
  {

  }

  /**
   * @param[out] current The new wobble value
   * @param[in] propertyTime The current time since the wobble effect started
   * @param[in] propertyPosition The scroll-position
   * @param[in] propertyOffset The scroll-overshoot
   */
  Vector3 operator()(const Vector3& current,
                    const PropertyInput& propertyTime,
                    const PropertyInput& propertyPosition,
                    const PropertyInput& propertyOffsetX,
                    const PropertyInput& propertyOffsetY)
  {
    Vector3 dir;

    if(mStabilized)
    {
      // check if animation cycle id has changed (if so then this spells
      // the start of a new animation)
      if(mAnimationCycleId != mWobbleEffect.GetAnimationCycleId())
      {
        mStabilized = false;
      }
    }
    else
    {
      // not stable (i.e. wobbling)
      Vector3 offset(propertyOffsetX.GetFloat(), propertyOffsetY.GetFloat(), 0.0f);
      const Vector3& position = propertyPosition.GetVector3() - offset;
      const float time = propertyTime.GetFloat();
      const float timePassed = time - mTime;

      mTime = time;

      if(timePassed>0)
      {
        const Vector3 delta = position - mChase;

        // Check to see if wobble has stabilized.
        if( (fabsf(delta.x) < STABILITY_DELTA_THRESHOLD) &&
            (fabsf(mVelocity.x) < STABILITY_DELTA_THRESHOLD) )
        {
          mStabilityTimeCounter+=timePassed;

          if(mStabilityTimeCounter > WOBBLEEFFECT_STABLE_TIME_THRESHOLD)
          {
            mStabilityTimeCounter = 0.0f;
            mStabilized = true;
            mWobbleEffect.IncrementStableCount();
            mAnimationCycleId = mWobbleEffect.GetAnimationCycleId();
          }
        }
        else
        {
          mStabilityTimeCounter = 0.0f;
        }

        if(!mStabilized)
        {
          float t = timePassed * WOBBLEEFFECT_TIME_FACTOR;

          mVelocity *= pow( WOBBLEEFFECT_FRICTION_COEFFICIENT, t );
          mVelocity += delta * 0.1f * t;
          mChase += mVelocity;
        }
        else
        {
          // stabilized, so chase should be exactly on position.
          mChase = position;
        }
      }

      dir.x = propertyPosition.GetVector3().x - mChase.x;
      dir.y = propertyPosition.GetVector3().y - mChase.y;
    } // end else

    return dir;
  }

  Vector3 mChase;                                 ///< Chaser position
  Vector3 mVelocity;                              ///< Velocity of Chaser
  float mTime;                                    ///< Current time.
  float mStabilityTimeCounter;                    ///< Time in seconds that stable for.
  bool mStabilized;                               ///< Stabilized flag.
  ScrollViewWobbleEffect& mWobbleEffect;          ///< Reference to Wobble Effect
  unsigned int mAnimationCycleId;                 ///< Animation Cycle Id
};

ScrollViewWobbleEffect::ScrollViewWobbleEffect()
: mPropertyTime(Property::INVALID_INDEX),
  mStableCurrent(0),
  mAnimationCycleId(0)
{
}

ScrollViewWobbleEffect::~ScrollViewWobbleEffect()
{
}

void ScrollViewWobbleEffect::IncrementStableCount()
{
  mStableCurrent++;
}

unsigned int ScrollViewWobbleEffect::GetAnimationCycleId() const
{
  return mAnimationCycleId;
}

void ScrollViewWobbleEffect::OnAttach(Toolkit::ScrollView& scrollView)
{
  mStableCurrent = 0;
  mAnimationCycleId = 0;

  // Create effect-time property if not already created.
  if(mPropertyTime == Property::INVALID_INDEX)
  {
    mPropertyTime = SafeRegisterProperty( scrollView, Toolkit::ScrollViewWobbleEffect::EFFECT_TIME, 0.0f );
  }

  // Connect to the scroll view signals
  scrollView.ScrollStartedSignal().Connect(this, &ScrollViewWobbleEffect::OnScrollStart);
  scrollView.ScrollUpdatedSignal().Connect(this, &ScrollViewWobbleEffect::OnScrollUpdate);
  scrollView.ScrollCompletedSignal().Connect(this, &ScrollViewWobbleEffect::OnScrollComplete);

  AttachActor(scrollView);
}

void ScrollViewWobbleEffect::OnDetach(Toolkit::ScrollView& scrollView)
{
  scrollView.ScrollStartedSignal().Disconnect(this, &ScrollViewWobbleEffect::OnScrollStart);
  scrollView.ScrollUpdatedSignal().Disconnect(this, &ScrollViewWobbleEffect::OnScrollUpdate);
  scrollView.ScrollCompletedSignal().Disconnect(this, &ScrollViewWobbleEffect::OnScrollComplete);

  if(mAnimation)
  {
    mAnimation.FinishedSignal().Disconnect(this, &ScrollViewWobbleEffect::OnAnimationFinished);
    mAnimation.Clear();
    mAnimation.Reset();
  }
}

void ScrollViewWobbleEffect::AttachActor(Actor actor)
{
  Property::Index propertyPosition = actor.GetPropertyIndex(Toolkit::ScrollView::SCROLL_POSITION_PROPERTY_NAME);
  Property::Index propertyOvershootX = actor.GetPropertyIndex(Toolkit::ScrollView::SCROLL_OVERSHOOT_X_PROPERTY_NAME);
  Property::Index propertyOvershootY = actor.GetPropertyIndex(Toolkit::ScrollView::SCROLL_OVERSHOOT_Y_PROPERTY_NAME);

  // Create effect-overshoot property if not already created.
  Property::Index propertyEffectOvershoot = actor.GetPropertyIndex(Toolkit::ScrollViewWobbleEffect::EFFECT_OVERSHOOT);
  if(propertyEffectOvershoot == Property::INVALID_INDEX)
  {
    propertyEffectOvershoot = actor.RegisterProperty(Toolkit::ScrollViewWobbleEffect::EFFECT_OVERSHOOT, Vector3::ZERO);
  }

  Actor scrollView = GetScrollView();

  Constraint constraint = Constraint::New<Vector3>( propertyEffectOvershoot,
                                                    Source(scrollView, mPropertyTime),
                                                    Source(actor, propertyPosition),
                                                    Source(actor, propertyOvershootX),
                                                    Source(actor, propertyOvershootY),
                                                    ScrollViewWobbleEffectConstraint(*this) );
  actor.ApplyConstraint(constraint);
}

void ScrollViewWobbleEffect::DetachActor(Actor actor)
{
  // TODO: remove the specific constraint defined in AttachActor (and possibly
  // unregister property) - neither functionality exists in Dali.
}

void ScrollViewWobbleEffect::ContinueAnimation(float endTime)
{

  // continue animating
  if(mAnimation)
  {
    mAnimation.FinishedSignal().Disconnect(this, &ScrollViewWobbleEffect::OnAnimationFinished);
    mAnimation.Clear();
  }

  Actor scrollView = GetScrollView();

  mAnimation = Animation::New(WOBBLEEFFECT_ANIMATION_MAX_TIME);
  mAnimation.AnimateTo( Property(scrollView, mPropertyTime), endTime, AlphaFunctions::Linear );
  mAnimation.FinishedSignal().Connect(this, &ScrollViewWobbleEffect::OnAnimationFinished);
  mAnimation.Play();

}

void ScrollViewWobbleEffect::OnScrollStart( const Vector3& position )
{
  // When animation starts, all constraints all unstable,
  // and we change the animation cycle id.
  mStableCurrent = 0;
  mAnimationCycleId++;

  GetScrollView().SetProperty(mPropertyTime, 0.0f);

  ContinueAnimation(WOBBLEEFFECT_ANIMATION_MAX_TIME);
}

void ScrollViewWobbleEffect::OnScrollUpdate( const Vector3& position )
{
  // nothing to do
}

void ScrollViewWobbleEffect::OnScrollComplete( const Vector3& position )
{
  // nothing to do
}

void ScrollViewWobbleEffect::OnAnimationFinished( Animation& animation )
{
  if(mStableCurrent!=1)
  {
    // still unstable, so continue animating.
    float endTime = GetScrollView().GetProperty<float>(mPropertyTime) + WOBBLEEFFECT_ANIMATION_MAX_TIME;
    ContinueAnimation(endTime);
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
