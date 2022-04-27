/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/events/pan-gesture-devel.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/events/wheel-event.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/scroll-bar/scroll-bar.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-overshoot-indicator-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-effect-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-impl-property-handler.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-mode.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-constraints.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>

//#define ENABLED_SCROLL_STATE_LOGGING

#ifdef ENABLED_SCROLL_STATE_LOGGING
#define DALI_LOG_SCROLL_STATE(format, ...) Dali::Integration::Log::LogMessageWithFunctionLine(Dali::Integration::Log::DebugInfo, "%s:%d " format "\n", __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define DALI_LOG_SCROLL_STATE(format, ...)
#endif

// TODO: Change to two class system:
// 1. DraggableActor (is an actor which can be dragged anywhere, can be set to range using the ruler)
// 2. ScrollView (contains a draggable actor that can a) be dragged in the negative X, and Y domain, b) has a hitArea for touches)
// TODO: external components (page and status overlays).
// TODO: Orientation.
// TODO: upgrade Vector2/3 to support returning Unit vectors, normals, & cross product (dot product is already provided)

namespace
{
using namespace Dali;

constexpr float DEFAULT_SLOW_SNAP_ANIMATION_DURATION(0.5f);  ///< Default Drag-Release animation time.
constexpr float DEFAULT_FAST_SNAP_ANIMATION_DURATION(0.25f); ///< Default Drag-Flick animation time.
constexpr float DEFAULT_SNAP_OVERSHOOT_DURATION(0.5f);       ///< Default Overshoot snapping animation time.
constexpr float DEFAULT_MAX_OVERSHOOT(100.0f);               ///< Default maximum allowed overshoot in pixels

constexpr float DEFAULT_AXIS_AUTO_LOCK_GRADIENT(0.36f); ///< Default Axis-AutoLock gradient threshold. default is 0.36:1 (20 degrees)
constexpr float DEFAULT_FRICTION_COEFFICIENT(1.0f);     ///< Default Friction Co-efficient. (in stage diagonals per second)
constexpr float DEFAULT_FLICK_SPEED_COEFFICIENT(1.0f);  ///< Default Flick speed coefficient (multiples input touch velocity)
constexpr float DEFAULT_MAX_FLICK_SPEED(3.0f);          ///< Default Maximum flick speed. (in stage diagonals per second)

constexpr Dali::Vector2 DEFAULT_MIN_FLICK_DISTANCE(30.0f, 30.0f);  ///< minimum distance for pan before flick allowed
constexpr float         DEFAULT_MIN_FLICK_SPEED_THRESHOLD(500.0f); ///< Minimum pan speed required for flick in pixels/s

constexpr float FREE_FLICK_SPEED_THRESHOLD      = 200.0f; ///< Free-Flick threshold in pixels/ms
constexpr float AUTOLOCK_AXIS_MINIMUM_DISTANCE2 = 100.0f; ///< Auto-lock axis after minimum distance squared.
constexpr float FLICK_ORTHO_ANGLE_RANGE         = 75.0f;  ///< degrees. (if >45, then supports diagonal flicking)

constexpr Dali::Vector2 DEFAULT_WHEEL_SCROLL_DISTANCE_STEP_PROPORTION(0.17f, 0.1f); ///< The step of horizontal scroll distance in the proportion of stage size for each wheel event received.

constexpr unsigned long MINIMUM_TIME_BETWEEN_DOWN_AND_UP_FOR_RESET(150u);
constexpr float         TOUCH_DOWN_TIMER_INTERVAL = 100.0f;
constexpr float         DEFAULT_SCROLL_UPDATE_DISTANCE(30.0f); ///< Default distance to travel in pixels for scroll update signal

const std::string INTERNAL_MAX_POSITION_PROPERTY_NAME("internalMaxPosition");

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
  if(bias == Dali::Toolkit::DIRECTION_BIAS_NONE)
  {
    return Dali::ShortestDistanceInDomain(a, b, start, end);
  }
  //  (a-start + end-b)
  float size = end - start;
  float vect = b - a;

  if(vect > 0)
  {
    // +ve vector
    if(bias == Dali::Toolkit::DIRECTION_BIAS_RIGHT) // going right, take the vector.
    {
      return vect;
    }
    else
    {
      float aRight = a + size;
      return b - aRight;
    }
  }
  else
  {
    // -ve vector
    if(bias == Dali::Toolkit::DIRECTION_BIAS_LEFT) // going left, take the vector.
    {
      return vect;
    }
    else
    {
      float aLeft = a - size;
      return b - aLeft;
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
Dali::Vector3 GetPositionOfAnchor(Dali::Actor& actor, const Dali::Vector3& anchor)
{
  Dali::Vector3 childPosition = actor.GetCurrentProperty<Dali::Vector3>(Dali::Actor::Property::POSITION);
  Dali::Vector3 childAnchor   = -actor.GetCurrentProperty<Dali::Vector3>(Dali::Actor::Property::ANCHOR_POINT) + anchor;
  Dali::Vector3 childSize     = actor.GetCurrentProperty<Dali::Vector3>(Dali::Actor::Property::SIZE);

  return childPosition + childAnchor * childSize;
}

/**
 * Returns the closest actor to the given position
 * @param[in] actor The scrollview actor
 * @param[in] internalActor The internal actor (to ignore)
 * @param[in] position The given position
 * @param[in] dirX Direction to search in
 * @param[in] dirY Direction to search in
 * @param[in] dirZ Direction to search in
 * @return the closest child actor
 */
using FindDirection = Dali::Toolkit::Internal::ScrollView::FindDirection;

Actor FindClosestActorToPosition(
  CustomActor actor, Actor internalActor, const Vector3& position, FindDirection dirX, FindDirection dirY, FindDirection dirZ)
{
  Actor   closestChild;
  float   closestDistance2 = 0.0f;
  Vector3 actualPosition   = position;

  unsigned int numChildren = actor.GetChildCount();

  for(unsigned int i = 0; i < numChildren; ++i)
  {
    Actor child = actor.GetChildAt(i);

    if(internalActor == child) // ignore internal actor.
    {
      continue;
    }

    Vector3 childPosition = GetPositionOfAnchor(child, AnchorPoint::CENTER);

    Vector3 delta = childPosition - actualPosition;

    // X-axis checking (only find Actors to the [dirX] of actualPosition)
    if(dirX > FindDirection::All) // != All,None
    {
      FindDirection deltaH = delta.x > 0 ? FindDirection::Right : FindDirection::Left;
      if(dirX != deltaH)
      {
        continue;
      }
    }

    // Y-axis checking (only find Actors to the [dirY] of actualPosition)
    if(dirY > FindDirection::All) // != All,None
    {
      FindDirection deltaV = delta.y > 0 ? FindDirection::Down : FindDirection::Up;
      if(dirY != deltaV)
      {
        continue;
      }
    }

    // Z-axis checking (only find Actors to the [dirZ] of actualPosition)
    if(dirZ > FindDirection::All) // != All,None
    {
      FindDirection deltaV = delta.y > 0 ? FindDirection::In : FindDirection::Out;
      if(dirZ != deltaV)
      {
        continue;
      }
    }

    // compare child to closest child in terms of distance.
    float distance2 = 0.0f;

    // distance2 = the Square of the relevant dimensions of delta
    if(dirX != FindDirection::None)
    {
      distance2 += delta.x * delta.x;
    }

    if(dirY != FindDirection::None)
    {
      distance2 += delta.y * delta.y;
    }

    if(dirZ != FindDirection::None)
    {
      distance2 += delta.z * delta.z;
    }

    if(closestChild) // Next time.
    {
      if(distance2 < closestDistance2)
      {
        closestChild     = child;
        closestDistance2 = distance2;
      }
    }
    else // First time.
    {
      closestChild     = child;
      closestDistance2 = distance2;
    }
  }

  return closestChild;
}

// AlphaFunctions /////////////////////////////////////////////////////////////////////////////////

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

/**
 * Clamp a position
 * @param[in] size The size to clamp to
 * @param[in] rulerX The horizontal ruler
 * @param[in] rulerY The vertical ruler
 * @param[in,out] position The position to clamp
 * @param[out] clamped the clamped state
 */
void ClampPosition(const Vector3& size, Dali::Toolkit::RulerPtr rulerX, Dali::Toolkit::RulerPtr rulerY, Vector2& position, Dali::Toolkit::ClampState2D& clamped)
{
  position.x = -rulerX->Clamp(-position.x, size.width, 1.0f, clamped.x);  // NOTE: X & Y rulers think in -ve coordinate system.
  position.y = -rulerY->Clamp(-position.y, size.height, 1.0f, clamped.y); // That is scrolling RIGHT (e.g. 100.0, 0.0) means moving LEFT.
}

/**
 * TODO: In situations where axes are different (X snap, Y free)
 * Each axis should really have their own independent animation (time and equation)
 * Consider, X axis snapping to nearest grid point (EaseOut over fixed time)
 * Consider, Y axis simulating physics to arrive at a point (Physics equation over variable time)
 * Currently, the axes have been split however, they both use the same EaseOut equation.
 *
 * @param[in] scrollView The main scrollview
 * @param[in] rulerX The X ruler
 * @param[in] rulerY The Y ruler
 * @param[in] lockAxis Which axis (if any) is locked.
 * @param[in] velocity Current pan velocity
 * @param[in] maxOvershoot Maximum overshoot
 * @param[in] inAcessibilityPan True if we are currently panning with accessibility
 * @param[out] positionSnap The target position of snap animation
 * @param[out] positionDuration The duration of the snap animation
 * @param[out] alphaFunction The snap animation alpha function
 * @param[out] isFlick if we are flicking or not
 * @param[out] isFreeFlick if we are free flicking or not
 */
void SnapWithVelocity(
  Dali::Toolkit::Internal::ScrollView&          scrollView,
  Dali::Toolkit::RulerPtr                       rulerX,
  Dali::Toolkit::RulerPtr                       rulerY,
  Dali::Toolkit::Internal::ScrollView::LockAxis lockAxis,
  Vector2                                       velocity,
  Vector2                                       maxOvershoot,
  Vector2&                                      positionSnap,
  Vector2&                                      positionDuration,
  AlphaFunction&                                alphaFunction,
  bool                                          inAccessibilityPan,
  bool&                                         isFlick,
  bool&                                         isFreeFlick)
{
  // Animator takes over now, touches are assumed not to interfere.
  // And if touches do interfere, then we'll stop animation, update PrePosition
  // to current mScroll's properties, and then resume.
  // Note: For Flicking this may work a bit different...

  float         angle      = atan2(velocity.y, velocity.x);
  float         speed2     = velocity.LengthSquared();
  float         biasX      = 0.5f;
  float         biasY      = 0.5f;
  FindDirection horizontal = FindDirection::None;
  FindDirection vertical   = FindDirection::None;

  using LockAxis = Dali::Toolkit::Internal::ScrollView::LockAxis;

  // orthoAngleRange = Angle tolerance within the Exact N,E,S,W direction
  // that will be accepted as a general N,E,S,W flick direction.

  const float orthoAngleRange      = FLICK_ORTHO_ANGLE_RANGE * M_PI / 180.0f;
  const float flickSpeedThreshold2 = scrollView.GetMinimumSpeedForFlick() * scrollView.GetMinimumSpeedForFlick();

  // Flick logic X Axis

  if(rulerX->IsEnabled() && lockAxis != LockAxis::LockHorizontal)
  {
    horizontal = FindDirection::All;

    if(speed2 > flickSpeedThreshold2 || // exceeds flick threshold
       inAccessibilityPan)              // With AccessibilityPan its easier to move between snap positions
    {
      if((angle >= -orthoAngleRange) && (angle < orthoAngleRange)) // Swiping East
      {
        biasX = 0.0f, horizontal = FindDirection::Left;

        // This guards against an error where no movement occurs, due to the flick finishing
        // before the update-thread has advanced mScrollPostPosition past the the previous snap point.
        positionSnap.x += 1.0f;
      }
      else if((angle >= M_PI - orthoAngleRange) || (angle < -M_PI + orthoAngleRange)) // Swiping West
      {
        biasX = 1.0f, horizontal = FindDirection::Right;

        // This guards against an error where no movement occurs, due to the flick finishing
        // before the update-thread has advanced mScrollPostPosition past the the previous snap point.
        positionSnap.x -= 1.0f;
      }
    }
  }

  // Flick logic Y Axis

  if(rulerY->IsEnabled() && lockAxis != LockAxis::LockVertical)
  {
    vertical = FindDirection::All;

    if(speed2 > flickSpeedThreshold2 || // exceeds flick threshold
       inAccessibilityPan)              // With AccessibilityPan its easier to move between snap positions
    {
      if((angle >= M_PI_2 - orthoAngleRange) && (angle < M_PI_2 + orthoAngleRange)) // Swiping South
      {
        biasY = 0.0f, vertical = FindDirection::Up;
      }
      else if((angle >= -M_PI_2 - orthoAngleRange) && (angle < -M_PI_2 + orthoAngleRange)) // Swiping North
      {
        biasY = 1.0f, vertical = FindDirection::Down;
      }
    }
  }

  // isFlick: Whether this gesture is a flick or not.
  isFlick = (horizontal != FindDirection::All || vertical != FindDirection::All);
  // isFreeFlick: Whether this gesture is a flick under free panning criteria.
  isFreeFlick = velocity.LengthSquared() > (FREE_FLICK_SPEED_THRESHOLD * FREE_FLICK_SPEED_THRESHOLD);

  if(isFlick || isFreeFlick)
  {
    positionDuration = Vector2::ONE * scrollView.GetScrollFlickDuration();
    alphaFunction    = scrollView.GetScrollFlickAlphaFunction();
  }

  // Calculate next positionSnap ////////////////////////////////////////////////////////////

  if(scrollView.GetActorAutoSnap())
  {
    Vector3 size = scrollView.Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE);

    Actor child = scrollView.FindClosestActorToPosition(Vector3(size.width * 0.5f, size.height * 0.5f, 0.0f), horizontal, vertical);

    if(!child && isFlick)
    {
      // If we conducted a direction limited search and found no actor, then just snap to the closest actor.
      child = scrollView.FindClosestActorToPosition(Vector3(size.width * 0.5f, size.height * 0.5f, 0.0f));
    }

    if(child)
    {
      Vector2 position = scrollView.Self().GetCurrentProperty<Vector2>(Toolkit::ScrollView::Property::SCROLL_POSITION);

      // Get center-point of the Actor.
      Vector3 childPosition = GetPositionOfAnchor(child, AnchorPoint::CENTER);

      if(rulerX->IsEnabled())
      {
        positionSnap.x = position.x - childPosition.x + size.width * 0.5f;
      }
      if(rulerY->IsEnabled())
      {
        positionSnap.y = position.y - childPosition.y + size.height * 0.5f;
      }
    }
  }

  Vector2 startPosition = positionSnap;
  positionSnap.x        = -rulerX->Snap(-positionSnap.x, biasX); // NOTE: X & Y rulers think in -ve coordinate system.
  positionSnap.y        = -rulerY->Snap(-positionSnap.y, biasY); // That is scrolling RIGHT (e.g. 100.0, 0.0) means moving LEFT.

  Dali::Toolkit::ClampState2D clamped;
  Vector3                     size = scrollView.Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  Vector2                     clampDelta(Vector2::ZERO);
  ClampPosition(size, rulerX, rulerY, positionSnap, clamped);

  if((rulerX->GetType() == Dali::Toolkit::Ruler::FREE || rulerY->GetType() == Dali::Toolkit::Ruler::FREE) &&
     isFreeFlick && !scrollView.GetActorAutoSnap())
  {
    // Calculate target position based on velocity of flick.

    // a = Deceleration (Set to diagonal stage length * friction coefficient)
    // u = Initial Velocity (Flick velocity)
    // v = 0 (Final Velocity)
    // t = Time (Velocity / Deceleration)
    Vector2 stageSize   = Stage::GetCurrent().GetSize();
    float   stageLength = Vector3(stageSize.x, stageSize.y, 0.0f).Length();
    float   a           = (stageLength * scrollView.GetFrictionCoefficient());
    Vector3 u           = Vector3(velocity.x, velocity.y, 0.0f) * scrollView.GetFlickSpeedCoefficient();
    float   speed       = u.Length();
    u /= speed;

    // TODO: Change this to a decay function. (faster you flick, the slower it should be)
    speed = std::min(speed, stageLength * scrollView.GetMaxFlickSpeed());
    u *= speed;
    alphaFunction = ConstantDecelerationAlphaFunction;

    float t = speed / a;

    if(rulerX->IsEnabled() && rulerX->GetType() == Dali::Toolkit::Ruler::FREE)
    {
      positionSnap.x += t * u.x * 0.5f;
    }

    if(rulerY->IsEnabled() && rulerY->GetType() == Dali::Toolkit::Ruler::FREE)
    {
      positionSnap.y += t * u.y * 0.5f;
    }

    clampDelta = positionSnap;
    ClampPosition(size, rulerX, rulerY, positionSnap, clamped);

    if((positionSnap - startPosition).LengthSquared() > Math::MACHINE_EPSILON_0)
    {
      clampDelta -= positionSnap;
      clampDelta.x = clampDelta.x > 0.0f ? std::min(clampDelta.x, maxOvershoot.x) : std::max(clampDelta.x, -maxOvershoot.x);
      clampDelta.y = clampDelta.y > 0.0f ? std::min(clampDelta.y, maxOvershoot.y) : std::max(clampDelta.y, -maxOvershoot.y);
    }
    else
    {
      clampDelta = Vector2::ZERO;
    }

    // If Axis is Free and has velocity, then calculate time taken
    // to reach target based on velocity in axis.
    if(rulerX->IsEnabled() && rulerX->GetType() == Dali::Toolkit::Ruler::FREE)
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

    if(rulerY->IsEnabled() && rulerY->GetType() == Dali::Toolkit::Ruler::FREE)
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

  if(scrollView.IsOvershootEnabled())
  {
    // Scroll to the end of the overshoot only when overshoot is enabled.
    positionSnap += clampDelta;
  }
}

Dali::Vector2 GetPosition(Dali::Actor actor)
{
  Vector2     screenPosition          = actor.GetProperty<Vector2>(Actor::Property::SCREEN_POSITION);
  Vector3     size                    = actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE) * actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_SCALE);
  bool        positionUsesAnchorPoint = actor.GetProperty<bool>(Actor::Property::POSITION_USES_ANCHOR_POINT);
  Vector3     anchorPointOffSet       = size * (positionUsesAnchorPoint ? actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT) : AnchorPoint::TOP_LEFT);
  Vector2     position                = Vector2(screenPosition.x - anchorPointOffSet.x, screenPosition.y - anchorPointOffSet.y);

  return position;
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
DALI_TYPE_REGISTRATION_BEGIN(Toolkit::ScrollView, Toolkit::Scrollable, Create)

DALI_PROPERTY_REGISTRATION(Toolkit, ScrollView, "wrapEnabled", BOOLEAN, WRAP_ENABLED)
DALI_PROPERTY_REGISTRATION(Toolkit, ScrollView, "panningEnabled", BOOLEAN, PANNING_ENABLED)
DALI_PROPERTY_REGISTRATION(Toolkit, ScrollView, "axisAutoLockEnabled", BOOLEAN, AXIS_AUTO_LOCK_ENABLED)
DALI_PROPERTY_REGISTRATION(Toolkit, ScrollView, "wheelScrollDistanceStep", VECTOR2, WHEEL_SCROLL_DISTANCE_STEP)
DALI_PROPERTY_REGISTRATION(Toolkit, ScrollView, "scrollMode", MAP, SCROLL_MODE)

DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ScrollView, "scrollPosition", VECTOR2, SCROLL_POSITION)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ScrollView, "scrollPrePosition", VECTOR2, SCROLL_PRE_POSITION)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(Toolkit, ScrollView, "scrollPrePositionX", SCROLL_PRE_POSITION_X, SCROLL_PRE_POSITION, 0)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(Toolkit, ScrollView, "scrollPrePositionY", SCROLL_PRE_POSITION_Y, SCROLL_PRE_POSITION, 1)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ScrollView, "scrollPrePositionMax", VECTOR2, SCROLL_PRE_POSITION_MAX)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(Toolkit, ScrollView, "scrollPrePositionMaxX", SCROLL_PRE_POSITION_MAX_X, SCROLL_PRE_POSITION_MAX, 0)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(Toolkit, ScrollView, "scrollPrePositionMaxY", SCROLL_PRE_POSITION_MAX_Y, SCROLL_PRE_POSITION_MAX, 1)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ScrollView, "overshootX", FLOAT, OVERSHOOT_X)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ScrollView, "overshootY", FLOAT, OVERSHOOT_Y)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ScrollView, "scrollFinal", VECTOR2, SCROLL_FINAL)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(Toolkit, ScrollView, "scrollFinalX", SCROLL_FINAL_X, SCROLL_FINAL, 0)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(Toolkit, ScrollView, "scrollFinalY", SCROLL_FINAL_Y, SCROLL_FINAL, 1)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ScrollView, "wrap", BOOLEAN, WRAP)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ScrollView, "panning", BOOLEAN, PANNING)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ScrollView, "scrolling", BOOLEAN, SCROLLING)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ScrollView, "scrollDomainSize", VECTOR2, SCROLL_DOMAIN_SIZE)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(Toolkit, ScrollView, "scrollDomainSizeX", SCROLL_DOMAIN_SIZE_X, SCROLL_DOMAIN_SIZE, 0)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(Toolkit, ScrollView, "scrollDomainSizeY", SCROLL_DOMAIN_SIZE_Y, SCROLL_DOMAIN_SIZE, 1)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ScrollView, "scrollDomainOffset", VECTOR2, SCROLL_DOMAIN_OFFSET)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ScrollView, "scrollPositionDelta", VECTOR2, SCROLL_POSITION_DELTA)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit, ScrollView, "startPagePosition", VECTOR3, START_PAGE_POSITION)

DALI_SIGNAL_REGISTRATION(Toolkit, ScrollView, "valueChanged", SIGNAL_SNAP_STARTED)

DALI_TYPE_REGISTRATION_END()

} // namespace

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
: ScrollBase(ControlBehaviour(DISABLE_STYLE_CHANGE_SIGNALS)), // Enable size negotiation
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
  mCanScrollVertical(true),
  mTransientScrollBar(true)
{
}

void ScrollView::OnInitialize()
{
  Actor self = Self();

  // Internal Actor, used to hide actors from enumerations.
  // Also actors added to Internal actor appear as overlays e.g. ScrollBar components.
  mInternalActor = Actor::New();
  self.Add(mInternalActor);

  mInternalActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mInternalActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  mInternalActor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);

  mAlterChild = true;

  mScrollPostPosition = mScrollPrePosition = Vector2::ZERO;

  mWheelScrollDistanceStep = Stage::GetCurrent().GetSize() * DEFAULT_WHEEL_SCROLL_DISTANCE_STEP_PROPORTION;

  mGestureStackDepth = 0;

  self.TouchedSignal().Connect(this, &ScrollView::OnTouch);
  EnableGestureDetection(GestureType::Value(GestureType::PAN));

  // By default we'll allow the user to freely drag the scroll view,
  // while disabling the other rulers.
  RulerPtr ruler = new DefaultRuler();
  mRulerX        = ruler;
  mRulerY        = ruler;

  self.SetProperty(Toolkit::Scrollable::Property::CAN_SCROLL_VERTICAL, mCanScrollVertical);
  self.SetProperty(Toolkit::Scrollable::Property::CAN_SCROLL_HORIZONTAL, mCanScrollHorizontal);

  ScrollViewPropertyHandler::UpdatePropertyDomain(*this);
  mConstraints.SetInternalConstraints(*this);

  // Connect wheel event
  self.WheelEventSignal().Connect(this, &ScrollView::OnWheelEvent);

  self.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::SCROLL_PANE);
}

DevelControl::ControlAccessible* ScrollView::CreateAccessibleObject()
{
  return new ScrollViewAccessible(Self());
}

void ScrollView::OnSceneConnection(int depth)
{
  DALI_LOG_SCROLL_STATE("[0x%X]", this);

  if(mSensitive)
  {
    SetScrollSensitive(false);
    SetScrollSensitive(true);
  }

  if(IsOvershootEnabled())
  {
    // try and make sure property notifications are set
    EnableScrollOvershoot(true);
  }

  ScrollBase::OnSceneConnection(depth);
}

void ScrollView::OnSceneDisconnection()
{
  DALI_LOG_SCROLL_STATE("[0x%X]", this);

  StopAnimation();

  ScrollBase::OnSceneDisconnection();
}

ScrollView::~ScrollView()
{
  DALI_LOG_SCROLL_STATE("[0x%X]", this);
}

void ScrollView::ApplyEffect(Toolkit::ScrollViewEffect effect)
{
  Dali::Toolkit::ScrollView self = Dali::Toolkit::ScrollView::DownCast(Self());

  // Assertion check to ensure effect doesn't already exist in this scrollview
  bool effectAlreadyExistsInScrollView(false);
  for(ScrollViewEffectIter iter = mEffects.begin(); iter != mEffects.end(); ++iter)
  {
    if(*iter == effect)
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
  for(ScrollViewEffectIter iter = mEffects.begin(); iter != mEffects.end(); ++iter)
  {
    if(*iter == effect)
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

  for(ScrollViewEffectIter effectIter = mEffects.begin(); effectIter != mEffects.end(); ++effectIter)
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

void ScrollView::SetRulerX(RulerPtr ruler)
{
  mRulerX = ruler;

  ScrollViewPropertyHandler::UpdatePropertyDomain(*this);
  mConstraints.UpdateMainInternalConstraint(*this);
}

void ScrollView::SetRulerY(RulerPtr ruler)
{
  mRulerY = ruler;

  ScrollViewPropertyHandler::UpdatePropertyDomain(*this);
  mConstraints.UpdateMainInternalConstraint(*this);
}

void ScrollView::SetScrollSensitive(bool sensitive)
{
  Actor              self = Self();
  PanGestureDetector panGesture(GetPanGestureDetector());

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
    if(mPanning)
    {
      PanGesture cancelGesture = DevelPanGesture::New(GestureState::CANCELLED);
      OnPan(cancelGesture);
    }

    panGesture.Detach(self);
    mSensitive = sensitive;

    mGestureStackDepth = 0;
    DALI_LOG_SCROLL_STATE("[0x%X] AFTER: panning:[%d]", this, int(mPanning));
  }
}

void ScrollView::SetMaxOvershoot(float overshootX, float overshootY)
{
  mMaxOvershoot.x      = overshootX;
  mMaxOvershoot.y      = overshootY;
  mUserMaxOvershoot    = mMaxOvershoot;
  mDefaultMaxOvershoot = false;
  mConstraints.UpdateMainInternalConstraint(*this);
}

bool ScrollView::GetActorAutoSnap()
{
  return mActorAutoSnapEnabled;
}

void ScrollView::SetAutoResize(bool enable)
{
  mAutoResizeContainerEnabled = enable;
  // TODO: This needs a lot of issues to be addressed before working.
}

void ScrollView::SetWrapMode(bool enable)
{
  mWrapMode = enable;
  Self().SetProperty(Toolkit::ScrollView::Property::WRAP, enable);
}

void ScrollView::SetAxisAutoLock(bool enable)
{
  mAxisAutoLock = enable;
  mConstraints.UpdateMainInternalConstraint(*this);
}

void ScrollView::SetAxisAutoLockGradient(float gradient)
{
  DALI_ASSERT_DEBUG(gradient >= 0.0f && gradient <= 1.0f);
  mAxisAutoLockGradient = gradient;
  mConstraints.UpdateMainInternalConstraint(*this);
}

void ScrollView::SetFrictionCoefficient(float friction)
{
  DALI_ASSERT_DEBUG(friction > 0.0f);
  mFrictionCoefficient = friction;
}

unsigned int ScrollView::GetCurrentPage() const
{
  // in case animation is currently taking place.
  Vector2 position = GetPropertyPosition();

  Actor        self           = Self();
  unsigned int page           = 0;
  unsigned int pagesPerVolume = 1;
  unsigned int volume         = 0;

  // if rulerX is enabled, then get page count (columns)
  page           = mRulerX->GetPageFromPosition(-position.x, mWrapMode);
  volume         = mRulerY->GetPageFromPosition(-position.y, mWrapMode);
  pagesPerVolume = mRulerX->GetTotalPages();

  return volume * pagesPerVolume + page;
}

Vector2 ScrollView::GetCurrentScrollPosition() const
{
  return -GetPropertyPosition();
}

void ScrollView::TransformTo(const Vector2& position,
                             DirectionBias  horizontalBias,
                             DirectionBias  verticalBias)
{
  TransformTo(position, mSnapDuration, mSnapAlphaFunction, horizontalBias, verticalBias);
}

void ScrollView::TransformTo(const Vector2& position, float duration, AlphaFunction alpha, DirectionBias horizontalBias, DirectionBias verticalBias)
{
  // If this is called while the timer is running, then cancel it
  StopTouchDownTimer();

  Actor self(Self());

  // Guard against destruction during signal emission
  // Note that Emit() methods are called indirectly e.g. from within ScrollView::AnimateTo()
  Toolkit::ScrollView handle(GetOwner());

  DALI_LOG_SCROLL_STATE("[0x%X] pos[%.2f,%.2f], duration[%.2f] bias[%d, %d]",
                        this,
                        position.x,
                        position.y,
                        duration,
                        int(horizontalBias),
                        int(verticalBias));

  Vector2 currentScrollPosition = GetCurrentScrollPosition();
  self.SetProperty(Toolkit::ScrollView::Property::START_PAGE_POSITION, Vector3(currentScrollPosition));

  if(mScrolling) // are we interrupting a current scroll?
  {
    // set mScrolling to false, in case user has code that interrogates mScrolling Getter() in complete.
    mScrolling = false;
    DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignal 1 [%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y);
    mScrollCompletedSignal.Emit(currentScrollPosition);
  }

  if(mPanning) // are we interrupting a current pan?
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Interrupting Pan, set to false", this);
    mPanning           = false;
    mGestureStackDepth = 0;
    self.SetProperty(Toolkit::ScrollView::Property::PANNING, false);

    if(mConstraints.mScrollMainInternalPrePositionConstraint)
    {
      mConstraints.mScrollMainInternalPrePositionConstraint.Remove();
    }
  }

  self.SetProperty(Toolkit::ScrollView::Property::SCROLLING, true);
  mScrolling = true;

  DALI_LOG_SCROLL_STATE("[0x%X] mScrollStartedSignal 1 [%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y);
  mScrollStartedSignal.Emit(currentScrollPosition);
  bool animating = AnimateTo(-position,
                             Vector2::ONE * duration,
                             alpha,
                             true,
                             horizontalBias,
                             verticalBias,
                             SNAP);

  if(!animating)
  {
    // if not animating, then this pan has completed right now.
    self.SetProperty(Toolkit::ScrollView::Property::SCROLLING, false);
    mScrolling = false;

    // If we have no duration, then in the next update frame, we will be at the position specified as we just set.
    // In this scenario, we cannot return the currentScrollPosition as this is out-of-date and should instead return the requested final position
    Vector2 completedPosition(currentScrollPosition);
    if(duration <= Math::MACHINE_EPSILON_10)
    {
      completedPosition = position;
    }

    DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignal 2 [%.2f, %.2f]", this, completedPosition.x, completedPosition.y);
    SetScrollUpdateNotification(false);
    mScrollCompletedSignal.Emit(completedPosition);
  }
}

void ScrollView::ScrollTo(const Vector2& position)
{
  ScrollTo(position, mSnapDuration);
}

void ScrollView::ScrollTo(const Vector2& position, float duration)
{
  ScrollTo(position, duration, DIRECTION_BIAS_NONE, DIRECTION_BIAS_NONE);
}

void ScrollView::ScrollTo(const Vector2& position, float duration, AlphaFunction alpha)
{
  ScrollTo(position, duration, alpha, DIRECTION_BIAS_NONE, DIRECTION_BIAS_NONE);
}

void ScrollView::ScrollTo(const Vector2& position, float duration, DirectionBias horizontalBias, DirectionBias verticalBias)
{
  ScrollTo(position, duration, mSnapAlphaFunction, horizontalBias, verticalBias);
}

void ScrollView::ScrollTo(const Vector2& position, float duration, AlphaFunction alpha, DirectionBias horizontalBias, DirectionBias verticalBias)
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
  Vector2      position;
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

void ScrollView::ScrollTo(Actor& actor)
{
  ScrollTo(actor, mSnapDuration);
}

void ScrollView::ScrollTo(Actor& actor, float duration)
{
  DALI_ASSERT_ALWAYS(actor.GetParent() == Self());

  Actor   self        = Self();
  Vector3 size        = self.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  Vector3 position    = actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
  Vector2 prePosition = GetPropertyPrePosition();
  position.GetVectorXY() -= prePosition;

  ScrollTo(Vector2(position.x - size.width * 0.5f, position.y - size.height * 0.5f), duration);
}

Actor ScrollView::FindClosestActor()
{
  Actor   self = Self();
  Vector3 size = self.GetCurrentProperty<Vector3>(Actor::Property::SIZE);

  return FindClosestActorToPosition(Vector3(size.width * 0.5f, size.height * 0.5f, 0.0f));
}

Actor ScrollView::FindClosestActorToPosition(const Vector3& position, FindDirection dirX, FindDirection dirY, FindDirection dirZ)
{
  return ::FindClosestActorToPosition(Self(), mInternalActor, position, dirX, dirY, dirZ);
}

bool ScrollView::ScrollToSnapPoint()
{
  DALI_LOG_SCROLL_STATE("[0x%X]", this);
  Vector2 stationaryVelocity = Vector2(0.0f, 0.0f);
  return SnapWithVelocity(stationaryVelocity);
}

bool ScrollView::SnapWithVelocity(Vector2 velocity)
{
  Vector2       positionSnap     = mScrollPrePosition;
  Vector2       positionDuration = Vector2::ONE * mSnapDuration;
  AlphaFunction alphaFunction    = mSnapAlphaFunction;
  bool          isFlick;
  bool          isFreeFlick;

  ::SnapWithVelocity(*this, mRulerX, mRulerY, mLockAxis, velocity, mMaxOvershoot, positionSnap, positionDuration, alphaFunction, mInAccessibilityPan, isFlick, isFreeFlick);

  bool animating = AnimateTo(positionSnap, positionDuration, alphaFunction, false, DIRECTION_BIAS_NONE, DIRECTION_BIAS_NONE, isFlick || isFreeFlick ? FLICK : SNAP);

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

bool ScrollView::AnimateTo(const Vector2& position, const Vector2& positionDuration, AlphaFunction alpha, bool findShortcuts, DirectionBias horizontalBias, DirectionBias verticalBias, SnapType snapType)
{
  // Here we perform an animation on a number of properties (depending on which have changed)
  // The animation is applied to all ScrollBases
  Actor self            = Self();
  mScrollTargetPosition = position;
  float totalDuration   = 0.0f;

  bool positionChanged = (mScrollTargetPosition != mScrollPostPosition);

  if(positionChanged)
  {
    totalDuration = std::max(totalDuration, positionDuration.x);
    totalDuration = std::max(totalDuration, positionDuration.y);
  }
  else
  {
    // try to animate for a frame, on some occasions update will be changing scroll value while event side thinks it hasnt changed
    totalDuration   = 0.01f;
    positionChanged = true;
  }

  StopAnimation();

  // Position Delta ///////////////////////////////////////////////////////
  if(positionChanged)
  {
    mConstraints.UpdateMainInternalConstraint(*this);
    if(mWrapMode && findShortcuts)
    {
      // In Wrap Mode, the shortest distance is a little less intuitive...
      const RulerDomain rulerDomainX = mRulerX->GetDomain();
      const RulerDomain rulerDomainY = mRulerY->GetDomain();

      if(mRulerX->IsEnabled())
      {
        float dir               = VectorInDomain(-mScrollPrePosition.x, -mScrollTargetPosition.x, rulerDomainX.min, rulerDomainX.max, horizontalBias);
        mScrollTargetPosition.x = mScrollPrePosition.x + -dir;
      }

      if(mRulerY->IsEnabled())
      {
        float dir               = VectorInDomain(-mScrollPrePosition.y, -mScrollTargetPosition.y, rulerDomainY.min, rulerDomainY.max, verticalBias);
        mScrollTargetPosition.y = mScrollPrePosition.y + -dir;
      }
    }

    // note we have two separate animations for X & Y, this deals with sliding diagonally and hitting
    // a horizonal/vertical wall.delay
    AnimateInternalXTo(mScrollTargetPosition.x, positionDuration.x, alpha);
    AnimateInternalYTo(mScrollTargetPosition.y, positionDuration.y, alpha);

    if(!(mScrollStateFlags & SCROLL_ANIMATION_FLAGS))
    {
      DALI_LOG_SCROLL_STATE("[0x%X] Setting SCROLL_PRE_POSITION To[%.2f, %.2f]", this, mScrollTargetPosition.x, mScrollTargetPosition.y);
      self.SetProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, mScrollTargetPosition);
      mScrollPrePosition  = mScrollTargetPosition;
      mScrollPostPosition = mScrollTargetPosition;
      WrapPosition(mScrollPostPosition);
    }

    DALI_LOG_SCROLL_STATE("[0x%X] position-changed, mScrollTargetPosition[%.2f, %.2f], mScrollPrePosition[%.2f, %.2f], mScrollPostPosition[%.2f, %.2f]", this, mScrollTargetPosition.x, mScrollTargetPosition.y, mScrollPrePosition.x, mScrollPrePosition.y, mScrollPostPosition.x, mScrollPostPosition.y);
    DALI_LOG_SCROLL_STATE("[0x%X] SCROLL_PRE_POSITION[%.2f, %.2f], SCROLL_POSITION[%.2f, %.2f]", this, self.GetCurrentProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION).Get<Vector2>().x, self.GetCurrentProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION).Get<Vector2>().y, self.GetCurrentProperty(Toolkit::ScrollView::Property::SCROLL_POSITION).Get<Vector2>().x, self.GetCurrentProperty(Toolkit::ScrollView::Property::SCROLL_POSITION).Get<Vector2>().y);
  }

  SetScrollUpdateNotification(true);

  // Always send a snap event when AnimateTo is called.
  Toolkit::ScrollView::SnapEvent snapEvent;
  snapEvent.type     = snapType;
  snapEvent.position = -mScrollTargetPosition;
  snapEvent.duration = totalDuration;

  DALI_LOG_SCROLL_STATE("[0x%X] mSnapStartedSignal [%.2f, %.2f]", this, snapEvent.position.x, snapEvent.position.y);
  mSnapStartedSignal.Emit(snapEvent);

  return (mScrollStateFlags & SCROLL_ANIMATION_FLAGS) != 0;
}

void ScrollView::EnableScrollOvershoot(bool enable)
{
  if(enable)
  {
    if(!mOvershootIndicator)
    {
      mOvershootIndicator = ScrollOvershootIndicator::New();
    }

    mOvershootIndicator->AttachToScrollable(*this);
  }
  else
  {
    mMaxOvershoot = mUserMaxOvershoot;

    if(mOvershootIndicator)
    {
      mOvershootIndicator->DetachFromScrollable(*this);
    }
  }

  mConstraints.UpdateMainInternalConstraint(*this);
}

void ScrollView::AddOverlay(Actor actor)
{
  actor.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
  mInternalActor.Add(actor);
}

void ScrollView::RemoveOverlay(Actor actor)
{
  mInternalActor.Remove(actor);
}

void ScrollView::SetOvershootSize(const Vector2& size)
{
  mOvershootSize = size;
  if(IsOvershootEnabled() && mOvershootIndicator)
  {
    mOvershootIndicator->AttachToScrollable(*this);
  }
}

void ScrollView::SetOvershootEffectColor(const Vector4& color)
{
  mOvershootEffectColor = color;
  if(mOvershootIndicator)
  {
    mOvershootIndicator->SetOvershootEffectColor(color);
  }
}

void ScrollView::SetScrollingDirection(Radian direction, Radian threshold)
{
  PanGestureDetector panGesture(GetPanGestureDetector());

  // First remove just in case we have some set, then add.
  panGesture.RemoveDirection(direction);
  panGesture.AddDirection(direction, threshold);
}

void ScrollView::RemoveScrollingDirection(Radian direction)
{
  PanGestureDetector panGesture(GetPanGestureDetector());
  panGesture.RemoveDirection(direction);
}

Toolkit::ScrollView::SnapStartedSignalType& ScrollView::SnapStartedSignal()
{
  return mSnapStartedSignal;
}

bool ScrollView::ScrollViewAccessible::ScrollToChild(Actor child)
{
  auto scrollView = Dali::Toolkit::ScrollView::DownCast(Self());
  if(Toolkit::GetImpl(scrollView).FindClosestActor() == child)
  {
    return false;
  }

  auto childPosition = GetPosition(child);
  auto selfPosition  = GetPosition(Self());

  scrollView.ScrollTo(childPosition - selfPosition, scrollView.GetScrollFlickDuration());

  return true;
}

void ScrollView::FindAndUnbindActor(Actor child)
{
  UnbindActor(child);
}

Vector2 ScrollView::GetPropertyPrePosition() const
{
  Vector2 position = Self().GetCurrentProperty<Vector2>(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION);
  WrapPosition(position);
  return position;
}

Vector2 ScrollView::GetPropertyPosition() const
{
  Vector2 position = Self().GetCurrentProperty<Vector2>(Toolkit::ScrollView::Property::SCROLL_POSITION);
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
  DALI_LOG_SCROLL_STATE("[0x%X] Setting SCROLL_PRE_POSITION To[%.2f, %.2f]", this, mScrollPrePosition.x, mScrollPrePosition.y);
  self.SetProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, mScrollPrePosition);

  Vector2 currentScrollPosition = GetCurrentScrollPosition();
  DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignal 3 current[%.2f, %.2f], mScrollTargetPosition[%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y, -mScrollTargetPosition.x, -mScrollTargetPosition.y);
  mScrollCompletedSignal.Emit(currentScrollPosition);

  mDomainOffset += deltaPosition - mScrollPostPosition;
  self.SetProperty(Toolkit::ScrollView::Property::SCROLL_DOMAIN_OFFSET, mDomainOffset);
  HandleStoppedAnimation();
}

void ScrollView::SetScrollUpdateNotification(bool enabled)
{
  Actor self = Self();
  if(mScrollXUpdateNotification)
  {
    // disconnect now to avoid a notification before removed from update thread
    mScrollXUpdateNotification.NotifySignal().Disconnect(this, &ScrollView::OnScrollUpdateNotification);
    self.RemovePropertyNotification(mScrollXUpdateNotification);
    mScrollXUpdateNotification.Reset();
  }
  if(enabled && !mScrollUpdatedSignal.Empty())
  {
    // Only set up the notification when the application has connected to the updated signal
    mScrollXUpdateNotification = self.AddPropertyNotification(Toolkit::ScrollView::Property::SCROLL_POSITION, 0, StepCondition(mScrollUpdateDistance, 0.0f));
    mScrollXUpdateNotification.NotifySignal().Connect(this, &ScrollView::OnScrollUpdateNotification);
  }
  if(mScrollYUpdateNotification)
  {
    // disconnect now to avoid a notification before removed from update thread
    mScrollYUpdateNotification.NotifySignal().Disconnect(this, &ScrollView::OnScrollUpdateNotification);
    self.RemovePropertyNotification(mScrollYUpdateNotification);
    mScrollYUpdateNotification.Reset();
  }
  if(enabled && !mScrollUpdatedSignal.Empty())
  {
    // Only set up the notification when the application has connected to the updated signal
    mScrollYUpdateNotification = self.AddPropertyNotification(Toolkit::ScrollView::Property::SCROLL_POSITION, 1, StepCondition(mScrollUpdateDistance, 0.0f));
    mScrollYUpdateNotification.NotifySignal().Connect(this, &ScrollView::OnScrollUpdateNotification);
  }
}

void ScrollView::OnScrollUpdateNotification(Dali::PropertyNotification& source)
{
  // Guard against destruction during signal emission
  Toolkit::ScrollView handle(GetOwner());

  Vector2 currentScrollPosition = GetCurrentScrollPosition();
  mScrollUpdatedSignal.Emit(currentScrollPosition);
}

bool ScrollView::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool                connected(true);
  Toolkit::ScrollView view = Toolkit::ScrollView::DownCast(handle);

  if(0 == strcmp(signalName.c_str(), SIGNAL_SNAP_STARTED))
  {
    view.SnapStartedSignal().Connect(tracker, functor);
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
  ScrollViewPropertyHandler::UpdatePropertyDomain(*this);
}

void ScrollView::OnSizeSet(const Vector3& size)
{
  // need to update domain properties for new size
  if(mDefaultMaxOvershoot)
  {
    mUserMaxOvershoot.x = size.x * 0.5f;
    mUserMaxOvershoot.y = size.y * 0.5f;
    if(!IsOvershootEnabled())
    {
      mMaxOvershoot = mUserMaxOvershoot;
    }
  }
  ScrollViewPropertyHandler::UpdatePropertyDomain(*this);
  mConstraints.UpdateMainInternalConstraint(*this);
  if(IsOvershootEnabled())
  {
    mOvershootIndicator->Reset();
  }

  ScrollBase::OnSizeSet(size);
}

void ScrollView::OnChildAdd(Actor& child)
{
  ScrollBase::OnChildAdd(child);

  Dali::Toolkit::ScrollBar scrollBar = Dali::Toolkit::ScrollBar::DownCast(child);
  if(scrollBar)
  {
    mScrollBar = scrollBar;
    scrollBar.SetProperty(Dali::Actor::Property::NAME, "ScrollBar");

    mInternalActor.Add(scrollBar);
    if(scrollBar.GetScrollDirection() == Toolkit::ScrollBar::HORIZONTAL)
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

    if(mTransientScrollBar)
    {
      // Show the scroll-indicator for a brief period
      Property::Map emptyMap;
      scrollBar.DoAction("ShowTransientIndicator", emptyMap);
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

  ScrollBase::OnChildRemove(child);
}

void ScrollView::StartTouchDownTimer()
{
  if(!mTouchDownTimer)
  {
    mTouchDownTimer = Timer::New(TOUCH_DOWN_TIMER_INTERVAL);
    mTouchDownTimer.TickSignal().Connect(this, &ScrollView::OnTouchDownTimeout);
  }

  mTouchDownTimer.Start();
}

void ScrollView::StopTouchDownTimer()
{
  if(mTouchDownTimer)
  {
    mTouchDownTimer.Stop();
  }
}

bool ScrollView::OnTouchDownTimeout()
{
  DALI_LOG_SCROLL_STATE("[0x%X]", this);

  mTouchDownTimeoutReached = true;

  unsigned int currentScrollStateFlags(mScrollStateFlags); // Cleared in StopAnimation so keep local copy for comparison
  if(currentScrollStateFlags & (SCROLL_ANIMATION_FLAGS | SNAP_ANIMATION_FLAGS))
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Scrolling Or snapping flags set, stopping animation", this);

    StopAnimation();
    if(currentScrollStateFlags & SCROLL_ANIMATION_FLAGS)
    {
      DALI_LOG_SCROLL_STATE("[0x%X] Scrolling flags set, emitting signal", this);

      mScrollInterrupted = true;
      // reset domain offset as scrolling from original plane.
      mDomainOffset = Vector2::ZERO;
      Self().SetProperty(Toolkit::ScrollView::Property::SCROLL_DOMAIN_OFFSET, Vector2::ZERO);

      UpdateLocalScrollProperties();
      Vector2 currentScrollPosition = GetCurrentScrollPosition();
      DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignal 4 [%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y);
      mScrollCompletedSignal.Emit(currentScrollPosition);
    }
  }

  return false;
}

bool ScrollView::OnTouch(Actor actor, const TouchEvent& touch)
{
  if(!mSensitive)
  {
    DALI_LOG_SCROLL_STATE("[0x%X], Not Sensitive, ignoring", this);

    // Ignore this touch event, if scrollview is insensitive.
    return false;
  }

  // Ignore events with multiple-touch points
  if(touch.GetPointCount() != 1)
  {
    DALI_LOG_SCROLL_STATE("[0x%X], multiple touch, ignoring", this);

    return false;
  }

  const PointState::Type pointState = touch.GetState(0);
  if(pointState == PointState::DOWN)
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Down", this);

    if(mGestureStackDepth == 0)
    {
      mTouchDownTime = touch.GetTime();

      // This allows time for a pan-gesture to start, to avoid breaking snap-animation behavior with fast flicks.
      // If touch-down does not become a pan (after timeout interval), then snap-animation can be interrupted.
      mTouchDownTimeoutReached = false;
      mScrollInterrupted       = false;
      StartTouchDownTimer();
    }
  }
  else if((pointState == PointState::UP) ||
          ((pointState == PointState::INTERRUPTED) && (touch.GetHitActor(0) == Self())))
  {
    DALI_LOG_SCROLL_STATE("[0x%X] %s", this, ((pointState == PointState::UP) ? "Up" : "Interrupted"));

    StopTouchDownTimer();

    // if the user touches and releases without enough movement to go
    // into a gesture state, then we should snap to nearest point.
    // otherwise our scroll could be stopped (interrupted) half way through an animation.
    if(mGestureStackDepth == 0 && mTouchDownTimeoutReached)
    {
      if((pointState == PointState::INTERRUPTED) ||
         ((touch.GetTime() - mTouchDownTime) >= MINIMUM_TIME_BETWEEN_DOWN_AND_UP_FOR_RESET))
      {
        // Reset the velocity only if down was received a while ago
        mLastVelocity = Vector2(0.0f, 0.0f);
      }

      UpdateLocalScrollProperties();
      // Only finish the transform if scrolling was interrupted on down or if we are scrolling
      if(mScrollInterrupted || mScrolling)
      {
        DALI_LOG_SCROLL_STATE("[0x%X] Calling FinishTransform", this);

        FinishTransform();
      }
    }
    mTouchDownTimeoutReached = false;
    mScrollInterrupted       = false;
  }

  return false;
}

bool ScrollView::OnWheelEvent(Actor actor, const WheelEvent& event)
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
    if(mRulerX->GetType() == Ruler::FREE)
    {
      // Free panning mode
      targetScrollPosition.x += event.GetDelta() * mWheelScrollDistanceStep.x;
      ClampPosition(targetScrollPosition);
      ScrollTo(-targetScrollPosition);
    }
    else if(!mScrolling)
    {
      // Snap mode, only respond to the event when the previous snap animation is finished.
      ScrollTo(GetCurrentPage() - event.GetDelta());
    }
  }
  else
  {
    // If the ruler in the Y axis is enabled, scroll in the Y axis.
    if(mRulerY->GetType() == Ruler::FREE)
    {
      // Free panning mode
      targetScrollPosition.y += event.GetDelta() * mWheelScrollDistanceStep.y;
      ClampPosition(targetScrollPosition);
      ScrollTo(-targetScrollPosition);
    }
    else if(!mScrolling)
    {
      // Snap mode, only respond to the event when the previous snap animation is finished.
      ScrollTo(GetCurrentPage() - event.GetDelta() * mRulerX->GetTotalPages());
    }
  }

  return true;
}

void ScrollView::ResetScrolling()
{
  Actor self = Self();
  self.GetCurrentProperty(Toolkit::ScrollView::Property::SCROLL_POSITION).Get(mScrollPostPosition);
  mScrollPrePosition = mScrollPostPosition;
  DALI_LOG_SCROLL_STATE("[0x%X] Setting SCROLL_PRE_POSITION To[%.2f, %.2f]", this, mScrollPostPosition.x, mScrollPostPosition.y);
  self.SetProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, mScrollPostPosition);
}

void ScrollView::UpdateLocalScrollProperties()
{
  Actor self = Self();
  self.GetCurrentProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION).Get(mScrollPrePosition);
  self.GetCurrentProperty(Toolkit::ScrollView::Property::SCROLL_POSITION).Get(mScrollPostPosition);
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

  if(mScrollStateFlags & SCROLL_X_STATE_MASK)
  {
    // already performing animation on internal x position
    StopAnimation(mInternalXAnimation);
  }

  if(mScrollStateFlags & SCROLL_Y_STATE_MASK)
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

void ScrollView::AnimateInternalXTo(float position, float duration, AlphaFunction alpha)
{
  StopAnimation(mInternalXAnimation);

  if(duration > Math::MACHINE_EPSILON_10)
  {
    Actor self = Self();
    DALI_LOG_SCROLL_STATE("[0x%X], Animating from[%.2f] to[%.2f]", this, self.GetCurrentProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION).Get<Vector2>().x, position);
    mInternalXAnimation = Animation::New(duration);
    DALI_LOG_SCROLL_STATE("[0x%X], mInternalXAnimation[0x%X]", this, mInternalXAnimation.GetObjectPtr());
    mInternalXAnimation.FinishedSignal().Connect(this, &ScrollView::OnScrollAnimationFinished);
    mInternalXAnimation.AnimateTo(Property(self, Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, 0), position, alpha, TimePeriod(duration));
    mInternalXAnimation.Play();

    // erase current state flags
    mScrollStateFlags &= ~SCROLL_X_STATE_MASK;
    // add internal animation state flag
    mScrollStateFlags |= AnimatingInternalX;
  }
}

void ScrollView::AnimateInternalYTo(float position, float duration, AlphaFunction alpha)
{
  StopAnimation(mInternalYAnimation);

  if(duration > Math::MACHINE_EPSILON_10)
  {
    Actor self = Self();
    DALI_LOG_SCROLL_STATE("[0x%X], Animating from[%.2f] to[%.2f]", this, self.GetCurrentProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION).Get<Vector2>().y, position);
    mInternalYAnimation = Animation::New(duration);
    DALI_LOG_SCROLL_STATE("[0x%X], mInternalYAnimation[0x%X]", this, mInternalYAnimation.GetObjectPtr());
    mInternalYAnimation.FinishedSignal().Connect(this, &ScrollView::OnScrollAnimationFinished);
    mInternalYAnimation.AnimateTo(Property(self, Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, 1), position, alpha, TimePeriod(duration));
    mInternalYAnimation.Play();

    // erase current state flags
    mScrollStateFlags &= ~SCROLL_Y_STATE_MASK;
    // add internal animation state flag
    mScrollStateFlags |= AnimatingInternalY;
  }
}

void ScrollView::OnScrollAnimationFinished(Animation& source)
{
  // Guard against destruction during signal emission
  // Note that ScrollCompletedSignal is emitted from HandleSnapAnimationFinished()
  Toolkit::ScrollView handle(GetOwner());

  bool scrollingFinished = false;

  // update our local scroll positions
  UpdateLocalScrollProperties();

  if(source == mInternalXAnimation)
  {
    DALI_LOG_SCROLL_STATE("[0x%X] mInternalXAnimation[0x%X], expected[%.2f], actual[%.2f], post[%.2f]", this, mInternalXAnimation.GetObjectPtr(), mScrollTargetPosition.x, Self().GetCurrentProperty(SCROLL_PRE_POSITION).Get<Vector2>().x, mScrollPostPosition.x);

    if(!(mScrollStateFlags & AnimatingInternalY))
    {
      scrollingFinished = true;
    }
    mInternalXAnimation.Reset();
    // wrap pre scroll x position and set it
    if(mWrapMode)
    {
      const RulerDomain rulerDomain = mRulerX->GetDomain();
      mScrollPrePosition.x          = -WrapInDomain(-mScrollPrePosition.x, rulerDomain.min, rulerDomain.max);
      DALI_LOG_SCROLL_STATE("[0x%X] Setting SCROLL_PRE_POSITION To[%.2f, %.2f]", this, mScrollPrePosition.x, mScrollPrePosition.y);
      handle.SetProperty(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION, mScrollPrePosition);
    }
    SnapInternalXTo(mScrollPostPosition.x);
  }

  if(source == mInternalYAnimation)
  {
    DALI_LOG_SCROLL_STATE("[0x%X] mInternalYAnimation[0x%X], expected[%.2f], actual[%.2f], post[%.2f]", this, mInternalYAnimation.GetObjectPtr(), mScrollTargetPosition.y, DevelHandle::GetProperty(Self(), SCROLL_PRE_POSITION).Get<Vector2>().y, mScrollPostPosition.y);

    if(!(mScrollStateFlags & AnimatingInternalX))
    {
      scrollingFinished = true;
    }
    mInternalYAnimation.Reset();
    if(mWrapMode)
    {
      // wrap pre scroll y position and set it
      const RulerDomain rulerDomain = mRulerY->GetDomain();
      mScrollPrePosition.y          = -WrapInDomain(-mScrollPrePosition.y, rulerDomain.min, rulerDomain.max);
      DALI_LOG_SCROLL_STATE("[0x%X] Setting SCROLL_PRE_POSITION To[%.2f, %.2f]", this, mScrollPrePosition.x, mScrollPrePosition.y);
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

void ScrollView::OnSnapInternalPositionFinished(Animation& source)
{
  Actor self = Self();
  UpdateLocalScrollProperties();
  if(source == mInternalXAnimation)
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Finished X PostPosition Animation", this);

    // clear internal x animation flags
    mScrollStateFlags &= ~SCROLL_X_STATE_MASK;
    mInternalXAnimation.Reset();
    WrapPosition(mScrollPrePosition);
  }
  if(source == mInternalYAnimation)
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Finished Y PostPosition Animation", this);

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
  DALI_LOG_SCROLL_STATE("[0x%X] duration[%.2f]", this, duration);
  if(duration > Math::MACHINE_EPSILON_1)
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Starting X Snap Animation to[%.2f]", this, position);

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
  DALI_LOG_SCROLL_STATE("[0x%X] duration[%.2f]", this, duration);
  if(duration > Math::MACHINE_EPSILON_1)
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Starting Y Snap Animation to[%.2f]", this, position);

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
  if(mGestureStackDepth++ == 0)
  {
    Actor self = Self();
    StopTouchDownTimer();
    StopAnimation();
    mPanDelta     = Vector2::ZERO;
    mLastVelocity = Vector2::ZERO;
    if(!mScrolling)
    {
      mLockAxis = LockPossible;
    }

    if(mScrollStateFlags & SCROLL_X_STATE_MASK)
    {
      StopAnimation(mInternalXAnimation);
    }
    if(mScrollStateFlags & SCROLL_Y_STATE_MASK)
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
      mScrollCompletedSignal.Emit(-mScrollPostPosition);
    }
  }
}

void ScrollView::GestureContinuing(const Vector2& panDelta)
{
  mPanDelta.x += panDelta.x;
  mPanDelta.y += panDelta.y;

  // Save the velocity, there is a bug in PanGesture
  // Whereby the GestureState::FINISHED's velocity is either:
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
// BUG: GestureState::FINISHED doesn't always return velocity on release (due to
// timeDelta between last two events being 0 sometimes, or posiiton being the same)
void ScrollView::OnPan(const PanGesture& gesture)
{
  // Guard against destruction during signal emission
  // Note that Emit() methods are called indirectly e.g. from within ScrollView::OnGestureEx()
  Actor self(Self());

  if(!mSensitive)
  {
    DALI_LOG_SCROLL_STATE("[0x%X] Pan Ignored, Insensitive", this);

    // If another callback on the same original signal disables sensitivity,
    // this callback will still be called, so we must suppress it.
    return;
  }

  // translate Gesture input to get useful data...
  switch(gesture.GetState())
  {
    case GestureState::STARTED:
    {
      DALI_LOG_SCROLL_STATE("[0x%X] Pan Started", this);
      const Vector2& position = gesture.GetPosition();
      mPanStartPosition       = position - gesture.GetDisplacement();
      UpdateLocalScrollProperties();
      GestureStarted();
      mPanning = true;
      self.SetProperty(Toolkit::ScrollView::Property::PANNING, true);
      self.SetProperty(Toolkit::ScrollView::Property::START_PAGE_POSITION, Vector3(position.x, position.y, 0.0f));

      mConstraints.UpdateMainInternalConstraint(*this);
      Toolkit::ScrollBar scrollBar = mScrollBar.GetHandle();
      if(scrollBar && mTransientScrollBar)
      {
        Vector3                     size         = Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE);
        const Toolkit::RulerDomain& rulerDomainX = mRulerX->GetDomain();
        const Toolkit::RulerDomain& rulerDomainY = mRulerY->GetDomain();

        if((rulerDomainX.max > size.width) || (rulerDomainY.max > size.height))
        {
          scrollBar.ShowIndicator();
        }
      }
      break;
    }

    case GestureState::CONTINUING:
    {
      if(mPanning)
      {
        DALI_LOG_SCROLL_STATE("[0x%X] Pan Continuing", this);
        GestureContinuing(gesture.GetScreenDisplacement());
      }
      else
      {
        // If we do not think we are panning, then we should not do anything here
        return;
      }
      break;
    }

    case GestureState::FINISHED:
    case GestureState::CANCELLED:
    {
      if(mPanning)
      {
        DALI_LOG_SCROLL_STATE("[0x%X] Pan %s", this, ((gesture.GetState() == GestureState::FINISHED) ? "Finished" : "Cancelled"));

        UpdateLocalScrollProperties();
        mLastVelocity = gesture.GetVelocity();
        mPanning      = false;
        self.SetProperty(Toolkit::ScrollView::Property::PANNING, false);

        if(mConstraints.mScrollMainInternalPrePositionConstraint)
        {
          mConstraints.mScrollMainInternalPrePositionConstraint.Remove();
        }

        Toolkit::ScrollBar scrollBar = mScrollBar.GetHandle();
        if(scrollBar && mTransientScrollBar)
        {
          scrollBar.HideIndicator();
        }
      }
      else
      {
        // If we do not think we are panning, then we should not do anything here
        return;
      }
      break;
    }

    case GestureState::POSSIBLE:
    case GestureState::CLEAR:
    {
      // Nothing to do, not needed.
      break;
    }

  } // end switch(gesture.state)

  OnGestureEx(gesture.GetState());
}

void ScrollView::OnGestureEx(GestureState state)
{
  // call necessary signals for application developer

  if(state == GestureState::STARTED)
  {
    Vector2 currentScrollPosition = GetCurrentScrollPosition();
    Self().SetProperty(Toolkit::ScrollView::Property::SCROLLING, true);
    mScrolling = true;
    DALI_LOG_SCROLL_STATE("[0x%X] mScrollStartedSignal 2 [%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y);
    mScrollStartedSignal.Emit(currentScrollPosition);
  }
  else if((state == GestureState::FINISHED) ||
          (state == GestureState::CANCELLED)) // Finished/default
  {
    // when all the gestures have finished, we finish the transform.
    // so if a user decides to pan (1 gesture), and then pan+zoom (2 gestures)
    // then stop panning (back to 1 gesture), and then stop zooming (0 gestures).
    // this is the point we end, and perform necessary snapping.
    mGestureStackDepth--;
    if(mGestureStackDepth == 0)
    {
      // no flick if we have not exceeded min flick distance
      if((fabsf(mPanDelta.x) < mMinFlickDistance.x) && (fabsf(mPanDelta.y) < mMinFlickDistance.y))
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

    if(fabs(mScrollPrePosition.x - mScrollTargetPosition.x) > Math::MACHINE_EPSILON_10)
    {
      SnapInternalXTo(mScrollTargetPosition.x);
    }
    if(fabs(mScrollPrePosition.y - mScrollTargetPosition.y) > Math::MACHINE_EPSILON_10)
    {
      SnapInternalYTo(mScrollTargetPosition.y);
    }
    Vector2 currentScrollPosition = GetCurrentScrollPosition();
    DALI_LOG_SCROLL_STATE("[0x%X] mScrollCompletedSignal 6 [%.2f, %.2f]", this, currentScrollPosition.x, currentScrollPosition.y);
    mScrollCompletedSignal.Emit(currentScrollPosition);
  }
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

void ScrollView::ClampPosition(Vector2& position, ClampState2D& clamped) const
{
  Vector3 size = Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE);

  ::ClampPosition(size, mRulerX, mRulerY, position, clamped);
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

void ScrollView::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  ScrollViewPropertyHandler::Set(object, index, value);
}

Property::Value ScrollView::GetProperty(BaseObject* object, Property::Index index)
{
  return ScrollViewPropertyHandler::Get(object, index);
}

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

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
