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

%define ITEMVIEW_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}
%enddef


%define ITEMVIEW_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

  public class LayoutActivatedEventArgs : EventArgs
  {
  }

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void LayoutActivatedEventCallbackDelegate();
  private DaliEventHandler<object,LayoutActivatedEventArgs> _itemViewLayoutActivatedEventHandler;
  private LayoutActivatedEventCallbackDelegate _itemViewLayoutActivatedEventCallbackDelegate;

  public event DaliEventHandler<object,LayoutActivatedEventArgs> LayoutActivated
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_itemViewLayoutActivatedEventHandler == null)
           {
              _itemViewLayoutActivatedEventHandler += value;

              _itemViewLayoutActivatedEventCallbackDelegate = new LayoutActivatedEventCallbackDelegate(OnLayoutActivated);
              this.LayoutActivatedSignal().Connect(_itemViewLayoutActivatedEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_itemViewLayoutActivatedEventHandler != null)
           {
              this.LayoutActivatedSignal().Disconnect(_itemViewLayoutActivatedEventCallbackDelegate);
           }

           _itemViewLayoutActivatedEventHandler -= value;
        }
     }
  }

  // Callback for ItemView LayoutActivatedSignal
  private void OnLayoutActivated()
  {
     LayoutActivatedEventArgs e = new LayoutActivatedEventArgs();

     if (_itemViewLayoutActivatedEventHandler != null)
     {
        //here we send all data to user event handlers
        _itemViewLayoutActivatedEventHandler(this, e);
     }
  }

/* Properties earlier added by Ruby Script */

  public float MinimumSwipeSpeed
  {
    get
    {
      float temp = 0.0f;
      GetProperty( ItemView.Property.MINIMUM_SWIPE_SPEED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ItemView.Property.MINIMUM_SWIPE_SPEED, new Dali.Property.Value( value ) );
    }
  }
  public float MinimumSwipeDistance
  {
    get
    {
      float temp = 0.0f;
      GetProperty( ItemView.Property.MINIMUM_SWIPE_DISTANCE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ItemView.Property.MINIMUM_SWIPE_DISTANCE, new Dali.Property.Value( value ) );
    }
  }
  public float WheelScrollDistanceStep
  {
    get
    {
      float temp = 0.0f;
      GetProperty( ItemView.Property.WHEEL_SCROLL_DISTANCE_STEP).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ItemView.Property.WHEEL_SCROLL_DISTANCE_STEP, new Dali.Property.Value( value ) );
    }
  }
  public bool SnapToItemEnabled
  {
    get
    {
      bool temp = false;
      GetProperty( ItemView.Property.SNAP_TO_ITEM_ENABLED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ItemView.Property.SNAP_TO_ITEM_ENABLED, new Dali.Property.Value( value ) );
    }
  }
  public float RefreshInterval
  {
    get
    {
      float temp = 0.0f;
      GetProperty( ItemView.Property.REFRESH_INTERVAL).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ItemView.Property.REFRESH_INTERVAL, new Dali.Property.Value( value ) );
    }
  }
  public float LayoutPosition
  {
    get
    {
      float temp = 0.0f;
      GetProperty( ItemView.Property.LAYOUT_POSITION).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ItemView.Property.LAYOUT_POSITION, new Dali.Property.Value( value ) );
    }
  }
  public float ScrollSpeed
  {
    get
    {
      float temp = 0.0f;
      GetProperty( ItemView.Property.SCROLL_SPEED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ItemView.Property.SCROLL_SPEED, new Dali.Property.Value( value ) );
    }
  }
  public float Overshoot
  {
    get
    {
      float temp = 0.0f;
      GetProperty( ItemView.Property.OVERSHOOT).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ItemView.Property.OVERSHOOT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size ScrollDirection
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( ItemView.Property.SCROLL_DIRECTION).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( ItemView.Property.SCROLL_DIRECTION, new Dali.Property.Value( value ) );
    }
  }
  public int LayoutOrientation
  {
    get
    {
      int temp = 0;
      GetProperty( ItemView.Property.LAYOUT_ORIENTATION).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ItemView.Property.LAYOUT_ORIENTATION, new Dali.Property.Value( value ) );
    }
  }
  public float ScrollContentSize
  {
    get
    {
      float temp = 0.0f;
      GetProperty( ItemView.Property.SCROLL_CONTENT_SIZE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ItemView.Property.SCROLL_CONTENT_SIZE, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */

%}
%enddef

%define DALI_ITEMVIEW_EVENTHANDLER_PARAM( NameSpace, ClassName)
  ITEMVIEW_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  ITEMVIEW_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);
%enddef

namespace Dali
{
  DALI_ITEMVIEW_EVENTHANDLER_PARAM( Dali::Toolkit, ItemView);
}

