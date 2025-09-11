/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/animation.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>
#include <dali-toolkit/internal/controls/control/control-accessibility-data.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/control.h>

namespace
{
Dali::Toolkit::Internal::Control::Impl& GetControlImplementation(Dali::Toolkit::Control control)
{
  auto& internalControl = Dali::Toolkit::Internal::GetImplementation(control);

  return Dali::Toolkit::Internal::Control::Impl::Get(internalControl);
}

} // unnamed namespace

namespace Dali
{
namespace Toolkit
{
namespace DevelControl
{
void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.RegisterVisual(index, visual);
}

void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, int depthIndex)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.RegisterVisual(index, visual, depthIndex);
}

void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, bool enabled)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.RegisterVisual(index, visual, enabled);
}

void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, bool enabled, int depthIndex)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.RegisterVisual(index, visual, enabled, depthIndex);
}

void UnregisterVisual(Internal::Control& control, Dali::Property::Index index)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.UnregisterVisual(index);
}

Toolkit::Visual::Base GetVisual(const Internal::Control& control, Dali::Property::Index index)
{
  const Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  return controlDataImpl.GetVisual(index);
}

void EnableVisual(Internal::Control& control, Dali::Property::Index index, bool enable)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.EnableVisual(index, enable);
}

bool IsVisualEnabled(const Internal::Control& control, Dali::Property::Index index)
{
  const Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  return controlDataImpl.IsVisualEnabled(index);
}

Dali::Animation CreateTransition(Internal::Control& control, const Toolkit::TransitionData& handle)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  return controlDataImpl.CreateTransition(handle);
}

void AddTransitions(Internal::Control&             control,
                    Dali::Animation                animation,
                    const Toolkit::TransitionData& transitionData)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.AddTransitions(animation, transitionData);
}

void DoAction(Control& control, Dali::Property::Index visualIndex, Dali::Property::Index actionId, const Dali::Property::Value attributes)
{
  Internal::Control&       controlInternal = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(controlInternal);
  controlDataImpl.DoAction(visualIndex, actionId, attributes);
}

void DoActionExtension(Control& control, Dali::Property::Index visualIndex, Dali::Property::Index actionId, Dali::Any attributes)
{
  Internal::Control&       controlInternal = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(controlInternal);
  controlDataImpl.DoActionExtension(visualIndex, actionId, attributes);
}

void EnableCornerPropertiesOverridden(Internal::Control& control, Visual::Base& visual, bool enable, Dali::Constraint cornerRadiusConstraint)
{
  Internal::Control::Impl::Get(control).EnableCornerPropertiesOverridden(visual, enable, cornerRadiusConstraint);
}

void SetInputMethodContext(Internal::Control& control, InputMethodContext& inputMethodContext)
{
  Internal::Control::Impl::Get(control).SetInputMethodContext(inputMethodContext);
}

VisualEventSignalType& VisualEventSignal(Control control)
{
  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(internalControl);
  return controlDataImpl.VisualEventSignal();
}

Dali::Property GetVisualProperty(Control control, Dali::Property::Index index, Dali::Property::Key visualPropertyKey)
{
  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(internalControl);
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

void AppendAccessibilityRelation(Toolkit::Control control, Dali::Actor destination, Dali::Accessibility::RelationType relation)
{
  if(auto destinationAccessible = Accessibility::Accessible::Get(destination))
  {
    GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityProps.relations[relation].insert(destinationAccessible);
  }
}

void RemoveAccessibilityRelation(Toolkit::Control control, Dali::Actor destination, Dali::Accessibility::RelationType relation)
{
  auto& controlImpl = GetControlImplementation(control);

  auto* accessibilityData = controlImpl.GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    if(auto destinationAccessible = Accessibility::Accessible::Get(destination))
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

std::vector<Accessibility::Relation> GetAccessibilityRelations(Toolkit::Control control)
{
  std::vector<Accessibility::Relation> result;

  auto& controlImpl = GetControlImplementation(control);

  const auto* accessibilityData = controlImpl.GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    const auto& relations = accessibilityData->mAccessibilityProps.relations;
    for(const auto& relation : relations)
    {
      const auto& targets = relation.second;

      result.emplace_back(Accessibility::Relation{relation.first, {}});
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

void SetAccessibilityReadingInfoType(Toolkit::Control control, const Dali::Accessibility::ReadingInfoTypes types)
{
  GetControlImplementation(control).SetAccessibilityReadingInfoType(types);
}

Dali::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType(Toolkit::Control control)
{
  return GetControlImplementation(control).GetAccessibilityReadingInfoType();
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

Dali::Accessibility::States GetAccessibilityStates(Toolkit::Control control)
{
  auto controlAccessible = GetControlImplementation(control).GetAccessibleObject();
  if(DALI_LIKELY(controlAccessible))
  {
    return controlAccessible->GetStates();
  }
  return Dali::Accessibility::States{};
}

void NotifyAccessibilityStateChange(Toolkit::Control control, Dali::Accessibility::States states, bool recurse)
{
  auto controlAccessible = GetControlImplementation(control).GetAccessibleObject();
  if(DALI_LIKELY(controlAccessible))
  {
    controlAccessible->NotifyAccessibilityStateChange(std::move(states), recurse);
  }
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

void EmitAccessibilityStateChanged(Dali::Actor actor, Accessibility::State state, int newValue)
{
  auto bridge  = Accessibility::Bridge::GetCurrentBridge();
  auto control = Toolkit::Control::DownCast(actor);
  if(DALI_LIKELY(control))
  {
    if(state == Accessibility::State::SHOWING)
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

  if(bridge->IsUp())
  {
    auto accessible = dynamic_cast<Accessibility::ActorAccessible*>(Accessibility::Accessible::Get(actor));
    if(DALI_LIKELY(accessible))
    {
      accessible->EmitStateChanged(state, newValue, 0);
    }
  }
}

} // namespace DevelControl

} // namespace Toolkit

} // namespace Dali
