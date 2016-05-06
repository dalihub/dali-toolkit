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
#include "renderer-api.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <rendering/renderer-wrapper.h>
#include <rendering/geometry-api.h>
#include <rendering/geometry-wrapper.h>
#include <rendering/texture-set-api.h>
#include <rendering/texture-set-wrapper.h>
#include <rendering/shader-api.h>

namespace Dali
{

namespace V8Plugin
{

/**
 * ## Renderer API
 *
 * Renderer is a handle to an object that can be used to provide an image to a material.
 *
 * @class Renderer
 * @extends Handle
 */

Renderer RendererApi::GetRenderer( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
  void* ptr = field->Value();

  RendererWrapper* wrapper = static_cast< RendererWrapper *>(ptr);
  return wrapper->GetRenderer();
}

Renderer RendererApi::GetRendererFromParams( int paramIndex,
                                             bool& found,
                                             v8::Isolate* isolate,
                                             const v8::FunctionCallbackInfo< v8::Value >& args )
{
  found = false;

  v8::HandleScope handleScope( isolate );
  BaseWrappedObject* wrappedObject = V8Utils::GetWrappedDaliObjectParameter( paramIndex, BaseWrappedObject::RENDERER, isolate, args );
  if( wrappedObject )
  {
    found = true;
    RendererWrapper* wrapper = static_cast< RendererWrapper *>(wrappedObject);
    return wrapper->GetRenderer();
  }
  else
  {
    return Renderer();
  }
}

/**
 * Create a new renderer object.
 *
 * @constructor
 * @method Renderer
 * @for Renderer
 * @param {Object} geometry The geometry to be used by this renderer
 * @param {Object} shader The shader to be used by this renderer
 * @return {Object} Renderer
 */
Renderer RendererApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found( false );
  Geometry geometry = GeometryApi::GetGeometryFromParams( 0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing geometry from param 0" );
    return Renderer();
  }

  found = false;
  Shader shader = ShaderApi::GetShaderFromParams( 1, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing shader from param 0" );
    return Renderer();
  }

  return Renderer::New(geometry, shader);
}

/**
 * Sets the geometry to be used by this renderer
 *
 * @method setGeometry
 * @for Renderer
 * @param {Object} geometry The geometry to be used by this renderer
 */
void RendererApi::SetGeometry( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Renderer renderer = GetRenderer( isolate, args );

  bool found( false );
  Geometry geometry = GeometryApi::GetGeometryFromParams( 0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing geometry from param 0" );
  }
  else
  {
    renderer.SetGeometry(geometry);
  }
}

/**
 * Gets the geometry used by this renderer
 *
 * @method getGeometry
 * @for Renderer
 * @return {Object} The geometry used by this renderer
 */
void RendererApi::GetGeometry( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Renderer renderer = GetRenderer( isolate, args );
  Geometry geometry = renderer.GetGeometry();

  // Wrap the geometry
  v8::Local<v8::Object> localObject = GeometryWrapper::WrapGeometry( isolate, geometry );
  args.GetReturnValue().Set( localObject );
}

/**
 * Sets the texture set to be used by this renderer
 *
 * @method setTextures
 * @for Renderer
 * @param {Object} textureSet The TextureSet to be used by this renderer
 */
void RendererApi::SetTextures( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Renderer renderer = GetRenderer( isolate, args );

  bool found( false );
  TextureSet textureSet = TextureSetApi::GetTextureSetFromParams( 0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing texture set from param 0" );
  }
  else
  {
    renderer.SetTextures(textureSet);
  }
}

/**
 * Gets the texture set used by this renderer
 *
 * @method getTextures
 * @for Renderer
 * @return {Object} The texture set used by this renderer
 */
void RendererApi::GetTextures( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Renderer renderer = GetRenderer( isolate, args );
  TextureSet textureSet = renderer.GetTextures();

  // Wrap the textureset
  v8::Local<v8::Object> localObject = TextureSetWrapper::WrapTextureSet( isolate, textureSet );
  args.GetReturnValue().Set( localObject );
}

} // namespace V8Plugin

} // namespace Dali
