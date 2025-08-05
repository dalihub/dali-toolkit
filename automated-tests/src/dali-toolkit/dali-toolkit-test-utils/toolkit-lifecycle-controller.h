#ifndef TOOLKIT_LIFECYCLE_CONTROLLER_H
#define TOOLKIT_LIFECYCLE_CONTROLLER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>
#include <string>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
class LifecycleController;
}
} // namespace Internal

class LifecycleController : public BaseHandle
{
public:
  typedef Signal<void(void)> LifecycleSignalType;
  LifecycleController();
  ~LifecycleController();
  static LifecycleController Get();
  LifecycleSignalType&       PreInitSignal();
  LifecycleSignalType&       InitSignal();
  LifecycleSignalType&       TerminateSignal();
  LifecycleController(Internal::Adaptor::LifecycleController* impl);
};

} // namespace Dali

#endif // TOOLKIT_LIFECYCLE_CONTROLLER_H
