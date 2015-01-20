#ifndef __DALI_V8PLUGIN_STAGE_API_H__
#define __DALI_V8PLUGIN_STAGE_API_H__

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
#include <dali/public-api/common/stage.h>


namespace Dali
{

namespace V8Plugin
{

namespace StageApi
{

  /**
   * helper
   */
  Dali::Stage GetStage( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * StageApi API see image-actor.h for a description
    */
  void Add( const v8::FunctionCallbackInfo< v8::Value >& args );
  void Remove( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetSize( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetRenderTaskList( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetLayer( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetLayerCount( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetRootLayer( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetBackgroundColor( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetBackgroundColor( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetDpi( const v8::FunctionCallbackInfo< v8::Value >& args );

}; // namespace StageApi

} // namespace V8Plugin

} // namespace Dali

#endif // header __DALI_V8PLUGIN_STAGE_API_H__
