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

%define ResourceImage_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}
%enddef

%define ResourceImage_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

public class LoadingFinishedEventArgs : EventArgs
{
   private ResourceImage _resourceImage;

   public ResourceImage ResourceImage
   {
      get
      {
         return _resourceImage;
      }
      set
      {
         _resourceImage = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void LoadingFinishedEventCallbackDelegate(IntPtr ResourceImage);
  private DaliEventHandler<object,LoadingFinishedEventArgs> _resourceImageLoadingFinishedEventHandler;
  private LoadingFinishedEventCallbackDelegate _resourceImageLoadingFinishedEventCallbackDelegate;

  public event DaliEventHandler<object,LoadingFinishedEventArgs> LoadingFinished
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_resourceImageLoadingFinishedEventHandler == null)
           {
              _resourceImageLoadingFinishedEventHandler += value;

              _resourceImageLoadingFinishedEventCallbackDelegate = new LoadingFinishedEventCallbackDelegate(OnLoadingFinished);
              this.LoadingFinishedSignal().Connect(_resourceImageLoadingFinishedEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_resourceImageLoadingFinishedEventHandler != null)
           {
              this.LoadingFinishedSignal().Disconnect(_resourceImageLoadingFinishedEventCallbackDelegate);
           }

           _resourceImageLoadingFinishedEventHandler -= value;
        }
     }
  }

  // Callback for ResourceImage LoadingFinishedSignal
  private void OnLoadingFinished(IntPtr data)
  {
     LoadingFinishedEventArgs e = new LoadingFinishedEventArgs();

     // Populate all members of "e" (LoadingFinishedEventArgs) with real data
     e.ResourceImage = ResourceImage.GetResourceImageFromPtr(data);

     if (_resourceImageLoadingFinishedEventHandler != null)
     {
        //here we send all data to user event handlers
        _resourceImageLoadingFinishedEventHandler(this, e);
     }
  }


public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

%}

%enddef


%define DALI_resourceImage_EVENTHANDLER_PARAM( NameSpace, ClassName)
  ResourceImage_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  ResourceImage_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);
%enddef

namespace Dali
{
  DALI_resourceImage_EVENTHANDLER_PARAM( Dali, ResourceImage);
}
