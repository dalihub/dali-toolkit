#ifndef __DALI_V8PLUGIN_RENDERER_WRAPPER_H__
#define __DALI_V8PLUGIN_RENDERER_WRAPPER_H__

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
#include <dali/devel-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <object/handle-wrapper.h>

namespace Dali
{

namespace V8Plugin
{


/**
 * A Renderer wrapper.
 * Provides access to Renderer specific functionality and V8 memory handling.
 */
class RendererWrapper : public HandleWrapper
{

public:

  /**
   * @brief Constructor
   * @param[in] renderer DALi renderer
   * @param[in] gc garbage collection interface
   */
  RendererWrapper( const Renderer& renderer,
                GarbageCollectorInterface& gc );

  /**
   * @brief Destructor
   */
 virtual ~RendererWrapper()
  {
  };

  /**
   * @brief Creates a new Renderer wrapped inside a Javascript Object.
   * @param[in] args v8 function call arguments interpreted
   */
  static void NewRenderer( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * Wraps a renderer
   */
  static v8::Handle<v8::Object> WrapRenderer(v8::Isolate* isolate, const Dali::Renderer& );


  // The RendererAttribute ObjectTemplate, we cache templates so we don't have
  // keep generating them everytime we want to create a Renderer
  static v8::Persistent<v8::ObjectTemplate> mRendererTemplate;

  /**
   * @brief Gets the handle of renderer
   * @return the handle of renderer
   */
  Renderer GetRenderer();


private:

  // Renderer
  Renderer mRenderer;

  /**
   * @brief Creates the object template that used to create renderer at runtime
   * @param[in] isolate v8 isolated instance
   * @return the object template
   */
  static v8::Handle<v8::ObjectTemplate> MakeRendererTemplate( v8::Isolate* isolate );

  /**
   * @brief Gets the object template that used to create renderer at runtime
   * @param[in] isolate v8 isolated instance
   * @return the object template
   */
  static v8::Local<v8::ObjectTemplate> GetRendererTemplate( v8::Isolate* isolate );

};

} // namespace V8Plugin

} // namespace Dali

#endif // __DALI_V8PLUGIN_RENDERER_WRAPPER_H__
