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
#include <dali-toolkit/internal/controls/scroll-component/scroll-bar-internal-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/animation/active-constraint.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/images/resource-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/enums.h>

using namespace Dali;

namespace
{
/**
 * Squares input value
 * i.e. y = x*x
 * @param[in] x Input value to be squared
 * @return Result (x*x)
 */
template<typename T>
inline T Square(T x)
{
  return x*x;
}

const char* BAR_TAB_IMAGE_PATH = DALI_IMAGE_DIR "popup_scroll.png";
const Vector4 BAR_TAB_NINE_PATCH_BORDER(0.0f, 12.0f, 14.0f, 14.0f);
const Vector3 BAR_TAB_SIZE(18.0f, 72.0f, 0.0f);
const Vector3 BAR_TAB_OFFSET_V(-18.0f, 0.0f, 0.1f);
const Vector3 BAR_TAB_OFFSET_H(0.0f, -18.0f, 0.1f);
const float BAR_CONTRACT_DELAY(0.8f);
const float BAR_SHOW_TIME(0.4f);
const float BAR_HIDE_TIME(0.5f);
const int SECOND_UNIT(1000);

/**
 * ScrollBarInternal Visibility Constraint
 * Returns whether scroll bar is visible
 */
bool ScrollBarInternalVisibilityConstraint(const bool& current,
    const PropertyInput& canScrollProperty)
{
  bool canScroll = canScrollProperty.GetBoolean();
  return canScroll;
}

/**
 * ScrollBarInternal Size Constraint
 * Resize ScrollBarInternal Size depends on both ScrollSize and DomainSize
 */
struct ScrollBarInternalSizeConstraint
{
  /**
   * @param[in] vertical Whether this constraint controls a vertical scrollbar (true)
   * or a horizontal one (false)
   */
  ScrollBarInternalSizeConstraint(bool vertical)
  : mVertical(vertical)
  {
  }

  /**
   * Constraint operator
   * @param[in] current The current ScrollBarInternal size
   * @param[in] scrollMinProperty The container's minimum position.
   * @param[in] scrollMaxProperty The container's maximum position.
   * @param[in] scrollDirectionProperty The container's scroll direction.
   * @param[in] scrollSizeProperty The container's size of viewport.
   * @return The new ScrollBarInternal position is returned.
   */
  Vector3 operator()(const Vector3& current,
                     const PropertyInput& scrollMinProperty,
                     const PropertyInput& scrollMaxProperty,
                     const PropertyInput& scrollDirectionProperty,
                     const PropertyInput& scrollSizeProperty)
  {
    const Vector3& min = scrollMinProperty.GetVector3();
    const Vector3& max = scrollMaxProperty.GetVector3();
    const Vector3& scrollDirection = scrollDirectionProperty.GetVector3();
    const Toolkit::ControlOrientation::Type& orientation = static_cast<Toolkit::ControlOrientation::Type>(scrollDirection.z);
    const Vector3& size = scrollSizeProperty.GetVector3();
    const Vector3 domainSize = max - min;

    if (mVertical && Toolkit::IsVertical(orientation))
    {
      float mod = fabsf(domainSize.height) > size.height ? size.height * ( size.height / fabsf(domainSize.height) ) : size.height * ( (size.height - fabsf(domainSize.height * 0.5f)) / size.height);
      return Vector3( current.width, mod, current.depth );
    }
    else
    {
      float mod = fabsf(domainSize.height) > size.width ? size.width * ( size.width / fabsf(domainSize.height) ) : size.width * ( (size.width - fabsf(domainSize.height * 0.5f)) / size.width);
      return Vector3( current.width, mod, current.depth );
    }
  }

  bool mVertical;  ///< Whether vertical or horizontal
};

/**
 * ScrollBarInternal rotation Constraint
 * Rotate ScrollBarInternal depends on the scroll direction
 */
struct ScrollBarInternalRotationConstraint
{
  /**
   * @param[in] vertical Whether this constraint controls a vertical scrollbar (true)
   * or a horizontal one (false)
   */
  ScrollBarInternalRotationConstraint(bool vertical)
  : mVertical(vertical)
  {
  }

