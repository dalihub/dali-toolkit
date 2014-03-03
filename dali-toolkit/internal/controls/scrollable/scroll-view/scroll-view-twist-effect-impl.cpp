
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

#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-effect-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-twist-effect-impl.h>

using namespace Dali;

namespace // unnamed namespace
{

const float TWISTEFFECT_ANIMATION_MAX_TIME = 60.0f;         ///< Animation time (every time finishes, checks if it needs to go again)
const float TWISTEFFECT_DEFAULT_DROPOFF = 0.7f;             ///< Default drop off amount
const float TWISTEFFECT_DEFAULT_DROPOFF_DISTANCE_X = 720.0f;  ///< Default drop off distance
const float TWISTEFFECT_DEFAULT_DROPOFF_DISTANCE_Y = 1280.0f;  ///< Default drop off distance

// Hop Easing equation.
// Starts with a -ve cosine ranging from 0 to pi.
// Then plateaus.
// Then finishes with a -ve cosine ranging from pi to 0
// 0......(RISE).....PI (SUSTAIN) PI.....(FALL)......0
//                xxxxxxxxxxxxxxxxxxxxx
//              x                       x
//            x                           x
//           x                             x
//           x                             x
//          x                               x
//        x                                   x
// xxxxxx                                       xxxxxx

const float HOP_RISE(0.25f);
const float HOP_FALL(0.5f);
const float DELAY(0.5f);

float HopEasing(float progress)
{
  // progress from 0.0 - HOP_RISE (go from 0.0 to 1.0)
  if(progress < HOP_RISE)
  {
    return 0.5f - cosf(progress/HOP_RISE * Math::PI) * 0.5f;
  }

  progress += HOP_FALL - 1.0f;

  // progress from 0.0 - HOP_FALL (go from 1.0 to 0.0)
  if(progress > 0.0f )
  {
    return 0.5f + cosf(progress/HOP_FALL * Math::PI) * 0.5f;
  }

  // progress at plateau.
  return 1.0f;
}

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
 * @param[in] x input X value
 * @param[in] x0 input minimum bound
 * @param[in] x1 input maximum bound
 * @param[in] y0 output minimum bound
 * @param[in] y1 output maximum bound
 * @return The result of the mapping is returned.
 */
float Rescale(float x, float x0, float x1, float y0, float y1)
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
 * ScrollTwistRotationConstraint
 *
 * Rotate constraint adjusts the angle of the Actors
 * based on actor's world-position relative to the middle of the screen.
 * When at middle of screen Angles on X and Y Axes is 0.
 * When one screen away from the middle Angle is 90 degrees (pi/2)
 */
struct ScrollDropoffTwistRotationConstraint
{
  /**
   * Constraint constructor
   * @param[in] angleSwing The amount the Actor should revolve in radians
   * for a given page worth of distance.
   */
  ScrollDropoffTwistRotationConstraint(const Vector2& angleSwing, const Vector2& dropOff, const Vector2& distance, AlphaFunction function)
  : mAngleSwing(angleSwing),
    mDropOff(dropOff),
    mDropOffDistance(distance),
    mDropOffFunction(function)
  {
  }

