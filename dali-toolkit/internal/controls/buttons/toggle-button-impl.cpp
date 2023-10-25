/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include "toggle-button-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/tooltip/tooltip-properties.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/public-api/align-enumerations.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/visuals/text-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

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
  return Toolkit::ToggleButton::New();
}

// Properties
DALI_TYPE_REGISTRATION_BEGIN(Toolkit::ToggleButton, Toolkit::Button, Create)

DALI_PROPERTY_REGISTRATION(Toolkit, ToggleButton, "stateVisuals", ARRAY, STATE_VISUALS)
DALI_PROPERTY_REGISTRATION(Toolkit, ToggleButton, "tooltips", ARRAY, TOOLTIPS)
DALI_PROPERTY_REGISTRATION(Toolkit, ToggleButton, "currentStateIndex", INTEGER, CURRENT_STATE_INDEX)

DALI_TYPE_REGISTRATION_END()

} // unnamed namespace

Dali::Toolkit::ToggleButton ToggleButton::New()
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "ToggleButton::New\n");
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<ToggleButton> internalToggleButton = new ToggleButton();

  // Pass ownership to CustomActor
  Dali::Toolkit::ToggleButton toggleButton(*internalToggleButton);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalToggleButton->Initialize();

  return toggleButton;
}

ToggleButton::ToggleButton()
: Button(),
  mToggleStates(),
  mToggleVisuals(),
  mToggleSelectedVisuals(),
  mToggleDisabledVisuals(),
  mToggleDisabledSelectedVisuals(),
  mToggleTooltips(),
  mCurrentToggleIndex(0)
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "ToggleButton::Constructor\n");
  SetTogglableButton(false);
}

ToggleButton::~ToggleButton()
{
}

void ToggleButton::OnInitialize()
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "ToggleButton::OnInitialize\n");
  Button::OnInitialize();

  // Toggle button requires the Leave event.
  Actor self = Self();
  self.SetProperty(Actor::Property::LEAVE_REQUIRED, true);

  self.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::TOGGLE_BUTTON);
}

DevelControl::ControlAccessible* ToggleButton::CreateAccessibleObject()
{
  return new ToggleButtonAccessible(Self());
}

void ToggleButton::SetProperty(BaseObject* object, Property::Index propertyIndex, const Property::Value& value)
{
  Toolkit::ToggleButton toggleButton = Toolkit::ToggleButton::DownCast(Dali::BaseHandle(object));

  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "ToggleButton::SetProperty index[%d]\n", propertyIndex);

  if(toggleButton)
  {
    ToggleButton& toggleButtonImpl(GetImplementation(toggleButton));

    switch(propertyIndex)
    {
      case Toolkit::ToggleButton::Property::STATE_VISUALS:
      {
        Property::Array stateArray;
        if(value.Get(stateArray))
        {
          toggleButtonImpl.SetToggleStates(stateArray);
        }

        break;
      }
      case Toolkit::ToggleButton::Property::TOOLTIPS:
      {
        const Property::Array* tipArray = value.GetArray();
        if(tipArray)
        {
          std::vector<std::string> tips;
          size_t                   tipsCount = tipArray->Count();
          tips.resize(tipsCount);
          for(size_t i = 0; i != tipsCount; ++i)
          {
            tipArray->GetElementAt(i).Get(tips[i]);
          }
          toggleButtonImpl.SetToggleTooltips(tips);
        }
        break;
      }
      default:
      {
        break;
      }
    } // end of switch
  }
}

Property::Value ToggleButton::GetProperty(BaseObject* object, Property::Index propertyIndex)
{
  Property::Value value;

  Toolkit::ToggleButton toggleButton = Toolkit::ToggleButton::DownCast(Dali::BaseHandle(object));

  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "ToggleButton::GetProperty index[%d]\n", propertyIndex);

  if(toggleButton)
  {
    ToggleButton& toggleButtonImpl(GetImplementation(toggleButton));

    switch(propertyIndex)
    {
      case Toolkit::ToggleButton::Property::STATE_VISUALS:
      {
        value = toggleButtonImpl.GetToggleStates();
        break;
      }
      case Toolkit::ToggleButton::Property::TOOLTIPS:
      {
        Property::Value  value1(Property::ARRAY);
        Property::Array* tipArray = value1.GetArray();

        if(tipArray)
        {
          std::vector<std::string> tips = toggleButtonImpl.GetToggleTooltips();
          size_t                   tipsCount(tips.size());
          for(size_t i(0); i != tipsCount; ++i)
          {
            tipArray->PushBack(tips[i]);
          }
        }
        value = value1;
        break;
      }
      case Toolkit::ToggleButton::Property::CURRENT_STATE_INDEX:
      {
        value = static_cast<int>(toggleButtonImpl.mCurrentToggleIndex);
        break;
      }
    } // end of switch
  }

  return value;
}

