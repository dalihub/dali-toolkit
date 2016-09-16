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

%define BUILDER_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
    using System;
    using System.Runtime.InteropServices;

%}

%enddef


%define BUILDER_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
  %typemap(cscode) NameSpace::ClassName %{

    public class QuitEventArgs : EventArgs
    {
    }

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate void QuitEventHandler(object source, QuitEventArgs e);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void QuitEventCallbackDelegate();
    private QuitEventHandler _builderQuitEventHandler;
    private QuitEventCallbackDelegate _builderQuitEventCallbackDelegate;

    public event QuitEventHandler Quit
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_builderQuitEventHandler == null)
          {
            _builderQuitEventHandler += value;

            _builderQuitEventCallbackDelegate = new QuitEventCallbackDelegate(OnQuit);
            this.QuitSignal().Connect(_builderQuitEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_builderQuitEventHandler != null)
          {
            this.QuitSignal().Disconnect(_builderQuitEventCallbackDelegate);
          }

          _builderQuitEventHandler -= value;
        }
      }
    }

    // Callback for Builder QuitSignal
    private void OnQuit()
    {
      QuitEventArgs e = new QuitEventArgs();

      if (_builderQuitEventHandler != null)
      {
        //here we send all data to user event handlers
        _builderQuitEventHandler(this, e);
      }
    }

    %}
    %enddef

%define DALI_BUILDER_EVENTHANDLER_PARAM( NameSpace, ClassName)
  BUILDER_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  BUILDER_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);
  %enddef

  namespace Dali
{
  DALI_BUILDER_EVENTHANDLER_PARAM( Dali::Toolkit, Builder);
}

