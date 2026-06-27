/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include "control-devel.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-bridge.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/animation.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-accessible.h>
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>
#include <dali-toolkit/internal/controls/control/control-accessibility-data.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/control.h>

namespace
{
Dali::Toolkit::Internal::Control& GetControlImplementation(Dali::Toolkit::Control control)
{
  auto& internalControl = Dali::Toolkit::GetImplementation(control);

  return Dali::Toolkit::Internal::Control::Get(internalControl);
}

Dali::Integration::Accessibility::RelationType ToIntegrationRelationType(Dali::Toolkit::Accessibility::RelationType relation)
{
  return static_cast<Dali::Integration::Accessibility::RelationType>(relation);
}

} // unnamed namespace

namespace Dali
{
namespace Toolkit
{
namespace DevelControl
{
void RegisterVisual(ControlImpl& control, Dali::Property::Index index, Toolkit::Visual::Base& visual)
{
  auto& controlDataImpl = Internal::Control::Get(control);
  controlDataImpl.RegisterVisual(index, visual);
}

void RegisterVisual(ControlImpl& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, int depthIndex)
{
  auto& controlDataImpl = Internal::Control::Get(control);
  controlDataImpl.RegisterVisual(index, visual, depthIndex);
}

void RegisterVisual(ControlImpl& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, bool enabled)
{
  auto& controlDataImpl = Internal::Control::Get(control);
  controlDataImpl.RegisterVisual(index, visual, enabled);
}

void RegisterVisual(ControlImpl& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, bool enabled, int depthIndex)
{
  auto& controlDataImpl = Internal::Control::Get(control);
  controlDataImpl.RegisterVisual(index, visual, enabled, depthIndex);
}

void UnregisterVisual(ControlImpl& control, Dali::Property::Index index)
{
  auto& controlDataImpl = Internal::Control::Get(control);
  controlDataImpl.UnregisterVisual(index);
}

Toolkit::Visual::Base GetVisual(const ControlImpl& control, Dali::Property::Index index)
{
  const auto& controlDataImpl = Internal::Control::Get(control);
  return controlDataImpl.GetVisual(index);
}

void EnableVisual(ControlImpl& control, Dali::Property::Index index, bool enable)
{
  auto& controlDataImpl = Internal::Control::Get(control);
  controlDataImpl.EnableVisual(index, enable);
}

bool IsVisualEnabled(const ControlImpl& control, Dali::Property::Index index)
{
  const auto& controlDataImpl = Internal::Control::Get(control);
  return controlDataImpl.IsVisualEnabled(index);
}

Dali::Animation CreateTransition(ControlImpl& control, const Toolkit::TransitionData& handle)
{
  auto& controlDataImpl = Internal::Control::Get(control);
  return controlDataImpl.CreateTransition(handle);
}

void AddTransitions(ControlImpl&                   control,
                    Dali::Animation                animation,
                    const Toolkit::TransitionData& transitionData)
{
  auto& controlDataImpl = Internal::Control::Get(control);
  controlDataImpl.AddTransitions(animation, transitionData);
}

void DoAction(Control& control, Dali::Property::Index visualIndex, Dali::Property::Index actionId, const Dali::Property::Value& attributes)
{
  ControlImpl& controlInternal = Toolkit::GetImplementation(control);
  auto&        controlDataImpl = Internal::Control::Get(controlInternal);
  controlDataImpl.DoAction(visualIndex, actionId, attributes);
}

void DoActionExtension(Control& control, Dali::Property::Index visualIndex, Dali::Property::Index actionId, const Dali::Any& attributes)
{
  ControlImpl& controlInternal = Toolkit::GetImplementation(control);
  auto&        controlDataImpl = Internal::Control::Get(controlInternal);
  controlDataImpl.DoActionExtension(visualIndex, actionId, attributes);
}

void EnableCornerPropertiesOverridden(ControlImpl& control, Visual::Base& visual, bool enable, Dali::Constraint cornerRadiusConstraint)
{
  Internal::Control::Get(control).EnableCornerPropertiesOverridden(visual, enable, cornerRadiusConstraint);
}

void SetInputMethodContext(ControlImpl& control, InputMethodContext& inputMethodContext)
{
  Internal::Control::Get(control).SetInputMethodContext(inputMethodContext);
}

VisualEventSignalType& VisualEventSignal(Control control)
{
  ControlImpl& internalControl = Toolkit::GetImplementation(control);
  auto&        controlDataImpl = Internal::Control::Get(internalControl);
  return controlDataImpl.VisualEventSignal();
}

Dali::Property GetVisualProperty(Control control, Dali::Property::Index index, Dali::Property::Key visualPropertyKey)
{
  ControlImpl& internalControl = Toolkit::GetImplementation(control);
  auto&        controlDataImpl = Internal::Control::Get(internalControl);
  return controlDataImpl.GetVisualProperty(index, visualPropertyKey);
}

Toolkit::DevelControl::AccessibilityActivateSignalType& AccessibilityActivateSignal(Toolkit::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityActivateSignal;
}

Toolkit::DevelControl::AccessibilityReadingSkippedSignalType& AccessibilityReadingSkippedSignal(Toolkit::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityReadingSkippedSignal;
}

Toolkit::DevelControl::AccessibilityReadingPausedSignalType& AccessibilityReadingPausedSignal(Toolkit::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityReadingPausedSignal;
}

Toolkit::DevelControl::AccessibilityReadingResumedSignalType& AccessibilityReadingResumedSignal(Toolkit::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityReadingResumedSignal;
}

Toolkit::DevelControl::AccessibilityReadingCancelledSignalType& AccessibilityReadingCancelledSignal(Toolkit::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityReadingCancelledSignal;
}

Toolkit::DevelControl::AccessibilityReadingStoppedSignalType& AccessibilityReadingStoppedSignal(Toolkit::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityReadingStoppedSignal;
}

Toolkit::DevelControl::AccessibilityGetNameSignalType& AccessibilityGetNameSignal(Toolkit::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityGetNameSignal;
}

Toolkit::DevelControl::AccessibilityGetDescriptionSignalType& AccessibilityGetDescriptionSignal(Toolkit::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityGetDescriptionSignal;
}

Toolkit::DevelControl::AccessibilityDoGestureSignalType& AccessibilityDoGestureSignal(Toolkit::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityDoGestureSignal;
}

Toolkit::DevelControl::AccessibilityActionSignalType& AccessibilityActionSignal(Toolkit::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityActionSignal;
}

Toolkit::DevelControl::AccessibilityHighlightedSignalType& AccessibilityHighlightedSignal(Toolkit::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityHighlightedSignal;
}

void AppendAccessibilityRelation(Toolkit::Control control, Dali::Actor destination, Dali::Toolkit::Accessibility::RelationType relation)
{
  if(auto destinationAccessible = Dali::Accessibility::Accessible::Get(destination))
  {
    GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityProps.relations[relation].insert(destinationAccessible);
  }
}

void RemoveAccessibilityRelation(Toolkit::Control control, Dali::Actor destination, Dali::Toolkit::Accessibility::RelationType relation)
{
  auto& controlImpl = GetControlImplementation(control);

  auto* accessibilityData = controlImpl.GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    if(auto destinationAccessible = Dali::Accessibility::Accessible::Get(destination))
    {
      auto& relations = accessibilityData->mAccessibilityProps.relations;

      relations[relation].erase(destinationAccessible);

      if(relations[relation].empty())
      {
        relations.erase(relation);
      }
    }
  }
}

std::vector<Dali::Devel::Accessibility::Relation> GetAccessibilityRelations(Toolkit::Control control)
{
  std::vector<Dali::Devel::Accessibility::Relation> result;

  auto& controlImpl = GetControlImplementation(control);

  const auto* accessibilityData = controlImpl.GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    const auto& relations = accessibilityData->mAccessibilityProps.relations;
    for(const auto& relation : relations)
    {
      const auto& targets = relation.second;

      result.emplace_back(Dali::Devel::Accessibility::Relation{ToIntegrationRelationType(relation.first), {}});
      std::copy(targets.begin(), targets.end(), std::back_inserter(result.back().mTargets));
    }
  }

