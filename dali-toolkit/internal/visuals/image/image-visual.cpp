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
 *
 */

// CLASS HEADER
#include <dali-toolkit/internal/visuals/image/image-visual.h>

// EXTERNAL HEADERS
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/rendering/texture-devel.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/adaptor-framework/async-task-manager.h>
#include <dali/public-api/rendering/decorated-visual-renderer.h>
#include <cstring> // for strlen()

// INTERNAL HEADERS
#include <dali-toolkit/devel-api/visuals/image-visual-actions-devel.h>
#include <dali-toolkit/internal/texture-manager/texture-manager-impl.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/image-visual-shader-factory.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-url.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
const int CUSTOM_PROPERTY_COUNT(7); // ltr, wrap, pixel area, atlas, pixalign, crop to mask, mask texture ratio

// fitting modes
DALI_ENUM_TO_STRING_TABLE_BEGIN(FITTING_MODE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::FittingMode, SHRINK_TO_FIT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::FittingMode, SCALE_TO_FILL)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::FittingMode, FIT_WIDTH)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::FittingMode, FIT_HEIGHT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::FittingMode, DEFAULT)
DALI_ENUM_TO_STRING_TABLE_END(FITTING_MODE)

// sampling modes
DALI_ENUM_TO_STRING_TABLE_BEGIN(SAMPLING_MODE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, BOX)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, NEAREST)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, LINEAR)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, BOX_THEN_NEAREST)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, BOX_THEN_LINEAR)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, NO_FILTER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, DONT_CARE)
DALI_ENUM_TO_STRING_TABLE_END(SAMPLING_MODE)

// wrap modes
DALI_ENUM_TO_STRING_TABLE_BEGIN(WRAP_MODE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::WrapMode, DEFAULT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::WrapMode, CLAMP_TO_EDGE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::WrapMode, REPEAT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::WrapMode, MIRRORED_REPEAT)
DALI_ENUM_TO_STRING_TABLE_END(WRAP_MODE)

// load policies
DALI_ENUM_TO_STRING_TABLE_BEGIN(LOAD_POLICY)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::ImageVisual::LoadPolicy, IMMEDIATE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::ImageVisual::LoadPolicy, ATTACHED)
DALI_ENUM_TO_STRING_TABLE_END(LOAD_POLICY)

// release policies
DALI_ENUM_TO_STRING_TABLE_BEGIN(RELEASE_POLICY)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::ImageVisual::ReleasePolicy, DETACHED)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::ImageVisual::ReleasePolicy, DESTROYED)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::ImageVisual::ReleasePolicy, NEVER)
DALI_ENUM_TO_STRING_TABLE_END(RELEASE_POLICY)

const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

constexpr uint32_t TEXTURE_COUNT_FOR_GPU_ALPHA_MASK = 2u;

Geometry CreateGeometry(VisualFactoryCache& factoryCache, ImageDimensions gridSize)
{
  Geometry geometry;

  if(gridSize == ImageDimensions(1, 1))
  {
    geometry = factoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);
  }
  else
  {
    geometry = VisualFactoryCache::CreateGridGeometry(gridSize);
  }

  return geometry;
}

} // unnamed namespace

ImageVisualPtr ImageVisual::New(VisualFactoryCache&       factoryCache,
                                ImageVisualShaderFactory& shaderFactory,
                                const VisualUrl&          imageUrl,
                                const Property::Map&      properties,
                                ImageDimensions           size,
                                FittingMode::Type         fittingMode,
                                Dali::SamplingMode::Type  samplingMode)
{
  ImageVisualPtr imageVisualPtr(new ImageVisual(factoryCache, shaderFactory, imageUrl, size, fittingMode, samplingMode));
  imageVisualPtr->SetProperties(properties);
  imageVisualPtr->Initialize();
  return imageVisualPtr;
}

ImageVisualPtr ImageVisual::New(VisualFactoryCache&       factoryCache,
                                ImageVisualShaderFactory& shaderFactory,
                                const VisualUrl&          imageUrl,
                                ImageDimensions           size,
                                FittingMode::Type         fittingMode,
                                Dali::SamplingMode::Type  samplingMode)
{
  ImageVisualPtr imageVisualPtr(new ImageVisual(factoryCache, shaderFactory, imageUrl, size, fittingMode, samplingMode));
  imageVisualPtr->Initialize();
  return imageVisualPtr;
}

ImageVisual::ImageVisual(VisualFactoryCache&       factoryCache,
                         ImageVisualShaderFactory& shaderFactory,
                         const VisualUrl&          imageUrl,
                         ImageDimensions           size,
                         FittingMode::Type         fittingMode,
                         Dali::SamplingMode::Type  samplingMode)
: Visual::Base(factoryCache, Visual::FittingMode::FILL, Toolkit::Visual::IMAGE),
  mPixelArea(FULL_TEXTURE_RECT),
  mPlacementActor(),
  mImageUrl(imageUrl),
  mMaskingData(),
  mDesiredSize(size),
  mTextureId(TextureManager::INVALID_TEXTURE_ID),
  mTextures(),
  mImageVisualShaderFactory(shaderFactory),
  mFittingMode(fittingMode),
  mSamplingMode(samplingMode),
  mWrapModeU(WrapMode::DEFAULT),
  mWrapModeV(WrapMode::DEFAULT),
  mLoadPolicy(Toolkit::ImageVisual::LoadPolicy::ATTACHED),
  mReleasePolicy(Toolkit::ImageVisual::ReleasePolicy::DETACHED),
  mAtlasRect(0.0f, 0.0f, 0.0f, 0.0f),
  mAtlasRectSize(0, 0),
  mLoadState(TextureManager::LoadState::NOT_STARTED),
  mAttemptAtlasing(false),
  mOrientationCorrection(true)
{
  EnablePreMultipliedAlpha(mFactoryCache.GetPreMultiplyOnLoad());
}

