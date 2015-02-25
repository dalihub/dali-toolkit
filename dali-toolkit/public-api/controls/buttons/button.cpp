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

#include <dali-toolkit/public-api/controls/buttons/button.h>

// INTERNAL INCLUDES

#include <dali-toolkit/internal/controls/buttons/button-impl.h>

namespace Dali
{

namespace Toolkit
{

Button::Button()
{}

Button::Button( const Button& button )
: Control( button )
{
}

Button& Button::operator=( const Button& button )
{
  if( &button != this )
  {
    Control::operator=( button );
  }
  return *this;
}

Button::~Button()
{
}

Button Button::DownCast( BaseHandle handle )
{
  return Control::DownCast<Button, Internal::Button>(handle);
}

void Button::SetDisabled( bool disabled )
{
  Dali::Toolkit::GetImplementation( *this ).SetDisabled( disabled );
}

bool Button::IsDisabled() const
{
  return Dali::Toolkit::GetImplementation( *this ).IsDisabled();
}

void Button::SetAutoRepeating( bool autoRepeating )
{
  Dali::Toolkit::GetImplementation( *this ).SetAutoRepeating( autoRepeating );
}

bool Button::IsAutoRepeating() const
{
  return Dali::Toolkit::GetImplementation( *this ).IsAutoRepeating();
}

void Button::SetInitialAutoRepeatingDelay( float initialAutoRepeatingDelay )
{
  Dali::Toolkit::GetImplementation( *this ).SetInitialAutoRepeatingDelay( initialAutoRepeatingDelay );
}

float Button::GetInitialAutoRepeatingDelay() const
{
  return Dali::Toolkit::GetImplementation( *this ).GetInitialAutoRepeatingDelay();
}

void Button::SetNextAutoRepeatingDelay( float nextAutoRepeatingDelay )
{
  Dali::Toolkit::GetImplementation( *this ).SetNextAutoRepeatingDelay( nextAutoRepeatingDelay );
}

float Button::GetNextAutoRepeatingDelay() const
{
  return Dali::Toolkit::GetImplementation( *this ).GetNextAutoRepeatingDelay();
}

void Button::SetTogglableButton( bool togglable )
{
  Dali::Toolkit::GetImplementation( *this ).SetTogglableButton( togglable );
}

bool Button::IsTogglableButton() const
{
  return Dali::Toolkit::GetImplementation( *this ).IsTogglableButton();
}

void Button::SetSelected( bool selected )
{
  Dali::Toolkit::GetImplementation( *this ).SetSelected( selected );
}

bool Button::IsSelected() const
{
  return Dali::Toolkit::GetImplementation( *this ).IsSelected();
}

void Button::SetAnimationTime( float animationTime )
{
  Dali::Toolkit::GetImplementation( *this ).SetAnimationTime( animationTime );
}

float Button::GetAnimationTime() const
{
  return Dali::Toolkit::GetImplementation( *this ).GetAnimationTime();
}

void Button::SetLabel( const std::string& label )
{
  Dali::Toolkit::GetImplementation( *this ).SetLabel( label );
}

void Button::SetLabel( Actor label )
{
  Dali::Toolkit::GetImplementation( *this ).SetLabel( label );
}

Actor Button::GetLabel() const
{
  return Dali::Toolkit::GetImplementation( *this ).GetLabel();
}

Button::ButtonSignalType& Button::PressedSignal()
{
  return Dali::Toolkit::GetImplementation( *this ).PressedSignal();
}

Button::ButtonSignalType& Button::ReleasedSignal()
{
  return Dali::Toolkit::GetImplementation( *this ).ReleasedSignal();
}

Button::ButtonSignalType& Button::ClickedSignal()
{
  return Dali::Toolkit::GetImplementation( *this ).ClickedSignal();
}

Button::ButtonSignalType& Button::StateChangedSignal()
{
  return Dali::Toolkit::GetImplementation( *this ).StateChangedSignal();
}

Button::Button( Internal::Button& implementation )
: Control( implementation )
{
}

Button::Button( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::Button>(internal);
}

} // namespace Toolkit

} // namespace Dali
