#ifndef __DALI_V8PLUGIN_EMIT_NOTIFICATION_INTERFACE_H__
#define __DALI_V8PLUGIN_EMIT_NOTIFICATION_INTERFACE_H__

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

namespace Dali
{

namespace V8Plugin
{

/**
 * abstract interface to allow the signal manager know if
 * a JavaScript disconnect/ connect has been called, while inside a callback
 */
class EmitNotificationInterface
{

  public:

  /**
   * Constructor
   */
  EmitNotificationInterface() {}

  /**
   * Virtual destructor
   */
  virtual ~EmitNotificationInterface() {}

  /**
   * Call back has been entered.
   * E.g. Actor on touch has called a JavaScript function
   */
   virtual void CallbackEnter() =0 ;

   /**
    * Call back has been entered.
    * E.g. Actor on touch has called a JavaScript function
    */
   virtual void CallbackExit() =0 ;

};


} // namespace V8Plugin

} // namespace Dali

#endif // __DALI_V8PLUGIN_EMIT_NOTIFICATION_INTERFACE_H__