ImageVisual::~ImageVisual()
{
  if(Stage::IsInstalled())
  {
    if(mImageUrl.IsValid())
    {
      // Decrease reference count of External Resources :
      // EncodedImageBuffer or ExternalTextures.
      // Ensure the stage is still valid before accessing texture manager.
      if(mImageUrl.GetProtocolType() == VisualUrl::TEXTURE)
      {
        TextureManager& textureManager = mFactoryCache.GetTextureManager();
        textureManager.RemoveExternalTexture(mImageUrl.GetUrl());
      }
      else if(mImageUrl.IsBufferResource())
      {
        TextureManager& textureManager = mFactoryCache.GetTextureManager();
        textureManager.RemoveEncodedImageBuffer(mImageUrl.GetUrl());
      }
    }

    // ImageVisual destroyed so remove texture unless ReleasePolicy is set to never release
    if((mTextureId != TextureManager::INVALID_TEXTURE_ID) && (mReleasePolicy != Toolkit::ImageVisual::ReleasePolicy::NEVER))
    {
      RemoveTexture();
    }

    ResetFastTrackLoadingTask();
  }
}

void ImageVisual::DoSetProperties(const Property::Map& propertyMap)
{
  // Url is already received in constructor
  for(Property::Map::SizeType iter = 0; iter < propertyMap.Count(); ++iter)
  {
    KeyValuePair keyValue = propertyMap.GetKeyValue(iter);
    if(keyValue.first.type == Property::Key::INDEX)
    {
      DoSetProperty(keyValue.first.indexKey, keyValue.second);
    }
    else
    {
      if(keyValue.first == IMAGE_FITTING_MODE)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::FITTING_MODE, keyValue.second);
      }
      else if(keyValue.first == IMAGE_SAMPLING_MODE)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::SAMPLING_MODE, keyValue.second);
      }
      else if(keyValue.first == IMAGE_DESIRED_WIDTH)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::DESIRED_WIDTH, keyValue.second);
      }
      else if(keyValue.first == IMAGE_DESIRED_HEIGHT)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::DESIRED_HEIGHT, keyValue.second);
      }
      else if(keyValue.first == PIXEL_AREA_UNIFORM_NAME)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::PIXEL_AREA, keyValue.second);
      }
      else if(keyValue.first == IMAGE_WRAP_MODE_U)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::WRAP_MODE_U, keyValue.second);
      }
      else if(keyValue.first == IMAGE_WRAP_MODE_V)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::WRAP_MODE_V, keyValue.second);
      }
      else if(keyValue.first == SYNCHRONOUS_LOADING)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, keyValue.second);
      }
      else if(keyValue.first == IMAGE_ATLASING)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::ATLASING, keyValue.second);
      }
      else if(keyValue.first == ALPHA_MASK_URL)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::ALPHA_MASK_URL, keyValue.second);
      }
      else if(keyValue.first == MASK_CONTENT_SCALE_NAME)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::MASK_CONTENT_SCALE, keyValue.second);
      }
      else if(keyValue.first == CROP_TO_MASK_NAME)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::CROP_TO_MASK, keyValue.second);
      }
      else if(keyValue.first == MASKING_TYPE_NAME)
      {
        DoSetProperty(Toolkit::DevelImageVisual::Property::MASKING_TYPE, keyValue.second);
      }
      else if(keyValue.first == LOAD_POLICY_NAME)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::LOAD_POLICY, keyValue.second);
      }
      else if(keyValue.first == RELEASE_POLICY_NAME)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::RELEASE_POLICY, keyValue.second);
      }
      else if(keyValue.first == ORIENTATION_CORRECTION_NAME)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::ORIENTATION_CORRECTION, keyValue.second);
      }
      else if(keyValue.first == FAST_TRACK_UPLOADING_NAME)
      {
        DoSetProperty(Toolkit::DevelImageVisual::Property::FAST_TRACK_UPLOADING, keyValue.second);
      }
    }
  }
  // Load image immediately if LOAD_POLICY requires it
  if(mLoadPolicy == Toolkit::ImageVisual::LoadPolicy::IMMEDIATE)
  {
    auto attemptAtlasing = AttemptAtlasing();
    LoadTexture(attemptAtlasing, mAtlasRect, mTextures, mOrientationCorrection, TextureManager::ReloadPolicy::CACHED);
  }
}

