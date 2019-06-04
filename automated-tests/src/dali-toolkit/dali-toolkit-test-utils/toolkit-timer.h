#ifndef DALI_TOOLKIT_TOOLKIT_TIMER_H
#define DALI_TOOLKIT_TOOLKIT_TIMER_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#define DALI_TIMER_H
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{

namespace Internal
{
namespace Adaptor
{
class Timer;
}
}

class Timer : public BaseHandle
{
public:
  void MockEmitSignal();

public:
  typedef Signal< bool () > TimerSignalType;
  Timer();
  static Timer New( unsigned int milliSec );
  Timer( const Timer& timer );
  Timer& operator=( const Timer& timer );
  ~Timer();
  static Timer DownCast( BaseHandle handle );
  void Start();
  void Stop();
  void SetInterval( unsigned int milliSec );
  unsigned int GetInterval() const;
  bool IsRunning() const;
  TimerSignalType& TickSignal();
private:
  Timer(Internal::Adaptor::Timer* timer);
};

} // namespace Dali

namespace Test
{
int GetTimerCount();
void EmitGlobalTimerSignal();
bool AreTimersRunning();
}

#endif // DALI_TOOLKIT_TOOLKIT_TIMER_H
