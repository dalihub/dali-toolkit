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
const float MAX_OVERSHOOT_NOTIFY_AMOUNT = 0.9f;                     // maximum amount to set notification for increased overshoot, beyond this we just wait for it to reduce again
const float MIN_OVERSHOOT_NOTIFY_AMOUNT = Math::MACHINE_EPSILON_1;  // minimum amount to set notification for reduced overshoot, beyond this we just wait for it to increase again
const float OVERSHOOT_NOTIFY_STEP = 0.1f;                           // amount to set notifications beyond current overshoot value

ScrollOvershootIndicator::ScrollOvershootIndicator() :
  mEffectX(NULL),
  mEffectY(NULL)
{
}

ScrollOvershootIndicator::~ScrollOvershootIndicator()
{

}

ScrollOvershootIndicator* ScrollOvershootIndicator::New()
{
  ScrollOvershootIndicator* scrollOvershootPtr = new ScrollOvershootIndicator();
  return scrollOvershootPtr;
}

void ScrollOvershootIndicator::AttachToScrollable(Scrollable& scrollable)
{
  if(!mEffectX)
  {
    mEffectX = ScrollOvershootEffectRipple::New(false, scrollable);
  }
  mEffectX->Apply();
  if(!mEffectY)
  {
    mEffectY = ScrollOvershootEffectRipple::New(true, scrollable);
  }
  mEffectY->Apply();
}

void ScrollOvershootIndicator::DetachFromScrollable(Scrollable& scrollable)
{
  if(mEffectX)
  {
    mEffectX->Remove(scrollable);
  }
  if(mEffectY)
  {
    mEffectY->Remove(scrollable);
  }
}

void ScrollOvershootIndicator::Reset()
{
  mEffectX->Reset();
  mEffectY->Reset();
}

ScrollOvershootEffect::ScrollOvershootEffect( bool vertical ) :
    mVertical(vertical)
{

}

bool ScrollOvershootEffect::IsVertical() const
{
  return mVertical;
}

ScrollOvershootEffectRipple::ScrollOvershootEffectRipple( bool vertical, Scrollable& scrollable ) :
    ScrollOvershootEffect( vertical ),
    mAttachedScrollView(scrollable),
    mCanScrollPropertyIndex(Property::INVALID_INDEX),
    mOvershootProperty(Property::INVALID_INDEX),
    mEffectOvershootProperty(Property::INVALID_INDEX),
    mMaxOvershootImageSize(DEFAULT_MAX_OVERSHOOT_HEIGHT),
    mOvershootAnimationDuration(DEFAULT_OVERSHOOT_ANIMATION_DURATION),
    mOvershoot(0.0f),
    mAnimationStateFlags(0)
{
  mRippleEffect = BouncingEffect::New(Scrollable::DEFAULT_OVERSHOOT_COLOUR);
  mOvershootImage = CreateSolidColorActor(Vector4::ONE);
  mOvershootImage.SetParentOrigin(ParentOrigin::TOP_LEFT);
  mOvershootImage.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  mOvershootImage.SetDrawMode(DrawMode::OVERLAY);
  mOvershootImage.SetShaderEffect(mRippleEffect);
  mOvershootImage.SetVisible(false);
}

void ScrollOvershootEffectRipple::Apply()
{
  Actor self = mAttachedScrollView.Self();
  mOvershootProperty = self.GetPropertyIndex(IsVertical() ? Toolkit::ScrollView::SCROLL_OVERSHOOT_Y_PROPERTY_NAME : Toolkit::ScrollView::SCROLL_OVERSHOOT_X_PROPERTY_NAME);
  mCanScrollPropertyIndex = self.GetPropertyIndex(IsVertical() ? Scrollable::SCROLLABLE_CAN_SCROLL_VERTICAL : Scrollable::SCROLLABLE_CAN_SCROLL_HORIZONTAL);
  mEffectOvershootProperty = mRippleEffect.GetPropertyIndex(mRippleEffect.GetProgressRatePropertyName());

  // make sure height is set, since we only create a constraint for image width
  mOvershootImage.SetSize(OVERSHOOT_RIPPLE_IMAGE_1_PIXEL_AREA.width, OVERSHOOT_RIPPLE_IMAGE_1_PIXEL_AREA.height);

  mAttachedScrollView.AddOverlay(mOvershootImage);

  UpdatePropertyNotifications();
}

