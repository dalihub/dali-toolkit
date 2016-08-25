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

 /**
  * WARNING: The macro below may be over-ridden by any other typemap(cscode)
  * customisation for a class.
  *
  * MACRO to inject a  C# function in to a SWIG generated DALi C# wrapper class.
  * The MACRO allow you to create a C# wrapper class for a DALi C++ object (passed as a C PTR).
  *
  * Required to get access to any data passed as a parameter in a Signal ( in C# they are delegate parameters).
  * E.g.
  *
  * CREATE_CSHARP_WRAPPER_FROM_C_PTR_FUNCTION( TouchData );
  * Creates a function called GetTouchDataFromPtr which allows you to:
  *
  * static void OnStageTouched(IntPtr data)
  * {
  *    TouchData touchData = TouchData.GetTouchDataFromPtr( data );
  * }
  *
  * ## means concat in a SWIG macro
  */

%define DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( NameSpace, ClassName )

 %typemap(cscode) NameSpace::ClassName %{
 public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
	if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }
%}

%enddef

DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, Application );

DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, Actor );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, Image );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, ResourceImage );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, Animation );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, TouchEvent );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, TouchData );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, HoverEvent );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, WheelEvent );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, KeyEvent );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, LongPressGesture );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, PanGesture );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, PinchGesture );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, TapGesture );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, PropertyNotification );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, BaseHandle );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, RefObject );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, RenderTask );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, Vector2 );

DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, AccessibilityManager );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali, StyleManager );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali::Toolkit, Control );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali::Toolkit, Button );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali::Toolkit, GaussianBlurView );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali::Toolkit, PageTurnView );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali::Toolkit, TextEditor );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali::Toolkit, TextField );

DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali::Toolkit, TextField );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali::Toolkit, VideoView );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali::Toolkit, Slider );
DALI_CREATE_C_PTR_TO_CSHARP_FUNCTION( Dali::Toolkit::ScrollView, SnapEvent );
 
  


