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

// DaliWrapper.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "DaliWrapper.h"

BOOL APIENTRY DllMain( HANDLE hModule,
                        DWORD  ul_reason_for_call,
                        LPVOID lpReserved
                      )
{
  switch (ul_reason_for_call)
  {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}

// This is an example of an exported variable
DALIWRAPPER_API int nDaliWrapper=0;

// This is an example of an exported function.
DALIWRAPPER_API int fnDaliWrapper(void)
{
  return 18;
}

// This is the constructor of a class that has been exported.
// see DaliWrapper.h for the class definition
CDaliWrapper::CDaliWrapper()
{
  return;
}
