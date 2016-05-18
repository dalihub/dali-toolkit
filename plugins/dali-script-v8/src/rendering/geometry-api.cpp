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
#include "geometry-api.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <rendering/geometry-wrapper.h>
#include <object/property-buffer-api.h>

namespace Dali
{

namespace V8Plugin
{

/**
 * ## Geometry API
 *
 * Geometry is handle to an object that can be used to define a geometric elements.
 *
 * @class Geometry
 * @extends Handle
 */

Geometry GeometryApi::GetGeometry( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
  void* ptr = field->Value();

  GeometryWrapper* wrapper = static_cast< GeometryWrapper *>(ptr);
  return wrapper->GetGeometry();
}

Geometry GeometryApi::GetGeometryFromParams( int paramIndex,
                                             bool& found,
                                             v8::Isolate* isolate,
                                             const v8::FunctionCallbackInfo< v8::Value >& args )
{
  found = false;

  v8::HandleScope handleScope( isolate );
  BaseWrappedObject* wrappedObject = V8Utils::GetWrappedDaliObjectParameter( paramIndex, BaseWrappedObject::GEOMETRY, isolate, args );
  if( wrappedObject )
  {
    found = true;
    GeometryWrapper* wrapper = static_cast< GeometryWrapper *>(wrappedObject);
    return wrapper->GetGeometry();
  }
  else
  {
    return Geometry();
  }
}

/**
 * Create a new geometry object.
 *
 * @constructor
 * @method Geometry
 * @for Geometry
 * @return {Object} Geometry
 */
Geometry GeometryApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  return Geometry::New();
}

/**
 * Add a PropertyBuffer to be used as source of geometry vertices
 *
 * @method addVertexBuffer
 * @for Geometry
 * @param {Object} vertexBuffer PropertyBuffer to be used as source of geometry vertices
 * @return {interger} Index of the newly added buffer, can be used with RemoveVertexBuffer
 *                    to remove this buffer if no longer required
 * @example
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
 */
void GeometryApi::AddVertexBuffer( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Geometry geometry = GetGeometry( isolate, args );

  bool found( false );
  PropertyBuffer vertexBuffer = PropertyBufferApi::GetPropertyBufferFromParams( 0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid property buffer parameter" );
  }
  else
  {
    args.GetReturnValue().Set( v8::Integer::New( isolate, geometry.AddVertexBuffer(vertexBuffer) ) );
  }
}

/**
 * Retrieve the number of vertex buffers that have been added to this geometry
 *
 * @method getNumberOfVertexBuffers
 * @for Geometry
 * @return {integer} Number of vertex buffers that have been added to this geometry
 */
void GeometryApi::GetNumberOfVertexBuffers( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Geometry geometry = GetGeometry( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, geometry.GetNumberOfVertexBuffers() ) );
}

/**
 * Remove a vertex buffer
 *
 * @method removeVertexBuffer
 * @for Geometry
 * @param {integer} index Index to the vertex buffer to remove,
 *                  which must be between 0 and getNumberOfVertexBuffers()
 */
void GeometryApi::RemoveVertexBuffer( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Geometry geometry = GetGeometry( isolate, args );

  bool found( false );
  int index = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing index from param 0" );
  }
  else
  {
    geometry.RemoveVertexBuffer(static_cast<std::size_t>(index));
  }
}

/**
 * Set a PropertyBuffer to be used as a source of indices for the geometry
 *
 * This buffer is required to have exactly one component and it must be of the
 * type dali.PROPERTY_INTEGER
 *
 * By setting this buffer it will cause the geometry to be rendered using indices.
 * To unset call SetIndexBuffer with an empty handle.
 *
 * @method setIndexBuffer
 * @for Geometry
 * @param {Uint32Array} data The data that will be copied to the buffer
 *
 * @example
 * var indexData = [0, 1, 1, 2, 2, 3, 3, 4, 4, 0];
 * var indexDataArray = new Uint32Array(indexData.length);
 * indexDataArray.set(indexData, 0);
 *
 * geometry.SetIndexBuffer( indexDataArray, indexDataArray.length );
 *```
 */
void GeometryApi::SetIndexBuffer( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Geometry geometry = GetGeometry( isolate, args );

  bool found( false );
  void* data = V8Utils::GetArrayBufferViewParameter( PARAMETER_0, found, isolate, args);

  if( ! found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid data parameter" );
  }
  else
  {
    unsigned int size = V8Utils::GetIntegerParameter( PARAMETER_1, found, isolate, args, 0);
    if( !found )
    {
      DALI_SCRIPT_EXCEPTION( isolate, "missing buffer size from param 1" );
    }
    else
    {
      Dali::Vector<unsigned short> indices;
      indices.Resize( size );
      unsigned int* indexData = static_cast<unsigned int*>(data);
      for( size_t i(0); i<size; ++i )
      {
        indices[i] = indexData[i];
      }
      geometry.SetIndexBuffer( &indices[0], size );
    }
  }
}

/**
 * Set the type of primitives this geometry contains
 *
 * @method setGeometryType
 * @for Geometry
 * @param {integer} geometryType Type of primitives this geometry contains
 * @example
 *      // geometry type is one of the following
 *      dali.GEOMETRY_POINTS
 *      dali.GEOMETRY_LINES
 *      dali.GEOMETRY_LINE_LOOP
 *      dali.GEOMETRY_LINE_STRIP
 *      dali.GEOMETRY_TRIANGLES
 *      dali.GEOMETRY_TRIANGLE_FAN
 *      dali.GEOMETRY_TRIANGLE_STRIP
 *
 *      geometry.SetGeometryType( dali.GEOMETRY_LINES );
 */
void GeometryApi::SetGeometryType( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Geometry geometry = GetGeometry( isolate, args );

  bool found( false );
  int geometryType = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing geometryType from param 0" );
  }
  else
  {
    geometry.SetGeometryType(static_cast<Geometry::GeometryType>(geometryType));
  }
}

/**
 * Get the type of primitives this geometry contains
 *
 * @method getGeometryType
 * @for Geometry
 * @return {integer} Type of primitives this geometry contains
 * @example
 *      // returns one of the following
 *      dali.GEOMETRY_POINTS
 *      dali.GEOMETRY_LINES
 *      dali.GEOMETRY_LINE_LOOP
 *      dali.GEOMETRY_LINE_STRIP
 *      dali.GEOMETRY_TRIANGLES
 *      dali.GEOMETRY_TRIANGLE_FAN
 *      dali.GEOMETRY_TRIANGLE_STRIP
 */
void GeometryApi::GetGeometryType( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Geometry geometry = GetGeometry( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, geometry.GetGeometryType() ) );
}

} // namespace V8Plugin

} // namespace Dali
