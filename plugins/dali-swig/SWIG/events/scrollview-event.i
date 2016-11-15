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

%define SCROLLVIEW_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}
%enddef

%define SCROLLVIEW_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/**
  * @brief Event arguments that passed via SnapStarted signal
  *
  */
public class SnapStartedEventArgs : EventArgs
{
   private Dali.ScrollView.SnapEvent _snapEvent;

   /**
     * @brief SnapEvent - is the SnapEvent information like snap or flick (it tells the target position, scale, rotation for the snap or flick).
     *
     */
   public Dali.ScrollView.SnapEvent SnapEventInfo
   {
      get
      {
         return _snapEvent;
      }
      set
      {
         _snapEvent = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void SnapStartedCallbackDelegate(IntPtr data);
  private DaliEventHandler<object,SnapStartedEventArgs> _scrollViewSnapStartedEventHandler;
  private SnapStartedCallbackDelegate _scrollViewSnapStartedCallbackDelegate;

  /**
    * @brief Event for SnapStarted signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of SnapStartedEventHandler-DaliEventHandler<object,SnapStartedEventArgs>) provided by the user.
    * SnapStarted signal is emitted hen the ScrollView has started to snap or flick (it tells the target
    * position, scale, rotation for the snap or flick).
    *
    */
 public event DaliEventHandler<object,SnapStartedEventArgs> SnapStarted
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_scrollViewSnapStartedEventHandler == null)
           {
              _scrollViewSnapStartedEventHandler += value;

              _scrollViewSnapStartedCallbackDelegate = new SnapStartedCallbackDelegate(OnSnapStarted);
              this.SnapStartedSignal().Connect(_scrollViewSnapStartedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_scrollViewSnapStartedEventHandler != null)
           {
              this.SnapStartedSignal().Disconnect(_scrollViewSnapStartedCallbackDelegate);
           }

           _scrollViewSnapStartedEventHandler -= value;
        }
     }
  }

  // Callback for ScrollView SnapStarted signal
  private void OnSnapStarted(IntPtr data)
  {
     SnapStartedEventArgs e = new SnapStartedEventArgs();

     // Populate all members of "e" (SnapStartedEventArgs) with real data
     e.SnapEventInfo = SnapEvent.GetSnapEventFromPtr( data );

     if (_scrollViewSnapStartedEventHandler != null)
     {
        //here we send all data to user event handlers
        _scrollViewSnapStartedEventHandler(this, e);
     }
  }

/* Properties earlier added by Ruby Script */

  public bool WrapEnabled
  {
    get
    {
      bool temp = false;
      GetProperty( ScrollView.Property.WRAP_ENABLED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ScrollView.Property.WRAP_ENABLED, new Dali.Property.Value( value ) );
    }
  }
  public bool PanningEnabled
  {
    get
    {
      bool temp = false;
      GetProperty( ScrollView.Property.PANNING_ENABLED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ScrollView.Property.PANNING_ENABLED, new Dali.Property.Value( value ) );
    }
  }
  public bool AxisAutoLockEnabled
  {
    get
    {
      bool temp = false;
      GetProperty( ScrollView.Property.AXIS_AUTO_LOCK_ENABLED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ScrollView.Property.AXIS_AUTO_LOCK_ENABLED, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size WheelScrollDistanceStep
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( ScrollView.Property.WHEEL_SCROLL_DISTANCE_STEP).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( ScrollView.Property.WHEEL_SCROLL_DISTANCE_STEP, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size ScrollPosition
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( ScrollView.Property.SCROLL_POSITION).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( ScrollView.Property.SCROLL_POSITION, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size ScrollPrePosition
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( ScrollView.Property.SCROLL_PRE_POSITION).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( ScrollView.Property.SCROLL_PRE_POSITION, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size ScrollPrePositionMax
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( ScrollView.Property.SCROLL_PRE_POSITION_MAX).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( ScrollView.Property.SCROLL_PRE_POSITION_MAX, new Dali.Property.Value( value ) );
    }
  }
  public float OvershootX
  {
    get
    {
      float temp = 0.0f;
      GetProperty( ScrollView.Property.OVERSHOOT_X).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ScrollView.Property.OVERSHOOT_X, new Dali.Property.Value( value ) );
    }
  }
  public float OvershootY
  {
    get
    {
      float temp = 0.0f;
      GetProperty( ScrollView.Property.OVERSHOOT_Y).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ScrollView.Property.OVERSHOOT_Y, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size ScrollFinal
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( ScrollView.Property.SCROLL_FINAL).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( ScrollView.Property.SCROLL_FINAL, new Dali.Property.Value( value ) );
    }
  }
  public bool Wrap
  {
    get
    {
      bool temp = false;
      GetProperty( ScrollView.Property.WRAP).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ScrollView.Property.WRAP, new Dali.Property.Value( value ) );
    }
  }
  public bool Panning
  {
    get
    {
      bool temp = false;
      GetProperty( ScrollView.Property.PANNING).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ScrollView.Property.PANNING, new Dali.Property.Value( value ) );
    }
  }
  public bool Scrolling
  {
    get
    {
      bool temp = false;
      GetProperty( ScrollView.Property.SCROLLING).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ScrollView.Property.SCROLLING, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size ScrollDomainSize
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( ScrollView.Property.SCROLL_DOMAIN_SIZE).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( ScrollView.Property.SCROLL_DOMAIN_SIZE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size ScrollDomainOffset
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( ScrollView.Property.SCROLL_DOMAIN_OFFSET).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( ScrollView.Property.SCROLL_DOMAIN_OFFSET, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size ScrollPositionDelta
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( ScrollView.Property.SCROLL_POSITION_DELTA).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( ScrollView.Property.SCROLL_POSITION_DELTA, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Position StartPagePosition
  {
    get
    {
      Vector3 temp = new Vector3(0.0f,0.0f,0.0f);
      GetProperty( ScrollView.Property.START_PAGE_POSITION).Get(  temp );
      Dali.CSharp.Position ret = new Dali.CSharp.Position(temp.x, temp.y, temp.z);
      return ret;
    }
    set
    {
      SetProperty( ScrollView.Property.START_PAGE_POSITION, new Dali.Property.Value( value ) );
    }
  }


/* Properties ends */

%}

%enddef


%define DALI_SCROLLVIEW_EVENTHANDLER_PARAM( NameSpace, ClassName)

  SCROLLVIEW_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  SCROLLVIEW_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_SCROLLVIEW_EVENTHANDLER_PARAM( Dali::Toolkit, ScrollView);
}
