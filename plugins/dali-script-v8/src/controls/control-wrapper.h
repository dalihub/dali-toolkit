#ifndef __DALI_V8PLUGIN_CONTROL_WRAPPER_H__
#define __DALI_V8PLUGIN_CONTROL_WRAPPER_H__

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
#include <dali/public-api/common/dali-vector.h>
#include <dali-toolkit/public-api/controls/control.h>

// INTERNAL INCLUDES
#include <actors/actor-wrapper.h>

namespace Dali
{

namespace V8Plugin
{


/**
 * Wraps a Dali Control.
 */
class ControlWrapper : public ActorWrapper
{

public:

  /**
   * Control type used an index.
   * These enums are used to index the ControlApiLookup table in control-wrapper.cpp.
   * Any changes made must be reflected in the ControlApiLookup otherwise it may segfault when creating a control
   */
  enum ControlType
  {
    UNKNOWN_CONTROL = -1,
    CONTROL         = 0,
    ITEMVIEW        = 1,
    SCROLLVIEW      = 2
  };

  /**
   * Constructor
   * @param control DALi control
   * @param gc garbage collection interface
   */
  ControlWrapper( Toolkit::Control control,
                GarbageCollectorInterface& gc );

  /**
   * destructor
   */
  virtual ~ControlWrapper();

  /**
   * @brief Creates a new Control wrapped inside a Javascript Object.
   * @note: the control type is passed as a parameter e.g. 'ItemView'
   * @param[in] args v8 function call arguments interpreted
   */
  static void NewControl( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * @brief Wraps a control of a given type
   */
  static v8::Handle<v8::Object> WrapControl(v8::Isolate* isolate, Toolkit::Control control, ControlType controlType);

  /**
   * @brief Wraps a control, the type is looked up from the control
   */
  static v8::Handle<v8::Object> WrapControl(v8::Isolate* isolate, Toolkit::Control control );

  // The Control ObjectTemplates.
  static v8::Persistent<v8::ObjectTemplate> mControlTemplate;
  static v8::Persistent<v8::ObjectTemplate> mItemViewTemplate;
  static v8::Persistent<v8::ObjectTemplate> mScrollViewTemplate;

  /**
   * @return the wrapped control
   */
  Toolkit::Control GetControl();

  /**
   * @return the control type
   */
  static ControlWrapper::ControlType GetControlType( const std::string& name );

  /**
   * @brief Register the garbage to be released when the wrapped control is deleted.
   */
  static void RegisterGarbage(void* garbage);

private:

  /**
   * Helper to make the control template
   */
  static v8::Handle<v8::ObjectTemplate> MakeDaliControlTemplate( v8::Isolate* isolate, ControlType controlType );

  /**
   * Helper, get a control template given a control type
   */
  static v8::Local<v8::ObjectTemplate> GetControlTemplate( v8::Isolate* isolate, ControlType type );

  Toolkit::Control mControl;
  static Vector< void* > mControlGarbageContainer;

};

} // namespace V8Plugin

} // namespace Dali

#endif // header
