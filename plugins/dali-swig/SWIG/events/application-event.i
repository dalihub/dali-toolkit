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
      * @brief Event arguments that passed via AUIApplicationInit signal
      *
      */
    public class AUIApplicationInitEventArgs : EventArgs
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
      * @brief Event arguments that passed via AUIApplicationTerminate signal
      *
      */
    public class AUIApplicationTerminateEventArgs : EventArgs
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
      * @brief Event arguments that passed via AUIApplicationPause signal
      *
      */
    public class AUIApplicationPauseEventArgs : EventArgs
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
      * @brief Event arguments that passed via AUIApplicationResume signal
      *
      */
    public class AUIApplicationResumeEventArgs : EventArgs
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
      * @brief Event arguments that passed via AUIApplicationReset signal
      *
      */
    public class AUIApplicationResetEventArgs : EventArgs
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
      * @brief Event arguments that passed via AUIApplicationResize signal
      *
      */
    public class AUIApplicationResizeEventArgs : EventArgs
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
      * @brief Event arguments that passed via AUIApplicationLanguageChanged signal
      *
      */
    public class AUIApplicationLanguageChangedEventArgs : EventArgs
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
      * @brief Event arguments that passed via AUIApplicationRegionChanged signal
      *
      */
    public class AUIApplicationRegionChangedEventArgs : EventArgs
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
      * @brief Event arguments that passed via AUIApplicationBatteryLow signal
      *
      */
    public class AUIApplicationBatteryLowEventArgs : EventArgs
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
      * @brief Event arguments that passed via AUIApplicationMemoryLow signal
      *
      */
    public class AUIApplicationMemoryLowEventArgs : EventArgs
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
      * @brief Event arguments that passed via AUIApplicationAppControl	 signal
      *
      */
    public class AUIApplicationAppControlEventArgs : EventArgs
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
    private delegate void AUIApplicationInitEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,AUIApplicationInitEventArgs> _applicationInitEventHandler;
    private AUIApplicationInitEventCallbackDelegate _applicationInitEventCallbackDelegate;


    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void AUIApplicationTerminateEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,AUIApplicationTerminateEventArgs> _applicationTerminateEventHandler;
    private AUIApplicationTerminateEventCallbackDelegate _applicationTerminateEventCallbackDelegate;


    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void AUIApplicationPauseEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,AUIApplicationPauseEventArgs> _applicationPauseEventHandler;
    private AUIApplicationPauseEventCallbackDelegate _applicationPauseEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void AUIApplicationResumeEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,AUIApplicationResumeEventArgs> _applicationResumeEventHandler;
    private AUIApplicationResumeEventCallbackDelegate _applicationResumeEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void AUIApplicationResetEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,AUIApplicationResetEventArgs> _applicationResetEventHandler;
    private AUIApplicationResetEventCallbackDelegate _applicationResetEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void AUIApplicationResizeEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,AUIApplicationResizeEventArgs> _applicationResizeEventHandler;
    private AUIApplicationResizeEventCallbackDelegate _applicationResizeEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void AUIApplicationLanguageChangedEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,AUIApplicationLanguageChangedEventArgs> _applicationLanguageChangedEventHandler;
    private AUIApplicationLanguageChangedEventCallbackDelegate _applicationLanguageChangedEventCallbackDelegate;


    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void AUIApplicationRegionChangedEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,AUIApplicationRegionChangedEventArgs> _applicationRegionChangedEventHandler;
    private AUIApplicationRegionChangedEventCallbackDelegate _applicationRegionChangedEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void AUIApplicationBatteryLowEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,AUIApplicationBatteryLowEventArgs> _applicationBatteryLowEventHandler;
    private AUIApplicationBatteryLowEventCallbackDelegate _applicationBatteryLowEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void AUIApplicationMemoryLowEventCallbackDelegate(IntPtr application);
    private DaliEventHandler<object,AUIApplicationMemoryLowEventArgs> _applicationMemoryLowEventHandler;
    private AUIApplicationMemoryLowEventCallbackDelegate _applicationMemoryLowEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void AUIApplicationAppControlEventCallbackDelegate(IntPtr application, IntPtr voidp);
    private DaliEventHandler<object,AUIApplicationAppControlEventArgs> _applicationAppControlEventHandler;
    private AUIApplicationAppControlEventCallbackDelegate _applicationAppControlEventCallbackDelegate;

    /**
      * @brief Event for Initialized signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of AUIApplicationInitEventHandler - DaliEventHandler<object,AUIApplicationInitEventArgs>)
      *  provided by the user. Initialized signal is emitted when application is initialised
      */
    public event DaliEventHandler<object,AUIApplicationInitEventArgs> Initialized
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationInitEventHandler == null)
          {
            _applicationInitEventHandler += value;

            _applicationInitEventCallbackDelegate = new AUIApplicationInitEventCallbackDelegate(OnApplicationInit);
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
      AUIApplicationInitEventArgs e = new AUIApplicationInitEventArgs();

      // Populate all members of "e" (AUIApplicationInitEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationInitEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationInitEventHandler(this, e);
      }
    }

    /**
      * @brief Event for Terminated signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of AUIApplicationTerminateEventHandler-DaliEventHandler<object,AUIApplicationTerminateEventArgs>)
      *  provided by the user. Terminated signal is emitted when application is terminated
      */
    public event DaliEventHandler<object,AUIApplicationTerminateEventArgs> Terminated
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationTerminateEventHandler == null)
          {
            _applicationTerminateEventHandler += value;

            _applicationTerminateEventCallbackDelegate = new AUIApplicationTerminateEventCallbackDelegate(OnAUIApplicationTerminate);
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
    private void OnAUIApplicationTerminate(IntPtr data)
    {
      AUIApplicationTerminateEventArgs e = new AUIApplicationTerminateEventArgs();

      // Populate all members of "e" (AUIApplicationTerminateEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationTerminateEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationTerminateEventHandler(this, e);
      }
    }

    /**
      * @brief Event for Paused signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of AUIApplicationPauseEventHandler-DaliEventHandler<object,AUIApplicationPauseEventArgs>)
      * provided by the user. Paused signal is emitted when application is paused
      */
    public event DaliEventHandler<object,AUIApplicationPauseEventArgs> Paused
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationPauseEventHandler == null)
          {
            _applicationPauseEventHandler += value;

            _applicationPauseEventCallbackDelegate = new AUIApplicationPauseEventCallbackDelegate(OnAUIApplicationPause);
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
    private void OnAUIApplicationPause(IntPtr data)
    {
      AUIApplicationPauseEventArgs e = new AUIApplicationPauseEventArgs();

      // Populate all members of "e" (AUIApplicationPauseEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationPauseEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationPauseEventHandler(this, e);
      }
    }

    /**
      * @brief Event for Resumed signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of AUIApplicationResumeEventHandler-DaliEventHandler<object,AUIApplicationResumeEventArgs>)
      *  provided by the user. Resumed signal is emitted when application is resumed
      */
    public event DaliEventHandler<object,AUIApplicationResumeEventArgs> Resumed
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationResumeEventHandler == null)
          {
            _applicationResumeEventHandler += value;

            _applicationResumeEventCallbackDelegate = new AUIApplicationResumeEventCallbackDelegate(OnAUIApplicationResume);
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
    private void OnAUIApplicationResume(IntPtr data)
    {
      AUIApplicationResumeEventArgs e = new AUIApplicationResumeEventArgs();

      // Populate all members of "e" (AUIApplicationResumeEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationResumeEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationResumeEventHandler(this, e);
      }
    }

    /**
      * @brief Event for Reset signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of AUIApplicationResetEventHandler-DaliEventHandler<object,AUIApplicationResetEventArgs>)
      *  provided by the user. Reset signal is emitted when application is reset
      */
    public event DaliEventHandler<object,AUIApplicationResetEventArgs> Reset
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationResetEventHandler == null)
          {
            _applicationResetEventHandler += value;

            _applicationResetEventCallbackDelegate = new AUIApplicationResetEventCallbackDelegate(OnAUIApplicationReset);
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
    private void OnAUIApplicationReset(IntPtr data)
    {
      AUIApplicationResetEventArgs e = new AUIApplicationResetEventArgs();

      // Populate all members of "e" (AUIApplicationResetEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationResetEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationResetEventHandler(this, e);
      }
    }

    /**
      * @brief Event for Resized signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of AUIApplicationResizeEventHandler-DaliEventHandler<object,AUIApplicationResizeEventArgs>)
      *  provided by the user. Resized signal is emitted when application is resized
      */
    public event DaliEventHandler<object,AUIApplicationResizeEventArgs> Resized
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationResizeEventHandler == null)
          {
            _applicationResizeEventHandler += value;

            _applicationResizeEventCallbackDelegate = new AUIApplicationResizeEventCallbackDelegate(OnAUIApplicationResize);
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
    private void OnAUIApplicationResize(IntPtr data)
    {
      AUIApplicationResizeEventArgs e = new AUIApplicationResizeEventArgs();

      // Populate all members of "e" (AUIApplicationResizeEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationResizeEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationResizeEventHandler(this, e);
      }
    }

    /**
      * @brief Event for LanguageChanged signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of AUIApplicationLanguageChangedEventHandler-DaliEventHandler<object,AUIApplicationLanguageChangedEventArgs>)
      *  provided by the user. LanguageChanged signal is emitted when the region of the device is changed.
      */
    public event DaliEventHandler<object,AUIApplicationLanguageChangedEventArgs> LanguageChanged
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationLanguageChangedEventHandler == null)
          {
            _applicationLanguageChangedEventHandler += value;

            _applicationLanguageChangedEventCallbackDelegate = new AUIApplicationLanguageChangedEventCallbackDelegate(OnAUIApplicationLanguageChanged);
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
    private void OnAUIApplicationLanguageChanged(IntPtr data)
    {
      AUIApplicationLanguageChangedEventArgs e = new AUIApplicationLanguageChangedEventArgs();

      // Populate all members of "e" (AUIApplicationLanguageChangedEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationLanguageChangedEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationLanguageChangedEventHandler(this, e);
      }
    }

    /**
      * @brief Event for RegionChanged signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of AUIApplicationRegionChangedEventHandler-DaliEventHandler<object,AUIApplicationRegionChangedEventArgs>)
      *  provided by the user. RegionChanged signal is emitted when the region of the device is changed.
      */
    public event DaliEventHandler<object,AUIApplicationRegionChangedEventArgs> RegionChanged
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationRegionChangedEventHandler == null)
          {
            _applicationRegionChangedEventHandler += value;

            _applicationRegionChangedEventCallbackDelegate = new AUIApplicationRegionChangedEventCallbackDelegate(OnAUIApplicationRegionChanged);
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
    private void OnAUIApplicationRegionChanged(IntPtr data)
    {
      AUIApplicationRegionChangedEventArgs e = new AUIApplicationRegionChangedEventArgs();

      // Populate all members of "e" (AUIApplicationRegionChangedEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationRegionChangedEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationRegionChangedEventHandler(this, e);
      }
    }

    /**
      * @brief Event for BatteryLow signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of AUIApplicationBatteryLowEventHandler-DaliEventHandler<object,AUIApplicationBatteryLowEventArgs>) 
      * provided by the user. BatteryLow signal is emitted when the battery level of the device is low.
      */
    public event DaliEventHandler<object,AUIApplicationBatteryLowEventArgs> BatteryLow
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationBatteryLowEventHandler == null)
          {
            _applicationBatteryLowEventHandler += value;

            _applicationBatteryLowEventCallbackDelegate = new AUIApplicationBatteryLowEventCallbackDelegate(OnAUIApplicationBatteryLow);
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
    private void OnAUIApplicationBatteryLow(IntPtr data)
    {
      AUIApplicationBatteryLowEventArgs e = new AUIApplicationBatteryLowEventArgs();

      // Populate all members of "e" (AUIApplicationBatteryLowEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationBatteryLowEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationBatteryLowEventHandler(this, e);
      }
    }

    /**
      * @brief Event for MemoryLow signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of AUIApplicationMemoryLowEventHandler-DaliEventHandler<object,AUIApplicationMemoryLowEventArgs>)
      *  provided by the user. MemoryLow signal is emitted when the memory level of the device is low.
      */
    public event DaliEventHandler<object,AUIApplicationMemoryLowEventArgs> MemoryLow
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationMemoryLowEventHandler == null)
          {
            _applicationMemoryLowEventHandler += value;

            _applicationMemoryLowEventCallbackDelegate = new AUIApplicationMemoryLowEventCallbackDelegate(OnAUIApplicationMemoryLow);
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
    private void OnAUIApplicationMemoryLow(IntPtr data)
    {
      AUIApplicationMemoryLowEventArgs e = new AUIApplicationMemoryLowEventArgs();

      // Populate all members of "e" (AUIApplicationMemoryLowEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(data);

      if (_applicationMemoryLowEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationMemoryLowEventHandler(this, e);
      }
    }

    /**
      * @brief Event for AppControl signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of AUIApplicationAppControlEventHandler-DaliEventHandler<object,AUIApplicationAppControlEventArgs>)
      *  provided by the user. AppControl signal is emitted when another application sends a launch request to the application.
      */
    public event DaliEventHandler<object,AUIApplicationAppControlEventArgs> AppControl
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_applicationAppControlEventHandler == null)
          {
            _applicationAppControlEventHandler += value;

            _applicationAppControlEventCallbackDelegate = new AUIApplicationAppControlEventCallbackDelegate(OnAUIApplicationAppControl);
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
    private void OnAUIApplicationAppControl(IntPtr application, IntPtr voidp)
    {
      AUIApplicationAppControlEventArgs e = new AUIApplicationAppControlEventArgs();

      // Populate all members of "e" (AUIApplicationAppControlEventArgs) with real data
      e.Application = Application.GetApplicationFromPtr(application);
      e.VoidP = voidp;

      if (_applicationAppControlEventHandler != null)
      {
        //here we send all data to user event handlers
        _applicationAppControlEventHandler(this, e);
      }
    }

    private static  Application instance; // singleton

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
      return NewApplication("",Application.WINDOW_MODE.OPAQUE);
    }

    public static Application NewApplication(string stylesheet) {
      return NewApplication(stylesheet, Application.WINDOW_MODE.OPAQUE);
    }

    public static Application NewApplication(string stylesheet, Application.WINDOW_MODE windowMode) {

      Application ret = New(1, stylesheet, windowMode);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();

      // we've got an application now connect the signals
      ret.SetupDelegates();
      // set the singleton

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


