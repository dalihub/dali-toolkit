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
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/events/mouse-wheel-event.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-constraints.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-component-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-effect-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-overshoot-indicator-impl.h>

//#define ENABLED_SCROLL_STATE_LOGGING

#ifdef ENABLED_SCROLL_STATE_LOGGING
#define DALI_LOG_SCROLL_STATE(format, args...) Dali::Integration::Log::LogMessage(Dali::Integration::Log::DebugInfo, "%s:%d " format "\n", __PRETTY_FUNCTION__, __LINE__, ## args)
#else
#define DALI_LOG_SCROLL_STATE(format, args...)
#endif

// TODO: Change to two class system:
// 1. DraggableActor (is an actor which can be dragged anywhere, can be set to range using the ruler)
// 2. ScrollView (contains a draggable actor that can a) be dragged in the negative X, and Y domain, b) has a hitArea for touches)
// TODO: external components (page and status overlays).
// TODO: Orientation.
// TODO: upgrade Vector2/3 to support returning Unit vectors, normals, & cross product (dot product is already provided)

using namespace Dali;

namespace
{

const int DEFAULT_REFRESH_INTERVAL_MILLISECONDS = 50;                                     ///< Refresh rate TODO: Animation should have an update signal (and see item-view-impl)
const Vector2 DEFAULT_MIN_FLICK_DISTANCE(30.0f, 30.0f);                                   ///< minimum distance for pan before flick allowed
const float DEFAULT_MIN_FLICK_SPEED_THRESHOLD(500.0f);                          ///< Minimum pan speed required for flick in pixels/s
const float FREE_FLICK_SPEED_THRESHOLD = 200.0f;                                          ///< Free-Flick threshold in pixels/ms
const float AUTOLOCK_AXIS_MINIMUM_DISTANCE2 = 100.0f;                                     ///< Auto-lock axis after minimum distance squared.
const float FLICK_ORTHO_ANGLE_RANGE = 75.0f;                                              ///< degrees. (if >45, then supports diagonal flicking)
const unsigned int MAXIMUM_NUMBER_OF_VALUES = 5;                                          ///< Number of values to use for weighted pan calculation.
const Vector2 DEFAULT_MOUSE_WHEEL_SCROLL_DISTANCE_STEP_PROPORTION = Vector2(0.17f, 0.1f); ///< The step of horizontal scroll distance in the proportion of stage size for each mouse wheel event received.
const unsigned long MINIMUM_TIME_BETWEEN_DOWN_AND_UP_FOR_RESET( 150u );
const float DEFAULT_OVERSHOOT_ANIMATION_DURATION = 0.35f;  // time in seconds
const Vector2 OVERSCROLL_CLAMP(1.0f, 1.0f);                // maximum overscroll allowed in pixels when overshoot indicator is being used
const float TOUCH_DOWN_TIMER_INTERVAL = 100.0f;
const float DEFAULT_SCROLL_UPDATE_DISTANCE( 30.0f );                               ///< Default distance to travel in pixels for scroll update signal

// predefined effect values
const Vector3 ANGLE_CAROUSEL_ROTATE(Math::PI * 0.5f, Math::PI * 0.5f, 0.0f);
const Vector3 ANGLE_CUBE_PAGE_ROTATE(Math::PI * 0.2f, Math::PI * 0.2f, 0.0f);  ///< Cube page rotates as if it has ten sides with the camera positioned inside
const Vector2 ANGLE_CUSTOM_CUBE_SWING(-Math::PI * 0.45f, -Math::PI * 0.45f);  ///< outer cube pages swing 90 degrees as they pan offscreen
const Vector2 ANGLE_SPIRAL_SWING_IN(Math::PI * 0.5f, Math::PI * 0.5f);
const Vector2 ANGLE_SPIRAL_SWING_OUT(Math::PI * 0.35f, Math::PI * 0.35f);
const Vector2 ANGLE_OUTER_CUBE_SWING(Math::PI * 0.5f, Math::PI * 0.5f);  ///< outer cube pages swing 90 degrees as they pan offscreen

// Helpers ////////////////////////////////////////////////////////////////////////////////////////

/**
 * Find the vector (distance) from (a) to (b)
 * in domain (start) to (end)
 * (\ / start)               (\ / end)
 *   |-a                 b<----|
 *
 * @note assumes both (a) and (b) are already with the domain
 * (start) to (end)
 *
 * @param[in] a the current point
 * @param[in] b the target point
 * @param[in] start the start of the domain
 * @param[in] end the end of the domain
 * @param[in] bias whether to only take the right direction or the left direction,
 * or the shortest direction.
 * @return the shortest direction and distance
 */
float VectorInDomain(float a, float b, float start, float end, Dali::Toolkit::DirectionBias bias)
{
  if(bias == Dali::Toolkit::DirectionBiasNone)
  {
    return ShortestDistanceInDomain( a, b, start, end );
  }
  //  (a-start + end-b)
  float size = end-start;
  float vect = b-a;

  if(vect > 0)
  {
    // +ve vector
    if(bias == Dali::Toolkit::DirectionBiasRight) // going right, take the vector.
    {
      return vect;
    }
    else
    {
      float aRight = a+size;
      return b-aRight;
    }
  }
  else
  {
    // -ve vector
    if(bias == Dali::Toolkit::DirectionBiasLeft) // going left, take the vector.
    {
      return vect;
    }
    else
    {
      float aLeft = a-size;
      return b-aLeft;
    }
  }
}

/**
 * Returns the position of the anchor within actor
 *
 * @param actor The Actor
 * @param anchor The Anchor point of interest.
 * @return The position of the Anchor
 */
Vector3 GetPositionOfAnchor(Actor &actor, const Vector3 &anchor)
{
  Vector3 childPosition = actor.GetCurrentPosition();
  Vector3 childAnchor = - actor.GetCurrentAnchorPoint() + anchor;
  Vector3 childSize = actor.GetCurrentSize();

  return childPosition + childAnchor * childSize;
}

// AlphaFunctions /////////////////////////////////////////////////////////////////////////////////

float FinalDefaultAlphaFunction(float offset)
{
  return offset * 0.5f;
}

/**
 * ConstantDecelerationAlphaFunction
 * Newtoninan distance for constant deceleration
 * v = 1 - t, s = t - 1/2 t^2
 * when t = 0, s = 0.0 (min distance)
 * when t = 1, s = 0.5 (max distance)
 * progress = s / (max-min) = 2t - t^2
 *
 * @param[in] offset The input progress
 * @return The output progress
 */
float ConstantDecelerationAlphaFunction(float progress)
{
  return progress * 2.0f - progress * progress;
}

// Internal Constraints ///////////////////////////////////////////////////////////////////////////

/**
 * Internal Relative position Constraint
 * Generates the relative position value of the scroll view
 * based on the absolute position, and it's relation to the
 * scroll domain. This is a value from 0.0f to 1.0f in each
 * scroll position axis.
 */
Vector3 InternalRelativePositionConstraint(const Vector3&    current,
                                           const PropertyInput& scrollPositionProperty,
                                           const PropertyInput& scrollMinProperty,
                                           const PropertyInput& scrollMaxProperty,
                                           const PropertyInput& scrollSizeProperty)
{
  Vector3 position = -scrollPositionProperty.GetVector3();
  const Vector3& min = scrollMinProperty.GetVector3();
  const Vector3& max = scrollMaxProperty.GetVector3();
  const Vector3& size = scrollSizeProperty.GetVector3();

  position.x = WrapInDomain(position.x, min.x, max.x);
  position.y = WrapInDomain(position.y, min.y, max.y);

  Vector3 relativePosition;
  Vector3 domainSize = (max - min) - size;

  relativePosition.x = domainSize.x > Math::MACHINE_EPSILON_1 ? fabsf((position.x - min.x) / domainSize.x) : 0.0f;
  relativePosition.y = domainSize.y > Math::MACHINE_EPSILON_1 ? fabsf((position.y - min.y) / domainSize.y) : 0.0f;

  return relativePosition;
}

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

/**
 * Returns whether to lock scrolling to a particular axis
 *
 * @param[in] panDelta Distance panned since gesture started
 * @param[in] currentLockAxis The current lock axis value
 * @param[in] lockGradient How quickly to lock to a particular axis
 *
 * @return The new axis lock state
 */
ScrollView::LockAxis GetLockAxis(const Vector2& panDelta, ScrollView::LockAxis currentLockAxis, float lockGradient)
{
  if(panDelta.LengthSquared() > AUTOLOCK_AXIS_MINIMUM_DISTANCE2 &&
      currentLockAxis == ScrollView::LockPossible)
  {
    float dx = fabsf(panDelta.x);
    float dy = fabsf(panDelta.y);
    if(dx * lockGradient >= dy)
    {
      // 0.36:1 gradient to the horizontal (deviate < 20 degrees)
      currentLockAxis = ScrollView::LockVertical;
    }
    else if(dy * lockGradient > dx)
    {
      // 0.36:1 gradient to the vertical (deviate < 20 degrees)
      currentLockAxis = ScrollView::LockHorizontal;
    }
    else
    {
      currentLockAxis = ScrollView::LockNone;
    }
  }
  return currentLockAxis;
}

/**
 * Internal Pre-Position Property Constraint.
 *
 * Generates position property based on current position + gesture displacement.
 * Or generates position property based on positionX/Y.
 * Note: This is the position prior to any clamping at scroll boundaries.
 */
struct InternalPrePositionConstraint
{
  InternalPrePositionConstraint(const Vector2& initialPanPosition,
                                const Vector2& initialPanMask,
                                bool axisAutoLock,
                                float axisAutoLockGradient,
                                ScrollView::LockAxis initialLockAxis,
                                const Vector2& maxOvershoot,
                                const RulerDomain& domainX, const RulerDomain& domainY)
  : mLocalStart(initialPanPosition),
    mInitialPanMask(initialPanMask),
    mDomainMin( -domainX.min, -domainY.min ),
    mDomainMax( -domainX.max, -domainY.max ),
    mMaxOvershoot(maxOvershoot),
    mAxisAutoLockGradient(axisAutoLockGradient),
    mLockAxis(initialLockAxis),
    mAxisAutoLock(axisAutoLock),
    mWasPanning(false),
    mClampX( domainX.enabled ),
    mClampY( domainY.enabled )
  {
  }

  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& gesturePositionProperty,
                     const PropertyInput& sizeProperty)
  {
    Vector3 scrollPostPosition = current;
    Vector2 panPosition = gesturePositionProperty.GetVector2();

    if(!mWasPanning)
    {
      mPrePosition = current;
      mCurrentPanMask = mInitialPanMask;
      mWasPanning = true;
    }

    // Calculate Deltas...
    Vector2 currentPosition = gesturePositionProperty.GetVector2();
    Vector2 panDelta( currentPosition - mLocalStart );

    // Axis Auto Lock - locks the panning to the horizontal or vertical axis if the pan
    // appears mostly horizontal or mostly vertical respectively...
    if( mAxisAutoLock )
    {
      mLockAxis = GetLockAxis(panDelta, mLockAxis, mAxisAutoLockGradient);
      if( mLockAxis == ScrollView::LockVertical )
      {
        mCurrentPanMask.y = 0.0f;
      }
      else if( mLockAxis == ScrollView::LockHorizontal )
      {
        mCurrentPanMask.x = 0.0f;
      }
    }

    // Restrict deltas based on ruler enable/disable and axis-lock state...
    panDelta *= mCurrentPanMask;

    // Perform Position transform based on input deltas...
    scrollPostPosition = mPrePosition;
    scrollPostPosition.GetVectorXY() += panDelta;

    // if no wrapping then clamp preposition to maximum overshoot amount
    const Vector3& size = sizeProperty.GetVector3();
    if( mClampX )
    {
      float newXPosition = Clamp(scrollPostPosition.x, (mDomainMax.x + size.x) - mMaxOvershoot.x, mDomainMin.x + mMaxOvershoot.x );
      if( (newXPosition < scrollPostPosition.x - Math::MACHINE_EPSILON_1)
              || (newXPosition > scrollPostPosition.x + Math::MACHINE_EPSILON_1) )
      {
        mPrePosition.x = newXPosition;
        mLocalStart.x = panPosition.x;
      }
      scrollPostPosition.x = newXPosition;
    }
    if( mClampY )
    {
      float newYPosition = Clamp(scrollPostPosition.y, (mDomainMax.y + size.y) - mMaxOvershoot.y, mDomainMin.y + mMaxOvershoot.y );
      if( (newYPosition < scrollPostPosition.y - Math::MACHINE_EPSILON_1)
              || (newYPosition > scrollPostPosition.y + Math::MACHINE_EPSILON_1) )
      {
        mPrePosition.y = newYPosition;
        mLocalStart.y = panPosition.y;
      }
      scrollPostPosition.y = newYPosition;
    }

    return scrollPostPosition;
  }

