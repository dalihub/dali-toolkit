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

// INTERNAL INCLUDES
#include <dali/public-api/events/mouse-wheel-event.h>

#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-constraints.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-component-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-effect-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-overshoot-indicator-impl.h>

// TODO: Change to two class system:
// 1. DraggableActor (is an actor which can be dragged anywhere/scaled/rotated, can be set to range using the ruler)
// 2. ScrollView (contains a draggable actor that can a) be dragged in the negative X, and Y domain, b) has a hitArea for touches)
// TODO: Rotation
// TODO: Asymetrical scaling
// TODO: external components (page and status overlays).
// TODO: Orientation.
// TODO: upgrade Vector2/3 to support returning Unit vectors, normals, & cross product (dot product is already provided)

using namespace Dali;

namespace
{

const int DEFAULT_REFRESH_INTERVAL_MILLISECONDS = 50;                                     ///< Refresh rate TODO: Animation should have an update signal (and see item-view-impl)
const float FLICK_SPEED_THRESHOLD = 500.0f;                                               ///< Flick threshold in pixels/ms
const float FREE_FLICK_SPEED_THRESHOLD = 200.0f;                                          ///< Free-Flick threshold in pixels/ms
const float AUTOLOCK_AXIS_MINIMUM_DISTANCE2 = 100.0f;                                     ///< Auto-lock axis after minimum distance squared.
const float FLICK_ORTHO_ANGLE_RANGE = 60.0f;                                              ///< degrees. (if >45, then supports diagonal flicking)
const unsigned int MAXIMUM_NUMBER_OF_VALUES = 5;                                          ///< Number of values to use for weighted pan calculation.
const Vector2 DEFAULT_MOUSE_WHEEL_SCROLL_DISTANCE_STEP_PROPORTION = Vector2(0.17f, 0.1f); ///< The step of horizontal scroll distance in the proportion of stage size for each mouse wheel event received.
const unsigned long MINIMUM_TIME_BETWEEN_DOWN_AND_UP_FOR_RESET( 150u );

// predefined effect values
const Vector3 ANGLE_CAROUSEL_ROTATE(Math::PI * 0.5f, Math::PI * 0.5f, 0.0f);
const Vector3 ANGLE_CUBE_PAGE_ROTATE(Math::PI * 0.2f, Math::PI * 0.2f, 0.0f);  ///< Cube page rotates as if it has ten sides with the camera positioned inside
const Vector2 ANGLE_CUSTOM_CUBE_SWING(-Math::PI * 0.45f, -Math::PI * 0.45f);  ///< outer cube pages swing 90 degrees as they pan offscreen
const Vector2 ANGLE_SPIRAL_SWING_IN(Math::PI * 0.5f, Math::PI * 0.5f);
const Vector2 ANGLE_SPIRAL_SWING_OUT(Math::PI * 0.35f, Math::PI * 0.35f);
const Vector2 ANGLE_OUTER_CUBE_SWING(Math::PI * 0.5f, Math::PI * 0.5f);  ///< outer cube pages swing 90 degrees as they pan offscreen

// Helpers ////////////////////////////////////////////////////////////////////////////////////////

// TODO: GetAngle for Vector2 can be moved.
// GetAngle for Vector3 needs to be measured against a normal/plane.

/**
 * @param[in] vector The 3D vector to be measured
 * @return angle in radians from 0 to 2PI
 */
float GetAngle(const Vector3& vector)
{
  return atan2(vector.y, vector.x) + Math::PI;
}

/**
 * @param[in] vector The 2D vector to be measured
 * @return angle in radians from 0 to 2PI
 */
float GetAngle(const Vector2& vector)
{
  return atan2(vector.y, vector.x) + Math::PI;
}

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
  const Vector3& position = -scrollPositionProperty.GetVector3();
  const Vector3& min = scrollMinProperty.GetVector3();
  const Vector3& max = scrollMaxProperty.GetVector3();
  const Vector3& size = scrollSizeProperty.GetVector3();

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
 * Internal Pre-Position Property Constraint.
 *
 * Generates position property based on current position + gesture displacement.
 * Or generates position property based on positionX/Y.
 * Note: This is the position prior to any clamping at scroll boundaries.
 * TODO: Scale & Rotation Transforms.
 */
struct InternalPrePositionConstraint
{
  InternalPrePositionConstraint(const Vector2& initialPanMask,
                                bool axisAutoLock,
                                float axisAutoLockGradient)
  : mInitialPanMask(initialPanMask),
    mAxisAutoLock(axisAutoLock),
    mLockAxis(ScrollView::LockPossible),
    mAxisAutoLockGradient(axisAutoLockGradient),
    mPrePosition(Vector3::ZERO),
    mWasPanning(false)
  {
  }

  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& gesturePositionProperty,
                     const PropertyInput& gestureDisplacementProperty,
                     const PropertyInput& scrollPositionXProperty,
                     const PropertyInput& scrollPositionYProperty,
                     const PropertyInput& panningProperty)
  {
    const bool panning = panningProperty.GetBoolean();
    Vector3 scrollPostPosition;

    if(panning)
    {
      // Check if panning has just started...
      if(!mWasPanning)
      {
        mLocalStart = gesturePositionProperty.GetVector2() - gestureDisplacementProperty.GetVector2();
        mPrePosition = current;
        mLockAxis = ScrollView::LockPossible;

        mCurrentPanMask = mInitialPanMask;
      }

      // Calculate Deltas...
      Vector2 currentPosition = gesturePositionProperty.GetVector2();
      Vector2 panDelta( currentPosition - mLocalStart );

      // Axis Auto Lock - locks the panning to the horizontal or vertical axis if the pan
      // appears mostly horizontal or mostly vertical respectively...
      AxisAutoLock(panDelta);

      // Restrict deltas based on ruler enable/disable and axis-lock state...
      panDelta *= mCurrentPanMask;

      // Perform Position transform based on input deltas...
      scrollPostPosition = mPrePosition;
      scrollPostPosition.GetVectorXY() += panDelta;
    }
    else
    {
      scrollPostPosition.x = scrollPositionXProperty.GetFloat();
      scrollPostPosition.y = scrollPositionYProperty.GetFloat();
    }

    mWasPanning = panning;
    return scrollPostPosition;
  }

  void AxisAutoLock(Vector2& panDelta)
  {
    if(mAxisAutoLock)
    {
      if(panDelta.LengthSquared() > AUTOLOCK_AXIS_MINIMUM_DISTANCE2 &&
          mLockAxis == ScrollView::LockPossible)
      {
        float dx = fabsf(panDelta.x);
        float dy = fabsf(panDelta.y);
        if(dx * mAxisAutoLockGradient >= dy)
        {
          // 0.36:1 gradient to the horizontal (deviate < 20 degrees)
          mLockAxis = ScrollView::LockVertical;
          mCurrentPanMask.y = 0.0f;
        }
        else if(dy * mAxisAutoLockGradient > dx)
        {
          // 0.36:1 gradient to the vertical (deviate < 20 degrees)
          mLockAxis = ScrollView::LockHorizontal;
          mCurrentPanMask.x = 0.0f;
        }
        else
        {
          mLockAxis = ScrollView::LockNone;
        }
      }
    } // end if mAxisAutoLock
  }

  Vector2 mLocalStart;
  Vector2 mInitialPanMask;              ///< Initial pan mask (based on ruler settings)
  Vector2 mCurrentPanMask;              ///< Current pan mask that can be altered by axis lock mode.

  bool mAxisAutoLock;                   ///< Set by ScrollView
  ScrollView::LockAxis mLockAxis;
  float mAxisAutoLockGradient;          ///< Set by ScrollView
  Vector3 mPrePosition;
  bool mWasPanning;
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
  InternalPositionConstraint(const RulerDomain& domainX, const RulerDomain& domainY)
  : mDomainMin( -domainX.min, -domainY.min ),
    mDomainMax( -domainX.max, -domainY.max ),
    mClampX( domainX.enabled ),
    mClampY( domainY.enabled )
  {
  }

  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& scrollPositionProperty,
                     const PropertyInput& scrollSizeProperty)
  {
    Vector3 position = scrollPositionProperty.GetVector3();
    const Vector2& size = scrollSizeProperty.GetVector3().GetVectorXY();

    position.x = mClampX ? Clamp(position.x, mDomainMax.x + size.x, mDomainMin.x ) : position.x;
    position.y = mClampY ? Clamp(position.y, mDomainMax.y + size.y, mDomainMin.y ) : position.y;

    return position;
  }

  Vector2 mDomainMin;
  Vector2 mDomainMax;
  bool mClampX;
  bool mClampY;

};

/**
 * This constraint updates the X overshoot property using the difference
 * mPropertyPrePosition.x and mPropertyPosition.x, returning a relative value between 0.0f and 1.0f
 */
struct OvershootXConstraint
{
  OvershootXConstraint(float maxOvershoot) : mLastOvershoot(0.0f), mMaxOvershoot(maxOvershoot) {}

  float operator()(const float&    current,
      const PropertyInput& scrollPrePositionProperty,
      const PropertyInput& scrollPostPositionProperty)
  {
    Vector3 scrollPrePosition = scrollPrePositionProperty.GetVector3();
    Vector3 scrollPostPosition = scrollPostPositionProperty.GetVector3();
    float newOvershoot = scrollPrePosition.x - scrollPostPosition.x;
    return (newOvershoot > 0.0f ? std::min(newOvershoot, mMaxOvershoot) : std::max(newOvershoot, -mMaxOvershoot)) / mMaxOvershoot;
  }

  float mLastOvershoot;
  float mMaxOvershoot;
};

/**
 * This constraint updates the Y overshoot property using the difference
 * mPropertyPrePosition.y and mPropertyPosition.y, returning a relative value between 0.0f and 1.0f
 */
struct OvershootYConstraint
{
  OvershootYConstraint(float maxOvershoot) : mLastOvershoot(0.0f), mMaxOvershoot(maxOvershoot) {}

  float operator()(const float&    current,
      const PropertyInput& scrollPrePositionProperty,
      const PropertyInput& scrollPostPositionProperty)
  {
    Vector3 scrollPrePosition = scrollPrePositionProperty.GetVector3();
    Vector3 scrollPostPosition = scrollPostPositionProperty.GetVector3();
    float newOvershoot = scrollPrePosition.y - scrollPostPosition.y;
    return (newOvershoot > 0.0f ? std::min(newOvershoot, mMaxOvershoot) : std::max(newOvershoot, -mMaxOvershoot)) / mMaxOvershoot;
  }

  float mLastOvershoot;
  float mMaxOvershoot;
};

/**
 * When panning, this constraint updates the X property, otherwise
 * it has no effect on the X property.
 */
float InternalXConstraint(const float&    current,
                          const PropertyInput& scrollPosition,
                          const PropertyInput& panningProperty)
{
  return scrollPosition.GetVector3().x;
}

/**
 * When panning, this constraint updates the Y property, otherwise
 * it has no effect on the Y property.
 */
float InternalYConstraint(const float&    current,
                          const PropertyInput& scrollPosition,
                          const PropertyInput& panningProperty)
{
  return scrollPosition.GetVector3().y;
}

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
  mInitialized(false),
  mScrolling(false),
  mScrollInterrupted(false),
  mTouchDownTime(0u),
  mSensitive(true),
  mGestureStackDepth(0),
  mRotationDelta(0.0f),
  mScrollPreRotation(0.0f),
  mScrollPostRotation(0.0f),
  mTouchDownReceived(false),
  mActorAutoSnapEnabled(false),
  mAutoResizeContainerEnabled(false),
  mWrapMode(false),
  mAxisAutoLock(false),
  mMinTouchesForPanning(1),
  mMaxTouchesForPanning(1),
  mLockAxis(LockPossible),
  mRefreshIntervalMilliseconds(DEFAULT_REFRESH_INTERVAL_MILLISECONDS),
  mAlterChild(false),
  mOvershootDelay(1.0f),
  mMaxOvershoot(Toolkit::ScrollView::DEFAULT_MAX_OVERSHOOT, Toolkit::ScrollView::DEFAULT_MAX_OVERSHOOT),
  mDefaultMaxOvershoot(true),
  mSnapOvershootDuration(Toolkit::ScrollView::DEFAULT_SNAP_OVERSHOOT_DURATION),
  mSnapOvershootAlphaFunction(AlphaFunctions::EaseOut),
  mSnapDuration(Toolkit::ScrollView::DEFAULT_SLOW_SNAP_ANIMATION_DURATION),
  mSnapAlphaFunction(AlphaFunctions::EaseOut),
  mFlickDuration(Toolkit::ScrollView::DEFAULT_FAST_SNAP_ANIMATION_DURATION),
  mFlickAlphaFunction(AlphaFunctions::EaseOut),
  mAxisAutoLockGradient(Toolkit::ScrollView::DEFAULT_AXIS_AUTO_LOCK_GRADIENT),
  mFrictionCoefficient(Toolkit::ScrollView::DEFAULT_FRICTION_COEFFICIENT),
  mFlickSpeedCoefficient(Toolkit::ScrollView::DEFAULT_FLICK_SPEED_COEFFICIENT),
  mMaxFlickSpeed(Toolkit::ScrollView::DEFAULT_MAX_FLICK_SPEED)
{
  SetRequiresMouseWheelEvents(true);
}

