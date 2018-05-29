#ifndef __DALI_TOOLKIT_EVENT_THREAD_CALLBACK_H__
#define __DALI_TOOLKIT_EVENT_THREAD_CALLBACK_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#define __DALI_EVENT_THREAD_CALLBACK_H_

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali/public-api/signals/callback.h>

namespace Dali
{

class DALI_TOOLKIT_API EventThreadCallback
{
public:

  EventThreadCallback( CallbackBase* callback );

  ~EventThreadCallback();

  void Trigger();

  bool WaitingForTrigger();

  CallbackBase* GetCallback();

private:

  // undefined copy constructor.
  EventThreadCallback( const EventThreadCallback& );

  // undefined assignment operator
  EventThreadCallback& operator=( const EventThreadCallback& );

private:

  struct Impl;
  Impl* mImpl;
};

}

namespace Test
{

/**
 * Wait for the tested code to create an event trigger, then
 * wait for triggerCount Trigger calls to occur, and execute the trigger
 * callback afterwards.
 *
 * Will wait for a maximum of 30s before failing the test and returning.
 */
bool WaitForEventThreadTrigger( int triggerCount, int timeoutInSeconds=30 );

}


#endif /* __DALI_TOOLKIT_EVENT_THREAD_CALLBACK_H__ */
