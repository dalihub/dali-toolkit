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
      return _sliderValueChangedEventHandler(this, e);
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
      return _sliderSlidingFinishedEventHandler(this, e);
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
      return _sliderMarkReachedEventHandler(this, e);
     }
     return false;
  }

  public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

/* Properties earlier added by Ruby Script */

  public float LowerBound
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Slider.Property.LOWER_BOUND).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Slider.Property.LOWER_BOUND, new Dali.Property.Value( value ) );
    }
  }
  public float UpperBound
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Slider.Property.UPPER_BOUND).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Slider.Property.UPPER_BOUND, new Dali.Property.Value( value ) );
    }
  }
  public float Value
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Slider.Property.VALUE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Slider.Property.VALUE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map TrackVisual
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( Slider.Property.TRACK_VISUAL).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( Slider.Property.TRACK_VISUAL, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map HandleVisual
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( Slider.Property.HANDLE_VISUAL).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( Slider.Property.HANDLE_VISUAL, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map ProgressVisual
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( Slider.Property.PROGRESS_VISUAL).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( Slider.Property.PROGRESS_VISUAL, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map PopupVisual
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( Slider.Property.POPUP_VISUAL).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( Slider.Property.POPUP_VISUAL, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map PopupArrowVisual
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( Slider.Property.POPUP_ARROW_VISUAL).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( Slider.Property.POPUP_ARROW_VISUAL, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color DisabledColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( Slider.Property.DISABLED_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( Slider.Property.DISABLED_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public int ValuePrecision
  {
    get
    {
      int temp = 0;
      GetProperty( Slider.Property.VALUE_PRECISION).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Slider.Property.VALUE_PRECISION, new Dali.Property.Value( value ) );
    }
  }
  public bool ShowPopup
  {
    get
    {
      bool temp = false;
      GetProperty( Slider.Property.SHOW_POPUP).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Slider.Property.SHOW_POPUP, new Dali.Property.Value( value ) );
    }
  }
  public bool ShowValue
  {
    get
    {
      bool temp = false;
      GetProperty( Slider.Property.SHOW_VALUE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Slider.Property.SHOW_VALUE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Array Marks
  {
    get
    {
      Dali.Property.Array temp = new Dali.Property.Array();
      GetProperty( Slider.Property.MARKS).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( Slider.Property.MARKS, new Dali.Property.Value( value ) );
    }
  }
  public bool SnapToMarks
  {
    get
    {
      bool temp = false;
      GetProperty( Slider.Property.SNAP_TO_MARKS).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Slider.Property.SNAP_TO_MARKS, new Dali.Property.Value( value ) );
    }
  }
  public float MarkTolerance
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Slider.Property.MARK_TOLERANCE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Slider.Property.MARK_TOLERANCE, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */

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
