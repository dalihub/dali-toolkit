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
#include "wireframe-visual.h"

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/devel-visual-properties.h>
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
const char * const POSITION_ATTRIBUTE_NAME("aPosition");
const char * const INDEX_NAME("indices");

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
attribute mediump vec2  aPosition;\n
uniform   mediump mat4  uMvpMatrix;\n
uniform   mediump vec3  uSize;\n
\n

//Visual size and offset
uniform mediump vec2 offset;\n
uniform mediump vec2 size;\n
uniform mediump vec4 offsetSizeMode;\n
uniform mediump vec2 origin;\n
uniform mediump vec2 anchorPoint;\n

vec4 ComputeVertexPosition()\n
{\n
  vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );\n
  vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);\n
  return vec4( (aPosition + anchorPoint)*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );\n
}\n

void main()\n
{\n
  gl_Position = uMvpMatrix * ComputeVertexPosition();\n
}\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(\n
uniform lowp vec4 uColor;\n
\n
void main()\n
{\n
  gl_FragColor = uColor;\n
}\n
);

}

WireframeVisualPtr WireframeVisual::New( VisualFactoryCache& factoryCache )
{
  return new WireframeVisual( factoryCache );
}

WireframeVisual::WireframeVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache )
{
}

WireframeVisual::~WireframeVisual()
{
}

void WireframeVisual::DoSetProperties( const Property::Map& propertyMap )
{
  // no properties supported at the moment
}

void WireframeVisual::DoSetOnStage( Actor& actor )
{
  InitializeRenderer();

  actor.AddRenderer( mImpl->mRenderer );
}

void WireframeVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::VisualProperty::TYPE, Toolkit::Visual::WIREFRAME );
}

void WireframeVisual::InitializeRenderer()
{
  Shader shader = mFactoryCache.GetShader( VisualFactoryCache::WIREFRAME_SHADER );
  if( !shader )
  {
    shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
    mFactoryCache.SaveShader( VisualFactoryCache::WIREFRAME_SHADER, shader );
  }

  Geometry geometry = mFactoryCache.GetGeometry( VisualFactoryCache::WIREFRAME_GEOMETRY );
  if( !geometry )
  {
    geometry = CreateQuadWireframeGeometry();
    mFactoryCache.SaveGeometry( VisualFactoryCache::WIREFRAME_GEOMETRY, geometry );
  }

  //Create the renderer
  mImpl->mRenderer = Renderer::New( geometry, shader);

  //Register transform properties
  mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
}

Geometry WireframeVisual::CreateQuadWireframeGeometry()
{
  const float halfWidth = 0.5f;
  const float halfHeight = 0.5f;
  struct QuadVertex { Vector2 position;};
  QuadVertex quadVertexData[4] =
  {
      { Vector2(-halfWidth, -halfHeight) },
      { Vector2( halfWidth, -halfHeight) },
      { Vector2( halfWidth,  halfHeight) },
      { Vector2(-halfWidth,  halfHeight) }
  };

  Property::Map quadVertexFormat;
  quadVertexFormat[POSITION_ATTRIBUTE_NAME] = Property::VECTOR2;
  PropertyBuffer quadVertices = PropertyBuffer::New( quadVertexFormat );
  quadVertices.SetData( quadVertexData, 4 );

  // Create indices
  unsigned short indexData[10] = { 0, 1, 1, 2, 2, 3, 3, 0 };

  // Create the geometry object
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( quadVertices );
  geometry.SetIndexBuffer( indexData, sizeof(indexData)/sizeof(indexData[0]) );
  geometry.SetType( Geometry::LINES );

  return geometry;
}

void WireframeVisual::OnSetTransform()
{
  if( mImpl->mRenderer )
  {
    //Register transform properties
    mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
