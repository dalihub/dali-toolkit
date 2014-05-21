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

#include <dali-toolkit/internal/controls/scroll-bar/scroll-bar-impl.h>
#include <dali-toolkit/internal/controls/scrollable/item-view/item-view-impl.h>

using namespace Dali;

namespace
{

const char* DEFAULT_INDICATOR_IMAGE_PATH = DALI_IMAGE_DIR "popup_scroll.png";
const Vector4 DEFAULT_INDICATOR_NINE_PATCH_BORDER(4.0f, 9.0f, 7.0f, 11.0f);
const float MINIMUM_INDICATOR_HEIGHT(20.0f); // The minimum indicator height for the nine patch border
const float DEFAULT_SLIDER_DEPTH(1.0f);
const float INDICATOR_SHOW_TIME(0.5f);
const float INDICATOR_HIDE_TIME(0.5f);
const float DEFAULT_PAN_GESTURE_PROCESS_TIME(16.7f); // 16.7 milliseconds, i.e. one frame
const float DEFAULT_INDICATOR_FIXED_HEIGHT(80.0f);

/**
 * Indicator size constraint
 * Indicator size depends on both indicator's parent size and the scroll content size
 */
struct IndicatorSizeConstraint
{
  /**
   * @param[in] contentSize The size of scrollable content
   */
  IndicatorSizeConstraint(float contentSize)
  : mContentSize(contentSize)
  {
  }

  /**
   * Constraint operator
   * @param[in] current The current indicator size
   * @param[in] parentSizeProperty The parent size of scroll indicator.
   * @return The new scroll indicator size.
   */
  Vector3 operator()(const Vector3& current,
                     const PropertyInput& parentSizeProperty)
  {
    const Vector3& parentSize = parentSizeProperty.GetVector3();
    float height = mContentSize > parentSize.height ? parentSize.height * ( parentSize.height / mContentSize ) : parentSize.height * ( (parentSize.height - mContentSize * 0.5f) / parentSize.height);
    return Vector3( parentSize.width, std::max(MINIMUM_INDICATOR_HEIGHT, height), parentSize.depth );
  }

  float mContentSize;  ///< The size of scrollable content
};

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

TypeRegistration typeRegistration( typeid(Toolkit::ScrollBar), typeid(Toolkit::ScrollComponent), Create );

PropertyRegistration property1( typeRegistration, "indicator-height-policy", Toolkit::ScrollBar::PROPERTY_INDICATOR_HEIGHT_POLICY, Property::STRING, &ScrollBar::SetProperty, &ScrollBar::GetProperty );
PropertyRegistration property2( typeRegistration, "indicator-fixed-height",  Toolkit::ScrollBar::PROPERTY_INDICATOR_FIXED_HEIGHT,  Property::FLOAT,  &ScrollBar::SetProperty, &ScrollBar::GetProperty );
}

ScrollBar::ScrollBar()
: mScrollStart(0.0f),
  mIsPanning(false),
  mCurrentScrollPosition(0.0f),
  mIndicatorHeightPolicy(Toolkit::ScrollBar::Variable),
  mIndicatorFixedHeight(DEFAULT_INDICATOR_FIXED_HEIGHT)
{
}

ScrollBar::~ScrollBar()
{
}

void ScrollBar::OnInitialize()
{
  Actor self = Self();

  Image indicatorImage = Image::New( DEFAULT_INDICATOR_IMAGE_PATH );
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

void ScrollBar::SetBackgroundImage( Image image, const Vector4& border )
{
  if (!mBackground )
  {
    mBackground = ImageActor::New( image );
    mBackground.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mBackground.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    Self().Add(mBackground);
  }
  else
  {
    mBackground.SetImage(image);
  }

  mBackground.SetNinePatchBorder( border );
  mBackground.SetStyle( ImageActor::STYLE_NINE_PATCH );
}

void ScrollBar::SetIndicatorImage( Image image, const Vector4& border )
{
  mIndicator.SetImage(image);
  mIndicator.SetNinePatchBorder( border );
  mIndicator.SetStyle( ImageActor::STYLE_NINE_PATCH );
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
      constraint = Constraint::New<Vector3>( Actor::SIZE,
                                             ParentSource( Actor::SIZE ),
                                             IndicatorSizeConstraint( mScrollConnector.GetContentLength() ) );
      mIndicatorSizeConstraint = mIndicator.ApplyConstraint( constraint );
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

    if( mBackground )
    {
      mBackground.RemoveConstraints();

      constraint = Constraint::New<Vector3>(Actor::SIZE,
                                            ParentSource(Actor::SIZE),
                                            EqualToConstraint());
      mBackground.ApplyConstraint(constraint);
    }
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
  mScrollPositionNotifiedSignal.Emit(mScrollPositionObject.GetProperty<float>( Toolkit::ScrollConnector::SCROLL_POSITION ));
}

void ScrollBar::Show()
{
  // Cancel any animation
  if(mAnimation)
  {
    mAnimation.Clear();
    mAnimation.Reset();
  }

  mAnimation = Animation::New( INDICATOR_SHOW_TIME );
  mAnimation.OpacityTo( Self(), 1.0f, AlphaFunctions::EaseIn );
  mAnimation.Play();
}

void ScrollBar::Hide()
{
  // Cancel any animation
  if(mAnimation)
  {
    mAnimation.Clear();
    mAnimation.Reset();
  }

  mAnimation = Animation::New( INDICATOR_HIDE_TIME );
  mAnimation.OpacityTo( Self(), 0.0f, AlphaFunctions::EaseIn );
  mAnimation.Play();
}

bool ScrollBar::OnPanGestureProcessTick()
{
  // Update the scroll position property.
  mScrollPositionObject.SetProperty( Toolkit::ScrollConnector::SCROLL_POSITION, mCurrentScrollPosition );

  Dali::Toolkit::ItemView itemView = Dali::Toolkit::ItemView::DownCast(Self().GetParent());
  if(itemView)
  {
    // Refresh ItemView immediately when the scroll position is changed.
    GetImpl(itemView).DoRefresh(mCurrentScrollPosition, false); // No need to cache extra items.
  }

  return true;
}

void ScrollBar::OnPan( PanGesture gesture )
{
  if(mScrollPositionObject)
  {
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
        mScrollStart = mScrollPositionObject.GetProperty<float>( Toolkit::ScrollConnector::SCROLL_POSITION );
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

        break;
      }
    }

    Dali::Toolkit::ItemView itemView = Dali::Toolkit::ItemView::DownCast(Self().GetParent());
    if(itemView)
    {
      // Disable automatic refresh in ItemView during fast scrolling
      GetImpl(itemView).SetRefreshEnabled(!mIsPanning);
    }
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
