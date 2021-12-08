/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/control.h>

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

static Toolkit::Internal::Control::Impl* GetControlImplementation(Dali::Actor actor)
{
  Dali::Toolkit::Control control = Toolkit::Control::DownCast(actor);
  if(control)
  {
    auto& internalControl = Toolkit::Internal::GetImplementation(control);
    auto& controlDataImpl = Toolkit::Internal::Control::Impl::Get(internalControl);
    return &controlDataImpl;
  }
  return nullptr;
}

Toolkit::DevelControl::AccessibilityActivateSignalType& AccessibilityActivateSignal(Toolkit::Control control)
{
  return GetControlImplementation(control)->mAccessibilityActivateSignal;
}

Toolkit::DevelControl::AccessibilityReadingSkippedSignalType& AccessibilityReadingSkippedSignal(Toolkit::Control control)
{
  return GetControlImplementation(control)->mAccessibilityReadingSkippedSignal;
}

Toolkit::DevelControl::AccessibilityReadingPausedSignalType& AccessibilityReadingPausedSignal(Toolkit::Control control)
{
  return GetControlImplementation(control)->mAccessibilityReadingPausedSignal;
}

Toolkit::DevelControl::AccessibilityReadingResumedSignalType& AccessibilityReadingResumedSignal(Toolkit::Control control)
{
  return GetControlImplementation(control)->mAccessibilityReadingResumedSignal;
}

Toolkit::DevelControl::AccessibilityReadingCancelledSignalType& AccessibilityReadingCancelledSignal(Toolkit::Control control)
{
  return GetControlImplementation(control)->mAccessibilityReadingCancelledSignal;
}

Toolkit::DevelControl::AccessibilityReadingStoppedSignalType& AccessibilityReadingStoppedSignal(Toolkit::Control control)
{
  return GetControlImplementation(control)->mAccessibilityReadingStoppedSignal;
}

Toolkit::DevelControl::AccessibilityGetNameSignalType& AccessibilityGetNameSignal(Toolkit::Control control)
{
  return GetControlImplementation(control)->mAccessibilityGetNameSignal;
}

Toolkit::DevelControl::AccessibilityGetDescriptionSignalType& AccessibilityGetDescriptionSignal(Toolkit::Control control)
{
  return GetControlImplementation(control)->mAccessibilityGetDescriptionSignal;
}

Toolkit::DevelControl::AccessibilityDoGestureSignalType& AccessibilityDoGestureSignal(Toolkit::Control control)
{
  return GetControlImplementation(control)->mAccessibilityDoGestureSignal;
}

void AppendAccessibilityRelation(Dali::Actor control, Actor destination, Dali::Accessibility::RelationType relation)
{
  if(auto controlDataImpl = GetControlImplementation(control))
  {
    auto object = controlDataImpl->GetAccessibilityObject(destination);
    if(object)
    {
      controlDataImpl->mAccessibilityRelations[relation].insert(object);
    }
  }
}

void RemoveAccessibilityRelation(Dali::Actor control, Actor destination, Dali::Accessibility::RelationType relation)
{
  if(auto controlDataImpl = GetControlImplementation(control))
  {
    auto object = controlDataImpl->GetAccessibilityObject(destination);
    if(object)
    {
      auto& relations = controlDataImpl->mAccessibilityRelations;

      relations[relation].erase(object);

      if(relations[relation].empty())
      {
        relations.erase(relation);
      }
    }
  }
}

std::vector<std::vector<Accessibility::Address>> GetAccessibilityRelations(Dali::Actor control)
{
  if(auto controlDataImpl = GetControlImplementation(control))
  {
    std::vector<std::vector<Accessibility::Address>> result(static_cast<std::size_t>(Accessibility::RelationType::MAX_COUNT));

    // Map every Accessible* to its Address
    for(auto& relation : controlDataImpl->mAccessibilityRelations)
    {
      auto  index   = static_cast<std::size_t>(relation.first);
      auto& targets = relation.second;

      std::transform(targets.begin(), targets.end(), std::back_inserter(result[index]), [](auto* x) {
        return x->GetAddress();
      });
    }

    return result;
  }

  return {};
}

void ClearAccessibilityRelations(Dali::Actor control)
{
  if(auto controlDataImpl = GetControlImplementation(control))
  {
    controlDataImpl->mAccessibilityRelations.clear();
  }
}

void SetAccessibilityConstructor(Dali::Actor control, std::function<std::unique_ptr<Dali::Accessibility::Accessible>(Dali::Actor)> constructor)
{
  if(auto controlDataImpl = GetControlImplementation(control))
  {
    controlDataImpl->mAccessibilityConstructor = constructor;
  }
}

void AppendAccessibilityAttribute(Dali::Actor control, const std::string& key, const std::string value)
{
  if(auto controlDataImpl = GetControlImplementation(control))
  {
    controlDataImpl->AppendAccessibilityAttribute(key, value);
  }
}

void RemoveAccessibilityAttribute(Dali::Actor control, const std::string& key)
{
  if(auto controlDataImpl = GetControlImplementation(control))
  {
    controlDataImpl->RemoveAccessibilityAttribute(key);
  }
}

void ClearAccessibilityAttributes(Dali::Actor control)
{
  if(auto controlDataImpl = GetControlImplementation(control))
  {
    controlDataImpl->ClearAccessibilityAttributes();
  }
}

void SetAccessibilityReadingInfoType(Dali::Actor control, const Dali::Accessibility::ReadingInfoTypes types)
{
  if(auto controlDataImpl = GetControlImplementation(control))
  {
    controlDataImpl->SetAccessibilityReadingInfoType(types);
  }
}

Dali::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType(Dali::Actor control)
{
  if(auto controlDataImpl = GetControlImplementation(control))
  {
    return controlDataImpl->GetAccessibilityReadingInfoType();
  }
  return {};
}

bool ClearAccessibilityHighlight(Dali::Actor control)
{
  if(auto controlDataImpl = GetControlImplementation(control))
  {
    auto object = dynamic_cast<Dali::Accessibility::Component*>(controlDataImpl->GetAccessibilityObject());
    if(object)
    {
      return object->ClearHighlight();
    }
  }
  return false;
}

bool GrabAccessibilityHighlight(Dali::Actor control)
{
  if(auto controlDataImpl = GetControlImplementation(control))
  {
    auto object = dynamic_cast<Dali::Accessibility::Component*>(controlDataImpl->GetAccessibilityObject());
    if(object)
    {
      return object->GrabHighlight();
    }
  }
  return false;
}

Dali::Accessibility::States GetAccessibilityStates(Dali::Actor control)
{
  if(auto controlDataImpl = GetControlImplementation(control))
  {
    auto object = dynamic_cast<Dali::Accessibility::Component*>(controlDataImpl->GetAccessibilityObject());
    if(object)
    {
      return object->GetStates();
    }
  }
  return {};
}

void NotifyAccessibilityStateChange(Dali::Actor control, Dali::Accessibility::States states, bool isRecursive)
{
  if(auto controlDataImpl = GetControlImplementation(control))
  {
    controlDataImpl->GetAccessibilityObject()->NotifyAccessibilityStateChange(std::move(states), isRecursive);
  }
}

Dali::Accessibility::Accessible* GetBoundAccessibilityObject(Dali::Actor control)
{
  return Dali::Accessibility::Accessible::Get(control);
}

} // namespace DevelControl

} // namespace Toolkit

} // namespace Dali