void ScrollOvershootEffectRipple::Remove( Scrollable& scrollable )
{
  if(mOvershootImage)
  {
    if(mOvershootIncreaseNotification)
    {
      scrollable.Self().RemovePropertyNotification(mOvershootIncreaseNotification);
      mOvershootIncreaseNotification.Reset();
    }
    if(mOvershootDecreaseNotification)
    {
      scrollable.Self().RemovePropertyNotification(mOvershootDecreaseNotification);
      mOvershootDecreaseNotification.Reset();
    }
    scrollable.RemoveOverlay(mOvershootImage);
  }
}

void ScrollOvershootEffectRipple::Reset()
{
  mOvershootImage.SetVisible(false);
  mRippleEffect.SetUniform(mRippleEffect.GetProgressRatePropertyName(), 0.0f);
}

void ScrollOvershootEffectRipple::UpdatePropertyNotifications()
{
  float absOvershoot = fabsf(mOvershoot);

  Actor self = mAttachedScrollView.Self();
  // update overshoot increase notify
  if( mOvershootIncreaseNotification )
  {
    self.RemovePropertyNotification( mOvershootIncreaseNotification );
    mOvershootIncreaseNotification.Reset();
  }
  if( absOvershoot < MAX_OVERSHOOT_NOTIFY_AMOUNT )
  {
    float increaseStep = absOvershoot + OVERSHOOT_NOTIFY_STEP;
    if( increaseStep > MAX_OVERSHOOT_NOTIFY_AMOUNT )
    {
      increaseStep = MAX_OVERSHOOT_NOTIFY_AMOUNT;
    }
    mOvershootIncreaseNotification = self.AddPropertyNotification( mOvershootProperty, OutsideCondition(-increaseStep, increaseStep) );
    mOvershootIncreaseNotification.SetNotifyMode(PropertyNotification::NotifyOnTrue);
    mOvershootIncreaseNotification.NotifySignal().Connect(this, &ScrollOvershootEffectRipple::OnOvershootNotification);
  }

  // update overshoot decrease notify
  if( mOvershootDecreaseNotification )
  {
    self.RemovePropertyNotification( mOvershootDecreaseNotification );
    mOvershootDecreaseNotification.Reset();
  }
  if( absOvershoot > MIN_OVERSHOOT_NOTIFY_AMOUNT )
  {
    float reduceStep = absOvershoot - OVERSHOOT_NOTIFY_STEP;
    if( reduceStep < MIN_OVERSHOOT_NOTIFY_AMOUNT )
    {
      reduceStep = MIN_OVERSHOOT_NOTIFY_AMOUNT;
    }
    mOvershootDecreaseNotification = self.AddPropertyNotification( mOvershootProperty, InsideCondition(-reduceStep, reduceStep) );
    mOvershootDecreaseNotification.SetNotifyMode(PropertyNotification::NotifyOnTrue);
    mOvershootDecreaseNotification.NotifySignal().Connect(this, &ScrollOvershootEffectRipple::OnOvershootNotification);
  }
}

