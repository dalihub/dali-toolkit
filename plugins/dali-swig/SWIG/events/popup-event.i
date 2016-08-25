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
  public delegate void OutsideTouchedEventHandler(object source, OutsideTouchedEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void ShowingEventHandler(object source, ShowingEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void ShownEventHandler(object source, ShownEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void HidingEventHandler(object source, HidingEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void HiddenEventHandler(object source, HiddenEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void OutsideTouchedEventCallbackDelegate();
  private OutsideTouchedEventHandler _popUpOutsideTouchedEventHandler;
  private OutsideTouchedEventCallbackDelegate _popUpOutsideTouchedEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void ShowingEventCallbackDelegate();
  private ShowingEventHandler _popUpShowingEventHandler;
  private ShowingEventCallbackDelegate _popUpShowingEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void ShownEventCallbackDelegate();
  private ShownEventHandler _popUpShownEventHandler;
  private ShownEventCallbackDelegate _popUpShownEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void HidingEventCallbackDelegate();
  private HidingEventHandler _popUpHidingEventHandler;
  private HidingEventCallbackDelegate _popUpHidingEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void HiddenEventCallbackDelegate();
  private HiddenEventHandler _popUpHiddenEventHandler;
  private HiddenEventCallbackDelegate _popUpHiddenEventCallbackDelegate;

  public event OutsideTouchedEventHandler OutsideTouched
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

  public event ShowingEventHandler Showing
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


  public event ShownEventHandler Shown
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

  public event HidingEventHandler Hiding
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

  public event HiddenEventHandler Hidden
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

