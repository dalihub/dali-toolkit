/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

%define KEYBOARD_FOCUS_MANAGER_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;
%}

%enddef


%define KEYBOARD_FOCUS_MANAGER_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/**
  * @brief Event arguments that passed via FocusChanged signal
  *
  */
public class FocusChangedEventArgs : EventArgs
{
   private Actor _actorCurrent;
   private Actor _actorNext;

   /**
     * @brief Actor - is the original focused Actor
     *
     */
   public Actor ActorCurrent
   {
      get
      {
         return _actorCurrent;
      }
      set
      {
         _actorCurrent = value;
      }
   }

   /**
     * @brief Actor - is the current focused Actor
     *
     */
   public Actor ActorNext
   {
      get
      {
         return _actorNext;
      }
      set
      {
         _actorNext = value;
      }
   }
}

/**
  * @brief Event arguments that passed via FocusGroupChanged signal
  *
  */
public class FocusGroupChangedEventArgs : EventArgs
{
   private Actor _currentFocusedActor;
   private bool _forwardDirection;

   /**
     * @brief Actor - is the current focused Actor
     *
     */
   public Actor CurrentFocusedActor
   {
      get
      {
         return _currentFocusedActor;
      }
      set
      {
         _currentFocusedActor = value;
      }
   }

   /**
     * @brief ForwardDirection - is the direction (forward or backward) in which to move the focus next
     *
     */
   public bool ForwardDirection
   {
      get
      {
         return _forwardDirection;
      }
      set
      {
         _forwardDirection = value;
      }
   }
}

/**
  * @brief Event arguments that passed via FocusedActorEnterKey signal
  *
  */
public class FocusedActorEnterKeyEventArgs : EventArgs
{
   private Actor _actor;