void ToggleButton::CreateVisualsForAllStates(const Property::Array& states, std::vector<Toolkit::Visual::Base>& visuals)
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "ToggleButton::CreateVisualsForAllStates\n");

  visuals.clear();

  for(unsigned int i = 0; i < states.Count(); i++)
  {
    Property::Value value(states[i]);

    Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
    Toolkit::Visual::Base  stateVisual;

    if(value.GetType() == Property::MAP)
    {
      Property::Map* map = value.GetMap();
      if(map && !map->Empty()) // Empty map results in current visual removal.
      {
        DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "ToggleButton::CreateVisuals Using Map\n");
        stateVisual = visualFactory.CreateVisual(*map);
      }
    }
    else if(value.GetType() == Property::STRING)
    {
      std::string imageUrl = value.Get<std::string>();
      DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "ToggleButton::CreateVisuals Using image URL\n");
      if(!imageUrl.empty())
      {
        stateVisual = visualFactory.CreateVisual(imageUrl, ImageDimensions());
      }
    }

    if(stateVisual)
    {
      stateVisual.SetDepthIndex(DepthIndex::CONTENT);
      visuals.push_back(stateVisual);
    }
  } // end of for
  DALI_LOG_INFO(gLogButtonFilter, Debug::Verbose, "ToggleButton::CreateVisuals mToggleVisuals:%d\n", mToggleVisuals.size());
}

void ToggleButton::SetToggleStates(const Property::Array& states)
{ //this should really be generalized to be either string or maps so that any visual can be created.
  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "ToggleButton::SetToggleStates\n");
  if(!states.Empty())
  {
    mToggleStates.Clear();
    mToggleStates = states;
    /* New toggle button index from 0. */
    mCurrentToggleIndex = 0;

    // Create all visuals, save to mToggleVisuals.
    CreateVisualsForAllStates(states, mToggleVisuals);
    CreateVisualsForAllStates(states, mToggleSelectedVisuals);
    CreateVisualsForAllStates(states, mToggleDisabledVisuals);
    CreateVisualsForAllStates(states, mToggleDisabledSelectedVisuals);

    DALI_LOG_INFO(gLogButtonFilter, Debug::General, "ToggleButton::Began to register visual.\n");

    PrepareVisual(Toolkit::Button::Property::UNSELECTED_VISUAL, mToggleVisuals[mCurrentToggleIndex]);
    PrepareVisual(Toolkit::Button::Property::SELECTED_VISUAL, mToggleSelectedVisuals[mCurrentToggleIndex]);
    PrepareVisual(Toolkit::Button::Property::DISABLED_UNSELECTED_VISUAL, mToggleDisabledVisuals[mCurrentToggleIndex]);
    PrepareVisual(Toolkit::Button::Property::DISABLED_SELECTED_VISUAL, mToggleDisabledSelectedVisuals[mCurrentToggleIndex]);

    RelayoutRequest();
  }
}

Property::Array ToggleButton::GetToggleStates() const
{
  return mToggleStates;
}

void ToggleButton::SetToggleTooltips(std::vector<std::string>& tips)
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "ToggleButton::SetToggleTooltips\n");
  if(!tips.empty())
  {
    mToggleTooltips.clear();
    mToggleTooltips.swap(tips);
  }

  if(!mToggleTooltips.empty() && (mCurrentToggleIndex < mToggleTooltips.size()))
  {
    Self().SetProperty(Toolkit::DevelControl::Property::TOOLTIP, mToggleTooltips[mCurrentToggleIndex]);
  }

  RelayoutRequest();
}

const std::vector<std::string>& ToggleButton::GetToggleTooltips() const
{
  return mToggleTooltips;
}

void ToggleButton::PrepareVisual(Property::Index index, Toolkit::Visual::Base& visual)
{
  bool enabled = false; // Disabled by default

  // Unregister the visual with the given index if registered previously
  if(DevelControl::GetVisual(*this, index))
  {
    // Check whether it was enabled to ensure we do the same with the new visual we're registering
    enabled = DevelControl::IsVisualEnabled(*this, index);
    DevelControl::UnregisterVisual(*this, index);
  }

  DevelControl::RegisterVisual(*this, index, visual, enabled);
}