void ImageVisual::DoSetProperty(Property::Index index, const Property::Value& value)
{
  switch(index)
  {
    case Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING:
    {
      bool sync = false;
      if(value.Get(sync))
      {
        if(sync)
        {
          mImpl->mFlags |= Visual::Base::Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
        }
        else
        {
          mImpl->mFlags &= ~Visual::Base::Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
        }
      }
      else
      {
        DALI_LOG_ERROR("ImageVisual: synchronousLoading property has incorrect type\n");
      }
      break;
    }

    case Toolkit::ImageVisual::Property::DESIRED_WIDTH:
    {
      int32_t desiredWidth = 0;
      if(value.Get(desiredWidth))
      {
        mDesiredSize.SetWidth(desiredWidth);
      }
      else
      {
        DALI_LOG_ERROR("ImageVisual: desiredWidth property has incorrect type\n");
      }
      break;
    }

    case Toolkit::ImageVisual::Property::DESIRED_HEIGHT:
    {
      int32_t desiredHeight = 0;
      if(value.Get(desiredHeight))
      {
        mDesiredSize.SetHeight(desiredHeight);
      }
      else
      {
        DALI_LOG_ERROR("ImageVisual: desiredHeight property has incorrect type\n");
      }
      break;
    }

    case Toolkit::ImageVisual::Property::FITTING_MODE:
    {
      int fittingMode = 0;
      Scripting::GetEnumerationProperty(value, FITTING_MODE_TABLE, FITTING_MODE_TABLE_COUNT, fittingMode);
      mFittingMode = Dali::FittingMode::Type(fittingMode);
      break;
    }

    case Toolkit::ImageVisual::Property::SAMPLING_MODE:
    {
      int samplingMode = 0;
      Scripting::GetEnumerationProperty(value, SAMPLING_MODE_TABLE, SAMPLING_MODE_TABLE_COUNT, samplingMode);
      mSamplingMode = Dali::SamplingMode::Type(samplingMode);
      break;
    }

    case Toolkit::ImageVisual::Property::PIXEL_AREA:
    {
      value.Get(mPixelArea);
      break;
    }

    case Toolkit::ImageVisual::Property::WRAP_MODE_U:
    {
      int wrapMode = 0;
      Scripting::GetEnumerationProperty(value, WRAP_MODE_TABLE, WRAP_MODE_TABLE_COUNT, wrapMode);
      mWrapModeU = Dali::WrapMode::Type(wrapMode);
      break;
    }

    case Toolkit::ImageVisual::Property::WRAP_MODE_V:
    {
      int wrapMode = 0;
      Scripting::GetEnumerationProperty(value, WRAP_MODE_TABLE, WRAP_MODE_TABLE_COUNT, wrapMode);
      mWrapModeV = Dali::WrapMode::Type(wrapMode);
      break;
    }

    case Toolkit::ImageVisual::Property::ATLASING:
    {
      value.Get(mAttemptAtlasing);
      break;
    }

    case Toolkit::ImageVisual::Property::ALPHA_MASK_URL:
    {
      std::string alphaUrl = "";
      if(value.Get(alphaUrl))
      {
        AllocateMaskData();
        mMaskingData->mAlphaMaskUrl = alphaUrl;
      }
      break;
    }

    case Toolkit::ImageVisual::Property::MASK_CONTENT_SCALE:
    {
      float scale = 1.0f;
      if(value.Get(scale))
      {
        AllocateMaskData();
        mMaskingData->mContentScaleFactor = scale;
      }
      break;
    }

    case Toolkit::ImageVisual::Property::CROP_TO_MASK:
    {
      bool crop = false;
      if(value.Get(crop))
      {
        AllocateMaskData();
        mMaskingData->mCropToMask = crop;
      }
      break;
    }

    case Toolkit::DevelImageVisual::Property::MASKING_TYPE:
    {
      int maskingType = 0;
      if(value.Get(maskingType))
      {
        AllocateMaskData();
        if(mImageUrl.IsValid() && mImageUrl.GetProtocolType() == VisualUrl::TEXTURE)
        {
          // For external textures, only gpu masking is available.
          // Therefore, MASKING_TYPE is set to MASKING_ON_RENDERING forcelly.
          mMaskingData->mPreappliedMasking = false;
        }
        else
        {
          mMaskingData->mPreappliedMasking = Toolkit::DevelImageVisual::MaskingType::Type(maskingType) == Toolkit::DevelImageVisual::MaskingType::MASKING_ON_LOADING ? true : false;
        }
      }
      break;
    }

    case Toolkit::ImageVisual::Property::RELEASE_POLICY:
    {
      int releasePolicy = 0;
      Scripting::GetEnumerationProperty(value, RELEASE_POLICY_TABLE, RELEASE_POLICY_TABLE_COUNT, releasePolicy);
      mReleasePolicy = Toolkit::ImageVisual::ReleasePolicy::Type(releasePolicy);
      break;
    }

    case Toolkit::ImageVisual::Property::LOAD_POLICY:
    {
      int loadPolicy = 0;
      Scripting::GetEnumerationProperty(value, LOAD_POLICY_TABLE, LOAD_POLICY_TABLE_COUNT, loadPolicy);
      mLoadPolicy = Toolkit::ImageVisual::LoadPolicy::Type(loadPolicy);
      break;
    }
    case Toolkit::ImageVisual::Property::ORIENTATION_CORRECTION:
    {
      bool orientationCorrection(mOrientationCorrection);
      if(value.Get(orientationCorrection))
      {
        mOrientationCorrection = orientationCorrection;
      }
      break;
    }

    case Toolkit::DevelImageVisual::Property::FAST_TRACK_UPLOADING:
    {
      bool fastTrackUploading = false;
      if(value.Get(fastTrackUploading))
      {
        mUseFastTrackUploading = fastTrackUploading;
      }
      break;
    }
  }
}

void ImageVisual::AllocateMaskData()
{
  if(!mMaskingData)
  {
    mMaskingData.reset(new TextureManager::MaskingData());
    if(mImageUrl.IsValid() && mImageUrl.GetProtocolType() == VisualUrl::TEXTURE)
    {
      mMaskingData->mPreappliedMasking = false;
    }
  }
}

void ImageVisual::GetNaturalSize(Vector2& naturalSize)
{
  if(mDesiredSize.GetWidth() > 0 && mDesiredSize.GetHeight() > 0)
  {
    naturalSize.x = mDesiredSize.GetWidth();
    naturalSize.y = mDesiredSize.GetHeight();
    return;
  }
  else if(mImpl->mRenderer) // Check if we have a loaded image
  {
    if(mImpl->mFlags & Visual::Base::Impl::IS_ATLASING_APPLIED)
    {
      naturalSize.x = mAtlasRectSize.GetWidth();
      naturalSize.y = mAtlasRectSize.GetHeight();
      return;
    }

    auto textureSet = mImpl->mRenderer.GetTextures();
    if(textureSet && textureSet.GetTextureCount())
    {
      if(mTextureSize != Vector2::ZERO)
      {
        naturalSize = mTextureSize;
        return;
      }
    }
  }

  if(mMaskingData != NULL && mMaskingData->mAlphaMaskUrl.IsValid() &&
     mMaskingData->mCropToMask)
  {
    ImageDimensions dimensions = Dali::GetClosestImageSize(mMaskingData->mAlphaMaskUrl.GetUrl());
    if(dimensions != ImageDimensions(0, 0))
    {
      naturalSize.x = dimensions.GetWidth();
      naturalSize.y = dimensions.GetHeight();
    }
    return;
  }
  else if(mImageUrl.IsValid())
  {
    if(mImageUrl.GetProtocolType() == VisualUrl::LOCAL)
    {
      ImageDimensions dimensions = Dali::GetClosestImageSize(mImageUrl.GetUrl());

      if(dimensions != ImageDimensions(0, 0))
      {
        naturalSize.x = dimensions.GetWidth();
        naturalSize.y = dimensions.GetHeight();
      }
      else
      {
        Actor   actor     = mPlacementActor.GetHandle();
        Vector2 imageSize = Vector2::ZERO;
        if(actor)
        {
          imageSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
        }
        else
        {
          imageSize = mPlacementActorSize;
        }

        mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize);
        Texture brokenImage = mImpl->mRenderer.GetTextures().GetTexture(0);
        naturalSize.x       = brokenImage.GetWidth();
        naturalSize.y       = brokenImage.GetWidth();
      }
      return;
    }
  }
  naturalSize = Vector2::ZERO;
}

void ImageVisual::OnInitialize()
{
  Geometry geometry;

  // Get the geometry
  if(mImpl->mCustomShader)
  {
    geometry = CreateGeometry(mFactoryCache, mImpl->mCustomShader->mGridSize);
  }
  else // Get any geometry associated with the texture
  {
    TextureManager& textureManager = mFactoryCache.GetTextureManager();

    uint32_t firstElementCount{0u};
    uint32_t secondElementCount{0u};
    geometry = textureManager.GetRenderGeometry(mTextureId, firstElementCount, secondElementCount);

    if(!firstElementCount && !secondElementCount) // Otherwise use quad
    {
      geometry = CreateGeometry(mFactoryCache, ImageDimensions(1, 1));
    }
  }

  // Increase reference count of External Resources :
  // EncodedImageBuffer or ExternalTextures.
  // Reference count will be decreased at destructor of the visual.
  if(mImageUrl.IsValid() && (mImageUrl.IsBufferResource() || mImageUrl.GetProtocolType() == VisualUrl::TEXTURE))
  {
    TextureManager& textureManager = mFactoryCache.GetTextureManager();
    textureManager.UseExternalResource(mImageUrl.GetUrl());
  }

  Shader shader = GenerateShader();

  // Create the renderer
  mImpl->mRenderer = DecoratedVisualRenderer::New(geometry, shader);
  mImpl->mRenderer.ReserveCustomProperties(CUSTOM_PROPERTY_COUNT);

  //Register transform properties
  mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);

  EnablePreMultipliedAlpha(IsPreMultipliedAlphaEnabled());

  if(mMaskingData)
  {
    mImpl->mRenderer.RegisterProperty(CROP_TO_MASK_NAME, static_cast<float>(mMaskingData->mCropToMask));
  }
}

void ImageVisual::LoadTexture(bool& atlasing, Vector4& atlasRect, TextureSet& textures, bool orientationCorrection, TextureManager::ReloadPolicy forceReload)
{
  TextureManager& textureManager = mFactoryCache.GetTextureManager();

  ImageAtlasManagerPtr atlasManager        = nullptr;
  AtlasUploadObserver* atlasUploadObserver = nullptr;
  auto                 textureObserver     = this;

  if(atlasing)
  {
    atlasManager        = mFactoryCache.GetAtlasManager();
    atlasUploadObserver = this;
  }

  auto preMultiplyOnLoad = IsPreMultipliedAlphaEnabled() && !mImpl->mCustomShader
                             ? TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD
                             : TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

  bool synchronousLoading = IsSynchronousLoadingRequired();
  bool loadingStatus;

  // Remove previous loading task.
  ResetFastTrackLoadingTask();

  // Rare case. If someone call LoadTexture during fast track loading task running, (Ex : Action::RELOAD)
  // we should remove previously added renderer now.
  if(mRendererAdded)
  {
    Actor actor = mPlacementActor.GetHandle();
    if(actor)
    {
      actor.RemoveRenderer(mImpl->mRenderer);
      mRendererAdded = false;
    }
  }

  /**
   * @brief Check whether FastTrackUploading is avaliable or not.
   * @return True if we can use fast track uploading feature. False otherwise.
   */
  auto IsFastTrackUploadingAvailable = [&]() {
    if(mUseFastTrackUploading &&
       mLoadPolicy == Toolkit::ImageVisual::LoadPolicy::ATTACHED &&
       mReleasePolicy == Toolkit::ImageVisual::ReleasePolicy::DETACHED &&
       forceReload == TextureManager::ReloadPolicy::CACHED &&
       (mImageUrl.GetProtocolType() == VisualUrl::LOCAL || mImageUrl.GetProtocolType() == VisualUrl::REMOTE) &&
       !synchronousLoading &&
       !atlasing &&
       !mImpl->mCustomShader &&
       !(mMaskingData && mMaskingData->mAlphaMaskUrl.IsValid()))
    {
      return true;
    }
    else if(mUseFastTrackUploading)
    {
      DALI_LOG_DEBUG_INFO("FastTrack : Fail to load fast track. mUrl : [%s]%s%s%s%s%s%s%s%s\n",
                          mImageUrl.GetUrl().c_str(),
                          (mLoadPolicy != Toolkit::ImageVisual::LoadPolicy::ATTACHED) ? "/ mLoadPolicy != ATTACHED" : "",
                          (mReleasePolicy != Toolkit::ImageVisual::ReleasePolicy::DETACHED) ? "/ mReleasePolicy != DETACHED" : "",
                          (forceReload != TextureManager::ReloadPolicy::CACHED) ? "/ forceReload != CACHED" : "",
                          (!(mImageUrl.GetProtocolType() == VisualUrl::LOCAL || mImageUrl.GetProtocolType() == VisualUrl::REMOTE)) ? "/ url is not image" : "",
                          (synchronousLoading) ? "/ synchronousLoading" : "",
                          (atlasing) ? "/ atlasing" : "",
                          (mImpl->mCustomShader) ? "/ use customs shader" : "",
                          (mMaskingData && mMaskingData->mAlphaMaskUrl.IsValid()) ? "/ use masking url" : "");
    }
    return false;
  };

  if(IsFastTrackUploadingAvailable())
  {
    // Enable PremultipliedAlpha first.
    EnablePreMultipliedAlpha(preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD);

    // Set new TextureSet with fast track loading task
    mFastTrackLoadingTask = new FastTrackLoadingTask(mImageUrl, mDesiredSize, mFittingMode, mSamplingMode, mOrientationCorrection, preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD ? DevelAsyncImageLoader::PreMultiplyOnLoad::ON : DevelAsyncImageLoader::PreMultiplyOnLoad::OFF, MakeCallback(this, &ImageVisual::FastLoadComplete));

    TextureSet textureSet = TextureSet::New();
    textureSet.SetTexture(0u, mFastTrackLoadingTask->mTexture);
    mImpl->mRenderer.SetTextures(textureSet);

    Dali::AsyncTaskManager::Get().AddTask(mFastTrackLoadingTask);

    mLoadState = TextureManager::LoadState::LOADING;
  }
  else
  {
    textures = textureManager.LoadTexture(mImageUrl, mDesiredSize, mFittingMode, mSamplingMode, mMaskingData, synchronousLoading, mTextureId, atlasRect, mAtlasRectSize, atlasing, loadingStatus, textureObserver, atlasUploadObserver, atlasManager, mOrientationCorrection, forceReload, preMultiplyOnLoad);
  }

  if(textures)
  {
    if(loadingStatus)
    {
      mLoadState = TextureManager::LoadState::LOADING;
    }
    else
    {
      mLoadState = TextureManager::LoadState::LOAD_FINISHED;
    }

    EnablePreMultipliedAlpha(preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD);
    if(!atlasing)
    {
      Sampler sampler = Sampler::New();
      sampler.SetWrapMode(mWrapModeU, mWrapModeV);
      textures.SetSampler(0u, sampler);
    }
  }
  else if(synchronousLoading)
  {
    // Synchronous loading is failed
    mLoadState = TextureManager::LoadState::LOAD_FAILED;
  }

  if(atlasing) // Flag needs to be set before creating renderer
  {
    mImpl->mFlags |= Visual::Base::Impl::IS_ATLASING_APPLIED;
  }
  else
  {
    mImpl->mFlags &= ~Visual::Base::Impl::IS_ATLASING_APPLIED;
  }
}

