/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/animated-image/animated-image-visual.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/rendering/decorated-visual-renderer.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/image-loader/image-atlas.h>
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/internal/visuals/animated-image/fixed-image-cache.h>
#include <dali-toolkit/internal/visuals/animated-image/rolling-animated-image-cache.h>
#include <dali-toolkit/internal/visuals/animated-image/rolling-image-cache.h>
#include <dali-toolkit/internal/visuals/image/image-visual-shader-factory.h>
#include <dali-toolkit/internal/visuals/image/image-visual-shader-feature-builder.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
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
const int CUSTOM_PROPERTY_COUNT(6); // ltr, wrap, pixel area, crop to mask, mask texture ratio, pre-multiplied alph

// fitting modes
DALI_ENUM_TO_STRING_TABLE_BEGIN(FITTING_MODE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::FittingMode, SHRINK_TO_FIT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::FittingMode, SCALE_TO_FILL)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::FittingMode, FIT_WIDTH)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::FittingMode, FIT_HEIGHT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::FittingMode, VISUAL_FITTING)
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
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, LANCZOS)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, BOX_THEN_LANCZOS)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, DEFAULT)
DALI_ENUM_TO_STRING_TABLE_END(SAMPLING_MODE)

// stop behavior
DALI_ENUM_TO_STRING_TABLE_BEGIN(STOP_BEHAVIOR)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::DevelImageVisual::StopBehavior, CURRENT_FRAME)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::DevelImageVisual::StopBehavior, FIRST_FRAME)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::DevelImageVisual::StopBehavior, LAST_FRAME)
DALI_ENUM_TO_STRING_TABLE_END(STOP_BEHAVIOR)

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

static constexpr uint32_t SINGLE_IMAGE_COUNT = 1u;
static constexpr uint32_t FIRST_FRAME_INDEX  = 0u;
static constexpr uint16_t MINIMUM_CACHESIZE  = 1;
static constexpr Vector4  FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);
static constexpr auto     LOOP_FOREVER = -1;
static constexpr auto     FIRST_LOOP   = 0u;

constexpr float MINIMUM_FRAME_SPEED_FACTOR(0.01f);
constexpr float MAXIMUM_FRAME_SPEED_FACTOR(100.0f);

constexpr float ALPHA_VALUE_PREMULTIPLIED(1.0f);

constexpr uint32_t TEXTURE_COUNT_FOR_GPU_ALPHA_MASK = 2u;

#if defined(DEBUG_ENABLED)
Debug::Filter* gAnimImgLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_ANIMATED_IMAGE");
#endif

/**
 * @brief Safety method to calculate interval with speed factor.
 */
template<typename T>
inline uint32_t CalculateInterval(const T interval, const float frameSpeedFactor)
{
  return DALI_LIKELY(Dali::Equals(frameSpeedFactor, 1.0f)) ? static_cast<uint32_t>(interval) : static_cast<uint32_t>(static_cast<float>(interval) / (frameSpeedFactor));
}
} // namespace

/**
 * Multi-image  Flow of execution
 *
 *   | New
 *   |   DoSetProperties()
 *   |   OnInitialize()
 *   |     CreateImageCache()
 *   |
 *   | DoSetOnScene()
 *   |   PrepareTextureSet()
 *   |     cache->FirstFrame()
 *   |
 *   | FrameReady(textureSet)
 *   |   StartFirstFrame:
 *   |     actor.AddRenderer
 *   |     start timer
 *   |   mRenderer.SetTextures(textureSet)
 *   |
 *   | Timer ticks
 *   |   DisplayNextFrame()
 *   |     if front frame is ready,
 *   |       mRenderer.SetTextures( front frame's texture )
 *   |     else
 *   |       Waiting for frame ready.
 *   |
 *   | FrameReady(textureSet)
 *   |   mRenderer.SetTextures(textureSet)
 *   V
 *  Time
 */

AnimatedImageVisualPtr AnimatedImageVisual::New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, const Property::Map& properties)
{
  AnimatedImageVisualPtr visual(new AnimatedImageVisual(factoryCache, shaderFactory, ImageDimensions()));
  visual->InitializeAnimatedImage(imageUrl);
  visual->SetProperties(properties);

  visual->Initialize();

  return visual;
}

AnimatedImageVisualPtr AnimatedImageVisual::New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const Property::Array& imageUrls, const Property::Map& properties)
{
  AnimatedImageVisualPtr visual(new AnimatedImageVisual(factoryCache, shaderFactory, ImageDimensions()));
  visual->mImageUrls = new ImageCache::UrlList();
  visual->mImageUrls->reserve(imageUrls.Count());

  for(unsigned int i = 0; i < imageUrls.Count(); ++i)
  {
    ImageCache::UrlStore urlStore;
    urlStore.mTextureId = TextureManager::INVALID_TEXTURE_ID;
    urlStore.mUrl       = imageUrls[i].Get<std::string>();
    visual->mImageUrls->push_back(urlStore);
  }
  visual->mFrameCount = imageUrls.Count();
  visual->SetProperties(properties);

  visual->Initialize();

  return visual;
}

AnimatedImageVisualPtr AnimatedImageVisual::New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, ImageDimensions size)
{
  AnimatedImageVisualPtr visual(new AnimatedImageVisual(factoryCache, shaderFactory, size));
  visual->InitializeAnimatedImage(imageUrl);

  visual->Initialize();

  return visual;
}

