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
#ifdef DGETTEXT_ENABLED
#include <libintl.h>
#endif

#include <dali/devel-api/actors/actor-devel.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/asset-manager/asset-manager.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>

namespace Dali::Toolkit::DevelControl
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

AccessibleImpl::AccessibleImpl(Dali::Actor self, Dali::Accessibility::Role role, bool modal)
: mSelf(self),
  mIsModal(modal)
{
  auto control = Dali::Toolkit::Control::DownCast(Self());

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);
  if(controlImpl.mAccessibilityRole == Dali::Accessibility::Role::UNKNOWN)
  {
    controlImpl.mAccessibilityRole = role;
  }

  Self().PropertySetSignal().Connect(&controlImpl, [this, &controlImpl](Dali::Handle& handle, Dali::Property::Index index, Dali::Property::Value value) {
    if(this->Self() != Dali::Accessibility::Accessible::GetCurrentlyHighlightedActor())
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
  auto control = Dali::Toolkit::Control::DownCast(Self());

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);
  std::string name;

  if(!controlImpl.mAccessibilityGetNameSignal.Empty())
  {
    controlImpl.mAccessibilityGetNameSignal.Emit(name);
  }
  else if(controlImpl.mAccessibilityNameSet)
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

  if(controlImpl.mAccessibilityTranslationDomainSet)
  {
    return GetLocaleText(name, controlImpl.mAccessibilityTranslationDomain.c_str());
  }

  return GetLocaleText(name);
}

std::string AccessibleImpl::GetNameRaw()
{
  return {};
}

std::string AccessibleImpl::GetDescription()
{
  auto control = Dali::Toolkit::Control::DownCast(Self());

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);
  std::string description;

  if(!controlImpl.mAccessibilityGetDescriptionSignal.Empty())
  {
    controlImpl.mAccessibilityGetDescriptionSignal.Emit(description);
  }
  else if(controlImpl.mAccessibilityDescriptionSet)
  {
    description = controlImpl.mAccessibilityDescription;
  }
  else
  {
    description = GetDescriptionRaw();
  }
  if(controlImpl.mAccessibilityTranslationDomainSet)
  {
    return GetLocaleText(description, controlImpl.mAccessibilityTranslationDomain.c_str());
  }

  return GetLocaleText(description);
}

std::string AccessibleImpl::GetDescriptionRaw()
{
  return {};
}

Dali::Accessibility::Accessible* AccessibleImpl::GetParent()
{
  return Dali::Accessibility::Accessible::Get(Self().GetParent());
}

size_t AccessibleImpl::GetChildCount()
{
  return Self().GetChildCount();
}

Dali::Accessibility::Accessible* AccessibleImpl::GetChildAtIndex(size_t index)
{
  return Dali::Accessibility::Accessible::Get(Self().GetChildAt(static_cast<unsigned int>(index)));
}

size_t AccessibleImpl::GetIndexInParent()
{
  auto self = Self();
  auto parent = self.GetParent();
  DALI_ASSERT_ALWAYS(parent && "can't call GetIndexInParent on object without parent");

  auto count = parent.GetChildCount();
  for(auto i = 0u; i < count; ++i)
  {
    auto child = parent.GetChildAt(i);
    if(child == self)
    {
      return i;
    }
  }
  DALI_ASSERT_ALWAYS(false && "object isn't child of it's parent");
  return static_cast<size_t>(-1);
}

Dali::Accessibility::Role AccessibleImpl::GetRole()
{
  return Self().GetProperty<Dali::Accessibility::Role>(Toolkit::DevelControl::Property::ACCESSIBILITY_ROLE);
}

std::string AccessibleImpl::GetLocalizedRoleName()
{
  return GetLocaleText(GetRoleName());
}

bool AccessibleImpl::IsShowing()
{
  Dali::Actor self = Self();
  if(self.GetProperty(Dali::DevelActor::Property::CULLED).Get<bool>() || !self.GetCurrentProperty<bool>(Actor::Property::VISIBLE))
  {
    return false;
  }

  auto* child  = this;
  auto* parent = dynamic_cast<Toolkit::DevelControl::AccessibleImpl*>(child->GetParent());
  if(!parent)
  {
    return true;
  }

  auto childExtent = child->GetExtents(Dali::Accessibility::CoordinateType::WINDOW);
  while(parent)
  {
    auto control      = Dali::Toolkit::Control::DownCast(parent->Self());
    auto clipMode     = control.GetProperty(Actor::Property::CLIPPING_MODE).Get<bool>();
    auto parentExtent = parent->GetExtents(Dali::Accessibility::CoordinateType::WINDOW);
    if ((clipMode != ClippingMode::DISABLED) && !parentExtent.Intersects(childExtent))
    {
      return false;
    }
    parent = dynamic_cast<Toolkit::DevelControl::AccessibleImpl*>(parent->GetParent());
  }

  return true;
}

Dali::Accessibility::States AccessibleImpl::CalculateStates()
{
  Dali::Actor self = Self();
  Dali::Accessibility::States state;
  state[Dali::Accessibility::State::FOCUSABLE] = self.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE);
  state[Dali::Accessibility::State::FOCUSED]   = Toolkit::KeyboardFocusManager::Get().GetCurrentFocusActor() == self;

  if(self.GetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE).GetType() == Dali::Property::NONE)
  {
    state[Dali::Accessibility::State::HIGHLIGHTABLE] = false;
  }
  else
  {
    state[Dali::Accessibility::State::HIGHLIGHTABLE] = self.GetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE).Get<bool>();
  }

  state[Dali::Accessibility::State::HIGHLIGHTED] = GetCurrentlyHighlightedActor() == self;
  state[Dali::Accessibility::State::ENABLED]     = true;
  state[Dali::Accessibility::State::SENSITIVE]   = true;
  state[Dali::Accessibility::State::VISIBLE]     = true;

  if(mIsModal)
  {
    state[Dali::Accessibility::State::MODAL] = true;
  }
  state[Dali::Accessibility::State::SHOWING] = IsShowing();
  state[Dali::Accessibility::State::DEFUNCT] = !self.GetProperty(Dali::DevelActor::Property::CONNECTED_TO_SCENE).Get<bool>();
  return state;
}

