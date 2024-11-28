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
#include "control-accessible.h"

// EXTERNAL INCLUDES
#ifdef DGETTEXT_ENABLED
#include <libintl.h>
#endif

#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/asset-manager/asset-manager.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/visuals/image/image-visual.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>

namespace Dali::Toolkit::DevelControl
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_CONTROL_ACCESSIBLE");
#endif

std::string GetLocaleText(std::string string, const char* domain = "dali-toolkit")
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

bool IsAtspiRole(int32_t rawRole)
{
  return rawRole >= static_cast<int32_t>(Dali::Accessibility::Role::INVALID) && rawRole < static_cast<int32_t>(Dali::Accessibility::Role::MAX_COUNT);
}

bool IsRoleV2(int32_t rawRole)
{
  return rawRole >= static_cast<int32_t>(ROLE_START_INDEX) && rawRole < static_cast<int32_t>(AccessibilityRole::MAX_COUNT);
}

#define TO_V1_ROLE_TYPE(v2RoleType, v1RoleType) \
  case AccessibilityRole::v2RoleType:           \
  {                                             \
    return Role::v1RoleType;                    \
  }
#define TO_SAME_ROLE_TYPE(roleType) \
  case AccessibilityRole::roleType: \
  {                                 \
    return Role::roleType;          \
  }

Dali::Accessibility::Role ConvertV2RoleToAtspiRole(AccessibilityRole role)
{
  using Dali::Accessibility::Role;
  switch(role)
  {
    TO_V1_ROLE_TYPE(ADJUSTABLE, SLIDER)
    TO_SAME_ROLE_TYPE(ALERT)
    TO_V1_ROLE_TYPE(BUTTON, PUSH_BUTTON)
    TO_SAME_ROLE_TYPE(CHECK_BOX)
    TO_SAME_ROLE_TYPE(COMBO_BOX)
    TO_V1_ROLE_TYPE(CONTAINER, FILLER)
    TO_SAME_ROLE_TYPE(DIALOG)
    TO_SAME_ROLE_TYPE(ENTRY)
    TO_SAME_ROLE_TYPE(HEADER)
    TO_SAME_ROLE_TYPE(IMAGE)
    TO_SAME_ROLE_TYPE(LINK)
    TO_SAME_ROLE_TYPE(LIST)
    TO_SAME_ROLE_TYPE(LIST_ITEM)
    TO_SAME_ROLE_TYPE(MENU)
    TO_SAME_ROLE_TYPE(MENU_BAR)
    TO_SAME_ROLE_TYPE(MENU_ITEM)
    TO_V1_ROLE_TYPE(NONE, UNKNOWN)
    TO_SAME_ROLE_TYPE(PASSWORD_TEXT)
    TO_SAME_ROLE_TYPE(POPUP_MENU)
    TO_SAME_ROLE_TYPE(PROGRESS_BAR)
    TO_SAME_ROLE_TYPE(RADIO_BUTTON)
    TO_SAME_ROLE_TYPE(SCROLL_BAR)
    TO_SAME_ROLE_TYPE(SPIN_BUTTON)
    TO_V1_ROLE_TYPE(TAB, PAGE_TAB)
    TO_V1_ROLE_TYPE(TAB_LIST, PAGE_TAB_LIST)
    TO_V1_ROLE_TYPE(TEXT, LABEL)
    TO_SAME_ROLE_TYPE(TOGGLE_BUTTON)
    TO_SAME_ROLE_TYPE(TOOL_BAR)
    default:
    {
      return Role::UNKNOWN;
    }
  }
}

Dali::Accessibility::Role ConvertRawRoleToAtspiRole(int32_t rawRole)
{
  if(IsAtspiRole(rawRole))
  {
    return static_cast<Dali::Accessibility::Role>(rawRole);
  }
  else if(IsRoleV2(rawRole))
  {
    return ConvertV2RoleToAtspiRole(static_cast<AccessibilityRole>(rawRole));
  }
  else
  {
    return Dali::Accessibility::Role::UNKNOWN;
  }
}

bool IsModalRole(int32_t rawRole)
{
  return IsRoleV2(rawRole) && (static_cast<AccessibilityRole>(rawRole) == AccessibilityRole::ALERT ||
                               static_cast<AccessibilityRole>(rawRole) == AccessibilityRole::DIALOG ||
                               static_cast<AccessibilityRole>(rawRole) == AccessibilityRole::POPUP_MENU);
}

bool IsHighlightableRole(int32_t rawRole)
{
  return IsRoleV2(rawRole) && static_cast<AccessibilityRole>(rawRole) != AccessibilityRole::NONE;
}

using Dali::Toolkit::Internal::TriStateProperty;
bool IsHighlightable(TriStateProperty highlightable, int32_t rawRole)
{
  switch(highlightable)
  {
    case TriStateProperty::AUTO:
    {
      return IsHighlightableRole(rawRole);
    }
    case TriStateProperty::TRUE:
    {
      return true;
    }
    default:
    {
      return false;
    }
  }
}

