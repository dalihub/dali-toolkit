/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include "color-visual.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

//INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
const char * const COLOR_NAME("mixColor");

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
  uniform lowp vec4 mixColor;\n
  \n
  void main()\n
  {\n
    gl_FragColor = mixColor*uColor;\n
  }\n
);
}

ColorVisual::ColorVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache ),
  mMixColorIndex( Property::INVALID_INDEX )
{
}

ColorVisual::~ColorVisual()
{
}

void ColorVisual::DoInitialize( Actor& actor, const Property::Map& propertyMap )
{
  Property::Value* color = propertyMap.Find( Toolkit::ColorVisual::Property::MIX_COLOR, COLOR_NAME );
  if( !( color && color->Get(mMixColor) ) )
  {
    DALI_LOG_ERROR( "Fail to provide a color to the ColorVisual object\n" );
  }
}

void ColorVisual::SetSize( const Vector2& size )
{
  Visual::Base::SetSize( size );

  // ToDo: renderer responds to the size change
}

void ColorVisual::DoSetOnStage( Actor& actor )
{
  InitializeRenderer();
}

void ColorVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR );
  map.Insert( Toolkit::ColorVisual::Property::MIX_COLOR, mMixColor );
}

void ColorVisual::InitializeRenderer()
{
  Geometry geometry = mFactoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );
  if( !geometry )
  {
    geometry =  VisualFactoryCache::CreateQuadGeometry();
    mFactoryCache.SaveGeometry( VisualFactoryCache::QUAD_GEOMETRY, geometry );
  }

  Shader shader = mFactoryCache.GetShader( VisualFactoryCache::COLOR_SHADER );
  if( !shader )
  {
    shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
    mFactoryCache.SaveShader( VisualFactoryCache::COLOR_SHADER, shader );
  }

  mImpl->mRenderer = Renderer::New( geometry, shader );

  mMixColorIndex = mImpl->mRenderer.RegisterProperty( Toolkit::ColorVisual::Property::MIX_COLOR, COLOR_NAME, mMixColor );
  if( mMixColor.a < 1.f )
  {
    mImpl->mRenderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );
  }
}

void ColorVisual::SetColor(const Vector4& color)
{
  mMixColor = color;

  if( mImpl->mRenderer )
  {
    (mImpl->mRenderer).SetProperty( mMixColorIndex, color );
    if( color.a < 1.f )
    {
      mImpl->mRenderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
