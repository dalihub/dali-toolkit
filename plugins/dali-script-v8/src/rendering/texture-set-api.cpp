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
#include "texture-set-api.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <rendering/texture-set-wrapper.h>
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
 * ## TextureSet API
 *
 * TextureSet is a handle to an object that contains the textures used by a renderer
 *
 * @class TextureSet
 * @extends Handle
 */

TextureSet TextureSetApi::GetTextureSet( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
  void* ptr = field->Value();

  TextureSetWrapper* wrapper = static_cast< TextureSetWrapper *>(ptr);
  return wrapper->GetTextureSet();
}

TextureSet TextureSetApi::GetTextureSetFromParams( int paramIndex,
                                               bool& found,
                                               v8::Isolate* isolate,
                                               const v8::FunctionCallbackInfo< v8::Value >& args )
{
  found = false;

  v8::HandleScope handleScope( isolate );
  BaseWrappedObject* wrappedObject = V8Utils::GetWrappedDaliObjectParameter( paramIndex, BaseWrappedObject::TEXTURE_SET, isolate, args );
  if( wrappedObject )
  {
    found = true;
    TextureSetWrapper* wrapper = static_cast< TextureSetWrapper *>(wrappedObject);
    return wrapper->GetTextureSet();
  }
  else
  {
    return TextureSet();
  }
}

/**
 * Create a new texture set object.
 *
 * @constructor
 * @method TextureSet
 * @for TextureSet
 * @return {Object} TextureSet
 */
TextureSet TextureSetApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  return TextureSet::New();
}


/**
 * Sets the image to be used by a given texture
 * @method setImage
 * @for TextureSet
 * @param {integer} index The index of the texture in the array of textures
 * @param {Object} image The image used by this sampler
 */
void TextureSetApi::SetImage( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  TextureSet textureSet = GetTextureSet( isolate, args );

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
    textureSet.SetImage(index, image);
  }
}

/**
 * Set the sampler used by a given texture
 * @method setSampler
 * @for TextureSet
 * @param {integer} index The index of the texture in the array of textures
 * @param {Object} sampler The new sampler
 */
void TextureSetApi::SetSampler( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  TextureSet textureSet = GetTextureSet( isolate, args );

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
    textureSet.SetSampler(index, sampler);
  }
}

} // namespace V8Plugin

} // namespace Dali
