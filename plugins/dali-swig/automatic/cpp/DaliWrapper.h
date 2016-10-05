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

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the DALIWRAPPER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// DALIWRAPPER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef DALIWRAPPER_EXPORTS
#define DALIWRAPPER_API __declspec(dllexport)
#else
#define DALIWRAPPER_API __declspec(dllimport)
#endif

// This class is exported from the DaliWrapper.dll
class DALIWRAPPER_API CDaliWrapper
{

public:
  CDaliWrapper(void);
  // TODO: add your methods here.

};

extern DALIWRAPPER_API int nDaliWrapper;

DALIWRAPPER_API int fnDaliWrapper(void);
