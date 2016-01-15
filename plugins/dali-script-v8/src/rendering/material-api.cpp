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
#include "material-api.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <rendering/material-wrapper.h>
#include <rendering/shader-wrapper.h>
#include <rendering/shader-api.h>
#include <rendering/sampler-wrapper.h>
#include <rendering/sampler-api.h>
#include <image/image-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

/**
 * ## Material API
 *
 * Material is a handle to an object that specifies the visual properties of the renderer.
 *
 * @class Material
 * @extends Handle
 */

Material MaterialApi::GetMaterial( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
  void* ptr = field->Value();

  MaterialWrapper* wrapper = static_cast< MaterialWrapper *>(ptr);
  return wrapper->GetMaterial();
}

Material MaterialApi::GetMaterialFromParams( int paramIndex,
                                               bool& found,
                                               v8::Isolate* isolate,
                                               const v8::FunctionCallbackInfo< v8::Value >& args )
{
  found = false;

  v8::HandleScope handleScope( isolate );
  BaseWrappedObject* wrappedObject = V8Utils::GetWrappedDaliObjectParameter( paramIndex, BaseWrappedObject::MATERIAL, isolate, args );
  if( wrappedObject )
  {
    found = true;
    MaterialWrapper* wrapper = static_cast< MaterialWrapper *>(wrappedObject);
    return wrapper->GetMaterial();
  }
  else
  {
    return Material();
  }
}

/**
 * Create a new material object.
 *
 * @constructor
 * @method Material
 * @for Material
 * @param {Object} shader The shader used by the material
 * @return {Object} Material
 */
Material MaterialApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found( false );
  Shader shader = ShaderApi::GetShaderFromParams( 0, found, isolate, args );
  if( found )
  {
    return Material::New( shader );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing shader from param 0" );
    return Material();
  }
}

/**
 * Set the Shader used by this material
 *
 * @method setShader
 * @for Material
 * @param {Object} shader Handle to the shader
 */
void MaterialApi::SetShader( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Material material = GetMaterial( isolate, args );

  bool found( false );
  Shader shader = ShaderApi::GetShaderFromParams( 0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid shader parameter" );
  }
  else
  {
    return material.SetShader( shader );
  }
}

/**
 * Get the Shader used by this material
 *
 * @method getShader
 * @for Material
 * @return {Object} Shader
 */
void MaterialApi::GetShader( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Material material = GetMaterial( isolate, args );
  Shader shader = material.GetShader();

  // Wrap the shader
  v8::Local<v8::Object> localObject = ShaderWrapper::WrapShader( isolate, shader );
  args.GetReturnValue().Set( localObject );
}

/**
 * Add a new texture to be used by the material
 *
 * @method addTexture
 * @for Material
 * @param {Object} image The image used by this sampler
 * @param {String} uniformName The string with the name of the uniform
 * @param {Object} sampler The sampler to add to this material
 * @return {integer} The index of the texture in the array of textures or -1 if texture can not be added
 */
void MaterialApi::AddTexture( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Material material = GetMaterial( isolate, args );

  bool found( false );
  Image image = V8Utils::GetImageParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing image from param 0" );
    return;
  }

  found = false;
  std::string uniformName = V8Utils::GetStringParameter( PARAMETER_1, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing uniform name from param 1" );
    return;
  }

  found = false;
  Sampler sampler = SamplerApi::GetSamplerFromParams( PARAMETER_2, found, isolate, args );
  if( !found )
  {
    args.GetReturnValue().Set( v8::Integer::New( isolate, material.AddTexture( image, uniformName ) ) );
  }
  else
  {
    args.GetReturnValue().Set( v8::Integer::New( isolate, material.AddTexture( image, uniformName, sampler ) ) );
  }
}

/**
 * Removes a texture from the material
 *
 * @method removeTexture
 * @for Material
 * @param {integer} index The index of the texture in the array of textures
 */
void MaterialApi::RemoveTexture( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Material material = GetMaterial( isolate, args );

  bool found( false );
  int index = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid index parameter" );
  }
  else
  {
    material.RemoveTexture( index );
  }
}

/**
 * Sets the image to be used by a given texture
 * @method setTextureImage
 * @for Material
 * @param {integer} index The index of the texture in the array of textures
 * @param {Object} image The image used by this sampler
 */
void MaterialApi::SetTextureImage( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Material material = GetMaterial( isolate, args );

  bool found( false );
  int index = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid index parameter" );
    return;
  }

  found = false;
  Image image = V8Utils::GetImageParameter( PARAMETER_1, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing image from param 1" );
  }
  else
  {
    material.SetTextureImage(index, image);
  }
}

/**
 * Set the sampler used by a given texture
 * @method setTextureSampler
 * @for Material
 * @param {integer} index The index of the texture in the array of textures
 * @param {Object} sampler The new sampler
 */
