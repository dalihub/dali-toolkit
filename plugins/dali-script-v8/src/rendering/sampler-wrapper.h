#ifndef __DALI_V8PLUGIN_SAMPLER_WRAPPER_H__
#define __DALI_V8PLUGIN_SAMPLER_WRAPPER_H__

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
#include <dali/devel-api/rendering/sampler.h>

// INTERNAL INCLUDES
#include <shared/base-wrapped-object.h>

namespace Dali
{

namespace V8Plugin
{

/**
 * A Sampler wrapper.
 * Provides access to Sampler specific functionality and V8 memory handling.
 */
class SamplerWrapper : public BaseWrappedObject
{

public:

  /**
   * @brief Constructor
   * @param[in] sampler DALi sampler
   * @param[in] gc garbage collection interface
   */
  SamplerWrapper( const Sampler& sampler,
                GarbageCollectorInterface& gc );

  /**
   * @brief Destructor
   */
  virtual ~SamplerWrapper()
  {
  };

  /**
   * @brief Creates a new Sampler wrapped inside a Javascript Object.
   * @param[in] args v8 function call arguments interpreted
   */
  static void NewSampler( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * Wraps a sampler
   */
  static v8::Handle<v8::Object> WrapSampler(v8::Isolate* isolate, const Sampler& );

  // The SamplerAttribute ObjectTemplate, we cache templates so we don't have
  // keep generating them everytime we want to create a Sampler
  static v8::Persistent<v8::ObjectTemplate> mSamplerTemplate;

  /**
   * @brief Gets the handle of sampler
   * @return the handle of sampler
   */
  Sampler GetSampler();

private:

  // Sampler
  Sampler mSampler;

  /**
   * @brief Creates the object template that used to create sampler at runtime
   * @param[in] isolate v8 isolated instance
   * @return the object template
   */
  static v8::Handle<v8::ObjectTemplate> MakeSamplerTemplate( v8::Isolate* isolate );

  /**
   * @brief Gets the object template that used to create sampler at runtime
   * @param[in] isolate v8 isolated instance
   * @return the object template
   */
  static v8::Local<v8::ObjectTemplate> GetSamplerTemplate( v8::Isolate* isolate );
};

} // namespace V8Plugin

} // namespace Dali

#endif // __DALI_V8PLUGIN_SAMPLER_WRAPPER_H__
