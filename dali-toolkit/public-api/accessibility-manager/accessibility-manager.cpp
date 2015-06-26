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
#include <dali-toolkit/public-api/accessibility-manager/accessibility-manager.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/singleton-service.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/accessibility-manager/accessibility-manager-impl.h>

namespace Dali
{

namespace Toolkit
{

AccessibilityManager::AccessibilityManager()
{
}

AccessibilityManager::~AccessibilityManager()
{
}

AccessibilityManager AccessibilityManager::Get()
{
  AccessibilityManager manager;

  // Check whether the accessibility manager is already created
  SingletonService singletonService( SingletonService::Get() );
  if ( singletonService )
  {
    Dali::BaseHandle handle = singletonService.GetSingleton(typeid(AccessibilityManager));
    if(handle)
    {
      // If so, downcast the handle of singleton to focus manager
      manager = AccessibilityManager(dynamic_cast<Internal::AccessibilityManager*>(handle.GetObjectPtr()));
    }

    if(!manager)
    {
      // If not, create the accessibility manager and register it as a singleton
      Internal::AccessibilityManager* internalManager = new Internal::AccessibilityManager();
      manager = AccessibilityManager( internalManager );
      internalManager->Initialise();
      singletonService.Register( typeid(manager), manager );
    }
  }

  return manager;
}

AccessibilityManager::AccessibilityManager(Internal::AccessibilityManager *impl)
  : BaseHandle(impl)
{
}

void AccessibilityManager::SetAccessibilityAttribute(Actor actor, AccessibilityAttribute type, const std::string& text)
{
  GetImpl(*this).SetAccessibilityAttribute(actor, type, text);
}

std::string AccessibilityManager::GetAccessibilityAttribute(Actor actor, AccessibilityAttribute type) const
{
  return GetImpl(*this).GetAccessibilityAttribute(actor, type);
}

void AccessibilityManager::SetFocusOrder(Actor actor, const unsigned int order)
{
  GetImpl(*this).SetFocusOrder(actor, order);
}

unsigned int AccessibilityManager::GetFocusOrder(Actor actor) const
{
  return GetImpl(*this).GetFocusOrder(actor);
}

unsigned int AccessibilityManager::GenerateNewFocusOrder() const
{
  return GetImpl(*this).GenerateNewFocusOrder();
}

Actor AccessibilityManager::GetActorByFocusOrder(const unsigned int order)
{
  return GetImpl(*this).GetActorByFocusOrder(order);
}

bool AccessibilityManager::SetCurrentFocusActor(Actor actor)
{
  return GetImpl(*this).SetCurrentFocusActor(actor);
}

Actor AccessibilityManager::GetCurrentFocusActor()
{
  return GetImpl(*this).GetCurrentFocusActor();
}

Actor AccessibilityManager::GetCurrentFocusGroup()
{
  return GetImpl(*this).GetCurrentFocusGroup();
}

unsigned int AccessibilityManager::GetCurrentFocusOrder()
{
  return GetImpl(*this).GetCurrentFocusOrder();
}

bool AccessibilityManager::MoveFocusForward()
{
  return GetImpl(*this).MoveFocusForward();
}

bool AccessibilityManager::MoveFocusBackward()
{
  return GetImpl(*this).MoveFocusBackward();
}

void AccessibilityManager::ClearFocus()
{
  GetImpl(*this).ClearFocus();
}

void AccessibilityManager::Reset()
{
  GetImpl(*this).Reset();
}

void AccessibilityManager::SetFocusGroup(Actor actor, bool isFocusGroup)
{
  GetImpl(*this).SetFocusGroup(actor, isFocusGroup);
}

bool AccessibilityManager::IsFocusGroup(Actor actor) const
{
  return GetImpl(*this).IsFocusGroup(actor);
}

void AccessibilityManager::SetGroupMode(bool enabled)
{
  GetImpl(*this).SetGroupMode(enabled);
}

bool AccessibilityManager::GetGroupMode() const
{
  return GetImpl(*this).GetGroupMode();
}

void AccessibilityManager::SetWrapMode(bool wrapped)
{
  GetImpl(*this).SetWrapMode(wrapped);
}

bool AccessibilityManager::GetWrapMode() const
{
  return GetImpl(*this).GetWrapMode();
}

void AccessibilityManager::SetFocusIndicatorActor(Actor indicator)
{
  GetImpl(*this).SetFocusIndicatorActor(indicator);
}

Actor AccessibilityManager::GetFocusIndicatorActor()
{
  return GetImpl(*this).GetFocusIndicatorActor();
}

Actor AccessibilityManager::GetFocusGroup(Actor actor)
{
  return GetImpl(*this).GetFocusGroup(actor);
}

AccessibilityManager::FocusChangedSignalType& AccessibilityManager::FocusChangedSignal()
{
  return GetImpl(*this).FocusChangedSignal();
}

AccessibilityManager::FocusOvershotSignalType& AccessibilityManager::FocusOvershotSignal()
{
  return GetImpl(*this).FocusOvershotSignal();
}

AccessibilityManager::FocusedActorActivatedSignalType& AccessibilityManager::FocusedActorActivatedSignal()
{
  return GetImpl(*this).FocusedActorActivatedSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::StatusChangedSignal()
{
  return GetImpl(*this).StatusChangedSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionNextSignal()
{
  return GetImpl(*this).ActionNextSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionPreviousSignal()
{
  return GetImpl(*this).ActionPreviousSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionActivateSignal()
{
  return GetImpl(*this).ActionActivateSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionOverSignal()
{
  return GetImpl(*this).ActionOverSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadSignal()
{
  return GetImpl(*this).ActionReadSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadNextSignal()
{
  return GetImpl(*this).ActionReadNextSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadPreviousSignal()
{
  return GetImpl(*this).ActionReadPreviousSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionUpSignal()
{
  return GetImpl(*this).ActionUpSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionDownSignal()
{
  return GetImpl(*this).ActionDownSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionClearFocusSignal()
{
  return GetImpl(*this).ActionClearFocusSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionBackSignal()
{
  return GetImpl(*this).ActionBackSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionScrollUpSignal()
{
  return GetImpl(*this).ActionScrollUpSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionScrollDownSignal()
{
  return GetImpl(*this).ActionScrollDownSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionPageLeftSignal()
{
  return GetImpl(*this).ActionPageLeftSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionPageRightSignal()
{
  return GetImpl(*this).ActionPageRightSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionPageUpSignal()
{
  return GetImpl(*this).ActionPageUpSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionPageDownSignal()
{
  return GetImpl(*this).ActionPageDownSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionMoveToFirstSignal()
{
  return GetImpl(*this).ActionMoveToFirstSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionMoveToLastSignal()
{
  return GetImpl(*this).ActionMoveToLastSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadFromTopSignal()
{
  return GetImpl(*this).ActionReadFromTopSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadFromNextSignal()
{
  return GetImpl(*this).ActionReadFromNextSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionZoomSignal()
{
  return GetImpl(*this).ActionZoomSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadIndicatorInformationSignal()
{
  return GetImpl(*this).ActionReadIndicatorInformationSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadPauseResumeSignal()
{
  return GetImpl(*this).ActionReadPauseResumeSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionStartStopSignal()
{
  return GetImpl(*this).ActionStartStopSignal();
}

AccessibilityManager::AccessibilityActionScrollSignalType& AccessibilityManager::ActionScrollSignal()
{
  return GetImpl(*this).ActionScrollSignal();
}

} // namespace Toolkit

} // namespace Dali
