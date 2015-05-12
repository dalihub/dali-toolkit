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
#include "renderable-actor-api.h"

// INTERNAL INCLUDES
#include <object/handle-wrapper.h>
#include <v8-utils.h>
#include <object/property-value-wrapper.h>
#include <shader-effects/shader-effect-api.h>
#include <shader-effects/shader-effect-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

namespace //unnamed name space
{
RenderableActor GetRenderableActor( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  HandleWrapper* handleWrapper = HandleWrapper::Unwrap( isolate, args.This() );
  return RenderableActor::DownCast( handleWrapper->mHandle );
}
}
/***************************************
 * RENDERABLE ACTOR FUNCTIONS
 *
 ****************************************/
/**
 * Allows modification of an actors position in the depth sort algorithm.
 *
 * The offset can be altered for each coplanar actor hence allowing an order of painting.
 * @param { Number }  depthOffset the offset to be given to the actor. Positive values pushing it further back.
 * @for RenderableActor
 * @method setSortModifier
 */
void RenderableActorApi::SetSortModifier( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderableActor actor = GetRenderableActor( isolate, args );

  bool found( false );
  float value = V8Utils::GetFloatParameter( PARAMETER_0, found, isolate, args, 0.f );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
    return;
  }
  actor.SetSortModifier( value );
}

/**
 * Retrieves the offset used to modify an actors position in the depth sort algorithm.
 * @for RenderableActor
 * @method getSortModifier .
 * @return { Number} the offset that has been given to the actor. Positive values pushing it further back
 */
void RenderableActorApi::GetSortModifier( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderableActor actor = GetRenderableActor( isolate, args );

  args.GetReturnValue().Set( v8::Number::New( isolate, actor.GetSortModifier() ) );

}

/**
 * Set the face-culling mode for this actor.
 * @for RenderableActor
 * @method setCullFace
 * @param {Number} cullMode
 * @example
 *      // cull mode should be one of the following constants
 *      dali.CULL_FACE_DISABLE        // Face culling disabled
 *      dali.CULL_FRONT_FACE          // Cull front facing polygons
 *      dali.CULL_BACK_FACE           // Cull back facing polygons
 *      dali.CULL_FRONT_AND_BACK_FACE // Cull front and back facing polygons
 *      actor.SetCullFace( dali.CULL_FRONT_FACE );
 */
void RenderableActorApi::SetCullFace( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderableActor actor = GetRenderableActor( isolate, args );

  bool found( false );
  int cullMode = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
    return;
  }

  actor.SetCullFace(  static_cast<Dali::CullFaceMode>( cullMode ) );

}

/**
 * Retrieve the face-culling mode for this actor.
 * @for RenderableActor
 * @method getCullFace
 * @return {Number} cullMode
 * @example
 *      // cull mode is one of the following
 *      dali.CULL_FACE_DISABLE        // Face culling disabled
 *      dali.CULL_FRONT_FACE          // Cull front facing polygons
 *      dali.CULL_BACK_FACE           // Cull back facing polygons
 *      dali.CULL_FRONT_AND_BACK_FACE // Cull front and back facing polygon
 */
void RenderableActorApi::GetCullFace( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderableActor actor = GetRenderableActor( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, actor.GetCullFace() ) );

}

/**
 * Sets the blending mode.
 *
 * If blending is disabled (BLENDING_OFF) fade in and fade out animations do not work.
 *
 * @example
 *      // blend mode is one of the following
 *      dali.BLENDING_OFF       // Blending is disabled.
 *      dali.BLENDING_AUTO      // Blending is enabled if there is alpha channel.
 *      dali.BLENDING_ON        // Blending is enabled.
 *      actor.SetBlendMode( dali.BLENDING_AUTO );
 *
 * @for RenderableActor
 * @method setBlendMode
 * @param { Number } blendMode
 */
void RenderableActorApi::SetBlendMode( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderableActor actor = GetRenderableActor( isolate, args );

  bool found( false );
  int mode = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid BlendMode parameter" );
    return;
  }
  actor.SetBlendMode( static_cast<Dali::BlendingMode::Type>( mode ) );

}

/**
 * @for RenderableActor
 * @method getBlendMode
 * @return { Number } blendMode
 * @example returns one of the following:
 *
 *        dali.BLENDING_OFF       // Blending is disabled.
 *        dali.BLENDING_AUTO      // Blending is enabled if there is alpha channel.
 *        dali.BLENDING_ON        // Blending is enabled.
 *
 */
void RenderableActorApi::GetBlendMode( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderableActor actor = GetRenderableActor( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, actor.GetBlendMode() ) );

}

