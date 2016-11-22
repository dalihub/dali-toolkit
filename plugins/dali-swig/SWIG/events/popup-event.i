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

%define POPUP_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}

%enddef


%define POPUP_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

  public class OutsideTouchedEventArgs : EventArgs
  {
  }

  public class ShowingEventArgs : EventArgs
  {
  }

  public class ShownEventArgs : EventArgs
  {
  }

  public class HidingEventArgs : EventArgs
  {
  }

  public class HiddenEventArgs : EventArgs
  {
  }

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void OutsideTouchedEventCallbackDelegate();
  private DaliEventHandler<object,OutsideTouchedEventArgs> _popUpOutsideTouchedEventHandler;
  private OutsideTouchedEventCallbackDelegate _popUpOutsideTouchedEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void ShowingEventCallbackDelegate();
  private DaliEventHandler<object,ShowingEventArgs> _popUpShowingEventHandler;
  private ShowingEventCallbackDelegate _popUpShowingEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void ShownEventCallbackDelegate();
  private DaliEventHandler<object,ShownEventArgs> _popUpShownEventHandler;
  private ShownEventCallbackDelegate _popUpShownEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void HidingEventCallbackDelegate();
  private DaliEventHandler<object,HidingEventArgs> _popUpHidingEventHandler;
  private HidingEventCallbackDelegate _popUpHidingEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void HiddenEventCallbackDelegate();
  private DaliEventHandler<object,HiddenEventArgs> _popUpHiddenEventHandler;
  private HiddenEventCallbackDelegate _popUpHiddenEventCallbackDelegate;

  public event DaliEventHandler<object,OutsideTouchedEventArgs> OutsideTouched
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_popUpOutsideTouchedEventHandler == null)
           {
              _popUpOutsideTouchedEventHandler += value;

              _popUpOutsideTouchedEventCallbackDelegate = new OutsideTouchedEventCallbackDelegate(OnOutsideTouched);
              this.OutsideTouchedSignal().Connect(_popUpOutsideTouchedEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_popUpOutsideTouchedEventHandler != null)
           {
              this.OutsideTouchedSignal().Disconnect(_popUpOutsideTouchedEventCallbackDelegate);
           }

           _popUpOutsideTouchedEventHandler -= value;
        }
     }
  }

  // Callback for Popup OutsideTouchedSignal
  private void OnOutsideTouched()
  {
     OutsideTouchedEventArgs e = new OutsideTouchedEventArgs();

     if (_popUpOutsideTouchedEventHandler != null)
     {
        //here we send all data to user event handlers
        _popUpOutsideTouchedEventHandler(this, e);
     }
  }

  public event DaliEventHandler<object,ShowingEventArgs> Showing
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_popUpShowingEventHandler == null)
           {
              _popUpShowingEventHandler += value;

              _popUpShowingEventCallbackDelegate = new ShowingEventCallbackDelegate(OnShowing);
              this.ShowingSignal().Connect(_popUpShowingEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_popUpShowingEventHandler != null)
           {
              this.ShowingSignal().Disconnect(_popUpShowingEventCallbackDelegate);
           }

           _popUpShowingEventHandler -= value;
        }
     }
  }

  // Callback for ShowingSignal
  private void OnShowing()
  {
     ShowingEventArgs e = new ShowingEventArgs();

     if (_popUpShowingEventHandler != null)
     {
        //here we send all data to user event handlers
        _popUpShowingEventHandler(this, e);
     }
  }


  public event DaliEventHandler<object,ShownEventArgs> Shown
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_popUpShownEventHandler == null)
           {
              _popUpShownEventHandler += value;

              _popUpShownEventCallbackDelegate = new ShownEventCallbackDelegate(OnShown);
              this.ShownSignal().Connect(_popUpShownEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_popUpShownEventHandler != null)
           {
              this.ShownSignal().Disconnect(_popUpShownEventCallbackDelegate);
           }

           _popUpShownEventHandler -= value;
        }
     }
  }

  // Callback for ShownSignal
  private void OnShown()
  {
     ShownEventArgs e = new ShownEventArgs();

     if (_popUpShownEventHandler != null)
     {
        //here we send all data to user event handlers
        _popUpShownEventHandler(this, e);
     }
  }

  public event DaliEventHandler<object,HidingEventArgs> Hiding
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_popUpHidingEventHandler == null)
           {
              _popUpHidingEventHandler += value;

              _popUpHidingEventCallbackDelegate = new HidingEventCallbackDelegate(OnHiding);
              this.HidingSignal().Connect(_popUpHidingEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_popUpHidingEventHandler != null)
           {
              this.HidingSignal().Disconnect(_popUpHidingEventCallbackDelegate);
           }

           _popUpHidingEventHandler -= value;
        }
     }
  }

  // Callback for HidingSignal
  private void OnHiding()
  {
     HidingEventArgs e = new HidingEventArgs();

     if (_popUpHidingEventHandler != null)
     {
        //here we send all data to user event handlers
        _popUpHidingEventHandler(this, e);
     }
  }

  public event DaliEventHandler<object,HiddenEventArgs> Hidden
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_popUpHiddenEventHandler == null)
           {
              _popUpHiddenEventHandler += value;

              _popUpHiddenEventCallbackDelegate = new HiddenEventCallbackDelegate(OnHidden);
              this.HiddenSignal().Connect(_popUpHiddenEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_popUpHiddenEventHandler != null)
           {
              this.HiddenSignal().Disconnect(_popUpHiddenEventCallbackDelegate);
           }

           _popUpHiddenEventHandler -= value;
        }
     }
  }

  // Callback for HiddenSignal
  private void OnHidden()
  {
     HiddenEventArgs e = new HiddenEventArgs();

     if (_popUpHiddenEventHandler != null)
     {
        //here we send all data to user event handlers
        _popUpHiddenEventHandler(this, e);
     }
  }

