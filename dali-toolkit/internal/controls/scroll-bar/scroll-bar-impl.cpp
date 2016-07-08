/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <cstring> // for strcmp
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scrollable/item-view/item-view-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

using namespace Dali;

namespace
{

const char* DEFAULT_INDICATOR_IMAGE_PATH = DALI_IMAGE_DIR "popup_scroll.9.png";
const float DEFAULT_SLIDER_DEPTH(1.0f);
const float DEFAULT_INDICATOR_SHOW_DURATION(0.5f);
const float DEFAULT_INDICATOR_HIDE_DURATION(0.5f);
const float DEFAULT_PAN_GESTURE_PROCESS_TIME(16.7f); // 16.7 milliseconds, i.e. one frame
const float DEFAULT_INDICATOR_FIXED_HEIGHT(80.0f);
const float DEFAULT_INDICATOR_MINIMUM_HEIGHT(0.0f);
const float DEFAULT_INDICATOR_START_PADDING(0.0f);
const float DEFAULT_INDICATOR_END_PADDING(0.0f);

/**
 * Indicator size constraint
 * Indicator size depends on both indicator's parent size and the scroll content size
 */
struct IndicatorSizeConstraint
{
  /**
   * @param[in] minimumHeight The minimum height for the indicator
   * @param[in] padding The sum of the padding at the start & end of the indicator
   */
  IndicatorSizeConstraint( float minimumHeight, float padding )
  : mMinimumHeight( minimumHeight ),
    mPadding( padding )
  {
  }

  /**
   * Constraint operator
   * @param[in] current The current indicator size
   * @param[in] parentSizeProperty The parent size of scroll indicator.
   * @return The new scroll indicator size.
   */
  void operator()( Vector3& current, const PropertyInputContainer& inputs )
  {
    const Vector3& parentSize = inputs[0]->GetVector3();
    const float contentSize = inputs[1]->GetFloat();

    // Take into account padding that may exist at the beginning and end of the indicator.
    const float parentHeightMinusPadding = parentSize.height - mPadding;

    float height = contentSize > parentHeightMinusPadding ?
                   parentHeightMinusPadding * ( parentHeightMinusPadding / contentSize ) :
                   parentHeightMinusPadding * ( ( parentHeightMinusPadding - contentSize * 0.5f ) / parentHeightMinusPadding );

    current.y = std::max( mMinimumHeight, height );
  }

  float mMinimumHeight;
  float mPadding;
};

/**
 * Indicator position constraint
 * Positions the indicator to reflect the current scroll position within the scroll domain.
 */
struct IndicatorPositionConstraint
{
  /**
   * @param[in] startPadding The padding at the start of the indicator
   * @param[in] endPadding The padding at the end of the indicator
   */
  IndicatorPositionConstraint( float startPadding, float endPadding )
  : mStartPadding( startPadding ),
    mEndPadding( endPadding )
  {
  }

  /**
   * Constraint operator
   * @param[in,out] current The current indicator position
   * @param[in] inputs Contains the size of indicator, the size of indicator's parent, and the scroll position of the scrollable container (from 0.0 -> 1.0 in each axis)
   * @return The new indicator position is returned.
   */
  void operator()( Vector3& current, const PropertyInputContainer& inputs )
  {
    const Vector3& indicatorSize = inputs[0]->GetVector3();
    const Vector3& parentSize = inputs[1]->GetVector3();
    const float scrollPosition = -inputs[2]->GetFloat();
    const float minimumScrollPosition = inputs[3]->GetFloat();
    const float maximumScrollPosition = inputs[4]->GetFloat();

    // Take into account padding that may exist at the beginning and end of the indicator.
    const float parentHeightMinusPadding = parentSize.height - ( mStartPadding + mEndPadding );

    float relativePosition = std::max( 0.0f, std::min( 1.0f, ( scrollPosition - minimumScrollPosition ) / ( maximumScrollPosition - minimumScrollPosition ) ) );
    current.y = mStartPadding + ( parentHeightMinusPadding - indicatorSize.height ) * relativePosition;
    current.z = DEFAULT_SLIDER_DEPTH;
  }

