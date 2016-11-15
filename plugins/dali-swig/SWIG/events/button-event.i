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

// Example from Swig MACRO

%define BUTTON_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}

%enddef

%define BUTTON_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

public class ClickedEventArgs : EventArgs
{
   private Button _button;

   public Button Button
   {
      get
      {
         return _button;
      }
      set
      {
         _button = value;
      }
   }
}

public class PressedEventArgs : EventArgs
{
   private Button _button;

   public Button Button
   {
      get
      {
         return _button;
      }
      set
      {
         _button = value;
      }
   }
}

public class ReleasedEventArgs : EventArgs
{
   private Button _button;

   public Button Button
   {
      get
      {
         return _button;
      }
      set
      {
         _button = value;
      }
   }
}

public class StateChangedEventArgs : EventArgs
{
   private Button _button;

   public Button Button
   {
      get
      {
         return _button;
      }
      set
      {
         _button = value;
      }
   }
}


  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate bool ClickedCallbackDelegate(global::System.IntPtr data);
  private DaliEventHandlerWithReturnType<object,ClickedEventArgs,bool> _buttonClickedEventHandler;
  private ClickedCallbackDelegate _buttonClickedCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate bool PressedCallbackDelegate(global::System.IntPtr data);
  private DaliEventHandlerWithReturnType<object,PressedEventArgs,bool> _buttonPressedEventHandler;
  private PressedCallbackDelegate _buttonPressedCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate bool ReleasedCallbackDelegate(global::System.IntPtr data);
  private DaliEventHandlerWithReturnType<object,ReleasedEventArgs,bool>  _buttonReleasedEventHandler;
  private ReleasedCallbackDelegate _buttonReleasedCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate bool StateChangedCallbackDelegate(global::System.IntPtr data);
  private DaliEventHandlerWithReturnType<object,StateChangedEventArgs,bool> _buttonStateChangedEventHandler;
  private StateChangedCallbackDelegate _buttonStateChangedCallbackDelegate;


  public event DaliEventHandlerWithReturnType<object,ClickedEventArgs,bool> Clicked
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_buttonClickedEventHandler == null)
           {
              _buttonClickedEventHandler += value;

              _buttonClickedCallbackDelegate = new ClickedCallbackDelegate(OnClicked);
              this.ClickedSignal().Connect(_buttonClickedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_buttonClickedEventHandler != null)
           {
               this.ClickedSignal().Disconnect(_buttonClickedCallbackDelegate);
           }

           _buttonClickedEventHandler -= value;
        }
     }
  }

  // Callback for button click signal
  private bool OnClicked (IntPtr data)
  {
     ClickedEventArgs e = new ClickedEventArgs();

     e.Button = Button.GetButtonFromPtr(data);

     if (_buttonClickedEventHandler != null)
     {
        //here we send all data to user event handlers
        return _buttonClickedEventHandler(this, e);
     }
     return false;
  }


  public event DaliEventHandlerWithReturnType<object,PressedEventArgs,bool> Pressed
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_buttonPressedEventHandler == null)
           {
              _buttonPressedEventHandler += value;

              _buttonPressedCallbackDelegate = new PressedCallbackDelegate(OnPressed);
              this.PressedSignal().Connect(_buttonPressedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_buttonPressedEventHandler != null)
           {
               this.PressedSignal().Disconnect(_buttonPressedCallbackDelegate);
           }

           _buttonPressedEventHandler -= value;
        }
     }
  }

  // Callback for button click signal
  private bool OnPressed (IntPtr data)
  {
     PressedEventArgs e = new PressedEventArgs();

     e.Button = Button.GetButtonFromPtr(data);

     if (_buttonPressedEventHandler != null)
     {
        //here we send all data to user event handlers
       return _buttonPressedEventHandler(this, e);
     }
     return false;
  }


  public event DaliEventHandlerWithReturnType<object,ReleasedEventArgs,bool> Released
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_buttonReleasedEventHandler == null)
           {
              _buttonReleasedEventHandler += value;

              _buttonReleasedCallbackDelegate = new ReleasedCallbackDelegate(OnReleased);
              this.ReleasedSignal().Connect(_buttonReleasedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_buttonReleasedEventHandler != null)
           {
               this.ReleasedSignal().Disconnect(_buttonReleasedCallbackDelegate);
           }

           _buttonReleasedEventHandler -= value;
        }
     }
  }

  // Callback for button click signal
  private bool OnReleased (IntPtr data)
  {
     ReleasedEventArgs e = new ReleasedEventArgs();

     e.Button = Button.GetButtonFromPtr(data);

     if (_buttonReleasedEventHandler != null)
     {
        //here we send all data to user event handlers
       return _buttonReleasedEventHandler(this, e);
     }
     return false;
  }


  public event DaliEventHandlerWithReturnType<object,StateChangedEventArgs,bool> StateChanged
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_buttonStateChangedEventHandler == null)
           {
              _buttonStateChangedEventHandler += value;

              _buttonStateChangedCallbackDelegate = new StateChangedCallbackDelegate(OnStateChanged);
              this.StateChangedSignal().Connect(_buttonStateChangedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_buttonStateChangedEventHandler != null)
           {
               this.StateChangedSignal().Disconnect(_buttonStateChangedCallbackDelegate);
           }

           _buttonStateChangedEventHandler -= value;
        }
     }
  }

  // Callback for button click signal
  private bool OnStateChanged (IntPtr data)
  {
     StateChangedEventArgs e = new StateChangedEventArgs();

     e.Button = Button.GetButtonFromPtr(data);

     if (_buttonStateChangedEventHandler != null)
     {
        //here we send all data to user event handlers
       return _buttonStateChangedEventHandler(this, e);
     }
     return false;
  }

 public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }


/* Properties earlier added by Ruby Script */

  public bool Disabled
  {
    get
    {
      bool temp = false;
      GetProperty( Button.Property.DISABLED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Button.Property.DISABLED, new Dali.Property.Value( value ) );
    }
  }
  public bool AutoRepeating
  {
    get
    {
      bool temp = false;
      GetProperty( Button.Property.AUTO_REPEATING).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Button.Property.AUTO_REPEATING, new Dali.Property.Value( value ) );
    }
  }
  public float InitialAutoRepeatingDelay
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Button.Property.INITIAL_AUTO_REPEATING_DELAY).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Button.Property.INITIAL_AUTO_REPEATING_DELAY, new Dali.Property.Value( value ) );
    }
  }
  public float NextAutoRepeatingDelay
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Button.Property.NEXT_AUTO_REPEATING_DELAY).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Button.Property.NEXT_AUTO_REPEATING_DELAY, new Dali.Property.Value( value ) );
    }
  }
  public bool Togglable
  {
    get
    {
      bool temp = false;
      GetProperty( Button.Property.TOGGLABLE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Button.Property.TOGGLABLE, new Dali.Property.Value( value ) );
    }
  }
  public bool Selected
  {
    get
    {
      bool temp = false;
      GetProperty( Button.Property.SELECTED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Button.Property.SELECTED, new Dali.Property.Value( value ) );
    }
  }
  public string UnselectedStateImage
  {
    get
    {
      string temp;
      GetProperty( Button.Property.UNSELECTED_STATE_IMAGE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Button.Property.UNSELECTED_STATE_IMAGE, new Dali.Property.Value( value ) );
    }
  }
  public string SelectedStateImage
  {
    get
    {
      string temp;
      GetProperty( Button.Property.SELECTED_STATE_IMAGE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Button.Property.SELECTED_STATE_IMAGE, new Dali.Property.Value( value ) );
    }
  }
  public string DisabledStateImage
  {
    get
    {
      string temp;
      GetProperty( Button.Property.DISABLED_STATE_IMAGE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Button.Property.DISABLED_STATE_IMAGE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color UnselectedColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( Button.Property.UNSELECTED_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( Button.Property.UNSELECTED_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color SelectedColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( Button.Property.SELECTED_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( Button.Property.SELECTED_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map Label
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( Button.Property.LABEL).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( Button.Property.LABEL, new Dali.Property.Value( value ) );
    }
  }
  public string LabelText
  {
    get
    {
      string temp;
      GetProperty( Button.Property.LABEL_TEXT).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Button.Property.LABEL_TEXT, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */

%}
%enddef


%define BUTTON_EVENTHANDLER_PARAM( NameSpace, ClassName)

  BUTTON_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  BUTTON_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  BUTTON_EVENTHANDLER_PARAM( Dali::Toolkit, Button);

} // namespace DALi