std::string FetchImageSrcFromMap(const Dali::Property::Map& imageMap)
{
  auto urlVal = imageMap.Find(Toolkit::ImageVisual::Property::URL);
  if(urlVal)
  {
    if(urlVal->GetType() == Dali::Property::STRING)
    {
      return urlVal->Get<std::string>();
    }
    else if(urlVal->GetType() == Dali::Property::ARRAY)
    {
      auto urlArray = urlVal->GetArray();
      if(urlArray && !urlArray->Empty())
      {
        // Returns first element if url is an array
        return (*urlArray)[0].Get<std::string>();
      }
    }
  }
  return {};
}

std::string FetchImageSrc(const Toolkit::ImageView& imageView)
{
  const auto imageUrl = imageView.GetProperty<std::string>(Toolkit::ImageView::Property::IMAGE);
  if(!imageUrl.empty())
  {
    return imageUrl;
  }

  const auto imageMap = imageView.GetProperty<Dali::Property::Map>(Toolkit::ImageView::Property::IMAGE);
  if(!imageMap.Empty())
  {
    return FetchImageSrcFromMap(imageMap);
  }
  return {};
}

} // unnamed namespace

ControlAccessible::ControlAccessible(Dali::Actor self)
: ActorAccessible(self)
{
}

std::string ControlAccessible::GetName() const
{
  auto control = Dali::Toolkit::Control::DownCast(Self());

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);
  std::string              name;

  if(!controlImpl.mAccessibilityGetNameSignal.Empty())
  {
    controlImpl.mAccessibilityGetNameSignal.Emit(name);
  }
  else if(!controlImpl.mAccessibilityProps.name.empty())
  {
    name = controlImpl.mAccessibilityProps.name;
  }
  else if(auto raw = GetNameRaw(); !raw.empty())
  {
    name = raw;
  }
  else
  {
    name = Self().GetProperty<std::string>(Actor::Property::NAME);
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
  std::string              description;

  if(!controlImpl.mAccessibilityGetDescriptionSignal.Empty())
  {
    controlImpl.mAccessibilityGetDescriptionSignal.Emit(description);
  }
  else if(!controlImpl.mAccessibilityProps.description.empty())
  {
    description = controlImpl.mAccessibilityProps.description;
  }
  else
  {
    description = GetDescriptionRaw();
  }

  return GetLocaleText(description);
}

std::string ControlAccessible::GetDescriptionRaw() const
{
  return {};
}

std::string ControlAccessible::GetValue() const
{
  return Self().GetProperty<std::string>(Toolkit::DevelControl::Property::ACCESSIBILITY_VALUE);
}

Dali::Accessibility::Role ControlAccessible::GetRole() const
{
  int32_t rawRole = Self().GetProperty<int32_t>(Toolkit::DevelControl::Property::ACCESSIBILITY_ROLE);
  return ConvertRawRoleToAtspiRole(rawRole);
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

  Dali::Actor parent = self.GetParent();
  if(!parent)
  {
    return true;
  }

  while(parent)
  {
    if(!parent.GetProperty<bool>(Actor::Property::VISIBLE))
    {
      return false;
    }
    parent = parent.GetParent();
  }

  return true;
}

void ControlAccessible::ApplyAccessibilityProps(Dali::Accessibility::States& states)
{
  using Dali::Accessibility::State;
  auto control = Dali::Toolkit::Control::DownCast(Self());

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);

  // Apply states
  const auto& props       = controlImpl.mAccessibilityProps;
  states[State::ENABLED]  = props.states[AccessibilityState::ENABLED];
  states[State::SELECTED] = props.states[AccessibilityState::SELECTED];
  states[State::CHECKED]  = props.states[AccessibilityState::CHECKED];
  states[State::BUSY]     = props.states[AccessibilityState::BUSY];
  states[State::EXPANDED] = props.states[AccessibilityState::EXPANDED];

  // Apply traits
  states[State::MODAL]         = props.isModal || IsModalRole(props.role);
  states[State::HIGHLIGHTABLE] = IsHighlightable(props.isHighlightable, props.role);
}

Dali::Accessibility::States ControlAccessible::CalculateStates()
{
  using Dali::Accessibility::State;

  Dali::Actor                 self = Self();
  Dali::Accessibility::States states;

  states[State::FOCUSABLE]   = self.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE);
  states[State::FOCUSED]     = Toolkit::KeyboardFocusManager::Get().GetCurrentFocusActor() == self;
  states[State::HIGHLIGHTED] = GetCurrentlyHighlightedActor() == self;
  states[State::SENSITIVE]   = (Dali::DevelActor::IsHittable(self) && Dali::DevelActor::GetTouchRequired(self));
  states[State::VISIBLE]     = self.GetProperty<bool>(Actor::Property::VISIBLE);
  states[State::SHOWING]     = IsShowing();
  states[State::DEFUNCT]     = !self.GetProperty(Dali::DevelActor::Property::CONNECTED_TO_SCENE).Get<bool>();

  ApplyAccessibilityProps(states);

  return states;
}

