/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/environment-variable.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/common/hash.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/math/math-utils.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/utility/npatch-helper.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-manager.h>
#include <dali-toolkit/internal/visuals/color/color-visual.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/svg/svg-visual.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

constexpr auto LOAD_IMAGE_YUV_PLANES_ENV = "DALI_LOAD_IMAGE_YUV_PLANES";

bool NeedToLoadYuvPlanes()
{
  auto loadYuvPlanesString = Dali::EnvironmentVariable::GetEnvironmentVariable(LOAD_IMAGE_YUV_PLANES_ENV);
  bool loadYuvPlanes       = loadYuvPlanesString ? std::atoi(loadYuvPlanesString) : false;
  return loadYuvPlanes;
}
} // namespace

VisualFactoryCache::VisualFactoryCache(bool preMultiplyOnLoad)
: mLoadYuvPlanes(NeedToLoadYuvPlanes()),
  mTextureManager(mLoadYuvPlanes),
  mVectorAnimationManager(nullptr),
  mPreMultiplyOnLoad(preMultiplyOnLoad),
  mBrokenImageInfoContainer(),
  mDefaultBrokenImageUrl(""),
  mUseDefaultBrokenImageOnly(true)
{
}

VisualFactoryCache::~VisualFactoryCache()
{
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
    mAtlasManager->SetBrokenImage(mDefaultBrokenImageUrl);
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
      pixelData                                      = Devel::PixelBuffer::Convert(pixelBuffer); // takes ownership of buffer
      mBrokenImageInfoContainer[brokenIndex].texture = Texture::New(Dali::TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight());
      mBrokenImageInfoContainer[brokenIndex].texture.Upload(pixelData);
      mBrokenImageInfoContainer[brokenIndex].width  = pixelData.GetWidth();
      mBrokenImageInfoContainer[brokenIndex].height = pixelData.GetHeight();
    }
  }
  return mBrokenImageInfoContainer[brokenIndex].texture;
}

void VisualFactoryCache::SetPreMultiplyOnLoad(bool preMultiply)
{
  mPreMultiplyOnLoad = preMultiply;
}

bool VisualFactoryCache::GetPreMultiplyOnLoad() const
{
  return mPreMultiplyOnLoad;
}

bool VisualFactoryCache::GetLoadYuvPlanes() const
{
  return mLoadYuvPlanes;
}

void VisualFactoryCache::SetBrokenImageUrl(std::string& defaultBrokenUrl, const std::vector<std::string>& brokenImageUrlList)
{
  mUseDefaultBrokenImageOnly = false;
  mBrokenImageInfoContainer.clear();
  mBrokenImageInfoContainer.assign(brokenImageUrlList.size(), BrokenImageInfo());
  for(unsigned int i = 0; i < brokenImageUrlList.size(); i++)
  {
    mBrokenImageInfoContainer[i].url = brokenImageUrlList[i];
  }

  mDefaultBrokenImageUrl = defaultBrokenUrl;
}

VisualUrl::Type VisualFactoryCache::GetBrokenImageVisualType(int index)
{
  return mBrokenImageInfoContainer[index].visualType;
}

