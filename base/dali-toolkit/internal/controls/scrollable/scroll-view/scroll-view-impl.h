#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_H__

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

// EXTERNAL INCLUDES
#include <dali/dali.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-base-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-effect.h>

// predefined effect includes
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-custom-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class ScrollView;
typedef IntrusivePtr<ScrollView>    ScrollViewPtr;

class ScrollInternalConstraints;
typedef IntrusivePtr<ScrollInternalConstraints>    ScrollInternalConstraintsPtr;

class ScrollOvershootIndicator;
typedef IntrusivePtr<ScrollOvershootIndicator> ScrollOvershootIndicatorPtr;

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
    None = -3,        ///< Includes none within the search query.
    All = -2,         ///< Includes all within the search query.
    Left = -1,        ///< Includes only those not right !(>)
    Right = 1,        ///< Includes only those right (>)
    Up = -1,          ///< Includes only those not below  !(>)
    Down = 1,         ///< Includes only those below (>)
    Out = -1,         ///< Includes only those not infront  !(>)
    In = 1            ///< Includes only those infront (>)
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

  static const unsigned int SCROLL_X_STATE_MASK = AnimatingInternalX | SnappingInternalX;
  static const unsigned int SCROLL_Y_STATE_MASK = AnimatingInternalY | SnappingInternalY;
  static const unsigned int SCROLL_ANIMATION_FLAGS = AnimatingInternalX | AnimatingInternalY;
  static const unsigned int SNAP_ANIMATION_FLAGS = SnappingInternalX | SnappingInternalY;

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
  AlphaFunction GetScrollSnapAlphaFunction() const;

  /**
   * @copydoc Toolkit::ScrollView::SetScrollSnapAlphaFunction
   */
  void SetScrollSnapAlphaFunction(AlphaFunction alpha);

  /**
   * @copydoc Toolkit::ScrollView::GetScrollFlickAlphaFunction
   */
  AlphaFunction GetScrollFlickAlphaFunction() const;

  /**
   * @copydoc Toolkit::ScrollView::SetScrollFlickAlphaFunction
   */
  void SetScrollFlickAlphaFunction(AlphaFunction alpha);

  /**
   * @copydoc Toolkit::ScrollView::GetScrollSnapDuration
   */
  float GetScrollSnapDuration() const;

  /**
   * @copydoc Toolkit::ScrollView::SetScrollSnapDuration
   */
  void SetScrollSnapDuration(float time);

  /**
   * @copydoc Toolkit::ScrollView::GetScrollFlickDuration
   */
  float GetScrollFlickDuration() const;

  /**
   * @copydoc Toolkit::ScrollView::SetScrollFlickDuration
   */
  void SetScrollFlickDuration(float time);

  /**
   * @copydoc Toolkit::ScrollView::ApplyEffect
   */
  void ApplyEffect(Toolkit::ScrollViewEffect effect);

  /**
   * @brief ApplyEffect Applies a predefined effect
   * @param effect
   */
  Toolkit::ScrollViewEffect ApplyEffect(Toolkit::ScrollView::PageEffect effect);

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
  const RulerPtr GetRulerX() const;

  /**
   * @copydoc Toolkit::ScrollView::GetRulerY
   */
  const RulerPtr GetRulerY() const;

  /**
   * @copydoc Toolkit::ScrollView::SetRulerX
   */
  void SetRulerX(RulerPtr ruler);

  /**
   * @copydoc Toolkit::ScrollView::SetRulerY
   */
  void SetRulerY(RulerPtr ruler);

  /**
   * @copydoc Toolkit::ScrollView::SetRulerScaleX
   */
  void SetRulerScaleX(RulerPtr ruler);

  /**
   * @copydoc Toolkit::ScrollView::SetRulerScaleY
   */
  void SetRulerScaleY(RulerPtr ruler);

  /**
   * Set Rotation axis ruler (defines how rotating is snapped in radians)
   * @param[in] ruler The ruler to be used for the Rotation axis
   */
  void SetRulerRotation(RulerPtr ruler);

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
  void SetSnapOvershootAlphaFunction(AlphaFunction alpha);

  /**
   * @copydoc Toolkit::ScrollView::SetSnapOvershootDuration
   */
  void SetSnapOvershootDuration(float duration);

  /**
   * @copydoc Toolkit::ScrollView::SetTouchesRequiredForPanning
   */
  void SetTouchesRequiredForPanning(unsigned int minTouches, unsigned int maxTouches, bool endOutside);

  /**
   * @copydoc Toolkit::ScrollView::SetActorAutoSnap
   */
  void SetActorAutoSnap(bool enable);

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
  bool GetWrapMode() const;

  /**
   * @copydoc Toolkit::ScrollView::SetWrapMode
   */
  void SetWrapMode(bool enable);

  /**
   * @copydoc Toolkit::ScrollView::GetRefreshInterval
   */
  int GetRefreshInterval() const;

  /**
   * @copydoc Toolkit::ScrollView::SetRefreshInterval
   */
  void SetRefreshInterval(int milliseconds);

  /**
   * @copydoc Toolkit::ScrollView::GetAxisAutoLock
   */
  bool GetAxisAutoLock() const;

  /**
   * @copydoc Toolkit::ScrollView::SetAxisAutoLock
   */
  void SetAxisAutoLock(bool enable);

  /**
   * @copydoc Toolkit::ScrollView::GetAxisAutoLockGradient
   */
  float GetAxisAutoLockGradient() const;

  /**
   * @copydoc Toolkit::ScrollView::SetAxisAutoLockGradient
   */
  void SetAxisAutoLockGradient(float gradient);

  /**
   * @copydoc Toolkit::ScrollView::GetFrictionCoefficient
   */
  float GetFrictionCoefficient() const;

  /**
   * @copydoc Toolkit::ScrollView::SetFrictionCoefficient
   */
  void SetFrictionCoefficient(float friction);

  /**
   * @copydoc Toolkit::ScrollView::GetFlickSpeedCoefficient
   */
  float GetFlickSpeedCoefficient() const;

  /**
   * @copydoc Toolkit::ScrollView::SetFlickSpeedCoefficient
   */
  void SetFlickSpeedCoefficient(float speed);

  /**
   * @copydoc Toolkit::ScrollView::GetMaxFlickSpeed
   */
  float GetMaxFlickSpeed() const;

  /**
   * @copydoc Toolkit::ScrollView::SetMaxFlickSpeed
   */
  void SetMaxFlickSpeed(float speed);

  /**
   * @copydoc Toolkit::ScrollView::GetMouseWheelScrollDistanceStep
   */
  Vector2 GetMouseWheelScrollDistanceStep() const;

  /**
   * @copydoc Toolkit::ScrollView::SetMouseWheelScrollDistanceStep
   */
  void SetMouseWheelScrollDistanceStep(Vector2 step);

  /**
   * @copydoc Toolkit::ScrollView::GetCurrentPage
   */
  unsigned int GetCurrentPage() const;

  /**
   * @copydoc Toolkit::ScrollView::GetCurrentScrollPosition
   */
  Vector3 GetCurrentScrollPosition() const;

  /**
   * @copydoc Toolkit::ScrollView::SetScrollPosition
   */
  void SetScrollPosition(const Vector3& position);

  /**
   * @copydoc Toolkit::ScrollView::GetCurrentScrollScale
   */
  Vector3 GetCurrentScrollScale() const;

  /**
   * @copydoc Toolkit::Scrollable::GetDomainSize
   */
  Vector3 GetDomainSize() const;

  /**
   * @copydoc Toolkit::ScrollView::TransformTo(const Vector3& position, const Vector3& scale, float rotation)
   */
  void TransformTo(const Vector3& position, const Vector3& scale, float rotation,
                   DirectionBias horizontalBias = DirectionBiasNone, DirectionBias verticalBias = DirectionBiasNone);

  /**
   * @copydoc Toolkit::ScrollView::TransformTo(const Vector3& position, const Vector3& scale, float rotation, float duration)
   */
  void TransformTo(const Vector3& position, const Vector3& scale, float rotation, float duration,
                   DirectionBias horizontalBias = DirectionBiasNone, DirectionBias verticalBias = DirectionBiasNone);

  /**
   * @copydoc Toolkit::ScrollView::ScrollTo(const Vector3 &position)
   */
  void ScrollTo(const Vector3 &position);

  /**
   * @copydoc Toolkit::Scrollable::ScrollTo(const Vector3& position, float duration)
   */
  void ScrollTo(const Vector3& position, float duration);

  /**
   * @copydoc Toolkit::ScrollView::ScrollTo(const Vector3 &position, float duration, DirectionBias horizontalBias, DirectionBias verticalBias)
   */
  void ScrollTo(const Vector3& position, float duration,
                DirectionBias horizontalBias, DirectionBias verticalBias);

  /**
   * @copydoc Toolkit::ScrollView::ScrollTo(unsigned int page)
   */
  void ScrollTo(unsigned int page);

  /**
   * @copydoc Toolkit::ScrollView::ScrollTo(unsigned int page, float duration, DirectionBias bias)
   */
  void ScrollTo(unsigned int page, float duration, DirectionBias bias = DirectionBiasNone);

  /**
   * @copydoc Toolkit::ScrollView::ScrollTo(Actor& actor)
   */
  void ScrollTo(Actor &actor);

  /**
   * @copydoc Toolkit::ScrollView::ScrollTo(Actor& actor, float duration)
   */
  void ScrollTo(Actor &actor, float duration);

  /**
   * @copydoc Toolkit::ScrollView::SetScrollingDirection()
   */
  void SetScrollingDirection( Radian direction, Radian threshold );

  /**
   * @copydoc Toolkit::ScrollView::RemoveScrollingDirection()
   */
  void RemoveScrollingDirection( Radian angle );

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
   * @copydoc Toolkit::ScrollView::ScaleTo(const Vector3& scale)
   */
  void ScaleTo(const Vector3& scale);

  /**
   * @copydoc Toolkit::ScrollView::ScaleTo(const Vector3& scale, float duration)
   */
  void ScaleTo(const Vector3& scale, float duration);

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
   * Animates to position/scale/rotation transform.
   *
   * @param[in] position The position to animate to
   * @param[in] positionDuration The number of seconds this animation should run for in each axis.
   * @param[in] scale The scale to animate to
   * @param[in] scaleDuration The number of seconds this animation should run for in each axis.
   * @param[in] rotation The angle to animate to
   * @param[in] rotationDuration The number of seconds this animation should run for in each axis.
   * @param[in] alpha The easing alpha function to use.
   * @param[in] findShortcuts (optional) Whether to find the shortest route (in Wrap mode)
   * @param[in] horizontalBias (optional) Whether to bias animation to left or right (or no biasing)
   * @param[in] verticalBias (optional) Whether to bias animation to top or bottom (or no biasing)
   * @return True if animation necessary and taking place to reach desired transform.
   */
  bool AnimateTo(const Vector3& position, const Vector3& positionDuration,
                             const Vector3& scale, const Vector3& scaleDuration,
                             float rotation, float rotationDuration,
                             AlphaFunction alpha, bool findShortcuts = true,
                             DirectionBias horizontalBias = DirectionBiasNone, DirectionBias verticalBias = DirectionBiasNone,
                             SnapType snapType = Snap);

  /**
   * @copydoc Toolkit::Scrollable::AddOverlay()
   */
  void AddOverlay(Actor actor);

  /**
   * @copydoc Toolkit::Scrollable::RemoveOverlay()
   */
  void RemoveOverlay(Actor actor);

public: //Signals

  /**
   * @copydoc Dali::Toolkit::ScrollView::SnapStartedSignal()
   */
  Toolkit::ScrollView::SnapStartedSignalV2& SnapStartedSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

private: // private overriden functions from CustomActorImpl and Controls

  /**
   * @copydoc Dali::CustomActorImpl::OnSizeAnimation(Animation&, const Vector3&)
   */
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize);

  /**
   * @copydoc Dali::Control::OnControlSizeSet(const Vector3&)
   */
  virtual void OnControlSizeSet( const Vector3& size );

  /**
   * From CustomActorImpl; called after a child has been added to the owning actor.
   * @param[in] child The child which has been added.
   */
  virtual void OnChildAdd(Actor& child);

  /**
   * From CustomActorImpl; called shortly before a child is removed from the owning actor.
   * @param[in] child The child being removed.
   */
  virtual void OnChildRemove(Actor& child);

  /**
   * @copydoc Dali::CustomActorImpl::OnPropertySet( Property::Index index, Property::Value propertyValue )
   */
  virtual void OnPropertySet( Property::Index index, Property::Value propertyValue );

  /**
   * From CustomActorImpl; called after a touch-signal is received by the owning actor.
   *
   * We don't listen to these events as content within the contain may consume events.
   *
   * @param[in] event The touch event.
   * @return True if the event should be consumed.
   */
  virtual bool OnTouchEvent(const TouchEvent& event);

  /**
   * From CustomActorImpl; called after a mouse-wheel-event is received by the owning actor.
   * @param[in] event The mouse wheel event.
   * @return True if the event should be consumed.
   */
  virtual bool OnMouseWheelEvent(const MouseWheelEvent& event);

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::Control::OnControlStageConnection()
   */
  virtual void OnControlStageConnection();

  /**
   * @copydoc Toolkit::Control::OnControlStageConnection()
   */
  virtual void OnControlStageDisconnection();

  /**
   * @copydoc Toolkit::Control::OnAccessibilityPan()
   */
  virtual bool OnAccessibilityPan(PanGesture gesture);

  /**
   * @copydoc Toolkit::Scrollable::SetOvershootEnabled()
   */
  virtual void SetOvershootEnabled(bool enable);

