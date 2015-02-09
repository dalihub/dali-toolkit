/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/focus-manager/focus-manager.h>

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/singleton-service.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/focus-manager/focus-manager-impl.h>

namespace Dali
{

namespace Toolkit
{

const char* const FocusManager::SIGNAL_FOCUS_CHANGED = "focus-changed";
const char* const FocusManager::SIGNAL_FOCUS_OVERSHOT = "focus-overshot";
const char* const FocusManager::SIGNAL_FOCUSED_ACTOR_ACTIVATED = "focused-actor-activated";

FocusManager::FocusManager()
{
}

FocusManager::~FocusManager()
{
}

FocusManager FocusManager::Get()
{
  FocusManager manager;

  // Check whether the focus manager is already created
  SingletonService singletonService( SingletonService::Get() );
  if ( singletonService )
  {
    Dali::BaseHandle handle = singletonService.GetSingleton(typeid(FocusManager));
    if(handle)
    {
      // If so, downcast the handle of singleton to focus manager
      manager = FocusManager(dynamic_cast<Internal::FocusManager*>(handle.GetObjectPtr()));
    }

    if(!manager)
    {
      // If not, create the focus manager and register it as a singleton
      manager = FocusManager(new Internal::FocusManager());
      singletonService.Register(typeid(manager), manager);
    }
  }

  return manager;
}

FocusManager::FocusManager(Internal::FocusManager *impl)
  : BaseHandle(impl)
{
}

void FocusManager::SetAccessibilityAttribute(Actor actor, AccessibilityAttribute type, const std::string& text)
{
  GetImpl(*this).SetAccessibilityAttribute(actor, type, text);
}

std::string FocusManager::GetAccessibilityAttribute(Actor actor, AccessibilityAttribute type) const
{
  return GetImpl(*this).GetAccessibilityAttribute(actor, type);
}

void FocusManager::SetFocusOrder(Actor actor, const unsigned int order)
{
  GetImpl(*this).SetFocusOrder(actor, order);
}

unsigned int FocusManager::GetFocusOrder(Actor actor) const
{
  return GetImpl(*this).GetFocusOrder(actor);
}

unsigned int FocusManager::GenerateNewFocusOrder() const
{
  return GetImpl(*this).GenerateNewFocusOrder();
}

Actor FocusManager::GetActorByFocusOrder(const unsigned int order)
{
  return GetImpl(*this).GetActorByFocusOrder(order);
}

bool FocusManager::SetCurrentFocusActor(Actor actor)
{
  return GetImpl(*this).SetCurrentFocusActor(actor);
}

Actor FocusManager::GetCurrentFocusActor()
{
  return GetImpl(*this).GetCurrentFocusActor();
}

Actor FocusManager::GetCurrentFocusGroup()
{
  return GetImpl(*this).GetCurrentFocusGroup();
}

unsigned int FocusManager::GetCurrentFocusOrder()
{
  return GetImpl(*this).GetCurrentFocusOrder();
}

bool FocusManager::MoveFocusForward()
{
  return GetImpl(*this).MoveFocusForward();
}

bool FocusManager::MoveFocusBackward()
{
  return GetImpl(*this).MoveFocusBackward();
}

void FocusManager::ClearFocus()
{
  GetImpl(*this).ClearFocus();
}

void FocusManager::Reset()
{
  GetImpl(*this).Reset();
}

void FocusManager::SetFocusGroup(Actor actor, bool isFocusGroup)
{
  GetImpl(*this).SetFocusGroup(actor, isFocusGroup);
}

bool FocusManager::IsFocusGroup(Actor actor) const
{
  return GetImpl(*this).IsFocusGroup(actor);
}

void FocusManager::SetGroupMode(bool enabled)
{
  GetImpl(*this).SetGroupMode(enabled);
}

bool FocusManager::GetGroupMode() const
{
  return GetImpl(*this).GetGroupMode();
}

void FocusManager::SetWrapMode(bool wrapped)
{
  GetImpl(*this).SetWrapMode(wrapped);
}

bool FocusManager::GetWrapMode() const
{
  return GetImpl(*this).GetWrapMode();
}

void FocusManager::SetFocusIndicatorActor(Actor indicator)
{
  GetImpl(*this).SetFocusIndicatorActor(indicator);
}

Actor FocusManager::GetFocusIndicatorActor()
{
  return GetImpl(*this).GetFocusIndicatorActor();
}

Actor FocusManager::GetFocusGroup(Actor actor)
{
  return GetImpl(*this).GetFocusGroup(actor);
}

FocusManager::FocusChangedSignalType& FocusManager::FocusChangedSignal()
{
  return GetImpl(*this).FocusChangedSignal();
}

FocusManager::FocusOvershotSignalType& FocusManager::FocusOvershotSignal()
{
  return GetImpl(*this).FocusOvershotSignal();
}

FocusManager::FocusedActorActivatedSignalType& FocusManager::FocusedActorActivatedSignal()
{
  return GetImpl(*this).FocusedActorActivatedSignal();
}

} // namespace Toolkit

} // namespace Dali
