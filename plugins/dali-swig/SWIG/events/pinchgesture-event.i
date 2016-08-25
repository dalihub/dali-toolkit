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

%define PINCHGESTURE_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}

%enddef

%define PINCHGESTURE_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

public class DetectedEventArgs : EventArgs
{
   private Actor _actor;
   private PinchGesture _pinchGesture;

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

   public PinchGesture PinchGesture
   {
      get
      {
         return _pinchGesture;
      }
      set
      {
         _pinchGesture = value;
      }
   }
}

[UnmanagedFunctionPointer(CallingConvention.StdCall)]
public delegate void DetectedEventHandler(object source, DetectedEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void DetectedCallbackDelegate(IntPtr actor, IntPtr pinchGesture);
  private DetectedEventHandler _pinchGestureEventHandler;
  private DetectedCallbackDelegate _pinchGestureCallbackDelegate;


  public event DetectedEventHandler Detected
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_pinchGestureEventHandler == null)
           {
              _pinchGestureEventHandler += value;

              _pinchGestureCallbackDelegate = new DetectedCallbackDelegate(OnPinchGestureDetected);
              this.DetectedSignal().Connect(_pinchGestureCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_pinchGestureEventHandler != null)
           {
              this.DetectedSignal().Disconnect(_pinchGestureCallbackDelegate);
           }

           _pinchGestureEventHandler -= value;
        }
     }
  }

 private void OnPinchGestureDetected(IntPtr actor, IntPtr pinchGesture)
  {
   DetectedEventArgs e = new DetectedEventArgs();

   // Populate all members of "e" (DetectedEventArgs) with real data
   e.Actor = Actor.GetActorFromPtr(actor);
   e.PinchGesture = Dali.PinchGesture.GetPinchGestureFromPtr(pinchGesture);

   if (_pinchGestureEventHandler != null)
   {
      //here we send all data to user event handlers
      _pinchGestureEventHandler(this, e);
   }

  }


public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

%}

%enddef

%define DALI_PINCHGESTURE_EVENTHANDLER_PARAM( NameSpace, ClassName)

  PINCHGESTURE_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  PINCHGESTURE_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_PINCHGESTURE_EVENTHANDLER_PARAM( Dali, PinchGestureDetector);
}
