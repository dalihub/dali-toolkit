#ifndef __DALI_V8PLUGIN_GARBAGE_COLLECTOR_H__
#define __DALI_V8PLUGIN_GARBAGE_COLLECTOR_H__

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


// INTERNAL INCLUDES
#include <interfaces/garbage-collector-interface.h>
#include <dali/public-api/common/set-wrapper.h>


namespace Dali
{

namespace V8Plugin
{


/**
 * @brief concrete interface of Dali's garbage collector interface.
 */
class GarbageCollector : public GarbageCollectorInterface
{

public:

  /**
   * Constructor
   */
  GarbageCollector();

  /**
   * Destructor
   */
  ~GarbageCollector();

  /**
   * @copydoc GarbageCollectorInterface::Register()
   */
  virtual void Register( BaseWrappedObject* object );

  /**
   * @copydoc GarbageCollectorInterface::UnRegister()
   */
  virtual void UnRegister( BaseWrappedObject* object );

  /**
   * @copydoc GarbageCollectorInterface::GarbageCollect()
   */
  virtual void GarbageCollect();


private:

  /**
   * Map between dali wrapped object (void *)
   */
  typedef std::set< BaseWrappedObject* > ObjectMap;
  ObjectMap mObjectMap;   ///< lookup

};

} // V8Plugin

} // Dali

#endif // header __DALI_V8PLUGIN_GARBAGE_COLLECTOR_H__
