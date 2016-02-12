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

} // namespace V8Plugin

} // namespace Dali
