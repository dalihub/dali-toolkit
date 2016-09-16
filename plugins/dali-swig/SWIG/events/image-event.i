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

%define IMAGE_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}
%enddef

%define IMAGE_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{


/**
  * @brief Event arguments that passed via Uploaded signal
  *
  */
public class UploadedEventArgs : EventArgs
{
   private Image _image;
   /**
     * @brief Image - is the image data that gets uploaded to GL.
     *
     */
   public Image Image
   {
      get
      {
         return _image;
      }
      set
      {
         _image = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void UploadedEventHandler(object source, UploadedEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void UploadedEventCallbackDelegate(IntPtr image);
  private UploadedEventHandler _imageUploadedEventHandler;
  private UploadedEventCallbackDelegate _imageUploadedEventCallbackDelegate;

  /**
    * @brief Event for Uploaded signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of UploadedEventHandler) provided by the user.
    * Uploaded signal is emitted when the image data gets uploaded to GL.
    */
  public event UploadedEventHandler Uploaded
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_imageUploadedEventHandler == null)
           {
              _imageUploadedEventHandler += value;

              _imageUploadedEventCallbackDelegate = new UploadedEventCallbackDelegate(OnUploaded);
              this.UploadedSignal().Connect(_imageUploadedEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_imageUploadedEventHandler != null)
           {
              this.UploadedSignal().Disconnect(_imageUploadedEventCallbackDelegate);
           }

           _imageUploadedEventHandler -= value;
        }
     }
  }

  // Callback for Image UploadedSignal
  private void OnUploaded(IntPtr data)
  {
     UploadedEventArgs e = new UploadedEventArgs();

     // Populate all members of "e" (UploadedEventArgs) with real data
     e.Image = Image.GetImageFromPtr(data);

     if (_imageUploadedEventHandler != null)
     {
        //here we send all data to user event handlers
        _imageUploadedEventHandler(this, e);
     }
  }


public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

%}

%enddef


%define DALI_IMAGE_EVENTHANDLER_PARAM( NameSpace, ClassName)
  IMAGE_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  IMAGE_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);
%enddef

namespace Dali
{
  DALI_IMAGE_EVENTHANDLER_PARAM( Dali, Image);
}