void AnimatedImageVisual::InitializeAnimatedImage(const VisualUrl& imageUrl)
{
  mImageUrl             = imageUrl;
  mAnimatedImageLoading = AnimatedImageLoading::New(imageUrl.GetUrl(), imageUrl.IsLocalResource());

  // If we fail to load the animated image, we will try to load as a normal image.
  if(!mAnimatedImageLoading)
  {
    mImageUrls = new ImageCache::UrlList();
    mImageUrls->reserve(SINGLE_IMAGE_COUNT);

    for(unsigned int i = 0; i < SINGLE_IMAGE_COUNT; ++i)
    {
      ImageCache::UrlStore urlStore;
      urlStore.mTextureId = TextureManager::INVALID_TEXTURE_ID;
      urlStore.mUrl       = imageUrl;
      mImageUrls->push_back(urlStore);
    }
    mFrameCount = SINGLE_IMAGE_COUNT;
  }
}

void AnimatedImageVisual::CreateImageCache()
{
  DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "AnimatedImageVisual::CreateImageCache()  batchSize:%d  cacheSize:%d\n", mBatchSize, mCacheSize);

  TextureManager& textureManager = mFactoryCache.GetTextureManager();

  if(mAnimatedImageLoading)
  {
    mImageCache = new RollingAnimatedImageCache(textureManager, mDesiredSize, mFittingMode, mSamplingMode, mAnimatedImageLoading, mMaskingData, *this, mCacheSize, mBatchSize, mWrapModeU, mWrapModeV, IsSynchronousLoadingRequired(), IsPreMultipliedAlphaEnabled());
  }
  else if(mImageUrls)
  {
    // Ensure the batch size and cache size are no bigger than the number of URLs,
    // and that the cache is at least as big as the batch size.
    uint16_t numUrls   = mImageUrls->size();
    uint16_t batchSize = std::max(std::min(mBatchSize, numUrls), MINIMUM_CACHESIZE);
    uint16_t cacheSize = std::max(std::min(std::max(batchSize, mCacheSize), numUrls), MINIMUM_CACHESIZE);
    if(cacheSize < numUrls)
    {
      mImageCache = new RollingImageCache(textureManager, mDesiredSize, mFittingMode, mSamplingMode, *mImageUrls, mMaskingData, *this, cacheSize, batchSize, mFrameDelay, IsPreMultipliedAlphaEnabled());
    }
    else
    {
      mImageCache = new FixedImageCache(textureManager, mDesiredSize, mFittingMode, mSamplingMode, *mImageUrls, mMaskingData, *this, batchSize, mFrameDelay, IsPreMultipliedAlphaEnabled());
    }
  }

  if(DALI_UNLIKELY(!mImageCache))
  {
    DALI_LOG_ERROR("mImageCache is null\n");
  }
}

AnimatedImageVisual::AnimatedImageVisual(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, ImageDimensions desiredSize)
: Visual::Base(factoryCache, Visual::FittingMode::DONT_CARE, Toolkit::Visual::ANIMATED_IMAGE),
  mFrameDelayTimer(),
  mPlacementActor(),
  mImageVisualShaderFactory(shaderFactory),
  mPixelArea(FULL_TEXTURE_RECT),
  mPixelAreaIndex(Property::INVALID_INDEX),
  mPreMultipliedAlphaIndex(Property::INVALID_INDEX),
  mImageUrl(),
  mAnimatedImageLoading(),
  mFrameIndexForJumpTo(0),
  mCurrentFrameIndex(FIRST_FRAME_INDEX),
  mImageUrls(NULL),
  mImageCache(NULL),
  mCacheSize(2),
  mBatchSize(2),
  mFrameDelay(100),
  mLoopCount(LOOP_FOREVER),
  mCurrentLoopIndex(FIRST_LOOP),
  mLoadPolicy(Toolkit::ImageVisual::LoadPolicy::ATTACHED),
  mReleasePolicy(Toolkit::ImageVisual::ReleasePolicy::DETACHED),
  mMaskingData(),
  mDesiredSize(desiredSize),
  mFrameSpeedFactor(1.0f),
  mFrameCount(0),
  mImageSize(),
  mActionStatus(DevelAnimatedImageVisual::Action::PLAY),
  mWrapModeU(WrapMode::DEFAULT),
  mWrapModeV(WrapMode::DEFAULT),
  mStopBehavior(DevelImageVisual::StopBehavior::CURRENT_FRAME),
  mFittingMode(FittingMode::VISUAL_FITTING),
  mSamplingMode(SamplingMode::BOX_THEN_LINEAR),
  mStartFirstFrame(false),
  mIsJumpTo(false)
{
  EnablePreMultipliedAlpha(mFactoryCache.GetPreMultiplyOnLoad());
}

AnimatedImageVisual::~AnimatedImageVisual()
{
  if(DALI_LIKELY(mImageCache))
  {
    // AnimatedImageVisual destroyed so remove texture unless ReleasePolicy is set to never release
    // If this is animated image, clear cache always.
    // Else if this is single frame image, this is affected be release policy.
    if(mFrameCount > SINGLE_IMAGE_COUNT || mReleasePolicy != Toolkit::ImageVisual::ReleasePolicy::NEVER)
    {
      mImageCache->ClearCache();
    }
  }
  delete mImageCache;
  delete mImageUrls;
}

