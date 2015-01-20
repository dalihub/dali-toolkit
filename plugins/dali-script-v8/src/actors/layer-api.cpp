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
#include "layer-api.h"

// INTERNAL INCLUDES
#include <object/handle-wrapper.h>
#include <v8-utils.h>

namespace Dali
{

namespace V8Plugin
{

namespace //unnamed name space
{

Layer GetLayer( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  HandleWrapper* handleWrapper = HandleWrapper::Unwrap( isolate, args.This() );
  return Layer::DownCast( handleWrapper->mHandle );
}

} //unnamed name space

/***************************************
 * LAYER FUNCTIONS
 *
 ****************************************/
/**
 * Constructor
 *
 * @constructor
 * @method Layer
 * @for Layer
 * @return {Object} Layer
 */
Actor LayerApi::New( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  return Layer::New();
}

/**
 *  Query the depth of the layer
 *
 * 0 is bottom most layer, higher number is on top
 * Condition: layer is on the stage
 * If layer is not added to the stage, returns 0.
 * @return  {Number}  the current depth of the layer.
 */
void LayerApi::GetDepth( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Layer layer = GetLayer( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, layer.GetDepth() ) );

}

/**
 * Increment the depth of the layer.
 * Condition: layer is on the stage
 * @method Raise
 * @for Layer
 *
 */
void LayerApi::Raise( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Layer layer = GetLayer( isolate, args );
  layer.Raise();
}

/**
 * Decrement the depth of the layer.
 * Condition: layer is on the stage
 * @for Layer
 * @method lower
 */
void LayerApi::Lower( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Layer layer = GetLayer( isolate, args );
  layer.Lower();
}

/**
 *  Ensures the layers depth is greater than the target layer.
 *
 * If the layer already is above target layer its depth is not changed
 * If the layer was below target, its new depth will be immediately above target
 * Note! All layers between this layer and target get new depth values
 * Condition: layer is on the stage
 * Condition: target layer is on the stage
 * @for Layer
 * @method raiseAbove
 * @param target layer to get above of
 */
void LayerApi::RaiseAbove( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Layer layer = GetLayer( isolate, args );
  bool found(false);

  Layer target = V8Utils::GetLayerParameter( PARAMETER_0, found, isolate, args );
  if( found )
  {
    layer.RaiseAbove( target );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "layer param not found");
  }
}

/**
 *  Ensures the layers depth is less than the target layer.
 *
 * If the layer already is below the layer its depth is not changed
 * If the layer was above target, its new depth will be immediately below target
 * Note! All layers between this layer and target get new depth values
 * Conditions: layer is on the stage, target layer is on the stage
 * @param target layer to get below of
 * @for Layer
 * @method lowerBelow
 */
void LayerApi::LowerBelow( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Layer layer = GetLayer( isolate, args );
  bool found( false );

  Layer target = V8Utils::GetLayerParameter( PARAMETER_0, found, isolate, args );

  if( found )
  {
    layer.LowerBelow( target );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "layer param not found");
  }

}

/**
 * Raises the layer to the top.
 * Conditions:  layer is on the stage
 * @for Layer
 * @method raiseToTop
 */
void LayerApi::RaiseToTop( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Layer layer = GetLayer( isolate, args );
  layer.RaiseToTop();
}

/**
 * Lowers the layer to the bottom.
 * Conditions:  layer is on the stage
 * @for Layer
 * @method lowerToBottom
 */
void LayerApi::ToBottom( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Layer layer = GetLayer( isolate, args );
  layer.LowerToBottom();
}

/**
 *  Moves the layer directly above the given layer.
 *
 * After the call this layers depth will be immediately above target
 * Note! All layers between this layer and target get new depth values
 * Conditions: layer is on the stage, target layer is on the stage
 * @param target layer to get on top of
 * @for Layer
 * @method moveAbove
 */
void LayerApi::MoveAbove( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Layer layer = GetLayer( isolate, args );
  bool found( false );

  Layer target = V8Utils::GetLayerParameter( PARAMETER_0, found, isolate, args );
  if( found )
  {
    layer.MoveAbove( target );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "layer param not found");
  }
}

/**
 *  Moves the layer directly below the given layer.
 *
 * After the call this layers depth will be immediately below target
 * Note! All layers between this layer and target get new depth values
 * Condition: layer is on the stage
 * Condition: target layer is on the stage
 * @param target layer to get below of
 * @for Layer
 * @method moveBelow
 */
void LayerApi::MoveBelow( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Layer layer = GetLayer( isolate, args );
  bool found( false );

  Layer target = V8Utils::GetLayerParameter( PARAMETER_0, found, isolate, args );

  if( found )
  {
    layer.MoveBelow( target );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "layer param not found");
  }
}

// should really be a property
/**
 *  Whether to disable the depth test.
 *
 * By default a layer enables depth test if there is more than one opaque actor or if there is one opaque actor and one, or more, transparent actors.
 * However, it's possible to disable the depth test by calling this method.
 *
 * @param {Boolean}  disable true disables depth test. false sets the default behaviour.
 * @for Layer
 * @method setDepthTestDisabled
 */
void LayerApi::SetDepthTestDisabled( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Layer layer = GetLayer( isolate, args );
  bool parameterFound( false );
  bool depthTest = V8Utils::GetBooleanParameter( PARAMETER_0, parameterFound, isolate, args );
  if( !parameterFound )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
    return;
  }
  layer.SetDepthTestDisabled( depthTest );

}

/**
 *  Retrieves whether depth test is disabled.
 *
 * @return { Boolean}  true if depth test is disabled.
 * @for Layer
 * @method isDepthTestDisabled
 */
void LayerApi::IsDepthTestDisabled( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Layer layer = GetLayer( isolate, args );
  args.GetReturnValue().Set( v8::Boolean::New( isolate, layer.IsDepthTestDisabled() ) );
}



} // namespace V8Plugin

} // namespace Dali
