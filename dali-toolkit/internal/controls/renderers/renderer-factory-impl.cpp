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
#include <dali-toolkit/internal/controls/renderers/border/border-renderer.h>
#include <dali-toolkit/internal/controls/renderers/color/color-renderer.h>
#include <dali-toolkit/internal/controls/renderers/gradient/gradient-renderer.h>
#include <dali-toolkit/internal/controls/renderers/npatch/npatch-renderer.h>
#include <dali-toolkit/internal/controls/renderers/image/image-renderer.h>
#include <dali-toolkit/internal/controls/renderers/renderer-factory-cache.h>

namespace
{
const char * const RENDERER_TYPE_NAME( "rendererType" );

const char * const COLOR_RENDERER("colorRenderer");
const char * const BORDER_RENDERER("borderRenderer");
const char * const GRADIENT_RENDERER("gradientRenderer");
const char * const IMAGE_RENDERER("imageRenderer");
const char * const N_PATCH_RENDERER("nPatchRenderer");

const char * const BROKEN_RENDERER_IMAGE_URL( DALI_IMAGE_DIR "broken.png");
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
    if( !mFactoryCache )
    {
      mFactoryCache = new RendererFactoryCache();
    }

    if( typeValue ==  COLOR_RENDERER )
    {
      rendererPtr = new ColorRenderer( *( mFactoryCache.Get() ) );
    }
    else if( typeValue ==  GRADIENT_RENDERER )
    {
      rendererPtr = new GradientRenderer( *( mFactoryCache.Get() ) );
    }
    else if( typeValue ==  IMAGE_RENDERER )
    {
      rendererPtr = new ImageRenderer( *( mFactoryCache.Get() ) );
    }
    else if( typeValue ==  N_PATCH_RENDERER )
    {
      rendererPtr = new NPatchRenderer( *( mFactoryCache.Get() ) );
    }
    else if( typeValue == BORDER_RENDERER )
    {
      rendererPtr = new BorderRenderer( *( mFactoryCache.Get() ) );
    }
  }

  if( rendererPtr )
  {
    Actor actor;
    rendererPtr->Initialize( actor, propertyMap );
  }
  else
  {
    DALI_LOG_ERROR( "Renderer type unknown" );
  }

  return Toolkit::ControlRenderer( rendererPtr );
}

Toolkit::ControlRenderer RendererFactory::GetControlRenderer( const Vector4& color )
{
  if( !mFactoryCache )
  {
    mFactoryCache = new RendererFactoryCache();
  }

  ColorRenderer* rendererPtr = new ColorRenderer( *( mFactoryCache.Get() ) );
  rendererPtr->SetColor( color );

  return Toolkit::ControlRenderer( rendererPtr );
}

void RendererFactory::ResetRenderer( Toolkit::ControlRenderer& renderer, Actor& actor, const Vector4& color )
{
  if( renderer )
  {
    ColorRenderer* rendererPtr = dynamic_cast< ColorRenderer* >( &GetImplementation( renderer ) );
    if( rendererPtr )
    {
      rendererPtr->SetColor( color );
      return;
    }

    renderer.RemoveAndReset( actor );
  }

  renderer = GetControlRenderer( color );
  if( actor && actor.OnStage() )
  {
    renderer.SetOnStage( actor );
  }
}

Toolkit::ControlRenderer RendererFactory::GetControlRenderer( float borderSize, const Vector4& borderColor )
{
  if( !mFactoryCache )
  {
    mFactoryCache = new RendererFactoryCache();
  }
  BorderRenderer* rendererPtr = new BorderRenderer( *mFactoryCache.Get() );

  if( !mFactoryCache )
  {
    mFactoryCache = new RendererFactoryCache();
  }

  rendererPtr->SetBorderSize( borderSize );
  rendererPtr->SetBorderColor( borderColor );

  return Toolkit::ControlRenderer( rendererPtr );
}

Toolkit::ControlRenderer RendererFactory::GetControlRenderer( const Image& image )
{
  if( !mFactoryCache )
  {
    mFactoryCache = new RendererFactoryCache();
  }

  NinePatchImage npatchImage = NinePatchImage::DownCast( image );
  if( npatchImage )
  {
    NPatchRenderer* rendererPtr = new NPatchRenderer( *( mFactoryCache.Get() ) );
    rendererPtr->SetImage( npatchImage );

    return Toolkit::ControlRenderer( rendererPtr );
  }
  else
  {
    ImageRenderer* rendererPtr = new ImageRenderer( *( mFactoryCache.Get() ) );
    Actor actor;
    rendererPtr->SetImage( actor, image );

    return Toolkit::ControlRenderer( rendererPtr );
  }
}

