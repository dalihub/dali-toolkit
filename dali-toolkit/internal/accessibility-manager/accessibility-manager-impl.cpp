/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <cstring> // for strcmp
#include <dali/public-api/actors/layer.h>
#include <dali/devel-api/adaptor-framework/sound-player.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/devel-api/events/hit-test-algorithm.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali/devel-api/adaptor-framework/accessibility.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

AccessibilityManager::AccessibilityManager()
{
}

AccessibilityManager::~AccessibilityManager()
{
}

void AccessibilityManager::Initialise()
{
}

void AccessibilityManager::SetAccessibilityAttribute(Actor actor, Toolkit::AccessibilityManager::AccessibilityAttribute type, const std::string& text)
{
}

std::string AccessibilityManager::GetAccessibilityAttribute(Actor actor, Toolkit::AccessibilityManager::AccessibilityAttribute type) const
{
  return "";
}

void AccessibilityManager::SetFocusOrder(Actor actor, const unsigned int order)
{
}

unsigned int AccessibilityManager::GetFocusOrder(Actor actor) const
{
  return 0;
}

unsigned int AccessibilityManager::GenerateNewFocusOrder() const
{
  return 0;
}

Actor AccessibilityManager::GetActorByFocusOrder(const unsigned int order)
{
  return {};
}

bool AccessibilityManager::SetCurrentFocusActor(Actor actor)
{
  return false;
}

Actor AccessibilityManager::GetCurrentFocusActor()
{
  return {};
}

Actor AccessibilityManager::GetCurrentFocusGroup()
{
  return {};
}

unsigned int AccessibilityManager::GetCurrentFocusOrder()
{
  return 0;
}

bool AccessibilityManager::MoveFocusForward()
{
  return false;
}

bool AccessibilityManager::MoveFocusBackward()
{
  return false;
}

void AccessibilityManager::ClearFocus()
{
}

void AccessibilityManager::Reset()
{
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
  return false;
}

void AccessibilityManager::SetFocusIndicatorActor(Actor indicator)
{
}

Actor AccessibilityManager::GetFocusIndicatorActor()
{
	 return {};
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
