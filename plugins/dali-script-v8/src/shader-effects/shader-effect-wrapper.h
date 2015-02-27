#ifndef __DALI_V8PLUGIN_SHADER_EFFECT_WRAPPER_H__
#define __DALI_V8PLUGIN_SHADER_EFFECT_WRAPPER_H__

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
#include <dali/public-api/shader-effects/shader-effect.h>

// INTERNAL INCLUDES
#include <object/handle-wrapper.h>

namespace Dali
{

namespace V8Plugin
{


/**
 * An ShaderEffect  wrapper.
 * Provides access to ShaderEffect specific functionality and V8 memory handling.
 */
class ShaderEffectWrapper : public HandleWrapper
{

public:

  ShaderEffectWrapper( const ShaderEffect& shaderEffect,
                GarbageCollectorInterface& gc );

  virtual ~ShaderEffectWrapper()
  {
  };

  /**
   * @brief Creates a new ShaderEffect wrapped inside a Javascript Object.
   * @param[in] args v8 function call arguments interpreted
   */
  static void NewShaderEffect( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * Wraps a  shaderEffect
   */
  static v8::Handle<v8::Object> WrapShaderEffect(v8::Isolate* isolate, const Dali::ShaderEffect& );


  // The ShaderEffectAttribute ObjectTemplate, we cache templates so we don't have
  // keep generating them everytime we want to create an ShaderEffect
  static v8::Persistent<v8::ObjectTemplate> mShaderEffectTemplate;

  ShaderEffect GetShaderEffect();


private:

  // ShaderEffect
  ShaderEffect mShaderEffect;

  static v8::Handle<v8::ObjectTemplate> MakeShaderEffectTemplate( v8::Isolate* isolate );
  static v8::Local<v8::ObjectTemplate> GetShaderEffectTemplate( v8::Isolate* isolate );

};

} // namespace V8Plugin

} // namespace Dali

#endif // __DALI_V8PLUGIN_SHADER_EFFECT_WRAPPER_H__
