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
#include "accessible-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/asset-manager/asset-manager.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>

namespace Dali::Toolkit::DevelControl {

AccessibleImpl::AccessibleImpl(Dali::Actor self, Dali::Accessibility::Role role, bool modal)
: self(self),
  modal(modal)
{
  auto control = Dali::Toolkit::Control::DownCast(self);

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);
  if(controlImpl.mAccessibilityRole == Dali::Accessibility::Role::UNKNOWN)
    controlImpl.mAccessibilityRole = role;

  self.PropertySetSignal().Connect(&controlImpl, [this, &controlImpl](Dali::Handle& handle, Dali::Property::Index index, Dali::Property::Value value) {
    if(this->self != Dali::Accessibility::Accessible::GetCurrentlyHighlightedActor())
    {
      return;
    }

    if(index == DevelControl::Property::ACCESSIBILITY_NAME || (index == GetNamePropertyIndex() && !controlImpl.mAccessibilityNameSet))
    {
      if(controlImpl.mAccessibilityGetNameSignal.Empty())
      {
        Emit(Dali::Accessibility::ObjectPropertyChangeEvent::NAME);
      }
    }

    if(index == DevelControl::Property::ACCESSIBILITY_DESCRIPTION || (index == GetDescriptionPropertyIndex() && !controlImpl.mAccessibilityDescriptionSet))
    {
      if(controlImpl.mAccessibilityGetDescriptionSignal.Empty())
      {
        Emit(Dali::Accessibility::ObjectPropertyChangeEvent::DESCRIPTION);
      }
    }
  });
}

std::string AccessibleImpl::GetName()
{
  auto control = Dali::Toolkit::Control::DownCast(self);

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);

  if(!controlImpl.mAccessibilityGetNameSignal.Empty())
  {
    std::string ret;
    controlImpl.mAccessibilityGetNameSignal.Emit(ret);
    return ret;
  }

  if(controlImpl.mAccessibilityNameSet)
    return controlImpl.mAccessibilityName;

  if(auto raw = GetNameRaw(); !raw.empty())
    return raw;

  return self.GetProperty<std::string>(Actor::Property::NAME);
}

std::string AccessibleImpl::GetNameRaw()
{
  return {};
}

std::string AccessibleImpl::GetDescription()
{
  auto control = Dali::Toolkit::Control::DownCast(self);

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);

  if(!controlImpl.mAccessibilityGetDescriptionSignal.Empty())
  {
    std::string ret;
    controlImpl.mAccessibilityGetDescriptionSignal.Emit(ret);
    return ret;
  }

  if(controlImpl.mAccessibilityDescriptionSet)
    return controlImpl.mAccessibilityDescription;

  return GetDescriptionRaw();
}

std::string AccessibleImpl::GetDescriptionRaw()
{
  return "";
}

Dali::Accessibility::Accessible* AccessibleImpl::GetParent()
{
  return Dali::Accessibility::Accessible::Get(self.GetParent());
}

size_t AccessibleImpl::GetChildCount()
{
  return self.GetChildCount();
}

Dali::Accessibility::Accessible* AccessibleImpl::GetChildAtIndex(size_t index)
{
  return Dali::Accessibility::Accessible::Get(self.GetChildAt(static_cast<unsigned int>(index)));
}

size_t AccessibleImpl::GetIndexInParent()
{
  auto s      = self;
  auto parent = s.GetParent();
  DALI_ASSERT_ALWAYS(parent && "can't call GetIndexInParent on object without parent");
  auto count = parent.GetChildCount();
  for(auto i = 0u; i < count; ++i)
  {
    auto c = parent.GetChildAt(i);
    if(c == s)
      return i;
  }
  DALI_ASSERT_ALWAYS(false && "object isn't child of it's parent");
  return static_cast<size_t>(-1);
}

Dali::Accessibility::Role AccessibleImpl::GetRole()
{
  return self.GetProperty<Dali::Accessibility::Role>(Toolkit::DevelControl::Property::ACCESSIBILITY_ROLE);
}