  /**
   * @param[in] current The current orientation of this Actor
   * @param[in] actorPositionProperty The actor's world-position property
   * @param[in] scrollOvershootXProperty The scroll-view's overshoot property (SCROLL_OVERSHOOT_X_PROPERTY_NAME)
   * @param[in] scrollOvershootYProperty The scroll-view's overshoot property (SCROLL_OVERSHOOT_Y_PROPERTY_NAME)
   * @param[in] pageSizeProperty The size of the page. (scrollView SIZE)
   * @return The new orientation of this Actor.
   */
  Quaternion operator()(const Quaternion& current,
                        const PropertyInput& actorPositionProperty,
                        const PropertyInput& scrollablePositionProperty,
                        const PropertyInput& scrollOvershootXProperty,
                        const PropertyInput& scrollOvershootYProperty,
                        const PropertyInput& pageSizeProperty,
                        const PropertyInput& activateProperty)
  {
    const Vector3& position = actorPositionProperty.GetVector3();
    const Vector3& parentPosition = scrollablePositionProperty.GetVector3();
    const Vector3& pageSize = pageSizeProperty.GetVector3();
    const Vector2 overshoot(scrollOvershootXProperty.GetFloat(), scrollOvershootYProperty.GetFloat());

    if(fabsf(overshoot.x) < Math::MACHINE_EPSILON_0 && fabsf(overshoot.y) < Math::MACHINE_EPSILON_0)
    {
      return current;
    }

    const float& activate = activateProperty.GetFloat();

    if(activate < Math::MACHINE_EPSILON_0)
    {
      return current;
    }

    // get distance from centre of scrollable container
    Vector2 distance = position.GetVectorXY() - parentPosition.GetVectorXY();

    if( overshoot.x > 0.0f )
    {
      distance.x += pageSize.x * 0.5f;
    }
    else
    {
      distance.x -= pageSize.x * 0.5f;
    }
    distance.x = Clamp(fabsf(distance.x), 0.0f, mDropOffDistance.x);

    if( overshoot.y > 0.0f )
    {
      distance.y += pageSize.y * 0.5f;
    }
    else
    {
      distance.y -= pageSize.y * 0.5f;
    }
    distance.y = Clamp(fabsf(distance.y), 0.0f, mDropOffDistance.y);

    Vector2 angleMod = distance / mDropOffDistance;
    if(mDropOffFunction)
    {
      angleMod.x = mDropOffFunction(angleMod.x);
      angleMod.y = mDropOffFunction(angleMod.y);
    }
    angleMod = Vector2::ONE - (angleMod * mDropOff);

    Vector2 angle = angleMod * mAngleSwing * overshoot;

    Quaternion rotation = Quaternion(angle.x, Vector3::YAXIS) *
                          Quaternion(-angle.y, Vector3::XAXIS) *
                          current;

    return rotation;
  }

  const Vector2 mAngleSwing;                                    ///< Maximum amount in X and Y axes to rotate.
  const Vector2 mDropOff;
  const Vector2 mDropOffDistance;
  AlphaFunction mDropOffFunction;
};

/**
 * ScrollTwistRotationConstraint
 *
 * Rotate constraint adjusts the angle of the Actors
 * based on actor's world-position relative to the middle of the screen.
 * When at middle of screen Angles on X and Y Axes is 0.
 * When one screen away from the middle Angle is 90 degrees (pi/2)
 */
struct ScrollTwistRotationConstraint
{
  /**
   * Constraint constructor
   * @param[in] angleSwing The amount the Actor should revolve in radians
   * for a given page worth of distance.
   */
  ScrollTwistRotationConstraint(const Vector2& angleSwing)
  : mAngleSwing(angleSwing)
  {
  }

  /**
   * @param[in] current The current orientation of this Actor
   * @param[in] actorPositionProperty The actor's world-position property
   * @param[in] scrollOvershootXProperty The scroll-view's overshoot property (SCROLL_OVERSHOOT_X_PROPERTY_NAME)
   * @param[in] scrollOvershootYProperty The scroll-view's overshoot property (SCROLL_OVERSHOOT_Y_PROPERTY_NAME)
   * @param[in] pageSizeProperty The size of the page. (scrollView SIZE)
   * @return The new orientation of this Actor.
   */
  Quaternion operator()(const Quaternion& current,
                        const PropertyInput& scrollOvershootXProperty,
                        const PropertyInput& scrollOvershootYProperty,
                        const PropertyInput& activateProperty)
  {
    const Vector2 overshoot(scrollOvershootXProperty.GetFloat(), scrollOvershootYProperty.GetFloat());

    if(fabsf(overshoot.x) < Math::MACHINE_EPSILON_0 && fabsf(overshoot.y) < Math::MACHINE_EPSILON_0)
    {
      return current;
    }

    const float& activate = activateProperty.GetFloat();

    if(activate < Math::MACHINE_EPSILON_0)
    {
      return current;
    }

    Quaternion rotation = Quaternion(overshoot.x * mAngleSwing.x, Vector3::YAXIS) *
                          Quaternion(-overshoot.y * mAngleSwing.y, Vector3::XAXIS) *
                          current;

    return rotation;
  }

