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
 */

// CLASS HEADER
#include <dali-toolkit/devel-api/layouting/layout-controller.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/singleton-service.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/layouting/layout-controller-impl.h>


namespace Dali
{
namespace Toolkit
{

LayoutController::LayoutController()
{
}

LayoutController::~LayoutController()
{
}

LayoutController LayoutController::Get()
{
  LayoutController layoutController;

  SingletonService singletonService( SingletonService::Get() );
  if ( singletonService )
  {
    Dali::BaseHandle handle = singletonService.GetSingleton( typeid(LayoutController) );
    if( handle )
    {
      // If so, downcast the handle of singleton to layout controller
      layoutController = LayoutController( dynamic_cast<Internal::LayoutController*>( handle.GetObjectPtr() ) );
    }

    if( !layoutController )
    {
      // If not, create the layout controller and register it as a singleton
      Internal::LayoutController* impl = new Internal::LayoutController();
      layoutController = LayoutController( impl );
      impl->Initialize();

      // Registering the singleton will automatically register the processor with Core.
      singletonService.Register( typeid(layoutController), layoutController );
    }
  }

  return layoutController;
}

void LayoutController::RequestLayout( LayoutItem layout )
{
  GetImpl( *this ).RequestLayout( GetImplementation( layout ), -1, Actor(), Actor() );
}

void LayoutController::RequestLayout( LayoutItem layout, Dali::Toolkit::LayoutTransitionData::Type layoutTransitionType )
{
  GetImpl( *this ).RequestLayout( GetImplementation( layout ), layoutTransitionType, Actor(), Actor() );
}

void LayoutController::RequestLayout( LayoutItem layout, Dali::Toolkit::LayoutTransitionData::Type layoutTransitionType, Actor gainedChild, Actor lostChild  )
{
  GetImpl( *this ).RequestLayout( GetImplementation( layout ), layoutTransitionType, gainedChild, lostChild );
}

LayoutController::LayoutController( Internal::LayoutController *impl )
: BaseHandle( impl )
{
}

} //namespace Toolkit
} //namespace Dali
