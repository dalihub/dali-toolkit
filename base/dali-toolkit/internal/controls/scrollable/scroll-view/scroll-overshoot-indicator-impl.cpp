//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-overshoot-indicator-impl.h>

// EXTERNAL INCLUDES
#include <boost/bind.hpp>

#include <dali-toolkit/internal/controls/scrollable/scrollable-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

const char* OVERSHOOT_OVERLAY_IMAGE_PATH = DALI_IMAGE_DIR "scroll_overshoot.png";
const char* OVERSHOOT_OVERLAY_RIPPLE_IMAGE_PATH = DALI_IMAGE_DIR "overshoot_ripple.png";
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

ScrollOvershootEffectGradient::ScrollOvershootEffectGradient(bool vertical) :
    ScrollOvershootEffect(vertical),
    mMaxOvershootImageSize(DEFAULT_MAX_OVERSHOOT_HEIGHT)
{
  Image overshootImage = Image::New( OVERSHOOT_OVERLAY_IMAGE_PATH );
  mOvershootImage = ImageActor::New( overshootImage );
  mOvershootImage.SetParentOrigin(ParentOrigin::TOP_LEFT);
  mOvershootImage.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  mOvershootImage.SetDrawMode(DrawMode::OVERLAY);
}

void ScrollOvershootEffectGradient::Apply(Scrollable& scrollable)
{
  Actor scrollableActor = scrollable.Self();
  int overshootXPropertyIndex = scrollableActor.GetPropertyIndex(Toolkit::ScrollView::SCROLL_OVERSHOOT_X_PROPERTY_NAME);
  int overshootYPropertyIndex = scrollableActor.GetPropertyIndex(Toolkit::ScrollView::SCROLL_OVERSHOOT_Y_PROPERTY_NAME);

  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE,
                                                    Source( scrollableActor, overshootXPropertyIndex ),
                                                    Source( scrollableActor, overshootYPropertyIndex ),
                                                    Source( scrollableActor, Actor::SIZE ),
                                                    boost::bind( &ScrollOvershootEffectGradient::SizeConstraint, this, _1, _2, _3, _4) );
  mSizeConstraint = mOvershootImage.ApplyConstraint(constraint);

  constraint = Constraint::New<Quaternion>( Actor::ROTATION,
                                            Source( scrollableActor, overshootXPropertyIndex ),
                                            Source( scrollableActor, overshootYPropertyIndex ),
                                            boost::bind( &ScrollOvershootEffectGradient::RotationConstraint, this, _1, _2, _3) );
  mRotationConstraint = mOvershootImage.ApplyConstraint(constraint);

  constraint = Constraint::New<Vector3>( Actor::POSITION,
                                         Source( scrollableActor, Actor::SIZE ),
                                         Source( scrollableActor, overshootXPropertyIndex ),
                                         Source( scrollableActor, overshootYPropertyIndex ),
                                         boost::bind( &ScrollOvershootEffectGradient::PositionConstraint, this, _1, _2, _3, _4) );
  mPositionConstraint = mOvershootImage.ApplyConstraint(constraint);

  constraint = Constraint::New<bool>( Actor::VISIBLE,
                                      Source( scrollableActor, IsVertical() ? scrollableActor.GetPropertyIndex(Scrollable::SCROLLABLE_CAN_SCROLL_VERTICAL) : scrollableActor.GetPropertyIndex(Scrollable::SCROLLABLE_CAN_SCROLL_HORIZONTAL)),
                                      boost::bind( &ScrollOvershootEffectGradient::VisibilityConstraint, this, _1, _2) );
  mVisibilityConstraint = mOvershootImage.ApplyConstraint(constraint);
  scrollable.AddOverlay(mOvershootImage);
  SetPropertyNotifications(scrollableActor);
}

