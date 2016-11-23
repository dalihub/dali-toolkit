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

%define SCROLLABLE_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}
%enddef

%define SCROLLABLE_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

public class StartedEventArgs : EventArgs
{
   private Vector2 _vector2;

   public Vector2 Vector2
   {
      get
      {
         return _vector2;
      }
      set
      {
         _vector2 = value;
      }
   }
}

public class UpdatedEventArgs : EventArgs
{
   private Vector2 _vector2;

   public Vector2 Vector2
   {
      get
      {
         return _vector2;
      }
      set
      {
         _vector2 = value;
      }
   }
}

public class CompletedEventArgs : EventArgs
{
   private Vector2 _vector2;

   public Vector2 Vector2
   {
      get
      {
         return _vector2;
      }
      set
      {
         _vector2 = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void StartedCallbackDelegate(IntPtr vector2);
  private DaliEventHandler<object,StartedEventArgs> _scrollableStartedEventHandler;
  private StartedCallbackDelegate _scrollableStartedCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void UpdatedCallbackDelegate(IntPtr vector2);
  private DaliEventHandler<object,UpdatedEventArgs> _scrollableUpdatedEventHandler;
  private UpdatedCallbackDelegate _scrollableUpdatedCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void CompletedCallbackDelegate(IntPtr vector2);
  private DaliEventHandler<object,CompletedEventArgs> _scrollableCompletedEventHandler;
  private CompletedCallbackDelegate _scrollableCompletedCallbackDelegate;

  public event DaliEventHandler<object,StartedEventArgs> ScrollStarted
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_scrollableStartedEventHandler == null)
           {
              _scrollableStartedEventHandler += value;

              _scrollableStartedCallbackDelegate = new StartedCallbackDelegate(OnStarted);
              this.ScrollStartedSignal().Connect(_scrollableStartedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_scrollableStartedEventHandler != null)
           {
              this.ScrollStartedSignal().Disconnect(_scrollableStartedCallbackDelegate);
           }

           _scrollableStartedEventHandler -= value;
        }
     }
  }

 private void OnStarted(IntPtr vector2)
  {
   StartedEventArgs e = new StartedEventArgs();

   // Populate all members of "e" (StartedEventArgs) with real data
   e.Vector2 = Dali.Vector2.GetVector2FromPtr(vector2);

   if (_scrollableStartedEventHandler != null)
   {
      //here we send all data to user event handlers
      _scrollableStartedEventHandler(this, e);
   }

  }

  public event DaliEventHandler<object,UpdatedEventArgs> ScrollUpdated
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_scrollableUpdatedEventHandler == null)
           {
              _scrollableUpdatedEventHandler += value;

              _scrollableUpdatedCallbackDelegate = new UpdatedCallbackDelegate(OnUpdated);
              this.ScrollUpdatedSignal().Connect(_scrollableUpdatedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_scrollableUpdatedEventHandler != null)
           {
              this.ScrollUpdatedSignal().Disconnect(_scrollableUpdatedCallbackDelegate);
           }

           _scrollableUpdatedEventHandler -= value;
        }
     }
  }

 private void OnUpdated(IntPtr vector2)
  {
   UpdatedEventArgs e = new UpdatedEventArgs();

   // Populate all members of "e" (UpdatedEventArgs) with real data
   e.Vector2 = Dali.Vector2.GetVector2FromPtr(vector2);

   if (_scrollableUpdatedEventHandler != null)
   {
      //here we send all data to user event handlers
      _scrollableUpdatedEventHandler(this, e);
   }

  }

  public event DaliEventHandler<object,CompletedEventArgs> ScrollCompleted
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_scrollableCompletedEventHandler == null)
           {
              _scrollableCompletedEventHandler += value;

              _scrollableCompletedCallbackDelegate = new CompletedCallbackDelegate(OnCompleted);
              this.ScrollCompletedSignal().Connect(_scrollableCompletedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_scrollableCompletedEventHandler != null)
           {
              this.ScrollCompletedSignal().Disconnect(_scrollableCompletedCallbackDelegate);
           }

           _scrollableCompletedEventHandler -= value;
        }
     }
  }

 private void OnCompleted(IntPtr vector2)
  {
   CompletedEventArgs e = new CompletedEventArgs();

   // Populate all members of "e" (CompletedEventArgs) with real data
   e.Vector2 = Dali.Vector2.GetVector2FromPtr(vector2);

   if (_scrollableCompletedEventHandler != null)
   {
      //here we send all data to user event handlers
      _scrollableCompletedEventHandler(this, e);
   }

  }

%}

%enddef

%define DALI_SCROLLABLE_EVENTHANDLER_PARAM( NameSpace, ClassName)

  SCROLLABLE_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  SCROLLABLE_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_SCROLLABLE_EVENTHANDLER_PARAM( Dali::Toolkit, Scrollable);
}
