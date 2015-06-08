/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/focus-manager/accessibility-focus-manager.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/singleton-service.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/focus-manager/accessibility-focus-manager-impl.h>

namespace Dali
{

namespace Toolkit
{

AccessibilityFocusManager::AccessibilityFocusManager()
{
}

AccessibilityFocusManager::~AccessibilityFocusManager()
{
}

AccessibilityFocusManager AccessibilityFocusManager::Get()
{
  AccessibilityFocusManager manager;

  // Check whether the accessibility focus manager is already created
  SingletonService singletonService( SingletonService::Get() );
  if ( singletonService )
  {
    Dali::BaseHandle handle = singletonService.GetSingleton(typeid(AccessibilityFocusManager));
    if(handle)
    {
      // If so, downcast the handle of singleton to focus manager
      manager = AccessibilityFocusManager(dynamic_cast<Internal::AccessibilityFocusManager*>(handle.GetObjectPtr()));
    }

    if(!manager)
    {
      // If not, create the accessibility focus manager and register it as a singleton
      manager = AccessibilityFocusManager(new Internal::AccessibilityFocusManager());
      singletonService.Register(typeid(manager), manager);
    }
  }

  return manager;
}

AccessibilityFocusManager::AccessibilityFocusManager(Internal::AccessibilityFocusManager *impl)
  : BaseHandle(impl)
{
}

void AccessibilityFocusManager::SetAccessibilityAttribute(Actor actor, AccessibilityAttribute type, const std::string& text)
{
  GetImpl(*this).SetAccessibilityAttribute(actor, type, text);
}

std::string AccessibilityFocusManager::GetAccessibilityAttribute(Actor actor, AccessibilityAttribute type) const
{
  return GetImpl(*this).GetAccessibilityAttribute(actor, type);
}

void AccessibilityFocusManager::SetFocusOrder(Actor actor, const unsigned int order)
{
  GetImpl(*this).SetFocusOrder(actor, order);
}

unsigned int AccessibilityFocusManager::GetFocusOrder(Actor actor) const
{
  return GetImpl(*this).GetFocusOrder(actor);
}

unsigned int AccessibilityFocusManager::GenerateNewFocusOrder() const
{
  return GetImpl(*this).GenerateNewFocusOrder();
}

Actor AccessibilityFocusManager::GetActorByFocusOrder(const unsigned int order)
{
  return GetImpl(*this).GetActorByFocusOrder(order);
}

bool AccessibilityFocusManager::SetCurrentFocusActor(Actor actor)
{
  return GetImpl(*this).SetCurrentFocusActor(actor);
}

Actor AccessibilityFocusManager::GetCurrentFocusActor()
{
  return GetImpl(*this).GetCurrentFocusActor();
}

Actor AccessibilityFocusManager::GetCurrentFocusGroup()
{
  return GetImpl(*this).GetCurrentFocusGroup();
}

unsigned int AccessibilityFocusManager::GetCurrentFocusOrder()
{
  return GetImpl(*this).GetCurrentFocusOrder();
}

bool AccessibilityFocusManager::MoveFocusForward()
{
  return GetImpl(*this).MoveFocusForward();
}

bool AccessibilityFocusManager::MoveFocusBackward()
{
  return GetImpl(*this).MoveFocusBackward();
}

void AccessibilityFocusManager::ClearFocus()
{
  GetImpl(*this).ClearFocus();
}

void AccessibilityFocusManager::Reset()
{
  GetImpl(*this).Reset();
}

void AccessibilityFocusManager::SetFocusGroup(Actor actor, bool isFocusGroup)
{
  GetImpl(*this).SetFocusGroup(actor, isFocusGroup);
}

bool AccessibilityFocusManager::IsFocusGroup(Actor actor) const
{
  return GetImpl(*this).IsFocusGroup(actor);
}

void AccessibilityFocusManager::SetGroupMode(bool enabled)
{
  GetImpl(*this).SetGroupMode(enabled);
}

bool AccessibilityFocusManager::GetGroupMode() const
{
  return GetImpl(*this).GetGroupMode();
}

void AccessibilityFocusManager::SetWrapMode(bool wrapped)
{
  GetImpl(*this).SetWrapMode(wrapped);
}

bool AccessibilityFocusManager::GetWrapMode() const
{
  return GetImpl(*this).GetWrapMode();
}

void AccessibilityFocusManager::SetFocusIndicatorActor(Actor indicator)
{
  GetImpl(*this).SetFocusIndicatorActor(indicator);
}

Actor AccessibilityFocusManager::GetFocusIndicatorActor()
{
  return GetImpl(*this).GetFocusIndicatorActor();
}

Actor AccessibilityFocusManager::GetFocusGroup(Actor actor)
{
  return GetImpl(*this).GetFocusGroup(actor);
}

AccessibilityFocusManager::FocusChangedSignalType& AccessibilityFocusManager::FocusChangedSignal()
{
  return GetImpl(*this).FocusChangedSignal();
}

AccessibilityFocusManager::FocusOvershotSignalType& AccessibilityFocusManager::FocusOvershotSignal()
{
  return GetImpl(*this).FocusOvershotSignal();
}

AccessibilityFocusManager::FocusedActorActivatedSignalType& AccessibilityFocusManager::FocusedActorActivatedSignal()
{
  return GetImpl(*this).FocusedActorActivatedSignal();
}

} // namespace Toolkit

} // namespace Dali
