#ifndef __DALI_V8PLUGIN_BUILDER_API_H__
#define __DALI_V8PLUGIN_BUILDER_API_H__

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
#include <dali-toolkit/devel-api/builder/builder.h>


namespace Dali
{

namespace V8Plugin
{

namespace BuilderApi
{
  /**
   * Constructor
   */
  Dali::Toolkit::Builder New( const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * Builder API see animation.h for a description
   */
  void SetDuration( const v8::FunctionCallbackInfo< v8::Value >& args );
  void LoadFromString( const v8::FunctionCallbackInfo< v8::Value >& args );
  void LoadFromFile( const v8::FunctionCallbackInfo< v8::Value >& args );
  void AddConstants( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetConstants( const v8::FunctionCallbackInfo< v8::Value >& args );
  void CreateAnimation( const v8::FunctionCallbackInfo< v8::Value >& args );

  void Create( const v8::FunctionCallbackInfo< v8::Value >& args );
  void ApplyStyle( const v8::FunctionCallbackInfo< v8::Value >& args );
  void ApplyFromJson(const v8::FunctionCallbackInfo< v8::Value >& args);
  void AddActors( const v8::FunctionCallbackInfo< v8::Value >& args );
  void CreateRenderTask( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetShaderEffect( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetFrameBufferImage( const v8::FunctionCallbackInfo< v8::Value >& args );
}; // namespace BuilderApi

} // namespace V8Plugin

} // namespace Dali

#endif // header __DALI_V8PLUGIN_BUILDER_API_H__
