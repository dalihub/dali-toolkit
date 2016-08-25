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

%define LongPressGesture_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;
%}
%enddef

%define LongPressGesture_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{


public class DetectedEventArgs : EventArgs
{
   private Actor _actor;
   private LongPressGesture _longPressGesture;

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

   public LongPressGesture LongPressGesture
   {
      get
      {
         return _longPressGesture;
      }
      set
      {
         _longPressGesture = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void DetectedEventHandler(object source, DetectedEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void DetectedCallbackDelegate(IntPtr actor, IntPtr longPressGesture);
  private DetectedEventHandler _longPressGestureEventHandler;
  private DetectedCallbackDelegate _longPressGestureCallbackDelegate;


  public event DetectedEventHandler Detected
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_longPressGestureEventHandler == null)
           {
              _longPressGestureEventHandler += value;

              _longPressGestureCallbackDelegate = new DetectedCallbackDelegate(OnLongPressGestureDetected);
              this.DetectedSignal().Connect(_longPressGestureCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_longPressGestureEventHandler != null)
           {
              this.DetectedSignal().Disconnect(_longPressGestureCallbackDelegate);
           }

           _longPressGestureEventHandler -= value;
        }
     }
  }

 private void OnLongPressGestureDetected(IntPtr actor, IntPtr longPressGesture)
  {
   DetectedEventArgs e = new DetectedEventArgs();

   // Populate all members of "e" (LongPressGestureEventArgs) with real data
   e.Actor = Actor.GetActorFromPtr(actor);
   e.LongPressGesture = Dali.LongPressGesture.GetLongPressGestureFromPtr(longPressGesture);

   if (_longPressGestureEventHandler != null)
   {
      //here we send all data to user event handlers
      _longPressGestureEventHandler(this, e);
   }

  }


public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

%}

%enddef

%define DALI_longPressGesture_EVENTHANDLER_PARAM( NameSpace, ClassName)

  LongPressGesture_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  LongPressGesture_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_longPressGesture_EVENTHANDLER_PARAM( Dali, LongPressGestureDetector);
}
