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
 *
 */

// CLASS HEADER
#include "texture-manager-impl.h"

// EXTERNAL HEADERS
#include <dali/devel-api/adaptor-framework/environment-variable.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/rendering/geometry.h>

// INTERNAL HEADERS
#include <dali-toolkit/internal/texture-manager/texture-async-loading-helper.h>
#include <dali-toolkit/internal/texture-manager/texture-cache-manager.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/rendering-addon.h>

namespace
{
constexpr auto INITIAL_HASH_NUMBER                     = size_t{0u};
constexpr auto DEFAULT_NUMBER_OF_LOCAL_LOADER_THREADS  = size_t{4u};
constexpr auto DEFAULT_NUMBER_OF_REMOTE_LOADER_THREADS = size_t{8u};

constexpr auto NUMBER_OF_LOCAL_LOADER_THREADS_ENV  = "DALI_TEXTURE_LOCAL_THREADS";
constexpr auto NUMBER_OF_REMOTE_LOADER_THREADS_ENV = "DALI_TEXTURE_REMOTE_THREADS";

size_t GetNumberOfThreads(const char* environmentVariable, size_t defaultValue)
{
  using Dali::EnvironmentVariable::GetEnvironmentVariable;
  auto           numberString          = GetEnvironmentVariable(environmentVariable);
  auto           numberOfThreads       = numberString ? std::strtoul(numberString, nullptr, 10) : 0;
  constexpr auto MAX_NUMBER_OF_THREADS = 100u;
  DALI_ASSERT_DEBUG(numberOfThreads < MAX_NUMBER_OF_THREADS);
  return (numberOfThreads > 0 && numberOfThreads < MAX_NUMBER_OF_THREADS) ? numberOfThreads : defaultValue;
}

size_t GetNumberOfLocalLoaderThreads()
{
  return GetNumberOfThreads(NUMBER_OF_LOCAL_LOADER_THREADS_ENV, DEFAULT_NUMBER_OF_LOCAL_LOADER_THREADS);
}

size_t GetNumberOfRemoteLoaderThreads()
{
  return GetNumberOfThreads(NUMBER_OF_REMOTE_LOADER_THREADS_ENV, DEFAULT_NUMBER_OF_REMOTE_LOADER_THREADS);
}

} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
#ifdef DEBUG_ENABLED
// Define logfilter Internal namespace level so other files (e.g. texture-cache-manager.cpp) can also use this filter.
Debug::Filter* gTextureManagerLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_TEXTURE_MANAGER");

// clang-format off
#define GET_LOAD_STATE_STRING(loadState) \
  loadState == TextureManagerType::LoadState::NOT_STARTED      ? "NOT_STARTED"      : \
  loadState == TextureManagerType::LoadState::LOADING          ? "LOADING"          : \
  loadState == TextureManagerType::LoadState::LOAD_FINISHED    ? "LOAD_FINISHED"    : \
  loadState == TextureManagerType::LoadState::WAITING_FOR_MASK ? "WAITING_FOR_MASK" : \
  loadState == TextureManagerType::LoadState::MASK_APPLYING    ? "MASK_APPLYING"    : \
  loadState == TextureManagerType::LoadState::MASK_APPLIED     ? "MASK_APPLIED"     : \
  loadState == TextureManagerType::LoadState::UPLOADED         ? "UPLOADED"         : \
  loadState == TextureManagerType::LoadState::CANCELLED        ? "CANCELLED"        : \
  loadState == TextureManagerType::LoadState::LOAD_FAILED      ? "LOAD_FAILED"      : \
                                                                 "Unknown"
// clang-format on
#endif

namespace
{
const uint32_t DEFAULT_ATLAS_SIZE(1024u);               ///< This size can fit 8 by 8 images of average size 128 * 128
const Vector4  FULL_ATLAS_RECT(0.0f, 0.0f, 1.0f, 1.0f); ///< UV Rectangle that covers the full Texture

void PreMultiply(Devel::PixelBuffer pixelBuffer, TextureManager::MultiplyOnLoad& preMultiplyOnLoad)
{
  if(Pixel::HasAlpha(pixelBuffer.GetPixelFormat()))
  {
    if(preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD)
    {
      pixelBuffer.MultiplyColorByAlpha();
    }
  }
  else
  {
    preMultiplyOnLoad = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  }
}

} // Anonymous namespace

TextureManager::MaskingData::MaskingData()
: mAlphaMaskUrl(),
  mAlphaMaskId(INVALID_TEXTURE_ID),
  mContentScaleFactor(1.0f),
  mCropToMask(true)
{
}

TextureManager::TextureManager()
: mTextureCacheManager(),
  mAsyncLocalLoaders(GetNumberOfLocalLoaderThreads(), [&]() { return TextureAsyncLoadingHelper(*this); }),
  mAsyncRemoteLoaders(GetNumberOfRemoteLoaderThreads(), [&]() { return TextureAsyncLoadingHelper(*this); }),
  mLifecycleObservers(),
  mLoadQueue(),
  mQueueLoadFlag(false)
{
  // Initialize the AddOn
  RenderingAddOn::Get();
}

TextureManager::~TextureManager()
{
  for(auto iter = mLifecycleObservers.Begin(), endIter = mLifecycleObservers.End(); iter != endIter; ++iter)
  {
    (*iter)->TextureManagerDestroyed();
  }
}

