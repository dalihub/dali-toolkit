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

// CLASS HEADER
#include "stage-api.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <object/property-value-wrapper.h>
#include <actors/actor-wrapper.h>
#include <actors/actor-api.h>
#include <render-tasks/render-task-list-wrapper.h>


namespace Dali
{

namespace V8Plugin
{


/***************************************
 * STAGE FUNCTIONS
 *
 ****************************************/
Dali::Stage StageApi::GetStage(v8::Isolate* isolate, const v8::FunctionCallbackInfo<v8::Value>& args)
{
  // the stage object should be in args, but for now just use GetCurrent
  return Stage::GetCurrent();
}

/**
 * Adds a child Actor to the Stage.
 * @method add
 * @for Stage
 * @param {Object} Actor
 */
void StageApi::Add( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Stage stage = GetStage( isolate, args );
  bool found( false );
  Actor actor = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );
  if( found )
  {
    stage.Add( actor );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing actor parameter");
  }
}

/**
 * Removes a child Actor to the Stage.
 * @method remove
 * @for Stage
 * @param {Object} Actor
 */
void StageApi::Remove( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Stage stage = GetStage( isolate, args );
  bool found( false );

  Actor actor = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );
  if( found )
  {
    stage.Remove( actor );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing actor parameter");
  }
}
/**
 * Get the size of the stage
 * @method getSize
 * @for Stage
 * @return {Object} size with properties { x: , y: }
 */
void StageApi::GetSize( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Stage stage = GetStage( isolate, args );
  Vector2 size( stage.GetSize() );

  v8::Local<v8::Object> sizeObject = v8::Object::New( isolate );
  sizeObject->Set( v8::String::NewFromUtf8( isolate, "x"), v8::Number::New( isolate,size.width ));
  sizeObject->Set( v8::String::NewFromUtf8( isolate, "y"), v8::Number::New( isolate,size.height ));

  // set return value
  args.GetReturnValue().Set( sizeObject );

}
/**
 * Retrieve the list of render-tasks.
 * @method getRenderTaskList
 * @for Stage
 * @return {Object} render task list
 */
void StageApi::GetRenderTaskList( const v8::FunctionCallbackInfo< v8::Value >& args  )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Stage stage = GetStage( isolate, args );

  RenderTaskList taskList = stage.GetRenderTaskList();

  v8::Local <v8::Object> object = RenderTaskListWrapper::WrapRenderTaskList( isolate, taskList );

  args.GetReturnValue().Set(  object );
}
/**
 * Query the number of on-stage layers.
 * Note that a default layer is always provided (count >= 1).
 * @method getLayerCount
 * @for Stage
 * @return {Number} number of layers
 */
void StageApi::GetLayerCount( const v8::FunctionCallbackInfo<v8::Value>& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Stage stage = GetStage( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, stage.GetLayerCount() ) );

}
/**
 * Retrieve the list of render-tasks.
 * @method getLayer
 * @for Stage
 * @param {Integer} depth
 * @return {Object} layer
 */
void StageApi::GetLayer( const v8::FunctionCallbackInfo< v8::Value >& args  )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Stage stage = GetStage( isolate, args );

  bool found( false );
  unsigned int depth = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Integer parameter missing" );
    return;
  }
  Layer rootLayer = stage.GetLayer( depth );

  v8::Handle < v8::Object > wrappedLayer = ActorWrapper::WrapActor( isolate, rootLayer );
  args.GetReturnValue().Set( wrappedLayer );
}
/**
 * Returns the Stage's Root Layer.
 * @method getRootLayer
 * @for Stage
 * @return {Object} root layer
 */
void StageApi::GetRootLayer( const v8::FunctionCallbackInfo<v8::Value>& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Stage stage = GetStage( isolate, args );

  Layer rootLayer = stage.GetRootLayer();

  v8::Handle < v8::Object > wrappedLayer = ActorWrapper::WrapActor( isolate, rootLayer );
  args.GetReturnValue().Set( wrappedLayer );
}

/**
 * Set the background color of the stage
 * @method setBackgroundColor
 * @for Stage
 * @param {Object} Array of [Red,Green,Blue,Alpha], e.g. dali.COLOR_RED, or [1,0,0,1] (full red and alpha);
 * @example
 *     dali.stage.setBackgroundColor( dali.COLOR_RED );
 *
 *     dali.stage.setBackgroundColor( [0.5,0.5,0.5,1 ] ); // set the background to grey
 */
void StageApi::SetBackgroundColor( const v8::FunctionCallbackInfo<v8::Value>& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Stage stage = GetStage( isolate, args );
  bool found( false );

  Vector4 backgroundColor = V8Utils::GetVector4Parameter( PARAMETER_0, found, isolate, args);
  if (!found)
  {
    DALI_SCRIPT_EXCEPTION( isolate, "no parameter not found" );
    return;
  }
  stage.SetBackgroundColor( backgroundColor );
}
/**
 * Get the background color of the stage
 * @method getBackgroundColor
 * @for Stage
 * @return {Object} object that contains the properties [red:,green:,blue,alpha:]
 */
void StageApi::GetBackgroundColor( const v8::FunctionCallbackInfo<v8::Value>& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Stage stage = GetStage( isolate, args );

  Vector4 color = stage.GetBackgroundColor();

  v8::Local<v8::Object> colorObject = v8::Object::New( isolate );

  colorObject->Set( v8::String::NewFromUtf8( isolate, "red"),   v8::Number::New( isolate,color.r));
  colorObject->Set( v8::String::NewFromUtf8( isolate, "green"), v8::Number::New( isolate,color.g));
  colorObject->Set( v8::String::NewFromUtf8( isolate, "blue"),  v8::Number::New( isolate,color.b));
  colorObject->Set( v8::String::NewFromUtf8( isolate, "alpha"), v8::Number::New( isolate,color.a));

  // set return value
  args.GetReturnValue().Set( colorObject );


}
/**
 * Retrieve the DPI of the display device to which the stage is connected.
 * @method getDpi
 * @for Stage
 * @return {Object} object that contains the properties [x:,y:]
 */
void StageApi::GetDpi( const v8::FunctionCallbackInfo<v8::Value>& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Stage stage = GetStage( isolate, args );

  Vector2 dpi( stage.GetDpi() );

  v8::Local<v8::Object> dpiObject = v8::Object::New( isolate );
  dpiObject->Set( v8::String::NewFromUtf8( isolate, "x"),   v8::Number::New( isolate,dpi.x));
  dpiObject->Set( v8::String::NewFromUtf8( isolate, "y"), v8::Number::New( isolate,dpi.y));


  args.GetReturnValue().Set(  dpiObject );
}


} // namespace V8Plugin

} // namespace Dali
