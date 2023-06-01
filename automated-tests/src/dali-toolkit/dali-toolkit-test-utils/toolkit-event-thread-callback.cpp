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
#include <math.h>
#include <semaphore.h>
#include <unistd.h>
#include <algorithm>
#include <climits>
#include <cstddef>
#include <cstdio>
#include <ctime>
#include <vector>

namespace
{
// Note, this is not thread safe - however, should not be using
// triggers from multiple threads - they should all be created on
// event thread.
std::vector<Dali::EventThreadCallback*> gEventThreadCallbacks;
} // namespace

namespace Dali
{
struct EventThreadCallback::Impl
{
  CallbackBase* callback;
  sem_t         mySemaphore;
};

EventThreadCallback::EventThreadCallback(CallbackBase* callback)
: mImpl(new Impl())
{
  mImpl->callback = callback;
  sem_init(&(mImpl->mySemaphore), 0, 0);

  gEventThreadCallbacks.push_back(this);
}

EventThreadCallback::~EventThreadCallback()
{
  std::vector<EventThreadCallback*>::iterator iter =
    std::find(gEventThreadCallbacks.begin(), gEventThreadCallbacks.end(), this);
  if(iter != gEventThreadCallbacks.end())
  {
    gEventThreadCallbacks.erase(iter);
  }
  delete mImpl;
}

void EventThreadCallback::Trigger()
{
  sem_post(&(mImpl->mySemaphore));
}

// returns true if timed out rather than triggered
bool EventThreadCallback::WaitingForTrigger()
{
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  if(now.tv_nsec < 999900000) // 999, 900, 000
    now.tv_nsec += 1000;
  else
  {
    now.tv_sec += 1;
    now.tv_nsec = 0;
  }

  int error = sem_timedwait(&(mImpl->mySemaphore), &now);
  return error != 0; // true if timeout
}

CallbackBase* EventThreadCallback::GetCallback()
{
  return mImpl->callback;
}

} // namespace Dali

namespace Test
{
bool WaitForEventThreadTrigger(int triggerCount, int timeoutInSeconds, int executeCallbacks)
{
  struct timespec startTime;
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &startTime);
  now.tv_sec  = startTime.tv_sec;
  now.tv_nsec = startTime.tv_nsec;

  // Round robin poll of each semaphore:
  while(triggerCount > 0)
  {
    if(gEventThreadCallbacks.size() > 0)
    {
      for(std::vector<Dali::EventThreadCallback*>::iterator iter = gEventThreadCallbacks.begin();
          iter != gEventThreadCallbacks.end();
          ++iter)
      {
        Dali::EventThreadCallback* eventTrigger = (*iter);
        Dali::CallbackBase*        callback     = eventTrigger->GetCallback();
        bool                       timedout     = eventTrigger->WaitingForTrigger();
        if(!timedout)
        {
          if(executeCallbacks)
          {
            // Semaphore was unlocked - execute the trigger
            Dali::CallbackBase::Execute(*callback);
          }
          triggerCount--;
        }
        if(triggerCount <= 0)
        {
          break;
        }
      }
    }
    clock_gettime(CLOCK_REALTIME, &now);
    if(now.tv_sec - startTime.tv_sec > timeoutInSeconds)
    {
      // Ensure we break out of the loop if elapsed time has passed
      break;
    }
  }

  clock_gettime(CLOCK_REALTIME, &now);
  if(now.tv_sec > startTime.tv_sec + 1)
  {
    fprintf(stderr, "WaitForEventThreadTrigger took %ld seconds\n", now.tv_sec - startTime.tv_sec);
  }
  return triggerCount == 0;
}

} // namespace Test