  /**
   * Constraint operator
   * @param[in] current The current ScrollBarInternal rotation
   * @param[in] scrollDirectionProperty The container's scroll direction.
   * @return The new ScrollBarInternal rotation is returned.
   */
  Quaternion operator()(const Quaternion& current,
                        const PropertyInput& scrollDirectionProperty)
  {
    const Vector3& scrollDirection = scrollDirectionProperty.GetVector3();
    const Toolkit::ControlOrientation::Type& orientation = static_cast<Toolkit::ControlOrientation::Type>(scrollDirection.z);

    if( (mVertical && Toolkit::IsVertical(orientation)) || (!mVertical && Toolkit::IsHorizontal(orientation)) )
    {
      return Quaternion(0.0f, Vector3::ZAXIS);
    }
    else
    {
      return Quaternion(0.5f * Math::PI, Vector3::ZAXIS);
    }
  }

  bool mVertical;  ///< Whether vertical or horizontal
};

/**
 * ScrollBarInternal Position Constraint
 * Positions the scroll bar to reflect the current scroll position
 * within the domain.
 */
struct ScrollBarInternalPositionConstraint
{
  /**
   * @param[in] vertical Whether this constraint controls a vertical scrollbar (true)
   * or a horizontal one (false)
   * @param[in] wrap Whether to base scrollbar on original position or wrapped position
   */
  ScrollBarInternalPositionConstraint(bool vertical, bool wrap = false)
  : mVertical(vertical),
    mWrap(wrap)
  {
  }

