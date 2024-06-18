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
#include <dali-toolkit/internal/visuals/npatch/npatch-visual.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/utility/npatch-helper.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/image/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/image/image-visual-shader-factory.h>
#include <dali-toolkit/internal/visuals/image/image-visual-shader-feature-builder.h>
#include <dali-toolkit/internal/visuals/npatch/npatch-loader.h>
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
const int CUSTOM_PROPERTY_COUNT(5); // fixed(3),stretch,aux
}

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

  if(mId == NPatchData::INVALID_NPATCH_DATA_ID)
  {
    bool preMultiplyOnLoad = IsPreMultipliedAlphaEnabled() && !mImpl->mCustomShader ? true : false;
    mId                    = mLoader.Load(textureManager, this, mImageUrl, mBorder, preMultiplyOnLoad, synchronousLoading);

    NPatchDataPtr data;
    if(mLoader.GetNPatchData(mId, data) && data->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
    {
      EnablePreMultipliedAlpha(data->IsPreMultiplied());
    }
  }

  if(mAuxiliaryTextureId == TextureManager::INVALID_TEXTURE_ID && mAuxiliaryUrl.IsValid())
  {
    auto preMultiplyOnLoad = IsPreMultipliedAlphaEnabled() && !mImpl->mCustomShader
                               ? TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD
                               : TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

    TextureManager::MaskingDataPointer maskingDataPtr       = nullptr;
    ImageAtlasManagerPtr               imageAtlasManagerPtr = nullptr;

    bool atlasing      = false;
    auto atlasRect     = Vector4::ZERO;
    auto atlasRectSize = Dali::ImageDimensions();

    bool loadingStatus = false;

    // Load the auxiliary image
    mAuxiliaryTextureSet = textureManager.LoadTexture(mAuxiliaryUrl, Dali::ImageDimensions(), FittingMode::DEFAULT, SamplingMode::BOX_THEN_LINEAR, maskingDataPtr, synchronousLoading, mAuxiliaryTextureId, atlasRect, atlasRectSize, atlasing, loadingStatus, this, nullptr, imageAtlasManagerPtr, true, TextureManager::ReloadPolicy::CACHED, preMultiplyOnLoad);

    // If synchronousLoading is true, we can check the auxiliaryResource's status now.
    if(synchronousLoading)
    {
      mAuxiliaryResourceStatus = (mAuxiliaryTextureSet && mAuxiliaryTextureSet.GetTextureCount() > 0u) ? Toolkit::Visual::ResourceStatus::READY : Toolkit::Visual::ResourceStatus::FAILED;
    }
  }
}

void NPatchVisual::GetNaturalSize(Vector2& naturalSize)
{
  naturalSize.x = 0u;
  naturalSize.y = 0u;

  // load now if not already loaded
  NPatchDataPtr data;
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

  if(mAuxiliaryTextureSet && mAuxiliaryTextureSet.GetTextureCount() > 0u)
  {
    naturalSize.x = std::max(naturalSize.x, float(mAuxiliaryTextureSet.GetTexture(0u).GetWidth()));
    naturalSize.y = std::max(naturalSize.y, float(mAuxiliaryTextureSet.GetTexture(0u).GetHeight()));
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

  // Set mPlacementActor now, because some case, LoadImages can use this information in LoadComplete API.
  // at this case, we try to SetResouce to mPlaceActor twice. so, we should avoid that case.
  mPlacementActor = actor;

  NPatchDataPtr data;
  if(mImpl->mRenderer && mLoader.GetNPatchData(mId, data) && data->GetLoadingState() != NPatchData::LoadingState::LOADING)
  {
    // If mAuxiliaryUrl need to be loaded, we should wait it until LoadComplete called.
    if(!mAuxiliaryUrl.IsValid() || mAuxiliaryResourceStatus != Toolkit::Visual::ResourceStatus::PREPARING)
    {
      SetResource();
    }
  }
}

void NPatchVisual::DoSetOffScene(Actor& actor)
{
  if(mReleasePolicy == Toolkit::ImageVisual::ReleasePolicy::DETACHED)
  {
    if(mId != NPatchData::INVALID_NPATCH_DATA_ID)
    {
      mLoader.RequestRemove(mId, this);
      mImpl->mResourceStatus = Toolkit::Visual::ResourceStatus::PREPARING;
      mId                    = NPatchData::INVALID_NPATCH_DATA_ID;
    }
    if(mAuxiliaryTextureId != TextureManager::INVALID_TEXTURE_ID)
    {
      TextureManager& textureManager = mFactoryCache.GetTextureManager();
      textureManager.RequestRemove(mAuxiliaryTextureId, this);
      mAuxiliaryTextureId      = TextureManager::INVALID_TEXTURE_ID;
      mAuxiliaryResourceStatus = Toolkit::Visual::ResourceStatus::PREPARING;
      mAuxiliaryTextureSet.Reset();
    }
  }

  actor.RemoveRenderer(mImpl->mRenderer);
  mPlacementActor.Reset();
}

void NPatchVisual::OnSetTransform()
{
  if(mImpl->mRenderer)
  {
    mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
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
: Visual::Base(factoryCache, Visual::FittingMode::DONT_CARE, Toolkit::Visual::N_PATCH),
  mPlacementActor(),
  mLoader(factoryCache.GetNPatchLoader()),
  mImageVisualShaderFactory(shaderFactory),
  mImageUrl(),
  mAuxiliaryUrl(),
  mId(NPatchData::INVALID_NPATCH_DATA_ID),
  mAuxiliaryTextureSet(),
  mAuxiliaryTextureId(TextureManager::INVALID_TEXTURE_ID),
  mAuxiliaryResourceStatus(Toolkit::Visual::ResourceStatus::PREPARING),
  mBorderOnly(false),
  mBorder(),
  mAuxiliaryImageAlpha(0.0f),
  mReleasePolicy(Toolkit::ImageVisual::ReleasePolicy::DETACHED)
{
  EnablePreMultipliedAlpha(mFactoryCache.GetPreMultiplyOnLoad());
}

NPatchVisual::~NPatchVisual()
{
  if(Stage::IsInstalled())
  {
    if(mReleasePolicy != Toolkit::ImageVisual::ReleasePolicy::NEVER)
    {
      if(mId != NPatchData::INVALID_NPATCH_DATA_ID)
      {
        mLoader.RequestRemove(mId, this);
        mId = NPatchData::INVALID_NPATCH_DATA_ID;
      }
      if(mAuxiliaryTextureId != TextureManager::INVALID_TEXTURE_ID)
      {
        TextureManager& textureManager = mFactoryCache.GetTextureManager();

        textureManager.RequestRemove(mAuxiliaryTextureId, this);
        mAuxiliaryTextureId = TextureManager::INVALID_TEXTURE_ID;
        mAuxiliaryTextureSet.Reset();
      }
    }
  }
}

void NPatchVisual::OnInitialize()
{
  // Get basic geometry and shader
  Geometry geometry                        = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);
  auto     imageVisualShaderFeatureBuilder = ImageVisualShaderFeatureBuilder();
  Shader   shader                          = mImageVisualShaderFactory.GetShader(
    mFactoryCache,
    imageVisualShaderFeatureBuilder);

  mImpl->mRenderer = VisualRenderer::New(geometry, shader);
  mImpl->mRenderer.ReserveCustomProperties(CUSTOM_PROPERTY_COUNT);

  //Register transform properties
  mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
}

Geometry NPatchVisual::CreateGeometry()
{
  Geometry      geometry;
  NPatchDataPtr data;
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
        geometry = !mBorderOnly ? NPatchHelper::CreateGridGeometry(gridSize) : NPatchHelper::CreateBorderGeometry(gridSize);
      }
      else
      {
        uint32_t elementCount[2];
        geometry = !mBorderOnly ? RenderingAddOn::Get().CreateGeometryGrid(data->GetRenderingMap(), gridSize, elementCount) : NPatchHelper::CreateBorderGeometry(gridSize);
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
  Shader        shader;
  NPatchDataPtr data;
  // 0 is either no data (load failed?) or no stretch regions on image
  // for both cases we use the default shader
  NPatchUtility::StretchRanges::SizeType xStretchCount = 0;
  NPatchUtility::StretchRanges::SizeType yStretchCount = 0;

  auto fragmentShader = mAuxiliaryResourceStatus == Toolkit::Visual::ResourceStatus::READY ? SHADER_NPATCH_VISUAL_MASK_SHADER_FRAG
                                                                                           : SHADER_NPATCH_VISUAL_SHADER_FRAG;
  auto shaderType     = mAuxiliaryResourceStatus == Toolkit::Visual::ResourceStatus::READY ? VisualFactoryCache::NINE_PATCH_MASK_SHADER
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
        shader = mFactoryCache.GenerateAndSaveShader(shaderType, SHADER_NPATCH_VISUAL_3X3_SHADER_VERT, fragmentShader);
      }
    }
    else if(xStretchCount > 0 || yStretchCount > 0)
    {
      std::stringstream shaderName;
      shaderName << "N_PATCH_" << xStretchCount << "x" << yStretchCount;

      std::stringstream vertexShader;
      vertexShader << "#define FACTOR_SIZE_X " << xStretchCount + 2 << "\n"
                   << "#define FACTOR_SIZE_Y " << yStretchCount + 2 << "\n"
                   << SHADER_NPATCH_VISUAL_SHADER_VERT;

      shader = Shader::New(vertexShader.str(), fragmentShader, Dali::Shader::Hint::NONE, shaderName.str());
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
      std::stringstream shaderName;
      shaderName << "N_PATCH_" << xStretchCount << "x" << yStretchCount;

      std::stringstream vertexShader;
      vertexShader << "#define FACTOR_SIZE_X " << xStretchCount + 2 << "\n"
                   << "#define FACTOR_SIZE_Y " << yStretchCount + 2 << "\n"
                   << SHADER_NPATCH_VISUAL_SHADER_VERT;

      shader = Shader::New(vertexShader.str(), fragmentShader, hints, shaderName.str());
    }
  }

  return shader;
}

