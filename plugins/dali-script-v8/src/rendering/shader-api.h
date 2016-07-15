#ifndef DALI_V8PLUGIN_SHADER_API_H
#define DALI_V8PLUGIN_SHADER_API_H

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

// EXTERNAL INCLUDES
#include <v8.h>
#include <dali/public-api/rendering/shader.h>


namespace Dali
{

namespace V8Plugin
{

namespace ShaderApi
{

  /**
   * Constructor
   */
   Shader New( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

   /**
    * @brief Helper to get shader from the JavaScript object held in the given function argument
    * @param[in] paramIndex Argument index the object is held in
    * @param[in] found Whether shader is found in the given function parameter
    * @param[in] isolate v8 isolated instance
    * @param[in] args v8 function call arguments interpreted
    */
   Shader GetShaderFromParams( int paramIndex,
                             bool& found,
                             v8::Isolate* isolate,
                             const v8::FunctionCallbackInfo< v8::Value >& args );

}; // namespace ShaderApi

} // namespace V8Plugin

} // namespace Dali

#endif // DALI_V8PLUGIN_SHADER_API_H
