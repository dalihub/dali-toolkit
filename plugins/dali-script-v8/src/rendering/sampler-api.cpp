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
#include "sampler-api.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <rendering/sampler-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

/**
 * ## Sampler API
 *
 * Sampler is a handle to an object that can be used to provide the sampling parameters to sample textures
 *
 * @class Sampler
 */

Sampler SamplerApi::GetSampler( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
  void* ptr = field->Value();

  SamplerWrapper* wrapper = static_cast< SamplerWrapper *>(ptr);
  return wrapper->GetSampler();
}

Sampler SamplerApi::GetSamplerFromParams( int paramIndex,
                                             bool& found,
                                             v8::Isolate* isolate,
                                             const v8::FunctionCallbackInfo< v8::Value >& args )
{
  found = false;

  v8::HandleScope handleScope( isolate );
  BaseWrappedObject* wrappedObject = V8Utils::GetWrappedDaliObjectParameter( paramIndex, BaseWrappedObject::SAMPLER, isolate, args );
  if( wrappedObject )
  {
    found = true;
    SamplerWrapper* wrapper = static_cast< SamplerWrapper *>(wrappedObject);
    return wrapper->GetSampler();
  }
  else
  {
    return Sampler();
  }
}

/**
 * Create a new sampler object.
 *
 * @constructor
 * @method Sampler
 * @for Sampler
 * @return {Object} Sampler
 */
Sampler SamplerApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  return Sampler::New();
}

/**
 * Set the filter modes for this sampler
 * @method setFilterMode
 * @for Sampler
 * @param {integer} minFilter The minification filter that will be used
 * @param {integer} magFilter The magnification filter that will be used
 * @example
 *      // filter mode is one of the following
 *      dali.FILTER_MODE_NONE             // Use GL system defaults (minification NEAREST_MIPMAP_LINEAR, magnification LINEAR)
 *      dali.FILTER_MODE_DEFAULT          // Use dali defaults (minification LINEAR, magnification LINEAR)
 *      dali.FILTER_MODE_NEAREST          // Filter nearest
 *      dali.FILTER_MODE_LINEAR           // Filter linear
 *
 *      sampler.setFilterMode( dali.FILTER_MODE_DEFAULT, dali.FILTER_MODE_LINEAR );
 */
void SamplerApi::SetFilterMode( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Sampler sampler = GetSampler( isolate, args );

  bool found( false );
  int minFilter = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing minification filter from param 0" );
    return;
  }

  found = false;
  int magFilter = V8Utils::GetIntegerParameter( PARAMETER_1, found, isolate, args, 0 /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing magnification filter from param 1" );
  }
  else
  {
    sampler.SetFilterMode(static_cast<FilterMode::Type>(minFilter), static_cast<FilterMode::Type>(magFilter));
  }
}

/**
 * Set the wrap modes for this sampler
 * @method setWrapMode
 * @for Sampler
 * @param {integer} uWrap Wrap mode for u coordinates
 * @param {integer} vWrap Wrap mode for v coordinates
 * @example
 *      // wrap mode is one of the following
 *      dali.WRAP_MODE_DEFAULT
 *      dali.WRAP_MODE_CLAMP_TO_EDGE
 *      dali.WRAP_MODE_REPEAT
 *      dali.WRAP_MODE_MIRRORED_REPEAT
 *
 *      sampler.setWrapMode( dali.WRAP_MODE_CLAMP_TO_EDGE, dali.WRAP_MODE_REPEAT );
 */
void SamplerApi::SetWrapMode( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Sampler sampler = GetSampler( isolate, args );

  bool found( false );
  int uWrap = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing wrap mode for u coordinates from param 0" );
    return;
  }

  found = false;
  int vWrap = V8Utils::GetIntegerParameter( PARAMETER_1, found, isolate, args, 0 /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing wrap mode for v coordinates from param 1" );
  }
  else
  {
    sampler.SetWrapMode(static_cast<WrapMode::Type>(uWrap), static_cast<WrapMode::Type>(vWrap));
  }
}

} // namespace V8Plugin

} // namespace Dali
