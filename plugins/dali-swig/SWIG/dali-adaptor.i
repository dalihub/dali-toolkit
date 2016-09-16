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

%ignore *::Application(Internal::Adaptor::Application*);
%ignore *::DragAndDropDetector(Internal::Adaptor::DragAndDropDetector*);
%ignore *::Timer(Internal::Adaptor::Timer*);
%ignore *::Window(Internal::Adaptor::Window*);

%rename(StyleChangeType) Dali::StyleChange::Type;

%include <dali/public-api/adaptor-framework/style-change.h>
%include <dali/public-api/adaptor-framework/timer.h>
%include <dali/devel-api/adaptor-framework/drag-and-drop-detector.h>
%include <dali/public-api/adaptor-framework/window.h>

#if defined(SWIGCSHARP)

// %include <arrays_csharp.i>
// CSHARP_ARRAYS(char **, string)
// %typemap(imtype, inattributes="[global::System.Runtime.InteropServices.In, global::System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.LPArray, SizeParamIndex=0, ArraySubType=global::System.Runtime.InteropServices.UnmanagedType.LPStr)]") char **INPUT[] "string[]"
// %apply int *INPUT { int *argc };
// %apply char **INPUT[]  { char **argv[] };

%{
  // keep argcs and argv so they're always available to DALi
  int argC = 1;
  char **argV = NULL;
%}

// Temporary hack to generate argv. Todo... support conversions from the C# strings[] in the argv that is generated below

%typemap(in) ( int* argc, char **argv[]  ) {

  // Todo generate argv data from the C# args
  char **array;         // two dimensional array
  int numStrings = 1;     // number of strings
  int stringLength = 30;      // max string length.
  array = (char **)malloc( (numStrings + 1 )* sizeof(char *) );
  argV = array;

  // allocate the string data
  for( int i=0; i < numStrings; i++)
  {
    array[i]=(char *)malloc( stringLength * sizeof(char *) );
  }
  array[ numStrings ] =  NULL; // we allocated +1 for hold the NULL part

  strcpy( array[0], "dali-csharp-app");

  $1 = &argC;
  $2 = &argV;
}

// add interop services to the file
//%typemap(csimports) Dali::Application "using System.Runtime.InteropServices;"

%csmethodmodifiers Dali::Application::New() "
  /**
  * Outer::outer_method(int)
  */
  public";

%typemap(in) ( int* argc, char **argv[]  ) {

  // Todo generate argv data from the C# args
  char **array;         // two dimensional array
  int numStrings = 1;     // number of strings
  int stringLength = 30;      // max string length.
  array = (char **)malloc( (numStrings + 1 )* sizeof(char *) );
  argV = array;

  // allocate the string data
  for( int i=0; i < numStrings; i++)
  {
    array[i]=(char *)malloc( stringLength * sizeof(char *) );
  }
  array[ numStrings ] =  NULL; // we allocated +1 for hold the NULL part

  strcpy( array[0], "dali-csharp-app");

  $1 = &argC;
  $2 = &argV;
}

%apply int* INPUT { int* argc };

#endif

%include <dali/public-api/adaptor-framework/application.h>

%template(ApplicationSignal) Dali::Signal<void(Dali::Application&)>;
%template(ApplicationControlSignal) Dali::Signal<void(Dali::Application&, void*)>;
%template(TimerSignalType) Dali::Signal<bool()>;


// Application specialisation
// Macro to convert from a void * callback to a DALi C# object

