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

%define PANGESTURE_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}

%enddef

%define PANGESTURE_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{


public class DetectedEventArgs : EventArgs
{
   private Actor _actor;
   private PanGesture _panGesture;

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

   public PanGesture PanGesture
   {
      get
      {
         return _panGesture;
      }
      set
      {
         _panGesture = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void DetectedEventHandler(object source, DetectedEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void DetectedCallbackDelegate(IntPtr actor, IntPtr panGesture);
  private DetectedEventHandler _panGestureEventHandler;
  private DetectedCallbackDelegate _panGestureCallbackDelegate;


  public event DetectedEventHandler Detected
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_panGestureEventHandler == null)
           {
              _panGestureEventHandler += value;

              _panGestureCallbackDelegate = new DetectedCallbackDelegate(OnPanGestureDetected);
              this.DetectedSignal().Connect(_panGestureCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_panGestureEventHandler != null)
           {
              this.DetectedSignal().Disconnect(_panGestureCallbackDelegate);
           }

           _panGestureEventHandler -= value;
        }
     }
  }

 private void OnPanGestureDetected(IntPtr actor, IntPtr panGesture)
  {
   DetectedEventArgs e = new DetectedEventArgs();

   // Populate all members of "e" (PanGestureEventArgs) with real data
   e.Actor = Actor.GetActorFromPtr(actor);
   e.PanGesture = Dali.PanGesture.GetPanGestureFromPtr(panGesture);

   if (_panGestureEventHandler != null)
   {
      //here we send all data to user event handlers
      _panGestureEventHandler(this, e);
   }

  }


public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

%}

%enddef

%define DALI_PANGESTURE_EVENTHANDLER_PARAM( NameSpace, ClassName)

  PANGESTURE_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  PANGESTURE_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_PANGESTURE_EVENTHANDLER_PARAM( Dali, PanGestureDetector);
}