TextureSet TextureManager::LoadAnimatedImageTexture(
  Dali::AnimatedImageLoading      animatedImageLoading,
  const std::uint32_t&            frameIndex,
  const Dali::SamplingMode::Type& samplingMode,
  const bool&                     synchronousLoading,
  TextureManager::TextureId&      textureId,
  const Dali::WrapMode::Type&     wrapModeU,
  const Dali::WrapMode::Type&     wrapModeV,
  TextureUploadObserver*          textureObserver)
{
  TextureSet textureSet;

  if(synchronousLoading)
  {
    Devel::PixelBuffer pixelBuffer;
    if(animatedImageLoading)
    {
      pixelBuffer = animatedImageLoading.LoadFrame(frameIndex);
    }
    if(!pixelBuffer)
    {
      DALI_LOG_ERROR("TextureManager::LoadAnimatedImageTexture: Synchronous loading is failed\n");
    }
    else
    {
      PixelData pixelData = Devel::PixelBuffer::Convert(pixelBuffer); // takes ownership of buffer
      if(!textureSet)
      {
        Texture texture = Texture::New(Dali::TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight());
        texture.Upload(pixelData);
        textureSet = TextureSet::New();
        textureSet.SetTexture(0u, texture);
      }
    }
  }
  else
  {
    auto preMultiply                    = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
    textureId                           = RequestLoadInternal(animatedImageLoading.GetUrl(), INVALID_TEXTURE_ID, 1.0f, ImageDimensions(), FittingMode::SCALE_TO_FILL, SamplingMode::BOX_THEN_LINEAR, UseAtlas::NO_ATLAS, false, StorageType::UPLOAD_TO_TEXTURE, textureObserver, true, TextureManager::ReloadPolicy::CACHED, preMultiply, animatedImageLoading, frameIndex, false);
    TextureManager::LoadState loadState = mTextureCacheManager.GetTextureStateInternal(textureId);
    if(loadState == TextureManager::LoadState::UPLOADED)
    {
      // LoadComplete has already been called - keep the same texture set
      textureSet = GetTextureSet(textureId);
    }
  }

  if(textureSet)
  {
    Sampler sampler = Sampler::New();
    sampler.SetWrapMode(wrapModeU, wrapModeV);
    textureSet.SetSampler(0u, sampler);
  }

  return textureSet;
}

Devel::PixelBuffer TextureManager::LoadPixelBuffer(
  const VisualUrl&                url,
  const Dali::ImageDimensions&    desiredSize,
  const Dali::FittingMode::Type&  fittingMode,
  const Dali::SamplingMode::Type& samplingMode,
  const bool&                     synchronousLoading,
  TextureUploadObserver*          textureObserver,
  const bool&                     orientationCorrection,
  TextureManager::MultiplyOnLoad& preMultiplyOnLoad)
{
  Devel::PixelBuffer pixelBuffer;
  if(synchronousLoading)
  {
    if(url.IsValid())
    {
      if(url.IsBufferResource())
      {
        const EncodedImageBuffer& encodedImageBuffer = mTextureCacheManager.GetEncodedImageBuffer(url.GetUrl());
        if(encodedImageBuffer)
        {
          pixelBuffer = LoadImageFromBuffer(encodedImageBuffer.GetRawBuffer(), desiredSize, fittingMode, samplingMode, orientationCorrection);
        }
      }
      else
      {
        pixelBuffer = LoadImageFromFile(url.GetUrl(), desiredSize, fittingMode, samplingMode, orientationCorrection);
      }
      if(pixelBuffer && preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD)
      {
        PreMultiply(pixelBuffer, preMultiplyOnLoad);
      }
    }
  }
  else
  {
    RequestLoadInternal(url, INVALID_TEXTURE_ID, 1.0f, desiredSize, fittingMode, samplingMode, UseAtlas::NO_ATLAS, false, StorageType::RETURN_PIXEL_BUFFER, textureObserver, orientationCorrection, TextureManager::ReloadPolicy::FORCED, preMultiplyOnLoad, Dali::AnimatedImageLoading(), 0u, false);
  }

  return pixelBuffer;
}

