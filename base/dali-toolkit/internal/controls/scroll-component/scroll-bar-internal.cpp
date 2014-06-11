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

#include <dali-toolkit/internal/controls/scroll-component/scroll-bar-internal.h>
#include <dali-toolkit/internal/controls/scroll-component/scroll-bar-internal-impl.h>

namespace Dali
{

namespace Toolkit
{

ScrollBarInternal::ScrollBarInternal()
{
}

ScrollBarInternal::ScrollBarInternal(Internal::ScrollBarInternal& implementation)
: ScrollComponent(implementation)
{
}

ScrollBarInternal::ScrollBarInternal( Dali::Internal::CustomActor* internal )
: ScrollComponent( internal )
{
  VerifyCustomActorPointer<Internal::ScrollBarInternal>(internal);
}

ScrollBarInternal::ScrollBarInternal( const ScrollBarInternal& scrollBar )
: ScrollComponent(scrollBar)
{
}

ScrollBarInternal& ScrollBarInternal::operator=( const ScrollBarInternal& scrollBar )
{
  if( &scrollBar != this )
  {
    Control::operator=( scrollBar );
  }
  return *this;
}

ScrollBarInternal ScrollBarInternal::New(Scrollable& container, bool vertical)
{
  return Internal::ScrollBarInternal::New(container, vertical);
}

ScrollBarInternal ScrollBarInternal::DownCast( BaseHandle handle )
{
  return Control::DownCast<ScrollBarInternal, Internal::ScrollBarInternal>(handle);
}

ScrollBarInternal::~ScrollBarInternal()
{
}

void ScrollBarInternal::Show()
{
  GetImpl(*this).Show();
}

void ScrollBarInternal::Hide()
{
  GetImpl(*this).Hide();
}

} // namespace Toolkit

} // namespace Dali
