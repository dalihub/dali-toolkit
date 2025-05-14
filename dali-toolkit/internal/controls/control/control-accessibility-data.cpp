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
#include "control-accessibility-data.h"

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry-helper.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
static constexpr const char* READING_INFO_TYPE_NAME           = "name";
static constexpr const char* READING_INFO_TYPE_ATTRIBUTE_NAME = "reading_info_type";
static constexpr const char* READING_INFO_TYPE_ROLE           = "role";
static constexpr const char* READING_INFO_TYPE_DESCRIPTION    = "description";
static constexpr const char* READING_INFO_TYPE_STATE          = "state";
static constexpr const char* READING_INFO_TYPE_SEPARATOR      = "|";

Dali::Rect<> GetShowingGeometry(Dali::Rect<> rect, Dali::Toolkit::DevelControl::ControlAccessible* accessible)
{
  Rect<>  parentRect;
  Vector2 currentPosition;
  auto    parent = dynamic_cast<Toolkit::DevelControl::ControlAccessible*>(accessible->GetParent());

  while(parent)
  {
    parentRect = parent->GetExtents(Dali::Accessibility::CoordinateType::WINDOW);

    currentPosition.x = rect.x;
    currentPosition.y = rect.y;

    rect.x      = rect.x > parentRect.x ? rect.x : parentRect.x;
    rect.y      = rect.y > parentRect.y ? rect.y : parentRect.y;
    rect.width  = currentPosition.x + rect.width < parentRect.x + parentRect.width ? currentPosition.x + rect.width - rect.x : parentRect.x + parentRect.width - rect.x;
    rect.height = currentPosition.y + rect.height < parentRect.y + parentRect.height ? currentPosition.y + rect.height - rect.y : parentRect.y + parentRect.height - rect.y;

    if(rect.width < 0 || rect.height < 0)
    {
      return rect;
    }

    parent = dynamic_cast<Toolkit::DevelControl::ControlAccessible*>(parent->GetParent());
  }

  return rect;
}
static bool IsShowingGeometryOnScreen(Dali::Rect<> rect)
{
  return rect.width > 0 && rect.height > 0;
}

static constexpr uint32_t DEFAULT_READING_INFO_TYPES_RAW_DATA = (1u << static_cast<uint32_t>(Dali::Accessibility::ReadingInfoType::NAME)) |
                                                                (1u << static_cast<uint32_t>(Dali::Accessibility::ReadingInfoType::ROLE)) |
                                                                (1u << static_cast<uint32_t>(Dali::Accessibility::ReadingInfoType::DESCRIPTION)) |
                                                                (1u << static_cast<uint32_t>(Dali::Accessibility::ReadingInfoType::STATE));

static constexpr uint32_t DEFAULT_DEVEL_CONTROL_ACCESSIBILITY_STATES_RAW_DATA = (1u << static_cast<uint32_t>(DevelControl::AccessibilityState::ENABLED)) |
                                                                                (0u << static_cast<uint32_t>(DevelControl::AccessibilityState::SELECTED)) |
                                                                                (0u << static_cast<uint32_t>(DevelControl::AccessibilityState::CHECKED)) |
                                                                                (0u << static_cast<uint32_t>(DevelControl::AccessibilityState::BUSY)) |
                                                                                (0u << static_cast<uint32_t>(DevelControl::AccessibilityState::EXPANDED));

} // unnamed namespace

Control::Impl::AccessibilityData::AccessibilityData(Control& controlImpl)
: mAccessibilityGetNameSignal(),
  mAccessibilityGetDescriptionSignal(),
  mAccessibilityDoGestureSignal(),
  mControlImpl(controlImpl),
  mIsAccessibilityPositionPropertyNotificationSet(false),
  mIsAccessibilityPropertySetSignalRegistered(false)
{
  mAccessibilityProps.states = GetDefaultControlAccessibilityStates();
}

void Control::Impl::AccessibilityData::AppendAccessibilityAttribute(const std::string& key, const std::string value)
{
  Property::Value* checkedValue = mAccessibilityProps.extraAttributes.Find(key);
  if(checkedValue)
  {
    mAccessibilityProps.extraAttributes[key] = Property::Value(value);
  }
  else
  {
    mAccessibilityProps.extraAttributes.Insert(key, value);
  }
}

