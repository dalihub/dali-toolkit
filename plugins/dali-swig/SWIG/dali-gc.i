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

%define DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( NameSpace, ClassName, NewClassName )
%typemap(csfinalize) NameSpace::ClassName %{
  ~NewClassName() {
    DisposeQueue.Instance.Add(this);
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( NameSpace, ClassName, NewClassName )
%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_##NewClassName(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }
%}
%enddef

%define DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION_RENAME( NameSpace, ClassName, NewClassName )
%typemap(csdestruct_derived, methodname="Dispose", methodmodifiers="public") NameSpace::ClassName %{{
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_##NewClassName(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
      base.Dispose();
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

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, KeyEvent, Key );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, KeyEvent, Key );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, HoverEvent, Hover );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, HoverEvent, Hover );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, TouchEvent );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, TouchEvent );
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, WheelEvent, Wheel );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, WheelEvent, Wheel );

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
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, TouchData, Touch );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION_RENAME( Dali, TouchData, Touch );

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

DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::Application&)>, ApplicationSignal);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::Application&)>, ApplicationSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::Application&, void*)>, ApplicationControlSignal);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::Application&, void*)>, ApplicationControlSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::Toolkit::VideoView&)>, VideoViewSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::Toolkit::VideoView&)>, VideoViewSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::Image)>, ImageSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::Image)>, ImageSignal);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::ResourceImage)>, ResourceImageSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::ResourceImage)>, ResourceImageSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Rect<int>, RectInteger);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Rect<int>, RectInteger);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Rect<double>, RectDouble);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Rect<double>, RectDouble);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Rect<float>, RectFloat);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Rect<float>, RectFloat);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Rect<unsigned int>, RectUnsignedInteger);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Rect<unsigned int>, RectUnsignedInteger);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( std, vector<Dali::TouchPoint>, TouchPointContainer);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( std, vector<Dali::TouchPoint>, TouchPointContainer);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Vector<int>, VectorInteger);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Vector<int>, VectorInteger);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Vector<float>, VectorFloat);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Vector<float>, VectorFloat);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Vector<unsigned char>, VectorUnsignedChar);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Vector<unsigned char>, VectorUnsignedChar);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Vector<Dali::Uint16Pair>, VectorUint16Pair);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Vector<Dali::Uint16Pair>, VectorUint16Pair);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<bool(Dali::Toolkit::AccessibilityManager&)>, AccessibilityActionSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<bool(Dali::Toolkit::AccessibilityManager&)>, AccessibilityActionSignal);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<bool(Dali::Toolkit::AccessibilityManager&, const Dali::TouchEvent&)>, AccessibilityActionScrollSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<bool(Dali::Toolkit::AccessibilityManager&, const Dali::TouchEvent&)>, AccessibilityActionScrollSignal);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::Actor, Dali::Toolkit::AccessibilityManager::FocusOvershotDirection)>, AccessibilityFocusOvershotSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::Actor, Dali::Toolkit::AccessibilityManager::FocusOvershotDirection)>, AccessibilityFocusOvershotSignal);
//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_STRINGVALUEPAIR( Dali, pair<std::string, Dali::Property::Value>);
//DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_STRINGVALUEPAIR( Dali, pair<std::string, Dali::Property::Value>);
//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_BOOLSIGNAL( Dali, Signal<bool()>);
//DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_BOOLSIGNAL( Dali, Signal<bool()>);
//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_TIMERSIGNALTYPE( Dali, Signal<bool()>);
//DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_TIMERSIGNALTYPE( Dali, Signal<bool()>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void (Dali::Actor)>, ActorSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void (Dali::Actor)>, ActorSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::Animation&)>, AnimationSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::Animation&)>, AnimationSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<bool(Dali::Toolkit::Button)>, ButtonSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<bool(Dali::Toolkit::Button)>, ButtonSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::Toolkit::PageTurnView, unsigned int, bool)>, PageTurnSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::Toolkit::PageTurnView, unsigned int, bool)>, PageTurnSignal);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::Toolkit::PageTurnView)>, PagePanSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::Toolkit::PageTurnView)>, PagePanSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void (Dali::Actor, const Dali::PanGesture&)>, PanGestureDetectedSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void (Dali::Actor, const Dali::PanGesture&)>, PanGestureDetectedSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void (Dali::Actor, const Dali::PinchGesture&)>, PinchGestureDetectedSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void (Dali::Actor, const Dali::PinchGesture&)>, PinchGestureDetectedSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::RenderTask&)>, RenderTaskSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::RenderTask&)>, RenderTaskSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal< void(const Dali::Vector2&)>, ScrollableSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal< void(const Dali::Vector2&)>, ScrollableSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal< void(const Dali::Toolkit::ScrollView::SnapEvent&)>, ScrollViewSnapStartedSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal< void(const Dali::Toolkit::ScrollView::SnapEvent&)>, ScrollViewSnapStartedSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<bool(Dali::Toolkit::Slider, float)>, SliderValueChangedSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<bool(Dali::Toolkit::Slider, float)>, SliderValueChangedSignal);
DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<bool(Dali::Toolkit::Slider, int)>, SliderMarkReachedSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<bool(Dali::Toolkit::Slider, int)>, SliderMarkReachedSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void (const Dali::WheelEvent&)>, StageWheelSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void (const Dali::WheelEvent&)>, StageWheelSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::Toolkit::StyleManager, Dali::StyleChange::Type)>, StyleChangedSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::Toolkit::StyleManager, Dali::StyleChange::Type)>,StyleChangedSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void (Dali::Actor, const Dali::TapGesture&)>, TapGestureDetectedSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void (Dali::Actor, const Dali::TapGesture&)>, TapGestureDetectedSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::Toolkit::TextEditor)>, TextEditorSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::Toolkit::TextEditor)>, TextEditorSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::Toolkit::TextField)>, TextFieldSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::Toolkit::TextField)>, TextFieldSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( std, vector<Dali::Actor>, ActorContainer);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( std, vector<Dali::Actor>, ActorContainer);