  const Vector2 mAngleSwing;
};

/**
 * ScrollTwistPositionConstraint
 *
 * Position constraint adjusts the position of the Actors
 * based on their parent page's position relative to the middle of the screen.
 * When at middle of the screen the position is not altered.
 * When one screen away from middle the position is rotated about it's origin + mAnchor
 */
struct ScrollTwistPositionConstraint
{
  /**
   * Constraint constructor
   */
  ScrollTwistPositionConstraint(float delayMin, float delayMax)
  : mDelayMin(delayMin),
    mDelayMax(delayMax),
    mCurrentDelayFactor(0.0f)
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
                     const PropertyInput& scrollPositionProperty,
                     const PropertyInput& effectReferenceProperty,
                     const PropertyInput& effectTimeProperty,
                     const PropertyInput& sizeProperty,
                     const PropertyInput& activateProperty)
  {
    const Vector3& scrollPosition = scrollPositionProperty.GetVector3();
    const float& activate = activateProperty.GetFloat();

    if(activate < Math::MACHINE_EPSILON_0)
    {
      mScrollPosition = scrollPosition;
      return current + mScrollPosition;
    }
    const Vector3& pagePosition = pagePositionProperty.GetVector3();
    const Vector3& referencePoint = effectReferenceProperty.GetVector3();
    // Determine the relative position of the actor from the scrolling reference point.
    // (the further away from the refernce, the longer the delay should be)
    Vector3 relativePosition = pagePosition + current - referencePoint;
    float f = relativePosition.x;

    // f represents this absolute distance. Get as a relative distance and inverse exponential
    // (as delay equation is has an exponential effect i.e. the closer delayFactor to 1.0f,
    // the longer the delay would appear exponentially)
    f = fabsf(f / sizeProperty.GetVector3().width);
    f = std::min(f, 1.0f);
    f = 1.0f - (1.0f - f) * (1.0f - f);
    // at center delay factor is mDelayMin, at maximum (1.0) it is mDelayMax
    f = Rescale(f, 0.0f, 1.0f, mDelayMin, mDelayMax);

    // Will take 0.25s for current delay factor to equal target delay factor
    // This prevents users quickly dragging from different points and noticing a jerk.
    mCurrentDelayFactor = Chase( mCurrentDelayFactor, f, 4.0f/60.0f );
    float delay = activate * mCurrentDelayFactor;
    mScrollPosition = mScrollPosition * delay + scrollPosition * (1.0f-delay);

    return current + mScrollPosition;
  }

private:

  Vector3 mScrollPosition;              ///< The current scroll position
  float mDelayMin;
  float mDelayMax;
  float mCurrentDelayFactor;

};

/**
 * ScrollTwistScaleConstraint
 *
 * Scale constraint adjusts the scale of the Actors
 * based on a supplied depth property value.
 */
struct ScrollTwistScaleConstraint
{
  /**
   * Constraint constructor
   */
  ScrollTwistScaleConstraint(float scaleAmount)
  : mScaleAmount(scaleAmount)
  {
  }

  /**
   * @param[in] current The current position
   * @param[in] scrollPositionProperty The scroll-view's position property (SCROLL_POSITION_PROPERTY_NAME)
   * @return The new position of this Actor.
   */
  Vector3 operator()(const Vector3& current,
                     const PropertyInput& depthProperty)
  {
    float depth = depthProperty.GetFloat();

    return current * (1.0f - depth * mScaleAmount); // contract by mScaleAmount of original size.
  }

private:

  float mScaleAmount;
};

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

ScrollViewTwistEffect::ScrollViewTwistEffect()
: mFlags(DefaultFlags),
  mPropertyTime(Property::INVALID_INDEX),
  mEnableEffect(true),
  mAdditionalEffects(false),
  mPropertyReference(Property::INVALID_INDEX),
  mPropertyActivate(Property::INVALID_INDEX),
  mMinimumDistanceForShrink(Toolkit::ScrollViewTwistEffect::DEFAULT_MINIMUM_DISTANCE_FOR_SHRINK),
  mMaxSwingAngle(Math::PI_2, Math::PI_2),
  mDropOff(TWISTEFFECT_DEFAULT_DROPOFF, TWISTEFFECT_DEFAULT_DROPOFF),
  mDropOffDistance(TWISTEFFECT_DEFAULT_DROPOFF_DISTANCE_X, TWISTEFFECT_DEFAULT_DROPOFF_DISTANCE_Y),
  mDropOffFunction(NULL)
{
}

