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

#if defined(SWIGCSHARP)
%module(directors="1") NDalic
#define DALI_EXPORT_API
#define DALI_IMPORT_API
#define DALI_INTERNAL
#else
%module Dali
#endif

%include exception.i
%include dali-gc.i

%exception {
    try {
        $action
    } catch (std::out_of_range& e) {
        SWIG_exception(SWIG_IndexError,const_cast<char*>(e.what()));
    } catch (std::exception& e) {
        SWIG_exception(SWIG_RuntimeError,const_cast<char*>(e.what()));
    } catch (...) {
        SWIG_exception(SWIG_UnknownError,"unknown error");
    }
}

#if defined(SWIGCSHARP)
%{
#define SWIGSTDCALL
%}
#endif

%{
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/viewport.h>
#include <dali/public-api/object/property-key.h>
#include <dali/devel-api/object/csharp-type-info.h>
#include <dali/devel-api/object/csharp-type-registry.h>

#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/adaptor-framework/style-change.h>
#include <dali/devel-api/adaptor-framework/drag-and-drop-detector.h>
#include <dali/devel-api/adaptor-framework/application-extensions.h>

#include <dali/devel-api/images/nine-patch-image.h>

#include <dali-toolkit/devel-api/builder/builder.h>

#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>

#include <dali-toolkit/devel-api/controls/popup/popup.h>
#include <dali-toolkit/devel-api/controls/progress-bar/progress-bar.h>
#include <dali-toolkit/devel-api/controls/gaussian-blur-view/gaussian-blur-view.h>
#include <dali-toolkit/devel-api/controls/page-turn-view/page-factory.h>
#include <dali-toolkit/devel-api/controls/page-turn-view/page-turn-view.h>
#include <dali-toolkit/devel-api/controls/page-turn-view/page-turn-landscape-view.h>
#include <dali-toolkit/devel-api/controls/page-turn-view/page-turn-portrait-view.h>

#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>

#include <dali-toolkit/devel-api/visuals/text-visual-properties.h>

#include <dali-toolkit/public-api/controls/scrollable/item-view/item-view-declarations.h>

// add here SWIG version check

#if defined(_MSC_VER)         // Microsoft Visual C++ 6.0
// disable Swig-dependent warnings

// 'identifier1' has C-linkage specified,
// but returns UDT 'identifier2' which is incompatible with C
#pragma warning(disable: 4190)

// 'int' : forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable: 4800)

// debug info too long etc etc
#pragma warning(disable: 4786)
#endif
%}

%include stl.i
%include exception.i
%include typemaps.i
%include std_common.i
%include carrays.i

%include cpointer.i
%pointer_class(float, floatp);
%pointer_class(int, intp);
%pointer_class(double, doublep);
%pointer_class(unsigned int, uintp);
%pointer_class(unsigned short, ushortp);
%pointer_cast(int, unsigned int, int_to_uint);

%apply unsigned char { uint8_t };
%apply unsigned short int { uint16_t };
%apply unsigned int { uint32_t };
%apply int { int32_t };

#if defined(SWIGCSHARP)

%typemap(ctype)  void * "void *"
%typemap(imtype) void * "System.IntPtr"
%typemap(cstype) void * "System.IntPtr"
%typemap(csin)   void * "$csinput"
%typemap(in)     void * %{ $1 = $input; %}
%typemap(out)    void * %{ $result = $1; %}
%typemap(csout)  void * { return $imcall; }

#endif

#if defined(SWIGCSHARP)

// string &
%typemap(ctype) std::string & "char**"
%typemap(imtype) std::string & "out string"
%typemap(cstype) std::string & "out string"

//C++
%typemap(in, canthrow=1) std::string &
%{
  //typemap in
  std::string temp;
  $1 = &temp;
%}

%typemap(argout) std::string &
%{
  //Typemap argout in c++ file.
  //This will convert c++ string to c# string
  *$input = SWIG_csharp_string_callback($1->c_str());
%}

%typemap(argout) const std::string &
%{
  //argout typemap for const std::string&
%}

%typemap(csin) std::string & "out $csinput"

%typemap(throws, canthrow=1) string &
%{
  SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, $1.c_str());
  return $null;