Geometry VisualFactoryCache::GetNPatchGeometry(int index)
{
  Geometry      geometry;
  NPatchDataPtr data;
  if(mNPatchLoader.GetNPatchData(mBrokenImageInfoContainer[index].npatchId, data) && data->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
  {
    if(data->GetStretchPixelsX().Size() == 1 && data->GetStretchPixelsY().Size() == 1)
    {
      geometry = GetGeometry(VisualFactoryCache::NINE_PATCH_GEOMETRY);
      if(!geometry)
      {
        geometry = NPatchHelper::CreateGridGeometry(Uint16Pair(3, 3));
        SaveGeometry(VisualFactoryCache::NINE_PATCH_GEOMETRY, geometry);
      }
    }
    else if(data->GetStretchPixelsX().Size() > 0 || data->GetStretchPixelsY().Size() > 0)
    {
      Uint16Pair gridSize(2 * data->GetStretchPixelsX().Size() + 1, 2 * data->GetStretchPixelsY().Size() + 1);
      geometry = NPatchHelper::CreateGridGeometry(gridSize);
    }
  }
  else
  {
    // no N patch data so use default geometry
    geometry = GetGeometry(VisualFactoryCache::NINE_PATCH_GEOMETRY);
    if(!geometry)
    {
      geometry = NPatchHelper::CreateGridGeometry(Uint16Pair(3, 3));
      SaveGeometry(VisualFactoryCache::NINE_PATCH_GEOMETRY, geometry);
    }
  }
  return geometry;
}

Shader VisualFactoryCache::GetNPatchShader(int index)
{
  Shader        shader;
  NPatchDataPtr data;
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

void VisualFactoryCache::ApplyTextureAndUniforms(Renderer& renderer, int index)
{
  NPatchDataPtr data;
  TextureSet    textureSet;
  if(mNPatchLoader.GetNPatchData(mBrokenImageInfoContainer[index].npatchId, data) && data->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
  {
    textureSet                               = data->GetTextures();
    mBrokenImageInfoContainer[index].texture = textureSet.GetTexture(0);
    NPatchHelper::ApplyTextureAndUniforms(renderer, data.Get());
    renderer.SetTextures(textureSet);
  }
}

void VisualFactoryCache::UpdateBrokenImageRenderer(Renderer& renderer, const Vector2& size, const bool& rendererIsImage)
{
  bool useDefaultBrokenImage = false;
  if(mBrokenImageInfoContainer.size() == 0)
  {
    useDefaultBrokenImage = true;
  }

  // Load Information for broken image
  for(uint32_t index = 0; (index < mBrokenImageInfoContainer.size()) && !useDefaultBrokenImage; index++)
  {
    if(mBrokenImageInfoContainer[index].width == 0 && mBrokenImageInfoContainer[index].height == 0)
    {
      if(!mBrokenImageInfoContainer[index].url.empty())
      {
        VisualUrl visualUrl(mBrokenImageInfoContainer[index].url);
        mBrokenImageInfoContainer[index].visualType = visualUrl.GetType();
        if(mBrokenImageInfoContainer[index].visualType == VisualUrl::Type::N_PATCH)
        {
          NPatchDataPtr data;
          Rect<int>     border;
          mBrokenImageInfoContainer[index].npatchId = mNPatchLoader.Load(mTextureManager, NULL, mBrokenImageInfoContainer[index].url, border, mPreMultiplyOnLoad, true);
          if(mNPatchLoader.GetNPatchData(mBrokenImageInfoContainer[index].npatchId, data) && data->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
          {
            mBrokenImageInfoContainer[index].width  = data->GetCroppedWidth();
            mBrokenImageInfoContainer[index].height = data->GetCroppedHeight();
          }
          else
          {
            DALI_LOG_ERROR("Can't update renderer for broken image. maybe image loading is failed [index:%d] [path:%s] \n", index, mBrokenImageInfoContainer[index].url.c_str());
            useDefaultBrokenImage = true;
          }
        }
        else
        {
          if(!GetBrokenVisualImage(index))
          {
            DALI_LOG_ERROR("Can't update renderer for broken image. maybe image loading is failed [index:%d] [path:%s] \n", index, mBrokenImageInfoContainer[index].url.c_str());
            useDefaultBrokenImage = true;
          }
        }
      }
    }
  }

  if(!mUseDefaultBrokenImageOnly && useDefaultBrokenImage)
  {
    // Clear broken info
    mBrokenImageInfoContainer.clear();

    // assign for broken image
    const int defaultBrokenIndex = 0;
    mBrokenImageInfoContainer.assign(1, BrokenImageInfo());
    mBrokenImageInfoContainer[defaultBrokenIndex].url = mDefaultBrokenImageUrl;
    VisualUrl visualUrl(mBrokenImageInfoContainer[defaultBrokenIndex].url);
    mBrokenImageInfoContainer[defaultBrokenIndex].visualType = visualUrl.GetType();
    mUseDefaultBrokenImageOnly                               = true;
  }

  // Set Texutre to renderer
  int brokenIndex = GetProperBrokenImageIndex(size);
  if(GetBrokenImageVisualType(brokenIndex) == VisualUrl::N_PATCH)
  {
    // Set geometry and shader for npatch
    Geometry geometry = GetNPatchGeometry(brokenIndex);
    Shader   shader   = GetNPatchShader(brokenIndex);
    renderer.SetGeometry(geometry);
    renderer.SetShader(shader);
    ApplyTextureAndUniforms(renderer, brokenIndex);
  }
  else
  {
    // Create single image renderer only if rederer is not use normal ImageShader. i.e. npatch visual.
    if(!rendererIsImage)
    {
      Geometry geometry = GetGeometry(QUAD_GEOMETRY);
      Shader   shader   = GetShader(IMAGE_SHADER);
      if(!shader)
      {
        std::string vertexShader   = std::string(Dali::Shader::GetVertexShaderPrefix() + SHADER_IMAGE_VISUAL_SHADER_VERT.data());
        std::string fragmentShader = std::string(Dali::Shader::GetFragmentShaderPrefix() + SHADER_IMAGE_VISUAL_SHADER_FRAG.data());
        shader                     = Shader::New(vertexShader, fragmentShader);
        shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);
        SaveShader(IMAGE_SHADER, shader);
      }
      renderer.SetGeometry(geometry);
      renderer.SetShader(shader);
    }
    Texture    brokenImage = GetBrokenVisualImage(brokenIndex);
    TextureSet textureSet  = TextureSet::New();
    textureSet.SetTexture(0u, brokenImage);
    renderer.SetTextures(textureSet);
  }
}

int32_t VisualFactoryCache::GetProperBrokenImageIndex(const Vector2& size)
{
  // Sets the default broken type
  int32_t returnIndex = 0;
  if(Dali::EqualsZero(size.width) || Dali::EqualsZero(size.height) || mUseDefaultBrokenImageOnly)
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
