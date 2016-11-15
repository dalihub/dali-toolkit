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

%define RenderTask_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}

%enddef

%define RenderTask_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{


public class FinishedEventArgs : EventArgs
{
   private RenderTask _renderTask;

   public RenderTask RenderTask
   {
      get
      {
         return _renderTask;
      }
      set
      {
         _renderTask = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void RenderTaskCallbackDelegate(IntPtr renderTask);
  private DaliEventHandler<object,FinishedEventArgs> _renderTaskEventHandler;
  private RenderTaskCallbackDelegate _renderTaskCallbackDelegate;


  public event DaliEventHandler<object,FinishedEventArgs> Finished
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_renderTaskEventHandler == null)
           {
              _renderTaskEventHandler += value;

              _renderTaskCallbackDelegate = new RenderTaskCallbackDelegate(OnFinished);
              this.FinishedSignal().Connect(_renderTaskCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_renderTaskEventHandler != null)
           {
              this.FinishedSignal().Disconnect(_renderTaskCallbackDelegate);
           }

           _renderTaskEventHandler -= value;
        }
     }
  }

 private void OnFinished(IntPtr actor, IntPtr renderTask)
  {
   FinishedEventArgs e = new FinishedEventArgs();

   // Populate all members of "e" (FinishedEventArgs) with real data
   e.RenderTask = Dali.RenderTask.GetRenderTaskFromPtr(renderTask);

   if (_renderTaskEventHandler != null)
   {
      //here we send all data to user event handlers
      _renderTaskEventHandler(this, e);
   }

  }


public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
   ClassName ret = new ClassName(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

%}

%enddef

%define DALI_renderTask_EVENTHANDLER_PARAM( NameSpace, ClassName)

  RenderTask_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  RenderTask_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_renderTask_EVENTHANDLER_PARAM( Dali, RenderTask);
}
