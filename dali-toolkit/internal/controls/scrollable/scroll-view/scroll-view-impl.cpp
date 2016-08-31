/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <cstring> // for strcmp
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/events/wheel-event.h>
#include <dali/public-api/events/touch-data.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scroll-bar/scroll-bar.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-constraints.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-overshoot-indicator-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-effect-impl.h>

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
const float DEFAULT_SLOW_SNAP_ANIMATION_DURATION(0.5f);             ///< Default Drag-Release animation time.
const float DEFAULT_FAST_SNAP_ANIMATION_DURATION(0.25f);            ///< Default Drag-Flick animation time.
const float DEFAULT_SNAP_OVERSHOOT_DURATION(0.5f);                  ///< Default Overshoot snapping animation time.
const float DEFAULT_MAX_OVERSHOOT(100.0f);                          ///< Default maximum allowed overshoot in pixels

const float DEFAULT_AXIS_AUTO_LOCK_GRADIENT(0.36f);                 ///< Default Axis-AutoLock gradient threshold. default is 0.36:1 (20 degrees)
const float DEFAULT_FRICTION_COEFFICIENT(1.0f);                     ///< Default Friction Co-efficient. (in stage diagonals per second)
const float DEFAULT_FLICK_SPEED_COEFFICIENT(1.0f);                  ///< Default Flick speed coefficient (multiples input touch velocity)
const float DEFAULT_MAX_FLICK_SPEED(3.0f);                          ///< Default Maximum flick speed. (in stage diagonals per second)

const Vector2 DEFAULT_MIN_FLICK_DISTANCE(30.0f, 30.0f);              ///< minimum distance for pan before flick allowed
const float DEFAULT_MIN_FLICK_SPEED_THRESHOLD(500.0f);              ///< Minimum pan speed required for flick in pixels/s
const float FREE_FLICK_SPEED_THRESHOLD = 200.0f;                    ///< Free-Flick threshold in pixels/ms
const float AUTOLOCK_AXIS_MINIMUM_DISTANCE2 = 100.0f;               ///< Auto-lock axis after minimum distance squared.
const float FLICK_ORTHO_ANGLE_RANGE = 75.0f;                        ///< degrees. (if >45, then supports diagonal flicking)
const Vector2 DEFAULT_WHEEL_SCROLL_DISTANCE_STEP_PROPORTION = Vector2(0.17f, 0.1f); ///< The step of horizontal scroll distance in the proportion of stage size for each wheel event received.
const unsigned long MINIMUM_TIME_BETWEEN_DOWN_AND_UP_FOR_RESET( 150u );
const float TOUCH_DOWN_TIMER_INTERVAL = 100.0f;
const float DEFAULT_SCROLL_UPDATE_DISTANCE( 30.0f );                ///< Default distance to travel in pixels for scroll update signal

const std::string INTERNAL_MAX_POSITION_PROPERTY_NAME( "internalMaxPosition" );

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
void InternalRelativePositionConstraint( Vector2& relativePosition, const PropertyInputContainer& inputs)
{
  Vector2 position = -inputs[0]->GetVector2();
  const Vector2& min = inputs[1]->GetVector2();
  const Vector2& max = inputs[2]->GetVector2();
  const Vector3& size = inputs[3]->GetVector3();

  position.x = WrapInDomain(position.x, min.x, max.x);
  position.y = WrapInDomain(position.y, min.y, max.y);

  Vector2 domainSize = (max - min) - size.GetVectorXY();

  relativePosition.x = domainSize.x > Math::MACHINE_EPSILON_1 ? fabsf((position.x - min.x) / domainSize.x) : 0.0f;
  relativePosition.y = domainSize.y > Math::MACHINE_EPSILON_1 ? fabsf((position.y - min.y) / domainSize.y) : 0.0f;
}

/**
 * Internal scroll domain Constraint
 * Generates the scroll domain of the scroll view.
 */
void InternalScrollDomainConstraint( Vector2& scrollDomain, const PropertyInputContainer& inputs)
{
  const Vector2& min = inputs[0]->GetVector2();
  const Vector2& max = inputs[1]->GetVector2();
  const Vector3& size = inputs[2]->GetVector3();

  scrollDomain = (max - min) - size.GetVectorXY();
}

/**
 * Internal maximum scroll position Constraint
 * Generates the maximum scroll position of the scroll view.
 */
void InternalPrePositionMaxConstraint( Vector2& scrollMax, const PropertyInputContainer& inputs)
{
  const Vector2& max = inputs[0]->GetVector2();
  const Vector3& size = inputs[1]->GetVector3();

  scrollMax = max - size.GetVectorXY();
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

BaseHandle Create()
{
  return Toolkit::ScrollView::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::ScrollView, Toolkit::Scrollable, Create )

DALI_PROPERTY_REGISTRATION( Toolkit, ScrollView, "wrapEnabled",                BOOLEAN,   WRAP_ENABLED                )
DALI_PROPERTY_REGISTRATION( Toolkit, ScrollView, "panningEnabled",             BOOLEAN,   PANNING_ENABLED             )
DALI_PROPERTY_REGISTRATION( Toolkit, ScrollView, "axisAutoLockEnabled",        BOOLEAN,   AXIS_AUTO_LOCK_ENABLED      )
DALI_PROPERTY_REGISTRATION( Toolkit, ScrollView, "wheelScrollDistanceStep",    VECTOR2,   WHEEL_SCROLL_DISTANCE_STEP  )

DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, ScrollView, "scrollPosition",  VECTOR2, SCROLL_POSITION)
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, ScrollView, "scrollPrePosition",   VECTOR2, SCROLL_PRE_POSITION)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( Toolkit, ScrollView, "scrollPrePositionX",    SCROLL_PRE_POSITION_X, SCROLL_PRE_POSITION, 0)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( Toolkit, ScrollView, "scrollPrePositionY",    SCROLL_PRE_POSITION_Y, SCROLL_PRE_POSITION, 1)
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, ScrollView, "scrollPrePositionMax",    VECTOR2, SCROLL_PRE_POSITION_MAX)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( Toolkit, ScrollView, "scrollPrePositionMaxX",     SCROLL_PRE_POSITION_MAX_X, SCROLL_PRE_POSITION_MAX, 0)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( Toolkit, ScrollView, "scrollPrePositionMaxY",     SCROLL_PRE_POSITION_MAX_Y, SCROLL_PRE_POSITION_MAX, 1)
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, ScrollView, "overshootX",  FLOAT, OVERSHOOT_X)
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, ScrollView, "overshootY",  FLOAT, OVERSHOOT_Y)
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, ScrollView, "scrollFinal",  VECTOR2, SCROLL_FINAL)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( Toolkit, ScrollView, "scrollFinalX",   SCROLL_FINAL_X, SCROLL_FINAL,0)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( Toolkit, ScrollView, "scrollFinalY",   SCROLL_FINAL_Y, SCROLL_FINAL,1)
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, ScrollView, "wrap", BOOLEAN, WRAP)
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, ScrollView, "panning", BOOLEAN, PANNING)
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, ScrollView, "scrolling", BOOLEAN, SCROLLING)
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, ScrollView, "scrollDomainSize",   VECTOR2, SCROLL_DOMAIN_SIZE)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( Toolkit, ScrollView, "scrollDomainSizeX",    SCROLL_DOMAIN_SIZE_X, SCROLL_DOMAIN_SIZE, 0)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( Toolkit, ScrollView, "scrollDomainSizeY",    SCROLL_DOMAIN_SIZE_Y, SCROLL_DOMAIN_SIZE, 1)
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, ScrollView, "scrollDomainOffset",   VECTOR2, SCROLL_DOMAIN_OFFSET)
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, ScrollView, "scrollPositionDelta",   VECTOR2, SCROLL_POSITION_DELTA)
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, ScrollView, "startPagePosition",   VECTOR3, START_PAGE_POSITION)

