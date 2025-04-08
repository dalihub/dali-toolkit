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

// CLASS HEADER
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-impl-constraints.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/constraint-integ.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-constraints.h>
#include <dali-toolkit/public-api/toolkit-constraint-tag-ranges.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
static constexpr uint32_t SCROLL_VIEW_CONSTRAINT_TAG(Dali::Toolkit::ConstraintTagRanges::TOOLKIT_CONSTRAINT_TAG_START + 40);

float FinalDefaultAlphaFunction(float offset)
{
  return offset * 0.5f;
}

/**
 * Internal Relative position Constraint
 * Generates the relative position value of the scroll view
 * based on the absolute position, and it's relation to the
 * scroll domain. This is a value from 0.0f to 1.0f in each
 * scroll position axis.
 */
void InternalRelativePositionConstraint(Vector2& relativePosition, const PropertyInputContainer& inputs)
{
  Vector2        position = -inputs[0]->GetVector2();
  const Vector2& min      = inputs[1]->GetVector2();
  const Vector2& max      = inputs[2]->GetVector2();
  const Vector3& size     = inputs[3]->GetVector3();

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
void InternalScrollDomainConstraint(Vector2& scrollDomain, const PropertyInputContainer& inputs)
{
  const Vector2& min  = inputs[0]->GetVector2();
  const Vector2& max  = inputs[1]->GetVector2();
  const Vector3& size = inputs[2]->GetVector3();

  scrollDomain = (max - min) - size.GetVectorXY();
}

/**
 * Internal maximum scroll position Constraint
 * Generates the maximum scroll position of the scroll view.
 */
void InternalPrePositionMaxConstraint(Vector2& scrollMax, const PropertyInputContainer& inputs)
{
  const Vector2& max  = inputs[0]->GetVector2();
  const Vector3& size = inputs[1]->GetVector3();

  scrollMax = max - size.GetVectorXY();
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
  InternalPrePositionConstraint(const Vector2&       initialPanPosition,
                                const Vector2&       initialPanMask,
                                bool                 axisAutoLock,
                                float                axisAutoLockGradient,
                                ScrollView::LockAxis initialLockAxis,
                                const Vector2&       maxOvershoot,
                                const RulerPtr&      rulerX,
                                const RulerPtr&      rulerY)
  : mLocalStart(initialPanPosition),
    mInitialPanMask(initialPanMask),
    mMaxOvershoot(maxOvershoot),
    mAxisAutoLockGradient(axisAutoLockGradient),
    mLockAxis(initialLockAxis),
    mAxisAutoLock(axisAutoLock),
    mWasPanning(false)
  {
    const RulerDomain& rulerDomainX = rulerX->GetDomain();
    const RulerDomain& rulerDomainY = rulerY->GetDomain();
    mDomainMin                      = Vector2(rulerDomainX.min, -rulerDomainY.min);
    mDomainMax                      = Vector2(-rulerDomainX.max, -rulerDomainY.max);
    mClampX                         = rulerDomainX.enabled;
    mClampY                         = rulerDomainY.enabled;
    mFixedRulerX                    = rulerX->GetType() == Ruler::FIXED;
    mFixedRulerY                    = rulerY->GetType() == Ruler::FIXED;
  }

  void operator()(Vector2& scrollPostPosition, const PropertyInputContainer& inputs)
  {
    const Vector2& panPosition = inputs[0]->GetVector2();
    const bool&    inGesture   = inputs[1]->GetBoolean();

    // First check if we are within a gesture.
    // The ScrollView may have received a start gesture from ::OnPan()
    // while the finish gesture is received now in this constraint.
    // This gesture must then be rejected as the value will be "old".
    // Typically the last value from the end of the last gesture.
    // If we are rejecting the gesture, we simply don't modify the constraint target.
    if(inGesture)
    {
      if(!mWasPanning)
      {
        mPrePosition    = scrollPostPosition;
        mStartPosition  = mPrePosition;
        mCurrentPanMask = mInitialPanMask;
        mWasPanning     = true;
      }

      // Calculate Deltas...
      const Vector2& currentPosition = panPosition;
      Vector2        panDelta(currentPosition - mLocalStart);

      // Axis Auto Lock - locks the panning to the horizontal or vertical axis if the pan
      // appears mostly horizontal or mostly vertical respectively...
      if(mAxisAutoLock)
      {
        mLockAxis = GetLockAxis(panDelta, mLockAxis, mAxisAutoLockGradient);
        if(mLockAxis == ScrollView::LockVertical)
        {
          mCurrentPanMask.y = 0.0f;
        }
        else if(mLockAxis == ScrollView::LockHorizontal)
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
      if(mClampX)
      {
        float newXPosition = Clamp(scrollPostPosition.x, (mDomainMax.x + size.x) - mMaxOvershoot.x, mDomainMin.x + mMaxOvershoot.x);
        if((newXPosition < scrollPostPosition.x - Math::MACHINE_EPSILON_1) || (newXPosition > scrollPostPosition.x + Math::MACHINE_EPSILON_1))
        {
          mPrePosition.x = newXPosition;
          mLocalStart.x  = panPosition.x;
        }
        scrollPostPosition.x = newXPosition;
      }
      if(mClampY)
      {
        float newYPosition = Clamp(scrollPostPosition.y, (mDomainMax.y + size.y) - mMaxOvershoot.y, mDomainMin.y + mMaxOvershoot.y);
        if((newYPosition < scrollPostPosition.y - Math::MACHINE_EPSILON_1) || (newYPosition > scrollPostPosition.y + Math::MACHINE_EPSILON_1))
        {
          mPrePosition.y = newYPosition;
          mLocalStart.y  = panPosition.y;
        }
        scrollPostPosition.y = newYPosition;
      }

      // If we are using a fixed ruler in a particular axis, limit the maximum pages scrolled on that axis.
      if(mFixedRulerX || mFixedRulerY)
      {
        // Here we limit the maximum amount that can be moved from the starting position of the gesture to one page.
        // We do this only if we have a fixed ruler (on that axis) and the mode is enabled.
        // Note: 1.0f is subtracted to keep the value within one page size (otherwise we stray on to the page after).
        // Note: A further 1.0f is subtracted to handle a compensation that happens later within the flick handling code in SnapWithVelocity().
        //       When a flick is completed, an adjustment of 1.0f is sometimes made to allow for the scenario where:
        //       A flick finishes before the update thread has advanced the scroll position past the previous snap point.
        Vector2 viewPageSizeLimit(size.x - (1.0f + 1.0f), size.y - (1.0f - 1.0f));
        Vector2 minPosition(mStartPosition.x - viewPageSizeLimit.x, mStartPosition.y - viewPageSizeLimit.y);
        Vector2 maxPosition(mStartPosition.x + viewPageSizeLimit.x, mStartPosition.y + viewPageSizeLimit.y);

        if(mFixedRulerX)
        {
          scrollPostPosition.x = Clamp(scrollPostPosition.x, minPosition.x, maxPosition.x);
        }
        if(mFixedRulerY)
        {
          scrollPostPosition.y = Clamp(scrollPostPosition.y, minPosition.y, maxPosition.y);
        }
      }
    }
  }

  Vector2 mPrePosition;
  Vector2 mLocalStart;
  Vector2 mStartPosition;  ///< The start position of the gesture - used to limit scroll amount (not modified by clamping).
  Vector2 mInitialPanMask; ///< Initial pan mask (based on ruler settings).
  Vector2 mCurrentPanMask; ///< Current pan mask that can be altered by axis lock mode.
  Vector2 mDomainMin;
  Vector2 mDomainMax;
  Vector2 mMaxOvershoot;

  float                mAxisAutoLockGradient; ///< Set by ScrollView
  ScrollView::LockAxis mLockAxis;

  bool mAxisAutoLock : 1; ///< Set by ScrollView
  bool mWasPanning : 1;
  bool mClampX : 1;
  bool mClampY : 1;
  bool mFixedRulerX : 1;
  bool mFixedRulerY : 1;
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
  : mDomainMin(-domainX.min, -domainY.min),
    mDomainMax(-domainX.max, -domainY.max),
    mClampX(domainX.enabled),
    mClampY(domainY.enabled),
    mWrap(wrap)
  {
  }

  void operator()(Vector2& position, const PropertyInputContainer& inputs)
  {
    position            = inputs[0]->GetVector2();
    const Vector2& size = inputs[3]->GetVector3().GetVectorXY();
    const Vector2& min  = inputs[1]->GetVector2();
    const Vector2& max  = inputs[2]->GetVector2();

    if(mWrap)
    {
      position.x = -WrapInDomain(-position.x, min.x, max.x);
      position.y = -WrapInDomain(-position.y, min.y, max.y);
    }
    else
    {
      // clamp post position to domain
      position.x = mClampX ? Clamp(position.x, mDomainMax.x + size.x, mDomainMin.x) : position.x;
      position.y = mClampY ? Clamp(position.y, mDomainMax.y + size.y, mDomainMin.y) : position.y;
    }
  }

  Vector2 mDomainMin;
  Vector2 mDomainMax;
  bool    mClampX;
  bool    mClampY;
  bool    mWrap;
};

/**
 * This constraint updates the X overshoot property using the difference
 * SCROLL_PRE_POSITION.x and SCROLL_POSITION.x, returning a relative value between 0.0f and 1.0f
 */
struct OvershootXConstraint
{
  OvershootXConstraint(float maxOvershoot)
  : mMaxOvershoot(maxOvershoot)
  {
  }

  void operator()(float& current, const PropertyInputContainer& inputs)
  {
    if(inputs[2]->GetBoolean())
    {
      const Vector2& scrollPrePosition  = inputs[0]->GetVector2();
      const Vector2& scrollPostPosition = inputs[1]->GetVector2();
      float          newOvershoot       = scrollPrePosition.x - scrollPostPosition.x;
      current                           = (newOvershoot > 0.0f ? std::min(newOvershoot, mMaxOvershoot) : std::max(newOvershoot, -mMaxOvershoot)) / mMaxOvershoot;
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
  OvershootYConstraint(float maxOvershoot)
  : mMaxOvershoot(maxOvershoot)
  {
  }

  void operator()(float& current, const PropertyInputContainer& inputs)
  {
    if(inputs[2]->GetBoolean())
    {
      const Vector2& scrollPrePosition  = inputs[0]->GetVector2();
      const Vector2& scrollPostPosition = inputs[1]->GetVector2();
      float          newOvershoot       = scrollPrePosition.y - scrollPostPosition.y;
      current                           = (newOvershoot > 0.0f ? std::min(newOvershoot, mMaxOvershoot) : std::max(newOvershoot, -mMaxOvershoot)) / mMaxOvershoot;
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
void InternalPositionDeltaConstraint(Vector2& current, const PropertyInputContainer& inputs)
{
  const Vector2& scrollPosition = inputs[0]->GetVector2();
  const Vector2& scrollOffset   = inputs[1]->GetVector2();

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

  void operator()(Vector2& current, const PropertyInputContainer& inputs)
  {
    const float& overshootx = inputs[1]->GetFloat();
    const float& overshooty = inputs[2]->GetFloat();
    Vector2      offset(mFunctionX(overshootx),
                   mFunctionY(overshooty));

    current = inputs[0]->GetVector2() - offset;
  }

  AlphaFunctionPrototype mFunctionX;
  AlphaFunctionPrototype mFunctionY;
};

} // namespace

void ScrollViewConstraints::UpdateMainInternalConstraint(ScrollView& scrollView)
{
  // TODO: Only update the constraints which have changed, rather than remove all and add all again.
  // Requires a dali-core ApplyConstraintAt, or a ReplaceConstraint. The former is probably more flexible.
  Actor              scrollViewActor = scrollView.Self();
  PanGestureDetector detector(scrollView.GetPanGestureDetector());

  if(mScrollMainInternalPositionConstraint)
  {
    mScrollMainInternalPositionConstraint.Remove();
    mScrollMainInternalDeltaConstraint.Remove();
    mScrollMainInternalFinalConstraint.Remove();
    mScrollMainInternalRelativeConstraint.Remove();
    mScrollMainInternalDomainConstraint.Remove();
    mScrollMainInternalPrePositionMaxConstraint.Remove();
  }
  if(mScrollMainInternalPrePositionConstraint)
  {
    mScrollMainInternalPrePositionConstraint.Remove();
  }

  // TODO: It's probably better to use a local displacement value as this will give a displacement when scrolling just commences
  // but we need to make sure than the gesture system gives displacement since last frame (60Hz), not displacement since last touch event (90Hz).

  // 1. First calculate the pre-position (this is the scroll position if no clamping has taken place)
  Vector2 initialPanMask = Vector2(scrollView.mRulerX->IsEnabled() ? 1.0f : 0.0f, scrollView.mRulerY->IsEnabled() ? 1.0f : 0.0f);

  if(scrollView.mLockAxis == ScrollView::LockVertical)
  {
    initialPanMask.y = 0.0f;
  }
  else if(scrollView.mLockAxis == ScrollView::LockHorizontal)
  {
    initialPanMask.x = 0.0f;
  }

  if(scrollView.mPanning)
  {
    mScrollMainInternalPrePositionConstraint = Constraint::New<Vector2>(scrollViewActor,
                                                                        Toolkit::ScrollView::Property::SCROLL_PRE_POSITION,
                                                                        InternalPrePositionConstraint(scrollView.mPanStartPosition,
                                                                                                      initialPanMask,
                                                                                                      scrollView.mAxisAutoLock,
                                                                                                      scrollView.mAxisAutoLockGradient,
                                                                                                      scrollView.mLockAxis,
                                                                                                      scrollView.mMaxOvershoot,
                                                                                                      scrollView.mRulerX,
                                                                                                      scrollView.mRulerY));
    mScrollMainInternalPrePositionConstraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_POSITION));
    mScrollMainInternalPrePositionConstraint.AddSource(Source(detector, PanGestureDetector::Property::PANNING));
    mScrollMainInternalPrePositionConstraint.AddSource(Source(scrollViewActor, Actor::Property::SIZE));
    Dali::Integration::ConstraintSetInternalTag(mScrollMainInternalPrePositionConstraint, SCROLL_VIEW_CONSTRAINT_TAG);
    mScrollMainInternalPrePositionConstraint.Apply();
  }

  // 2. Second calculate the clamped position (actual position)
  mScrollMainInternalPositionConstraint = Constraint::New<Vector2>(scrollViewActor,
                                                                   Toolkit::ScrollView::Property::SCROLL_POSITION,
                                                                   InternalPositionConstraint(scrollView.mRulerX->GetDomain(),
                                                                                              scrollView.mRulerY->GetDomain(),
                                                                                              scrollView.mWrapMode));
  mScrollMainInternalPositionConstraint.AddSource(LocalSource(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION));
  mScrollMainInternalPositionConstraint.AddSource(LocalSource(Toolkit::Scrollable::Property::SCROLL_POSITION_MIN));
  mScrollMainInternalPositionConstraint.AddSource(LocalSource(Toolkit::Scrollable::Property::SCROLL_POSITION_MAX));
  mScrollMainInternalPositionConstraint.AddSource(Source(scrollViewActor, Actor::Property::SIZE));
  Dali::Integration::ConstraintSetInternalTag(mScrollMainInternalPositionConstraint, SCROLL_VIEW_CONSTRAINT_TAG);
  mScrollMainInternalPositionConstraint.Apply();

  mScrollMainInternalDeltaConstraint = Constraint::New<Vector2>(scrollViewActor, Toolkit::ScrollView::Property::SCROLL_POSITION_DELTA, InternalPositionDeltaConstraint);
  mScrollMainInternalDeltaConstraint.AddSource(LocalSource(Toolkit::ScrollView::Property::SCROLL_POSITION));
  mScrollMainInternalDeltaConstraint.AddSource(LocalSource(Toolkit::ScrollView::Property::SCROLL_DOMAIN_OFFSET));
  Dali::Integration::ConstraintSetInternalTag(mScrollMainInternalDeltaConstraint, SCROLL_VIEW_CONSTRAINT_TAG);
  mScrollMainInternalDeltaConstraint.Apply();

  mScrollMainInternalFinalConstraint = Constraint::New<Vector2>(scrollViewActor, Toolkit::ScrollView::Property::SCROLL_FINAL, InternalFinalConstraint(FinalDefaultAlphaFunction, FinalDefaultAlphaFunction));
  mScrollMainInternalFinalConstraint.AddSource(LocalSource(Toolkit::ScrollView::Property::SCROLL_POSITION));
  mScrollMainInternalFinalConstraint.AddSource(LocalSource(Toolkit::ScrollView::Property::OVERSHOOT_X));
  mScrollMainInternalFinalConstraint.AddSource(LocalSource(Toolkit::ScrollView::Property::OVERSHOOT_Y));
  Dali::Integration::ConstraintSetInternalTag(mScrollMainInternalFinalConstraint, SCROLL_VIEW_CONSTRAINT_TAG);
  mScrollMainInternalFinalConstraint.Apply();

  mScrollMainInternalRelativeConstraint = Constraint::New<Vector2>(scrollViewActor, Toolkit::Scrollable::Property::SCROLL_RELATIVE_POSITION, InternalRelativePositionConstraint);
  mScrollMainInternalRelativeConstraint.AddSource(LocalSource(Toolkit::ScrollView::Property::SCROLL_POSITION));
  mScrollMainInternalRelativeConstraint.AddSource(LocalSource(Toolkit::Scrollable::Property::SCROLL_POSITION_MIN));
  mScrollMainInternalRelativeConstraint.AddSource(LocalSource(Toolkit::Scrollable::Property::SCROLL_POSITION_MAX));
  mScrollMainInternalRelativeConstraint.AddSource(LocalSource(Actor::Property::SIZE));
  Dali::Integration::ConstraintSetInternalTag(mScrollMainInternalRelativeConstraint, SCROLL_VIEW_CONSTRAINT_TAG);
  mScrollMainInternalRelativeConstraint.Apply();

  mScrollMainInternalDomainConstraint = Constraint::New<Vector2>(scrollViewActor, Toolkit::ScrollView::Property::SCROLL_DOMAIN_SIZE, InternalScrollDomainConstraint);
  mScrollMainInternalDomainConstraint.AddSource(LocalSource(Toolkit::Scrollable::Property::SCROLL_POSITION_MIN));
  mScrollMainInternalDomainConstraint.AddSource(LocalSource(Toolkit::Scrollable::Property::SCROLL_POSITION_MAX));
  mScrollMainInternalDomainConstraint.AddSource(LocalSource(Actor::Property::SIZE));
  Dali::Integration::ConstraintSetInternalTag(mScrollMainInternalDomainConstraint, SCROLL_VIEW_CONSTRAINT_TAG);
  mScrollMainInternalDomainConstraint.Apply();

  mScrollMainInternalPrePositionMaxConstraint = Constraint::New<Vector2>(scrollViewActor, Toolkit::ScrollView::Property::SCROLL_PRE_POSITION_MAX, InternalPrePositionMaxConstraint);
  mScrollMainInternalPrePositionMaxConstraint.AddSource(LocalSource(Toolkit::Scrollable::Property::SCROLL_POSITION_MAX));
  mScrollMainInternalPrePositionMaxConstraint.AddSource(LocalSource(Actor::Property::SIZE));
  Dali::Integration::ConstraintSetInternalTag(mScrollMainInternalPrePositionMaxConstraint, SCROLL_VIEW_CONSTRAINT_TAG);
  mScrollMainInternalPrePositionMaxConstraint.Apply();

  // When panning we want to make sure overshoot values are affected by pre position and post position
  SetOvershootConstraintsEnabled(scrollView, !scrollView.mWrapMode);
}

void ScrollViewConstraints::SetOvershootConstraintsEnabled(ScrollView& scrollView, bool enabled)
{
  Actor scrollViewActor(scrollView.Self());
  // remove and reset, it may now be in wrong order with the main internal constraints
  if(mScrollMainInternalOvershootXConstraint)
  {
    mScrollMainInternalOvershootXConstraint.Remove();
    mScrollMainInternalOvershootXConstraint.Reset();
    mScrollMainInternalOvershootYConstraint.Remove();
    mScrollMainInternalOvershootYConstraint.Reset();
  }
  if(enabled)
  {
    mScrollMainInternalOvershootXConstraint = Constraint::New<float>(scrollViewActor, Toolkit::ScrollView::Property::OVERSHOOT_X, OvershootXConstraint(scrollView.mMaxOvershoot.x));
    mScrollMainInternalOvershootXConstraint.AddSource(LocalSource(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION));
    mScrollMainInternalOvershootXConstraint.AddSource(LocalSource(Toolkit::ScrollView::Property::SCROLL_POSITION));
    mScrollMainInternalOvershootXConstraint.AddSource(LocalSource(Toolkit::Scrollable::Property::CAN_SCROLL_HORIZONTAL));
    Dali::Integration::ConstraintSetInternalTag(mScrollMainInternalOvershootXConstraint, SCROLL_VIEW_CONSTRAINT_TAG);
    mScrollMainInternalOvershootXConstraint.Apply();

    mScrollMainInternalOvershootYConstraint = Constraint::New<float>(scrollViewActor, Toolkit::ScrollView::Property::OVERSHOOT_Y, OvershootYConstraint(scrollView.mMaxOvershoot.y));
    mScrollMainInternalOvershootYConstraint.AddSource(LocalSource(Toolkit::ScrollView::Property::SCROLL_PRE_POSITION));
    mScrollMainInternalOvershootYConstraint.AddSource(LocalSource(Toolkit::ScrollView::Property::SCROLL_POSITION));
    mScrollMainInternalOvershootYConstraint.AddSource(LocalSource(Toolkit::Scrollable::Property::CAN_SCROLL_VERTICAL));
    Dali::Integration::ConstraintSetInternalTag(mScrollMainInternalOvershootYConstraint, SCROLL_VIEW_CONSTRAINT_TAG);
    mScrollMainInternalOvershootYConstraint.Apply();
  }
  else
  {
    scrollViewActor.SetProperty(Toolkit::ScrollView::Property::OVERSHOOT_X, 0.0f);
    scrollViewActor.SetProperty(Toolkit::ScrollView::Property::OVERSHOOT_Y, 0.0f);
  }
}

void ScrollViewConstraints::SetInternalConstraints(ScrollView& scrollView)
{
  // Internal constraints (applied to target ScrollBase Actor itself) /////////
  UpdateMainInternalConstraint(scrollView);

  // User definable constraints to apply to all child actors //////////////////
  Actor scrollViewActor = scrollView.Self();

  // Apply some default constraints to ScrollView & its bound actors
  // Movement + Wrap function

  Constraint constraint;

  // MoveActor (scrolling)
  constraint = Constraint::New<Vector3>(scrollViewActor, Actor::Property::POSITION, MoveActorConstraint);
  constraint.AddSource(Source(scrollViewActor, Toolkit::ScrollView::Property::SCROLL_POSITION));
  constraint.SetRemoveAction(Constraint::DISCARD);
  Dali::Integration::ConstraintSetInternalTag(constraint, SCROLL_VIEW_CONSTRAINT_TAG);
  scrollView.ApplyConstraintToBoundActors(constraint);

  // WrapActor (wrap functionality)
  constraint = Constraint::New<Vector3>(scrollViewActor, Actor::Property::POSITION, WrapActorConstraint);
  constraint.AddSource(LocalSource(Actor::Property::SCALE));
  constraint.AddSource(LocalSource(Actor::Property::ANCHOR_POINT));
  constraint.AddSource(LocalSource(Actor::Property::SIZE));
  constraint.AddSource(Source(scrollViewActor, Toolkit::Scrollable::Property::SCROLL_POSITION_MIN));
  constraint.AddSource(Source(scrollViewActor, Toolkit::Scrollable::Property::SCROLL_POSITION_MAX));
  constraint.AddSource(Source(scrollViewActor, Toolkit::ScrollView::Property::WRAP));
  constraint.SetRemoveAction(Constraint::DISCARD);
  Dali::Integration::ConstraintSetInternalTag(constraint, SCROLL_VIEW_CONSTRAINT_TAG);
  scrollView.ApplyConstraintToBoundActors(constraint);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
