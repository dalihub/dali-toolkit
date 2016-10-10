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

%define TapGesture_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;
%}

%enddef

%define TapGesture_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{


public class DetectedEventArgs : EventArgs
{
   private Actor _actor;
   private TapGesture _tapGesture;

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

   public TapGesture TapGesture
   {
      get
      {
         return _tapGesture;
      }
      set
      {
         _tapGesture = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void DetectedCallbackDelegate(IntPtr actor, IntPtr TapGesture);
  private DaliEventHandler<object,DetectedEventArgs> _tapGestureEventHandler;
  private DetectedCallbackDelegate _tapGestureCallbackDelegate;


  public event DaliEventHandler<object,DetectedEventArgs> Detected
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_tapGestureEventHandler == null)
           {
              _tapGestureEventHandler += value;

              _tapGestureCallbackDelegate = new DetectedCallbackDelegate(OnTapGestureDetected);
              this.DetectedSignal().Connect(_tapGestureCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_tapGestureEventHandler != null)
           {
              this.DetectedSignal().Disconnect(_tapGestureCallbackDelegate);
           }

           _tapGestureEventHandler -= value;
        }
     }
  }

 private void OnTapGestureDetected(IntPtr actor, IntPtr tapGesture)
  {
   DetectedEventArgs e = new DetectedEventArgs();

   // Populate all members of "e" (DetectedEventArgs) with real data
   e.Actor = Actor.GetActorFromPtr(actor);
   e.TapGesture = Dali.TapGesture.GetTapGestureFromPtr(tapGesture);

   if (_tapGestureEventHandler != null)
   {
      //here we send all data to user event handlers
      _tapGestureEventHandler(this, e);
   }

  }


public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

%}

%enddef

%define DALI_tapGesture_EVENTHANDLER_PARAM( NameSpace, ClassName)

  TapGesture_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  TapGesture_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_tapGesture_EVENTHANDLER_PARAM( Dali, TapGestureDetector);
}
