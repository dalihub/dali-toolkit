#ifndef __DALI_V8PLUGIN_KEYBOARD_FOCUS_MANAGER_API_H__
#define __DALI_V8PLUGIN_KEYBOARD_FOCUS_MANAGER_API_H__

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

namespace Dali
{

namespace V8Plugin
{

namespace KeyboardFocusManagerApi
{
  /**
   * Constructor
   */
  Dali::Toolkit::KeyboardFocusManager New( const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * KeyboardFocusManager API see keyboard-focus-manager.h for a description
   */
  void SetCurrentFocusActor( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetCurrentFocusActor( const v8::FunctionCallbackInfo< v8::Value >& args );
  void MoveFocus( const v8::FunctionCallbackInfo< v8::Value >& args );
  void ClearFocus( const v8::FunctionCallbackInfo< v8::Value >& args );

  void SetAsFocusGroup( const v8::FunctionCallbackInfo< v8::Value >& args );
  void IsFocusGroup( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetFocusGroup( const v8::FunctionCallbackInfo< v8::Value >& args );

  void SetFocusGroupLoop( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetFocusGroupLoop( const v8::FunctionCallbackInfo< v8::Value >& args );

  void SetFocusIndicatorActor( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetFocusIndicatorActor( const v8::FunctionCallbackInfo< v8::Value >& args );

}; // namespace KeyboardFocusManagerApi

} // namespace V8Plugin

} // namespace Dali

#endif // header __DALI_V8PLUGIN_KEYBOARD_FOCUS_MANAGER_API_H__