ScrollViewTwistEffect::~ScrollViewTwistEffect()
{
}


float ScrollViewTwistEffect::GetMinimumDistanceForShrink() const
{
  return mMinimumDistanceForShrink;
}

void ScrollViewTwistEffect::SetMinimumDistanceForShrink(float distance)
{
  mMinimumDistanceForShrink = distance;
}

void ScrollViewTwistEffect::EnableEffect(bool enableFlag)
{
  mEnableEffect = enableFlag;
}

void ScrollViewTwistEffect::ApplyToActor(Actor child,
                                         bool additionalEffects,
                                         const Vector2& angleSwing,
                                         float scaleAmount,
                                         float delayMin,
                                         float delayMax)
{
  mMaxSwingAngle = angleSwing;
  mAdditionalEffects = additionalEffects;
  mScaleFactor = scaleAmount;
  mDelayMin = delayMin;
  mDelayMax = delayMax;
  if(mFlags & FlagDefaultDropOff)
  {
    Vector3 size = GetScrollView().GetCurrentSize();
    // size may still be 0 if the effect is applied before scroll view hits the stage
    if(size.x > Math::MACHINE_EPSILON_1)
    {
      mDropOffDistance.x = size.x;
    }
    if(size.y > Math::MACHINE_EPSILON_1)
    {
      mDropOffDistance.y = size.y;
    }
  }
  if(scaleAmount > Math::MACHINE_EPSILON_0)
  {
    mFlags |= FlagScale;
  }
  else
  {
    mFlags = mFlags & ~FlagScale;
  }
  if(mMaxSwingAngle.LengthSquared() > Math::MACHINE_EPSILON_0)
  {
    mFlags |= FlagTwist;
  }
  else
  {
    mFlags = mFlags & ~FlagTwist;
  }
  Apply(child);
}

void ScrollViewTwistEffect::Apply(Actor child)
{
  // Apply constraints to these actors //
  Constraint constraint;

  Toolkit::ScrollView scrollView = GetScrollView();

  if( mFlags & FlagScale )
  {
    constraint = Constraint::New<Vector3>( Actor::SCALE,
                                           Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollViewTwistEffect::EFFECT_DEPTH ) ),
                                           ScrollTwistScaleConstraint(mScaleFactor) );
    constraint.SetRemoveAction( Constraint::Discard );
    child.ApplyConstraint( constraint );
  }

  constraint = Constraint::New<Vector3>( Actor::POSITION,
                                          ParentSource(Actor::POSITION),
                                          Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_PROPERTY_NAME ) ),
                                          Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollViewTwistEffect::EFFECT_REFERENCE ) ),
                                          Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollViewTwistEffect::EFFECT_TIME ) ),
                                          Source(scrollView, Actor::SIZE),
                                          Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollViewTwistEffect::EFFECT_ACTIVATE) ),
                                         ScrollTwistPositionConstraint(mDelayMin, mDelayMax) );
  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );

  // use actor position to affect rotation
  if(mFlags & FlagTwist)
  {
    if(mFlags & FlagDropOff)
    {
      constraint = Constraint::New<Quaternion>( Actor::ROTATION,
                                                LocalSource(Actor::WORLD_POSITION),
                                                Source(scrollView, Actor::WORLD_POSITION ),
                                                Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_OVERSHOOT_X_PROPERTY_NAME ) ),
                                                Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_OVERSHOOT_Y_PROPERTY_NAME ) ),
                                                Source(scrollView, Actor::SIZE ),
                                                Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollViewTwistEffect::EFFECT_ACTIVATE) ),
                                                ScrollDropoffTwistRotationConstraint(mMaxSwingAngle, mDropOff, mDropOffDistance, mDropOffFunction) );
    }
    else
    {
      constraint = Constraint::New<Quaternion>( Actor::ROTATION,
                                                Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_OVERSHOOT_X_PROPERTY_NAME ) ),
                                                Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_OVERSHOOT_Y_PROPERTY_NAME ) ),
                                                Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollViewTwistEffect::EFFECT_ACTIVATE) ),
                                                ScrollTwistRotationConstraint(mMaxSwingAngle) );
    }
    constraint.SetRemoveAction( Constraint::Discard );
    child.ApplyConstraint( constraint );
  }
}

