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
#include "control-accessible.h"

// EXTERNAL INCLUDES
#ifdef DGETTEXT_ENABLED
#include <libintl.h>
#endif

#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/asset-manager/asset-manager.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>

namespace Dali::Toolkit::DevelControl
{
namespace
{
static std::string GetLocaleText(std::string string, const char *domain = "dali-toolkit")
{
#ifdef DGETTEXT_ENABLED
    /*TODO: currently non-localized string is used as a key for translation lookup. In case the lookup key formatting is forced
          consider calling utility function for converting non-localized string into well-formatted key before lookup. */
    return dgettext(domain, string.c_str());
#else
    return string;
#endif
}

static Dali::Actor CreateHighlightIndicatorActor()
{
  std::string focusBorderImagePath(AssetManager::GetDaliImagePath());
  focusBorderImagePath += "/keyboard_focus.9.png";

  // Create the default if it hasn't been set and one that's shared by all the
  // keyboard focusable actors
  auto actor = Toolkit::ImageView::New(focusBorderImagePath);
  actor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);

  DevelControl::AppendAccessibilityAttribute(actor, "highlight", std::string());
  actor.SetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, false);

  return actor;
}
} // unnamed namespace

ControlAccessible::ControlAccessible(Dali::Actor self)
: ActorAccessible(self)
{
  auto control = Toolkit::Control::DownCast(self);

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);

  self.PropertySetSignal().Connect(&controlImpl, [this, &controlImpl](Dali::Handle& handle, Dali::Property::Index index, Dali::Property::Value value) {
    if(this->Self() != Dali::Accessibility::Accessible::GetCurrentlyHighlightedActor())
    {
      return;
    }

    if(index == DevelControl::Property::ACCESSIBILITY_NAME || (index == GetNamePropertyIndex() && controlImpl.mAccessibilityName.empty()))
    {
      if(controlImpl.mAccessibilityGetNameSignal.Empty())
      {
        Emit(Dali::Accessibility::ObjectPropertyChangeEvent::NAME);
      }
    }

    if(index == DevelControl::Property::ACCESSIBILITY_DESCRIPTION || (index == GetDescriptionPropertyIndex() && controlImpl.mAccessibilityDescription.empty()))
    {
      if(controlImpl.mAccessibilityGetDescriptionSignal.Empty())
      {
        Emit(Dali::Accessibility::ObjectPropertyChangeEvent::DESCRIPTION);
      }
    }
  });
}

std::string ControlAccessible::GetName() const
{
  auto control = Dali::Toolkit::Control::DownCast(Self());

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);
  std::string name;

  if(!controlImpl.mAccessibilityGetNameSignal.Empty())
  {
    controlImpl.mAccessibilityGetNameSignal.Emit(name);
  }
  else if(!controlImpl.mAccessibilityName.empty())
  {
    name = controlImpl.mAccessibilityName;
  }
  else if(auto raw = GetNameRaw(); !raw.empty())
  {
    name = raw;
  }
  else
  {
    name = Self().GetProperty<std::string>(Actor::Property::NAME);
  }

  if(!controlImpl.mAccessibilityTranslationDomain.empty())
  {
    return GetLocaleText(name, controlImpl.mAccessibilityTranslationDomain.c_str());
  }

  return GetLocaleText(name);
}

std::string ControlAccessible::GetNameRaw() const
{
  return {};
}

std::string ControlAccessible::GetDescription() const
{
  auto control = Dali::Toolkit::Control::DownCast(Self());

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);
  std::string description;

  if(!controlImpl.mAccessibilityGetDescriptionSignal.Empty())
  {
    controlImpl.mAccessibilityGetDescriptionSignal.Emit(description);
  }
  else if(!controlImpl.mAccessibilityDescription.empty())
  {
    description = controlImpl.mAccessibilityDescription;
  }
  else
  {
    description = GetDescriptionRaw();
  }

  if(!controlImpl.mAccessibilityTranslationDomain.empty())
  {
    return GetLocaleText(description, controlImpl.mAccessibilityTranslationDomain.c_str());
  }

  return GetLocaleText(description);
}

std::string ControlAccessible::GetDescriptionRaw() const
{
  return {};
}

Dali::Accessibility::Role ControlAccessible::GetRole() const
{
  return Self().GetProperty<Dali::Accessibility::Role>(Toolkit::DevelControl::Property::ACCESSIBILITY_ROLE);
}

std::string ControlAccessible::GetLocalizedRoleName() const
{
  return GetLocaleText(GetRoleName());
}