  float mStartPadding;
  float mEndPadding;
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
  return Toolkit::ScrollBar::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::ScrollBar, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, ScrollBar, "scrollDirection",                   STRING, SCROLL_DIRECTION          )
DALI_PROPERTY_REGISTRATION( Toolkit, ScrollBar, "indicatorHeightPolicy",             STRING, INDICATOR_HEIGHT_POLICY   )
DALI_PROPERTY_REGISTRATION( Toolkit, ScrollBar, "indicatorFixedHeight",              FLOAT,  INDICATOR_FIXED_HEIGHT    )
DALI_PROPERTY_REGISTRATION( Toolkit, ScrollBar, "indicatorShowDuration",             FLOAT,  INDICATOR_SHOW_DURATION   )
DALI_PROPERTY_REGISTRATION( Toolkit, ScrollBar, "indicatorHideDuration",             FLOAT,  INDICATOR_HIDE_DURATION   )
DALI_PROPERTY_REGISTRATION( Toolkit, ScrollBar, "scrollPositionIntervals",           ARRAY,  SCROLL_POSITION_INTERVALS )
DALI_PROPERTY_REGISTRATION( Toolkit, ScrollBar, "indicatorMinimumHeight",            FLOAT,  INDICATOR_MINIMUM_HEIGHT  )
DALI_PROPERTY_REGISTRATION( Toolkit, ScrollBar, "indicatorStartPadding",             FLOAT,  INDICATOR_START_PADDING   )
DALI_PROPERTY_REGISTRATION( Toolkit, ScrollBar, "indicatorEndPadding",               FLOAT,  INDICATOR_END_PADDING     )

DALI_SIGNAL_REGISTRATION(   Toolkit, ScrollBar, "panFinished",                       PAN_FINISHED_SIGNAL )
DALI_SIGNAL_REGISTRATION(   Toolkit, ScrollBar, "scrollPositionIntervalReached",     SCROLL_POSITION_INTERVAL_REACHED_SIGNAL )

DALI_TYPE_REGISTRATION_END()

const char* SCROLL_DIRECTION_NAME[] = {"Vertical", "Horizontal"};
const char* INDICATOR_HEIGHT_POLICY_NAME[] = {"Variable", "Fixed"};

}