TextureSet TextureManager::LoadTexture(
  const VisualUrl&                    url,
  const Dali::ImageDimensions&        desiredSize,
  const Dali::FittingMode::Type&      fittingMode,
  const Dali::SamplingMode::Type&     samplingMode,
  MaskingDataPointer&                 maskInfo,
  const bool&                         synchronousLoading,
  TextureManager::TextureId&          textureId,
  Vector4&                            textureRect,
  Dali::ImageDimensions&              textureRectSize,
  bool&                               atlasingStatus,
  bool&                               loadingStatus,
  const Dali::WrapMode::Type&         wrapModeU,
  const Dali::WrapMode::Type&         wrapModeV,
  TextureUploadObserver*              textureObserver,
  AtlasUploadObserver*                atlasObserver,
  ImageAtlasManagerPtr                imageAtlasManager,
  const bool&                         orientationCorrection,
  const TextureManager::ReloadPolicy& reloadPolicy,
  TextureManager::MultiplyOnLoad&     preMultiplyOnLoad)
{
  TextureSet textureSet;

  loadingStatus = false;
  textureRect   = FULL_ATLAS_RECT;

  if(VisualUrl::TEXTURE == url.GetProtocolType())
  {
    std::string location = url.GetLocation();
    if(location.size() > 0u)
    {
      TextureId id = std::stoi(location);
      textureSet   = mTextureCacheManager.GetExternalTextureSet(id);
      if(textureSet)
      {
        preMultiplyOnLoad = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
        textureId         = id;
        return textureSet;
      }
    }
  }
  else
  {
    // For Atlas
    if(synchronousLoading && atlasingStatus && imageAtlasManager->CheckAtlasAvailable(url, desiredSize))
    {
      Devel::PixelBuffer pixelBuffer = LoadImageSynchronously(url, desiredSize, fittingMode, samplingMode, orientationCorrection);

      if(maskInfo && maskInfo->mAlphaMaskUrl.IsValid())
      {
        Devel::PixelBuffer maskPixelBuffer = LoadImageSynchronously(maskInfo->mAlphaMaskUrl.GetUrl(), ImageDimensions(), FittingMode::SCALE_TO_FILL, SamplingMode::NO_FILTER, true);
        if(maskPixelBuffer)
        {
          pixelBuffer.ApplyMask(maskPixelBuffer, maskInfo->mContentScaleFactor, maskInfo->mCropToMask);
        }
      }

      PixelData data;
      if(pixelBuffer)
      {
        PreMultiply(pixelBuffer, preMultiplyOnLoad);
        data = Devel::PixelBuffer::Convert(pixelBuffer); // takes ownership of buffer

        if(data)
        {
          textureSet = imageAtlasManager->Add(textureRect, data);
          if(textureSet)
          {
            textureRectSize.SetWidth(data.GetWidth());
            textureRectSize.SetHeight(data.GetHeight());
          }
        }
        else
        {
          DALI_LOG_ERROR("TextureManager::LoadTexture: Synchronous Texture loading with atlasing is failed.\n");
        }
      }
      if(!textureSet)
      {
        atlasingStatus = false;
      }
    }

    if(!textureSet)
    {
      loadingStatus = true;
      // Atlas manager can chage desired size when it is set by 0,0.
      // We should store into textureRectSize only if atlasing successed.
      // So copy inputed desiredSize, and replace value into textureRectSize only if atlasing success.
      Dali::ImageDimensions atlasDesiredSize = desiredSize;
      if(atlasingStatus)
      {
        textureSet = imageAtlasManager->Add(textureRect, url.GetUrl(), atlasDesiredSize, fittingMode, true, atlasObserver);
      }
      if(!textureSet) // big image, no atlasing or atlasing failed
      {
        atlasingStatus = false;
        if(!maskInfo || !maskInfo->mAlphaMaskUrl.IsValid())
        {
          textureId = RequestLoad(
            url,
            desiredSize,
            fittingMode,
            samplingMode,
            UseAtlas::NO_ATLAS,
            textureObserver,
            orientationCorrection,
            reloadPolicy,
            preMultiplyOnLoad,
            synchronousLoading);
        }
        else
        {
          maskInfo->mAlphaMaskId = RequestMaskLoad(maskInfo->mAlphaMaskUrl, synchronousLoading);
          textureId              = RequestLoad(
            url,
            maskInfo->mAlphaMaskId,
            maskInfo->mContentScaleFactor,
            desiredSize,
            fittingMode,
            samplingMode,
            UseAtlas::NO_ATLAS,
            maskInfo->mCropToMask,
            textureObserver,
            orientationCorrection,
            reloadPolicy,
            preMultiplyOnLoad,
            synchronousLoading);
        }

        TextureManager::LoadState loadState = mTextureCacheManager.GetTextureStateInternal(textureId);
        if(loadState == TextureManager::LoadState::UPLOADED)
        {
          // LoadComplete has already been called - keep the same texture set
          textureSet = mTextureCacheManager.GetTextureSet(textureId);
        }

        // If we are loading the texture, or waiting for the ready signal handler to complete, inform
        // caller that they need to wait.
        loadingStatus = (loadState == TextureManager::LoadState::LOADING ||
                         loadState == TextureManager::LoadState::WAITING_FOR_MASK ||
                         loadState == TextureManager::LoadState::MASK_APPLYING ||
                         loadState == TextureManager::LoadState::MASK_APPLIED ||
                         loadState == TextureManager::LoadState::NOT_STARTED ||
                         mQueueLoadFlag);
      }
      else
      {
        textureRectSize = atlasDesiredSize;
      }
    }
  }

  if(!atlasingStatus && textureSet)
  {
    Sampler sampler = Sampler::New();
    sampler.SetWrapMode(wrapModeU, wrapModeV);
    textureSet.SetSampler(0u, sampler);
  }

  if(synchronousLoading)
  {
    loadingStatus = false;
  }

  return textureSet;
}

TextureManager::TextureId TextureManager::RequestLoad(
  const VisualUrl&                    url,
  const ImageDimensions&              desiredSize,
  const Dali::FittingMode::Type&      fittingMode,
  const Dali::SamplingMode::Type&     samplingMode,
  const UseAtlas&                     useAtlas,
  TextureUploadObserver*              observer,
  const bool&                         orientationCorrection,
  const TextureManager::ReloadPolicy& reloadPolicy,
  TextureManager::MultiplyOnLoad&     preMultiplyOnLoad,
  const bool&                         synchronousLoading)
{
  return RequestLoadInternal(url, INVALID_TEXTURE_ID, 1.0f, desiredSize, fittingMode, samplingMode, useAtlas, false, StorageType::UPLOAD_TO_TEXTURE, observer, orientationCorrection, reloadPolicy, preMultiplyOnLoad, Dali::AnimatedImageLoading(), 0u, synchronousLoading);
}

TextureManager::TextureId TextureManager::RequestLoad(
  const VisualUrl&                    url,
  const TextureManager::TextureId&    maskTextureId,
  const float&                        contentScale,
  const Dali::ImageDimensions&        desiredSize,
  const Dali::FittingMode::Type&      fittingMode,
  const Dali::SamplingMode::Type&     samplingMode,
  const TextureManager::UseAtlas&     useAtlas,
  const bool&                         cropToMask,
  TextureUploadObserver*              observer,
  const bool&                         orientationCorrection,
  const TextureManager::ReloadPolicy& reloadPolicy,
  TextureManager::MultiplyOnLoad&     preMultiplyOnLoad,
  const bool&                         synchronousLoading)
{
  return RequestLoadInternal(url, maskTextureId, contentScale, desiredSize, fittingMode, samplingMode, useAtlas, cropToMask, StorageType::UPLOAD_TO_TEXTURE, observer, orientationCorrection, reloadPolicy, preMultiplyOnLoad, Dali::AnimatedImageLoading(), 0u, synchronousLoading);
}

TextureManager::TextureId TextureManager::RequestMaskLoad(
  const VisualUrl& maskUrl,
  const bool&      synchronousLoading)
{
  // Use the normal load procedure to get the alpha mask.
  auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  return RequestLoadInternal(maskUrl, INVALID_TEXTURE_ID, 1.0f, ImageDimensions(), FittingMode::SCALE_TO_FILL, SamplingMode::NO_FILTER, UseAtlas::NO_ATLAS, false, StorageType::KEEP_PIXEL_BUFFER, NULL, true, TextureManager::ReloadPolicy::CACHED, preMultiply, Dali::AnimatedImageLoading(), 0u, synchronousLoading);
}

TextureManager::TextureId TextureManager::RequestLoadInternal(
  const VisualUrl&                    url,
  const TextureManager::TextureId&    maskTextureId,
  const float&                        contentScale,
  const Dali::ImageDimensions&        desiredSize,
  const Dali::FittingMode::Type&      fittingMode,
  const Dali::SamplingMode::Type&     samplingMode,
  const TextureManager::UseAtlas&     useAtlas,
  const bool&                         cropToMask,
  const TextureManager::StorageType&  storageType,
  TextureUploadObserver*              observer,
  const bool&                         orientationCorrection,
  const TextureManager::ReloadPolicy& reloadPolicy,
  TextureManager::MultiplyOnLoad&     preMultiplyOnLoad,
  Dali::AnimatedImageLoading          animatedImageLoading,
  const std::uint32_t&                frameIndex,
  const bool&                         synchronousLoading)
{
  // First check if the requested Texture is cached.
  bool isAnimatedImage = (animatedImageLoading) ? true : false;

  TextureHash       textureHash = INITIAL_HASH_NUMBER;
  TextureCacheIndex cacheIndex  = INVALID_CACHE_INDEX;
  if(storageType != StorageType::RETURN_PIXEL_BUFFER && !isAnimatedImage)
  {
    textureHash = mTextureCacheManager.GenerateHash(url.GetUrl(), desiredSize, fittingMode, samplingMode, useAtlas, maskTextureId);

    // Look up the texture by hash. Note: The extra parameters are used in case of a hash collision.
    cacheIndex = mTextureCacheManager.FindCachedTexture(textureHash, url.GetUrl(), desiredSize, fittingMode, samplingMode, useAtlas, maskTextureId, preMultiplyOnLoad);
  }

  TextureManager::TextureId textureId = INVALID_TEXTURE_ID;
  // Check if the requested Texture exists in the cache.
  if(cacheIndex != INVALID_CACHE_INDEX)
  {
    if(TextureManager::ReloadPolicy::CACHED == reloadPolicy)
    {
      // Mark this texture being used by another client resource. Forced reload would replace the current texture
      // without the need for incrementing the reference count.
      ++(mTextureCacheManager[cacheIndex].referenceCount);
    }
    textureId = mTextureCacheManager[cacheIndex].textureId;

    // Update preMultiplyOnLoad value. It should be changed according to preMultiplied value of the cached info.
    preMultiplyOnLoad = mTextureCacheManager[cacheIndex].preMultiplied ? TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD : TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::General, "TextureManager::RequestLoad( url=%s observer=%p ) Using cached texture id@%d, textureId=%d\n", url.GetUrl().c_str(), observer, cacheIndex, textureId);
  }

  if(textureId == INVALID_TEXTURE_ID) // There was no caching, or caching not required
  {
    if(VisualUrl::BUFFER == url.GetProtocolType())
    {
      std::string location = url.GetLocation();
      if(location.size() > 0u)
      {
        TextureId                 targetId           = std::stoi(location);
        const EncodedImageBuffer& encodedImageBuffer = mTextureCacheManager.GetEncodedImageBuffer(targetId);
        if(encodedImageBuffer)
        {
          textureId = targetId;

          // Increase EncodedImageBuffer reference during it contains mTextureInfoContainer.
          // TODO! We should change action when reload policy is FORCE.
          // Eunki Hong will fix it after refactoring patch merged.
          mTextureCacheManager.UseExternalResource(url.GetUrl());
        }
      }
    }

    if(textureId == INVALID_TEXTURE_ID)
    {
      textureId = mTextureCacheManager.GenerateUniqueTextureId();
    }

    bool preMultiply = (preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD);

    // Cache new texutre, and get cacheIndex.
    cacheIndex = mTextureCacheManager.AppendCache(TextureInfo(textureId, maskTextureId, url, desiredSize, contentScale, fittingMode, samplingMode, false, cropToMask, useAtlas, textureHash, orientationCorrection, preMultiply, animatedImageLoading, frameIndex));

    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::General, "TextureManager::RequestLoad( url=%s observer=%p ) New texture, cacheIndex:%d, textureId=%d\n", url.GetUrl().c_str(), observer, cacheIndex, textureId);
  }

  // The below code path is common whether we are using the cache or not.
  // The textureInfoIndex now refers to either a pre-existing cached TextureInfo,
  // or a new TextureInfo just created.
  TextureInfo& textureInfo(mTextureCacheManager[cacheIndex]);
  textureInfo.maskTextureId         = maskTextureId;
  textureInfo.storageType           = storageType;
  textureInfo.orientationCorrection = orientationCorrection;

  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::General, "TextureInfo loadState:%s\n", GET_LOAD_STATE_STRING(textureInfo.loadState));

  // Force reloading of texture by setting loadState unless already loading or cancelled.
  if(TextureManager::ReloadPolicy::FORCED == reloadPolicy &&
     TextureManager::LoadState::LOADING != textureInfo.loadState &&
     TextureManager::LoadState::WAITING_FOR_MASK != textureInfo.loadState &&
     TextureManager::LoadState::MASK_APPLYING != textureInfo.loadState &&
     TextureManager::LoadState::MASK_APPLIED != textureInfo.loadState &&
     TextureManager::LoadState::CANCELLED != textureInfo.loadState)
  {
    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Verbose, "TextureManager::RequestLoad( url=%s observer=%p ) ForcedReload cacheIndex:%d, textureId=%d\n", url.GetUrl().c_str(), observer, cacheIndex, textureId);

    textureInfo.loadState = TextureManager::LoadState::NOT_STARTED;
  }

  if(!synchronousLoading)
  {
    // Check if we should add the observer.
    // Only do this if we have not loaded yet and it will not have loaded by the end of this method.
    switch(textureInfo.loadState)
    {
      case TextureManager::LoadState::LOAD_FAILED: // Failed notifies observer which then stops observing.
      case TextureManager::LoadState::NOT_STARTED:
      {
        LoadOrQueueTexture(textureInfo, observer); // If called inside NotifyObservers, queues until afterwards
        break;
      }
      case TextureManager::LoadState::LOADING:
      case TextureManager::LoadState::WAITING_FOR_MASK:
      case TextureManager::LoadState::MASK_APPLYING:
      case TextureManager::LoadState::MASK_APPLIED:
      {
        ObserveTexture(textureInfo, observer);
        break;
      }
      case TextureManager::LoadState::UPLOADED:
      {
        if(observer)
        {
          LoadOrQueueTexture(textureInfo, observer);
        }
        break;
      }
      case TextureManager::LoadState::CANCELLED:
      {
        // A cancelled texture hasn't finished loading yet. Treat as a loading texture
        // (it's ref count has already been incremented, above)
        textureInfo.loadState = TextureManager::LoadState::LOADING;
        ObserveTexture(textureInfo, observer);
        break;
      }
      case TextureManager::LoadState::LOAD_FINISHED:
      {
        // Loading has already completed.
        if(observer && textureInfo.storageType == StorageType::RETURN_PIXEL_BUFFER)
        {
          LoadOrQueueTexture(textureInfo, observer);
        }
        break;
      }
    }
  }
  else
  {
    // If the image is already finished to load, use cached texture.
    // We don't need to consider Observer because this is synchronous loading.
    if(textureInfo.loadState == TextureManager::LoadState::UPLOADED ||
       textureInfo.loadState == TextureManager::LoadState::LOAD_FINISHED)
    {
      return textureId;
    }
    else
    {
      Devel::PixelBuffer pixelBuffer = LoadImageSynchronously(url, desiredSize, fittingMode, samplingMode, orientationCorrection);

      if(!pixelBuffer)
      {
        // If pixelBuffer loading is failed in synchronously, call Remove() method.
        Remove(textureId, nullptr);
        return INVALID_TEXTURE_ID;
      }

      if(storageType == StorageType::KEEP_PIXEL_BUFFER) // For the mask image loading.
      {
        textureInfo.pixelBuffer = pixelBuffer; // Store the pixel data
        textureInfo.loadState   = LoadState::LOAD_FINISHED;
      }
      else // For the image loading.
      {
        if(maskTextureId != INVALID_TEXTURE_ID)
        {
          TextureCacheIndex maskCacheIndex = mTextureCacheManager.GetCacheIndexFromId(maskTextureId);
          if(maskCacheIndex != INVALID_CACHE_INDEX)
          {
            Devel::PixelBuffer maskPixelBuffer = mTextureCacheManager[maskCacheIndex].pixelBuffer;
            if(maskPixelBuffer)
            {
              pixelBuffer.ApplyMask(maskPixelBuffer, contentScale, cropToMask);
            }
          }
          else
          {
            DALI_LOG_ERROR("Mask image is not stored in cache.\n");
          }
        }
        PreMultiply(pixelBuffer, preMultiplyOnLoad);

        // Upload texture
        UploadTexture(pixelBuffer, textureInfo);
      }
    }
  }

  // Return the TextureId for which this Texture can now be referenced by externally.
  return textureId;
}