Dali::Accessibility::States ControlAccessible::GetStates()
{
  return CalculateStates();
}

void ControlAccessible::UpdateAttributes(Accessibility::Attributes& attributes) const
{
  static const std::string automationIdKey = "automationId";

  ActorAccessible::UpdateAttributes(attributes);

  Toolkit::Control control      = Toolkit::Control::DownCast(Self());
  auto             automationId = control.GetProperty<std::string>(DevelControl::Property::AUTOMATION_ID);

  if(automationId.empty())
  {
    attributes.erase(automationIdKey);
  }
  else
  {
    attributes.insert_or_assign(automationIdKey, std::move(automationId));
  }

  static const std::string imgSrcKey = "imgSrc";

  if(auto imageView = Toolkit::ImageView::DownCast(Self()))
  {
    auto imageSrc = FetchImageSrc(imageView);
    attributes.insert_or_assign(imgSrcKey, std::move(imageSrc));
  }
}

bool ControlAccessible::IsHidden() const
{
  auto control = Dali::Toolkit::Control::DownCast(Self());

  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);

  return controlImpl.mAccessibilityProps.isHidden;
}

bool ControlAccessible::GrabFocus()
{
  return Toolkit::KeyboardFocusManager::Get().SetCurrentFocusActor(Self());
}

void ControlAccessible::ScrollToSelf()
{
  auto* child  = this;
  auto* parent = dynamic_cast<Toolkit::DevelControl::ControlAccessible*>(child->GetParent());

  while(parent)
  {
    if(parent->IsScrollable())
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

void ControlAccessible::RegisterPropertySetSignal()
{
  auto                     control         = Dali::Toolkit::Control::DownCast(Self());
  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);
  controlImpl.RegisterAccessibilityPropertySetSignal();

  mStatesSnapshot = controlImpl.mAccessibilityProps.states;
}

void ControlAccessible::UnregisterPropertySetSignal()
{
  auto                     control         = Dali::Toolkit::Control::DownCast(Self());
  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);
  controlImpl.UnregisterAccessibilityPropertySetSignal();

  mStatesSnapshot = {};
}

bool ControlAccessible::GrabHighlight()
{
  Dali::Actor self                = Self();
  auto        oldHighlightedActor = GetCurrentlyHighlightedActor();

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
  RegisterPropertySetSignal();

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
    UnregisterPropertySetSignal();
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

  return DevelControl::GetAccessibilityRelations(control);
}

bool ControlAccessible::IsScrollable() const
{
  return Self().GetProperty<bool>(Toolkit::DevelControl::Property::ACCESSIBILITY_SCROLLABLE);
}

bool ControlAccessible::ScrollToChild(Actor child)
{
  auto control = Dali::Toolkit::Control::DownCast(Self());
  bool success = false;

  if(!DevelControl::AccessibilityActionSignal(control).Empty())
  {
    success = DevelControl::AccessibilityActionSignal(control).Emit({Accessibility::ActionType::SCROLL_TO_CHILD, child});
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Performed AccessibilityAction: scrollToChild, success : %d\n", success);
  }

  return success;
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

void ControlAccessible::OnStatePropertySet(AccessibilityStates newStates)
{
  int32_t rawRole = Self().GetProperty<int32_t>(Property::ACCESSIBILITY_ROLE);
  if(IsRoleV2(rawRole))
  {
    AccessibilityRole role = static_cast<AccessibilityRole>(rawRole);

    if(newStates[AccessibilityState::CHECKED] != mStatesSnapshot[AccessibilityState::CHECKED] &&
       (role == AccessibilityRole::CHECK_BOX || role == AccessibilityRole::RADIO_BUTTON || role == AccessibilityRole::TOGGLE_BUTTON))
    {
      EmitStateChanged(Accessibility::State::CHECKED, newStates[AccessibilityState::CHECKED]);
    }

    if(newStates[AccessibilityState::SELECTED] != mStatesSnapshot[AccessibilityState::SELECTED] &&
       (role == AccessibilityRole::BUTTON || role == AccessibilityRole::LIST_ITEM || role == AccessibilityRole::MENU_ITEM))
    {
      EmitStateChanged(Accessibility::State::SELECTED, newStates[AccessibilityState::SELECTED]);
    }
  }
  else
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "With V1 role, state change events are emitted manually by the app component.");
  }

  mStatesSnapshot = newStates;
}

bool ControlAccessible::IsModal(Actor actor)
{
  bool isModalPropertySet = actor.GetProperty<bool>(Property::ACCESSIBILITY_IS_MODAL);
  if(isModalPropertySet)
  {
    return true;
  }

  int32_t rawRole = actor.GetProperty<int32_t>(Property::ACCESSIBILITY_ROLE);
  return IsModalRole(rawRole);
}

} // namespace Dali::Toolkit::DevelControl
