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

%define ACCESSIBILITY_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
  %typemap(csimports) NameSpace::ClassName %{
    using System;
    using System.Runtime.InteropServices;
%}

%enddef


%define ACCESSIBILITY_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
  %typemap(cscode) NameSpace::ClassName %{


    /**
      * @brief Event arguments that passed via StatusChanged signal
      *
      */
    public class StatusChangedEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionNext signal
      *
      */
    public class ActionNextEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionPrevious signal
      *
      */
    public class ActionPreviousEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionActivate signal
      *
      */
    public class ActionActivateEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionRead signal
      *
      */
    public class ActionReadEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionOver signal
      *
      */
    public class ActionOverEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionReadNext signal
      *
      */
    public class ActionReadNextEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionReadPrevious signal
      *
      */
    public class ActionReadPreviousEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionUp signal
      *
      */
    public class ActionUpEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionDown signal
      *
      */
    public class ActionDownEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionClearFocus signal
      *
      */
    public class ActionClearFocusEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionBack signal
      *
      */
    public class ActionBackEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionScrollUp signal
      *
      */
    public class ActionScrollUpEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionScrollDown signal
      *
      */
    public class ActionScrollDownEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionPageLeft signal
      *
      */
    public class ActionPageLeftEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionPageRight signal
      *
      */
    public class ActionPageRightEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionPageUp signal
      *
      */
    public class ActionPageUpEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionPageDown signal
      *
      */
    public class ActionPageDownEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionMoveToFirst signal
      *
      */
    public class ActionMoveToFirstEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionMoveToLast signal
      *
      */
    public class ActionMoveToLastEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionReadFromTop signal
      *
      */
    public class ActionReadFromTopEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionReadFromNext signal
      *
      */
    public class ActionReadFromNextEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionZoom signal
      *
      */
    public class ActionZoomEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionReadIndicatorInformation signal
      *
      */
    public class ActionReadIndicatorInformationEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionReadPauseResume signal
      *
      */
    public class ActionReadPauseResumeEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionStartStop signal
      *
      */
    public class ActionStartStopEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionScroll signal
      *
      */
    public class ActionScrollEventArgs : EventArgs
    {
      private AccessibilityManager _accessibilityManager;
      private TouchEvent _touchEvent;

      public AccessibilityManager AccessibilityManager
      {
        get
        {
          return _accessibilityManager;
        }
        set
        {
          _accessibilityManager = value;
        }
      }

      public TouchEvent TouchEvent
      {
        get
        {
          return _touchEvent;
        }
        set
        {
          _touchEvent = value;
        }
      }
    }

    /**
      * @brief Event arguments that passed via ActionPageUp signal
      *
      */
    public class FocusChangedEventArgs : EventArgs
    {
      private Actor _actorCurrent;
      private Actor _actorNext;

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
      * @brief Event arguments that passed via FocusedActorActivated signal
      *
      */
    public class FocusedActorActivatedEventArgs : EventArgs
    {
      private Actor _actor;


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
      * @brief Event arguments that passed via FocusOvershot signal
      *
      */
    public class FocusOvershotEventArgs : EventArgs
    {
      private Actor _currentFocusedActor;
      private AccessibilityManager.FocusOvershotDirection _focusOvershotDirection;

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

      public AccessibilityManager.FocusOvershotDirection FocusOvershotDirection
      {
        get
        {
          return _focusOvershotDirection;
        }
        set
        {
          _focusOvershotDirection = value;
        }
      }
    }


    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool StatusChangedEventHandler(object source, StatusChangedEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionNextEventHandler(object source, ActionNextEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionPreviousEventHandler(object source, ActionPreviousEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionActivateEventHandler(object source, ActionActivateEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionReadEventHandler(object source, ActionReadEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionOverEventHandler(object source, ActionOverEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionReadNextEventHandler(object source, ActionReadNextEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionReadPreviousEventHandler(object source, ActionReadPreviousEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionUpEventHandler(object source, ActionUpEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionDownEventHandler(object source, ActionDownEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionClearFocusEventHandler(object source, ActionClearFocusEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionBackEventHandler(object source, ActionBackEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionScrollUpEventHandler(object source, ActionScrollUpEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionScrollDownEventHandler(object source, ActionScrollDownEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionPageLeftEventHandler(object source, ActionPageLeftEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionPageRightEventHandler(object source, ActionPageRightEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionPageUpEventHandler(object source, ActionPageUpEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionPageDownEventHandler(object source, ActionPageDownEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionMoveToFirstEventHandler(object source, ActionMoveToFirstEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionMoveToLastEventHandler(object source, ActionMoveToLastEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionReadFromTopEventHandler(object source, ActionReadFromTopEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionReadFromNextEventHandler(object source, ActionReadFromNextEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionZoomEventHandler(object source, ActionZoomEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionReadIndicatorInformationEventHandler(object source, ActionReadIndicatorInformationEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionReadPauseResumeEventHandler(object source, ActionReadPauseResumeEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionStartStopEventHandler(object source, ActionStartStopEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool ActionScrollEventHandler(object source, ActionScrollEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate void FocusChangedEventHandler(object source, FocusChangedEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate void FocusedActorActivatedEventHandler(object source, FocusedActorActivatedEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate void FocusOvershotEventHandler(object source, FocusOvershotEventArgs e);
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool StatusChangedEventCallbackDelegate(IntPtr accessibilityManager);
    private StatusChangedEventHandler _accessibilityManagerStatusChangedEventHandler;
    private StatusChangedEventCallbackDelegate _accessibilityManagerStatusChangedEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionNextEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionNextEventHandler _accessibilityManagerActionNextEventHandler;
    private ActionNextEventCallbackDelegate _accessibilityManagerActionNextEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionPreviousEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionPreviousEventHandler _accessibilityManagerActionPreviousEventHandler;
    private ActionPreviousEventCallbackDelegate _accessibilityManagerActionPreviousEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionActivateEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionActivateEventHandler _accessibilityManagerActionActivateEventHandler;
    private ActionActivateEventCallbackDelegate _accessibilityManagerActionActivateEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionReadEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionReadEventHandler _accessibilityManagerActionReadEventHandler;
    private ActionReadEventCallbackDelegate _accessibilityManagerActionReadEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionOverEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionOverEventHandler _accessibilityManagerActionOverEventHandler;
    private ActionOverEventCallbackDelegate _accessibilityManagerActionOverEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionReadNextEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionReadNextEventHandler _accessibilityManagerActionReadNextEventHandler;
    private ActionReadNextEventCallbackDelegate _accessibilityManagerActionReadNextEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionReadPreviousEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionReadPreviousEventHandler _accessibilityManagerActionReadPreviousEventHandler;
    private ActionReadPreviousEventCallbackDelegate _accessibilityManagerActionReadPreviousEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionUpEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionUpEventHandler _accessibilityManagerActionUpEventHandler;
    private ActionUpEventCallbackDelegate _accessibilityManagerActionUpEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionDownEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionDownEventHandler _accessibilityManagerActionDownEventHandler;
    private ActionDownEventCallbackDelegate _accessibilityManagerActionDownEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionClearFocusEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionClearFocusEventHandler _accessibilityManagerActionClearFocusEventHandler;
    private ActionClearFocusEventCallbackDelegate _accessibilityManagerActionClearFocusEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionBackEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionBackEventHandler _accessibilityManagerActionBackEventHandler;
    private ActionBackEventCallbackDelegate _accessibilityManagerActionBackEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionScrollUpEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionScrollUpEventHandler _accessibilityManagerActionScrollUpEventHandler;
    private ActionScrollUpEventCallbackDelegate _accessibilityManagerActionScrollUpEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionScrollDownEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionScrollDownEventHandler _accessibilityManagerActionScrollDownEventHandler;
    private ActionScrollDownEventCallbackDelegate _accessibilityManagerActionScrollDownEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionPageLeftEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionPageLeftEventHandler _accessibilityManagerActionPageLeftEventHandler;
    private ActionPageLeftEventCallbackDelegate _accessibilityManagerActionPageLeftEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionPageRightEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionPageRightEventHandler _accessibilityManagerActionPageRightEventHandler;
    private ActionPageRightEventCallbackDelegate _accessibilityManagerActionPageRightEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionPageUpEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionPageUpEventHandler _accessibilityManagerActionPageUpEventHandler;
    private ActionPageUpEventCallbackDelegate _accessibilityManagerActionPageUpEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionPageDownEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionPageDownEventHandler _accessibilityManagerActionPageDownEventHandler;
    private ActionPageDownEventCallbackDelegate _accessibilityManagerActionPageDownEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionMoveToFirstEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionMoveToFirstEventHandler _accessibilityManagerActionMoveToFirstEventHandler;
    private ActionMoveToFirstEventCallbackDelegate _accessibilityManagerActionMoveToFirstEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionMoveToLastEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionMoveToLastEventHandler _accessibilityManagerActionMoveToLastEventHandler;
    private ActionMoveToLastEventCallbackDelegate _accessibilityManagerActionMoveToLastEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionReadFromTopEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionReadFromTopEventHandler _accessibilityManagerActionReadFromTopEventHandler;
    private ActionReadFromTopEventCallbackDelegate _accessibilityManagerActionReadFromTopEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionReadFromNextEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionReadFromNextEventHandler _accessibilityManagerActionReadFromNextEventHandler;
    private ActionReadFromNextEventCallbackDelegate _accessibilityManagerActionReadFromNextEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionZoomEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionZoomEventHandler _accessibilityManagerActionZoomEventHandler;
    private ActionZoomEventCallbackDelegate _accessibilityManagerActionZoomEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionReadIndicatorInformationEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionReadIndicatorInformationEventHandler _accessibilityManagerActionReadIndicatorInformationEventHandler;
    private ActionReadIndicatorInformationEventCallbackDelegate _accessibilityManagerActionReadIndicatorInformationEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionReadPauseResumeEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionReadPauseResumeEventHandler _accessibilityManagerActionReadPauseResumeEventHandler;
    private ActionReadPauseResumeEventCallbackDelegate _accessibilityManagerActionReadPauseResumeEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionStartStopEventCallbackDelegate(IntPtr accessibilityManager);
    private ActionStartStopEventHandler _accessibilityManagerActionStartStopEventHandler;
    private ActionStartStopEventCallbackDelegate _accessibilityManagerActionStartStopEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate bool ActionScrollEventCallbackDelegate(IntPtr accessibilityManager, IntPtr touchEvent);
    private ActionScrollEventHandler _accessibilityManagerActionScrollEventHandler;
    private ActionScrollEventCallbackDelegate _accessibilityManagerActionScrollEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void FocusChangedEventCallbackDelegate(IntPtr actor1, IntPtr actor2);
    private FocusChangedEventHandler _accessibilityManagerFocusChangedEventHandler;
    private FocusChangedEventCallbackDelegate _accessibilityManagerFocusChangedEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void FocusedActorActivatedEventCallbackDelegate(IntPtr actor);
    private FocusedActorActivatedEventHandler _accessibilityManagerFocusedActorActivatedEventHandler;
    private FocusedActorActivatedEventCallbackDelegate _accessibilityManagerFocusedActorActivatedEventCallbackDelegate;

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void FocusOvershotEventCallbackDelegate(IntPtr currentFocusedActor, AccessibilityManager.FocusOvershotDirection direction);
    private FocusOvershotEventHandler _accessibilityManagerFocusOvershotEventHandler;
    private FocusOvershotEventCallbackDelegate _accessibilityManagerFocusOvershotEventCallbackDelegate;

    public event StatusChangedEventHandler StatusChanged
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerStatusChangedEventHandler == null)
          {
            _accessibilityManagerStatusChangedEventHandler += value;

            _accessibilityManagerStatusChangedEventCallbackDelegate = new StatusChangedEventCallbackDelegate(OnStatusChanged);
            this.StatusChangedSignal().Connect(_accessibilityManagerStatusChangedEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerStatusChangedEventHandler != null)
          {
            this.StatusChangedSignal().Disconnect(_accessibilityManagerStatusChangedEventCallbackDelegate);
          }

          _accessibilityManagerStatusChangedEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager StatusChangedSignal
    private bool OnStatusChanged(IntPtr data)
    {
      StatusChangedEventArgs e = new StatusChangedEventArgs();

      // Populate all members of "e" (StatusChangedEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerStatusChangedEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerStatusChangedEventHandler(this, e);
      }
      return false;
    }

    public event ActionNextEventHandler ActionNext
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionNextEventHandler == null)
          {
            _accessibilityManagerActionNextEventHandler += value;

            _accessibilityManagerActionNextEventCallbackDelegate = new ActionNextEventCallbackDelegate(OnActionNext);
            this.ActionNextSignal().Connect(_accessibilityManagerActionNextEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionNextEventHandler != null)
          {
            this.ActionNextSignal().Disconnect(_accessibilityManagerActionNextEventCallbackDelegate);
          }

          _accessibilityManagerActionNextEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionNextSignal
    private bool OnActionNext(IntPtr data)
    {
      ActionNextEventArgs e = new ActionNextEventArgs();

      // Populate all members of "e" (ActionNextEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionNextEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionNextEventHandler(this, e);
      }
      return false;
    }

    public event ActionPreviousEventHandler ActionPrevious
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionPreviousEventHandler == null)
          {
            _accessibilityManagerActionPreviousEventHandler += value;

            _accessibilityManagerActionPreviousEventCallbackDelegate = new ActionPreviousEventCallbackDelegate(OnActionPrevious);
            this.ActionPreviousSignal().Connect(_accessibilityManagerActionPreviousEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionPreviousEventHandler != null)
          {
            this.ActionPreviousSignal().Disconnect(_accessibilityManagerActionPreviousEventCallbackDelegate);
          }

          _accessibilityManagerActionPreviousEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionPreviousSignal
    private bool OnActionPrevious(IntPtr data)
    {
      ActionPreviousEventArgs e = new ActionPreviousEventArgs();

      // Populate all members of "e" (ActionPreviousEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionPreviousEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionPreviousEventHandler(this, e);
      }
      return false;
    }

    public event ActionActivateEventHandler ActionActivate
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionActivateEventHandler == null)
          {
            _accessibilityManagerActionActivateEventHandler += value;

            _accessibilityManagerActionActivateEventCallbackDelegate = new ActionActivateEventCallbackDelegate(OnActionActivate);
            this.ActionActivateSignal().Connect(_accessibilityManagerActionActivateEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionActivateEventHandler != null)
          {
            this.ActionActivateSignal().Disconnect(_accessibilityManagerActionActivateEventCallbackDelegate);
          }

          _accessibilityManagerActionActivateEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionActivateSignal
    private bool OnActionActivate(IntPtr data)
    {
      ActionActivateEventArgs e = new ActionActivateEventArgs();

      // Populate all members of "e" (ActionActivateEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionActivateEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionActivateEventHandler(this, e);
      }
      return false;
    }

    public event ActionReadEventHandler ActionRead
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionReadEventHandler == null)
          {
            _accessibilityManagerActionReadEventHandler += value;

            _accessibilityManagerActionReadEventCallbackDelegate = new ActionReadEventCallbackDelegate(OnActionRead);
            this.ActionReadSignal().Connect(_accessibilityManagerActionReadEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionReadEventHandler != null)
          {
            this.ActionReadSignal().Disconnect(_accessibilityManagerActionReadEventCallbackDelegate);
          }

          _accessibilityManagerActionReadEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionReadSignal
    private bool OnActionRead(IntPtr data)
    {
      ActionReadEventArgs e = new ActionReadEventArgs();

      // Populate all members of "e" (ActionReadEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionReadEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionReadEventHandler(this, e);
      }
      return false;
    }

    public event ActionOverEventHandler ActionOver
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionOverEventHandler == null)
          {
            _accessibilityManagerActionOverEventHandler += value;

            _accessibilityManagerActionOverEventCallbackDelegate = new ActionOverEventCallbackDelegate(OnActionOver);
            this.ActionOverSignal().Connect(_accessibilityManagerActionOverEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionOverEventHandler != null)
          {
            this.ActionOverSignal().Disconnect(_accessibilityManagerActionOverEventCallbackDelegate);
          }

          _accessibilityManagerActionOverEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionOverSignal
    private bool OnActionOver(IntPtr data)
    {
      ActionOverEventArgs e = new ActionOverEventArgs();

      // Populate all members of "e" (ActionOverEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionOverEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionOverEventHandler(this, e);
      }
      return false;
    }

    public event ActionReadNextEventHandler ActionReadNext
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionReadNextEventHandler == null)
          {
            _accessibilityManagerActionReadNextEventHandler += value;

            _accessibilityManagerActionReadNextEventCallbackDelegate = new ActionReadNextEventCallbackDelegate(OnActionReadNext);
            this.ActionReadNextSignal().Connect(_accessibilityManagerActionReadNextEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionReadNextEventHandler != null)
          {
            this.ActionReadNextSignal().Disconnect(_accessibilityManagerActionReadNextEventCallbackDelegate);
          }

          _accessibilityManagerActionReadNextEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionReadNextSignal
    private bool OnActionReadNext(IntPtr data)
    {
      ActionReadNextEventArgs e = new ActionReadNextEventArgs();

      // Populate all members of "e" (ActionReadNextEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionReadNextEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionReadNextEventHandler(this, e);
      }
      return false;
    }


    public event ActionReadPreviousEventHandler ActionReadPrevious
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionReadPreviousEventHandler == null)
          {
            _accessibilityManagerActionReadPreviousEventHandler += value;

            _accessibilityManagerActionReadPreviousEventCallbackDelegate = new ActionReadPreviousEventCallbackDelegate(OnActionReadPrevious);
            this.ActionReadPreviousSignal().Connect(_accessibilityManagerActionReadPreviousEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionReadPreviousEventHandler != null)
          {
            this.ActionReadPreviousSignal().Disconnect(_accessibilityManagerActionReadPreviousEventCallbackDelegate);
          }

          _accessibilityManagerActionReadPreviousEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionReadPreviousSignal
    private bool OnActionReadPrevious(IntPtr data)
    {
      ActionReadPreviousEventArgs e = new ActionReadPreviousEventArgs();

      // Populate all members of "e" (ActionReadPreviousEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionReadPreviousEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionReadPreviousEventHandler(this, e);
      }
      return false;
    }

    public event ActionUpEventHandler ActionUp
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionUpEventHandler == null)
          {
            _accessibilityManagerActionUpEventHandler += value;

            _accessibilityManagerActionUpEventCallbackDelegate = new ActionUpEventCallbackDelegate(OnActionUp);
            this.ActionUpSignal().Connect(_accessibilityManagerActionUpEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionUpEventHandler != null)
          {
            this.ActionUpSignal().Disconnect(_accessibilityManagerActionUpEventCallbackDelegate);
          }

          _accessibilityManagerActionUpEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionUpSignal
    private bool OnActionUp(IntPtr data)
    {
      ActionUpEventArgs e = new ActionUpEventArgs();

      // Populate all members of "e" (ActionUpEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionUpEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionUpEventHandler(this, e);
      }
      return false;
    }

    public event ActionDownEventHandler ActionDown
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionDownEventHandler == null)
          {
            _accessibilityManagerActionDownEventHandler += value;

            _accessibilityManagerActionDownEventCallbackDelegate = new ActionDownEventCallbackDelegate(OnActionDown);
            this.ActionDownSignal().Connect(_accessibilityManagerActionDownEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionDownEventHandler != null)
          {
            this.ActionDownSignal().Disconnect(_accessibilityManagerActionDownEventCallbackDelegate);
          }

          _accessibilityManagerActionDownEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionDownSignal
    private bool OnActionDown(IntPtr data)
    {
      ActionDownEventArgs e = new ActionDownEventArgs();

      // Populate all members of "e" (ActionDownEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionDownEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionDownEventHandler(this, e);
      }
      return false;
    }

    public event ActionClearFocusEventHandler ActionClearFocus
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionClearFocusEventHandler == null)
          {
            _accessibilityManagerActionClearFocusEventHandler += value;

            _accessibilityManagerActionClearFocusEventCallbackDelegate = new ActionClearFocusEventCallbackDelegate(OnActionClearFocus);
            this.ActionClearFocusSignal().Connect(_accessibilityManagerActionClearFocusEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionClearFocusEventHandler != null)
          {
            this.ActionClearFocusSignal().Disconnect(_accessibilityManagerActionClearFocusEventCallbackDelegate);
          }

          _accessibilityManagerActionClearFocusEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionClearFocusSignal
    private bool OnActionClearFocus(IntPtr data)
    {
      ActionClearFocusEventArgs e = new ActionClearFocusEventArgs();

      // Populate all members of "e" (ActionClearFocusEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionClearFocusEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionClearFocusEventHandler(this, e);
      }
      return false;
    }

    public event ActionBackEventHandler ActionBack
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionBackEventHandler == null)
          {
            _accessibilityManagerActionBackEventHandler += value;

            _accessibilityManagerActionBackEventCallbackDelegate = new ActionBackEventCallbackDelegate(OnActionBack);
            this.ActionBackSignal().Connect(_accessibilityManagerActionBackEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionBackEventHandler != null)
          {
            this.ActionBackSignal().Disconnect(_accessibilityManagerActionBackEventCallbackDelegate);
          }

          _accessibilityManagerActionBackEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionBackSignal
    private bool OnActionBack(IntPtr data)
    {
      ActionBackEventArgs e = new ActionBackEventArgs();

      // Populate all members of "e" (ActionBackEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionBackEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionBackEventHandler(this, e);
      }
      return false;
    }

    public event ActionScrollUpEventHandler ActionScrollUp
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionScrollUpEventHandler == null)
          {
            _accessibilityManagerActionScrollUpEventHandler += value;

            _accessibilityManagerActionScrollUpEventCallbackDelegate = new ActionScrollUpEventCallbackDelegate(OnActionScrollUp);
            this.ActionScrollUpSignal().Connect(_accessibilityManagerActionScrollUpEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionScrollUpEventHandler != null)
          {
            this.ActionScrollUpSignal().Disconnect(_accessibilityManagerActionScrollUpEventCallbackDelegate);
          }

          _accessibilityManagerActionScrollUpEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionScrollUpSignal
    private bool OnActionScrollUp(IntPtr data)
    {
      ActionScrollUpEventArgs e = new ActionScrollUpEventArgs();

      // Populate all members of "e" (ActionScrollUpEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionScrollUpEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionScrollUpEventHandler(this, e);
      }
      return false;
    }

    public event ActionScrollDownEventHandler ActionScrollDown
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionScrollDownEventHandler == null)
          {
            _accessibilityManagerActionScrollDownEventHandler += value;

            _accessibilityManagerActionScrollDownEventCallbackDelegate = new ActionScrollDownEventCallbackDelegate(OnActionScrollDown);
            this.ActionScrollDownSignal().Connect(_accessibilityManagerActionScrollDownEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionScrollDownEventHandler != null)
          {
            this.ActionScrollDownSignal().Disconnect(_accessibilityManagerActionScrollDownEventCallbackDelegate);
          }

          _accessibilityManagerActionScrollDownEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionScrollDownSignal
    private bool OnActionScrollDown(IntPtr data)
    {
      ActionScrollDownEventArgs e = new ActionScrollDownEventArgs();

      // Populate all members of "e" (ActionScrollDownEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionScrollDownEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionScrollDownEventHandler(this, e);
      }
      return false;
    }


    public event ActionPageLeftEventHandler ActionPageLeft
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionPageLeftEventHandler == null)
          {
            _accessibilityManagerActionPageLeftEventHandler += value;

            _accessibilityManagerActionPageLeftEventCallbackDelegate = new ActionPageLeftEventCallbackDelegate(OnActionPageLeft);
            this.ActionPageLeftSignal().Connect(_accessibilityManagerActionPageLeftEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionPageLeftEventHandler != null)
          {
            this.ActionPageLeftSignal().Disconnect(_accessibilityManagerActionPageLeftEventCallbackDelegate);
          }

          _accessibilityManagerActionPageLeftEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionPageLeftSignal
    private bool OnActionPageLeft(IntPtr data)
    {
      ActionPageLeftEventArgs e = new ActionPageLeftEventArgs();

      // Populate all members of "e" (ActionPageLeftEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionPageLeftEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionPageLeftEventHandler(this, e);
      }
      return false;
    }

    public event ActionPageRightEventHandler ActionPageRight
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionPageRightEventHandler == null)
          {
            _accessibilityManagerActionPageRightEventHandler += value;

            _accessibilityManagerActionPageRightEventCallbackDelegate = new ActionPageRightEventCallbackDelegate(OnActionPageRight);
            this.ActionPageRightSignal().Connect(_accessibilityManagerActionPageRightEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionPageRightEventHandler != null)
          {
            this.ActionPageRightSignal().Disconnect(_accessibilityManagerActionPageRightEventCallbackDelegate);
          }

          _accessibilityManagerActionPageRightEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionPageRightSignal
    private bool OnActionPageRight(IntPtr data)
    {
      ActionPageRightEventArgs e = new ActionPageRightEventArgs();

      // Populate all members of "e" (ActionPageRightEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionPageRightEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionPageRightEventHandler(this, e);
      }
      return false;
    }

    public event ActionPageUpEventHandler ActionPageUp
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionPageUpEventHandler == null)
          {
            _accessibilityManagerActionPageUpEventHandler += value;

            _accessibilityManagerActionPageUpEventCallbackDelegate = new ActionPageUpEventCallbackDelegate(OnActionPageUp);
            this.ActionPageUpSignal().Connect(_accessibilityManagerActionPageUpEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionPageUpEventHandler != null)
          {
            this.ActionPageUpSignal().Disconnect(_accessibilityManagerActionPageUpEventCallbackDelegate);
          }

          _accessibilityManagerActionPageUpEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionPageUpSignal
    private bool OnActionPageUp(IntPtr data)
    {
      ActionPageUpEventArgs e = new ActionPageUpEventArgs();

      // Populate all members of "e" (ActionPageUpEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionPageUpEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionPageUpEventHandler(this, e);
      }
      return false;
    }


    public event ActionPageDownEventHandler ActionPageDown
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionPageDownEventHandler == null)
          {
            _accessibilityManagerActionPageDownEventHandler += value;

            _accessibilityManagerActionPageDownEventCallbackDelegate = new ActionPageDownEventCallbackDelegate(OnActionPageDown);
            this.ActionPageDownSignal().Connect(_accessibilityManagerActionPageDownEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionPageDownEventHandler != null)
          {
            this.ActionPageDownSignal().Disconnect(_accessibilityManagerActionPageDownEventCallbackDelegate);
          }

          _accessibilityManagerActionPageDownEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionPageDownSignal
    private bool OnActionPageDown(IntPtr data)
    {
      ActionPageDownEventArgs e = new ActionPageDownEventArgs();

      // Populate all members of "e" (ActionPageDownEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionPageDownEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionPageDownEventHandler(this, e);
      }
      return false;
    }


    public event ActionMoveToFirstEventHandler ActionMoveToFirst
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionMoveToFirstEventHandler == null)
          {
            _accessibilityManagerActionMoveToFirstEventHandler += value;

            _accessibilityManagerActionMoveToFirstEventCallbackDelegate = new ActionMoveToFirstEventCallbackDelegate(OnActionMoveToFirst);
            this.ActionMoveToFirstSignal().Connect(_accessibilityManagerActionMoveToFirstEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionMoveToFirstEventHandler != null)
          {
            this.ActionMoveToFirstSignal().Disconnect(_accessibilityManagerActionMoveToFirstEventCallbackDelegate);
          }

          _accessibilityManagerActionMoveToFirstEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionMoveToFirstSignal
    private bool OnActionMoveToFirst(IntPtr data)
    {
      ActionMoveToFirstEventArgs e = new ActionMoveToFirstEventArgs();

      // Populate all members of "e" (ActionMoveToFirstEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionMoveToFirstEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionMoveToFirstEventHandler(this, e);
      }
      return false;
    }

    public event ActionMoveToLastEventHandler ActionMoveToLast
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionMoveToLastEventHandler == null)
          {
            _accessibilityManagerActionMoveToLastEventHandler += value;

            _accessibilityManagerActionMoveToLastEventCallbackDelegate = new ActionMoveToLastEventCallbackDelegate(OnActionMoveToLast);
            this.ActionMoveToLastSignal().Connect(_accessibilityManagerActionMoveToLastEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionMoveToLastEventHandler != null)
          {
            this.ActionMoveToLastSignal().Disconnect(_accessibilityManagerActionMoveToLastEventCallbackDelegate);
          }

          _accessibilityManagerActionMoveToLastEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionMoveToLastSignal
    private bool OnActionMoveToLast(IntPtr data)
    {
      ActionMoveToLastEventArgs e = new ActionMoveToLastEventArgs();

      // Populate all members of "e" (ActionMoveToLastEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionMoveToLastEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionMoveToLastEventHandler(this, e);
      }
      return false;
    }

    public event ActionReadFromTopEventHandler ActionReadFromTop
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionReadFromTopEventHandler == null)
          {
            _accessibilityManagerActionReadFromTopEventHandler += value;

            _accessibilityManagerActionReadFromTopEventCallbackDelegate = new ActionReadFromTopEventCallbackDelegate(OnActionReadFromTop);
            this.ActionReadFromTopSignal().Connect(_accessibilityManagerActionReadFromTopEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionReadFromTopEventHandler != null)
          {
            this.ActionReadFromTopSignal().Disconnect(_accessibilityManagerActionReadFromTopEventCallbackDelegate);
          }

          _accessibilityManagerActionReadFromTopEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionReadFromTopSignal
    private bool OnActionReadFromTop(IntPtr data)
    {
      ActionReadFromTopEventArgs e = new ActionReadFromTopEventArgs();

      // Populate all members of "e" (ActionReadFromTopEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionReadFromTopEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionReadFromTopEventHandler(this, e);
      }
      return false;
    }

    public event ActionReadFromNextEventHandler ActionReadFromNext
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionReadFromNextEventHandler == null)
          {
            _accessibilityManagerActionReadFromNextEventHandler += value;

            _accessibilityManagerActionReadFromNextEventCallbackDelegate = new ActionReadFromNextEventCallbackDelegate(OnActionReadFromNext);
            this.ActionReadFromNextSignal().Connect(_accessibilityManagerActionReadFromNextEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionReadFromNextEventHandler != null)
          {
            this.ActionReadFromNextSignal().Disconnect(_accessibilityManagerActionReadFromNextEventCallbackDelegate);
          }

          _accessibilityManagerActionReadFromNextEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionReadFromNextSignal
    private bool OnActionReadFromNext(IntPtr data)
    {
      ActionReadFromNextEventArgs e = new ActionReadFromNextEventArgs();

      // Populate all members of "e" (ActionReadFromNextEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionReadFromNextEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionReadFromNextEventHandler(this, e);
      }
      return false;
    }

    public event ActionZoomEventHandler ActionZoom
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionZoomEventHandler == null)
          {
            _accessibilityManagerActionZoomEventHandler += value;

            _accessibilityManagerActionZoomEventCallbackDelegate = new ActionZoomEventCallbackDelegate(OnActionZoom);
            this.ActionZoomSignal().Connect(_accessibilityManagerActionZoomEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionZoomEventHandler != null)
          {
            this.ActionZoomSignal().Disconnect(_accessibilityManagerActionZoomEventCallbackDelegate);
          }

          _accessibilityManagerActionZoomEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionZoomSignal
    private bool OnActionZoom(IntPtr data)
    {
      ActionZoomEventArgs e = new ActionZoomEventArgs();

      // Populate all members of "e" (ActionZoomEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionZoomEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionZoomEventHandler(this, e);
      }
      return false;
    }

    public event ActionReadIndicatorInformationEventHandler ActionReadIndicatorInformation
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionReadIndicatorInformationEventHandler == null)
          {
            _accessibilityManagerActionReadIndicatorInformationEventHandler += value;

            _accessibilityManagerActionReadIndicatorInformationEventCallbackDelegate = new ActionReadIndicatorInformationEventCallbackDelegate(OnActionReadIndicatorInformation);
            this.ActionReadIndicatorInformationSignal().Connect(_accessibilityManagerActionReadIndicatorInformationEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionReadIndicatorInformationEventHandler != null)
          {
            this.ActionReadIndicatorInformationSignal().Disconnect(_accessibilityManagerActionReadIndicatorInformationEventCallbackDelegate);
          }

          _accessibilityManagerActionReadIndicatorInformationEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionReadIndicatorInformationSignal
    private bool OnActionReadIndicatorInformation(IntPtr data)
    {
      ActionReadIndicatorInformationEventArgs e = new ActionReadIndicatorInformationEventArgs();

      // Populate all members of "e" (ActionReadIndicatorInformationEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionReadIndicatorInformationEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionReadIndicatorInformationEventHandler(this, e);
      }
      return false;
    }

    public event ActionReadPauseResumeEventHandler ActionReadPauseResume
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionReadPauseResumeEventHandler == null)
          {
            _accessibilityManagerActionReadPauseResumeEventHandler += value;

            _accessibilityManagerActionReadPauseResumeEventCallbackDelegate = new ActionReadPauseResumeEventCallbackDelegate(OnActionReadPauseResume);
            this.ActionReadPauseResumeSignal().Connect(_accessibilityManagerActionReadPauseResumeEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionReadPauseResumeEventHandler != null)
          {
            this.ActionReadPauseResumeSignal().Disconnect(_accessibilityManagerActionReadPauseResumeEventCallbackDelegate);
          }

          _accessibilityManagerActionReadPauseResumeEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionReadPauseResumeSignal
    private bool OnActionReadPauseResume(IntPtr data)
    {
      ActionReadPauseResumeEventArgs e = new ActionReadPauseResumeEventArgs();

      // Populate all members of "e" (ActionReadPauseResumeEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionReadPauseResumeEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionReadPauseResumeEventHandler(this, e);
      }
      return false;
    }

    public event ActionStartStopEventHandler ActionStartStop
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionStartStopEventHandler == null)
          {
            _accessibilityManagerActionStartStopEventHandler += value;

            _accessibilityManagerActionStartStopEventCallbackDelegate = new ActionStartStopEventCallbackDelegate(OnActionStartStop);
            this.ActionStartStopSignal().Connect(_accessibilityManagerActionStartStopEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionStartStopEventHandler != null)
          {
            this.ActionStartStopSignal().Disconnect(_accessibilityManagerActionStartStopEventCallbackDelegate);
          }

          _accessibilityManagerActionStartStopEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionStartStopSignal
    private bool OnActionStartStop(IntPtr data)
    {
      ActionStartStopEventArgs e = new ActionStartStopEventArgs();

      // Populate all members of "e" (ActionStartStopEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(data);

      if (_accessibilityManagerActionStartStopEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionStartStopEventHandler(this, e);
      }
      return false;
    }

    public event ActionScrollEventHandler ActionScroll
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerActionScrollEventHandler == null)
          {
            _accessibilityManagerActionScrollEventHandler += value;

            _accessibilityManagerActionScrollEventCallbackDelegate = new ActionScrollEventCallbackDelegate(OnActionScroll);
            this.ActionScrollSignal().Connect(_accessibilityManagerActionScrollEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerActionScrollEventHandler != null)
          {
            this.ActionScrollSignal().Disconnect(_accessibilityManagerActionScrollEventCallbackDelegate);
          }

          _accessibilityManagerActionScrollEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager ActionScrollSignal
    private bool OnActionScroll(IntPtr accessibilityManager, IntPtr touchEvent)
    {
      ActionScrollEventArgs e = new ActionScrollEventArgs();

      // Populate all members of "e" (ActionScrollEventArgs) with real data
      e.AccessibilityManager = AccessibilityManager.GetAccessibilityManagerFromPtr(accessibilityManager);
      e.TouchEvent = TouchEvent.GetTouchEventFromPtr(touchEvent);

      if (_accessibilityManagerActionScrollEventHandler != null)
      {
        //here we send all data to user event handlers
        return _accessibilityManagerActionScrollEventHandler(this, e);
      }
      return false;
    }

    public event FocusChangedEventHandler FocusChanged
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerFocusChangedEventHandler == null)
          {
            _accessibilityManagerFocusChangedEventHandler += value;

            _accessibilityManagerFocusChangedEventCallbackDelegate = new FocusChangedEventCallbackDelegate(OnFocusChanged);
            this.FocusChangedSignal().Connect(_accessibilityManagerFocusChangedEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerFocusChangedEventHandler != null)
          {
            this.FocusChangedSignal().Disconnect(_accessibilityManagerFocusChangedEventCallbackDelegate);
          }

          _accessibilityManagerFocusChangedEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager FocusChangedSignal
    private void OnFocusChanged(IntPtr actor1, IntPtr actor2)
    {
      FocusChangedEventArgs e = new FocusChangedEventArgs();

      // Populate all members of "e" (FocusChangedEventArgs) with real data
      e.ActorCurrent = Actor.GetActorFromPtr(actor1);
      e.ActorNext = Actor.GetActorFromPtr(actor2);

      if (_accessibilityManagerFocusChangedEventHandler != null)
      {
        //here we send all data to user event handlers
        _accessibilityManagerFocusChangedEventHandler(this, e);
      }
    }

    public event FocusedActorActivatedEventHandler FocusedActorActivated
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerFocusedActorActivatedEventHandler == null)
          {
            _accessibilityManagerFocusedActorActivatedEventHandler += value;

            _accessibilityManagerFocusedActorActivatedEventCallbackDelegate = new FocusedActorActivatedEventCallbackDelegate(OnFocusedActorActivated);
            this.FocusedActorActivatedSignal().Connect(_accessibilityManagerFocusedActorActivatedEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerFocusedActorActivatedEventHandler != null)
          {
            this.FocusedActorActivatedSignal().Disconnect(_accessibilityManagerFocusedActorActivatedEventCallbackDelegate);
          }

          _accessibilityManagerFocusedActorActivatedEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager FocusedActorActivatedSignal
    private void OnFocusedActorActivated(IntPtr actor)
    {
      FocusedActorActivatedEventArgs e = new FocusedActorActivatedEventArgs();

      // Populate all members of "e" (FocusedActorActivatedEventArgs) with real data
      e.Actor = Actor.GetActorFromPtr(actor);

      if (_accessibilityManagerFocusedActorActivatedEventHandler != null)
      {
        //here we send all data to user event handlers
        _accessibilityManagerFocusedActorActivatedEventHandler(this, e);
      }
    }


    public event FocusOvershotEventHandler FocusOvershot
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_accessibilityManagerFocusOvershotEventHandler == null)
          {
            _accessibilityManagerFocusOvershotEventHandler += value;

            _accessibilityManagerFocusOvershotEventCallbackDelegate = new FocusOvershotEventCallbackDelegate(OnFocusOvershot);
            this.FocusOvershotSignal().Connect(_accessibilityManagerFocusOvershotEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_accessibilityManagerFocusOvershotEventHandler != null)
          {
            this.FocusOvershotSignal().Disconnect(_accessibilityManagerFocusOvershotEventCallbackDelegate);
          }

          _accessibilityManagerFocusOvershotEventHandler -= value;
        }
      }
    }

    // Callback for AccessibilityManager FocusOvershotSignal
    private void OnFocusOvershot(IntPtr currentFocusedActor, AccessibilityManager.FocusOvershotDirection direction)
    {
      FocusOvershotEventArgs e = new FocusOvershotEventArgs();

      // Populate all members of "e" (FocusOvershotEventArgs) with real data
      e.CurrentFocusedActor = Actor.GetActorFromPtr(currentFocusedActor);
      e.FocusOvershotDirection = direction;

      if (_accessibilityManagerFocusOvershotEventHandler != null)
      {
        //here we send all data to user event handlers
        _accessibilityManagerFocusOvershotEventHandler(this, e);
      }
    }


    public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
      ClassName ret = new ClassName(cPtr, false);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    }

    %}
    %enddef

%define DALI_ACCESSIBILITY_EVENTHANDLER_PARAM( NameSpace, ClassName)
  ACCESSIBILITY_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  ACCESSIBILITY_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);
  %enddef

  namespace Dali
{
  DALI_ACCESSIBILITY_EVENTHANDLER_PARAM( Dali::Toolkit, AccessibilityManager);
}