%}




// Type registry type maps
%typemap(cstype) Dali::TypeInfo::CreateFunction "System.Delegate"
%typemap(csin, pre ="System.IntPtr ip = System.Runtime.InteropServices.Marshal.GetFunctionPointerForDelegate($csinput); ")
  Dali::TypeInfo::CreateFunction "new System.Runtime.InteropServices.HandleRef(null, ip)" // null was this

%typemap(cstype) Dali::CSharpTypeInfo::CreateFunction "System.Delegate"
%typemap(csin, pre ="System.IntPtr ip = System.Runtime.InteropServices.Marshal.GetFunctionPointerForDelegate($csinput); ")
   Dali::CSharpTypeInfo::CreateFunction "new System.Runtime.InteropServices.HandleRef(null, ip)" // null was this



%typemap(cstype) Dali::CSharpTypeInfo::SetPropertyFunction "System.Delegate"
%typemap(csin, pre ="System.IntPtr ip = System.Runtime.InteropServices.Marshal.GetFunctionPointerForDelegate($csinput); ")
   Dali::CSharpTypeInfo::SetPropertyFunction "new System.Runtime.InteropServices.HandleRef(null, ip)" // null was this



%typemap(cstype) Dali::CSharpTypeInfo::GetPropertyFunction "System.Delegate"
%typemap(csin, pre ="System.IntPtr ip2 = System.Runtime.InteropServices.Marshal.GetFunctionPointerForDelegate($csinput); ")
   Dali::CSharpTypeInfo::GetPropertyFunction "new System.Runtime.InteropServices.HandleRef(null, ip2)" // null was this







#endif



/**
 * Extend the type registry to allow for registering of C# controls and custom properties
 */
%extend Dali::TypeRegistration {


   static void RegisterControl( const std::string& controlName, Dali::CSharpTypeInfo::CreateFunction createFunc )
   {
     Dali::CSharpTypeRegistry::RegisterType( controlName, typeid( Dali::Toolkit::Control), createFunc );
   };


   static void RegisterProperty( const std::string& controlName,
                    const std::string& propertyName,
                    int index,
                    Property::Type type,
                    Dali::CSharpTypeInfo::SetPropertyFunction setFunc,
                    Dali::CSharpTypeInfo::GetPropertyFunction getFunc )
   {
     Dali::CSharpTypeRegistry::RegisterProperty( controlName, propertyName, index, type, setFunc, getFunc );
   };

};




%ignore operator<<;
%ignore *::GetImplementation();
%ignore *::GetImplementation(Dali::BaseHandle&);

%{
using namespace Dali;
using namespace Dali::Toolkit;
%}

%feature("director") Dali::Toolkit::Internal::Control;
%feature("notabstract") Dali::Toolkit::FixedRuler;
%feature("notabstract") Dali::Toolkit::DefaultRuler;

// Note... all the typemap declarations have to be included before the DALi C++ head files are include otherwise
// they have no effect.

%include signal-parameters.i
%include signals.i

%include events/actor-event.i
%include events/accessibilitymanager-event.i
%include events/application-event.i
%include events/animation-event.i
%include events/button-event.i
%include events/builder-event.i
%include events/control-event.i
%include events/gaussian-blur-view-event.i
%include events/image-event.i
%include events/itemview-event.i
%include events/objectregistry-event.i
%include events/popup-event.i
%include events/pinchgesture-event.i
%include events/pageturnview-event.i
%include events/pangesture-event.i
%include events/propertynotification-event.i
%include events/longpressgesture-event.i
%include events/rectinteger.i
%include events/resourceimage-event.i
%include events/scrollable-event.i
%include events/scrollbar-event.i
%include events/scrollview-event.i
%include events/slider-event.i
%include events/stage-event.i
%include events/stylemanager-event.i
%include events/tapgesture-event.i
%include events/texteditor-event.i
%include events/textfield-event.i
%include events/timer-event.i
%include events/videoview-event.i

%include alphafunction.i
%include name-changed.i
%include property-value.i
%include dali-operator.i
%include dali-core.i
%include dali-adaptor.i
%include dali-toolkit.i

