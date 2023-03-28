/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/scrollable.h>

using namespace Dali;

namespace Dali
{
namespace Toolkit
{
///////////////////////////////////////////////////////////////////////////////////////////////////
// RulerDomain
///////////////////////////////////////////////////////////////////////////////////////////////////

RulerDomain::RulerDomain(float min, float max, bool enabled)
: min(min),
  max(max),
  enabled(enabled)
{
}

float RulerDomain::Clamp(float x, float length, float scale) const
{
  ClampState clamped;

  return Clamp(x, length, scale, clamped);
}

float RulerDomain::Clamp(float x, float length, float scale, ClampState& clamped) const
{
  if(!enabled)
  {
    clamped = NOT_CLAMPED;
    return x;
  }

  const float minExtent = min * scale;
  const float maxExtent = max * scale - length;
  if(x < minExtent)
  {
    clamped = CLAMPED_TO_MIN;
    return minExtent;
  }
  else if(x > maxExtent)
  {
    clamped = CLAMPED_TO_MAX;
    return maxExtent;
  }

  clamped = NOT_CLAMPED;
  return x;
}

float RulerDomain::GetSize() const
{
  return max - min;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Ruler
///////////////////////////////////////////////////////////////////////////////////////////////////

Ruler::Ruler()
: mType(FREE),
  mEnabled(true),
  mDomain(RulerDomain(0.0f, 1.0f, false))
{
}

Ruler::~Ruler()
{
}

Ruler::RulerType Ruler::GetType() const
{
  return mType;
}

bool Ruler::IsEnabled() const
{
  return mEnabled;
}

void Ruler::Enable()
{
  mEnabled = true;
}

void Ruler::Disable()
{
  mEnabled = false;
}

void Ruler::SetDomain(RulerDomain domain)
{
  mDomain = domain;
}

const RulerDomain& Ruler::GetDomain() const
{
  return mDomain;
}

void Ruler::DisableDomain()
{
  mDomain = RulerDomain(0.0f, 1.0f, false);
}

float Ruler::Clamp(float x, float length, float scale) const
{
  return mDomain.Clamp(x, length, scale);
}

float Ruler::Clamp(float x, float length, float scale, ClampState& clamped) const
{
  return mDomain.Clamp(x, length, scale, clamped);
}

float Ruler::SnapAndClamp(float x, float bias, float length, float scale) const
{
  return Clamp(Snap(x, bias), length, scale);
}

float Ruler::SnapAndClamp(float x, float bias, float length, float scale, ClampState& clamped) const
{
  return Clamp(Snap(x, bias), length, scale, clamped);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// DefaultRuler
///////////////////////////////////////////////////////////////////////////////////////////////////

DefaultRuler::DefaultRuler()
{
  mType = FREE;
}

float DefaultRuler::Snap(float x, float bias) const
{
  return x;
}

float DefaultRuler::GetPositionFromPage(unsigned int page, unsigned int& volume, bool wrap) const
{
  volume = 0;
  return 0.0f;
}

unsigned int DefaultRuler::GetPageFromPosition(float position, bool wrap) const
{
  return 0;
}

unsigned int DefaultRuler::GetTotalPages() const
{
  return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// FixedRuler
///////////////////////////////////////////////////////////////////////////////////////////////////

FixedRuler::FixedRuler(float spacing)
: mSpacing(spacing)
{
  if(fabsf(mSpacing) <= Math::MACHINE_EPSILON_1)
  {
    DALI_LOG_ERROR("Page spacing too small (%f).\n", double(spacing));
    mSpacing = spacing >= 0.0f ? Math::MACHINE_EPSILON_1 : -Math::MACHINE_EPSILON_1;
  }
  mType = FIXED;
}

float FixedRuler::Snap(float x, float bias) const
{
  return floor(x / mSpacing + bias) * mSpacing;
}

float FixedRuler::GetPositionFromPage(unsigned int page, unsigned int& volume, bool wrap) const
{
  float position = mDomain.min;

  volume = 0;

  // spacing must be present.
  if(mEnabled)
  {
    unsigned int column = page;

    // In carry mode, a volume (carry) is produced when page exceeds limit within domain
    if(wrap)
    {
      unsigned int pagesPerVolume = mDomain.GetSize() / mSpacing;
      if(pagesPerVolume > 0)
      {
        column += pagesPerVolume;
        column %= pagesPerVolume;
        volume = page / pagesPerVolume;
      }
    }

    position = mDomain.min + column * mSpacing;
  }
  else // Domain (or Spacing) is not present, carry page to volume.
  {
    if(wrap)
    {
      volume = page;
    }
  }

  return position;
}

unsigned int FixedRuler::GetPageFromPosition(float position, bool wrap) const
{
  unsigned int page = 0;

  // spacing must be present.
  if(mEnabled)
  {
    if(wrap)
    {
      position = WrapInDomain(position, mDomain.min, mDomain.max);
    }
    page = std::max(static_cast<double>(0.0f), static_cast<double>(floor((position - mDomain.min) / mSpacing + 0.5f)));

    if(wrap)
    {
      unsigned int pagesPerVolume = mDomain.GetSize() / mSpacing;
      // Defensive check to avoid a divide by zero below when the ruler is in an invalid state (entire domain smaller than spacing between pages of it):
      if(pagesPerVolume < 1u)
      {
        pagesPerVolume = 1u;
        DALI_LOG_ERROR("Ruler domain(%f) is smaller than its spacing(%f).\n", mDomain.GetSize() * 1.0, mSpacing * 1.0);
      }
      page %= pagesPerVolume;
    }
  }

  return page;
}

unsigned int FixedRuler::GetTotalPages() const
{
  unsigned int pagesPerVolume = 1;

  // spacing must be present.
  if(mEnabled)
  {
    pagesPerVolume = mDomain.GetSize() / mSpacing;
  }

  return pagesPerVolume;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ScrollView
///////////////////////////////////////////////////////////////////////////////////////////////////

ScrollView::ScrollView()
{
}

ScrollView::ScrollView(Internal::ScrollView& implementation)
: Scrollable(implementation)
{
}

ScrollView::ScrollView(Dali::Internal::CustomActor* internal)
: Scrollable(internal)
{
  VerifyCustomActorPointer<Internal::ScrollView>(internal);
}

ScrollView::ScrollView(const ScrollView& handle) = default;

ScrollView::ScrollView(ScrollView&& rhs) noexcept = default;

ScrollView& ScrollView::operator=(const ScrollView& handle) = default;

ScrollView& ScrollView::operator=(ScrollView&& rhs) noexcept = default;

ScrollView ScrollView::New()
{
  return Internal::ScrollView::New();
}

ScrollView::~ScrollView()
{
}

ScrollView ScrollView::DownCast(BaseHandle handle)
{
  return Control::DownCast<ScrollView, Internal::ScrollView>(handle);
}

AlphaFunction ScrollView::GetScrollSnapAlphaFunction() const
{
  return GetImpl(*this).GetScrollSnapAlphaFunction();
}

void ScrollView::SetScrollSnapAlphaFunction(AlphaFunction alpha)
{
  GetImpl(*this).SetScrollSnapAlphaFunction(alpha);
}

AlphaFunction ScrollView::GetScrollFlickAlphaFunction() const
{
  return GetImpl(*this).GetScrollFlickAlphaFunction();
}

void ScrollView::SetScrollFlickAlphaFunction(AlphaFunction alpha)
{
  GetImpl(*this).SetScrollFlickAlphaFunction(alpha);
}

float ScrollView::GetScrollSnapDuration() const
{
  return GetImpl(*this).GetScrollSnapDuration();
}

void ScrollView::SetScrollSnapDuration(float time)
{
  GetImpl(*this).SetScrollSnapDuration(time);
}

float ScrollView::GetScrollFlickDuration() const
{
  return GetImpl(*this).GetScrollFlickDuration();
}

void ScrollView::SetScrollFlickDuration(float time)
{
  GetImpl(*this).SetScrollFlickDuration(time);
}

void ScrollView::SetRulerX(RulerPtr ruler)
{
  GetImpl(*this).SetRulerX(ruler);
}

void ScrollView::SetRulerY(RulerPtr ruler)
{
  GetImpl(*this).SetRulerY(ruler);
}

void ScrollView::SetScrollSensitive(bool sensitive)
{
  GetImpl(*this).SetScrollSensitive(sensitive);
}

void ScrollView::SetMaxOvershoot(float overshootX, float overshootY)
{
  GetImpl(*this).SetMaxOvershoot(overshootX, overshootY);
}

void ScrollView::SetSnapOvershootAlphaFunction(AlphaFunction alpha)
{
  GetImpl(*this).SetSnapOvershootAlphaFunction(alpha);
}

void ScrollView::SetSnapOvershootDuration(float duration)
{
  GetImpl(*this).SetSnapOvershootDuration(duration);
}

void ScrollView::SetActorAutoSnap(bool enable)
{
  GetImpl(*this).SetActorAutoSnap(enable);
}

void ScrollView::SetWrapMode(bool enable)
{
  GetImpl(*this).SetWrapMode(enable);
}

int ScrollView::GetScrollUpdateDistance() const
{
  return GetImpl(*this).GetScrollUpdateDistance();
}

void ScrollView::SetScrollUpdateDistance(int distance)
{
  GetImpl(*this).SetScrollUpdateDistance(distance);
}

bool ScrollView::GetAxisAutoLock() const
{
  return GetImpl(*this).GetAxisAutoLock();
}

void ScrollView::SetAxisAutoLock(bool enable)
{
  GetImpl(*this).SetAxisAutoLock(enable);
}

float ScrollView::GetAxisAutoLockGradient() const
{
  return GetImpl(*this).GetAxisAutoLockGradient();
}

void ScrollView::SetAxisAutoLockGradient(float gradient)
{
  GetImpl(*this).SetAxisAutoLockGradient(gradient);
}

float ScrollView::GetFrictionCoefficient() const
{
  return GetImpl(*this).GetFrictionCoefficient();
}

void ScrollView::SetFrictionCoefficient(float friction)
{
  GetImpl(*this).SetFrictionCoefficient(friction);
}

float ScrollView::GetFlickSpeedCoefficient() const
{
  return GetImpl(*this).GetFlickSpeedCoefficient();
}

void ScrollView::SetFlickSpeedCoefficient(float speed)
{
  GetImpl(*this).SetFlickSpeedCoefficient(speed);
}

Vector2 ScrollView::GetMinimumDistanceForFlick() const
{
  return GetImpl(*this).GetMinimumDistanceForFlick();
}

void ScrollView::SetMinimumDistanceForFlick(const Vector2& distance)
{
  GetImpl(*this).SetMinimumDistanceForFlick(distance);
}

float ScrollView::GetMinimumSpeedForFlick() const
{
  return GetImpl(*this).GetMinimumSpeedForFlick();
}

void ScrollView::SetMinimumSpeedForFlick(float speed)
{
  GetImpl(*this).SetMinimumSpeedForFlick(speed);
}

float ScrollView::GetMaxFlickSpeed() const
{
  return GetImpl(*this).GetMaxFlickSpeed();
}

void ScrollView::SetMaxFlickSpeed(float speed)
{
  GetImpl(*this).SetMaxFlickSpeed(speed);
}

Vector2 ScrollView::GetWheelScrollDistanceStep() const
{
  return GetImpl(*this).GetWheelScrollDistanceStep();
}

void ScrollView::SetWheelScrollDistanceStep(Vector2 step)
{
  GetImpl(*this).SetWheelScrollDistanceStep(step);
}

Vector2 ScrollView::GetCurrentScrollPosition() const
{
  return GetImpl(*this).GetCurrentScrollPosition();
}

unsigned int ScrollView::GetCurrentPage() const
{
  return GetImpl(*this).GetCurrentPage();
}

void ScrollView::ScrollTo(const Vector2& position)
{
  GetImpl(*this).ScrollTo(position);
}

void ScrollView::ScrollTo(const Vector2& position, float duration)
{
  GetImpl(*this).ScrollTo(position, duration);
}

void ScrollView::ScrollTo(const Vector2& position, float duration, AlphaFunction alpha)
{
  GetImpl(*this).ScrollTo(position, duration, alpha);
}

void ScrollView::ScrollTo(const Vector2& position, float duration, DirectionBias horizontalBias, DirectionBias verticalBias)
{
  GetImpl(*this).ScrollTo(position, duration, horizontalBias, verticalBias);
}

void ScrollView::ScrollTo(const Vector2& position, float duration, AlphaFunction alpha, DirectionBias horizontalBias, DirectionBias verticalBias)
{
  GetImpl(*this).ScrollTo(position, duration, alpha, horizontalBias, verticalBias);
}

void ScrollView::ScrollTo(unsigned int page)
{
  GetImpl(*this).ScrollTo(page);
}

void ScrollView::ScrollTo(unsigned int page, float duration)
{
  GetImpl(*this).ScrollTo(page, duration);
}

void ScrollView::ScrollTo(unsigned int page, float duration, DirectionBias bias)
{
  GetImpl(*this).ScrollTo(page, duration, bias);
}

void ScrollView::ScrollTo(Actor& actor)
{
  GetImpl(*this).ScrollTo(actor);
}

void ScrollView::ScrollTo(Actor& actor, float duration)
{
  GetImpl(*this).ScrollTo(actor, duration);
}

bool ScrollView::ScrollToSnapPoint()
{
  return GetImpl(*this).ScrollToSnapPoint();
}

void ScrollView::ApplyConstraintToChildren(Constraint constraint)
{
  GetImpl(*this).ApplyConstraintToChildren(constraint);
}

void ScrollView::RemoveConstraintsFromChildren()
{
  GetImpl(*this).RemoveConstraintsFromChildren();
}

void ScrollView::ApplyEffect(ScrollViewEffect effect)
{
  GetImpl(*this).ApplyEffect(effect);
}

void ScrollView::RemoveEffect(ScrollViewEffect effect)
{
  GetImpl(*this).RemoveEffect(effect);
}

void ScrollView::RemoveAllEffects()
{
  GetImpl(*this).RemoveAllEffects();
}

void ScrollView::BindActor(Actor child)
{
  GetImpl(*this).BindActor(child);
}

void ScrollView::UnbindActor(Actor child)
{
  GetImpl(*this).UnbindActor(child);
}

ScrollView::SnapStartedSignalType& ScrollView::SnapStartedSignal()
{
  return GetImpl(*this).SnapStartedSignal();
}

void ScrollView::SetScrollingDirection(Radian direction, Radian threshold)
{
  GetImpl(*this).SetScrollingDirection(direction, threshold);
}

void ScrollView::RemoveScrollingDirection(Radian direction)
{
  GetImpl(*this).RemoveScrollingDirection(direction);
}

} // namespace Toolkit

} // namespace Dali