void TextureManager::Remove(const TextureManager::TextureId& textureId, TextureUploadObserver* observer)
{
  // Remove textureId in CacheManager.
  mTextureCacheManager.RemoveCache(textureId);

  if(observer)
  {
    // Remove element from the LoadQueue
    for(auto&& element : mLoadQueue)
    {
      if(element.mObserver == observer)
      {
        // Do not erase the item. We will clear it later in ProcessQueuedTextures().
        element.mObserver = nullptr;
        break;
      }
    }
  }
}

Devel::PixelBuffer TextureManager::LoadImageSynchronously(
  const VisualUrl&                url,
  const Dali::ImageDimensions&    desiredSize,
  const Dali::FittingMode::Type&  fittingMode,
  const Dali::SamplingMode::Type& samplingMode,
  const bool&                     orientationCorrection)
{
  Devel::PixelBuffer pixelBuffer;
  if(url.IsBufferResource())
  {
    const EncodedImageBuffer& encodedImageBuffer = mTextureCacheManager.GetEncodedImageBuffer(url.GetUrl());
    if(encodedImageBuffer)
    {
      pixelBuffer = LoadImageFromBuffer(encodedImageBuffer.GetRawBuffer(), desiredSize, fittingMode, samplingMode, orientationCorrection);
    }
  }
  else
  {
    pixelBuffer = LoadImageFromFile(url.GetUrl(), desiredSize, fittingMode, samplingMode, orientationCorrection);
  }
  return pixelBuffer;
}

void TextureManager::AddObserver(TextureManager::LifecycleObserver& observer)
{
  // make sure an observer doesn't observe the same object twice
  // otherwise it will get multiple calls to ObjectDestroyed()
  DALI_ASSERT_DEBUG(mLifecycleObservers.End() == std::find(mLifecycleObservers.Begin(), mLifecycleObservers.End(), &observer));
  mLifecycleObservers.PushBack(&observer);
}

