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
#include "renderer-factory.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/singleton-service.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/renderers/renderer-factory-impl.h>


namespace Dali
{

namespace Toolkit
{

RendererFactory RendererFactory::Get()
{
  RendererFactory factory;

  // Check whether the RendererFactory is already created
  SingletonService singletonService( SingletonService::Get() );
  if( singletonService )
  {
    BaseHandle handle = singletonService.GetSingleton( typeid(RendererFactory) );
    if( handle )
    {
      //If so, downcast the handle of singleton to RendererFactory
      factory = RendererFactory( dynamic_cast<Internal::RendererFactory*>(handle.GetObjectPtr()) );
    }

    if( !factory )
    {
      // If not, create the RendererFactory and register it as a singleton
      factory = RendererFactory( new Internal::RendererFactory() );
      singletonService.Register( typeid(RendererFactory), factory );

    }
  }

  return factory;
}

RendererFactory::RendererFactory()
{
}

RendererFactory::~RendererFactory()
{
}

RendererFactory::RendererFactory( const RendererFactory& handle )
: BaseHandle( handle )
{
}

RendererFactory& RendererFactory::operator=( const RendererFactory& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

RendererFactory::RendererFactory(Internal::RendererFactory *impl)
: BaseHandle(impl)
{
}

ControlRenderer RendererFactory::GetControlRenderer( const Property::Map& propertyMap )
{
  return GetImplementation( *this ).GetControlRenderer( propertyMap );
}

ControlRenderer RendererFactory::GetControlRenderer( const Vector4& color )
{
  return GetImplementation( *this ).GetControlRenderer( color );
}

void RendererFactory::ResetRenderer( ControlRenderer& renderer, Actor& actor, const Vector4& color )
{
  GetImplementation( *this ).ResetRenderer( renderer, actor, color );
}

ControlRenderer RendererFactory::GetControlRenderer( float borderSize, const Vector4& borderColor )
{
  return GetImplementation( *this ).GetControlRenderer( borderSize, borderColor );
}

ControlRenderer RendererFactory::GetControlRenderer( const Image& image )
{
  return GetImplementation( *this ).GetControlRenderer( image );
}

void RendererFactory::ResetRenderer( ControlRenderer& renderer, Actor& actor, const Image& image )
{
  GetImplementation( *this ).ResetRenderer( renderer, actor, image );
}

ControlRenderer RendererFactory::GetControlRenderer( const std::string& url, ImageDimensions size )
{
  return GetImplementation( *this ).GetControlRenderer( url, size );
}

void RendererFactory::ResetRenderer( ControlRenderer& renderer, Actor& actor, const std::string& url, ImageDimensions size )
{
  GetImplementation( *this ).ResetRenderer( renderer, actor, url, size );
}

void RendererFactory::ResetRenderer( ControlRenderer& renderer, Actor& actor, const Property::Map& propertyMap )
{
  GetImplementation( *this ).ResetRenderer( renderer, actor, propertyMap );
}

} // namespace Toolkit

} // namespace Dali
