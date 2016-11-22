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

%define VIDEOVIEW_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}
%enddef

%define VIDEOVIEW_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/**
  * @brief Event arguments that passed via Finished signal
  *
  */
public class FinishedEventArgs : EventArgs
{
   private VideoView _videoView;

   /**
     * @brief VideoView - VideoView is a control for video playback and display.
     *
     */
   public VideoView VideoView
   {
      get
      {
         return _videoView;
      }
      set
      {
         _videoView = value;
      }
   }
}


  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void FinishedCallbackDelegate(IntPtr data);
  private DaliEventHandler<object,FinishedEventArgs> _videoViewFinishedEventHandler;
  private FinishedCallbackDelegate _videoViewFinishedCallbackDelegate;

  /**
    * @brief Event for Finished signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of FinishedEventHandler-DaliEventHandler<object,FinishedEventArgs>) provided by the user.
    * Finished signal is emitted when a video playback have finished.
    */
  public event DaliEventHandler<object,FinishedEventArgs> Finished
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_videoViewFinishedEventHandler == null)
           {
              _videoViewFinishedEventHandler += value;

              _videoViewFinishedCallbackDelegate = new FinishedCallbackDelegate(OnFinished);
              this.FinishedSignal().Connect(_videoViewFinishedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_videoViewFinishedEventHandler != null)
           {
              this.FinishedSignal().Disconnect(_videoViewFinishedCallbackDelegate);
           }

           _videoViewFinishedEventHandler -= value;
        }
     }
  }

  // Callback for VideoView Finished signal
  private void OnFinished(IntPtr data)
  {
     FinishedEventArgs e = new FinishedEventArgs();

     // Populate all members of "e" (FinishedEventArgs) with real data
     e.VideoView = VideoView.GetVideoViewFromPtr( data );

     if (_videoViewFinishedEventHandler != null)
     {
        //here we send all data to user event handlers
        _videoViewFinishedEventHandler(this, e);
     }
  }

  public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

/* Properties earlier added by Ruby Script */

  public Dali.Property.Map Video
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( VideoView.Property.VIDEO).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( VideoView.Property.VIDEO, new Dali.Property.Value( value ) );
    }
  }
  public bool Looping
  {
    get
    {
      bool temp = false;
      GetProperty( VideoView.Property.LOOPING).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( VideoView.Property.LOOPING, new Dali.Property.Value( value ) );
    }
  }
  public bool Muted
  {
    get
    {
      bool temp = false;
      GetProperty( VideoView.Property.MUTED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( VideoView.Property.MUTED, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map Volume
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( VideoView.Property.VOLUME).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( VideoView.Property.VOLUME, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */

%}

%enddef


%define DALI_VIDEOVIEW_EVENTHANDLER_PARAM( NameSpace, ClassName)

  VIDEOVIEW_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  VIDEOVIEW_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_VIDEOVIEW_EVENTHANDLER_PARAM( Dali::Toolkit, VideoView);
}
