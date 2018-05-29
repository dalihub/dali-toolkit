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

#include "toolkit-timer.h"

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{
class Timer;

typedef IntrusivePtr<Timer> TimerPtr;

Dali::Timer::TimerSignalType gTickSignal;
int gTimerCount = 0;
bool gKeepTimersRunning = false;

/**
 * Implementation of the timer
 */
class Timer : public BaseObject
{
public:
  void MockEmitSignal();

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

  Dali::Timer::TimerSignalType& TickSignal();

private: // Implementation

  // not implemented
  Timer( const Timer& );
  Timer& operator=( const Timer& );

private: // Data

  unsigned int mInterval;
  bool mRunning;
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
  TimerPtr timerImpl = new Timer( milliSec );
  return timerImpl;
}

Timer::Timer( unsigned int milliSec )
: mInterval( milliSec ),
  mRunning( false )
{
  ++gTimerCount;
}

Timer::~Timer()
{
  --gTimerCount;
}

void Timer::Start()
{
  mRunning = true;
}

void Timer::Stop()
{
  mRunning = false;
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
  return mRunning;
}

bool Timer::Tick()
{
  return false;
}

Dali::Timer::TimerSignalType& Timer::TickSignal()
{
  return gTickSignal;
}

// Mock setup functions:

void Timer::MockEmitSignal()
{
  if( gTimerCount > 1 )
  {
    // Only emit the signal if we have more than just the timer created in the test function
    gTickSignal.Emit();
  }
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
  Dali::Internal::Adaptor::gKeepTimersRunning = true;
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

Timer::TimerSignalType& Timer::TickSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).TickSignal();
}

Timer::Timer(Internal::Adaptor::Timer* timer)
: BaseHandle(timer)
{
}

// Mock setup functions:

void Timer::MockEmitSignal()
{
  Internal::Adaptor::GetImplementation( *this ).MockEmitSignal();
}

} // namespace Dali


namespace Test
{

int GetTimerCount()
{
  return Dali::Internal::Adaptor::gTimerCount;
}

void EmitGlobalTimerSignal()
{
  // @todo Multiplex timers properly.
  Dali::Internal::Adaptor::gKeepTimersRunning = Dali::Internal::Adaptor::gTickSignal.Emit();
}

bool AreTimersRunning()
{
  return Dali::Internal::Adaptor::gKeepTimersRunning;
}

}
