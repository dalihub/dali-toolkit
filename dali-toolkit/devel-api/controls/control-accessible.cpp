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
#include <dali/public-api/object/type-info.h>

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

constexpr const char* ATTR_IMG_SRC_KEY = "imgSrc";

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

Dali::Actor CreateHighlightIndicatorActor()
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
  return Self().GetProperty<Dali::Accessibility::Role>(Toolkit::DevelControl::Property::ACCESSIBILITY_ROLE);
}

std::string ControlAccessible::GetLocalizedRoleName() const
{
  return GetLocaleText(GetRoleName());
}

bool ControlAccessible::IsShowing()
{
  Dali::Actor self = Self();
  if(!self.GetProperty<bool>(Actor::Property::VISIBLE) || Dali::EqualsZero(self.GetProperty<Vector4>(Actor::Property::WORLD_COLOR).a) || self.GetProperty<bool>(Dali::DevelActor::Property::CULLED))
  {
    return false;
  }

  auto* child  = this;
  auto* parent = dynamic_cast<Toolkit::DevelControl::ControlAccessible*>(child->GetParent());
  if(!parent)
  {
    return true;
  }

  while(parent)
  {
    auto control = Dali::Toolkit::Control::DownCast(parent->Self());
    if(!control.GetProperty<bool>(Actor::Property::VISIBLE))
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

  Dali::Actor                 self = Self();
  Dali::Accessibility::States states;

  states[State::FOCUSABLE]     = self.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE);
  states[State::FOCUSED]       = Toolkit::KeyboardFocusManager::Get().GetCurrentFocusActor() == self;
  states[State::HIGHLIGHTABLE] = self.GetProperty<bool>(Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE);
  states[State::HIGHLIGHTED]   = IsHighlighted();
  states[State::ENABLED]       = true;
  states[State::SENSITIVE]     = (Dali::DevelActor::IsHittable(self) && Dali::DevelActor::GetTouchRequired(self));
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
  static const std::string automationIdKey = "automationId";
  static const std::string classKey        = "class";

  Accessibility::Attributes result;
  Toolkit::Control          control        = Toolkit::Control::DownCast(Self());
  Dali::Property::Value     property       = control.GetProperty(DevelControl::Property::ACCESSIBILITY_ATTRIBUTES);
  Dali::Property::Map*      attributeMap   = property.GetMap();
  std::size_t               attributeCount = attributeMap ? attributeMap->Count() : 0U;

  for(std::size_t i = 0; i < attributeCount; i++)
  {
    Dali::Property::Key mapKey = attributeMap->GetKeyAt(i);
    std::string         mapValue;

    if(mapKey.type == Dali::Property::Key::STRING && attributeMap->GetValue(i).Get(mapValue))
    {
      result.emplace(std::move(mapKey.stringKey), std::move(mapValue));
    }
  }

  auto automationId = control.GetProperty<std::string>(DevelControl::Property::AUTOMATION_ID);
  if(!automationId.empty())
  {
    result.emplace(automationIdKey, std::move(automationId));
  }

  if(auto imageView = Toolkit::ImageView::DownCast(Self()))
  {
    auto imageSrc = FetchImageSrc(imageView);
    if(!imageSrc.empty())
    {
      result.emplace(ATTR_IMG_SRC_KEY, std::move(imageSrc));
    }
  }

  // Add "class" if not present already
  if(result.find(classKey) == result.end())
  {
    Dali::TypeInfo typeInfo;
    Self().GetTypeInfo(typeInfo);
    if(typeInfo)
    {
      const std::string& typeName = typeInfo.GetName();

      result.emplace(classKey, typeName);

      // Save the 'typeName' so we don't have to calculate it again
      DevelControl::AppendAccessibilityAttribute(control, classKey, typeName);
    }
  }

  return result;
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
}

void ControlAccessible::UnregisterPropertySetSignal()
{
  auto                     control         = Dali::Toolkit::Control::DownCast(Self());
  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);
  controlImpl.UnregisterAccessibilityPropertySetSignal();
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
  if(!Dali::Accessibility::IsUp())
  {
    return false;
  }

  if(IsHighlighted())
  {
    UnregisterPropertySetSignal();
    UnregisterPositionPropertyNotification();
    Self().Remove(mCurrentHighlightActor.GetHandle());
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

} // namespace Dali::Toolkit::DevelControl