void ScrollViewTwistEffect::SetSwingDropOff(const Vector2& dropOff, const Vector2& distance, AlphaFunction function)
{
  if( mDropOffDistance.LengthSquared() > Math::MACHINE_EPSILON_1 && mDropOff.LengthSquared() > Math::MACHINE_EPSILON_1 )
  {
    mFlags |= FlagDropOff;
    mDropOff = dropOff;
    mDropOffDistance = distance;
    mDropOffFunction = function;
  }
  else
  {
    mFlags = mFlags & ~FlagDropOff;
  }
  // can no longer use default dop off
  mFlags = mFlags & ~FlagDefaultDropOff;
}

void ScrollViewTwistEffect::OnAttach(Toolkit::ScrollView& scrollView)
{
  // Create effect-time property if not already created.
  if(mPropertyTime == Property::INVALID_INDEX)
  {
    mPropertyTime = SafeRegisterProperty( scrollView, Toolkit::ScrollViewTwistEffect::EFFECT_TIME, 0.0f );
    mPropertyReference = SafeRegisterProperty( scrollView, Toolkit::ScrollViewTwistEffect::EFFECT_REFERENCE, Vector3::ZERO );
    mPropertyDepth = SafeRegisterProperty( scrollView, Toolkit::ScrollViewTwistEffect::EFFECT_DEPTH, 0.0f);
    mPropertyActivate = SafeRegisterProperty(scrollView,Toolkit::ScrollViewTwistEffect::EFFECT_ACTIVATE,1.0f);
  }

  // Connect to the scroll view signals
  scrollView.ScrollStartedSignal().Connect(this, &ScrollViewTwistEffect::OnScrollStart);
  scrollView.SnapStartedSignal().Connect(this, &ScrollViewTwistEffect::OnScrollSnap);
  scrollView.ScrollUpdatedSignal().Connect(this, &ScrollViewTwistEffect::OnScrollUpdate);
  scrollView.ScrollCompletedSignal().Connect(this, &ScrollViewTwistEffect::OnScrollComplete);

  AttachActor(scrollView);
}

void ScrollViewTwistEffect::OnDetach(Toolkit::ScrollView& scrollView)
{
  scrollView.ScrollStartedSignal().Disconnect(this, &ScrollViewTwistEffect::OnScrollStart);
  scrollView.SnapStartedSignal().Disconnect(this, &ScrollViewTwistEffect::OnScrollSnap);
  scrollView.ScrollUpdatedSignal().Disconnect(this, &ScrollViewTwistEffect::OnScrollUpdate);
  scrollView.ScrollCompletedSignal().Disconnect(this, &ScrollViewTwistEffect::OnScrollComplete);

  if(mAnimation)
  {
    mAnimation.FinishedSignal().Disconnect(this, &ScrollViewTwistEffect::OnAnimationFinished);
    mAnimation.Clear();
    mAnimation.Reset();
  }
}

void ScrollViewTwistEffect::AttachActor(Actor actor)
{

}

void ScrollViewTwistEffect::DetachActor(Actor actor)
{
  // TODO: remove the specific constraint defined in AttachActor (and possibly
  // unregister property) - neither functionality exists in Dali.
}

void ScrollViewTwistEffect::ContinueAnimation(float endTime)
{
  // continue animating
  if(mAnimation)
  {
    mAnimation.FinishedSignal().Disconnect(this, &ScrollViewTwistEffect::OnAnimationFinished);
    mAnimation.Clear();
  }

  Actor scrollView = GetScrollView();

  mAnimation = Animation::New(TWISTEFFECT_ANIMATION_MAX_TIME);
  mAnimation.AnimateTo( Property(scrollView, mPropertyTime), endTime, AlphaFunctions::Linear );
  mAnimation.FinishedSignal().Connect(this, &ScrollViewTwistEffect::OnAnimationFinished);
  mAnimation.Play();
}