void AnimatedImageVisual::GetNaturalSize(Vector2& naturalSize)
{
  if(mDesiredSize.GetWidth() > 0 && mDesiredSize.GetHeight() > 0)
  {
    if(mImpl->mRenderer)
    {
      auto textureSet = mImpl->mRenderer.GetTextures();
      if(textureSet && textureSet.GetTextureCount())
      {
        auto texture = textureSet.GetTexture(0);
        if(texture)
        {
          Dali::Vector2 textureSize;
          textureSize.x = texture.GetWidth();
          textureSize.y = texture.GetHeight();
          if(textureSize != Vector2::ZERO)
          {
            naturalSize = textureSize;
            return;
          }
        }
      }
    }

    naturalSize.x = mDesiredSize.GetWidth();
    naturalSize.y = mDesiredSize.GetHeight();
    return;
  }

  naturalSize = Vector2::ZERO;
  if(mImageSize.GetWidth() == 0 && mImageSize.GetHeight() == 0)
  {
    if(mMaskingData && mMaskingData->mAlphaMaskUrl.IsValid() &&
       mMaskingData->mCropToMask)
    {
      ImageDimensions dimensions = Dali::GetClosestImageSize(mMaskingData->mAlphaMaskUrl.GetUrl());
      if(dimensions != ImageDimensions(0, 0))
      {
        mImageSize    = dimensions;
        naturalSize.x = dimensions.GetWidth();
        naturalSize.y = dimensions.GetHeight();
        return;
      }
    }

    if(mImageUrl.IsValid() && mAnimatedImageLoading)
    {
      mImageSize = mAnimatedImageLoading.GetImageSize();
    }
    else if(mImageUrls && mImageUrls->size() > 0)
    {
      mImageSize = Dali::GetClosestImageSize((*mImageUrls)[0].mUrl.GetUrl());
    }
  }

  naturalSize.width  = mImageSize.GetWidth();
  naturalSize.height = mImageSize.GetHeight();
}

void AnimatedImageVisual::DoCreatePropertyMap(Property::Map& map) const
{
  map.Clear();

  bool sync = IsSynchronousLoadingRequired();
  map.Insert(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, sync);

  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::ANIMATED_IMAGE);

  if(mImageUrl.IsValid())
  {
    map.Insert(Toolkit::ImageVisual::Property::URL, mImageUrl.GetUrl());
  }
  if(mImageUrls != NULL && !mImageUrls->empty())
  {
    Property::Array urls;
    for(unsigned int i = 0; i < mImageUrls->size(); ++i)
    {
      urls.Add((*mImageUrls)[i].mUrl.GetUrl());
    }
    Property::Value value(const_cast<Property::Array&>(urls));
    map.Insert(Toolkit::ImageVisual::Property::URL, value);
  }

  if(mImpl->mRenderer && mPixelAreaIndex != Property::INVALID_INDEX)
  {
    // Update values from Renderer
    Vector4 pixelArea = mImpl->mRenderer.GetProperty<Vector4>(mPixelAreaIndex);
    map.Insert(Toolkit::ImageVisual::Property::PIXEL_AREA, pixelArea);
  }
  else
  {
    map.Insert(Toolkit::ImageVisual::Property::PIXEL_AREA, mPixelArea);
  }

  map.Insert(Toolkit::ImageVisual::Property::WRAP_MODE_U, mWrapModeU);
  map.Insert(Toolkit::ImageVisual::Property::WRAP_MODE_V, mWrapModeV);

  map.Insert(Toolkit::ImageVisual::Property::BATCH_SIZE, static_cast<int>(mBatchSize));
  map.Insert(Toolkit::ImageVisual::Property::CACHE_SIZE, static_cast<int>(mCacheSize));
  map.Insert(Toolkit::ImageVisual::Property::FRAME_DELAY, static_cast<int>(mFrameDelay));
  map.Insert(Toolkit::DevelImageVisual::Property::LOOP_COUNT, static_cast<int>(mLoopCount));
  map.Insert(Toolkit::DevelImageVisual::Property::CURRENT_FRAME_NUMBER, (mImageCache) ? static_cast<int32_t>(mImageCache->GetCurrentFrameIndex()) : -1);

  // This returns -1 until the loading is finished.
  auto frameCount = int32_t(mFrameCount);
  if(mImageCache && frameCount == 0)
  {
    frameCount = mImageCache->GetTotalFrameCount();

    if(frameCount <= int32_t(SINGLE_IMAGE_COUNT) && mAnimatedImageLoading && mAnimatedImageLoading.HasLoadingSucceeded())
    {
      frameCount = int32_t(mAnimatedImageLoading.GetImageCount());
    }
    else
    {
      frameCount = -1;
    }
  }

  map.Insert(Toolkit::DevelImageVisual::Property::TOTAL_FRAME_NUMBER, static_cast<int>(frameCount));

  map.Insert(Toolkit::DevelImageVisual::Property::STOP_BEHAVIOR, mStopBehavior);

  if(mMaskingData != nullptr)
  {
    map.Insert(Toolkit::ImageVisual::Property::ALPHA_MASK_URL, mMaskingData->mAlphaMaskUrl.GetUrl());
    map.Insert(Toolkit::ImageVisual::Property::MASK_CONTENT_SCALE, mMaskingData->mContentScaleFactor);
    map.Insert(Toolkit::ImageVisual::Property::CROP_TO_MASK, mMaskingData->mCropToMask);
    map.Insert(Toolkit::DevelImageVisual::Property::MASKING_TYPE, mMaskingData->mPreappliedMasking ? DevelImageVisual::MaskingType::MASKING_ON_LOADING : DevelImageVisual::MaskingType::MASKING_ON_RENDERING);
  }

  map.Insert(Toolkit::ImageVisual::Property::LOAD_POLICY, mLoadPolicy);
  map.Insert(Toolkit::ImageVisual::Property::RELEASE_POLICY, mReleasePolicy);
  map.Insert(Toolkit::ImageVisual::Property::FITTING_MODE, mFittingMode);
  map.Insert(Toolkit::ImageVisual::Property::SAMPLING_MODE, mSamplingMode);
  map.Insert(Toolkit::ImageVisual::Property::DESIRED_WIDTH, mDesiredSize.GetWidth());
  map.Insert(Toolkit::ImageVisual::Property::DESIRED_HEIGHT, mDesiredSize.GetHeight());
  map.Insert(Toolkit::DevelImageVisual::Property::FRAME_SPEED_FACTOR, mFrameSpeedFactor);
}

void AnimatedImageVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::ANIMATED_IMAGE);
  map.Insert(Toolkit::ImageVisual::Property::DESIRED_WIDTH, mDesiredSize.GetWidth());
  map.Insert(Toolkit::ImageVisual::Property::DESIRED_HEIGHT, mDesiredSize.GetHeight());
}

void AnimatedImageVisual::EnablePreMultipliedAlpha(bool preMultiplied)
{
  if(mImpl->mRenderer)
  {
    if(mPreMultipliedAlphaIndex != Property::INVALID_INDEX)
    {
      mImpl->mRenderer.SetProperty(mPreMultipliedAlphaIndex, preMultiplied ? 1.0f : 0.0f);
    }
    else if(!preMultiplied)
    {
      // Register PREMULTIPLIED_ALPHA only if it become false.
      // Default PREMULTIPLIED_ALPHA value is 1.0f, at image-visual-shader-factory.cpp
      mPreMultipliedAlphaIndex = mImpl->mRenderer.RegisterProperty(Toolkit::Visual::Property::PREMULTIPLIED_ALPHA, PREMULTIPLIED_ALPHA, 0.0f);
    }
  }

  Visual::Base::EnablePreMultipliedAlpha(preMultiplied);
}

void AnimatedImageVisual::OnDoAction(const Dali::Property::Index actionId, const Dali::Property::Value& attributes)
{
  // Make not set any action when the resource status is already failed.
  if(mImpl->mResourceStatus == Toolkit::Visual::ResourceStatus::FAILED)
  {
    return;
  }

  // Check if action is valid for this visual type and perform action if possible
  switch(actionId)
  {
    case DevelAnimatedImageVisual::Action::PAUSE:
    {
      // Pause will be executed on next timer tick
      mActionStatus = DevelAnimatedImageVisual::Action::PAUSE;
      break;
    }
    case DevelAnimatedImageVisual::Action::PLAY:
    {
      if(mFrameDelayTimer && IsOnScene() && mActionStatus != DevelAnimatedImageVisual::Action::PLAY)
      {
        mFrameDelayTimer.Start();
      }
      mActionStatus = DevelAnimatedImageVisual::Action::PLAY;
      break;
    }
    case DevelAnimatedImageVisual::Action::STOP:
    {
      // STOP reset functionality will actually be done in a future change
      // Stop will be executed on next timer tick
      mActionStatus     = DevelAnimatedImageVisual::Action::STOP;
      mCurrentLoopIndex = FIRST_LOOP;
      if(IsOnScene())
      {
        DisplayNextFrame();
      }
      break;
    }
    case DevelAnimatedImageVisual::Action::JUMP_TO:
    {
      int32_t frameNumber;
      if(attributes.Get(frameNumber))
      {
        if(frameNumber < 0 || frameNumber >= static_cast<int32_t>(mFrameCount))
        {
          DALI_LOG_ERROR("Invalid frame index used.\n");
        }
        else
        {
          mIsJumpTo            = true;
          mFrameIndexForJumpTo = frameNumber;
          if(IsOnScene())
          {
            DisplayNextFrame();
          }
        }
      }
      break;
    }
  }
}

void AnimatedImageVisual::DoSetProperties(const Property::Map& propertyMap)
{
  // url[s] already passed in from constructor
  for(Property::Map::SizeType iter = 0; iter < propertyMap.Count(); ++iter)
  {
    KeyValuePair keyValue = propertyMap.GetKeyValue(iter);
    if(keyValue.first.type == Property::Key::INDEX)
    {
      DoSetProperty(keyValue.first.indexKey, keyValue.second);
    }
    else
    {
      if(keyValue.first == PIXEL_AREA_UNIFORM_NAME)
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
      else if(keyValue.first == BATCH_SIZE_NAME)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::BATCH_SIZE, keyValue.second);
      }
      else if(keyValue.first == CACHE_SIZE_NAME)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::CACHE_SIZE, keyValue.second);
      }
      else if(keyValue.first == FRAME_DELAY_NAME)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::FRAME_DELAY, keyValue.second);
      }
      else if(keyValue.first == LOOP_COUNT_NAME)
      {
        DoSetProperty(Toolkit::DevelImageVisual::Property::LOOP_COUNT, keyValue.second);
      }
      else if(keyValue.first == STOP_BEHAVIOR_NAME)
      {
        DoSetProperty(Toolkit::DevelImageVisual::Property::STOP_BEHAVIOR, keyValue.second);
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
      else if(keyValue.first == SYNCHRONOUS_LOADING)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, keyValue.second);
      }
      else if(keyValue.first == IMAGE_FITTING_MODE)
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
      else if(keyValue.first == FRAME_SPEED_FACTOR)
      {
        DoSetProperty(Toolkit::DevelImageVisual::Property::FRAME_SPEED_FACTOR, keyValue.second);
      }
    }
  }
  // Load image immediately if LOAD_POLICY requires it
  if(mLoadPolicy == Toolkit::ImageVisual::LoadPolicy::IMMEDIATE)
  {
    PrepareTextureSet();
  }
}

