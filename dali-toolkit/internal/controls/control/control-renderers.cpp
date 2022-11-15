/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/control/control-renderers.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
Geometry CreateGridGeometry(Uint16Pair gridSize)
{
  uint16_t gridWidth  = gridSize.GetWidth();
  uint16_t gridHeight = gridSize.GetHeight();

  // Create vertices
  Vector<Vector2> vertices;
  vertices.Reserve((gridWidth + 1u) * (gridHeight + 1u));

  for(uint16_t y = 0u; y < gridHeight + 1u; ++y)
  {
    for(uint16_t x = 0u; x < gridWidth + 1u; ++x)
    {
      vertices.PushBack(Vector2((float)x / gridWidth - 0.5f, (float)y / gridHeight - 0.5f));
    }
  }

  // Create indices
  Vector<unsigned short> indices;
  indices.Reserve((gridWidth + 2u) * gridHeight * 2u - 2u);

  for(uint16_t row = 0u; row < gridHeight; ++row)
  {
    uint32_t rowStartIndex     = row * (gridWidth + 1u);
    uint32_t nextRowStartIndex = rowStartIndex + gridWidth + 1u;

    if(row != 0u) // degenerate index on non-first row
    {
      indices.PushBack(rowStartIndex);
    }

    for(unsigned int column = 0u; column < gridWidth + 1u; column++) // main strip
    {
      indices.PushBack(rowStartIndex + column);
      indices.PushBack(nextRowStartIndex + column);
    }

    if(row != gridHeight - 1u) // degenerate index on non-last row
    {
      indices.PushBack(nextRowStartIndex + gridWidth);
    }
  }

  Property::Map vertexFormat;
  vertexFormat["aPosition"] = Property::VECTOR2;
  VertexBuffer vertexBuffer = VertexBuffer::New(vertexFormat);
  if(vertices.Size() > 0)
  {
    vertexBuffer.SetData(&vertices[0], vertices.Size());
  }

  // Create the geometry object
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);
  if(indices.Size() > 0)
  {
    geometry.SetIndexBuffer(&indices[0], indices.Size());
  }

  geometry.SetType(Geometry::TRIANGLE_STRIP);

  return geometry;
}

Dali::Renderer CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc)
{
  Dali::Shader shader = Dali::Shader::New(vertexSrc, fragmentSrc);

  Dali::Geometry texturedQuadGeometry = Dali::Geometry::New();

  struct VertexPosition
  {
    Dali::Vector2 position;
  };
  struct VertexTexture
  {
    Dali::Vector2 texture;
  };

  VertexPosition positionArray[] =
    {
      {Dali::Vector2(-0.5f, -0.5f)},
      {Dali::Vector2(0.5f, -0.5f)},
      {Dali::Vector2(-0.5f, 0.5f)},
      {Dali::Vector2(0.5f, 0.5f)}};
  uint32_t numberOfVertices = sizeof(positionArray) / sizeof(VertexPosition);

  Dali::Property::Map positionVertexFormat;
  positionVertexFormat["aPosition"]   = Dali::Property::VECTOR2;
  Dali::VertexBuffer positionVertices = Dali::VertexBuffer::New(positionVertexFormat);
  positionVertices.SetData(positionArray, numberOfVertices);
  texturedQuadGeometry.AddVertexBuffer(positionVertices);

  const uint16_t indices[] = {0, 3, 1, 0, 2, 3};
  texturedQuadGeometry.SetIndexBuffer(&indices[0], sizeof(indices) / sizeof(indices[0]));

  Dali::Renderer renderer = Dali::Renderer::New(texturedQuadGeometry, shader);

  Dali::TextureSet textureSet = Dali::TextureSet::New();
  renderer.SetTextures(textureSet);

  return renderer;
}

Dali::Renderer CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc, Dali::Shader::Hint::Value hints, Uint16Pair gridSize)
{
  Dali::Shader shader = Dali::Shader::New(vertexSrc, fragmentSrc, hints);

  Dali::Geometry gridGeometry = CreateGridGeometry(gridSize);

  Dali::Renderer renderer = Dali::Renderer::New(gridGeometry, shader);

  Dali::TextureSet textureSet = Dali::TextureSet::New();
  renderer.SetTextures(textureSet);

  return renderer;
}

void SetRendererTexture(Dali::Renderer renderer, Dali::Texture texture)
{
  if(renderer)
  {
    Dali::TextureSet textureSet = renderer.GetTextures();
    textureSet.SetTexture(0u, texture);
  }
}

void SetRendererTexture(Dali::Renderer renderer, Dali::FrameBuffer frameBuffer)
{
  if(frameBuffer)
  {
    Dali::Texture texture = frameBuffer.GetColorTexture();
    SetRendererTexture(renderer, texture);
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
