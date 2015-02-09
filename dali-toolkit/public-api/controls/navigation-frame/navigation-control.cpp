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
#include "navigation-control.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/navigation-frame/navigation-control-impl.h>

namespace Dali
{

namespace Toolkit
{

const char* const NavigationControl::ACTION_PUSH = "push";
const char* const NavigationControl::ACTION_POP = "pop";

NavigationControl::NavigationControl()
{
}

NavigationControl::NavigationControl( const NavigationControl& handle )
: Control(handle)
{
}

NavigationControl& NavigationControl::operator=( const NavigationControl& handle)
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

NavigationControl::~NavigationControl()
{
}

NavigationControl NavigationControl::New()
{
  return Internal::NavigationControl::New();
}

NavigationControl NavigationControl::DownCast( BaseHandle handle )
{
  return Control::DownCast<NavigationControl, Internal::NavigationControl>(handle);
}

NavigationControl::NavigationControl( Internal::NavigationControl& implementation )
: Control( implementation )
{
}

NavigationControl::NavigationControl( Dali::Internal::CustomActor* internal )
: Control( internal)
{
  VerifyCustomActorPointer<Internal::NavigationControl>(internal);
}


void NavigationControl::PushItem( Page item )
{
  GetImpl( *this ).PushItem( item );
}

Page NavigationControl::PopItem()
{
  return GetImpl( *this ).PopItem();
}

size_t NavigationControl::GetItemCount() const
{
  return GetImpl( *this ).GetItemCount();
}

Page NavigationControl::GetItem(std::size_t index) const
{
  return GetImpl( *this ).GetItem( index );
}

Page NavigationControl::GetCurrentItem() const
{
  return GetImpl(*this ).GetCurrentItem();
}

void NavigationControl::SetBackground( Actor background)
{
  GetImpl( *this ).SetBackground( background );
}

void NavigationControl::CreateNavigationToolBar( NaviToolBarStyle toolBarStylePortrait, NaviToolBarStyle toolBarStyleLandscape )
{
  GetImpl( *this ).CreateNavigationToolBar( toolBarStylePortrait, toolBarStyleLandscape );
}

void NavigationControl::CreateNavigationTitleBar( NaviTitleBarStyle titleBarStylePortrait, NaviTitleBarStyle titleBarStyleLandscape )
{
  GetImpl( *this ).CreateNavigationTitleBar( titleBarStylePortrait, titleBarStyleLandscape );
}

void NavigationControl::OrientationChanged( int angle )
{
  GetImpl( *this ).OrientationChanged( angle );
}

void NavigationControl::SetOrientationRotateAnimation( float duration, AlphaFunction alphaFunc)
{
  GetImpl( *this ).SetOrientationRotateAnimation( duration, alphaFunc );
}

NavigationControl::ItemPushedSignalType& NavigationControl::ItemPushedSignal()
{
  return GetImpl( *this ).ItemPushedSignal();
}

NavigationControl::ItemPoppedSignalType& NavigationControl::ItemPoppedSignal()
{
  return GetImpl( *this ).ItemPoppedSignal();
}

} // namespace Toolkit

} // namespace Dali
