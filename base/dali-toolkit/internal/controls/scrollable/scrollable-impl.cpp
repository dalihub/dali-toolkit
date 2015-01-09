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

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scrollable/scrollable-impl.h>
#include <dali-toolkit/internal/controls/scroll-component/scroll-bar-internal-impl.h>

using namespace Dali;

namespace
{

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

const Property::Index Scrollable::PROPERTY_OVERSHOOT_EFFECT_COLOR( Toolkit::Internal::Control::CONTROL_PROPERTY_END_INDEX + 1 );
const Property::Index Scrollable::PROPERTY_OVERSHOOT_ANIMATION_SPEED( Toolkit::Internal::Control::CONTROL_PROPERTY_END_INDEX + 2 );

namespace Internal
{

namespace
{
const Vector4 DEFAULT_OVERSHOOT_COLOUR(0.0f, 0.64f, 0.85f, 0.25f);
const float DEFAULT_OVERSHOOT_ANIMATION_SPEED(120.0f); // 120 pixels per second

BaseHandle Create()
{
  // empty handle as we cannot create Scrollable (but type registered for scroll signal)
  return BaseHandle();
}

TypeRegistration mType( typeid(Toolkit::Scrollable), typeid(Toolkit::Control), Create );

SignalConnectorType s1(mType, Toolkit::Scrollable::SIGNAL_SCROLL_STARTED,   &Scrollable::DoConnectSignal);
SignalConnectorType s2(mType, Toolkit::Scrollable::SIGNAL_SCROLL_COMPLETED, &Scrollable::DoConnectSignal);
SignalConnectorType s3(mType, Toolkit::Scrollable::SIGNAL_SCROLL_UPDATED,   &Scrollable::DoConnectSignal);
SignalConnectorType s4(mType, Toolkit::Scrollable::SIGNAL_SCROLL_CLAMPED,   &Scrollable::DoConnectSignal);

PropertyRegistration property1( mType,
                                "overshoot-effect-color",
                                Toolkit::Scrollable::PROPERTY_OVERSHOOT_EFFECT_COLOR,
                                Property::VECTOR4,
                                &Scrollable::SetProperty,
                                &Scrollable::GetProperty );

PropertyRegistration property2( mType,
                                "overshoot-animation-speed",
                                Toolkit::Scrollable::PROPERTY_OVERSHOOT_ANIMATION_SPEED,
                                Property::FLOAT,
                                &Scrollable::SetProperty,
                                &Scrollable::GetProperty );

}

const std::string Scrollable::SCROLLABLE_CAN_SCROLL_VERTICAL( "scrollable-can-scroll-vertical" );
const std::string Scrollable::SCROLLABLE_CAN_SCROLL_HORIZONTAL( "scrollable-can-scroll-horizontal" );

///////////////////////////////////////////////////////////////////////////////////////////////////
// Scrollable
///////////////////////////////////////////////////////////////////////////////////////////////////

// Scrollable controls are not layout containers so they dont need size negotiation..
// we dont want size negotiation while scrolling if we can avoid it
Scrollable::Scrollable()
: Control( ControlBehaviour( REQUIRES_TOUCH_EVENTS | REQUIRES_STYLE_CHANGE_SIGNALS | NO_SIZE_NEGOTIATION ) ),
  mOvershootEffectColor(  DEFAULT_OVERSHOOT_COLOUR ),
  mOvershootAnimationSpeed ( DEFAULT_OVERSHOOT_ANIMATION_SPEED ),
  mPropertyRelativePosition(Property::INVALID_INDEX),
  mPropertyPositionMin(Property::INVALID_INDEX),
  mPropertyPositionMax(Property::INVALID_INDEX),
  mPropertyScrollDirection(Property::INVALID_INDEX),
  mPropertyCanScrollVertical(Property::INVALID_INDEX),
  mPropertyCanScrollHorizontal(Property::INVALID_INDEX),
  mOvershootEnabled(false)
{
}

Scrollable::~Scrollable()
{
  // Clear scroll components, forces their destruction before Scrollable is destroyed.
  mComponents.clear();
}

void Scrollable::RegisterCommonProperties()
{
  Actor self = Self();

  // Register properties.
  mPropertyRelativePosition = self.RegisterProperty(Toolkit::Scrollable::SCROLL_RELATIVE_POSITION_PROPERTY_NAME, Vector3::ZERO);
  mPropertyPositionMin = self.RegisterProperty(Toolkit::Scrollable::SCROLL_POSITION_MIN_PROPERTY_NAME, Vector3::ZERO);
  mPropertyPositionMax = self.RegisterProperty(Toolkit::Scrollable::SCROLL_POSITION_MAX_PROPERTY_NAME, Vector3::ZERO);
  mPropertyScrollDirection = self.RegisterProperty(Toolkit::Scrollable::SCROLL_DIRECTION_PROPERTY_NAME, Vector3::ZERO);
  mPropertyCanScrollVertical = self.RegisterProperty(SCROLLABLE_CAN_SCROLL_VERTICAL, true);
  mPropertyCanScrollHorizontal = self.RegisterProperty(SCROLLABLE_CAN_SCROLL_HORIZONTAL, true);
}

bool Scrollable::IsScrollComponentEnabled(Toolkit::Scrollable::ScrollComponentType type) const
{
  if(type == Toolkit::Scrollable::OvershootIndicator)
  {
    return mOvershootEnabled;
  }
  return (mComponents.find(type) != mComponents.end());
}

void Scrollable::EnableScrollComponent(Toolkit::Scrollable::ScrollComponentType type)
{
  if(type == Toolkit::Scrollable::OvershootIndicator)
  {
    if( !mOvershootEnabled )
    {
      SetOvershootEnabled(true);
      mOvershootEnabled = true;
    }
    return;
  }
  if( mComponents.find(type) == mComponents.end() )
  {
    // Create ScrollComponent
    Toolkit::Scrollable scrollable = Toolkit::Scrollable::DownCast(Self());
    Toolkit::ScrollComponent scrollComponent = NewScrollComponent(scrollable, type);
    Toolkit::ScrollComponentImpl& component = static_cast<Toolkit::ScrollComponentImpl&>(scrollComponent.GetImplementation());
    ScrollComponentPtr componentPtr(&component);

    mComponents[type] = componentPtr;
  }
}

void Scrollable::DisableScrollComponent(Toolkit::Scrollable::ScrollComponentType type)
{
  if(type == Toolkit::Scrollable::OvershootIndicator)
  {
    if( mOvershootEnabled )
    {
      SetOvershootEnabled(false);
      mOvershootEnabled = false;
    }
    return;
  }
  ComponentIter pair = mComponents.find( type );

  if( mComponents.end() != pair )
  {
    ScrollComponentPtr component = pair->second;

    // Disconnect the scroll component first.
    component->OnDisconnect();

    // Destroy ScrollComponent.
    mComponents.erase( type );
  }
}

Vector4 Scrollable::GetOvershootEffectColor() const
{
  return mOvershootEffectColor;
};

void Scrollable::SetOvershootAnimationSpeed( float pixelsPerSecond )
{
  mOvershootAnimationSpeed = pixelsPerSecond;
}

float Scrollable::GetOvershootAnimationSpeed() const
{
  return mOvershootAnimationSpeed;
};

Toolkit::Scrollable::ScrollStartedSignalV2& Scrollable::ScrollStartedSignal()
{
  return mScrollStartedSignalV2;
}

Toolkit::Scrollable::ScrollUpdatedSignalV2& Scrollable::ScrollUpdatedSignal()
{
  return mScrollUpdatedSignalV2;
}

Toolkit::Scrollable::ScrollCompletedSignalV2& Scrollable::ScrollCompletedSignal()
{
  return mScrollCompletedSignalV2;
}

Toolkit::Scrollable::ScrollClampedSignalV2& Scrollable::ScrollClampedSignal()
{
  return mScrollClampedSignalV2;
}

bool Scrollable::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::Scrollable scrollable = Toolkit::Scrollable::DownCast( handle );

