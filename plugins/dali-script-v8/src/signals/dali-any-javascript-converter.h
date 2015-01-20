#ifndef __DALI_V8PLUGIN_DALI_ANY_JAVASCRIPT_CONVERTER_H__
#define __DALI_V8PLUGIN_DALI_ANY_JAVASCRIPT_CONVERTER_H__

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
#include <dali/public-api/object/any.h>

namespace Dali
{

namespace V8Plugin
{

namespace DaliAnyConverter
{


/**
 * Converts a Dali::Any object to a JavaScript object based on it's type
 */
v8::Local<v8::Value> ConvertToJavaScriptObject( v8::Isolate* isolate, Dali::Any& value );


/**
 * Converts from a JavaScript object back into a Dali::Any object
 * Used for return values from JavaScript callbacks
 * E.g. to convert from a JavaScript bool object to a C++ bool
 */

Dali::Any ConvertToDaliAny( v8::Isolate* isolate, v8::Local<v8::Value>, const Dali::Any& requestedType);

}

} // namespace V8Plugin

} // namespace Dali

#endif // __DALI_V8PLUGIN_DALI_ANY_JAVASCRIPT_CONVERTER_H__