ScrollBar::ScrollBar(Toolkit::ScrollBar::Direction direction)
: Control( ControlBehaviour( REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mIndicatorShowAlpha(1.0f),
  mDirection(direction),
  mScrollableObject(WeakHandleBase()),
  mPropertyScrollPosition(Property::INVALID_INDEX),
  mPropertyMinScrollPosition(Property::INVALID_INDEX),
  mPropertyMaxScrollPosition(Property::INVALID_INDEX),
  mPropertyScrollContentSize(Property::INVALID_INDEX),
  mIndicatorShowDuration(DEFAULT_INDICATOR_SHOW_DURATION),
  mIndicatorHideDuration(DEFAULT_INDICATOR_HIDE_DURATION),
  mScrollStart(0.0f),
  mCurrentScrollPosition(0.0f),
  mIndicatorHeightPolicy(Toolkit::ScrollBar::Variable),
  mIndicatorFixedHeight(DEFAULT_INDICATOR_FIXED_HEIGHT),
  mIndicatorMinimumHeight(DEFAULT_INDICATOR_MINIMUM_HEIGHT),
  mIndicatorStartPadding(DEFAULT_INDICATOR_START_PADDING),
  mIndicatorEndPadding(DEFAULT_INDICATOR_END_PADDING),
  mIsPanning(false),
  mIndicatorFirstShow(true)
{
}

ScrollBar::~ScrollBar()
{
}

void ScrollBar::OnInitialize()
{
  CreateDefaultIndicatorActor();
  Self().SetDrawMode(DrawMode::OVERLAY_2D);
}

void ScrollBar::SetScrollPropertySource( Handle handle, Property::Index propertyScrollPosition, Property::Index propertyMinScrollPosition, Property::Index propertyMaxScrollPosition, Property::Index propertyScrollContentSize )
{
  if( handle
      && propertyScrollPosition != Property::INVALID_INDEX
      && propertyMinScrollPosition != Property::INVALID_INDEX
      && propertyMaxScrollPosition != Property::INVALID_INDEX
      && propertyScrollContentSize != Property::INVALID_INDEX )
  {
    mScrollableObject = WeakHandleBase(handle);
    mPropertyScrollPosition = propertyScrollPosition;
    mPropertyMinScrollPosition = propertyMinScrollPosition;
    mPropertyMaxScrollPosition = propertyMaxScrollPosition;
    mPropertyScrollContentSize = propertyScrollContentSize;

    ApplyConstraints();
  }
  else
  {
    DALI_LOG_ERROR("Can not set empty handle of source object or invalid source property index\n");
  }
}

void ScrollBar::CreateDefaultIndicatorActor()
{
  Toolkit::ImageView indicator = Toolkit::ImageView::New( DEFAULT_INDICATOR_IMAGE_PATH );
  indicator.SetParentOrigin( ParentOrigin::TOP_LEFT );
  indicator.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  SetScrollIndicator(indicator);
}

void ScrollBar::SetScrollIndicator( Actor indicator )
{
  // Don't allow empty handle
  if( indicator )
  {
    mIndicator = indicator;
    mIndicatorFirstShow = true;
    Self().Add(mIndicator);

    EnableGestureDetection(Gesture::Type(Gesture::Pan));

    PanGestureDetector detector( GetPanGestureDetector() );
    detector.DetachAll();
    detector.Attach( mIndicator );

    unsigned int childCount = mIndicator.GetChildCount();
    for ( unsigned int index = 0; index < childCount; index++ )
    {
      Actor child = mIndicator.GetChildAt( index );
      if ( child )
      {
        detector.Attach( child );
      }
    }
  }
  else
  {
    DALI_LOG_ERROR("Empty handle of scroll indicator\n");
  }
}

Actor ScrollBar::GetScrollIndicator()
{
  return mIndicator;
}

void ScrollBar::ApplyConstraints()
{
  Handle scrollableHandle = mScrollableObject.GetBaseHandle();

  if( scrollableHandle )
  {
    if(mIndicatorSizeConstraint)
    {
      mIndicatorSizeConstraint.Remove();
    }

    // Set indicator height according to the indicator's height policy
    if(mIndicatorHeightPolicy == Toolkit::ScrollBar::Fixed)
    {
      mIndicator.SetSize(Self().GetCurrentSize().width, mIndicatorFixedHeight);
    }
    else
    {
      mIndicatorSizeConstraint = Constraint::New<Vector3>( mIndicator, Actor::Property::SIZE,
                                                           IndicatorSizeConstraint( mIndicatorMinimumHeight, mIndicatorStartPadding + mIndicatorEndPadding ) );
      mIndicatorSizeConstraint.AddSource( ParentSource( Actor::Property::SIZE ) );
      mIndicatorSizeConstraint.AddSource( Source( scrollableHandle, mPropertyScrollContentSize ) );
      mIndicatorSizeConstraint.Apply();
    }

    if(mIndicatorPositionConstraint)
    {
      mIndicatorPositionConstraint.Remove();
    }

    mIndicatorPositionConstraint = Constraint::New<Vector3>( mIndicator, Actor::Property::POSITION,
                                                             IndicatorPositionConstraint( mIndicatorStartPadding, mIndicatorEndPadding ) );
    mIndicatorPositionConstraint.AddSource( LocalSource( Actor::Property::SIZE ) );
    mIndicatorPositionConstraint.AddSource( ParentSource( Actor::Property::SIZE ) );
    mIndicatorPositionConstraint.AddSource( Source( scrollableHandle, mPropertyScrollPosition ) );
    mIndicatorPositionConstraint.AddSource( Source( scrollableHandle, mPropertyMinScrollPosition ) );
    mIndicatorPositionConstraint.AddSource( Source( scrollableHandle, mPropertyMaxScrollPosition ) );
    mIndicatorPositionConstraint.Apply();
  }
}

void ScrollBar::SetScrollPositionIntervals( const Dali::Vector<float>& positions )
{
  mScrollPositionIntervals = positions;

  Handle scrollableHandle = mScrollableObject.GetBaseHandle();

  if( scrollableHandle )
  {
    if( mPositionNotification )
    {
      scrollableHandle.RemovePropertyNotification(mPositionNotification);
    }

    mPositionNotification = scrollableHandle.AddPropertyNotification( mPropertyScrollPosition, VariableStepCondition(mScrollPositionIntervals) );
    mPositionNotification.NotifySignal().Connect( this, &ScrollBar::OnScrollPositionIntervalReached );
  }
}

Dali::Vector<float> ScrollBar::GetScrollPositionIntervals() const
{
  return mScrollPositionIntervals;
}

void ScrollBar::OnScrollPositionIntervalReached(PropertyNotification& source)
{
  // Emit the signal to notify the scroll position crossing
  Handle scrollableHandle = mScrollableObject.GetBaseHandle();
  if(scrollableHandle)
  {
    mScrollPositionIntervalReachedSignal.Emit(scrollableHandle.GetProperty<float>(mPropertyScrollPosition));
  }
}

void ScrollBar::ShowIndicator()
{
  // Cancel any animation
  if(mAnimation)
  {
    mAnimation.Clear();
    mAnimation.Reset();
  }

  if( mIndicatorFirstShow )
  {
    // Preserve the alpha value from the stylesheet
    mIndicatorShowAlpha = Self().GetCurrentColor().a;
    mIndicatorFirstShow = false;
  }

  if(mIndicatorShowDuration > 0.0f)
  {
    mAnimation = Animation::New( mIndicatorShowDuration );
    mAnimation.AnimateTo( Property( mIndicator, Actor::Property::COLOR_ALPHA ), mIndicatorShowAlpha, AlphaFunction::EASE_IN );
    mAnimation.Play();
  }
  else
  {
    mIndicator.SetOpacity(mIndicatorShowAlpha);
  }
}

void ScrollBar::HideIndicator()
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
    mAnimation.AnimateTo( Property( mIndicator, Actor::Property::COLOR_ALPHA ), 0.0f, AlphaFunction::EASE_IN );
    mAnimation.Play();
  }
  else
  {
    mIndicator.SetOpacity(0.0f);
  }
}