Dali::Accessibility::States AccessibleImpl::CalculateStates()
{
  Dali::Accessibility::States s;
  s[Dali::Accessibility::State::FOCUSABLE] = self.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE);
  s[Dali::Accessibility::State::FOCUSED]   = Toolkit::KeyboardFocusManager::Get().GetCurrentFocusActor() == self;
  if(self.GetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE).GetType() == Dali::Property::NONE)
    s[Dali::Accessibility::State::HIGHLIGHTABLE] = false;
  else
    s[Dali::Accessibility::State::HIGHLIGHTABLE] = self.GetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE).Get<bool>();
  s[Dali::Accessibility::State::HIGHLIGHTED] = GetCurrentlyHighlightedActor() == self;
  s[Dali::Accessibility::State::ENABLED]     = true;
  s[Dali::Accessibility::State::SENSITIVE]   = true;
  s[Dali::Accessibility::State::ANIMATED]    = self.GetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_ANIMATED).Get<bool>();
  s[Dali::Accessibility::State::VISIBLE]     = true;
  if(modal)
  {
    s[Dali::Accessibility::State::MODAL] = true;
  }
  s[Dali::Accessibility::State::SHOWING] = !self.GetProperty(Dali::DevelActor::Property::CULLED).Get<bool>() && self.GetCurrentProperty<bool>(Actor::Property::VISIBLE);

  s[Dali::Accessibility::State::DEFUNCT] = !self.GetProperty(Dali::DevelActor::Property::CONNECTED_TO_SCENE).Get<bool>();
  return s;
}

Dali::Accessibility::States AccessibleImpl::GetStates()
{
  return CalculateStates();
}

Dali::Accessibility::Attributes AccessibleImpl::GetAttributes()
{
  std::unordered_map<std::string, std::string> attribute_map;
  auto                                         q = Dali::Toolkit::Control::DownCast(self);
  auto                                         w =
    q.GetProperty(Dali::Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES);
  auto z = w.GetMap();

  if(z)
  {
    auto map_size = z->Count();

    for(unsigned int i = 0; i < map_size; i++)
    {
      auto map_key = z->GetKeyAt(i);
      if(map_key.type == Dali::Property::Key::STRING)
      {
        std::string map_value;
        if(z->GetValue(i).Get(map_value))
        {
          attribute_map.emplace(std::move(map_key.stringKey),
                                std::move(map_value));
        }
      }
    }
  }

  return attribute_map;
}

Dali::Accessibility::ComponentLayer AccessibleImpl::GetLayer()
{
  return Dali::Accessibility::ComponentLayer::WINDOW;
}

Dali::Rect<> AccessibleImpl::GetExtents(Dali::Accessibility::CoordType ctype)
{
  Vector2 screenPosition =
    self.GetProperty(Dali::DevelActor::Property::SCREEN_POSITION)
      .Get<Vector2>();
  auto size = self.GetCurrentProperty<Vector3>(Actor::Property::SIZE) * self.GetCurrentProperty<Vector3>(Actor::Property::WORLD_SCALE);
  bool positionUsesAnchorPoint =
    self.GetProperty(Dali::DevelActor::Property::POSITION_USES_ANCHOR_POINT)
      .Get<bool>();
  Vector3 anchorPointOffSet =
    size * (positionUsesAnchorPoint ? self.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT)
                                    : AnchorPoint::TOP_LEFT);
  Vector2 position = Vector2(screenPosition.x - anchorPointOffSet.x,
                             screenPosition.y - anchorPointOffSet.y);

  return {position.x, position.y, size.x, size.y};
}

int16_t AccessibleImpl::GetMdiZOrder()
{
  return 0;
}
double AccessibleImpl::GetAlpha()
{
  return 0;
}

bool AccessibleImpl::GrabFocus()
{
  return Toolkit::KeyboardFocusManager::Get().SetCurrentFocusActor(self);
}

static Dali::Actor CreateHighlightIndicatorActor()
{
  std::string focusBorderImagePath(AssetManager::GetDaliImagePath());
  focusBorderImagePath += "/keyboard_focus.9.png";
  // Create the default if it hasn't been set and one that's shared by all the
  // keyboard focusable actors
  auto actor = Toolkit::ImageView::New(focusBorderImagePath);
  actor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  DevelControl::AppendAccessibilityAttribute(actor, "highlight", "");
  actor.SetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_ANIMATED, true);
  actor.SetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, false);

  return actor;
}