  return result;
}

void ClearAccessibilityRelations(Toolkit::Control control)
{
  auto& controlImpl = GetControlImplementation(control);

  auto* accessibilityData = controlImpl.GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    accessibilityData->mAccessibilityProps.relations.clear();
  }
}

void AppendAccessibilityAttribute(Toolkit::Control control, const std::string& key, const std::string& value)
{
  GetControlImplementation(control).AppendAccessibilityAttribute(key, value);
}

void RemoveAccessibilityAttribute(Toolkit::Control control, const std::string& key)
{
  GetControlImplementation(control).RemoveAccessibilityAttribute(key);
}

void ClearAccessibilityAttributes(Toolkit::Control control)
{
  GetControlImplementation(control).ClearAccessibilityAttributes();
}

void SetAccessibilityReadingInfoType(Toolkit::Control control, const Dali::Integration::Accessibility::ReadingInfoTypes types)
{
  GetControlImplementation(control).SetAccessibilityReadingInfoType(types);
}

void SetAccessibilityReadingInfoTypeRaw(Toolkit::Control control, uint32_t types)
{
  SetAccessibilityReadingInfoType(control, Dali::Integration::Accessibility::ReadingInfoTypes{types});
}

Dali::Integration::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType(Toolkit::Control control)
{
  return GetControlImplementation(control).GetAccessibilityReadingInfoType();
}

