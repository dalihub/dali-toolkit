//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER

#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

#include <dali-toolkit/internal/focus-manager/keyboard-focus-manager-impl.h>

namespace Dali
{

namespace Toolkit
{

const char* const KeyboardFocusManager::SIGNAL_PRE_FOCUS_CHANGE = "keyboard-pre-focus-change";
const char* const KeyboardFocusManager::SIGNAL_FOCUS_CHANGED = "keyboard-focus-changed";
const char* const KeyboardFocusManager::SIGNAL_FOCUS_GROUP_CHANGED = "keyboard-focus-group-changed";
const char* const KeyboardFocusManager::SIGNAL_FOCUSED_ACTOR_ACTIVATED = "keyboard-focused-actor-activated";

KeyboardFocusManager::KeyboardFocusManager()
{
}

KeyboardFocusManager::~KeyboardFocusManager()
{
}

KeyboardFocusManager KeyboardFocusManager::Get()
{
  return Internal::KeyboardFocusManager::Get();
}

KeyboardFocusManager::KeyboardFocusManager(Internal::KeyboardFocusManager *impl)
  : BaseHandle(impl)
{
}

bool KeyboardFocusManager::SetCurrentFocusActor(Actor actor)
{
  return GetImpl(*this).SetCurrentFocusActor(actor);
}

Actor KeyboardFocusManager::GetCurrentFocusActor()
{
  return GetImpl(*this).GetCurrentFocusActor();
}

bool KeyboardFocusManager::MoveFocus(Control::KeyboardFocusNavigationDirection direction)
{
  return GetImpl(*this).MoveFocus(direction);
}

void KeyboardFocusManager::ClearFocus()
{
  GetImpl(*this).ClearFocus();
}

void KeyboardFocusManager::SetAsFocusGroup(Actor actor, bool isFocusGroup)
{
  // deprecated method.
  GetImpl(*this).SetAsFocusGroup(actor, isFocusGroup);
}

bool KeyboardFocusManager::IsFocusGroup(Actor actor) const
{
  // deprecated method.
  return GetImpl(*this).IsFocusGroup(actor);
}

Actor KeyboardFocusManager::GetFocusGroup(Actor actor)
{
  return GetImpl(*this).GetFocusGroup(actor);
}

void KeyboardFocusManager::SetFocusGroupLoop(bool enabled)
{
  GetImpl(*this).SetFocusGroupLoop(enabled);
}

bool KeyboardFocusManager::GetFocusGroupLoop() const
{
  return GetImpl(*this).GetFocusGroupLoop();
}

void KeyboardFocusManager::SetFocusIndicatorActor(Actor indicator)
{
  GetImpl(*this).SetFocusIndicatorActor(indicator);
}

Actor KeyboardFocusManager::GetFocusIndicatorActor()
{
  return GetImpl(*this).GetFocusIndicatorActor();
}

KeyboardFocusManager::PreFocusChangeSignalV2& KeyboardFocusManager::PreFocusChangeSignal()
{
  return GetImpl(*this).PreFocusChangeSignal();
}

KeyboardFocusManager::FocusChangedSignalV2& KeyboardFocusManager::FocusChangedSignal()
{
  return GetImpl(*this).FocusChangedSignal();
}

KeyboardFocusManager::FocusGroupChangedSignalV2& KeyboardFocusManager::FocusGroupChangedSignal()
{
  return GetImpl(*this).FocusGroupChangedSignal();
}

KeyboardFocusManager::FocusedActorActivatedSignalV2& KeyboardFocusManager::FocusedActorActivatedSignal()
{
  return GetImpl(*this).FocusedActorActivatedSignal();
}

} // namespace Toolkit

} // namespace Dali
