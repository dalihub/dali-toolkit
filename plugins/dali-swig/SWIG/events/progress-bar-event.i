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

%define PROGRESS_BAR_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}
%enddef

%define PROGRESS_BAR_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

public class ValueChangedEventArgs : EventArgs
{
   private ProgressBar _progressBar;
   private float _progressValue;
   private float _secondaryProgressValue;

   public ProgressBar ProgressBar
   {
      get
      {
         return _progressBar;
      }
      set
      {
         _progressBar = value;
      }
   }

   public float ProgressValue
   {
      get
      {
         return _progressValue;
      }
      set
      {
         _progressValue = value;
      }
   }

   public float SecondaryProgressValue
   {
      get
      {
         return _secondaryProgressValue;
      }
      set
      {
         _secondaryProgressValue = value;
      }
   }

}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void ValueChangedCallbackDelegate(IntPtr progressBar, float progressValue, float secondaryProgressValue);
  private DaliEventHandler<object,ValueChangedEventArgs> _progressBarValueChangedEventHandler;
  private ValueChangedCallbackDelegate _progressBarValueChangedCallbackDelegate;

  public event DaliEventHandler<object,ValueChangedEventArgs> ValueChanged
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_progressBarValueChangedEventHandler == null)
           {
              _progressBarValueChangedEventHandler += value;

              _progressBarValueChangedCallbackDelegate = new ValueChangedCallbackDelegate(OnValueChanged);
              this.ValueChangedSignal().Connect(_progressBarValueChangedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_progressBarValueChangedEventHandler != null)
           {
              this.ValueChangedSignal().Disconnect(_progressBarValueChangedCallbackDelegate);
           }

           _progressBarValueChangedEventHandler -= value;
        }
     }
  }

  // Callback for ProgressBar ValueChanged signal
  private void OnValueChanged(IntPtr progressBar, float progressValue, float secondaryProgressValue)
  {
     ValueChangedEventArgs e = new ValueChangedEventArgs();

     // Populate all members of "e" (ValueChangedEventArgs) with real page
     e.ProgressBar = ProgressBar.GetProgressBarFromPtr( progressBar );
     e.ProgressValue = progressValue;
     e.SecondaryProgressValue = secondaryProgressValue;

     if (_progressBarValueChangedEventHandler != null)
     {
       _progressBarValueChangedEventHandler(this, e);
     }
  }

  public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

%}

%enddef


%define DALI_PROGRESS_BAR_EVENTHANDLER_PARAM( NameSpace, ClassName)

  PROGRESS_BAR_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  PROGRESS_BAR_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_PROGRESS_BAR_EVENTHANDLER_PARAM( Dali::Toolkit, ProgressBar);
}