  /**
   * Constraint operator
   * @param[in] current The current ScrollBarInternal position
   * @param[in] scrollBarSizeProperty ScrollBarInternal size
   * @param[in] scrollRelativePositionProperty The container's relative position (from 0.0 -> 1.0 in each axis)
   * @param[in] scrollMinProperty The container's minimum position.
   * @param[in] scrollMaxProperty The container's maximum position.
   * @param[in] scrollDirectionProperty The container's scroll direction.
   * @param[in] scrollSizeProperty The container's size of viewport.
   * @return The new ScrollBarInternal position is returned.
   */
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& scrollBarSizeProperty,
                     const PropertyInput& scrollRelativePositionProperty,
                     const PropertyInput& scrollMinProperty,
                     const PropertyInput& scrollMaxProperty,
                     const PropertyInput& scrollDirectionProperty,
                     const PropertyInput& scrollSizeProperty)
  {
    Vector3 barSize = scrollBarSizeProperty.GetVector3();
    Vector3 relativePosition = scrollRelativePositionProperty.GetVector3();
    Vector3 size = scrollSizeProperty.GetVector3();
    const Vector3& min = scrollMinProperty.GetVector3();
    const Vector3& max = scrollMaxProperty.GetVector3();
    const Vector3& scrollDirection = scrollDirectionProperty.GetVector3();
    const Toolkit::ControlOrientation::Type& orientation = static_cast<Toolkit::ControlOrientation::Type>(scrollDirection.z);

    Vector3 domainSize = max - min;
    domainSize.x = fabsf(domainSize.x);
    domainSize.y = fabsf(domainSize.y);
    domainSize -= size;

    Vector3 mask;            // Mask movement aspect of scroll bar
    Vector3 relativeOffset;  // base position of scroll bar in relation to the container
    Vector3 absoluteOffset;  // absolute offset position of scroll bar

    if(mVertical)
    {
      switch(orientation)
      {
        case Toolkit::ControlOrientation::Up:
        {
          mask = Vector3::YAXIS;
          relativeOffset = (scrollDirection.y < 0.0f && relativePosition.y <= 0.0f) ? Vector3(1.0f, 1.0f, 0.0f) : Vector3(1.0f, 0.0f, 0.0f); // Right side of stage.
          absoluteOffset = (scrollDirection.y < 0.0f && relativePosition.y <= 0.0f) ? BAR_TAB_OFFSET_V + Vector3( barSize.width * 0.5f, -barSize.height * 0.5f, 1.0f ) : BAR_TAB_OFFSET_V + Vector3( barSize.width * 0.5f, barSize.height * 0.5f, 1.0f );
          break;
        }
        case Toolkit::ControlOrientation::Left:
        {
          mask = Vector3::XAXIS;
          relativeOffset = (scrollDirection.x <= 0.0f && relativePosition.y <= 0.0f) ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(0.0f, 0.0f, 0.0f); // Bottom side of stage.
          absoluteOffset = (scrollDirection.x <= 0.0f && relativePosition.y <= 0.0f) ? Vector3( -barSize.height * 0.5f, barSize.width * 0.5f, 1.0f ) : Vector3( barSize.height * 0.5f, barSize.width * 0.5f, 1.0f );
          break;
        }
        case Toolkit::ControlOrientation::Down:
        {
          mask = Vector3::YAXIS;
          relativeOffset = (scrollDirection.y <= 0.0f && relativePosition.y <= 0.0f) ? Vector3(0.0f, 1.0f, 0.0f) : Vector3(0.0f, 0.0f, 0.0f); // Left side of stage.
          absoluteOffset = (scrollDirection.y <= 0.0f && relativePosition.y <= 0.0f) ? Vector3( barSize.width * 0.5f, -barSize.height * 0.5f, 1.0f ) : Vector3( barSize.width * 0.5f, barSize.height * 0.5f, 1.0f );
          break;
        }
        case Toolkit::ControlOrientation::Right:
        {
          mask = Vector3::XAXIS;
          relativeOffset = (scrollDirection.x <= 0.0f && relativePosition.y <= 0.0f) ? Vector3(1.0f, 1.0f, 0.0f) : Vector3(0.0f, 1.0f, 0.0f); // Up side of stage.
          absoluteOffset = (scrollDirection.x <= 0.0f && relativePosition.y <= 0.0f) ? Vector3( -barSize.height * 0.5f, -barSize.width * 0.5f, 1.0f ) : Vector3( barSize.height * 0.5f, -barSize.width * 0.5f, 1.0f );
          break;
        }
      }
    }
    else
    {
      mask = Vector3::XAXIS;
      relativeOffset = Vector3(0.0f, 1.0f, 0.0f); // Bottom side of stage.
      absoluteOffset = BAR_TAB_OFFSET_H + Vector3( barSize.height * 0.5f, barSize.width * 0.5f, 1.0f );
    }

    Vector3 maskedRelativePosition = Toolkit::IsVertical(orientation) ? Vector3(relativePosition.x * (size.x-barSize.y), relativePosition.y * (size.y-barSize.y), 0.0f) * mask
                                   : Vector3(relativePosition.y * (size.x-barSize.y), relativePosition.x * (size.y-barSize.y), 0.0f) * mask;

    Vector3 finalPosition = relativeOffset * size + absoluteOffset + maskedRelativePosition;

    // If Wrapped Slider, then position 1 domain either before or after current slider.
    if(mWrap)
    {
      if(finalPosition.x < 0.5f)
      {
        finalPosition.x += size.x;
      }
      else
      {
        finalPosition.x -= size.x;
      }

      if(finalPosition.y < 0.5f)
      {
        finalPosition.y += size.y;
      }
      else
      {
        finalPosition.y -= size.y;
      }
    }

    return finalPosition;
  }

  bool mVertical;           ///< Whether vertical or horizontal.
  bool mWrap;               ///< Whether to wrap this position.
};

/**
 * ScrollBarInternal HitSize Constraint
 * Resizes HitArea to size of the container.
 */
