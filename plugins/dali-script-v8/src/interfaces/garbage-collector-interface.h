#ifndef __DALI_V8PLUGIN_GARBAGE_COLLECTOR_INTERFACE_H__
#define __DALI_V8PLUGIN_GARBAGE_COLLECTOR_INTERFACE_H__

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

class BaseWrappedObject;

/**
 * @brief Tracks all Dali-wrapped objects that are used by v8.
 * Required to delete any wrapped objects that are not cleaned up after V8 has shut down.
 *
 * Unfortunately the v8 garbage collection may never run while executing a script,
 * and currently doesn't run on shutdown (for performance reasons with Chrome browser).
 *
 * This means we have to manually keep track of all objects, and perform our
 * own garbage collection on shut down.
 *
 * For every object created:
 * - Store a weak handle to it
 * - Register a callback to be informed if v8 garbage collector decides it's no longer required.
 * -- Call back is done using v8 SetWeak(), which states:
 *    "There is no guarantee as to *when* or even *if* the callback is invoked."
 *
 */
class GarbageCollectorInterface
{

public:

  /**
   * Register an object with Dali's garbage collector
   * @param object a wrapped object
   */
  virtual void Register( BaseWrappedObject* object ) = 0;

  /**
   * Un-register an object with Dali's garbage collector
   * @param object a wrapped object
   */
  virtual void UnRegister( BaseWrappedObject* object )  = 0;

  /**
   * Delete all
   */
  virtual void GarbageCollect() = 0;

protected:

  /**
   * Constructor
   */
  GarbageCollectorInterface()
  {
  }

  /**
   * virtual destructor
   */
  virtual ~GarbageCollectorInterface()
  {
  }

  // Undefined copy constructor.
  GarbageCollectorInterface( const GarbageCollectorInterface& );

  // Undefined assignment operator.
  GarbageCollectorInterface& operator=( const GarbageCollectorInterface& );


};

} // V8Plugin

} // Dali

#endif // header __DALI_V8PLUGIN_GARBAGE_COLLECTOR_INTERFACE_H__
