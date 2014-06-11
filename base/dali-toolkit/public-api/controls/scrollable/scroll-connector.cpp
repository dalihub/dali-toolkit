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

#include <dali-toolkit/public-api/controls/scrollable/scroll-connector.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-connector-impl.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

const char* const ScrollConnector::SCROLL_POSITION_PROPERTY_NAME = "scroll-position";
const char* const ScrollConnector::OVERSHOOT_PROPERTY_NAME       = "overshoot";

const Property::Index ScrollConnector::SCROLL_POSITION = Internal::ScrollConnector::SCROLL_POSITION;
const Property::Index ScrollConnector::OVERSHOOT       = Internal::ScrollConnector::OVERSHOOT;

const char* const ScrollConnector::DOMAIN_CHANGED_SIGNAL_NAME    = "domain-changed";

ScrollConnector ScrollConnector::New()
{
  return ScrollConnector( Internal::ScrollConnector::New() );
}

ScrollConnector::ScrollConnector()
{
}

ScrollConnector::ScrollConnector( const ScrollConnector& handle )
: BaseHandle( handle )
{
}

ScrollConnector::~ScrollConnector()
{
}

ScrollConnector ScrollConnector::DownCast( BaseHandle handle )
{
  return ScrollConnector( dynamic_cast<Internal::ScrollConnector*>(handle.GetObjectPtr()) );
}

void ScrollConnector::SetScrollDomain( float min, float max, float length )
{
  GetImpl(*this).SetScrollDomain( min, max, length );
}

float ScrollConnector::GetMinLimit() const
{
  return GetImpl(*this).GetMinLimit();
}

float ScrollConnector::GetMaxLimit() const
{
  return GetImpl(*this).GetMaxLimit();
}

float ScrollConnector::GetContentLength() const
{
  return GetImpl(*this).GetContentLength();
}

Constrainable ScrollConnector::GetScrollPositionObject() const
{
  return GetImpl(*this).GetScrollPositionObject();
}

ScrollConnector::DomainChangedSignalType& ScrollConnector::DomainChangedSignal()
{
  return GetImpl(*this).DomainChangedSignal();
}

ScrollConnector::ScrollConnector( Internal::ScrollConnector* impl )
: BaseHandle( impl )
{
}

} // namespace Toolkit

} // namespace Dali
