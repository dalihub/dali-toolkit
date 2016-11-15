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

%define SCROLLABLE_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}
%enddef

%define SCROLLABLE_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

public class StartedEventArgs : EventArgs
{
   private Vector2 _vector2;

   public Vector2 Vector2
   {
      get
      {
         return _vector2;
      }
      set
      {
         _vector2 = value;
      }
   }
}

public class UpdatedEventArgs : EventArgs
{
   private Vector2 _vector2;

   public Vector2 Vector2
   {
      get
      {
         return _vector2;
      }
      set
      {
         _vector2 = value;
      }
   }
}

public class CompletedEventArgs : EventArgs
{
   private Vector2 _vector2;

   public Vector2 Vector2
   {
      get
      {
         return _vector2;
      }
      set
      {
         _vector2 = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void StartedCallbackDelegate(IntPtr vector2);
  private DaliEventHandler<object,StartedEventArgs> _scrollableStartedEventHandler;
  private StartedCallbackDelegate _scrollableStartedCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void UpdatedCallbackDelegate(IntPtr vector2);
  private DaliEventHandler<object,UpdatedEventArgs> _scrollableUpdatedEventHandler;
  private UpdatedCallbackDelegate _scrollableUpdatedCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void CompletedCallbackDelegate(IntPtr vector2);
  private DaliEventHandler<object,CompletedEventArgs> _scrollableCompletedEventHandler;
  private CompletedCallbackDelegate _scrollableCompletedCallbackDelegate;

  public event DaliEventHandler<object,StartedEventArgs> ScrollStarted
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_scrollableStartedEventHandler == null)
           {
              _scrollableStartedEventHandler += value;

              _scrollableStartedCallbackDelegate = new StartedCallbackDelegate(OnStarted);
              this.ScrollStartedSignal().Connect(_scrollableStartedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_scrollableStartedEventHandler != null)
           {
              this.ScrollStartedSignal().Disconnect(_scrollableStartedCallbackDelegate);
           }

           _scrollableStartedEventHandler -= value;
        }
     }
  }

 private void OnStarted(IntPtr vector2)
  {
   StartedEventArgs e = new StartedEventArgs();

   // Populate all members of "e" (StartedEventArgs) with real data
   e.Vector2 = Dali.Vector2.GetVector2FromPtr(vector2);

   if (_scrollableStartedEventHandler != null)
   {
      //here we send all data to user event handlers
      _scrollableStartedEventHandler(this, e);
   }

  }

  public event DaliEventHandler<object,UpdatedEventArgs> ScrollUpdated
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_scrollableUpdatedEventHandler == null)
           {
              _scrollableUpdatedEventHandler += value;

              _scrollableUpdatedCallbackDelegate = new UpdatedCallbackDelegate(OnUpdated);
              this.ScrollUpdatedSignal().Connect(_scrollableUpdatedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_scrollableUpdatedEventHandler != null)
           {
              this.ScrollUpdatedSignal().Disconnect(_scrollableUpdatedCallbackDelegate);
           }

           _scrollableUpdatedEventHandler -= value;
        }
     }
  }

 private void OnUpdated(IntPtr vector2)
  {
   UpdatedEventArgs e = new UpdatedEventArgs();

   // Populate all members of "e" (UpdatedEventArgs) with real data
   e.Vector2 = Dali.Vector2.GetVector2FromPtr(vector2);

   if (_scrollableUpdatedEventHandler != null)
   {
      //here we send all data to user event handlers
      _scrollableUpdatedEventHandler(this, e);
   }

  }

  public event DaliEventHandler<object,CompletedEventArgs> ScrollCompleted
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_scrollableCompletedEventHandler == null)
           {
              _scrollableCompletedEventHandler += value;

              _scrollableCompletedCallbackDelegate = new CompletedCallbackDelegate(OnCompleted);
              this.ScrollCompletedSignal().Connect(_scrollableCompletedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_scrollableCompletedEventHandler != null)
           {
              this.ScrollCompletedSignal().Disconnect(_scrollableCompletedCallbackDelegate);
           }

           _scrollableCompletedEventHandler -= value;
        }
     }
  }

 private void OnCompleted(IntPtr vector2)
  {
   CompletedEventArgs e = new CompletedEventArgs();

   // Populate all members of "e" (CompletedEventArgs) with real data
   e.Vector2 = Dali.Vector2.GetVector2FromPtr(vector2);

   if (_scrollableCompletedEventHandler != null)
   {
      //here we send all data to user event handlers
      _scrollableCompletedEventHandler(this, e);
   }

  }

/* Properties earlier added by Ruby Script */

  public Dali.CSharp.Color OvershootEffectColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( Scrollable.Property.OVERSHOOT_EFFECT_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( Scrollable.Property.OVERSHOOT_EFFECT_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public float OvershootAnimationSpeed
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Scrollable.Property.OVERSHOOT_ANIMATION_SPEED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Scrollable.Property.OVERSHOOT_ANIMATION_SPEED, new Dali.Property.Value( value ) );
    }
  }
  public bool OvershootEnabled
  {
    get
    {
      bool temp = false;
      GetProperty( Scrollable.Property.OVERSHOOT_ENABLED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Scrollable.Property.OVERSHOOT_ENABLED, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size OvershootSize
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( Scrollable.Property.OVERSHOOT_SIZE).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( Scrollable.Property.OVERSHOOT_SIZE, new Dali.Property.Value( value ) );
    }
  }
  public int ScrollToAlphaFunction
  {
    get
    {
      int temp = 0;
      GetProperty( Scrollable.Property.SCROLL_TO_ALPHA_FUNCTION).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Scrollable.Property.SCROLL_TO_ALPHA_FUNCTION, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size ScrollRelativePosition
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( Scrollable.Property.SCROLL_RELATIVE_POSITION).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( Scrollable.Property.SCROLL_RELATIVE_POSITION, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size ScrollPositionMin
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( Scrollable.Property.SCROLL_POSITION_MIN).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( Scrollable.Property.SCROLL_POSITION_MIN, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size ScrollPositionMax
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( Scrollable.Property.SCROLL_POSITION_MAX).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( Scrollable.Property.SCROLL_POSITION_MAX, new Dali.Property.Value( value ) );
    }
  }
  public bool CanScrollVertical
  {
    get
    {
      bool temp = false;
      GetProperty( Scrollable.Property.CAN_SCROLL_VERTICAL).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Scrollable.Property.CAN_SCROLL_VERTICAL, new Dali.Property.Value( value ) );
    }
  }
  public bool CanScrollHorizontal
  {
    get
    {
      bool temp = false;
      GetProperty( Scrollable.Property.CAN_SCROLL_HORIZONTAL).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Scrollable.Property.CAN_SCROLL_HORIZONTAL, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */

%}

%enddef

%define DALI_SCROLLABLE_EVENTHANDLER_PARAM( NameSpace, ClassName)

  SCROLLABLE_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  SCROLLABLE_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_SCROLLABLE_EVENTHANDLER_PARAM( Dali::Toolkit, Scrollable);
}
