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
const char * const COLOR_NAME("blend-color");
const char * const COLOR_UNIFORM_NAME("uBlendColor");


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
  uniform lowp vec4 uBlendColor;\n
  \n
  void main()\n
  {\n
    gl_FragColor = uBlendColor*uColor;\n
  }\n
);
}

ColorRenderer::ColorRenderer()
: ControlRenderer()
{
}

ColorRenderer::~ColorRenderer()
{
}

void ColorRenderer::Initialize( RendererFactoryCache& factoryCache, const Property::Map& propertyMap )
{
  mImpl->mGeometry = factoryCache.GetGeometry( RendererFactoryCache::QUAD_GEOMETRY );
  if( !(mImpl->mGeometry) )
  {
    mImpl->mGeometry =  RendererFactoryCache::CreateQuadGeometry();
    factoryCache.SaveGeometry( RendererFactoryCache::QUAD_GEOMETRY, mImpl->mGeometry );
  }

  mImpl->mShader = factoryCache.GetShader( RendererFactoryCache::COLOR_SHADER );
  if( !(mImpl->mShader) )
  {
    mImpl->mShader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
    factoryCache.SaveShader( RendererFactoryCache::COLOR_SHADER, mImpl->mShader );
  }

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

void ColorRenderer::SetOnStage( Actor& actor )
{
  ControlRenderer::SetOnStage(actor);
  (mImpl->mRenderer).RegisterProperty( COLOR_UNIFORM_NAME, mBlendColor );
  if( mBlendColor.a < 1.f )
  {
    (mImpl->mRenderer).GetMaterial().SetBlendMode( BlendingMode::ON );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
