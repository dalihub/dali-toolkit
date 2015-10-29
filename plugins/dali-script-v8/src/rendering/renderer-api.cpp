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
#include <rendering/material-api.h>
#include <rendering/material-wrapper.h>

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
 * @param {Object} material The material to be used by this renderer
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
  Material material = MaterialApi::GetMaterialFromParams( 1, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing material from param 0" );
    return Renderer();
  }

  return Renderer::New(geometry, material);
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
 * Sets the material to be used by this renderer
 *
 * @method setMaterial
 * @for Renderer
 * @param {Object} material The material to be used by this renderer
 */
void RendererApi::SetMaterial( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Renderer renderer = GetRenderer( isolate, args );

  bool found( false );
  Material material = MaterialApi::GetMaterialFromParams( 0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing material from param 0" );
  }
  else
  {
    renderer.SetMaterial(material);
  }
}

/**
 * Gets the material used by this renderer
 *
 * @method getMaterial
 * @for Renderer
 * @return {Object} The material used by this renderer
 */
void RendererApi::GetMaterial( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Renderer renderer = GetRenderer( isolate, args );
  Material material = renderer.GetMaterial();

  // Wrap the material
  v8::Local<v8::Object> localObject = MaterialWrapper::WrapMaterial( isolate, material );
  args.GetReturnValue().Set( localObject );
}

} // namespace V8Plugin

} // namespace Dali
