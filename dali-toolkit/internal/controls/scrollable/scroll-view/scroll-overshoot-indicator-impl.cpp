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
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-overshoot-indicator-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scrollable/scrollable-impl.h>
#include <dali-toolkit/internal/controls/scrollable/bouncing-effect-actor.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>

using namespace Dali;

namespace
{

const float OVERSHOOT_BOUNCE_ACTOR_RESIZE_THRESHOLD = 180.0f;

// local helper function to resize the height of the bounce actor
float GetBounceActorHeight( float width, float defaultHeight )
{
  return (width > OVERSHOOT_BOUNCE_ACTOR_RESIZE_THRESHOLD) ? defaultHeight : defaultHeight * 0.5f;
}

const float MAX_OVERSHOOT_NOTIFY_AMOUNT = 0.99f;                     // maximum amount to set notification for increased overshoot, beyond this we just wait for it to reduce again
const float MIN_OVERSHOOT_NOTIFY_AMOUNT = Math::MACHINE_EPSILON_0;  // minimum amount to set notification for reduced overshoot, beyond this we just wait for it to increase again
const float OVERSHOOT_NOTIFY_STEP = 0.01f;                          // amount to set notifications beyond current overshoot value

}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

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

void ScrollOvershootIndicator::SetOvershootEffectColor( const Vector4& color )
{
  if(mEffectX)
  {
    mEffectX->SetOvershootEffectColor(color);
  }
  if(mEffectY)
  {
    mEffectY->SetOvershootEffectColor(color);
  }
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
    mOvershootProperty(Property::INVALID_INDEX),
    mEffectOvershootProperty(Property::INVALID_INDEX),
    mOvershoot(0.0f),
    mOvershootSize( scrollable.GetOvershootSize() ),
    mAnimationStateFlags(0)
{
  mOvershootOverlay = CreateBouncingEffectActor(mEffectOvershootProperty);
  mOvershootOverlay.SetColor(mAttachedScrollView.GetOvershootEffectColor());
  mOvershootOverlay.SetParentOrigin(ParentOrigin::TOP_LEFT);
  mOvershootOverlay.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  mOvershootOverlay.SetVisible(false);

}

void ScrollOvershootEffectRipple::Apply()
{
  Actor self = mAttachedScrollView.Self();
  mOvershootProperty = IsVertical() ? Toolkit::ScrollView::Property::OVERSHOOT_Y : Toolkit::ScrollView::Property::OVERSHOOT_X;

  // make sure height is set, since we only create a constraint for image width
  mOvershootOverlay.SetSize( mOvershootSize );

  mAttachedScrollView.AddOverlay(mOvershootOverlay);

  UpdatePropertyNotifications();
}

void ScrollOvershootEffectRipple::Remove( Scrollable& scrollable )
{
  if(mOvershootOverlay)
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
    scrollable.RemoveOverlay(mOvershootOverlay);
  }
}