Dali::Accessibility::States AccessibleImpl::GetStates()
{
  return CalculateStates();
}

Dali::Accessibility::Attributes AccessibleImpl::GetAttributes()
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

Dali::Accessibility::ComponentLayer AccessibleImpl::GetLayer()
{
  return Dali::Accessibility::ComponentLayer::WINDOW;
}

Dali::Rect<> AccessibleImpl::GetExtents(Dali::Accessibility::CoordinateType type)
{
  Dali::Actor self = Self();

  Vector2 screenPosition = self.GetProperty(Dali::DevelActor::Property::SCREEN_POSITION).Get<Vector2>();
  auto size = self.GetCurrentProperty<Vector3>(Actor::Property::SIZE) * self.GetCurrentProperty<Vector3>(Actor::Property::WORLD_SCALE);
  bool positionUsesAnchorPoint = self.GetProperty(Dali::DevelActor::Property::POSITION_USES_ANCHOR_POINT).Get<bool>();
  Vector3 anchorPointOffSet = size * (positionUsesAnchorPoint ? self.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT) : AnchorPoint::TOP_LEFT);
  Vector2 position = Vector2((screenPosition.x - anchorPointOffSet.x), (screenPosition.y - anchorPointOffSet.y));

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
  return Toolkit::KeyboardFocusManager::Get().SetCurrentFocusActor(Self());
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

void AccessibleImpl::ScrollToSelf()
{
  auto* child = this;
  auto* parent = dynamic_cast<Toolkit::DevelControl::AccessibleImpl*>(child->GetParent());

  while (parent)
  {
    if (parent->IsScrollable())
    {
      parent->ScrollToChild(child->Self());
    }

    child = parent;
    parent = dynamic_cast<Toolkit::DevelControl::AccessibleImpl*>(parent->GetParent());
  }
}

void AccessibleImpl::RegisterPositionPropertyNotification()
{
  auto                     control         = Dali::Toolkit::Control::DownCast(Self());
  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);
  controlImpl.RegisterAccessibilityPositionPropertyNotification();
}

void AccessibleImpl::UnregisterPositionPropertyNotification()
{
  auto                     control         = Dali::Toolkit::Control::DownCast(Self());
  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);
  controlImpl.UnregisterAccessibilityPositionPropertyNotification();
}

bool AccessibleImpl::GrabHighlight()
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
    auto oldHighlightObject = dynamic_cast<Dali::Accessibility::Component*>(Internal::Control::Impl::GetAccessibilityObject(oldHighlightedActor));
    if(oldHighlightObject)
    {
      oldHighlightObject->ClearHighlight();
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
  // AccessibleImpl_NUI. The highlight could move from AccessibleImpl_NUI to
  // AccessibleImpl. In this case, highlight has incorrect size.
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

bool AccessibleImpl::ClearHighlight()
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

std::string AccessibleImpl::GetActionName(size_t index)
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

std::string AccessibleImpl::GetLocalizedActionName(size_t index)
{
  return GetLocaleText(GetActionName(index));
}

std::string AccessibleImpl::GetActionDescription(size_t index)
{
  return {};
}

size_t AccessibleImpl::GetActionCount()
{
  Dali::TypeInfo type;
  Self().GetTypeInfo(type);
  DALI_ASSERT_ALWAYS(type && "no TypeInfo object");
  return type.GetActionCount();
}

std::string AccessibleImpl::GetActionKeyBinding(size_t index)
{
  return {};
}

bool AccessibleImpl::DoAction(size_t index)
{
  std::string actionName = GetActionName(index);
  return Self().DoAction(actionName, {});
}

bool AccessibleImpl::DoAction(const std::string& name)
{
  return Self().DoAction(name, {});
}

bool AccessibleImpl::DoGesture(const Dali::Accessibility::GestureInfo& gestureInfo)
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

std::vector<Dali::Accessibility::Relation> AccessibleImpl::GetRelationSet()
{
  auto control = Dali::Toolkit::Control::DownCast(Self());

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);

  std::vector<Dali::Accessibility::Relation> ret;

  auto& relation = controlImpl.mAccessibilityRelations;
  for(auto i = 0u; i < relation.size(); ++i)
  {
    if(relation[i].empty()) continue;

    ret.emplace_back(Accessibility::Relation{static_cast<Accessibility::RelationType>(i), relation[i]});
  }

  return ret;
}

bool AccessibleImpl::ScrollToChild(Actor child)
{
  return false;
}

Dali::Property::Index AccessibleImpl::GetNamePropertyIndex()
{
  return Actor::Property::NAME;
}

Dali::Property::Index AccessibleImpl::GetDescriptionPropertyIndex()
{
  return Dali::Property::INVALID_INDEX;
}

void AccessibleImpl::SetLastPosition(Vector2 position)
{
  mLastPosition = position;
}

Vector2 AccessibleImpl::GetLastPosition() const
{
  return mLastPosition;
}

} // namespace Dali::Toolkit::DevelControl
