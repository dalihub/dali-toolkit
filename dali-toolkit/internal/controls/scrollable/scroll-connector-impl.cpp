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
#include <dali-toolkit/internal/controls/scrollable/scroll-connector-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-index.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

// Signals

const char* const DOMAIN_CHANGED_SIGNAL_NAME =          "domain-changed";
const char* const SCROLL_POSITION_CHANGED_SIGNAL_NAME = "scroll-position-changed";

TypeRegistration typeRegistration( typeid( Toolkit::ScrollConnector ), typeid( Dali::BaseHandle ), NULL );

SignalConnectorType signalConnector1( typeRegistration, DOMAIN_CHANGED_SIGNAL_NAME , &ScrollConnector::DoConnectSignal );
SignalConnectorType signalConnector2( typeRegistration, SCROLL_POSITION_CHANGED_SIGNAL_NAME , &ScrollConnector::DoConnectSignal );

}

const Property::Index ScrollConnector::SCROLL_POSITION = Dali::PROPERTY_CUSTOM_START_INDEX;
const Property::Index ScrollConnector::OVERSHOOT       = Dali::PROPERTY_CUSTOM_START_INDEX + 1;

ScrollConnector* ScrollConnector::New()
{
  return new ScrollConnector();
}

void ScrollConnector::SetScrollDomain( float min, float max, float length )
{
  mMinLimit = min;
  mMaxLimit = max;
  mContentLength = length;

  mDomainChangedSignal.Emit( mMinLimit, mMaxLimit, mContentLength );
}

void ScrollConnector::SetScrollPosition( float position )
{
  mScrollPositionObject.SetProperty( Toolkit::ScrollConnector::SCROLL_POSITION, position );
  mScrollPositionChangedSignal.Emit( position );
}

bool ScrollConnector::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::ScrollConnector scrollConnector = Toolkit::ScrollConnector::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), DOMAIN_CHANGED_SIGNAL_NAME ) )
  {
    scrollConnector.DomainChangedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SCROLL_POSITION_CHANGED_SIGNAL_NAME ) )
  {
    scrollConnector.ScrollPositionChangedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

ScrollConnector::ScrollConnector()
: mMinLimit( 0.0f ),
  mMaxLimit( 0.0f ),
  mContentLength( 0.0f )
{
  mScrollPositionObject = Constrainable::New();

  mScrollPositionObject.RegisterProperty( Toolkit::ScrollConnector::SCROLL_POSITION_PROPERTY_NAME, 0.0f );
  mScrollPositionObject.RegisterProperty( Toolkit::ScrollConnector::OVERSHOOT_PROPERTY_NAME, 0.0f );
}

ScrollConnector::~ScrollConnector()
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
