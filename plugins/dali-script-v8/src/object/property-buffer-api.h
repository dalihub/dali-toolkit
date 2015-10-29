#ifndef __DALI_V8PLUGIN_PROPERTY_BUFFER_API_H__
#define __DALI_V8PLUGIN_PROPERTY_BUFFER_API_H__

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

// EXTERNAL INCLUDES
#include <v8.h>
#include <dali/devel-api/object/property-buffer.h>


namespace Dali
{

namespace V8Plugin
{

namespace PropertyBufferApi
{

  /**
   * @brief Helper to get property buffer from args.This()
   * @param[in] isolate v8 isolated instance
   * @param[in] args v8 function call arguments interpreted
   */
  PropertyBuffer GetPropertyBuffer( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * @brief Helper to get property buffer from the JavaScript object held in the given function argument
   * @param[in] paramIndex Argument index the object is held in
   * @param[in] found Whether property buffer is found in the given function parameter
   * @param[in] isolate v8 isolated instance
   * @param[in] args v8 function call arguments interpreted
   */
  PropertyBuffer GetPropertyBufferFromParams( int paramIndex,
                                                  bool& found,
                                                  v8::Isolate* isolate,
                                                  const v8::FunctionCallbackInfo< v8::Value >& args );

 /**
  * Constructor
  */
  PropertyBuffer New( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * PropertyBuffer API see property-buffer.h for a description
   */
  void SetData( const v8::FunctionCallbackInfo< v8::Value >& args );

}; // namespace PropertyBufferApi

} // namespace V8Plugin

} // namespace Dali

#endif // header __DALI_V8PLUGIN_PROPERTY_BUFFER_API_H__
