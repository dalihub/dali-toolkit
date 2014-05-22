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
#include <dali/public-api/adaptor-framework/common/timer.h>

namespace Dali
{

namespace Internal
{
namespace Adaptor
{
class Timer;

}
}

/**
 * This creates a stubbed Timer so that internal Toolkit Adaptor calls work.
 */
class ToolkitTimer
{
public: // Constants

public: // Construction & Destruction

  ToolkitTimer();
  ~ToolkitTimer();

public:

  Timer GetTimer();

public: // Signal Emissions

  Dali::Timer::TimerSignalV2& TickSignal();

public: // TEST FUNCTIONS

private:

  Internal::Adaptor::Timer* mTimerStub;
  friend class Internal::Adaptor::Timer;
  Timer mTimer; // Hold a handle ourselves.
};

} // namespace Dali

#endif // __DALI_TOOLKIT_TOOLKIT_TIMER_H__
