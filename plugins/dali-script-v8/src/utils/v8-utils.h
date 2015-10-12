#ifndef __DALI_V8PLUGIN_V8_UTILS_H__
#define __DALI_V8PLUGIN_V8_UTILS_H__

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
#include <string>
#include <v8.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <shared/base-wrapped-object.h>

namespace Dali
{

namespace V8Plugin
{

enum
{
  PARAMETER_0 = 0,   ///< first parameter of a function call
  PARAMETER_1 = 1,   ///< second parameter of a function call
  PARAMETER_2 = 2,   ///< third parameter of a function call
  PARAMETER_3 = 3,   ///< forth parameter of a function call
  PARAMETER_4 = 4,   ///< fifth parameter of a function call
};

#define DALI_SCRIPT_EXCEPTION( isolate, message ) V8Utils::ScriptError( __FUNCTION__ , isolate, message );

namespace V8Utils
{
/**
 * Print the list of arguments to std out
 * @param[in] args v8 function args interpreted as (string, string,...)
 */
void Log(const v8::FunctionCallbackInfo< v8::Value >& args) ;

/**
 * Print out using DALI_LOG_ERROR
 * @param[in] args v8 function args interpreted as (string, string,...)
 */
void LogError(const v8::FunctionCallbackInfo< v8::Value >& args) ;

/**
 * Read a files contents
 *
 * @param[in] filename The filename
 * @param[out] contents string contents of the file
 */
void GetFileContents( const std::string &filename, std::string& contents );

/**
 * Extract the directory path from a full path filename
 * @param fileName file name
 * @param directory directory name
 */
void GetFileDirectory( const std::string& fileName, std::string& directory );

/**
 * Extract file name from a full path + file name
 * @param[in] fullPathName full path + file name
 * @param[out] fileName file name
 */
void GetFileName( const std::string& fullPathName, std::string& fileName);

/**
 * Return the module name, e.g. if the module is
 * my_module.js it will get 'my_module'
 * @param[in] fileName full path module name
 * @param[out] moduleName module name
 */
void GetModuleName( const std::string& fileName, std::string& moduleName );

/**
 * Report an exception by writing as a warning to the Dali Log
 *
 * @param[in] try_catch The v8 TryCatch exception object
 */
void ReportException(  v8::Isolate* isolate, v8::TryCatch* try_catch) ;

/**
 * Depending on what coding standard used this can
 * convert the first character to lower case,
 * E.g.  GetId becomes getId
 */
std::string GetJavaScriptFunctionName(  const char* functionName );

 /**
  * Get V8 Version
  *
  * @param[in] args v8 function args ignored
  * @return a version string
  */
 void Version(const v8::FunctionCallbackInfo< v8::Value >& args) ;

/**
 * Convert v8 string as a std::string
 * @param[in] value v8 function args interpreted as (v8_string)
 * @return std string
 */
 std::string v8StringToStdString( const v8::Handle<v8::Value>& value) ;

/**
 * Convert hyphenated to camelCase (Dali property to wrapper property name)
 *
 * @param[in] hyphenatedName a hyphenated std::string to convert
 * @return a camelCase'd std::string
 */
 std::string PropertyNameToJavaScriptName(const std::string& hyphenatedName);

/**
 * Convert camelCase to hyphenated to (Wrapper property name to Dali property)
 * E.g. anchorPoint to "anchor-point"
 * @param[in] camelCase a std::string camelCase
 * @return a hyphenated std::string conversion
 */
 std::string JavaScriptNameToPropertyName(const std::string& camelCase);

/**
 * Script error, throws an exception
 */
void ScriptError( const char* function, v8::Isolate* isolate, std::string errorString );

/**
 * @return in the value is a boolean primitive or a boolean object
 */
bool IsBooleanPrimitiveOrObject( const v8::Local<v8::Value>& value );

/**
 * @return the value of boolean primitive or boolean object value
 */
bool GetBooleanValue( v8::Isolate* isolate, const v8::Local<v8::Value>& value );

/**
 * @return true if the value is a number or a number object
 */
bool IsNumberPrimitiveOrObject( const v8::Local<v8::Value>& value );

/**
 * @return the number of a number primitive or number object value
 */
float GetNumberValue( v8::Isolate* isolate, const v8::Local<v8::Value>& value );

/**
 * @return if the value is a string primitve or a string object
 */
bool IsStringPrimitiveOrObject( const v8::Local<v8::Value>& value );

/**
 * @return true if the value is a string or a string object
 */
bool IsStringPrimitiveOrObject( const v8::Local<v8::Value>& value );

/**
 * @return the string from a string value or a string object
 */
std::string GetStringValue( v8::Isolate* isolate, const v8::Local<v8::Value>& value );

/******************************************
 *  Helper functions for extracting a DALi object from a JavaScript object
 *******************************************/

/**
 * Given a JavaScript object, either extract the embedded DALi property value or
 * convert it to a property value
 * @param[out] found whether the property was found
 * @return property value
 */
Property::Value GetPropertyValueFromObject( bool& found, v8::Isolate* isolate, const v8::Local<v8::Value >& value );

/**
 * Given a JavaScript object with
 * @param [in] object JavaScrript object
 * @return DALi ProperyMap from the JavaScript object
 */
Property::Map GetPropertyMapFromObject( v8::Isolate* isolate, const v8::Local<v8::Object>& object);

/**
 * Extract a DALi wrapped object, from a JavaScript object held in a function argument
 * @param[in] index argument index the object is held in
 * @param[in] type type of wrapped object to extract
 */

Actor GetActorFromObject( v8::Isolate* isolate, bool& found, v8::Local<v8::Object>& object);


/******************************************
 *  Helper functions to extract a C++ native type (int,float,string) or a DALi object
 *  ( handle, vector, actor, layer, property value, property map ) from a
 *  JavaScript function argument.
 *  E.g.
 *  // JavaScript code
 *     myJavaScriptFunction( 23, 345, myActor );
 *
 *  // C++ code
 * bool found;
 *
 * Actor myActor = v8Utils::GetActorParameter( PARAMETER_2, found, JavaScript Args );
 ******************************************/
/**
 * @param [in] index parameter index, e.g. callMyFunc( index0, index1, index2).
 * @param[out] found whether the parameter was found
 * @return integer value from the JavaScript function arguments
 */
int GetIntegerParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args, int defaultValue  );

/**
 * @param [in] index parameter index, e.g. callMyFunc( index0, index1, index2).
 * @param[out] found whether the parameter was found
 * @return float value from the JavaScript function arguments
 */
float GetFloatParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args, float defaultValue  );

/**
 * @param [in] index parameter index, e.g. callMyFunc( index0, index1, index2).
 * @param[out] found whether the parameter was found
 * @return string value from the JavaScript function arguments
 */
std::string GetStringParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

/**
 * @param [in] index parameter index, e.g. callMyFunc( index0, index1, index2).
 * @param[out] found whether the parameter was found
 * @return boolean value from the JavaScript function arguments
 */
bool GetBooleanParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

/**
 * @param [in] index parameter index, e.g. callMyFunc( index0, index1, index2).
 * @param[out] found whether the parameter was found
 * @return ArrayBufferView from the JavaScript function arguments
 */
void* GetArrayBufferViewParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args  );

/**
 * @param [in] index parameter index, e.g. callMyFunc( index0, index1, index2).
 * @param[out] found whether the parameter was found
 * @return DALi Handle value from the JavaScript function arguments
 */
Handle GetHandleParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args  );

/**
 * @param [in] index parameter index, e.g. callMyFunc( index0, index1, index2).
 * @param[out] found whether the parameter was found
 * @return Vector2 from the JavaScript function arguments
 */
Vector2 GetVector2Parameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

