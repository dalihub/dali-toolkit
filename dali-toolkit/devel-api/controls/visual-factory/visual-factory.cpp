 /*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include "visual-factory.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/singleton-service.h>
#include <dali/devel-api/adaptor-framework/environment-variable.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/renderers/visual-factory-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace
{
  const char * const DALI_DEBUG_RENDERING("DALI_DEBUG_RENDERING");
}

VisualFactory VisualFactory::Get()
{
  VisualFactory factory;

  // Check whether the VisualFactory is already created
  SingletonService singletonService( SingletonService::Get() );
  if( singletonService )
  {
    BaseHandle handle = singletonService.GetSingleton( typeid(VisualFactory) );
    if( handle )
    {
      //If so, downcast the handle of singleton to VisualFactory
      factory = VisualFactory( dynamic_cast<Internal::VisualFactory*>(handle.GetObjectPtr()) );
    }

    if( !factory )// If not, create the VisualFactory and register it as a singleton
    {
      // Check whether debug rendering is required
      if( EnvironmentVariable::GetEnvironmentVariable( DALI_DEBUG_RENDERING ) )
      {
        factory = VisualFactory( new Internal::VisualFactory(true) );
      }
      else
      {
        factory = VisualFactory( new Internal::VisualFactory(false) );
      }
      singletonService.Register( typeid(VisualFactory), factory );

    }
  }

  return factory;
}

VisualFactory::VisualFactory()
{
}

VisualFactory::~VisualFactory()
{
}

VisualFactory::VisualFactory( const VisualFactory& handle )
: BaseHandle( handle )
{
}

VisualFactory& VisualFactory::operator=( const VisualFactory& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

VisualFactory::VisualFactory(Internal::VisualFactory *impl)
: BaseHandle(impl)
{
}

Visual VisualFactory::CreateVisual( const Property::Map& propertyMap )
{
  return GetImplementation( *this ).CreateVisual( propertyMap );
}

Visual VisualFactory::CreateVisual( const Image& image )
{
  return GetImplementation( *this ).CreateVisual( image );
}

Visual VisualFactory::CreateVisual( const std::string& url, ImageDimensions size )
{
  return GetImplementation( *this ).CreateVisual( url, size );
}

} // namespace Toolkit

} // namespace Dali
