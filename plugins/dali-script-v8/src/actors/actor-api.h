#ifndef __DALI_V8PLUGIN_ACTOR_API_H__
#define __DALI_V8PLUGIN_ACTOR_API_H__

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
#include <dali/public-api/actors/actor.h>

namespace Dali
{

namespace V8Plugin
{

namespace TextLabelApi
{
  /**
   * Temporary TextView constructor
   */
  Actor New( const v8::FunctionCallbackInfo< v8::Value >& args );
}

namespace ActorApi
{

  /**
   * constructor
   */
  Actor New( const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * Actor API. See actor.h for description of functions
   */
  void IsRoot( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetId( const v8::FunctionCallbackInfo< v8::Value >& args);
  void OnStage( const v8::FunctionCallbackInfo< v8::Value >& args );
  void IsLayer( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetLayer( const v8::FunctionCallbackInfo< v8::Value >& args);
  void AddActor( const v8::FunctionCallbackInfo< v8::Value >& args);
  void RemoveActor( const v8::FunctionCallbackInfo< v8::Value >& args);
  void IsEqualTo( const v8::FunctionCallbackInfo<v8::Value>& args );
  void Unparent( const v8::FunctionCallbackInfo< v8::Value >& args);
  void GetChildAt( const v8::FunctionCallbackInfo< v8::Value >& args);
  void FindChildByName( const v8::FunctionCallbackInfo< v8::Value >& args);
  void FindChildById( const v8::FunctionCallbackInfo< v8::Value >& args);
  void GetParent( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetChildCount( const v8::FunctionCallbackInfo< v8::Value >& args );
  void ScreenToLocal( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetKeyboardFocusable( const v8::FunctionCallbackInfo< v8::Value >& args );
  void IsKeyboardFocusable( const v8::FunctionCallbackInfo< v8::Value >& args );
  void TranslateBy( const v8::FunctionCallbackInfo< v8::Value >& args );
  void RotateBy( const v8::FunctionCallbackInfo< v8::Value >& args );
  void ScaleBy( const v8::FunctionCallbackInfo< v8::Value >& args );

  // new function just for JavaScript API, to apply a PathConstraint to an actor
  void ApplyPathConstraint( const v8::FunctionCallbackInfo< v8::Value >& args );

  // new function just for JavaScript API, to remove a PathConstraint from an actor
  void RemovePathConstraint( const v8::FunctionCallbackInfo< v8::Value >& args );

  // new function just for JavaScript API, to help developers know what type of actor
  // they're dealing with, returns actor name as a string
  void GetActorType( const v8::FunctionCallbackInfo< v8::Value >& args );


}; // namespace ActorApi

} // namespace V8Plugin

} // namespace Dali

#endif // header __DALI_V8PLUGIN_ACTOR_API_H__
