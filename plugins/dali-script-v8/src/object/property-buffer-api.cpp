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
#include "property-buffer-api.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <object/property-buffer-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

namespace // unnamed namespace
{

struct PropertyBufferParameters
{
  PropertyBufferParameters()
      : mSize( 0 )
  {
  }

  PropertyBuffer NewPropertyBuffer()
  {
    return PropertyBuffer::New( mBufferFormat,
                                mSize);
  }

  Property::Map mBufferFormat;
  std::size_t mSize;
};

} // unnamed space

/**
 * ## PropertyBuffer API
 *
 * PropertyBuffer is a handle to an object that contains a buffer of structured properties.
 * It can be used to provide data to Geometry objects.
 *
 * ### Simple example
 *
 *```
 *    var vertexFormat ={ "aPosition" : dali.PROPERTY_VECTOR2 };
 *    var vertexData = [    0,     1,
 *                      -0.95,  0.31,
 *                      -0.59, -0.81,
 *                       0.59, -0.81,
 *                       0.95,  0.31];
 *
 *    var vertexDataArray = new Float32Array(vertexData.length);
 *    vertexDataArray.set(vertexData, 0);
 *    var vertices = new dali.PropertyBuffer(vertexFormat, 5);
 *    vertices.setData(vertexDataArray);
 *
 *    var geometry = new dali.Geometry();
 *    geometry.addVertexBuffer( vertices );
 *```
 * @class PropertyBuffer
 */

PropertyBuffer PropertyBufferApi::GetPropertyBuffer( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
  void* ptr = field->Value();

  PropertyBufferWrapper* wrapper = static_cast< PropertyBufferWrapper *>(ptr);
  return wrapper->GetPropertyBuffer();
}

PropertyBuffer PropertyBufferApi::GetPropertyBufferFromParams( int paramIndex,
                          bool& found,
                          v8::Isolate* isolate,
                          const v8::FunctionCallbackInfo< v8::Value >& args )
{
  found = false;

  v8::HandleScope handleScope( isolate );
  BaseWrappedObject* wrappedObject = V8Utils::GetWrappedDaliObjectParameter( paramIndex, BaseWrappedObject::PROPERTY_BUFFER, isolate, args );
  if( wrappedObject )
  {
    found = true;
    PropertyBufferWrapper* wrapper = static_cast< PropertyBufferWrapper *>(wrappedObject);
    return wrapper->GetPropertyBuffer();
  }
  else
  {
    return PropertyBuffer();
  }
}

/**
 * Create a new PropertyBuffer
 *
 * PropertyBuffers contains a buffer of structured properties and can be
 * used to provide data to Geometry objects.
 *
 * @constructor
 * @for PropertyBuffer
 * @method PropertyBuffer
 * @param {Object} bufferFormat
 * @param {integer} size
 * @return {Object} PropertyBuffer
 * @example
 *```
 *    var bufferFormat = {
 *                         "aPosition" : dali.PROPERTY_VECTOR2,
 *                         "aTexCoord" : dali.PROPERTY_VECTOR2,
 *                         "aHue"      : dali.PROPERTY_FLOAT,
 *                       };
 *
 *    var buffer = new dali.PropertyBuffer(bufferFormat, 5);
 *```
 */
PropertyBuffer PropertyBufferApi::New( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  bool found( false );
  Dali::Property::Map bufferFormat = V8Utils::GetPropertyMapParameter( PARAMETER_0, found, isolate, args);
  if( !found || bufferFormat.Empty() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid property map parameter" );
    return PropertyBuffer();
  }

  found = false;
  int size = V8Utils::GetIntegerParameter( PARAMETER_1, found, isolate, args, 0);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing buffer size from param 1" );
    return PropertyBuffer();
  }

  return PropertyBuffer::New(bufferFormat, static_cast<std::size_t>(size));
}

/**
 * Update the whole buffer information
 *
 * This function expects an array of data with the same format that was given
 * in the construction, and the number of elements to be the same as the size
 * of the buffer.
 *
 * @method setData
 * @for PropertyBuffer
 * @param {Object} data The data that will be copied to the buffer.
 * @example
 *```
 *   var vertexData = [    0,     1,
 *                     -0.95,  0.31,
 *                     -0.59, -0.81,
 *                      0.59, -0.81,
 *                      0.95,  0.31];
 *
 *   var vertexDataArray = new Float32Array(vertexData.length);
 *   vertexDataArray.set(vertexData, 0);
 *
 *   propertyBuffer.setData( vertexDataArray );
 *```
 */
void PropertyBufferApi::SetData( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  PropertyBuffer buffer = GetPropertyBuffer( isolate, args );

  bool found( false );
  void* data = V8Utils::GetArrayBufferViewParameter( PARAMETER_0, found, isolate, args);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid data parameter" );
  }
  else
  {
    buffer.SetData( data );
  }
}

} // namespace V8Plugin

} // namespace Dali