void AnimatedImageVisual::DoSetProperty(Property::Index        index,
                                        const Property::Value& value)
{
  switch(index)
  {
    case Toolkit::ImageVisual::Property::PIXEL_AREA:
    {
      value.Get(mPixelArea);

      if(DALI_UNLIKELY(mImpl->mRenderer))
      {
        // Unusual case. SetProperty called after OnInitialize().
        // Assume that DoAction call UPDATE_PROPERTY.
        if(mPixelAreaIndex != Property::INVALID_INDEX)
        {
          mImpl->mRenderer.SetProperty(mPixelAreaIndex, mPixelArea);
        }
        else if(mPixelArea != FULL_TEXTURE_RECT)
        {
          mPixelAreaIndex = mImpl->mRenderer.RegisterProperty(Toolkit::ImageVisual::Property::PIXEL_AREA, PIXEL_AREA_UNIFORM_NAME, mPixelArea);
        }
      }
      break;
    }
    case Toolkit::ImageVisual::Property::WRAP_MODE_U:
    {
      int wrapMode = 0;
      if(Scripting::GetEnumerationProperty(value, WRAP_MODE_TABLE, WRAP_MODE_TABLE_COUNT, wrapMode))
      {
        mWrapModeU = Dali::WrapMode::Type(wrapMode);
      }
      else
      {
        mWrapModeU = Dali::WrapMode::Type::DEFAULT;
      }
      break;
    }
    case Toolkit::ImageVisual::Property::WRAP_MODE_V:
    {
      int wrapMode = 0;
      if(Scripting::GetEnumerationProperty(value, WRAP_MODE_TABLE, WRAP_MODE_TABLE_COUNT, wrapMode))
      {
        mWrapModeV = Dali::WrapMode::Type(wrapMode);
      }
      else
      {
        mWrapModeV = Dali::WrapMode::Type::DEFAULT;
      }
      break;
    }

    case Toolkit::ImageVisual::Property::BATCH_SIZE:
    {
      int batchSize;
      if(value.Get(batchSize))
      {
        if(batchSize < 2)
        {
          DALI_LOG_ERROR("The minimum value of batch size is 2.");
        }
        else
        {
          mBatchSize = batchSize;
        }
      }
      break;
    }

    case Toolkit::ImageVisual::Property::CACHE_SIZE:
    {
      int cacheSize;
      if(value.Get(cacheSize))
      {
        if(cacheSize < 2)
        {
          DALI_LOG_ERROR("The minimum value of cache size is 2.");
        }
        else
        {
          mCacheSize = cacheSize;
        }
      }
      break;
    }

    case Toolkit::ImageVisual::Property::FRAME_DELAY:
    {
      int frameDelay;
      if(value.Get(frameDelay))
      {
        mFrameDelay = frameDelay;
        if(DALI_LIKELY(mImageCache))
        {
          mImageCache->SetInterval(CalculateInterval(mFrameDelay, mFrameSpeedFactor));
        }
      }
      break;
    }

    case Toolkit::DevelImageVisual::Property::LOOP_COUNT:
    {
      int loopCount;
      if(value.Get(loopCount))
      {
        mLoopCount = loopCount;
      }
      break;
    }

    case Toolkit::DevelImageVisual::Property::STOP_BEHAVIOR:
    {
      int32_t stopBehavior = mStopBehavior;
      if(Scripting::GetEnumerationProperty(value, STOP_BEHAVIOR_TABLE, STOP_BEHAVIOR_TABLE_COUNT, stopBehavior))
      {
        mStopBehavior = DevelImageVisual::StopBehavior::Type(stopBehavior);
      }
      break;
    }

    case Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING:
    {
      bool sync = false;
      value.Get(sync);
      if(sync)
      {
        mImpl->mFlags |= Visual::Base::Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
      }
      else
      {
        mImpl->mFlags &= ~Visual::Base::Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
      }
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
        mMaskingData->mPreappliedMasking = Toolkit::DevelImageVisual::MaskingType::Type(maskingType) == Toolkit::DevelImageVisual::MaskingType::MASKING_ON_LOADING ? true : false;
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

    case Toolkit::ImageVisual::Property::DESIRED_WIDTH:
    {
      float desiredWidth = 0.0f;
      if(value.Get(desiredWidth))
      {
        mDesiredSize.SetWidth(desiredWidth);
      }
      else
      {
        DALI_LOG_ERROR("AnimatedImageVisual: desiredWidth property has incorrect type\n");
      }
      break;
    }

    case Toolkit::ImageVisual::Property::DESIRED_HEIGHT:
    {
      float desiredHeight = 0.0f;
      if(value.Get(desiredHeight))
      {
        mDesiredSize.SetHeight(desiredHeight);
      }
      else
      {
        DALI_LOG_ERROR("AnimatedImageVisual: desiredHeight property has incorrect type\n");
      }
      break;
    }

    case Toolkit::DevelImageVisual::Property::FRAME_SPEED_FACTOR:
    {
      float frameSpeedFactor = 1.0f;
      if(value.Get(frameSpeedFactor))
      {
        // TODO : Could we remove this limitation?
        Dali::ClampInPlace(frameSpeedFactor, MINIMUM_FRAME_SPEED_FACTOR, MAXIMUM_FRAME_SPEED_FACTOR);

        if(!Dali::Equals(mFrameSpeedFactor, frameSpeedFactor))
        {
          mFrameSpeedFactor = frameSpeedFactor;
        }
      }
      break;
    }
  }
}

void AnimatedImageVisual::DoSetOnScene(Actor& actor)
{
  mStartFirstFrame = true;
  mPlacementActor  = actor;
  actor.InheritedVisibilityChangedSignal().Connect(this, &AnimatedImageVisual::OnControlInheritedVisibilityChanged);

  // We should clear cached informations before mImageCache->FirstFrame();
  // TODO : Could we remove this cache clearing code?
  if(mReleasePolicy != Toolkit::ImageVisual::ReleasePolicy::DETACHED)
  {
    if(DALI_LIKELY(mImageCache))
    {
      mImageCache->ClearCache(); // If INVALID_TEXTURE_ID then removal will be attempted on atlas
    }
    mImpl->mResourceStatus = Toolkit::Visual::ResourceStatus::PREPARING;

    TextureSet textureSet = TextureSet::New();
    mImpl->mRenderer.SetTextures(textureSet);
  }

  PrepareTextureSet();
}

void AnimatedImageVisual::DoSetOffScene(Actor& actor)
{
  DALI_ASSERT_DEBUG((bool)mImpl->mRenderer && "There should always be a renderer whilst on stage");

  if(mFrameDelayTimer)
  {
    mFrameDelayTimer.Stop();
    mFrameDelayTimer.Reset();
  }

  actor.RemoveRenderer(mImpl->mRenderer);
  if(mReleasePolicy == Toolkit::ImageVisual::ReleasePolicy::DETACHED)
  {
    if(DALI_LIKELY(mImageCache))
    {
      mImageCache->ClearCache(); // If INVALID_TEXTURE_ID then removal will be attempted on atlas
    }
    mImpl->mResourceStatus = Toolkit::Visual::ResourceStatus::PREPARING;

    TextureSet textureSet = TextureSet::New();
    mImpl->mRenderer.SetTextures(textureSet);
  }

  mPlacementActor.Reset();
  mStartFirstFrame   = false;
  mCurrentFrameIndex = FIRST_FRAME_INDEX;
  mCurrentLoopIndex  = FIRST_LOOP;

  actor.InheritedVisibilityChangedSignal().Disconnect(this, &AnimatedImageVisual::OnControlInheritedVisibilityChanged);
}

void AnimatedImageVisual::OnSetTransform()
{
  if(mImpl->mRenderer)
  {
    mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
  }
}

void AnimatedImageVisual::UpdateShader()
{
  if(mImpl->mRenderer)
  {
    Shader shader = GenerateShader();
    mImpl->mRenderer.SetShader(shader);
  }
}

Shader AnimatedImageVisual::GenerateShader() const
{
  Shader shader;
  if(mImpl->mCustomShader)
  {
    shader = Shader::New(mImpl->mCustomShader->mVertexShader.empty() ? mImageVisualShaderFactory.GetVertexShaderSource().data() : mImpl->mCustomShader->mVertexShader,
                         mImpl->mCustomShader->mFragmentShader.empty() ? mImageVisualShaderFactory.GetFragmentShaderSource().data() : mImpl->mCustomShader->mFragmentShader,
                         mImpl->mCustomShader->mHints);

    shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);

    // Most of image visual shader user (like svg, animated vector image visual) use pre-multiplied alpha.
    // If the visual dont want to using pre-multiplied alpha, it should be set as 0.0f as renderer side.
    shader.RegisterProperty(PREMULTIPLIED_ALPHA, ALPHA_VALUE_PREMULTIPLIED);
  }
  else
  {
    bool defaultWrapMode                 = mWrapModeU <= WrapMode::CLAMP_TO_EDGE && mWrapModeV <= WrapMode::CLAMP_TO_EDGE;
    bool requiredAlphaMaskingOnRendering = (mMaskingData && !mMaskingData->mMaskImageLoadingFailed) ? !mMaskingData->mPreappliedMasking : false;

    shader = mImageVisualShaderFactory.GetShader(
      mFactoryCache,
      ImageVisualShaderFeature::FeatureBuilder()
        .ApplyDefaultTextureWrapMode(defaultWrapMode)
        .EnableRoundedCorner(IsRoundedCornerRequired(), IsSquircleCornerRequired())
        .EnableBorderline(IsBorderlineRequired())
        .EnableAlphaMaskingOnRendering(requiredAlphaMaskingOnRendering));
  }
  return shader;
}