void ScrollView::OnInitialize()
{
  Actor self = Self();
  self.SetLeaveRequired(true);

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
  mScrollPostScale = mScrollPreScale = Vector3::ONE;
  mScrollPostRotation = mScrollPreRotation = 0.0f;

  mMouseWheelScrollDistanceStep = Stage::GetCurrent().GetSize() * DEFAULT_MOUSE_WHEEL_SCROLL_DISTANCE_STEP_PROPORTION;

  mInitialized = true;

  mGestureStackDepth = 0;

  EnableGestureDetection( Gesture::Type( Gesture::Pan ) );

  // For pan, default to only 1 touch required, ignoring touches outside this range.
  SetTouchesRequiredForPanning(1, 1, false);

  // By default we'll allow the user to freely drag the scroll view,
  // while disabling the other rulers.
  RulerPtr ruler = new DefaultRuler();
  RulerPtr rulerDisabled = new DefaultRuler();
  rulerDisabled->Disable();
  mRulerX = ruler;
  mRulerY = ruler;
  mRulerScaleX = rulerDisabled;
  mRulerScaleY = rulerDisabled;
  mRulerRotation = rulerDisabled;

  EnableScrollComponent(Toolkit::Scrollable::OvershootIndicator);

  Vector3 size = GetControlSize();
  UpdatePropertyDomain(size);
  SetInternalConstraints();
}

void ScrollView::OnControlStageConnection()
{
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
  if ( mSnapOvershootAnimation )
  {
    SetOvershootToOrigin();
  }

  StopAnimation();
}

ScrollView::~ScrollView()
{
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
  Vector3 min;
  Vector3 max;

  bool canScrollVertical = false;
  bool canScrollHorizontal = false;
  Actor self = Self();
  if(mRulerX->IsEnabled())
  {
    const Toolkit::RulerDomain& rulerDomain = mRulerX->GetDomain();
    min.x = rulerDomain.min;
    max.x = rulerDomain.max;

    // make sure new scroll value is within new domain
    float newScroll = min.x;
    int scrollXPropertyIndex = self.GetPropertyIndex(Toolkit::ScrollView::SCROLL_X_PROPERTY_NAME);
    if((fabsf(max.x - min.x) - size.x) > Math::MACHINE_EPSILON_1)
    {
      canScrollHorizontal = true;
      float currentScroll = self.GetProperty<float>(scrollXPropertyIndex);
      newScroll = Clamp(currentScroll, -(max.x - size.x), -min.x);
    }
    self.SetProperty(scrollXPropertyIndex, newScroll);
  }

  if(mRulerY->IsEnabled())
  {
    const Toolkit::RulerDomain& rulerDomain = mRulerY->GetDomain();
    min.y = rulerDomain.min;
    max.y = rulerDomain.max;

    // make sure new scroll value is within new domain
    float newScroll = min.y;
    int scrollYPropertyIndex = self.GetPropertyIndex(Toolkit::ScrollView::SCROLL_Y_PROPERTY_NAME);
    if((fabsf(max.y - min.y) - size.y) > Math::MACHINE_EPSILON_1)
    {
      canScrollVertical = true;
      float currentScroll = self.GetProperty<float>(scrollYPropertyIndex);
      newScroll = Clamp(currentScroll, -(max.y - size.y), -min.y);
    }
    self.SetProperty(scrollYPropertyIndex, newScroll);
  }
  self.SetProperty(mPropertyCanScrollVertical, canScrollVertical);
  self.SetProperty(mPropertyCanScrollHorizontal, canScrollHorizontal);

  self.SetProperty(mPropertyPositionMin, min );
  self.SetProperty(mPropertyPositionMax, max );
}

void ScrollView::SetRulerScaleX(RulerPtr ruler)
{
  mRulerScaleX = ruler;
  UpdateMainInternalConstraint();
}

void ScrollView::SetRulerScaleY(RulerPtr ruler)
{
  mRulerScaleY = ruler;
  UpdateMainInternalConstraint();
}

void ScrollView::SetRulerRotation(RulerPtr ruler)
{
  mRulerRotation = ruler;
  UpdateMainInternalConstraint();
}

void ScrollView::SetScrollSensitive(bool sensitive)
{
  Actor self = Self();
  PanGestureDetector panGesture( GetPanGestureDetector() );

  if((!mSensitive) && (sensitive))
  {
    mSensitive = sensitive;
    panGesture.Attach(self);
  }
  else if((mSensitive) && (!sensitive))
  {
    // while the scroll view is panning, the state needs to be reset.
    bool isPanning = self.GetProperty<bool>( mPropertyPanning );
    if ( isPanning )
    {
      PanGesture cancelGesture( Gesture::Cancelled );
      OnPan( cancelGesture );
    }

    panGesture.Detach(self);
    mSensitive = sensitive;

    mGestureStackDepth = 0;
  }
}

