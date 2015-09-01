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
#include <dali-toolkit/devel-api/controls/popup/popup.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/popup/popup-impl.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// Popup
///////////////////////////////////////////////////////////////////////////////////////////////////

Popup::Popup()
{
}

Popup::Popup( const Popup& handle )
: Control( handle )
{
}

Popup& Popup::operator=( const Popup& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

Popup::Popup(Internal::Popup& implementation)
: Control(implementation)
{
}

Popup::Popup( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::Popup>(internal);
}

Popup Popup::New()
{
  return Internal::Popup::New();
}

Popup::~Popup()
{
}

Popup Popup::DownCast( BaseHandle handle )
{
  return Control::DownCast<Popup, Internal::Popup>(handle);
}

// Properties:

void Popup::SetTitle( Actor titleActor )
{
  GetImpl( *this ).SetTitle( titleActor );
}

Actor Popup::GetTitle() const
{
  return GetImpl( *this ).GetTitle();
}

void Popup::SetContent( Actor content )
{
  GetImpl( *this ).SetContent( content );
}

Actor Popup::GetContent() const
{
  return GetImpl( *this ).GetContent();
}

void Popup::SetFooter( Actor footer )
{
  GetImpl( *this ).SetFooter( footer );
}

Actor Popup::GetFooter() const
{
  return GetImpl( *this ).GetFooter();
}

void Popup::SetDisplayState( Toolkit::Popup::DisplayState displayState )
{
  GetImpl( *this ).SetDisplayState( displayState );
}

Toolkit::Popup::DisplayState Popup::GetDisplayState() const
{
  return GetImpl( *this ).GetDisplayState();
}

// Signals:

Popup::TouchedOutsideSignalType& Popup::OutsideTouchedSignal()
{
  return GetImpl( *this ).OutsideTouchedSignal();
}

Popup::DisplayStateChangeSignalType& Popup::ShowingSignal()
{
  return GetImpl( *this ).ShowingSignal();
}

Popup::DisplayStateChangeSignalType& Popup::ShownSignal()
{
  return GetImpl( *this ).ShownSignal();
}

Popup::DisplayStateChangeSignalType& Popup::HidingSignal()
{
  return GetImpl( *this ).HidingSignal();
}

Popup::DisplayStateChangeSignalType& Popup::HiddenSignal()
{
  return GetImpl( *this ).HiddenSignal();
}

} // namespace Toolkit

} // namespace Dali