Dali::Property AnimatedImageVisual::OnGetPropertyObject(Dali::Property::Key key)
{
  if((key.type == Property::Key::INDEX && key.indexKey == Toolkit::ImageVisual::Property::PIXEL_AREA) ||
     (key.type == Property::Key::STRING && key.stringKey == PIXEL_AREA_UNIFORM_NAME))
  {
    if(DALI_LIKELY(mImpl->mRenderer))
    {
      if(mPixelAreaIndex == Property::INVALID_INDEX)
      {
        mPixelAreaIndex = mImpl->mRenderer.RegisterProperty(Toolkit::ImageVisual::Property::PIXEL_AREA, PIXEL_AREA_UNIFORM_NAME, mPixelArea);
      }
      return Dali::Property(mImpl->mRenderer, mPixelAreaIndex);
    }
  }

  Handle handle;
  return Dali::Property(handle, Property::INVALID_INDEX);
}

void AnimatedImageVisual::OnInitialize()
{
  CreateImageCache();

  bool   defaultWrapMode = mWrapModeU <= WrapMode::CLAMP_TO_EDGE && mWrapModeV <= WrapMode::CLAMP_TO_EDGE;
  Shader shader          = GenerateShader();

  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);

  mImpl->mRenderer = DecoratedVisualRenderer::New(geometry, shader);
  mImpl->mRenderer.ReserveCustomProperties(CUSTOM_PROPERTY_COUNT);

  // Register transform properties
  mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);

  if(!defaultWrapMode) // custom wrap mode
  {
    Vector2 wrapMode(mWrapModeU - WrapMode::CLAMP_TO_EDGE, mWrapModeV - WrapMode::CLAMP_TO_EDGE);
    wrapMode.Clamp(Vector2::ZERO, Vector2(2.f, 2.f));
    mImpl->mRenderer.RegisterUniqueProperty(WRAP_MODE_UNIFORM_NAME, wrapMode);
  }

  if(mPixelArea != FULL_TEXTURE_RECT)
  {
    mPixelAreaIndex = mImpl->mRenderer.RegisterUniqueProperty(Toolkit::ImageVisual::Property::PIXEL_AREA, PIXEL_AREA_UNIFORM_NAME, mPixelArea);
  }

  if(mMaskingData)
  {
    mImpl->mRenderer.RegisterUniqueProperty(Toolkit::ImageVisual::Property::CROP_TO_MASK, CROP_TO_MASK_NAME, static_cast<float>(mMaskingData->mCropToMask));
  }

  // Enable PreMultipliedAlpha if it need.
  auto preMultiplyOnLoad = IsPreMultipliedAlphaEnabled() && !mImpl->mCustomShader
                             ? TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD
                             : TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  EnablePreMultipliedAlpha(preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD);
}

