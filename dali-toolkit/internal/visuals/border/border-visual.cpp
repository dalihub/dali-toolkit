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
#include "border-visual.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/border-visual-properties.h>
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
const char * const COLOR_NAME("borderColor");
const char * const SIZE_NAME("borderSize");
const char * const ANTI_ALIASING("antiAliasing");

const char * const POSITION_ATTRIBUTE_NAME("aPosition");
const char * const DRIFT_ATTRIBUTE_NAME("aDrift");
const char * const INDEX_NAME("indices");


const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  attribute mediump vec2 aDrift;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  uniform mediump float borderSize;\n
  \n
  void main()\n
  {\n
    vec2 position = aPosition*uSize.xy + aDrift*borderSize;\n
    gl_Position = uMvpMatrix * vec4(position, 0.0, 1.0);\n
  }\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  uniform lowp vec4 uColor;\n
  uniform lowp vec4 borderColor;\n
  \n
  void main()\n
  {\n
    gl_FragColor = borderColor*uColor;\n
  }\n
);

const char* VERTEX_SHADER_ANTI_ALIASING = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  attribute mediump vec2 aDrift;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  uniform mediump float borderSize;\n
  varying mediump float vAlpha;\n
  \n
  void main()\n
  {\n
    vec2 position = aPosition*(uSize.xy+vec2(0.75)) + aDrift*(borderSize+1.5);\n
    gl_Position = uMvpMatrix * vec4(position, 0.0, 1.0);\n
    vAlpha = min( abs(aDrift.x), abs(aDrift.y) )*(borderSize+1.5);
  }\n
);

const char* FRAGMENT_SHADER_ANTI_ALIASING = DALI_COMPOSE_SHADER(
  uniform lowp vec4 uColor;\n
  uniform lowp vec4 borderColor;\n
  uniform mediump float borderSize;\n
  varying mediump float vAlpha;\n
  \n
  void main()\n
  {\n
    gl_FragColor = borderColor*uColor;\n
    gl_FragColor.a *= smoothstep(0.0, 1.5, vAlpha)*smoothstep( borderSize+1.5, borderSize, vAlpha );\n
  }\n
);
}

BorderVisual::BorderVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache ),
  mBorderColor( Color::TRANSPARENT ),
  mBorderSize( 0.f ),
  mBorderColorIndex( Property::INVALID_INDEX ),
  mBorderSizeIndex( Property::INVALID_INDEX ),
  mAntiAliasing( false )
{
}

BorderVisual::~BorderVisual()
{
}

void BorderVisual::DoInitialize( Actor& actor, const Property::Map& propertyMap )
{
  Property::Value* color = propertyMap.Find( Toolkit::BorderVisual::Property::COLOR, COLOR_NAME );
  if( !( color && color->Get(mBorderColor) ) )
  {
    DALI_LOG_ERROR( "Fail to provide a border color to the BorderVisual object\n" );
  }

  Property::Value* size = propertyMap.Find( Toolkit::BorderVisual::Property::SIZE, SIZE_NAME );
  if( !( size && size->Get(mBorderSize) ) )
  {
    DALI_LOG_ERROR( "Fail to provide a border size to the BorderVisual object\n" );
  }

  Property::Value* antiAliasing = propertyMap.Find( Toolkit::BorderVisual::Property::ANTI_ALIASING, ANTI_ALIASING );
  if( antiAliasing )
  {
    antiAliasing->Get( mAntiAliasing );
  }
}

void BorderVisual::DoSetOnStage( Actor& actor )
{
  InitializeRenderer();

  mBorderColorIndex = (mImpl->mRenderer).RegisterProperty( Toolkit::BorderVisual::Property::COLOR, COLOR_NAME, mBorderColor );
  if( mBorderColor.a < 1.f || mAntiAliasing)
  {
    mImpl->mRenderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );
  }
  mBorderSizeIndex = (mImpl->mRenderer).RegisterProperty( Toolkit::BorderVisual::Property::SIZE, SIZE_NAME, mBorderSize );

  actor.AddRenderer( mImpl->mRenderer );
}

void BorderVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::Visual::BORDER );
  map.Insert( Toolkit::BorderVisual::Property::COLOR, mBorderColor );
  map.Insert( Toolkit::BorderVisual::Property::SIZE, mBorderSize );
  map.Insert( Toolkit::BorderVisual::Property::ANTI_ALIASING, mAntiAliasing );
}

void BorderVisual::DoSetProperty( Dali::Property::Index index, const Dali::Property::Value& propertyValue )
{
  // TODO
}

Dali::Property::Value BorderVisual::DoGetProperty( Dali::Property::Index index )
{
  // TODO
  return Dali::Property::Value();
}

void BorderVisual::InitializeRenderer()
{
  Geometry geometry = mFactoryCache.GetGeometry( VisualFactoryCache::BORDER_GEOMETRY );
  if( !geometry )
  {
    geometry =  CreateBorderGeometry();
    mFactoryCache.SaveGeometry( VisualFactoryCache::BORDER_GEOMETRY, geometry );
  }


  Shader shader = GetBorderShader();
  mImpl->mRenderer = Renderer::New( geometry, shader  );

}