  Vector3 mPrePosition;
  Vector2 mLocalStart;
  Vector2 mInitialPanMask;              ///< Initial pan mask (based on ruler settings)
  Vector2 mCurrentPanMask;              ///< Current pan mask that can be altered by axis lock mode.
  Vector2 mDomainMin;
  Vector2 mDomainMax;
  Vector2 mMaxOvershoot;

  float mAxisAutoLockGradient;          ///< Set by ScrollView
  ScrollView::LockAxis mLockAxis;

  bool mAxisAutoLock:1;                   ///< Set by ScrollView
  bool mWasPanning:1;
  bool mClampX:1;
  bool mClampY:1;
};

/**
 * Internal Position Property Constraint.
 *
 * Generates position property based on pre-position
 * Note: This is the position after clamping.
 * (uses result of InternalPrePositionConstraint)
 */
struct InternalPositionConstraint
{
  InternalPositionConstraint(const RulerDomain& domainX, const RulerDomain& domainY, bool wrap)
  : mDomainMin( -domainX.min, -domainY.min ),
    mDomainMax( -domainX.max, -domainY.max ),
    mClampX( domainX.enabled ),
    mClampY( domainY.enabled ),
    mWrap( wrap )
  {
  }

  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& scrollPositionProperty,
                     const PropertyInput& scrollMinProperty,
                     const PropertyInput& scrollMaxProperty,
                     const PropertyInput& scrollSizeProperty)
  {
    Vector3 position = scrollPositionProperty.GetVector3();
    const Vector2& size = scrollSizeProperty.GetVector3().GetVectorXY();
    const Vector3& min = scrollMinProperty.GetVector3();
    const Vector3& max = scrollMaxProperty.GetVector3();

    if( mWrap )
    {
      position.x = -WrapInDomain(-position.x, min.x, max.x);
      position.y = -WrapInDomain(-position.y, min.y, max.y);
    }
    else
    {
      // clamp post position to domain
      position.x = mClampX ? Clamp(position.x, mDomainMax.x + size.x, mDomainMin.x ) : position.x;
      position.y = mClampY ? Clamp(position.y, mDomainMax.y + size.y, mDomainMin.y ) : position.y;
    }

    return position;
  }

  Vector2 mDomainMin;
  Vector2 mDomainMax;
  bool mClampX;
  bool mClampY;
  bool mWrap;

};

/**
 * This constraint updates the X overshoot property using the difference
 * mPropertyPrePosition.x and mPropertyPosition.x, returning a relative value between 0.0f and 1.0f
 */
struct OvershootXConstraint
{
  OvershootXConstraint(float maxOvershoot) : mMaxOvershoot(maxOvershoot) {}

  float operator()(const float&    current,
      const PropertyInput& scrollPrePositionProperty,
      const PropertyInput& scrollPostPositionProperty,
      const PropertyInput& canScrollProperty)
  {
    if( canScrollProperty.GetBoolean() )
    {
      const Vector3& scrollPrePosition = scrollPrePositionProperty.GetVector3();
      const Vector3& scrollPostPosition = scrollPostPositionProperty.GetVector3();
      float newOvershoot = scrollPrePosition.x - scrollPostPosition.x;
      return (newOvershoot > 0.0f ? std::min(newOvershoot, mMaxOvershoot) : std::max(newOvershoot, -mMaxOvershoot)) / mMaxOvershoot;
    }
    return 0.0f;
  }

  float mMaxOvershoot;
};

/**
 * This constraint updates the Y overshoot property using the difference
 * mPropertyPrePosition.y and mPropertyPosition.y, returning a relative value between 0.0f and 1.0f
 */
struct OvershootYConstraint
{
  OvershootYConstraint(float maxOvershoot) : mMaxOvershoot(maxOvershoot) {}

  float operator()(const float&    current,
      const PropertyInput& scrollPrePositionProperty,
      const PropertyInput& scrollPostPositionProperty,
      const PropertyInput& canScrollProperty)
  {
    if( canScrollProperty.GetBoolean() )
    {
      const Vector3& scrollPrePosition = scrollPrePositionProperty.GetVector3();
      const Vector3& scrollPostPosition = scrollPostPositionProperty.GetVector3();
      float newOvershoot = scrollPrePosition.y - scrollPostPosition.y;
      return (newOvershoot > 0.0f ? std::min(newOvershoot, mMaxOvershoot) : std::max(newOvershoot, -mMaxOvershoot)) / mMaxOvershoot;
    }
    return 0.0f;
  }

  float mMaxOvershoot;
};

/**
 * Internal Position-Delta Property Constraint.
 *
 * Generates position-delta property based on scroll-position + scroll-offset properties.
 */
Vector3 InternalPositionDeltaConstraint(const Vector3&    current,
                                        const PropertyInput& scrollPositionProperty,
                                        const PropertyInput& scrollOffsetProperty)
{
  const Vector3& scrollPosition = scrollPositionProperty.GetVector3();
  const Vector3& scrollOffset = scrollOffsetProperty.GetVector3();

  return scrollPosition + scrollOffset;
}

/**
 * Internal Final Position Constraint
 * The position of content is:
 * of scroll-position + f(scroll-overshoot)
 * where f(...) function defines how overshoot
 * should affect final-position.
 */
struct InternalFinalConstraint
{
  InternalFinalConstraint(AlphaFunction functionX,
                          AlphaFunction functionY)
  : mFunctionX(functionX),
    mFunctionY(functionY)
  {
  }

  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& scrollPositionProperty,
                     const PropertyInput& scrollOvershootXProperty,
                     const PropertyInput& scrollOvershootYProperty)
  {
    const float& overshootx = scrollOvershootXProperty.GetFloat();
    const float& overshooty = scrollOvershootYProperty.GetFloat();
    Vector3 offset( mFunctionX(overshootx),
                    mFunctionY(overshooty),
                    0.0f);

    return scrollPositionProperty.GetVector3() - offset;
  }

  AlphaFunction mFunctionX;
  AlphaFunction mFunctionY;
};


BaseHandle Create()
{
  return Toolkit::ScrollView::New();
}

TypeRegistration typeRegistration( typeid(Toolkit::ScrollView), typeid(Toolkit::Scrollable), Create );

SignalConnectorType signalConnector1( typeRegistration, Toolkit::ScrollView::SIGNAL_SNAP_STARTED, &ScrollView::DoConnectSignal );

}


///////////////////////////////////////////////////////////////////////////////////////////////////
// ScrollView
///////////////////////////////////////////////////////////////////////////////////////////////////

Dali::Toolkit::ScrollView ScrollView::New()
{
  // Create the implementation
  ScrollViewPtr scrollView(new ScrollView());

  // Pass ownership to CustomActor via derived handle
  Dali::Toolkit::ScrollView handle(*scrollView);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  scrollView->Initialize();

  return handle;
}

ScrollView::ScrollView()
: ScrollBase(),
  mTouchDownTime(0u),
  mGestureStackDepth(0),
  mScrollStateFlags(0),
  mMinTouchesForPanning(1),
  mMaxTouchesForPanning(1),
  mLockAxis(LockPossible),
  mScrollUpdateDistance(DEFAULT_SCROLL_UPDATE_DISTANCE),
  mOvershootDelay(1.0f),
  mMaxOvershoot(Toolkit::ScrollView::DEFAULT_MAX_OVERSHOOT, Toolkit::ScrollView::DEFAULT_MAX_OVERSHOOT),
  mUserMaxOvershoot(Toolkit::ScrollView::DEFAULT_MAX_OVERSHOOT, Toolkit::ScrollView::DEFAULT_MAX_OVERSHOOT),
  mSnapOvershootDuration(Toolkit::ScrollView::DEFAULT_SNAP_OVERSHOOT_DURATION),
  mSnapOvershootAlphaFunction(AlphaFunctions::EaseOut),
  mSnapDuration(Toolkit::ScrollView::DEFAULT_SLOW_SNAP_ANIMATION_DURATION),
  mSnapAlphaFunction(AlphaFunctions::EaseOut),
  mMinFlickDistance(DEFAULT_MIN_FLICK_DISTANCE),
  mFlickSpeedThreshold(DEFAULT_MIN_FLICK_SPEED_THRESHOLD),
  mFlickDuration(Toolkit::ScrollView::DEFAULT_FAST_SNAP_ANIMATION_DURATION),
  mFlickAlphaFunction(AlphaFunctions::EaseOut),
  mAxisAutoLockGradient(Toolkit::ScrollView::DEFAULT_AXIS_AUTO_LOCK_GRADIENT),
  mFrictionCoefficient(Toolkit::ScrollView::DEFAULT_FRICTION_COEFFICIENT),
  mFlickSpeedCoefficient(Toolkit::ScrollView::DEFAULT_FLICK_SPEED_COEFFICIENT),
  mMaxFlickSpeed(Toolkit::ScrollView::DEFAULT_MAX_FLICK_SPEED),
  mInAccessibilityPan(false),
  mInitialized(false),
  mScrolling(false),
  mScrollInterrupted(false),
  mPanning(false),
  mSensitive(true),
  mTouchDownTimeoutReached(false),
  mActorAutoSnapEnabled(false),
  mAutoResizeContainerEnabled(false),
  mWrapMode(false),
  mAxisAutoLock(false),
  mAlterChild(false),
  mDefaultMaxOvershoot(true),
  mCanScrollHorizontal(true),
  mCanScrollVertical(true)
{
  SetRequiresMouseWheelEvents(true);
}

void ScrollView::OnInitialize()
{
  Actor self = Self();

  // Internal Actor, used to hide actors from enumerations.
  // Also actors added to Internal actor appear as overlays e.g. ScrollBar components.
  mInternalActor = Actor::New();
  mInternalActor.SetDrawMode(DrawMode::OVERLAY);
  self.Add(mInternalActor);
  mInternalActor.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
  mInternalActor.SetParentOrigin(ParentOrigin::CENTER);
  mInternalActor.SetAnchorPoint(AnchorPoint::CENTER);

  mAlterChild = true;

  // Register Scroll Properties.
  RegisterProperties();

  mScrollPostPosition = mScrollPrePosition = Vector3::ZERO;

  mMouseWheelScrollDistanceStep = Stage::GetCurrent().GetSize() * DEFAULT_MOUSE_WHEEL_SCROLL_DISTANCE_STEP_PROPORTION;

  mInitialized = true;

  mGestureStackDepth = 0;

  EnableGestureDetection( Gesture::Type( Gesture::Pan ) );

  // For pan, default to only 1 touch required, ignoring touches outside this range.
  SetTouchesRequiredForPanning(1, 1, false);

  // By default we'll allow the user to freely drag the scroll view,
  // while disabling the other rulers.
  RulerPtr ruler = new DefaultRuler();
  mRulerX = ruler;
  mRulerY = ruler;

  EnableScrollComponent(Toolkit::Scrollable::OvershootIndicator);

  Vector3 size = GetControlSize();
  UpdatePropertyDomain(size);
  SetInternalConstraints();
}

void ScrollView::OnControlStageConnection()
{
  DALI_LOG_SCROLL_STATE("[0x%X]", this);

  if ( mSensitive )
  {
    SetScrollSensitive( false );
    SetScrollSensitive( true );
  }
  if(IsScrollComponentEnabled(Toolkit::Scrollable::OvershootIndicator))
  {
    // try and make sure property notifications are set
    EnableScrollComponent(Toolkit::Scrollable::OvershootIndicator);
  }
}

void ScrollView::OnControlStageDisconnection()
{
  DALI_LOG_SCROLL_STATE("[0x%X]", this);

  StopAnimation();
}

ScrollView::~ScrollView()
{
  DALI_LOG_SCROLL_STATE("[0x%X]", this);
}

AlphaFunction ScrollView::GetScrollSnapAlphaFunction() const
{
  return mSnapAlphaFunction;
}

void ScrollView::SetScrollSnapAlphaFunction(AlphaFunction alpha)
{
  mSnapAlphaFunction = alpha;
}

AlphaFunction ScrollView::GetScrollFlickAlphaFunction() const
{
  return mFlickAlphaFunction;
}

void ScrollView::SetScrollFlickAlphaFunction(AlphaFunction alpha)
{
  mFlickAlphaFunction = alpha;
}

float ScrollView::GetScrollSnapDuration() const
{
  return mSnapDuration;
}

void ScrollView::SetScrollSnapDuration(float time)
{
  mSnapDuration = time;
}

float ScrollView::GetScrollFlickDuration() const
{
  return mFlickDuration;
}

void ScrollView::SetScrollFlickDuration(float time)
{
  mFlickDuration = time;
}

