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

%define APPLICATION_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
  %typemap(csimports) NameSpace::ClassName %{
    using System;
    using System.Runtime.InteropServices;

    /**
      * @brief Event arguments that passed via NUIApplicationInit signal
      *
      */
    public class NUIApplicationInitEventArgs : EventArgs
    {
    private Application _application;

      /**
        * @brief Application - is the application that is being initialized
        *
        */
    public Application Application
      {
        get
        {
          return _application;
        }
        set
        {
          _application = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via NUIApplicationTerminate signal
      *
      */
    public class NUIApplicationTerminateEventArgs : EventArgs
    {
    private Application _application;
      /**
        * @brief Application - is the application that is being Terminated
        *
        */
    public Application Application
      {
        get
        {
          return _application;
        }
        set
        {
          _application = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via NUIApplicationPause signal
      *
      */
    public class NUIApplicationPauseEventArgs : EventArgs
    {
    private Application _application;
      /**
        * @brief Application - is the application that is being Paused
        *
        */
    public Application Application
      {
        get
        {
          return _application;
        }
        set
        {
          _application = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via NUIApplicationResume signal
      *
      */
    public class NUIApplicationResumeEventArgs : EventArgs
    {
    private Application _application;
      /**
        * @brief Application - is the application that is being Resumed
        *
        */
    public Application Application
      {
        get
        {
          return _application;
        }
        set
        {
          _application = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via NUIApplicationReset signal
      *
      */
    public class NUIApplicationResetEventArgs : EventArgs
    {
    private Application _application;
      /**
        * @brief Application - is the application that is being Reset
        *
        */
    public Application Application
      {
        get
        {
          return _application;
        }
        set
        {
          _application = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via NUIApplicationResize signal
      *
      */
    public class NUIApplicationResizeEventArgs : EventArgs
    {
    private Application _application;
      /**
        * @brief Application - is the application that is being Resized
        *
        */
    public Application Application
      {
        get
        {
          return _application;
        }
        set
        {
          _application = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via NUIApplicationLanguageChanged signal
      *
      */
    public class NUIApplicationLanguageChangedEventArgs : EventArgs
    {
    private Application _application;
      /**
        * @brief Application - is the application that is being affected with Device's language change
        *
        */
    public Application Application
      {
        get
        {
          return _application;
        }
        set
        {
          _application = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via NUIApplicationRegionChanged signal
      *
      */
    public class NUIApplicationRegionChangedEventArgs : EventArgs
    {
    private Application _application;
      /**
        * @brief Application - is the application that is being affected with Device's region change
        *
        */
    public Application Application
      {
        get
        {
          return _application;
        }
        set
        {
          _application = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via NUIApplicationBatteryLow signal
      *
      */
    public class NUIApplicationBatteryLowEventArgs : EventArgs
    {
    private Application _application;
      /**
        * @brief Application - is the application that is being affected when the battery level of the device is low
        *
        */
    public Application Application
      {
        get
        {
          return _application;
        }
        set
        {
          _application = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via NUIApplicationMemoryLow signal
      *
      */
    public class NUIApplicationMemoryLowEventArgs : EventArgs
    {
    private Application _application;
      /**
        * @brief Application - is the application that is being affected when the memory level of the device is low
        *
        */
    public Application Application
      {
        get
        {
          return _application;
        }
        set
        {
          _application = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via NUIApplicationAppControl	 signal
      *
      */
    public class NUIApplicationAppControlEventArgs : EventArgs
    {
    private Application _application;
    private IntPtr _voidp;
      /**
        * @brief Application - is the application that is receiving the launch request from another application
        *
        */
    public Application Application
      {
        get
        {
          return _application;
        }
        set
        {
          _application = value;
        }
      }
      /**
        * @brief VoidP - contains the information about why the application is launched
        *
        */
    public IntPtr VoidP
      {
        get
        {
          return _voidp;
        }
        set
        {
          _voidp = value;
        }
      }
    }
%}

%enddef


%define APPLICATION_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
  %typemap(cscode) NameSpace::ClassName %{
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void NUIApplicationInitEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,NUIApplicationInitEventArgs> _applicationInitEventHandler;
    private NUIApplicationInitEventCallbackDelegate _applicationInitEventCallbackDelegate;


    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void NUIApplicationTerminateEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,NUIApplicationTerminateEventArgs> _applicationTerminateEventHandler;
    private NUIApplicationTerminateEventCallbackDelegate _applicationTerminateEventCallbackDelegate;


    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void NUIApplicationPauseEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,NUIApplicationPauseEventArgs> _applicationPauseEventHandler;
    private NUIApplicationPauseEventCallbackDelegate _applicationPauseEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void NUIApplicationResumeEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,NUIApplicationResumeEventArgs> _applicationResumeEventHandler;
    private NUIApplicationResumeEventCallbackDelegate _applicationResumeEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void NUIApplicationResetEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,NUIApplicationResetEventArgs> _applicationResetEventHandler;
    private NUIApplicationResetEventCallbackDelegate _applicationResetEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void NUIApplicationResizeEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,NUIApplicationResizeEventArgs> _applicationResizeEventHandler;
    private NUIApplicationResizeEventCallbackDelegate _applicationResizeEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void NUIApplicationLanguageChangedEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,NUIApplicationLanguageChangedEventArgs> _applicationLanguageChangedEventHandler;
    private NUIApplicationLanguageChangedEventCallbackDelegate _applicationLanguageChangedEventCallbackDelegate;


    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void NUIApplicationRegionChangedEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,NUIApplicationRegionChangedEventArgs> _applicationRegionChangedEventHandler;
    private NUIApplicationRegionChangedEventCallbackDelegate _applicationRegionChangedEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void NUIApplicationBatteryLowEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,NUIApplicationBatteryLowEventArgs> _applicationBatteryLowEventHandler;
    private NUIApplicationBatteryLowEventCallbackDelegate _applicationBatteryLowEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void NUIApplicationMemoryLowEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,NUIApplicationMemoryLowEventArgs> _applicationMemoryLowEventHandler;
    private NUIApplicationMemoryLowEventCallbackDelegate _applicationMemoryLowEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void NUIApplicationAppControlEventCallbackDelegate(IntPtr application, IntPtr voidp);
    private DaliEventHandler<object,NUIApplicationAppControlEventArgs> _applicationAppControlEventHandler;
    private NUIApplicationAppControlEventCallbackDelegate _applicationAppControlEventCallbackDelegate;

    /**
      * @brief Event for Initialized signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of NUIApplicationInitEventHandler - DaliEventHandler<object,NUIApplicationInitEventArgs>)
      *  provided by the user. Initialized signal is emitted when application is initialised
      */
    public event DaliEventHandler<object,NUIApplicationInitEventArgs> Initialized
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationInitEventHandler == null)
          {
            _applicationInitEventHandler += value;

            _applicationInitEventCallbackDelegate = new NUIApplicationInitEventCallbackDelegate(OnApplicationInit);
            this.InitSignal().Connect(_applicationInitEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_applicationInitEventHandler != null)
          {
            this.InitSignal().Disconnect(_applicationInitEventCallbackDelegate);
          }

          _applicationInitEventHandler -= value;
        }
      }
    }

    // Callback for Application InitSignal
    private void OnApplicationInit(IntPtr data)
    {
      NUIApplicationInitEventArgs e = new NUIApplicationInitEventArgs();

      // Populate all members of "e" (NUIApplicationInitEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationInitEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationInitEventHandler(this, e);
      }
    }

    /**
      * @brief Event for Terminated signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of NUIApplicationTerminateEventHandler-DaliEventHandler<object,NUIApplicationTerminateEventArgs>)
      *  provided by the user. Terminated signal is emitted when application is terminated
      */
    public event DaliEventHandler<object,NUIApplicationTerminateEventArgs> Terminated
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationTerminateEventHandler == null)
          {
            _applicationTerminateEventHandler += value;

            _applicationTerminateEventCallbackDelegate = new NUIApplicationTerminateEventCallbackDelegate(OnNUIApplicationTerminate);
            this.TerminateSignal().Connect(_applicationTerminateEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_applicationTerminateEventHandler != null)
          {
            this.TerminateSignal().Disconnect(_applicationTerminateEventCallbackDelegate);
          }

          _applicationTerminateEventHandler -= value;
        }
      }
    }

    // Callback for Application TerminateSignal
    private void OnNUIApplicationTerminate(IntPtr data)
    {
      NUIApplicationTerminateEventArgs e = new NUIApplicationTerminateEventArgs();

      // Populate all members of "e" (NUIApplicationTerminateEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationTerminateEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationTerminateEventHandler(this, e);
      }
    }

    /**
      * @brief Event for Paused signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of NUIApplicationPauseEventHandler-DaliEventHandler<object,NUIApplicationPauseEventArgs>)
      * provided by the user. Paused signal is emitted when application is paused
      */
    public event DaliEventHandler<object,NUIApplicationPauseEventArgs> Paused
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationPauseEventHandler == null)
          {
            _applicationPauseEventHandler += value;

            _applicationPauseEventCallbackDelegate = new NUIApplicationPauseEventCallbackDelegate(OnNUIApplicationPause);
            this.PauseSignal().Connect(_applicationPauseEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_applicationPauseEventHandler != null)
          {
            this.PauseSignal().Disconnect(_applicationPauseEventCallbackDelegate);
          }

          _applicationPauseEventHandler -= value;
        }
      }
    }

    // Callback for Application PauseSignal
    private void OnNUIApplicationPause(IntPtr data)
    {
      NUIApplicationPauseEventArgs e = new NUIApplicationPauseEventArgs();

      // Populate all members of "e" (NUIApplicationPauseEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationPauseEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationPauseEventHandler(this, e);
      }
    }

    /**
      * @brief Event for Resumed signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of NUIApplicationResumeEventHandler-DaliEventHandler<object,NUIApplicationResumeEventArgs>)
      *  provided by the user. Resumed signal is emitted when application is resumed
      */
    public event DaliEventHandler<object,NUIApplicationResumeEventArgs> Resumed
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationResumeEventHandler == null)
          {
            _applicationResumeEventHandler += value;

            _applicationResumeEventCallbackDelegate = new NUIApplicationResumeEventCallbackDelegate(OnNUIApplicationResume);
            this.ResumeSignal().Connect(_applicationResumeEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_applicationResumeEventHandler != null)
          {
            this.ResumeSignal().Disconnect(_applicationResumeEventCallbackDelegate);
          }

          _applicationResumeEventHandler -= value;
        }
      }
    }

    // Callback for Application ResumeSignal
    private void OnNUIApplicationResume(IntPtr data)
    {
      NUIApplicationResumeEventArgs e = new NUIApplicationResumeEventArgs();

      // Populate all members of "e" (NUIApplicationResumeEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationResumeEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationResumeEventHandler(this, e);
      }
    }

    /**
      * @brief Event for Reset signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of NUIApplicationResetEventHandler-DaliEventHandler<object,NUIApplicationResetEventArgs>)
      *  provided by the user. Reset signal is emitted when application is reset
      */
    public event DaliEventHandler<object,NUIApplicationResetEventArgs> Reset
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationResetEventHandler == null)
          {
            _applicationResetEventHandler += value;

            _applicationResetEventCallbackDelegate = new NUIApplicationResetEventCallbackDelegate(OnNUIApplicationReset);
            this.ResetSignal().Connect(_applicationResetEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_applicationResetEventHandler != null)
          {
            this.ResetSignal().Disconnect(_applicationResetEventCallbackDelegate);
          }

          _applicationResetEventHandler -= value;
        }
      }
    }

    // Callback for Application ResetSignal
    private void OnNUIApplicationReset(IntPtr data)
    {
      NUIApplicationResetEventArgs e = new NUIApplicationResetEventArgs();

      // Populate all members of "e" (NUIApplicationResetEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationResetEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationResetEventHandler(this, e);
      }
    }

    /**
      * @brief Event for Resized signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of NUIApplicationResizeEventHandler-DaliEventHandler<object,NUIApplicationResizeEventArgs>)
      *  provided by the user. Resized signal is emitted when application is resized
      */
    public event DaliEventHandler<object,NUIApplicationResizeEventArgs> Resized
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationResizeEventHandler == null)
          {
            _applicationResizeEventHandler += value;

            _applicationResizeEventCallbackDelegate = new NUIApplicationResizeEventCallbackDelegate(OnNUIApplicationResize);
            this.ResizeSignal().Connect(_applicationResizeEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_applicationResizeEventHandler != null)
          {
            this.ResizeSignal().Disconnect(_applicationResizeEventCallbackDelegate);
          }

          _applicationResizeEventHandler -= value;
        }
      }
    }

    // Callback for Application ResizeSignal
    private void OnNUIApplicationResize(IntPtr data)
    {
      NUIApplicationResizeEventArgs e = new NUIApplicationResizeEventArgs();

      // Populate all members of "e" (NUIApplicationResizeEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationResizeEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationResizeEventHandler(this, e);
      }
    }

    /**
      * @brief Event for LanguageChanged signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of NUIApplicationLanguageChangedEventHandler-DaliEventHandler<object,NUIApplicationLanguageChangedEventArgs>)
      *  provided by the user. LanguageChanged signal is emitted when the region of the device is changed.
      */
    public event DaliEventHandler<object,NUIApplicationLanguageChangedEventArgs> LanguageChanged
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationLanguageChangedEventHandler == null)
          {
            _applicationLanguageChangedEventHandler += value;

            _applicationLanguageChangedEventCallbackDelegate = new NUIApplicationLanguageChangedEventCallbackDelegate(OnNUIApplicationLanguageChanged);
            this.LanguageChangedSignal().Connect(_applicationLanguageChangedEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_applicationLanguageChangedEventHandler != null)
          {
            this.LanguageChangedSignal().Disconnect(_applicationLanguageChangedEventCallbackDelegate);
          }

          _applicationLanguageChangedEventHandler -= value;
        }
      }
    }

    // Callback for Application LanguageChangedSignal
    private void OnNUIApplicationLanguageChanged(IntPtr data)
    {
      NUIApplicationLanguageChangedEventArgs e = new NUIApplicationLanguageChangedEventArgs();

      // Populate all members of "e" (NUIApplicationLanguageChangedEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationLanguageChangedEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationLanguageChangedEventHandler(this, e);
      }
    }

    /**
      * @brief Event for RegionChanged signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of NUIApplicationRegionChangedEventHandler-DaliEventHandler<object,NUIApplicationRegionChangedEventArgs>)
      *  provided by the user. RegionChanged signal is emitted when the region of the device is changed.
      */
    public event DaliEventHandler<object,NUIApplicationRegionChangedEventArgs> RegionChanged
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationRegionChangedEventHandler == null)
          {
            _applicationRegionChangedEventHandler += value;

            _applicationRegionChangedEventCallbackDelegate = new NUIApplicationRegionChangedEventCallbackDelegate(OnNUIApplicationRegionChanged);
            this.RegionChangedSignal().Connect(_applicationRegionChangedEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_applicationRegionChangedEventHandler != null)
          {
            this.RegionChangedSignal().Disconnect(_applicationRegionChangedEventCallbackDelegate);
          }

          _applicationRegionChangedEventHandler -= value;
        }
      }
    }

    // Callback for Application RegionChangedSignal
    private void OnNUIApplicationRegionChanged(IntPtr data)
    {
      NUIApplicationRegionChangedEventArgs e = new NUIApplicationRegionChangedEventArgs();

      // Populate all members of "e" (NUIApplicationRegionChangedEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationRegionChangedEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationRegionChangedEventHandler(this, e);
      }
    }

    /**
      * @brief Event for BatteryLow signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of NUIApplicationBatteryLowEventHandler-DaliEventHandler<object,NUIApplicationBatteryLowEventArgs>) 
      * provided by the user. BatteryLow signal is emitted when the battery level of the device is low.
      */
    public event DaliEventHandler<object,NUIApplicationBatteryLowEventArgs> BatteryLow
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationBatteryLowEventHandler == null)
          {
            _applicationBatteryLowEventHandler += value;

            _applicationBatteryLowEventCallbackDelegate = new NUIApplicationBatteryLowEventCallbackDelegate(OnNUIApplicationBatteryLow);
            this.BatteryLowSignal().Connect(_applicationBatteryLowEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_applicationBatteryLowEventHandler != null)
          {
            this.BatteryLowSignal().Disconnect(_applicationBatteryLowEventCallbackDelegate);
          }

          _applicationBatteryLowEventHandler -= value;
        }
      }
    }

    // Callback for Application BatteryLowSignal
    private void OnNUIApplicationBatteryLow(IntPtr data)
    {
      NUIApplicationBatteryLowEventArgs e = new NUIApplicationBatteryLowEventArgs();

      // Populate all members of "e" (NUIApplicationBatteryLowEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationBatteryLowEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationBatteryLowEventHandler(this, e);
      }
    }

    /**
      * @brief Event for MemoryLow signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of NUIApplicationMemoryLowEventHandler-DaliEventHandler<object,NUIApplicationMemoryLowEventArgs>)
      *  provided by the user. MemoryLow signal is emitted when the memory level of the device is low.
      */
    public event DaliEventHandler<object,NUIApplicationMemoryLowEventArgs> MemoryLow
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationMemoryLowEventHandler == null)
          {
            _applicationMemoryLowEventHandler += value;

            _applicationMemoryLowEventCallbackDelegate = new NUIApplicationMemoryLowEventCallbackDelegate(OnNUIApplicationMemoryLow);
            this.MemoryLowSignal().Connect(_applicationMemoryLowEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_applicationMemoryLowEventHandler != null)
          {
            this.MemoryLowSignal().Disconnect(_applicationMemoryLowEventCallbackDelegate);
          }

          _applicationMemoryLowEventHandler -= value;
        }
      }
    }

    // Callback for Application MemoryLowSignal
    private void OnNUIApplicationMemoryLow(IntPtr data)
    {
      NUIApplicationMemoryLowEventArgs e = new NUIApplicationMemoryLowEventArgs();

      // Populate all members of "e" (NUIApplicationMemoryLowEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationMemoryLowEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationMemoryLowEventHandler(this, e);
      }
    }

    /**
      * @brief Event for AppControl signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of NUIApplicationAppControlEventHandler-DaliEventHandler<object,NUIApplicationAppControlEventArgs>)
      *  provided by the user. AppControl signal is emitted when another application sends a launch request to the application.
      */
    public event DaliEventHandler<object,NUIApplicationAppControlEventArgs> AppControl
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationAppControlEventHandler == null)
          {
            _applicationAppControlEventHandler += value;

            _applicationAppControlEventCallbackDelegate = new NUIApplicationAppControlEventCallbackDelegate(OnNUIApplicationAppControl);
            this.AppControlSignal().Connect(_applicationAppControlEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_applicationAppControlEventHandler != null)
          {
            this.AppControlSignal().Disconnect(_applicationAppControlEventCallbackDelegate);
          }

          _applicationAppControlEventHandler -= value;
        }
      }
    }

    // Callback for Application AppControlSignal
    private void OnNUIApplicationAppControl(IntPtr application, IntPtr voidp)
    {
      NUIApplicationAppControlEventArgs e = new NUIApplicationAppControlEventArgs();

      // Populate all members of "e" (NUIApplicationAppControlEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(application);
      e.VoidP = voidp;

      if (_applicationAppControlEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationAppControlEventHandler(this, e);
      }
    }

    private static Application _instance; // singleton

    public delegate void InitDelegate();

    public delegate void TerminateDelegate();

    public delegate void PauseDelegate();

    public delegate void ResumeDelegate();

    public delegate void ResizeDelegate();

    public delegate void AppControlDelegate();

    public delegate void LanguageChangedDelegate();

    public delegate void RegionChangedDelegate();

    public delegate void BatteryLowDelegate();

    public delegate void MemoryLowDelegate();

    [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.StdCall)]
    internal delegate void InitDelegateInternal();

    [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.StdCall)]
    internal delegate void TerminateDelegateInternal();

    [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.StdCall)]
    internal delegate void PauseDelegateInternal();

    [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.StdCall)]
    internal delegate void ResumeDelegateInternal();

    [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.StdCall)]
    internal delegate void ResizeDelegateInternal();

    [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.StdCall)]
    internal delegate void AppControlDelegateInternal();

    [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.StdCall)]
    internal delegate void LanguageChangedDelegateInternal();

    [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.StdCall)]
    internal delegate void RegionChangedDelegateInternal();

    [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.StdCall)]
    internal delegate void BatteryLowDelegateInternal();

    [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.StdCall)]
    internal delegate void MemoryLowDelegateInternal();

    static void Initialize()
    {
      //	instance.InitDelegate();
    }

    public static Application Instance
    {
       get
       {
            return _instance;
       }
    }

    public static Application GetApplicationFromPtr(global::System.IntPtr cPtr) {
      Application ret = new Application(cPtr, false);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    }

    internal void SetupDelegates() {
      InitDelegateInternal initializeCallback = new InitDelegateInternal( Initialize );
      System.Console.WriteLine( "InitSignal connection count");

      this.InitSignal().Connect( initializeCallback );
      //Console.WriteLine( "InitSignal connection count = " + app.InitSignal().GetConnectionCount() );
    }

    public static Application NewApplication() {
     _instance = NewApplication("", Application.WINDOW_MODE.OPAQUE);
     return _instance;
    }

    public static Application NewApplication(string stylesheet) {
      _instance = NewApplication(stylesheet, Application.WINDOW_MODE.OPAQUE);
      return _instance;
    }

    public static Application NewApplication(string stylesheet, Application.WINDOW_MODE windowMode) {

      // register all Views with the type registry, so that can be created / styled via JSON
      ViewRegistryHelper.Initialize();

      Application ret = New(1, stylesheet, windowMode);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();

      // we've got an application now connect the signals
      ret.SetupDelegates();
      // set the singleton
      _instance = ret;
      return ret;
    }

    public bool AddIdle(System.Delegate func) {
      System.IntPtr ip = System.Runtime.InteropServices.Marshal.GetFunctionPointerForDelegate(func);
      System.IntPtr ip2 = NDalicManualPINVOKE.MakeCallback(new System.Runtime.InteropServices.HandleRef(this, ip));

      bool ret = NDalicPINVOKE.Application_AddIdle(swigCPtr, new System.Runtime.InteropServices.HandleRef(this, ip2));

      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    }

    %}
    %enddef

%define DALI_APPLICATION_EVENTHANDLER_PARAM( NameSpace, ClassName)
  APPLICATION_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  APPLICATION_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);
  %enddef

  namespace Dali
{
  DALI_APPLICATION_EVENTHANDLER_PARAM( Dali, Application);
}