void ScrollOvershootEffectRipple::Reset()
{
  mOvershootOverlay.SetVisible(false);
  mOvershootOverlay.SetProperty( mEffectOvershootProperty, 0.f);
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

void ScrollOvershootEffectRipple::SetOvershootEffectColor( const Vector4& color )
{
  if(mOvershootOverlay)
  {
    mOvershootOverlay.SetColor(color);
  }
}

void ScrollOvershootEffectRipple::UpdateVisibility( bool visible )
{
  mOvershootOverlay.SetVisible(visible);
  // make sure overshoot image is correctly placed
  if( visible )
  {
    Actor self = mAttachedScrollView.Self();
    if(mOvershoot > 0.0f)
    {
      // positive overshoot
      const Vector3 size = mOvershootOverlay.GetCurrentSize();
      Vector3 relativeOffset;
      const Vector3 parentSize = self.GetCurrentSize();
      if(IsVertical())
      {
        mOvershootOverlay.SetOrientation( Quaternion( Radian( 0.0f ), Vector3::ZAXIS ) );
        mOvershootOverlay.SetSize(parentSize.width, GetBounceActorHeight(parentSize.width, mOvershootSize.height), size.depth);
      }
      else
      {
        mOvershootOverlay.SetOrientation( Quaternion( Radian( 1.5f * Math::PI ), Vector3::ZAXIS ) );
        mOvershootOverlay.SetSize(parentSize.height, GetBounceActorHeight(parentSize.height, mOvershootSize.height), size.depth);
        relativeOffset = Vector3(0.0f, 1.0f, 0.0f);
      }
      mOvershootOverlay.SetPosition(relativeOffset * parentSize);
    }
    else
    {
      // negative overshoot
      const Vector3 size = mOvershootOverlay.GetCurrentSize();
      Vector3 relativeOffset;
      const Vector3 parentSize = self.GetCurrentSize();
      if(IsVertical())
      {
        mOvershootOverlay.SetOrientation( Quaternion( Radian( Math::PI ), Vector3::ZAXIS ) );
        mOvershootOverlay.SetSize(parentSize.width, GetBounceActorHeight(parentSize.width, mOvershootSize.height), size.depth);
        relativeOffset = Vector3(1.0f, 1.0f, 0.0f);
      }
      else
      {
        mOvershootOverlay.SetOrientation( Quaternion( Radian( 0.5f * Math::PI ), Vector3::ZAXIS ) );
        mOvershootOverlay.SetSize(parentSize.height, GetBounceActorHeight(parentSize.height, mOvershootSize.height), size.depth);
        relativeOffset = Vector3(1.0f, 0.0f, 0.0f);
      }
      mOvershootOverlay.SetPosition(relativeOffset * parentSize);
    }
  }
}

void ScrollOvershootEffectRipple::OnOvershootNotification(PropertyNotification& source)
{
  Actor self = mAttachedScrollView.Self();
  mOvershoot = self.GetProperty<float>(mOvershootProperty);
  SetOvershoot(mOvershoot, false);
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

  if( absAmount > Math::MACHINE_EPSILON_1 )
  {
    UpdateVisibility(true);
  }

  float overshootAnimationSpeed = mAttachedScrollView.Self().GetProperty<float>(Toolkit::Scrollable::Property::OVERSHOOT_ANIMATION_SPEED);

  if( animate && overshootAnimationSpeed > Math::MACHINE_EPSILON_0 )
  {
    float currentOvershoot = fabsf( mOvershootOverlay.GetProperty( mEffectOvershootProperty ).Get<float>() );
    float duration = mOvershootOverlay.GetCurrentSize().height * (animatingOn ? (1.0f - currentOvershoot) : currentOvershoot) / overshootAnimationSpeed;

    if( duration > Math::MACHINE_EPSILON_0 )
    {
      if(mScrollOvershootAnimation)
      {
        mScrollOvershootAnimation.FinishedSignal().Disconnect( this, &ScrollOvershootEffectRipple::OnOvershootAnimFinished );
        mScrollOvershootAnimation.Stop();
        mScrollOvershootAnimation.Reset();
      }
      mScrollOvershootAnimation = Animation::New(duration);
      mScrollOvershootAnimation.FinishedSignal().Connect( this, &ScrollOvershootEffectRipple::OnOvershootAnimFinished );
      mScrollOvershootAnimation.AnimateTo( Property(mOvershootOverlay, mEffectOvershootProperty), amount, TimePeriod(duration) );
      mScrollOvershootAnimation.Play();
      mAnimationStateFlags = animatingOn ? AnimatingIn : AnimatingOut;
    }
  }
  else
  {
    mOvershootOverlay.SetProperty( mEffectOvershootProperty, amount);
  }
}

void ScrollOvershootEffectRipple::OnOvershootAnimFinished(Animation& animation)
{
  bool animateOff = false;
  if( mAnimationStateFlags & AnimatingOut )
  {
    // should now be offscreen
    mOvershootOverlay.SetVisible(false);
  }
  if( (mAnimationStateFlags & AnimateBack) )
  {
    animateOff = true;
  }
  mScrollOvershootAnimation.FinishedSignal().Disconnect( this, &ScrollOvershootEffectRipple::OnOvershootAnimFinished );
  mScrollOvershootAnimation.Stop();
  mScrollOvershootAnimation.Reset();
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
