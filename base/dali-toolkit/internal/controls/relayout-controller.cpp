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

// FILE HEADER

#include "relayout-controller.h"
#include "relayout-controller-impl.h"

// EXTERNAL INCLUDES

#include <boost/thread/tss.hpp>
#include <stack>
#include <sstream>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES

#include "dali-toolkit/public-api/controls/control.h"
#include "dali-toolkit/public-api/controls/control-impl.h"
#include "dali-toolkit/public-api/controls/text-view/text-view.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

RelayoutController::RelayoutController()
{

}

RelayoutController::~RelayoutController()
{

}

RelayoutController RelayoutController::Get()
{
  RelayoutController controller;

  // Check whether the RelayoutController is already created
  Dali::Adaptor& adaptor = Dali::Adaptor::Get();
  Dali::BaseHandle handle = adaptor.GetSingleton(typeid(RelayoutController));

  if(handle)
  {
    // If so, downcast the handle of singleton to RelayoutController
    controller = RelayoutController(dynamic_cast<Internal::RelayoutControllerImpl*>(handle.GetObjectPtr()));
  }

  if(!controller)
  {
    // If not, create the RelayoutController and register it as a singleton
    controller = RelayoutController(new Internal::RelayoutControllerImpl());
    adaptor.RegisterSingleton(typeid(controller), controller);
  }

  return controller;
}

RelayoutController::RelayoutController(Internal::RelayoutControllerImpl *impl)
  : BaseHandle(impl)
{
}

void RelayoutController::Request()
{
  GetImpl(*this).Request();
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali

