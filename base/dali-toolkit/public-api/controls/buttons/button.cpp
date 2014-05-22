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

const char* const Button::SIGNAL_CLICKED = "clicked";

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

void Button::SetDimmed( bool dimmed )
{
  Dali::Toolkit::GetImplementation( *this ).SetDimmed( dimmed );
}

bool Button::IsDimmed() const
{
  return Dali::Toolkit::GetImplementation( *this ).IsDimmed();
}

void Button::SetAnimationTime( float animationTime )
{
  Dali::Toolkit::GetImplementation( *this ).SetAnimationTime( animationTime );
}

float Button::GetAnimationTime() const
{
  return Dali::Toolkit::GetImplementation( *this ).GetAnimationTime();
}

Button::ClickedSignalV2& Button::ClickedSignal()
{
  return Dali::Toolkit::GetImplementation( *this ).ClickedSignal();
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
