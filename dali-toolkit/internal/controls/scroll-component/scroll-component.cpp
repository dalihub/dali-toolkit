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

#include <dali-toolkit/internal/controls/scroll-component/scroll-component.h>
#include <dali-toolkit/internal/controls/scroll-component/scroll-component-impl.h>

namespace Dali
{

namespace Toolkit
{

ScrollComponent::ScrollComponent()
{
}

ScrollComponent::ScrollComponent(Internal::ScrollComponent& implementation)
: Control(implementation)
{
}

ScrollComponent::ScrollComponent( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::ScrollComponent>(internal);
}

ScrollComponent::ScrollComponent( const ScrollComponent& scrollComponentNew )
: Control(scrollComponentNew)
{
}

ScrollComponent& ScrollComponent::operator=( const ScrollComponent& scrollComponentNew )
{
  if( &scrollComponentNew != this )
  {
    Control::operator=( scrollComponentNew );
  }
  return *this;
}

ScrollComponent ScrollComponent::DownCast( BaseHandle handle )
{
  return Control::DownCast<ScrollComponent, Internal::ScrollComponent>(handle);
}

ScrollComponent::~ScrollComponent()
{
}

} // namespace Toolkit

} // namespace Dali
