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

%define Animation_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
  %typemap(csimports) NameSpace::ClassName %{
    using System;
    using System.Runtime.InteropServices;

    %}
    %enddef

%define Animation_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
  %typemap(cscode) NameSpace::ClassName %{


    /**
      * @brief Event arguments that passed via Finished signal
      *
      */
    public class FinishedEventArgs : EventArgs
    {
      private Animation _animation;

      /**
        * @brief Animation - is the Animation which has finished with the animation.
        *
        */
      public Animation Animation
      {
        get
        {
          return _animation;
        }
        set
        {
          _animation = value;
        }
      }
    }

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate void FinishedEventHandler(object source, FinishedEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void FinishedEventCallbackDelegate(IntPtr Animation);
    private FinishedEventHandler _animationFinishedEventHandler;
    private FinishedEventCallbackDelegate _animationFinishedEventCallbackDelegate;

    /**
      * @brief Event for Finished signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of FinishedEventHandler) provided by the user.
      * Finished signal is emitted when an Animation's animations have finished.
      */
    public event FinishedEventHandler Finished
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_animationFinishedEventHandler == null)
          {
            _animationFinishedEventHandler += value;

            _animationFinishedEventCallbackDelegate = new FinishedEventCallbackDelegate(OnFinished);
            this.FinishedSignal().Connect(_animationFinishedEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_animationFinishedEventHandler != null)
          {
            this.FinishedSignal().Disconnect(_animationFinishedEventCallbackDelegate);
          }

          _animationFinishedEventHandler -= value;
        }
      }
    }

    // Callback for Animation FinishedSignal
    private void OnFinished(IntPtr data)
    {
      FinishedEventArgs e = new FinishedEventArgs();

      // Populate all members of "e" (FinishedEventArgs) with real data
      e.Animation = Animation.GetAnimationFromPtr(data);

      if (_animationFinishedEventHandler != null)
      {
        //here we send all data to user event handlers
        _animationFinishedEventHandler(this, e);
      }
    }


    public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
      ClassName ret = new ClassName(cPtr, false);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    }

    %}

    %enddef


%define DALI_animation_EVENTHANDLER_PARAM( NameSpace, ClassName)
  Animation_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  Animation_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);
  %enddef

  namespace Dali
{
  DALI_animation_EVENTHANDLER_PARAM( Dali, Animation);
}
