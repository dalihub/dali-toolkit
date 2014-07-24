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
#include <dali-toolkit/public-api/controls/scrollable/scroll-component.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/scroll-component-impl.h>

namespace Dali
{

namespace Toolkit
{

ScrollComponent::ScrollComponent()
{
}

ScrollComponent::ScrollComponent( ScrollComponentImpl& implementation )
: Control( implementation )
{
}

ScrollComponent::ScrollComponent( Dali::Internal::CustomActor* actor )
: Control( actor )
{
  VerifyCustomActorPointer<ScrollComponentImpl>( actor );
}

ScrollComponent::ScrollComponent( const ScrollComponent& scrollComponent )
: Control( scrollComponent )
{
}

ScrollComponent& ScrollComponent::operator=( const ScrollComponent& scrollComponent )
{
  if( &scrollComponent != this )
  {
    Control::operator=( scrollComponent );
  }
  return *this;
}

ScrollComponent ScrollComponent::DownCast( BaseHandle handle )
{
  return Control::DownCast<ScrollComponent, ScrollComponentImpl>(handle);
}

ScrollComponent::~ScrollComponent()
{
}

void ScrollComponent::SetScrollConnector( ScrollConnector connector )
{
  GetImpl(*this).SetScrollConnector(connector);
}

ScrollConnector ScrollComponent::GetScrollConnector() const
{
  return GetImpl(*this).GetScrollConnector();
}

} // namespace Toolkit

} // namespace Dali