void TextureManager::RemoveObserver(TextureManager::LifecycleObserver& observer)
{
  // Find the observer...
  auto endIter = mLifecycleObservers.End();
  for(auto iter = mLifecycleObservers.Begin(); iter != endIter; ++iter)
  {
    if((*iter) == &observer)
    {
      mLifecycleObservers.Erase(iter);
      break;
    }
  }
  DALI_ASSERT_DEBUG(endIter != mLifecycleObservers.End());
}

void TextureManager::LoadOrQueueTexture(TextureManager::TextureInfo& textureInfo, TextureUploadObserver* observer)
{
  switch(textureInfo.loadState)
  {
    case LoadState::NOT_STARTED:
    case LoadState::LOAD_FAILED:
    {
      if(mQueueLoadFlag)
      {
        QueueLoadTexture(textureInfo, observer);
      }
      else
      {
        LoadTexture(textureInfo, observer);
      }
      break;
    }
    case LoadState::UPLOADED:
    {
      if(mQueueLoadFlag)
      {
        QueueLoadTexture(textureInfo, observer);
      }
      else
      {
        // The Texture has already loaded. The other observers have already been notified.
        // We need to send a "late" loaded notification for this observer.
        observer->LoadComplete(true, TextureUploadObserver::TextureInformation(TextureUploadObserver::ReturnType::TEXTURE, textureInfo.textureId, textureInfo.textureSet, (textureInfo.useAtlas == UseAtlas::USE_ATLAS) ? true : false, textureInfo.atlasRect, textureInfo.preMultiplied));
      }
      break;
    }
    case LoadState::LOADING:
    case LoadState::CANCELLED:
    case LoadState::LOAD_FINISHED:
    case LoadState::WAITING_FOR_MASK:
    case LoadState::MASK_APPLYING:
    case LoadState::MASK_APPLIED:
    {
      break;
    }
  }
}

void TextureManager::QueueLoadTexture(const TextureManager::TextureInfo& textureInfo, TextureUploadObserver* observer)
{
  const auto& textureId = textureInfo.textureId;
  mLoadQueue.PushBack(LoadQueueElement(textureId, observer));

  observer->DestructionSignal().Connect(this, &TextureManager::ObserverDestroyed);
}

void TextureManager::LoadTexture(TextureManager::TextureInfo& textureInfo, TextureUploadObserver* observer)
{
  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::LoadTexture(): url:%s sync:%s\n", textureInfo.url.GetUrl().c_str(), textureInfo.loadSynchronously ? "T" : "F");

  textureInfo.loadState = LoadState::LOADING;
  if(!textureInfo.loadSynchronously)
  {
    auto& loadersContainer  = (textureInfo.url.IsLocalResource() || textureInfo.url.IsBufferResource()) ? mAsyncLocalLoaders : mAsyncRemoteLoaders;
    auto  loadingHelperIt   = loadersContainer.GetNext();
    auto  premultiplyOnLoad = (textureInfo.preMultiplyOnLoad && textureInfo.maskTextureId == INVALID_TEXTURE_ID) ? DevelAsyncImageLoader::PreMultiplyOnLoad::ON : DevelAsyncImageLoader::PreMultiplyOnLoad::OFF;
    DALI_ASSERT_ALWAYS(loadingHelperIt != loadersContainer.End());
    if(textureInfo.animatedImageLoading)
    {
      loadingHelperIt->LoadAnimatedImage(textureInfo.textureId, textureInfo.animatedImageLoading, textureInfo.frameIndex);
    }
    else
    {
      loadingHelperIt->Load(textureInfo.textureId, textureInfo.url, textureInfo.desiredSize, textureInfo.fittingMode, textureInfo.samplingMode, textureInfo.orientationCorrection, premultiplyOnLoad);
    }
  }
  ObserveTexture(textureInfo, observer);
}

void TextureManager::ProcessQueuedTextures()
{
  for(auto&& element : mLoadQueue)
  {
    if(!element.mObserver)
    {
      continue;
    }

    TextureCacheIndex cacheIndex = mTextureCacheManager.GetCacheIndexFromId(element.mTextureId);
    if(cacheIndex != INVALID_CACHE_INDEX)
    {
      TextureInfo& textureInfo(mTextureCacheManager[cacheIndex]);
      if(textureInfo.loadState == LoadState::UPLOADED)
      {
        element.mObserver->LoadComplete(true, TextureUploadObserver::TextureInformation(TextureUploadObserver::ReturnType::TEXTURE, textureInfo.textureId, textureInfo.textureSet, (textureInfo.useAtlas == UseAtlas::USE_ATLAS) ? true : false, textureInfo.atlasRect, textureInfo.preMultiplied));
      }
      else if(textureInfo.loadState == LoadState::LOAD_FINISHED && textureInfo.storageType == StorageType::RETURN_PIXEL_BUFFER)
      {
        element.mObserver->LoadComplete(true, TextureUploadObserver::TextureInformation(TextureUploadObserver::ReturnType::PIXEL_BUFFER, textureInfo.pixelBuffer, textureInfo.url.GetUrl(), textureInfo.preMultiplied));
      }
      else
      {
        LoadTexture(textureInfo, element.mObserver);
      }
    }
  }
  mLoadQueue.Clear();
}

void TextureManager::ObserveTexture(TextureManager::TextureInfo& textureInfo,
                                    TextureUploadObserver*       observer)
{
  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::ObserveTexture(): url:%s observer:%p\n", textureInfo.url.GetUrl().c_str(), observer);

  if(observer)
  {
    textureInfo.observerList.PushBack(observer);
    observer->DestructionSignal().Connect(this, &TextureManager::ObserverDestroyed);
  }
}