bool ScrollBar::OnPanGestureProcessTick()
{
  // Update the scroll position property.
  Handle scrollableHandle = mScrollableObject.GetBaseHandle();
  if( scrollableHandle )
  {
    scrollableHandle.SetProperty(mPropertyScrollPosition, mCurrentScrollPosition);
  }

  return true;
}

void ScrollBar::OnPan( const PanGesture& gesture )
{
  Handle scrollableHandle = mScrollableObject.GetBaseHandle();

  if(scrollableHandle)
  {
    Dali::Toolkit::ItemView itemView = Dali::Toolkit::ItemView::DownCast(scrollableHandle);

    switch(gesture.state)
    {
      case Gesture::Started:
      {
        if( !mPanProcessTimer )
        {
          // Make sure the pan gesture is only being processed once per frame.
          mPanProcessTimer = Timer::New( DEFAULT_PAN_GESTURE_PROCESS_TIME );
          mPanProcessTimer.TickSignal().Connect( this, &ScrollBar::OnPanGestureProcessTick );
          mPanProcessTimer.Start();
        }

        ShowIndicator();
        mScrollStart = scrollableHandle.GetProperty<float>(mPropertyScrollPosition);
        mGestureDisplacement = Vector3::ZERO;
        mIsPanning = true;

        break;
      }
      case Gesture::Continuing:
      {
        mGestureDisplacement.x += gesture.displacement.x;
        mGestureDisplacement.y += gesture.displacement.y;

        float minScrollPosition = scrollableHandle.GetProperty<float>( mPropertyMinScrollPosition );
        float maxScrollPosition = scrollableHandle.GetProperty<float>( mPropertyMaxScrollPosition );

        // The domain size is the internal range
        float domainSize = maxScrollPosition - minScrollPosition;
        float logicalSize = Self().GetCurrentSize().y - ( mIndicator.GetCurrentSize().y + mIndicatorStartPadding + mIndicatorEndPadding );

        mCurrentScrollPosition = mScrollStart - ( ( mGestureDisplacement.y * domainSize ) / logicalSize );
        mCurrentScrollPosition = -std::min( maxScrollPosition, std::max( -mCurrentScrollPosition, minScrollPosition ) );

        break;
      }
      default:
      {
        mIsPanning = false;

        if( mPanProcessTimer )
        {
          // Destroy the timer when pan gesture is finished.
          mPanProcessTimer.Stop();
          mPanProcessTimer.TickSignal().Disconnect( this, &ScrollBar::OnPanGestureProcessTick );
          mPanProcessTimer.Reset();
        }

        if(itemView)
        {
          // Refresh the ItemView cache with extra items
          GetImpl(itemView).DoRefresh(mCurrentScrollPosition, true);
        }

        mPanFinishedSignal.Emit();

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

void ScrollBar::OnSizeSet( const Vector3& size )
{
  if(mIndicatorHeightPolicy == Toolkit::ScrollBar::Fixed)
  {
    mIndicator.SetSize(size.width, mIndicatorFixedHeight);
  }
}

void ScrollBar::SetScrollDirection( Toolkit::ScrollBar::Direction direction )
{
  mDirection = direction;
}

Toolkit::ScrollBar::Direction ScrollBar::GetScrollDirection() const
{
  return mDirection;
}

void ScrollBar::SetIndicatorHeightPolicy( Toolkit::ScrollBar::IndicatorHeightPolicy policy )
{
  if( policy != mIndicatorHeightPolicy )
  {
    mIndicatorHeightPolicy = policy;
    ApplyConstraints();
  }
}

Toolkit::ScrollBar::IndicatorHeightPolicy ScrollBar::GetIndicatorHeightPolicy() const
{
  return mIndicatorHeightPolicy;
}

void ScrollBar::SetIndicatorFixedHeight( float height )
{
  mIndicatorFixedHeight = height;

  if(mIndicatorHeightPolicy == Toolkit::ScrollBar::Fixed)
  {
    mIndicator.SetSize(Self().GetCurrentSize().width, mIndicatorFixedHeight);
  }
}

float ScrollBar::GetIndicatorFixedHeight() const
{
  return mIndicatorFixedHeight;
}

void ScrollBar::SetIndicatorShowDuration( float durationSeconds )
{
  mIndicatorShowDuration = durationSeconds;
}

float ScrollBar::GetIndicatorShowDuration() const
{
  return mIndicatorShowDuration;
}

void ScrollBar::SetIndicatorHideDuration( float durationSeconds )
{
  mIndicatorHideDuration = durationSeconds;
}

float ScrollBar::GetIndicatorHideDuration() const
{
  return mIndicatorHideDuration;
}

void ScrollBar::OnScrollDirectionPropertySet( Property::Value propertyValue )
{
  std::string directionName( propertyValue.Get<std::string>() );
  if(directionName == "Vertical")
  {
    SetScrollDirection(Toolkit::ScrollBar::Vertical);
  }
  else if(directionName == "Horizontal")
  {
    SetScrollDirection(Toolkit::ScrollBar::Horizontal);
  }
  else
  {
    DALI_ASSERT_ALWAYS( !"ScrollBar::OnScrollDirectionPropertySet(). Invalid Property value." );
  }
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

  if( 0 == strcmp( signalName.c_str(), PAN_FINISHED_SIGNAL ) )
  {
    scrollBar.PanFinishedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SCROLL_POSITION_INTERVAL_REACHED_SIGNAL ) )
  {
    scrollBar.ScrollPositionIntervalReachedSignal().Connect( tracker, functor );
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
      case Toolkit::ScrollBar::Property::SCROLL_DIRECTION:
      {
        scrollBarImpl.OnScrollDirectionPropertySet( value );
        break;
      }
      case Toolkit::ScrollBar::Property::INDICATOR_HEIGHT_POLICY:
      {
        scrollBarImpl.OnIndicatorHeightPolicyPropertySet( value );
        break;
      }
      case Toolkit::ScrollBar::Property::INDICATOR_FIXED_HEIGHT:
      {
        scrollBarImpl.SetIndicatorFixedHeight(value.Get<float>());
        break;
      }
      case Toolkit::ScrollBar::Property::INDICATOR_SHOW_DURATION:
      {
        scrollBarImpl.SetIndicatorShowDuration(value.Get<float>());
        break;
      }
      case Toolkit::ScrollBar::Property::INDICATOR_HIDE_DURATION:
      {
        scrollBarImpl.SetIndicatorHideDuration(value.Get<float>());
        break;
      }
      case Toolkit::ScrollBar::Property::SCROLL_POSITION_INTERVALS:
      {
        Property::Array* array = value.GetArray();
        if( array )
        {
          Dali::Vector<float> positions;
          size_t positionCount = array->Count();
          positions.Resize( positionCount );
          for( size_t i = 0; i != positionCount; ++i )
          {
            array->GetElementAt( i ).Get( positions[i] );
          }

          scrollBarImpl.SetScrollPositionIntervals(positions);
        }
        break;
      }
      case Toolkit::ScrollBar::Property::INDICATOR_MINIMUM_HEIGHT:
      {
        scrollBarImpl.mIndicatorMinimumHeight = value.Get<float>();
        scrollBarImpl.ApplyConstraints();
        break;
      }
      case Toolkit::ScrollBar::Property::INDICATOR_START_PADDING:
      {
        scrollBarImpl.mIndicatorStartPadding = value.Get<float>();
        scrollBarImpl.ApplyConstraints();
        break;
      }
      case Toolkit::ScrollBar::Property::INDICATOR_END_PADDING:
      {
        scrollBarImpl.mIndicatorEndPadding = value.Get<float>();
        scrollBarImpl.ApplyConstraints();
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
      case Toolkit::ScrollBar::Property::SCROLL_DIRECTION:
      {
        value = SCROLL_DIRECTION_NAME[ scrollBarImpl.GetScrollDirection() ];
        break;
      }
      case Toolkit::ScrollBar::Property::INDICATOR_HEIGHT_POLICY:
      {
        value = INDICATOR_HEIGHT_POLICY_NAME[ scrollBarImpl.GetIndicatorHeightPolicy() ];
        break;
      }
      case Toolkit::ScrollBar::Property::INDICATOR_FIXED_HEIGHT:
      {
        value = scrollBarImpl.GetIndicatorFixedHeight();
        break;
      }
      case Toolkit::ScrollBar::Property::INDICATOR_SHOW_DURATION:
      {
        value = scrollBarImpl.GetIndicatorShowDuration();
        break;
      }
      case Toolkit::ScrollBar::Property::INDICATOR_HIDE_DURATION:
      {
        value = scrollBarImpl.GetIndicatorHideDuration();
        break;
      }
      case Toolkit::ScrollBar::Property::SCROLL_POSITION_INTERVALS:
      {
        Property::Value value( Property::ARRAY );
        Property::Array* array = value.GetArray();

        if( array )
        {
          Dali::Vector<float> positions = scrollBarImpl.GetScrollPositionIntervals();
          size_t positionCount( array->Count() );
          for( size_t i( 0 ); i != positionCount; ++i )
          {
            array->PushBack( positions[i] );
          }
        }
        break;
      }
      case Toolkit::ScrollBar::Property::INDICATOR_MINIMUM_HEIGHT:
      {
        value = scrollBarImpl.mIndicatorMinimumHeight;
        break;
      }
      case Toolkit::ScrollBar::Property::INDICATOR_START_PADDING:
      {
        value = scrollBarImpl.mIndicatorStartPadding;
        break;
      }
      case Toolkit::ScrollBar::Property::INDICATOR_END_PADDING:
      {
        value = scrollBarImpl.mIndicatorEndPadding;
        break;
      }
    }
  }
  return value;
}

Toolkit::ScrollBar ScrollBar::New(Toolkit::ScrollBar::Direction direction)
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< ScrollBar > impl = new ScrollBar(direction);

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