bool ControlAccessible::IsShowing()
{
  Dali::Actor self = Self();
  if(!self.GetProperty<bool>(Actor::Property::VISIBLE) || self.GetProperty<Vector4>(Actor::Property::WORLD_COLOR).a == 0 || self.GetProperty<bool>(Dali::DevelActor::Property::CULLED))
  {
    return false;
  }

  auto* child  = this;
  auto* parent = dynamic_cast<Toolkit::DevelControl::ControlAccessible*>(child->GetParent());
  if(!parent)
  {
    return true;
  }

  auto childExtent = child->GetExtents(Dali::Accessibility::CoordinateType::WINDOW);
  while(parent)
  {
    auto control      = Dali::Toolkit::Control::DownCast(parent->Self());
    if(!control.GetProperty<bool>(Actor::Property::VISIBLE))
    {
      return false;
    }
    auto clipMode     = control.GetProperty(Actor::Property::CLIPPING_MODE).Get<bool>();
    auto parentExtent = parent->GetExtents(Dali::Accessibility::CoordinateType::WINDOW);
    if ((clipMode != ClippingMode::DISABLED) && !parentExtent.Intersects(childExtent))
    {
      return false;
    }
    parent = dynamic_cast<Toolkit::DevelControl::ControlAccessible*>(parent->GetParent());
  }

  return true;
}

Dali::Accessibility::States ControlAccessible::CalculateStates()
{
  using Dali::Accessibility::State;

  Dali::Actor self = Self();
  Dali::Accessibility::States states;

  states[State::FOCUSABLE]     = self.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE);
  states[State::FOCUSED]       = Toolkit::KeyboardFocusManager::Get().GetCurrentFocusActor() == self;
  states[State::HIGHLIGHTABLE] = self.GetProperty<bool>(Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE);
  states[State::HIGHLIGHTED]   = GetCurrentlyHighlightedActor() == self;
  states[State::ENABLED]       = true;
  states[State::SENSITIVE]     = true;
  states[State::VISIBLE]       = self.GetProperty<bool>(Actor::Property::VISIBLE);
  states[State::SHOWING]       = IsShowing();
  states[State::DEFUNCT]       = !self.GetProperty(Dali::DevelActor::Property::CONNECTED_TO_SCENE).Get<bool>();

  return states;
}

Dali::Accessibility::States ControlAccessible::GetStates()
{
  return CalculateStates();
}

Dali::Accessibility::Attributes ControlAccessible::GetAttributes() const
{
  std::unordered_map<std::string, std::string> attributeMap;
  auto control = Dali::Toolkit::Control::DownCast(Self());
  auto attribute = control.GetProperty(Dali::Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES);
  auto map = attribute.GetMap();

  if(map)
  {
    auto mapSize = map->Count();

    for(unsigned int i = 0; i < mapSize; i++)
    {
      auto mapKey = map->GetKeyAt(i);
      if(mapKey.type == Dali::Property::Key::STRING)
      {
        std::string mapValue;
        if(map->GetValue(i).Get(mapValue))
        {
          attributeMap.emplace(std::move(mapKey.stringKey), std::move(mapValue));
        }
      }
    }
  }

  return attributeMap;
}

bool ControlAccessible::IsHidden() const
{
  auto control = Dali::Toolkit::Control::DownCast(Self());

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);

  return controlImpl.mAccessibilityHidden;
}

bool ControlAccessible::GrabFocus()
{
  return Toolkit::KeyboardFocusManager::Get().SetCurrentFocusActor(Self());
}

void ControlAccessible::ScrollToSelf()
{
  auto* child = this;
  auto* parent = dynamic_cast<Toolkit::DevelControl::ControlAccessible*>(child->GetParent());

  while (parent)
  {
    if (parent->IsScrollable())
    {
      parent->ScrollToChild(child->Self());
    }

    parent = dynamic_cast<Toolkit::DevelControl::ControlAccessible*>(parent->GetParent());
  }
}

void ControlAccessible::RegisterPositionPropertyNotification()
{
  auto                     control         = Dali::Toolkit::Control::DownCast(Self());
  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);
  controlImpl.RegisterAccessibilityPositionPropertyNotification();
}

void ControlAccessible::UnregisterPositionPropertyNotification()
{
  auto                     control         = Dali::Toolkit::Control::DownCast(Self());
  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);
  controlImpl.UnregisterAccessibilityPositionPropertyNotification();
}

