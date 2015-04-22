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
#include <cstring> // for strcmp
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::ScrollConnector, Dali::BaseHandle, NULL )

DALI_SIGNAL_REGISTRATION( Toolkit, ScrollConnector, "domain-changed",          DOMAIN_CHANGED_SIGNAL_NAME          )
DALI_SIGNAL_REGISTRATION( Toolkit, ScrollConnector, "scroll-position-changed", SCROLL_POSITION_CHANGED_SIGNAL_NAME )

DALI_TYPE_REGISTRATION_END()

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
  mScrollPositionObject = Handle::New();

  mScrollPositionObject.RegisterProperty( Toolkit::ScrollConnector::SCROLL_POSITION_PROPERTY_NAME, 0.0f );
  mScrollPositionObject.RegisterProperty( Toolkit::ScrollConnector::OVERSHOOT_PROPERTY_NAME, 0.0f );
}

ScrollConnector::~ScrollConnector()
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