void Control::Impl::AccessibilityData::CheckHighlightedObjectGeometry()
{
  auto accessible = GetAccessibleObject();
  if(DALI_LIKELY(accessible))
  {
    auto lastPosition   = accessible->GetLastPosition();
    auto accessibleRect = accessible->GetExtents(Dali::Accessibility::CoordinateType::WINDOW);
    auto rect           = GetShowingGeometry(accessibleRect, accessible.get());

    switch(mAccessibilityLastScreenRelativeMoveType)
    {
      case Dali::Accessibility::ScreenRelativeMoveType::OUTSIDE:
      {
        if(IsShowingGeometryOnScreen(rect))
        {
          mAccessibilityLastScreenRelativeMoveType = Dali::Accessibility::ScreenRelativeMoveType::INSIDE;
        }
        break;
      }
      case Dali::Accessibility::ScreenRelativeMoveType::INSIDE:
      {
        if(rect.width < 0 && !Dali::Equals(accessibleRect.x, lastPosition.x))
        {
          mAccessibilityLastScreenRelativeMoveType = (accessibleRect.x < lastPosition.x) ? Dali::Accessibility::ScreenRelativeMoveType::OUTGOING_TOP_LEFT : Dali::Accessibility::ScreenRelativeMoveType::OUTGOING_BOTTOM_RIGHT;
        }
        if(rect.height < 0 && !Dali::Equals(accessibleRect.y, lastPosition.y))
        {
          mAccessibilityLastScreenRelativeMoveType = (accessibleRect.y < lastPosition.y) ? Dali::Accessibility::ScreenRelativeMoveType::OUTGOING_TOP_LEFT : Dali::Accessibility::ScreenRelativeMoveType::OUTGOING_BOTTOM_RIGHT;
        }
        // notify AT-clients on outgoing moves only
        if(mAccessibilityLastScreenRelativeMoveType != Dali::Accessibility::ScreenRelativeMoveType::INSIDE)
        {
          accessible->EmitMovedOutOfScreen(mAccessibilityLastScreenRelativeMoveType);
        }
        break;
      }
      case Dali::Accessibility::ScreenRelativeMoveType::OUTGOING_TOP_LEFT:
      case Dali::Accessibility::ScreenRelativeMoveType::OUTGOING_BOTTOM_RIGHT:
      {
        if(IsShowingGeometryOnScreen(rect))
        {
          mAccessibilityLastScreenRelativeMoveType = Dali::Accessibility::ScreenRelativeMoveType::INSIDE;
        }
        else
        {
          mAccessibilityLastScreenRelativeMoveType = Dali::Accessibility::ScreenRelativeMoveType::OUTSIDE;
        }
        break;
      }
      default:
      {
        break;
      }
    }

    accessible->SetLastPosition(Vector2(accessibleRect.x, accessibleRect.y));
  }
}

void Control::Impl::AccessibilityData::RegisterAccessibilityPositionPropertyNotification()
{
  if(mIsAccessibilityPositionPropertyNotificationSet)
  {
    return;
  }
  // set default value until first move of object is detected
  mAccessibilityLastScreenRelativeMoveType = Dali::Accessibility::ScreenRelativeMoveType::OUTSIDE;
  // recalculate mAccessibilityLastScreenRelativeMoveType accordingly to the initial position
  CheckHighlightedObjectGeometry();
  mAccessibilityPositionNotification = mControlImpl.Self().AddPropertyNotification(Actor::Property::WORLD_POSITION, StepCondition(1.0f, 1.0f));
  mAccessibilityPositionNotification.SetNotifyMode(PropertyNotification::NOTIFY_ON_CHANGED);
  mAccessibilityPositionNotification.NotifySignal().Connect(this, [this](PropertyNotification&) { CheckHighlightedObjectGeometry(); });
  mIsAccessibilityPositionPropertyNotificationSet = true;
}

void Control::Impl::AccessibilityData::UnregisterAccessibilityPositionPropertyNotification()
{
  mControlImpl.Self().RemovePropertyNotification(mAccessibilityPositionNotification);
  mIsAccessibilityPositionPropertyNotificationSet = false;
}

void Control::Impl::AccessibilityData::RegisterAccessibilityPropertySetSignal()
{
  if(mIsAccessibilityPropertySetSignalRegistered)
  {
    return;
  }
  mControlImpl.Self().PropertySetSignal().Connect(this, &Control::Impl::AccessibilityData::OnAccessibilityPropertySet);
  mIsAccessibilityPropertySetSignalRegistered = true;
}

void Control::Impl::AccessibilityData::UnregisterAccessibilityPropertySetSignal()
{
  if(!mIsAccessibilityPropertySetSignalRegistered)
  {
    return;
  }
  mControlImpl.Self().PropertySetSignal().Disconnect(this, &Control::Impl::AccessibilityData::OnAccessibilityPropertySet);
  mIsAccessibilityPropertySetSignalRegistered = false;
}

