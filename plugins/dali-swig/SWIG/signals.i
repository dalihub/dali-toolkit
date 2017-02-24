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

// Example from Swig CSHARP documentation
// To allow a CDate class to take
// To achieve this mapping, we need to alter the default code generation slightly so that at the C# layer, a System.DateTime is converted into a CDate. The
// intermediary layer will still take a pointer to the underlying CDate class. The typemaps to achieve this are shown below.

// %typemap(cstype) const CDate & "System.DateTime"
// %typemap(csin,
// pre="
// CDate temp$csinput = new CDate($csinput.Year, $csinput.Month, $csinput.Day);"
// ) const CDate &
// "$csclassname.getCPtr(temp$csinput)"
// The csin typemap is used for converting function parameter types from the type
// used in the proxy, module or type wrapper class to the type used in the PInvoke class.


%include <dali/public-api/signals/dali-signal.h>

/*
 * By default SWIG maps Derived types, structs, and classes to C pointers.
 * So for Signals which have a Connect /Disconnect function, the function parameter just gets maps to a C pointer.
 * However, call backs in C# are done using delegates, so we change the Connect / Disconnect parameter from
 * something like  void (*func)( Dali::Actor ) to a C# delegate.
 * the parameter type is changed using the typemap(cstype)  cstype = csharp-type
 * The actual conversion from a C# delegate to a c function pointer is done when Connect/Disconnect is called
 * using  typemap(csin) with GetFunctionPointerForDelegate ( csin = csharp in code?).
 * So when connect is called on a Signal it will call the   void Signal::Connect( void (*func)( Arg0 arg0 ) ) -- which
 * just takes a standard C function pointer. Not ideal as there is no delegate / connection tracking.
 *
 */
%define SIGNAL_TYPEMAP_HELPER( SignalSignature )
%typemap(cstype) SignalSignature "System.Delegate"
%typemap(csin, pre ="System.IntPtr ip = System.Runtime.InteropServices.Marshal.GetFunctionPointerForDelegate($csinput); ")
   SignalSignature "new System.Runtime.InteropServices.HandleRef(this, ip)"
%enddef

/**
 * SWIG can't auto generate wrappers for template parameters that are functions ( which dali-signal has)
 * so we have make them ourselves
 */

%define SIGNAL_TEMPLATE_HELPER_0( returnType,  returnFunc)
  template<> class Signal< returnType () >
  {
  public:
    %extend
    {
      bool Empty() const
      {
         return $self->Empty();
      }
      std::size_t GetConnectionCount() const
      {
        return $self->GetConnectionCount();
      }
      void Connect( returnType ( *func ) () )
      {
          $self->Connect( func );
      }
      void Disconnect( returnType ( *func ) () )
      {
          $self->Disconnect( func );
      }
      returnType Emit()
      {
          returnFunc $self->Emit();
      }
    }
  };
%enddef

%define SIGNAL_TEMPLATE_HELPER_1( returnType,  returnFunc, argumentType )
  template<> class Signal< returnType ( argumentType ) >
  {
  public:
    %extend
    {
      bool Empty() const
      {
         return $self->Empty();
      }
      std::size_t GetConnectionCount() const
      {
        return $self->GetConnectionCount();
      }
      void Connect( returnType ( *func ) (  argumentType  ) )
      {
          $self->Connect( func );
      }
      void Disconnect( returnType ( *func ) (  argumentType  ) )
      {
          $self->Disconnect( func );
      }
      returnType Emit( argumentType arg)
      {
          returnFunc $self->Emit( arg );
      }
    }
  };
%enddef

