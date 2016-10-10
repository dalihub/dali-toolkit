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

%define SLIDER_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}
%enddef

%define SLIDER_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

public class ValueChangedEventArgs : EventArgs
{
   private Slider _slider;
   private float _slideValue;

   public Slider Slider
   {
      get
      {
         return _slider;
      }
      set
      {
         _slider = value;
      }
   }

   public float SlideValue
   {
      get
      {
         return _slideValue;
      }
      set
      {
         _slideValue = value;
      }
   }
}

public class SlidingFinishedEventArgs : EventArgs
{
   private Slider _slider;
   private float _slideValue;

   public Slider Slider
   {
      get
      {
         return _slider;
      }
      set
      {
         _slider = value;
      }
   }

   public float SlideValue
   {
      get
      {
         return _slideValue;
      }
      set
      {
         _slideValue = value;
      }
   }
}

public class MarkReachedEventArgs : EventArgs
{
   private Slider _slider;
   private int _slideValue;

   public Slider Slider
   {
      get
      {
         return _slider;
      }
      set
      {
         _slider = value;
      }
   }

   public int SlideValue
   {
      get
      {
         return _slideValue;
      }
      set
      {
         _slideValue = value;
      }
   }
}


  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate bool ValueChangedCallbackDelegate(IntPtr slider, float slideValue);
  private DaliEventHandlerWithReturnType<object,ValueChangedEventArgs,bool> _sliderValueChangedEventHandler;
  private ValueChangedCallbackDelegate _sliderValueChangedCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate bool SlidingFinishedCallbackDelegate(IntPtr slider, float slideValue);
  private DaliEventHandlerWithReturnType<object,SlidingFinishedEventArgs,bool> _sliderSlidingFinishedEventHandler;
  private SlidingFinishedCallbackDelegate _sliderSlidingFinishedCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate bool MarkReachedCallbackDelegate(IntPtr slider, int slideValue);
  private DaliEventHandlerWithReturnType<object,MarkReachedEventArgs,bool> _sliderMarkReachedEventHandler;
  private MarkReachedCallbackDelegate _sliderMarkReachedCallbackDelegate;

  public event DaliEventHandlerWithReturnType<object,ValueChangedEventArgs,bool> ValueChanged
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_sliderValueChangedEventHandler == null)
           {
              _sliderValueChangedEventHandler += value;

              _sliderValueChangedCallbackDelegate = new ValueChangedCallbackDelegate(OnValueChanged);
              this.ValueChangedSignal().Connect(_sliderValueChangedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_sliderValueChangedEventHandler != null)
           {
              this.ValueChangedSignal().Disconnect(_sliderValueChangedCallbackDelegate);
           }

           _sliderValueChangedEventHandler -= value;
        }
     }
  }

  // Callback for Slider ValueChanged signal
  private bool OnValueChanged(IntPtr slider, float slideValue)
  {
     ValueChangedEventArgs e = new ValueChangedEventArgs();

     // Populate all members of "e" (ValueChangedEventArgs) with real page
     e.Slider = Slider.GetSliderFromPtr( slider );
     e.SlideValue = slideValue;

     if (_sliderValueChangedEventHandler != null)
     {
        //here we send all page to user event handlers
      return _sliderValueChangedEventHandler(this, e, true);
     }
     return false;
  }

  public event DaliEventHandlerWithReturnType<object,SlidingFinishedEventArgs,bool> SlidingFinished
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_sliderSlidingFinishedEventHandler == null)
           {
              _sliderSlidingFinishedEventHandler += value;

              _sliderSlidingFinishedCallbackDelegate = new SlidingFinishedCallbackDelegate(OnSlidingFinished);
              this.SlidingFinishedSignal().Connect(_sliderSlidingFinishedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_sliderSlidingFinishedEventHandler != null)
           {
              this.SlidingFinishedSignal().Disconnect(_sliderSlidingFinishedCallbackDelegate);
           }

           _sliderSlidingFinishedEventHandler -= value;
        }
     }
  }

  // Callback for Slider SlidingFinished signal
  private bool OnSlidingFinished(IntPtr slider, float slideValue)
  {
     SlidingFinishedEventArgs e = new SlidingFinishedEventArgs();

     // Populate all members of "e" (SlidingFinishedEventArgs) with real page
     e.Slider = Slider.GetSliderFromPtr( slider );
     e.SlideValue = slideValue;

     if (_sliderSlidingFinishedEventHandler != null)
     {
        //here we send all page to user event handlers
      return _sliderSlidingFinishedEventHandler(this, e, true);
     }
     return false;
  }

  public event DaliEventHandlerWithReturnType<object,MarkReachedEventArgs,bool> MarkReached
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_sliderMarkReachedEventHandler == null)
           {
              _sliderMarkReachedEventHandler += value;

              _sliderMarkReachedCallbackDelegate = new MarkReachedCallbackDelegate(OnMarkReached);
              this.MarkReachedSignal().Connect(_sliderMarkReachedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_sliderMarkReachedEventHandler != null)
           {
              this.MarkReachedSignal().Disconnect(_sliderMarkReachedCallbackDelegate);
           }

           _sliderMarkReachedEventHandler -= value;
        }
     }
  }

  // Callback for Slider MarkReached signal
  private bool OnMarkReached(IntPtr slider, int slideValue)
  {
     MarkReachedEventArgs e = new MarkReachedEventArgs();

     // Populate all members of "e" (MarkReachedEventArgs) with real page
     e.Slider = Slider.GetSliderFromPtr( slider );
     e.SlideValue = slideValue;

     if (_sliderMarkReachedEventHandler != null)
     {
        //here we send all page to user event handlers
      return _sliderMarkReachedEventHandler(this, e, true);
     }
     return false;
  }

  public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

%}

%enddef


%define DALI_SLIDER_EVENTHANDLER_PARAM( NameSpace, ClassName)

  SLIDER_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  SLIDER_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_SLIDER_EVENTHANDLER_PARAM( Dali::Toolkit, Slider);
}
