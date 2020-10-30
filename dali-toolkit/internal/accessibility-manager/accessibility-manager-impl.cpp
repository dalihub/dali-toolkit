/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include "accessibility-manager-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/devel-api/adaptor-framework/accessibility-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

AccessibilityManager::AccessibilityManager()
{
  mFocusOrder.push_back( {} ); // zero has a special meaning
}

AccessibilityManager::~AccessibilityManager()
{
}

void AccessibilityManager::SetAccessibilityAttribute(Actor actor, Toolkit::AccessibilityManager::AccessibilityAttribute type, const std::string& text)
{
  switch ( type )
  {
  case Toolkit::AccessibilityManager::ACCESSIBILITY_LABEL:
    actor.SetProperty( Toolkit::DevelControl::Property::ACCESSIBILITY_NAME, text );
    break;

  case Toolkit::AccessibilityManager::ACCESSIBILITY_HINT:
    actor.SetProperty( Toolkit::DevelControl::Property::ACCESSIBILITY_DESCRIPTION, text );
    break;

  case Toolkit::AccessibilityManager::ACCESSIBILITY_TRAIT:
  case Toolkit::AccessibilityManager::ACCESSIBILITY_VALUE:
  default:
    break;
  }
}

std::string AccessibilityManager::GetAccessibilityAttribute(Actor actor, Toolkit::AccessibilityManager::AccessibilityAttribute type) const
{
  switch ( type )
  {
  case Toolkit::AccessibilityManager::ACCESSIBILITY_LABEL:
    return actor.GetProperty< std::string >( Toolkit::DevelControl::Property::ACCESSIBILITY_NAME );

  case Toolkit::AccessibilityManager::ACCESSIBILITY_HINT:
    return actor.GetProperty< std::string >( Toolkit::DevelControl::Property::ACCESSIBILITY_DESCRIPTION );

  case Toolkit::AccessibilityManager::ACCESSIBILITY_TRAIT:
  case Toolkit::AccessibilityManager::ACCESSIBILITY_VALUE:
  default:
    return "";
  }
}

void AccessibilityManager::SetFocusOrder(Actor actor, const unsigned int order)
{
  if (order == 0)
    return;

  if (order >= mFocusOrder.size())
    mFocusOrder.resize(order + 1);

  auto it = mFocusOrder.begin() + order;
  mFocusOrder.insert(it, actor);

  if (order > 0)
  {
    Actor prev = mFocusOrder[order - 1];
    DevelControl::AppendAccessibilityRelation( prev, actor, Accessibility::RelationType::FLOWS_TO );
    DevelControl::AppendAccessibilityRelation( actor, prev, Accessibility::RelationType::FLOWS_FROM );
  }

  if (order + 1 < mFocusOrder.size())
  {
    Actor next = mFocusOrder[order + 1];
    DevelControl::AppendAccessibilityRelation( actor, next, Accessibility::RelationType::FLOWS_TO );
    DevelControl::AppendAccessibilityRelation( next, actor, Accessibility::RelationType::FLOWS_FROM );
  }
}

unsigned int AccessibilityManager::GetFocusOrder(Actor actor) const
{
  for (auto it = mFocusOrder.begin(); it != mFocusOrder.end(); ++it)
  {
    if (actor == *it)
      return it - mFocusOrder.begin();
  }

  return 0;
}

unsigned int AccessibilityManager::GenerateNewFocusOrder() const
{
  return static_cast<unsigned>(mFocusOrder.size());
}

Actor AccessibilityManager::GetActorByFocusOrder(const unsigned int order)
{
  Actor actor;

  if (order > 0 && order < mFocusOrder.size())
    actor = mFocusOrder[order];

  return actor;
}

bool AccessibilityManager::SetCurrentFocusActor(Actor actor)
{
  Dali::Accessibility::Accessible::SetCurrentlyHighlightedActor(actor);

  return true;
}

Actor AccessibilityManager::GetCurrentFocusActor()
{
  return Dali::Accessibility::Accessible::GetCurrentlyHighlightedActor();
}

Actor AccessibilityManager::GetCurrentFocusGroup()
{
  return GetFocusGroup(GetCurrentFocusActor());
}

unsigned int AccessibilityManager::GetCurrentFocusOrder()
{
  auto actor = GetCurrentFocusActor();
  unsigned order = 0;

  if (actor)
    order = GetFocusOrder( actor );

  return order;
}

bool AccessibilityManager::MoveFocusForward()
{
  unsigned current = GetCurrentFocusOrder();

  if (current + 1 < mFocusOrder.size())
    return SetCurrentFocusActor(mFocusOrder[current + 1]);

  return false;
}

bool AccessibilityManager::MoveFocusBackward()
{
  unsigned current = GetCurrentFocusOrder();

  if (current > 1) // zero has a special meaning
    return SetCurrentFocusActor(mFocusOrder[current - 1]);

  return false;
}

void AccessibilityManager::ClearFocus()
{
  auto actor = GetCurrentFocusActor();
  Toolkit::DevelControl::ClearAccessibilityHighlight( actor );
}

void AccessibilityManager::Reset()
{
  ClearFocus();

  for (std::size_t i = 2; i < mFocusOrder.size(); ++i)
  {
    Actor prev = mFocusOrder[i - 1];
    Actor next = mFocusOrder[i];

    DevelControl::RemoveAccessibilityRelation( prev, next, Accessibility::RelationType::FLOWS_TO );
    DevelControl::RemoveAccessibilityRelation( next, prev, Accessibility::RelationType::FLOWS_FROM );
  }

  mFocusOrder.clear();
  mFocusOrder.push_back( {} );
}

void AccessibilityManager::SetFocusGroup(Actor actor, bool isFocusGroup)
{
}

bool AccessibilityManager::IsFocusGroup(Actor actor) const
{
  return false;
}

Actor AccessibilityManager::GetFocusGroup(Actor actor)
{
  return {};
}

Vector2 AccessibilityManager::GetReadPosition() const
{
  return {};
}

void AccessibilityManager::SetGroupMode(bool enabled)
{
}

bool AccessibilityManager::GetGroupMode() const
{
  return false;
}

void AccessibilityManager::SetWrapMode(bool wrapped)
{
}

bool AccessibilityManager::GetWrapMode() const
{
  return true;
}

void AccessibilityManager::SetFocusIndicatorActor(Actor indicator)
{
  Dali::Accessibility::Accessible::SetHighlightActor( indicator );
}

Actor AccessibilityManager::GetFocusIndicatorActor()
{
  return Dali::Accessibility::Accessible::GetHighlightActor();
}

Toolkit::AccessibilityManager::FocusChangedSignalType& AccessibilityManager::FocusChangedSignal()
{
  return mFocusChangedSignal;
}

Toolkit::AccessibilityManager::FocusOvershotSignalType& AccessibilityManager::FocusOvershotSignal()
{
  return mFocusOvershotSignal;
}

Toolkit::AccessibilityManager::FocusedActorActivatedSignalType& AccessibilityManager::FocusedActorActivatedSignal()
{
  return mFocusedActorActivatedSignal;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