void Control::Impl::AccessibilityData::OnAccessibilityPropertySet(Dali::Handle& handle, Dali::Property::Index index, const Dali::Property::Value& value)
{
  auto accessible = GetAccessibleObject();
  if(DALI_LIKELY(accessible))
  {
    if(mAccessibilityGetNameSignal.Empty())
    {
      if(index == DevelControl::Property::ACCESSIBILITY_NAME || (mAccessibilityProps.name.empty() && index == accessible->GetNamePropertyIndex()))
      {
        accessible->Emit(Dali::Accessibility::ObjectPropertyChangeEvent::NAME);
        return;
      }
    }

    if(mAccessibilityGetDescriptionSignal.Empty())
    {
      if(index == DevelControl::Property::ACCESSIBILITY_DESCRIPTION || (mAccessibilityProps.description.empty() && index == accessible->GetDescriptionPropertyIndex()))
      {
        accessible->Emit(Dali::Accessibility::ObjectPropertyChangeEvent::DESCRIPTION);
        return;
      }
    }

    if(index == DevelControl::Property::ACCESSIBILITY_VALUE)
    {
      accessible->Emit(Dali::Accessibility::ObjectPropertyChangeEvent::VALUE);
      return;
    }

    if(index == DevelControl::Property::ACCESSIBILITY_STATES)
    {
      accessible->OnStatePropertySet(mAccessibilityProps.states);
      return;
    }
  }
}

Dali::Accessibility::ReadingInfoTypes Control::Impl::AccessibilityData::GetAccessibilityReadingInfoType() const
{
  std::string value{};
  auto        place = mAccessibilityProps.extraAttributes.Find(READING_INFO_TYPE_ATTRIBUTE_NAME);
  if(place)
  {
    place->Get(value);
  }
  else
  {
    return GetDefaultReadingInfoTypes();
  }

  if(value.empty())
  {
    return {};
  }

  Dali::Accessibility::ReadingInfoTypes types;

  if(value.find(READING_INFO_TYPE_NAME) != std::string::npos)
  {
    types[Dali::Accessibility::ReadingInfoType::NAME] = true;
  }
  if(value.find(READING_INFO_TYPE_ROLE) != std::string::npos)
  {
    types[Dali::Accessibility::ReadingInfoType::ROLE] = true;
  }
  if(value.find(READING_INFO_TYPE_DESCRIPTION) != std::string::npos)
  {
    types[Dali::Accessibility::ReadingInfoType::DESCRIPTION] = true;
  }
  if(value.find(READING_INFO_TYPE_STATE) != std::string::npos)
  {
    types[Dali::Accessibility::ReadingInfoType::STATE] = true;
  }

  return types;
}

void Control::Impl::AccessibilityData::RemoveAccessibilityAttribute(const std::string& key)
{
  Property::Value* value = mAccessibilityProps.extraAttributes.Find(key);
  if(value)
  {
    mAccessibilityProps.extraAttributes[key] = Property::Value();
  }
}

void Control::Impl::AccessibilityData::ClearAccessibilityAttributes()
{
  mAccessibilityProps.extraAttributes.Clear();
}

void Control::Impl::AccessibilityData::SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types)
{
  std::string value{};
  if(types[Dali::Accessibility::ReadingInfoType::NAME])
  {
    value += READING_INFO_TYPE_NAME;
  }
  if(types[Dali::Accessibility::ReadingInfoType::ROLE])
  {
    if(!value.empty())
    {
      value += READING_INFO_TYPE_SEPARATOR;
    }
    value += READING_INFO_TYPE_ROLE;
  }
  if(types[Dali::Accessibility::ReadingInfoType::DESCRIPTION])
  {
    if(!value.empty())
    {
      value += READING_INFO_TYPE_SEPARATOR;
    }
    value += READING_INFO_TYPE_DESCRIPTION;
  }
  if(types[Dali::Accessibility::ReadingInfoType::STATE])
  {
    if(!value.empty())
    {
      value += READING_INFO_TYPE_SEPARATOR;
    }
    value += READING_INFO_TYPE_STATE;
  }
  AppendAccessibilityAttribute(READING_INFO_TYPE_ATTRIBUTE_NAME, value);
}

std::shared_ptr<Toolkit::DevelControl::ControlAccessible> Control::Impl::AccessibilityData::GetAccessibleObject()
{
  return std::dynamic_pointer_cast<DevelControl::ControlAccessible>(Accessibility::Accessible::GetOwningPtr(mControlImpl.Self()));
}

Dali::Accessibility::ReadingInfoTypes Control::Impl::AccessibilityData::GetDefaultReadingInfoTypes()
{
  return Dali::Accessibility::ReadingInfoTypes{DEFAULT_READING_INFO_TYPES_RAW_DATA};
}

Toolkit::DevelControl::AccessibilityStates Control::Impl::AccessibilityData::GetDefaultControlAccessibilityStates()
{
  return Toolkit::DevelControl::AccessibilityStates{DEFAULT_DEVEL_CONTROL_ACCESSIBILITY_STATES_RAW_DATA};
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
