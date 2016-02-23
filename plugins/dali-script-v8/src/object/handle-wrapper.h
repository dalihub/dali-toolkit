#ifndef __DALI_V8PLUGIN_HANDLE_WRAPPER_H__
#define __DALI_V8PLUGIN_HANDLE_WRAPPER_H__

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
#include <dali/public-api/object/handle.h>

// INTERNAL INCLUDES
#include <shared/base-wrapped-object.h>
#include <signals/signal-manager.h>

namespace Dali
{

namespace V8Plugin
{

class BaseCallBack;

class HandleWrapper : public BaseWrappedObject
{
public:
  HandleWrapper( BaseWrappedObject::Type type,
                Handle handle,
                GarbageCollectorInterface& gc );

  virtual ~HandleWrapper();

  /**
   * @brief get the value for a property for JavaScript object than contains a Dali Handle.
   * E.g. Get( "x", JavaScript object that wraps a Dali Handle )
   * @param[in] propertyName property name
   * @param[in] info reference to PropertyCallbackInfo structure (contains the Javascript
   * object and the return value).
   */
  static void PropertyGet( v8::Local<v8::String> propertyName,
                                          const v8::PropertyCallbackInfo<v8::Value>& info);

  /**
   * @brief Set the value for a property for JavaScript object than contains a Dali Handle.
   * E.g. Set( "x", 103, JavaScript object that wraps a Dali Handle)
   * @param[in] propertyName property name
   * @param[in] javaScriptValue javascript value to set, this is typically a number
   * @param[in] info reference to PropertyCallbackInfo structure (contains the Javascript
   * object).
   */
  static void PropertySet( v8::Local<v8::String> propertyName,
                    v8::Local<v8::Value> javaScriptValue,
                    const v8::PropertyCallbackInfo<v8::Value>& info);

  static HandleWrapper* Unwrap( v8::Isolate* isolate, v8::Handle< v8::Object> obj);

  /**
   * Should be called by an class that inherits from HandleWrapper to add
   * property get / set functionality to the javascript object
   * along with Signal connect / disconnect
   */
  static void AddInterceptsToTemplate( v8::Isolate* isolate, v8::Local<v8::ObjectTemplate>& objTemplate );

  /**
   * @brief Register an animatable property for a JavaScript object that
   * contains a Dali Handle.
   * @param[in] args v8 function call arguments interpreted
   */
  static void RegisterAnimatableProperty( const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * @brief Register a custom property for a JavaScript object that
   * contains a Dali Handle.
   * @param[in] args v8 function call arguments interpreted
   */
  static void RegisterCustomProperty( const v8::FunctionCallbackInfo< v8::Value >& args );

  Handle GetHandle() { return mHandle; }
  Handle mHandle;
  ConnectionTracker mConnectionTracker;


  virtual SignalManager* GetSignalManager() { return &mSignalManager;}

private:

  SignalManager mSignalManager;

};


} // namespace V8Plugin

} // namespace Dali

#endif // header
