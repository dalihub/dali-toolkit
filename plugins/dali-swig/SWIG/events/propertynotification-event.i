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

%define PROPERTYNOTIFICATION_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}

%enddef


%define PROPERTYNOTIFICATION_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/**
  * @brief Event arguments that passed via Notify signal
  *
  */
public class NotifyEventArgs : EventArgs
{
   private PropertyNotification _propertyNotification;

   /**
     * @brief PropertyNotification - is the PropertyNotification handle that has the notification properties.
     *
     */
   public PropertyNotification PropertyNotification
   {
      get
      {
         return _propertyNotification;
      }
      set
      {
         _propertyNotification = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void NotifyEventHandler(object source, NotifyEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void NotifyEventCallbackDelegate(IntPtr propertyNotification);
  private NotifyEventHandler _propertyNotificationNotifyEventHandler;
  private NotifyEventCallbackDelegate _propertyNotificationNotifyEventCallbackDelegate;

  /**
    * @brief Event for Notified signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of NotifyEventHandler) provided by the user.
    * Notified signal is emitted when the notification upon a condition of the property being met, has occurred.
    */
  public event NotifyEventHandler Notified
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_propertyNotificationNotifyEventHandler == null)
           {
              _propertyNotificationNotifyEventHandler += value;

              _propertyNotificationNotifyEventCallbackDelegate = new NotifyEventCallbackDelegate(OnPropertyNotificationNotify);
              this.NotifySignal().Connect(_propertyNotificationNotifyEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_propertyNotificationNotifyEventHandler != null)
           {
              this.NotifySignal().Disconnect(_propertyNotificationNotifyEventCallbackDelegate);
           }

           _propertyNotificationNotifyEventHandler -= value;
        }
     }
  }

  // Callback for PropertyNotification NotifySignal
  private void OnPropertyNotificationNotify(IntPtr propertyNotification)
  {
     NotifyEventArgs e = new NotifyEventArgs();
     e.PropertyNotification  = GetPropertyNotificationFromPtr(propertyNotification);

     if (_propertyNotificationNotifyEventHandler != null)
     {
        //here we send all data to user event handlers
        _propertyNotificationNotifyEventHandler(this, e);
     }
  }

  public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

%}
%enddef

%define DALI_PROPERTYNOTIFICATION_EVENTHANDLER_PARAM( NameSpace, ClassName)
  PROPERTYNOTIFICATION_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  PROPERTYNOTIFICATION_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);
%enddef

namespace Dali
{
  DALI_PROPERTYNOTIFICATION_EVENTHANDLER_PARAM( Dali, PropertyNotification);
}

