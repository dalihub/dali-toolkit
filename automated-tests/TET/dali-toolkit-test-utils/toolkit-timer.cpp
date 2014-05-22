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

namespace
{

Timer gTimer;

/*
 * This is a global signal that all users of this stub will connect to.
 * If we were to use a timer stub with real ticks we may wish to have this as a member variable created with each New Timer.
 * Currently unable to get the boost singal connect to work with the stub timer when using a member hence this global.
 */
Dali::Timer::TimerSignalV2 gTimerTick;
}

/**
 * Stub for the Timer
 */

Timer Timer::New( unsigned int milliSec )
{
  DALI_ASSERT_ALWAYS( gTimer );

  return gTimer;
}

Timer::TimerSignalV2& Timer::TickSignal()
{
   return gTimerTick;
}

bool Timer::IsRunning() const
{
  return true;
}

Timer::Timer( const Timer& timer )
: BaseHandle(timer)
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

namespace Internal
{

namespace Adaptor
{

/**
 * Stub for the Internal Timer
 */
class Timer : public BaseObject
{
public: // Creation & Destruction

  Timer();
  Timer(ToolkitTimer *timer);
  ~Timer();

  void Start();

  void Stop();

  bool IsRunning() const;

public: // Signals

private:

  ToolkitTimer* mToolkitTimer;
  bool mRunning;
};

Timer::Timer()
: mToolkitTimer(NULL),
  mRunning( true )
{
}

Timer::Timer(ToolkitTimer *timer)
: mToolkitTimer(timer),
  mRunning( true )
{
}

Timer::~Timer()
{
}

void Timer::Start()
{
  DALI_ASSERT_ALWAYS( gTimer );
}

void Timer::Stop()
{
  DALI_ASSERT_ALWAYS( gTimer );
}

bool Timer::IsRunning() const
{
  DALI_ASSERT_ALWAYS( gTimer );
  return mRunning;
}

} // namespace Adaptor

} // namespace Internal

////////////////////////////////////////////////////////////////////////////////////////////////////

ToolkitTimer::ToolkitTimer()
: mTimerStub(new Internal::Adaptor::Timer(this)),
  mTimer( mTimerStub )
{
  if ( mTimer )
  {
    gTimer = mTimer;
  }
}

ToolkitTimer::~ToolkitTimer()
{
  gTimer.Reset();
}

Timer ToolkitTimer::GetTimer()
{
  return mTimer;
}

} // namespace Dali
