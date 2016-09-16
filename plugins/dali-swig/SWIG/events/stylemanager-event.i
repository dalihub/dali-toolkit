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

%define STYLEMANAGER_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;
%}
%enddef

%define STYLEMANAGER_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{


/**
  * @brief Event arguments that passed via StyleChanged signal
  *
  */
public class StyleChangedEventArgs : EventArgs
{
   private StyleManager _styleManager;
   private Dali.StyleChangeType _styleChange;

   /**
     * @brief StyleManager - is the StyleManager that informs applications of system theme change,
     * and supports application theme change at runtime.
     *
     */
   public StyleManager StyleManager
   {
      get
      {
         return _styleManager;
      }
      set
      {
         _styleManager = value;
      }
   }

   /**
     * @brief StyleChange - contains Style change information (default font changed or
     * default font size changed or theme has changed)
     *
     */
   public Dali.StyleChangeType StyleChange
   {
      get
      {
         return _styleChange;
      }
      set
      {
         _styleChange = value;
      }
   }

}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void StyleChangedEventHandler(object source, StyleChangedEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void StyleChangedCallbackDelegate(IntPtr styleManager, Dali.StyleChangeType styleChange);
  private StyleChangedEventHandler _styleManagerStyleChangedEventHandler;
  private StyleChangedCallbackDelegate _styleManagerStyleChangedCallbackDelegate;

  /**
    * @brief Event for StyleChanged signal which can be used to subscribe/unsubscribe the
    * event handler (in the type of StyleChangedEventHandler) provided by the user.
    * StyleChanged signal is is emitted after the style (e.g. theme/font change) has changed
    * and the controls have been informed.
    */
  public event StyleChangedEventHandler StyleChanged
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_styleManagerStyleChangedEventHandler == null)
           {
              _styleManagerStyleChangedEventHandler += value;

              _styleManagerStyleChangedCallbackDelegate = new StyleChangedCallbackDelegate(OnStyleChanged);
              this.StyleChangedSignal().Connect(_styleManagerStyleChangedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_styleManagerStyleChangedEventHandler != null)
           {
              this.StyleChangedSignal().Disconnect(_styleManagerStyleChangedCallbackDelegate);
           }

           _styleManagerStyleChangedEventHandler -= value;
        }
     }
  }

  // Callback for StyleManager StyleChangedsignal
  private void OnStyleChanged(IntPtr styleManager, Dali.StyleChangeType styleChange)
  {
     StyleChangedEventArgs e = new StyleChangedEventArgs();

     // Populate all members of "e" (StyleChangedEventArgs) with real data
     e.StyleManager = StyleManager.GetStyleManagerFromPtr( styleManager );
     e.StyleChange = styleChange;

     if (_styleManagerStyleChangedEventHandler != null)
     {
        //here we send all data to user event handlers
        _styleManagerStyleChangedEventHandler(this, e);
     }
  }

   public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
      ClassName ret = new ClassName(cPtr, false);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
      return ret;
   }

%}

%enddef


%define DALI_STYLEMANAGER_EVENTHANDLER_PARAM( NameSpace, ClassName)

  STYLEMANAGER_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  STYLEMANAGER_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_STYLEMANAGER_EVENTHANDLER_PARAM( Dali::Toolkit, StyleManager);
}
