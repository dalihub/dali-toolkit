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

