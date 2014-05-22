#ifndef __DALI_TOOLKIT_TOOLKIT_TIMER_H__
#define __DALI_TOOLKIT_TOOLKIT_TIMER_H__

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

// EXTERNAL INCLUDES
#include <string>

// PUBLIC INCLUDES
#define __DALI_TIMER_H__
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali
{

class Timer : public BaseHandle
{
public:
  typedef SignalV2< bool () > TimerSignalV2;
  static Timer New( unsigned int milliSec );
  Timer();
  Timer( const Timer& timer );
  Timer& operator=( const Timer& timer );
  virtual ~Timer();
  static Timer DownCast( BaseHandle handle );
  using BaseHandle::operator=;
  void Start();
  void Stop();
  void SetInterval( unsigned int milliSec );
  unsigned int GetInterval() const;
  bool IsRunning() const;
  TimerSignalV2& TickSignal();
private:
  Dali::Timer::TimerSignalV2 mTickSignal;
  unsigned int mInterval;
  bool mIsRunning;
};

} // namespace Dali

#endif // __DALI_TOOLKIT_TOOLKIT_TIMER_H__
