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

  private event EventHandler<TouchEventArgs> _stageTouchEventHandler;
  private EventCallbackDelegateType1<IntPtr> _stageTouchCallbackDelegate;

  /**
    * @brief Event for TouchEvent signal which can be used to subscribe/unsubscribe the event handler
    * TouchEvent signal is emitted when the screen is touched and when the touch ends
    * (i.e. the down & up touch events only).
    *
    */
  public event EventHandler<TouchEventArgs> TouchEvent
  {
     add
     {
        lock(this)
        {
          _stageTouchEventHandler += value;
          _stageTouchCallbackDelegate = OnStageTouch;
          this.TouchSignal().Connect(_stageTouchCallbackDelegate);
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

  private void OnStageTouch(IntPtr data)
  {
    TouchEventArgs e = new TouchEventArgs();

    if( data != null )
    {
      e.TouchData = TouchData.GetTouchDataFromPtr( data );
    }

    if (_stageTouchEventHandler != null)
    {
      _stageTouchEventHandler(this, e);
    }
  }


  /**
    * @brief WheelEvent arguments that passed via Wheel signal
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

  private event EventHandler<WheelEventArgs> _stageWheelEventHandler;
  private EventCallbackDelegateType1<IntPtr> _stageWheelCallbackDelegate;

  /**
    * @brief Event for WheelEvent signal which can be used to subscribe/unsubscribe the event handler
    * WheelEvent signal is emitted is emitted when wheel event is received.
    *
    */
  public event EventHandler<WheelEventArgs> WheelEvent
  {
     add
     {
        lock(this)
        {
          _stageWheelEventHandler += value;
          _stageWheelCallbackDelegate = OnStageWheel;
          this.WheelEventSignal().Connect(_stageWheelCallbackDelegate);
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

  private void OnStageWheel(IntPtr data)
  {
    WheelEventArgs e = new WheelEventArgs();

    if( data != null )
    {
      e.WheelEvent = Dali.WheelEvent.GetWheelEventFromPtr( data );
    }

    if (_stageWheelEventHandler != null)
    {
      _stageWheelEventHandler(this, e);
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

  private event EventHandler<KeyEventArgs> _stageKeyEventHandler;
  private EventCallbackDelegateType1<IntPtr> _stageKeyCallbackDelegate;

  /**
    * @brief Event for KeyEvent signal which can be used to subscribe/unsubscribe the event handler
    * KeyEvent signal is emitted is emitted when key event is received.
    *
    */
  public event EventHandler<KeyEventArgs> KeyEvent
  {
     add
     {
        lock(this)
        {
            _stageKeyEventHandler += value;
            _stageKeyCallbackDelegate = OnStageKey;
            this.KeyEventSignal().Connect(_stageKeyCallbackDelegate);
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
  private void OnStageKey(IntPtr data)
  {
    KeyEventArgs e = new KeyEventArgs();

    if( data != null )
    {
      e.KeyEvent = Dali.KeyEvent.GetKeyEventFromPtr( data );
    }

    if (_stageKeyEventHandler != null)
    {
      //here we send all data to user event handlers
      _stageKeyEventHandler(this, e);
    }
  }


  private event EventHandler _stageEventProcessingFinishedEventHandler;
  private EventCallbackDelegateType0 _stageEventProcessingFinishedEventCallbackDelegate;

  /**
    * @brief Event for EventProcessingFinished signal which can be used to subscribe/unsubscribe the event handler
    * provided by the user. EventProcessingFinished signal is emitted just after the event processing is finished.
    *
    */
  public event EventHandler EventProcessingFinished
  {
     add
     {
        lock(this)
        {
          _stageEventProcessingFinishedEventHandler += value;
          _stageEventProcessingFinishedEventCallbackDelegate = OnEventProcessingFinished;
          this.EventProcessingFinishedSignal().Connect(_stageEventProcessingFinishedEventCallbackDelegate);
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
     if (_stageEventProcessingFinishedEventHandler != null)
     {
        _stageEventProcessingFinishedEventHandler(this, null);
     }
  }


  private EventHandler _stageContextLostEventHandler;
  private EventCallbackDelegateType0 _stageContextLostEventCallbackDelegate;

  /**
    * @brief Event for ContextLost signal which can be used to subscribe/unsubscribe the event handler
    * ContextLost signal is emitted when the GL context is lost (Platform specific behaviour).
    *
    */
  public event EventHandler ContextLost
  {
     add
     {
        lock(this)
        {
            _stageContextLostEventHandler += value;
            _stageContextLostEventCallbackDelegate = OnContextLost;
            this.ContextLostSignal().Connect(_stageContextLostEventCallbackDelegate);
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
     if (_stageContextLostEventHandler != null)
     {
        _stageContextLostEventHandler(this, null);
     }
  }


  private EventHandler _stageContextRegainedEventHandler;
  private EventCallbackDelegateType0 _stageContextRegainedEventCallbackDelegate;

  /**
    * @brief Event for ContextRegained signal which can be used to subscribe/unsubscribe the event handler
    * provided by the user. ContextRegained signal is emitted when the GL context is regained (Platform specific
    * behaviour).
    *
    */
  public event EventHandler ContextRegained
  {
     add
     {
        lock(this)
        {
            _stageContextRegainedEventHandler += value;
            _stageContextRegainedEventCallbackDelegate = OnContextRegained;
            this.ContextRegainedSignal().Connect(_stageContextRegainedEventCallbackDelegate);
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
     if (_stageContextRegainedEventHandler != null)
     {
        _stageContextRegainedEventHandler(this, null);
     }
  }


  private EventHandler _stageSceneCreatedEventHandler;
  private EventCallbackDelegateType0 _stageSceneCreatedEventCallbackDelegate;

  /**
    * @brief Event for SceneCreated signal which can be used to subscribe/unsubscribe the event handler
    * SceneCreated signal is emitted after the initial scene is created.
    *
    */
  public event EventHandler SceneCreated
  {
     add
     {
        lock(this)
        {
            _stageSceneCreatedEventHandler += value;
            _stageSceneCreatedEventCallbackDelegate = OnSceneCreated;
            this.SceneCreatedSignal().Connect(_stageSceneCreatedEventCallbackDelegate);
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
     if (_stageSceneCreatedEventHandler != null)
     {
        _stageSceneCreatedEventHandler(this, null);
     }
  }


  public Vector2 Size
  {
     get
     {
        Vector2 ret = GetSize();
        return ret;
     }
  }

  public Vector4 BackgroundColor
  {
     set
     {
        SetBackgroundColor(value);
     }
     get
     {
        Vector4 ret = GetBackgroundColor();
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