void ScrollView::ApplyEffect(Toolkit::ScrollViewEffect effect)
{
  Dali::Toolkit::ScrollView self = Dali::Toolkit::ScrollView::DownCast(Self());

  // Assertion check to ensure effect doesn't already exist in this scrollview
  bool effectAlreadyExistsInScrollView(false);
  for (ScrollViewEffectIter iter = mEffects.begin(); iter != mEffects.end(); ++iter)
  {
    if(*iter==effect)
    {
      effectAlreadyExistsInScrollView = true;
      break;
    }
  }

  DALI_ASSERT_ALWAYS(!effectAlreadyExistsInScrollView);

  // add effect to effects list
  mEffects.push_back(effect);

  // invoke Attachment request to ScrollView first
  GetImpl(effect).Attach(self);
}

Toolkit::ScrollViewEffect ScrollView::ApplyEffect(Toolkit::ScrollView::PageEffect effect)
{
  Toolkit::ScrollViewEffect scrollEffect;
  switch(effect)
  {
    case Toolkit::ScrollView::PageEffectNone:
    {
      break;
    }
    case Toolkit::ScrollView::PageEffectOuterCube:
    {
      Toolkit::ScrollViewCustomEffect customEffect;
      scrollEffect = customEffect = Toolkit::ScrollViewCustomEffect::New();
      Vector2 pageSize = Stage::GetCurrent().GetSize();
      // set the page translation to the slide off distance, also add an extra value to space the pages, having a smaller spacing on translationOut will allow the spacing to reduce over time
      // the page moving onto screen will start 50.0f further out (1.0f * 50.0f) and the spacing will reduce as its position reaches the centre (0.0f * 50.0f)
      // the page moving off screen will slowly build a spacing from 0.0f to 20.0f
      // the spacing from each page is added together for the final spacing between the two pages.
      customEffect.SetPageTranslation(Vector3(pageSize.x, pageSize.y, 0) + Vector3(50.0f, 50.0f, 0.0f), Vector3(pageSize.x, pageSize.y, 0) + Vector3(20.0f, 20.0f, 0.0f));
      customEffect.SetSwingAngleOut(ANGLE_CUSTOM_CUBE_SWING.x, Vector3(0.0f, -1.0f, 0.0f));
      customEffect.SetSwingAnchor(AnchorPoint::CENTER, AnchorPoint::CENTER_LEFT);
      customEffect.SetOpacityThreshold(0.7f);
      break;
    }
    case Toolkit::ScrollView::PageEffectDepth:
    {
      Toolkit::ScrollViewCustomEffect customEffect;
      scrollEffect = customEffect = Toolkit::ScrollViewCustomEffect::New();
      break;
    }
    case Toolkit::ScrollView::PageEffectInnerCube:
    {
      Toolkit::ScrollViewCustomEffect customEffect;
      scrollEffect = customEffect = Toolkit::ScrollViewCustomEffect::New();
      customEffect.SetPageSpacing(Vector2(30.0f, 30.0f));
      customEffect.SetAngledOriginPageRotation(ANGLE_CUBE_PAGE_ROTATE);
      customEffect.SetSwingAngle(ANGLE_CUBE_PAGE_ROTATE.x, Vector3(0,-1,0));
      customEffect.SetOpacityThreshold(0.5f);
      break;
    }
    case Toolkit::ScrollView::PageEffectCarousel:
    {
      Toolkit::ScrollViewCustomEffect customEffect;
      scrollEffect = customEffect = Toolkit::ScrollViewCustomEffect::New();
      customEffect.SetPageTranslation(Vector3(0,0,0), Vector3(-30, 0, 0));
      customEffect.SetPageSpacing(Vector2(60.0f, 60.0f));
      customEffect.SetAngledOriginPageRotation(-ANGLE_CUBE_PAGE_ROTATE);
      customEffect.SetOpacityThreshold(0.2f, 0.6f);
      break;
    }
    case Toolkit::ScrollView::PageEffectSpiral:
    {
      Toolkit::ScrollViewCustomEffect customEffect;
      scrollEffect = customEffect = Toolkit::ScrollViewCustomEffect::New();

      Vector2 pageSize = Stage::GetCurrent().GetSize();
      customEffect.SetSwingAngle(-ANGLE_SPIRAL_SWING_IN.x, Vector3(0.0f, -1.0f, 0.0f), ANGLE_SPIRAL_SWING_OUT.x, Vector3(0.0f, -1.0f, 0.0f));
      //customEffect.SetSwingAngleAlphaFunctionOut(AlphaFunctions::EaseOut);
      customEffect.SetSwingAnchor(AnchorPoint::CENTER_RIGHT);
      customEffect.SetPageTranslation(Vector3(pageSize.x, pageSize.y, 0) + Vector3(100.0f, 100.0f, 0.0f), Vector3(pageSize.x, pageSize.y, -pageSize.y * 2.0f) * 0.33f);
      //customEffect.SetPageTranslateAlphaFunctionOut(AlphaFunctions::EaseOut);
      customEffect.SetOpacityThreshold(0.75f, 0.6f);
      customEffect.SetOpacityAlphaFunctionIn(AlphaFunctions::EaseInOut);
      break;
    }
    default:
    {
      DALI_ASSERT_DEBUG(0 && "unknown scroll view effect");
    }
  }
  RemoveConstraintsFromChildren();
  if(scrollEffect)
  {
    ApplyEffect(scrollEffect);
  }
  return scrollEffect;
}

void ScrollView::RemoveEffect(Toolkit::ScrollViewEffect effect)
{
  Dali::Toolkit::ScrollView self = Dali::Toolkit::ScrollView::DownCast(Self());

  // remove effect from effects list
  bool effectExistedInScrollView(false);
  for (ScrollViewEffectIter iter = mEffects.begin(); iter != mEffects.end(); ++iter)
  {
    if(*iter==effect)
    {
      mEffects.erase(iter);
      effectExistedInScrollView = true;
      break;
    }
  }

  // Assertion check to ensure effect existed.
  DALI_ASSERT_ALWAYS(effectExistedInScrollView);

  // invoke Detachment request to ScrollView last
  GetImpl(effect).Detach(self);
}

void ScrollView::RemoveAllEffects()
{
  Dali::Toolkit::ScrollView self = Dali::Toolkit::ScrollView::DownCast(Self());

  for (ScrollViewEffectIter effectIter = mEffects.begin(); effectIter != mEffects.end(); ++effectIter)
  {
    Toolkit::ScrollViewEffect effect = *effectIter;

    // invoke Detachment request to ScrollView last
    GetImpl(effect).Detach(self);
  }

  mEffects.clear();
}

void ScrollView::ApplyConstraintToChildren(Constraint constraint)
{
  ApplyConstraintToBoundActors(constraint);
}

void ScrollView::RemoveConstraintsFromChildren()
{
  RemoveConstraintsFromBoundActors();
}

const RulerPtr ScrollView::GetRulerX() const
{
  return mRulerX;
}

const RulerPtr ScrollView::GetRulerY() const
{
  return mRulerY;
}

void ScrollView::SetRulerX(RulerPtr ruler)
{
  mRulerX = ruler;

  Vector3 size = GetControlSize();
  UpdatePropertyDomain(size);
  UpdateMainInternalConstraint();
}

void ScrollView::SetRulerY(RulerPtr ruler)
{
  mRulerY = ruler;

  Vector3 size = GetControlSize();
  UpdatePropertyDomain(size);
  UpdateMainInternalConstraint();
}

void ScrollView::UpdatePropertyDomain(const Vector3& size)
{
  Actor self = Self();
  Vector3 min = mMinScroll;
  Vector3 max = mMaxScroll;
  bool scrollPositionChanged = false;
  bool domainChanged = false;

  bool canScrollVertical = false;
  bool canScrollHorizontal = false;
  UpdateLocalScrollProperties();
  if(mRulerX->IsEnabled())
  {
    const Toolkit::RulerDomain& rulerDomain = mRulerX->GetDomain();
    if( fabsf(min.x - rulerDomain.min) > Math::MACHINE_EPSILON_100
        || fabsf(max.x - rulerDomain.max) > Math::MACHINE_EPSILON_100 )
    {
      domainChanged = true;
      min.x = rulerDomain.min;
      max.x = rulerDomain.max;

      // make sure new scroll value is within new domain
      if( mScrollPrePosition.x < min.x
          || mScrollPrePosition.x > max.x )
      {
        scrollPositionChanged = true;
        mScrollPrePosition.x = Clamp(mScrollPrePosition.x, -(max.x - size.x), -min.x);
      }
    }
    if( (fabsf(rulerDomain.max - rulerDomain.min) - size.x) > Math::MACHINE_EPSILON_100 )
    {
      canScrollHorizontal = true;
    }
  }
  else if( fabs(min.x) > Math::MACHINE_EPSILON_100
           || fabs(max.x) > Math::MACHINE_EPSILON_100 )
  {
    // need to reset to 0
    domainChanged = true;
    min.x = 0.0f;
    max.x = 0.0f;
    canScrollHorizontal = false;
  }

  if(mRulerY->IsEnabled())
  {
    const Toolkit::RulerDomain& rulerDomain = mRulerY->GetDomain();
    if( fabsf(min.y - rulerDomain.min) > Math::MACHINE_EPSILON_100
        || fabsf(max.y - rulerDomain.max) > Math::MACHINE_EPSILON_100 )
    {
      domainChanged = true;
      min.y = rulerDomain.min;
      max.y = rulerDomain.max;

      // make sure new scroll value is within new domain
      if( mScrollPrePosition.y < min.y
          || mScrollPrePosition.y > max.y )
      {
        scrollPositionChanged = true;
        mScrollPrePosition.y = Clamp(mScrollPrePosition.y, -(max.y - size.y), -min.y);
      }
    }
    if( (fabsf(rulerDomain.max - rulerDomain.min) - size.y) > Math::MACHINE_EPSILON_100 )
    {
      canScrollVertical = true;
    }
  }
  else if( fabs(min.y) > Math::MACHINE_EPSILON_100
           || fabs(max.y) > Math::MACHINE_EPSILON_100 )
  {
    // need to reset to 0
    domainChanged = true;
    min.y = 0.0f;
    max.y = 0.0f;
    canScrollVertical = false;
  }

  // avoid setting properties if possible, otherwise this will cause an entire update as well as triggering constraints using each property we update
  if( mCanScrollVertical != canScrollVertical )
  {
    mCanScrollVertical = canScrollVertical;
    self.SetProperty(mPropertyCanScrollVertical, canScrollVertical);
  }
  if( mCanScrollHorizontal != canScrollHorizontal )
  {
    mCanScrollHorizontal = canScrollHorizontal;
    self.SetProperty(mPropertyCanScrollHorizontal, canScrollHorizontal);
  }
  if( scrollPositionChanged )
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Domain Changed, setting mPropertyPrePosition To[%.2f, %.2f]", this, mScrollPrePosition.x, mScrollPrePosition.y );
    self.SetProperty(mPropertyPrePosition, mScrollPrePosition);
  }
  if( domainChanged )
  {
    mMinScroll = min;
    mMaxScroll = max;
    self.SetProperty(mPropertyPositionMin, mMinScroll );
    self.SetProperty(mPropertyPositionMax, mMaxScroll );
  }
}

void ScrollView::SetScrollSensitive(bool sensitive)
{
  Actor self = Self();
  PanGestureDetector panGesture( GetPanGestureDetector() );

  DALI_LOG_SCROLL_STATE("[0x%X] sensitive: before:[%d] setting[%d]", this, int(mSensitive), int(sensitive));

  if((!mSensitive) && (sensitive))
  {
    mSensitive = sensitive;
    panGesture.Attach(self);
  }
  else if((mSensitive) && (!sensitive))
  {
    DALI_LOG_SCROLL_STATE("[0x%X] BEFORE: panning:[%d]", this, int(mPanning));

    // while the scroll view is panning, the state needs to be reset.
    if ( mPanning )
    {
      PanGesture cancelGesture( Gesture::Cancelled );
      OnPan( cancelGesture );
    }

    panGesture.Detach(self);
    mSensitive = sensitive;

    mGestureStackDepth = 0;
    DALI_LOG_SCROLL_STATE("[0x%X] AFTER: panning:[%d]", this, int(mPanning));
  }
}

void ScrollView::SetMaxOvershoot(float overshootX, float overshootY)
{
  mMaxOvershoot.x = overshootX;
  mMaxOvershoot.y = overshootY;
  mUserMaxOvershoot = mMaxOvershoot;
  mDefaultMaxOvershoot = false;
  UpdateMainInternalConstraint();
}

void ScrollView::SetSnapOvershootAlphaFunction(AlphaFunction alpha)
{
  mSnapOvershootAlphaFunction = alpha;
}

