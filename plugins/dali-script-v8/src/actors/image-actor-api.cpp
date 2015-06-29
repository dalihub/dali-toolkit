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
#include "image-actor-api.h"

// INTERNAL INCLUDES
#include <object/handle-wrapper.h>
#include <v8-utils.h>
#include <object/property-value-wrapper.h>
#include <image/image-api.h>
#include <image/image-wrapper.h>
#include <shader-effects/shader-effect-api.h>
#include <shader-effects/shader-effect-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

namespace //unnamed name space
{

ImageActor GetImageActor( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  HandleWrapper* handleWrapper = HandleWrapper::Unwrap( isolate, args.This() );
  return ImageActor::DownCast( handleWrapper->mHandle );
}

} //unnamed name space

/**
 * Constructor
 *
 * @constructor
 * @method ImageActor
 * @for ImageActor
 * @param {Object} [image] Image object
 * @param {Object} [pixelArea] Vector4
 * @return {Object} ImageActor
 * @example
 *    new DALI.imageActor( image, [10,23,35,56] );
 */
Actor ImageActorApi::New( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  // Image actor has 3 different constructors
  // New();
  // New( image);
  // New( image, PixelArea pixelArea);

  if( args.Length() == 0 )
  {
    return ImageActor::New();
  }

  // args.Length> 0, must have an Image parameter
  bool found( false );
  Image image = V8Utils::GetImageParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing image from param 0" );
    return ImageActor();
  }

  // check for PixelArea, accept a DALI Vector4 object ( which can be a JavaScript array)
  // e.g.  new DALI.imageActor( image, [10,23,35,56] );
  // or    new DALI.imageActor( image, Vector4 );

  if( args.Length() > 1 )
  {
    Vector4 rect = V8Utils::GetVector4Parameter( PARAMETER_1, found, isolate, args );
    if( !found )
    {
      DALI_SCRIPT_EXCEPTION( isolate, " bad parameters" );
      return ImageActor();
    }
    Rect<int>rectangle( static_cast<int>(rect.x),
                        static_cast<int>(rect.y),
                        static_cast<int>(rect.z),
                        static_cast<int>(rect.w));

    return ImageActor::New( image, rectangle );
  }
  else
  {
    return ImageActor::New( image );
  }
}

/**
 * Set the image rendered by the actor.
 *
 * When the image is loaded the actor's size will be reset to the image size,
 * unless a custom size was chosen, e.g. via actor.size or a pixel area
 * was set.
 * Note: The old image will continue to be displayed until the given image has loaded.
 * @for ImageActor
 * @method setImage
 * @param {Object} image The image to display.
 *
 */
void ImageActorApi::SetImage( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found( false );
  Image image = V8Utils::GetImageParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameters" );
    return;
  }
  ImageActor imageActor = GetImageActor( isolate, args );
  imageActor.SetImage( image );
}

/**
 * brief Retrieve the image rendered by the actor's attachment.
 * @for ImageActor
 * @method getImage
 * @return {Object} the image.
 */
void ImageActorApi::GetImage( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageActor imageActor = GetImageActor( isolate, args );
  Image image = imageActor.GetImage();

  // wrap the image
  v8::Local<v8::Object> localObject = ImageWrapper::WrapImage( isolate, image );
  args.GetReturnValue().Set( localObject );
}

/**
 * Allows modification of an actors position in the depth sort algorithm.
 *
 * The offset can be altered for each coplanar actor hence allowing an order of painting.
 * @param { Number }  depthOffset the offset to be given to the actor. Positive values pushing it further back.
 * @for ImageActor
 * @method setSortModifier
 */
void ImageActorApi::SetSortModifier( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageActor imageActor = GetImageActor( isolate, args );

  bool found( false );
  float value = V8Utils::GetFloatParameter( PARAMETER_0, found, isolate, args, 0.f );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
    return;
  }
  imageActor.SetSortModifier( value );
}

/**
 * Retrieves the offset used to modify an actors position in the depth sort algorithm.
 * @for ImageActor
 * @method getSortModifier .
 * @return { Number} the offset that has been given to the actor. Positive values pushing it further back
 */
void ImageActorApi::GetSortModifier( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageActor imageActor = GetImageActor( isolate, args );

  args.GetReturnValue().Set( v8::Number::New( isolate, imageActor.GetSortModifier() ) );

}

