#ifndef DALI_V8PLUGIN_PROPERTY_BUFFER_WRAPPER_H
#define DALI_V8PLUGIN_PROPERTY_BUFFER_WRAPPER_H

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
#include <dali/public-api/rendering/property-buffer.h>

// INTERNAL INCLUDES
#include <shared/base-wrapped-object.h>

namespace Dali
{

namespace V8Plugin
{


/**
 * A PropertyBuffer wrapper.
 * Provides access to PropertyBuffer specific functionality and V8 memory handling.
 */
class PropertyBufferWrapper : public BaseWrappedObject
{

public:

  /**
   * @brief Constructor
   * @param[in] propertyBuffer DALi property buffer
   * @param[in] gc garbage collection interface
   */
  PropertyBufferWrapper( const PropertyBuffer& propertyBuffer,
                GarbageCollectorInterface& gc );

  /**
   * @brief Destructor
   */
  virtual ~PropertyBufferWrapper()
  {
  }

  /**
   * @brief Creates a new PropertyBuffer wrapped inside a Javascript Object.
   * @param[in] args v8 function call arguments interpreted
   */
  static void NewPropertyBuffer( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * Wraps a property buffer
   */
  static v8::Handle<v8::Object> WrapPropertyBuffer(v8::Isolate* isolate, const Dali::PropertyBuffer& );


  // The PropertyBufferAttribute ObjectTemplate, we cache templates so we don't have
  // keep generating them everytime we want to create a PropertyBuffer
  static v8::Persistent<v8::ObjectTemplate> mPropertyBufferTemplate;

  /**
   * @brief Gets the handle of property buffer
   */
  PropertyBuffer GetPropertyBuffer();


private:

  // PropertyBuffer
  PropertyBuffer mPropertyBuffer;

  static v8::Handle<v8::ObjectTemplate> MakePropertyBufferTemplate( v8::Isolate* isolate );
  static v8::Local<v8::ObjectTemplate> GetPropertyBufferTemplate( v8::Isolate* isolate );

};

} // namespace V8Plugin

} // namespace Dali

#endif // DALI_V8PLUGIN_PROPERTY_BUFFER_WRAPPER_H
