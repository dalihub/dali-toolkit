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
       private TouchData _touchData;

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
    public class HoverArgs : EventArgs
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
    private WheelEvent _wheelEvent;

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
    private delegate bool KeyCallbackDelegate(IntPtr control, IntPtr keyEvent);
    private DaliEventHandlerWithReturnType<object,KeyEventArgs,bool> _KeyEventHandler;
    private KeyCallbackDelegate _KeyCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void OnRelayoutEventCallbackDelegate(IntPtr control);
    private DaliEventHandler<object,OnRelayoutEventArgs> _viewOnRelayoutEventHandler;
    private OnRelayoutEventCallbackDelegate _viewOnRelayoutEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool TouchCallbackDelegate(IntPtr view, IntPtr touchData);
    private DaliEventHandlerWithReturnType<object,TouchEventArgs,bool> _viewTouchDataEventHandler;
    private TouchCallbackDelegate _viewTouchDataCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool HoverCallbackDelegate(IntPtr view, IntPtr hover);
    private DaliEventHandlerWithReturnType<object,HoverArgs,bool> _viewHoverHandler;
    private HoverCallbackDelegate _viewHoverCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool WheelEventCallbackDelegate(IntPtr view, IntPtr wheelEvent);
    private DaliEventHandlerWithReturnType<object,WheelEventArgs,bool> _viewWheelEventHandler;
    private WheelEventCallbackDelegate _viewWheelEventCallbackDelegate;

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
            Console.WriteLine("View Keyevent EVENT Locked....");
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
      Console.WriteLine("View Keyevent ....");
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
          if (_viewTouchDataEventHandler == null)
          {
            _viewTouchDataEventHandler += value;
            Console.WriteLine("View Touch EVENT LOCKED....");
            _viewTouchDataCallbackDelegate = new TouchCallbackDelegate(OnTouch);
            this.TouchSignal().Connect(_viewTouchDataCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_viewTouchDataEventHandler != null)
          {
            this.TouchSignal().Disconnect(_viewTouchDataCallbackDelegate);
          }

          _viewTouchDataEventHandler -= value;
        }
      }
    }

    // Callback for View TouchSignal
    private bool OnTouch(IntPtr view, IntPtr touchData)
    {
      TouchEventArgs e = new TouchEventArgs();
      Console.WriteLine("View Touch EVENT....");
      // Populate all members of "e" (TouchEventArgs) with real data
      e.View = View.GetViewFromPtr(view);
      e.TouchData = Dali.TouchData.GetTouchDataFromPtr(touchData);

      if (_viewTouchDataEventHandler != null)
      {
        //here we send all data to user event handlers
        return _viewTouchDataEventHandler(this, e);
      }

      return false;
    }

    /**
      * @brief Event for Hovered signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of HoverHandler-DaliEventHandlerWithReturnType<object,HoverArgs,bool>)
      * provided by the user. Hovered signal is emitted when hover input is received.
      */
    public event DaliEventHandlerWithReturnType<object,HoverArgs,bool> Hovered
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
      HoverArgs e = new HoverArgs();

      // Populate all members of "e" (HoverArgs) with real data
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
          if (_viewWheelEventHandler == null)
          {
            _viewWheelEventHandler += value;
            Console.WriteLine("View Wheel EVENT LOCKED....");
            _viewWheelEventCallbackDelegate = new WheelEventCallbackDelegate(OnWheelEvent);
            this.WheelEventSignal().Connect(_viewWheelEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_viewWheelEventHandler != null)
          {
            this.WheelEventSignal().Disconnect(_viewWheelEventCallbackDelegate);
          }

          _viewWheelEventHandler -= value;
        }
      }
    }

    // Callback for View Wheel signal
    private bool OnWheelEvent(IntPtr view, IntPtr wheelEvent)
    {
      WheelEventArgs e = new WheelEventArgs();
      Console.WriteLine("View Wheel EVENT ....");
      // Populate all members of "e" (WheelEventArgs) with real data
      e.View = View.GetViewFromPtr(view);
      e.WheelEvent = Dali.WheelEvent.GetWheelEventFromPtr(wheelEvent);

      if (_viewWheelEventHandler != null)
      {
        //here we send all data to user event handlers
        return _viewWheelEventHandler(this, e);
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
