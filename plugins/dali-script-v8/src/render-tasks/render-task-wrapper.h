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
#include <dali/public-api/render-tasks/render-task.h>

// INTERNAL INCLUDES
#include <object/handle-wrapper.h>

namespace Dali
{

namespace V8Plugin
{


/**
 * An RenderTask  wrapper.
 * Provides access to RenderTask specific functionality and V8 memory handling.
 */
class RenderTaskWrapper : public HandleWrapper
{

public:

  RenderTaskWrapper( const RenderTask& shaderEffect,
                GarbageCollectorInterface& gc );

  virtual ~RenderTaskWrapper() {}


  /**
   * Wraps a render task
   */
  static v8::Handle<v8::Object> WrapRenderTask(v8::Isolate* isolate, const Dali::RenderTask& );


  // The RenderTaskAttribute ObjectTemplate, we cache templates so we don't have
  // keep generating them everytime we want to create an RenderTask
  static v8::Persistent<v8::ObjectTemplate> mRenderTaskTemplate;

  RenderTask GetRenderTask();


private:

  // RenderTask
  RenderTask mRenderTask;

  static v8::Handle<v8::ObjectTemplate> MakeRenderTaskTemplate( v8::Isolate* isolate );
  static v8::Local<v8::ObjectTemplate> GetRenderTaskTemplate( v8::Isolate* isolate );

};

} // namespace V8Plugin

} // namespace Dali

#endif // __DALI_V8PLUGIN_SHADER_EFFECT_WRAPPER_H__
