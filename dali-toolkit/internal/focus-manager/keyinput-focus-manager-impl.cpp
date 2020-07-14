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
#include "keyinput-focus-manager-impl.h"

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <dali/public-api/actors/layer.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>

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

}

KeyInputFocusManager::KeyInputFocusManager()
: mSlotDelegate( this ),
  mCurrentFocusControl()
{
  // Retrieve all the existing widnows
  Dali::SceneHolderList sceneHolders = Adaptor::Get().GetSceneHolders();
  for( auto iter = sceneHolders.begin(); iter != sceneHolders.end(); ++iter )
  {
    ( *iter ).KeyEventGeneratedSignal().Connect( mSlotDelegate, &KeyInputFocusManager::OnKeyEvent );
  }

  // Get notified when any new scene holder is created afterwards
  Adaptor::Get().WindowCreatedSignal().Connect( mSlotDelegate, &KeyInputFocusManager::OnSceneHolderCreated );
}

KeyInputFocusManager::~KeyInputFocusManager()
{
}

void KeyInputFocusManager::OnSceneHolderCreated( Dali::Integration::SceneHolder& sceneHolder )
{
  sceneHolder.KeyEventGeneratedSignal().Connect( mSlotDelegate, &KeyInputFocusManager::OnKeyEvent );
}

void KeyInputFocusManager::SetFocus( Toolkit::Control control )
{
  if( !control )
  {
    // No-op
    return;
  }

  if( control == mCurrentFocusControl )
  {
    // Control already has focus
    return;
  }

  control.OffStageSignal().Connect( mSlotDelegate, &KeyInputFocusManager::OnFocusControlStageDisconnection );

  Dali::Toolkit::Control previousFocusControl = GetCurrentFocusControl();
  if( previousFocusControl )
  {
    // Notify the control that it has lost key input focus
    GetImplementation( previousFocusControl ).OnKeyInputFocusLost();
  }

  // Set control to currentFocusControl
  mCurrentFocusControl = control;

  // Tell the new actor that it has gained focus.
  GetImplementation( control ).OnKeyInputFocusGained();

  // Emit the signal to inform focus change to the application.
  if ( !mKeyInputFocusChangedSignal.Empty() )
  {
    mKeyInputFocusChangedSignal.Emit( control, previousFocusControl );
  }
}

void KeyInputFocusManager::RemoveFocus( Toolkit::Control control )
{
  if( control == mCurrentFocusControl )
  {
    control.OffStageSignal().Disconnect( mSlotDelegate, &KeyInputFocusManager::OnFocusControlStageDisconnection );

    // Notify the control that it has lost key input focus
    GetImplementation( control ).OnKeyInputFocusLost();

    mCurrentFocusControl.Reset();
  }
}

Toolkit::Control KeyInputFocusManager::GetCurrentFocusControl() const
{
  return mCurrentFocusControl;
}

Toolkit::KeyInputFocusManager::KeyInputFocusChangedSignalType& KeyInputFocusManager::KeyInputFocusChangedSignal()
{
  return mKeyInputFocusChangedSignal;
}

bool KeyInputFocusManager::OnKeyEvent( const KeyEvent& event )
{
  bool consumed = false;

  Toolkit::Control control = GetCurrentFocusControl();
  if( control )
  {
    // Notify the control about the key event
    consumed = EmitKeyEventSignal( control, event );
  }

  return consumed;
}

bool KeyInputFocusManager::EmitKeyEventSignal( Toolkit::Control control, const KeyEvent& event )
{
  bool consumed = false;

  if( control )
  {
    consumed = GetImplementation( control ).EmitKeyEventSignal( event );

    // if control doesn't consume KeyEvent, give KeyEvent to its parent.
    if( !consumed )
    {
      Toolkit::Control parent = Toolkit::Control::DownCast( control.GetParent() );

      if( parent )
      {
        consumed = EmitKeyEventSignal( parent, event );
      }
    }
  }

  return consumed;
}

void KeyInputFocusManager::OnFocusControlStageDisconnection( Dali::Actor actor )
{
  RemoveFocus( Dali::Toolkit::Control::DownCast( actor ) );
}


bool KeyInputFocusManager::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  KeyInputFocusManager* manager = dynamic_cast<KeyInputFocusManager*>( object );

  if( manager )
  {
    if( 0 == strcmp( signalName.c_str(), SIGNAL_KEY_INPUT_FOCUS_CHANGED ) )
    {
      manager->KeyInputFocusChangedSignal().Connect( tracker, functor );
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
