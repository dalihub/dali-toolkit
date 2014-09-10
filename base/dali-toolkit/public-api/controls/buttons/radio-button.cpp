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

#include <dali-toolkit/public-api/controls/buttons/radio-button.h>

// INTERNAL INCLUDES

#include <dali-toolkit/internal/controls/buttons/radio-button-impl.h>

namespace Dali
{

namespace Toolkit
{

RadioButton::RadioButton()
  : Button()
{
}

RadioButton::RadioButton(Internal::RadioButton& implementation)
  : Button(implementation)
{
}

RadioButton::RadioButton(const RadioButton& radioButton)
  : Button(radioButton)
{
}

RadioButton& RadioButton::operator=(const RadioButton& radioButton )
{
  if( &radioButton != this )
  {
    Button::operator=( radioButton );
  }
  return *this;
}

RadioButton::RadioButton(Dali::Internal::CustomActor* internal)
  : Button(internal)
{
  VerifyCustomActorPointer<Internal::RadioButton>( internal );
}

RadioButton::~RadioButton()
{
}

RadioButton RadioButton::New()
{
  return Internal::RadioButton::New();
}

RadioButton RadioButton::New(const std::string& label)
{
  RadioButton radioButton = Internal::RadioButton::New();
  radioButton.SetLabel(label);
  return radioButton;
}

RadioButton RadioButton::New(Actor label)
{
  RadioButton radioButton = Internal::RadioButton::New();
  radioButton.SetLabel(label);
  return radioButton;
}

RadioButton RadioButton::DownCast(BaseHandle handle)
{
  return Control::DownCast<RadioButton, Internal::RadioButton>( handle );
}

void RadioButton::SetLabel(const std::string& label)
{
  Dali::Toolkit::GetImplementation(*this).SetLabel(label);
}

void RadioButton::SetLabel(Actor label)
{
  Dali::Toolkit::GetImplementation(*this).SetLabel(label);
}

Actor RadioButton::GetLabel() const
{
  return Dali::Toolkit::GetImplementation(*this).GetLabel();
}

void RadioButton::SetActive(bool active)
{
  Dali::Toolkit::GetImplementation(*this).SetActive(active);
}

bool RadioButton::IsActive()const
{
  return Dali::Toolkit::GetImplementation(*this).IsActive();
}

void RadioButton::ToggleState()
{
  Dali::Toolkit::GetImplementation(*this).ToggleState();
}

} // namespace Toolkit

} // namespace Dali