void RendererFactory::ResetRenderer( Toolkit::ControlRenderer& renderer, Actor& actor, const Image& image )
{
  if( renderer )
  {
    NinePatchImage npatchImage = NinePatchImage::DownCast( image );
    if( npatchImage )
    {
      NPatchRenderer* rendererPtr = dynamic_cast< NPatchRenderer* >( &GetImplementation( renderer ) );
      if( rendererPtr )
      {
        rendererPtr->SetImage( npatchImage );
        return;
      }
    }
    else
    {
      ImageRenderer* rendererPtr = dynamic_cast< ImageRenderer* >( &GetImplementation( renderer ) );
      if( rendererPtr )
      {
        rendererPtr->SetImage( actor, image );
        return;
      }
    }

    renderer.RemoveAndReset( actor );
  }

  renderer = GetControlRenderer( image );
  if( actor && actor.OnStage() )
  {
    renderer.SetOnStage( actor );
  }
}

Toolkit::ControlRenderer RendererFactory::GetControlRenderer( const std::string& url )
{
  if( !mFactoryCache )
  {
    mFactoryCache = new RendererFactoryCache();
  }

  if( NinePatchImage::IsNinePatchUrl( url ) )
  {
    NPatchRenderer* rendererPtr = new NPatchRenderer( *( mFactoryCache.Get() ) );
    rendererPtr->SetImage( url );

    return Toolkit::ControlRenderer( rendererPtr );
  }
  else
  {
    ImageRenderer* rendererPtr = new ImageRenderer( *( mFactoryCache.Get() ) );
    Actor actor;
    rendererPtr->SetImage( actor, url );

    return Toolkit::ControlRenderer( rendererPtr );
  }
}

void RendererFactory::ResetRenderer( Toolkit::ControlRenderer& renderer, Actor& actor, const std::string& url )
{
  if( renderer )
  {
    if( NinePatchImage::IsNinePatchUrl( url ) )
    {
      NPatchRenderer* rendererPtr = dynamic_cast< NPatchRenderer* >( &GetImplementation( renderer ) );
      if( rendererPtr )
      {
        rendererPtr->SetImage( url );
        return;
      }
    }
    else
    {
      ImageRenderer* rendererPtr = dynamic_cast< ImageRenderer* >( &GetImplementation( renderer ) );
      if( rendererPtr )
      {
        rendererPtr->SetImage( actor, url );
        return;
      }
    }

    renderer.RemoveAndReset( actor );
  }

  renderer = GetControlRenderer( url );
  if( actor && actor.OnStage() )
  {
    renderer.SetOnStage( actor );
  }
}

void RendererFactory::ResetRenderer( Toolkit::ControlRenderer& renderer, Actor& actor, const Property::Map& propertyMap )
{
  if( renderer )
  {
    ControlRenderer& controlRenderer = GetImplementation( renderer );

    Property::Value* type = propertyMap.Find( RENDERER_TYPE_NAME );
    std::string typeValue ;

    //If there's no renderer type specified or if there hasn't been a renderer type change then we can reuse the renderer
    if( !type || !type->Get( typeValue ) ||
        ( typeValue ==  IMAGE_RENDERER    && typeid( controlRenderer ) == typeid( ImageRenderer ) ) ||
        ( typeValue ==  N_PATCH_RENDERER  && typeid( controlRenderer ) == typeid( NPatchRenderer ) ) ||
        ( typeValue ==  COLOR_RENDERER    && typeid( controlRenderer ) == typeid( ColorRenderer ) )||
        ( typeValue ==  GRADIENT_RENDERER && typeid( controlRenderer ) == typeid( GradientRenderer ) ) ||
        ( typeValue ==  BORDER_RENDERER   && typeid( controlRenderer ) == typeid( BorderRenderer ) ) )
    {
      controlRenderer.Initialize( actor, propertyMap );
      return;
    }

    renderer.RemoveAndReset( actor );
  }

  renderer = GetControlRenderer( propertyMap );
  if( actor && actor.OnStage() )
  {
    renderer.SetOnStage( actor );
  }
}

Image RendererFactory::GetBrokenRendererImage()
{
  return ResourceImage::New( BROKEN_RENDERER_IMAGE_URL );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