void ScrollOvershootEffectGradient::Remove(Scrollable& scrollable)
{
  if(mOvershootImage)
  {
    if(mSizeConstraint)
    {
      mOvershootImage.RemoveConstraint(mSizeConstraint);
      mSizeConstraint = NULL;
    }
    if(mRotationConstraint)
    {
      mOvershootImage.RemoveConstraint(mRotationConstraint);
      mRotationConstraint = NULL;
    }
    if(mPositionConstraint)
    {
      mOvershootImage.RemoveConstraint(mPositionConstraint);
      mPositionConstraint = NULL;
    }
    if(mVisibilityConstraint)
    {
      mOvershootImage.RemoveConstraint(mVisibilityConstraint);
      mVisibilityConstraint = NULL;
    }
    scrollable.RemoveOverlay(mOvershootImage);
  }
}

Vector3 ScrollOvershootEffectGradient::SizeConstraint(const Vector3& current,
    const PropertyInput& overshootPropertyX, const PropertyInput& overshootPropertyY,
    const PropertyInput& parentSizeProperty)
{
  float overshootx = overshootPropertyX.GetFloat();
  float overshooty = overshootPropertyY.GetFloat();
  const Vector3 parentSize = parentSizeProperty.GetVector3();

  float overlayWidth = IsVertical() ? parentSize.x : parentSize.y;
  float overlayHeight = mMaxOvershootImageSize * fabsf(IsVertical() ? overshooty : overshootx);

  return Vector3(overlayWidth, overlayHeight, current.z);
}

Quaternion ScrollOvershootEffectGradient::RotationConstraint(const Quaternion& current,
    const PropertyInput& overshootPropertyX, const PropertyInput& overshootPropertyY)
{
  float overshootx = overshootPropertyX.GetFloat();
  float overshooty = overshootPropertyY.GetFloat();

  Quaternion rotation;

  if(IsVertical())
  {
    if(overshooty < -Math::MACHINE_EPSILON_0)
    {
      rotation = Quaternion(Math::PI, Vector3::ZAXIS);
    }
    else if(overshooty > Math::MACHINE_EPSILON_0)
    {
      rotation = Quaternion(0.0f, Vector3::ZAXIS);
    }
  }
  else
  {
    if(overshootx < -Math::MACHINE_EPSILON_0)
    {
      rotation = Quaternion(0.5f * Math::PI, Vector3::ZAXIS);
    }
    else if(overshootx > Math::MACHINE_EPSILON_0)
    {
      rotation = Quaternion(1.5f * Math::PI, Vector3::ZAXIS);
    }
  }

  return rotation;
}

Vector3 ScrollOvershootEffectGradient::PositionConstraint(const Vector3&    current,
    const PropertyInput& parentSizeProperty,
    const PropertyInput& overshootPropertyX, const PropertyInput& overshootPropertyY)
{
  float overshootx = overshootPropertyX.GetFloat();
  float overshooty = overshootPropertyY.GetFloat();
  const Vector3 parentSize = parentSizeProperty.GetVector3();

  Vector3 relativeOffset = Vector3::ZERO;

  if(IsVertical())
  {
    if(overshooty > Math::MACHINE_EPSILON_0)
    {
      relativeOffset = Vector3(0.0f, 0.0f, 0.0f);
    }
    else if (overshooty < -Math::MACHINE_EPSILON_0)
    {
      relativeOffset = Vector3(1.0f, 1.0f, 0.0f);
    }
  }
  else
  {
    if(overshootx > Math::MACHINE_EPSILON_0)
    {
      relativeOffset = Vector3(0.0f, 1.0f, 0.0f);
    }
    else if (overshootx < -Math::MACHINE_EPSILON_0)
    {
      relativeOffset = Vector3(1.0f, 0.0f, 0.0f);
    }
  }

  return relativeOffset * parentSize;
}

bool ScrollOvershootEffectGradient::VisibilityConstraint(const bool& current,
    const PropertyInput& canScrollProperty)
{
  return canScrollProperty.GetBoolean();
}

ScrollOvershootEffectGradientPtr ScrollOvershootEffectGradient::New(bool vertical)
{
  return new ScrollOvershootEffectGradient(vertical);
}