void ScrollView::SetMaxOvershoot(float overshootX, float overshootY)
{
  mMaxOvershoot.x = overshootX;
  mMaxOvershoot.y = overshootY;
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

int ScrollView::GetRefreshInterval() const
{
  return mRefreshIntervalMilliseconds;
}

void ScrollView::SetRefreshInterval(int milliseconds)
{
  mRefreshIntervalMilliseconds = milliseconds;
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
  Vector3 position = GetPropertyPrePosition();

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
  // in case animation is currently taking place.
  return -GetPropertyPrePosition();
}

Vector3 ScrollView::GetCurrentScrollScale() const
{
  // in case animation is currently taking place.
  return GetPropertyScale();
}

Vector3 ScrollView::GetDomainSize() const
{
  Vector3 size = Self().GetCurrentSize();

  const RulerDomain& xDomain = GetRulerX()->GetDomain();
  const RulerDomain& yDomain = GetRulerY()->GetDomain();

  Vector3 domainSize = Vector3( xDomain.max - xDomain.min, yDomain.max - yDomain.min, 0.0f ) - size;
  return domainSize;
}

void ScrollView::TransformTo(const Vector3& position, const Vector3& scale, float rotation,
                             DirectionBias horizontalBias, DirectionBias verticalBias)
{
  TransformTo(position, scale, rotation, mSnapDuration, horizontalBias, verticalBias);
}

void ScrollView::TransformTo(const Vector3& position, const Vector3& scale, float rotation, float duration,
                             DirectionBias horizontalBias, DirectionBias verticalBias)
{
  // Guard against destruction during signal emission
  // Note that Emit() methods are called indirectly e.g. from within ScrollView::AnimateTo()
  Toolkit::ScrollView handle( GetOwner() );

  Vector3 currentScrollPosition = GetCurrentScrollPosition();
  Self().SetProperty( mPropertyScrollStartPagePosition, currentScrollPosition );

  if(mScrolling) // are we interrupting a current scroll?
  {
    // set mScrolling to false, in case user has code that interrogates mScrolling Getter() in complete.
    mScrolling = false;
    mScrollCompletedSignalV2.Emit( currentScrollPosition );
  }

  Self().SetProperty(mPropertyScrolling, true);
  mScrolling = true;
  mScrollStartedSignalV2.Emit( currentScrollPosition );
  bool animating = AnimateTo(-position,
                             Vector3::ONE * duration,
                             scale,
                             Vector3::ONE * duration,
                             rotation,
                             duration,
                             mSnapAlphaFunction,
                             true,
                             horizontalBias,
                             verticalBias,
                             Snap);

  if(!animating)
  {
    // if not animating, then this pan has completed right now.
    Self().SetProperty(mPropertyScrolling, false);
    mScrolling = false;
    mScrollCompletedSignalV2.Emit( currentScrollPosition );
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

void ScrollView::ScrollTo(const Vector3& position, float duration,
                          DirectionBias horizontalBias, DirectionBias verticalBias)
{
  TransformTo(position, mScrollPostScale, mScrollPostRotation, duration, horizontalBias, verticalBias);
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
  Vector2 stationaryVelocity = Vector2(0.0f, 0.0f);
  return SnapWithVelocity( stationaryVelocity );
}

void ScrollView::ScaleTo(const Vector3& scale)
{
  ScaleTo(scale, mSnapDuration);
}

void ScrollView::ScaleTo(const Vector3& scale, float duration)
{
  TransformTo(mScrollPostPosition, scale, mScrollPostRotation, duration);
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
  Vector3 scaleDuration = Vector3::ONE * mSnapDuration;
  float rotationDuration = mSnapDuration;
  float biasX = 0.5f;
  float biasY = 0.5f;
  FindDirection horizontal = None;
  FindDirection vertical = None;

  // orthoAngleRange = Angle tolerance within the Exact N,E,S,W direction
  // that will be accepted as a general N,E,S,W flick direction.

  const float orthoAngleRange = FLICK_ORTHO_ANGLE_RANGE * M_PI / 180.0f;
  const float flickSpeedThreshold2 = FLICK_SPEED_THRESHOLD*FLICK_SPEED_THRESHOLD;

  // Flick logic X Axis

  if(mRulerX->IsEnabled())
  {
    horizontal = All;

    if(speed2 > flickSpeedThreshold2) // exceeds flick threshold
    {
      if((angle >= -orthoAngleRange) && (angle < orthoAngleRange)) // Swiping East
      {
        biasX = 0.0f, horizontal = Left;
      }
      else if((angle >= M_PI-orthoAngleRange) || (angle < -M_PI+orthoAngleRange)) // Swiping West
      {
        biasX = 1.0f, horizontal = Right;
      }
    }
  }

  // Flick logic Y Axis

  if(mRulerY->IsEnabled())
  {
    vertical = All;

    if(speed2 > flickSpeedThreshold2) // exceeds flick threshold
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

  // Position Snap ////////////////////////////////////////////////////////////
  Vector3 positionSnap = mScrollPostPosition;

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

  // Scale Snap ///////////////////////////////////////////////////////////////
  Vector3 scaleSnap = mScrollPostScale;

  scaleSnap.x = mRulerScaleX->Snap(scaleSnap.x);
  scaleSnap.y = mRulerScaleY->Snap(scaleSnap.y);

  ClampScale(scaleSnap);

  // Rotation Snap ////////////////////////////////////////////////////////////
  float rotationSnap = mScrollPostRotation;
  // TODO: implement rotation snap

  bool animating = AnimateTo(positionSnap, positionDuration,
                             scaleSnap, scaleDuration,
                             rotationSnap, rotationDuration,
                             alphaFunction, false,
                             DirectionBiasNone, DirectionBiasNone,
                             isFlick || isFreeFlick ? Flick : Snap);

  if(animating)
  {
    AnimateOvershootToOrigin(positionDuration.x, positionDuration.y);
  }

  return animating;
}

void ScrollView::StopAnimation(void)
{
  // Clear Snap animation if exists.
  if(mSnapAnimation)
  {
    mSnapAnimation.Stop();
    mSnapAnimation.FinishedSignal().Disconnect(this, &ScrollView::OnSnapAnimationFinished);
    mSnapAnimation.Clear();
    mSnapAnimation = NULL;
  }
  if(mSnapXAnimation)
  {
    mSnapXAnimation.Stop();
    mSnapXAnimation.FinishedSignal().Disconnect(this, &ScrollView::OnSnapXAnimationFinished);
    mSnapXAnimation.Clear();
    mSnapXAnimation = NULL;
  }
  if(mSnapYAnimation)
  {
    mSnapYAnimation.Stop();
    mSnapYAnimation.FinishedSignal().Disconnect(this, &ScrollView::OnSnapYAnimationFinished);
    mSnapYAnimation.Clear();
    mSnapYAnimation = NULL;
  }
  if(mSnapOvershootAnimation)
  {
    mSnapOvershootAnimation.FinishedSignal().Disconnect(this, &ScrollView::OnSnapOvershootAnimationFinished);
    mSnapOvershootAnimation.Stop();
    mSnapOvershootAnimation.Clear();
    mSnapOvershootAnimation = NULL;
  }
  HandleStoppedAnimation();
}

bool ScrollView::AnimateTo(const Vector3& position, const Vector3& positionDuration,
                           const Vector3& scale, const Vector3& scaleDuration,
                           float rotation, float rotationDuration,
                           AlphaFunction alpha, bool findShortcuts,
                           DirectionBias horizontalBias, DirectionBias verticalBias,
                           SnapType snapType)
{
  // Here we perform an animation on a number of properties (depending on which have changed)
  // The animation is applied to all ScrollBases
  Actor self = Self();
  bool startAnimation = false;
  Vector3 positionTransformed = position;
  float totalDuration = 0.0f;

  bool positionChanged = (positionTransformed != mScrollPostPosition);
  bool scaleChanged = (scale != mScrollPostScale);
  bool rotationChanged = fabsf(rotation - mScrollPostRotation) > Math::MACHINE_EPSILON_0;

  if(positionChanged)
  {
    totalDuration = std::max(totalDuration, positionDuration.x);
    totalDuration = std::max(totalDuration, positionDuration.y);
  }

  if(scaleChanged)
  {
    totalDuration = std::max(totalDuration, scaleDuration.x);
    totalDuration = std::max(totalDuration, scaleDuration.y);
  }

  if(rotationChanged)
  {
    totalDuration = std::max(totalDuration, rotationDuration);
  }

  if(totalDuration > Math::MACHINE_EPSILON_1)
  {
    StopAnimation();
    mSnapAnimation = Animation::New(totalDuration);
    mSnapAnimation.FinishedSignal().Connect(this, &ScrollView::OnSnapAnimationFinished);
    mSnapXAnimation = Animation::New(positionDuration.x);
    mSnapXAnimation.FinishedSignal().Connect(this, &ScrollView::OnSnapXAnimationFinished);
    mSnapYAnimation = Animation::New(positionDuration.y);
    mSnapYAnimation.FinishedSignal().Connect(this, &ScrollView::OnSnapYAnimationFinished);
    startAnimation = true;

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
          float dir = VectorInDomain(-mScrollPostPosition.x, -positionTransformed.x, rulerDomainX.min, rulerDomainX.max, horizontalBias);
          positionTransformed.x = mScrollPostPosition.x + -dir;
        }

        if(mRulerY->IsEnabled())
        {
          float dir = VectorInDomain(-mScrollPostPosition.y, -positionTransformed.y, rulerDomainY.min, rulerDomainY.max, verticalBias);
          positionTransformed.y = mScrollPostPosition.y + -dir;
        }
      }

      // note we have two separate animations for X & Y, this deals with sliding diagonally and hitting
      // a horizonal/vertical wall.delay
      mSnapXAnimation.AnimateTo( Property(self, mPropertyX), positionTransformed.x, alpha, TimePeriod(0.0f, positionDuration.x));
      mSnapYAnimation.AnimateTo( Property(self, mPropertyY), positionTransformed.y, alpha, TimePeriod(0.0f, positionDuration.y));
    }

    // Scale Delta ///////////////////////////////////////////////////////
    if(scaleChanged)
    {
      // TODO: for non-uniform scaling to different bounds e.g. scaling a square to a 4:3 aspect ratio screen with a velocity
      // the height will hit first, and then the width, so that would require two different animation times just like position.
      mSnapAnimation.AnimateTo( Property(self, mPropertyScale), scale, alpha, TimePeriod(0.0f, scaleDuration.x));
    }

    mSnapAnimation.AnimateTo( Property(self, mPropertyTime), totalDuration, AlphaFunctions::Linear );

    mSnapAnimation.Play();
    mSnapXAnimation.Play();
    mSnapYAnimation.Play();
    StartRefreshTimer();
  } // end if(totalDuration > Math::MACHINE_EPSILON_1)
  else // totalDuration == 0
  {
    // instantly set transform.
    if(positionChanged)
    {
      self.SetProperty(mPropertyX, positionTransformed.x);
      self.SetProperty(mPropertyY, positionTransformed.y);

      mScrollPrePosition = mScrollPostPosition = positionTransformed;
    }

    if(scaleChanged)
    {
      self.SetProperty(mPropertyScale, scale);

      mScrollPreScale = mScrollPostScale = scale;
    }
  }

  // Always send a snap event when AnimateTo is called.
  Toolkit::ScrollView::SnapEvent snapEvent;
  snapEvent.type = snapType;
  snapEvent.position = positionTransformed;
  snapEvent.scale = scale;
  snapEvent.rotation = rotation;
  snapEvent.duration = totalDuration;

  mSnapStartedSignalV2.Emit( snapEvent );

  return startAnimation;
}

void ScrollView::SetOvershootEnabled(bool enabled)
{
  if(enabled && !mOvershootIndicator)
  {
    mOvershootIndicator = ScrollOvershootIndicator::New(*this);
  }
  mOvershootIndicator->Enable(enabled);
}

void ScrollView::AddOverlay(Actor actor)
{
  mInternalActor.Add( actor );
}

void ScrollView::RemoveOverlay(Actor actor)
{
  mInternalActor.Remove( actor );
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
  Vector3 position(Self().GetProperty<float>(mPropertyX), Self().GetProperty<float>(mPropertyY), 0.0f);
  WrapPosition(position);

  return position;
}

Vector3 ScrollView::GetPropertyPosition() const
{
  Vector3 position = Self().GetProperty<Vector3>(mPropertyPosition);
  WrapPosition(position);

  return position;
}

Vector3 ScrollView::GetPropertyScale() const
{
  return Self().GetProperty<Vector3>(mPropertyScale);
}

void ScrollView::HandleStoppedAnimation()
{
  // Animation has stopped, so stop sending the scroll-update signal.
  CancelRefreshTimer();

  // cement transform now, and allow interactivity to resume.
  mScrollPostPosition = GetPropertyPosition();

  mScrollPostScale = GetPropertyScale();

  // Update Actor position with this wrapped value.

  Self().SetProperty(mPropertyX, mScrollPostPosition.x);
  Self().SetProperty(mPropertyY, mScrollPostPosition.y);
  // TODO Rotation

  mScrollPrePosition = mScrollPostPosition;
  mScrollPreScale = mScrollPostScale;
  mScrollPreRotation = mScrollPostRotation;
}

void ScrollView::HandleSnapAnimationFinished()
{
  // Emit Signal that scrolling has completed.
  mScrolling = false;
  Self().SetProperty(mPropertyScrolling, false);

  Vector3 deltaPosition(Self().GetProperty<float>(mPropertyX),
                        Self().GetProperty<float>(mPropertyY),
                        0.0f);

  Vector3 currentScrollPosition = GetCurrentScrollPosition();
  mScrollCompletedSignalV2.Emit( currentScrollPosition );

  mDomainOffset += deltaPosition - mScrollPostPosition;
  Self().SetProperty(mPropertyDomainOffset, mDomainOffset);
  HandleStoppedAnimation();
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
    mMaxOvershoot.x = size.x * 0.5f;
    mMaxOvershoot.y = size.y * 0.5f;
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

bool ScrollView::OnTouchEvent(const TouchEvent& event)
{
  if(!mSensitive)
  {
    // Ignore this touch event, if scrollview is insensitive.
    return false;
  }

  // Ignore events with multiple-touch points
  if (event.GetPointCount() != 1)
  {
    return false;
  }

  if (event.GetPoint(0).state == TouchPoint::Down)
  {
    mTouchDownTime = event.time;
    mTouchDownReceived = true;
    mTouchDownPosition = event.GetPoint(0).local;

    if( mSnapAnimation || mSnapXAnimation || mSnapYAnimation || mSnapOvershootAnimation )
    {
      mScrollInterrupted = true;
      StopAnimation();
    }

    if(mScrolling) // are we interrupting a current scroll?
    {
      // reset domain offset as scrolling from original plane.
      mDomainOffset = Vector3::ZERO;
      Self().SetProperty(mPropertyDomainOffset, Vector3::ZERO);

      mScrolling = false;
      Vector3 currentScrollPosition = GetCurrentScrollPosition();
      mScrollCompletedSignalV2.Emit( currentScrollPosition );
    }
  }
  else if(event.GetPoint(0).state == TouchPoint::Up)
  {
    // if the user touches and releases without enough movement to go
    // into a gesture state, then we should snap to nearest point.
    // otherwise our scroll could be stopped (interrupted) half way through an animation.
    if(mGestureStackDepth==0 && mTouchDownReceived)
    {
      unsigned timeDelta( event.time - mTouchDownTime );
      if ( timeDelta >= MINIMUM_TIME_BETWEEN_DOWN_AND_UP_FOR_RESET )
      {
        // Reset the velocity only if down was received a while ago
        mLastVelocity = Vector2( 0.0f, 0.0f );
      }
      else
      {
        Vector2 positionDelta( mTouchDownPosition - event.GetPoint(0).local );
        mLastVelocity = positionDelta / timeDelta;
      }

      // Only finish the transform if scrolling was interrupted on down or if we are scrolling
      if ( mSnapAnimation || mSnapXAnimation || mSnapYAnimation || mSnapOvershootAnimation || mScrollInterrupted || mScrolling )
      {
        FinishTransform();
      }
    }
    mTouchDownReceived = false;
    mScrollInterrupted = false;
  }

  return true; // consume since we're potentially scrolling
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
      targetScrollPosition.x -= event.z * mMouseWheelScrollDistanceStep.x;
      ClampPosition(targetScrollPosition);
      ScrollTo(-targetScrollPosition);
    }
    else if(!mScrolling)
    {
      // Snap mode, only respond to the event when the previous snap animation is finished.
      ScrollTo(GetCurrentPage() + event.z);
    }
  }
  else
  {
    // If the ruler in the Y axis is enabled, scroll in the Y axis.
    if(mRulerY->GetType() == Ruler::Free)
    {
      // Free panning mode
      targetScrollPosition.y -= event.z * mMouseWheelScrollDistanceStep.y;
      ClampPosition(targetScrollPosition);
      ScrollTo(-targetScrollPosition);
    }
    else if(!mScrolling)
    {
      // Snap mode, only respond to the event when the previous snap animation is finished.
      ScrollTo(GetCurrentPage() + event.z * mRulerX->GetTotalPages());
    }
  }

  return true;
}