bool ImageVisual::AttemptAtlasing()
{
  return (!mImpl->mCustomShader && (mImageUrl.IsLocalResource() || mImageUrl.IsBufferResource()) && mAttemptAtlasing);
}

void ImageVisual::InitializeRenderer()
{
  auto attemptAtlasing = AttemptAtlasing();

  // Load Texture if mTextures is empty.
  // mTextures is already set, the mTexture can be used to create Renderer.
  // There are two cases mTextures is empty.
  // 1. mTextureId == TextureManager::INVALID_TEXTURE_ID
  //  - Visual is on stage with LoadPolicy::ATTACHED
  // 2. mTextureId != TextureManager::INVALID_TEXTURE_ID
  //  - If ReleasePolicy is DESTROYED, InitializeRenderer called every on stage called.
  //  - Then every resources those contained in Visual are Reset but mTextureId is remained when the Off stage time,
  //  - So, mTextures needed to be get from texture manager to created resources like mImpl->mRenderer.
  if(!mTextures)
  {
    if(mTextureId == TextureManager::INVALID_TEXTURE_ID)
    {
      LoadTexture(attemptAtlasing, mAtlasRect, mTextures, mOrientationCorrection, TextureManager::ReloadPolicy::CACHED);
    }
    else
    {
      mTextures = mFactoryCache.GetTextureManager().GetTextureSet(mTextureId);
      if(!(mImpl->mFlags & Visual::Base::Impl::IS_ATLASING_APPLIED) && mTextures)
      {
        Sampler sampler = Sampler::New();
        sampler.SetWrapMode(mWrapModeU, mWrapModeV);
        mTextures.SetSampler(0u, sampler);
      }
    }
  }

  if(mTextures)
  {
    mImpl->mRenderer.SetTextures(mTextures);
    ComputeTextureSize();
    CheckMaskTexture();

    bool needToUpdateShader = DevelTexture::IsNative(mTextures.GetTexture(0));

    if(mTextures.GetTextureCount() == 3)
    {
      if(mTextures.GetTexture(0).GetPixelFormat() == Pixel::L8 && mTextures.GetTexture(1).GetPixelFormat() == Pixel::CHROMINANCE_U && mTextures.GetTexture(2).GetPixelFormat() == Pixel::CHROMINANCE_V)
      {
        mNeedYuvToRgb      = true;
        needToUpdateShader = true;
      }
    }

    if(needToUpdateShader)
    {
      UpdateShader();
    }
    mTextures.Reset(); // Visual should not keep a handle to the texture after this point.
  }

  if(attemptAtlasing) // the texture is packed inside atlas
  {
    mImpl->mRenderer.RegisterProperty(ATLAS_RECT_UNIFORM_NAME, mAtlasRect);

    bool defaultWrapMode = mWrapModeU <= WrapMode::CLAMP_TO_EDGE && mWrapModeV <= WrapMode::CLAMP_TO_EDGE;

    if(!defaultWrapMode) // custom wrap mode
    {
      Vector2 wrapMode(mWrapModeU - WrapMode::CLAMP_TO_EDGE, mWrapModeV - WrapMode::CLAMP_TO_EDGE);
      wrapMode.Clamp(Vector2::ZERO, Vector2(2.f, 2.f));
      mImpl->mRenderer.RegisterProperty(WRAP_MODE_UNIFORM_NAME, wrapMode);
    }
  }
}

void ImageVisual::DoSetOnScene(Actor& actor)
{
  if(mImageUrl.IsValid())
  {
    InitializeRenderer();
  }

  if(!mImpl->mRenderer)
  {
    return;
  }

  mPlacementActor = actor;

  if(mPixelArea != FULL_TEXTURE_RECT)
  {
    mImpl->mRenderer.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, mPixelArea);
  }

  if(mLoadState == TextureManager::LoadState::LOAD_FINISHED)
  {
    actor.AddRenderer(mImpl->mRenderer);
    mRendererAdded = true;
    mPlacementActor.Reset();

    // Image loaded and ready to display
    ResourceReady(Toolkit::Visual::ResourceStatus::READY);
  }
  else if(mLoadState == TextureManager::LoadState::LOAD_FAILED)
  {
    Vector2 imageSize = Vector2::ZERO;
    if(actor)
    {
      imageSize           = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
      mPlacementActorSize = imageSize;
    }

    mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize);
    actor.AddRenderer(mImpl->mRenderer);
    mRendererAdded = true;

    mPlacementActor.Reset();

    ResourceReady(Toolkit::Visual::ResourceStatus::FAILED);
  }
  else
  {
    if(mFastTrackLoadingTask)
    {
      actor.AddRenderer(mImpl->mRenderer);
      mRendererAdded = true;
    }
  }
}