void MaterialApi::SetTextureSampler( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Material material = GetMaterial( isolate, args );

  bool found( false );
  int index = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid index parameter" );
    return;
  }

  found = false;
  Sampler sampler = SamplerApi::GetSamplerFromParams( PARAMETER_1, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing sampler from param 1" );
  }
  else
  {
    material.SetTextureSampler(index, sampler);
  }
}

/**
 * Set the uniform name of a given texture
 * @method setTextureUniformName
 * @for Material
 * @param {integer} index The index of the texture in the array of textures
 * @param {string} uniformName The new uniform name
 */
void MaterialApi::SetTextureUniformName( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Material material = GetMaterial( isolate, args );

  bool found( false );
  int index = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid index parameter" );
    return;
  }

  found = false;
  std::string uniformName = V8Utils::GetStringParameter( PARAMETER_1, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid uniform name parameter" );
  }
  else
  {
    material.SetTextureUniformName(index, uniformName);
  }
}

/**
 * Retrive the index of a texture given its uniform name
 * @method getTextureIndex
 * @for Material
 * @param {string} uniformName The uniform name
 * @return {integer} The index in the array of textures or -1 if the texture is not found
 */
void MaterialApi::GetTextureIndex( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Material material = GetMaterial( isolate, args );

  bool found( false );
  std::string uniformName = V8Utils::GetStringParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid uniform name parameter" );
  }
  else
  {
    args.GetReturnValue().Set( v8::Integer::New( isolate, material.GetTextureIndex(uniformName) ) );
  }
}

/**
 * Retrieve the number of textures used by the material
 *
 * @method getNumberOfTextures
 * @for Material
 * @return {integer} The number of textures
 */
void MaterialApi::GetNumberOfTextures( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Material material = GetMaterial( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, material.GetNumberOfTextures() ) );
}

/**
 * Set the culling mode for this material
 *
 * @method setFaceCullingMode
 * @for Material
 * @param {integer} cullingMode The culling mode for this material
 * @example
 *      // face culling mode is one of the following
 *      dali.MATERIAL_NONE                           // None of the faces should be culled
 *      dali.MATERIAL_CULL_BACK                      // Cull back face, back face should never be shown
 *      dali.MATERIAL_CULL_FRONT                     // Cull front face, back face should never be shown
 *      dali.MATERIAL_CULL_BACK_AND_FRONT            // Cull back and front faces, if the geometry is composed of triangles none of the faces will be shown
 *
 *      material.setFaceCullingMode( dali.MATERIAL_NONE );
 */
void MaterialApi::SetFaceCullingMode( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Material material = GetMaterial( isolate, args );

  bool found( false );
  int mode = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid cullingMode parameter" );
  }
  else
  {
    material.SetFaceCullingMode( static_cast<Material::FaceCullingMode>(mode) );
  }
}

/**
 * Set the blending mode.
 *
 * If blending is disabled (BLENDING_OFF) fade in and fade out animations do not work.
 *
 * @for Material
 * @method setBlendMode
 * @param { integer } mode The blending mode.
 * @example
 *      // blend mode is one of the following
 *      dali.BLENDING_OFF       // Blending is disabled.
 *      dali.BLENDING_AUTO      // Blending is enabled if there is alpha channel.
 *      dali.BLENDING_ON        // Blending is enabled.
 *
 *      material.setBlendMode( dali.BLENDING_AUTO );
 */
void MaterialApi::SetBlendMode( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Material material = GetMaterial( isolate, args );

  bool found( false );
  int mode = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid blendMode parameter" );
  }
  else
  {
    material.SetBlendMode( static_cast<Dali::BlendingMode::Type>( mode ) );
  }
}

/**
 * Retrieves the blending mode.
 *
 * @for Material
 * @method getBlendMode
 * @return { integer } blendMode
 * @example returns one of the following:
 *
 *        dali.BLENDING_OFF       // Blending is disabled.
 *        dali.BLENDING_AUTO      // Blending is enabled if there is alpha channel.
 *        dali.BLENDING_ON        // Blending is enabled.
 */
void MaterialApi::GetBlendMode( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Material material = GetMaterial( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, material.GetBlendMode() ) );
}

/**
 * Specify the pixel arithmetic used when the actor is blended.
 *
 * @for Material
 * @method setBlendFunc
 * @param {integer} srcFactorRgb Source Blending RGB
 * @param {integer} destFactorRgb Destination Blending RGB
 * @param {integer} srcFactorAlpha Source Blending Alpha
 * @param {integer} destFactorAlpha Destinatino Blending Alpha
 * @example
 *    //blending constants
 *    dali.BLEND_FACTOR_ZERO
 *    dali.BLEND_FACTOR_ONE
 *    dali.BLEND_FACTOR_SRC_COLOR
 *    dali.BLEND_FACTOR_ONE_MINUS_SRC_COLOR
 *    dali.BLEND_FACTOR_SRC_ALPHA
 *    dali.BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
 *    dali.BLEND_FACTOR_DST_ALPHA
 *    dali.BLEND_FACTOR_ONE_MINUS_DST_ALPHA
 *    dali.BLEND_FACTOR_DST_COLOR
 *    dali.BLEND_FACTOR_ONE_MINUS_DST_COLOR
 *    dali.BLEND_FACTOR_SRC_ALPHA_SATURATE
 *    dali.BLEND_FACTOR_CONSTANT_COLOR
 *    dali.BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR
 *    dali.BLEND_FACTOR_CONSTANT_ALPHA
 *    dali.BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA
 *
 *    material.setBlendFunc( dali.BLEND_FACTOR_CONSTANT_COLOR, dali.BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
 *                           dali.BLEND_FACTOR_CONSTANT_ALPHA, dali.BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA );
 */