struct ScrollBarInternalHitSizeConstraint
{
  /**
   * @param[in] vertical Whether this constraint controls a vertical scrollbar (true)
   * or a horizontal one (false)
   * @param[in] thickness The thickness of the scrollbar
   */
  ScrollBarInternalHitSizeConstraint(bool vertical,
                             float thickness)
  : mVertical(vertical),
    mThickness(thickness)
  {
  }

  /**
   * Constraint operator
   * @param[in] current The current HitSize
   * @param[in] scrollDirectionProperty The container's scroll direction.
   * @param[in] scrollSizeProperty The container's size of viewport.
   * @return The new ScrollBarInternal Hit Area size is returned.
   */
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& scrollDirectionProperty,
                     const PropertyInput& scrollSizeProperty)
  {
    const Vector3& scrollDirection = scrollDirectionProperty.GetVector3();
    const Toolkit::ControlOrientation::Type& orientation = static_cast<Toolkit::ControlOrientation::Type>(scrollDirection.z);
    Vector3 size = scrollSizeProperty.GetVector3();

    Vector3 mask;            // Mask size aspect of hit area.
    Vector3 offset;          // Add Offset size.

    if( (mVertical && Toolkit::IsVertical(orientation)) || (!mVertical && Toolkit::IsHorizontal(orientation)) )
    {
      mask = Vector3::YAXIS;
      offset = Vector3::XAXIS * mThickness;
    }
    else
    {
      mask = Vector3::XAXIS;
      offset = Vector3::YAXIS * mThickness;
    }

    return size * mask + offset;
  }

  bool mVertical;           ///< Whether vertical or horizontal.
  float mThickness;          ///< Thickness of the scroll bar
};

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

using namespace Dali;

BaseHandle Create()
{
  return BaseHandle();
}

TypeRegistration mType( typeid(Toolkit::ScrollBarInternal), typeid(Toolkit::ScrollComponent), Create );

}

