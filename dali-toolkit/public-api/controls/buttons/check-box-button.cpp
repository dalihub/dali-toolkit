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

#include "check-box-button.h"

// INTERNAL INCLUDES

#include <dali-toolkit/internal/controls/buttons/check-box-button-impl.h>

namespace Dali
{

namespace Toolkit
{

const char* const CheckBoxButton::ACTION_CHECK_BOX_BUTTON_CLICK = "check-box-button-click";

CheckBoxButton::CheckBoxButton()
: Button()
{
}

CheckBoxButton::CheckBoxButton( const CheckBoxButton& checkBox )
: Button( checkBox )
{
}

CheckBoxButton& CheckBoxButton::operator=( const CheckBoxButton& checkBox )
{
  if( &checkBox != this )
  {
    Button::operator=( checkBox );
  }
  return *this;
}

CheckBoxButton::~CheckBoxButton()
{
}

CheckBoxButton CheckBoxButton::New()
{
  return Internal::CheckBoxButton::New();
}

CheckBoxButton CheckBoxButton::DownCast( BaseHandle handle )
{
  return Control::DownCast<CheckBoxButton, Internal::CheckBoxButton>(handle);
}

void CheckBoxButton::SetChecked( bool checked )
{
  Dali::Toolkit::GetImplementation( *this ).SetChecked( checked );
}

bool CheckBoxButton::IsChecked() const
{
  return Dali::Toolkit::GetImplementation( *this ).IsChecked();
}

void CheckBoxButton::SetBackgroundImage( Image image )
{
  Dali::Toolkit::GetImplementation( *this ).SetBackgroundImage( image );
}

void CheckBoxButton::SetBackgroundImage( Actor image )
{
  Dali::Toolkit::GetImplementation( *this ).SetBackgroundImage( image );
}

Actor CheckBoxButton::GetBackgroundImage() const
{
  return Dali::Toolkit::GetImplementation( *this ).GetBackgroundImage();
}

void CheckBoxButton::SetCheckedImage( Image image )
{
  Dali::Toolkit::GetImplementation( *this ).SetCheckedImage( image );
}

void CheckBoxButton::SetCheckedImage( Actor image )
{
  Dali::Toolkit::GetImplementation( *this ).SetCheckedImage( image );
}


Actor CheckBoxButton::GetCheckedImage() const
{
  return Dali::Toolkit::GetImplementation( *this ).GetCheckedImage();
}

void CheckBoxButton::SetDisabledBackgroundImage( Image image )
{
  Dali::Toolkit::GetImplementation( *this ).SetDisabledBackgroundImage( image );
}

void CheckBoxButton::SetDisabledBackgroundImage( Actor image )
{
  Dali::Toolkit::GetImplementation( *this ).SetDisabledBackgroundImage( image );
}

Actor CheckBoxButton::GetDisabledBackgroundImage() const
{
  return Dali::Toolkit::GetImplementation( *this ).GetDisabledBackgroundImage();
}

void CheckBoxButton::SetDisabledCheckedImage( Image image )
{
  Dali::Toolkit::GetImplementation( *this ).SetDisabledCheckedImage( image );
}

void CheckBoxButton::SetDisabledCheckedImage( Actor image )
{
  Dali::Toolkit::GetImplementation( *this ).SetDisabledCheckedImage( image );
}

Actor CheckBoxButton::GetDisabledCheckedImage() const
{
  return Dali::Toolkit::GetImplementation( *this ).GetDisabledCheckedImage();
}

CheckBoxButton::CheckBoxButton( Internal::CheckBoxButton& implementation )
: Button( implementation )
{
}

CheckBoxButton::CheckBoxButton( Dali::Internal::CustomActor* internal )
: Button( internal )
{
  VerifyCustomActorPointer<Internal::CheckBoxButton>(internal);
}

} // namespace Toolkit

} // namespace Dali