void ScrollView::OnSnapAnimationFinished( Animation& source )
{
  mSnapAnimation.FinishedSignal().Disconnect( this, &ScrollView::OnSnapAnimationFinished );
  mSnapAnimation = NULL;
}

void ScrollView::OnSnapXAnimationFinished( Animation& source )
{
  // Guard against destruction during signal emission
  // Note that ScrollCompletedSignal is emitted from HandleSnapAnimationFinished()
  Toolkit::ScrollView handle( GetOwner() );

  if(!mSnapYAnimation)
  {
    HandleSnapAnimationFinished();
  }
  if(mScrollMainInternalOvershootXConstraint)
  {
    Self().RemoveConstraint(mScrollMainInternalOvershootXConstraint);
    mScrollMainInternalOvershootXConstraint.Reset();
    mScrollMainInternalOvershootXConstraint = 0;
  }
  mSnapXAnimation.FinishedSignal().Disconnect(this, &ScrollView::OnSnapXAnimationFinished);
  mSnapXAnimation.Reset();
  mSnapXAnimation = NULL;
  if( IsScrollComponentEnabled(Toolkit::Scrollable::OvershootIndicator) )
  {
    // kick start animation to 0
    Self().SetProperty(mPropertyOvershootX, 0.0f);
  }
}