void MaterialApi::SetBlendFunc( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Material material = GetMaterial( isolate, args );

  int params[4];
  bool foundAllParams(false);
  V8Utils::ReadIntegerArguments( foundAllParams, &params[0], 4, args, 0 );
  if( foundAllParams )
  {
    material.SetBlendFunc( static_cast< Dali::BlendingFactor::Type>(params[0]),
                           static_cast< Dali::BlendingFactor::Type>(params[1]),
                           static_cast< Dali::BlendingFactor::Type>(params[2]),
                           static_cast< Dali::BlendingFactor::Type>(params[3]) );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid blendFunc parameter");
  }
}

/**
 * Query the pixel arithmetic used when the actor is blended.
 *
 * @for Material
 * @method getBlendFunc
 * @return {Object} Blend properties
 * @example Blend properties object has 4 fields
 *
 *      blendProperties.sourceRgb // source rgb enum
 *      blendProperties.destinationRgb // destination rgb enum
 *      blendProperties.sourceAlpha source // alpha enum
 *      blendProperties.destinationAlpha // destination alpha enum
 */
void MaterialApi::GetBlendFunc( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  // Pass by reference doesn't work in Javascript
  // For now just return a vector 4...

  BlendingFactor::Type srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha;
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Material material = GetMaterial( isolate, args );

  material.GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );

  v8::Local<v8::Object> blendProperties = v8::Object::New( isolate );

  blendProperties->Set( v8::String::NewFromUtf8( isolate, "sourceRgb" ),        v8::Integer::New( isolate, srcFactorRgb) );
  blendProperties->Set( v8::String::NewFromUtf8( isolate, "destinationRgb" ),   v8::Integer::New( isolate, destFactorRgb ) );
  blendProperties->Set( v8::String::NewFromUtf8( isolate, "sourceAlpha" ),      v8::Integer::New( isolate, srcFactorAlpha  ) );
  blendProperties->Set( v8::String::NewFromUtf8( isolate, "destinationAlpha" ), v8::Integer::New( isolate, destFactorAlpha ) );

  args.GetReturnValue().Set( blendProperties );
}

/**
 * Specify the equation used when the actor is blended.
 *
 * @for Material
 * @method setBlendEquation
 * @param { integer } equationRgb The equation used for combining red, green, and blue components.
 * @param { integer } equationAlpha The equation used for combining the alpha component.
 * @example
 *      // blend equation is one of the following
 *      dali.BLEND_EQUATION_ADD
 *      dali.BLEND_EQUATION_SUBTRACT
 *      dali.BLEND_EQUATION_REVERSE_SUBTRACT
 *
 *      material.setBlendEquation( dali.BLEND_EQUATION_ADD, dali.BLEND_EQUATION_REVERSE_SUBTRACT );
 */
void MaterialApi::SetBlendEquation( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Material material = GetMaterial( isolate, args );

  int params[2];
  bool foundAllParams(false);
  V8Utils::ReadIntegerArguments( foundAllParams, &params[0], 2, args, 0 );
  if( foundAllParams )
  {
    material.SetBlendEquation( static_cast< BlendingEquation::Type>(params[0]), static_cast< BlendingEquation::Type>(params[1]) );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid BlendEquation parameter");
  }
}

/**
 * Query the equation used when the actor is blended.
 *
 * @for Material
 * @method getBlendEquation
 * @return {Object} Blend equations
 * @example Blend equations object has 2 fields
 *
 *      blendEquations.equationRgb // equation used for combining rgb components
 *      blendEquations.equationAlpha // equation used for combining alpha components
 */
void MaterialApi::GetBlendEquation( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  // Pass by reference doesn't work in Javascript
  // For now just return a vector 2...

  BlendingEquation::Type equationRgb, equationAlpha;
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Material material = GetMaterial( isolate, args );

  material.GetBlendEquation( equationRgb, equationAlpha );

  v8::Local<v8::Object> blendEquations = v8::Object::New( isolate );

  blendEquations->Set( v8::String::NewFromUtf8( isolate, "equationRgb" ),   v8::Integer::New( isolate, equationRgb) );
  blendEquations->Set( v8::String::NewFromUtf8( isolate, "equationAlpha" ), v8::Integer::New( isolate, equationAlpha ) );

  args.GetReturnValue().Set( blendEquations );
}

} // namespace V8Plugin

} // namespace Dali
