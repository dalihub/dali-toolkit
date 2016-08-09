#ifndef DALI_V8PLUGIN_GEOMETRY_API_H
#define DALI_V8PLUGIN_GEOMETRY_API_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <v8.h>
#include <dali/public-api/rendering/geometry.h>


namespace Dali
{

namespace V8Plugin
{

namespace GeometryApi
{

  /**
   * Helper to get geometry from args.This()
   */
  Geometry GetGeometry( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * @brief Helper to get geometry from the JavaScript object held in the given function argument
   * @param[in] paramIndex Argument index the object is held in
   * @param[in] found Whether geometry is found in the given function parameter
   * @param[in] isolate v8 isolated instance
   * @param[in] args v8 function call arguments interpreted
   */
  Geometry GetGeometryFromParams( int paramIndex,
                                     bool& found,
                                     v8::Isolate* isolate,
                                     const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * Constructor
   */
   Geometry New( const v8::FunctionCallbackInfo< v8::Value >& args );

   /**
    * Geometry API see geometry.h for a description
    */
   void AddVertexBuffer( const v8::FunctionCallbackInfo< v8::Value >& args );
   void GetNumberOfVertexBuffers( const v8::FunctionCallbackInfo< v8::Value >& args );
   void RemoveVertexBuffer( const v8::FunctionCallbackInfo< v8::Value >& args );
   void SetIndexBuffer( const v8::FunctionCallbackInfo< v8::Value >& args );
   void SetType( const v8::FunctionCallbackInfo< v8::Value >& args );
   void GetType( const v8::FunctionCallbackInfo< v8::Value >& args );

}; // namespace GeometryApi

} // namespace V8Plugin

} // namespace Dali

#endif // DALI_V8PLUGIN_GEOMETRY_API_H
