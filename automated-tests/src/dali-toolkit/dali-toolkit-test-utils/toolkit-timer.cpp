/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include "toolkit-timer.h"

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{
class Timer;

typedef IntrusivePtr<Timer> TimerPtr;

/**
 * Implementation of the timer
 */
class Timer : public BaseObject
{
public:
  static TimerPtr New( unsigned int milliSec );
  Timer( unsigned int milliSec );
  virtual ~Timer();

  void Start();
  void Stop();
  void SetInterval( unsigned int interval );
  unsigned int GetInterval() const;
  bool IsRunning() const;
  bool Tick();

public: // Signals

  Dali::Timer::TimerSignalV2& TickSignal();

private: // Implementation

  // not implemented
  Timer( const Timer& );
  Timer& operator=( const Timer& );

private: // Data

  Dali::Timer::TimerSignalV2 mTickSignal;
  unsigned int mInterval;
};

inline Timer& GetImplementation(Dali::Timer& timer)
{
  DALI_ASSERT_ALWAYS(timer && "Timer handle is empty");

  BaseObject& handle = timer.GetBaseObject();

  return static_cast<Internal::Adaptor::Timer&>(handle);
}

inline const Timer& GetImplementation(const Dali::Timer& timer)
{
  DALI_ASSERT_ALWAYS(timer && "Timer handle is empty");

  const BaseObject& handle = timer.GetBaseObject();

  return static_cast<const Internal::Adaptor::Timer&>(handle);
}

TimerPtr Timer::New( unsigned int milliSec )
{
  TimerPtr timerImpl = new Timer(10);
  return timerImpl;
}

Timer::Timer( unsigned int milliSec )
: mInterval( milliSec )
{
}

Timer::~Timer()
{
}

void Timer::Start()
{
}

void Timer::Stop()
{
}

void Timer::SetInterval( unsigned int interval )
{
  mInterval = interval;
}

unsigned int Timer::GetInterval() const
{
  return mInterval;
}

bool Timer::IsRunning() const
{
  return true;
}

bool Timer::Tick()
{
  return false;
}

Dali::Timer::TimerSignalV2& Timer::TickSignal()
{
  return mTickSignal;
}

} // namespace Adaptor

} // namespace Internal

/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

Timer::Timer()
{

}

Timer Timer::New( unsigned int milliSec )
{
  Internal::Adaptor::TimerPtr internal = Internal::Adaptor::Timer::New( milliSec );
  return Timer(internal.Get());
}

Timer::Timer( const Timer& timer )
:BaseHandle( timer )
{
}

Timer& Timer::operator=( const Timer& timer )
{
  // check self assignment
  if( *this != timer )
  {
    BaseHandle::operator=(timer);
  }
  return *this;
}

Timer::~Timer()
{
}

void Timer::Start()
{
  Internal::Adaptor::GetImplementation( *this ).Start();
}

void Timer::Stop()
{
  Internal::Adaptor::GetImplementation( *this ).Stop();
}

void Timer::SetInterval( unsigned int milliSec )
{
  Internal::Adaptor::GetImplementation( *this ).SetInterval( milliSec );
}

unsigned int Timer::GetInterval() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetInterval();
}

bool Timer::IsRunning() const
{
  return true;
}

Timer::TimerSignalV2& Timer::TickSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).TickSignal();
}

Timer::Timer(Internal::Adaptor::Timer* timer)
: BaseHandle(timer)
{
}

} // namespace Dali

