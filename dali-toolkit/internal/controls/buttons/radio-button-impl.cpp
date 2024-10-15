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
#include "radio-button-impl.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry.h>

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gLogButtonFilter;
#endif

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
BaseHandle Create()
{
  return Toolkit::RadioButton::New();
}

TypeRegistration typeRegistration(typeid(Toolkit::RadioButton), typeid(Toolkit::Button), Create);

} // namespace

Dali::Toolkit::RadioButton RadioButton::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<RadioButton> internalRadioButton = new RadioButton();

  // Pass ownership to CustomActor
  Dali::Toolkit::RadioButton radioButton(*internalRadioButton);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalRadioButton->Initialize();

  return radioButton;
}

RadioButton::RadioButton()
{
  SetTogglableButton(true);
}

RadioButton::~RadioButton()
{
}

void RadioButton::OnInitialize()
{
  Button::OnInitialize();

  Self().SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::RADIO_BUTTON);
}

DevelControl::ControlAccessible* RadioButton::CreateAccessibleObject()
{
  return new RadioButtonAccessible(Self());
}

bool RadioButton::OnToggleReleased()
{
  // Radio button overrides toggle release (button up) as doesn't allow un-selection to be performed on it directly.
  return false;
}

void RadioButton::OnStateChange(State newState)
{
  // Radio button can be part of a group, if a button in the group is selected then all others should be unselected
  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "RadioButton::OnStateChange state(%d)\n", newState);

  if(SELECTED_STATE == newState)
  {
    Actor parent = Self().GetParent();
    if(parent)
    {
      for(unsigned int i = 0; i < parent.GetChildCount(); ++i)
      {
        Dali::Toolkit::RadioButton radioButtonChild = Dali::Toolkit::RadioButton::DownCast(parent.GetChildAt(i));
        if(radioButtonChild && radioButtonChild != Self())
        {
          radioButtonChild.SetProperty(Toolkit::Button::Property::SELECTED, false);
        }
      }
    }
  }

  // TODO: replace it with OnPropertySet hook once Button::Property::SELECTED will be consistently used
  if((Dali::Accessibility::Accessible::GetCurrentlyHighlightedActor() == Self()) && (newState == SELECTED_STATE || newState == UNSELECTED_STATE))
  {
    auto accessible = GetAccessibleObject();
    if(DALI_LIKELY(accessible))
    {
      accessible->EmitStateChanged(Dali::Accessibility::State::CHECKED, newState == SELECTED_STATE ? 1 : 0, 0);
    }
  }
}

Dali::Accessibility::States RadioButton::RadioButtonAccessible::CalculateStates()
{
  auto state = Button::ButtonAccessible::CalculateStates();
  auto self  = Toolkit::Button::DownCast(Self());

  if(self.GetProperty<bool>(Toolkit::Button::Property::SELECTED))
  {
    state[Dali::Accessibility::State::CHECKED] = true;
  }

  state[Dali::Accessibility::State::SELECTABLE] = true;
  return state;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
