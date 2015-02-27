#ifndef __DALI_V8PLUGIN_OBJECT_TEMPLATE_HELPER_H__
#define __DALI_V8PLUGIN_OBJECT_TEMPLATE_HELPER_H__

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

// EXTERNAL INCLUDES
#include <v8.h>

// INTERNAL INCLUDES
#include <shared/api-function.h>


namespace Dali
{

namespace V8Plugin
{

namespace ObjectTemplateHelper
{



/**
 * @brief Used for adding signal connect / disconnect functionality to an object template
 * E.g.
 */
void AddSignalConnectAndDisconnect( v8::Isolate* isolate,  v8::Local<v8::ObjectTemplate>& objTemplate );

enum FunctionType
{
  NORMAL_FUNCTIONS = 0,
  CONSTRUCTOR_FUNCTIONS = 1
};

/**
 * Installs a list of functions on to an object template (e.g. actor.add, actor.setDrawMode() )
 */
void InstallFunctions( v8::Isolate* isolate,
                              v8::Local<v8::ObjectTemplate>& objTemplate,
                              const ApiFunction* functionTable,
                              unsigned int tableCount,
                              FunctionType type = NORMAL_FUNCTIONS);


}
} // V8Plugin

} // Dali

#endif // __DALI_V8PLUGIN_OBJECT_TEMPLATE_HELPER_H__
