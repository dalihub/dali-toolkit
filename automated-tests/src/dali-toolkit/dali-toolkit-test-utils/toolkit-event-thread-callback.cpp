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
 */

// CLASS HEADER
#include "toolkit-event-thread-callback.h"

// EXTERNAL INCLUDES
#include <cstddef>
#include <semaphore.h>
#include <math.h>
#include <climits>

namespace Dali
{

namespace
{
EventThreadCallback* gEventThreadCallback = NULL;
}

struct EventThreadCallback::Impl
{
  CallbackBase* callback;
  unsigned int triggeredCount;
  unsigned int expectedCount;
  sem_t mySemaphore;
};

EventThreadCallback::EventThreadCallback( CallbackBase* callback )
: mImpl( new Impl() )
{
  mImpl->callback = callback;
  mImpl->triggeredCount = 0u;
  mImpl->expectedCount = UINT_MAX;
  sem_init( &(mImpl->mySemaphore), 0, 0 );
  gEventThreadCallback = this;
}

EventThreadCallback::~EventThreadCallback()
{
  delete mImpl;
}

void EventThreadCallback::Trigger()
{
  mImpl->triggeredCount++;
  if(  mImpl->triggeredCount >= mImpl->expectedCount )
  {
    sem_post( &(mImpl->mySemaphore) );
  }
}

void EventThreadCallback::WaitingForTrigger(unsigned int count)
{
  if(  mImpl->triggeredCount >= count )
  {
    return;
  }
  mImpl->expectedCount = count;
  sem_wait( &(mImpl->mySemaphore) );
}

CallbackBase* EventThreadCallback::GetCallback()
{
  return mImpl->callback;
}

EventThreadCallback* EventThreadCallback::Get()
{
  return gEventThreadCallback;
}

}