void ToggleButton::RelayoutVisual(Property::Index index, const Vector2& size)
{
  Toolkit::Visual::Base visual = DevelControl::GetVisual(*this, index);
  if(visual)
  {
    Size    visualSize     = Size::ZERO;
    Vector2 visualPosition = Vector2::ZERO;

    visual.GetNaturalSize(visualSize);

    DALI_LOG_INFO(gLogButtonFilter, Debug::General, "ToggleButton::OnRelayout Setting visual size to(%f,%f)\n", visualSize.width, visualSize.height);
    DALI_LOG_INFO(gLogButtonFilter, Debug::General, "ToggleButton::OnRelayout Setting visual position to(%f,%f)\n", visualPosition.x, visualPosition.y);

    Property::Map visualTransform;
    visualTransform.Add(Toolkit::Visual::Transform::Property::SIZE, visualSize)
      .Add(Toolkit::Visual::Transform::Property::OFFSET, visualPosition)
      .Add(Toolkit::Visual::Transform::Property::OFFSET_POLICY, Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE))
      .Add(Toolkit::Visual::Transform::Property::SIZE_POLICY, Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE))
      .Add(Toolkit::Visual::Transform::Property::ORIGIN, Toolkit::Align::CENTER)
      .Add(Toolkit::Visual::Transform::Property::ANCHOR_POINT, Toolkit::Align::CENTER);

    visual.SetTransformAndSize(visualTransform, size);
  }
}

void ToggleButton::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "ToggleButton::OnRelayout targetSize(%f,%f) ptr(%p)\n", size.width, size.height, this);

  RelayoutVisual(Toolkit::Button::Property::UNSELECTED_VISUAL, size);
  RelayoutVisual(Toolkit::Button::Property::SELECTED_VISUAL, size);
  RelayoutVisual(Toolkit::Button::Property::DISABLED_UNSELECTED_VISUAL, size);
  RelayoutVisual(Toolkit::Button::Property::DISABLED_SELECTED_VISUAL, size);
}

void ToggleButton::OnPressed()
{
  DALI_LOG_INFO(gLogButtonFilter, Debug::General, "ToggleButton::OnPressed\n");
  // State index will add 1 only when button is pressed.
  mCurrentToggleIndex = (mCurrentToggleIndex + 1) % static_cast<uint32_t>(mToggleVisuals.size());

  // Both create SelectedVisual and UnselectedVisual
  PrepareVisual(Toolkit::Button::Property::UNSELECTED_VISUAL, mToggleVisuals[mCurrentToggleIndex]);
  PrepareVisual(Toolkit::Button::Property::SELECTED_VISUAL, mToggleSelectedVisuals[mCurrentToggleIndex]);
  PrepareVisual(Toolkit::Button::Property::DISABLED_UNSELECTED_VISUAL, mToggleDisabledVisuals[mCurrentToggleIndex]);
  PrepareVisual(Toolkit::Button::Property::DISABLED_SELECTED_VISUAL, mToggleDisabledSelectedVisuals[mCurrentToggleIndex]);

  //Need to check mCurrentToggleIndex, it must less than the size of mToggleTooltips.
  if(!mToggleTooltips.empty() && (mCurrentToggleIndex < mToggleTooltips.size()))
  {
    Self().SetProperty(Toolkit::DevelControl::Property::TOOLTIP, mToggleTooltips[mCurrentToggleIndex]);
  }

  RelayoutRequest();
}

Dali::Accessibility::States ToggleButton::ToggleButtonAccessible::CalculateStates()
{
  auto states = Button::ButtonAccessible::CalculateStates();
  auto button = Toolkit::ToggleButton::DownCast(Self());
  if(button.GetProperty<int>(Toolkit::ToggleButton::Property::CURRENT_STATE_INDEX))
  {
    states[Dali::Accessibility::State::CHECKED] = true;
  }
  return states;
}

std::string ToggleButton::ToggleButtonAccessible::GetDescriptionRaw() const
{
  auto button   = Toolkit::ToggleButton::DownCast(Self());
  auto index    = button.GetProperty<int>(Toolkit::ToggleButton::Property::CURRENT_STATE_INDEX);
  auto tooltips = button.GetProperty<Property::Array>(Toolkit::ToggleButton::Property::TOOLTIPS);

  return tooltips[index].Get<std::string>();
}

Property::Index ToggleButton::ToggleButtonAccessible::GetDescriptionPropertyIndex()
{
  return Toolkit::ToggleButton::Property::TOOLTIPS;
}

void ToggleButton::OnStateChange(State newState)
{
  // TODO: replace it with OnPropertySet hook once Button::Property::SELECTED will be consistently used
  if((Self() == Dali::Accessibility::Accessible::GetCurrentlyHighlightedActor()) && (newState == SELECTED_STATE || newState == UNSELECTED_STATE))
  {
    auto* accessible = GetAccessibleObject();

    accessible->EmitStateChanged(Dali::Accessibility::State::CHECKED, mCurrentToggleIndex ? 1 : 0, 0);
    accessible->Emit(Dali::Accessibility::ObjectPropertyChangeEvent::DESCRIPTION);
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
