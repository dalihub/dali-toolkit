/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/visuals/border-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
const int CUSTOM_PROPERTY_COUNT(2); // color,size

const char* const POSITION_ATTRIBUTE_NAME("aPosition");
const char* const DRIFT_ATTRIBUTE_NAME("aDrift");
const char* const INDEX_NAME("indices");
} // namespace

BorderVisualPtr BorderVisual::New(VisualFactoryCache& factoryCache, const Property::Map& properties)
{
  BorderVisualPtr borderVisualPtr(new BorderVisual(factoryCache));
  borderVisualPtr->SetProperties(properties);
  borderVisualPtr->Initialize();
  return borderVisualPtr;
}

BorderVisual::BorderVisual(VisualFactoryCache& factoryCache)
: Visual::Base(factoryCache, Visual::FittingMode::DONT_CARE, Toolkit::Visual::BORDER),
  mBorderColor(Color::TRANSPARENT),
  mBorderSize(0.f),
  mBorderColorIndex(Property::INVALID_INDEX),
  mBorderSizeIndex(Property::INVALID_INDEX),
  mAntiAliasing(false)
{
}

BorderVisual::~BorderVisual()
{
}

void BorderVisual::DoSetProperties(const Property::Map& propertyMap)
{
  for(Property::Map::SizeType iter = 0; iter < propertyMap.Count(); ++iter)
  {
    KeyValuePair keyValue = propertyMap.GetKeyValue(iter);
    if(keyValue.first.type == Property::Key::INDEX)
    {
      DoSetProperty(keyValue.first.indexKey, keyValue.second);
    }
    else
    {
      if(keyValue.first == COLOR_NAME)
      {
        DoSetProperty(Toolkit::BorderVisual::Property::COLOR, keyValue.second);
      }
      else if(keyValue.first == SIZE_NAME)
      {
        DoSetProperty(Toolkit::BorderVisual::Property::SIZE, keyValue.second);
      }
      else if(keyValue.first == ANTI_ALIASING)
      {
        DoSetProperty(Toolkit::BorderVisual::Property::ANTI_ALIASING, keyValue.second);
      }
    }
  }
}

void BorderVisual::DoSetProperty(Dali::Property::Index        index,
                                 const Dali::Property::Value& value)
{
  switch(index)
  {
    case Toolkit::BorderVisual::Property::COLOR:
    {
      if(!value.Get(mBorderColor))
      {
        DALI_LOG_ERROR("BorderVisual: borderColor property has incorrect type\n");
      }
      break;
    }
    case Toolkit::BorderVisual::Property::SIZE:
    {
      if(!value.Get(mBorderSize))
      {
        DALI_LOG_ERROR("BorderVisual: borderSize property has incorrect type\n");
      }
      break;
    }
    case Toolkit::BorderVisual::Property::ANTI_ALIASING:
    {
      if(!value.Get(mAntiAliasing))
      {
        DALI_LOG_ERROR("BorderVisual: antiAliasing property has incorrect type\n");
      }
      break;
    }
  }
}

void BorderVisual::DoSetOnScene(Actor& actor)
{
  if(mBorderColorIndex == Property::INVALID_INDEX)
  {
    mBorderColorIndex = mImpl->mRenderer.RegisterUniqueProperty(Toolkit::BorderVisual::Property::COLOR, COLOR_NAME, mBorderColor);
  }
  if(mBorderColor.a < 1.f || mAntiAliasing)
  {
    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
  }
  if(mBorderSizeIndex == Property::INVALID_INDEX)
  {
    mBorderSizeIndex = mImpl->mRenderer.RegisterUniqueProperty(Toolkit::BorderVisual::Property::SIZE, SIZE_NAME, mBorderSize);
  }

  actor.AddRenderer(mImpl->mRenderer);

  // Border Visual Generated and ready to display
  ResourceReady(Toolkit::Visual::ResourceStatus::READY);
}

void BorderVisual::DoCreatePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::BORDER);
  map.Insert(Toolkit::BorderVisual::Property::COLOR, mBorderColor);
  map.Insert(Toolkit::BorderVisual::Property::SIZE, mBorderSize);
  map.Insert(Toolkit::BorderVisual::Property::ANTI_ALIASING, mAntiAliasing);
}

void BorderVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  // Do nothing
}

void BorderVisual::OnSetTransform()
{
  if(mImpl->mRenderer)
  {
    mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
  }
}

