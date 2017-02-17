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
     private Touch _touch;

     /**
       * @brief Touch - contains the information of touch points
       *
       */
     public Touch Touch
     {
        get
        {
           return _touch;
        }
        set
        {
           _touch = value;
        }
     }
  }

  private event EventHandler<TouchEventArgs> _stageTouchHandler;
  private EventCallbackDelegateType1<IntPtr> _stageTouchCallbackDelegate;

  /**
    * @brief Event for TouchEvent signal which can be used to subscribe/unsubscribe the event handler
    * TouchEvent signal is emitted when the screen is touched and when the touch ends
    * (i.e. the down & up touch events only).
    *
    */
  public event EventHandler<TouchEventArgs> Touch
  {
     add
     {
        lock(this)
        {
          _stageTouchHandler += value;
          _stageTouchCallbackDelegate = OnStageTouch;
          this.TouchSignal().Connect(_stageTouchCallbackDelegate);
        }
     }
     remove
     {
        lock(this)
        {
           if (_stageTouchHandler != null)
           {
              this.TouchSignal().Disconnect(_stageTouchCallbackDelegate);
           }
           _stageTouchHandler -= value;
        }
     }
  }

  private void OnStageTouch(IntPtr data)
  {
    TouchEventArgs e = new TouchEventArgs();

    if( data != null )
    {
      e.Touch = Dali.Touch.GetTouchFromPtr( data );
    }

    if (_stageTouchHandler != null)
    {
      _stageTouchHandler(this, e);
    }
  }

  /**
    * @brief Wheel arguments that passed via Wheel signal
    *
    */
  public class WheelEventArgs : EventArgs
  {
     private Wheel _wheel;

     /**
       * @brief Wheel - store a wheel rolling type MOUSE_WHEEL or CUSTOM_WHEEL
       *
       */
     public Wheel Wheel
     {
        get
        {
           return _wheel;
        }
        set
        {
           _wheel = value;
        }
     }
  }

  private event EventHandler<WheelEventArgs> _stageWheelHandler;
  private EventCallbackDelegateType1<IntPtr> _stageWheelCallbackDelegate;

  /**
    * @brief Event for Wheel signal which can be used to subscribe/unsubscribe the event handler
    * Wheel signal is emitted is emitted when wheel event is received.
    *
    */
  public event EventHandler<WheelEventArgs> Wheel
  {
     add
     {
        lock(this)
        {
          _stageWheelHandler += value;
          _stageWheelCallbackDelegate = OnStageWheel;
          this.WheelEventSignal().Connect(_stageWheelCallbackDelegate);
        }
     }
     remove
     {
        lock(this)
        {
           if (_stageWheelHandler != null)
           {
              this.WheelEventSignal().Disconnect(_stageWheelCallbackDelegate);
           }
           _stageWheelHandler -= value;
        }
     }
  }

  private void OnStageWheel(IntPtr data)
  {
    WheelEventArgs e = new WheelEventArgs();

    if( data != null )
    {
      e.Wheel = Dali.Wheel.GetWheelFromPtr( data );
    }

    if (_stageWheelHandler != null)
    {
      _stageWheelHandler(this, e);
    }
  }

  /**
    * @brief Event arguments that passed via Key signal
    *
    */
  public class KeyEventArgs : EventArgs
  {
     private Key _key;

     /**
       * @brief Key - is the keyevent sent to Stage.
       *
       */
     public Key Key
     {
        get
        {
           return _key;
        }
        set
        {
           _key = value;
        }
     }
  }

  private event EventHandler<KeyEventArgs> _stageKeyHandler;
  private EventCallbackDelegateType1<IntPtr> _stageKeyCallbackDelegate;

  /**
    * @brief Event for Key signal which can be used to subscribe/unsubscribe the event handler
    * Key signal is emitted is emitted when key event is received.
    *
    */
  public event EventHandler<KeyEventArgs> Key
  {
     add
     {
        lock(this)
        {
            _stageKeyHandler += value;
            _stageKeyCallbackDelegate = OnStageKey;
            this.KeyEventSignal().Connect(_stageKeyCallbackDelegate);
        }
     }
     remove
     {
        lock(this)
        {
           if (_stageKeyHandler != null)
           {
              this.KeyEventSignal().Disconnect(_stageKeyCallbackDelegate);
           }
           _stageKeyHandler -= value;
        }
     }
  }

  // Callback for Stage KeyEventsignal
  private void OnStageKey(IntPtr data)
  {
    KeyEventArgs e = new KeyEventArgs();

    if( data != null )
    {
      e.Key = Dali.Key.GetKeyFromPtr( data );
    }

    if (_stageKeyHandler != null)
    {
      //here we send all data to user event handlers
      _stageKeyHandler(this, e);
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

  private static readonly Stage instance = Stage.GetCurrent();

  public static Stage Instance
  {
      get
      {
          return instance;
      }
  }

  public Layer GetDefaultLayer()
  {
    return this.GetRootLayer();
  }

  public void AddLayer(Layer layer)
  {
    this.Add( (Actor)layer );
  }

  public void RemoveLayer(Layer layer)
  {
    this.Remove( (Actor)layer );
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