void ScrollView::OnSnapYAnimationFinished( Animation& source )
{
  // Guard against destruction during signal emission
  // Note that ScrollCompletedSignal is emitted from HandleSnapAnimationFinished()
  Toolkit::ScrollView handle( GetOwner() );

  if(!mSnapXAnimation)
  {
    HandleSnapAnimationFinished();
  }
  if(mScrollMainInternalOvershootYConstraint)
  {
    Self().RemoveConstraint(mScrollMainInternalOvershootYConstraint);
    mScrollMainInternalOvershootYConstraint.Reset();
    mScrollMainInternalOvershootYConstraint = 0;
  }
  mSnapYAnimation.FinishedSignal().Disconnect(this, &ScrollView::OnSnapYAnimationFinished);
  mSnapYAnimation.Reset();
  mSnapYAnimation = NULL;
  if( IsScrollComponentEnabled(Toolkit::Scrollable::OvershootIndicator) )
  {
    // kick start animation to 0
    Self().SetProperty(mPropertyOvershootY, 0.0f);
  }
}

void ScrollView::GestureStarted()
{
  // we handle the first gesture.
  // if we're currently doing a gesture and receive another
  // we continue and combine the effects of the gesture instead of reseting.
  if(mGestureStackDepth++==0)
  {
    StopAnimation();
    mPanDelta = Vector3::ZERO;
    mScaleDelta = Vector3::ONE;
    mRotationDelta = 0.0f;
    mLastVelocity = Vector2(0.0f, 0.0f);
    mLockAxis = LockPossible;

    if(mScrolling) // are we interrupting a current scroll?
    {
      // set mScrolling to false, in case user has code that interrogates mScrolling Getter() in complete.
      mScrolling = false;
      Vector3 currentScrollPosition = GetCurrentScrollPosition();
      mScrollCompletedSignalV2.Emit( currentScrollPosition );
    }
  }
}

void ScrollView::GestureContinuing(Vector2 panDelta, Vector2 scaleDelta, float rotationDelta)
{
  mPanDelta.x+= panDelta.x;
  mPanDelta.y+= panDelta.y;
  mScaleDelta.x*= scaleDelta.x;
  mScaleDelta.y*= scaleDelta.y;
  mRotationDelta+= rotationDelta;

  // Save the velocity, there is a bug in PanGesture
  // Whereby the Gesture::Finished's velocity is either:
  // NaN (due to time delta of zero between the last two events)
  // or 0 (due to position being the same between the last two events)

  // Axis Auto Lock - locks the panning to the horizontal or vertical axis if the pan
  // appears mostly horizontal or mostly vertical respectively.
  if(mAxisAutoLock)
  {
    if(mPanDelta.LengthSquared() > AUTOLOCK_AXIS_MINIMUM_DISTANCE2 &&
        mLockAxis == LockPossible)
    {
      float dx = fabsf(mPanDelta.x);
      float dy = fabsf(mPanDelta.y);
      if(dx * mAxisAutoLockGradient >= dy)
      {
        // 0.36:1 gradient to the horizontal (deviate < 20 degrees)
        mLockAxis = LockVertical;
      }
      else if(dy * mAxisAutoLockGradient > dx)
      {
        // 0.36:1 gradient to the vertical (deviate < 20 degrees)
        mLockAxis = LockHorizontal;
      }
      else
      {
        mLockAxis = LockNone;
      }
    }
  } // end if mAxisAutoLock
}

// TODO: Upgrade to use a more powerful gesture detector (one that supports multiple touches on pan - so works as pan and flick gesture)
// TODO: Reimplement Scaling (pinching 2+ points)
// TODO: Reimplment Rotation (pinching 2+ points)
// BUG: Gesture::Finished doesn't always return velocity on release (due to
// timeDelta between last two events being 0 sometimes, or posiiton being the same)
void ScrollView::OnPan(PanGesture gesture)
{
  // Guard against destruction during signal emission
  // Note that Emit() methods are called indirectly e.g. from within ScrollView::OnGestureEx()
  Actor self( Self() );

  if(!mSensitive)
  {
    // If another callback on the same original signal disables sensitivity,
    // this callback will still be called, so we must suppress it.
    return;
  }

  // translate Gesture input to get useful data...
  switch(gesture.state)
  {
    case Gesture::Started:
    {
      GestureStarted();
      self.SetProperty( mPropertyPanning, true );
      self.SetProperty( mPropertyScrollStartPagePosition, GetCurrentScrollPosition() );

      //  Update property: X & Y = Position (only when in panning mode - in snapping mode, X & Y are animated).
      Constraint constraint = Constraint::New<float>( mPropertyX,
                                           LocalSource( mPropertyPosition ),
                                           Source( self, mPropertyPanning ),
                                           InternalXConstraint );
      mScrollMainInternalXConstraint = self.ApplyConstraint(constraint);

      constraint = Constraint::New<float>( mPropertyY,
                                           LocalSource( mPropertyPosition ),
                                           Source( self, mPropertyPanning ),
                                           InternalYConstraint );
      mScrollMainInternalYConstraint = self.ApplyConstraint(constraint);
      // When panning we want to make sure overshoot values are affected by pre position and post position
      SetOvershootConstraintsEnabled(true);
      break;
    }

    case Gesture::Continuing:
    {
      // Nothing to do, handled in constraint.
      break;
    }

    case Gesture::Finished:
    case Gesture::Cancelled:
    {
      mLastVelocity = gesture.velocity;
      self.SetProperty( mPropertyPanning, false );

      // Remove X & Y position constraints as they are not required when we are not panning.
      self.RemoveConstraint(mScrollMainInternalXConstraint);
      self.RemoveConstraint(mScrollMainInternalYConstraint);
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
      FinishTransform();
    }
  }
}

