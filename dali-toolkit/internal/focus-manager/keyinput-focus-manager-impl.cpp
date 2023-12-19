/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include "keyinput-focus-manager-impl.h"

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/debug.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/public-api/actors/layer.h>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
// Signals

const char* const SIGNAL_KEY_INPUT_FOCUS_CHANGED = "keyInputFocusChanged";

} // namespace

KeyInputFocusManager::KeyInputFocusManager()
: mSlotDelegate(this),
  mCurrentFocusControl(),
  mCurrentWindowId(0)
{
  // Retrieve all the existing widnows
  Dali::SceneHolderList sceneHolders = Adaptor::Get().GetSceneHolders();
  for(auto iter = sceneHolders.begin(); iter != sceneHolders.end(); ++iter)
  {
    (*iter).KeyEventGeneratedSignal().Connect(mSlotDelegate, &KeyInputFocusManager::OnKeyEvent);
  }

  // Get notified when any new scene holder is created afterwards
  Adaptor::Get().WindowCreatedSignal().Connect(mSlotDelegate, &KeyInputFocusManager::OnSceneHolderCreated);
}

KeyInputFocusManager::~KeyInputFocusManager()
{
}

void KeyInputFocusManager::OnSceneHolderCreated(Dali::Integration::SceneHolder& sceneHolder)
{
  sceneHolder.KeyEventGeneratedSignal().Connect(mSlotDelegate, &KeyInputFocusManager::OnKeyEvent);
}

void KeyInputFocusManager::SetFocus(Toolkit::Control control)
{
  if(!control)
  {
    // No-op
    return;
  }

  if(control == mCurrentFocusControl)
  {
    // Control already has focus
    return;
  }

  control.OffSceneSignal().Connect(mSlotDelegate, &KeyInputFocusManager::OnFocusControlSceneDisconnection);

  Dali::Toolkit::Control previousFocusControl = GetCurrentFocusControl();

  // Set control to currentFocusControl
  mCurrentFocusControl = control;
  mCurrentWindowId = static_cast<uint32_t>(Integration::SceneHolder::Get(control).GetNativeId());

  if(previousFocusControl)
  {
    // Notify the control that it has lost key input focus
    GetImplementation(previousFocusControl).OnKeyInputFocusLost();
  }

  // Tell the new actor that it has gained focus.
  GetImplementation(control).OnKeyInputFocusGained();

  // Emit the signal to inform focus change to the application.
  if(!mKeyInputFocusChangedSignal.Empty())
  {
    mKeyInputFocusChangedSignal.Emit(control, previousFocusControl);
  }
}

void KeyInputFocusManager::RemoveFocus(Toolkit::Control control)
{
  if(control && control == mCurrentFocusControl)
  {
    DALI_LOG_RELEASE_INFO("RemoveFocus id:(%d)\n", control.GetProperty<int32_t>(Dali::Actor::Property::ID));
    control.OffSceneSignal().Disconnect(mSlotDelegate, &KeyInputFocusManager::OnFocusControlSceneDisconnection);

    mCurrentFocusControl.Reset();
    mCurrentWindowId = 0;

    // Notify the control that it has lost key input focus
    GetImplementation(control).OnKeyInputFocusLost();
  }
}

Toolkit::Control KeyInputFocusManager::GetCurrentFocusControl() const
{
  return mCurrentFocusControl;
}

uint32_t KeyInputFocusManager::GetCurrentWindowId() const
{
  return mCurrentWindowId;
}

Toolkit::KeyInputFocusManager::KeyInputFocusChangedSignalType& KeyInputFocusManager::KeyInputFocusChangedSignal()
{
  return mKeyInputFocusChangedSignal;
}

bool KeyInputFocusManager::OnKeyEvent(const KeyEvent& event)
{
  bool consumed = false;

  Toolkit::Control control = GetCurrentFocusControl();
  if(control)
  {
    // Key events that occur in windows other than the currently focused control are skipped.
    uint32_t eventWindowId = event.GetWindowId();
    if(eventWindowId > 0 && GetCurrentWindowId() != eventWindowId)
    {
      DALI_LOG_RELEASE_INFO("Current control window id %d, window ID where key event occurred %d : key event skip\n", GetCurrentWindowId(), eventWindowId);
      return consumed;
    }

    Dali::Actor dispatch = control;
    while(dispatch)
    {
      // If the DISPATCH_KEY_EVENTS is false, it cannot emit key event.
      Toolkit::Control dispatchControl = Toolkit::Control::DownCast(dispatch);
      if(dispatchControl && !dispatchControl.GetProperty<bool>(Toolkit::DevelControl::Property::DISPATCH_KEY_EVENTS))
      {
        return true;
      }
      dispatch = dispatch.GetParent();
    }

    // Notify the control about the key event
    consumed = EmitKeyEventSignal(control, event);
  }

  return consumed;
}

bool KeyInputFocusManager::EmitKeyEventSignal(Toolkit::Control control, const KeyEvent& event)
{
  bool consumed = false;

  if(control)
  {
    consumed = GetImplementation(control).EmitKeyEventSignal(event);

    // if control doesn't consume KeyEvent, give KeyEvent to its parent.
    if(!consumed)
    {
      Toolkit::Control parent = Toolkit::Control::DownCast(control.GetParent());

      if(parent)
      {
        consumed = EmitKeyEventSignal(parent, event);
      }
    }
  }

  return consumed;
}

void KeyInputFocusManager::OnFocusControlSceneDisconnection(Dali::Actor actor)
{
  RemoveFocus(Dali::Toolkit::Control::DownCast(actor));
}

bool KeyInputFocusManager::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  bool                  connected(true);
  KeyInputFocusManager* manager = dynamic_cast<KeyInputFocusManager*>(object);

  if(manager)
  {
    if(0 == strcmp(signalName.c_str(), SIGNAL_KEY_INPUT_FOCUS_CHANGED))
    {
      manager->KeyInputFocusChangedSignal().Connect(tracker, functor);
    }
    else
    {
      // signalName does not match any signal
      connected = false;
    }
  }

  return connected;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