void ImageVisual::DoSetOffScene(Actor& actor)
{
  // Visual::Base::SetOffScene only calls DoSetOffScene if mRenderer exists (is on onstage)

  // Image release is dependent on the ReleasePolicy, renderer is removed.
  actor.RemoveRenderer(mImpl->mRenderer);
  mRendererAdded = false;

  if(mReleasePolicy == Toolkit::ImageVisual::ReleasePolicy::DETACHED)
  {
    RemoveTexture(); // If INVALID_TEXTURE_ID then removal will be attempted on atlas
    mImpl->mResourceStatus = Toolkit::Visual::ResourceStatus::PREPARING;

    TextureSet textureSet = TextureSet::New();
    mImpl->mRenderer.SetTextures(textureSet);
    ComputeTextureSize();

    mLoadState = TextureManager::LoadState::NOT_STARTED;
  }

  mPlacementActor.Reset();
}

void ImageVisual::DoCreatePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);

  bool sync = IsSynchronousLoadingRequired();
  map.Insert(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, sync);
  if(mImageUrl.IsValid())
  {
    map.Insert(Toolkit::ImageVisual::Property::URL, mImageUrl.GetUrl());
    map.Insert(Toolkit::ImageVisual::Property::DESIRED_WIDTH, mDesiredSize.GetWidth());
    map.Insert(Toolkit::ImageVisual::Property::DESIRED_HEIGHT, mDesiredSize.GetHeight());
  }

  map.Insert(Toolkit::ImageVisual::Property::FITTING_MODE, mFittingMode);
  map.Insert(Toolkit::ImageVisual::Property::SAMPLING_MODE, mSamplingMode);

  map.Insert(Toolkit::ImageVisual::Property::PIXEL_AREA, mPixelArea);
  map.Insert(Toolkit::ImageVisual::Property::WRAP_MODE_U, mWrapModeU);
  map.Insert(Toolkit::ImageVisual::Property::WRAP_MODE_V, mWrapModeV);

  map.Insert(Toolkit::ImageVisual::Property::ATLASING, mAttemptAtlasing);

  if(mMaskingData != NULL)
  {
    map.Insert(Toolkit::ImageVisual::Property::ALPHA_MASK_URL, mMaskingData->mAlphaMaskUrl.GetUrl());
    map.Insert(Toolkit::ImageVisual::Property::MASK_CONTENT_SCALE, mMaskingData->mContentScaleFactor);
    map.Insert(Toolkit::ImageVisual::Property::CROP_TO_MASK, mMaskingData->mCropToMask);
    map.Insert(Toolkit::DevelImageVisual::Property::MASKING_TYPE, mMaskingData->mPreappliedMasking ? DevelImageVisual::MaskingType::MASKING_ON_LOADING : DevelImageVisual::MaskingType::MASKING_ON_RENDERING);
  }

  map.Insert(Toolkit::ImageVisual::Property::LOAD_POLICY, mLoadPolicy);
  map.Insert(Toolkit::ImageVisual::Property::RELEASE_POLICY, mReleasePolicy);
  map.Insert(Toolkit::ImageVisual::Property::ORIENTATION_CORRECTION, mOrientationCorrection);

  map.Insert(Toolkit::DevelImageVisual::Property::FAST_TRACK_UPLOADING, mUseFastTrackUploading);
}

void ImageVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  if(mImageUrl.IsValid())
  {
    map.Insert(Toolkit::ImageVisual::Property::DESIRED_WIDTH, mDesiredSize.GetWidth());
    map.Insert(Toolkit::ImageVisual::Property::DESIRED_HEIGHT, mDesiredSize.GetHeight());
  }
}

void ImageVisual::OnDoAction(const Dali::Property::Index actionId, const Dali::Property::Value& attributes)
{
  // Check if action is valid for this visual type and perform action if possible

  switch(actionId)
  {
    case DevelImageVisual::Action::RELOAD:
    {
      auto attemptAtlasing = AttemptAtlasing();
      LoadTexture(attemptAtlasing, mAtlasRect, mTextures, mOrientationCorrection, TextureManager::ReloadPolicy::FORCED);
      break;
    }
  }
}

void ImageVisual::OnSetTransform()
{
  if(mImpl->mRenderer)
  {
    mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
  }
}

void ImageVisual::UpdateShader()
{
  if(mImpl->mRenderer)
  {
    Shader shader = GenerateShader();
    mImpl->mRenderer.SetShader(shader);
  }
}

// From existing atlas manager
void ImageVisual::UploadCompleted()
{
  // Texture has been uploaded. If weak handle is holding a placement actor,
  // it is the time to add the renderer to actor.
  Actor actor = mPlacementActor.GetHandle();
  if(actor)
  {
    mImpl->mRenderer.RegisterProperty(ATLAS_RECT_UNIFORM_NAME, mAtlasRect);
    actor.AddRenderer(mImpl->mRenderer);
    mRendererAdded = true;
    // reset the weak handle so that the renderer only get added to actor once
    mPlacementActor.Reset();
  }

  // Image loaded
  ResourceReady(Toolkit::Visual::ResourceStatus::READY);
  mLoadState = TextureManager::LoadState::LOAD_FINISHED;
}