void ScrollView::SetSnapOvershootDuration(float duration)
{
  mSnapOvershootDuration = duration;
}

void ScrollView::SetTouchesRequiredForPanning(unsigned int minTouches, unsigned int maxTouches, bool endOutside)
{
  PanGestureDetector panGesture( GetPanGestureDetector() );

  mMinTouchesForPanning = minTouches;
  mMaxTouchesForPanning = maxTouches;

  if(endOutside)
  {
    panGesture.SetMinimumTouchesRequired(minTouches);
    panGesture.SetMaximumTouchesRequired(maxTouches);
  }
  else
  {
    panGesture.SetMinimumTouchesRequired(1);
    panGesture.SetMaximumTouchesRequired(UINT_MAX);
  }
}

void ScrollView::SetActorAutoSnap(bool enable)
{
  mActorAutoSnapEnabled = enable;
}

void ScrollView::SetAutoResize(bool enable)
{
  mAutoResizeContainerEnabled = enable;
  // TODO: This needs a lot of issues to be addressed before working.
}

bool ScrollView::GetWrapMode() const
{
  return mWrapMode;
}

void ScrollView::SetWrapMode(bool enable)
{
  mWrapMode = enable;
  Self().SetProperty(mPropertyWrap, enable);
}

int ScrollView::GetScrollUpdateDistance() const
{
  return mScrollUpdateDistance;
}

void ScrollView::SetScrollUpdateDistance(int distance)
{
  mScrollUpdateDistance = distance;
}

bool ScrollView::GetAxisAutoLock() const
{
  return mAxisAutoLock;
}

void ScrollView::SetAxisAutoLock(bool enable)
{
  mAxisAutoLock = enable;
  UpdateMainInternalConstraint();
}

float ScrollView::GetAxisAutoLockGradient() const
{
  return mAxisAutoLockGradient;
}

void ScrollView::SetAxisAutoLockGradient(float gradient)
{
  DALI_ASSERT_DEBUG( gradient >= 0.0f && gradient <= 1.0f );
  mAxisAutoLockGradient = gradient;
  UpdateMainInternalConstraint();
}

float ScrollView::GetFrictionCoefficient() const
{
  return mFrictionCoefficient;
}

void ScrollView::SetFrictionCoefficient(float friction)
{
  DALI_ASSERT_DEBUG( friction > 0.0f );
  mFrictionCoefficient = friction;
}

float ScrollView::GetFlickSpeedCoefficient() const
{
  return mFlickSpeedCoefficient;
}

void ScrollView::SetFlickSpeedCoefficient(float speed)
{
  mFlickSpeedCoefficient = speed;
}

Vector2 ScrollView::GetMinimumDistanceForFlick() const
{
  return mMinFlickDistance;
}

void ScrollView::SetMinimumDistanceForFlick( const Vector2& distance )
{
  mMinFlickDistance = distance;
}

float ScrollView::GetMinimumSpeedForFlick() const
{
  return mFlickSpeedThreshold;
}

void ScrollView::SetMinimumSpeedForFlick( float speed )
{
  mFlickSpeedThreshold = speed;
}

float ScrollView::GetMaxFlickSpeed() const
{
  return mMaxFlickSpeed;
}

void ScrollView::SetMaxFlickSpeed(float speed)
{
  mMaxFlickSpeed = speed;
}

void ScrollView::SetMouseWheelScrollDistanceStep(Vector2 step)
{
  mMouseWheelScrollDistanceStep = step;
}

Vector2 ScrollView::GetMouseWheelScrollDistanceStep() const
{
  return mMouseWheelScrollDistanceStep;
}

unsigned int ScrollView::GetCurrentPage() const
{
  // in case animation is currently taking place.
  Vector3 position = GetPropertyPosition();

  Actor self = Self();
  unsigned int page = 0;
  unsigned int pagesPerVolume = 1;
  unsigned int volume = 0;

  // if rulerX is enabled, then get page count (columns)
  page = mRulerX->GetPageFromPosition(-position.x, mWrapMode);
  volume = mRulerY->GetPageFromPosition(-position.y, mWrapMode);
  pagesPerVolume = mRulerX->GetTotalPages();

  return volume * pagesPerVolume + page;
}

Vector3 ScrollView::GetCurrentScrollPosition() const
{
  return -GetPropertyPosition();
}

void ScrollView::SetScrollPosition(const Vector3& position)
{
  mScrollPrePosition = position;
}

Vector3 ScrollView::GetDomainSize() const
{
  Vector3 size = Self().GetCurrentSize();

  const RulerDomain& xDomain = GetRulerX()->GetDomain();
  const RulerDomain& yDomain = GetRulerY()->GetDomain();

  Vector3 domainSize = Vector3( xDomain.max - xDomain.min, yDomain.max - yDomain.min, 0.0f ) - size;
  return domainSize;
}

void ScrollView::TransformTo(const Vector3& position,
                             DirectionBias horizontalBias, DirectionBias verticalBias)
{
  TransformTo(position, mSnapDuration, mSnapAlphaFunction, horizontalBias, verticalBias);
}

void ScrollView::TransformTo(const Vector3& position, float duration, AlphaFunction alpha,
                             DirectionBias horizontalBias, DirectionBias verticalBias)
{
  Actor self( Self() );

  // Guard against destruction during signal emission
  // Note that Emit() methods are called indirectly e.g. from within ScrollView::AnimateTo()
  Toolkit::ScrollView handle( GetOwner() );

  DALI_LOG_SCROLL_STATE("[0x%X] pos[%.2f,%.2f], duration[%.2f] bias[%d, %d]",
    this, position.x, position.y, duration, int(horizontalBias), int(verticalBias));

  Vector3 currentScrollPosition = GetCurrentScrollPosition();
  self.SetProperty( mPropertyScrollStartPagePosition, currentScrollPosition );

  if( mScrolling ) // are we interrupting a current scroll?
  {
    // set mScrolling to false, in case user has code that interrogates mScrolling Getter() in complete.
    mScrolling = false;
    DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignalV2 1 [%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y);
    mScrollCompletedSignalV2.Emit( currentScrollPosition );
  }

  if( mPanning ) // are we interrupting a current pan?
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Interrupting Pan, set to false", this );
    mPanning = false;
    mGestureStackDepth = 0;
    self.SetProperty( mPropertyPanning, false );

    if( mScrollMainInternalPrePositionConstraint )
    {
      self.RemoveConstraint(mScrollMainInternalPrePositionConstraint);
    }
  }

  self.SetProperty(mPropertyScrolling, true);
  mScrolling = true;

  DALI_LOG_SCROLL_STATE("[0x%X] mScrollStartedSignalV2 1 [%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y);
  mScrollStartedSignalV2.Emit( currentScrollPosition );
  bool animating = AnimateTo(-position,
                             Vector3::ONE * duration,
                             alpha,
                             true,
                             horizontalBias,
                             verticalBias,
                             Snap);

  if(!animating)
  {
    // if not animating, then this pan has completed right now.
    self.SetProperty(mPropertyScrolling, false);
    mScrolling = false;

    // If we have no duration, then in the next update frame, we will be at the position specified as we just set.
    // In this scenario, we cannot return the currentScrollPosition as this is out-of-date and should instead return the requested final position
    Vector3 completedPosition( currentScrollPosition );
    if( duration <= Math::MACHINE_EPSILON_10 )
    {
      completedPosition = position;
    }

    DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignalV2 2 [%.2f, %.2f]", this, completedPosition.x, completedPosition.y);
    SetScrollUpdateNotification(false);
    mScrollCompletedSignalV2.Emit( completedPosition );
  }
}

void ScrollView::ScrollTo(const Vector3& position)
{
  ScrollTo(position, mSnapDuration );
}

void ScrollView::ScrollTo(const Vector3& position, float duration)
{
  ScrollTo(position, duration, DirectionBiasNone, DirectionBiasNone);
}

void ScrollView::ScrollTo(const Vector3& position, float duration, AlphaFunction alpha)
{
  ScrollTo(position, duration, alpha, DirectionBiasNone, DirectionBiasNone);
}

void ScrollView::ScrollTo(const Vector3& position, float duration,
                          DirectionBias horizontalBias, DirectionBias verticalBias)
{
  ScrollTo(position, duration, mSnapAlphaFunction, horizontalBias, verticalBias);
}

void ScrollView::ScrollTo(const Vector3& position, float duration, AlphaFunction alpha,
                DirectionBias horizontalBias, DirectionBias verticalBias)
{
  DALI_LOG_SCROLL_STATE("[0x%X] position[%.2f, %.2f] duration[%.2f]", this, position.x, position.y, duration, int(horizontalBias), int(verticalBias));
  TransformTo(position, duration, alpha, horizontalBias, verticalBias);
}

void ScrollView::ScrollTo(unsigned int page)
{
  ScrollTo(page, mSnapDuration);
}

void ScrollView::ScrollTo(unsigned int page, float duration, DirectionBias bias)
{
  Vector3 position;
  unsigned int volume;
  unsigned int libraries;

  // The position to scroll to is continuous and linear
  // unless a domain has been enabled on the X axis.
  // or if WrapMode has been enabled.
  bool carryX = mRulerX->GetDomain().enabled | mWrapMode;
  bool carryY = mRulerY->GetDomain().enabled | mWrapMode;

  position.x = mRulerX->GetPositionFromPage(page, volume, carryX);
  position.y = mRulerY->GetPositionFromPage(volume, libraries, carryY);

  ScrollTo(position, duration, bias, bias);
}

void ScrollView::ScrollTo(Actor &actor)
{
  ScrollTo(actor, mSnapDuration);
}

void ScrollView::ScrollTo(Actor &actor, float duration)
{
  DALI_ASSERT_ALWAYS(actor.GetParent() == Self());

  Actor self = Self();
  Vector3 size = self.GetCurrentSize();
  Vector3 position = actor.GetCurrentPosition();
  position -= GetPropertyPrePosition();

  ScrollTo(Vector3(position.x - size.width * 0.5f, position.y - size.height * 0.5f, 0.0f), duration);
}

Actor ScrollView::FindClosestActor()
{
  Actor self = Self();
  Vector3 size = self.GetCurrentSize();

  return FindClosestActorToPosition(Vector3(size.width * 0.5f,size.height * 0.5f,0.0f));
}

Actor ScrollView::FindClosestActorToPosition(const Vector3& position, FindDirection dirX, FindDirection dirY, FindDirection dirZ)
{
  Actor closestChild;
  float closestDistance2 = 0.0f;
  Vector3 actualPosition = position;

  unsigned int numChildren = Self().GetChildCount();

  for(unsigned int i = 0; i < numChildren; ++i)
  {
    Actor child = Self().GetChildAt(i);

    if(mInternalActor == child) // ignore internal actor.
    {
      continue;
    }

    Vector3 childPosition = GetPositionOfAnchor(child, AnchorPoint::CENTER);

    Vector3 delta = childPosition - actualPosition;

    // X-axis checking (only find Actors to the [dirX] of actualPosition)
    if(dirX > All) // != All,None
    {
      FindDirection deltaH = delta.x > 0 ? Right : Left;
      if(dirX != deltaH)
      {
        continue;
      }
    }

    // Y-axis checking (only find Actors to the [dirY] of actualPosition)
    if(dirY > All) // != All,None
    {
      FindDirection deltaV = delta.y > 0 ? Down : Up;
      if(dirY  != deltaV)
      {
        continue;
      }
    }

    // Z-axis checking (only find Actors to the [dirZ] of actualPosition)
    if(dirZ > All) // != All,None
    {
      FindDirection deltaV = delta.y > 0 ? In : Out;
      if(dirZ  != deltaV)
      {
        continue;
      }
    }

    // compare child to closest child in terms of distance.
    float distance2 = 0.0f;

    // distance2 = the Square of the relevant dimensions of delta
    if(dirX != None)
    {
      distance2 += delta.x * delta.x;
    }

    if(dirY != None)
    {
      distance2 += delta.y * delta.y;
    }

    if(dirZ != None)
    {
      distance2 += delta.z * delta.z;
    }

    if(closestChild) // Next time.
    {
      if(distance2 < closestDistance2)
      {
        closestChild = child;
        closestDistance2 = distance2;
      }
    }
    else // First time.
    {
      closestChild = child;
      closestDistance2 = distance2;
    }
  }

  return closestChild;
}

bool ScrollView::ScrollToSnapPoint()
{
  DALI_LOG_SCROLL_STATE("[0x%X]", this );
  Vector2 stationaryVelocity = Vector2(0.0f, 0.0f);
  return SnapWithVelocity( stationaryVelocity );
}

