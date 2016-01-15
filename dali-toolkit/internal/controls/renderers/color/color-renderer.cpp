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
#include "color-renderer.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

//INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/renderers/renderer-factory-impl.h>
#include <dali-toolkit/internal/controls/renderers/renderer-factory-cache.h>
#include <dali-toolkit/internal/controls/renderers/control-renderer-data-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
const char * const RENDERER_TYPE("rendererType");
const char * const RENDERER_TYPE_VALUE("colorRenderer");
const char * const COLOR_NAME("blendColor");

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  \n
  void main()\n
  {\n
    mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
    vertexPosition.xyz *= uSize;\n
    gl_Position = uMvpMatrix * vertexPosition;\n
  }\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  uniform lowp vec4 uColor;\n
  uniform lowp vec4 blendColor;\n
  \n
  void main()\n
  {\n
    gl_FragColor = blendColor*uColor;\n
  }\n
);
}

ColorRenderer::ColorRenderer( RendererFactoryCache& factoryCache )
: ControlRenderer( factoryCache ),
  mBlendColorIndex( Property::INVALID_INDEX )
{
}

ColorRenderer::~ColorRenderer()
{
}

void ColorRenderer::DoInitialize( Actor& actor, const Property::Map& propertyMap )
{
  Property::Value* color = propertyMap.Find( COLOR_NAME );
  if( !( color && color->Get(mBlendColor) ) )
  {
    DALI_LOG_ERROR( "Fail to provide a color to the ColorRenderer object" );
  }
}

void ColorRenderer::SetSize( const Vector2& size )
{
  ControlRenderer::SetSize( size );

  // ToDo: renderer responds to the size change
}

void ColorRenderer::SetClipRect( const Rect<int>& clipRect )
{
  ControlRenderer::SetClipRect( clipRect );

  //ToDo: renderer responds to the clipRect change
}

void ColorRenderer::SetOffset( const Vector2& offset )
{
  //ToDo: renderer applies the offset
}

void ColorRenderer::DoSetOnStage( Actor& actor )
{
  InitializeRenderer();
}

void ColorRenderer::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( RENDERER_TYPE, RENDERER_TYPE_VALUE );
  map.Insert( COLOR_NAME, mBlendColor );
}

void ColorRenderer::InitializeRenderer()
{
  Geometry geometry = mFactoryCache.GetGeometry( RendererFactoryCache::QUAD_GEOMETRY );
  if( !geometry )
  {
    geometry =  RendererFactoryCache::CreateQuadGeometry();
    mFactoryCache.SaveGeometry( RendererFactoryCache::QUAD_GEOMETRY, geometry );
  }

  Shader shader = mFactoryCache.GetShader( RendererFactoryCache::COLOR_SHADER );
  if( !shader )
  {
    shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
    mFactoryCache.SaveShader( RendererFactoryCache::COLOR_SHADER, shader );
  }

  Material material = Material::New( shader );
  mImpl->mRenderer = Renderer::New( geometry, material );

  mBlendColorIndex = mImpl->mRenderer.RegisterProperty( COLOR_NAME, mBlendColor );
  if( mBlendColor.a < 1.f )
  {
    mImpl->mRenderer.GetMaterial().SetBlendMode( BlendingMode::ON );
  }
}

void ColorRenderer::SetColor(const Vector4& color)
{
  mBlendColor = color;

  if( mImpl->mRenderer )
  {
    (mImpl->mRenderer).SetProperty( mBlendColorIndex, color );
    if( color.a < 1.f &&  (mImpl->mRenderer).GetMaterial().GetBlendMode() != BlendingMode::ON)
    {
      (mImpl->mRenderer).GetMaterial().SetBlendMode( BlendingMode::ON );
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
