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
#include "keyinput-focus-manager-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/common/stage.h>

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
  mObjectRegistry = Dali::Stage::GetCurrent().GetObjectRegistry();
  mObjectRegistry.ObjectDestroyedSignal().Connect( this, &KeyInputFocusManager::OnObjectDestroyed );
}

KeyInputFocusManager::~KeyInputFocusManager()
{
}

void KeyInputFocusManager::SetFocus( Toolkit::Control control )
{
  if( !control )
  {
    // No-op
    return;
  }

  FocusStackIterator pos = FindFocusControlInStack( control );

  if( ( mFocusStack.Count() != 0 ) && ( pos == mFocusStack.End()-1 ) )
  {
    // Control already in front, so No-op
    return;
  }

  if( pos != mFocusStack.End() )
  {
    // A previously focused control wants to regain focus
    mFocusStack.Erase( pos );
  }
  else
  {
    control.OffStageSignal().Connect( mSlotDelegate, &KeyInputFocusManager::OnFocusControlStageDisconnection );
  }

  Dali::Toolkit::Control previousFocusControl = GetCurrentFocusControl();
  if( previousFocusControl )
  {
    // Notify the control that it has lost key input focus
    previousFocusControl.GetImplementation().OnKeyInputFocusLost();
  }

  mFocusStack.PushBack( &control.GetBaseObject() );

  // Tell the new actor that it has gained focus.
  control.GetImplementation().OnKeyInputFocusGained();

  // Emit the signal to inform focus change to the application.
  if ( !mKeyInputFocusChangedSignal.Empty() )
  {
    mKeyInputFocusChangedSignal.Emit( control, previousFocusControl );
  }
}

void KeyInputFocusManager::RemoveFocus( Toolkit::Control control )
{
  if( control )
  {
    FocusStackIterator pos = FindFocusControlInStack( control );
    if( pos != mFocusStack.End() )
    {
      control.OffStageSignal().Disconnect( mSlotDelegate, &KeyInputFocusManager::OnFocusControlStageDisconnection );

      // Notify the control that it has lost key input focus
      control.GetImplementation().OnKeyInputFocusLost();

      // If this is the top-most actor, pop it and change focus to the previous control
      if( pos == mFocusStack.End() - 1 )
      {
        mFocusStack.Erase( pos );

        Toolkit::Control previouslyFocusedControl = GetCurrentFocusControl();
        if( previouslyFocusedControl )
        {
          // Tell the control that it has gained focus.
          previouslyFocusedControl.GetImplementation().OnKeyInputFocusGained();
        }
      }
      else
      {
        // If the removed control is not currently focused, then no need to emit signal.
        mFocusStack.Erase( pos );
      }
    }
  }
}

Toolkit::Control KeyInputFocusManager::GetCurrentFocusControl() const
{
  Toolkit::Control currentControl;

  FocusStack::SizeType count = mFocusStack.Count();
  if( count != 0 )
  {
    BaseObject* object = mFocusStack[ count - 1 ];
    BaseHandle handle( object );
    currentControl = Dali::Toolkit::Control::DownCast( handle );
  }
  return currentControl;
}

bool KeyInputFocusManager::IsKeyboardListener( Toolkit::Control control ) const
{
  bool result = false;

  if( FindFocusControlInStack( control ) != mFocusStack.End() )
  {
    result = true;
  }

  return result;
}

Toolkit::KeyInputFocusManager::KeyInputFocusChangedSignalType& KeyInputFocusManager::KeyInputFocusChangedSignal()
{
  return mKeyInputFocusChangedSignal;
}

Toolkit::KeyInputFocusManager::UnhandledKeyEventSignalType& KeyInputFocusManager::UnhandledKeyEventSignal()
{
  return mUnhandledKeyEventSignal;
}

KeyInputFocusManager::FocusStackIterator KeyInputFocusManager::FindFocusControlInStack( Toolkit::Control control ) const
{
  BaseObject* controlObject = &control.GetBaseObject();
  return std::find( mFocusStack.Begin(), mFocusStack.End(), controlObject );
}

void KeyInputFocusManager::OnKeyEvent( const KeyEvent& event )
{
  bool consumed = false;

  if( mFocusStack.Count() > 0 )
  {
    FocusStack::SizeType index = mFocusStack.Count();
    while( mFocusStack.Count() != 0 && !consumed && index > 0 )
    {
      --index;
      BaseObject* object = mFocusStack[ index ];
      BaseHandle handle( object );
      Toolkit::Control control = Toolkit::Control::DownCast( object );
      if( control )
      {
        // Notify the control about the key event
        consumed = control.GetImplementation().EmitKeyEventSignal( event );
      }
    }
  }

  if( !consumed )
  {
    // Emit signal to inform that a key event is not consumed.
    if( !mUnhandledKeyEventSignal.Empty() )
    {
      mUnhandledKeyEventSignal.Emit(event);
    }
  }
}

void KeyInputFocusManager::OnFocusControlStageDisconnection( Dali::Actor actor )
{
  RemoveFocus( Dali::Toolkit::Control::DownCast( actor ) );
}

void KeyInputFocusManager::OnObjectDestroyed( const Dali::RefObject* object )
{
  // The object is already destroyed. Don't create handles to it, or try sending
  // signals to it. Remove it's pointer from the stack.
  const BaseObject* baseObject = static_cast<const BaseObject*>( object );
  FocusStackIterator pos = std::find( mFocusStack.Begin(), mFocusStack.End(), baseObject );
  if( pos != mFocusStack.End() )
  {
    mFocusStack.Erase( pos );
  }
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