ScrollBarInternal::ScrollBarInternal(Toolkit::Scrollable& container, bool vertical)
: mContainer(static_cast<Toolkit::Internal::Scrollable&>(container.GetImplementation())),
  mVertical(vertical),
  mAxisMask(vertical ? Vector3::YAXIS : Vector3::XAXIS),
  mDragMode(false)
{
  Image sliderImage = ResourceImage::New( BAR_TAB_IMAGE_PATH );

  mSlider = ImageActor::New( sliderImage );
  mSlider.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mSlider.SetAnchorPoint( AnchorPoint::CENTER );
  mSlider.SetSize( BAR_TAB_SIZE );
  mSlider.SetStyle( ImageActor::STYLE_NINE_PATCH );
  mSlider.SetNinePatchBorder( BAR_TAB_NINE_PATCH_BORDER );

  // A duplicate Slider should appear 1 domain away from the original Slider
  mSliderWrap = ImageActor::New( sliderImage );
  mSliderWrap.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mSliderWrap.SetAnchorPoint( AnchorPoint::CENTER );
  mSliderWrap.SetSize( BAR_TAB_SIZE );
  mSliderWrap.SetStyle( ImageActor::STYLE_NINE_PATCH );
  mSliderWrap.SetNinePatchBorder( BAR_TAB_NINE_PATCH_BORDER );

  // target the container to observe for scrolling
  Actor target = mContainer.Self();
  Constraint constraint = Constraint::New<bool>( Actor::Property::Visible,
                                      Source( target, vertical ? target.GetPropertyIndex(Scrollable::SCROLLABLE_CAN_SCROLL_VERTICAL) : target.GetPropertyIndex(Scrollable::SCROLLABLE_CAN_SCROLL_HORIZONTAL)),
                                      ScrollBarInternalVisibilityConstraint );
  mSlider.ApplyConstraint( constraint );
  mSliderWrap.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector3>( Actor::Property::Size,
                                                   Source( target, target.GetPropertyIndex( Toolkit::Scrollable::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                                   Source( target, target.GetPropertyIndex( Toolkit::Scrollable::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                                   Source( target, target.GetPropertyIndex( Toolkit::Scrollable::SCROLL_DIRECTION_PROPERTY_NAME ) ),
                                                   Source( target, Actor::Property::Size ),
                                                   ScrollBarInternalSizeConstraint( vertical ) );
  mSlider.ApplyConstraint( constraint );
  mSliderWrap.ApplyConstraint( constraint );

  constraint = Constraint::New<Quaternion>( Actor::Property::Rotation,
                                            Source( target, target.GetPropertyIndex( Toolkit::Scrollable::SCROLL_DIRECTION_PROPERTY_NAME ) ),
                                            ScrollBarInternalRotationConstraint( vertical ) );
  mSlider.ApplyConstraint( constraint );
  mSliderWrap.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector3>( Actor::Property::Position,
                                         Source( mSlider, Actor::Property::Size),
                                         Source( target, target.GetPropertyIndex( Toolkit::Scrollable::SCROLL_RELATIVE_POSITION_PROPERTY_NAME ) ),
                                         Source( target, target.GetPropertyIndex( Toolkit::Scrollable::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                         Source( target, target.GetPropertyIndex( Toolkit::Scrollable::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                         Source( target, target.GetPropertyIndex( Toolkit::Scrollable::SCROLL_DIRECTION_PROPERTY_NAME ) ),
                                         Source( target, Actor::Property::Size ),
                                         ScrollBarInternalPositionConstraint(vertical) );

  mSlider.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector3>( Actor::Property::Position,
                                         Source( mSlider, Actor::Property::Size),
                                         Source( target, target.GetPropertyIndex( Toolkit::Scrollable::SCROLL_RELATIVE_POSITION_PROPERTY_NAME ) ),
                                         Source( target, target.GetPropertyIndex( Toolkit::Scrollable::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                         Source( target, target.GetPropertyIndex( Toolkit::Scrollable::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                         Source( target, target.GetPropertyIndex( Toolkit::Scrollable::SCROLL_DIRECTION_PROPERTY_NAME ) ),
                                         Source( target, Actor::Property::Size ),
                                         ScrollBarInternalPositionConstraint(vertical, true) );
  mSliderWrap.ApplyConstraint( constraint );

  // Add Sliders to internal Actor, to avoid mixing up with regular
  // Actors added by user.
  mContainer.AddOverlay( mSlider );
  mContainer.AddOverlay( mSliderWrap );
  mContainer.ScrollStartedSignal().Connect( this, &ScrollBarInternal::OnStarted );
  mContainer.ScrollCompletedSignal().Connect( this, &ScrollBarInternal::OnCompleted );

  // Hit Area for dragging slider /////////////////////////////////////////////
  mHitArea = Actor::New();
  mHitArea.SetPosition(0.0f, 0.0f, 0.2f);

  mContainer.AddOverlay( mHitArea );
  constraint = Constraint::New<Vector3>( Actor::Property::Size,
                                         Source( target, target.GetPropertyIndex( Toolkit::Scrollable::SCROLL_DIRECTION_PROPERTY_NAME ) ),
                                         Source( target, Actor::Property::Size ),
                                         ScrollBarInternalHitSizeConstraint(vertical, BAR_TAB_SIZE.width) );
  mHitArea.ApplyConstraint( constraint );

  if(vertical)
  {
    mHitArea.SetParentOrigin(ParentOrigin::CENTER_RIGHT);
    mHitArea.SetAnchorPoint(AnchorPoint::CENTER_RIGHT);
  }
  else
  {
    mHitArea.SetParentOrigin(ParentOrigin::BOTTOM_CENTER);
    mHitArea.SetAnchorPoint(AnchorPoint::BOTTOM_CENTER);
  }

  WaitingContractDelay();
}

ScrollBarInternal::~ScrollBarInternal()
{
  DestructTimer();
}

void ScrollBarInternal::OnInitialize()
{
  EnableGestureDetection(Gesture::Type(Gesture::Pan));
}

void ScrollBarInternal::OnDisconnect()
{
  // Disconnect all connected callback functions.
  mContainer.RemoveOverlay( mSlider );
  mContainer.RemoveOverlay( mSliderWrap );
}

void ScrollBarInternal::OnPanGesture(Actor actor, PanGesture gesture)
{
  switch(gesture.state)
  {
    case Gesture::Started:
    {
      mDragMode = true;
      Show();
      mScrollStart = mContainer.GetCurrentScrollPosition();
      mGestureDisplacement = Vector3::ZERO;
      break;
    }
    case Gesture::Continuing:
    {
      Vector3 delta(gesture.displacement.x, gesture.displacement.y, 0.0f);
      mGestureDisplacement+=delta;

      Vector3 size = mContainer.Self().GetCurrentSize();
      Vector3 span = size - Vector3(BAR_TAB_SIZE.y, BAR_TAB_SIZE.y, 1.0f);
      Vector3 domainSize = mContainer.GetDomainSize();

      Vector3 position = mScrollStart + mGestureDisplacement * mAxisMask * domainSize / span;
      mContainer.ScrollTo(position, 0.0f);
      break;
    }
    default:
    {
      mDragMode = false;
      break;
    }
  }
}

void ScrollBarInternal::OnStarted(const Vector3& position)
{
  // TODO: Need to disable this for the scrollbar which isn't being scrolled.
  if(!mDragMode)
  {
    mDragMode = true;
    Show();
  }
}

void ScrollBarInternal::OnCompleted(const Vector3& position)
{
  if( mDragMode )
  {
    mDragMode = false;

    WaitingContractDelay();
  }
}

bool ScrollBarInternal::OnContractDelayExpired()
{
  if ( !mDragMode )
  {
    Hide();
  }

  DestructTimer();

  return true;
}

void ScrollBarInternal::Show()
{
  // Cancel any animation
  if(mAnimation)
  {
    mAnimation.Clear();
    mAnimation.Reset();
  }

  mAnimation = Animation::New( BAR_SHOW_TIME );
  mAnimation.OpacityTo( mSlider, 1.0f, AlphaFunctions::EaseIn );
  mAnimation.OpacityTo( mSliderWrap, 1.0f, AlphaFunctions::EaseIn );
  mAnimation.Play();

  DestructTimer();
}

void ScrollBarInternal::Hide()
{
  // Cancel any animation
  if(mAnimation)
  {
    mAnimation.Clear();
    mAnimation.Reset();
  }

  mAnimation = Animation::New( BAR_HIDE_TIME );
  mAnimation.OpacityTo( mSlider, 0.0f, AlphaFunctions::EaseIn );
  mAnimation.OpacityTo( mSliderWrap, 0.0f, AlphaFunctions::EaseIn );
  mAnimation.Play();
}

void ScrollBarInternal::CreateTimer()
{
  if( !mTimer )
  {
    // Create timer for contract delay
    mTimer = Timer::New( BAR_CONTRACT_DELAY * SECOND_UNIT );
    mTimer.TickSignal().Connect( this, &ScrollBarInternal::OnContractDelayExpired );
  }
}

void ScrollBarInternal::DestructTimer()
{
  if( mTimer )
  {
    mTimer.Stop();
    mTimer.TickSignal().Disconnect( this, &ScrollBarInternal::OnContractDelayExpired );
    mTimer.Reset();
  }
}

void ScrollBarInternal::WaitingContractDelay()
{
  CreateTimer();
  mTimer.Start();
}

Toolkit::ScrollBarInternal ScrollBarInternal::New(Toolkit::Scrollable& container, bool vertical)
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< ScrollBarInternal > impl = new ScrollBarInternal( container, vertical );

  // Pass ownership to CustomActor handle
  Toolkit::ScrollBarInternal handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
