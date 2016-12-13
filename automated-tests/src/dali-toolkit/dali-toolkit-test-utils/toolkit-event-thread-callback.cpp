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
#include <ctime>
#include <climits>
#include <cstdio>
#include <unistd.h>

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

bool EventThreadCallback::WaitingForTrigger(unsigned int count, unsigned int seconds)
{
  if(  mImpl->triggeredCount >= count )
  {
    return true;
  }
  struct timespec now;
  clock_gettime( CLOCK_REALTIME, &now );
  now.tv_sec += seconds;
  mImpl->expectedCount = count;
  int error = sem_timedwait( &(mImpl->mySemaphore), &now );
  return error != 0;
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

namespace Test
{

bool WaitForEventThreadTrigger( int triggerCount )
{
  bool success = true;
  const int TEST_TIMEOUT(30);

  struct timespec startTime;
  struct timespec now;
  clock_gettime( CLOCK_REALTIME, &startTime );
  now.tv_sec = startTime.tv_sec;
  now.tv_nsec = startTime.tv_nsec;

  Dali::EventThreadCallback* eventTrigger = NULL;
  while( eventTrigger == NULL )
  {
    eventTrigger = Dali::EventThreadCallback::Get();
    clock_gettime( CLOCK_REALTIME, &now );
    if( now.tv_sec - startTime.tv_sec > TEST_TIMEOUT )
    {
      success = false;
      break;
    }
    usleep(10);
  }
  if( eventTrigger != NULL )
  {
    Dali::CallbackBase* callback = eventTrigger->GetCallback();
    eventTrigger->WaitingForTrigger( triggerCount, TEST_TIMEOUT - (now.tv_sec - startTime.tv_sec) );
    Dali::CallbackBase::Execute( *callback );
  }

  clock_gettime( CLOCK_REALTIME, &now );
  if( now.tv_sec > startTime.tv_sec + 1 )
  {
    fprintf(stderr, "WaitForEventThreadTrigger took %ld seconds\n", now.tv_sec - startTime.tv_sec );
  }
  return success;
}

}
