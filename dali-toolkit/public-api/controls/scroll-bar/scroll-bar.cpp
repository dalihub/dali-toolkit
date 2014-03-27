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

#include <dali-toolkit/public-api/controls/scroll-bar/scroll-bar.h>
#include <dali-toolkit/internal/controls/scroll-bar/scroll-bar-impl.h>

namespace Dali
{

namespace Toolkit
{

const char* const ScrollBar::SCROLL_POSITION_NOTIFIED_SIGNAL_NAME = "scroll-position-notified";

ScrollBar::ScrollBar()
{
}

ScrollBar::ScrollBar(Internal::ScrollBar& implementation)
: Control( implementation )
{
}

ScrollBar::ScrollBar( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::ScrollBar>(internal);
}

ScrollBar::ScrollBar( const ScrollBar& scrollBar )
: Control( scrollBar )
{
}

ScrollBar& ScrollBar::operator=( const ScrollBar& scrollBar )
{
  if( &scrollBar != this )
  {
    Control::operator=( scrollBar );
  }
  return *this;
}

ScrollBar ScrollBar::New()
{
  return Internal::ScrollBar::New();
}

ScrollBar ScrollBar::DownCast( BaseHandle handle )
{
  return Control::DownCast<ScrollBar, Internal::ScrollBar>(handle);
}

ScrollBar::~ScrollBar()
{
}

void ScrollBar::SetScrollConnector( ScrollConnector connector )
{
  GetImpl(*this).SetScrollConnector(connector);
}

void ScrollBar::SetBackgroundImage( Image image, const Vector4& border )
{
  GetImpl(*this).SetBackgroundImage(image, border);
}

void ScrollBar::SetIndicatorImage( Image image, const Vector4& border )
{
  GetImpl(*this).SetIndicatorImage(image, border);
}

Actor ScrollBar::GetScrollIndicator()
{
  return GetImpl(*this).GetScrollIndicator();
}

void ScrollBar::SetPositionNotifications( const std::vector<float>& positions )
{
  GetImpl(*this).SetPositionNotifications(positions);
}

void ScrollBar::Show()
{
  GetImpl(*this).Show();
}

void ScrollBar::Hide()
{
  GetImpl(*this).Hide();
}

ScrollBar::ScrollPositionNotifiedSignalType& ScrollBar::ScrollPositionNotifiedSignal()
{
  return GetImpl(*this).ScrollPositionNotifiedSignal();
}

} // namespace Toolkit

} // namespace Dali
