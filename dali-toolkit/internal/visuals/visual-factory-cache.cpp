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
 */

// CLASS HEADER
#include "visual-factory-cache.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/common/hash.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-manager.h>
#include <dali-toolkit/internal/visuals/color/color-visual.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/svg/svg-visual.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
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

} //unnamed namespace

VisualFactoryCache::VisualFactoryCache(bool preMultiplyOnLoad)
: mSvgRasterizeThread(NULL),
  mVectorAnimationManager(),
  mPreMultiplyOnLoad(preMultiplyOnLoad),
  mBrokenImageInfoContainer()
{
}

VisualFactoryCache::~VisualFactoryCache()
{
  SvgRasterizeThread::TerminateThread(mSvgRasterizeThread);
}

Geometry VisualFactoryCache::GetGeometry(GeometryType type)
{
  if(!mGeometry[type] && type == QUAD_GEOMETRY)
  {
    mGeometry[type] = CreateQuadGeometry();
  }

  return mGeometry[type];
}

void VisualFactoryCache::SaveGeometry(GeometryType type, Geometry geometry)
{
  mGeometry[type] = geometry;
}

Shader VisualFactoryCache::GetShader(ShaderType type)
{
  return mShader[type];
}

void VisualFactoryCache::SaveShader(ShaderType type, Shader shader)
{
  mShader[type] = shader;
}

Geometry VisualFactoryCache::CreateQuadGeometry()
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
      {Vector2(-halfWidth, halfHeight)},
      {Vector2(halfWidth, -halfHeight)},
      {Vector2(halfWidth, halfHeight)}};

  Property::Map quadVertexFormat;
  quadVertexFormat["aPosition"] = Property::VECTOR2;
  VertexBuffer quadVertices     = VertexBuffer::New(quadVertexFormat);
  quadVertices.SetData(quadVertexData, 4);

  // Create the geometry object
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(quadVertices);
  geometry.SetType(Geometry::TRIANGLE_STRIP);

  return geometry;
}

ImageAtlasManagerPtr VisualFactoryCache::GetAtlasManager()
{
  if(!mAtlasManager)
  {
    mAtlasManager = new ImageAtlasManager();
    if(!mBrokenImageInfoContainer.empty())
    {
      mAtlasManager->SetBrokenImage(mBrokenImageInfoContainer[0].url);
    }
  }

  return mAtlasManager;
}

TextureManager& VisualFactoryCache::GetTextureManager()
{
  return mTextureManager;
}

NPatchLoader& VisualFactoryCache::GetNPatchLoader()
{
  return mNPatchLoader;
}

SvgRasterizeThread* VisualFactoryCache::GetSVGRasterizationThread()
{
  if(!mSvgRasterizeThread)
  {
    mSvgRasterizeThread = new SvgRasterizeThread();
    mSvgRasterizeThread->Start();
  }
  return mSvgRasterizeThread;
}

VectorAnimationManager& VisualFactoryCache::GetVectorAnimationManager()
{
  if(!mVectorAnimationManager)
  {
    mVectorAnimationManager = std::unique_ptr<VectorAnimationManager>(new VectorAnimationManager());
  }
  return *mVectorAnimationManager;
}