// From FastTrackLoadingTask
void ImageVisual::FastLoadComplete(FastTrackLoadingTaskPtr task)
{
  Toolkit::Visual::ResourceStatus resourceStatus;

  DALI_ASSERT_ALWAYS(task.Get() == mFastTrackLoadingTask.Get() && "Task was not canceled successfully!");
  DALI_ASSERT_ALWAYS(mRendererAdded && "Some FastTrack logic missed!");

  Actor actor = mPlacementActor.GetHandle();

  if(mFastTrackLoadingTask && mFastTrackLoadingTask->mLoadSuccess)
  {
    resourceStatus = Toolkit::Visual::ResourceStatus::READY;
    mLoadState     = TextureManager::LoadState::LOAD_FINISHED;
    if(mFastTrackLoadingTask->mLoadYuvImage)
    {
      // TODO : Implement here.
    }
    else
    {
      // TODO : Implement here.
    }

    // Change premultiplied alpha flag after change renderer.
    EnablePreMultipliedAlpha(mFastTrackLoadingTask->mPremultiplied);
  }
  else
  {
    resourceStatus    = Toolkit::Visual::ResourceStatus::FAILED;
    mLoadState        = TextureManager::LoadState::LOAD_FAILED;
    Vector2 imageSize = Vector2::ZERO;
    if(actor)
    {
      imageSize           = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
      mPlacementActorSize = imageSize;
    }
    else
    {
      imageSize = mPlacementActorSize;
    }

    // Change renderer as broken.
    if(actor)
    {
      actor.RemoveRenderer(mImpl->mRenderer);
    }
    mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize);
    if(actor)
    {
      actor.AddRenderer(mImpl->mRenderer);
    }
  }

  ResetFastTrackLoadingTask();

  // Signal to observers ( control ) that resources are ready. Must be all resources.
  ResourceReady(resourceStatus);
}

// From Texture Manager
void ImageVisual::LoadComplete(bool loadingSuccess, TextureInformation textureInformation)
{
  Toolkit::Visual::ResourceStatus resourceStatus;
  if(mImpl->mRenderer)
  {
    if(textureInformation.useAtlasing)
    {
      mImpl->mRenderer.RegisterProperty(ATLAS_RECT_UNIFORM_NAME, mAtlasRect);
    }

    EnablePreMultipliedAlpha(textureInformation.preMultiplied);

    Actor actor = mPlacementActor.GetHandle();
    if(!loadingSuccess)
    {
      Vector2 imageSize = Vector2::ZERO;
      if(actor)
      {
        imageSize           = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
        mPlacementActorSize = imageSize;
      }
      else
      {
        imageSize = mPlacementActorSize;
      }

      mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize);
      textureInformation.textureSet = mImpl->mRenderer.GetTextures();
    }
    else
    {
      if(!textureInformation.useAtlasing)
      {
        Sampler sampler = Sampler::New();
        sampler.SetWrapMode(mWrapModeU, mWrapModeV);
        textureInformation.textureSet.SetSampler(0u, sampler);
      }

      mImpl->mRenderer.SetTextures(textureInformation.textureSet);
      ComputeTextureSize();
      CheckMaskTexture();

      if(textureInformation.textureSet.GetTextureCount() == 3)
      {
        if(textureInformation.textureSet.GetTexture(0).GetPixelFormat() == Pixel::L8 && textureInformation.textureSet.GetTexture(1).GetPixelFormat() == Pixel::CHROMINANCE_U && textureInformation.textureSet.GetTexture(2).GetPixelFormat() == Pixel::CHROMINANCE_V)
        {
          mNeedYuvToRgb = true;
          UpdateShader();
        }
      }
    }

    if(actor)
    {
      actor.AddRenderer(mImpl->mRenderer);
      mRendererAdded = true;
      // reset the weak handle so that the renderer only get added to actor once
      mPlacementActor.Reset();
    }
  }

  // Storing TextureSet needed when renderer staged.
  if(!mImpl->mRenderer)
  {
    mTextures = textureInformation.textureSet;
  }

  // Image loaded, set status regardless of staged status.
  if(loadingSuccess)
  {
    resourceStatus = Toolkit::Visual::ResourceStatus::READY;
    mLoadState     = TextureManager::LoadState::LOAD_FINISHED;
  }
  else
  {
    resourceStatus = Toolkit::Visual::ResourceStatus::FAILED;
    mLoadState     = TextureManager::LoadState::LOAD_FAILED;
  }

  // use geometry if needed
  if(loadingSuccess)
  {
    uint32_t firstElementCount{0u};
    uint32_t secondElementCount{0u};
    auto     geometry = mFactoryCache.GetTextureManager().GetRenderGeometry(mTextureId, firstElementCount, secondElementCount);
    if(mImpl->mRenderer && geometry)
    {
      mImpl->mRenderer.SetGeometry(geometry);
      Dali::DevelRenderer::DrawCommand drawCommand{};
      drawCommand.drawType = DevelRenderer::DrawType::INDEXED;

      if(firstElementCount)
      {
        drawCommand.firstIndex   = 0;
        drawCommand.elementCount = firstElementCount;
        drawCommand.queue        = DevelRenderer::RENDER_QUEUE_OPAQUE;
        DevelRenderer::AddDrawCommand(mImpl->mRenderer, drawCommand);
      }

      if(secondElementCount)
      {
        drawCommand.firstIndex   = firstElementCount;
        drawCommand.elementCount = secondElementCount;
        drawCommand.queue        = DevelRenderer::RENDER_QUEUE_TRANSPARENT;
        DevelRenderer::AddDrawCommand(mImpl->mRenderer, drawCommand);
      }
    }
  }

  // Signal to observers ( control ) that resources are ready. Must be all resources.
  ResourceReady(resourceStatus);
}

void ImageVisual::RemoveTexture()
{
  if(mTextureId != TextureManager::INVALID_TEXTURE_ID)
  {
    mFactoryCache.GetTextureManager().RequestRemove(mTextureId, this);
    mTextureId = TextureManager::INVALID_TEXTURE_ID;
  }
  else
  {
    ResetFastTrackLoadingTask();

    Vector4         atlasRect(0.f, 0.f, 1.f, 1.f);
    Property::Index index = mImpl->mRenderer.GetPropertyIndex(ATLAS_RECT_UNIFORM_NAME);
    if(index != Property::INVALID_INDEX)
    {
      Property::Value atlasRectValue = mImpl->mRenderer.GetProperty(index);
      atlasRectValue.Get(atlasRect);
    }

    TextureSet textureSet = mImpl->mRenderer.GetTextures();

    if(index != Property::INVALID_INDEX)
    {
      mFactoryCache.GetAtlasManager()->Remove(textureSet, atlasRect);
    }
  }
}

