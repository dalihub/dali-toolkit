#ifndef __DALI_V8PLUGIN_KEYBOARD_FOCUS_MANAGER_WRAPPER_H__
#define __DALI_V8PLUGIN_KEYBOARD_FOCUS_MANAGER_WRAPPER_H__

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
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>

// INTERNAL INCLUDES
#include <shared/base-wrapped-object.h>
#include <signals/signal-manager.h>

namespace Dali
{

namespace V8Plugin
{

/**
 * KeyboardFocusManager  wrapper.
 * Provides access to KeyboardFocusManager specific functionality
 */
class KeyboardFocusManagerWrapper : public BaseWrappedObject
{

public:

  KeyboardFocusManagerWrapper( const Dali::Toolkit::KeyboardFocusManager& keyboardFocusManager,
                GarbageCollectorInterface& gc );

  virtual ~KeyboardFocusManagerWrapper();

/*
  *
   * @brief Creates a new KeyboardFocusManager wrapped inside a Javascript Object.
   * @param[in] args v8 function call arguments interpreted

  static void NewKeyboardFocusManager( const v8::FunctionCallbackInfo< v8::Value >& args);
*/

  /**
   * Wraps a keyboardFocusManager
   */
  static v8::Handle<v8::Object> WrapKeyboardFocusManager(v8::Isolate* isolate, const Dali::Toolkit::KeyboardFocusManager& );


  Dali::Toolkit::KeyboardFocusManager GetKeyboardFocusManager();


  virtual SignalManager* GetSignalManager() { return &mSignalManager;}

  static std::string GetDirectionName(  Toolkit::Control::KeyboardFocusNavigationDirection dir );

private:


  static v8::Local<v8::ObjectTemplate> GetKeyboardFocusManagerTemplate( v8::Isolate* isolate );
  Dali::Toolkit::KeyboardFocusManager mKeyboardFocusManager;
  SignalManager mSignalManager;
};

} // namespace V8Plugin

} // namespace Dali

#endif // __DALI_V8PLUGIN_KEYBOARD_FOCUS_MANAGER_WRAPPER_H__