void AnimatedImageVisual::StartFirstFrame(TextureSet& textureSet, uint32_t firstInterval)
{
  DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "AnimatedImageVisual::StartFirstFrame()\n");

  mStartFirstFrame = false;
  if(mImpl->mRenderer)
  {
    mImpl->mRenderer.SetTextures(textureSet);
    CheckMaskTexture();

    Actor actor = mPlacementActor.GetHandle();
    if(actor)
    {
      actor.AddRenderer(mImpl->mRenderer);
      mPlacementActor.Reset();
    }
  }

  mCurrentFrameIndex = FIRST_FRAME_INDEX;
  if(mImpl->mResourceStatus != Toolkit::Visual::ResourceStatus::FAILED)
  {
    if(mFrameCount > SINGLE_IMAGE_COUNT)
    {
      mFrameDelayTimer = Timer::New(CalculateInterval(firstInterval, mFrameSpeedFactor));
      mFrameDelayTimer.TickSignal().Connect(this, &AnimatedImageVisual::DisplayNextFrame);
      mFrameDelayTimer.Start();
    }

    DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "ResourceReady(ResourceStatus::READY)\n");
    ResourceReady(Toolkit::Visual::ResourceStatus::READY);
  }
}

void AnimatedImageVisual::PrepareTextureSet()
{
  TextureSet textureSet;
  if(DALI_LIKELY(mImageCache))
  {
    textureSet = mImageCache->FirstFrame();
  }
  else
  {
    // preMultiplied should be false because broken image don't premultiply alpha on load
    FrameReady(TextureSet(), 0, false);
  }

  // Check whether synchronous loading is true or false for the first frame.
  if(textureSet)
  {
    SetImageSize(textureSet);
  }
}

void AnimatedImageVisual::SetImageSize(TextureSet& textureSet)
{
  if(DALI_LIKELY(textureSet && textureSet.GetTextureCount() > 0u))
  {
    Texture texture = textureSet.GetTexture(0);
    if(texture)
    {
      mImageSize.SetWidth(texture.GetWidth());
      mImageSize.SetHeight(texture.GetHeight());
    }

    if(textureSet.GetTextureCount() > 1u && mMaskingData && mMaskingData->mCropToMask)
    {
      Texture maskTexture = textureSet.GetTexture(1);
      if(maskTexture)
      {
        mImageSize.SetWidth(std::min(static_cast<uint32_t>(mImageSize.GetWidth() * mMaskingData->mContentScaleFactor), maskTexture.GetWidth()));
        mImageSize.SetHeight(std::min(static_cast<uint32_t>(mImageSize.GetHeight() * mMaskingData->mContentScaleFactor), maskTexture.GetHeight()));

        float   textureWidth  = std::max(static_cast<float>(texture.GetWidth() * mMaskingData->mContentScaleFactor), Dali::Math::MACHINE_EPSILON_1);
        float   textureHeight = std::max(static_cast<float>(texture.GetHeight() * mMaskingData->mContentScaleFactor), Dali::Math::MACHINE_EPSILON_1);
        Vector2 textureRatio(std::min(static_cast<float>(maskTexture.GetWidth()), textureWidth) / textureWidth,
                             std::min(static_cast<float>(maskTexture.GetHeight()), textureHeight) / textureHeight);
        mImpl->mRenderer.RegisterProperty(MASK_TEXTURE_RATIO_NAME, textureRatio);
      }
    }
  }
}

