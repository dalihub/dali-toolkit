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

// CLASS HEADER
#include "garbage-collector.h"

// INTERNAL INCLUDES
#include <shared/base-wrapped-object.h>

namespace Dali
{

namespace V8Plugin
{

GarbageCollector::GarbageCollector()
{

}
GarbageCollector::~GarbageCollector()
{
  GarbageCollect();
}

void GarbageCollector::Register( BaseWrappedObject* object )
{
  mObjectMap.insert( object );
};


void GarbageCollector::UnRegister( BaseWrappedObject* object )
{
  mObjectMap.erase( object );
}


void GarbageCollector::GarbageCollect()
{
  for( ObjectMap::iterator iter = mObjectMap.begin(); iter != mObjectMap.end();  )
  {
    BaseWrappedObject* object = *iter;
    iter++; // iterator will be invalidated if we delete the object first.
    delete object; // object will call GarbageCollector.UnRegister
  }

}

} // V8Plugin

} // Dali