uint32_t GetAccessibilityReadingInfoTypeRaw(Toolkit::Control control)
{
  return GetAccessibilityReadingInfoType(control).GetRawData()[0];
}

bool ClearAccessibilityHighlight(Toolkit::Control control)
{
  auto controlAccessible = GetControlImplementation(control).GetAccessibleObject();
  if(DALI_LIKELY(controlAccessible))
  {
    return controlAccessible->ClearHighlight();
  }
  return false;
}

bool GrabAccessibilityHighlight(Toolkit::Control control)
{
  auto controlAccessible = GetControlImplementation(control).GetAccessibleObject();
  if(DALI_LIKELY(controlAccessible))
  {
    return controlAccessible->GrabHighlight();
  }
  return false;
}

Dali::Integration::Accessibility::States GetAccessibilityStates(Toolkit::Control control)
{
  auto controlAccessible = GetControlImplementation(control).GetAccessibleObject();
  if(DALI_LIKELY(controlAccessible))
  {
    return controlAccessible->GetStates();
  }
  return Dali::Integration::Accessibility::States{};
}

uint64_t GetAccessibilityStatesRaw(Toolkit::Control control)
{
  return GetAccessibilityStates(control).GetRawData64();
}

void NotifyAccessibilityStateChange(Toolkit::Control control, Dali::Integration::Accessibility::States states, bool recurse)
{
  auto controlAccessible = GetControlImplementation(control).GetAccessibleObject();
  if(DALI_LIKELY(controlAccessible))
  {
    controlAccessible->NotifyAccessibilityStateChange(std::move(states), recurse);
  }
}

void NotifyAccessibilityStateChangeRaw(Toolkit::Control control, uint64_t states, bool recurse)
{
  NotifyAccessibilityStateChange(control, Dali::Integration::Accessibility::States{states}, recurse);
}

bool IsAccessibleCreated(Toolkit::Control control)
{
  return GetControlImplementation(control).IsAccessibleCreated();
}

void EnableCreateAccessible(Toolkit::Control control, bool enable)
{
  GetControlImplementation(control).EnableCreateAccessible(enable);
}

bool IsCreateAccessibleEnabled(Toolkit::Control control)
{
  return GetControlImplementation(control).IsCreateAccessibleEnabled();
}

void EmitAccessibilityStateChanged(Dali::Actor actor, Dali::Integration::Accessibility::State state, int newValue)
{
  auto bridge  = Integration::Accessibility::Bridge::GetCurrentBridge();
  auto control = Toolkit::Control::DownCast(actor);
  if(DALI_LIKELY(control && bridge))
  {
    if(state == Dali::Integration::Accessibility::State::SHOWING)
    {
      bool isModal = ControlAccessible::IsModal(control);
      if(isModal)
      {
        if(newValue == 1)
        {
          bridge->RegisterDefaultLabel(actor);
        }
        else
        {
          bridge->UnregisterDefaultLabel(actor);
        }
      }
    }
  }

  if(bridge && bridge->IsUp())
  {
    auto accessible = dynamic_cast<Dali::Accessibility::ActorAccessible*>(Dali::Accessibility::Accessible::Get(actor));
    if(DALI_LIKELY(accessible))
    {
      accessible->EmitStateChanged(state, newValue, 0);
    }
  }
}

void NotifyAccessibilityPresentationChanged(Dali::Actor actor, bool presented)
{
  EmitAccessibilityStateChanged(actor, Dali::Integration::Accessibility::State::SHOWING, presented ? 1 : 0);
}

} // namespace DevelControl

} // namespace Toolkit

} // namespace Dali