private:

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
   * Resets all scrolling animations and states, leaving current scroll position at mPropertyPosition
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
  void AnimateInternalXTo( float position, float duration, AlphaFunction alpha );

  /**
   * Animates the internal y property to the given value
   *
   * @param[in] position The Y position to animate to
   * @param[in] duration The time in seconds for animation
   * @param[in] alpha The alpha function to use for animating
   */
  void AnimateInternalYTo( float position, float duration, AlphaFunction alpha );

  /**
   * Called whenever a snap animation on the x-axis has completed
   * @param[in] source the Animation instance that has completed.
   */
  void OnScrollAnimationFinished( Animation& source );

  /**
   * This is called internally whenever the Scroll Rulers are
   * modified. This will update the properties: 'scroll-position-min'
   * and 'scroll-position-max' to reflect the changes.
   *
   * @param[in] size size of the visible scroll area (ScrollView control size)
   */
  void UpdatePropertyDomain(const Vector3& size);

  /**
   * Called when the gesture starts.
   */
  void GestureStarted();

  /**
   * Amalgamated Gesture Continuing event
   *
   * @param[in] panDelta average panning delta from base position (0)
   * @param[in] scaleDelta average scale delta from base scale (1)
   * @param[in] rotationDelta average rotation delta from base angle (0)
   */
  void GestureContinuing(const Vector2& panDelta, const Vector2& scaleDelta, float rotationDelta);

  /**
   * Called upon pan gesture event.
   *
   * @param[in] gesture The gesture event.
   */
  void OnPan(PanGesture pan);

  /**
   * Extension of the above gestures.
   *
   * @param[in] gesture The gesture event.
   */
  void OnGestureEx(Gesture::State state);

  /**
   * Performs snapping while taking into account Velocity of gesture
   * (velocity in pixels/sec)
   *
   * @param[in] velocity velocity in pixels/sec
   */
  bool SnapWithVelocity(Vector2 velocity);

  /**
   * Updates Container Transform based on Pan, Scale, and Rotation props.
   * (occurs when continuing gesture i.e. dragging/pinching.)
   */
  void UpdateTransform();

  /**
   * Finishes Container Transform
   * (occurs upon finishing gesture i.e. releasing)
   */
  void FinishTransform();

  /**
   * Returns overshoot vector based on current position
   *
   * Overshoot vector is defined as how far outside of bounds
   * the viewport is trying to view (prior to being clamped).
   *
   * an overshoot of (100,50), means user is in bottom right corner,
   * trying to pan +100 to the right, and +50 below. This can be used
   * to determine an effect, such as stretching.
   *
   * @param[in] position The position for which you wish to obtain overshoot vector
   */
  Vector3 GetOvershoot(Vector3& position) const;

  /**
   * Clamps position within the domain set up by X/Y Rulers
   *
   * @param[in,out] position The position you wish to clamp
   */
  void ClampPosition(Vector3& position) const;

  /**
   * Clamps position within the domain set up by X/Y Rulers
   *
   * @param[in,out] position The position you wish to clamp
   * @param[out] clamped The results of the clamping.
   */
  void ClampPosition(Vector3& position, ClampState3 &clamped) const;

  /**
   * Wraps position within the domain set up by X/Y Rulers
   *
   * @note Only wraps if mWrapMode is enabled, and respective domains
   * are enabled.
   *
   * @param[in,out] position The position you wish to wrap
   */
  void WrapPosition(Vector3& position) const;

  /**
   * Clamps scale within the domain set up by Scale-X/Scale-Y Rulers
   *
   * @param[in,out] scale The scale you wish to clamp
   */
  void ClampScale(Vector3& scale) const;

  /**
   * Clamps scale within the domain set up by Scale-X/Scale-Y Rulers
   *
   * @param[in,out] scale The scale you wish to clamp
   * @param[out] clamped The results of the clamping.
   */
  void ClampScale(Vector3& scale, ClampState3 &clamped) const;

  /**
   * Updates the main internal scroll constraints with new ruler and domain
   * values
   */
  void UpdateMainInternalConstraint();

  /**
   * Enables/disables the overshoot constraints
   *
   * @param[in] enabled whether to enable or disable the overshoot constraints
   */
  void SetOvershootConstraintsEnabled(bool enabled);

  /**
   * Sets internal constraints for this ScrollView.
   * Many of these internal constraints are based on properties within
   * ScrollView.
   */
  void SetInternalConstraints();

protected:

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
  virtual void FindAndUnbindActor(Actor child);

  /**
   * Gets position property.
   *
   * @return The current position
   */
  Vector3 GetPropertyPrePosition() const;

  /**
   * Gets position property.
   *
   * @return The current position
   */
  Vector3 GetPropertyPosition() const;

  /**
   * Gets scale property.
   *
   * @return The current scale
   */
  Vector3 GetPropertyScale() const;

  /**
   * Handles a Stopped animation. Its position/scale/rotation properties need to be
   * saved, and the animation flag switched off.
   */
  void HandleStoppedAnimation();

  /**
   * Handles a Stopped animation (whether the animation completed, or was
   * manually stopped). Its position/scale/rotation properties need to be
   * saved, and the animation flag switched off.
   */
  void HandleSnapAnimationFinished();

  /**
   * Helper to start the refresh timer.
   */
  void StartRefreshTimer();

  /**
   * Helper to cancel the refresh timer.
   */
  void CancelRefreshTimer();

  /**
   * Refresh the ScrollView (used when animating to update application developer of changes)
   * @return True if the refresh timer should be kept running.
   */
  bool OnRefreshTick();

private:

  // Undefined
  ScrollView(const ScrollView&);

  // Undefined
  ScrollView& operator=(const ScrollView& rhs);

private:

  unsigned long mTouchDownTime;         ///< The touch down time

  int mGestureStackDepth;               ///< How many gestures are currently occuring.
  Vector2 mGestureReferencePosition;    ///< Point where scaling should occur from.
  Vector2 mPinchGestureLastPosition;
  Vector2 mPinchGestureLastScale;

  Vector3 mPanDelta;                    ///< Amount currently panned.
  Vector3 mScaleDelta;                  ///< Amount currently scaled.
  float mRotationDelta;                 ///< Amount currently rotated.

  unsigned int mScrollStateFlags;       ///< flags indicating current state of scrolling
  // Scroll delegate pre and post position/scale/rotation properties...
  Vector3 mScrollPrePosition;           ///< Wrapped scroll position, but not clamped
  Vector3 mScrollPostPosition;          ///< Wrapped and clamped, this is the final scroll position used
  Vector3 mScrollTargetPosition;        ///< Final target position for an animated scroll
  Vector3 mScrollPreScale;              ///< Scroll delegate pre-scale
  Vector3 mScrollPostScale;             ///< Scroll delegate post-scale (affected by current touch)
  float mScrollPreRotation;             ///< Scroll delegate pre-rotation
  float mScrollPostRotation;            ///< Scroll delegate post-rotation (affected by current touch)
  Vector3 mDomainOffset;                ///< Domain offset (this keeps track of the domain boundaries that scroll positions traverses)

  // Rulers for each axes...
  RulerPtr mRulerX;
  RulerPtr mRulerY;
  RulerPtr mRulerScaleX;
  RulerPtr mRulerScaleY;
  RulerPtr mRulerRotation;

  unsigned int mMinTouchesForPanning;   ///< Minimum number of touches for panning to be used.
  unsigned int mMaxTouchesForPanning;   ///< Maximum number of touches for panning to be used.

  Animation mSnapAnimation;             ///< Used to animate rotation and scaling of scroll properties
  Animation mInternalXAnimation;        ///< Animates mPropertyX to a snap position or application requested scroll position
  Animation mInternalYAnimation;        ///< Animates mPropertyY to a snap position or application requested scroll position


  Vector2 mLastVelocity;                ///< Record the last velocity from PanGesture (Finish event doesn't have correct velocity)
  LockAxis mLockAxis;

  Timer mTouchDownTimer;                ///< Used to interrupt snap-animation. This cannot be done in OnTouchEvent without breaking fast flick behavior.
  Timer mOvershootRefreshTimer;
  Timer mRefreshTimer;                  ///< Refresh timer is used to provide the Application developer with updates as animations run.
  int mRefreshIntervalMilliseconds;     ///< Refresh timer interval.

  Actor mInternalActor;                 ///< Internal actor (we keep internal actors in here e.g. scrollbars, so we can ignore it in searches)

  ScrollViewEffectContainer mEffects;   ///< Container keeping track of all the applied effects.

  float     mOvershootDelay;                    ///< Time to wait for input before reducing overshoot back to 0
  Vector2   mMaxOvershoot;                      ///< Number of scrollable pixels that will take overshoot from 0.0f to 1.0f
  Vector2   mUserMaxOvershoot;                  ///< Set by user, allows overriding of default max overshoot for the scroll indicator
  float     mSnapOvershootDuration;             ///< Duration for overshoot snapping back to Vector3::ZERO
  AlphaFunction mSnapOvershootAlphaFunction;    ///< AlphaFunction to be used for this overshoot.

  float mSnapDuration;                          ///< Time for the snap animation to take (in seconds).
  AlphaFunction mSnapAlphaFunction;             ///< AlphaFunction to be used for the Snap Animation.

  float mFlickDuration;                         ///< Time for the flick animation to take (in seconds).
  AlphaFunction mFlickAlphaFunction;            ///< AlphaFunction to be used for the Flick Animation.

  float mAxisAutoLockGradient;                  ///< Axis Auto-lock gradient threshold. Above this gradient and it will lock scrolling to closest axis.
  float mFrictionCoefficient;                   ///< Friction coefficient. Amount of friction to apply to free panning flick animation. in stage.lengths/sec
  float mFlickSpeedCoefficient;                 ///< Flick velocity coefficient. Input touch velocity is multiplied by this.
  float mMaxFlickSpeed;                         ///< Maximum flick speed. Maximum speed of flick in stage.lengths/sec.

  Vector2 mMouseWheelScrollDistanceStep;        ///< The step of scroll distance in actor coordinates in X and Y axes for each mouse wheel event received.

  //ScrollInternalConstraintsPtr mScrollInternalConstraints;
  ActiveConstraint mScrollMainInternalPrePositionConstraint;
  ActiveConstraint mScrollMainInternalPositionConstraint;
  ActiveConstraint mScrollMainInternalXConstraint;
  ActiveConstraint mScrollMainInternalYConstraint;
  ActiveConstraint mScrollMainInternalOvershootXConstraint;
  ActiveConstraint mScrollMainInternalOvershootYConstraint;
  ActiveConstraint mScrollMainInternalDeltaConstraint;
  ActiveConstraint mScrollMainInternalFinalConstraint;
  ActiveConstraint mScrollMainInternalRelativeConstraint;

  ScrollOvershootIndicatorPtr mOvershootIndicator;

  Toolkit::ScrollView::SnapStartedSignalV2 mSnapStartedSignalV2;

  bool mInAccessibilityPan : 1;           ///< With AccessibilityPan its easier to move between snap positions
  bool mInitialized:1;
  bool mScrolling:1;                      ///< Flag indicating whether the scroll view is being scrolled (by user or animation)
  bool mScrollInterrupted:1;              ///< Flag set for when a down event interrupts a scroll
  bool mPanning:1;                        ///< Whether scroll view is currently panning or not
  bool mSensitive:1;                      ///< Scroll Sensitivity Flag.
  bool mTouchDownTimeoutReached:1;        ///< Indicates when down event timeout occured without corresponding up event (touch still down)
  bool mActorAutoSnapEnabled:1;           ///< Whether to automatically snap to closest actor.
  bool mAutoResizeContainerEnabled:1;     ///< Whether to automatically resize container (affects RulerDomain's on X/Y axes)
  bool mWrapMode:1;                       ///< Whether to wrap contents based on container size.
  bool mAxisAutoLock:1;                   ///< Whether to automatically lock axis when panning.
  bool mAlterChild:1;                     ///< Internal flag to control behavior of OnChildAdd/OnChildRemove when Adding internal Actors.
  bool mDefaultMaxOvershoot:1;            ///< Whether to use default max overshoot or application defined one
  bool mUserSetPosition:1;                ///< SetScrollPosition has been called, return this position until internals get control of scroll position again
};

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

#endif // __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_H__
