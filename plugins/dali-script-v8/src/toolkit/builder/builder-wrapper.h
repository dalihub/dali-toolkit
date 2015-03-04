#ifndef __DALI_V8PLUGIN_BUILDER_WRAPPER_H__
#define __DALI_V8PLUGIN_BUILDER_WRAPPER_H__

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
#include <dali-toolkit/public-api/builder/builder.h>

// INTERNAL INCLUDES
#include <shared/base-wrapped-object.h>


namespace Dali
{

namespace V8Plugin
{

/**
 * Builder  wrapper.
 * Provides access to Builder specific functionality
 */
class BuilderWrapper : public BaseWrappedObject
{

public:

  BuilderWrapper( const Dali::Toolkit::Builder& builder,
                GarbageCollectorInterface& gc );

  virtual ~BuilderWrapper();

  /**
   * @brief Creates a new Builder wrapped inside a Javascript Object.
   * @param[in] args v8 function call arguments interpreted
   */
  static void NewBuilder( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * Wraps a builder
   */
  static v8::Handle<v8::Object> WrapBuilder(v8::Isolate* isolate, const Dali::Toolkit::Builder& );


  Dali::Toolkit::Builder GetBuilder();


private:

  static v8::Local<v8::ObjectTemplate> GetBuilderTemplate( v8::Isolate* isolate );
  Dali::Toolkit::Builder mBuilder;
};

} // namespace V8Plugin

} // namespace Dali

#endif // __DALI_V8PLUGIN_BUILDER_WRAPPER_H__