  if( Toolkit::Scrollable::SIGNAL_SCROLL_STARTED == signalName )
  {
    scrollable.ScrollStartedSignal().Connect( tracker, functor );
  }
  else if( Toolkit::Scrollable::SIGNAL_SCROLL_UPDATED == signalName )
  {
    scrollable.ScrollUpdatedSignal().Connect( tracker, functor );
  }
  else if( Toolkit::Scrollable::SIGNAL_SCROLL_COMPLETED == signalName )
  {
    scrollable.ScrollCompletedSignal().Connect( tracker, functor );
  }
  else if( Toolkit::Scrollable::SIGNAL_SCROLL_CLAMPED == signalName )
  {
    scrollable.ScrollClampedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void Scrollable::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::Scrollable scrollable = Toolkit::Scrollable::DownCast( Dali::BaseHandle( object ) );

  if( scrollable )
  {
    Scrollable& scrollableImpl( GetImpl( scrollable ) );
    switch( index )
    {
      case Toolkit::Scrollable::PROPERTY_OVERSHOOT_EFFECT_COLOR:
      {
        scrollableImpl.SetOvershootEffectColor( value.Get<Vector4>() );
        break;
      }
      case Toolkit::Scrollable::PROPERTY_OVERSHOOT_ANIMATION_SPEED:
      {
        scrollableImpl.SetOvershootAnimationSpeed( value.Get<float>() );
        break;
      }
    }
  }
}

Property::Value Scrollable::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::Scrollable scrollable = Toolkit::Scrollable::DownCast( Dali::BaseHandle( object ) );

  if( scrollable )
  {
    Scrollable& scrollableImpl( GetImpl( scrollable ) );
    switch( index )
    {
      case Toolkit::Scrollable::PROPERTY_OVERSHOOT_EFFECT_COLOR:
      {
        value = scrollableImpl.GetOvershootEffectColor();
        break;
      }
      case Toolkit::Scrollable::PROPERTY_OVERSHOOT_ANIMATION_SPEED:
      {
        value = scrollableImpl.GetOvershootAnimationSpeed();
        break;
      }
    }
  }

  return value;
}

Toolkit::ScrollComponent Scrollable::NewScrollComponent(Toolkit::Scrollable& scrollable, Toolkit::Scrollable::ScrollComponentType type)
{
  Toolkit::ScrollComponent instance;

  switch(type)
  {
    case Toolkit::Scrollable::VerticalScrollBar:
    {
      instance = static_cast<Toolkit::ScrollComponent>(Toolkit::ScrollBarInternal::New(scrollable, true));
      break;
    }
    case Toolkit::Scrollable::HorizontalScrollBar:
    {
      instance = static_cast<Toolkit::ScrollComponent>(Toolkit::ScrollBarInternal::New(scrollable, false));
      break;
    }
    case Toolkit::Scrollable::OvershootIndicator:
    {
      DALI_ASSERT_ALWAYS(!"Unrecognized component type");
      break;
    }
  }

  return instance;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