DALI_SIGNAL_REGISTRATION( Toolkit, ScrollView, "valueChanged",  SIGNAL_SNAP_STARTED )

DALI_TYPE_REGISTRATION_END()

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
  InternalPrePositionConstraint( const Vector2& initialPanPosition,
                                 const Vector2& initialPanMask,
                                 bool axisAutoLock,
                                 float axisAutoLockGradient,
                                 ScrollView::LockAxis initialLockAxis,
                                 const Vector2& maxOvershoot,
                                 const RulerPtr& rulerX, const RulerPtr& rulerY )
  : mLocalStart( initialPanPosition ),
    mInitialPanMask( initialPanMask ),
    mMaxOvershoot( maxOvershoot ),
    mAxisAutoLockGradient( axisAutoLockGradient ),
    mLockAxis( initialLockAxis ),
    mAxisAutoLock( axisAutoLock ),
    mWasPanning( false )
  {
    const RulerDomain& rulerDomainX = rulerX->GetDomain();
    const RulerDomain& rulerDomainY = rulerY->GetDomain();
    mDomainMin = Vector2( rulerDomainX.min, -rulerDomainY.min );
    mDomainMax = Vector2( -rulerDomainX.max, -rulerDomainY.max );
    mClampX = rulerDomainX.enabled;
    mClampY = rulerDomainY.enabled;
    mFixedRulerX = rulerX->GetType() == Ruler::Fixed;
    mFixedRulerY = rulerY->GetType() == Ruler::Fixed;
  }

  void operator()( Vector2& scrollPostPosition, const PropertyInputContainer& inputs )
  {
    const Vector2& panPosition = inputs[0]->GetVector2();
    const bool& inGesture = inputs[1]->GetBoolean();

    // First check if we are within a gesture.
    // The ScrollView may have received a start gesture from ::OnPan()
    // while the finish gesture is received now in this constraint.
    // This gesture must then be rejected as the value will be "old".
    // Typically the last value from the end of the last gesture.
    // If we are rejecting the gesture, we simply don't modify the constraint target.
    if( inGesture )
    {
      if( !mWasPanning )
      {
        mPrePosition = scrollPostPosition;
        mStartPosition = mPrePosition;
        mCurrentPanMask = mInitialPanMask;
        mWasPanning = true;
      }

      // Calculate Deltas...
      const Vector2& currentPosition = panPosition;
      Vector2 panDelta( currentPosition - mLocalStart );

      // Axis Auto Lock - locks the panning to the horizontal or vertical axis if the pan
      // appears mostly horizontal or mostly vertical respectively...
      if( mAxisAutoLock )
      {
        mLockAxis = GetLockAxis( panDelta, mLockAxis, mAxisAutoLockGradient );
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
      scrollPostPosition += panDelta;

      // if no wrapping then clamp preposition to maximum overshoot amount
      const Vector3& size = inputs[2]->GetVector3();
      if( mClampX )
      {
        float newXPosition = Clamp( scrollPostPosition.x, ( mDomainMax.x + size.x ) - mMaxOvershoot.x, mDomainMin.x + mMaxOvershoot.x );
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
        float newYPosition = Clamp( scrollPostPosition.y, ( mDomainMax.y + size.y ) - mMaxOvershoot.y, mDomainMin.y + mMaxOvershoot.y );
        if( ( newYPosition < scrollPostPosition.y - Math::MACHINE_EPSILON_1 )
          || ( newYPosition > scrollPostPosition.y + Math::MACHINE_EPSILON_1 ) )
        {
          mPrePosition.y = newYPosition;
          mLocalStart.y = panPosition.y;
        }
        scrollPostPosition.y = newYPosition;
      }

      // If we are using a fixed ruler in a particular axis, limit the maximum pages scrolled on that axis.
      if( mFixedRulerX || mFixedRulerY )
      {
        // Here we limit the maximum amount that can be moved from the starting position of the gesture to one page.
        // We do this only if we have a fixed ruler (on that axis) and the mode is enabled.
        // Note: 1.0f is subtracted to keep the value within one page size (otherwise we stray on to the page after).
        // Note: A further 1.0f is subtracted to handle a compensation that happens later within the flick handling code in SnapWithVelocity().
        //       When a flick is completed, an adjustment of 1.0f is sometimes made to allow for the scenario where:
        //       A flick finishes before the update thread has advanced the scroll position past the previous snap point.
        Vector2 pageSizeLimit( size.x - ( 1.0f + 1.0f ), size.y - ( 1.0f - 1.0f ) );
        Vector2 minPosition( mStartPosition.x - pageSizeLimit.x, mStartPosition.y - pageSizeLimit.y );
        Vector2 maxPosition( mStartPosition.x + pageSizeLimit.x, mStartPosition.y + pageSizeLimit.y );

        if( mFixedRulerX )
        {
          scrollPostPosition.x = Clamp( scrollPostPosition.x, minPosition.x, maxPosition.x );
        }
        if( mFixedRulerY )
        {
          scrollPostPosition.y = Clamp( scrollPostPosition.y, minPosition.y, maxPosition.y );
        }
      }
    }
  }

  Vector2 mPrePosition;
  Vector2 mLocalStart;
  Vector2 mStartPosition;               ///< The start position of the gesture - used to limit scroll amount (not modified by clamping).
  Vector2 mInitialPanMask;              ///< Initial pan mask (based on ruler settings).
  Vector2 mCurrentPanMask;              ///< Current pan mask that can be altered by axis lock mode.
  Vector2 mDomainMin;
  Vector2 mDomainMax;
  Vector2 mMaxOvershoot;

  float mAxisAutoLockGradient;          ///< Set by ScrollView
  ScrollView::LockAxis mLockAxis;

  bool mAxisAutoLock:1;                 ///< Set by ScrollView
  bool mWasPanning:1;
  bool mClampX:1;
  bool mClampY:1;
  bool mFixedRulerX:1;
  bool mFixedRulerY:1;
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

  void operator()( Vector2& position, const PropertyInputContainer& inputs )
  {
    position = inputs[0]->GetVector2();
    const Vector2& size = inputs[3]->GetVector3().GetVectorXY();
    const Vector2& min = inputs[1]->GetVector2();
    const Vector2& max = inputs[2]->GetVector2();

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
  }

  Vector2 mDomainMin;
  Vector2 mDomainMax;
  bool mClampX;
  bool mClampY;
  bool mWrap;

};

/**
 * This constraint updates the X overshoot property using the difference
 * SCROLL_PRE_POSITION.x and SCROLL_POSITION.x, returning a relative value between 0.0f and 1.0f
 */
struct OvershootXConstraint
{
  OvershootXConstraint(float maxOvershoot) : mMaxOvershoot(maxOvershoot) {}

  void operator()( float& current, const PropertyInputContainer& inputs )
  {
    if( inputs[2]->GetBoolean() )
    {
      const Vector2& scrollPrePosition = inputs[0]->GetVector2();
      const Vector2& scrollPostPosition = inputs[1]->GetVector2();
      float newOvershoot = scrollPrePosition.x - scrollPostPosition.x;
      current = (newOvershoot > 0.0f ? std::min(newOvershoot, mMaxOvershoot) : std::max(newOvershoot, -mMaxOvershoot)) / mMaxOvershoot;
    }
    else
    {
      current = 0.0f;
    }
  }

  float mMaxOvershoot;
};

/**
 * This constraint updates the Y overshoot property using the difference
 * SCROLL_PRE_POSITION.y and SCROLL_POSITION.y, returning a relative value between 0.0f and 1.0f
 */
struct OvershootYConstraint
{
  OvershootYConstraint(float maxOvershoot) : mMaxOvershoot(maxOvershoot) {}

  void operator()( float& current, const PropertyInputContainer& inputs )
  {
    if( inputs[2]->GetBoolean() )
    {
      const Vector2& scrollPrePosition = inputs[0]->GetVector2();
      const Vector2& scrollPostPosition = inputs[1]->GetVector2();
      float newOvershoot = scrollPrePosition.y - scrollPostPosition.y;
      current = (newOvershoot > 0.0f ? std::min(newOvershoot, mMaxOvershoot) : std::max(newOvershoot, -mMaxOvershoot)) / mMaxOvershoot;
    }
    else
    {
      current = 0.0f;
    }
  }

  float mMaxOvershoot;
};

/**
 * Internal Position-Delta Property Constraint.
 *
 * Generates position-delta property based on scroll-position + scroll-offset properties.
 */
void InternalPositionDeltaConstraint( Vector2& current, const PropertyInputContainer& inputs )
{
  const Vector2& scrollPosition = inputs[0]->GetVector2();
  const Vector2& scrollOffset = inputs[1]->GetVector2();

  current = scrollPosition + scrollOffset;
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
  InternalFinalConstraint(AlphaFunctionPrototype functionX,
                          AlphaFunctionPrototype functionY)
  : mFunctionX(functionX),
    mFunctionY(functionY)
  {
  }

  void operator()( Vector2& current, const PropertyInputContainer& inputs )
  {
    const float& overshootx = inputs[1]->GetFloat();
    const float& overshooty = inputs[2]->GetFloat();
    Vector2 offset( mFunctionX(overshootx),
                    mFunctionY(overshooty) );

    current = inputs[0]->GetVector2() - offset;
  }

  AlphaFunctionPrototype mFunctionX;
  AlphaFunctionPrototype mFunctionY;
};

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
: ScrollBase( ControlBehaviour( REQUIRES_WHEEL_EVENTS ) ),   // Enable size negotiation
  mTouchDownTime(0u),
  mGestureStackDepth(0),
  mScrollStateFlags(0),
  mLockAxis(LockPossible),
  mScrollUpdateDistance(DEFAULT_SCROLL_UPDATE_DISTANCE),
  mMaxOvershoot(DEFAULT_MAX_OVERSHOOT, DEFAULT_MAX_OVERSHOOT),
  mUserMaxOvershoot(DEFAULT_MAX_OVERSHOOT, DEFAULT_MAX_OVERSHOOT),
  mSnapOvershootDuration(DEFAULT_SNAP_OVERSHOOT_DURATION),
  mSnapOvershootAlphaFunction(AlphaFunction::EASE_OUT),
  mSnapDuration(DEFAULT_SLOW_SNAP_ANIMATION_DURATION),
  mSnapAlphaFunction(AlphaFunction::EASE_OUT),
  mMinFlickDistance(DEFAULT_MIN_FLICK_DISTANCE),
  mFlickSpeedThreshold(DEFAULT_MIN_FLICK_SPEED_THRESHOLD),
  mFlickDuration(DEFAULT_FAST_SNAP_ANIMATION_DURATION),
  mFlickAlphaFunction(AlphaFunction::EASE_OUT),
  mAxisAutoLockGradient(DEFAULT_AXIS_AUTO_LOCK_GRADIENT),
  mFrictionCoefficient(DEFAULT_FRICTION_COEFFICIENT),
  mFlickSpeedCoefficient(DEFAULT_FLICK_SPEED_COEFFICIENT),
  mMaxFlickSpeed(DEFAULT_MAX_FLICK_SPEED),
  mWheelScrollDistanceStep(Vector2::ZERO),
  mInAccessibilityPan(false),
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
}

void ScrollView::OnInitialize()
{
  Actor self = Self();

  // Internal Actor, used to hide actors from enumerations.
  // Also actors added to Internal actor appear as overlays e.g. ScrollBar components.
  mInternalActor = Actor::New();
  self.Add(mInternalActor);

  mInternalActor.SetParentOrigin(ParentOrigin::CENTER);
  mInternalActor.SetAnchorPoint(AnchorPoint::CENTER);
  mInternalActor.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

  mAlterChild = true;

  mScrollPostPosition = mScrollPrePosition = Vector2::ZERO;

  mWheelScrollDistanceStep = Stage::GetCurrent().GetSize() * DEFAULT_WHEEL_SCROLL_DISTANCE_STEP_PROPORTION;

  mGestureStackDepth = 0;

  self.TouchSignal().Connect( this, &ScrollView::OnTouch );
  EnableGestureDetection( Gesture::Type( Gesture::Pan ) );

  // By default we'll allow the user to freely drag the scroll view,
  // while disabling the other rulers.
  RulerPtr ruler = new DefaultRuler();
  mRulerX = ruler;
  mRulerY = ruler;

  self.SetProperty(Toolkit::Scrollable::Property::CAN_SCROLL_VERTICAL, mCanScrollVertical);
  self.SetProperty(Toolkit::Scrollable::Property::CAN_SCROLL_HORIZONTAL, mCanScrollHorizontal);

  UpdatePropertyDomain();
  SetInternalConstraints();
}

void ScrollView::OnStageConnection( int depth )
{
  ScrollBase::OnStageConnection( depth );

  DALI_LOG_SCROLL_STATE("[0x%X]", this);

  if ( mSensitive )
  {
    SetScrollSensitive( false );
    SetScrollSensitive( true );
  }
  if(IsOvershootEnabled())
  {
    // try and make sure property notifications are set
    EnableScrollOvershoot(true);
  }
}

void ScrollView::OnStageDisconnection()
{
  DALI_LOG_SCROLL_STATE("[0x%X]", this);

  StopAnimation();

  ScrollBase::OnStageDisconnection();
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

  UpdatePropertyDomain();
  UpdateMainInternalConstraint();
}

void ScrollView::SetRulerY(RulerPtr ruler)
{
  mRulerY = ruler;

  UpdatePropertyDomain();
  UpdateMainInternalConstraint();
}

void ScrollView::UpdatePropertyDomain()
{
  Actor self = Self();
  Vector3 size = self.GetTargetSize();
  Vector2 min = mMinScroll;
  Vector2 max = mMaxScroll;
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
    self.SetProperty(Toolkit::Scrollable::Property::CAN_SCROLL_VERTICAL, canScrollVertical);
  }
  if( mCanScrollHorizontal != canScrollHorizontal )
  {
    mCanScrollHorizontal = canScrollHorizontal;
    self.SetProperty(Toolkit::Scrollable::Property::CAN_SCROLL_HORIZONTAL, canScrollHorizontal);
  }
  if( scrollPositionChanged )
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Domain Changed, setting SCROLL_PRE_POSITION To[%.2f, %.2f]", this, mScrollPrePosition.x, mScrollPrePosition.y );
    self.SetProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, mScrollPrePosition);
  }
  if( domainChanged )
  {
    mMinScroll = min;
    mMaxScroll = max;
    self.SetProperty(Toolkit::Scrollable::Property::SCROLL_POSITION_MIN, mMinScroll );
    self.SetProperty(Toolkit::Scrollable::Property::SCROLL_POSITION_MAX, mMaxScroll );
  }
}

bool ScrollView::GetScrollSensitive()
{
  return mSensitive;
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

float ScrollView::GetSnapOvershootDuration()
{
  return mSnapOvershootDuration;
}

void ScrollView::SetSnapOvershootDuration(float duration)
{
  mSnapOvershootDuration = duration;
}

bool ScrollView::GetActorAutoSnap()
{
  return mActorAutoSnapEnabled;
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
  Self().SetProperty(Toolkit::ScrollView::Property::WRAP, enable);
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

void ScrollView::SetWheelScrollDistanceStep(Vector2 step)
{
  mWheelScrollDistanceStep = step;
}

Vector2 ScrollView::GetWheelScrollDistanceStep() const
{
  return mWheelScrollDistanceStep;
}

unsigned int ScrollView::GetCurrentPage() const
{
  // in case animation is currently taking place.
  Vector2 position = GetPropertyPosition();

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

Vector2 ScrollView::GetCurrentScrollPosition() const
{
  return -GetPropertyPosition();
}

void ScrollView::TransformTo(const Vector2& position,
                             DirectionBias horizontalBias, DirectionBias verticalBias)
{
  TransformTo(position, mSnapDuration, mSnapAlphaFunction, horizontalBias, verticalBias);
}

void ScrollView::TransformTo(const Vector2& position, float duration, AlphaFunction alpha,
                             DirectionBias horizontalBias, DirectionBias verticalBias)
{
  // If this is called while the timer is running, then cancel it
  StopTouchDownTimer();

  Actor self( Self() );

  // Guard against destruction during signal emission
  // Note that Emit() methods are called indirectly e.g. from within ScrollView::AnimateTo()
  Toolkit::ScrollView handle( GetOwner() );

  DALI_LOG_SCROLL_STATE("[0x%X] pos[%.2f,%.2f], duration[%.2f] bias[%d, %d]",
    this, position.x, position.y, duration, int(horizontalBias), int(verticalBias));

  Vector2 currentScrollPosition = GetCurrentScrollPosition();
  self.SetProperty( Toolkit::ScrollView::Property::START_PAGE_POSITION, Vector3(currentScrollPosition) );

  if( mScrolling ) // are we interrupting a current scroll?
  {
    // set mScrolling to false, in case user has code that interrogates mScrolling Getter() in complete.
    mScrolling = false;
    DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignal 1 [%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y);
    mScrollCompletedSignal.Emit( currentScrollPosition );
  }

  if( mPanning ) // are we interrupting a current pan?
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Interrupting Pan, set to false", this );
    mPanning = false;
    mGestureStackDepth = 0;
    self.SetProperty( Toolkit::ScrollView::Property::PANNING, false );

    if( mScrollMainInternalPrePositionConstraint )
    {
      mScrollMainInternalPrePositionConstraint.Remove();
    }
  }

  self.SetProperty(Toolkit::ScrollView::Property::SCROLLING, true);
  mScrolling = true;

  DALI_LOG_SCROLL_STATE("[0x%X] mScrollStartedSignal 1 [%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y);
  mScrollStartedSignal.Emit( currentScrollPosition );
  bool animating = AnimateTo(-position,
                             Vector2::ONE * duration,
                             alpha,
                             true,
                             horizontalBias,
                             verticalBias,
                             Snap);

  if(!animating)
  {
    // if not animating, then this pan has completed right now.
    self.SetProperty(Toolkit::ScrollView::Property::SCROLLING, false);
    mScrolling = false;

    // If we have no duration, then in the next update frame, we will be at the position specified as we just set.
    // In this scenario, we cannot return the currentScrollPosition as this is out-of-date and should instead return the requested final position
    Vector2 completedPosition( currentScrollPosition );
    if( duration <= Math::MACHINE_EPSILON_10 )
    {
      completedPosition = position;
    }

    DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignal 2 [%.2f, %.2f]", this, completedPosition.x, completedPosition.y);
    SetScrollUpdateNotification(false);
    mScrollCompletedSignal.Emit( completedPosition );
  }
}

void ScrollView::ScrollTo(const Vector2& position)
{
  ScrollTo(position, mSnapDuration );
}

void ScrollView::ScrollTo(const Vector2& position, float duration)
{
  ScrollTo(position, duration, DirectionBiasNone, DirectionBiasNone);
}

void ScrollView::ScrollTo(const Vector2& position, float duration, AlphaFunction alpha)
{
  ScrollTo(position, duration, alpha, DirectionBiasNone, DirectionBiasNone);
}

void ScrollView::ScrollTo(const Vector2& position, float duration,
                          DirectionBias horizontalBias, DirectionBias verticalBias)
{
  ScrollTo(position, duration, mSnapAlphaFunction, horizontalBias, verticalBias);
}

void ScrollView::ScrollTo(const Vector2& position, float duration, AlphaFunction alpha,
                DirectionBias horizontalBias, DirectionBias verticalBias)
{
  DALI_LOG_SCROLL_STATE("[0x%X] position[%.2f, %.2f] duration[%.2f], bias[%d, %d]", this, position.x, position.y, duration, int(horizontalBias), int(verticalBias));
  TransformTo(position, duration, alpha, horizontalBias, verticalBias);
}

void ScrollView::ScrollTo(unsigned int page)
{
  ScrollTo(page, mSnapDuration);
}

void ScrollView::ScrollTo(unsigned int page, float duration, DirectionBias bias)
{
  Vector2 position;
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
  Vector2 prePosition = GetPropertyPrePosition();
  position.GetVectorXY() -= prePosition;

  ScrollTo(Vector2(position.x - size.width * 0.5f, position.y - size.height * 0.5f), duration);
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
  Vector2 positionDuration = Vector2::ONE * mSnapDuration;
  float biasX = 0.5f;
  float biasY = 0.5f;
  FindDirection horizontal = None;
  FindDirection vertical = None;

  // orthoAngleRange = Angle tolerance within the Exact N,E,S,W direction
  // that will be accepted as a general N,E,S,W flick direction.

  const float orthoAngleRange = FLICK_ORTHO_ANGLE_RANGE * M_PI / 180.0f;
  const float flickSpeedThreshold2 = mFlickSpeedThreshold * mFlickSpeedThreshold;

  Vector2 positionSnap = mScrollPrePosition;

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
    positionDuration = Vector2::ONE * mFlickDuration;
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
      Vector2 position = Self().GetProperty<Vector2>(Toolkit::ScrollView::Property::SCROLL_POSITION);

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

  Vector2 startPosition = positionSnap;
  positionSnap.x = -mRulerX->Snap(-positionSnap.x, biasX);  // NOTE: X & Y rulers think in -ve coordinate system.
  positionSnap.y = -mRulerY->Snap(-positionSnap.y, biasY);  // That is scrolling RIGHT (e.g. 100.0, 0.0) means moving LEFT.

  Vector2 clampDelta(Vector2::ZERO);
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
      clampDelta = Vector2::ZERO;
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

  if(IsOvershootEnabled())
  {
    // Scroll to the end of the overshoot only when overshoot is enabled.
    positionSnap += clampDelta;
  }

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

bool ScrollView::AnimateTo(const Vector2& position, const Vector2& positionDuration,
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
    UpdateMainInternalConstraint();
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
      DALI_LOG_SCROLL_STATE("[0x%X] Setting SCROLL_PRE_POSITION To[%.2f, %.2f]", this, mScrollTargetPosition.x, mScrollTargetPosition.y );
      self.SetProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, mScrollTargetPosition);
      mScrollPrePosition = mScrollTargetPosition;
      mScrollPostPosition = mScrollTargetPosition;
      WrapPosition(mScrollPostPosition);
    }

    DALI_LOG_SCROLL_STATE("[0x%X] position-changed, mScrollTargetPosition[%.2f, %.2f], mScrollPrePosition[%.2f, %.2f], mScrollPostPosition[%.2f, %.2f]", this, mScrollTargetPosition.x, mScrollTargetPosition.y, mScrollPrePosition.x, mScrollPrePosition.y, mScrollPostPosition.x, mScrollPostPosition.y );
    DALI_LOG_SCROLL_STATE("[0x%X] SCROLL_PRE_POSITION[%.2f, %.2f], SCROLL_POSITION[%.2f, %.2f]", this, self.GetProperty( Toolkit::ScrollView::Property::SCROLL_PRE_POSITION ).Get<Vector2>().x, self.GetProperty( Toolkit::ScrollView::Property::SCROLL_PRE_POSITION ).Get<Vector2>().y, self.GetProperty( Toolkit::ScrollView::Property::SCROLL_POSITION ).Get<Vector2>().x, self.GetProperty( Toolkit::ScrollView::Property::SCROLL_POSITION ).Get<Vector2>().y );
  }

  SetScrollUpdateNotification(true);

  // Always send a snap event when AnimateTo is called.
  Toolkit::ScrollView::SnapEvent snapEvent;
  snapEvent.type = snapType;
  snapEvent.position = -mScrollTargetPosition;
  snapEvent.duration = totalDuration;

  DALI_LOG_SCROLL_STATE("[0x%X] mSnapStartedSignal [%.2f, %.2f]", this, snapEvent.position.x, snapEvent.position.y);
  mSnapStartedSignal.Emit( snapEvent );

  return (mScrollStateFlags & SCROLL_ANIMATION_FLAGS) != 0;
}

void ScrollView::EnableScrollOvershoot(bool enable)
{
  if (enable)
  {
    if (!mOvershootIndicator)
    {
      mOvershootIndicator = ScrollOvershootIndicator::New();
    }

    mOvershootIndicator->AttachToScrollable(*this);
  }
  else
  {
    mMaxOvershoot = mUserMaxOvershoot;

    if (mOvershootIndicator)
    {
      mOvershootIndicator->DetachFromScrollable(*this);
    }
  }

  UpdateMainInternalConstraint();
}

void ScrollView::AddOverlay(Actor actor)
{
  actor.SetDrawMode( DrawMode::OVERLAY_2D );
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

Toolkit::ScrollView::SnapStartedSignalType& ScrollView::SnapStartedSignal()
{
  return mSnapStartedSignal;
}

void ScrollView::FindAndUnbindActor(Actor child)
{
  UnbindActor(child);
}

Vector2 ScrollView::GetPropertyPrePosition() const
{
  Vector2 position = Self().GetProperty<Vector2>(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION);
  WrapPosition(position);
  return position;
}

Vector2 ScrollView::GetPropertyPosition() const
{
  Vector2 position = Self().GetProperty<Vector2>(Toolkit::ScrollView::Property::SCROLL_POSITION);
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
  self.SetProperty(Toolkit::ScrollView::Property::SCROLLING, false);

  Vector2 deltaPosition(mScrollPrePosition);

  UpdateLocalScrollProperties();
  WrapPosition(mScrollPrePosition);
  DALI_LOG_SCROLL_STATE("[0x%X] Setting SCROLL_PRE_POSITION To[%.2f, %.2f]", this, mScrollPrePosition.x, mScrollPrePosition.y );
  self.SetProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, mScrollPrePosition);

  Vector2 currentScrollPosition = GetCurrentScrollPosition();
  DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignal 3 current[%.2f, %.2f], mScrollTargetPosition[%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y, -mScrollTargetPosition.x, -mScrollTargetPosition.y );
  mScrollCompletedSignal.Emit( currentScrollPosition );

  mDomainOffset += deltaPosition - mScrollPostPosition;
  self.SetProperty(Toolkit::ScrollView::Property::SCROLL_DOMAIN_OFFSET, mDomainOffset);
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
  if( enabled && !mScrollUpdatedSignal.Empty())
  {
    // Only set up the notification when the application has connected to the updated signal
    mScrollXUpdateNotification = self.AddPropertyNotification(Toolkit::ScrollView::Property::SCROLL_POSITION, 0, StepCondition(mScrollUpdateDistance, 0.0f));
    mScrollXUpdateNotification.NotifySignal().Connect( this, &ScrollView::OnScrollUpdateNotification );
  }
  if( mScrollYUpdateNotification )
  {
    // disconnect now to avoid a notification before removed from update thread
    mScrollYUpdateNotification.NotifySignal().Disconnect(this, &ScrollView::OnScrollUpdateNotification);
    self.RemovePropertyNotification(mScrollYUpdateNotification);
    mScrollYUpdateNotification.Reset();
  }
  if( enabled && !mScrollUpdatedSignal.Empty())
  {
    // Only set up the notification when the application has connected to the updated signal
    mScrollYUpdateNotification = self.AddPropertyNotification(Toolkit::ScrollView::Property::SCROLL_POSITION, 1, StepCondition(mScrollUpdateDistance, 0.0f));
    mScrollYUpdateNotification.NotifySignal().Connect( this, &ScrollView::OnScrollUpdateNotification );
  }
}

void ScrollView::OnScrollUpdateNotification(Dali::PropertyNotification& source)
{
  // Guard against destruction during signal emission
  Toolkit::ScrollView handle( GetOwner() );

  Vector2 currentScrollPosition = GetCurrentScrollPosition();
  mScrollUpdatedSignal.Emit( currentScrollPosition );
}

bool ScrollView::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::ScrollView view = Toolkit::ScrollView::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), SIGNAL_SNAP_STARTED ) )
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
  UpdatePropertyDomain();
}

void ScrollView::OnSizeSet( const Vector3& size )
{
  // need to update domain properties for new size
  if( mDefaultMaxOvershoot )
  {
    mUserMaxOvershoot.x = size.x * 0.5f;
    mUserMaxOvershoot.y = size.y * 0.5f;
    if( !IsOvershootEnabled() )
    {
      mMaxOvershoot = mUserMaxOvershoot;
    }
  }
  UpdatePropertyDomain();
  UpdateMainInternalConstraint();
  if( IsOvershootEnabled() )
  {
    mOvershootIndicator->Reset();
  }
}

void ScrollView::OnChildAdd(Actor& child)
{
  ScrollBase::OnChildAdd( child );

  Dali::Toolkit::ScrollBar scrollBar = Dali::Toolkit::ScrollBar::DownCast(child);
  if(scrollBar)
  {
    mInternalActor.Add(scrollBar);
    if(scrollBar.GetScrollDirection() == Toolkit::ScrollBar::Horizontal)
    {
      scrollBar.SetScrollPropertySource(Self(),
                                        Toolkit::ScrollView::Property::SCROLL_PRE_POSITION_X,
                                        Toolkit::Scrollable::Property::SCROLL_POSITION_MIN_X,
                                        Toolkit::ScrollView::Property::SCROLL_PRE_POSITION_MAX_X,
                                        Toolkit::ScrollView::Property::SCROLL_DOMAIN_SIZE_X);
    }
    else
    {
      scrollBar.SetScrollPropertySource(Self(),
                                        Toolkit::ScrollView::Property::SCROLL_PRE_POSITION_Y,
                                        Toolkit::Scrollable::Property::SCROLL_POSITION_MIN_Y,
                                        Toolkit::ScrollView::Property::SCROLL_PRE_POSITION_MAX_Y,
                                        Toolkit::ScrollView::Property::SCROLL_DOMAIN_SIZE_Y);
    }
  }
  else if(mAlterChild)
  {
    BindActor(child);
  }
}

void ScrollView::OnChildRemove(Actor& child)
{
  // TODO: Actor needs a RemoveConstraint method to take out an individual constraint.
  UnbindActor(child);

  ScrollBase::OnChildRemove( child );
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
      mDomainOffset = Vector2::ZERO;
      Self().SetProperty(Toolkit::ScrollView::Property::SCROLL_DOMAIN_OFFSET, Vector2::ZERO);

      UpdateLocalScrollProperties();
      Vector2 currentScrollPosition = GetCurrentScrollPosition();
      DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignal 4 [%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y);
      mScrollCompletedSignal.Emit( currentScrollPosition );
    }
  }

  return false;
}

bool ScrollView::OnTouch( Actor actor, const TouchData& touch )
{
  if(!mSensitive)
  {
    DALI_LOG_SCROLL_STATE("[0x%X], Not Sensitive, ignoring", this);

    // Ignore this touch event, if scrollview is insensitive.
    return false;
  }

  // Ignore events with multiple-touch points
  if (touch.GetPointCount() != 1)
  {
    DALI_LOG_SCROLL_STATE("[0x%X], multiple touch, ignoring", this);

    return false;
  }

  const PointState::Type pointState = touch.GetState( 0 );
  if( pointState == PointState::DOWN )
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Down", this);

    if(mGestureStackDepth==0)
    {
      mTouchDownTime = touch.GetTime();

      // This allows time for a pan-gesture to start, to avoid breaking snap-animation behavior with fast flicks.
      // If touch-down does not become a pan (after timeout interval), then snap-animation can be interrupted.
      mTouchDownTimeoutReached = false;
      mScrollInterrupted = false;
      StartTouchDownTimer();
    }
  }
  else if( ( pointState == PointState::UP ) ||
           ( ( pointState == PointState::INTERRUPTED ) && ( touch.GetHitActor( 0 )== Self() ) ) )
  {
    DALI_LOG_SCROLL_STATE("[0x%X] %s", this, ( ( pointState == TouchPoint::Up ) ? "Up" : "Interrupted" ) );

    StopTouchDownTimer();

    // if the user touches and releases without enough movement to go
    // into a gesture state, then we should snap to nearest point.
    // otherwise our scroll could be stopped (interrupted) half way through an animation.
    if(mGestureStackDepth==0 && mTouchDownTimeoutReached)
    {
      if( ( pointState == PointState::INTERRUPTED ) ||
          ( ( touch.GetTime() - mTouchDownTime ) >= MINIMUM_TIME_BETWEEN_DOWN_AND_UP_FOR_RESET ) )
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

bool ScrollView::OnWheelEvent(const WheelEvent& event)
{
  if(!mSensitive)
  {
    // Ignore this wheel event, if scrollview is insensitive.
    return false;
  }

  Vector2 targetScrollPosition = GetPropertyPosition();

  if(mRulerX->IsEnabled() && !mRulerY->IsEnabled())
  {
    // If only the ruler in the X axis is enabled, scroll in the X axis.
    if(mRulerX->GetType() == Ruler::Free)
    {
      // Free panning mode
      targetScrollPosition.x += event.z * mWheelScrollDistanceStep.x;
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
      targetScrollPosition.y += event.z * mWheelScrollDistanceStep.y;
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
  self.GetProperty(Toolkit::ScrollView::Property::SCROLL_POSITION).Get(mScrollPostPosition);
  mScrollPrePosition = mScrollPostPosition;
  DALI_LOG_SCROLL_STATE("[0x%X] Setting SCROLL_PRE_POSITION To[%.2f, %.2f]", this, mScrollPostPosition.x, mScrollPostPosition.y );
  self.SetProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, mScrollPostPosition);
}

void ScrollView::UpdateLocalScrollProperties()
{
  Actor self = Self();
  self.GetProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION).Get(mScrollPrePosition);
  self.GetProperty(Toolkit::ScrollView::Property::SCROLL_POSITION).Get(mScrollPostPosition);
}

// private functions

void ScrollView::PreAnimatedScrollSetup()
{
  // SCROLL_PRE_POSITION is our unclamped property with wrapping
  // SCROLL_POSITION is our final scroll position after clamping

  Actor self = Self();

  Vector2 deltaPosition(mScrollPostPosition);
  WrapPosition(mScrollPostPosition);
  mDomainOffset += deltaPosition - mScrollPostPosition;
  Self().SetProperty(Toolkit::ScrollView::Property::SCROLL_DOMAIN_OFFSET, mDomainOffset);

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
    DALI_LOG_SCROLL_STATE("[0x%X], Animating from[%.2f] to[%.2f]", this, self.GetProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION).Get<Vector2>().x, position );
    mInternalXAnimation = Animation::New(duration);
    DALI_LOG_SCROLL_STATE("[0x%X], mInternalXAnimation[0x%X]", this, mInternalXAnimation.GetObjectPtr() );
    mInternalXAnimation.FinishedSignal().Connect(this, &ScrollView::OnScrollAnimationFinished);
    mInternalXAnimation.AnimateTo( Property(self, Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, 0), position, alpha, TimePeriod(duration));
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
    DALI_LOG_SCROLL_STATE("[0x%X], Animating from[%.2f] to[%.2f]", this, self.GetProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION).Get<Vector2>().y, position );
    mInternalYAnimation = Animation::New(duration);
    DALI_LOG_SCROLL_STATE("[0x%X], mInternalYAnimation[0x%X]", this, mInternalYAnimation.GetObjectPtr() );
    mInternalYAnimation.FinishedSignal().Connect(this, &ScrollView::OnScrollAnimationFinished);
    mInternalYAnimation.AnimateTo( Property(self, Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, 1), position, alpha, TimePeriod(duration));
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
    DALI_LOG_SCROLL_STATE("[0x%X] mInternalXAnimation[0x%X], expected[%.2f], actual[%.2f], post[%.2f]", this, mInternalXAnimation.GetObjectPtr(), mScrollTargetPosition.x, Self().GetProperty(SCROLL_PRE_POSITION).Get<Vector2>().x, mScrollPostPosition.x );

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
      DALI_LOG_SCROLL_STATE("[0x%X] Setting SCROLL_PRE_POSITION To[%.2f, %.2f]", this, mScrollPrePosition.x, mScrollPrePosition.y );
      handle.SetProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, mScrollPrePosition);
    }
    SnapInternalXTo(mScrollPostPosition.x);
  }

  if( source == mInternalYAnimation )
  {
    DALI_LOG_SCROLL_STATE("[0x%X] mInternalYAnimation[0x%X], expected[%.2f], actual[%.2f], post[%.2f]", this, mInternalYAnimation.GetObjectPtr(), mScrollTargetPosition.y, Self().GetProperty(SCROLL_PRE_POSITION).Get<Vector2>().y, mScrollPostPosition.y );

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
      DALI_LOG_SCROLL_STATE("[0x%X] Setting SCROLL_PRE_POSITION To[%.2f, %.2f]", this, mScrollPrePosition.x, mScrollPrePosition.y );
      handle.SetProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, mScrollPrePosition);
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
    mInternalXAnimation.AnimateTo(Property(self, Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, 0), position);
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
    mInternalYAnimation.AnimateTo(Property(self, Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, 1), position);
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
    mPanDelta = Vector2::ZERO;
    mLastVelocity = Vector2::ZERO;
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
      DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignal 5 [%.2f, %.2f]", this, -mScrollPostPosition.x, -mScrollPostPosition.y);
      mScrollCompletedSignal.Emit( -mScrollPostPosition );
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
    mLockAxis = GetLockAxis(mPanDelta, mLockAxis, mAxisAutoLockGradient);
  } // end if mAxisAutoLock
}

// TODO: Upgrade to use a more powerful gesture detector (one that supports multiple touches on pan - so works as pan and flick gesture)
// BUG: Gesture::Finished doesn't always return velocity on release (due to
// timeDelta between last two events being 0 sometimes, or posiiton being the same)
void ScrollView::OnPan( const PanGesture& gesture )
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
      self.SetProperty( Toolkit::ScrollView::Property::PANNING, true );
      self.SetProperty( Toolkit::ScrollView::Property::START_PAGE_POSITION, Vector3(gesture.position.x, gesture.position.y, 0.0f) );

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
        self.SetProperty( Toolkit::ScrollView::Property::PANNING, false );

        if( mScrollMainInternalPrePositionConstraint )
        {
          mScrollMainInternalPrePositionConstraint.Remove();
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
    Vector2 currentScrollPosition = GetCurrentScrollPosition();
    Self().SetProperty(Toolkit::ScrollView::Property::SCROLLING, true);
    mScrolling = true;
    DALI_LOG_SCROLL_STATE("[0x%X] mScrollStartedSignal 2 [%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y);
    mScrollStartedSignal.Emit( currentScrollPosition );
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
    Self().SetProperty(Toolkit::ScrollView::Property::SCROLLING, false);

    if( fabs(mScrollPrePosition.x - mScrollTargetPosition.x) > Math::MACHINE_EPSILON_10 )
    {
      SnapInternalXTo(mScrollTargetPosition.x);
    }
    if( fabs(mScrollPrePosition.y - mScrollTargetPosition.y) > Math::MACHINE_EPSILON_10 )
    {
      SnapInternalYTo(mScrollTargetPosition.y);
    }
    Vector2 currentScrollPosition = GetCurrentScrollPosition();
    DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignal 6 [%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y);
    mScrollCompletedSignal.Emit( currentScrollPosition );
  }
}

Vector2 ScrollView::GetOvershoot(Vector2& position) const
{
  Vector3 size = Self().GetCurrentSize();
  Vector2 overshoot;

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

void ScrollView::ClampPosition(Vector2& position) const
{
  ClampState2D clamped;
  ClampPosition(position, clamped);
}

void ScrollView::ClampPosition(Vector2& position, ClampState2D &clamped) const
{
  Vector3 size = Self().GetCurrentSize();

  position.x = -mRulerX->Clamp(-position.x, size.width, 1.0f, clamped.x);    // NOTE: X & Y rulers think in -ve coordinate system.
  position.y = -mRulerY->Clamp(-position.y, size.height, 1.0f, clamped.y);   // That is scrolling RIGHT (e.g. 100.0, 0.0) means moving LEFT.
}

void ScrollView::WrapPosition(Vector2& position) const
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
    mScrollMainInternalPositionConstraint.Remove();
    mScrollMainInternalDeltaConstraint.Remove();
    mScrollMainInternalFinalConstraint.Remove();
    mScrollMainInternalRelativeConstraint.Remove();
    mScrollMainInternalDomainConstraint.Remove();
    mScrollMainInternalPrePositionMaxConstraint.Remove();
  }
  if( mScrollMainInternalPrePositionConstraint )
  {
    mScrollMainInternalPrePositionConstraint.Remove();
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

  if( mPanning )
  {
    mScrollMainInternalPrePositionConstraint = Constraint::New<Vector2>( self,
                                                                         Toolkit::ScrollView::Property::SCROLL_PRE_POSITION,
                                                                         InternalPrePositionConstraint( mPanStartPosition,
                                                                                                        initialPanMask,
                                                                                                        mAxisAutoLock,
                                                                                                        mAxisAutoLockGradient,
                                                                                                        mLockAxis,
                                                                                                        mMaxOvershoot,
                                                                                                        mRulerX,
                                                                                                        mRulerY ) );
    mScrollMainInternalPrePositionConstraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_POSITION ) );
    mScrollMainInternalPrePositionConstraint.AddSource( Source( detector, PanGestureDetector::Property::PANNING ) );
    mScrollMainInternalPrePositionConstraint.AddSource( Source( self, Actor::Property::SIZE ) );
    mScrollMainInternalPrePositionConstraint.Apply();
  }

  // 2. Second calculate the clamped position (actual position)
  mScrollMainInternalPositionConstraint = Constraint::New<Vector2>( self,
                                                                    Toolkit::ScrollView::Property::SCROLL_POSITION,
                                                                    InternalPositionConstraint( mRulerX->GetDomain(),
                                                                                                mRulerY->GetDomain(),
                                                                                                mWrapMode ) );
  mScrollMainInternalPositionConstraint.AddSource( LocalSource( Toolkit::ScrollView::Property::SCROLL_PRE_POSITION ) );
  mScrollMainInternalPositionConstraint.AddSource( LocalSource( Toolkit::Scrollable::Property::SCROLL_POSITION_MIN ) );
  mScrollMainInternalPositionConstraint.AddSource( LocalSource( Toolkit::Scrollable::Property::SCROLL_POSITION_MAX ) );
  mScrollMainInternalPositionConstraint.AddSource( Source( self, Actor::Property::SIZE ) );
  mScrollMainInternalPositionConstraint.Apply();

  mScrollMainInternalDeltaConstraint = Constraint::New<Vector2>( self, Toolkit::ScrollView::Property::SCROLL_POSITION_DELTA, InternalPositionDeltaConstraint );
  mScrollMainInternalDeltaConstraint.AddSource( LocalSource( Toolkit::ScrollView::Property::SCROLL_POSITION ) );
  mScrollMainInternalDeltaConstraint.AddSource( LocalSource( Toolkit::ScrollView::Property::SCROLL_DOMAIN_OFFSET ) );
  mScrollMainInternalDeltaConstraint.Apply();

  mScrollMainInternalFinalConstraint = Constraint::New<Vector2>( self, Toolkit::ScrollView::Property::SCROLL_FINAL,
                                                                 InternalFinalConstraint( FinalDefaultAlphaFunction,
                                                                                          FinalDefaultAlphaFunction ) );
  mScrollMainInternalFinalConstraint.AddSource( LocalSource( Toolkit::ScrollView::Property::SCROLL_POSITION ) );
  mScrollMainInternalFinalConstraint.AddSource( LocalSource( Toolkit::ScrollView::Property::OVERSHOOT_X ) );
  mScrollMainInternalFinalConstraint.AddSource( LocalSource( Toolkit::ScrollView::Property::OVERSHOOT_Y ) );
  mScrollMainInternalFinalConstraint.Apply();

  mScrollMainInternalRelativeConstraint = Constraint::New<Vector2>( self, Toolkit::Scrollable::Property::SCROLL_RELATIVE_POSITION, InternalRelativePositionConstraint );
  mScrollMainInternalRelativeConstraint.AddSource( LocalSource( Toolkit::ScrollView::Property::SCROLL_POSITION ) );
  mScrollMainInternalRelativeConstraint.AddSource( LocalSource( Toolkit::Scrollable::Property::SCROLL_POSITION_MIN ) );
  mScrollMainInternalRelativeConstraint.AddSource( LocalSource( Toolkit::Scrollable::Property::SCROLL_POSITION_MAX ) );
  mScrollMainInternalRelativeConstraint.AddSource( LocalSource( Actor::Property::SIZE ) );
  mScrollMainInternalRelativeConstraint.Apply();

  mScrollMainInternalDomainConstraint = Constraint::New<Vector2>( self, Toolkit::ScrollView::Property::SCROLL_DOMAIN_SIZE, InternalScrollDomainConstraint );
  mScrollMainInternalDomainConstraint.AddSource( LocalSource( Toolkit::Scrollable::Property::SCROLL_POSITION_MIN ) );
  mScrollMainInternalDomainConstraint.AddSource( LocalSource( Toolkit::Scrollable::Property::SCROLL_POSITION_MAX ) );
  mScrollMainInternalDomainConstraint.AddSource( LocalSource( Actor::Property::SIZE ) );
  mScrollMainInternalDomainConstraint.Apply();

  mScrollMainInternalPrePositionMaxConstraint = Constraint::New<Vector2>( self, Toolkit::ScrollView::Property::SCROLL_PRE_POSITION_MAX, InternalPrePositionMaxConstraint );
  mScrollMainInternalPrePositionMaxConstraint.AddSource( LocalSource( Toolkit::Scrollable::Property::SCROLL_POSITION_MAX ) );
  mScrollMainInternalPrePositionMaxConstraint.AddSource( LocalSource( Actor::Property::SIZE ) );
  mScrollMainInternalPrePositionMaxConstraint.Apply();

  // When panning we want to make sure overshoot values are affected by pre position and post position
  SetOvershootConstraintsEnabled(!mWrapMode);
}

