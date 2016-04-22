#ifndef __DALI_V8PLUGIN_RENDERER_API_H__
#define __DALI_V8PLUGIN_RENDERER_API_H__

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


namespace Dali
{

namespace V8Plugin
{

namespace RendererApi
{

  /**
   * Helper to get renderer from args.This()
   */
  Renderer GetRenderer( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * @brief Helper to get renderer from the JavaScript object held in the given function argument
   * @param[in] paramIndex Argument index the object is held in
   * @param[in] found Whether renderer is found in the given function parameter
   * @param[in] isolate v8 isolated instance
   * @param[in] args v8 function call arguments interpreted
   */
  Renderer GetRendererFromParams( int paramIndex,
                                     bool& found,
                                     v8::Isolate* isolate,
                                     const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * Constructor
   */
   Renderer New( const v8::FunctionCallbackInfo< v8::Value >& args );

   /**
    * Renderer API see renderer.h for a description
    */
   void SetGeometry( const v8::FunctionCallbackInfo< v8::Value >& args );
   void GetGeometry( const v8::FunctionCallbackInfo< v8::Value >& args );
   void SetTextures( const v8::FunctionCallbackInfo< v8::Value >& args );
   void GetTextures( const v8::FunctionCallbackInfo< v8::Value >& args );
   void SetBlendFunc( const v8::FunctionCallbackInfo< v8::Value >& args );
   void GetBlendFunc( const v8::FunctionCallbackInfo< v8::Value >& args );
   void SetBlendEquation( const v8::FunctionCallbackInfo< v8::Value >& args );
   void GetBlendEquation( const v8::FunctionCallbackInfo< v8::Value >& args );

}; // namespace RendererApi

} // namespace V8Plugin

} // namespace Dali

#endif // header __DALI_V8PLUGIN_RENDERER_API_H__
