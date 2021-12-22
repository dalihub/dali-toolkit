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
#include "npatch-visual.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/devel-api/common/stage.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/image-visual-shader-factory.h>
#include <dali-toolkit/internal/visuals/npatch-loader.h>
#include <dali-toolkit/internal/visuals/rendering-addon.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
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

void AddVertex(Vector<Vector2>& vertices, unsigned int x, unsigned int y)
{
  vertices.PushBack(Vector2(x, y));
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

} //unnamed namespace

/////////////////NPatchVisual////////////////

NPatchVisualPtr NPatchVisual::New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, const Property::Map& properties)
{
  NPatchVisualPtr nPatchVisual(new NPatchVisual(factoryCache, shaderFactory));
  nPatchVisual->mImageUrl = imageUrl;
  nPatchVisual->SetProperties(properties);
  nPatchVisual->Initialize();
  return nPatchVisual;
}

NPatchVisualPtr NPatchVisual::New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl)
{
  NPatchVisualPtr nPatchVisual(new NPatchVisual(factoryCache, shaderFactory));
  nPatchVisual->mImageUrl = imageUrl;
  nPatchVisual->Initialize();
  return nPatchVisual;
}

void NPatchVisual::LoadImages()
{
  TextureManager& textureManager     = mFactoryCache.GetTextureManager();
  bool            synchronousLoading = mImpl->mFlags & Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;

  if(mId == NPatchData::INVALID_NPATCH_DATA_ID && (mImageUrl.IsLocalResource() || mImageUrl.IsBufferResource()))
  {
    bool preMultiplyOnLoad = IsPreMultipliedAlphaEnabled() && !mImpl->mCustomShader ? true : false;
    mId                    = mLoader.Load(textureManager, this, mImageUrl, mBorder, preMultiplyOnLoad, synchronousLoading);

    const NPatchData* data;
    if(mLoader.GetNPatchData(mId, data) && data->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
    {
      EnablePreMultipliedAlpha(data->IsPreMultiplied());
    }
  }

  if(!mAuxiliaryPixelBuffer && mAuxiliaryUrl.IsValid() && (mAuxiliaryUrl.IsLocalResource() || mAuxiliaryUrl.IsBufferResource()))
  {
    // Load the auxiliary image
    auto preMultiplyOnLoading = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
    mAuxiliaryPixelBuffer     = textureManager.LoadPixelBuffer(mAuxiliaryUrl, Dali::ImageDimensions(), FittingMode::DEFAULT, SamplingMode::BOX_THEN_LINEAR, synchronousLoading, this, true, preMultiplyOnLoading);
  }
}

void NPatchVisual::GetNaturalSize(Vector2& naturalSize)
{
  naturalSize.x = 0u;
  naturalSize.y = 0u;

  // load now if not already loaded
  const NPatchData* data;
  if(mLoader.GetNPatchData(mId, data) && data->GetLoadingState() != NPatchData::LoadingState::LOADING)
  {
    naturalSize.x = data->GetCroppedWidth();
    naturalSize.y = data->GetCroppedHeight();
  }
  else
  {
    if(mImageUrl.IsValid())
    {
      ImageDimensions dimensions = Dali::GetOriginalImageSize(mImageUrl.GetUrl());
      if(dimensions != ImageDimensions(0, 0))
      {
        naturalSize.x = dimensions.GetWidth();
        naturalSize.y = dimensions.GetHeight();
      }
    }
  }

  if(mAuxiliaryPixelBuffer)
  {
    naturalSize.x = std::max(naturalSize.x, float(mAuxiliaryPixelBuffer.GetWidth()));
    naturalSize.y = std::max(naturalSize.y, float(mAuxiliaryPixelBuffer.GetHeight()));
  }
}

void NPatchVisual::DoSetProperties(const Property::Map& propertyMap)
{
  // URL is already passed in via constructor

  Property::Value* borderOnlyValue = propertyMap.Find(Toolkit::ImageVisual::Property::BORDER_ONLY, BORDER_ONLY);
  if(borderOnlyValue)
  {
    borderOnlyValue->Get(mBorderOnly);
  }

  Property::Value* borderValue = propertyMap.Find(Toolkit::ImageVisual::Property::BORDER, BORDER);
  if(borderValue && !borderValue->Get(mBorder)) // If value exists and is rect, just set mBorder
  {
    // Not a rect so try vector4
    Vector4 border;
    if(borderValue->Get(border))
    {
      mBorder.left   = static_cast<int>(border.x);
      mBorder.right  = static_cast<int>(border.y);
      mBorder.bottom = static_cast<int>(border.z);
      mBorder.top    = static_cast<int>(border.w);
    }
  }

  Property::Value* auxImage = propertyMap.Find(Toolkit::DevelImageVisual::Property::AUXILIARY_IMAGE, AUXILIARY_IMAGE_NAME);
  if(auxImage)
  {
    std::string url;
    if(auxImage->Get(url))
    {
      mAuxiliaryUrl = url;
    }
  }

  Property::Value* auxImageAlpha = propertyMap.Find(Toolkit::DevelImageVisual::Property::AUXILIARY_IMAGE_ALPHA, AUXILIARY_IMAGE_ALPHA_NAME);
  if(auxImageAlpha)
  {
    auxImageAlpha->Get(mAuxiliaryImageAlpha);
  }

  Property::Value* synchronousLoading = propertyMap.Find(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, SYNCHRONOUS_LOADING);
  if(synchronousLoading)
  {
    bool sync = false;
    synchronousLoading->Get(sync);
    if(sync)
    {
      mImpl->mFlags |= Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
    }
    else
    {
      mImpl->mFlags &= ~Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
    }
  }

  Property::Value* releasePolicy = propertyMap.Find(Toolkit::ImageVisual::Property::RELEASE_POLICY, RELEASE_POLICY_NAME);
  if(releasePolicy)
  {
    releasePolicy->Get(mReleasePolicy);
  }
}

void NPatchVisual::DoSetOnScene(Actor& actor)
{
  // load when first go on stage
  LoadImages();

  const NPatchData* data;
  if(mLoader.GetNPatchData(mId, data))
  {
    Geometry geometry = CreateGeometry();
    Shader   shader   = CreateShader();

    mImpl->mRenderer.SetGeometry(geometry);
    mImpl->mRenderer.SetShader(shader);

    mPlacementActor = actor;
    if(data->GetLoadingState() != NPatchData::LoadingState::LOADING)
    {
      if(RenderingAddOn::Get().IsValid())
      {
        RenderingAddOn::Get().SubmitRenderTask(mImpl->mRenderer, data->GetRenderingMap());
      }

      ApplyTextureAndUniforms();
      actor.AddRenderer(mImpl->mRenderer);
      mPlacementActor.Reset();

      // npatch loaded and ready to display
      ResourceReady(Toolkit::Visual::ResourceStatus::READY);
    }
  }
}

void NPatchVisual::DoSetOffScene(Actor& actor)
{
  if((mId != NPatchData::INVALID_NPATCH_DATA_ID) && mReleasePolicy == Toolkit::ImageVisual::ReleasePolicy::DETACHED)
  {
    mLoader.Remove(mId, this);
    mImpl->mResourceStatus = Toolkit::Visual::ResourceStatus::PREPARING;
    mId                    = NPatchData::INVALID_NPATCH_DATA_ID;
  }

  actor.RemoveRenderer(mImpl->mRenderer);
  mPlacementActor.Reset();
}

void NPatchVisual::OnSetTransform()
{
  if(mImpl->mRenderer)
  {
    mImpl->mTransform.RegisterUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
  }
}

void NPatchVisual::DoCreatePropertyMap(Property::Map& map) const
{
  map.Clear();
  bool sync = IsSynchronousLoadingRequired();
  map.Insert(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, sync);
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::N_PATCH);
  map.Insert(Toolkit::ImageVisual::Property::URL, mImageUrl.GetUrl());
  map.Insert(Toolkit::ImageVisual::Property::BORDER_ONLY, mBorderOnly);
  map.Insert(Toolkit::ImageVisual::Property::BORDER, mBorder);
  map.Insert(Toolkit::ImageVisual::Property::RELEASE_POLICY, mReleasePolicy);

  if(mAuxiliaryUrl.IsValid())
  {
    map.Insert(Toolkit::DevelImageVisual::Property::AUXILIARY_IMAGE, mAuxiliaryUrl.GetUrl());
    map.Insert(Toolkit::DevelImageVisual::Property::AUXILIARY_IMAGE_ALPHA, mAuxiliaryImageAlpha);
  }
}

void NPatchVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  if(mAuxiliaryUrl.IsValid())
  {
    map.Insert(Toolkit::DevelImageVisual::Property::AUXILIARY_IMAGE, mAuxiliaryUrl.GetUrl());
    map.Insert(Toolkit::DevelImageVisual::Property::AUXILIARY_IMAGE_ALPHA, mAuxiliaryImageAlpha);
  }
}

NPatchVisual::NPatchVisual(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory)
: Visual::Base(factoryCache, Visual::FittingMode::FILL, Toolkit::Visual::N_PATCH),
  mPlacementActor(),
  mLoader(factoryCache.GetNPatchLoader()),
  mImageVisualShaderFactory(shaderFactory),
  mImageUrl(),
  mAuxiliaryUrl(),
  mId(NPatchData::INVALID_NPATCH_DATA_ID),
  mBorderOnly(false),
  mBorder(),
  mAuxiliaryImageAlpha(0.0f),
  mReleasePolicy(Toolkit::ImageVisual::ReleasePolicy::DETACHED)
{
  EnablePreMultipliedAlpha(mFactoryCache.GetPreMultiplyOnLoad());
}

NPatchVisual::~NPatchVisual()
{
  if(Stage::IsInstalled() && (mId != NPatchData::INVALID_NPATCH_DATA_ID) && (mReleasePolicy != Toolkit::ImageVisual::ReleasePolicy::NEVER))
  {
    mLoader.Remove(mId, this);
    mId = NPatchData::INVALID_NPATCH_DATA_ID;
  }
}

