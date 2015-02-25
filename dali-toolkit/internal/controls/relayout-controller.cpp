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

// CLASS HEADER
#include "relayout-controller.h"

// EXTERNAL INCLUDES
#include <stack>
#include <sstream>
#include <dali/public-api/adaptor-framework/singleton-service.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES

#include "relayout-controller-impl.h"
#include "dali-toolkit/public-api/controls/control.h"
#include "dali-toolkit/public-api/controls/control-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
// Flag to avoid doing more than one request per frame
// getting the singleton handle can be expensive as it requires calling to adaptor, dynamic cast etc
// and it can get called 100 times per frame easily in startup and new view initialization
  bool gRelayoutRequestPending = false;
}

RelayoutController::RelayoutController()
{

}

RelayoutController::~RelayoutController()
{

}

void RelayoutController::Request()
{
  // are we already going to do it this frame
  if( !gRelayoutRequestPending )
  {
    RelayoutController controller;

    // Check whether the RelayoutController is already created
    SingletonService singletonService( SingletonService::Get() );
    if ( singletonService )
    {
      Dali::BaseHandle handle = singletonService.GetSingleton(typeid(RelayoutController));

      if(handle)
      {
        // If so, downcast the handle of singleton to RelayoutController
        controller = RelayoutController(dynamic_cast<Internal::RelayoutControllerImpl*>(handle.GetObjectPtr()));
      }

      if(!controller)
      {
        // If not, create the RelayoutController and register it as a singleton
        controller = RelayoutController( new Internal::RelayoutControllerImpl(gRelayoutRequestPending) );
        singletonService.Register( typeid(controller), controller );
      }

      GetImpl(controller).Request();
      gRelayoutRequestPending = true;
    }
  }
}

RelayoutController::RelayoutController(Internal::RelayoutControllerImpl *impl)
  : BaseHandle(impl)
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