void ScrollOvershootEffectRipple::UpdateVisibility( bool visible )
{
  mOvershootImage.SetVisible(visible);
  // make sure overshoot image is correctly placed
  if( visible )
  {
    Actor self = mAttachedScrollView.Self();
    if(mOvershoot > 0.0f)
    {
      // positive overshoot
      const Vector3 imageSize = mOvershootImage.GetCurrentSize();
      Vector3 relativeOffset;
      const Vector3 parentSize = self.GetCurrentSize();
      if(IsVertical())
      {
        mOvershootImage.SetRotation(Quaternion(0.0f, Vector3::ZAXIS));
        mOvershootImage.SetSize(parentSize.width, imageSize.height, imageSize.depth);
      }
      else
      {
        mOvershootImage.SetRotation(Quaternion(1.5f * Math::PI, Vector3::ZAXIS));
        mOvershootImage.SetSize(parentSize.height, imageSize.height, imageSize.depth);
        relativeOffset = Vector3(0.0f, 1.0f, 0.0f);
      }
      mOvershootImage.SetPosition(relativeOffset * parentSize);
    }
    else
    {
      // negative overshoot
      const Vector3 imageSize = mOvershootImage.GetCurrentSize();
      Vector3 relativeOffset;
      const Vector3 parentSize = self.GetCurrentSize();
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
}

void ScrollOvershootEffectRipple::OnOvershootNotification(PropertyNotification& source)
{
  Actor self = mAttachedScrollView.Self();
  mOvershoot = self.GetProperty<float>(mOvershootProperty);
  if( source == mOvershootIncreaseNotification )
  {
    if( mOvershoot > Math::MACHINE_EPSILON_0 )
    {
      SetOvershoot(1.0f);
    }
    else if ( mOvershoot < -Math::MACHINE_EPSILON_0 )
    {
      SetOvershoot(-1.0f);
    }
  }
  else if( source == mOvershootDecreaseNotification )
  {
    SetOvershoot(0.0f);
    // overshoot reducing
  }
  UpdatePropertyNotifications();
}

void ScrollOvershootEffectRipple::SetOvershoot(float amount, bool animate)
{
  float absAmount = fabsf(amount);
  bool animatingOn = absAmount > Math::MACHINE_EPSILON_0;
  if( (animatingOn && (mAnimationStateFlags & AnimatingIn)) )
  {
    // trying to do what we are already doing
    if( mAnimationStateFlags & AnimateBack )
    {
      mAnimationStateFlags &= ~AnimateBack;
    }
    return;
  }
  if( (!animatingOn && (mAnimationStateFlags & AnimatingOut)) )
  {
    // trying to do what we are already doing
    return;
  }
  if( !animatingOn && (mAnimationStateFlags & AnimatingIn) )
  {
    // dont interrupt while animating on
    mAnimationStateFlags |= AnimateBack;
    return;
  }
  // When we need to animate overshoot to 0
  if( mOvershootAnimationDuration > Math::MACHINE_EPSILON_1 )
  {
    // setup the new overshoot to 0 animation
    float currentOvershoot = fabsf( mRippleEffect.GetProperty<float>( mEffectOvershootProperty ) );
    float duration = mOvershootAnimationDuration * (animatingOn ? (1.0f - currentOvershoot) : currentOvershoot);

    if( duration > Math::MACHINE_EPSILON_0 )
    {
      if(mScrollOvershootAnimation)
      {
        mScrollOvershootAnimation.FinishedSignal().Disconnect( this, &ScrollOvershootEffectRipple::OnOvershootAnimFinished );
        mScrollOvershootAnimation.Stop();
        mScrollOvershootAnimation.Clear();
        mScrollOvershootAnimation = NULL;
      }
      mScrollOvershootAnimation = Animation::New(duration);
      mScrollOvershootAnimation.FinishedSignal().Connect( this, &ScrollOvershootEffectRipple::OnOvershootAnimFinished );
      mScrollOvershootAnimation.AnimateTo( Property(mRippleEffect, mEffectOvershootProperty), amount, TimePeriod(0.0f, duration) );
      mScrollOvershootAnimation.Play();
      mAnimationStateFlags = animatingOn ? AnimatingIn : AnimatingOut;
    }
  }
  else
  {
    mRippleEffect.SetProgressRate(amount);
  }
  if( absAmount > Math::MACHINE_EPSILON_1 )
  {
    UpdateVisibility(true);
  }
}

void ScrollOvershootEffectRipple::OnOvershootAnimFinished(Animation& animation)
{
  bool animateOff = false;
  if( mAnimationStateFlags & AnimatingOut )
  {
    // should now be offscreen
    mOvershootImage.SetVisible(false);
  }
  if( (mAnimationStateFlags & AnimateBack) )
  {
    animateOff = true;
  }
  mScrollOvershootAnimation.FinishedSignal().Disconnect( this, &ScrollOvershootEffectRipple::OnOvershootAnimFinished );
  mScrollOvershootAnimation.Stop();
  mScrollOvershootAnimation.Clear();
  mScrollOvershootAnimation = NULL;
  mAnimationStateFlags = 0;
  if( animateOff )
  {
    SetOvershoot(0.0f, true);
  }
}

ScrollOvershootEffectRipplePtr ScrollOvershootEffectRipple::New( bool vertical, Scrollable& scrollable )
{
  return new ScrollOvershootEffectRipple(vertical, scrollable);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