/**
 * @param [in] index parameter index, e.g. callMyFunc( index0, index1, index2).
 * @param[out] found whether the parameter was found
 * @return Vector3 from the JavaScript function arguments
 */
Vector3 GetVector3Parameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args);

/**
 * @param [in] index parameter index, e.g. callMyFunc( index0, index1, index2).
 * @param[out] found whether the parameter was found
 * @return Vector4 from the JavaScript function arguments
 */
Vector4 GetVector4Parameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args);

/**
 * @param [in] index parameter index, e.g. callMyFunc( index0, index1, index2).
 * @param[out] found whether the parameter was found
 * @return Rect<int> from the JavaScript function arguments
 */
Rect<int> GetRectIntParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

/**
 * @param [in] index parameter index, e.g. callMyFunc( index0, index1, index2).
 * @param[out] found whether the parameter was found
 * @return Actor from the JavaScript function arguments
 */
Actor GetActorParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

/**
 * @param [in] index parameter index, e.g. callMyFunc( index0, index1, index2).
 * @param[out] found whether the parameter was found
 * @return Layer from the JavaScript function arguments
 */
Layer GetLayerParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

/**
 * @param [in] index parameter index, e.g. callMyFunc( index0, index1, index2).
 * @param[out] found whether the parameter was found
 * @return Image from the JavaScript function arguments
 */
Image GetImageParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

/**
 * @param [in] index parameter index, e.g. callMyFunc( index0, index1, index2).
 * @param[out] found whether the parameter was found
 * @return RenderTask from the JavaScript function arguments
 */
RenderTask GetRenderTaskParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

/**
 * Extract a DALi wrapped object, from a JavaScript object held in a function argument
 * @param[in] index argument index the object is held in
 * @param[in] type type of wrapped object to extract
 */
BaseWrappedObject* GetWrappedDaliObjectParameter( unsigned int index, BaseWrappedObject::Type type, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

/**
 * @param [in] index parameter index, e.g. callMyFunc( index0, index1, index2).
 * @param[out] found whether the parameter was found
 * @return ProperyValue from the  JavaScript function arguments
 */
Property::Value GetPropertyValueParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

/**
 * @param [in] index parameter index, e.g. callMyFunc( index0, index1, index2).
 * @param[out] found whether the parameter was found
 * @return ProperyMap from the  JavaScript function arguments
 */
Property::Map GetPropertyMapParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

/**
 * Generate a JavaScript property map, from a DALi property map
 * @param [in] DALi map property map
 * @param [in] JavaScript property map
 */
void CreatePropertyMap( v8::Isolate* isolate, const Property::Map& map, v8::Local<v8::Object>& object );


/**
 * Read multiple float arguments from v8 args object
 * e.g. myJavaFunc( 3.4, 7.6, 8.5 );
 *
 * @param[out] foundAllArguments flag to say all arguments are found
 * @param[out] data pointer to a pre-allocated array of floats
 * @param[in] dataSize the nunber of floats in the data parameter
 * @param[in] defaultValue default value assigned to floats that don't exist in the arguments
 */
void ReadFloatArguments( bool& foundAllArguments, float* data, unsigned int dataSize, const v8::FunctionCallbackInfo< v8::Value >& args, float defaultValue );

/**
 * Read multiple integer arguments from v8 args object
 * e.g. myJavaFunc( 3, 7, 8 );
 *
 * @param[out] foundAllArguments flag to say all arguments are found
 * @param[out] data pointer to a pre-allocated array of integers
 * @param[in] dataSize the nunber of integers in the data parameter
 * @param[in] defaultValue default value assigned to integers that don't exist in the arguments
 */
void ReadIntegerArguments( bool& foundAllArguments, int* data, int dataSize, const v8::FunctionCallbackInfo< v8::Value >& args, int defaultValue );

} // namespace V8Utils

} // namespace V8Plugin

} // namespace Dali


#endif // __DALI_V8PLUGIN_V8_UTILS_H__

