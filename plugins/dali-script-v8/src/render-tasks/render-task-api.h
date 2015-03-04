#ifndef __DALI_V8PLUGIN_RENDER_TASK_API_H__
#define __DALI_V8PLUGIN_RENDER_TASK_API_H__

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

namespace Dali
{

namespace V8Plugin
{

namespace RenderTaskApi
{


  /**
   * RenderTask API see render-task.h for a description
   */
  void SetSourceActor( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetSourceActor(const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetExclusive( const v8::FunctionCallbackInfo< v8::Value >& args );
  void IsExclusive( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetInputEnabled( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetInputEnabled( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetCameraActor( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetCameraActor( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetTargetFrameBuffer(const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetTargetFrameBuffer( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetScreenToFrameBufferMappingActor( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetScreenToFrameBufferMappingActor( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetViewport( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetViewport( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetClearEnabled( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetClearEnabled( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetCullMode ( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetCullMode( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetRefreshRate( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetRefreshRate( const v8::FunctionCallbackInfo< v8::Value >& args );

}; // namespace RenderTaskApi

} // namespace V8Plugin

} // namespace Dali

#endif // header __DALI_V8PLUGIN_RENDER_TASK_API_H__
