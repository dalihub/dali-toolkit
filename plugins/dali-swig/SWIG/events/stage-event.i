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

%define STAGE_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}
%enddef

%define STAGE_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/**
  * @brief Event arguments that passed via Touch signal
  *
  */
public class TouchEventArgs : EventArgs
{
   private TouchData _touchData;

   /**
     * @brief TouchData - contains the information of touch points
     *
     */
   public TouchData TouchData
   {
      get
      {
         return _touchData;
      }
      set
      {
         _touchData = value;
      }
   }
}


/**
  * @brief Event arguments that passed via Wheel signal
  *
  */
public class WheelEventArgs : EventArgs
{
   private WheelEvent _wheelEvent;

   /**
     * @brief WheelEvent - store a wheel rolling type MOUSE_WHEEL or CUSTOM_WHEEL
     *
     */
   public WheelEvent WheelEvent
   {
      get
      {
         return _wheelEvent;
      }
      set
      {
         _wheelEvent = value;
      }
   }
}

/**
  * @brief Event arguments that passed via KeyEvent signal
  *
  */
public class KeyEventArgs : EventArgs
{
   private KeyEvent _keyEvent;

   /**
     * @brief KeyEvent - is the keyevent sent to Stage.
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

/**
  * @brief Event arguments that passed via EventProcessingFinished signal
  *
  */
public class EventProcessingFinishedEventArgs : EventArgs
{
}

/**
  * @brief Event arguments that passed via ContextLost signal
  *
  */
public class ContextLostEventArgs : EventArgs
{
}

/**
  * @brief Event arguments that passed via ContextRegained signal
  *
  */
public class ContextRegainedEventArgs : EventArgs
{
}

/**
  * @brief Event arguments that passed via SceneCreated signal
  *
  */
public class SceneCreatedEventArgs : EventArgs
{
}


  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void StageTouchCallbackDelegate(IntPtr data);
  private DaliEventHandler<object,TouchEventArgs> _stageTouchEventHandler;
  private StageTouchCallbackDelegate _stageTouchCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void WheelCallbackDelegate(IntPtr data);
  private DaliEventHandler<object,WheelEventArgs> _stageWheelEventHandler;
  private WheelCallbackDelegate _stageWheelCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void KeyCallbackDelegate(IntPtr data);
  private DaliEventHandler<object,KeyEventArgs> _stageKeyEventHandler;
  private KeyCallbackDelegate _stageKeyCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void EventProcessingFinishedEventCallbackDelegate();
  private DaliEventHandler<object,EventProcessingFinishedEventArgs> _stageEventProcessingFinishedEventHandler;
  private EventProcessingFinishedEventCallbackDelegate _stageEventProcessingFinishedEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void ContextLostEventCallbackDelegate();
  private DaliEventHandler<object,ContextLostEventArgs> _stageContextLostEventHandler;
  private ContextLostEventCallbackDelegate _stageContextLostEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void ContextRegainedEventCallbackDelegate();
  private DaliEventHandler<object,ContextRegainedEventArgs> _stageContextRegainedEventHandler;
  private ContextRegainedEventCallbackDelegate _stageContextRegainedEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void SceneCreatedEventCallbackDelegate();
  private DaliEventHandler<object,SceneCreatedEventArgs> _stageSceneCreatedEventHandler;
  private SceneCreatedEventCallbackDelegate _stageSceneCreatedEventCallbackDelegate;

