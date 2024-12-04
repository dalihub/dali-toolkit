/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include "check-box-button-impl.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry.h>

//INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/shader-effects/image-region-effect.h>
#include <dali-toolkit/internal/controls/image-view/image-view-impl.h>

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
  return Toolkit::CheckBoxButton::New();
}

TypeRegistration mType(typeid(Toolkit::CheckBoxButton), typeid(Toolkit::Button), Create);

} // namespace

Dali::Toolkit::CheckBoxButton CheckBoxButton::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<CheckBoxButton> internalCheckBoxButton = new CheckBoxButton();

  // Pass ownership to CustomActor
  Dali::Toolkit::CheckBoxButton checkBoxButton(*internalCheckBoxButton);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalCheckBoxButton->Initialize();

  return checkBoxButton;
}

CheckBoxButton::CheckBoxButton()
: Button()
{
  SetTogglableButton(true);
}

CheckBoxButton::~CheckBoxButton()
{
}

void CheckBoxButton::OnInitialize()
{
  Button::OnInitialize();

  Self().SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::CHECK_BOX);
}

DevelControl::ControlAccessible* CheckBoxButton::CreateAccessibleObject()
{
  return new CheckBoxButton::CheckBoxButtonAccessible(Self());
}

Dali::Accessibility::States CheckBoxButton::CheckBoxButtonAccessible::CalculateStates()
{
  auto state = Button::ButtonAccessible::CalculateStates();
  auto self  = Toolkit::Button::DownCast(Self());
  if(self.GetProperty<bool>(Toolkit::Button::Property::SELECTED))
  {
    state[Dali::Accessibility::State::CHECKED] = true;
  }
  return state;
}

void CheckBoxButton::OnStateChange(State newState)
{
  // TODO: replace it with OnPropertySet hook once Button::Property::SELECTED will be consistently used
  if(newState == SELECTED_STATE || newState == UNSELECTED_STATE)
  {
    auto accessible = std::dynamic_pointer_cast<Dali::Accessibility::ActorAccessible>(GetAccessibleObject());
    if(DALI_LIKELY(accessible) && accessible->IsHighlighted())
    {
      accessible->EmitStateChanged(Dali::Accessibility::State::CHECKED, newState == SELECTED_STATE ? 1 : 0, 0);
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
