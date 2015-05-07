/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/scroll-bar/scroll-bar.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scroll-bar/scroll-bar-impl.h>

namespace Dali
{

namespace Toolkit
{

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

ScrollBar::ScrollBar( const ScrollBar& handle )
: Control( handle )
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

ScrollBar ScrollBar::New(ScrollBar::Direction direction)
{
  return Internal::ScrollBar::New(direction);
}

ScrollBar ScrollBar::DownCast( BaseHandle handle )
{
  return Control::DownCast<ScrollBar, Internal::ScrollBar>(handle);
}

ScrollBar::~ScrollBar()
{
}

void ScrollBar::SetScrollPropertySource( Handle handle, Dali::Property::Index propertyScrollPosition, Dali::Property::Index propertyMinScrollPosition, Dali::Property::Index propertyMaxScrollPosition, Dali::Property::Index propertyScrollContentSize )
{
  GetImpl(*this).SetScrollPropertySource(handle, propertyScrollPosition, propertyMinScrollPosition, propertyMaxScrollPosition, propertyScrollContentSize);
}

void ScrollBar::SetScrollIndicator( Actor indicator )
{
  GetImpl(*this).SetScrollIndicator(indicator);
}

Actor ScrollBar::GetScrollIndicator()
{
  return GetImpl(*this).GetScrollIndicator();
}

void ScrollBar::SetScrollPositionIntervals( const Dali::Vector<float>& positions )
{
  GetImpl(*this).SetScrollPositionIntervals(positions);
}

Dali::Vector<float> ScrollBar::GetScrollPositionIntervals() const
{
  return GetImpl(*this).GetScrollPositionIntervals();
}

void ScrollBar::SetScrollDirection( ScrollBar::Direction direction )
{
  GetImpl(*this).SetScrollDirection(direction);
}

ScrollBar::Direction ScrollBar::GetScrollDirection() const
{
  return GetImpl(*this).GetScrollDirection();
}

void ScrollBar::SetIndicatorHeightPolicy( ScrollBar::IndicatorHeightPolicy policy )
{
  GetImpl(*this).SetIndicatorHeightPolicy(policy);
}

ScrollBar::IndicatorHeightPolicy ScrollBar::GetIndicatorHeightPolicy() const
{
  return GetImpl(*this).GetIndicatorHeightPolicy();
}

void ScrollBar::SetIndicatorFixedHeight( float height )
{
  GetImpl(*this).SetIndicatorFixedHeight(height);
}

float ScrollBar::GetIndicatorFixedHeight() const
{
  return GetImpl(*this).GetIndicatorFixedHeight();
}

void ScrollBar::SetIndicatorShowDuration( float durationSeconds )
{
  GetImpl(*this).SetIndicatorShowDuration(durationSeconds);
}

float ScrollBar::GetIndicatorShowDuration() const
{
  return GetImpl(*this).GetIndicatorShowDuration();
}

void ScrollBar::SetIndicatorHideDuration( float durationSeconds )
{
  GetImpl(*this).SetIndicatorHideDuration(durationSeconds);
}

float ScrollBar::GetIndicatorHideDuration() const
{
  return GetImpl(*this).GetIndicatorHideDuration();
}

void ScrollBar::ShowIndicator()
{
  GetImpl(*this).ShowIndicator();
}

void ScrollBar::HideIndicator()
{
  GetImpl(*this).HideIndicator();
}

ScrollBar::PanFinishedSignalType& ScrollBar::PanFinishedSignal()
{
  return GetImpl(*this).PanFinishedSignal();
}

ScrollBar::ScrollPositionIntervalReachedSignalType& ScrollBar::ScrollPositionIntervalReachedSignal()
{
  return GetImpl(*this).ScrollPositionIntervalReachedSignal();
}

} // namespace Toolkit

} // namespace Dali