void ScrollView::UpdateTransform()
{
// TODO: notify clamps using property notifications (or see if we need this, can deprecate it)
}

void ScrollView::FinishTransform()
{
  const Vector3& scrollPosition = Self().GetProperty<Vector3>(mPropertyPosition);

  mScrollPostPosition.x = scrollPosition.x;
  mScrollPostPosition.y = scrollPosition.y;

  Vector3 deltaPosition(mScrollPostPosition);
  // Cement PRE transform (PRE = POST), and Begin Snap Animation if necessary.
  WrapPosition(mScrollPostPosition);

  mDomainOffset += deltaPosition - mScrollPostPosition;
  Self().SetProperty(mPropertyDomainOffset, mDomainOffset);

  bool animating = SnapWithVelocity(mLastVelocity * 1000.0f);

  if(!animating)
  {
    AnimateOvershootToOrigin(0.0f, 0.0f);
    // if not animating, then this pan has completed right now.
    mScrolling = false;
    Self().SetProperty(mPropertyScrolling, false);
    Vector3 currentScrollPosition = GetCurrentScrollPosition();
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
  OnPan(gesture);
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

  // determine size of viewport relative to current scaled size.
  // e.g. if you're zoomed in 200%, then each pixel on screen is only 0.5 pixels on subject.
  if(fabsf(mScrollPostScale.x) > Math::MACHINE_EPSILON_0)
  {
    size.x /= mScrollPostScale.x;
  }

  if(fabsf(mScrollPostScale.y) > Math::MACHINE_EPSILON_0)
  {
    size.y /= mScrollPostScale.y;
  }

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

void ScrollView::ClampScale(Vector3& scale) const
{
  ClampState3 clamped;
  ClampScale(scale, clamped);
}

void ScrollView::ClampScale(Vector3& scale, ClampState3 &clamped) const
{
  scale.x = mRulerScaleX->Clamp(scale.x, 0.0f, 1.0f, clamped.x);
  scale.y = mRulerScaleY->Clamp(scale.y, 0.0f, 1.0f, clamped.y);
  clamped.z = NotClamped;
}

void ScrollView::UpdateMainInternalConstraint()
{
  // TODO: Only update the constraints which have changed, rather than remove all and add all again.
  // Requires a dali-core ApplyConstraintAt, or a ReplaceConstraint. The former is probably more flexible.
  Actor self = Self();
  PanGestureDetector detector( GetPanGestureDetector() );

  if(mScrollMainInternalPrePositionConstraint)
  {
    self.RemoveConstraint(mScrollMainInternalPrePositionConstraint);
    self.RemoveConstraint(mScrollMainInternalPositionConstraint);
    self.RemoveConstraint(mScrollMainInternalDeltaConstraint);
    self.RemoveConstraint(mScrollMainInternalFinalConstraint);
    self.RemoveConstraint(mScrollMainInternalRelativeConstraint);
  }

  // TODO: It's probably better to use a local displacement value as this will give a displacement when scrolling just commences
  // but we need to make sure than the gesture system gives displacement since last frame (60Hz), not displacement since last touch event (90Hz).

  // 1. First calculate the pre-position (this is the scroll position if no clamping has taken place)
  Vector2 initialPanMask = Vector2(mRulerX->IsEnabled() ? 1.0f : 0.0f, mRulerY->IsEnabled() ? 1.0f : 0.0f);

  Constraint constraint = Constraint::New<Vector3>( mPropertyPrePosition,
                                                    Source( detector, PanGestureDetector::LOCAL_POSITION ),
                                                    Source( detector, PanGestureDetector::LOCAL_DISPLACEMENT ),
                                                    LocalSource( mPropertyX ),
                                                    LocalSource( mPropertyY ),
                                                    Source( self, mPropertyPanning ),
                                                    InternalPrePositionConstraint( initialPanMask, mAxisAutoLock, mAxisAutoLockGradient ) );
  mScrollMainInternalPrePositionConstraint = self.ApplyConstraint(constraint);

  // 2. Second calculate the clamped position (actual position)
  constraint = Constraint::New<Vector3>( mPropertyPosition,
                                         LocalSource( mPropertyPrePosition ),
                                         Source( self, Actor::SIZE ),
                                         InternalPositionConstraint( mRulerX->GetDomain(),
                                                                     mRulerY->GetDomain()) );
  mScrollMainInternalPositionConstraint = self.ApplyConstraint(constraint);

  constraint = Constraint::New<Vector3>( mPropertyPositionDelta,
                                         LocalSource( mPropertyPosition ),
                                         LocalSource( mPropertyDomainOffset ),
                                         InternalPositionDeltaConstraint );
  mScrollMainInternalDeltaConstraint = self.ApplyConstraint(constraint);

  constraint = Constraint::New<Vector3>( mPropertyFinal,
                                         LocalSource( mPropertyPosition ),
                                         LocalSource( mPropertyOvershootX ),
                                         LocalSource( mPropertyOvershootY ),
                                         InternalFinalConstraint( FinalDefaultAlphaFunction,
                                                                  FinalDefaultAlphaFunction ) );
  mScrollMainInternalFinalConstraint = self.ApplyConstraint(constraint);

  constraint = Constraint::New<Vector3>( mPropertyRelativePosition,
                                         LocalSource( mPropertyPosition ),
                                         LocalSource( mPropertyPositionMin ),
                                         LocalSource( mPropertyPositionMax ),
                                         LocalSource( Actor::SIZE ),
                                         InternalRelativePositionConstraint );
  mScrollMainInternalRelativeConstraint = self.ApplyConstraint(constraint);

  if(mScrollMainInternalOvershootXConstraint)
  {
    // reset these constraints in correct order
    self.RemoveConstraint(mScrollMainInternalOvershootXConstraint);
    mScrollMainInternalOvershootXConstraint.Reset();

    Constraint constraint = Constraint::New<float>( mPropertyOvershootX,
                                           LocalSource( mPropertyPrePosition ),
                                           LocalSource( mPropertyPosition ),
                                           OvershootXConstraint(mMaxOvershoot.x) );
    mScrollMainInternalOvershootXConstraint = self.ApplyConstraint(constraint);
  }

  if(mScrollMainInternalOvershootYConstraint)
  {
    // reset these constraints in correct order
    self.RemoveConstraint(mScrollMainInternalOvershootYConstraint);
    mScrollMainInternalOvershootYConstraint.Reset();

    Constraint constraint = Constraint::New<float>( mPropertyOvershootY,
                                           LocalSource( mPropertyPrePosition ),
                                           LocalSource( mPropertyPosition ),
                                           OvershootXConstraint(mMaxOvershoot.y) );
    mScrollMainInternalOvershootYConstraint = self.ApplyConstraint(constraint);
  }
}

void ScrollView::SetOvershootConstraintsEnabled(bool enabled)
{
  Actor self( Self() );
  // remove and reset, it may now be in wrong order with the main internal constraints
  if(mScrollMainInternalOvershootXConstraint)
  {
    self.RemoveConstraint(mScrollMainInternalOvershootXConstraint);
    mScrollMainInternalOvershootXConstraint.Reset();
  }
  if(mScrollMainInternalOvershootYConstraint)
  {
    self.RemoveConstraint(mScrollMainInternalOvershootYConstraint);
    mScrollMainInternalOvershootYConstraint.Reset();
  }
  if(enabled)
  {
    Constraint constraint = Constraint::New<float>( mPropertyOvershootX,
                                           LocalSource( mPropertyPrePosition ),
                                           LocalSource( mPropertyPosition ),
                                           OvershootXConstraint(mMaxOvershoot.x) );
    mScrollMainInternalOvershootXConstraint = self.ApplyConstraint(constraint);
    constraint = Constraint::New<float>( mPropertyOvershootY,
                                           LocalSource( mPropertyPrePosition ),
                                           LocalSource( mPropertyPosition ),
                                           OvershootYConstraint(mMaxOvershoot.y) );
    mScrollMainInternalOvershootYConstraint = self.ApplyConstraint(constraint);
  }
}

void ScrollView::SetInternalConstraints()
{
  // Internal constraints (applied to target ScrollBase Actor itself) /////////
  UpdateMainInternalConstraint();

  // User definable constraints to apply to all child actors //////////////////
  Actor self = Self();

  // LocalSource - The Actors to be moved.
  // self - The ScrollView

  // Apply some default constraints to ScrollView.
  // Movement + Scaling + Wrap function

  Constraint constraint;

  // MoveScaledActor (scrolling/zooming)
  constraint = Constraint::New<Vector3>( Actor::POSITION,
                                         Source( self, mPropertyPosition ),
                                         Source( self, mPropertyScale ),
                                         MoveScaledActorConstraint );
  constraint.SetRemoveAction(Constraint::Discard);
  ApplyConstraintToBoundActors(constraint);

  // ScaleActor (scrolling/zooming)
  constraint = Constraint::New<Vector3>( Actor::SCALE,
                                         Source( self, mPropertyScale ),
                                         ScaleActorConstraint );
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

void ScrollView::SetOvershootToOrigin()
{
  // Clear Snap animation if exists.
  if(mSnapOvershootAnimation)
  {
    mSnapOvershootAnimation.FinishedSignal().Disconnect(this, &ScrollView::OnSnapOvershootAnimationFinished);
    mSnapOvershootAnimation.Stop();
    mSnapOvershootAnimation.Clear();
    mSnapOvershootAnimation = NULL;
  }
  SetOvershootConstraintsEnabled(false);
  Self().SetProperty(mPropertyOvershootX, 0.0f);
  Self().SetProperty(mPropertyOvershootY, 0.0f);
}

void ScrollView::AnimateOvershootToOrigin(float xDelay, float yDelay)
{
  if( IsScrollComponentEnabled(Toolkit::Scrollable::OvershootIndicator) )
  {
    if(xDelay < Math::MACHINE_EPSILON_1)
    {
      // kick start animation to 0
      Self().SetProperty(mPropertyOvershootX, 0.0f);
    }
    if(yDelay < Math::MACHINE_EPSILON_1)
    {
      // kick start animation to 0
      Self().SetProperty(mPropertyOvershootY, 0.0f);
    }
    return;
  }
  // When we need to animate overshoot to 0
  if(mSnapOvershootDuration > Math::MACHINE_EPSILON_1)
  {
    Actor self = Self();
    // Clear Snap animation if exists.
    if(mSnapOvershootAnimation)
    {
      mSnapOvershootAnimation.FinishedSignal().Disconnect( this, &ScrollView::OnSnapOvershootAnimationFinished );
      mSnapOvershootAnimation.Stop();
      mSnapOvershootAnimation.Clear();
      mSnapOvershootAnimation = NULL;
    }
    if(!mSnapXAnimation && mScrollMainInternalOvershootXConstraint)
    {
      // need to remove the x overshoot constraint now or it will override animation to 0
      Self().RemoveConstraint(mScrollMainInternalOvershootXConstraint);
      mScrollMainInternalOvershootXConstraint.Reset();
      mScrollMainInternalOvershootXConstraint = 0;
    }
    if(!mSnapYAnimation && mScrollMainInternalOvershootYConstraint)
    {
      // need to remove the y overshoot constraint now or it will override animation to 0
      Self().RemoveConstraint(mScrollMainInternalOvershootYConstraint);
      mScrollMainInternalOvershootYConstraint.Reset();
      mScrollMainInternalOvershootYConstraint = 0;
    }
    // setup the new overshoot to 0 animation
    float totalDuration = (xDelay > yDelay ? xDelay : yDelay) + mSnapOvershootDuration;
    mSnapOvershootAnimation = Animation::New(totalDuration);
    mSnapOvershootAnimation.FinishedSignal().Connect( this, &ScrollView::OnSnapOvershootAnimationFinished );

    mSnapOvershootAnimation.AnimateTo( Property(self, mPropertyOvershootX), 0.0f, mSnapOvershootAlphaFunction, TimePeriod(xDelay, mSnapOvershootDuration) );
    mSnapOvershootAnimation.AnimateTo( Property(self, mPropertyOvershootY), 0.0f, mSnapOvershootAlphaFunction, TimePeriod(yDelay, mSnapOvershootDuration) );

    mSnapOvershootAnimation.SetDuration(totalDuration);
    mSnapOvershootAnimation.Play();
  }
  else
  {
    SetOvershootToOrigin();
  }
}

void ScrollView::OnSnapOvershootAnimationFinished( Animation& source )
{
  mSnapOvershootAnimation = NULL;
}

void ScrollView::StartRefreshTimer()
{
  if(mRefreshIntervalMilliseconds > 0)
  {
    if (!mRefreshTimer)
    {
      mRefreshTimer = Timer::New( mRefreshIntervalMilliseconds );
      mRefreshTimer.TickSignal().Connect( this, &ScrollView::OnRefreshTick );
    }

    if (!mRefreshTimer.IsRunning())
    {
      mRefreshTimer.Start();
    }
  }
}

void ScrollView::CancelRefreshTimer()
{
  if (mRefreshTimer)
  {
    mRefreshTimer.Stop();
  }
}

bool ScrollView::OnRefreshTick()
{
  // Guard against destruction during signal emission
  Toolkit::ScrollView handle( GetOwner() );

  Vector3 currentScrollPosition = GetCurrentScrollPosition();
  mScrollUpdatedSignalV2.Emit( currentScrollPosition );

  return true;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
