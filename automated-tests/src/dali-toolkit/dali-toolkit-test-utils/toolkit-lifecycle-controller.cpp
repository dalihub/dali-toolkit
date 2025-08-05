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

#include "toolkit-lifecycle-controller.h"

#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
/********************************************************************************
 * Stub for Dali::Internal::Adaptor::LifecycleController
 ********************************************************************************/
namespace Internal
{
namespace Adaptor
{
class LifecycleController : public BaseObject
{
public: // Creation & Destruction
  LifecycleController();
  ~LifecycleController();
  static Dali::LifecycleController Get();

public: // Signals
  Dali::LifecycleController::LifecycleSignalType& PreInitSignal();
  Dali::LifecycleController::LifecycleSignalType& InitSignal();
  Dali::LifecycleController::LifecycleSignalType& TerminateSignal();

private:
  Dali::LifecycleController::LifecycleSignalType mPreInitSignal;
  Dali::LifecycleController::LifecycleSignalType mInitSignal;
  Dali::LifecycleController::LifecycleSignalType mTerminateSignal;
  static Dali::LifecycleController               mLifecycleController;
};

Dali::LifecycleController LifecycleController::mLifecycleController;

LifecycleController::LifecycleController()
{
}

LifecycleController::~LifecycleController()
{
}

Dali::LifecycleController LifecycleController::Get()
{
  if(!mLifecycleController)
  {
    mLifecycleController = Dali::LifecycleController(new Internal::Adaptor::LifecycleController());
  }
  return mLifecycleController;
}

Dali::LifecycleController::LifecycleSignalType& LifecycleController::PreInitSignal()
{
  return mPreInitSignal;
}

Dali::LifecycleController::LifecycleSignalType& LifecycleController::InitSignal()
{
  return mInitSignal;
}

Dali::LifecycleController::LifecycleSignalType& LifecycleController::TerminateSignal()
{
  return mTerminateSignal;
}

} // namespace Adaptor
} // namespace Internal

/********************************************************************************
 * Stub for Dali::LifecycleController
 ********************************************************************************/

LifecycleController::LifecycleController()
{
}
LifecycleController::~LifecycleController()
{
}

LifecycleController LifecycleController::Get()
{
  // Get the physical keyboard handle
  LifecycleController handle = Internal::Adaptor::LifecycleController::Get();
  return handle;
}

LifecycleController::LifecycleSignalType& LifecycleController::PreInitSignal()
{
  BaseObject&                             object     = GetBaseObject();
  Internal::Adaptor::LifecycleController& controller = static_cast<Internal::Adaptor::LifecycleController&>(object);
  return controller.PreInitSignal();
}

LifecycleController::LifecycleSignalType& LifecycleController::InitSignal()
{
  BaseObject&                             object     = GetBaseObject();
  Internal::Adaptor::LifecycleController& controller = static_cast<Internal::Adaptor::LifecycleController&>(object);
  return controller.InitSignal();
}

LifecycleController::LifecycleSignalType& LifecycleController::TerminateSignal()
{
  BaseObject&                             object     = GetBaseObject();
  Internal::Adaptor::LifecycleController& controller = static_cast<Internal::Adaptor::LifecycleController&>(object);
  return controller.TerminateSignal();
}

LifecycleController::LifecycleController(Internal::Adaptor::LifecycleController* impl)
: BaseHandle(impl)
{
}

} // namespace Dali
