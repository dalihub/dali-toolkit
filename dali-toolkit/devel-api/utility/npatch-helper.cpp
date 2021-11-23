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
#include <dali-toolkit/devel-api/utility/npatch-helper.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
namespace NPatchHelper
{
namespace
{
/**
 * @brief Creates the geometry formed from the vertices and indices
 *
 * @param[in]  vertices             The vertices to generate the geometry from
 * @param[in]  indices              The indices to generate the geometry from
 * @return The geometry formed from the vertices and indices
 */
Geometry GenerateGeometry(const Vector<Vector2>& vertices, const Vector<unsigned short>& indices)
{
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

  return geometry;
}

/**
 * @brief Adds the indices to form a quad composed off two triangles where the indices are organised in a grid
 *
 * @param[out] indices     The indices to add to
 * @param[in]  rowIdx      The row index to start the quad
 * @param[in]  nextRowIdx  The index to the next row
 */
void AddQuadIndices(Vector<unsigned short>& indices, unsigned int rowIdx, unsigned int nextRowIdx)
{
  indices.PushBack(rowIdx);
  indices.PushBack(nextRowIdx + 1);
  indices.PushBack(rowIdx + 1);

  indices.PushBack(rowIdx);
  indices.PushBack(nextRowIdx);
  indices.PushBack(nextRowIdx + 1);
}

/**
 * @brief Adds the vertices to create for npatch
 * @param[out] vertices The vertices to add to
 * @param[in]  x        The x value of vector
 * @param[in]  y        The y value of vector
 */
void AddVertex(Vector<Vector2>& vertices, unsigned int x, unsigned int y)
{
  vertices.PushBack(Vector2(x, y));
}

} // unnamed namespace

Geometry CreateGridGeometry(Uint16Pair gridSize)
{
  uint16_t gridWidth  = gridSize.GetWidth();
  uint16_t gridHeight = gridSize.GetHeight();

  // Create vertices
  Vector<Vector2> vertices;
  vertices.Reserve((gridWidth + 1) * (gridHeight + 1));

  for(int y = 0; y < gridHeight + 1; ++y)
  {
    for(int x = 0; x < gridWidth + 1; ++x)
    {
      AddVertex(vertices, x, y);
    }
  }

  // Create indices
  Vector<unsigned short> indices;
  indices.Reserve(gridWidth * gridHeight * 6);

  unsigned int rowIdx     = 0;
  unsigned int nextRowIdx = gridWidth + 1;
  for(int y = 0; y < gridHeight; ++y, ++nextRowIdx, ++rowIdx)
  {
    for(int x = 0; x < gridWidth; ++x, ++nextRowIdx, ++rowIdx)
    {
      AddQuadIndices(indices, rowIdx, nextRowIdx);
    }
  }

  return GenerateGeometry(vertices, indices);
}

Geometry CreateBorderGeometry(Uint16Pair gridSize)
{
  uint16_t gridWidth  = gridSize.GetWidth();
  uint16_t gridHeight = gridSize.GetHeight();

  // Create vertices
  Vector<Vector2> vertices;
  vertices.Reserve((gridWidth + 1) * (gridHeight + 1));

  //top
  int y = 0;
  for(; y < 2; ++y)
  {
    for(int x = 0; x < gridWidth + 1; ++x)
    {
      AddVertex(vertices, x, y);
    }
  }

  for(; y < gridHeight - 1; ++y)
  {
    //left
    AddVertex(vertices, 0, y);
    AddVertex(vertices, 1, y);

    //right
    AddVertex(vertices, gridWidth - 1, y);
    AddVertex(vertices, gridWidth, y);
  }

  //bottom
  for(; y < gridHeight + 1; ++y)
  {
    for(int x = 0; x < gridWidth + 1; ++x)
    {
      AddVertex(vertices, x, y);
    }
  }

  // Create indices
  Vector<unsigned short> indices;
  indices.Reserve(gridWidth * gridHeight * 6);

  //top
  unsigned int rowIdx     = 0;
  unsigned int nextRowIdx = gridWidth + 1;
  for(int x = 0; x < gridWidth; ++x, ++nextRowIdx, ++rowIdx)
  {
    AddQuadIndices(indices, rowIdx, nextRowIdx);
  }

  if(gridHeight > 2)
  {
    rowIdx     = gridWidth + 1;
    nextRowIdx = (gridWidth + 1) * 2;

    unsigned increment = gridWidth - 1;
    if(gridHeight > 3)
    {
      increment = 2;
      //second row left
      AddQuadIndices(indices, rowIdx, nextRowIdx);

      rowIdx     = gridWidth * 2;
      nextRowIdx = (gridWidth + 1) * 2 + 2;
      //second row right
      AddQuadIndices(indices, rowIdx, nextRowIdx);

      //left and right
      rowIdx     = nextRowIdx - 2;
      nextRowIdx = rowIdx + 4;
      for(int y = 2; y < 2 * (gridHeight - 3); ++y, rowIdx += 2, nextRowIdx += 2)
      {
        AddQuadIndices(indices, rowIdx, nextRowIdx);
      }
    }

    //second row left
    AddQuadIndices(indices, rowIdx, nextRowIdx);

    rowIdx += increment;
    nextRowIdx += gridWidth - 1;
    //second row right
    AddQuadIndices(indices, rowIdx, nextRowIdx);
  }

  //bottom
  rowIdx     = nextRowIdx - gridWidth + 1;
  nextRowIdx = rowIdx + gridWidth + 1;
  for(int x = 0; x < gridWidth; ++x, ++nextRowIdx, ++rowIdx)
  {
    AddQuadIndices(indices, rowIdx, nextRowIdx);
  }

  return GenerateGeometry(vertices, indices);
}

void RegisterStretchProperties(Renderer& renderer, const char* uniformName, const NPatchUtility::StretchRanges& stretchPixels, uint16_t imageExtent)
{
  uint16_t     prevEnd     = 0;
  uint16_t     prevFix     = 0;
  uint16_t     prevStretch = 0;
  unsigned int i           = 1;
  for(NPatchUtility::StretchRanges::ConstIterator it = stretchPixels.Begin(); it != stretchPixels.End(); ++it, ++i)
  {
    uint16_t start = it->GetX();
    uint16_t end   = it->GetY();

    uint16_t fix     = prevFix + start - prevEnd;
    uint16_t stretch = prevStretch + end - start;

    std::stringstream uniform;
    uniform << uniformName << "[" << i << "]";
    renderer.RegisterProperty(uniform.str(), Vector2(fix, stretch));

    prevEnd     = end;
    prevFix     = fix;
    prevStretch = stretch;
  }

  {
    prevFix += imageExtent - prevEnd;
    std::stringstream uniform;
    uniform << uniformName << "[" << i << "]";
    renderer.RegisterProperty(uniform.str(), Vector2(prevFix, prevStretch));
  }
}

void ApplyTextureAndUniforms(Renderer& renderer, const Internal::NPatchData* data)
{
  TextureSet        textureSet;
  textureSet = data->GetTextures();

  if(data->GetStretchPixelsX().Size() == 1 && data->GetStretchPixelsY().Size() == 1)
  {
    //special case for 9 patch
    Uint16Pair stretchX = data->GetStretchPixelsX()[0];
    Uint16Pair stretchY = data->GetStretchPixelsY()[0];

    uint16_t stretchWidth  = (stretchX.GetY() >= stretchX.GetX()) ? stretchX.GetY() - stretchX.GetX() : 0;
    uint16_t stretchHeight = (stretchY.GetY() >= stretchY.GetX()) ? stretchY.GetY() - stretchY.GetX() : 0;

    renderer.RegisterProperty("uFixed[0]", Vector2::ZERO);
    renderer.RegisterProperty("uFixed[1]", Vector2(stretchX.GetX(), stretchY.GetX()));
    renderer.RegisterProperty("uFixed[2]", Vector2(data->GetCroppedWidth() - stretchWidth, data->GetCroppedHeight() - stretchHeight));
    renderer.RegisterProperty("uStretchTotal", Vector2(stretchWidth, stretchHeight));
  }
  else
  {
    renderer.RegisterProperty("uNinePatchFactorsX[0]", Vector2::ZERO);
    renderer.RegisterProperty("uNinePatchFactorsY[0]", Vector2::ZERO);

    RegisterStretchProperties(renderer, "uNinePatchFactorsX", data->GetStretchPixelsX(), data->GetCroppedWidth());
    RegisterStretchProperties(renderer, "uNinePatchFactorsY", data->GetStretchPixelsY(), data->GetCroppedHeight());
  }
}

} // namespace NPatchHelper

} // namespace Toolkit

} // namespace Dali
