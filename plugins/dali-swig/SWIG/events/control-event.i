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
     * @brief Event arguments that passed via Key signal
     *
     */
    public class KeyEventArgs : EventArgs
    {
      private View _view;
      private Key _key;

      /**
       * @brief View - is the view that recieves the key.
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
       * @brief Key - is the key sent to the View.
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

    /**
      * @brief Event arguments that passed via OnRelayout signal
      *
      */
    public class OnRelayoutEventArgs : EventArgs
    {
      private View _view;

      /**
        * @brief View - is the view that is being resized upon relayout
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
      * @brief Event arguments that passed via Touch signal
      *
      */
    public class TouchEventArgs : EventArgs
    {
       private View _view;
       private Touch _touch;

      /**
        * @brief View - is the view that is being touched
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

    /**
      * @brief Event arguments that passed via Hover signal
      *
      */
    public class HoverEventArgs : EventArgs
    {
       private View _view;
       private Hover _hover;

      /**
        * @brief View - is the view that is being hovered
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
        * @brief Hover - contains touch points that represent the points
        * that are currently being hovered or the points where a hover has stopped
        *
        */
      public Hover Hover
      {
        get
        {
          return _hover;
        }
        set
        {
          _hover = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via Wheel signal
      *
      */
    public class WheelEventArgs : EventArgs
    {
      private View _view;
      private Wheel _wheel;

      /**
        * @brief View - is the view that is being wheeled
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
        * @brief Wheel - store a wheel rolling type : MOUSE_WHEEL or CUSTOM_WHEEL
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

    /**
      * @brief Event arguments that passed via OnStage signal
      *
      */
    public class OnStageEventArgs : EventArgs
    {
    private View _view;

      /**
        * @brief View - is the view that is being connected to the stage
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
      * @brief Event arguments that passed via OffStage signal
      *
      */
    public class OffStageEventArgs : EventArgs
    {
    private View _view;

      /**
        * @brief View - is the view that is being disconnected from the stage
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

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void KeyInputFocusGainedCallbackDelegate(IntPtr control);
    private DaliEventHandler<object,KeyInputFocusGainedEventArgs> _KeyInputFocusGainedEventHandler;
    private KeyInputFocusGainedCallbackDelegate _KeyInputFocusGainedCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void KeyInputFocusLostCallbackDelegate(IntPtr control);
    private DaliEventHandler<object,KeyInputFocusLostEventArgs> _KeyInputFocusLostEventHandler;
    private KeyInputFocusLostCallbackDelegate _KeyInputFocusLostCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool KeyCallbackDelegate(IntPtr control, IntPtr key);
    private DaliEventHandlerWithReturnType<object,KeyEventArgs,bool> _KeyHandler;
    private KeyCallbackDelegate _KeyCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void OnRelayoutEventCallbackDelegate(IntPtr control);
    private DaliEventHandler<object,OnRelayoutEventArgs> _viewOnRelayoutEventHandler;
    private OnRelayoutEventCallbackDelegate _viewOnRelayoutEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool TouchCallbackDelegate(IntPtr view, IntPtr touch);
    private DaliEventHandlerWithReturnType<object,TouchEventArgs,bool> _viewTouchHandler;
    private TouchCallbackDelegate _viewTouchCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool HoverCallbackDelegate(IntPtr view, IntPtr hover);
    private DaliEventHandlerWithReturnType<object,HoverEventArgs,bool> _viewHoverHandler;
    private HoverCallbackDelegate _viewHoverCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool WheelCallbackDelegate(IntPtr view, IntPtr wheel);
    private DaliEventHandlerWithReturnType<object,WheelEventArgs,bool> _viewWheelHandler;
    private WheelCallbackDelegate _viewWheelCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void OnStageEventCallbackDelegate(IntPtr control);
    private DaliEventHandler<object,OnStageEventArgs> _viewOnStageEventHandler;
    private OnStageEventCallbackDelegate _viewOnStageEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void OffStageEventCallbackDelegate(IntPtr control);
    private DaliEventHandler<object,OffStageEventArgs> _viewOffStageEventHandler;
    private OffStageEventCallbackDelegate _viewOffStageEventCallbackDelegate;

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
            Console.WriteLine("View Key EVENT Locked....");
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
      Console.WriteLine("View Key ....");
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
     * (in the type of KeyHandler-DaliEventHandlerWithReturnType<object,KeyEventArgs,bool>)
     * provided by the user. KeyPressed signal is emitted when key event is received.
     */
    public event DaliEventHandlerWithReturnType<object,KeyEventArgs,bool> KeyPressed
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_KeyHandler == null)
          {
            _KeyHandler += value;

            _KeyCallbackDelegate = new KeyCallbackDelegate(OnKey);
            this.KeyEventSignal().Connect(_KeyCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_KeyHandler != null)
          {
            this.KeyEventSignal().Disconnect(_KeyCallbackDelegate);
          }

          _KeyHandler -= value;
        }
      }
    }

    private bool OnKey(IntPtr view, IntPtr key)
    {
      KeyEventArgs e = new KeyEventArgs();

      // Populate all members of "e" (KeyEventArgs) with real data
      e.View = Dali.View.GetViewFromPtr(view);
      e.Key = Dali.Key.GetKeyFromPtr(key);

      if (_KeyHandler != null)
      {
        //here we send all data to user event handlers
        return _KeyHandler(this, e);
      }
      return false;

    }

    /**
      * @brief Event for OnRelayout signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of OnRelayoutEventHandler) provided by the user.
      * OnRelayout signal is emitted after the size has been set on the view during relayout.
      */
    public event DaliEventHandler<object,OnRelayoutEventArgs> OnRelayoutEvent
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_viewOnRelayoutEventHandler == null)
          {
            _viewOnRelayoutEventHandler += value;
            Console.WriteLine("View OnRelayoutEventArgs Locked....");
            _viewOnRelayoutEventCallbackDelegate = new OnRelayoutEventCallbackDelegate(OnRelayout);
            this.OnRelayoutSignal().Connect(_viewOnRelayoutEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_viewOnRelayoutEventHandler != null)
          {
            this.OnRelayoutSignal().Disconnect(_viewOnRelayoutEventCallbackDelegate);
          }

          _viewOnRelayoutEventHandler -= value;
        }
      }
    }

    // Callback for View OnRelayout signal
    private void OnRelayout(IntPtr data)
    {
      OnRelayoutEventArgs e = new OnRelayoutEventArgs();
      Console.WriteLine("View OnRelayoutEventArgs....");
      // Populate all members of "e" (OnRelayoutEventArgs) with real data
      e.View = View.GetViewFromPtr(data);

      if (_viewOnRelayoutEventHandler != null)
      {
        //here we send all data to user event handlers
        _viewOnRelayoutEventHandler(this, e);
      }
    }

    /**
      * @brief Event for Touched signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of TouchHandler-DaliEventHandlerWithReturnType<object,TouchEventArgs,bool>)
      * provided by the user. Touched signal is emitted when touch input is received.
      */
    public event DaliEventHandlerWithReturnType<object,TouchEventArgs,bool> Touched
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_viewTouchHandler == null)
          {
            _viewTouchHandler += value;
            Console.WriteLine("View Touch EVENT LOCKED....");
            _viewTouchCallbackDelegate = new TouchCallbackDelegate(OnTouch);
            this.TouchSignal().Connect(_viewTouchCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_viewTouchHandler != null)
          {
            this.TouchSignal().Disconnect(_viewTouchCallbackDelegate);
          }

          _viewTouchHandler -= value;
        }
      }
    }

    // Callback for View TouchSignal
    private bool OnTouch(IntPtr view, IntPtr touch)
    {
      TouchEventArgs e = new TouchEventArgs();
      Console.WriteLine("View Touch EVENT....");
      // Populate all members of "e" (TouchEventArgs) with real data
      e.View = View.GetViewFromPtr(view);
      e.Touch = Dali.Touch.GetTouchFromPtr(touch);

      if (_viewTouchHandler != null)
      {
        //here we send all data to user event handlers
        return _viewTouchHandler(this, e);
      }

      return false;
    }

    /**
      * @brief Event for Hovered signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of HoverHandler-DaliEventHandlerWithReturnType<object,HoverEventArgs,bool>)
      * provided by the user. Hovered signal is emitted when hover input is received.
      */
    public event DaliEventHandlerWithReturnType<object,HoverEventArgs,bool> Hovered
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_viewHoverHandler == null)
          {
            _viewHoverHandler += value;

            _viewHoverCallbackDelegate = new HoverCallbackDelegate(OnHover);
            this.HoveredSignal().Connect(_viewHoverCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_viewHoverHandler != null)
          {
            this.HoveredSignal().Disconnect(_viewHoverCallbackDelegate);
          }

          _viewHoverHandler -= value;
        }
      }
    }

    // Callback for View Hover signal
    private bool OnHover(IntPtr view, IntPtr hover)
    {
      HoverEventArgs e = new HoverEventArgs();

      // Populate all members of "e" (HoverEventArgs) with real data
      e.View = View.GetViewFromPtr(view);
      e.Hover = Dali.Hover.GetHoverFromPtr(hover);

      if (_viewHoverHandler != null)
      {
        //here we send all data to user event handlers
        return _viewHoverHandler(this, e);
      }

      return false;
    }

    /**
      * @brief Event for WheelMoved signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of WheelHandler-DaliEventHandlerWithReturnType<object,WheelEventArgs,bool>)
      * provided by the user. WheelMoved signal is emitted when wheel event is received.
      */
    public event DaliEventHandlerWithReturnType<object,WheelEventArgs,bool> WheelMoved
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_viewWheelHandler == null)
          {
            _viewWheelHandler += value;
            Console.WriteLine("View Wheel EVENT LOCKED....");
            _viewWheelCallbackDelegate = new WheelCallbackDelegate(OnWheel);
            this.WheelEventSignal().Connect(_viewWheelCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_viewWheelHandler != null)
          {
            this.WheelEventSignal().Disconnect(_viewWheelCallbackDelegate);
          }

          _viewWheelHandler -= value;
        }
      }
    }

    // Callback for View Wheel signal
    private bool OnWheel(IntPtr view, IntPtr wheel)
    {
      WheelEventArgs e = new WheelEventArgs();
      Console.WriteLine("View Wheel EVENT ....");
      // Populate all members of "e" (WheelEventArgs) with real data
      e.View = View.GetViewFromPtr(view);
      e.Wheel = Dali.Wheel.GetWheelFromPtr(wheel);

      if (_viewWheelHandler != null)
      {
        //here we send all data to user event handlers
        return _viewWheelHandler(this, e);
      }

      return false;
    }

    /**
      * @brief Event for OnStage signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of OnStageEventHandler) provided by the user.
      * OnStage signal is emitted after the view has been connected to the stage.
      */
    public event DaliEventHandler<object,OnStageEventArgs> OnStageEvent
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_viewOnStageEventHandler == null)
          {
            _viewOnStageEventHandler += value;

            _viewOnStageEventCallbackDelegate = new OnStageEventCallbackDelegate(OnStage);
            this.OnStageSignal().Connect(_viewOnStageEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_viewOnStageEventHandler != null)
          {
            this.OnStageSignal().Disconnect(_viewOnStageEventCallbackDelegate);
          }

          _viewOnStageEventHandler -= value;
        }
      }
    }

    // Callback for View OnStage signal
    private void OnStage(IntPtr data)
    {
      OnStageEventArgs e = new OnStageEventArgs();

      // Populate all members of "e" (OnStageEventArgs) with real data
      e.View = View.GetViewFromPtr(data);

      //Console.WriteLine("############# OnStage()! e.View.Name=" + e.View.Name);

      if (_viewOnStageEventHandler != null)
      {
        //here we send all data to user event handlers
        _viewOnStageEventHandler(this, e);
      }
    }

    /**
      * @brief Event for OffStage signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of OffStageEventHandler) provided by the user.
      * OffStage signal is emitted after the view has been disconnected from the stage.
      */
    public event DaliEventHandler<object,OffStageEventArgs> OffStageEvent
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_viewOffStageEventHandler == null)
          {
            _viewOffStageEventHandler += value;

            _viewOffStageEventCallbackDelegate = new OffStageEventCallbackDelegate(OffStage);
            this.OnStageSignal().Connect(_viewOffStageEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_viewOffStageEventHandler != null)
          {
            this.OnStageSignal().Disconnect(_viewOffStageEventCallbackDelegate);
          }

          _viewOffStageEventHandler -= value;
        }
      }
    }

    // Callback for View OffStage signal
    private void OffStage(IntPtr data)
    {
      OffStageEventArgs e = new OffStageEventArgs();

      // Populate all members of "e" (OffStageEventArgs) with real data
      e.View = View.GetViewFromPtr(data);

      if (_viewOffStageEventHandler != null)
      {
        //here we send all data to user event handlers
        _viewOffStageEventHandler(this, e);
      }
    }

    public static View GetViewFromPtr(global::System.IntPtr cPtr) {
      View ret = new View(cPtr, false);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    }

    public Dali.Property.Map Tooltip
    {
      get
      {
        Dali.Property.Map temp = new Dali.Property.Map();
        GetProperty( View.Property.TOOLTIP).Get(  temp );
        return temp;
      }
      set
      {
        SetProperty( View.Property.TOOLTIP, new Dali.Property.Value( value ) );
      }
    }

    public string TooltipText
    {
      set
      {
        SetProperty( View.Property.TOOLTIP, new Dali.Property.Value( value ) );
      }
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
