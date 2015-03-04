/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// HEADER
#include "object-template-helper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <signals/signal-manager.h>


namespace Dali
{

namespace V8Plugin
{

namespace ObjectTemplateHelper
{

void AddSignalConnectAndDisconnect( v8::Isolate* isolate,  v8::Local<v8::ObjectTemplate>& objTemplate )
{
  objTemplate->Set( v8::String::NewFromUtf8( isolate, "connect"),
                     v8::FunctionTemplate::New( isolate, SignalManager::SignalConnect) );



  objTemplate->Set( v8::String::NewFromUtf8( isolate, "disconnect"),
                     v8::FunctionTemplate::New( isolate, SignalManager::SignalDisconnect) );
}




void InstallFunctions( v8::Isolate* isolate,
                              v8::Local<v8::ObjectTemplate>& objTemplate,
                              const ApiFunction* functionTable,
                              unsigned int tableCount,
                              FunctionType type )
{
  // add our function properties
   for( unsigned int i = 0; i < tableCount; ++i )
   {
     const ApiFunction property =  functionTable[i];

     std::string funcName;

     if( type == NORMAL_FUNCTIONS )
     {
       funcName = V8Utils::GetJavaScriptFunctionName( property.name);
     }
     else
     {

       funcName = property.name;
     }
    // printf("Installing function %s \n",      funcName.c_str()     );
     objTemplate->Set( v8::String::NewFromUtf8(   isolate, funcName.c_str() ),
                      v8::FunctionTemplate::New( isolate, property.function ) );
   }
}

} // ObjectTemplateHelper

} // V8Plugin

} // Dali