%define SIGNAL_TEMPLATE_HELPER_2( returnType,  returnFunc, argumentType1, argumentType2 )
  template<> class Signal< returnType (  argumentType1, argumentType2 ) >
  {
  public:
    %extend
    {
      bool Empty() const
      {
         return $self->Empty();
      }
      std::size_t GetConnectionCount() const
      {
        return $self->GetConnectionCount();
      }
      void Connect( returnType ( *func ) (  argumentType1, argumentType2   ) )
      {
        $self->Connect( func );
      }
      void Disconnect( returnType ( *func ) (  argumentType1, argumentType2  ) )
      {
        $self->Disconnect( func );
      }
      returnType Emit( argumentType1 arg1, argumentType2 arg2 )
      {
        returnFunc $self->Emit( arg1, arg2 );
      }
    }
  };
%enddef

%define SIGNAL_TEMPLATE_HELPER_3( returnType, returnFunc, argumentType1, argumentType2, argumentType3 )
  template<> class Signal< returnType ( argumentType1, argumentType2, argumentType3 ) >
  {
  public:
    %extend
    {
      bool Empty() const
      {
         return $self->Empty();
      }
      std::size_t GetConnectionCount() const
      {
        return $self->GetConnectionCount();
      }
      void Connect( returnType ( *func ) ( argumentType1, argumentType2, argumentType3  ) )
      {
          return $self->Connect( func );
      }
      void Disconnect( returnType ( *func ) ( argumentType1, argumentType2, argumentType3  ) )
      {
          $self->Disconnect( func );
      }
      returnType Emit( argumentType1 arg1, argumentType2 arg2, argumentType3 arg3 )
      {
          returnFunc $self->Emit( arg1, arg2, arg3 );
      }
    }
  };
%enddef

////////////////////////////////
/*****************
 Macros for signals  without return values
*****************/

// the emit for some signal has to emit a return value
// this macro is just for signals that don't (instead of return Emit();.. it just does ;Emit();
%define NO_RETURN_FUNC;
%enddef

// Macro to define a csharp signal.
// 0 param signals ( no return )
%define DALI_SIGNAL_0_PARAM()

  SIGNAL_TYPEMAP_HELPER( void (*func) () );
  SIGNAL_TEMPLATE_HELPER_0( void, NO_RETURN_FUNC);
%enddef

// 1 param signals ( no return )
%define DALI_SIGNAL_1_PARAM( argumentType1 )

  SIGNAL_TYPEMAP_HELPER( void (*func) ( argumentType1 ) );
  SIGNAL_TEMPLATE_HELPER_1( void, NO_RETURN_FUNC, argumentType1);
%enddef

// 2 param signals ( no return )
%define DALI_SIGNAL_2_PARAM( argumentType1, argumentType2)

  SIGNAL_TYPEMAP_HELPER( void (*func) ( argumentType1, argumentType2) );
  SIGNAL_TEMPLATE_HELPER_2( void, NO_RETURN_FUNC, argumentType1, argumentType2);

%enddef

// 3 param signals ( no return )
%define DALI_SIGNAL_3_PARAM( argumentType1, argumentType2, argumentType3 )

  SIGNAL_TYPEMAP_HELPER( void (*func) ( argumentType1, argumentType2, argumentType3 ) );
  SIGNAL_TEMPLATE_HELPER_3( void, NO_RETURN_FUNC, argumentType1, argumentType2, argumentType3);

%enddef

/*****************
 Macros for signals with return values
*****************/

// 0 param signals ( with return )
%define DALI_SIGNAL_0_PARAM_RETURN( returnType )

  SIGNAL_TYPEMAP_HELPER( returnType (*func) () );
  SIGNAL_TEMPLATE_HELPER_0( returnType, return);
%enddef

// 1 param signals ( with return )
%define DALI_SIGNAL_1_PARAM_RETURN( returnType, argumentType1 )

  SIGNAL_TYPEMAP_HELPER( returnType (*func) ( argumentType1 ) );
  SIGNAL_TEMPLATE_HELPER_1( returnType, return, argumentType1);

%enddef

// 2 param signals ( with return )
%define DALI_SIGNAL_2_PARAM_RETURN( returnType, argumentType1, argumentType2)

  SIGNAL_TYPEMAP_HELPER( returnType (*func) ( argumentType1, argumentType2) );
  SIGNAL_TEMPLATE_HELPER_2( returnType, return, argumentType1, argumentType2);

%enddef

// 3 param signals ( with return )
%define DALI_SIGNAL_3_PARAM_RETURN( returnType, argumentType1, argumentType2, argumentType3 )

  SIGNAL_TYPEMAP_HELPER( returnType (*func) ( argumentType1, argumentType2, argumentType3 ) );
  SIGNAL_TEMPLATE_HELPER_3( returnType, return, argumentType1, argumentType2, argumentType3);

%enddef

namespace Dali
{
// Signal< void () >
DALI_SIGNAL_0_PARAM();

// Signal< bool () >
DALI_SIGNAL_0_PARAM_RETURN( bool );

// Signal< void (Actor) >
DALI_SIGNAL_1_PARAM( Dali::Actor );

//  Signal< void (float) >
DALI_SIGNAL_1_PARAM( float );

// Signal< void (Dali::Application&) >
DALI_SIGNAL_1_PARAM( Dali::Application& );

// Signal< void (Dali::Application&, void*) >
DALI_SIGNAL_2_PARAM( Dali::Application& , void* );

// Signal< void (Dali::Image) >
DALI_SIGNAL_1_PARAM( Dali::Image );

// Signal< void (Dali::ResourceImage) >
DALI_SIGNAL_1_PARAM( Dali::ResourceImage );

// Signal< void (Dali::Animation&) >
DALI_SIGNAL_1_PARAM( Dali::Animation& );

// Signal< void (Dali::Actor, const Dali::TouchEvent&) >
DALI_SIGNAL_2_PARAM( Dali::Actor, const Dali::TouchEvent& );

//  Signal< bool (Dali::Actor, const Dali::TouchData&) >
DALI_SIGNAL_2_PARAM_RETURN( bool, Dali::Actor, const Dali::TouchData& );

// Signal< bool (Dali::Actor, const Dali::HoverEvent&) >
DALI_SIGNAL_2_PARAM_RETURN( bool , Dali::Actor, const Dali::HoverEvent& );

// Signal< bool (Dali::Actor, const Dali::WheelEvent&) >
DALI_SIGNAL_2_PARAM_RETURN( bool , Dali::Actor, const Dali::WheelEvent& );

// Signal< void (const Dali::KeyEvent&) >
DALI_SIGNAL_1_PARAM( const Dali::KeyEvent&  );

// Signal< void (const Dali::TouchData&) >
DALI_SIGNAL_1_PARAM( const Dali::TouchData& );

// Signal< void (const Dali::HoverEvent&) >
DALI_SIGNAL_1_PARAM( const Dali::HoverEvent& );

// Signal< void (const Dali::WheelEvent&) >
DALI_SIGNAL_1_PARAM( const Dali::WheelEvent& );

// Signal< void (const Dali::LongPressGesture&) >
DALI_SIGNAL_2_PARAM( Dali::Actor, const Dali::LongPressGesture& );

// Signal< void (Dali::Actor, const Dali::PanGesture&) >
DALI_SIGNAL_2_PARAM( Dali::Actor, const Dali::PanGesture& );

// Signal< void (Dali::Actor, const Dali::PinchGesture&) >
DALI_SIGNAL_2_PARAM( Dali::Actor, const Dali::PinchGesture& );

// Signal< void (Dali::Actor, const Dali::TapGesture&) >
DALI_SIGNAL_2_PARAM( Dali::Actor, const Dali::TapGesture& );

// Signal< void (Dali::PropertyNotification) >
DALI_SIGNAL_1_PARAM( Dali::PropertyNotification& ) ;

//  Signal< void (Dali::BaseHandle) >
DALI_SIGNAL_1_PARAM( Dali::BaseHandle );

//  Signal< void (const Dali::RefObject*) >
DALI_SIGNAL_1_PARAM( const Dali::RefObject* );

// Signal< void (const Dali::RenderTask&) >
DALI_SIGNAL_1_PARAM( const Dali::RenderTask& );

// Signal< bool ( const Dali::Toolkit::AccessibilityManager& ) >
DALI_SIGNAL_1_PARAM_RETURN( bool, Dali::Toolkit::AccessibilityManager& );

// Signal< bool ( const Dali::Toolkit::AccessibilityManager&, const Dali::TouchEvent& ) >
DALI_SIGNAL_2_PARAM_RETURN( bool, const Dali::Toolkit::AccessibilityManager&, const Dali::TouchEvent& );

// Signal< void ( const Dali::Actor Dali::Toolkit::AccessibilityManager::FocusOvershotDirection ) >
DALI_SIGNAL_2_PARAM( Dali::Actor, Dali::Toolkit::AccessibilityManager::FocusOvershotDirection );

// Signal< bool ( Dali::Toolkit::AccessibilityManager&, const Dali::TouchEvent& ) >
DALI_SIGNAL_2_PARAM_RETURN( bool ,Dali::Toolkit::AccessibilityManager&, const Dali::TouchEvent&);

// Signal< void (  Dali::Toolkit::StyleManager, Dali::StyleChange::Type) >
DALI_SIGNAL_2_PARAM( Dali::Toolkit::StyleManager, Dali::StyleChange::Type);

// void Signal<  Dali::Actor, bool >;
DALI_SIGNAL_2_PARAM( Dali::Actor, bool);

// void Signal<  Dali::Actor, Dali::Actor >;
DALI_SIGNAL_2_PARAM( Dali::Actor, Dali::Actor);

// bool Signal< Dali::Toolkit::Button >;
DALI_SIGNAL_1_PARAM_RETURN( bool, Dali::Toolkit::Button);

// void Signal< Dali::Toolkit::GaussianBlurView >;
DALI_SIGNAL_1_PARAM( Dali::Toolkit::GaussianBlurView);

// void Signal< Dali::Toolkit::PageTurnView, unsigned int, bool >;
DALI_SIGNAL_3_PARAM( Dali::Toolkit::PageTurnView, unsigned int, bool );

// void Signal< Dali::Toolkit::PageTurnView >;
DALI_SIGNAL_1_PARAM( Dali::Toolkit::PageTurnView );

// void Signal< const Dali::Toolkit::ScrollView::SnapEvent& >;
DALI_SIGNAL_1_PARAM( const Dali::Toolkit::ScrollView::SnapEvent& );

// void Signal< const Dali::Vector2& >;
DALI_SIGNAL_1_PARAM( const Dali::Vector2& );

// void Signal< Dali::Toolkit::TextEditor >;
DALI_SIGNAL_1_PARAM( Dali::Toolkit::TextEditor );

// void Signal< Dali::Toolkit::TextField >;
DALI_SIGNAL_1_PARAM( Dali::Toolkit::TextField );

// bool Signal< Dali::Toolkit::Control, const Dali::KeyEvent& >;
DALI_SIGNAL_2_PARAM_RETURN( bool, Dali::Toolkit::Control, const Dali::KeyEvent& );

// void Signal< Dali::Toolkit::Control >;
DALI_SIGNAL_1_PARAM( Dali::Toolkit::Control );

// void Signal< Dali::Toolkit::VideoView& >;
DALI_SIGNAL_1_PARAM( Dali::Toolkit::VideoView& );

// Signal< bool (Dali::Toolkit::Slider, float) >;
DALI_SIGNAL_2_PARAM_RETURN( bool,Dali::Toolkit::Slider, float );

//  Signal< bool(Dali::Toolkit::Slider, int) >;
DALI_SIGNAL_2_PARAM_RETURN( bool,Dali::Toolkit::Slider, int );

} // namespace DALi