namespace
{

const std::string OVERSHOOT_PROPERTY_NAME( "uOvershoot" );
const std::string OVERSHOOT_IMAGE_COUNT_PROPERTY_NAME( "uOvershootImageCount" );

} // namespace

OvershootRippleEffect::OvershootRippleEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
OvershootRippleEffect::OvershootRippleEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

OvershootRippleEffect::~OvershootRippleEffect()
{
}

OvershootRippleEffect OvershootRippleEffect::New()
{
  std::string vertextShader(
      "precision mediump float;                     \n"
      "uniform  float  uOvershoot;                  \n"
      "uniform  float  uOvershootImageCount;        \n"
      "void main()                                  \n"
      "{                                            \n"
      "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0); \n"
      "  vTexCoord = aTexCoord;                     \n"
      "  vTexCoord.y += (1.0 / uOvershootImageCount) * min(floor((abs(uOvershoot) * (uOvershootImageCount - 1.0)) + 0.5), (uOvershootImageCount - 1.0)); \n"
      "}                                            \n" );

  std::string fragmentShader(
      "void main()                                  \n"
      "{                                            \n"
      "  gl_FragColor = texture2D(sTexture, vTexCoord);    \n"
      "}                                            \n" );

  // Create the implementation, temporarily owned on stack,
  Dali::ShaderEffect shaderEffectCustom =  Dali::ShaderEffect::New(
      vertextShader,
      fragmentShader);

  /* Pass ownership to OvershootRippleEffect through overloaded constructor, So that it now has access to the
     Dali::ShaderEffect implementation */
  OvershootRippleEffect handle( shaderEffectCustom );
  handle.SetUniform(OVERSHOOT_PROPERTY_NAME, 0.0f);
  handle.SetUniform(OVERSHOOT_IMAGE_COUNT_PROPERTY_NAME, 10.0f);
  return handle;
}

void OvershootRippleEffect::SetOvershoot( float overshoot )
{
  SetUniform( OVERSHOOT_PROPERTY_NAME, overshoot );
}

void OvershootRippleEffect::SetOvershootImageCount( float imageCount )
{
  SetUniform( OVERSHOOT_IMAGE_COUNT_PROPERTY_NAME, imageCount );
}

const std::string& OvershootRippleEffect::GetOvershootPropertyName() const
{
  return OVERSHOOT_PROPERTY_NAME;
}

const std::string& OvershootRippleEffect::GetOvershootImageCountPropertyName() const
{
  return OVERSHOOT_IMAGE_COUNT_PROPERTY_NAME;
}

ScrollOvershootEffectRipple::ScrollOvershootEffectRipple(bool vertical) :
    ScrollOvershootEffect(vertical),
    mMaxOvershootImageSize(DEFAULT_MAX_OVERSHOOT_HEIGHT)
{
  mRippleEffect = OvershootRippleEffect::New();
  Image overshootImage = Image::New( OVERSHOOT_OVERLAY_RIPPLE_IMAGE_PATH );
  mOvershootImage = ImageActor::New( overshootImage );
  mOvershootImage.SetParentOrigin(ParentOrigin::TOP_LEFT);
  mOvershootImage.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  mOvershootImage.SetDrawMode(DrawMode::OVERLAY);
  mOvershootImage.SetShaderEffect(mRippleEffect);
  mOvershootImage.SetPixelArea(OVERSHOOT_RIPPLE_IMAGE_1_PIXEL_AREA);
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
  mRippleEffect.SetUniform(mRippleEffect.GetOvershootPropertyName(), 0.0f);
  if(mScrollOvershootAnimation)
  {
    mScrollOvershootAnimation.Clear();
    mScrollOvershootAnimation.Reset();
  }
}

void ScrollOvershootEffectRipple::UpdateConstraints(Actor& scrollable)
{
  int overshootPropertyIndex = mRippleEffect.GetPropertyIndex(mRippleEffect.GetOvershootPropertyName());
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

  int overShootProperty = mRippleEffect.GetPropertyIndex(mRippleEffect.GetOvershootPropertyName());
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