// TODO: In situations where axes are different (X snap, Y free)
// Each axis should really have their own independent animation (time and equation)
// Consider, X axis snapping to nearest grid point (EaseOut over fixed time)
// Consider, Y axis simulating physics to arrive at a point (Physics equation over variable time)
// Currently, the axes have been split however, they both use the same EaseOut equation.
bool ScrollView::SnapWithVelocity(Vector2 velocity)
{
  // Animator takes over now, touches are assumed not to interfere.
  // And if touches do interfere, then we'll stop animation, update PrePosition
  // to current mScroll's properties, and then resume.
  // Note: For Flicking this may work a bit different...

  float angle = atan2(velocity.y, velocity.x);
  float speed2 = velocity.LengthSquared();
  AlphaFunction alphaFunction = mSnapAlphaFunction;
  Vector3 positionDuration = Vector3::ONE * mSnapDuration;
  float biasX = 0.5f;
  float biasY = 0.5f;
  FindDirection horizontal = None;
  FindDirection vertical = None;

  // orthoAngleRange = Angle tolerance within the Exact N,E,S,W direction
  // that will be accepted as a general N,E,S,W flick direction.

  const float orthoAngleRange = FLICK_ORTHO_ANGLE_RANGE * M_PI / 180.0f;
  const float flickSpeedThreshold2 = mFlickSpeedThreshold * mFlickSpeedThreshold;

  Vector3 positionSnap = mScrollPrePosition;

  // Flick logic X Axis

  if(mRulerX->IsEnabled() && mLockAxis != LockHorizontal)
  {
    horizontal = All;

    if( speed2 > flickSpeedThreshold2 || // exceeds flick threshold
        mInAccessibilityPan ) // With AccessibilityPan its easier to move between snap positions
    {
      if((angle >= -orthoAngleRange) && (angle < orthoAngleRange)) // Swiping East
      {
        biasX = 0.0f, horizontal = Left;

        // This guards against an error where no movement occurs, due to the flick finishing
        // before the update-thread has advanced mScrollPostPosition past the the previous snap point.
        positionSnap.x += 1.0f;
      }
      else if((angle >= M_PI-orthoAngleRange) || (angle < -M_PI+orthoAngleRange)) // Swiping West
      {
        biasX = 1.0f, horizontal = Right;

        // This guards against an error where no movement occurs, due to the flick finishing
        // before the update-thread has advanced mScrollPostPosition past the the previous snap point.
        positionSnap.x -= 1.0f;
      }
    }
  }

  // Flick logic Y Axis

  if(mRulerY->IsEnabled() && mLockAxis != LockVertical)
  {
    vertical = All;

    if( speed2 > flickSpeedThreshold2 || // exceeds flick threshold
        mInAccessibilityPan ) // With AccessibilityPan its easier to move between snap positions
    {
      if((angle >= M_PI_2-orthoAngleRange) && (angle < M_PI_2+orthoAngleRange)) // Swiping South
      {
        biasY = 0.0f, vertical = Up;
      }
      else if((angle >= -M_PI_2-orthoAngleRange) && (angle < -M_PI_2+orthoAngleRange)) // Swiping North
      {
        biasY = 1.0f, vertical = Down;
      }
    }
  }

  // isFlick: Whether this gesture is a flick or not.
  bool isFlick = (horizontal != All || vertical != All);
  // isFreeFlick: Whether this gesture is a flick under free panning criteria.
  bool isFreeFlick = velocity.LengthSquared() > (FREE_FLICK_SPEED_THRESHOLD*FREE_FLICK_SPEED_THRESHOLD);

  if(isFlick || isFreeFlick)
  {
    positionDuration = Vector3::ONE * mFlickDuration;
    alphaFunction = mFlickAlphaFunction;
  }

  // Calculate next positionSnap ////////////////////////////////////////////////////////////

  if(mActorAutoSnapEnabled)
  {
    Vector3 size = Self().GetCurrentSize();

    Actor child = FindClosestActorToPosition( Vector3(size.width * 0.5f,size.height * 0.5f,0.0f), horizontal, vertical );

    if(!child && isFlick )
    {
      // If we conducted a direction limited search and found no actor, then just snap to the closest actor.
      child = FindClosestActorToPosition( Vector3(size.width * 0.5f,size.height * 0.5f,0.0f) );
    }

    if(child)
    {
      Vector3 position = Self().GetProperty<Vector3>(mPropertyPosition);

      // Get center-point of the Actor.
      Vector3 childPosition = GetPositionOfAnchor(child, AnchorPoint::CENTER);

      if(mRulerX->IsEnabled())
      {
        positionSnap.x = position.x - childPosition.x + size.width * 0.5f;
      }
      if(mRulerY->IsEnabled())
      {
        positionSnap.y = position.y - childPosition.y + size.height * 0.5f;
      }
    }
  }

  Vector3 startPosition = positionSnap;
  positionSnap.x = -mRulerX->Snap(-positionSnap.x, biasX);  // NOTE: X & Y rulers think in -ve coordinate system.
  positionSnap.y = -mRulerY->Snap(-positionSnap.y, biasY);  // That is scrolling RIGHT (e.g. 100.0, 0.0) means moving LEFT.

  Vector3 clampDelta(Vector3::ZERO);
  ClampPosition(positionSnap);

  if( (mRulerX->GetType() == Ruler::Free || mRulerY->GetType() == Ruler::Free)
      && isFreeFlick && !mActorAutoSnapEnabled)
  {
    // Calculate target position based on velocity of flick.

    // a = Deceleration (Set to diagonal stage length * friction coefficient)
    // u = Initial Velocity (Flick velocity)
    // v = 0 (Final Velocity)
    // t = Time (Velocity / Deceleration)
    Vector2 stageSize = Stage::GetCurrent().GetSize();
    float stageLength = Vector3(stageSize.x, stageSize.y, 0.0f).Length();
    float a = (stageLength * mFrictionCoefficient);
    Vector3 u = Vector3(velocity.x, velocity.y, 0.0f) * mFlickSpeedCoefficient;
    float speed = u.Length();
    u/= speed;

    // TODO: Change this to a decay function. (faster you flick, the slower it should be)
    speed = std::min(speed, stageLength * mMaxFlickSpeed );
    u*= speed;
    alphaFunction = ConstantDecelerationAlphaFunction;

    float t = speed / a;

    if(mRulerX->IsEnabled() && mRulerX->GetType() == Ruler::Free)
    {
      positionSnap.x += t*u.x*0.5f;
    }

    if(mRulerY->IsEnabled() && mRulerY->GetType() == Ruler::Free)
    {
      positionSnap.y += t*u.y*0.5f;
    }

    clampDelta = positionSnap;
    ClampPosition(positionSnap);
    if((positionSnap - startPosition).LengthSquared() > Math::MACHINE_EPSILON_0)
    {
      clampDelta -= positionSnap;
      clampDelta.x = clampDelta.x > 0.0f ? std::min(clampDelta.x, mMaxOvershoot.x) : std::max(clampDelta.x, -mMaxOvershoot.x);
      clampDelta.y = clampDelta.y > 0.0f ? std::min(clampDelta.y, mMaxOvershoot.y) : std::max(clampDelta.y, -mMaxOvershoot.y);
    }
    else
    {
      clampDelta = Vector3::ZERO;
    }

    // If Axis is Free and has velocity, then calculate time taken
    // to reach target based on velocity in axis.
    if(mRulerX->IsEnabled() && mRulerX->GetType() == Ruler::Free)
    {
      float deltaX = fabsf(startPosition.x - positionSnap.x);

      if(fabsf(u.x) > Math::MACHINE_EPSILON_1)
      {
        positionDuration.x = fabsf(deltaX / u.x);
      }
      else
      {
        positionDuration.x = 0;
      }
    }

    if(mRulerY->IsEnabled() && mRulerY->GetType() == Ruler::Free)
    {
      float deltaY = fabsf(startPosition.y - positionSnap.y);

      if(fabsf(u.y) > Math::MACHINE_EPSILON_1)
      {
        positionDuration.y = fabsf(deltaY / u.y);
      }
      else
      {
        positionDuration.y = 0;
      }
    }
  }
  positionSnap += clampDelta;

  bool animating = AnimateTo(positionSnap, positionDuration,
                             alphaFunction, false,
                             DirectionBiasNone, DirectionBiasNone,
                             isFlick || isFreeFlick ? Flick : Snap);

  return animating;
}

void ScrollView::StopAnimation(void)
{
  // Clear Snap animation if exists.
  StopAnimation(mInternalXAnimation);
  StopAnimation(mInternalYAnimation);
  mScrollStateFlags = 0;
  // remove scroll animation flags
  HandleStoppedAnimation();
}

void ScrollView::StopAnimation(Animation& animation)
{
  if(animation)
  {
    animation.Stop();
    animation.Reset();
  }
}

bool ScrollView::AnimateTo(const Vector3& position, const Vector3& positionDuration,
                           AlphaFunction alpha, bool findShortcuts,
                           DirectionBias horizontalBias, DirectionBias verticalBias,
                           SnapType snapType)
{
  // Here we perform an animation on a number of properties (depending on which have changed)
  // The animation is applied to all ScrollBases
  Actor self = Self();
  mScrollTargetPosition = position;
  float totalDuration = 0.0f;

  bool positionChanged = (mScrollTargetPosition != mScrollPostPosition);

  if(positionChanged)
  {
    totalDuration = std::max(totalDuration, positionDuration.x);
    totalDuration = std::max(totalDuration, positionDuration.y);
  }
  else
  {
    // try to animate for a frame, on some occasions update will be changing scroll value while event side thinks it hasnt changed
    totalDuration = 0.01f;
    positionChanged = true;
  }

  StopAnimation();

  // Position Delta ///////////////////////////////////////////////////////
  if(positionChanged)
  {
    if(mWrapMode && findShortcuts)
    {
      // In Wrap Mode, the shortest distance is a little less intuitive...
      const RulerDomain rulerDomainX = mRulerX->GetDomain();
      const RulerDomain rulerDomainY = mRulerY->GetDomain();

      if(mRulerX->IsEnabled())
      {
        float dir = VectorInDomain(-mScrollPrePosition.x, -mScrollTargetPosition.x, rulerDomainX.min, rulerDomainX.max, horizontalBias);
        mScrollTargetPosition.x = mScrollPrePosition.x + -dir;
      }

      if(mRulerY->IsEnabled())
      {
        float dir = VectorInDomain(-mScrollPrePosition.y, -mScrollTargetPosition.y, rulerDomainY.min, rulerDomainY.max, verticalBias);
        mScrollTargetPosition.y = mScrollPrePosition.y + -dir;
      }
    }

    // note we have two separate animations for X & Y, this deals with sliding diagonally and hitting
    // a horizonal/vertical wall.delay
    AnimateInternalXTo(mScrollTargetPosition.x, positionDuration.x, alpha);
    AnimateInternalYTo(mScrollTargetPosition.y, positionDuration.y, alpha);

    if( !(mScrollStateFlags & SCROLL_ANIMATION_FLAGS) )
    {
      DALI_LOG_SCROLL_STATE("[0x%X] Setting mPropertyPrePosition To[%.2f, %.2f]", this, mScrollTargetPosition.x, mScrollTargetPosition.y );
      self.SetProperty(mPropertyPrePosition, mScrollTargetPosition);
      mScrollPrePosition = mScrollTargetPosition;
      mScrollPostPosition = mScrollTargetPosition;
      WrapPosition(mScrollPostPosition);
    }

    DALI_LOG_SCROLL_STATE("[0x%X] position-changed, mScrollTargetPosition[%.2f, %.2f], mScrollPrePosition[%.2f, %.2f], mScrollPostPosition[%.2f, %.2f]", this, mScrollTargetPosition.x, mScrollTargetPosition.y, mScrollPrePosition.x, mScrollPrePosition.y, mScrollPostPosition.x, mScrollPostPosition.y );
    DALI_LOG_SCROLL_STATE("[0x%X] mPropertyPrePosition[%.2f, %.2f], mPropertyPosition[%.2f, %.2f]", this, self.GetProperty( mPropertyPrePosition ).Get<Vector3>().x, self.GetProperty( mPropertyPrePosition ).Get<Vector3>().y, self.GetProperty( mPropertyPosition ).Get<Vector3>().x, self.GetProperty( mPropertyPosition ).Get<Vector3>().y );
  }

  SetScrollUpdateNotification(true);

  // Always send a snap event when AnimateTo is called.
  Toolkit::ScrollView::SnapEvent snapEvent;
  snapEvent.type = snapType;
  snapEvent.position = -mScrollTargetPosition;
  snapEvent.duration = totalDuration;

  DALI_LOG_SCROLL_STATE("[0x%X] mSnapStartedSignalV2 [%.2f, %.2f]", this, snapEvent.position.x, snapEvent.position.y);
  mSnapStartedSignalV2.Emit( snapEvent );

  return (mScrollStateFlags & SCROLL_ANIMATION_FLAGS) != 0;
}