void TextureManager::AsyncLoadComplete(const TextureManager::TextureId& textureId, Devel::PixelBuffer pixelBuffer)
{
  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::AsyncLoadComplete( textureId:%d )\n", textureId);
  TextureCacheIndex cacheIndex = mTextureCacheManager.GetCacheIndexFromId(textureId);
  if(cacheIndex != INVALID_CACHE_INDEX)
  {
    TextureInfo& textureInfo(mTextureCacheManager[cacheIndex]);

    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "  textureId:%d Url:%s CacheIndex:%d LoadState: %s\n", textureInfo.textureId, textureInfo.url.GetUrl().c_str(), cacheIndex, GET_LOAD_STATE_STRING(textureInfo.loadState));

    if(textureInfo.loadState != LoadState::CANCELLED)
    {
      // textureInfo can be invalidated after this call (as the mTextureInfoContainer may be modified)
      PostLoad(textureInfo, pixelBuffer);
    }
    else
    {
      Remove(textureInfo.textureId, nullptr);
    }
  }
}

void TextureManager::PostLoad(TextureManager::TextureInfo& textureInfo, Devel::PixelBuffer& pixelBuffer)
{
  // Was the load successful?
  if(pixelBuffer && (pixelBuffer.GetWidth() != 0) && (pixelBuffer.GetHeight() != 0))
  {
    // No atlas support for now
    textureInfo.useAtlas      = UseAtlas::NO_ATLAS;
    textureInfo.preMultiplied = pixelBuffer.IsAlphaPreMultiplied();

    if(textureInfo.storageType == StorageType::UPLOAD_TO_TEXTURE)
    {
      // If there is a mask texture ID associated with this texture, then apply the mask
      // if it's already loaded. If it hasn't, and the mask is still loading,
      // wait for the mask to finish loading.
      // note, If the texture is already uploaded synchronously during loading,
      // we don't need to apply mask.
      if(textureInfo.loadState != LoadState::UPLOADED &&
         textureInfo.maskTextureId != INVALID_TEXTURE_ID)
      {
        if(textureInfo.loadState == LoadState::MASK_APPLYING)
        {
          textureInfo.loadState = LoadState::MASK_APPLIED;
          UploadTexture(pixelBuffer, textureInfo);
          NotifyObservers(textureInfo, true);
        }
        else
        {
          LoadState maskLoadState = mTextureCacheManager.GetTextureStateInternal(textureInfo.maskTextureId);
          textureInfo.pixelBuffer = pixelBuffer; // Store the pixel buffer temporarily
          if(maskLoadState == LoadState::LOADING)
          {
            textureInfo.loadState = LoadState::WAITING_FOR_MASK;
          }
          else if(maskLoadState == LoadState::LOAD_FINISHED)
          {
            // Send New Task to Thread
            ApplyMask(textureInfo, textureInfo.maskTextureId);
          }
        }
      }
      else
      {
        UploadTexture(pixelBuffer, textureInfo);
        NotifyObservers(textureInfo, true);
      }
    }
    else
    {
      textureInfo.pixelBuffer = pixelBuffer; // Store the pixel data
      textureInfo.loadState   = LoadState::LOAD_FINISHED;

      if(textureInfo.storageType == StorageType::RETURN_PIXEL_BUFFER)
      {
        NotifyObservers(textureInfo, true);
      }
      else
      {
        // Check if there was another texture waiting for this load to complete
        // (e.g. if this was an image mask, and its load is on a different thread)
        CheckForWaitingTexture(textureInfo);
      }
    }
  }
  else
  {
    textureInfo.loadState = LoadState::LOAD_FAILED;
    CheckForWaitingTexture(textureInfo);
    NotifyObservers(textureInfo, false);
  }
}

void TextureManager::CheckForWaitingTexture(TextureManager::TextureInfo& maskTextureInfo)
{
  // Search the cache, checking if any texture has this texture id as a
  // maskTextureId:
  const TextureCacheIndex size = static_cast<TextureCacheIndex>(mTextureCacheManager.size());

  for(TextureCacheIndex cacheIndex = 0; cacheIndex < size; ++cacheIndex)
  {
    if(mTextureCacheManager[cacheIndex].maskTextureId == maskTextureInfo.textureId &&
       mTextureCacheManager[cacheIndex].loadState == LoadState::WAITING_FOR_MASK)
    {
      TextureInfo& textureInfo(mTextureCacheManager[cacheIndex]);

      if(maskTextureInfo.loadState == LoadState::LOAD_FINISHED)
      {
        // Send New Task to Thread
        ApplyMask(textureInfo, maskTextureInfo.textureId);
      }
      else
      {
        textureInfo.pixelBuffer.Reset();
        textureInfo.loadState = LoadState::LOAD_FAILED;
        NotifyObservers(textureInfo, false);
      }
    }
  }
}

void TextureManager::ApplyMask(TextureManager::TextureInfo& textureInfo, const TextureManager::TextureId& maskTextureId)
{
  TextureCacheIndex maskCacheIndex = mTextureCacheManager.GetCacheIndexFromId(maskTextureId);
  if(maskCacheIndex != INVALID_CACHE_INDEX)
  {
    Devel::PixelBuffer maskPixelBuffer = mTextureCacheManager[maskCacheIndex].pixelBuffer;
    Devel::PixelBuffer pixelBuffer     = textureInfo.pixelBuffer;
    textureInfo.pixelBuffer.Reset();

    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::ApplyMask(): url:%s sync:%s\n", textureInfo.url.GetUrl().c_str(), textureInfo.loadSynchronously ? "T" : "F");

    textureInfo.loadState   = LoadState::MASK_APPLYING;
    auto& loadersContainer  = (textureInfo.url.IsLocalResource() || textureInfo.url.IsBufferResource()) ? mAsyncLocalLoaders : mAsyncRemoteLoaders;
    auto  loadingHelperIt   = loadersContainer.GetNext();
    auto  premultiplyOnLoad = textureInfo.preMultiplyOnLoad ? DevelAsyncImageLoader::PreMultiplyOnLoad::ON : DevelAsyncImageLoader::PreMultiplyOnLoad::OFF;
    DALI_ASSERT_ALWAYS(loadingHelperIt != loadersContainer.End());
    loadingHelperIt->ApplyMask(textureInfo.textureId, pixelBuffer, maskPixelBuffer, textureInfo.scaleFactor, textureInfo.cropToMask, premultiplyOnLoad);
  }
}

