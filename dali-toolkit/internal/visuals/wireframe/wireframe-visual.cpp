/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
const char* const POSITION_ATTRIBUTE_NAME("aPosition");
const char* const INDEX_NAME("indices");
} // namespace

WireframeVisualPtr WireframeVisual::New(VisualFactoryCache& factoryCache, const Property::Map& properties)
{
  Visual::BasePtr emtptyVisual;

  return New(factoryCache, emtptyVisual, properties);
}

WireframeVisualPtr WireframeVisual::New(VisualFactoryCache& factoryCache, Visual::BasePtr actualVisual)
{
  WireframeVisualPtr wireframeVisual(new WireframeVisual(factoryCache, actualVisual));
  wireframeVisual->Initialize();
  return wireframeVisual;
}

WireframeVisualPtr WireframeVisual::New(VisualFactoryCache& factoryCache, Visual::BasePtr actualVisual, const Property::Map& properties)
{
  WireframeVisualPtr wireframeVisual(new WireframeVisual(factoryCache, actualVisual));

  // Instead of calling SetProperties, looking for the only valid property 'transform'
  Property::Value* transformValue = properties.Find(Toolkit::Visual::Property::TRANSFORM, TRANSFORM);
  Property::Map    transformMap;
  if(transformValue && transformValue->Get(transformMap))
  {
    wireframeVisual->SetTransformAndSize(transformMap, Vector2::ZERO);
  }
  wireframeVisual->Initialize();
  return wireframeVisual;
}

WireframeVisual::WireframeVisual(VisualFactoryCache& factoryCache, Visual::BasePtr actualVisual)
: Visual::Base(factoryCache, Visual::FittingMode::FILL, actualVisual ? actualVisual->GetType() : Toolkit::Visual::WIREFRAME),
  mActualVisual(actualVisual)
{
}

WireframeVisual::~WireframeVisual()
{
}

float WireframeVisual::GetHeightForWidth(float width)
{
  if(mActualVisual)
  {
    return mActualVisual->GetHeightForWidth(width);
  }
  else
  {
    return Visual::Base::GetHeightForWidth(width);
  }
}

void WireframeVisual::GetNaturalSize(Vector2& naturalSize)
{
  if(mActualVisual)
  {
    mActualVisual->GetNaturalSize(naturalSize);
  }
  else
  {
    Visual::Base::GetNaturalSize(naturalSize);
  }
}

void WireframeVisual::DoCreatePropertyMap(Property::Map& map) const
{
  if(mActualVisual)
  {
    mActualVisual->CreatePropertyMap(map);
  }
  else
  {
    map.Clear();
    map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::WIREFRAME);
  }
}

void WireframeVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  // Do nothing
}

void WireframeVisual::DoSetProperties(const Property::Map& propertyMap)
{
  Property::Value* mixValue = propertyMap.Find(Toolkit::Visual::Property::MIX_COLOR, MIX_COLOR);
  if(mixValue)
  {
    Vector4 mixColor;
    mixValue->Get(mixColor);
    SetMixColor(mixColor);
  }
}

void WireframeVisual::DoSetOnScene(Actor& actor)
{
  actor.AddRenderer(mImpl->mRenderer);

  // Wireframe generated and ready to display
  ResourceReady(Toolkit::Visual::ResourceStatus::READY);
}

void WireframeVisual::OnInitialize()
{
  Shader shader = mFactoryCache.GetShader(VisualFactoryCache::WIREFRAME_SHADER);
  if(!shader)
  {
    shader = Shader::New(SHADER_WIREFRAME_VISUAL_SHADER_VERT, SHADER_WIREFRAME_VISUAL_SHADER_FRAG);
    mFactoryCache.SaveShader(VisualFactoryCache::WIREFRAME_SHADER, shader);
  }

  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::WIREFRAME_GEOMETRY);
  if(!geometry)
  {
    geometry = CreateQuadWireframeGeometry();
    mFactoryCache.SaveGeometry(VisualFactoryCache::WIREFRAME_GEOMETRY, geometry);
  }

  //Create the renderer
  mImpl->mRenderer = Renderer::New(geometry, shader);

  //Register transform properties
  mImpl->mTransform.RegisterUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
}

Geometry WireframeVisual::CreateQuadWireframeGeometry()
{
  const float halfWidth  = 0.5f;
  const float halfHeight = 0.5f;
  struct QuadVertex
  {
    Vector2 position;
  };
  QuadVertex quadVertexData[4] =
    {
      {Vector2(-halfWidth, -halfHeight)},
      {Vector2(halfWidth, -halfHeight)},
      {Vector2(halfWidth, halfHeight)},
      {Vector2(-halfWidth, halfHeight)}};

  Property::Map quadVertexFormat;
  quadVertexFormat[POSITION_ATTRIBUTE_NAME] = Property::VECTOR2;
  VertexBuffer quadVertices                 = VertexBuffer::New(quadVertexFormat);
  quadVertices.SetData(quadVertexData, 4);

  // Create indices
  unsigned short indexData[10] = {0, 1, 1, 2, 2, 3, 3, 0};

  // Create the geometry object
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(quadVertices);
  geometry.SetIndexBuffer(indexData, sizeof(indexData) / sizeof(indexData[0]));
  geometry.SetType(Geometry::LINES);

  return geometry;
}

void WireframeVisual::OnSetTransform()
{
  if(mImpl->mRenderer)
  {
    //Register transform properties
    mImpl->mTransform.RegisterUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
  }
}

Visual::Base& WireframeVisual::GetVisualObject()
{
  if(mActualVisual)
  {
    return *mActualVisual.Get();
  }

  return *this;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