/* Properties earlier added by Ruby Script */

  public Dali.Property.Map Title
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( Popup.Property.TITLE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.TITLE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map Content
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( Popup.Property.CONTENT).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.CONTENT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map Footer
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( Popup.Property.FOOTER).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.FOOTER, new Dali.Property.Value( value ) );
    }
  }
  public string DisplayState
  {
    get
    {
      string temp;
      GetProperty( Popup.Property.DISPLAY_STATE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.DISPLAY_STATE, new Dali.Property.Value( value ) );
    }
  }
  public bool TouchTransparent
  {
    get
    {
      bool temp = false;
      GetProperty( Popup.Property.TOUCH_TRANSPARENT).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.TOUCH_TRANSPARENT, new Dali.Property.Value( value ) );
    }
  }
  public bool TailVisibility
  {
    get
    {
      bool temp = false;
      GetProperty( Popup.Property.TAIL_VISIBILITY).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.TAIL_VISIBILITY, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Position TailPosition
  {
    get
    {
      Vector3 temp = new Vector3(0.0f,0.0f,0.0f);
      GetProperty( Popup.Property.TAIL_POSITION).Get(  temp );
      Dali.CSharp.Position ret = new Dali.CSharp.Position(temp.x, temp.y, temp.z);
      return ret;
    }
    set
    {
      SetProperty( Popup.Property.TAIL_POSITION, new Dali.Property.Value( value ) );
    }
  }
  public string ContextualMode
  {
    get
    {
      string temp;
      GetProperty( Popup.Property.CONTEXTUAL_MODE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.CONTEXTUAL_MODE, new Dali.Property.Value( value ) );
    }
  }
  public float AnimationDuration
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Popup.Property.ANIMATION_DURATION).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.ANIMATION_DURATION, new Dali.Property.Value( value ) );
    }
  }
  public string AnimationMode
  {
    get
    {
      string temp;
      GetProperty( Popup.Property.ANIMATION_MODE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.ANIMATION_MODE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map EntryAnimation
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( Popup.Property.ENTRY_ANIMATION).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.ENTRY_ANIMATION, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map ExitAnimation
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( Popup.Property.EXIT_ANIMATION).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.EXIT_ANIMATION, new Dali.Property.Value( value ) );
    }
  }
  public int AutoHideDelay
  {
    get
    {
      int temp = 0;
      GetProperty( Popup.Property.AUTO_HIDE_DELAY).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.AUTO_HIDE_DELAY, new Dali.Property.Value( value ) );
    }
  }
  public bool BackingEnabled
  {
    get
    {
      bool temp = false;
      GetProperty( Popup.Property.BACKING_ENABLED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.BACKING_ENABLED, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color BackingColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( Popup.Property.BACKING_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( Popup.Property.BACKING_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public string PopupBackgroundImage
  {
    get
    {
      string temp;
      GetProperty( Popup.Property.POPUP_BACKGROUND_IMAGE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.POPUP_BACKGROUND_IMAGE, new Dali.Property.Value( value ) );
    }
  }
  public string TailUpImage
  {
    get
    {
      string temp;
      GetProperty( Popup.Property.TAIL_UP_IMAGE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.TAIL_UP_IMAGE, new Dali.Property.Value( value ) );
    }
  }
  public string TailDownImage
  {
    get
    {
      string temp;
      GetProperty( Popup.Property.TAIL_DOWN_IMAGE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.TAIL_DOWN_IMAGE, new Dali.Property.Value( value ) );
    }
  }
  public string TailLeftImage
  {
    get
    {
      string temp;
      GetProperty( Popup.Property.TAIL_LEFT_IMAGE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.TAIL_LEFT_IMAGE, new Dali.Property.Value( value ) );
    }
  }
  public string TailRightImage
  {
    get
    {
      string temp;
      GetProperty( Popup.Property.TAIL_RIGHT_IMAGE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Popup.Property.TAIL_RIGHT_IMAGE, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */

%}
%enddef

%define DALI_POPUP_EVENTHANDLER_PARAM( NameSpace, ClassName)
  POPUP_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  POPUP_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);
%enddef

namespace Dali
{
  DALI_POPUP_EVENTHANDLER_PARAM( Dali::Toolkit, Popup);
}