void ScrollViewTwistEffect::OnScrollStart( const Vector3& position )
{
  if(mActivateAnimation)
  {
    // if the animation after Scroll complete not terminate before another scroll action, stop the animation before start again
    mActivateAnimation.Stop();
    mActivateAnimation.Clear();
    mActivateAnimation = NULL;
  }

  GetScrollView().SetProperty(mPropertyTime, 0.0f);
  if(mEnableEffect)
  {
    GetScrollView().SetProperty(mPropertyActivate, 1.0f);
  }
  else
  {
    GetScrollView().SetProperty(mPropertyActivate, 0.0f);
  }
  GetScrollView().SetProperty(mPropertyReference, position);

  ContinueAnimation(TWISTEFFECT_ANIMATION_MAX_TIME);
}

void ScrollViewTwistEffect::OnScrollUpdate( const Vector3& position )
{
  // nothing to do
}

void ScrollViewTwistEffect::OnScrollComplete( const Vector3& position )
{
  if(!mEnableEffect)
  {
    OnActivateAnimationFinished(mAnimation);
    return;
  }
  Actor scrollView = GetScrollView();
  scrollView.SetProperty(mPropertyActivate, 1.0f);
  mActivateAnimation = Animation::New(DELAY);
  mActivateAnimation.AnimateTo( Property(scrollView, mPropertyActivate), 0.0f, AlphaFunctions::Linear);
  mActivateAnimation.FinishedSignal().Connect(this, &ScrollViewTwistEffect::OnActivateAnimationFinished);
  mActivateAnimation.Play();
}

void ScrollViewTwistEffect::OnScrollSnap( const Toolkit::ScrollView::SnapEvent& event )
{
  // If a Flicking snap is occuring and the distance is more than mMinimumDistanceForShrink
  // then animate depth effect i.e. shrink actors and then bring back in to regular size.
  // NOTE: ScrollView Snap returns a value opposite of GetCurrentScrollPosition
  // i.e. if you've "scrolled 100 pixels right" (so content on screen has shifted 100 pixels left)
  // then GetCurrentScrollPosition returns a positive value (100.0f, 0.0f) (position of where you're
  // look, not where content has been moved to).
  // event.position returns a negative value (-100.0f, 0.0f)
  // Would be a good idea to change SnapEvent in the API so it reflects GetCurrentScrollPosition.
  // TODO: Change scroll-view API, check if anything uses SnapEvent and change them correspondingly
  Vector3 targetScrollPosition(-event.position);

  Vector3 delta = targetScrollPosition - GetScrollView().GetCurrentScrollPosition();

  if(event.type==Flick && delta.Length() > mMinimumDistanceForShrink)
  {
    Actor scrollView = GetScrollView();

    Animation animation = Animation::New(event.duration);
    animation.AnimateTo( Property(scrollView, mPropertyDepth), 1.0f, HopEasing);
    animation.Play();
  }
}

void ScrollViewTwistEffect::OnAnimationFinished( Animation& animation )
{
  // still unstable, so continue animating.
  // TODO: Requires an instability check to ensure time animation finishes when delay is
  // less noticeable. i.e. all present scrollPositions are approx the same as mScrollPosition in constraints.
  // best solution for this is to switch to a single history vector of scroll position, and compare if
  // position has not deviated >= 0.5 pixel for the past 1 second.
  float endTime = GetScrollView().GetProperty<float>(mPropertyTime) + TWISTEFFECT_ANIMATION_MAX_TIME;
  ContinueAnimation(endTime);
}

void ScrollViewTwistEffect::OnActivateAnimationFinished( Animation& animation )
{
  if(mAnimation)
  {
    mAnimation.FinishedSignal().Disconnect(this, &ScrollViewTwistEffect::OnAnimationFinished);
    mAnimation.Clear();
    mAnimation.Reset();
  }
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali
