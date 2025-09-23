#ifndef DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_H
#define DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/scroll-bar/scroll-bar.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-base-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-impl-constraints.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-effect.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class ScrollView;
typedef IntrusivePtr<ScrollView> ScrollViewPtr;

class ScrollInternalConstraints;
typedef IntrusivePtr<ScrollInternalConstraints> ScrollInternalConstraintsPtr;

class ScrollOvershootIndicator;
typedef IntrusivePtr<ScrollOvershootIndicator> ScrollOvershootIndicatorPtr;

class ScrollViewPropertyHandler;

/**
 * @copydoc Toolkit::ScrollView
 */
class ScrollView : public ScrollBase
{
public:
  /**
   * FindDirection specifies how searching is conducted within the Find... routines.
   */
  enum FindDirection
  {
    None  = -3, ///< Includes none within the search query.
    All   = -2, ///< Includes all within the search query.
    Left  = -1, ///< Includes only those not right !(>)
    Right = 1,  ///< Includes only those right (>)
    Up    = -1, ///< Includes only those not below  !(>)
    Down  = 1,  ///< Includes only those below (>)
    Out   = -1, ///< Includes only those not infront  !(>)
    In    = 1   ///< Includes only those infront (>)
  };

  enum LockAxis
  {
    LockPossible = 0, ///< Locking is possible, but not set in stone yet.
    LockHorizontal,   ///< Locking is set to horizontal. (can pan vertically)
    LockVertical,     ///< Locking is set to vertical. (can pan horizontally)
    LockNone          ///< Locking is set to none (free panning).
  };

  enum ScrollStateFlag
  {
    AnimatingInternalX = 0x01, ///< animating mPropertyX due to externally requested ScrollTo or internal snapping operation
    AnimatingInternalY = 0x02, ///< animating mPropertyY due to externally requested ScrollTo or internal snapping operation
    SnappingInternalX  = 0x04, ///< snapping mPropertyX back to mPropertyPreScroll x value to remove x overshoot over time
    SnappingInternalY  = 0x08, ///< snapping mPropertyY back to mPropertyPreScroll y value to remove y overshoot over time
  };

