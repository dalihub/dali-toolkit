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
#include "renderer-factory-cache.h"

// Internal HEADER
#include <dali-toolkit/internal/controls/renderers/color/color-renderer.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

RendererFactoryCache::RendererFactoryCache()
{
}

RendererFactoryCache::~RendererFactoryCache()
{
  for( int i=0; i<= SHADER_TYPE_MAX; i++)
  {
    if(mShader[i])
    {
      mShader[i].Reset();
    }
  }

  for( int i=0; i<= GEOMETRY_TYPE_MAX; i++)
  {
    if(mGeometry[i])
    {
      mGeometry[i].Reset();
    }
  }
}


Geometry RendererFactoryCache::GetGeometry( GeometryType type )
{
  return mGeometry[type];
}

void RendererFactoryCache::SaveGeometry( GeometryType type, Geometry geometry )
{
  mGeometry[type] = geometry;
}

Shader RendererFactoryCache::GetShader( ShaderType type )
{
  return mShader[type];
}

void RendererFactoryCache::SaveShader( ShaderType type, Shader shader )
{
  mShader[type] = shader;
}

Geometry RendererFactoryCache::CreateQuadGeometry()
{
  const float halfWidth = 0.5f;
  const float halfHeight = 0.5f;
  struct QuadVertex { Vector2 position;};
  QuadVertex quadVertexData[4] =
  {
      { Vector2(-halfWidth, -halfHeight) },
      { Vector2( halfWidth, -halfHeight) },
      { Vector2(-halfWidth, halfHeight)  },
      { Vector2( halfWidth, halfHeight)  }
  };

  Property::Map quadVertexFormat;
  quadVertexFormat["aPosition"] = Property::VECTOR2;
  PropertyBuffer quadVertices = PropertyBuffer::New( quadVertexFormat, 4 );
  quadVertices.SetData(quadVertexData);

  // Create the geometry object
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( quadVertices );
  geometry.SetGeometryType( Geometry::TRIANGLE_STRIP );

  return geometry;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

