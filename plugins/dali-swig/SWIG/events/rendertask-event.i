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

 private void OnFinished(IntPtr renderTask)
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

/* Properties earlier added by Ruby Script */

  public Dali.CSharp.Size ViewportPosition
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( RenderTask.Property.VIEWPORT_POSITION).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( RenderTask.Property.VIEWPORT_POSITION, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size ViewportSize
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( RenderTask.Property.VIEWPORT_SIZE).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( RenderTask.Property.VIEWPORT_SIZE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color ClearColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( RenderTask.Property.CLEAR_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( RenderTask.Property.CLEAR_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public bool RequiresSync
  {
    get
    {
      bool temp = false;
      GetProperty( RenderTask.Property.REQUIRES_SYNC).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( RenderTask.Property.REQUIRES_SYNC, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */

%}

%enddef

%define DALI_renderTask_EVENTHANDLER_PARAM(NameSpace, ClassName)

  RenderTask_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName);
  RenderTask_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_renderTask_EVENTHANDLER_PARAM(Dali, RenderTask);
}