/**
 * Set the face-culling mode for this actor.
 * @for ImageActor
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
void ImageActorApi::SetCullFace( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageActor imageActor = GetImageActor( isolate, args );

  bool found( false );
  int cullMode = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
    return;
  }

  imageActor.SetCullFace(  static_cast<Dali::CullFaceMode>( cullMode ) );

}

/**
 * Retrieve the face-culling mode for this actor.
 * @for ImageActor
 * @method getCullFace
 * @return {Number} cullMode
 * @example
 *      // cull mode is one of the following
 *      dali.CULL_FACE_DISABLE        // Face culling disabled
 *      dali.CULL_FRONT_FACE          // Cull front facing polygons
 *      dali.CULL_BACK_FACE           // Cull back facing polygons
 *      dali.CULL_FRONT_AND_BACK_FACE // Cull front and back facing polygon
 */
void ImageActorApi::GetCullFace( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageActor imageActor = GetImageActor( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, imageActor.GetCullFace() ) );

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
 * @for ImageActor
 * @method setBlendMode
 * @param { Number } blendMode
 */
void ImageActorApi::SetBlendMode( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageActor imageActor = GetImageActor( isolate, args );

  bool found( false );
  int mode = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid BlendMode parameter" );
    return;
  }
  imageActor.SetBlendMode( static_cast<Dali::BlendingMode::Type>( mode ) );

}

/**
 * @for ImageActor
 * @method getBlendMode
 * @return { Number } blendMode
 * @example returns one of the following:
 *
 *        dali.BLENDING_OFF       // Blending is disabled.
 *        dali.BLENDING_AUTO      // Blending is enabled if there is alpha channel.
 *        dali.BLENDING_ON        // Blending is enabled.
 *
 */
void ImageActorApi::GetBlendMode( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageActor imageActor = GetImageActor( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, imageActor.GetBlendMode() ) );

}

/**
 * @for ImageActor
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
void ImageActorApi::SetBlendFunc( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageActor imageActor = GetImageActor( isolate, args );

  int params[4];
  bool foundAllParams(false);
  V8Utils::ReadIntegerArguments( foundAllParams, &params[0], 4, args,0 );
  if( foundAllParams )
  {
    imageActor.SetBlendFunc(  static_cast< Dali::BlendingFactor::Type>(params[0]),
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
 * @for ImageActor
 * @method GetBlendFunc
 * @return {Object} BlendProperties
 * @example Blend properties object has 4 fields
 *
 *      blendProperties.sourceRgb // source rgb enum
 *      blendProperties.destinationRgb  // destination rgb enum
 *      blendProperties.sourceAlpha source // alpha enum
 *      blendProperties.destinationAlpha // destination alpha enum
 */
void ImageActorApi::GetBlendFunc( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  // @todo pass by reference doesn't work in Javascript so need to decide what to return
  // for now just return a vector 4...

  BlendingFactor::Type srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha;
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageActor imageActor = GetImageActor( isolate, args );

  imageActor.GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );

  v8::Local<v8::Object> blendInfo = v8::Object::New( isolate );

  blendInfo->Set( v8::String::NewFromUtf8( isolate, "sourceRgb" ),        v8::Integer::New( isolate, srcFactorRgb) );
  blendInfo->Set( v8::String::NewFromUtf8( isolate, "destinationRgb" ),   v8::Integer::New( isolate, destFactorRgb ) );
  blendInfo->Set( v8::String::NewFromUtf8( isolate, "sourceAlpha" ),      v8::Integer::New( isolate, srcFactorAlpha  ) );
  blendInfo->Set( v8::String::NewFromUtf8( isolate, "destinationAlpha" ), v8::Integer::New( isolate, destFactorAlpha ) );

  args.GetReturnValue().Set( blendInfo );

}

/**
 * @for ImageActor
 * @method getShaderEffect
 * @return {Object} ShaderEffect object
 *
 * Retrieve the shader effect for the Actor.
 *
 * @example
 *    var shaderEffect = actor.getShaderEffect();
 *
 */
void ImageActorApi::GetShaderEffect( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageActor imageActor = GetImageActor( isolate, args );

  v8::Local < v8::Object > object = ShaderEffectWrapper::WrapShaderEffect( isolate, imageActor.GetShaderEffect() );
  args.GetReturnValue().Set( object );

}

/**
 * @for ImageActor
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
void ImageActorApi::SetShaderEffect( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageActor imageActor = GetImageActor( isolate, args );

  bool found( false );
  ShaderEffect effect = ShaderEffectApi::GetShaderEffectFromParams( 0, found, isolate, args );
  if( found )
  {
    imageActor.SetShaderEffect( effect );
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
 * @for ImageActor
 * @method removeShaderEffect
 */
void ImageActorApi::RemoveShaderEffect( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageActor imageActor = GetImageActor( isolate, args );
  imageActor.RemoveShaderEffect();
}

} // namespace V8Plugin

} // namespace Dali
