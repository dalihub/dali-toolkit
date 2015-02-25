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
#include <dali-toolkit/internal/controls/scroll-bar/scroll-bar-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/images/resource-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scrollable/item-view/item-view-impl.h>

using namespace Dali;

namespace
{

const char* DEFAULT_INDICATOR_IMAGE_PATH = DALI_IMAGE_DIR "popup_scroll.png";
const Vector4 DEFAULT_INDICATOR_NINE_PATCH_BORDER(4.0f, 9.0f, 7.0f, 11.0f);
const float MINIMUM_INDICATOR_HEIGHT(20.0f); // The minimum indicator height for the nine patch border
const float DEFAULT_SLIDER_DEPTH(1.0f);
const float DEFAULT_INDICATOR_SHOW_DURATION(0.5f);
const float DEFAULT_INDICATOR_HIDE_DURATION(0.5f);
const float DEFAULT_PAN_GESTURE_PROCESS_TIME(16.7f); // 16.7 milliseconds, i.e. one frame
const float DEFAULT_INDICATOR_FIXED_HEIGHT(80.0f);

/**
 * Indicator size depends on both indicator's parent size and the scroll content size
 */
Vector3 IndicatorSize( const Vector3& parentSize, float contentSize)
{
  float height = contentSize > parentSize.height ?
                 parentSize.height * ( parentSize.height / contentSize ) :
                 parentSize.height * ( (parentSize.height - contentSize * 0.5f) / parentSize.height);
  return Vector3( parentSize.width, std::max(MINIMUM_INDICATOR_HEIGHT, height), parentSize.depth );
}

/**
 * Indicator position constraint
 * Positions the indicator to reflect the current scroll position within the scroll domain.
 */
struct IndicatorPositionConstraint
{
  /**
   * @param[in] minPosition The minimum limit of scroll position
   * @param[in] maxPosition the maximum limit of scroll position
   */
  IndicatorPositionConstraint(float minPosition, float maxPosition)
  : mMinPosition(minPosition),
    mMaxPosition(maxPosition)
  {
  }

  /**
   * Constraint operator
   * @param[in] current The current indicator position
   * @param[in] indicatorSizeProperty The size of indicator.
   * @param[in] parentSizeProperty The parent size of indicator.
   * @param[in] scrollPositionProperty The scroll position of the scrollable container // (from 0.0 -> 1.0 in each axis)
   * @return The new indicator position is returned.
   */
  Vector3 operator()(const Vector3& current,
                     const PropertyInput& indicatorSizeProperty,
                     const PropertyInput& parentSizeProperty,
                     const PropertyInput& scrollPositionProperty)
  {
    Vector3 indicatorSize = indicatorSizeProperty.GetVector3();
    Vector3 parentSize = parentSizeProperty.GetVector3();
    float scrollPosition = scrollPositionProperty.GetFloat();

    const float domainSize = fabs(mMaxPosition - mMinPosition);
    float relativePosition = (mMaxPosition - scrollPosition) / domainSize;
    return Vector3(current.x, relativePosition * (parentSize.height - indicatorSize.height), DEFAULT_SLIDER_DEPTH);
  }

