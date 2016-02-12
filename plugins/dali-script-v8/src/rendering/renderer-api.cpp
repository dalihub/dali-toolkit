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

/**
 * Specify the pixel arithmetic used when the actor is blended.
 *
 * @for Renderer
 * @method setBlendFunc
 * @param {integer} srcFactorRgb Source Blending RGB
 * @param {integer} destFactorRgb Destination Blending RGB
 * @param {integer} srcFactorAlpha Source Blending Alpha
 * @param {integer} destFactorAlpha Destination Blending Alpha
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
 *    renderer.setBlendFunc( dali.BLEND_FACTOR_CONSTANT_COLOR, dali.BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
 *                           dali.BLEND_FACTOR_CONSTANT_ALPHA, dali.BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA );
 */
void RendererApi::SetBlendFunc( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Renderer renderer = GetRenderer( isolate, args );

  int params[4];
  bool foundAllParams(false);
  V8Utils::ReadIntegerArguments( foundAllParams, &params[0], 4, args, 0 );
  if( foundAllParams )
  {
    renderer.SetBlendFunc( static_cast< Dali::BlendingFactor::Type>(params[0]),
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
 * @for Renderer
 * @method getBlendFunc
 * @return {Object} Blend properties
 * @example Blend properties object has 4 fields
 *
 *      blendProperties.sourceRgb // source rgb enum
 *      blendProperties.destinationRgb // destination rgb enum
 *      blendProperties.sourceAlpha source // alpha enum
 *      blendProperties.destinationAlpha // destination alpha enum
 */
void RendererApi::GetBlendFunc( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  // Pass by reference doesn't work in Javascript
  // For now just return a vector 4...

  BlendingFactor::Type srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha;
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Renderer renderer = GetRenderer( isolate, args );

  renderer.GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );

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
 * @for Renderer
 * @method setBlendEquation
 * @param { integer } equationRgb The equation used for combining red, green, and blue components.
 * @param { integer } equationAlpha The equation used for combining the alpha component.
 * @example
 *      // blend equation is one of the following
 *      dali.BLEND_EQUATION_ADD
 *      dali.BLEND_EQUATION_SUBTRACT
 *      dali.BLEND_EQUATION_REVERSE_SUBTRACT
 *
 *      renderer.setBlendEquation( dali.BLEND_EQUATION_ADD, dali.BLEND_EQUATION_REVERSE_SUBTRACT );
 */
void RendererApi::SetBlendEquation( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Renderer renderer = GetRenderer( isolate, args );

  int params[2];
  bool foundAllParams(false);
  V8Utils::ReadIntegerArguments( foundAllParams, &params[0], 2, args, 0 );
  if( foundAllParams )
  {
    renderer.SetBlendEquation( static_cast< BlendingEquation::Type>(params[0]), static_cast< BlendingEquation::Type>(params[1]) );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid BlendEquation parameter");
  }
}

/**
 * Query the equation used when the actor is blended.
 *
 * @for Renderer
 * @method getBlendEquation
 * @return {Object} Blend equations
 * @example Blend equations object has 2 fields
 *
 *      blendEquations.equationRgb // equation used for combining rgb components
 *      blendEquations.equationAlpha // equation used for combining alpha components
 */
void RendererApi::GetBlendEquation( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  // Pass by reference doesn't work in Javascript
  // For now just return a vector 2...

  BlendingEquation::Type equationRgb, equationAlpha;
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Renderer renderer = GetRenderer( isolate, args );

  renderer.GetBlendEquation( equationRgb, equationAlpha );

  v8::Local<v8::Object> blendEquations = v8::Object::New( isolate );

  blendEquations->Set( v8::String::NewFromUtf8( isolate, "equationRgb" ),   v8::Integer::New( isolate, equationRgb) );
  blendEquations->Set( v8::String::NewFromUtf8( isolate, "equationAlpha" ), v8::Integer::New( isolate, equationAlpha ) );

  args.GetReturnValue().Set( blendEquations );
}

} // namespace V8Plugin

} // namespace Dali