void BorderVisual::SetBorderColor(const Vector4& color)
{
  mBorderColor = color;

  if( mImpl->mRenderer )
  {
    (mImpl->mRenderer).SetProperty( mBorderColorIndex, color );
    if( color.a < 1.f )
    {
      mImpl->mRenderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );
    }
  }
}

void BorderVisual::SetBorderSize( float size )
{
  mBorderSize = size;

  if( mImpl->mRenderer )
  {
    (mImpl->mRenderer).SetProperty( mBorderSizeIndex, size );
  }
}

void BorderVisual::RequireAntiAliasing( bool antiAliasing )
{
  if( mAntiAliasing != antiAliasing )
  {
    mAntiAliasing = antiAliasing;
    if( mImpl->mRenderer )
    {
      Shader borderShader( GetBorderShader() );
      mImpl->mRenderer.SetShader( borderShader );
      if( mAntiAliasing )
      {
        mImpl->mRenderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );
      }
    }
  }
}

Shader BorderVisual::GetBorderShader()
{
  Shader shader;
  if( mAntiAliasing )
  {
    shader = mFactoryCache.GetShader( VisualFactoryCache::BORDER_SHADER_ANTI_ALIASING );
    if( !shader )
    {
      shader = Shader::New( VERTEX_SHADER_ANTI_ALIASING, FRAGMENT_SHADER_ANTI_ALIASING );
      mFactoryCache.SaveShader( VisualFactoryCache::BORDER_SHADER_ANTI_ALIASING, shader );
    }
  }
  else
  {
    shader = mFactoryCache.GetShader( VisualFactoryCache::BORDER_SHADER );
    if( !shader )
    {
      shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
      mFactoryCache.SaveShader( VisualFactoryCache::BORDER_SHADER, shader );
    }
  }

  return shader;
}

/**
 * Vertices and triangles of the border geometry:
 *
 * vertex position = aPosition*uSize.xy + aDrift*uBorderSize;
 *
 * 0--1--2--3
 * |\ | /| /|
 * | \|/ |/ |
 * 4--5--6--7
 * |\ |  |\ |
 * | \|  | \|
 * 8--9--10-11
 * | /| /|\ |
 * |/ |/ | \|
 * 12-13-14-15
 */
Geometry BorderVisual::CreateBorderGeometry()
{
  const float halfWidth = 0.5f;
  const float halfHeight = 0.5f;
  struct BorderVertex { Vector2 position; Vector2 drift;};
  BorderVertex borderVertexData[16] =
  {
      { Vector2(-halfWidth, -halfHeight), Vector2(0.f, 0.f) },
      { Vector2(-halfWidth, -halfHeight), Vector2(1.f, 0.f) },
      { Vector2(halfWidth, -halfHeight),  Vector2(-1.f, 0.f) },
      { Vector2(halfWidth, -halfHeight),  Vector2(0.f, 0.f) },

      { Vector2(-halfWidth, -halfHeight), Vector2(0.f, 1.f) },
      { Vector2(-halfWidth, -halfHeight), Vector2(1.f, 1.f) },
      { Vector2(halfWidth, -halfHeight),  Vector2(-1.f, 1.f) },
      { Vector2(halfWidth, -halfHeight),  Vector2(0.f, 1.f) },

      { Vector2(-halfWidth, halfHeight), Vector2(0.f, -1.f) },
      { Vector2(-halfWidth, halfHeight), Vector2(1.f, -1.f) },
      { Vector2(halfWidth, halfHeight),  Vector2(-1.f, -1.f) },
      { Vector2(halfWidth, halfHeight),  Vector2(0.f, -1.f) },

      { Vector2(-halfWidth, halfHeight), Vector2(0.f, 0.f) },
      { Vector2(-halfWidth, halfHeight), Vector2(1.f, 0.f) },
      { Vector2(halfWidth, halfHeight),  Vector2(-1.f, 0.f) },
      { Vector2(halfWidth, halfHeight),  Vector2(0.f, 0.f) },
  };

  Property::Map borderVertexFormat;
  borderVertexFormat[POSITION_ATTRIBUTE_NAME] = Property::VECTOR2;
  borderVertexFormat[DRIFT_ATTRIBUTE_NAME] = Property::VECTOR2;
  PropertyBuffer borderVertices = PropertyBuffer::New( borderVertexFormat );
  borderVertices.SetData( borderVertexData, 16 );

  // Create indices
  unsigned short indexData[24] = { 1,5,2,6,3,7,7,6,11,10,15,14,14,10,13,9,12,8,8,9,4,5,0,1};

  // Create the geometry object
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( borderVertices );
  geometry.SetIndexBuffer( indexData, sizeof(indexData)/sizeof(indexData[0]) );
  geometry.SetType( Geometry::TRIANGLE_STRIP );

  return geometry;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
