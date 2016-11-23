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

%define SCROLLBAR_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}

%enddef


%define SCROLLBAR_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{


public class PanFinishedEventArgs : EventArgs
{
}

public class ScrollPositionIntervalReachedEventArgs : EventArgs
{
   private float _currentScrollPosition;

   public float CurrentScrollPosition
   {
      get
      {
         return _currentScrollPosition;
      }
      set
      {
         _currentScrollPosition = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void PanFinishedEventCallbackDelegate();
  private DaliEventHandler<object,PanFinishedEventArgs> _scrollBarPanFinishedEventHandler;
  private PanFinishedEventCallbackDelegate _scrollBarPanFinishedEventCallbackDelegate;
  
  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void ScrollPositionIntervalReachedEventCallbackDelegate();
  private DaliEventHandler<object,ScrollPositionIntervalReachedEventArgs> _scrollBarScrollPositionIntervalReachedEventHandler;
  private ScrollPositionIntervalReachedEventCallbackDelegate _scrollBarScrollPositionIntervalReachedEventCallbackDelegate;

  public event DaliEventHandler<object,PanFinishedEventArgs> PanFinished
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_scrollBarPanFinishedEventHandler == null)
           {
              _scrollBarPanFinishedEventHandler += value;

              _scrollBarPanFinishedEventCallbackDelegate = new PanFinishedEventCallbackDelegate(OnScrollBarPanFinished);
              this.PanFinishedSignal().Connect(_scrollBarPanFinishedEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_scrollBarPanFinishedEventHandler != null)
           {
              this.PanFinishedSignal().Disconnect(_scrollBarPanFinishedEventCallbackDelegate);
           }

           _scrollBarPanFinishedEventHandler -= value;
        }
     }
  }

  // Callback for ScrollBar PanFinishedSignal
  private void OnScrollBarPanFinished()
  {
     PanFinishedEventArgs e = new PanFinishedEventArgs();

     if (_scrollBarPanFinishedEventHandler != null)
     {
        //here we send all data to user event handlers
        _scrollBarPanFinishedEventHandler(this, e);
     }
  }


  public event DaliEventHandler<object,ScrollPositionIntervalReachedEventArgs> ScrollPositionIntervalReached
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_scrollBarScrollPositionIntervalReachedEventHandler == null)
           {
              _scrollBarScrollPositionIntervalReachedEventHandler += value;

              _scrollBarScrollPositionIntervalReachedEventCallbackDelegate = new ScrollPositionIntervalReachedEventCallbackDelegate(OnScrollBarScrollPositionIntervalReached);
              this.ScrollPositionIntervalReachedSignal().Connect(_scrollBarScrollPositionIntervalReachedEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_scrollBarScrollPositionIntervalReachedEventHandler != null)
           {
              this.ScrollPositionIntervalReachedSignal().Disconnect(_scrollBarScrollPositionIntervalReachedEventCallbackDelegate);
           }

           _scrollBarScrollPositionIntervalReachedEventHandler -= value;
        }
     }
  }

  // Callback for ScrollBar ScrollPositionIntervalReachedSignal
  private void OnScrollBarScrollPositionIntervalReached()
  {
     ScrollPositionIntervalReachedEventArgs e = new ScrollPositionIntervalReachedEventArgs();

     if (_scrollBarScrollPositionIntervalReachedEventHandler != null)
     {
        //here we send all data to user event handlers
        _scrollBarScrollPositionIntervalReachedEventHandler(this, e);
     }
  }

%}
%enddef

%define DALI_SCROLLBAR_EVENTHANDLER_PARAM( NameSpace, ClassName)
  SCROLLBAR_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  SCROLLBAR_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);
%enddef

namespace Dali
{
  DALI_SCROLLBAR_EVENTHANDLER_PARAM( Dali::Toolkit, ScrollBar);
}