void AnimatedImageVisual::FrameReady(TextureSet textureSet, uint32_t interval, bool preMultiplied)
{
  EnablePreMultipliedAlpha(preMultiplied);

  // When image visual requested to load new frame to mImageCache and it is failed.
  if(!mImageCache || !textureSet)
  {
    textureSet = SetLoadingFailed();
  }
  SetImageSize(textureSet);

  if(mStartFirstFrame)
  {
    if(DALI_LIKELY(mImageCache))
    {
      mFrameCount = mImageCache->GetTotalFrameCount();
    }
    StartFirstFrame(textureSet, interval);
  }
  else
  {
    if(mImpl->mRenderer)
    {
      if(mFrameDelayTimer && interval > 0u)
      {
        mFrameDelayTimer.SetInterval(CalculateInterval(interval, mFrameSpeedFactor));
      }
      mImpl->mRenderer.SetTextures(textureSet);
      CheckMaskTexture();
    }
  }
}

bool AnimatedImageVisual::DisplayNextFrame()
{
  TextureSet textureSet;
  bool       continueTimer = false;

  if(DALI_LIKELY(mImageCache))
  {
    uint32_t frameIndex = mImageCache->GetCurrentFrameIndex();

    if(mIsJumpTo)
    {
      mIsJumpTo  = false;
      frameIndex = mFrameIndexForJumpTo;
    }
    else if(mActionStatus == DevelAnimatedImageVisual::Action::PAUSE)
    {
      return false;
    }
    else if(mActionStatus == DevelAnimatedImageVisual::Action::STOP)
    {
      mCurrentLoopIndex = FIRST_LOOP;
      if(mStopBehavior == DevelImageVisual::StopBehavior::FIRST_FRAME)
      {
        frameIndex = FIRST_FRAME_INDEX;
      }
      else if(mStopBehavior == DevelImageVisual::StopBehavior::LAST_FRAME)
      {
        frameIndex = mFrameCount - 1;
      }
      else
      {
        return false; // Do not draw already rendered scene twice.
      }
    }
    else
    {
      if(mFrameCount > SINGLE_IMAGE_COUNT)
      {
        frameIndex++;
        if(frameIndex >= mFrameCount)
        {
          frameIndex = FIRST_FRAME_INDEX;
          ++mCurrentLoopIndex;
        }

        if(mLoopCount >= 0 && mCurrentLoopIndex >= mLoopCount)
        {
          // This will stop timer
          mActionStatus = DevelAnimatedImageVisual::Action::STOP;
          return DisplayNextFrame();
        }
      }
    }

    DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "AnimatedImageVisual::DisplayNextFrame(this:%p) CurrentFrameIndex:%d\n", this, frameIndex);

    textureSet = mImageCache->Frame(frameIndex);

    if(textureSet)
    {
      SetImageSize(textureSet);
      if(mImpl->mRenderer)
      {
        mImpl->mRenderer.SetTextures(textureSet);
        CheckMaskTexture();
      }
      if(mFrameDelayTimer)
      {
        mFrameDelayTimer.SetInterval(CalculateInterval(mImageCache->GetFrameInterval(frameIndex), mFrameSpeedFactor));
      }
    }

    mCurrentFrameIndex = frameIndex;
    continueTimer      = (mActionStatus == DevelAnimatedImageVisual::Action::PLAY && textureSet) ? true : false;
  }

  return continueTimer;
}

TextureSet AnimatedImageVisual::SetLoadingFailed()
{
  DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "ResourceReady(ResourceStatus::FAILED)\n");
  ResourceReady(Toolkit::Visual::ResourceStatus::FAILED);

  Actor   actor     = mPlacementActor.GetHandle();
  Vector2 imageSize = Vector2::ZERO;
  if(actor)
  {
    imageSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
  }

  TextureSet textureSet;
  if(DALI_LIKELY(mImpl->mRenderer))
  {
    mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize);
    textureSet = mImpl->mRenderer.GetTextures();
  }

  if(mFrameDelayTimer)
  {
    mFrameDelayTimer.Stop();
    mFrameDelayTimer.Reset();
  }

  SetImageSize(textureSet);

  return textureSet;
}

void AnimatedImageVisual::AllocateMaskData()
{
  if(!mMaskingData)
  {
    mMaskingData.reset(new TextureManager::MaskingData());
  }
}

void AnimatedImageVisual::CheckMaskTexture()
{
  if(mMaskingData && !mMaskingData->mPreappliedMasking)
  {
    bool       maskLoadFailed = true;
    TextureSet textures       = mImpl->mRenderer.GetTextures();
    if(textures && textures.GetTextureCount() >= TEXTURE_COUNT_FOR_GPU_ALPHA_MASK)
    {
      maskLoadFailed = false;
    }
    if(mMaskingData->mMaskImageLoadingFailed != maskLoadFailed)
    {
      mMaskingData->mMaskImageLoadingFailed = maskLoadFailed;
      UpdateShader();
    }
  }
}

void AnimatedImageVisual::OnControlInheritedVisibilityChanged(Actor actor, bool visible)
{
  if(!visible && mActionStatus != DevelAnimatedImageVisual::Action::STOP)
  {
    mActionStatus = DevelAnimatedImageVisual::Action::STOP;
    DisplayNextFrame();
    DALI_LOG_INFO(gAnimImgLogFilter, Debug::Verbose, "AnimatedImageVisual::OnControlInheritedVisibilityChanged: invisibile. Pause animation [%p]\n", this);
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