   /**
     * @brief Actor - is the current focused Actor which has the enter key pressed on it.
     *
     */
   public Actor Actor
   {
      get
      {
         return _actor;
      }
      set
      {
         _actor = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void FocusChangedEventHandler(object source, FocusChangedEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void FocusGroupChangedEventHandler(object source, FocusGroupChangedEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void FocusedActorEnterKeyEventHandler(object source, FocusedActorEnterKeyEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void FocusChangedEventCallbackDelegate(IntPtr actorCurrent, IntPtr actorNext);
  private FocusChangedEventHandler _keyboardFocusManagerFocusChangedEventHandler;
  private FocusChangedEventCallbackDelegate _keyboardFocusManagerFocusChangedEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void FocusGroupChangedEventCallbackDelegate(IntPtr currentFocusedActor, bool forwardDirection);
  private FocusGroupChangedEventHandler _keyboardFocusManagerFocusGroupChangedEventHandler;
  private FocusGroupChangedEventCallbackDelegate _keyboardFocusManagerFocusGroupChangedEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void FocusedActorEnterKeyEventCallbackDelegate(IntPtr actor);
  private FocusedActorEnterKeyEventHandler _keyboardFocusManagerFocusedActorEnterKeyEventHandler;
  private FocusedActorEnterKeyEventCallbackDelegate _keyboardFocusManagerFocusedActorEnterKeyEventCallbackDelegate;

  /**
    * @brief Event for FocusChanged signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of FocusChangedEventHandler) provided by the user.
    * FocusChanged signal is emitted after the current focused actor has been changed.
    */
  public event FocusChangedEventHandler FocusChanged
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_keyboardFocusManagerFocusChangedEventHandler == null)
           {
              _keyboardFocusManagerFocusChangedEventHandler += value;

              _keyboardFocusManagerFocusChangedEventCallbackDelegate = new FocusChangedEventCallbackDelegate(OnFocusChanged);
              this.FocusChangedSignal().Connect(_keyboardFocusManagerFocusChangedEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_keyboardFocusManagerFocusChangedEventHandler != null)
           {
              this.FocusChangedSignal().Disconnect(_keyboardFocusManagerFocusChangedEventCallbackDelegate);
           }

           _keyboardFocusManagerFocusChangedEventHandler -= value;
        }
     }
  }

  // Callback for KeyboardFocusManager FocusChangedSignal
  private void OnFocusChanged(IntPtr actorCurrent, IntPtr actorNext)
  {
     FocusChangedEventArgs e = new FocusChangedEventArgs();

     // Populate all members of "e" (FocusChangedEventArgs) with real data
     e.ActorCurrent = Actor.GetActorFromPtr(actorCurrent);
     e.ActorNext = Actor.GetActorFromPtr(actorNext);

     if (_keyboardFocusManagerFocusChangedEventHandler != null)
     {
        //here we send all data to user event handlers
        _keyboardFocusManagerFocusChangedEventHandler(this, e);
     }
  }

  /**
    * @brief Event for FocusGroupChanged signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of FocusGroupChangedEventHandler) provided by the user.
    * FocusGroupChanged signal is emitted when the focus group has been changed.
    */
  public event FocusGroupChangedEventHandler FocusGroupChanged
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_keyboardFocusManagerFocusGroupChangedEventHandler == null)
           {
              _keyboardFocusManagerFocusGroupChangedEventHandler += value;

              _keyboardFocusManagerFocusGroupChangedEventCallbackDelegate = new FocusGroupChangedEventCallbackDelegate(OnFocusGroupChanged);
              this.FocusGroupChangedSignal().Connect(_keyboardFocusManagerFocusGroupChangedEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_keyboardFocusManagerFocusGroupChangedEventHandler != null)
           {
              this.FocusGroupChangedSignal().Disconnect(_keyboardFocusManagerFocusGroupChangedEventCallbackDelegate);
           }

           _keyboardFocusManagerFocusGroupChangedEventHandler -= value;
        }
     }
  }

  // Callback for KeyboardFocusManager FocusGroupChangedSignal
  private void OnFocusGroupChanged(IntPtr currentFocusedActor, bool forwardDirection)
  {
     FocusGroupChangedEventArgs e = new FocusGroupChangedEventArgs();

     // Populate all members of "e" (FocusGroupChangedEventArgs) with real data
     e.CurrentFocusedActor = Actor.GetActorFromPtr(currentFocusedActor);
     e.ForwardDirection = forwardDirection;

     if (_keyboardFocusManagerFocusGroupChangedEventHandler != null)
     {
        //here we send all data to user event handlers
        _keyboardFocusManagerFocusGroupChangedEventHandler(this, e);
     }
  }

  /**
    * @brief Event for FocusedActorEnterKeyPressed signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of FocusedActorEnterKeyEventHandler) provided by the user.
    * FocusedActorEnterKeyPressed signal is emitted when the current focused actor has the enter key pressed on it.
    */
  public event FocusedActorEnterKeyEventHandler FocusedActorEnterKeyPressed
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_keyboardFocusManagerFocusedActorEnterKeyEventHandler == null)
           {
              _keyboardFocusManagerFocusedActorEnterKeyEventHandler += value;

              _keyboardFocusManagerFocusedActorEnterKeyEventCallbackDelegate = new FocusedActorEnterKeyEventCallbackDelegate(OnFocusedActorEnterKey);
              this.FocusedActorEnterKeySignal().Connect(_keyboardFocusManagerFocusedActorEnterKeyEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_keyboardFocusManagerFocusedActorEnterKeyEventHandler != null)
           {
              this.FocusedActorEnterKeySignal().Disconnect(_keyboardFocusManagerFocusedActorEnterKeyEventCallbackDelegate);
           }

           _keyboardFocusManagerFocusedActorEnterKeyEventHandler -= value;
        }
     }
  }

  // Callback for KeyboardFocusManager FocusedActorEnterKeySignal
  private void OnFocusedActorEnterKey(IntPtr actor)
  {
     FocusedActorEnterKeyEventArgs e = new FocusedActorEnterKeyEventArgs();

     // Populate all members of "e" (FocusedActorEnterKeyEventArgs) with real data
     e.Actor = Actor.GetActorFromPtr(actor);

     if (_keyboardFocusManagerFocusedActorEnterKeyEventHandler != null)
     {
        //here we send all data to user event handlers
        _keyboardFocusManagerFocusedActorEnterKeyEventHandler(this, e);
     }
  }

%}
%enddef

%define DALI_KEYBOARD_FOCUS_MANAGER_EVENTHANDLER_PARAM( NameSpace, ClassName)
  KEYBOARD_FOCUS_MANAGER_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  KEYBOARD_FOCUS_MANAGER_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);
%enddef

namespace Dali
{
  DALI_KEYBOARD_FOCUS_MANAGER_EVENTHANDLER_PARAM( Dali::Toolkit, KeyboardFocusManager);
}

