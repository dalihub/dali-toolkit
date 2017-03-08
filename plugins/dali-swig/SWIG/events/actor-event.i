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

%define ACTOR_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
    using System;
    using System.Runtime.InteropServices;
%}

%enddef

%define ACTOR_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
  %typemap(cscode) NameSpace::ClassName %{

    public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
      ClassName ret = new ClassName(cPtr, false);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    }

    public IntPtr GetPtrfrom ## ClassName ()
    {
       return (IntPtr)swigCPtr;
    }

    public Position CurrentPosition
    {
      get
      {
        return GetCurrentPosition();
      }
    }

    public Size3D CurrentSize
    {
      get
      {
        return GetCurrentSize();
      }
    }

    public Actor Parent
    {
      get
      {
        return GetParent();
      }
    }

   public bool Visibility
   {
      get
      {
        return IsVisible();
      }
   }

    public float Opacity
    {
      get
      {
        float temp = 0;
        GetProperty( Actor.Property.OPACITY ).Get( ref temp );
        return temp;
      }
      set
      {
        SetProperty( Actor.Property.OPACITY, new Dali.Property.Value( value ) );
      }
    }

   public Vector2 ScreenPosition
   {
      get
      {
        Vector2 temp = new Vector2( 0.0f, 0.0f );
        GetProperty( Actor.Property.SCREEN_POSITION ).Get( temp );
        return temp;
      }
   }

    public bool StateFocusEnable
    {
      set
      {
        SetKeyboardFocusable(value);
      }
      get
      {
        return IsKeyboardFocusable();
      }
    }

    public bool IsOnStage
    {
      get
      {
        return OnStage();
      }
    }

    public int SiblingOrder
    {
      get
      {
        int temp = 0;
        GetProperty( Actor.Property.SIBLING_ORDER ).Get( ref temp );
        return temp;
      }
      set
      {
        SetProperty( Actor.Property.SIBLING_ORDER, new Dali.Property.Value( value ) );
      }
    }

    public void Show()
    {
      SetVisible(true);
    }

    public void Hide()
    {
      SetVisible(false);
    }

    public void Raise()
    {
      NDalicPINVOKE.Raise(swigCPtr);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    }

    public void Lower()
    {
      NDalicPINVOKE.Lower(swigCPtr);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    }

    public void RaiseToTop()
    {
      NDalicPINVOKE.RaiseToTop(swigCPtr);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    }

    public void LowerToBottom()
    {
      NDalicPINVOKE.LowerToBottom(swigCPtr);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    }

    public void RaiseAbove(Actor target)
    {
      NDalicPINVOKE.RaiseAbove(swigCPtr, Actor.getCPtr(target));
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    }

    public void LowerBelow(Actor target)
    {
      NDalicPINVOKE.RaiseAbove(swigCPtr, Actor.getCPtr(target));
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    }

%}

%enddef

%define DALI_ACTOR_EVENTHANDLER_PARAM( NameSpace, ClassName)

ACTOR_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
ACTOR_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_ACTOR_EVENTHANDLER_PARAM( Dali, Actor);
}
