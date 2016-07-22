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
#include "debug-visual.h"

//INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/renderers/visual-factory-impl.h>
#include <dali-toolkit/internal/controls/renderers/visual-factory-cache.h>
#include <dali-toolkit/internal/controls/renderers/visual-data-impl.h>
#include <dali-toolkit/internal/controls/renderers/visual-string-constants.h>

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
void main()\n
{\n
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
  vertexPosition.xyz *= uSize;\n
  gl_Position = uMvpMatrix * vertexPosition;\n
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


DebugVisual::DebugVisual( VisualFactoryCache& factoryCache )
: Visual( factoryCache )
{
}

DebugVisual::~DebugVisual()
{}

void DebugVisual::DoSetOnStage( Actor& actor )
{
  InitializeRenderer();
}

void DebugVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( RENDERER_TYPE, DEBUG_RENDERER );
}

void DebugVisual::InitializeRenderer()
{
  mImpl->mRenderer = mFactoryCache.GetDebugVisual();
  if( !mImpl->mRenderer )
  {
    Geometry geometry = CreateQuadWireframeGeometry();
    Shader shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );

    mImpl->mRenderer = Renderer::New( geometry, shader);
    mFactoryCache.CacheDebugVisual( mImpl->mRenderer );
  }
}

Geometry DebugVisual::CreateQuadWireframeGeometry()
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
  geometry.SetGeometryType( Geometry::LINES );

  return geometry;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
