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

// EXTERNAL INCLUDES
#include <boost/bind.hpp>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scrollable/scrollable-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>

using namespace Dali;

namespace
{
const Vector2 OVERSHOOT_BOUNCE_ACTOR_DEFAULT_SIZE( 720.0f, 42.0f );
const float OVERSHOOT_BOUNCE_ACTOR_RESIZE_THRESHOLD = 180.0f;

// local helper function to resize the height of the bounce actor
float GetBounceActorHeight( float width )
{
  return (width > OVERSHOOT_BOUNCE_ACTOR_RESIZE_THRESHOLD) ? OVERSHOOT_BOUNCE_ACTOR_DEFAULT_SIZE.height : OVERSHOOT_BOUNCE_ACTOR_DEFAULT_SIZE.height * 0.5f;
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

void ScrollOvershootIndicator::ClearOvershoot()
{
  if(mEffectX)
  {
    mEffectX->SetOvershoot(0.0f);
  }
  if(mEffectY)
  {
    mEffectY->SetOvershoot(0.0f);
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
    mAnimationStateFlags(0)
{

}

void ScrollOvershootEffectRipple::Apply()
{
  Actor self = mAttachedScrollView.Self();
  mOvershootProperty = self.GetPropertyIndex(IsVertical() ? Toolkit::ScrollView::SCROLL_OVERSHOOT_Y_PROPERTY_NAME : Toolkit::ScrollView::SCROLL_OVERSHOOT_X_PROPERTY_NAME);

  // make sure height is set, since we only create a constraint for image width


  UpdatePropertyNotifications();
}

void ScrollOvershootEffectRipple::Remove( Scrollable& scrollable )
{
}

void ScrollOvershootEffectRipple::Reset()
{
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
}

void ScrollOvershootEffectRipple::UpdateVisibility( bool visible )
{
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
}

void ScrollOvershootEffectRipple::OnOvershootAnimFinished(Animation& animation)
{
  bool animateOff = false;
  if( mAnimationStateFlags & AnimatingOut )
  {
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