  /**
    * @brief Event for Touched signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of TouchEventHandler-DaliEventHandler<object,TouchEventArgs>) provided by the user.
    * Touched signal is emitted when the screen is touched and when the touch ends
    * (i.e. the down & up touch events only).
    *
    */
  public event DaliEventHandler<object,TouchEventArgs> Touched
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_stageTouchEventHandler == null)
           {
              _stageTouchEventHandler += value;

              _stageTouchCallbackDelegate = new StageTouchCallbackDelegate(OnStageTouch);
              this.TouchSignal().Connect(_stageTouchCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_stageTouchEventHandler != null)
           {
              this.TouchSignal().Disconnect(_stageTouchCallbackDelegate);
           }

           _stageTouchEventHandler -= value;
        }
     }
  }

  // Callback for Stage Touch signal
  private void OnStageTouch(IntPtr data)
  {
     TouchEventArgs e = new TouchEventArgs();

     // Populate all members of "e" (TouchEventArgs) with real data
     e.TouchData = TouchData.GetTouchDataFromPtr( data );

     if (_stageTouchEventHandler != null)
     {
        //here we send all data to user event handlers
        _stageTouchEventHandler(this, e);
     }
  }

  /**
    * @brief Event for WheelMoved signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of WheelEventHandler-DaliEventHandler<object,WheelEventArgs>) provided by the user.
    * WheelMoved signal is emitted is emitted when wheel event is received.
    *
    */
  public event DaliEventHandler<object,WheelEventArgs> WheelMoved
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_stageWheelEventHandler == null)
           {
              _stageWheelEventHandler += value;

              _stageWheelCallbackDelegate = new WheelCallbackDelegate(OnStageWheel);
              this.WheelEventSignal().Connect(_stageWheelCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_stageWheelEventHandler != null)
           {
              this.WheelEventSignal().Disconnect(_stageWheelCallbackDelegate);
           }

           _stageWheelEventHandler -= value;
        }
     }
  }

  // Callback for Stage WheelEventsignal
  private void OnStageWheel(IntPtr data)
  {
     WheelEventArgs e = new WheelEventArgs();

     // Populate all members of "e" (WheelEventArgs) with real data
     e.WheelEvent = Dali.WheelEvent.GetWheelEventFromPtr( data );

     if (_stageWheelEventHandler != null)
     {
        //here we send all data to user event handlers
        _stageWheelEventHandler(this, e);
     }
  }

  /**
    * @brief Event for KeyPressed signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of KeyEventHandler-DaliEventHandler<object,KeyEventArgs>) provided by the user.
    * KeyPressed signal is emitted is emitted when key event is received.
    *
    */
  public event DaliEventHandler<object,KeyEventArgs> KeyPressed
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_stageKeyEventHandler == null)
           {
              _stageKeyEventHandler += value;

              _stageKeyCallbackDelegate = new KeyCallbackDelegate(OnStageKey);
              this.KeyEventSignal().Connect(_stageKeyCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_stageKeyEventHandler != null)
           {
              this.KeyEventSignal().Disconnect(_stageKeyCallbackDelegate);
           }

           _stageKeyEventHandler -= value;
        }
     }
  }

  // Callback for Stage KeyEventsignal
  private void OnStageKey(IntPtr keyEvent)
  {
     KeyEventArgs e = new KeyEventArgs();

     // Populate all members of "e" (KeyEventArgs) with real data
     e.KeyEvent = Dali.KeyEvent.GetKeyEventFromPtr( keyEvent );

     if (_stageKeyEventHandler != null)
     {
        //here we send all data to user event handlers
        _stageKeyEventHandler(this, e);
     }
  }

  /**
    * @brief Event for EventProcessingFinished signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of EventProcessingFinishedEventHandler-DaliEventHandler<object,EventProcessingFinishedEventArgs>) 
    * provided by the user. EventProcessingFinished signal is emitted just after the event processing is finished.
    *
    */
  public event DaliEventHandler<object,EventProcessingFinishedEventArgs> EventProcessingFinished
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_stageEventProcessingFinishedEventHandler == null)
           {
              _stageEventProcessingFinishedEventHandler += value;

              _stageEventProcessingFinishedEventCallbackDelegate = new EventProcessingFinishedEventCallbackDelegate(OnEventProcessingFinished);
              this.EventProcessingFinishedSignal().Connect(_stageEventProcessingFinishedEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_stageEventProcessingFinishedEventHandler != null)
           {
              this.EventProcessingFinishedSignal().Disconnect(_stageEventProcessingFinishedEventCallbackDelegate);
           }

           _stageEventProcessingFinishedEventHandler -= value;
        }
     }
  }

  // Callback for Stage EventProcessingFinishedSignal
  private void OnEventProcessingFinished()
  {
     EventProcessingFinishedEventArgs e = new EventProcessingFinishedEventArgs();

     if (_stageEventProcessingFinishedEventHandler != null)
     {
        //here we send all data to user event handlers
        _stageEventProcessingFinishedEventHandler(this, e);
     }
  }

  /**
    * @brief Event for ContextLost signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of ContextLostEventHandler-DaliEventHandler<object,ContextLostEventArgs>) provided by the user.
    * ContextLost signal is emitted when the GL context is lost (Platform specific behaviour).
    *
    */
  public event DaliEventHandler<object,ContextLostEventArgs> ContextLost
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_stageContextLostEventHandler == null)
           {
              _stageContextLostEventHandler += value;

              _stageContextLostEventCallbackDelegate = new ContextLostEventCallbackDelegate(OnContextLost);
              this.ContextLostSignal().Connect(_stageContextLostEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_stageContextLostEventHandler != null)
           {
              this.ContextLostSignal().Disconnect(_stageContextLostEventCallbackDelegate);
           }

           _stageContextLostEventHandler -= value;
        }
     }
  }

  // Callback for Stage ContextLostSignal
  private void OnContextLost()
  {
     ContextLostEventArgs e = new ContextLostEventArgs();

     if (_stageContextLostEventHandler != null)
     {
        //here we send all data to user event handlers
        _stageContextLostEventHandler(this, e);
     }
  }

  /**
    * @brief Event for ContextRegained signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of ContextRegainedEventHandler-DaliEventHandler<object,ContextRegainedEventArgs>) 
    * provided by the user. ContextRegained signal is emitted when the GL context is regained (Platform specific
    * behaviour).
    *
    */
  public event DaliEventHandler<object,ContextRegainedEventArgs> ContextRegained
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_stageContextRegainedEventHandler == null)
           {
              _stageContextRegainedEventHandler += value;

              _stageContextRegainedEventCallbackDelegate = new ContextRegainedEventCallbackDelegate(OnContextRegained);
              this.ContextRegainedSignal().Connect(_stageContextRegainedEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_stageContextRegainedEventHandler != null)
           {
              this.ContextRegainedSignal().Disconnect(_stageContextRegainedEventCallbackDelegate);
           }

           _stageContextRegainedEventHandler -= value;
        }
     }
  }

  // Callback for Stage ContextRegainedSignal
  private void OnContextRegained()
  {
     ContextRegainedEventArgs e = new ContextRegainedEventArgs();

     if (_stageContextRegainedEventHandler != null)
     {
        //here we send all data to user event handlers
        _stageContextRegainedEventHandler(this, e);
     }
  }

  /**
    * @brief Event for SceneCreated signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of SceneCreatedEventHandler-DaliEventHandler<object,SceneCreatedEventArgs>) provided by the user.
    * SceneCreated signal is emitted after the initial scene is created.
    *
    */
  public event DaliEventHandler<object,SceneCreatedEventArgs> SceneCreated
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_stageSceneCreatedEventHandler == null)
           {
              _stageSceneCreatedEventHandler += value;

              _stageSceneCreatedEventCallbackDelegate = new SceneCreatedEventCallbackDelegate(OnSceneCreated);
              this.SceneCreatedSignal().Connect(_stageSceneCreatedEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_stageSceneCreatedEventHandler != null)
           {
              this.SceneCreatedSignal().Disconnect(_stageSceneCreatedEventCallbackDelegate);
           }

           _stageSceneCreatedEventHandler -= value;
        }
     }
  }

  // Callback for Stage SceneCreatedSignal
  private void OnSceneCreated()
  {
     SceneCreatedEventArgs e = new SceneCreatedEventArgs();

     if (_stageSceneCreatedEventHandler != null)
     {
        //here we send all data to user event handlers
        _stageSceneCreatedEventHandler(this, e);
     }
  }

  ///< name "Size", type Dali.Size (Stage Size value)
  //@since 1.0.0
  public Dali.Size Size
  {
     get
     {
        Vector2 ret1 = GetSize();
        Dali.Size ret= new Size(ret1);
        return ret;
     }
  }

  ///< name "BackgroundColor", type Dali.Color (Stage background color value)
  //@since 1.0.0
  public Dali.Color BackgroundColor
  {
     set
     {
        SetBackgroundColor(value);
     }
     get
     {
        Vector4 ret1 = GetBackgroundColor();
        Dali.Color ret = new Color(ret1);
        return ret;
     }
   }

%}

%enddef


%define DALI_STAGE_EVENTHANDLER_PARAM( NameSpace, ClassName)

  STAGE_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  STAGE_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_STAGE_EVENTHANDLER_PARAM( Dali, Stage);
}