  float mMinPosition;  ///< The minimum scroll position
  float mMaxPosition;  ///< The maximum scroll position
};

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

const Property::Index ScrollBar::PROPERTY_INDICATOR_HEIGHT_POLICY( Internal::ScrollBar::SCROLLBAR_PROPERTY_START_INDEX );
const Property::Index ScrollBar::PROPERTY_INDICATOR_FIXED_HEIGHT( Internal::ScrollBar::SCROLLBAR_PROPERTY_START_INDEX + 1 );
const Property::Index ScrollBar::PROPERTY_INDICATOR_SHOW_DURATION( Internal::ScrollBar::SCROLLBAR_PROPERTY_START_INDEX + 2 );
const Property::Index ScrollBar::PROPERTY_INDICATOR_HIDE_DURATION( Internal::ScrollBar::SCROLLBAR_PROPERTY_START_INDEX + 3 );

namespace Internal
{

namespace
{

using namespace Dali;

const char* INDICATOR_HEIGHT_POLICY_NAME[] = {"Variable", "Fixed"};

BaseHandle Create()
{
  return Toolkit::ScrollBar::New();
}

TypeRegistration typeRegistration( typeid( Toolkit::ScrollBar ), typeid( Toolkit::ScrollComponent ), Create );

const char* const SCROLL_POSITION_NOTIFIED_SIGNAL_NAME = "scroll-position-notified";

PropertyRegistration property1( typeRegistration, "indicator-height-policy", Toolkit::ScrollBar::PROPERTY_INDICATOR_HEIGHT_POLICY, Property::STRING, &ScrollBar::SetProperty, &ScrollBar::GetProperty );
PropertyRegistration property2( typeRegistration, "indicator-fixed-height",  Toolkit::ScrollBar::PROPERTY_INDICATOR_FIXED_HEIGHT,  Property::FLOAT,  &ScrollBar::SetProperty, &ScrollBar::GetProperty );
PropertyRegistration property3( typeRegistration, "indicator-show-duration",  Toolkit::ScrollBar::PROPERTY_INDICATOR_SHOW_DURATION,  Property::FLOAT,  &ScrollBar::SetProperty, &ScrollBar::GetProperty );
PropertyRegistration property4( typeRegistration, "indicator-hide-duration",  Toolkit::ScrollBar::PROPERTY_INDICATOR_HIDE_DURATION,  Property::FLOAT,  &ScrollBar::SetProperty, &ScrollBar::GetProperty );
}

ScrollBar::ScrollBar()
: mIndicatorShowDuration(DEFAULT_INDICATOR_SHOW_DURATION),
  mIndicatorHideDuration(DEFAULT_INDICATOR_HIDE_DURATION),
  mScrollStart(0.0f),
  mIsPanning(false),
  mCurrentScrollPosition(0.0f),
  mIndicatorHeightPolicy(Toolkit::ScrollBar::Variable),
  mIndicatorFixedHeight(DEFAULT_INDICATOR_FIXED_HEIGHT),
  mPropertyIndicatorPosition(Property::INVALID_INDEX)
{
}

ScrollBar::~ScrollBar()
{
}

void ScrollBar::OnInitialize()
{
  Actor self = Self();

  Image indicatorImage = ResourceImage::New( DEFAULT_INDICATOR_IMAGE_PATH );
  mIndicator = ImageActor::New( indicatorImage );
  mIndicator.SetNinePatchBorder( DEFAULT_INDICATOR_NINE_PATCH_BORDER );
  mIndicator.SetStyle( ImageActor::STYLE_NINE_PATCH );
  mIndicator.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mIndicator.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  self.Add(mIndicator);

  self.SetDrawMode(DrawMode::OVERLAY);

  // Enable the pan gesture which is attached to the control
  EnableGestureDetection(Gesture::Type(Gesture::Pan));
}

void ScrollBar::OnScrollConnectorSet( Toolkit::ScrollConnector oldConnector )
{
  if( oldConnector )
  {
    oldConnector.DomainChangedSignal().Disconnect(this, &ScrollBar::OnScrollDomainChanged);

    mScrollPositionObject.Reset();
  }

  if( mScrollConnector )
  {
    mScrollPositionObject = mScrollConnector.GetScrollPositionObject();

    ApplyConstraints();
    mScrollConnector.DomainChangedSignal().Connect(this, &ScrollBar::OnScrollDomainChanged);
  }
}

void ScrollBar::SetIndicatorImage( Image image )
{
  mIndicator.SetImage(image);
}

Actor ScrollBar::GetScrollIndicator()
{
  return mIndicator;
}

void ScrollBar::ApplyConstraints()
{
  if( mScrollConnector )
  {
    Constraint constraint;

    if(mIndicatorSizeConstraint)
    {
      mIndicator.RemoveConstraint(mIndicatorSizeConstraint);
    }

    // Set indicator height according to the indicator's height policy
    if(mIndicatorHeightPolicy == Toolkit::ScrollBar::Fixed)
    {
      mIndicator.SetSize(Self().GetCurrentSize().width, mIndicatorFixedHeight);
    }
    else
    {
      mIndicator.SetSize( IndicatorSize( Self().GetCurrentSize(), mScrollConnector.GetContentLength() ) );
    }

    if(mIndicatorPositionConstraint)
    {
      mIndicator.RemoveConstraint(mIndicatorPositionConstraint);
    }

    constraint = Constraint::New<Vector3>( Actor::POSITION,
                                           LocalSource( Actor::SIZE ),
                                           ParentSource( Actor::SIZE ),
                                           Source( mScrollPositionObject, Toolkit::ScrollConnector::SCROLL_POSITION ),
                                           IndicatorPositionConstraint( mScrollConnector.GetMinLimit(), mScrollConnector.GetMaxLimit() ) );
    mIndicatorPositionConstraint = mIndicator.ApplyConstraint( constraint );
  }
}

void ScrollBar::SetPositionNotifications( const std::vector<float>& positions )
{
  if(mScrollPositionObject)
  {
    if(mPositionNotification)
    {
      mScrollPositionObject.RemovePropertyNotification(mPositionNotification);
    }
    mPositionNotification = mScrollPositionObject.AddPropertyNotification( Toolkit::ScrollConnector::SCROLL_POSITION, VariableStepCondition(positions) );
    mPositionNotification.NotifySignal().Connect( this, &ScrollBar::OnScrollPositionNotified );
  }
}

void ScrollBar::OnScrollPositionNotified(PropertyNotification& source)
{
  // Emit the signal to notify the scroll position crossing
  mScrollPositionNotifiedSignal.Emit(mScrollConnector.GetScrollPosition());
}

void ScrollBar::Show()
{
  // Cancel any animation
  if(mAnimation)
  {
    mAnimation.Clear();
    mAnimation.Reset();
  }

  if(mIndicatorShowDuration > 0.0f)
  {
    mAnimation = Animation::New( mIndicatorShowDuration );
    mAnimation.OpacityTo( Self(), 1.0f, AlphaFunctions::EaseIn );
    mAnimation.Play();
  }
  else
  {
    Self().SetOpacity(1.0f);
  }
}

void ScrollBar::Hide()
{
  // Cancel any animation
  if(mAnimation)
  {
    mAnimation.Clear();
    mAnimation.Reset();
  }

  if(mIndicatorHideDuration > 0.0f)
  {
    mAnimation = Animation::New( mIndicatorHideDuration );
    mAnimation.OpacityTo( Self(), 0.0f, AlphaFunctions::EaseIn );
    mAnimation.Play();
  }
  else
  {
    Self().SetOpacity(0.0f);
  }
}

bool ScrollBar::OnPanGestureProcessTick()
{
  // Update the scroll position property.
  if( mScrollConnector )
  {
    mScrollConnector.SetScrollPosition(mCurrentScrollPosition);
  }

  return true;
}

void ScrollBar::OnPan( PanGesture gesture )
{
  if(mScrollConnector)
  {
    Dali::Toolkit::ItemView itemView = Dali::Toolkit::ItemView::DownCast(Self().GetParent());

    switch(gesture.state)
    {
      case Gesture::Started:
      {
        if( !mTimer )
        {
          // Make sure the pan gesture is only being processed once per frame.
          mTimer = Timer::New( DEFAULT_PAN_GESTURE_PROCESS_TIME );
          mTimer.TickSignal().Connect( this, &ScrollBar::OnPanGestureProcessTick );
          mTimer.Start();
        }

        Show();
        mScrollStart = mScrollConnector.GetScrollPosition();
        mGestureDisplacement = Vector3::ZERO;
        mIsPanning = true;

        break;
      }
      case Gesture::Continuing:
      {
        Vector3 delta(gesture.displacement.x, gesture.displacement.y, 0.0f);
        mGestureDisplacement+=delta;

        Vector3 span = Self().GetCurrentSize() - mIndicator.GetCurrentSize();
        const float domainSize = fabs(mScrollConnector.GetMaxLimit() - mScrollConnector.GetMinLimit());
        mCurrentScrollPosition = mScrollStart - mGestureDisplacement.y * domainSize / span.y;
        mCurrentScrollPosition = std::min(mScrollConnector.GetMaxLimit(), std::max(mCurrentScrollPosition, mScrollConnector.GetMinLimit()));

        break;
      }
      default:
      {
        mIsPanning = false;

        if( mTimer )
        {
          // Destroy the timer when pan gesture is finished.
          mTimer.Stop();
          mTimer.TickSignal().Disconnect( this, &ScrollBar::OnPanGestureProcessTick );
          mTimer.Reset();
        }

        if(itemView)
        {
          // Refresh the ItemView cache with extra items
          GetImpl(itemView).DoRefresh(mCurrentScrollPosition, true);
        }

        break;
      }
    }

    if(itemView)
    {
      // Disable automatic refresh in ItemView during fast scrolling
      GetImpl(itemView).SetRefreshEnabled(!mIsPanning);
    }
  }
}

void ScrollBar::OnControlSizeSet( const Vector3& size )
{
  if(mIndicatorHeightPolicy != Toolkit::ScrollBar::Fixed && mScrollConnector)
  {
    mIndicator.SetSize( IndicatorSize( size, mScrollConnector.GetContentLength() ) );
  }
}

void ScrollBar::OnScrollDomainChanged(float minPosition, float maxPosition, float contentSize)
{
  // Reapply constraints when the scroll domain is changed
  ApplyConstraints();
}

void ScrollBar::SetIndicatorHeightPolicy( Toolkit::ScrollBar::IndicatorHeightPolicy policy )
{
  mIndicatorHeightPolicy = policy;
  ApplyConstraints();
}

Toolkit::ScrollBar::IndicatorHeightPolicy ScrollBar::GetIndicatorHeightPolicy()
{
  return mIndicatorHeightPolicy;
}

void ScrollBar::SetIndicatorFixedHeight( float height )
{
  mIndicatorFixedHeight = height;
  ApplyConstraints();
}

float ScrollBar::GetIndicatorFixedHeight()
{
  return mIndicatorFixedHeight;
}

void ScrollBar::SetIndicatorShowDuration( float durationSeconds )
{
  mIndicatorShowDuration = durationSeconds;
}

float ScrollBar::GetIndicatorShowDuration()
{
  return mIndicatorShowDuration;
}

void ScrollBar::SetIndicatorHideDuration( float durationSeconds )
{
  mIndicatorHideDuration = durationSeconds;
}

float ScrollBar::GetIndicatorHideDuration()
{
  return mIndicatorHideDuration;
}

void ScrollBar::OnIndicatorHeightPolicyPropertySet( Property::Value propertyValue )
{
  std::string policyName( propertyValue.Get<std::string>() );
  if(policyName == "Variable")
  {
    SetIndicatorHeightPolicy(Toolkit::ScrollBar::Variable);
  }
  else if(policyName == "Fixed")
  {
    SetIndicatorHeightPolicy(Toolkit::ScrollBar::Fixed);
  }
  else
  {
    DALI_ASSERT_ALWAYS( !"ScrollBar::OnIndicatorHeightPolicyPropertySet(). Invalid Property value." );
  }
}

bool ScrollBar::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::ScrollBar scrollBar = Toolkit::ScrollBar::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), SCROLL_POSITION_NOTIFIED_SIGNAL_NAME ) )
  {
    scrollBar.ScrollPositionNotifiedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void ScrollBar::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::ScrollBar scrollBar = Toolkit::ScrollBar::DownCast( Dali::BaseHandle( object ) );

  if( scrollBar )
  {
    ScrollBar& scrollBarImpl( GetImpl( scrollBar ) );
    switch( index )
    {
      case Toolkit::ScrollBar::PROPERTY_INDICATOR_HEIGHT_POLICY:
      {
        scrollBarImpl.OnIndicatorHeightPolicyPropertySet( value );
        break;
      }
      case Toolkit::ScrollBar::PROPERTY_INDICATOR_FIXED_HEIGHT:
      {
        scrollBarImpl.SetIndicatorFixedHeight(value.Get<float>());
        break;
      }
      case Toolkit::ScrollBar::PROPERTY_INDICATOR_SHOW_DURATION:
      {
        scrollBarImpl.SetIndicatorShowDuration(value.Get<float>());
        break;
      }
      case Toolkit::ScrollBar::PROPERTY_INDICATOR_HIDE_DURATION:
      {
        scrollBarImpl.SetIndicatorHideDuration(value.Get<float>());
        break;
      }
    }
  }
}