void ScrollView::SetOvershootEnabled(bool enabled)
{
  if(enabled && !mOvershootIndicator)
  {
    mOvershootIndicator = ScrollOvershootIndicator::New();
  }
  if( enabled )
  {
    mMaxOvershoot = OVERSCROLL_CLAMP;
    mOvershootIndicator->AttachToScrollable(*this);
  }
  else
  {
    mMaxOvershoot = mUserMaxOvershoot;
    mOvershootIndicator->DetachFromScrollable(*this);
  }
  UpdateMainInternalConstraint();
}

void ScrollView::AddOverlay(Actor actor)
{
  mInternalActor.Add( actor );
}

void ScrollView::RemoveOverlay(Actor actor)
{
  mInternalActor.Remove( actor );
}

void ScrollView::SetOvershootEffectColor( const Vector4& color )
{
  mOvershootEffectColor = color;
  if( mOvershootIndicator )
  {
    mOvershootIndicator->SetOvershootEffectColor( color );
  }
}

void ScrollView::SetScrollingDirection( Radian direction, Radian threshold )
{
  PanGestureDetector panGesture( GetPanGestureDetector() );

  // First remove just in case we have some set, then add.
  panGesture.RemoveDirection( direction );
  panGesture.AddDirection( direction, threshold );
}

void ScrollView::RemoveScrollingDirection( Radian direction )
{
  PanGestureDetector panGesture( GetPanGestureDetector() );
  panGesture.RemoveDirection( direction );
}

Toolkit::ScrollView::SnapStartedSignalV2& ScrollView::SnapStartedSignal()
{
  return mSnapStartedSignalV2;
}

void ScrollView::FindAndUnbindActor(Actor child)
{
  UnbindActor(child);
}

Vector3 ScrollView::GetPropertyPrePosition() const
{
  Vector3 position = Self().GetProperty<Vector3>(mPropertyPrePosition);
  WrapPosition(position);
  return position;
}

Vector3 ScrollView::GetPropertyPosition() const
{
  Vector3 position = Self().GetProperty<Vector3>(mPropertyPosition);
  WrapPosition(position);

  return position;
}

void ScrollView::HandleStoppedAnimation()
{
  SetScrollUpdateNotification(false);
}

void ScrollView::HandleSnapAnimationFinished()
{
  // Emit Signal that scrolling has completed.
  mScrolling = false;
  Actor self = Self();
  self.SetProperty(mPropertyScrolling, false);

  Vector3 deltaPosition(mScrollPrePosition);

  UpdateLocalScrollProperties();
  WrapPosition(mScrollPrePosition);
  DALI_LOG_SCROLL_STATE("[0x%X] Setting mPropertyPrePosition To[%.2f, %.2f]", this, mScrollPrePosition.x, mScrollPrePosition.y );
  self.SetProperty(mPropertyPrePosition, mScrollPrePosition);

  Vector3 currentScrollPosition = GetCurrentScrollPosition();
  DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignalV2 3 current[%.2f, %.2f], mScrollTargetPosition[%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y, -mScrollTargetPosition.x, -mScrollTargetPosition.y );
  mScrollCompletedSignalV2.Emit( currentScrollPosition );

  mDomainOffset += deltaPosition - mScrollPostPosition;
  self.SetProperty(mPropertyDomainOffset, mDomainOffset);
  HandleStoppedAnimation();
}

void ScrollView::SetScrollUpdateNotification( bool enabled )
{
  Actor self = Self();
  if( mScrollXUpdateNotification )
  {
    // disconnect now to avoid a notification before removed from update thread
    mScrollXUpdateNotification.NotifySignal().Disconnect(this, &ScrollView::OnScrollUpdateNotification);
    self.RemovePropertyNotification(mScrollXUpdateNotification);
    mScrollXUpdateNotification.Reset();
  }
  if( enabled )
  {
    mScrollXUpdateNotification = self.AddPropertyNotification(mPropertyPosition, 0, StepCondition(mScrollUpdateDistance, 0.0f));
    mScrollXUpdateNotification.NotifySignal().Connect( this, &ScrollView::OnScrollUpdateNotification );
  }
  if( mScrollYUpdateNotification )
  {
    // disconnect now to avoid a notification before removed from update thread
    mScrollYUpdateNotification.NotifySignal().Disconnect(this, &ScrollView::OnScrollUpdateNotification);
    self.RemovePropertyNotification(mScrollYUpdateNotification);
    mScrollYUpdateNotification.Reset();
  }
  if( enabled )
  {
    mScrollYUpdateNotification = self.AddPropertyNotification(mPropertyPosition, 1, StepCondition(mScrollUpdateDistance, 0.0f));
    mScrollYUpdateNotification.NotifySignal().Connect( this, &ScrollView::OnScrollUpdateNotification );
  }
}

void ScrollView::OnScrollUpdateNotification(Dali::PropertyNotification& source)
{
  // Guard against destruction during signal emission
  Toolkit::ScrollView handle( GetOwner() );

  Vector3 currentScrollPosition = GetCurrentScrollPosition();
  mScrollUpdatedSignalV2.Emit( currentScrollPosition );
}

bool ScrollView::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::ScrollView view = Toolkit::ScrollView::DownCast( handle );

  if( Toolkit::ScrollView::SIGNAL_SNAP_STARTED == signalName )
  {
    view.SnapStartedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void ScrollView::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  // need to update domain properties for new size
  UpdatePropertyDomain(targetSize);
}

void ScrollView::OnControlSizeSet( const Vector3& size )
{
  // need to update domain properties for new size
  if( mDefaultMaxOvershoot )
  {
    mUserMaxOvershoot.x = size.x * 0.5f;
    mUserMaxOvershoot.y = size.y * 0.5f;
    if( !IsScrollComponentEnabled(Toolkit::Scrollable::OvershootIndicator) )
    {
      mMaxOvershoot = mUserMaxOvershoot;
    }
  }
  UpdatePropertyDomain(size);
  UpdateMainInternalConstraint();
  if( IsScrollComponentEnabled(Toolkit::Scrollable::OvershootIndicator) )
  {
    mOvershootIndicator->Reset();
  }
}

void ScrollView::OnChildAdd(Actor& child)
{
  if(mAlterChild)
  {
    BindActor(child);
  }
}

void ScrollView::OnChildRemove(Actor& child)
{
  // TODO: Actor needs a RemoveConstraint method to take out an individual constraint.
  UnbindActor(child);
}

void ScrollView::OnPropertySet( Property::Index index, Property::Value propertyValue )
{
  Actor self = Self();
  if( index == mPropertyPrePosition )
  {
    DALI_LOG_SCROLL_STATE("[0x%X]: mPropertyPrePosition[%.2f, %.2f]", this, propertyValue.Get<Vector3>().x, propertyValue.Get<Vector3>().y);
    propertyValue.Get(mScrollPrePosition);
  }
}

void ScrollView::StartTouchDownTimer()
{
  if ( !mTouchDownTimer )
  {
    mTouchDownTimer = Timer::New( TOUCH_DOWN_TIMER_INTERVAL );
    mTouchDownTimer.TickSignal().Connect( this, &ScrollView::OnTouchDownTimeout );
  }

  mTouchDownTimer.Start();
}

void ScrollView::StopTouchDownTimer()
{
  if ( mTouchDownTimer )
  {
    mTouchDownTimer.Stop();
  }
}

bool ScrollView::OnTouchDownTimeout()
{
  DALI_LOG_SCROLL_STATE("[0x%X]", this);

  mTouchDownTimeoutReached = true;

  unsigned int currentScrollStateFlags( mScrollStateFlags ); // Cleared in StopAnimation so keep local copy for comparison
  if( currentScrollStateFlags & (SCROLL_ANIMATION_FLAGS | SNAP_ANIMATION_FLAGS) )
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Scrolling Or snapping flags set, stopping animation", this);

    StopAnimation();
    if( currentScrollStateFlags & SCROLL_ANIMATION_FLAGS )
    {
      DALI_LOG_SCROLL_STATE("[0x%X] Scrolling flags set, emitting signal", this);

      mScrollInterrupted = true;
      // reset domain offset as scrolling from original plane.
      mDomainOffset = Vector3::ZERO;
      Self().SetProperty(mPropertyDomainOffset, Vector3::ZERO);

      UpdateLocalScrollProperties();
      Vector3 currentScrollPosition = GetCurrentScrollPosition();
      DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignalV2 4 [%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y);
      mScrollCompletedSignalV2.Emit( currentScrollPosition );
    }
  }

  return false;
}

bool ScrollView::OnTouchEvent(const TouchEvent& event)
{
  if(!mSensitive)
  {
    DALI_LOG_SCROLL_STATE("[0x%X], Not Sensitive, ignoring", this);

    // Ignore this touch event, if scrollview is insensitive.
    return false;
  }

  // Ignore events with multiple-touch points
  if (event.GetPointCount() != 1)
  {
    DALI_LOG_SCROLL_STATE("[0x%X], multiple touch, ignoring", this);

    return false;
  }

  const TouchPoint::State pointState = event.GetPoint(0).state;
  if( pointState == TouchPoint::Down )
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Down", this);

    if(mGestureStackDepth==0)
    {
      mTouchDownTime = event.time;

      // This allows time for a pan-gesture to start, to avoid breaking snap-animation behavior with fast flicks.
      // If touch-down does not become a pan (after timeout interval), then snap-animation can be interrupted.
      mTouchDownTimeoutReached = false;
      mScrollInterrupted = false;
      StartTouchDownTimer();
    }
  }
  else if( ( pointState == TouchPoint::Up ) ||
           ( ( pointState == TouchPoint::Interrupted ) && ( event.GetPoint(0).hitActor == Self() ) ) )
  {
    DALI_LOG_SCROLL_STATE("[0x%X] %s", this, ( ( pointState == TouchPoint::Up ) ? "Up" : "Interrupted" ) );

    StopTouchDownTimer();

    // if the user touches and releases without enough movement to go
    // into a gesture state, then we should snap to nearest point.
    // otherwise our scroll could be stopped (interrupted) half way through an animation.
    if(mGestureStackDepth==0 && mTouchDownTimeoutReached)
    {
      if( ( event.GetPoint(0).state == TouchPoint::Interrupted ) ||
          ( ( event.time - mTouchDownTime ) >= MINIMUM_TIME_BETWEEN_DOWN_AND_UP_FOR_RESET ) )
      {
        // Reset the velocity only if down was received a while ago
        mLastVelocity = Vector2( 0.0f, 0.0f );
      }

      UpdateLocalScrollProperties();
      // Only finish the transform if scrolling was interrupted on down or if we are scrolling
      if ( mScrollInterrupted || mScrolling )
      {
        DALI_LOG_SCROLL_STATE("[0x%X] Calling FinishTransform", this);

        FinishTransform();
      }
    }
    mTouchDownTimeoutReached = false;
    mScrollInterrupted = false;
  }

  return true;
}

bool ScrollView::OnMouseWheelEvent(const MouseWheelEvent& event)
{
  if(!mSensitive)
  {
    // Ignore this mouse wheel event, if scrollview is insensitive.
    return false;
  }

  Vector3 targetScrollPosition = GetPropertyPosition();

  if(mRulerX->IsEnabled() && !mRulerY->IsEnabled())
  {
    // If only the ruler in the X axis is enabled, scroll in the X axis.
    if(mRulerX->GetType() == Ruler::Free)
    {
      // Free panning mode
      targetScrollPosition.x += event.z * mMouseWheelScrollDistanceStep.x;
      ClampPosition(targetScrollPosition);
      ScrollTo(-targetScrollPosition);
    }
    else if(!mScrolling)
    {
      // Snap mode, only respond to the event when the previous snap animation is finished.
      ScrollTo(GetCurrentPage() - event.z);
    }
  }
  else
  {
    // If the ruler in the Y axis is enabled, scroll in the Y axis.
    if(mRulerY->GetType() == Ruler::Free)
    {
      // Free panning mode
      targetScrollPosition.y += event.z * mMouseWheelScrollDistanceStep.y;
      ClampPosition(targetScrollPosition);
      ScrollTo(-targetScrollPosition);
    }
    else if(!mScrolling)
    {
      // Snap mode, only respond to the event when the previous snap animation is finished.
      ScrollTo(GetCurrentPage() - event.z * mRulerX->GetTotalPages());
    }
  }

  return true;
}

void ScrollView::ResetScrolling()
{
  Actor self = Self();
  self.GetProperty(mPropertyPosition).Get(mScrollPostPosition);
  mScrollPrePosition = mScrollPostPosition;
  DALI_LOG_SCROLL_STATE("[0x%X] Setting mPropertyPrePosition To[%.2f, %.2f]", this, mScrollPostPosition.x, mScrollPostPosition.y );
  self.SetProperty(mPropertyPrePosition, mScrollPostPosition);
}

