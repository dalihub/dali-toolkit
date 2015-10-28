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
#include <cstring> // for strcmp
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/type-registry-helper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scrollable/scrollable-impl.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

BaseHandle Create()
{
  // empty handle as we cannot create Scrollable (but type registered for scroll signal)
  return BaseHandle();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::Scrollable, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, Scrollable, "overshoot-effect-color",    VECTOR4, OVERSHOOT_EFFECT_COLOR    )
DALI_PROPERTY_REGISTRATION( Toolkit, Scrollable, "overshoot-animation-speed", FLOAT,   OVERSHOOT_ANIMATION_SPEED )
const int OVERSHOOT_SIZE = Dali::Toolkit::Scrollable::Property::OVERSHOOT_ANIMATION_SPEED + 1; // OVERSHOOT_SIZE is not public yet
Dali::PropertyRegistration p1( typeRegistration, "overshoot-size", OVERSHOOT_SIZE, Property::VECTOR2, Dali::Toolkit::Internal::Scrollable::SetProperty, Dali::Toolkit::Internal::Scrollable::GetProperty );

DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, Scrollable, "scroll-relative-position", VECTOR2, SCROLL_RELATIVE_POSITION)
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, Scrollable, "scroll-position-min",      VECTOR2, SCROLL_POSITION_MIN)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( Toolkit, Scrollable, "scroll-position-min-x", SCROLL_POSITION_MIN_X, SCROLL_POSITION_MIN, 0)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( Toolkit, Scrollable, "scroll-position-min-y", SCROLL_POSITION_MIN_Y, SCROLL_POSITION_MIN, 1)
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, Scrollable, "scroll-position-max",      VECTOR2, SCROLL_POSITION_MAX)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( Toolkit, Scrollable, "scroll-position-max-x", SCROLL_POSITION_MAX_X, SCROLL_POSITION_MAX, 0)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( Toolkit, Scrollable, "scroll-position-max-y", SCROLL_POSITION_MAX_Y, SCROLL_POSITION_MAX, 1)
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, Scrollable, "can-scroll-vertical",      BOOLEAN, CAN_SCROLL_VERTICAL)
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, Scrollable, "can-scroll-horizontal",    BOOLEAN, CAN_SCROLL_HORIZONTAL)

DALI_SIGNAL_REGISTRATION(              Toolkit, Scrollable, "scroll-started",                    SIGNAL_SCROLL_STARTED    )
DALI_SIGNAL_REGISTRATION(              Toolkit, Scrollable, "scroll-completed",                  SIGNAL_SCROLL_COMPLETED  )
DALI_SIGNAL_REGISTRATION(              Toolkit, Scrollable, "scroll-updated",                    SIGNAL_SCROLL_UPDATED    )

DALI_TYPE_REGISTRATION_END()

const Vector4 DEFAULT_OVERSHOOT_COLOUR(0.0f, 0.64f, 0.85f, 0.25f);
const float DEFAULT_OVERSHOOT_ANIMATION_SPEED(120.0f); // 120 pixels per second
const Vector2 OVERSHOOT_DEFAULT_SIZE( 720.0f, 42.0f );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Scrollable
///////////////////////////////////////////////////////////////////////////////////////////////////

// Scrollable controls are not layout containers so they dont need size negotiation..
// we dont want size negotiation while scrolling if we can avoid it
Scrollable::Scrollable()
: Control( ControlBehaviour( REQUIRES_TOUCH_EVENTS | REQUIRES_STYLE_CHANGE_SIGNALS | DISABLE_SIZE_NEGOTIATION ) ),
  mOvershootEffectColor(  DEFAULT_OVERSHOOT_COLOUR ),
  mOvershootAnimationSpeed ( DEFAULT_OVERSHOOT_ANIMATION_SPEED ),
  mOvershootSize( OVERSHOOT_DEFAULT_SIZE ),
  mOvershootEnabled(true)
{
}

Scrollable::Scrollable( ControlBehaviour behaviourFlags )
: Control( ControlBehaviour( REQUIRES_TOUCH_EVENTS | REQUIRES_STYLE_CHANGE_SIGNALS | behaviourFlags ) ),
  mOvershootEffectColor(  DEFAULT_OVERSHOOT_COLOUR ),
  mOvershootAnimationSpeed ( DEFAULT_OVERSHOOT_ANIMATION_SPEED ),
  mOvershootSize( OVERSHOOT_DEFAULT_SIZE ),
  mOvershootEnabled(true)
{
}

Scrollable::~Scrollable()
{
}

bool Scrollable::IsOvershootEnabled() const
{
  return mOvershootEnabled;
}

void Scrollable::SetOvershootEnabled(bool enable)
{
  EnableScrollOvershoot(enable);
  mOvershootEnabled = enable;
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

const Vector2& Scrollable::GetOvershootSize() const
{
  return mOvershootSize;
}

Toolkit::Scrollable::ScrollStartedSignalType& Scrollable::ScrollStartedSignal()
{
  return mScrollStartedSignal;
}

Toolkit::Scrollable::ScrollUpdatedSignalType& Scrollable::ScrollUpdatedSignal()
{
  return mScrollUpdatedSignal;
}

Toolkit::Scrollable::ScrollCompletedSignalType& Scrollable::ScrollCompletedSignal()
{
  return mScrollCompletedSignal;
}

bool Scrollable::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::Scrollable scrollable = Toolkit::Scrollable::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), SIGNAL_SCROLL_STARTED ) )
  {
    scrollable.ScrollStartedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_SCROLL_UPDATED ) )
  {
    scrollable.ScrollUpdatedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_SCROLL_COMPLETED ) )
  {
    scrollable.ScrollCompletedSignal().Connect( tracker, functor );
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
      case Toolkit::Scrollable::Property::OVERSHOOT_EFFECT_COLOR:
      {
        scrollableImpl.SetOvershootEffectColor( value.Get<Vector4>() );
        break;
      }
      case Toolkit::Scrollable::Property::OVERSHOOT_ANIMATION_SPEED:
      {
        scrollableImpl.SetOvershootAnimationSpeed( value.Get<float>() );
        break;
      }
      case OVERSHOOT_SIZE: // OVERSHOOT_SIZE is not public yet
      {
        Vector2 input;
        if( value.Get( input ) )
        {
          scrollableImpl.mOvershootSize = input;
        }
        scrollableImpl.EnableScrollOvershoot( scrollableImpl.IsOvershootEnabled() );
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
      case Toolkit::Scrollable::Property::OVERSHOOT_EFFECT_COLOR:
      {
        value = scrollableImpl.GetOvershootEffectColor();
        break;
      }
      case Toolkit::Scrollable::Property::OVERSHOOT_ANIMATION_SPEED:
      {
        value = scrollableImpl.GetOvershootAnimationSpeed();
        break;
      }
      case OVERSHOOT_SIZE: // OVERSHOOT_SIZE is not public yet
      {
        value = scrollableImpl.mOvershootSize;
        break;
      }
    }
  }

  return value;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