Property::Value ScrollBar::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::ScrollBar scrollBar = Toolkit::ScrollBar::DownCast( Dali::BaseHandle( object ) );

  if( scrollBar )
  {
    ScrollBar& scrollBarImpl( GetImpl( scrollBar ) );
    switch( index )
    {
      case Toolkit::ScrollBar::PROPERTY_INDICATOR_HEIGHT_POLICY:
      {
        value = INDICATOR_HEIGHT_POLICY_NAME[ scrollBarImpl.GetIndicatorHeightPolicy() ];
        break;
      }
      case Toolkit::ScrollBar::PROPERTY_INDICATOR_FIXED_HEIGHT:
      {
        value = scrollBarImpl.GetIndicatorFixedHeight();
        break;
      }
      case Toolkit::ScrollBar::PROPERTY_INDICATOR_SHOW_DURATION:
      {
        value = scrollBarImpl.GetIndicatorShowDuration();
        break;
      }
      case Toolkit::ScrollBar::PROPERTY_INDICATOR_HIDE_DURATION:
      {
        value = scrollBarImpl.GetIndicatorHideDuration();
        break;
      }
    }
  }
  return value;
}

Toolkit::ScrollBar ScrollBar::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< ScrollBar > impl = new ScrollBar();

  // Pass ownership to CustomActor handle
  Toolkit::ScrollBar handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