bool AccessibleImpl::GrabHighlight()
{
  auto old = GetCurrentlyHighlightedActor();

  if(!Dali::Accessibility::IsUp())
    return false;
  if(self == old)
    return true;
  if(old)
  {
    auto c = dynamic_cast<Dali::Accessibility::Component*>(Internal::Control::Impl::GetAccessibilityObject(old));
    if(c)
      c->ClearHighlight();
  }
  auto highlight = GetHighlightActor();
  if(!highlight)
  {
    highlight = CreateHighlightIndicatorActor();
    SetHighlightActor(highlight);
  }
  highlight.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  highlight.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  highlight.SetProperty(Actor::Property::POSITION_Z, 1.0f);
  highlight.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 0.0f));

  // Remember the highlight actor, so that when the default is changed with
  // SetHighlightActor(), the currently displayed highlight can still be cleared.
  currentHighlightActor = highlight;
  EnsureSelfVisible();
  self.Add(highlight);
  SetCurrentlyHighlightedActor(self);
  EmitHighlighted(true);

  return true;
}

bool AccessibleImpl::ClearHighlight()
{
  if(!Dali::Accessibility::IsUp())
    return false;
  if(GetCurrentlyHighlightedActor() == self)
  {
    self.Remove(currentHighlightActor.GetHandle());
    currentHighlightActor = {};
    SetCurrentlyHighlightedActor({});
    EmitHighlighted(false);
    return true;
  }
  return false;
}

std::string AccessibleImpl::GetActionName(size_t index)
{
  if(index >= GetActionCount()) return "";
  Dali::TypeInfo type;
  self.GetTypeInfo(type);
  DALI_ASSERT_ALWAYS(type && "no TypeInfo object");
  return type.GetActionName(index);
}

std::string AccessibleImpl::GetLocalizedActionName(size_t index)
{
  // TODO: add localization
  return GetActionName(index);
}

std::string AccessibleImpl::GetActionDescription(size_t index)
{
  return "";
}

size_t AccessibleImpl::GetActionCount()
{
  Dali::TypeInfo type;
  self.GetTypeInfo(type);
  DALI_ASSERT_ALWAYS(type && "no TypeInfo object");
  return type.GetActionCount();
}

std::string AccessibleImpl::GetActionKeyBinding(size_t index)
{
  return "";
}

bool AccessibleImpl::DoAction(size_t index)
{
  std::string actionName = GetActionName(index);
  return self.DoAction(actionName, {});
}

bool AccessibleImpl::DoAction(const std::string& name)
{
  return self.DoAction(name, {});
}

bool AccessibleImpl::DoGesture(const Dali::Accessibility::GestureInfo& gestureInfo)
{
  auto control = Dali::Toolkit::Control::DownCast(self);

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

std::vector<Dali::Accessibility::Relation> AccessibleImpl::GetRelationSet()
{
  auto control = Dali::Toolkit::Control::DownCast(self);

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);

  std::vector<Dali::Accessibility::Relation> ret;

  auto& v = controlImpl.mAccessibilityRelations;
  for(auto i = 0u; i < v.size(); ++i)
  {
    if(v[i].empty())
      continue;

    ret.emplace_back(Accessibility::Relation{static_cast<Accessibility::RelationType>(i), v[i]});
  }

  return ret;
}

void AccessibleImpl::EnsureChildVisible(Actor child)
{
}

void AccessibleImpl::EnsureSelfVisible()
{
  auto parent = dynamic_cast<AccessibleImpl*>(GetParent());
  if(parent)
  {
    parent->EnsureChildVisible(self);
  }
}

Dali::Property::Index AccessibleImpl::GetNamePropertyIndex()
{
  return Actor::Property::NAME;
}

Dali::Property::Index AccessibleImpl::GetDescriptionPropertyIndex()
{
  return Dali::Property::INVALID_INDEX;
}

} // namespace Dali::Toolkit::DevelControl