void NPatchVisual::ApplyTextureAndUniforms()
{
  NPatchDataPtr data;
  TextureSet    textureSet;

  if(mLoader.GetNPatchData(mId, data) && data->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
  {
    textureSet = data->GetTextures();
    NPatchHelper::ApplyTextureAndUniforms(mImpl->mRenderer, data.Get());

    if(mAuxiliaryResourceStatus == Toolkit::Visual::ResourceStatus::READY)
    {
      DALI_ASSERT_ALWAYS(mAuxiliaryTextureId != TextureManager::INVALID_TEXTURE_ID);
      DALI_ASSERT_ALWAYS(mAuxiliaryTextureSet && mAuxiliaryTextureSet.GetTextureCount() > 0u);

      // TODO : This code exist due to the texture cache manager hold TextureSet, not Texture.
      // If we call textureSet.SetTexture(1, texture) directly, the cached TextureSet also be changed.
      // We should make pass utc-Dali-VisualFactory.cpp UtcDaliNPatchVisualAuxiliaryImage02().
      TextureSet tempTextureSet = TextureSet::New();
      tempTextureSet.SetTexture(1, mAuxiliaryTextureSet.GetTexture(0));
      tempTextureSet.SetTexture(0, textureSet.GetTexture(0));
      textureSet = tempTextureSet;

      mImpl->mRenderer.RegisterProperty(DevelImageVisual::Property::AUXILIARY_IMAGE_ALPHA,
                                        AUXILIARY_IMAGE_ALPHA_NAME,
                                        mAuxiliaryImageAlpha);
    }
    mImpl->mRenderer.SetTextures(textureSet);
  }
  else
  {
    DALI_LOG_ERROR("The N patch image '%s' is not a valid N patch image\n", mImageUrl.GetUrl().c_str());
    Actor   actor     = mPlacementActor.GetHandle();
    Vector2 imageSize = Vector2::ZERO;
    if(actor)
    {
      imageSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
    }
    mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize, false);
  }

  // Register transform properties
  mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
}

