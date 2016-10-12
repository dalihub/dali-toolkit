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

%define CONTROL_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}

%enddef

%define CONTROL_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{


/**
  * @brief Event arguments that passed via KeyInputFocusGained signal
  *
  */
public class KeyInputFocusGainedEventArgs : EventArgs
{
   private View _view;

   /**
     * @brief View - is the view that gets Key Input Focus
     *
     */
   public View View
   {
      get
      {
         return _view;
      }
      set
      {
         _view = value;
      }
   }
}

/**
  * @brief Event arguments that passed via KeyInputFocusLost signal
  *
  */
public class KeyInputFocusLostEventArgs : EventArgs
{
   private View _view;

   /**
     * @brief View - is the view that loses Key Input Focus
     *
     */
   public View View
   {
      get
      {
         return _view;
      }
      set
      {
         _view = value;
      }
   }
}

/**
  * @brief Event arguments that passed via KeyEvent signal
  *
  */
public class KeyEventArgs : EventArgs
{
   private View _view;
   private KeyEvent _keyEvent;

   /**
     * @brief View - is the view that recieves the keyevent.
     *
     */
   public View View
   {
      get
      {
         return _view;
      }
      set
      {
         _view = value;
      }
   }

   /**
     * @brief KeyEvent - is the keyevent sent to the View.
     *
     */
   public KeyEvent KeyEvent
   {
      get
      {
         return _keyEvent;
      }
      set
      {
         _keyEvent = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void KeyInputFocusGainedCallbackDelegate(IntPtr control);
  private DaliEventHandler<object,KeyInputFocusGainedEventArgs> _KeyInputFocusGainedEventHandler;
  private KeyInputFocusGainedCallbackDelegate _KeyInputFocusGainedCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void KeyInputFocusLostCallbackDelegate(IntPtr control);
  private DaliEventHandler<object,KeyInputFocusLostEventArgs> _KeyInputFocusLostEventHandler;
  private KeyInputFocusLostCallbackDelegate _KeyInputFocusLostCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate bool KeyCallbackDelegate(IntPtr control, IntPtr keyEvent);
  private DaliEventHandlerWithReturnType<object,KeyEventArgs,bool> _KeyEventHandler;
  private KeyCallbackDelegate _KeyCallbackDelegate;

  /**
    * @brief Event for KeyInputFocusGained signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of KeyInputFocusGainedEventHandler-DaliEventHandler<object,KeyInputFocusGainedEventArgs>) 
    * provided by the user. KeyInputFocusGained signal is emitted when the control gets Key Input Focus.
    */
  public event DaliEventHandler<object,KeyInputFocusGainedEventArgs> KeyInputFocusGained
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_KeyInputFocusGainedEventHandler == null)
           {
              _KeyInputFocusGainedEventHandler += value;

              _KeyInputFocusGainedCallbackDelegate = new KeyInputFocusGainedCallbackDelegate(OnKeyInputFocusGained);
              this.KeyInputFocusGainedSignal().Connect(_KeyInputFocusGainedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_KeyInputFocusGainedEventHandler != null)
           {
              this.KeyInputFocusGainedSignal().Disconnect(_KeyInputFocusGainedCallbackDelegate);
           }

           _KeyInputFocusGainedEventHandler -= value;
        }
     }
  }

 private void OnKeyInputFocusGained(IntPtr view)
  {
   KeyInputFocusGainedEventArgs e = new KeyInputFocusGainedEventArgs();

   // Populate all members of "e" (KeyInputFocusGainedEventArgs) with real data
   e.View = Dali.View.GetViewFromPtr(view);

   if (_KeyInputFocusGainedEventHandler != null)
   {
      //here we send all data to user event handlers
      _KeyInputFocusGainedEventHandler(this, e);
   }

  }

  /**
    * @brief Event for KeyInputFocusLost signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of KeyInputFocusLostEventHandler-DaliEventHandler<object,KeyInputFocusLostEventArgs>) 
    * provided by the user. KeyInputFocusLost signal is emitted when the control loses Key Input Focus.
    */
  public event DaliEventHandler<object,KeyInputFocusLostEventArgs> KeyInputFocusLost
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_KeyInputFocusLostEventHandler == null)
           {
              _KeyInputFocusLostEventHandler += value;

              _KeyInputFocusLostCallbackDelegate = new KeyInputFocusLostCallbackDelegate(OnKeyInputFocusLost);
              this.KeyInputFocusLostSignal().Connect(_KeyInputFocusLostCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_KeyInputFocusLostEventHandler != null)
           {
              this.KeyInputFocusLostSignal().Disconnect(_KeyInputFocusLostCallbackDelegate);
           }

           _KeyInputFocusLostEventHandler -= value;
        }
     }
  }

 private void OnKeyInputFocusLost(IntPtr view)
  {
   KeyInputFocusLostEventArgs e = new KeyInputFocusLostEventArgs();

   // Populate all members of "e" (KeyInputFocusLostEventArgs) with real data
   e.View = Dali.View.GetViewFromPtr(view);

   if (_KeyInputFocusLostEventHandler != null)
   {
      //here we send all data to user event handlers
      _KeyInputFocusLostEventHandler(this, e);
   }
  }

  /**
    * @brief Event for KeyPressed signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of KeyEventEventHandler-DaliEventHandlerWithReturnType<object,KeyEventArgs,bool>) 
    * provided by the user. KeyPressed signal is emitted when key event is received.
    */
  public event DaliEventHandlerWithReturnType<object,KeyEventArgs,bool> KeyPressed
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_KeyEventHandler == null)
           {
              _KeyEventHandler += value;

              _KeyCallbackDelegate = new KeyCallbackDelegate(OnKeyEvent);
              this.KeyEventSignal().Connect(_KeyCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_KeyEventHandler != null)
           {
              this.KeyEventSignal().Disconnect(_KeyCallbackDelegate);
           }

           _KeyEventHandler -= value;
        }
     }
  }

 private bool OnKeyEvent(IntPtr view, IntPtr keyEvent)
  {
   KeyEventArgs e = new KeyEventArgs();

   // Populate all members of "e" (KeyEventArgs) with real data
   e.View = Dali.View.GetViewFromPtr(view);
   e.KeyEvent = Dali.KeyEvent.GetKeyEventFromPtr(keyEvent);

   if (_KeyEventHandler != null)
   {
      //here we send all data to user event handlers
      return _KeyEventHandler(this, e);
   }
   return false;

  }

 public static View GetViewFromPtr(global::System.IntPtr cPtr) {
    View ret = new View(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

%}

%enddef

%define DALI_CONTROL_EVENTHANDLER_PARAM( NameSpace, ClassName)

  CONTROL_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  CONTROL_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_CONTROL_EVENTHANDLER_PARAM( Dali::Toolkit, Control);
}