void ImageVisual::ComputeTextureSize()
{
  if(mImpl->mRenderer)
  {
    auto textureSet = mImpl->mRenderer.GetTextures();
    if(textureSet && textureSet.GetTextureCount())
    {
      auto texture = textureSet.GetTexture(0);
      if(texture)
      {
        mTextureSize.x = texture.GetWidth();
        mTextureSize.y = texture.GetHeight();
        if(textureSet.GetTextureCount() > 1u && mMaskingData && !mMaskingData->mPreappliedMasking && mMaskingData->mCropToMask)
        {
          Texture maskTexture = textureSet.GetTexture(1);
          if(maskTexture)
          {
            mTextureSize.x = std::min(static_cast<uint32_t>(mTextureSize.x * mMaskingData->mContentScaleFactor), maskTexture.GetWidth());
            mTextureSize.y = std::min(static_cast<uint32_t>(mTextureSize.y * mMaskingData->mContentScaleFactor), maskTexture.GetHeight());
          }
        }
      }
    }
  }
}

Vector2 ImageVisual::ComputeMaskTextureRatio()
{
  Vector2 maskTextureRatio;
  if(mImpl->mRenderer)
  {
    auto textureSet = mImpl->mRenderer.GetTextures();
    if(textureSet && textureSet.GetTextureCount())
    {
      auto texture = textureSet.GetTexture(0);
      if(texture)
      {
        if(textureSet.GetTextureCount() > 1u && mMaskingData && !mMaskingData->mPreappliedMasking && mMaskingData->mCropToMask)
        {
          Texture maskTexture = textureSet.GetTexture(1);
          if(maskTexture)
          {
            float textureWidth  = std::max(static_cast<float>(texture.GetWidth() * mMaskingData->mContentScaleFactor), Dali::Math::MACHINE_EPSILON_1);
            float textureHeight = std::max(static_cast<float>(texture.GetHeight() * mMaskingData->mContentScaleFactor), Dali::Math::MACHINE_EPSILON_1);
            maskTextureRatio    = Vector2(std::min(static_cast<float>(maskTexture.GetWidth()), textureWidth) / textureWidth,
                                       std::min(static_cast<float>(maskTexture.GetHeight()), textureHeight) / textureHeight);
          }
        }
      }
    }
  }
  return maskTextureRatio;
}

Shader ImageVisual::GenerateShader() const
{
  Shader shader;

  bool       usesWholeTexture  = true;
  const bool useStandardShader = !mImpl->mCustomShader;
  const bool useNativeImage    = (mTextures && DevelTexture::IsNative(mTextures.GetTexture(0)));

  if(useStandardShader)
  {
    bool requiredAlphaMaskingOnRendering = (mMaskingData && !mMaskingData->mMaskImageLoadingFailed) ? !mMaskingData->mPreappliedMasking : false;
    // Create and cache the standard shader
    shader = mImageVisualShaderFactory.GetShader(
      mFactoryCache,
      ImageVisualShaderFeature::FeatureBuilder()
        .EnableTextureAtlas(mImpl->mFlags & Visual::Base::Impl::IS_ATLASING_APPLIED && !useNativeImage)
        .ApplyDefaultTextureWrapMode(mWrapModeU <= WrapMode::CLAMP_TO_EDGE && mWrapModeV <= WrapMode::CLAMP_TO_EDGE)
        .EnableRoundedCorner(IsRoundedCornerRequired())
        .EnableBorderline(IsBorderlineRequired())
        .SetTextureForFragmentShaderCheck(useNativeImage ? mTextures.GetTexture(0) : Dali::Texture())
        .EnableAlphaMaskingOnRendering(requiredAlphaMaskingOnRendering)
        .EnableYuvToRgb(mNeedYuvToRgb));
  }
  else
  {
    std::string_view vertexShaderView;
    std::string_view fragmentShaderView;

    if(mImpl->mCustomShader && !mImpl->mCustomShader->mVertexShader.empty())
    {
      vertexShaderView = mImpl->mCustomShader->mVertexShader;
      usesWholeTexture = false; // Impossible to tell.
    }
    else
    {
      vertexShaderView = mImageVisualShaderFactory.GetVertexShaderSource();
    }

    if(mImpl->mCustomShader && !mImpl->mCustomShader->mFragmentShader.empty())
    {
      fragmentShaderView = mImpl->mCustomShader->mFragmentShader;
    }
    else
    {
      fragmentShaderView = mImageVisualShaderFactory.GetFragmentShaderSource();
    }

    // If the texture is native, we may need to change prefix and sampler in
    // the fragment shader
    if(useNativeImage)
    {
      bool        modifiedFragmentShader = false;
      Texture     nativeTexture          = mTextures.GetTexture(0);
      std::string fragmentShaderString   = std::string(fragmentShaderView);

      modifiedFragmentShader = DevelTexture::ApplyNativeFragmentShader(nativeTexture, fragmentShaderString);
      if(modifiedFragmentShader)
      {
        fragmentShaderView = fragmentShaderString;
      }

      // Create shader here cause fragmentShaderString scope issue
      shader = Shader::New(vertexShaderView, fragmentShaderView, mImpl->mCustomShader->mHints);
    }
    else
    {
      shader = Shader::New(vertexShaderView, fragmentShaderView, mImpl->mCustomShader->mHints);
    }
  }

  if(usesWholeTexture)
  {
    shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);
  }

  return shader;
}

void ImageVisual::CheckMaskTexture()
{
  if(mMaskingData && !mMaskingData->mPreappliedMasking)
  {
    bool       maskLoadFailed = true;
    TextureSet textures       = mImpl->mRenderer.GetTextures();
    if(textures && textures.GetTextureCount() >= TEXTURE_COUNT_FOR_GPU_ALPHA_MASK)
    {
      if(mMaskingData->mCropToMask)
      {
        mImpl->mRenderer.RegisterProperty(MASK_TEXTURE_RATIO_NAME, ComputeMaskTextureRatio());
      }
      maskLoadFailed = false;
    }

    if(mMaskingData->mMaskImageLoadingFailed != maskLoadFailed)
    {
      mMaskingData->mMaskImageLoadingFailed = maskLoadFailed;
      UpdateShader();
    }
  }
}

void ImageVisual::ResetFastTrackLoadingTask()
{
  if(mFastTrackLoadingTask)
  {
    Dali::AsyncTaskManager::Get().RemoveTask(mFastTrackLoadingTask);
    mFastTrackLoadingTask.Reset();
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
