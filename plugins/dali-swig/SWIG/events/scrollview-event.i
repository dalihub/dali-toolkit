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

%define SCROLLVIEW_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}
%enddef

%define SCROLLVIEW_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/**
  * @brief Event arguments that passed via SnapStarted signal
  *
  */
public class SnapStartedEventArgs : EventArgs
{
   private Dali.ScrollView.SnapEvent _snapEvent;

   /**
     * @brief SnapEvent - is the SnapEvent information like snap or flick (it tells the target position, scale, rotation for the snap or flick).
     *
     */
   public Dali.ScrollView.SnapEvent SnapEventInfo
   {
      get
      {
         return _snapEvent;
      }
      set
      {
         _snapEvent = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void SnapStartedCallbackDelegate(IntPtr data);
  private DaliEventHandler<object,SnapStartedEventArgs> _scrollViewSnapStartedEventHandler;
  private SnapStartedCallbackDelegate _scrollViewSnapStartedCallbackDelegate;

  /**
    * @brief Event for SnapStarted signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of SnapStartedEventHandler-DaliEventHandler<object,SnapStartedEventArgs>) provided by the user.
    * SnapStarted signal is emitted hen the ScrollView has started to snap or flick (it tells the target
    * position, scale, rotation for the snap or flick).
    *
    */
 public event DaliEventHandler<object,SnapStartedEventArgs> SnapStarted
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_scrollViewSnapStartedEventHandler == null)
           {
              _scrollViewSnapStartedEventHandler += value;

              _scrollViewSnapStartedCallbackDelegate = new SnapStartedCallbackDelegate(OnSnapStarted);
              this.SnapStartedSignal().Connect(_scrollViewSnapStartedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_scrollViewSnapStartedEventHandler != null)
           {
              this.SnapStartedSignal().Disconnect(_scrollViewSnapStartedCallbackDelegate);
           }

           _scrollViewSnapStartedEventHandler -= value;
        }
     }
  }

  // Callback for ScrollView SnapStarted signal
  private void OnSnapStarted(IntPtr data)
  {
     SnapStartedEventArgs e = new SnapStartedEventArgs();

     // Populate all members of "e" (SnapStartedEventArgs) with real data
     e.SnapEventInfo = SnapEvent.GetSnapEventFromPtr( data );

     if (_scrollViewSnapStartedEventHandler != null)
     {
        //here we send all data to user event handlers
        _scrollViewSnapStartedEventHandler(this, e);
     }
  }

%}

%enddef


%define DALI_SCROLLVIEW_EVENTHANDLER_PARAM( NameSpace, ClassName)

  SCROLLVIEW_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  SCROLLVIEW_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_SCROLLVIEW_EVENTHANDLER_PARAM( Dali::Toolkit, ScrollView);
}
