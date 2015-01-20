#ifndef __DALI_V8PLUGIN_RENDERABLE_ACTOR_API_H__
#define __DALI_V8PLUGIN_RENDERABLE_ACTOR_API_H__

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
#include <dali/public-api/actors/renderable-actor.h>


namespace Dali
{

namespace V8Plugin
{

namespace RenderableActorApi
{
   /**
   * RenderableActor API see renderable-actor.h for a description
   * Unfortunately none of the functions are covered by the property system
   */
  void SetSortModifier( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetSortModifier( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetCullFace( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetCullFace( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetBlendMode( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetBlendMode( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetBlendFunc( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetBlendFunc( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetBlendEquation( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetBlendEquation( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetBlendColor( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetBlendColor( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetShaderEffect( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetShaderEffect( const v8::FunctionCallbackInfo< v8::Value >& args );
  void RemoveShaderEffect( const v8::FunctionCallbackInfo< v8::Value >& args );


}; // namespace RenderableActorApi

} // namespace V8Plugin

} // namespace Dali

#endif // header __DALI_V8PLUGIN_RENDERABLE_ACTOR_API_H__
