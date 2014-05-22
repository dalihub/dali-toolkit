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

#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-overshoot-indicator-impl.h>

// EXTERNAL INCLUDES
#include <boost/bind.hpp>

#include <dali-toolkit/internal/controls/scrollable/scrollable-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

const float DEFAULT_MAX_OVERSHOOT_HEIGHT = 36.0f;  // 36 pixels
const Rect<int> OVERSHOOT_RIPPLE_IMAGE_1_PIXEL_AREA( 0, 0, 720, 58 );
const float DEFAULT_OVERSHOOT_ANIMATION_DURATION = 0.35f;  // time in seconds

ScrollOvershootIndicator::ScrollOvershootIndicator(Scrollable& scrollable) :
  mScrollable(scrollable),
  mEffectX(NULL),
  mEffectY(NULL)
{
}

ScrollOvershootIndicator::~ScrollOvershootIndicator()
{

}

ScrollOvershootIndicator* ScrollOvershootIndicator::New(Scrollable& scrollable)
{
  ScrollOvershootIndicator* scrollOvershootPtr = new ScrollOvershootIndicator(scrollable);
  return scrollOvershootPtr;
}

void ScrollOvershootIndicator::Enable(bool enable)
{
  if(enable)
  {
    Actor scrollableActor = mScrollable.Self();
    if(!mEffectX)
    {
      mEffectX = ScrollOvershootEffectRipple::New(false);
    }
    mEffectX->Apply(mScrollable);
    if(!mEffectY)
    {
      mEffectY = ScrollOvershootEffectRipple::New(true);
    }
    mEffectY->Apply(mScrollable);
    mEffectY->SetPropertyNotifications(scrollableActor);
  }
  else
  {
    if(mEffectX)
    {
      mEffectX->Remove(mScrollable);
    }
    if(mEffectY)
    {
      mEffectY->Remove(mScrollable);
    }
  }
}

void ScrollOvershootIndicator::Reset()
{
  mEffectX->Reset();
  mEffectY->Reset();
}

ScrollOvershootEffect::ScrollOvershootEffect(bool vertical) :
    mVertical(vertical)
{

}

ScrollOvershootEffectRipple::ScrollOvershootEffectRipple(bool vertical) :
    ScrollOvershootEffect(vertical),
    mMaxOvershootImageSize(DEFAULT_MAX_OVERSHOOT_HEIGHT)
{
  mRippleEffect = BouncingEffect::New(Scrollable::DEFAULT_OVERSHOOT_COLOUR);
  mOvershootImage = CreateSolidColorActor(Vector4::ONE);
  mOvershootImage.SetParentOrigin(ParentOrigin::TOP_LEFT);
  mOvershootImage.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  mOvershootImage.SetDrawMode(DrawMode::OVERLAY);
  mOvershootImage.SetShaderEffect(mRippleEffect);
  mOvershootImage.SetVisible(false);
  mAnimatingOvershootOn = false;
  mAnimateOvershootOff = false;
}

void ScrollOvershootEffectRipple::Apply(Scrollable& scrollable)
{
  Actor scrollableActor = scrollable.Self();

  // make sure height is set, since we only create a constraint for image width
  mOvershootImage.SetSize(OVERSHOOT_RIPPLE_IMAGE_1_PIXEL_AREA.width, OVERSHOOT_RIPPLE_IMAGE_1_PIXEL_AREA.height);

  UpdateConstraints(scrollableActor);
  scrollable.AddOverlay(mOvershootImage);

  SetPropertyNotifications(scrollableActor);
}

void ScrollOvershootEffectRipple::Remove(Scrollable& scrollable)
{
  if(mOvershootImage)
  {
    if(mSizeConstraint)
    {
      mOvershootImage.RemoveConstraint(mSizeConstraint);
      mSizeConstraint = NULL;
    }
    if(mPositionConstraint)
    {
      mOvershootImage.RemoveConstraint(mPositionConstraint);
      mPositionConstraint = NULL;
    }
    scrollable.RemoveOverlay(mOvershootImage);
  }
}

void ScrollOvershootEffectRipple::Reset()
{
  mAnimatingOvershootOn = false;
  mAnimateOvershootOff = false;
  mOvershootImage.SetVisible(false);
  mRippleEffect.SetProgressRate(0.0f);
  if(mScrollOvershootAnimation)
  {
    mScrollOvershootAnimation.Clear();
    mScrollOvershootAnimation.Reset();
  }
}

void ScrollOvershootEffectRipple::UpdateConstraints(Actor& scrollable)
{
  int overshootPropertyIndex = mRippleEffect.GetPropertyIndex(mRippleEffect.GetProgressRatePropertyName());
  Constraint constraint;
  if(!mSizeConstraint)
  {
    constraint = Constraint::New<float>( Actor::SIZE_WIDTH,
                                                      Source( scrollable, IsVertical() ? Actor::SIZE_WIDTH : Actor::SIZE_HEIGHT),
                                                      EqualToConstraint() );
    mSizeConstraint = mOvershootImage.ApplyConstraint(constraint);
  }

  if(!mPositionConstraint)
  {
    constraint = Constraint::New<Vector3>( Actor::POSITION,
                                           Source( scrollable, Actor::SIZE ),
                                           Source( mRippleEffect, overshootPropertyIndex ),
                                           boost::bind( &ScrollOvershootEffectRipple::PositionConstraint, this, _1, _2, _3) );
    mPositionConstraint = mOvershootImage.ApplyConstraint(constraint);
  }
}

void ScrollOvershootEffectRipple::SetPropertyNotifications(Actor& scrollable)
{
  int overshootXPropertyIndex = scrollable.GetPropertyIndex(Toolkit::ScrollView::SCROLL_OVERSHOOT_X_PROPERTY_NAME);
  int overshootYPropertyIndex = scrollable.GetPropertyIndex(Toolkit::ScrollView::SCROLL_OVERSHOOT_Y_PROPERTY_NAME);
  mCanScrollPropertyIndex = scrollable.GetPropertyIndex(IsVertical() ? Scrollable::SCROLLABLE_CAN_SCROLL_VERTICAL : Scrollable::SCROLLABLE_CAN_SCROLL_HORIZONTAL);

  if(!mOvershootNegativeNotification)
  {
    mOvershootNegativeNotification = scrollable.AddPropertyNotification(IsVertical() ? overshootYPropertyIndex : overshootXPropertyIndex, LessThanCondition(-Math::MACHINE_EPSILON_1));
    mOvershootNegativeNotification.SetNotifyMode(PropertyNotification::NotifyOnChanged);
    mOvershootNegativeNotification.NotifySignal().Connect(this, &ScrollOvershootEffectRipple::OnNegativeOvershootNotification);
  }

  if(!mOvershootPositiveNotification)
  {
    mOvershootPositiveNotification = scrollable.AddPropertyNotification(IsVertical() ? overshootYPropertyIndex : overshootXPropertyIndex, GreaterThanCondition(Math::MACHINE_EPSILON_1));
    mOvershootPositiveNotification.SetNotifyMode(PropertyNotification::NotifyOnChanged);
    mOvershootPositiveNotification.NotifySignal().Connect(this, &ScrollOvershootEffectRipple::OnPositiveOvershootNotification);
  }
}

Vector3 ScrollOvershootEffectRipple::PositionConstraint(const Vector3& current,
    const PropertyInput& parentSizeProperty, const PropertyInput& overshootProperty)
{
  float overshoot = overshootProperty.GetFloat();
  const Vector3 parentSize = parentSizeProperty.GetVector3();

  Vector3 relativeOffset = Vector3::ZERO;

  if(IsVertical())
  {
    if(overshoot > Math::MACHINE_EPSILON_0)
    {
      relativeOffset = Vector3(0.0f, 0.0f, 0.0f);
    }
    else if (overshoot < -Math::MACHINE_EPSILON_0)
    {
      relativeOffset = Vector3(1.0f, 1.0f, 0.0f);
    }
  }
  else
  {
    if(overshoot > Math::MACHINE_EPSILON_0)
    {
      relativeOffset = Vector3(0.0f, 1.0f, 0.0f);
    }
    else if (overshoot < -Math::MACHINE_EPSILON_0)
    {
      relativeOffset = Vector3(1.0f, 0.0f, 0.0f);
    }
  }

  return relativeOffset * parentSize;
}

void ScrollOvershootEffectRipple::OnPositiveOvershootNotification(PropertyNotification& source)
{
  Actor delegate = Actor::DownCast(source.GetTarget());
  float overshoot = delegate.GetProperty<float>(source.GetTargetProperty());
  bool canScroll = delegate.GetProperty<bool>(mCanScrollPropertyIndex);
  if(!canScroll)
  {
    mOvershootImage.SetVisible(false);
    return;
  }
  mOvershootImage.SetVisible(true);

  if (fabsf(overshoot) < Math::MACHINE_EPSILON_1)
  {
    AnimateScrollOvershoot(0.0f);
    return;
  }
  if(overshoot > 0.0f)
  {
    const Vector3 imageSize = mOvershootImage.GetCurrentSize();
    Vector3 relativeOffset = Vector3::ZERO;
    const Vector3 parentSize = delegate.GetCurrentSize();
    AnimateScrollOvershoot(1.0f);
    if(IsVertical())
    {
      mOvershootImage.SetRotation(Quaternion(0.0f, Vector3::ZAXIS));
      mOvershootImage.SetSize(parentSize.width, imageSize.height, imageSize.depth);
      relativeOffset = Vector3(0.0f, 0.0f, 0.0f);
    }
    else
    {
      mOvershootImage.SetRotation(Quaternion(1.5f * Math::PI, Vector3::ZAXIS));
      mOvershootImage.SetSize(parentSize.height, imageSize.height, imageSize.depth);
      relativeOffset = Vector3(0.0f, 1.0f, 0.0f);
    }
    mOvershootImage.SetPosition(relativeOffset * parentSize);
  }
}

void ScrollOvershootEffectRipple::OnNegativeOvershootNotification(PropertyNotification& source)
{
  Actor delegate = Actor::DownCast(source.GetTarget());
  float overshoot = delegate.GetProperty<float>(source.GetTargetProperty());
  bool canScroll = delegate.GetProperty<bool>(mCanScrollPropertyIndex);
  if(!canScroll)
  {
    mOvershootImage.SetVisible(false);
    return;
  }
  mOvershootImage.SetVisible(true);

  if (fabsf(overshoot) < Math::MACHINE_EPSILON_1)
  {
    AnimateScrollOvershoot(0.0f);
    return;
  }

  if(overshoot < 0.0f)
  {
    const Vector3 imageSize = mOvershootImage.GetCurrentSize();
    Vector3 relativeOffset = Vector3::ZERO;
    const Vector3 parentSize = delegate.GetCurrentSize();
    AnimateScrollOvershoot(-1.0f);
    if(IsVertical())
    {
      mOvershootImage.SetRotation(Quaternion(Math::PI, Vector3::ZAXIS));
      mOvershootImage.SetSize(parentSize.width, imageSize.height, imageSize.depth);
      relativeOffset = Vector3(1.0f, 1.0f, 0.0f);
    }
    else
    {
      mOvershootImage.SetRotation(Quaternion(0.5f * Math::PI, Vector3::ZAXIS));
      mOvershootImage.SetSize(parentSize.height, imageSize.height, imageSize.depth);
      relativeOffset = Vector3(1.0f, 0.0f, 0.0f);
    }
    mOvershootImage.SetPosition(relativeOffset * parentSize);
  }
}

void ScrollOvershootEffectRipple::AnimateScrollOvershoot(float overshootAmount)
{
  bool animatingOn = fabsf(overshootAmount) > Math::MACHINE_EPSILON_1;

  // make sure we animate back if needed
  mAnimateOvershootOff = (!animatingOn && mAnimatingOvershootOn);

  int overShootProperty = mRippleEffect.GetPropertyIndex(mRippleEffect.GetProgressRatePropertyName());
  float currentOvershoot = mRippleEffect.GetProperty<float>(overShootProperty);
  if(((currentOvershoot < 0.0f && overshootAmount > 0.0f)
      || (currentOvershoot > 0.0f && overshootAmount < 0.0f)))
  {
    // cancel current animation
    mAnimatingOvershootOn = false;
    // reset currentOvershoot to 0.0f
    mRippleEffect.SetProperty(overShootProperty, 0.0f);
    currentOvershoot = 0.0f;
  }
  if( mAnimatingOvershootOn )
  {
    // animating on in same direction, do not allow animate off
    return;
  }
  float duration = DEFAULT_OVERSHOOT_ANIMATION_DURATION * (animatingOn ? (1.0f - fabsf(currentOvershoot)) : fabsf(currentOvershoot));

  if(mScrollOvershootAnimation)
  {
    mScrollOvershootAnimation.Clear();
    mScrollOvershootAnimation.Reset();
  }
  mScrollOvershootAnimation = Animation::New(duration);
  mScrollOvershootAnimation.FinishedSignal().Connect(this, &ScrollOvershootEffectRipple::OnOvershootAnimFinished);
  mScrollOvershootAnimation.AnimateTo( Property(mRippleEffect, overShootProperty), overshootAmount, TimePeriod(0.0f, duration) );
  mScrollOvershootAnimation.Play();

  mOvershootImage.SetVisible(true);

  mAnimatingOvershootOn = animatingOn;
}

void ScrollOvershootEffectRipple::OnOvershootAnimFinished(Animation& animation)
{
  if(!mAnimatingOvershootOn && !mAnimateOvershootOff)
  {
    // just finished animating overshoot to 0
    mOvershootImage.SetVisible(false);
  }
  mAnimatingOvershootOn = false;
  mScrollOvershootAnimation.FinishedSignal().Disconnect(this, &ScrollOvershootEffectRipple::OnOvershootAnimFinished);
  mScrollOvershootAnimation.Clear();
  mScrollOvershootAnimation.Reset();
  if(mAnimateOvershootOff)
  {
    AnimateScrollOvershoot(0.0f);
  }
}

ScrollOvershootEffectRipplePtr ScrollOvershootEffectRipple::New(bool vertical)
{
  return new ScrollOvershootEffectRipple(vertical);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
