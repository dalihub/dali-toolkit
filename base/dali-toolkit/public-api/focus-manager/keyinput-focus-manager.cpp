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
#include "keyinput-focus-manager.h"

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/singleton-service.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/focus-manager/keyinput-focus-manager-impl.h>

namespace Dali
{

namespace Toolkit
{

const char* const KeyInputFocusManager::SIGNAL_KEY_INPUT_FOCUS_CHANGED = "key-input-focus-changed";
const char* const KeyInputFocusManager::SIGNAL_UNHANDLED_KEY_EVENT = "unhandled-key-event";

KeyInputFocusManager::KeyInputFocusManager()
{
}

KeyInputFocusManager::~KeyInputFocusManager()
{
}

KeyInputFocusManager KeyInputFocusManager::Get()
{
  KeyInputFocusManager manager;

  // Check whether the focus manager is already created
  SingletonService singletonService( SingletonService::Get() );
  if ( singletonService )
  {
    Dali::BaseHandle handle = singletonService.GetSingleton(typeid(KeyInputFocusManager));
    if(handle)
    {
      // If so, downcast the handle of singleton to focus manager
      manager = KeyInputFocusManager(dynamic_cast<Internal::KeyInputFocusManager*>(handle.GetObjectPtr()));
    }

    if(!manager)
    {
      // If not, create the focus manager and register it as a singleton
      manager = KeyInputFocusManager(new Internal::KeyInputFocusManager());
      singletonService.Register(typeid(manager), manager);
    }
  }

  return manager;
}

KeyInputFocusManager::KeyInputFocusManager(Internal::KeyInputFocusManager *impl)
  : BaseHandle(impl)
{
}

void KeyInputFocusManager::SetFocus(Control control)
{
  GetImpl(*this).SetFocus(control);
}

Control KeyInputFocusManager::GetCurrentFocusControl() const
{
  return GetImpl(*this).GetCurrentFocusControl();
}

void KeyInputFocusManager::RemoveFocus(Control control)
{
  GetImpl(*this).RemoveFocus(control);
}

bool KeyInputFocusManager::IsKeyboardListener(Control control)
{
  return GetImpl(*this).IsKeyboardListener(control);
}

KeyInputFocusManager::KeyInputFocusChangedSignalV2& KeyInputFocusManager::KeyInputFocusChangedSignal()
{
  return GetImpl(*this).KeyInputFocusChangedSignal();
}

KeyInputFocusManager::UnhandledKeyEventSignalV2& KeyInputFocusManager::UnhandledKeyEventSignal()
{
  return GetImpl(*this).UnhandledKeyEventSignal();
}

} // namespace Toolkit

} // namespace Dali