/**
 * @for RenderableActor
 * @method setBlendFunc
 * @param {Number} SourceBlending RGB
 * @param {Number} DestinationBlending RGB
 * @param {Number} SourceBlending Alpha
 * @param {Number} DestinatinoBlending Alpha
 * @example
 *      //blending constants
      dali.BLEND_FACTOR_ZERO
      dali.BLEND_FACTOR_ONE
      dali.BLEND_FACTOR_SRC_COLOR
      dali.BLEND_FACTOR_ONE_MINUS_SRC_COLOR
      dali.BLEND_FACTOR_SRC_ALPHA
      dali.BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
      dali.BLEND_FACTOR_DST_ALPHA
      dali.BLEND_FACTOR_ONE_MINUS_DST_ALPHA
      dali.BLEND_FACTOR_DST_COLOR
      dali.BLEND_FACTOR_ONE_MINUS_DST_COLOR
      dali.BLEND_FACTOR_SRC_ALPHA_SATURATE
      dali.BLEND_FACTOR_CONSTANT_COLOR
      dali.BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR
      dali.BLEND_FACTOR_CONSTANT_ALPHA
      dali.BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA

      actor.setBlendFunc(  dali.BLEND_FACTOR_CONSTANT_COLOR, BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
                                dali.BLEND_FACTOR_CONSTANT_ALPHA, BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA);

      );
 */
void RenderableActorApi::SetBlendFunc( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderableActor actor =  GetRenderableActor( isolate, args );

  int params[4];
  bool foundAllParams(false);
  V8Utils::ReadIntegerArguments( foundAllParams, &params[0], 4, args,0 );
  if( foundAllParams )
  {
    actor.SetBlendFunc(  static_cast< Dali::BlendingFactor::Type>(params[0]),
                         static_cast< Dali::BlendingFactor::Type>(params[1]),
                         static_cast< Dali::BlendingFactor::Type>(params[2]),
                         static_cast< Dali::BlendingFactor::Type>(params[3]));
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid BlendMode parameter");
    return;
  }
}

/**
 * @for RenderableActor
 * @method GetBlendFunc
 * @return {Object} BlendProperties
 * @example Blend properties object has 4 fields
 *
 *      blendProperties.sourceRgb // source rgb enum
 *      blendProperties.destinationRgb  // destination rgb enum
 *      blendProperties.sourceAlpha source // alpha enum
 *      blendProperties.destinationAlpha // destination alpha enum
 */
void RenderableActorApi::GetBlendFunc( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  // @todo pass by reference doesn't work in Javascript so need to decide what to return
  // for now just return a vector 4...

  BlendingFactor::Type srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha;
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderableActor actor = GetRenderableActor( isolate, args );

  actor.GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );

  v8::Local<v8::Object> blendInfo = v8::Object::New( isolate );

  blendInfo->Set( v8::String::NewFromUtf8( isolate, "sourceRgb" ),        v8::Integer::New( isolate, srcFactorRgb) );
  blendInfo->Set( v8::String::NewFromUtf8( isolate, "destinationRgb" ),   v8::Integer::New( isolate, destFactorRgb ) );
  blendInfo->Set( v8::String::NewFromUtf8( isolate, "sourceAlpha" ),      v8::Integer::New( isolate, srcFactorAlpha  ) );
  blendInfo->Set( v8::String::NewFromUtf8( isolate, "destinationAlpha" ), v8::Integer::New( isolate, destFactorAlpha ) );

  args.GetReturnValue().Set( blendInfo );

}

/**
 * @for RenderableActor
 * @method getShaderEffect
 * @return {Object} ShaderEffect object
 *
 * Retrieve the shader effect for the Actor.
 *
 * @example
 *    var shaderEffect = actor.getShaderEffect();
 *
 */
void RenderableActorApi::GetShaderEffect( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderableActor actor = GetRenderableActor( isolate, args );

  v8::Local < v8::Object > object = ShaderEffectWrapper::WrapShaderEffect( isolate, actor.GetShaderEffect() );
  args.GetReturnValue().Set( object );

}

/**
 * @for RenderableActor
 * @method setShaderEffect
 * @param {Object} shaderEffect The shader effect.
 *
 * Sets the shader effect for the Actor.
 *
 * Shader effects provide special effects like rippling and bending.
 * Setting a shader effect removes any shader effect previously set by SetShaderEffect.
 * @example
 *      // first create the shaderOptions, then the shaderEffect
 *      var shader = new dali.ShaderEffect( shaderOptions );
 *      actor.setShaderEffect( shader );
 *
 */
void RenderableActorApi::SetShaderEffect( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderableActor actor = GetRenderableActor( isolate, args );

  bool found( false );
  ShaderEffect effect = ShaderEffectApi::GetShaderEffectFromParams( 0, found, isolate, args );
  if( found )
  {
    actor.SetShaderEffect( effect );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "shader effect parameter missing" );
  }
}

/**
 *
 * Removes the current shader effect
 *
 * @example
 *    actor.removeShaderEffect();
 *
 * @for RenderableActor
 * @method removeShaderEffect
 */
void RenderableActorApi::RemoveShaderEffect( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderableActor actor = GetRenderableActor( isolate, args );
  actor.RemoveShaderEffect();
}



} // namespace V8Plugin

} // namespace Dali