void NPatchVisual::OnInitialize()
{
  // Get basic geometry and shader
  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);
  Shader   shader   = mImageVisualShaderFactory.GetShader(
    mFactoryCache,
    ImageVisualShaderFeature::FeatureBuilder()
  );

  mImpl->mRenderer = Renderer::New(geometry, shader);

  //Register transform properties
  mImpl->mTransform.RegisterUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
}

Geometry NPatchVisual::CreateGeometry()
{
  Geometry          geometry;
  const NPatchData* data;
  if(mLoader.GetNPatchData(mId, data) && data->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
  {
    if(data->GetStretchPixelsX().Size() == 1 && data->GetStretchPixelsY().Size() == 1)
    {
      if(DALI_UNLIKELY(mBorderOnly))
      {
        geometry = GetNinePatchGeometry(VisualFactoryCache::NINE_PATCH_BORDER_GEOMETRY);
      }
      else
      {
        if(data->GetRenderingMap())
        {
          uint32_t elementCount[2];
          geometry = RenderingAddOn::Get().CreateGeometryGrid(data->GetRenderingMap(), Uint16Pair(3, 3), elementCount);
          if(mImpl->mRenderer)
          {
            RenderingAddOn::Get().SubmitRenderTask(mImpl->mRenderer, data->GetRenderingMap());
          }
        }
        else
        {
          geometry = GetNinePatchGeometry(VisualFactoryCache::NINE_PATCH_GEOMETRY);
        }
      }
    }
    else if(data->GetStretchPixelsX().Size() > 0 || data->GetStretchPixelsY().Size() > 0)
    {
      Uint16Pair gridSize(2 * data->GetStretchPixelsX().Size() + 1, 2 * data->GetStretchPixelsY().Size() + 1);
      if(!data->GetRenderingMap())
      {
        geometry = !mBorderOnly ? CreateGridGeometry(gridSize) : CreateBorderGeometry(gridSize);
      }
      else
      {
        uint32_t elementCount[2];
        geometry = !mBorderOnly ? RenderingAddOn::Get().CreateGeometryGrid(data->GetRenderingMap(), gridSize, elementCount) : CreateBorderGeometry(gridSize);
        if(mImpl->mRenderer)
        {
          RenderingAddOn::Get().SubmitRenderTask(mImpl->mRenderer, data->GetRenderingMap());
        }
      }
    }
  }
  else
  {
    // no N patch data so use default geometry
    geometry = GetNinePatchGeometry(VisualFactoryCache::NINE_PATCH_GEOMETRY);
  }
  return geometry;
}

Shader NPatchVisual::CreateShader()
{
  Shader            shader;
  const NPatchData* data;
  // 0 is either no data (load failed?) or no stretch regions on image
  // for both cases we use the default shader
  NPatchUtility::StretchRanges::SizeType xStretchCount = 0;
  NPatchUtility::StretchRanges::SizeType yStretchCount = 0;

  auto fragmentShader = mAuxiliaryPixelBuffer ? SHADER_NPATCH_VISUAL_MASK_SHADER_FRAG
                                              : SHADER_NPATCH_VISUAL_SHADER_FRAG;
  auto shaderType     = mAuxiliaryPixelBuffer ? VisualFactoryCache::NINE_PATCH_MASK_SHADER
                                              : VisualFactoryCache::NINE_PATCH_SHADER;

  // ask loader for the regions
  if(mLoader.GetNPatchData(mId, data))
  {
    xStretchCount = data->GetStretchPixelsX().Count();
    yStretchCount = data->GetStretchPixelsY().Count();
  }

  if(DALI_LIKELY(!mImpl->mCustomShader))
  {
    if(DALI_LIKELY((xStretchCount == 1 && yStretchCount == 1) ||
                   (xStretchCount == 0 && yStretchCount == 0)))
    {
      shader = mFactoryCache.GetShader(shaderType);
      if(DALI_UNLIKELY(!shader))
      {
        shader = Shader::New(SHADER_NPATCH_VISUAL_3X3_SHADER_VERT, fragmentShader);
        // Only cache vanilla 9 patch shaders
        mFactoryCache.SaveShader(shaderType, shader);
      }
    }
    else if(xStretchCount > 0 || yStretchCount > 0)
    {
      std::stringstream vertexShader;
      vertexShader << "#define FACTOR_SIZE_X " << xStretchCount + 2 << "\n"
                   << "#define FACTOR_SIZE_Y " << yStretchCount + 2 << "\n"
                   << SHADER_NPATCH_VISUAL_SHADER_VERT;

      shader = Shader::New(vertexShader.str(), fragmentShader);
    }
  }
  else
  {
    Dali::Shader::Hint::Value hints = Dali::Shader::Hint::NONE;

    if(!mImpl->mCustomShader->mFragmentShader.empty())
    {
      fragmentShader = mImpl->mCustomShader->mFragmentShader.c_str();
    }
    hints = mImpl->mCustomShader->mHints;

    /* Apply Custom Vertex Shader only if image is 9-patch */
    if((xStretchCount == 1 && yStretchCount == 1) ||
       (xStretchCount == 0 && yStretchCount == 0))
    {
      const char* vertexShader = SHADER_NPATCH_VISUAL_3X3_SHADER_VERT.data();

      if(!mImpl->mCustomShader->mVertexShader.empty())
      {
        vertexShader = mImpl->mCustomShader->mVertexShader.c_str();
      }
      shader = Shader::New(vertexShader, fragmentShader, hints);
    }
    else if(xStretchCount > 0 || yStretchCount > 0)
    {
      std::stringstream vertexShader;
      vertexShader << "#define FACTOR_SIZE_X " << xStretchCount + 2 << "\n"
                   << "#define FACTOR_SIZE_Y " << yStretchCount + 2 << "\n"
                   << SHADER_NPATCH_VISUAL_SHADER_VERT;

      shader = Shader::New(vertexShader.str(), fragmentShader, hints);
    }
  }

  return shader;
}

void NPatchVisual::ApplyTextureAndUniforms()
{
  const NPatchData* data;
  TextureSet        textureSet;

  if(mLoader.GetNPatchData(mId, data) && data->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
  {
    textureSet = data->GetTextures();

    if(data->GetStretchPixelsX().Size() == 1 && data->GetStretchPixelsY().Size() == 1)
    {
      //special case for 9 patch
      Uint16Pair stretchX = data->GetStretchPixelsX()[0];
      Uint16Pair stretchY = data->GetStretchPixelsY()[0];

      uint16_t stretchWidth  = (stretchX.GetY() >= stretchX.GetX()) ? stretchX.GetY() - stretchX.GetX() : 0;
      uint16_t stretchHeight = (stretchY.GetY() >= stretchY.GetX()) ? stretchY.GetY() - stretchY.GetX() : 0;

      mImpl->mRenderer.RegisterProperty("uFixed[0]", Vector2::ZERO);
      mImpl->mRenderer.RegisterProperty("uFixed[1]", Vector2(stretchX.GetX(), stretchY.GetX()));
      mImpl->mRenderer.RegisterProperty("uFixed[2]", Vector2(data->GetCroppedWidth() - stretchWidth, data->GetCroppedHeight() - stretchHeight));
      mImpl->mRenderer.RegisterProperty("uStretchTotal", Vector2(stretchWidth, stretchHeight));
    }
    else
    {
      mImpl->mRenderer.RegisterProperty("uNinePatchFactorsX[0]", Vector2::ZERO);
      mImpl->mRenderer.RegisterProperty("uNinePatchFactorsY[0]", Vector2::ZERO);

      RegisterStretchProperties(mImpl->mRenderer, "uNinePatchFactorsX", data->GetStretchPixelsX(), data->GetCroppedWidth());
      RegisterStretchProperties(mImpl->mRenderer, "uNinePatchFactorsY", data->GetStretchPixelsY(), data->GetCroppedHeight());
    }
  }
  else
  {
    DALI_LOG_ERROR("The N patch image '%s' is not a valid N patch image\n", mImageUrl.GetUrl().c_str());
    textureSet = TextureSet::New();

    Actor actor = mPlacementActor.GetHandle();
    Vector2 imageSize = Vector2::ZERO;
    if(actor)
    {
      imageSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
    }
    mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize);
    Texture croppedImage = mImpl->mRenderer.GetTextures().GetTexture(0);
    textureSet.SetTexture(0u, croppedImage);
    mImpl->mRenderer.RegisterProperty("uFixed[0]", Vector2::ZERO);
    mImpl->mRenderer.RegisterProperty("uFixed[1]", Vector2::ZERO);
    mImpl->mRenderer.RegisterProperty("uFixed[2]", Vector2::ZERO);
    mImpl->mRenderer.RegisterProperty("uStretchTotal", Vector2(croppedImage.GetWidth(), croppedImage.GetHeight()));
  }

  if(mAuxiliaryPixelBuffer)
  {
    // If the auxiliary image is smaller than the un-stretched NPatch, use CPU resizing to enlarge it to the
    // same size as the unstretched NPatch. This will give slightly higher quality results than just relying
    // on GL interpolation alone.
    if(mAuxiliaryPixelBuffer.GetWidth() < data->GetCroppedWidth() &&
       mAuxiliaryPixelBuffer.GetHeight() < data->GetCroppedHeight())
    {
      mAuxiliaryPixelBuffer.Resize(data->GetCroppedWidth(), data->GetCroppedHeight());
    }

    // Note, this resets mAuxiliaryPixelBuffer handle
    auto auxiliaryPixelData = Devel::PixelBuffer::Convert(mAuxiliaryPixelBuffer);

    auto texture = Texture::New(TextureType::TEXTURE_2D,
                                auxiliaryPixelData.GetPixelFormat(),
                                auxiliaryPixelData.GetWidth(),
                                auxiliaryPixelData.GetHeight());
    texture.Upload(auxiliaryPixelData);
    textureSet.SetTexture(1, texture);
    mImpl->mRenderer.RegisterProperty(DevelImageVisual::Property::AUXILIARY_IMAGE_ALPHA,
                                      AUXILIARY_IMAGE_ALPHA_NAME,
                                      mAuxiliaryImageAlpha);
  }
  mImpl->mRenderer.SetTextures(textureSet);

  // Register transform properties
  mImpl->mTransform.RegisterUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
}

Geometry NPatchVisual::GetNinePatchGeometry(VisualFactoryCache::GeometryType subType)
{
  Geometry geometry = mFactoryCache.GetGeometry(subType);
  if(!geometry)
  {
    if(DALI_LIKELY(VisualFactoryCache::NINE_PATCH_GEOMETRY == subType))
    {
      geometry = CreateGridGeometry(Uint16Pair(3, 3));
    }
    else if(VisualFactoryCache::NINE_PATCH_BORDER_GEOMETRY == subType)
    {
      geometry = CreateBorderGeometry(Uint16Pair(3, 3));
    }
    mFactoryCache.SaveGeometry(subType, geometry);
  }
  return geometry;
}

Geometry NPatchVisual::CreateGridGeometry(Uint16Pair gridSize)
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

Geometry NPatchVisual::CreateBorderGeometry(Uint16Pair gridSize)
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

void NPatchVisual::SetResource()
{
  const NPatchData* data;
  if(mImpl->mRenderer && mLoader.GetNPatchData(mId, data))
  {
    Geometry geometry = CreateGeometry();
    Shader   shader   = CreateShader();

    mImpl->mRenderer.SetGeometry(geometry);
    mImpl->mRenderer.SetShader(shader);

    Actor actor = mPlacementActor.GetHandle();
    if(actor)
    {
      ApplyTextureAndUniforms();
      actor.AddRenderer(mImpl->mRenderer);
      mPlacementActor.Reset();

      // npatch loaded and ready to display
      ResourceReady(Toolkit::Visual::ResourceStatus::READY);
    }
  }
}

void NPatchVisual::LoadComplete(bool loadSuccess, TextureInformation textureInformation)
{
  if(textureInformation.returnType == TextureUploadObserver::ReturnType::TEXTURE)
  {
    EnablePreMultipliedAlpha(textureInformation.preMultiplied);
    if(!loadSuccess)
    {
      // Image loaded and ready to display
      ResourceReady(Toolkit::Visual::ResourceStatus::FAILED);
    }

    if(mAuxiliaryPixelBuffer || !mAuxiliaryUrl.IsValid())
    {
      SetResource();
    }
  }
  else  // for the ReturnType::PIXEL_BUFFER
  {
    if(loadSuccess && textureInformation.url == mAuxiliaryUrl.GetUrl())
    {
      mAuxiliaryPixelBuffer = textureInformation.pixelBuffer;
      SetResource();
    }
    else
    {
      // Image loaded and ready to display
      ResourceReady(Toolkit::Visual::ResourceStatus::FAILED);
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