void BorderVisual::OnInitialize()
{
  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::BORDER_GEOMETRY);
  if(!geometry)
  {
    geometry = CreateBorderGeometry();
    mFactoryCache.SaveGeometry(VisualFactoryCache::BORDER_GEOMETRY, geometry);
  }

  Shader shader    = GetBorderShader();
  mImpl->mRenderer = VisualRenderer::New(geometry, shader);
  mImpl->mRenderer.ReserveCustomProperties(CUSTOM_PROPERTY_COUNT);

  //Register transform properties
  mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
}

Shader BorderVisual::GetBorderShader()
{
  Shader shader;
  if(mAntiAliasing)
  {
    shader = mFactoryCache.GetShader(VisualFactoryCache::BORDER_SHADER_ANTI_ALIASING);
    if(!shader)
    {
      shader = mFactoryCache.GenerateAndSaveShader(VisualFactoryCache::BORDER_SHADER_ANTI_ALIASING, Dali::Shader::GetVertexShaderPrefix() + SHADER_BORDER_VISUAL_ANTI_ALIASING_SHADER_VERT.data(), Dali::Shader::GetFragmentShaderPrefix() + SHADER_BORDER_VISUAL_ANTI_ALIASING_SHADER_FRAG.data());
    }
  }
  else
  {
    shader = mFactoryCache.GetShader(VisualFactoryCache::BORDER_SHADER);
    if(!shader)
    {
      shader = mFactoryCache.GenerateAndSaveShader(VisualFactoryCache::BORDER_SHADER, Dali::Shader::GetVertexShaderPrefix() + SHADER_BORDER_VISUAL_SHADER_VERT.data(), Dali::Shader::GetFragmentShaderPrefix() + SHADER_BORDER_VISUAL_SHADER_FRAG.data());
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
  const float halfWidth  = 0.5f;
  const float halfHeight = 0.5f;
  struct BorderVertex
  {
    Vector2 position;
    Vector2 drift;
  };
  BorderVertex borderVertexData[16] =
    {
      {Vector2(-halfWidth, -halfHeight), Vector2(0.f, 0.f)},
      {Vector2(-halfWidth, -halfHeight), Vector2(1.f, 0.f)},
      {Vector2(halfWidth, -halfHeight), Vector2(-1.f, 0.f)},
      {Vector2(halfWidth, -halfHeight), Vector2(0.f, 0.f)},

      {Vector2(-halfWidth, -halfHeight), Vector2(0.f, 1.f)},
      {Vector2(-halfWidth, -halfHeight), Vector2(1.f, 1.f)},
      {Vector2(halfWidth, -halfHeight), Vector2(-1.f, 1.f)},
      {Vector2(halfWidth, -halfHeight), Vector2(0.f, 1.f)},

      {Vector2(-halfWidth, halfHeight), Vector2(0.f, -1.f)},
      {Vector2(-halfWidth, halfHeight), Vector2(1.f, -1.f)},
      {Vector2(halfWidth, halfHeight), Vector2(-1.f, -1.f)},
      {Vector2(halfWidth, halfHeight), Vector2(0.f, -1.f)},

      {Vector2(-halfWidth, halfHeight), Vector2(0.f, 0.f)},
      {Vector2(-halfWidth, halfHeight), Vector2(1.f, 0.f)},
      {Vector2(halfWidth, halfHeight), Vector2(-1.f, 0.f)},
      {Vector2(halfWidth, halfHeight), Vector2(0.f, 0.f)},
    };

  Property::Map borderVertexFormat;
  borderVertexFormat[POSITION_ATTRIBUTE_NAME] = Property::VECTOR2;
  borderVertexFormat[DRIFT_ATTRIBUTE_NAME]    = Property::VECTOR2;
  VertexBuffer borderVertices                 = VertexBuffer::New(borderVertexFormat);
  borderVertices.SetData(borderVertexData, 16);

  // Create indices
  unsigned short indexData[24] = {1, 5, 2, 6, 3, 7, 7, 6, 11, 10, 15, 14, 14, 10, 13, 9, 12, 8, 8, 9, 4, 5, 0, 1};

  // Create the geometry object
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(borderVertices);
  geometry.SetIndexBuffer(indexData, sizeof(indexData) / sizeof(indexData[0]));
  geometry.SetType(Geometry::TRIANGLE_STRIP);

  return geometry;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