void ScrollView::SetOvershootConstraintsEnabled(bool enabled)
{
  Actor self( Self() );
  // remove and reset, it may now be in wrong order with the main internal constraints
  if( mScrollMainInternalOvershootXConstraint )
  {
    mScrollMainInternalOvershootXConstraint.Remove();
    mScrollMainInternalOvershootXConstraint.Reset();
    mScrollMainInternalOvershootYConstraint.Remove();
    mScrollMainInternalOvershootYConstraint.Reset();
  }
  if( enabled )
  {
    mScrollMainInternalOvershootXConstraint= Constraint::New<float>( self, Toolkit::ScrollView::Property::OVERSHOOT_X, OvershootXConstraint(mMaxOvershoot.x) );
    mScrollMainInternalOvershootXConstraint.AddSource( LocalSource( Toolkit::ScrollView::Property::SCROLL_PRE_POSITION ) );
    mScrollMainInternalOvershootXConstraint.AddSource( LocalSource( Toolkit::ScrollView::Property::SCROLL_POSITION ) );
    mScrollMainInternalOvershootXConstraint.AddSource( LocalSource( Toolkit::Scrollable::Property::CAN_SCROLL_HORIZONTAL ) );
    mScrollMainInternalOvershootXConstraint.Apply();

    mScrollMainInternalOvershootYConstraint = Constraint::New<float>( self, Toolkit::ScrollView::Property::OVERSHOOT_Y, OvershootYConstraint(mMaxOvershoot.y) );
    mScrollMainInternalOvershootYConstraint.AddSource( LocalSource( Toolkit::ScrollView::Property::SCROLL_PRE_POSITION ) );
    mScrollMainInternalOvershootYConstraint.AddSource( LocalSource( Toolkit::ScrollView::Property::SCROLL_POSITION ) );
    mScrollMainInternalOvershootYConstraint.AddSource( LocalSource( Toolkit::Scrollable::Property::CAN_SCROLL_VERTICAL ) );
    mScrollMainInternalOvershootYConstraint.Apply();
  }
  else
  {
    self.SetProperty(Toolkit::ScrollView::Property::OVERSHOOT_X, 0.0f);
    self.SetProperty(Toolkit::ScrollView::Property::OVERSHOOT_Y, 0.0f);
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
  constraint = Constraint::New<Vector3>( self, Actor::Property::POSITION, MoveActorConstraint );
  constraint.AddSource( Source( self, Toolkit::ScrollView::Property::SCROLL_POSITION ) );
  constraint.SetRemoveAction(Constraint::Discard);
  ApplyConstraintToBoundActors(constraint);

  // WrapActor (wrap functionality)
  constraint = Constraint::New<Vector3>( self, Actor::Property::POSITION, WrapActorConstraint );
  constraint.AddSource( LocalSource( Actor::Property::SCALE ) );
  constraint.AddSource( LocalSource( Actor::Property::ANCHOR_POINT ) );
  constraint.AddSource( LocalSource( Actor::Property::SIZE ) );
  constraint.AddSource( Source( self, Toolkit::Scrollable::Property::SCROLL_POSITION_MIN ) );
  constraint.AddSource( Source( self, Toolkit::Scrollable::Property::SCROLL_POSITION_MAX ) );
  constraint.AddSource( Source( self, Toolkit::ScrollView::Property::WRAP ) );
  constraint.SetRemoveAction(Constraint::Discard);
  ApplyConstraintToBoundActors(constraint);
}

void ScrollView::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::ScrollView scrollView = Toolkit::ScrollView::DownCast( Dali::BaseHandle( object ) );

  if( scrollView )
  {
    ScrollView& scrollViewImpl( GetImpl( scrollView ) );
    switch( index )
    {
      case Toolkit::ScrollView::Property::WRAP_ENABLED:
      {
        scrollViewImpl.SetWrapMode( value.Get<bool>() );
        break;
      }
      case Toolkit::ScrollView::Property::PANNING_ENABLED:
      {
        scrollViewImpl.SetScrollSensitive( value.Get<bool>() );
        break;
      }
      case Toolkit::ScrollView::Property::AXIS_AUTO_LOCK_ENABLED:
      {
        scrollViewImpl.SetAxisAutoLock( value.Get<bool>() );
        break;
      }
      case Toolkit::ScrollView::Property::WHEEL_SCROLL_DISTANCE_STEP:
      {
        scrollViewImpl.SetWheelScrollDistanceStep( value.Get<Vector2>() );
        break;
      }
    }
  }
}

Property::Value ScrollView::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::ScrollView scrollView = Toolkit::ScrollView::DownCast( Dali::BaseHandle( object ) );

  if( scrollView )
  {
    ScrollView& scrollViewImpl( GetImpl( scrollView ) );
    switch( index )
    {
      case Toolkit::ScrollView::Property::WRAP_ENABLED:
      {
        value = scrollViewImpl.GetWrapMode();
        break;
      }
      case Toolkit::ScrollView::Property::PANNING_ENABLED:
      {
        value = scrollViewImpl.GetScrollSensitive();
        break;
      }
      case Toolkit::ScrollView::Property::AXIS_AUTO_LOCK_ENABLED:
      {
        value = scrollViewImpl.GetAxisAutoLock();
        break;
      }
      case Toolkit::ScrollView::Property::WHEEL_SCROLL_DISTANCE_STEP:
      {
        value = scrollViewImpl.GetWheelScrollDistanceStep();
        break;
      }
    }
  }

  return value;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
