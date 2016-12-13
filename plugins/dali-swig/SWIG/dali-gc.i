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

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_##ClassName(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( NameSpace, ClassName )
%typemap(csdestruct_derived, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_##ClassName(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
      base.Dispose();
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~ClassName() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_VIDEOVIEWSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~VideoViewSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_VIDEOVIEWSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_VideoViewSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_APPLICATIONSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~ApplicationSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_APPLICATIONSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_ApplicationSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_APPLICATIONCONTROLSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~ApplicationControlSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_APPLICATIONCONTROLSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_ApplicationControlSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_IMAGESIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~ImageSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_IMAGESIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_ImageSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RESOURCESIMAGESIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~ResourceImageSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RESOURCESIMAGESIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_ResourceImageSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RECTINT( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~RectInteger() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RECTINT( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_RectInteger(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RECTDOUBLE( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~RectDouble() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RECTDOUBLE( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_RectDouble(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RECTFLOAT( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~RectFloat() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RECTFLOAT( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_RectFloat(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RECTUNSIGNEDINT( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~RectUnsignedInteger() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RECTUNSIGNEDINT( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_RectUnsignedInteger(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_STRINGVALUEPAIR( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~StringValuePair() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_STRINGVALUEPAIR( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_StringValuePair(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_TOUCHCONTAINER( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~TouchPointContainer() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_TOUCHCONTAINER( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_TouchPointContainer(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_BOOLSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~BoolSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_BOOLSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_BoolSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_ACCESSIBILITYACTIONSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~AccessibilityActionSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_ACCESSIBILITYACTIONSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_AccessibilityActionSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_ACCESSIBILITYACTIONSCROLLSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~AccessibilityActionScrollSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_ACCESSIBILITYACTIONSCROLLSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_AccessibilityActionScrollSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_ACCESSIBILITYFOCUSSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~AccessibilityFocusOvershotSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_ACCESSIBILITYFOCUSSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_AccessibilityFocusOvershotSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_VECTORINT( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~VectorInteger() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_VECTORINT( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_VectorInteger(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_VECTORFLOAT( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~VectorFloat() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_VECTORFLOAT( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_VectorFloat(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_VECTORUNSIGNEDCHAR( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~VectorUnsignedChar() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_VECTORUNSIGNEDCHAR( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_VectorUnsignedChar(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_VECTORUINT16PAIR( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~VectorUint16Pair() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_VECTORUINT16PAIR( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_VectorUint16Pair(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_TIMERSIGNALTYPE( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~TimerSignalType() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_TIMERSIGNALTYPE( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_TimerSignalType(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_ACTOR( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~ActorSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_ACTOR( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_ActorSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_ANIMATIONSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~AnimationSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_ANIMATIONSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_AnimationSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_BUTTON( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~ButtonSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_BUTTON( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_ButtonSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_PAGETURNSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~PageTurnSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_PAGETURNSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_PageTurnSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_PAGEVIEWSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~PagePanSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_PAGEVIEWSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_PagePanSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_PANGESTURESIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~PanGestureDetectedSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_PANGESTURESIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_PanGestureDetectedSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_PINCHGESTURESIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~PinchGestureDetectedSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_PINCHGESTURESIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_PinchGestureDetectedSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENDERTASKSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~RenderTaskSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENDERTASKSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_RenderTaskSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_SCROLLABLESIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~ScrollableSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_SCROLLABLESIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_ScrollableSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_SCROLLVIEWSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~ScrollViewSnapStartedSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_SCROLLVIEWSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_ScrollViewSnapStartedSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_SLIDERVALUECHANGEDSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~SliderValueChangedSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_SLIDERVALUECHANGEDSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_SliderValueChangedSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_SLIDERMARKCHANGEDSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~SliderMarkReachedSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_SLIDERMARKCHANGEDSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_SliderMarkReachedSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_STAGESIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~StageWheelEventSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_STAGESIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_StageWheelEventSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_STYLECHANGEDSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~StyleChangedSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_STYLECHANGEDSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_StyleChangedSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_TAPGESTURESIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~TapGestureDetectedSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_TAPGESTURESIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_TapGestureDetectedSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_TEXTFIELDSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~TextFieldSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_TEXTFIELDSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_TextFieldSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_TEXTEDITORSIGNAL( NameSpace, ClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~TextEditorSignal() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_TEXTEDITORSIGNAL( NameSpace, ClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_TextEditorSignal(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Any );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, Any );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Actor );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, Actor );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, AlphaFunction );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, AlphaFunction );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Animation );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, Animation );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, AngleAxis );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, AngleAxis );

//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, BaseObject );
//DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, BaseObject );
//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, RefObject );
//DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, RefObject );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Handle );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, Handle );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, ObjectRegistry );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, ObjectRegistry );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, BaseHandle );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, BaseHandle );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, BufferImage );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, BufferImage );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, CameraActor );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, CameraActor );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, CustomActor );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, CustomActor );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, ConnectionTracker );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, ConnectionTracker );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, ConnectionTrackerInterface );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, ConnectionTrackerInterface );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Degree );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, Degree );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, EncodedBufferImage );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, EncodedBufferImage );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, FrameBuffer );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, FrameBuffer );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, FrameBufferImage );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, FrameBufferImage );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Geometry );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, Geometry );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Gesture );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, Gesture );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, GestureDetector );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, GestureDetector );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, TapGesture );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, TapGesture );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, TapGestureDetector );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, TapGestureDetector );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, PanGesture );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, PanGesture );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, PanGestureDetector );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, PanGestureDetector );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, PinchGesture );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, PinchGesture );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, PinchGestureDetector );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, PinchGestureDetector );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, LongPressGesture );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, LongPressGesture );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, LongPressGestureDetector );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, LongPressGestureDetector );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, KeyEvent );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, KeyEvent );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, HoverEvent );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, HoverEvent );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, TouchEvent );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, TouchEvent );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, WheelEvent );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, WheelEvent );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Renderer );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, Renderer );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, RenderTask );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, RenderTask );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, RenderTaskList );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, RenderTaskList );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, RelayoutContainer );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, RelayoutContainer );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Radian );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, Radian );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Quaternion );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, Quaternion );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Matrix );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, Matrix );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Matrix3 );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, Matrix3 );

//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, RulerPtr );
//DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, RulerPtr );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Layer );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, Layer );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, KeyFrames );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, KeyFrames );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Stage );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, Stage );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Sampler );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, Sampler );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Shader );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, Shader );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Vector2 );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, Vector2 );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Vector3 );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, Vector3 );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Vector4 );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, Vector4 );
//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, VectorBase );
//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, VectorBase );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Image );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, Image );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, NativeImage );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, NativeImage );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, NinePatchImage );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, NinePatchImage );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, ResourceImage );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, ResourceImage );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Path );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, Path );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Property );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, Property );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, PropertyBuffer );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, PropertyBuffer );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, PropertyCondition );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, PropertyCondition );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, PropertyNotification );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, PropertyNotification );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, PixelData );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, PixelData );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Uint16Pair );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, Uint16Pair );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, TypeInfo );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, TypeInfo );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Texture );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, Texture );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, TextureSet );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, TextureSet );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, TimePeriod );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, TimePeriod );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, LinearConstrainer );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, LinearConstrainer );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, PathConstrainer );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, PathConstrainer );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, TouchPoint );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, TouchPoint );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, TouchData );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, TouchData );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Application );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, Application );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, DragAndDropDetector );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, DragAndDropDetector );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Timer );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, Timer );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, Window );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali, Window );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, AccessibilityManager );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, AccessibilityManager );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, Alignment );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, Alignment );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, Button );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, Button );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, CheckBoxButton );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, CheckBoxButton );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, PushButton );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, PushButton );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, RadioButton );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, RadioButton );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, Builder );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, Builder );

//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, View );
//DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, View );
//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, ViewImpl );
//DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, ViewImpl );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, FlexContainer );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, FlexContainer );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, GaussianBlurView );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, GaussianBlurView );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, ImageView );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, ImageView );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, ItemView );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, ItemView );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, ItemFactory );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali::Toolkit, ItemFactory );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, ItemLayout );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, ItemLayout );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, ItemRange );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali::Toolkit, ItemRange );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, Model3dView );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, Model3dView );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, PageFactory );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali::Toolkit, PageFactory );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, PageTurnLandscapeView );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, PageTurnLandscapeView );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, PageTurnPortraitView );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, PageTurnPortraitView );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, PageTurnView );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, PageTurnView );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, Popup );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, Popup );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, ProgressBar );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, ProgressBar );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, Scrollable );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, Scrollable );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, ScrollBar );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, ScrollBar );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, ScrollView );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, ScrollView );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, ScrollViewEffect );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, ScrollViewEffect );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, ScrollViewPagePathEffect );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, ScrollViewPagePathEffect );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, StyleManager );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, StyleManager );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, Slider );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, Slider );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, TableView );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, TableView );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, TextEditor );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, TextEditor );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, TextField );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, TextField );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, TextLabel );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, TextLabel );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, VideoView );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, VideoView );
//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, VisualBase );
//DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, VisualBase);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, VisualFactory );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, VisualFactory );

DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_APPLICATIONSIGNAL( Dali, Signal<void(Dali::Application&)>);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_APPLICATIONSIGNAL( Dali, Signal<void(Dali::Application&)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_APPLICATIONCONTROLSIGNAL( Dali, Signal<void(Dali::Application&, void*)>);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_APPLICATIONCONTROLSIGNAL( Dali, Signal<void(Dali::Application&, void*)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_VIDEOVIEWSIGNAL( Dali, Signal<void(Dali::Toolkit::VideoView&)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_VIDEOVIEWSIGNAL( Dali, Signal<void(Dali::Toolkit::VideoView&)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_IMAGESIGNAL( Dali, Signal<void(Dali::Image)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_IMAGESIGNAL( Dali, Signal<void(Dali::Image)>);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RESOURCESIMAGESIGNAL( Dali, Signal<void(Dali::ResourceImage)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RESOURCESIMAGESIGNAL( Dali, Signal<void(Dali::ResourceImage)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RECTINT( Dali, Rect<int>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RECTINT( Dali, Rect<int>);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RECTDOUBLE( Dali, Rect<double>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RECTDOUBLE( Dali, Rect<double>);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RECTFLOAT( Dali, Rect<float>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RECTFLOAT( Dali, Rect<float>);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RECTUNSIGNEDINT( Dali, Rect<unsigned int>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RECTUNSIGNEDINT( Dali, Rect<unsigned int>);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_TOUCHCONTAINER( std, vector<Dali::TouchPoint>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_TOUCHCONTAINER( std, vector<Dali::TouchPoint>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_VECTORINT( Dali, Vector<int>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_VECTORINT( Dali, Vector<int>);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_VECTORFLOAT( Dali, Vector<float>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_VECTORFLOAT( Dali, Vector<float>);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_VECTORUNSIGNEDCHAR( Dali, Vector<unsigned char>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_VECTORUNSIGNEDCHAR( Dali, Vector<unsigned char>);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_VECTORUINT16PAIR( Dali, Vector<Dali::Uint16Pair>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_VECTORUINT16PAIR( Dali, Vector<Dali::Uint16Pair>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_ACCESSIBILITYACTIONSIGNAL( Dali, Signal<bool(Dali::Toolkit::AccessibilityManager&)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_ACCESSIBILITYACTIONSIGNAL( Dali, Signal<bool(Dali::Toolkit::AccessibilityManager&)>);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_ACCESSIBILITYACTIONSCROLLSIGNAL( Dali, Signal<bool(Dali::Toolkit::AccessibilityManager&, const Dali::TouchEvent&)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_ACCESSIBILITYACTIONSCROLLSIGNAL( Dali, Signal<bool(Dali::Toolkit::AccessibilityManager&, const Dali::TouchEvent&)>);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_ACCESSIBILITYFOCUSSIGNAL( Dali, Signal<void(Dali::Actor, Dali::Toolkit::AccessibilityManager::FocusOvershotDirection)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_ACCESSIBILITYFOCUSSIGNAL( Dali, Signal<void(Dali::Actor, Dali::Toolkit::AccessibilityManager::FocusOvershotDirection)>);
//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_STRINGVALUEPAIR( Dali, pair<std::string, Dali::Property::Value>);
//DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_STRINGVALUEPAIR( Dali, pair<std::string, Dali::Property::Value>);
//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_BOOLSIGNAL( Dali, Signal<bool()>);
//DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_BOOLSIGNAL( Dali, Signal<bool()>);
//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_TIMERSIGNALTYPE( Dali, Signal<bool()>);
//DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_TIMERSIGNALTYPE( Dali, Signal<bool()>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_ACTOR( Dali, Signal<void (Dali::Actor)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_ACTOR( Dali, Signal<void (Dali::Actor)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_ANIMATIONSIGNAL( Dali, Signal<void(Dali::Animation&)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_ANIMATIONSIGNAL( Dali, Signal<void(Dali::Animation&)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_BUTTON( Dali, Signal<bool(Dali::Toolkit::Button)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_BUTTON( Dali, Signal<bool(Dali::Toolkit::Button)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_PAGETURNSIGNAL( Dali, Signal<void(Dali::Toolkit::PageTurnView, unsigned int, bool)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_PAGETURNSIGNAL( Dali, Signal<void(Dali::Toolkit::PageTurnView, unsigned int, bool)>);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_PAGEVIEWSIGNAL( Dali, Signal<void(Dali::Toolkit::PageTurnView)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_PAGEVIEWSIGNAL( Dali, Signal<void(Dali::Toolkit::PageTurnView)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_PANGESTURESIGNAL( Dali, Signal<void (Dali::Actor, const Dali::PanGesture&)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_PANGESTURESIGNAL( Dali, Signal<void (Dali::Actor, const Dali::PanGesture&)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_PINCHGESTURESIGNAL( Dali, Signal<void (Dali::Actor, const Dali::PinchGesture&)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_PINCHGESTURESIGNAL( Dali, Signal<void (Dali::Actor, const Dali::PinchGesture&)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENDERTASKSIGNAL( Dali, Signal<void(Dali::RenderTask&)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENDERTASKSIGNAL( Dali, Signal<void(Dali::RenderTask&)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_SCROLLABLESIGNAL( Dali, Signal< void(const Dali::Vector2&)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_SCROLLABLESIGNAL( Dali, Signal< void(const Dali::Vector2&)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_SCROLLVIEWSIGNAL( Dali, Signal< void(const Dali::Toolkit::ScrollView::SnapEvent&)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_SCROLLVIEWSIGNAL( Dali, Signal< void(const Dali::Toolkit::ScrollView::SnapEvent&)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_SLIDERVALUECHANGEDSIGNAL( Dali, Signal<bool(Dali::Toolkit::Slider, float)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_SLIDERVALUECHANGEDSIGNAL( Dali, Signal<bool(Dali::Toolkit::Slider, float)>);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_SLIDERMARKCHANGEDSIGNAL( Dali, Signal<bool(Dali::Toolkit::Slider, int)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_SLIDERMARKCHANGEDSIGNAL( Dali, Signal<bool(Dali::Toolkit::Slider, int)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_STAGESIGNAL( Dali, Signal<void (const Dali::WheelEvent&)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_STAGESIGNAL( Dali, Signal<void (const Dali::WheelEvent&)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_STYLECHANGEDSIGNAL( Dali, Signal<void(Dali::Toolkit::StyleManager, Dali::StyleChange::Type)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_STYLECHANGEDSIGNAL( Dali, Signal<void(Dali::Toolkit::StyleManager, Dali::StyleChange::Type)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_TAPGESTURESIGNAL( Dali, Signal<void (Dali::Actor, const Dali::TapGesture&)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_TAPGESTURESIGNAL( Dali, Signal<void (Dali::Actor, const Dali::TapGesture&)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_TEXTEDITORSIGNAL( Dali, Signal<void(Dali::Toolkit::TextEditor)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_TEXTEDITORSIGNAL( Dali, Signal<void(Dali::Toolkit::TextEditor)>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_TEXTFIELDSIGNAL( Dali, Signal<void(Dali::Toolkit::TextField)>);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_TEXTFIELDSIGNAL( Dali, Signal<void(Dali::Toolkit::TextField)>);