bool ControlAccessible::GrabHighlight()
{
  Dali::Actor self = Self();
  auto oldHighlightedActor = GetCurrentlyHighlightedActor();

  if(!Dali::Accessibility::IsUp())
  {
    return false;
  }

  if(self == oldHighlightedActor)
  {
    return true;
  }

  // Clear the old highlight.
  if(oldHighlightedActor)
  {
    auto oldHighlightedObject = Dali::Accessibility::Component::DownCast(Accessible::Get(oldHighlightedActor));
    if(oldHighlightedObject)
    {
      oldHighlightedObject->ClearHighlight();
    }
  }

  auto highlight = GetHighlightActor();
  if(!highlight)
  {
    highlight = CreateHighlightIndicatorActor();
    SetHighlightActor(highlight);
  }

  highlight.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  highlight.SetProperty(Actor::Property::POSITION_Z, 1.0f);
  highlight.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 0.0f));

  // Need to set resize policy again, to update SIZE property which is set by
  // NUIViewAccessible. The highlight could move from NUIViewAccessible to
  // ControlAccessible. In this case, highlight has incorrect size.
  highlight.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);

  // Remember the highlight actor, so that when the default is changed with
  // SetHighlightActor(), the currently displayed highlight can still be cleared.
  mCurrentHighlightActor = highlight;
  ScrollToSelf();
  self.Add(highlight);
  SetCurrentlyHighlightedActor(self);
  EmitHighlighted(true);
  RegisterPositionPropertyNotification();

  return true;
}

bool ControlAccessible::ClearHighlight()
{
  Dali::Actor self = Self();

  if(!Dali::Accessibility::IsUp())
  {
    return false;
  }

  if(GetCurrentlyHighlightedActor() == self)
  {
    UnregisterPositionPropertyNotification();
    self.Remove(mCurrentHighlightActor.GetHandle());
    mCurrentHighlightActor = {};
    SetCurrentlyHighlightedActor({});
    EmitHighlighted(false);
    return true;
  }
  return false;
}

std::string ControlAccessible::GetActionName(size_t index) const
{
  if(index >= GetActionCount())
  {
    return {};
  }

  Dali::TypeInfo type;
  Self().GetTypeInfo(type);
  DALI_ASSERT_ALWAYS(type && "no TypeInfo object");
  return type.GetActionName(index);
}

std::string ControlAccessible::GetLocalizedActionName(size_t index) const
{
  return GetLocaleText(GetActionName(index));
}

std::string ControlAccessible::GetActionDescription(size_t index) const
{
  return {};
}

size_t ControlAccessible::GetActionCount() const
{
  Dali::TypeInfo type;
  Self().GetTypeInfo(type);
  DALI_ASSERT_ALWAYS(type && "no TypeInfo object");
  return type.GetActionCount();
}

std::string ControlAccessible::GetActionKeyBinding(size_t index) const
{
  return {};
}

bool ControlAccessible::DoAction(size_t index)
{
  std::string actionName = GetActionName(index);
  return Self().DoAction(actionName, {});
}

bool ControlAccessible::DoAction(const std::string& name)
{
  return Self().DoAction(name, {});
}

bool ControlAccessible::DoGesture(const Dali::Accessibility::GestureInfo& gestureInfo)
{
  auto control = Dali::Toolkit::Control::DownCast(Self());

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);

  if(!controlImpl.mAccessibilityDoGestureSignal.Empty())
  {
    auto ret = std::make_pair(gestureInfo, false);
    controlImpl.mAccessibilityDoGestureSignal.Emit(ret);
    return ret.second;
  }

  return false;
}

std::vector<Dali::Accessibility::Relation> ControlAccessible::GetRelationSet()
{
  auto control = Dali::Toolkit::Control::DownCast(Self());

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);

  std::vector<Dali::Accessibility::Relation> ret;

  for(auto& relation : controlImpl.mAccessibilityRelations)
  {
    auto& targets = relation.second;

    ret.emplace_back(Accessibility::Relation{relation.first, {}});

    // Map every Accessible* to its Address
    std::transform(targets.begin(), targets.end(), std::back_inserter(ret.back().targets), [](auto* x) {
      return x->GetAddress();
    });
  }

  return ret;
}

bool ControlAccessible::ScrollToChild(Actor child)
{
  return false;
}

Dali::Property::Index ControlAccessible::GetNamePropertyIndex()
{
  return Actor::Property::NAME;
}

Dali::Property::Index ControlAccessible::GetDescriptionPropertyIndex()
{
  return Dali::Property::INVALID_INDEX;
}

void ControlAccessible::SetLastPosition(Vector2 position)
{
  mLastPosition = position;
}

Vector2 ControlAccessible::GetLastPosition() const
{
  return mLastPosition;
}

} // namespace Dali::Toolkit::DevelControl
