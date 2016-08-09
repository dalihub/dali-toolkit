#ifndef DALI_V8PLUGIN_SHADER_WRAPPER_H
#define DALI_V8PLUGIN_SHADER_WRAPPER_H

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

// INTERNAL INCLUDES
#include <object/handle-wrapper.h>

namespace Dali
{

namespace V8Plugin
{


/**
 * A Shader wrapper.
 * Provides access to Shader specific functionality and V8 memory handling.
 */
class ShaderWrapper : public HandleWrapper
{

public:

  /**
   * @brief Constructor
   * @param[in] shader DALi shader
   * @param[in] gc garbage collection interface
   */
  ShaderWrapper( const Shader& shader,
                GarbageCollectorInterface& gc );

  /**
   * @brief Destructor
   */
  virtual ~ShaderWrapper()
  {
  };

  /**
   * @brief Creates a new Shader wrapped inside a Javascript Object.
   * @param[in] args v8 function call arguments interpreted
   */
  static void NewShader( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * Wraps a shader
   */
  static v8::Handle<v8::Object> WrapShader(v8::Isolate* isolate, const Dali::Shader& );


  // The ShaderAttribute ObjectTemplate, we cache templates so we don't have
  // keep generating them everytime we want to create a Shader
  static v8::Persistent<v8::ObjectTemplate> mShaderTemplate;

  /**
   * @brief Gets the handle of shader
   * @return the handle of shader
   */
  Shader GetShader();


private:

  // Shader
  Shader mShader;

  /**
   * @brief Creates the object template that used to create shader at runtime
   * @param[in] isolate v8 isolated instance
   * @return the object template
   */
  static v8::Handle<v8::ObjectTemplate> MakeShaderTemplate( v8::Isolate* isolate );

  /**
   * @brief Gets the object template that used to create shader at runtime
   * @param[in] isolate v8 isolated instance
   * @return the object template
   */
  static v8::Local<v8::ObjectTemplate> GetShaderTemplate( v8::Isolate* isolate );

};

} // namespace V8Plugin

} // namespace Dali

#endif // DALI_V8PLUGIN_SHADER_WRAPPER_H__