void TextureManager::UploadTexture(Devel::PixelBuffer& pixelBuffer, TextureManager::TextureInfo& textureInfo)
{
  if(textureInfo.loadState != LoadState::UPLOADED && textureInfo.useAtlas != UseAtlas::USE_ATLAS)
  {
    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::General, "  TextureManager::UploadTexture() New Texture for textureId:%d\n", textureInfo.textureId);

    // Check if this pixelBuffer is premultiplied
    textureInfo.preMultiplied = pixelBuffer.IsAlphaPreMultiplied();

    auto& renderingAddOn = RenderingAddOn::Get();
    if(renderingAddOn.IsValid())
    {
      renderingAddOn.CreateGeometry(textureInfo.textureId, pixelBuffer);
    }

    Texture texture = Texture::New(Dali::TextureType::TEXTURE_2D, pixelBuffer.GetPixelFormat(), pixelBuffer.GetWidth(), pixelBuffer.GetHeight());

    PixelData pixelData = Devel::PixelBuffer::Convert(pixelBuffer);
    texture.Upload(pixelData);
    if(!textureInfo.textureSet)
    {
      textureInfo.textureSet = TextureSet::New();
    }
    textureInfo.textureSet.SetTexture(0u, texture);
  }

  // Update the load state.
  // Note: This is regardless of success as we care about whether a
  // load attempt is in progress or not.  If unsuccessful, a broken
  // image is still loaded.
  textureInfo.loadState = LoadState::UPLOADED;
}

void TextureManager::NotifyObservers(TextureManager::TextureInfo& textureInfo, const bool& success)
{
  TextureId textureId = textureInfo.textureId;

  // If there is an observer: Notify the load is complete, whether successful or not,
  // and erase it from the list
  TextureInfo* info = &textureInfo;

  mQueueLoadFlag = true;

  while(info->observerList.Count())
  {
    TextureUploadObserver* observer = info->observerList[0];

    // During LoadComplete() a Control ResourceReady() signal is emitted.
    // During that signal the app may add remove /add Textures (e.g. via
    // ImageViews).
    // It is possible for observers to be removed from the observer list,
    // and it is also possible for the mTextureInfoContainer to be modified,
    // invalidating the reference to the textureInfo struct.
    // Texture load requests for the same URL are deferred until the end of this
    // method.
    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::NotifyObservers() textureId:%d url:%s loadState:%s\n", textureId, textureInfo.url.GetUrl().c_str(), GET_LOAD_STATE_STRING(textureInfo.loadState));

    // It is possible for the observer to be deleted.
    // Disconnect and remove the observer first.
    observer->DestructionSignal().Disconnect(this, &TextureManager::ObserverDestroyed);

    info->observerList.Erase(info->observerList.Begin());

    if(info->storageType == StorageType::RETURN_PIXEL_BUFFER)
    {
      observer->LoadComplete(success, TextureUploadObserver::TextureInformation(TextureUploadObserver::ReturnType::PIXEL_BUFFER, info->pixelBuffer, info->url.GetUrl(), info->preMultiplied));
    }
    else
    {
      observer->LoadComplete(success, TextureUploadObserver::TextureInformation(TextureUploadObserver::ReturnType::TEXTURE, info->textureId, info->textureSet, (info->useAtlas == UseAtlas::USE_ATLAS) ? true : false, info->atlasRect, info->preMultiplied));
    }

    // Get the textureInfo from the container again as it may have been invalidated.
    TextureCacheIndex textureInfoIndex = mTextureCacheManager.GetCacheIndexFromId(textureId);
    if(textureInfoIndex == INVALID_CACHE_INDEX)
    {
      break; // texture has been removed - can stop.
    }
    info = &mTextureCacheManager[textureInfoIndex];
  }

  mQueueLoadFlag = false;
  ProcessQueuedTextures();

  if(info->storageType == StorageType::RETURN_PIXEL_BUFFER && info->observerList.Count() == 0)
  {
    Remove(info->textureId, nullptr);
  }
}

void TextureManager::ObserverDestroyed(TextureUploadObserver* observer)
{
  const TextureCacheIndex count = static_cast<TextureCacheIndex>(mTextureCacheManager.size());
  for(TextureCacheIndex i = 0; i < count; ++i)
  {
    TextureInfo& textureInfo(mTextureCacheManager[i]);
    for(TextureInfo::ObserverListType::Iterator j = textureInfo.observerList.Begin();
        j != textureInfo.observerList.End();)
    {
      if(*j == observer)
      {
        j = textureInfo.observerList.Erase(j);
      }
      else
      {
        ++j;
      }
    }
  }

  // Remove element from the LoadQueue
  for(auto&& element : mLoadQueue)
  {
    if(element.mObserver == observer)
    {
      element.mObserver = nullptr;
    }
  }
}

Dali::Geometry TextureManager::GetRenderGeometry(const TextureManager::TextureId& textureId, std::uint32_t& frontElements, std::uint32_t& backElements)
{
  return RenderingAddOn::Get().IsValid() ? RenderingAddOn::Get().GetGeometry(textureId, frontElements, backElements) : Geometry();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