Geometry NPatchVisual::GetNinePatchGeometry(VisualFactoryCache::GeometryType subType)
{
  Geometry geometry = mFactoryCache.GetGeometry(subType);
  if(!geometry)
  {
    if(DALI_LIKELY(VisualFactoryCache::NINE_PATCH_GEOMETRY == subType))
    {
      geometry = NPatchHelper::CreateGridGeometry(Uint16Pair(3, 3));
    }
    else if(VisualFactoryCache::NINE_PATCH_BORDER_GEOMETRY == subType)
    {
      geometry = NPatchHelper::CreateBorderGeometry(Uint16Pair(3, 3));
    }
    mFactoryCache.SaveGeometry(subType, geometry);
  }
  return geometry;
}

void NPatchVisual::SetResource()
{
  NPatchDataPtr data;
  if(mImpl->mRenderer && mLoader.GetNPatchData(mId, data))
  {
    Geometry geometry = CreateGeometry();
    Shader   shader   = CreateShader();

    mImpl->mRenderer.SetGeometry(geometry);
    mImpl->mRenderer.SetShader(shader);

    if(RenderingAddOn::Get().IsValid())
    {
      RenderingAddOn::Get().SubmitRenderTask(mImpl->mRenderer, data->GetRenderingMap());
    }
    Actor actor = mPlacementActor.GetHandle();
    if(actor)
    {
      ApplyTextureAndUniforms();
      actor.AddRenderer(mImpl->mRenderer);
      mPlacementActor.Reset();
    }

    // npatch loaded and ready to display
    if(data->GetLoadingState() != NPatchData::LoadingState::LOAD_COMPLETE)
    {
      ResourceReady(Toolkit::Visual::ResourceStatus::FAILED);
    }
    else
    {
      ResourceReady(Toolkit::Visual::ResourceStatus::READY);
    }
  }
}

void NPatchVisual::LoadComplete(bool loadSuccess, TextureInformation textureInformation)
{
  if(textureInformation.url.length() > 0) // For the Url.
  {
    if(DALI_UNLIKELY(mId == NPatchData::INVALID_NPATCH_DATA_ID))
    {
      // Special case when mLoader.Load call LoadComplete function before mId setup.
      // We can overwrite mId.
      mId = static_cast<NPatchData::NPatchDataId>(textureInformation.textureId);
    }
    if(loadSuccess)
    {
      EnablePreMultipliedAlpha(textureInformation.preMultiplied);
    }
  }
  else // For the AuxiliaryUrl
  {
    if(DALI_UNLIKELY(mAuxiliaryTextureId == TextureManager::INVALID_TEXTURE_ID))
    {
      // Special case when TextureManager.LoadTexture call LoadComplete function before mAuxiliaryTextureId setup.
      // We can overwrite mAuxiliaryTextureId.
      mAuxiliaryTextureId = textureInformation.textureId;
    }
    if(loadSuccess)
    {
      mAuxiliaryTextureSet     = textureInformation.textureSet;
      mAuxiliaryResourceStatus = Toolkit::Visual::ResourceStatus::READY;
    }
    else
    {
      mAuxiliaryResourceStatus = Toolkit::Visual::ResourceStatus::FAILED;
    }
  }

  // If auxiliaryUrl didn't required OR auxiliaryUrl load done.
  if(!mAuxiliaryUrl.IsValid() || mAuxiliaryResourceStatus != Toolkit::Visual::ResourceStatus::PREPARING)
  {
    NPatchDataPtr data;
    // and.. If Url loading done.
    if(mImpl->mRenderer && mLoader.GetNPatchData(mId, data) && data->GetLoadingState() != NPatchData::LoadingState::LOADING)
    {
      SetResource();
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