void ScrollView::UpdateLocalScrollProperties()
{
  Actor self = Self();
  self.GetProperty(mPropertyPrePosition).Get(mScrollPrePosition);
  self.GetProperty(mPropertyPosition).Get(mScrollPostPosition);
}

// private functions

void ScrollView::PreAnimatedScrollSetup()
{
  // mPropertyPrePosition is our unclamped property with wrapping
  // mPropertyPosition is our final scroll position after clamping

  Actor self = Self();

  Vector3 deltaPosition(mScrollPostPosition);
  WrapPosition(mScrollPostPosition);
  mDomainOffset += deltaPosition - mScrollPostPosition;
  Self().SetProperty(mPropertyDomainOffset, mDomainOffset);

  if( mScrollStateFlags & SCROLL_X_STATE_MASK )
  {
    // already performing animation on internal x position
    StopAnimation(mInternalXAnimation);
  }

  if( mScrollStateFlags & SCROLL_Y_STATE_MASK )
  {
    // already performing animation on internal y position
    StopAnimation(mInternalYAnimation);
  }

  mScrollStateFlags = 0;

  // Update Actor position with this wrapped value.
}

void ScrollView::FinaliseAnimatedScroll()
{
  // TODO - common animation finishing code in here
}

void ScrollView::AnimateInternalXTo( float position, float duration, AlphaFunction alpha )
{
  StopAnimation(mInternalXAnimation);

  if( duration > Math::MACHINE_EPSILON_10 )
  {
    Actor self = Self();
    DALI_LOG_SCROLL_STATE("[0x%X], Animating from[%.2f] to[%.2f]", this, self.GetProperty(mPropertyPrePosition).Get<Vector3>().x, position );
    mInternalXAnimation = Animation::New(duration);
    DALI_LOG_SCROLL_STATE("[0x%X], mInternalXAnimation[0x%X]", this, mInternalXAnimation.GetObjectPtr() );
    mInternalXAnimation.FinishedSignal().Connect(this, &ScrollView::OnScrollAnimationFinished);
    mInternalXAnimation.AnimateTo( Property(self, mPropertyPrePosition, 0), position, alpha, duration);
    mInternalXAnimation.Play();

    // erase current state flags
    mScrollStateFlags &= ~SCROLL_X_STATE_MASK;
    // add internal animation state flag
    mScrollStateFlags |= AnimatingInternalX;
  }
}

void ScrollView::AnimateInternalYTo( float position, float duration, AlphaFunction alpha )
{
  StopAnimation(mInternalYAnimation);

  if( duration > Math::MACHINE_EPSILON_10 )
  {
    Actor self = Self();
    DALI_LOG_SCROLL_STATE("[0x%X], Animating from[%.2f] to[%.2f]", this, self.GetProperty(mPropertyPrePosition).Get<Vector3>().y, position );
    mInternalYAnimation = Animation::New(duration);
    DALI_LOG_SCROLL_STATE("[0x%X], mInternalYAnimation[0x%X]", this, mInternalYAnimation.GetObjectPtr() );
    mInternalYAnimation.FinishedSignal().Connect(this, &ScrollView::OnScrollAnimationFinished);
    mInternalYAnimation.AnimateTo( Property(self, mPropertyPrePosition, 1), position, alpha, TimePeriod(duration));
    mInternalYAnimation.Play();

    // erase current state flags
    mScrollStateFlags &= ~SCROLL_Y_STATE_MASK;
    // add internal animation state flag
    mScrollStateFlags |= AnimatingInternalY;
  }
}

void ScrollView::OnScrollAnimationFinished( Animation& source )
{
  // Guard against destruction during signal emission
  // Note that ScrollCompletedSignal is emitted from HandleSnapAnimationFinished()
  Toolkit::ScrollView handle( GetOwner() );

  bool scrollingFinished = false;

  // update our local scroll positions
  UpdateLocalScrollProperties();

  if( source == mInternalXAnimation )
  {
    DALI_LOG_SCROLL_STATE("[0x%X] mInternalXAnimation[0x%X], expected[%.2f], actual[%.2f], post[%.2f]", this, mInternalXAnimation.GetObjectPtr(), mScrollTargetPosition.x, Self().GetProperty(mPropertyPrePosition).Get<Vector3>().x, mScrollPostPosition.x );

    if( !(mScrollStateFlags & AnimatingInternalY) )
    {
      scrollingFinished = true;
    }
    mInternalXAnimation.Reset();
    // wrap pre scroll x position and set it
    if( mWrapMode )
    {
      const RulerDomain rulerDomain = mRulerX->GetDomain();
      mScrollPrePosition.x = -WrapInDomain(-mScrollPrePosition.x, rulerDomain.min, rulerDomain.max);
      DALI_LOG_SCROLL_STATE("[0x%X] Setting mPropertyPrePosition To[%.2f, %.2f]", this, mScrollPrePosition.x, mScrollPrePosition.y );
      handle.SetProperty(mPropertyPrePosition, mScrollPrePosition);
    }
    SnapInternalXTo(mScrollPostPosition.x);
  }

  if( source == mInternalYAnimation )
  {
    DALI_LOG_SCROLL_STATE("[0x%X] mInternalYAnimation[0x%X], expected[%.2f], actual[%.2f], post[%.2f]", this, mInternalYAnimation.GetObjectPtr(), mScrollTargetPosition.y, Self().GetProperty(mPropertyPrePosition).Get<Vector3>().y, mScrollPostPosition.y );

    if( !(mScrollStateFlags & AnimatingInternalX) )
    {
      scrollingFinished = true;
    }
    mInternalYAnimation.Reset();
    if( mWrapMode )
    {
      // wrap pre scroll y position and set it
      const RulerDomain rulerDomain = mRulerY->GetDomain();
      mScrollPrePosition.y = -WrapInDomain(-mScrollPrePosition.y, rulerDomain.min, rulerDomain.max);
      DALI_LOG_SCROLL_STATE("[0x%X] Setting mPropertyPrePosition To[%.2f, %.2f]", this, mScrollPrePosition.x, mScrollPrePosition.y );
      handle.SetProperty(mPropertyPrePosition, mScrollPrePosition);
    }
    SnapInternalYTo(mScrollPostPosition.y);
  }

  DALI_LOG_SCROLL_STATE("[0x%X] scrollingFinished[%d] Animation[0x%X]", this, scrollingFinished, source.GetObjectPtr());

  if(scrollingFinished)
  {
    HandleSnapAnimationFinished();
  }
}

void ScrollView::OnSnapInternalPositionFinished( Animation& source )
{
  Actor self = Self();
  UpdateLocalScrollProperties();
  if( source == mInternalXAnimation )
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Finished X PostPosition Animation", this );

    // clear internal x animation flags
    mScrollStateFlags &= ~SCROLL_X_STATE_MASK;
    mInternalXAnimation.Reset();
    WrapPosition(mScrollPrePosition);
  }
  if( source == mInternalYAnimation )
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Finished Y PostPosition Animation", this );

    mScrollStateFlags &= ~SCROLL_Y_STATE_MASK;
    mInternalYAnimation.Reset();
    WrapPosition(mScrollPrePosition);
  }
}

void ScrollView::SnapInternalXTo(float position)
{
  Actor self = Self();

  StopAnimation(mInternalXAnimation);

  // erase current state flags
  mScrollStateFlags &= ~SCROLL_X_STATE_MASK;

  // if internal x not equal to inputed parameter, animate it
  float duration = std::min(fabsf((position - mScrollPrePosition.x) / mMaxOvershoot.x) * mSnapOvershootDuration, mSnapOvershootDuration);
  DALI_LOG_SCROLL_STATE("[0x%X] duration[%.2f]", this, duration );
  if( duration > Math::MACHINE_EPSILON_1 )
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Starting X Snap Animation to[%.2f]", this, position );

    mInternalXAnimation = Animation::New(duration);
    mInternalXAnimation.FinishedSignal().Connect(this, &ScrollView::OnSnapInternalPositionFinished);
    mInternalXAnimation.AnimateTo(Property(self, mPropertyPrePosition, 0), position);
    mInternalXAnimation.Play();

    // add internal animation state flag
    mScrollStateFlags |= SnappingInternalX;
  }
}

void ScrollView::SnapInternalYTo(float position)
{
  Actor self = Self();

  StopAnimation(mInternalYAnimation);

  // erase current state flags
  mScrollStateFlags &= ~SCROLL_Y_STATE_MASK;

  // if internal y not equal to inputed parameter, animate it
  float duration = std::min(fabsf((position - mScrollPrePosition.y) / mMaxOvershoot.y) * mSnapOvershootDuration, mSnapOvershootDuration);
  DALI_LOG_SCROLL_STATE("[0x%X] duration[%.2f]", this, duration );
  if( duration > Math::MACHINE_EPSILON_1 )
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Starting Y Snap Animation to[%.2f]", this, position );

    mInternalYAnimation = Animation::New(duration);
    mInternalYAnimation.FinishedSignal().Connect(this, &ScrollView::OnSnapInternalPositionFinished);
    mInternalYAnimation.AnimateTo(Property(self, mPropertyPrePosition, 1), position);
    mInternalYAnimation.Play();

    // add internal animation state flag
    mScrollStateFlags |= SnappingInternalY;
  }
}

void ScrollView::GestureStarted()
{
  // we handle the first gesture.
  // if we're currently doing a gesture and receive another
  // we continue and combine the effects of the gesture instead of reseting.
  if(mGestureStackDepth++==0)
  {
    Actor self = Self();
    StopTouchDownTimer();
    StopAnimation();
    mPanDelta = Vector3::ZERO;
    mLastVelocity = Vector2(0.0f, 0.0f);
    if( !mScrolling )
    {
      mLockAxis = LockPossible;
    }

    if( mScrollStateFlags & SCROLL_X_STATE_MASK )
    {
      StopAnimation(mInternalXAnimation);
    }
    if( mScrollStateFlags & SCROLL_Y_STATE_MASK )
    {
      StopAnimation(mInternalYAnimation);
    }
    mScrollStateFlags = 0;

    if(mScrolling) // are we interrupting a current scroll?
    {
      // set mScrolling to false, in case user has code that interrogates mScrolling Getter() in complete.
      mScrolling = false;
      // send negative scroll position since scroll internal scroll position works as an offset for actors,
      // give applications the position within the domain from the scroll view's anchor position
      DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignalV2 5 [%.2f, %.2f]", this, -mScrollPostPosition.x, -mScrollPostPosition.y);
      mScrollCompletedSignalV2.Emit( -mScrollPostPosition );
    }
  }
}

void ScrollView::GestureContinuing(const Vector2& panDelta)
{
  mPanDelta.x+= panDelta.x;
  mPanDelta.y+= panDelta.y;

  // Save the velocity, there is a bug in PanGesture
  // Whereby the Gesture::Finished's velocity is either:
  // NaN (due to time delta of zero between the last two events)
  // or 0 (due to position being the same between the last two events)

  // Axis Auto Lock - locks the panning to the horizontal or vertical axis if the pan
  // appears mostly horizontal or mostly vertical respectively.
  if(mAxisAutoLock)
  {
    mLockAxis = GetLockAxis(mPanDelta.GetVectorXY(), mLockAxis, mAxisAutoLockGradient);
  } // end if mAxisAutoLock
}

// TODO: Upgrade to use a more powerful gesture detector (one that supports multiple touches on pan - so works as pan and flick gesture)
// BUG: Gesture::Finished doesn't always return velocity on release (due to
// timeDelta between last two events being 0 sometimes, or posiiton being the same)
void ScrollView::OnPan(PanGesture gesture)
{
  // Guard against destruction during signal emission
  // Note that Emit() methods are called indirectly e.g. from within ScrollView::OnGestureEx()
  Actor self( Self() );

  if(!mSensitive)
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Pan Ignored, Insensitive", this);

    // If another callback on the same original signal disables sensitivity,
    // this callback will still be called, so we must suppress it.
    return;
  }

  // translate Gesture input to get useful data...
  switch(gesture.state)
  {
    case Gesture::Started:
    {
      DALI_LOG_SCROLL_STATE("[0x%X] Pan Started", this);
      mPanStartPosition = gesture.position - gesture.displacement;
      UpdateLocalScrollProperties();
      GestureStarted();
      mPanning = true;
      self.SetProperty( mPropertyPanning, true );
      self.SetProperty( mPropertyScrollStartPagePosition, Vector3(gesture.position.x, gesture.position.y, 0.0f) );

      UpdateMainInternalConstraint();
      break;
    }

    case Gesture::Continuing:
    {
      if ( mPanning )
      {
        DALI_LOG_SCROLL_STATE("[0x%X] Pan Continuing", this);
        GestureContinuing(gesture.screenDisplacement);
      }
      else
      {
        // If we do not think we are panning, then we should not do anything here
        return;
      }
      break;
    }

    case Gesture::Finished:
    case Gesture::Cancelled:
    {
      if ( mPanning )
      {
        DALI_LOG_SCROLL_STATE("[0x%X] Pan %s", this, ( ( gesture.state == Gesture::Finished ) ? "Finished" : "Cancelled" ) );

        UpdateLocalScrollProperties();
        mLastVelocity = gesture.velocity;
        mPanning = false;
        self.SetProperty( mPropertyPanning, false );

        if( mScrollMainInternalPrePositionConstraint )
        {
          self.RemoveConstraint(mScrollMainInternalPrePositionConstraint);
        }

        if( mOvershootIndicator )
        {
          mOvershootIndicator->ClearOvershoot();
        }
      }
      else
      {
        // If we do not think we are panning, then we should not do anything here
        return;
      }
      break;
    }

    case Gesture::Possible:
    case Gesture::Clear:
    {
      // Nothing to do, not needed.
      break;
    }

  } // end switch(gesture.state)

  OnGestureEx(gesture.state);
}

