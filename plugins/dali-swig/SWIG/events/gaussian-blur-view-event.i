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

%define GAUSSIAN_BLURR_VIEW_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}

%enddef

%define GAUSSIAN_BLURR_VIEW_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{


public class FinishedEventArgs : EventArgs
{
   private GaussianBlurView _gaussianBlurView;

   public GaussianBlurView GaussianBlurView
   {
      get
      {
         return _gaussianBlurView;
      }
      set
      {
         _gaussianBlurView = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void FinishedEventCallbackDelegate(IntPtr application);
  private DaliEventHandler<object,FinishedEventArgs> _gaussianFinishedEventHandler;
  private FinishedEventCallbackDelegate _gaussianFinishedEventCallbackDelegate;

  public event DaliEventHandler<object,FinishedEventArgs> Finished
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_gaussianFinishedEventHandler == null)
           {
              _gaussianFinishedEventHandler += value;

              _gaussianFinishedEventCallbackDelegate = new FinishedEventCallbackDelegate(OnFinished);
              this.FinishedSignal().Connect(_gaussianFinishedEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_gaussianFinishedEventHandler != null)
           {
              this.FinishedSignal().Disconnect(_gaussianFinishedEventCallbackDelegate);
           }

           _gaussianFinishedEventHandler -= value;
        }
     }
  }

  // Callback for GaussianBlurView FinishedSignal
  private void OnFinished(IntPtr data)
  {
     FinishedEventArgs e = new FinishedEventArgs();

     // Populate all members of "e" (FinishedEventArgs) with real data
     e.GaussianBlurView = GaussianBlurView.GetGaussianBlurViewFromPtr(data);

     if (_gaussianFinishedEventHandler != null)
     {
        //here we send all data to user event handlers
        _gaussianFinishedEventHandler(this, e);
     }
  }

 public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

%}
%enddef

%define DALI_GAUSSIAN_BLURR_VIEW_EVENTHANDLER_PARAM( NameSpace, ClassName)
  GAUSSIAN_BLURR_VIEW_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  GAUSSIAN_BLURR_VIEW_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);
%enddef

namespace Dali
{
  DALI_GAUSSIAN_BLURR_VIEW_EVENTHANDLER_PARAM( Dali::Toolkit, GaussianBlurView);
}