Geometry VisualFactoryCache::CreateGridGeometry(Uint16Pair gridSize)
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
      vertices.PushBack(Vector2((float)x / gridWidth - 0.5f, (float)y / gridHeight - 0.5f));
    }
  }

  // Create indices
  Vector<unsigned short> indices;
  indices.Reserve((gridWidth + 2) * gridHeight * 2 - 2);

  for(unsigned int row = 0u; row < gridHeight; ++row)
  {
    unsigned int rowStartIndex     = row * (gridWidth + 1u);
    unsigned int nextRowStartIndex = rowStartIndex + gridWidth + 1u;

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

  Property::Map indexFormat;
  indexFormat["indices"]         = Property::INTEGER;
  VertexBuffer indexVertexBuffer = VertexBuffer::New(indexFormat);

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

Texture VisualFactoryCache::GetBrokenVisualImage(uint32_t brokenIndex)
{
  if(!(mBrokenImageInfoContainer[brokenIndex].texture))
  {
    PixelData          pixelData;
    Devel::PixelBuffer pixelBuffer = LoadImageFromFile(mBrokenImageInfoContainer[brokenIndex].url);
    if(pixelBuffer)
    {
      pixelData                = Devel::PixelBuffer::Convert(pixelBuffer); // takes ownership of buffer
      mBrokenImageInfoContainer[brokenIndex].texture  = Texture::New(Dali::TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight());
      mBrokenImageInfoContainer[brokenIndex].texture.Upload(pixelData);
      mBrokenImageInfoContainer[brokenIndex].width = pixelData.GetWidth();
      mBrokenImageInfoContainer[brokenIndex].height = pixelData.GetHeight();
    }
  }
  return mBrokenImageInfoContainer[brokenIndex].texture;
}

void VisualFactoryCache::SetPreMultiplyOnLoad(bool preMultiply)
{
  mPreMultiplyOnLoad = preMultiply;
}

bool VisualFactoryCache::GetPreMultiplyOnLoad()
{
  return mPreMultiplyOnLoad;
}

void VisualFactoryCache::SetBrokenImageUrl(const std::vector<std::string>& brokenImageUrlList)
{
  mBrokenImageInfoContainer.clear();
  mBrokenImageInfoContainer.assign(brokenImageUrlList.size(), BrokenImageInfo());
  for(unsigned int i = 0; i < brokenImageUrlList.size(); i++)
  {
    mBrokenImageInfoContainer[i].url = brokenImageUrlList[i];
  }
}

VisualUrl::Type VisualFactoryCache::GetBrokenImageVisualType(int index)
{
  return mBrokenImageInfoContainer[index].visualType;
}

Geometry VisualFactoryCache::CreateNPatchGeometry(Uint16Pair gridSize)
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

Geometry VisualFactoryCache::GetNPatchGeometry(int index)
{
  Geometry          geometry;
  const NPatchData* data;
  if(mNPatchLoader.GetNPatchData(mBrokenImageInfoContainer[index].npatchId, data) && data->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
  {
    if(data->GetStretchPixelsX().Size() == 1 && data->GetStretchPixelsY().Size() == 1)
    {
      geometry = GetGeometry(VisualFactoryCache::NINE_PATCH_GEOMETRY);
      if(!geometry)
      {
        geometry = CreateNPatchGeometry(Uint16Pair(3,3));
        SaveGeometry(VisualFactoryCache::NINE_PATCH_GEOMETRY, geometry);
      }
    }
    else if(data->GetStretchPixelsX().Size() > 0 || data->GetStretchPixelsY().Size() > 0)
    {
      Uint16Pair gridSize(2 * data->GetStretchPixelsX().Size() + 1, 2 * data->GetStretchPixelsY().Size() + 1);
      geometry = CreateNPatchGeometry(gridSize);
    }
  }
  else
  {
    // no N patch data so use default geometry
    geometry = GetGeometry(VisualFactoryCache::NINE_PATCH_GEOMETRY);
    if(!geometry)
    {
      geometry = CreateNPatchGeometry(Uint16Pair(3,3));
      SaveGeometry(VisualFactoryCache::NINE_PATCH_GEOMETRY, geometry);
    }
  }
  return geometry;
}

Shader VisualFactoryCache::GetNPatchShader(int index)
{
  Shader            shader;
  const NPatchData* data;
  // 0 is either no data (load failed?) or no stretch regions on image
  // for both cases we use the default shader
  NPatchUtility::StretchRanges::SizeType xStretchCount = 0;
  NPatchUtility::StretchRanges::SizeType yStretchCount = 0;

  // ask loader for the regions
  if(mNPatchLoader.GetNPatchData(mBrokenImageInfoContainer[index].npatchId, data))
  {
    xStretchCount = data->GetStretchPixelsX().Count();
    yStretchCount = data->GetStretchPixelsY().Count();
  }

  if(DALI_LIKELY((xStretchCount == 0 && yStretchCount == 0) || (xStretchCount == 1 && yStretchCount == 1)))
  {
    shader = GetShader(VisualFactoryCache::NINE_PATCH_SHADER);
    if(DALI_UNLIKELY(!shader))
    {
      shader = Shader::New(SHADER_NPATCH_VISUAL_3X3_SHADER_VERT, SHADER_NPATCH_VISUAL_SHADER_FRAG);

      // Only cache vanilla 9 patch shaders
      SaveShader(VisualFactoryCache::NINE_PATCH_SHADER, shader);
    }
  }
  else if(xStretchCount > 0 || yStretchCount > 0)
  {
    std::stringstream vertexShader;
    vertexShader << "#define FACTOR_SIZE_X " << xStretchCount + 2 << "\n"
                 << "#define FACTOR_SIZE_Y " << yStretchCount + 2 << "\n"
                 << SHADER_NPATCH_VISUAL_SHADER_VERT;
    shader = Shader::New(vertexShader.str(), SHADER_NPATCH_VISUAL_SHADER_FRAG);
  }
  return shader;
}

void VisualFactoryCache::RegisterStretchProperties(Renderer& renderer, const char* uniformName, const NPatchUtility::StretchRanges& stretchPixels, uint16_t imageExtent)
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

void VisualFactoryCache::ApplyTextureAndUniforms(Renderer& renderer, int index)
{
  const NPatchData* data;
  TextureSet        textureSet;
  if(mNPatchLoader.GetNPatchData(mBrokenImageInfoContainer[index].npatchId, data) && data->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
  {
    textureSet = data->GetTextures();
    mBrokenImageInfoContainer[index].texture = data->GetTextures().GetTexture(0);

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
    renderer.SetTextures(textureSet);
  }
}

void VisualFactoryCache::UpdateBrokenImageRenderer(Renderer& renderer, const Vector2& size)
{

  bool useDefaultBrokenImage = false;
  if(mBrokenImageInfoContainer.size() == 1)
  {
    useDefaultBrokenImage = true;
  }

  // Load Information for broken image
  for(uint32_t index = 0; (index + 1 < mBrokenImageInfoContainer.size()) && !useDefaultBrokenImage; index++)
  {
    if(mBrokenImageInfoContainer[index].width == 0 && mBrokenImageInfoContainer[index].height == 0)
    {
      if(!mBrokenImageInfoContainer[index].url.empty())
      {
        VisualUrl visualUrl(mBrokenImageInfoContainer[index].url);
        mBrokenImageInfoContainer[index].visualType = visualUrl.GetType();
        if(mBrokenImageInfoContainer[index].visualType == VisualUrl::Type::N_PATCH)
        {
          const NPatchData* data;
          Rect<int> border;
          mBrokenImageInfoContainer[index].npatchId = mNPatchLoader.Load( mTextureManager, NULL, mBrokenImageInfoContainer[index].url, border, mPreMultiplyOnLoad, true);
          if(mNPatchLoader.GetNPatchData(mBrokenImageInfoContainer[index].npatchId, data) && data->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
          {
            mBrokenImageInfoContainer[index].width = data->GetCroppedWidth();
            mBrokenImageInfoContainer[index].height = data->GetCroppedHeight();
          }
          else
          {
            DALI_LOG_ERROR("Can't update renderer for broken image. maybe image loading is failed [index:%d] [path:%s] \n",index, mBrokenImageInfoContainer[index].url.c_str());
            useDefaultBrokenImage = true;
          }
        }
        else
        {
          if(!GetBrokenVisualImage(index))
          {
            DALI_LOG_ERROR("Can't update renderer for broken image. maybe image loading is failed [index:%d] [path:%s] \n",index, mBrokenImageInfoContainer[index].url.c_str());
            useDefaultBrokenImage = true;
          }
        }
      }
    }
  }

  if(useDefaultBrokenImage && mBrokenImageInfoContainer.size() > 1)
  {
    std::string brokenUrl = mBrokenImageInfoContainer[mBrokenImageInfoContainer.size() -1].url;

    mBrokenImageInfoContainer.clear();
    mBrokenImageInfoContainer.assign(1, BrokenImageInfo());

    const int defaultBrokenIndex = 0;
    mBrokenImageInfoContainer[defaultBrokenIndex].url = brokenUrl;
    VisualUrl visualUrl(mBrokenImageInfoContainer[defaultBrokenIndex].url);
    mBrokenImageInfoContainer[defaultBrokenIndex].visualType = visualUrl.GetType();
  }

  // Set Texutre to renderer
  int brokenIndex = GetProperBrokenImageIndex(size);
  if(GetBrokenImageVisualType(brokenIndex) == VisualUrl::N_PATCH)
  {
    // Set geometry and shader for npatch
    Geometry geometry = GetNPatchGeometry(brokenIndex);
    Shader shader = GetNPatchShader(brokenIndex);
    renderer.SetGeometry(geometry);
    renderer.SetShader(shader);
    ApplyTextureAndUniforms(renderer, brokenIndex);
  }
  else
  {
    Texture brokenImage = GetBrokenVisualImage(brokenIndex);
    TextureSet textureSet = TextureSet::New();
    textureSet.SetTexture(0u, brokenImage);
    renderer.SetTextures(textureSet);
  }
}

int32_t VisualFactoryCache::GetProperBrokenImageIndex(const Vector2& size)
{
  // Sets the default broken type
  int32_t returnIndex = 0;
  if((size.width == 0 || size.height == 0) || (mBrokenImageInfoContainer.size() == 1))
  {
    // To do : Need to add observer about size
    return returnIndex;
  }

  // Find the proper value if we know the size of the image
  for(int32_t index = static_cast<int32_t>(mBrokenImageInfoContainer.size()) - 1; index >= 0; index--)
  {
    // Skip if the value is not set
    if(mBrokenImageInfoContainer[index].width == 0 || mBrokenImageInfoContainer[index].height == 0)
    {
      continue;
    }

    if(mBrokenImageInfoContainer[index].width < size.width && mBrokenImageInfoContainer[index].height < size.height)
    {
      returnIndex = index;
      break;
    }
  }

  return returnIndex;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
