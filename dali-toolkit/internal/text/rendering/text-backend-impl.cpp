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
#include <dali-toolkit/internal/text/rendering/text-backend-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/devel-api/adaptor-framework/singleton-service.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/rendering-backend.h>
#include <dali-toolkit/internal/text/rendering/atlas/text-atlas-renderer.h>
#ifdef ENABLE_VECTOR_BASED_TEXT_RENDERING
#include <dali-toolkit/internal/text/rendering/vector-based/vector-based-renderer.h>
#endif

namespace Dali
{

namespace Toolkit
{

namespace Text
{

namespace Internal
{

struct Backend::Impl
{
  int temp; // placeholder for future backend implemenations
};

Backend::Backend()
: mImpl( NULL )
{
  mImpl = new Impl();
}

Backend::~Backend()
{
  delete mImpl;
}

Dali::Toolkit::Text::Backend Backend::Get()
{
  Dali::Toolkit::Text::Backend backendHandle;

  Dali::SingletonService service( SingletonService::Get() );
  if ( service )
  {
    // Check whether the singleton is already created
    Dali::BaseHandle handle = service.GetSingleton( typeid( Dali::Toolkit::Text::Backend ) );
    if(handle)
    {
      // If so, downcast the handle
      Backend* impl = dynamic_cast< Dali::Toolkit::Text::Internal::Backend* >( handle.GetObjectPtr() );
      backendHandle = Dali::Toolkit::Text::Backend( impl );
    }
    else // create and register the object
    {
      backendHandle = Dali::Toolkit::Text::Backend( new Backend );
      service.Register( typeid( backendHandle ), backendHandle );
    }
  }

  return backendHandle;
}

RendererPtr Backend::NewRenderer( unsigned int renderingType )
{
  RendererPtr renderer;

  switch( renderingType )
  {
    case Dali::Toolkit::Text::RENDERING_SHARED_ATLAS:
    {
      renderer = Dali::Toolkit::Text::AtlasRenderer::New();
    }
    break;

    case Dali::Toolkit::Text::RENDERING_VECTOR_BASED:
    {
#ifdef ENABLE_VECTOR_BASED_TEXT_RENDERING
      renderer = Dali::Toolkit::Text::VectorBasedRenderer::New();
#else
      renderer = Dali::Toolkit::Text::AtlasRenderer::New(); // Fallback to bitmap-based rendering
#endif
    }
    break;

    default:
    {
      DALI_LOG_WARNING( "Unknown renderer type: %d\n", renderingType );
      break;
    }
  }

  return renderer;
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali
