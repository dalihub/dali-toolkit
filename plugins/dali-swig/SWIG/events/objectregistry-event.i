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

%define OBJECT_REGISTRY_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}

%enddef


%define OBJECT_REGISTRY_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{


public class ObjectCreatedEventArgs : EventArgs
{
   private BaseHandle _baseHandle;

   public BaseHandle BaseHandle
   {
      get
      {
         return _baseHandle;
      }
      set
      {
         _baseHandle = value;
      }
   }
}

public class ObjectDestroyedEventArgs : EventArgs
{
   private RefObject _refObject;

   public RefObject RefObject
   {
      get
      {
         return _refObject;
      }
      set
      {
         _refObject = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void ObjectCreatedEventHandler(object source, ObjectCreatedEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void ObjectDestroyedEventHandler(object source, ObjectDestroyedEventArgs e);


  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void ObjectCreatedEventCallbackDelegate(IntPtr baseHandle);
  private ObjectCreatedEventHandler _objectRegistryObjectCreatedEventHandler;
  private ObjectCreatedEventCallbackDelegate _objectRegistryObjectCreatedEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void ObjectDestroyedEventCallbackDelegate(IntPtr fefObject);
  private ObjectDestroyedEventHandler _objectRegistryObjectDestroyedEventHandler;
  private ObjectDestroyedEventCallbackDelegate _objectRegistryObjectDestroyedEventCallbackDelegate;

  public event ObjectCreatedEventHandler ObjectCreated
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_objectRegistryObjectCreatedEventHandler == null)
           {
              _objectRegistryObjectCreatedEventHandler += value;

              _objectRegistryObjectCreatedEventCallbackDelegate = new ObjectCreatedEventCallbackDelegate(OnObjectCreated);
              this.ObjectCreatedSignal().Connect(_objectRegistryObjectCreatedEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_objectRegistryObjectCreatedEventHandler != null)
           {
              this.ObjectCreatedSignal().Disconnect(_objectRegistryObjectCreatedEventCallbackDelegate);
           }

           _objectRegistryObjectCreatedEventHandler -= value;
        }
     }
  }

  // Callback for ObjectRegistry ObjectCreatedSignal
  private void OnObjectCreated(IntPtr baseHandle)
  {
     ObjectCreatedEventArgs e = new ObjectCreatedEventArgs();

     // Populate all members of "e" (ObjectCreatedEventArgs) with real data
     //e.BaseHandle = BaseHandle.GetBaseHandleFromPtr(baseHandle); //GetBaseHandleFromPtr() is not present in BaseHandle.cs. Not sure what is the reason?

     if (_objectRegistryObjectCreatedEventHandler != null)
     {
        //here we send all data to user event handlers
        _objectRegistryObjectCreatedEventHandler(this, e);
     }
  }

  public event ObjectDestroyedEventHandler ObjectDestroyed
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_objectRegistryObjectDestroyedEventHandler == null)
           {
              _objectRegistryObjectDestroyedEventHandler += value;

              _objectRegistryObjectDestroyedEventCallbackDelegate = new ObjectDestroyedEventCallbackDelegate(OnObjectDestroyed);
              this.ObjectDestroyedSignal().Connect(_objectRegistryObjectDestroyedEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_objectRegistryObjectDestroyedEventHandler != null)
           {
              this.ObjectDestroyedSignal().Disconnect(_objectRegistryObjectDestroyedEventCallbackDelegate);
           }

           _objectRegistryObjectDestroyedEventHandler -= value;
        }
     }
  }

  // Callback for ObjectRegistry ObjectDestroyedSignal
  private void OnObjectDestroyed(IntPtr refObject)
  {
     ObjectDestroyedEventArgs e = new ObjectDestroyedEventArgs();

     // Populate all members of "e" (ObjectDestroyedEventArgs) with real data
     e.RefObject = RefObject.GetRefObjectFromPtr(refObject);

     if (_objectRegistryObjectDestroyedEventHandler != null)
     {
        //here we send all data to user event handlers
        _objectRegistryObjectDestroyedEventHandler(this, e);
     }
  }

%}
%enddef

%typemap(cscode) Dali::BaseHandle %{
 public static BaseHandle GetBaseHandleFromPtr(global::System.IntPtr cPtr) {
    BaseHandle ret = new BaseHandle(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }
%}


%define DALI_OBJECT_REGISTRY_EVENTHANDLER_PARAM( NameSpace, ClassName)
  OBJECT_REGISTRY_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  OBJECT_REGISTRY_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);
%enddef

namespace Dali
{
  DALI_OBJECT_REGISTRY_EVENTHANDLER_PARAM( Dali, ObjectRegistry);
}