  static const unsigned int SCROLL_X_STATE_MASK    = AnimatingInternalX | SnappingInternalX;
  static const unsigned int SCROLL_Y_STATE_MASK    = AnimatingInternalY | SnappingInternalY;
  static const unsigned int SCROLL_ANIMATION_FLAGS = AnimatingInternalX | AnimatingInternalY;
  static const unsigned int SNAP_ANIMATION_FLAGS   = SnappingInternalX | SnappingInternalY;

private:
  typedef std::vector<Dali::Toolkit::ScrollViewEffect> ScrollViewEffectContainer; ///< Container of Dali::Toolkit::ScrollViewEffect
  typedef ScrollViewEffectContainer::iterator          ScrollViewEffectIter;      ///< Iterator for Dali::Toolkit::ScrollViewEffectContainer

public:
  /**
   * Create a new ScrollView.
   * @return A public handle to the newly allocated ScrollView.
   */
  static Dali::Toolkit::ScrollView New();

public:
  /**
   * @copydoc Toolkit::ScrollView::GetScrollSnapAlphaFunction
   */
  AlphaFunction GetScrollSnapAlphaFunction() const
  {
    return mSnapAlphaFunction;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetScrollSnapAlphaFunction
   */
  void SetScrollSnapAlphaFunction(AlphaFunction alpha)
  {
    mSnapAlphaFunction = alpha;
  }

  /**
   * @copydoc Toolkit::ScrollView::GetScrollFlickAlphaFunction
   */
  AlphaFunction GetScrollFlickAlphaFunction() const
  {
    return mFlickAlphaFunction;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetScrollFlickAlphaFunction
   */
  void SetScrollFlickAlphaFunction(AlphaFunction alpha)
  {
    mFlickAlphaFunction = alpha;
  }

  /**
   * @copydoc Toolkit::ScrollView::GetScrollSnapDuration
   */
  float GetScrollSnapDuration() const
  {
    return mSnapDuration;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetScrollSnapDuration
   */
  void SetScrollSnapDuration(float time)
  {
    mSnapDuration = time;
  }

  /**
   * @copydoc Toolkit::ScrollView::GetScrollFlickDuration
   */
  float GetScrollFlickDuration() const
  {
    return mFlickDuration;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetScrollFlickDuration
   */
  void SetScrollFlickDuration(float time)
  {
    mFlickDuration = time;
  }

  /**
   * @copydoc Toolkit::ScrollView::ApplyEffect
   */
  void ApplyEffect(Toolkit::ScrollViewEffect effect);

  /**
   * @copydoc Toolkit::ScrollView::RemoveEffect
   */
  void RemoveEffect(Toolkit::ScrollViewEffect effect);

  /**
   * @copydoc Toolkit::ScrollView::RemoveAllEffects
   */
  void RemoveAllEffects();

  /**
   * @copydoc Toolkit::ScrollView::ApplyConstraintToChildren
   */
  void ApplyConstraintToChildren(Constraint constraint);

  /**
   * @copydoc Toolkit::ScrollView::RemoveConstraintsFromChildren
   */
  void RemoveConstraintsFromChildren();

  /**
   * @copydoc Toolkit::ScrollView::GetRulerX
   */
  const RulerPtr GetRulerX() const
  {
    return mRulerX;
  }

  /**
   * @copydoc Toolkit::ScrollView::GetRulerY
   */
  const RulerPtr GetRulerY() const
  {
    return mRulerY;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetRulerX
   */
  void SetRulerX(RulerPtr ruler);

  /**
   * @copydoc Toolkit::ScrollView::SetRulerY
   */
  void SetRulerY(RulerPtr ruler);

  /**
   * Retrieve the touch sensitivity.
   *
   * @return whether the touch sensitivity is true or false.
   */
  bool GetScrollSensitive()
  {
    return mSensitive;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetScrollSensitive
   */
  void SetScrollSensitive(bool sensitive);

  /**
   * @copydoc Toolkit::ScrollView::SetMaxOvershoot
   */
  void SetMaxOvershoot(float overshootX, float overshootY);

  /**
   * @copydoc Toolkit::ScrollView::SetSnapOvershootAlphaFunction
   */
  void SetSnapOvershootAlphaFunction(AlphaFunction alpha)
  {
    mSnapOvershootAlphaFunction = alpha;
  }

  /**
   * Retrieve the duartion of Snap Overshoot animation
   *
   * @return the duration.
   */
  float GetSnapOvershootDuration()
  {
    return mSnapOvershootDuration;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetSnapOvershootDuration
   */
  void SetSnapOvershootDuration(float duration)
  {
    mSnapOvershootDuration = duration;
  }

  /**
   * Retrieve whether Actor Auto-Snap mode is enabled or not.
   *
   * @return Actor Auto-Snap mode Enabled flag.
   */
  bool GetActorAutoSnap();

  /**
   * @copydoc Toolkit::ScrollView::SetActorAutoSnap
   */
  void SetActorAutoSnap(bool enable)
  {
    mActorAutoSnapEnabled = enable;
  }

  /**
   * Enables or Disables Auto Resizing mode for ScrollView contents.
   *
   * When enabled, the ScrollView's X/Y Domains are restricted to the
   * dimensions of the content's bounds, which may change as Actors are
   * Added/Removed, and repositioned.
   *
   * @note This has been disabled for now, as this requires some fundamental
   * changes to the way Actors positions and bounds are retrieved.
   * (currently only constraints have these initial state knowledge)
   *
   * @param[in] enable Enables (true), or disables (false) Auto Resize.
   */
  void SetAutoResize(bool enable);

  /**
   * Returns whether the wrap mode has been enabled (true) or not (false).
   *
   * @return Wrap Mode Enabled flag.
   */
  bool GetWrapMode() const
  {
    return mWrapMode;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetWrapMode
   */
  void SetWrapMode(bool enable);

  /**
   * @copydoc Toolkit::ScrollView::GetScrollupdateDistance
   */
  int GetScrollUpdateDistance() const
  {
    return mScrollUpdateDistance;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetScrollUpdateDistance
   */
  void SetScrollUpdateDistance(int distance)
  {
    mScrollUpdateDistance = distance;
  }

  /**
   * @copydoc Toolkit::ScrollView::GetAxisAutoLock
   */
  bool GetAxisAutoLock() const
  {
    return mAxisAutoLock;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetAxisAutoLock
   */
  void SetAxisAutoLock(bool enable);

  /**
   * @copydoc Toolkit::ScrollView::GetAxisAutoLockGradient
   */
  float GetAxisAutoLockGradient() const
  {
    return mAxisAutoLockGradient;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetAxisAutoLockGradient
   */
  void SetAxisAutoLockGradient(float gradient);

  /**
   * @copydoc Toolkit::ScrollView::GetFrictionCoefficient
   */
  float GetFrictionCoefficient() const
  {
    return mFrictionCoefficient;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetFrictionCoefficient
   */
  void SetFrictionCoefficient(float friction);

  /**
   * @copydoc Toolkit::ScrollView::GetFlickSpeedCoefficient
   */
  float GetFlickSpeedCoefficient() const
  {
    return mFlickSpeedCoefficient;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetFlickSpeedCoefficient
   */
  void SetFlickSpeedCoefficient(float speed)
  {
    mFlickSpeedCoefficient = speed;
  }

  /**
   * @copydoc Toolkit::ScrollView::GetMinimumDistanceForFlick
   */
  Vector2 GetMinimumDistanceForFlick() const
  {
    return mMinFlickDistance;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetMinimumDistanceForFlick
   */
  void SetMinimumDistanceForFlick(const Vector2& distance)
  {
    mMinFlickDistance = distance;
  }

  /**
   * @copydoc Toolkit::ScrollView::GetMinimumSpeedForFlick
   */
  float GetMinimumSpeedForFlick() const
  {
    return mFlickSpeedThreshold;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetMinimumSpeedForFlick
   */
  void SetMinimumSpeedForFlick(float speed)
  {
    mFlickSpeedThreshold = speed;
  }

  /**
   * @copydoc Toolkit::ScrollView::GetMaxFlickSpeed
   */
  float GetMaxFlickSpeed() const
  {
    return mMaxFlickSpeed;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetMaxFlickSpeed
   */
  void SetMaxFlickSpeed(float speed)
  {
    mMaxFlickSpeed = speed;
  }

  /**
   * @copydoc Toolkit::ScrollView::GetWheelScrollDistanceStep
   */
  Vector2 GetWheelScrollDistanceStep() const
  {
    return mWheelScrollDistanceStep;
  }

  /**
   * @copydoc Toolkit::ScrollView::SetWheelScrollDistanceStep
   */
  void SetWheelScrollDistanceStep(Vector2 step)
  {
    mWheelScrollDistanceStep = step;
  }

  /**
   * @copydoc Toolkit::ScrollView::GetCurrentPage
   */
  unsigned int GetCurrentPage() const;

  /**
   * @copydoc Toolkit::ScrollView::GetCurrentScrollPosition
   */
  Vector2 GetCurrentScrollPosition() const;

  /**
   * @copydoc ScrollTo(const Vector2&)
   */
  void TransformTo(const Vector2& position,
                   DirectionBias  horizontalBias = DIRECTION_BIAS_NONE,
                   DirectionBias  verticalBias   = DIRECTION_BIAS_NONE);

  /**
   * @copydoc ScrollTo(const Vector2&, float, AlhpaFunction, DirectionBias, DirectionBias)
   */
  void TransformTo(const Vector2& position, float duration, AlphaFunction alpha, DirectionBias horizontalBias = DIRECTION_BIAS_NONE, DirectionBias verticalBias = DIRECTION_BIAS_NONE);

  /**
   * @copydoc Toolkit::ScrollView::ScrollTo(const Vector2 &position)
   */
  void ScrollTo(const Vector2& position);

  /**
   * @copydoc Toolkit::Scrollable::ScrollTo(const Vector2& position, float duration)
   */
  void ScrollTo(const Vector2& position, float duration);

  /**
   * @copydoc Toolkit::Scrollable::ScrollTo(const Vector2& position, float duration, AlphaFunction alpha)
   */
  void ScrollTo(const Vector2& position, float duration, AlphaFunction alpha);

  /**
   * @copydoc Toolkit::ScrollView::ScrollTo(const Vector2 &position, float duration, DirectionBias horizontalBias, DirectionBias verticalBias)
   */
  void ScrollTo(const Vector2& position, float duration, DirectionBias horizontalBias, DirectionBias verticalBias);

  /**
   * @copydoc Toolkit::ScrollView::ScrollTo(const Vector2 &position, float duration, AlphaFunction alpha, DirectionBias horizontalBias, DirectionBias verticalBias)
   */
  void ScrollTo(const Vector2& position, float duration, AlphaFunction alpha, DirectionBias horizontalBias, DirectionBias verticalBias);

  /**
   * @copydoc Toolkit::ScrollView::ScrollTo(unsigned int page)
   */
  void ScrollTo(unsigned int page);

  /**
   * @copydoc Toolkit::ScrollView::ScrollTo(unsigned int page, float duration, DirectionBias bias)
   */
  void ScrollTo(unsigned int page, float duration, DirectionBias bias = DIRECTION_BIAS_NONE);

  /**
   * @copydoc Toolkit::ScrollView::ScrollTo(Actor& actor)
   */
  void ScrollTo(Actor& actor);

  /**
   * @copydoc Toolkit::ScrollView::ScrollTo(Actor& actor, float duration)
   */
  void ScrollTo(Actor& actor, float duration);

  /**
   * @copydoc Toolkit::ScrollView::SetScrollingDirection()
   */
  void SetScrollingDirection(Radian direction, Radian threshold);

  /**
   * @copydoc Toolkit::ScrollView::RemoveScrollingDirection()
   */
  void RemoveScrollingDirection(Radian angle);

  /**
   * Finds the closest Actor to the current center of the ScrollView.
   *
   * @return A handle to the actor if found, or an empty handle if not.
   */
  Actor FindClosestActor();

  /**
   * Finds the closest Actor to position in ScrollView
   *
   * @param[in] position position within ScrollView.
   * @param[in] dirX Whether to search only those elements that are Left,Right, or All
   * @param[in] dirY Whether to search only those elements that are Up,Down, or All
   * @param[in] dirZ Whether to search only those elements that are Out,In, or All
   * @return A handle to the actor if found, or an empty handle if not.
   */
  Actor FindClosestActorToPosition(const Vector3& position, FindDirection dirX = All, FindDirection dirY = All, FindDirection dirZ = All);

  /**
   * @copydoc Toolkit::ScrollView::ScrollToSnapPoint
   */
  bool ScrollToSnapPoint();

  /**
   * Stops animation
   */
  void StopAnimation(void);

  /**
   * Stops the input animation
   *
   * @param[in] the animation to stop
   */
  void StopAnimation(Animation& animation);

  /**
   * Animates to position transform.
   *
   * @param[in] position The position to animate to
   * @param[in] positionDuration The number of seconds this animation should run for in each axis.
   * @param[in] alpha The easing alpha function to use.
   * @param[in] findShortcuts (optional) Whether to find the shortest route (in Wrap mode)
   * @param[in] horizontalBias (optional) Whether to bias animation to left or right (or no biasing)
   * @param[in] verticalBias (optional) Whether to bias animation to top or bottom (or no biasing)
   * @return True if animation necessary and taking place to reach desired transform.
   */
  bool AnimateTo(const Vector2& position, const Vector2& positionDuration, AlphaFunction alpha, bool findShortcuts = true, DirectionBias horizontalBias = DIRECTION_BIAS_NONE, DirectionBias verticalBias = DIRECTION_BIAS_NONE, SnapType snapType = SNAP);

  /**
   * @copydoc Toolkit::Scrollable::AddOverlay()
   */
  void AddOverlay(Actor actor);

  /**
   * @copydoc Toolkit::Scrollable::RemoveOverlay()
   */
  void RemoveOverlay(Actor actor);

  /**
   * @copydoc Toolkit::Internal::Scrollable::SetOvershootSize
   */
  void SetOvershootSize(const Vector2& size);

  /**
   * @copydoc Toolkit::Internal::Scrollable::SetOvershootEffectColor
   */
  void SetOvershootEffectColor(const Vector4& color);

  //properties

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty(BaseObject* object, Property::Index index, const Property::Value& value);

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty(BaseObject* object, Property::Index index);

public: //Signals
  /**
   * @copydoc Dali::Toolkit::ScrollView::SnapStartedSignal()
   */
  Toolkit::ScrollView::SnapStartedSignalType& SnapStartedSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor);

private: // private overridden functions from CustomActorImpl and Controls
  /**
   * @copydoc Dali::CustomActorImpl::OnSizeAnimation(Animation&, const Vector3&)
   */
  void OnSizeAnimation(Animation& animation, const Vector3& targetSize) override;

  /**
   * @copydoc CustomActorImpl::OnSizeSet(const Vector3&)
   */
  void OnSizeSet(const Vector3& size) override;

  /**
   * From CustomActorImpl; called after a child has been added to the owning actor.
   * @param[in] child The child which has been added.
   */
  void OnChildAdd(Actor& child) override;

  /**
   * From CustomActorImpl; called shortly before a child is removed from the owning actor.
   * @param[in] child The child being removed.
   */
  void OnChildRemove(Actor& child) override;

  /**
   * Called after a wheelEvent is received by the owning actor.
   * @param[in] actor Actor associated with the event.
   * @param[in] event The wheel event.
   * @return True if the event should be consumed.
   */
  bool OnWheelEvent(Actor actor, const WheelEvent& event);

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  void OnInitialize() override;

  /**
   * @copydoc Toolkit::Internal::Control::CreateAccessibleObject()
   */
  DevelControl::ControlAccessible* CreateAccessibleObject() override;

  /**
   * @copydoc CustomActorImpl::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc CustomActorImpl::OnSceneDisconnection()
   */
  void OnSceneDisconnection() override;

  /**
   * @copydoc Toolkit::Control::OnAccessibilityPan()
   */
  bool OnAccessibilityPan(PanGesture gesture) override;

  /**
   * @copydoc Toolkit::Scrollable::EnableScrollOvershoot()
   */
  void EnableScrollOvershoot(bool enable) override;

private:
  /**
   * Called after a touchSignal is received by the owning actor.
   *
   * We don't consume these events as content within the container may consume events.
   *
   * @param[in] actor The touched actor.
   * @param[in] touch The touch information.
   * @return True if the event should be consumed.
   */
  bool OnTouch(Actor actor, const TouchEvent& touch);

  /**
   * Start a timer which calls OnTouchDownTimeout()
   */
  void StartTouchDownTimer();

  /**
   * Stop a timer which calls OnTouchDownTimeout()
   */
  void StopTouchDownTimer();

  /**
   * Helper to detect when touch-point has been down (outside of pan gesture)
   */
  bool OnTouchDownTimeout();

  /**
   * Called whenever a snap animation has completed
   * @param[in] source the Animation instance that has completed.
   * Resets all scrolling animations and states, leaving current scroll position at SCROLL_POSITION
   */
  void ResetScrolling();

  /**
   * Updates mScrollInternalPosition, mScrollPrePosition and mScrollPostPosition from their property counterparts
   */
  void UpdateLocalScrollProperties();

  /**
   * Makes sure scroll values are ready for animated scrolling
   */
  void PreAnimatedScrollSetup();

  /**
   * Finish an animated scroll, ensuring all scroll properties are updated
   * and synchronised
   */
  void FinaliseAnimatedScroll();

  /**
   * Animates the internal x property to the given value
   *
   * @param[in] position The X position to animate to
   * @param[in] duration The time in seconds for animation
   * @param[in] alpha The alpha function to use for animating
   */
  void AnimateInternalXTo(float position, float duration, AlphaFunction alpha);

  /**
   * Animates the internal y property to the given value
   *
   * @param[in] position The Y position to animate to
   * @param[in] duration The time in seconds for animation
   * @param[in] alpha The alpha function to use for animating
   */
  void AnimateInternalYTo(float position, float duration, AlphaFunction alpha);

  /**
   * Called whenever a snap animation on the x-axis has completed
   * @param[in] source the Animation instance that has completed.
   */
  void OnScrollAnimationFinished(Animation& source);

  /**
   * Called when either the X or Y internal scroll positions have finished snapping back to SCROLL_PRE_POSITION
   *
   * @param[in] source the Animation instance that has completed.
   */
  void OnSnapInternalPositionFinished(Animation& source);

  /**
   * Called whenever a snap animation on the x-axis has completed and we need to snap pre scroll
   * position to our clamped position
   * @param[in] position The x position to snap pre scroll property to
   */
  void SnapInternalXTo(float position);

  /**
   * Called whenever a snap animation on the y-axis has completed and we need to snap pre scroll
   * position to our clamped position
   * @param[in] position The y position to snap pre scroll property to
   */
  void SnapInternalYTo(float position);

  /**
   * Called when the gesture starts.
   */
  void GestureStarted();

  /**
   * Amalgamated Gesture Continuing event
   *
   * @param[in] panDelta average panning delta from base position (0)
   */
  void GestureContinuing(const Vector2& panDelta);

  /**
   * Called upon pan gesture event.
   *
   * @param[in] gesture The gesture event.
   */
  void OnPan(const PanGesture& pan);

  /**
   * Extension of the above gestures.
   *
   * @param[in] gesture The gesture event.
   */
  void OnGestureEx(GestureState state);

  /**
   * Performs snapping while taking into account Velocity of gesture
   * (velocity in pixels/sec)
   *
   * @param[in] velocity velocity in pixels/sec
   */
  bool SnapWithVelocity(Vector2 velocity);

  /**
   * Finishes Container Transform
   * (occurs upon finishing gesture i.e. releasing)
   */
  void FinishTransform();

  /**
   * Clamps position within the domain set up by X/Y Rulers
   *
   * @param[in,out] position The position you wish to clamp
   */
  void ClampPosition(Vector2& position) const;

  /**
   * Clamps position within the domain set up by X/Y Rulers
   *
   * @param[in,out] position The position you wish to clamp
   * @param[out] clamped The results of the clamping.
   */
  void ClampPosition(Vector2& position, ClampState2D& clamped) const;

  /**
   * Wraps position within the domain set up by X/Y Rulers
   *
   * @note Only wraps if mWrapMode is enabled, and respective domains
   * are enabled.
   *
   * @param[in,out] position The position you wish to wrap
   */
  void WrapPosition(Vector2& position) const;

protected:
  class ScrollViewAccessible : public Scrollable::ScrollableAccessible
  {
  public:
    using Scrollable::ScrollableAccessible::ScrollableAccessible;

    /**
     * @copydoc Dali::Toolkit::DevelControl::ControlAccessible::ScrollToChild()
     */
    bool ScrollToChild(Actor child) override;
  };

  /**
   * Construct a new ScrollView.
   */
  ScrollView();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ScrollView();

private:
  /**
   * Searches this ScrollView, and attempts to Unbind
   * systematically this Actor from the ScrollView attached.
   *
   * @param[in] child The actor to be unbound.
   */
  void FindAndUnbindActor(Actor child) override;

  /**
   * Gets position property.
   *
   * @return The current position
   */
  Vector2 GetPropertyPrePosition() const;

  /**
   * Gets position property.
   *
   * @return The current position
   */
  Vector2 GetPropertyPosition() const;

  /**
   * Handles a Stopped animation. Its position properties need to be saved, and the animation flag
   * switched off.
   */
  void HandleStoppedAnimation();

  /**
   * Handles a Stopped animation (whether the animation completed, or was
   * manually stopped). Its position properties need to be saved, and the
   * animation flag switched off.
   */
  void HandleSnapAnimationFinished();

  /**
   * Checks if the property notifications are active and adds them if not
   */
  void SetScrollUpdateNotification(bool enabled);

  /**
   * Refresh the ScrollView (used when animating to update application developer of changes)
   * @return True if the refresh timer should be kept running.
   */
  void OnScrollUpdateNotification(Dali::PropertyNotification& source);

private:
  // Undefined
  ScrollView(const ScrollView&);

  // Undefined
  ScrollView& operator=(const ScrollView& rhs);

private:
  ScrollViewConstraints mConstraints;

  unsigned long mTouchDownTime; ///< The touch down time

  int     mGestureStackDepth; ///< How many gestures are currently occuring.
  Vector2 mPanStartPosition;  ///< Where the pan gesture's touch down occured
  Vector2 mPanDelta;          ///< Amount currently panned.

  unsigned int mScrollStateFlags; ///< flags indicating current state of scrolling
  // Scroll delegate pre and post position properties...
  Vector2 mScrollPrePosition;    ///< Wrapped scroll position, but not clamped
  Vector2 mScrollPostPosition;   ///< Wrapped and clamped, this is the final scroll position used
  Vector2 mScrollTargetPosition; ///< Final target position for an animated scroll
  Vector2 mDomainOffset;         ///< Domain offset (this keeps track of the domain boundaries that scroll positions traverses)

  // Rulers for each axes...
  RulerPtr mRulerX;
  RulerPtr mRulerY;

  // Last property values set to ScrollView
  Vector2 mMinScroll;
  Vector2 mMaxScroll;

  Animation mInternalXAnimation; ///< Animates mPropertyX to a snap position or application requested scroll position
  Animation mInternalYAnimation; ///< Animates mPropertyY to a snap position or application requested scroll position

  Vector2  mLastVelocity; ///< Record the last velocity from PanGesture (Finish event doesn't have correct velocity)
  LockAxis mLockAxis;

  Timer mTouchDownTimer; ///< Used to interrupt snap-animation. This cannot be done in OnTouch without breaking fast flick behavior.

  float                      mScrollUpdateDistance;      ///< Distance for scrolling to travel for the scroll update notifications
  Dali::PropertyNotification mScrollXUpdateNotification; ///< scroll x position update notification
  Dali::PropertyNotification mScrollYUpdateNotification; ///< scroll y position update notification

  Actor mInternalActor; ///< Internal actor (we keep internal actors in here e.g. scrollbars, so we can ignore it in searches)

  ScrollViewEffectContainer mEffects; ///< Container keeping track of all the applied effects.

  Vector2       mMaxOvershoot;               ///< Number of scrollable pixels that will take overshoot from 0.0f to 1.0f
  Vector2       mUserMaxOvershoot;           ///< Set by user, allows overriding of default max overshoot for the scroll indicator
  float         mSnapOvershootDuration;      ///< Duration for overshoot snapping back to Vector2::ZERO
  AlphaFunction mSnapOvershootAlphaFunction; ///< AlphaFunction to be used for this overshoot.

  float         mSnapDuration;      ///< Time for the snap animation to take (in seconds).
  AlphaFunction mSnapAlphaFunction; ///< AlphaFunction to be used for the Snap Animation.

  Vector2       mMinFlickDistance;    ///< Minimum pan distance required for a flick
  float         mFlickSpeedThreshold; ///< Minimum pan speed required for a flick in pixels/ms
  float         mFlickDuration;       ///< Time for the flick animation to take (in seconds).
  AlphaFunction mFlickAlphaFunction;  ///< AlphaFunction to be used for the Flick Animation.

  float mAxisAutoLockGradient;  ///< Axis Auto-lock gradient threshold. Above this gradient and it will lock scrolling to closest axis.
  float mFrictionCoefficient;   ///< Friction coefficient. Amount of friction to apply to free panning flick animation. in stage.lengths/sec
  float mFlickSpeedCoefficient; ///< Flick velocity coefficient. Input touch velocity is multiplied by this.
  float mMaxFlickSpeed;         ///< Maximum flick speed. Maximum speed of flick in stage.lengths/sec.

  Vector2 mWheelScrollDistanceStep; ///< The step of scroll distance in actor coordinates in X and Y axes for each wheel event received.

  ScrollOvershootIndicatorPtr    mOvershootIndicator;
  WeakHandle<Toolkit::ScrollBar> mScrollBar;

  Toolkit::ScrollView::SnapStartedSignalType mSnapStartedSignal;

  bool mInAccessibilityPan : 1;         ///< With AccessibilityPan its easier to move between snap positions
  bool mScrolling : 1;                  ///< Flag indicating whether the scroll view is being scrolled (by user or animation)
  bool mScrollInterrupted : 1;          ///< Flag set for when a down event interrupts a scroll
  bool mPanning : 1;                    ///< Whether scroll view is currently panning or not
  bool mSensitive : 1;                  ///< Scroll Sensitivity Flag.
  bool mTouchDownTimeoutReached : 1;    ///< Indicates when down event timeout occured without corresponding up event (touch still down)
  bool mActorAutoSnapEnabled : 1;       ///< Whether to automatically snap to closest actor.
  bool mAutoResizeContainerEnabled : 1; ///< Whether to automatically resize container (affects RulerDomain's on X/Y axes)
  bool mWrapMode : 1;                   ///< Whether to wrap contents based on container size.
  bool mAxisAutoLock : 1;               ///< Whether to automatically lock axis when panning.
  bool mAlterChild : 1;                 ///< Internal flag to control behavior of OnChildAdd/OnChildRemove when Adding internal Actors.
  bool mDefaultMaxOvershoot : 1;        ///< Whether to use default max overshoot or application defined one
  bool mCanScrollHorizontal : 1;        ///< Local value of our property to check against
  bool mCanScrollVertical : 1;          ///< Local value of our property to check against
  bool mTransientScrollBar : 1;         ///< True if scroll-bar should be automatically show/hidden during/after panning

  friend ScrollViewConstraints;
  friend ScrollViewPropertyHandler;
};

/**
 * Returns whether to lock scrolling to a particular axis
 *
 * @param[in] panDelta Distance panned since gesture started
 * @param[in] currentLockAxis The current lock axis value
 * @param[in] lockGradient How quickly to lock to a particular axis
 *
 * @return The new axis lock state
 */
ScrollView::LockAxis GetLockAxis(const Vector2& panDelta, ScrollView::LockAxis currentLockAxis, float lockGradient);

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::ScrollView& GetImpl(Toolkit::ScrollView& scrollView)
{
  DALI_ASSERT_ALWAYS(scrollView);

  Dali::RefObject& handle = scrollView.GetImplementation();

  return static_cast<Toolkit::Internal::ScrollView&>(handle);
}

inline const Toolkit::Internal::ScrollView& GetImpl(const Toolkit::ScrollView& scrollView)
{
  DALI_ASSERT_ALWAYS(scrollView);

  const Dali::RefObject& handle = scrollView.GetImplementation();

  return static_cast<const Toolkit::Internal::ScrollView&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_H
