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
#include "keyinput-focus-manager-impl.h"

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

KeyInputFocusManager::KeyInputFocusManager()
: mSlotDelegate( this )
{
  Stage::GetCurrent().KeyEventSignal().Connect(mSlotDelegate, &KeyInputFocusManager::OnKeyEvent);
}

KeyInputFocusManager::~KeyInputFocusManager()
{
}

void KeyInputFocusManager::SetFocus(Toolkit::Control control)
{
  if(!control)
  {
   //No-op
    return;
  }

  unsigned int actorID = control.GetId();

  ActorQueueIterator pos = std::find( mFocusActorsQueue.begin(), mFocusActorsQueue.end(), actorID);

  if((!mFocusActorsQueue.empty()) && (pos == mFocusActorsQueue.begin()))
  {
    //Actor allready in front, so No-op
    return;
  }

  if(pos != mFocusActorsQueue.end())
  {
    //A previously focused actor wants to regain focus
    mFocusActorsQueue.erase(pos);
  }
  else
  {
    control.OffStageSignal().Connect( mSlotDelegate, &KeyInputFocusManager::OnFocusActorStageDisconnection );
  }

  Dali::Toolkit::Control previousFocusControl;
  if(!mFocusActorsQueue.empty())
  {
    previousFocusControl = Dali::Toolkit::Control::DownCast(Stage::GetCurrent().GetRootLayer().FindChildById(mFocusActorsQueue.front()));
    if(previousFocusControl)
    {
      // Notify the control that it has lost key input focus
      previousFocusControl.GetImplementation().OnKeyInputFocusLost();
    }
  }

  mFocusActorsQueue.push_front(actorID);

  // Tell the new actor that it has gained focus.
  control.GetImplementation().OnKeyInputFocusGained();

  // Emit the signal to inform focus change to the application.
  if ( !mKeyInputFocusChangedSignalV2.Empty() )
  {
    mKeyInputFocusChangedSignalV2.Emit( control, previousFocusControl );
  }
}

Toolkit::Control KeyInputFocusManager::GetCurrentFocusControl() const
{
  Toolkit::Control currentFocusControl;

  if(!mFocusActorsQueue.empty())
  {
    currentFocusControl = Dali::Toolkit::Control::DownCast(Stage::GetCurrent().GetRootLayer().FindChildById(mFocusActorsQueue.front()));
  }

  return currentFocusControl;
}

void KeyInputFocusManager::RemoveFocus(Toolkit::Control control)
{
  if(control)
  {
    unsigned int actorId = control.GetId();
    ActorQueueIterator pos = std::find( mFocusActorsQueue.begin(), mFocusActorsQueue.end(), actorId);

    if(pos != mFocusActorsQueue.end())
    {
      control.OffStageSignal().Disconnect( mSlotDelegate, &KeyInputFocusManager::OnFocusActorStageDisconnection );

      // Notify the control that it has lost key input focus
      control.GetImplementation().OnKeyInputFocusLost();

      if(pos == mFocusActorsQueue.begin())
      {
        Actor previousFocusActor;

        mFocusActorsQueue.erase(pos);
        if(!mFocusActorsQueue.empty())
        {
          previousFocusActor = Stage::GetCurrent().GetRootLayer().FindChildById(mFocusActorsQueue.front());
        }

        Dali::Toolkit::Control previouscontrol = Dali::Toolkit::Control::DownCast(previousFocusActor);
        if(previouscontrol)
        {
          // Tell the new actor that it has gained focus.
          previouscontrol.GetImplementation().OnKeyInputFocusGained();
        }
      }
      else
      {
        //If the removed actor is not currently focused, then no need to emit signal.
        mFocusActorsQueue.erase(pos);
      }

    }
  }
}

bool KeyInputFocusManager::IsKeyboardListener(Toolkit::Control control) const
{
  bool result = false;

  if(!mFocusActorsQueue.empty())
  {
    unsigned int actorId = control.GetId();
    ActorQueueConstIterator pos = std::find(mFocusActorsQueue.begin(), mFocusActorsQueue.end(), actorId);

    if(pos != mFocusActorsQueue.end())
    {
      result = true;
    }
  }

  return result;
}

Toolkit::KeyInputFocusManager::KeyInputFocusChangedSignalV2& KeyInputFocusManager::KeyInputFocusChangedSignal()
{
  return mKeyInputFocusChangedSignalV2;
}

Toolkit::KeyInputFocusManager::UnhandledKeyEventSignalV2& KeyInputFocusManager::UnhandledKeyEventSignal()
{
  return mUnhandledKeyEventSignalV2;
}

void KeyInputFocusManager::OnKeyEvent(const KeyEvent& event)
{
  bool consumed = false;

  ActorQueueIterator iter = mFocusActorsQueue.begin();

  Layer rootLayer = Stage::GetCurrent().GetRootLayer();

  while(!mFocusActorsQueue.empty() && !consumed && (iter != mFocusActorsQueue.end()))
  {
    Actor actor = rootLayer.FindChildById(*iter);
    Dali::Toolkit::Control control = Dali::Toolkit::Control::DownCast(actor);
    if(control)
    {
      // Notify the control about the key event
      consumed = control.GetImplementation().EmitKeyEventSignal(event);
    }
    iter++;
  }

  if(!consumed)
  {
    // Emit signal to inform that a key event is not consumed.
    if( !mUnhandledKeyEventSignalV2.Empty() )
    {
      mUnhandledKeyEventSignalV2.Emit(event);
    }
  }
}

void KeyInputFocusManager::OnFocusActorStageDisconnection( Dali::Actor actor )
{
  RemoveFocus(Dali::Toolkit::Control::DownCast(actor));
}

bool KeyInputFocusManager::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  KeyInputFocusManager* manager = dynamic_cast<KeyInputFocusManager*>(object);

  if( Dali::Toolkit::KeyInputFocusManager::SIGNAL_KEY_INPUT_FOCUS_CHANGED == signalName )
  {
    manager->KeyInputFocusChangedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