//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_ITEM( std, pair<unsigned int, Dali::Actor));
//DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_ITEM( std, pair<unsigned int, Dali::Actor>);

//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_ITEMCONTAINER( std, vector<std::pair<unsigned int, Dali::Actor>>);
//DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_ITEMCONTAINER( std, vector<std::pair<unsigned int, Dali::Actor>>);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( std, vector<unsigned int>, ItemIdContainer);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( std, vector<unsigned int>, ItemIdContainer);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::Actor, Dali::Actor)>, FocusChangedSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::Actor, Dali::Actor)>, FocusChangedSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::Actor, bool)>, FocusGroupChangedSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::Actor, bool)>, FocusGroupChangedSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void (Dali::Toolkit::GaussianBlurView)>, GaussianBlurViewSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void (Dali::Toolkit::GaussianBlurView)>, GaussianBlurViewSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<bool(Dali::Toolkit::Control, const Dali::KeyEvent&)>, ControlKeySignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<bool(Dali::Toolkit::Control, const Dali::KeyEvent&)>, ControlKeySignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void (const Dali::KeyEvent&)>, KeyEventSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void (const Dali::KeyEvent&)>, KeyEventSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::Toolkit::Control)>, KeyInputFocusSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::Toolkit::Control)>, KeyInputFocusSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, IntrusivePtr<Dali::Toolkit::Ruler>, RulerPtr);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, IntrusivePtr<Dali::Toolkit::Ruler>, RulerPtr);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void()>, VoidSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void()>, VoidSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(float)>, FloatSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(float)>, FloatSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(const Dali::RefObject*)>, ObjectDestroyedSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(const Dali::RefObject*)>, ObjectDestroyedSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::BaseHandle)>, ObjectCreatedSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::BaseHandle)>, ObjectCreatedSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void(Dali::PropertyNotification&)>, PropertyNotifySignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void(Dali::PropertyNotification&)>, PropertyNotifySignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void (Dali::Actor, const Dali::LongPressGesture&)>, LongPressGestureDetectedSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void (Dali::Actor, const Dali::LongPressGesture&)>, LongPressGestureDetectedSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<bool (Dali::Actor, const Dali::HoverEvent&)>, ActorHoverSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<bool (Dali::Actor, const Dali::HoverEvent&)>, ActorHoverSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<bool (Dali::Actor, const Dali::TouchData&)>, ActorTouchDataSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<bool (Dali::Actor, const Dali::TouchData&)>, ActorTouchDataSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<bool (Dali::Actor, const Dali::WheelEvent&)>, ActorWheelSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<bool (Dali::Actor, const Dali::WheelEvent&)>, ActorWheelSignal);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, Signal<void (const Dali::TouchData&)>, TouchSignal);
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, Signal<void (const Dali::TouchData&)>, TouchSignal);

//DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION_RENAME( Dali, (std::pair< Dali::Radian, Dali::Radian >), AngleThresholdPair);
//DALI_CREATE_CUSTOM_DISPOSE_FUNCTION_RENAME( Dali, (std::pair< Dali::Radian, Dali::Radian >), AngleThresholdPair);

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, ApplicationExtensions );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, ApplicationExtensions );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, SlotObserver );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, SlotObserver );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, SignalObserver );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, SignalObserver );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, AnimatablePropertyComponentRegistration );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, AnimatablePropertyComponentRegistration );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, AnimatablePropertyRegistration );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, AnimatablePropertyRegistration );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, AsyncImageLoader );
DALI_CREATE_CUSTOM_DISPOSE_DERIVED_FUNCTION( Dali::Toolkit, AsyncImageLoader );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, ChildPropertyRegistration );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, ChildPropertyRegistration );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali::Toolkit, ClampState2D );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali::Toolkit, ClampState2D );

DALI_CREATE_CUSTOM_DESTRUCTOR_FUNCTION( Dali, DaliException );
DALI_CREATE_CUSTOM_DISPOSE_FUNCTION( Dali, DaliException );
