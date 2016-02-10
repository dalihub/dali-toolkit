#ifndef __DALI_V8PLUGIN_PROPERTYVALUE_WRAPPER_H__
#define __DALI_V8PLUGIN_PROPERTYVALUE_WRAPPER_H__

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
#include <dali/public-api/object/property-value.h>

// INTERNAL INCLUDES
#include <shared/base-wrapped-object.h>


namespace Dali
{

namespace V8Plugin
{


/**
 * An PropertyValue wrapper.
 * Provides access to Property Value specific functionality and V8 memory handling.
 *
 * Only wraps complex types.
 * Native JavaScript Property Value types such as float, bool string etc are handled by the actor wrapper.
 * This supports sub property access for maps and vector.x etc and math functionality not easily mapped
 * to JavaScript objects.
 *
 */
class PropertyValueWrapper : public BaseWrappedObject
{
public:

  /**
   * @brief Constructor
   * @param[in] value property value to wrap
   * @param[in] gc Dali garbage collector interface
   */
  explicit PropertyValueWrapper( const Dali::Property::Value& value, GarbageCollectorInterface& gc );

  /**
   * @brief Destructor
   */
  virtual ~PropertyValueWrapper();

  /**
   * @brief Creates a new Rotation wrapped inside a Javascript Object.
   * @param[in] args v8 function call arguments interpreted as (x=0,y=0)
   */
  static void NewRotation( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * @brief Creates a new Matrix wrapped inside a Javascript Object.
   * @param[in] args v8 function call arguments can be a 4 x 4 matrix or a 3 x 3 matrix
   */
  static void NewMatrix( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * @brief Wraps a dali property value inside a JavaScript object
   * @return JavaScript object
   */
  static v8::Handle<v8::Object> WrapDaliProperty(v8::Isolate* isolate, const Dali::Property::Value &value);

  /**
   * @brief Extract a property value wrapper from a javascript object
   * @return property value wrapper
   */
  static PropertyValueWrapper*  Unwrap( v8::Isolate* isolate, v8::Handle< v8::Object> obj);

  /**
   * Extract a property value from a javascript object, of a given type
   * @return property value
   */
  static Dali::Property::Value ExtractPropertyValue( v8::Isolate* isolate, v8::Local< v8::Value> v8Value, Dali::Property::Type type);

  /**
   * Extract a property value from a javascript object
   * @return property value
   */
  static Dali::Property::Value ExtractPropertyValue( v8::Isolate* isolate, v8::Local< v8::Value> v8Value);

  /**
   * @brief Extract a vector or a matrix from a JavaScript array
   * @return property value ( vector or matrix)
   */
  static Dali::Property::Value VectorOrMatrixFromV8Array( v8::Isolate* isolate, const v8::Local<v8::Value>& v8Value);

  /**
   * @brief Extract a Dali Property::Array from a JavaScript array
   * @return array property value ( vector or matrix)
   */
  static Dali::Property::Value ArrayFromV8Array( v8::Isolate* isolate, const v8::Local<v8::Value>& v8Value);

  /**
   * @return property value
   */
   Dali::Property::Value GetValue()
   {
     return mValue;
   }

private:


   /**
    * @brief get the value for a property for JavaScript object than contains a Dali object.
    * E.g. Get( "x", JavaScript object that wraps a Dali Vector2 )
    * @param[in] propertyName property name
    * @param[in] info reference to PropertyCallbackInfo structure (contains the Javascript
    * object and the return value).
    */
   static void PropertyGet( v8::Local<v8::String> propertyName,
                             const v8::PropertyCallbackInfo<v8::Value>& info);


   /**
    * @brief Set the value for a property for JavaScript object than contains a Dali object.
    * E.g. Set( "x", 103, JavaScript object that wraps a Dali Vector2 )
    * @param[in] propertyName property name
    * @param[in] javaScriptValue javascript value to set, this is typically a number
    * @param[in] info reference to PropertyCallbackInfo structure (contains the Javascript
    * object).
    */
   static void PropertySet( v8::Local<v8::String> propertyName,
                     v8::Local<v8::Value> javaScriptValue,
                     const v8::PropertyCallbackInfo<v8::Value>& info);




  // The Property Value ObjectTemplates
  static v8::Persistent<v8::ObjectTemplate> mTemplatePrimitive;

  /**
   * @brief used to make Vectors, rotation and rect template
   * Just produces an object template with 2 internal fields
   * A type and a pointer to the dali object
   */
  static v8::Handle<v8::ObjectTemplate> MakeDaliPropertyTemplate( v8::Isolate* isolate );

private:

  Dali::Property::Value mValue;     ///< property value
};

} // V8Plugin

} // Dali

#endif // header
