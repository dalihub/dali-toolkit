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
 */

// CLASS HEADER
#include "renderer-factory-impl.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/type-registry-helper.h>

// Internal HEADER
#include <dali-toolkit/internal/controls/renderers/color/color-renderer.h>
#include <dali-toolkit/internal/controls/renderers/gradient/gradient-renderer.h>
#include <dali-toolkit/internal/controls/renderers/image/image-renderer.h>
#include <dali-toolkit/internal/controls/renderers/renderer-factory-cache.h>

namespace
{
const char * const RENDERER_TYPE_NAME( "renderer-type" );
const char * const COLOR_RENDERER("color-renderer");
const char * const GRADIENT_RENDERER("gradient-renderer");
const char * const IMAGE_RENDERER("image-renderer");
}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

BaseHandle Create()
{
  BaseHandle handle = Toolkit::RendererFactory::Get();

  return handle;
}

DALI_TYPE_REGISTRATION_BEGIN_CREATE( Toolkit::RendererFactory, Dali::BaseHandle, Create, true )
DALI_TYPE_REGISTRATION_END()

} // namespace

RendererFactory::RendererFactory()
{
}

RendererFactory::~RendererFactory()
{
}

Toolkit::ControlRenderer RendererFactory::GetControlRenderer( const Property::Map& propertyMap )
{
  ControlRenderer* rendererPtr = NULL;

  Property::Value* type = propertyMap.Find( RENDERER_TYPE_NAME );
  std::string typeValue ;
  if( type && type->Get( typeValue ))
  {
    if( typeValue ==  COLOR_RENDERER )
    {
      rendererPtr = new ColorRenderer();
    }
    else if( typeValue ==  GRADIENT_RENDERER )
    {
      rendererPtr = new GradientRenderer();
    }
    else if( typeValue ==  IMAGE_RENDERER )
    {
      rendererPtr = new ImageRenderer();
    }
  }

  if( rendererPtr )
  {
    if( !mFactoryCache )
    {
      mFactoryCache = new RendererFactoryCache();
    }
    rendererPtr->Initialize( *( mFactoryCache.Get() ), propertyMap );
  }
  else
  {
    DALI_LOG_ERROR( "Renderer type unknown" );
  }

  return Toolkit::ControlRenderer( rendererPtr );
}

Toolkit::ControlRenderer RendererFactory::GetControlRenderer( const Vector4& color )
{
  ColorRenderer* rendererPtr = new ColorRenderer();

  if( !mFactoryCache )
  {
    mFactoryCache = new RendererFactoryCache();
  }
  rendererPtr->Initialize( *( mFactoryCache.Get() ) );

  rendererPtr->SetColor( color );

  return Toolkit::ControlRenderer( rendererPtr );
}

bool RendererFactory::ResetRenderer( Toolkit::ControlRenderer& renderer, const Vector4& color )
{
  ColorRenderer* rendererPtr = dynamic_cast< ColorRenderer* >( &GetImplementation( renderer ) );
  if( rendererPtr )
  {
    rendererPtr->SetColor( color );
    return false;
  }
  else
  {
    renderer = GetControlRenderer( color );
    return true;
  }
}

Toolkit::ControlRenderer RendererFactory::GetControlRenderer( const Image& image )
{
  ImageRenderer* rendererPtr = new ImageRenderer();
  if( !mFactoryCache )
  {
    mFactoryCache = new RendererFactoryCache();
  }
  rendererPtr->Initialize( *( mFactoryCache.Get() ) );
  rendererPtr->SetImage( image );

  return Toolkit::ControlRenderer( rendererPtr );
}

bool RendererFactory::ResetRenderer( Toolkit::ControlRenderer& renderer, const Image& image )
{
  ImageRenderer* rendererPtr = dynamic_cast< ImageRenderer* >( &GetImplementation( renderer ) );
  if( rendererPtr )
  {
    rendererPtr->SetImage( image );
    return false;
  }
  else
  {
    renderer = GetControlRenderer( image );
    return true;
  }
}

Toolkit::ControlRenderer RendererFactory::GetControlRenderer( const std::string& url )
{
  ImageRenderer* rendererPtr = new ImageRenderer();
  if( !mFactoryCache )
  {
    mFactoryCache = new RendererFactoryCache();
  }
  rendererPtr->Initialize( *( mFactoryCache.Get() ) );
  rendererPtr->SetImage( url );

  return Toolkit::ControlRenderer( rendererPtr );
}

bool RendererFactory::ResetRenderer( Toolkit::ControlRenderer& renderer, const std::string& url )
{
  ImageRenderer* rendererPtr = dynamic_cast< ImageRenderer* >( &GetImplementation( renderer ) );
  if( rendererPtr )
  {
    rendererPtr->SetImage( url );
    return false;
  }
  else
  {
    renderer = GetControlRenderer( url );
    return true;
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

