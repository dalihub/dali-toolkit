/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#ifndef CSHARP_EVENT_THREAD_CALLBACK
#define CSHARP_EVENT_THREAD_CALLBACK
#endif

#include "common.h"
#include <dali/devel-api/adaptor-framework/event-thread-callback.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (SWIGSTDCALL* SWIG_CallbackMakeCallback)(void);

SWIGEXPORT void * SWIGSTDCALL CSharp_Dali_new_EventThreadCallback(SWIG_CallbackMakeCallback callbackOnMakeCallback) {
  void * jresult ;
  void (*arg1)(void) = (void (*)(void)) 0 ;
  Dali::EventThreadCallback *result = 0 ;

  arg1 = (void (*)(void))callbackOnMakeCallback;
  {
    try {
      result = (Dali::EventThreadCallback *)new Dali::EventThreadCallback((Dali::CallbackBase *)Dali::MakeCallback(arg1));
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = (void *)result;
  return jresult;
}

SWIGEXPORT void SWIGSTDCALL CSharp_Dali_delete_EventThreadCallback(void * jarg1) {
  Dali::EventThreadCallback *arg1 = (Dali::EventThreadCallback *) 0 ;

  arg1 = (Dali::EventThreadCallback *)jarg1;
  {
    try {
      delete arg1;
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}


SWIGEXPORT void SWIGSTDCALL CSharp_Dali_EventThreadCallback_Trigger(void * jarg1) {
  Dali::EventThreadCallback *arg1 = (Dali::EventThreadCallback *) 0 ;

  arg1 = (Dali::EventThreadCallback *)jarg1;
  {
    try {
      (arg1)->Trigger();
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}

#ifdef __cplusplus
}
#endif //CSHARP_EVENT_THREAD_CALLBACK