void ScrollView::OnGestureEx(Gesture::State state)
{
  // call necessary signals for application developer

  if(state == Gesture::Started)
  {
    Vector3 currentScrollPosition = GetCurrentScrollPosition();
    Self().SetProperty(mPropertyScrolling, true);
    mScrolling = true;
    DALI_LOG_SCROLL_STATE("[0x%X] mScrollStartedSignalV2 2 [%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y);
    mScrollStartedSignalV2.Emit( currentScrollPosition );
  }
  else if( (state == Gesture::Finished) ||
           (state == Gesture::Cancelled) ) // Finished/default
  {
    // when all the gestures have finished, we finish the transform.
    // so if a user decides to pan (1 gesture), and then pan+zoom (2 gestures)
    // then stop panning (back to 1 gesture), and then stop zooming (0 gestures).
    // this is the point we end, and perform necessary snapping.
    mGestureStackDepth--;
    if(mGestureStackDepth==0)
    {
      // no flick if we have not exceeded min flick distance
      if( (fabsf(mPanDelta.x) < mMinFlickDistance.x)
          && (fabsf(mPanDelta.y) < mMinFlickDistance.y) )
      {
        // reset flick velocity
        mLastVelocity = Vector2::ZERO;
      }
      FinishTransform();
    }
    else
    {
      DALI_LOG_SCROLL_STATE("[0x%X] mGestureStackDepth[%d]", this, mGestureStackDepth);
    }
  }
}

void ScrollView::FinishTransform()
{
  // at this stage internal x and x scroll position should have followed prescroll position exactly
  Actor self = Self();

  PreAnimatedScrollSetup();

  // convert pixels/millisecond to pixels per second
  bool animating = SnapWithVelocity(mLastVelocity * 1000.0f);

  if(!animating)
  {
    // if not animating, then this pan has completed right now.
    SetScrollUpdateNotification(false);
    mScrolling = false;
    Self().SetProperty(mPropertyScrolling, false);

    if( fabs(mScrollPrePosition.x - mScrollTargetPosition.x) > Math::MACHINE_EPSILON_10 )
    {
      SnapInternalXTo(mScrollTargetPosition.x);
    }
    if( fabs(mScrollPrePosition.y - mScrollTargetPosition.y) > Math::MACHINE_EPSILON_10 )
    {
      SnapInternalYTo(mScrollTargetPosition.y);
    }
    Vector3 currentScrollPosition = GetCurrentScrollPosition();
    DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignalV2 6 [%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y);
    mScrollCompletedSignalV2.Emit( currentScrollPosition );
  }
}

Vector3 ScrollView::GetOvershoot(Vector3& position) const
{
  Vector3 size = Self().GetCurrentSize();
  Vector3 overshoot;

  const RulerDomain rulerDomainX = mRulerX->GetDomain();
  const RulerDomain rulerDomainY = mRulerY->GetDomain();

  if(mRulerX->IsEnabled() && rulerDomainX.enabled)
  {
    const float left = rulerDomainX.min - position.x;
    const float right = size.width - rulerDomainX.max - position.x;
    if(left<0)
    {
      overshoot.x = left;
    }
    else if(right>0)
    {
      overshoot.x = right;
    }
  }

  if(mRulerY->IsEnabled() && rulerDomainY.enabled)
  {
    const float top = rulerDomainY.min - position.y;
    const float bottom = size.height - rulerDomainY.max - position.y;
    if(top<0)
    {
      overshoot.y = top;
    }
    else if(bottom>0)
    {
      overshoot.y = bottom;
    }
  }

  return overshoot;
}

bool ScrollView::OnAccessibilityPan(PanGesture gesture)
{
  // Keep track of whether this is an AccessibilityPan
  mInAccessibilityPan = true;
  OnPan(gesture);
  mInAccessibilityPan = false;

  return true;
}

void ScrollView::ClampPosition(Vector3& position) const
{
  ClampState3 clamped;
  ClampPosition(position, clamped);
}

void ScrollView::ClampPosition(Vector3& position, ClampState3 &clamped) const
{
  Vector3 size = Self().GetCurrentSize();

  position.x = -mRulerX->Clamp(-position.x, size.width, 1.0f, clamped.x);    // NOTE: X & Y rulers think in -ve coordinate system.
  position.y = -mRulerY->Clamp(-position.y, size.height, 1.0f, clamped.y);   // That is scrolling RIGHT (e.g. 100.0, 0.0) means moving LEFT.

  clamped.z = NotClamped;
}

void ScrollView::WrapPosition(Vector3& position) const
{
  if(mWrapMode)
  {
    const RulerDomain rulerDomainX = mRulerX->GetDomain();
    const RulerDomain rulerDomainY = mRulerY->GetDomain();

    if(mRulerX->IsEnabled())
    {
      position.x = -WrapInDomain(-position.x, rulerDomainX.min, rulerDomainX.max);
    }

    if(mRulerY->IsEnabled())
    {
      position.y = -WrapInDomain(-position.y, rulerDomainY.min, rulerDomainY.max);
    }
  }
}

void ScrollView::UpdateMainInternalConstraint()
{
  // TODO: Only update the constraints which have changed, rather than remove all and add all again.
  // Requires a dali-core ApplyConstraintAt, or a ReplaceConstraint. The former is probably more flexible.
  Actor self = Self();
  PanGestureDetector detector( GetPanGestureDetector() );

  if(mScrollMainInternalPositionConstraint)
  {
    self.RemoveConstraint(mScrollMainInternalPositionConstraint);
    self.RemoveConstraint(mScrollMainInternalDeltaConstraint);
    self.RemoveConstraint(mScrollMainInternalFinalConstraint);
    self.RemoveConstraint(mScrollMainInternalRelativeConstraint);
  }
  if( mScrollMainInternalPrePositionConstraint )
  {
    self.RemoveConstraint(mScrollMainInternalPrePositionConstraint);
  }

  // TODO: It's probably better to use a local displacement value as this will give a displacement when scrolling just commences
  // but we need to make sure than the gesture system gives displacement since last frame (60Hz), not displacement since last touch event (90Hz).

  // 1. First calculate the pre-position (this is the scroll position if no clamping has taken place)
  Vector2 initialPanMask = Vector2(mRulerX->IsEnabled() ? 1.0f : 0.0f, mRulerY->IsEnabled() ? 1.0f : 0.0f);

  if( mLockAxis == LockVertical )
  {
    initialPanMask.y = 0.0f;
  }
  else if( mLockAxis == LockHorizontal )
  {
    initialPanMask.x = 0.0f;
  }
  Constraint constraint;

  if( mPanning )
  {
    constraint = Constraint::New<Vector3>( mPropertyPrePosition,
                                                      Source( detector, PanGestureDetector::LOCAL_POSITION ),
                                                      Source( self, Actor::SIZE ),
                                                      InternalPrePositionConstraint( mPanStartPosition, initialPanMask, mAxisAutoLock, mAxisAutoLockGradient, mLockAxis, mMaxOvershoot, mRulerX->GetDomain(), mRulerY->GetDomain() ) );
    mScrollMainInternalPrePositionConstraint = self.ApplyConstraint( constraint );
  }

  // 2. Second calculate the clamped position (actual position)
  constraint = Constraint::New<Vector3>( mPropertyPosition,
                                         LocalSource( mPropertyPrePosition ),
                                         LocalSource( mPropertyPositionMin ),
                                         LocalSource( mPropertyPositionMax ),
                                         Source( self, Actor::SIZE ),
                                         InternalPositionConstraint( mRulerX->GetDomain(),
                                                                     mRulerY->GetDomain(), mWrapMode ) );
  mScrollMainInternalPositionConstraint = self.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector3>( mPropertyPositionDelta,
                                         LocalSource( mPropertyPosition ),
                                         LocalSource( mPropertyDomainOffset ),
                                         InternalPositionDeltaConstraint );
  mScrollMainInternalDeltaConstraint = self.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector3>( mPropertyFinal,
                                         LocalSource( mPropertyPosition ),
                                         LocalSource( mPropertyOvershootX ),
                                         LocalSource( mPropertyOvershootY ),
                                         InternalFinalConstraint( FinalDefaultAlphaFunction,
                                                                  FinalDefaultAlphaFunction ) );
  mScrollMainInternalFinalConstraint = self.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector3>( mPropertyRelativePosition,
                                         LocalSource( mPropertyPosition ),
                                         LocalSource( mPropertyPositionMin ),
                                         LocalSource( mPropertyPositionMax ),
                                         LocalSource( Actor::SIZE ),
                                         InternalRelativePositionConstraint );
  mScrollMainInternalRelativeConstraint = self.ApplyConstraint( constraint );

  // When panning we want to make sure overshoot values are affected by pre position and post position
  SetOvershootConstraintsEnabled(!mWrapMode);
}

void ScrollView::SetOvershootConstraintsEnabled(bool enabled)
{
  Actor self( Self() );
  // remove and reset, it may now be in wrong order with the main internal constraints
  if( mScrollMainInternalOvershootXConstraint )
  {
    self.RemoveConstraint(mScrollMainInternalOvershootXConstraint);
    mScrollMainInternalOvershootXConstraint.Reset();
    self.RemoveConstraint(mScrollMainInternalOvershootYConstraint);
    mScrollMainInternalOvershootYConstraint.Reset();
  }
  if( enabled )
  {
    Constraint constraint = Constraint::New<float>( mPropertyOvershootX,
                                           LocalSource( mPropertyPrePosition ),
                                           LocalSource( mPropertyPosition ),
                                           LocalSource( mPropertyCanScrollHorizontal ),
                                           OvershootXConstraint(mMaxOvershoot.x) );
    mScrollMainInternalOvershootXConstraint = self.ApplyConstraint( constraint );

    constraint = Constraint::New<float>( mPropertyOvershootY,
                                           LocalSource( mPropertyPrePosition ),
                                           LocalSource( mPropertyPosition ),
                                           LocalSource( mPropertyCanScrollVertical ),
                                           OvershootYConstraint(mMaxOvershoot.y) );
    mScrollMainInternalOvershootYConstraint = self.ApplyConstraint( constraint );
  }
  else
  {
    self.SetProperty(mPropertyOvershootX, 0.0f);
    self.SetProperty(mPropertyOvershootY, 0.0f);
  }
}

void ScrollView::SetInternalConstraints()
{
  // Internal constraints (applied to target ScrollBase Actor itself) /////////
  UpdateMainInternalConstraint();

  // User definable constraints to apply to all child actors //////////////////
  Actor self = Self();

  // Apply some default constraints to ScrollView & its bound actors
  // Movement + Wrap function

  Constraint constraint;

  // MoveActor (scrolling)
  constraint = Constraint::New<Vector3>( Actor::POSITION,
                                         Source( self, mPropertyPosition ),
                                         MoveActorConstraint );
  constraint.SetRemoveAction(Constraint::Discard);
  ApplyConstraintToBoundActors(constraint);

  // WrapActor (wrap functionality)
  constraint = Constraint::New<Vector3>( Actor::POSITION,
                                                 LocalSource( Actor::SCALE ),
                                                 LocalSource( Actor::ANCHOR_POINT ),
                                                 LocalSource( Actor::SIZE ),
                                                 Source( self, mPropertyPositionMin ),
                                                 Source( self, mPropertyPositionMax ),
                                                 Source( self, mPropertyWrap ),
                                                 WrapActorConstraint );
  constraint.SetRemoveAction(Constraint::Discard);
  ApplyConstraintToBoundActors(constraint);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
