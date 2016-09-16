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

%define TIMER_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}
%enddef

%define TIMER_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

  /**
    * @brief Event arguments that passed via Tick signal
    *
    */
  public class TickEventArgs : EventArgs
  {
  }

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate bool TickEventHandler(object source, TickEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate bool TickCallbackDelegate(IntPtr data);
  private TickEventHandler _timerTickEventHandler;
  private TickCallbackDelegate _timerTickCallbackDelegate;

  /**
    * @brief Event for Ticked signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of TickEventHandler) provided by the user.
    * Ticked signal is emitted after specified time interval.
    */
  public event TickEventHandler Ticked
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_timerTickEventHandler == null)
           {
              _timerTickEventHandler += value;

              _timerTickCallbackDelegate = new TickCallbackDelegate(OnTick);
              this.TickSignal().Connect(_timerTickCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_timerTickEventHandler != null)
           {
              this.TickSignal().Disconnect(_timerTickCallbackDelegate);
           }

           _timerTickEventHandler -= value;
        }
     }
  }

  // Callback for Timer Tick signal
  private bool OnTick(IntPtr data)
  {
     TickEventArgs e = new TickEventArgs();

     if (_timerTickEventHandler != null)
     {
        //here we send all data to user event handlers
        return _timerTickEventHandler(this, e);
     }
     return false;
  }

%}

%enddef


%define DALI_TIMER_EVENTHANDLER_PARAM( NameSpace, ClassName)

  TIMER_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  TIMER_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_TIMER_EVENTHANDLER_PARAM( Dali::Adaptor, Timer);
}
