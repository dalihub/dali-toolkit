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

%define ACTOR_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
    using System;
    using System.Runtime.InteropServices;
%}

%enddef

%define ACTOR_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
  %typemap(cscode) NameSpace::ClassName %{


    /**
      * @brief Event arguments that passed via Touch signal
      *
      */
    public class TouchEventArgs : EventArgs
    {
       private Actor _actor;
       private TouchData _touchData;

      /**
        * @brief Actor - is the actor that is being touched
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
      * @brief Event arguments that passed via Hover signal
      *
      */
    public class HoverEventArgs : EventArgs
    {
    private Actor _actor;
    private HoverEvent _hoverEvent;

      /**
        * @brief Actor - is the actor that is being hovered
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

      /**
        * @brief HoverEvent - contains touch points that represent the points
        * that are currently being hovered or the points where a hover has stopped
        *
        */
      public HoverEvent HoverEvent
      {
        get
        {
          return _hoverEvent;
        }
        set
        {
          _hoverEvent = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via Wheel signal
      *
      */
    public class WheelEventArgs : EventArgs
    {
    private Actor _actor;
    private WheelEvent _wheelEvent;

      /**
        * @brief Actor - is the actor that is being wheeled
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

      /**
        * @brief WheelEvent - store a wheel rolling type : MOUSE_WHEEL or CUSTOM_WHEEL
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
      * @brief Event arguments that passed via OnStage signal
      *
      */
    public class OnStageEventArgs : EventArgs
    {
    private Actor _actor;

      /**
        * @brief Actor - is the actor that is being connected to the stage
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

    /**
      * @brief Event arguments that passed via OffStage signal
      *
      */
    public class OffStageEventArgs : EventArgs
    {
    private Actor _actor;

      /**
        * @brief Actor - is the actor that is being disconnected from the stage
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

    /**
      * @brief Event arguments that passed via OnRelayout signal
      *
      */
    public class OnRelayoutEventArgs : EventArgs
    {
      private Actor _actor;

      /**
        * @brief Actor - is the actor that is being resized upon relayout
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
    private delegate bool TouchCallbackDelegate(IntPtr actor, IntPtr touchData);
    private DaliEventHandlerWithReturnType<object,TouchEventArgs,bool> _actorTouchDataEventHandler;
    private TouchCallbackDelegate _actorTouchDataCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool HoverEventCallbackDelegate(IntPtr actor, IntPtr hoverEvent);
    private DaliEventHandlerWithReturnType<object,HoverEventArgs,bool> _actorHoverEventHandler;
    private HoverEventCallbackDelegate _actorHoverEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool WheelEventCallbackDelegate(IntPtr actor, IntPtr wheelEvent);
    private DaliEventHandlerWithReturnType<object,WheelEventArgs,bool> _actorWheelEventHandler;
    private WheelEventCallbackDelegate _actorWheelEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void OnStageEventCallbackDelegate(IntPtr actor);
    private DaliEventHandler<object,OnStageEventArgs> _actorOnStageEventHandler;
    private OnStageEventCallbackDelegate _actorOnStageEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void OffStageEventCallbackDelegate(IntPtr actor);
    private DaliEventHandler<object,OffStageEventArgs> _actorOffStageEventHandler;
    private OffStageEventCallbackDelegate _actorOffStageEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void OnRelayoutEventCallbackDelegate(IntPtr actor);
    private DaliEventHandler<object,OnRelayoutEventArgs> _actorOnRelayoutEventHandler;
    private OnRelayoutEventCallbackDelegate _actorOnRelayoutEventCallbackDelegate;

    /**
      * @brief Event for Touched signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of TouchEventHandler-DaliEventHandlerWithReturnType<object,TouchEventArgs,bool>)
      * provided by the user. Touched signal is emitted when touch input is received.
      */
    public event DaliEventHandlerWithReturnType<object,TouchEventArgs,bool> Touched
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_actorTouchDataEventHandler == null)
          {
            _actorTouchDataEventHandler += value;

            _actorTouchDataCallbackDelegate = new TouchCallbackDelegate(OnTouch);
            this.TouchSignal().Connect(_actorTouchDataCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_actorTouchDataEventHandler != null)
          {
            this.TouchSignal().Disconnect(_actorTouchDataCallbackDelegate);
          }

          _actorTouchDataEventHandler -= value;
        }
      }
    }

    // Callback for Actor TouchSignal
    private bool OnTouch(IntPtr actor, IntPtr touchData)
    {
      TouchEventArgs e = new TouchEventArgs();

      // Populate all members of "e" (TouchEventArgs) with real data
      e.Actor = Actor.GetActorFromPtr(actor);
      e.TouchData = Dali.TouchData.GetTouchDataFromPtr(touchData);

      if (_actorTouchDataEventHandler != null)
      {
        //here we send all data to user event handlers
        return _actorTouchDataEventHandler(this, e);
      }

      return false;
    }

    /**
      * @brief Event for Hovered signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of HoverEventHandler-DaliEventHandlerWithReturnType<object,HoverEventArgs,bool>)
      * provided by the user. Hovered signal is emitted when hover input is received.
      */
    public event DaliEventHandlerWithReturnType<object,HoverEventArgs,bool> Hovered
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_actorHoverEventHandler == null)
          {
            _actorHoverEventHandler += value;

            _actorHoverEventCallbackDelegate = new HoverEventCallbackDelegate(OnHoverEvent);
            this.HoveredSignal().Connect(_actorHoverEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_actorHoverEventHandler != null)
          {
            this.HoveredSignal().Disconnect(_actorHoverEventCallbackDelegate);
          }

          _actorHoverEventHandler -= value;
        }
      }
    }

    // Callback for Actor Hover signal
    private bool OnHoverEvent(IntPtr actor, IntPtr hoverEvent)
    {
      HoverEventArgs e = new HoverEventArgs();

      // Populate all members of "e" (HoverEventArgs) with real data
      e.Actor = Actor.GetActorFromPtr(actor);
      e.HoverEvent = Dali.HoverEvent.GetHoverEventFromPtr(hoverEvent);

      if (_actorHoverEventHandler != null)
      {
        //here we send all data to user event handlers
        return _actorHoverEventHandler(this, e);
      }

      return false;
    }

    /**
      * @brief Event for WheelMoved signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of WheelEventHandler-DaliEventHandlerWithReturnType<object,WheelEventArgs,bool>)
      * provided by the user. WheelMoved signal is emitted when wheel event is received.
      */
    public event DaliEventHandlerWithReturnType<object,WheelEventArgs,bool> WheelMoved
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_actorWheelEventHandler == null)
          {
            _actorWheelEventHandler += value;

            _actorWheelEventCallbackDelegate = new WheelEventCallbackDelegate(OnWheelEvent);
            this.WheelEventSignal().Connect(_actorWheelEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_actorWheelEventHandler != null)
          {
            this.WheelEventSignal().Disconnect(_actorWheelEventCallbackDelegate);
          }

          _actorWheelEventHandler -= value;
        }
      }
    }

    // Callback for Actor Wheel signal
    private bool OnWheelEvent(IntPtr actor, IntPtr wheelEvent)
    {
      WheelEventArgs e = new WheelEventArgs();

      // Populate all members of "e" (WheelEventArgs) with real data
      e.Actor = Actor.GetActorFromPtr(actor);
      e.WheelEvent = Dali.WheelEvent.GetWheelEventFromPtr(wheelEvent);

      if (_actorWheelEventHandler != null)
      {
        //here we send all data to user event handlers
        return _actorWheelEventHandler(this, e);
      }

      return false;
    }

    /**
      * @brief Event for OnStage signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of OnStageEventHandler) provided by the user.
      * OnStage signal is emitted after the actor has been connected to the stage.
      */
    public event DaliEventHandler<object,OnStageEventArgs> OnStageEvent
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_actorOnStageEventHandler == null)
          {
            _actorOnStageEventHandler += value;

            _actorOnStageEventCallbackDelegate = new OnStageEventCallbackDelegate(OnStage);
            this.OnStageSignal().Connect(_actorOnStageEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_actorOnStageEventHandler != null)
          {
            this.OnStageSignal().Disconnect(_actorOnStageEventCallbackDelegate);
          }

          _actorOnStageEventHandler -= value;
        }
      }
    }

    // Callback for Actor OnStage signal
    private void OnStage(IntPtr data)
    {
      OnStageEventArgs e = new OnStageEventArgs();

      // Populate all members of "e" (OnStageEventArgs) with real data
      e.Actor = Actor.GetActorFromPtr(data);

      if (_actorOnStageEventHandler != null)
      {
        //here we send all data to user event handlers
        _actorOnStageEventHandler(this, e);
      }
    }

    /**
      * @brief Event for OffStage signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of OffStageEventHandler) provided by the user.
      * OffStage signal is emitted after the actor has been disconnected from the stage.
      */
    public event DaliEventHandler<object,OffStageEventArgs> OffStageEvent
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_actorOffStageEventHandler == null)
          {
            _actorOffStageEventHandler += value;

            _actorOffStageEventCallbackDelegate = new OffStageEventCallbackDelegate(OffStage);
            this.OnStageSignal().Connect(_actorOffStageEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_actorOffStageEventHandler != null)
          {
            this.OnStageSignal().Disconnect(_actorOffStageEventCallbackDelegate);
          }

          _actorOffStageEventHandler -= value;
        }
      }
    }

    // Callback for Actor OffStage signal
    private void OffStage(IntPtr data)
    {
      OffStageEventArgs e = new OffStageEventArgs();

      // Populate all members of "e" (OffStageEventArgs) with real data
      e.Actor = Actor.GetActorFromPtr(data);

      if (_actorOffStageEventHandler != null)
      {
        //here we send all data to user event handlers
        _actorOffStageEventHandler(this, e);
      }
    }

    /**
      * @brief Event for OnRelayout signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of OnRelayoutEventHandler) provided by the user.
      * OnRelayout signal is emitted after the size has been set on the actor during relayout.
      */
    public event DaliEventHandler<object,OnRelayoutEventArgs> OnRelayoutEvent
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_actorOnRelayoutEventHandler == null)
          {
            _actorOnRelayoutEventHandler += value;

            _actorOnRelayoutEventCallbackDelegate = new OnRelayoutEventCallbackDelegate(OnRelayout);
            this.OnRelayoutSignal().Connect(_actorOnRelayoutEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_actorOnRelayoutEventHandler != null)
          {
            this.OnRelayoutSignal().Disconnect(_actorOnRelayoutEventCallbackDelegate);
          }

          _actorOnRelayoutEventHandler -= value;
        }
      }
    }

    // Callback for Actor OnRelayout signal
    private void OnRelayout(IntPtr data)
    {
      OnRelayoutEventArgs e = new OnRelayoutEventArgs();

      // Populate all members of "e" (OnRelayoutEventArgs) with real data
      e.Actor = Actor.GetActorFromPtr(data);

      if (_actorOnRelayoutEventHandler != null)
      {
        //here we send all data to user event handlers
        _actorOnRelayoutEventHandler(this, e);
      }
    }

    public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
      ClassName ret = new ClassName(cPtr, false);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    }

    public IntPtr GetPtrfrom ## ClassName ()
    {
       return (IntPtr)swigCPtr;
    }

    public Actor Parent
    {
      get
      {
        Actor parent = GetParent();
        return parent;
      }
    }

   public bool Vibility
   {
      get
      {
        bool visibility = IsVisible();
        return visibility;
      }
    }

   public float Opacity
   {
      set
      {
        SetOpacity(value);
      }
      get
      {
        float ret = GetCurrentOpacity();
        return ret;
      }
   }

    public bool StateFocusEnable
    {
      set
      {
        SetKeyboardFocusable(value);
      }
      get
      {
        bool focusable = IsKeyboardFocusable();
        return focusable;
      }
    }

%}

%enddef

%define DALI_ACTOR_EVENTHANDLER_PARAM( NameSpace, ClassName)

ACTOR_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
ACTOR_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_ACTOR_EVENTHANDLER_PARAM( Dali, Actor);
}
