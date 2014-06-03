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
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

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
