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

constexpr auto TEXTURE_INDEX      = 0u; ///< The Index for texture
constexpr auto MASK_TEXTURE_INDEX = 1u; ///< The Index for mask texture

constexpr auto NUMBER_OF_LOCAL_LOADER_THREADS_ENV  = "DALI_TEXTURE_LOCAL_THREADS";
constexpr auto NUMBER_OF_REMOTE_LOADER_THREADS_ENV = "DALI_TEXTURE_REMOTE_THREADS";
constexpr auto LOAD_IMAGE_YUV_PLANES_ENV           = "DALI_LOAD_IMAGE_YUV_PLANES";

bool NeedToLoadYuvPlanes()
{
  auto loadYuvPlanesString = Dali::EnvironmentVariable::GetEnvironmentVariable(LOAD_IMAGE_YUV_PLANES_ENV);
  bool loadYuvPlanes       = loadYuvPlanesString ? std::atoi(loadYuvPlanesString) : false;
  return loadYuvPlanes;
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
  loadState == TextureManagerType::LoadState::MASK_CANCELLED   ? "MASK_CANCELLED"   : \
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
  mCropToMask(true),
  mPreappliedMasking(true),
  mMaskImageLoadingFailed(false)
{
}

TextureManager::TextureManager()
: mTextureCacheManager(),
  mAsyncLoader(std::unique_ptr<TextureAsyncLoadingHelper>(new TextureAsyncLoadingHelper(*this))),
  mLifecycleObservers(),
  mLoadQueue(),
  mRemoveQueue(),
  mLoadingQueueTextureId(INVALID_TEXTURE_ID),
  mLoadYuvPlanes(NeedToLoadYuvPlanes())
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
  const VisualUrl&                url,
  Dali::AnimatedImageLoading      animatedImageLoading,
  const uint32_t&                 frameIndex,
  TextureManager::TextureId&      textureId,
  MaskingDataPointer&             maskInfo,
  const Dali::SamplingMode::Type& samplingMode,
  const bool&                     synchronousLoading,
  TextureUploadObserver*          textureObserver,
  TextureManager::MultiplyOnLoad& preMultiplyOnLoad)
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
      Texture maskTexture;
      if(maskInfo && maskInfo->mAlphaMaskUrl.IsValid())
      {
        Devel::PixelBuffer maskPixelBuffer = LoadImageFromFile(maskInfo->mAlphaMaskUrl.GetUrl(), ImageDimensions(), FittingMode::SCALE_TO_FILL, SamplingMode::NO_FILTER, true);
        if(maskPixelBuffer)
        {
          if(!maskInfo->mPreappliedMasking)
          {
            PixelData maskPixelData = Devel::PixelBuffer::Convert(maskPixelBuffer); // takes ownership of buffer
            maskTexture             = Texture::New(Dali::TextureType::TEXTURE_2D, maskPixelData.GetPixelFormat(), maskPixelData.GetWidth(), maskPixelData.GetHeight());
            maskTexture.Upload(maskPixelData);
          }
          else
          {
            pixelBuffer.ApplyMask(maskPixelBuffer, maskInfo->mContentScaleFactor, maskInfo->mCropToMask);
          }
        }
        else
        {
          DALI_LOG_ERROR("TextureManager::LoadAnimatedImageTexture: Synchronous mask image loading is failed\n");
        }
      }

      if(preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD)
      {
        PreMultiply(pixelBuffer, preMultiplyOnLoad);
      }

      PixelData pixelData = Devel::PixelBuffer::Convert(pixelBuffer); // takes ownership of buffer
      if(!textureSet)
      {
        Texture texture = Texture::New(Dali::TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight());
        texture.Upload(pixelData);
        textureSet = TextureSet::New();
        textureSet.SetTexture(TEXTURE_INDEX, texture);
        if(maskTexture)
        {
          textureSet.SetTexture(MASK_TEXTURE_INDEX, maskTexture);
        }
      }
    }
  }
  else
  {
    TextureId alphaMaskId        = INVALID_TEXTURE_ID;
    float     contentScaleFactor = 1.0f;
    bool      cropToMask         = false;
    if(maskInfo && maskInfo->mAlphaMaskUrl.IsValid())
    {
      maskInfo->mAlphaMaskId = RequestMaskLoad(maskInfo->mAlphaMaskUrl, maskInfo->mPreappliedMasking ? StorageType::KEEP_PIXEL_BUFFER : StorageType::KEEP_TEXTURE);
      alphaMaskId            = maskInfo->mAlphaMaskId;
      if(maskInfo->mPreappliedMasking)
      {
        contentScaleFactor = maskInfo->mContentScaleFactor;
        cropToMask         = maskInfo->mCropToMask;
      }
    }

    textureId = RequestLoadInternal(url, alphaMaskId, contentScaleFactor, ImageDimensions(), FittingMode::SCALE_TO_FILL, SamplingMode::BOX_THEN_LINEAR, UseAtlas::NO_ATLAS, cropToMask, StorageType::UPLOAD_TO_TEXTURE, textureObserver, true, TextureManager::ReloadPolicy::CACHED, preMultiplyOnLoad, animatedImageLoading, frameIndex, false);

    TextureManager::LoadState loadState = mTextureCacheManager.GetTextureStateInternal(textureId);
    if(loadState == TextureManager::LoadState::UPLOADED)
    {
      // LoadComplete has already been called - keep the same texture set
      textureSet = GetTextureSet(textureId);
    }
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
        const EncodedImageBuffer& encodedImageBuffer = mTextureCacheManager.GetEncodedImageBuffer(url);
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
    if(synchronousLoading && atlasingStatus)
    {
      const bool synchronousAtlasAvaliable = (desiredSize != ImageDimensions() || url.IsLocalResource()) ? imageAtlasManager->CheckAtlasAvailable(url, desiredSize)
                                                                                                         : false;
      if(synchronousAtlasAvaliable)
      {
        std::vector<Devel::PixelBuffer> pixelBuffers;
        LoadImageSynchronously(url, desiredSize, fittingMode, samplingMode, orientationCorrection, false, pixelBuffers);

        if(!pixelBuffers.empty() && maskInfo && maskInfo->mAlphaMaskUrl.IsValid())
        {
          std::vector<Devel::PixelBuffer> maskPixelBuffers;
          LoadImageSynchronously(maskInfo->mAlphaMaskUrl, ImageDimensions(), FittingMode::SCALE_TO_FILL, SamplingMode::NO_FILTER, true, false, maskPixelBuffers);
          if(!maskPixelBuffers.empty())
          {
            pixelBuffers[0].ApplyMask(maskPixelBuffers[0], maskInfo->mContentScaleFactor, maskInfo->mCropToMask);
          }
        }

        PixelData data;
        if(!pixelBuffers.empty())
        {
          PreMultiply(pixelBuffers[0], preMultiplyOnLoad);
          data = Devel::PixelBuffer::Convert(pixelBuffers[0]); // takes ownership of buffer

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
        if(url.IsBufferResource())
        {
          const EncodedImageBuffer& encodedImageBuffer = GetEncodedImageBuffer(url.GetUrl());
          if(encodedImageBuffer)
          {
            textureSet = imageAtlasManager->Add(textureRect, encodedImageBuffer, desiredSize, fittingMode, true, atlasObserver);
          }
        }
        else
        {
          textureSet = imageAtlasManager->Add(textureRect, url, atlasDesiredSize, fittingMode, true, atlasObserver);
        }
      }
      if(!textureSet) // big image, no atlasing or atlasing failed
      {
        atlasingStatus = false;

        TextureId alphaMaskId        = INVALID_TEXTURE_ID;
        float     contentScaleFactor = 1.0f;
        bool      cropToMask         = false;
        if(maskInfo && maskInfo->mAlphaMaskUrl.IsValid())
        {
          maskInfo->mAlphaMaskId = RequestMaskLoad(maskInfo->mAlphaMaskUrl, maskInfo->mPreappliedMasking ? StorageType::KEEP_PIXEL_BUFFER : StorageType::KEEP_TEXTURE, synchronousLoading);
          alphaMaskId            = maskInfo->mAlphaMaskId;
          if(maskInfo && maskInfo->mPreappliedMasking)
          {
            contentScaleFactor = maskInfo->mContentScaleFactor;
            cropToMask         = maskInfo->mCropToMask;
          }
        }

        textureId = RequestLoad(
          url,
          alphaMaskId,
          contentScaleFactor,
          desiredSize,
          fittingMode,
          samplingMode,
          UseAtlas::NO_ATLAS,
          cropToMask,
          textureObserver,
          orientationCorrection,
          reloadPolicy,
          preMultiplyOnLoad,
          synchronousLoading);

        TextureManager::LoadState loadState = mTextureCacheManager.GetTextureStateInternal(textureId);
        if(loadState == TextureManager::LoadState::UPLOADED)
        {
          // LoadComplete has already been called - keep the same texture set
          textureSet = GetTextureSet(textureId);
        }

        // If we are loading the texture, or waiting for the ready signal handler to complete, inform
        // caller that they need to wait.
        loadingStatus = (loadState == TextureManager::LoadState::LOADING ||
                         loadState == TextureManager::LoadState::WAITING_FOR_MASK ||
                         loadState == TextureManager::LoadState::MASK_APPLYING ||
                         loadState == TextureManager::LoadState::MASK_APPLIED ||
                         loadState == TextureManager::LoadState::NOT_STARTED ||
                         mLoadingQueueTextureId != INVALID_TEXTURE_ID);
      }
      else
      {
        textureRectSize = atlasDesiredSize;
      }
    }
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
  StorageType      storageType,
  const bool&      synchronousLoading)
{
  // Use the normal load procedure to get the alpha mask.
  auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  return RequestLoadInternal(maskUrl, INVALID_TEXTURE_ID, 1.0f, ImageDimensions(), FittingMode::SCALE_TO_FILL, SamplingMode::NO_FILTER, UseAtlas::NO_ATLAS, false, storageType, NULL, true, TextureManager::ReloadPolicy::CACHED, preMultiply, Dali::AnimatedImageLoading(), 0u, synchronousLoading);
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
  TextureHash       textureHash   = INITIAL_HASH_NUMBER;
  TextureCacheIndex cacheIndex    = INVALID_CACHE_INDEX;
  bool              loadYuvPlanes = (mLoadYuvPlanes && maskTextureId == INVALID_TEXTURE_ID && storageType == StorageType::UPLOAD_TO_TEXTURE);

  if(storageType != StorageType::RETURN_PIXEL_BUFFER)
  {
    textureHash = mTextureCacheManager.GenerateHash(url, desiredSize, fittingMode, samplingMode, useAtlas, maskTextureId, cropToMask, frameIndex);

    // Look up the texture by hash. Note: The extra parameters are used in case of a hash collision.
    cacheIndex = mTextureCacheManager.FindCachedTexture(textureHash, url, desiredSize, fittingMode, samplingMode, useAtlas, storageType, maskTextureId, cropToMask, preMultiplyOnLoad, (animatedImageLoading) ? true : false, frameIndex);
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
    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::General, "TextureManager::RequestLoad( url=%s observer=%p ) Using cached texture id@%d, textureId=%d, maskTextureId=%d, frameindex=%d, premultiplied=%d\n", url.GetUrl().c_str(), observer, cacheIndex.GetIndex(), textureId, maskTextureId, frameIndex, mTextureCacheManager[cacheIndex].preMultiplied ? 1 : 0);
  }

  if(textureId == INVALID_TEXTURE_ID) // There was no caching, or caching not required
  {
    textureId = mTextureCacheManager.GenerateTextureId();

    bool preMultiply = (preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD);

    // Cache new texutre, and get cacheIndex.
    cacheIndex = mTextureCacheManager.AppendCache(TextureInfo(textureId, maskTextureId, url, desiredSize, contentScale, fittingMode, samplingMode, false, cropToMask, useAtlas, textureHash, orientationCorrection, preMultiply, animatedImageLoading, frameIndex, loadYuvPlanes));
    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::General, "TextureManager::RequestLoad( url=%s observer=%p ) New texture, cacheIndex:%d, textureId=%d, maskTextureId=%d, frameindex=%d premultiply=%d\n", url.GetUrl().c_str(), observer, cacheIndex.GetIndex(), textureId, maskTextureId, frameIndex, preMultiply);
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
     TextureManager::LoadState::CANCELLED != textureInfo.loadState &&
     TextureManager::LoadState::MASK_CANCELLED != textureInfo.loadState)
  {
    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Verbose, "TextureManager::RequestLoad( url=%s observer=%p ) ForcedReload cacheIndex:%d, textureId=%d, maskTextureId=%d\n", url.GetUrl().c_str(), observer, cacheIndex.GetIndex(), textureId, maskTextureId);
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
      case TextureManager::LoadState::MASK_CANCELLED:
      {
        // A cancelled texture hasn't finished mask applying yet. Treat as a mask applying texture
        // (it's ref count has already been incremented, above)
        textureInfo.loadState = TextureManager::LoadState::MASK_APPLYING;
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
    if(!(textureInfo.loadState == TextureManager::LoadState::UPLOADED ||
         textureInfo.loadState == TextureManager::LoadState::LOAD_FINISHED))
    {
      std::vector<Devel::PixelBuffer> pixelBuffers;
      LoadImageSynchronously(url, desiredSize, fittingMode, samplingMode, orientationCorrection, loadYuvPlanes, pixelBuffers);

      if(pixelBuffers.empty())
      {
        // If pixelBuffer loading is failed in synchronously, call Remove() method.
        Remove(textureId, nullptr);
        return INVALID_TEXTURE_ID;
      }

      if(storageType == StorageType::KEEP_PIXEL_BUFFER) // For the mask image loading.
      {
        textureInfo.pixelBuffer = pixelBuffers[0]; // Store the pixel data
        textureInfo.loadState   = LoadState::LOAD_FINISHED;
      }
      else // For the image loading.
      {
        Texture maskTexture;
        if(maskTextureId != INVALID_TEXTURE_ID)
        {
          TextureCacheIndex maskCacheIndex = mTextureCacheManager.GetCacheIndexFromId(maskTextureId);
          if(maskCacheIndex != INVALID_CACHE_INDEX)
          {
            if(mTextureCacheManager[maskCacheIndex].storageType == StorageType::KEEP_TEXTURE)
            {
              if(!mTextureCacheManager[maskCacheIndex].textures.empty())
              {
                maskTexture = mTextureCacheManager[maskCacheIndex].textures[0];
              }
            }
            else if(mTextureCacheManager[maskCacheIndex].storageType == StorageType::KEEP_PIXEL_BUFFER)
            {
              Devel::PixelBuffer maskPixelBuffer = mTextureCacheManager[maskCacheIndex].pixelBuffer;
              if(maskPixelBuffer)
              {
                pixelBuffers[0].ApplyMask(maskPixelBuffer, contentScale, cropToMask);
              }
              else
              {
                DALI_LOG_ERROR("Mask image cached invalid pixel buffer!\n");
              }
            }
          }
          else
          {
            DALI_LOG_ERROR("Mask image is not stored in cache.\n");
          }
        }
        PreMultiply(pixelBuffers[0], preMultiplyOnLoad);

        // Upload texture
        UploadTextures(pixelBuffers, textureInfo);
      }
    }
  }

  return textureId;
}

void TextureManager::Remove(const TextureManager::TextureId& textureId, TextureUploadObserver* observer)
{
  if(textureId != INVALID_TEXTURE_ID)
  {
    TextureCacheIndex textureCacheIndex = mTextureCacheManager.GetCacheIndexFromId(textureId);
    if(textureCacheIndex != INVALID_CACHE_INDEX)
    {
      TextureManager::TextureId maskTextureId = INVALID_TEXTURE_ID;
      TextureInfo&              textureInfo(mTextureCacheManager[textureCacheIndex]);
      // We only need to consider maskTextureId when texture's loadState is not cancelled. Because it is already deleted.
      if(textureInfo.loadState != LoadState::CANCELLED && textureInfo.loadState != LoadState::MASK_CANCELLED)
      {
        if(textureInfo.maskTextureId != INVALID_TEXTURE_ID)
        {
          maskTextureId = textureInfo.maskTextureId;
        }
      }

      DALI_LOG_INFO(gTextureManagerLogFilter, Debug::General, "TextureManager::Remove( textureId=%d observer=%p ) cacheIndex:%d removal maskTextureId=%d, loadingQueueTextureId=%d, loadState=%s\n", textureId, observer, textureCacheIndex.GetIndex(), maskTextureId, mLoadingQueueTextureId, GET_LOAD_STATE_STRING(textureInfo.loadState));

      // the case that LoadingQueue is working.
      if(mLoadingQueueTextureId != INVALID_TEXTURE_ID)
      {
        // If textureId is not same, this observer need to delete when ProcessRemoveQueue() is called.
        // If textureId is same, we should not call RemoveTextureObserver.
        // Because ObserverDestroyed signal already disconnected in NotifyObservers
        TextureUploadObserver* queueObserver = observer;
        if(mLoadingQueueTextureId == textureId)
        {
          queueObserver = nullptr;
        }

        // Remove element from the mLoadQueue
        for(auto&& element : mLoadQueue)
        {
          if(element.mTextureId == textureId && element.mObserver == observer)
          {
            // Do not erase the item. We will clear it later in ProcessLoadQueue().
            element.mTextureId = INVALID_TEXTURE_ID;
            element.mObserver  = nullptr;
            break;
          }
        }

        mRemoveQueue.PushBack(QueueElement(textureId, queueObserver));
      }
      else
      {
        // Remove its observer
        RemoveTextureObserver(textureInfo, observer);

        // Remove textureId in CacheManager. Now, textureInfo is invalidate.
        mTextureCacheManager.RemoveCache(textureInfo);

        // Remove maskTextureId in CacheManager
        if(maskTextureId != INVALID_TEXTURE_ID)
        {
          TextureCacheIndex maskCacheIndex = mTextureCacheManager.GetCacheIndexFromId(maskTextureId);
          if(maskCacheIndex != INVALID_CACHE_INDEX)
          {
            TextureInfo& maskTextureInfo(mTextureCacheManager[maskCacheIndex]);
            mTextureCacheManager.RemoveCache(maskTextureInfo);
          }
        }
      }
    }
  }
}

void TextureManager::LoadImageSynchronously(
  const VisualUrl&                 url,
  const Dali::ImageDimensions&     desiredSize,
  const Dali::FittingMode::Type&   fittingMode,
  const Dali::SamplingMode::Type&  samplingMode,
  const bool&                      orientationCorrection,
  const bool&                      loadYuvPlanes,
  std::vector<Devel::PixelBuffer>& pixelBuffers)
{
  Devel::PixelBuffer pixelBuffer;
  if(url.IsBufferResource())
  {
    const EncodedImageBuffer& encodedImageBuffer = mTextureCacheManager.GetEncodedImageBuffer(url);
    if(encodedImageBuffer)
    {
      pixelBuffer = LoadImageFromBuffer(encodedImageBuffer.GetRawBuffer(), desiredSize, fittingMode, samplingMode, orientationCorrection);
    }
  }
  else
  {
    if(loadYuvPlanes)
    {
      Dali::LoadImagePlanesFromFile(url.GetUrl(), pixelBuffers, desiredSize, fittingMode, samplingMode, orientationCorrection);
    }
    else
    {
      pixelBuffer = Dali::LoadImageFromFile(url.GetUrl(), desiredSize, fittingMode, samplingMode, orientationCorrection);
    }
  }

  if(pixelBuffer)
  {
    pixelBuffers.push_back(pixelBuffer);
  }
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
      if(mLoadingQueueTextureId != INVALID_TEXTURE_ID)
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
      if(mLoadingQueueTextureId != INVALID_TEXTURE_ID)
      {
        QueueLoadTexture(textureInfo, observer);
      }
      else
      {
        // The Texture has already loaded. The other observers have already been notified.
        // We need to send a "late" loaded notification for this observer.
        EmitLoadComplete(observer, textureInfo, true);
      }
      break;
    }
    case LoadState::LOADING:
    case LoadState::CANCELLED:
    case LoadState::MASK_CANCELLED:
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
  mLoadQueue.PushBack(QueueElement(textureId, observer));

  if(observer)
  {
    observer->DestructionSignal().Connect(this, &TextureManager::ObserverDestroyed);
  }
}

void TextureManager::LoadTexture(TextureManager::TextureInfo& textureInfo, TextureUploadObserver* observer)
{
  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::LoadTexture(): url:%s sync:%s\n", textureInfo.url.GetUrl().c_str(), textureInfo.loadSynchronously ? "T" : "F");
  textureInfo.loadState = LoadState::LOADING;
  if(!textureInfo.loadSynchronously)
  {
    auto  premultiplyOnLoad = (textureInfo.preMultiplyOnLoad && textureInfo.maskTextureId == INVALID_TEXTURE_ID) ? DevelAsyncImageLoader::PreMultiplyOnLoad::ON : DevelAsyncImageLoader::PreMultiplyOnLoad::OFF;
    if(textureInfo.animatedImageLoading)
    {
      mAsyncLoader->LoadAnimatedImage(textureInfo.textureId, textureInfo.animatedImageLoading, textureInfo.frameIndex, premultiplyOnLoad);
    }
    else
    {
      mAsyncLoader->Load(textureInfo.textureId, textureInfo.url, textureInfo.desiredSize, textureInfo.fittingMode, textureInfo.samplingMode, textureInfo.orientationCorrection, premultiplyOnLoad, textureInfo.loadYuvPlanes);
    }
  }
  ObserveTexture(textureInfo, observer);
}

void TextureManager::ProcessLoadQueue()
{
  for(auto&& element : mLoadQueue)
  {
    if(element.mTextureId == INVALID_TEXTURE_ID)
    {
      continue;
    }

    TextureCacheIndex cacheIndex = mTextureCacheManager.GetCacheIndexFromId(element.mTextureId);
    if(cacheIndex != INVALID_CACHE_INDEX)
    {
      TextureInfo& textureInfo(mTextureCacheManager[cacheIndex]);
      if((textureInfo.loadState == LoadState::UPLOADED) || (textureInfo.loadState == LoadState::LOAD_FINISHED && textureInfo.storageType == StorageType::RETURN_PIXEL_BUFFER))
      {
        if(element.mObserver)
        {
          EmitLoadComplete(element.mObserver, textureInfo, true);
        }
      }
      else if(textureInfo.loadState == LoadState::LOADING)
      {
        // Note : LOADING state texture cannot be queue.
        // This case be occured when same texture id are queue in mLoadQueue.
        ObserveTexture(textureInfo, element.mObserver);
      }
      else
      {
        LoadTexture(textureInfo, element.mObserver);
      }
    }
  }
  mLoadQueue.Clear();
}

void TextureManager::ProcessRemoveQueue()
{
  for(auto&& element : mRemoveQueue)
  {
    if(element.mTextureId != INVALID_TEXTURE_ID)
    {
      Remove(element.mTextureId, element.mObserver);
    }
  }
  mRemoveQueue.Clear();
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

void TextureManager::AsyncLoadComplete(const TextureManager::TextureId& textureId, std::vector<Devel::PixelBuffer>& pixelBuffers)
{
  TextureCacheIndex cacheIndex = mTextureCacheManager.GetCacheIndexFromId(textureId);
  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::AsyncLoadComplete( textureId:%d CacheIndex:%d )\n", textureId, cacheIndex.GetIndex());
  if(cacheIndex != INVALID_CACHE_INDEX)
  {
    TextureInfo& textureInfo(mTextureCacheManager[cacheIndex]);

    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "  textureId:%d Url:%s CacheIndex:%d LoadState: %s\n", textureInfo.textureId, textureInfo.url.GetUrl().c_str(), cacheIndex.GetIndex(), GET_LOAD_STATE_STRING(textureInfo.loadState));
    if(textureInfo.loadState != LoadState::CANCELLED && textureInfo.loadState != LoadState::MASK_CANCELLED)
    {
      // textureInfo can be invalidated after this call (as the mTextureInfoContainer may be modified)
      PostLoad(textureInfo, pixelBuffers);
    }
    else
    {
      Remove(textureInfo.textureId, nullptr);
    }
  }
}

void TextureManager::PostLoad(TextureManager::TextureInfo& textureInfo, std::vector<Devel::PixelBuffer>& pixelBuffers)
{
  // Was the load successful?
  if(!pixelBuffers.empty())
  {
    if(pixelBuffers.size() == 1)
    {
      Devel::PixelBuffer pixelBuffer = pixelBuffers[0];
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
              UploadTextures(pixelBuffers, textureInfo);
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
              else if(maskLoadState == LoadState::LOAD_FINISHED || maskLoadState == LoadState::UPLOADED)
              {
                // Send New Task to Thread
                TextureCacheIndex maskCacheIndex = mTextureCacheManager.GetCacheIndexFromId(textureInfo.maskTextureId);
                if(maskCacheIndex != INVALID_CACHE_INDEX)
                {
                  TextureInfo& maskTextureInfo(mTextureCacheManager[maskCacheIndex]);
                  if(maskTextureInfo.storageType == StorageType::KEEP_PIXEL_BUFFER)
                  {
                    // Send New Task to Thread
                    ApplyMask(textureInfo, textureInfo.maskTextureId);
                  }
                  else if(maskTextureInfo.storageType == StorageType::KEEP_TEXTURE)
                  {
                    // Upload image texture. textureInfo.loadState will be UPLOADED.
                    UploadTextures(pixelBuffers, textureInfo);

                    // notify mask texture set.
                    NotifyObservers(textureInfo, true);
                  }
                }
              }
              else // maskLoadState == LoadState::LOAD_FAILED
              {
                // Url texture load success, But alpha mask texture load failed. Run as normal image upload.
                DALI_LOG_ERROR("Alpha mask image loading failed! Image will not be masked\n");
                UploadTextures(pixelBuffers, textureInfo);
                NotifyObservers(textureInfo, true);
              }
            }
          }
          else
          {
            UploadTextures(pixelBuffers, textureInfo);
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
          else // for the StorageType::KEEP_PIXEL_BUFFER and StorageType::KEEP_TEXTURE
          {
            // Check if there was another texture waiting for this load to complete
            // (e.g. if this was an image mask, and its load is on a different thread)
            CheckForWaitingTexture(textureInfo);
          }
        }
      }
    }
    else
    {
      // YUV case
      // No atlas support for now
      textureInfo.useAtlas      = UseAtlas::NO_ATLAS;
      textureInfo.preMultiplied = false;

      UploadTextures(pixelBuffers, textureInfo);
      NotifyObservers(textureInfo, true);
    }
  }
  else
  {
    textureInfo.loadState = LoadState::LOAD_FAILED;
    if(textureInfo.storageType == StorageType::KEEP_PIXEL_BUFFER || textureInfo.storageType == StorageType::KEEP_TEXTURE)
    {
      // Check if there was another texture waiting for this load to complete
      // (e.g. if this was an image mask, and its load is on a different thread)
      CheckForWaitingTexture(textureInfo);
    }
    else
    {
      NotifyObservers(textureInfo, false);
    }
  }
}

void TextureManager::CheckForWaitingTexture(TextureManager::TextureInfo& maskTextureInfo)
{
  if(maskTextureInfo.loadState == LoadState::LOAD_FINISHED &&
     maskTextureInfo.storageType == StorageType::KEEP_TEXTURE)
  {
    // Upload mask texture. textureInfo.loadState will be UPLOADED.
    std::vector<Devel::PixelBuffer> pixelBuffers;
    pixelBuffers.push_back(maskTextureInfo.pixelBuffer);
    UploadTextures(pixelBuffers, maskTextureInfo);
  }

  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::CheckForWaitingTexture(): maskTextureId=%d, maskTextureUrl=%s\n", maskTextureInfo.textureId, maskTextureInfo.url.GetUrl().c_str());

  // Search the cache, checking if any texture has this texture id as a maskTextureId
  const std::size_t size = mTextureCacheManager.size();

  // Keep notify observer required textureIds.
  // Note : NotifyObservers can change mTextureCacheManager cache struct. We should check id's validation before notify.
  std::vector<TextureId> notifyRequiredTextureIds;

  // TODO : Refactorize here to not iterate whole cached image.
  for(TextureCacheIndex cacheIndex = TextureCacheIndex(TextureManagerType::TEXTURE_CACHE_INDEX_TYPE_LOCAL, 0u); cacheIndex.GetIndex() < size; ++cacheIndex.detailValue.index)
  {
    if(mTextureCacheManager[cacheIndex].maskTextureId == maskTextureInfo.textureId &&
       mTextureCacheManager[cacheIndex].loadState == LoadState::WAITING_FOR_MASK)
    {
      TextureInfo& textureInfo(mTextureCacheManager[cacheIndex]);

      if(maskTextureInfo.loadState == LoadState::LOAD_FINISHED)
      {
        if(maskTextureInfo.storageType == StorageType::KEEP_PIXEL_BUFFER)
        {
          // Send New Task to Thread
          ApplyMask(textureInfo, maskTextureInfo.textureId);
        }
      }
      else if(maskTextureInfo.loadState == LoadState::UPLOADED)
      {
        if(maskTextureInfo.storageType == StorageType::KEEP_TEXTURE)
        {
          // Upload image texture. textureInfo.loadState will be UPLOADED.
          std::vector<Devel::PixelBuffer> pixelBuffers;
          pixelBuffers.push_back(textureInfo.pixelBuffer);
          UploadTextures(pixelBuffers, textureInfo);

          // Increase reference counts for notify required textureId.
          // Now we can assume that we don't remove & re-assign this textureId
          // during NotifyObserver signal emit.
          maskTextureInfo.referenceCount++;
          textureInfo.referenceCount++;

          DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::CheckForWaitingTexture(): Ready to notify textureId=%d\n", textureInfo.textureId);

          notifyRequiredTextureIds.push_back(textureInfo.textureId);
        }
      }
      else // maskTextureInfo.loadState == LoadState::LOAD_FAILED
      {
        // Url texture load success, But alpha mask texture load failed. Run as normal image upload.
        DALI_LOG_ERROR("Alpha mask image loading failed! Image will not be masked\n");
        std::vector<Devel::PixelBuffer> pixelBuffers;
        pixelBuffers.push_back(textureInfo.pixelBuffer);
        UploadTextures(pixelBuffers, textureInfo);

        // Increase reference counts for notify required textureId.
        // Now we can assume that we don't remove & re-assign this textureId
        // during NotifyObserver signal emit.
        maskTextureInfo.referenceCount++;
        textureInfo.referenceCount++;

        DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::CheckForWaitingTexture(): Ready to notify textureId=%d\n", textureInfo.textureId);

        notifyRequiredTextureIds.push_back(textureInfo.textureId);
      }
    }
  }

  // Notify textures are masked
  for(const auto textureId : notifyRequiredTextureIds)
  {
    TextureCacheIndex textureCacheIndex = mTextureCacheManager.GetCacheIndexFromId(textureId);
    if(textureCacheIndex != INVALID_CACHE_INDEX)
    {
      TextureInfo& textureInfo(mTextureCacheManager[textureCacheIndex]);
      NotifyObservers(textureInfo, true);
    }
  }

  // Decrease reference count
  for(const auto textureId : notifyRequiredTextureIds)
  {
    Remove(textureId, nullptr);
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
    auto premultiplyOnLoad = textureInfo.preMultiplyOnLoad ? DevelAsyncImageLoader::PreMultiplyOnLoad::ON : DevelAsyncImageLoader::PreMultiplyOnLoad::OFF;
    mAsyncLoader->ApplyMask(textureInfo.textureId, pixelBuffer, maskPixelBuffer, textureInfo.scaleFactor, textureInfo.cropToMask, premultiplyOnLoad);
  }
}

void TextureManager::UploadTextures(std::vector<Devel::PixelBuffer>& pixelBuffers, TextureManager::TextureInfo& textureInfo)
{
  if(!pixelBuffers.empty() && textureInfo.loadState != LoadState::UPLOADED && textureInfo.useAtlas != UseAtlas::USE_ATLAS)
  {
    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::General, "  TextureManager::UploadTextures() New Texture for textureId:%d\n", textureInfo.textureId);

    // Check if this pixelBuffer is premultiplied
    textureInfo.preMultiplied = pixelBuffers[0].IsAlphaPreMultiplied();

    auto& renderingAddOn = RenderingAddOn::Get();
    if(renderingAddOn.IsValid())
    {
      renderingAddOn.CreateGeometry(textureInfo.textureId, pixelBuffers[0]);
    }

    // Remove previous textures and insert new textures
    textureInfo.textures.clear();

    for(auto&& pixelBuffer : pixelBuffers)
    {
      Texture   texture   = Texture::New(Dali::TextureType::TEXTURE_2D, pixelBuffer.GetPixelFormat(), pixelBuffer.GetWidth(), pixelBuffer.GetHeight());
      PixelData pixelData = Devel::PixelBuffer::Convert(pixelBuffer);
      texture.Upload(pixelData);
      textureInfo.textures.push_back(texture);
    }
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

  if(info->animatedImageLoading)
  {
    // If loading failed, we don't need to get frameCount and frameInterval.
    if(success)
    {
      info->frameCount    = info->animatedImageLoading.GetImageCount();
      info->frameInterval = info->animatedImageLoading.GetFrameInterval(info->frameIndex);
    }
    info->animatedImageLoading.Reset();
  }

  mLoadingQueueTextureId = textureId;

  // Reverse observer list that we can pop_back the observer.
  std::reverse(info->observerList.Begin(), info->observerList.End());

  while(info->observerList.Count())
  {
    TextureUploadObserver* observer = *(info->observerList.End() - 1u);

    // During LoadComplete() a Control ResourceReady() signal is emitted.
    // During that signal the app may add remove /add Textures (e.g. via
    // ImageViews).
    // It is possible for observers to be removed from the observer list,
    // and it is also possible for the mTextureInfoContainer to be modified,
    // invalidating the reference to the textureInfo struct.
    // Texture load requests for the same URL are deferred until the end of this
    // method.
    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::NotifyObservers() textureId:%d url:%s loadState:%s\n", textureId, info->url.GetUrl().c_str(), GET_LOAD_STATE_STRING(info->loadState));
    // It is possible for the observer to be deleted.
    // Disconnect and remove the observer first.
    observer->DestructionSignal().Disconnect(this, &TextureManager::ObserverDestroyed);

    info->observerList.Erase(info->observerList.End() - 1u);

    EmitLoadComplete(observer, *info, success);

    // Get the textureInfo from the container again as it may have been invalidated.
    TextureCacheIndex textureInfoIndex = mTextureCacheManager.GetCacheIndexFromId(textureId);
    if(textureInfoIndex == INVALID_CACHE_INDEX)
    {
      break; // texture has been removed - can stop.
    }
    info = &mTextureCacheManager[textureInfoIndex];
  }

  mLoadingQueueTextureId = INVALID_TEXTURE_ID;
  ProcessLoadQueue();
  ProcessRemoveQueue();

  if(info->storageType == StorageType::RETURN_PIXEL_BUFFER && info->observerList.Count() == 0)
  {
    Remove(info->textureId, nullptr);
  }
}

void TextureManager::ObserverDestroyed(TextureUploadObserver* observer)
{
  const std::size_t size = mTextureCacheManager.size();
  for(TextureCacheIndex cacheIndex = TextureCacheIndex(TextureManagerType::TEXTURE_CACHE_INDEX_TYPE_LOCAL, 0u); cacheIndex.GetIndex() < size; ++cacheIndex.detailValue.index)
  {
    TextureInfo& textureInfo(mTextureCacheManager[cacheIndex]);
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
      element.mTextureId = INVALID_TEXTURE_ID;
      element.mObserver  = nullptr;
    }
  }
}

Dali::Geometry TextureManager::GetRenderGeometry(const TextureManager::TextureId& textureId, std::uint32_t& frontElements, std::uint32_t& backElements)
{
  return RenderingAddOn::Get().IsValid() ? RenderingAddOn::Get().GetGeometry(textureId, frontElements, backElements) : Geometry();
}

void TextureManager::EmitLoadComplete(TextureUploadObserver* observer, TextureManager::TextureInfo& textureInfo, const bool& success)
{
  if(textureInfo.storageType == StorageType::RETURN_PIXEL_BUFFER)
  {
    observer->LoadComplete(success, TextureUploadObserver::TextureInformation(TextureUploadObserver::ReturnType::PIXEL_BUFFER, textureInfo.pixelBuffer, textureInfo.url.GetUrl(), textureInfo.preMultiplied));
  }
  else
  {
    TextureSet textureSet = GetTextureSet(textureInfo);
    if(textureInfo.isAnimatedImageFormat)
    {
      observer->LoadComplete(success, TextureUploadObserver::TextureInformation(TextureUploadObserver::ReturnType::ANIMATED_IMAGE_TEXTURE, textureInfo.textureId, textureSet, textureInfo.frameCount, textureInfo.frameInterval));
    }
    else
    {
      observer->LoadComplete(success, TextureUploadObserver::TextureInformation(TextureUploadObserver::ReturnType::TEXTURE, textureInfo.textureId, textureSet, (textureInfo.useAtlas == UseAtlas::USE_ATLAS) ? true : false, textureInfo.atlasRect, textureInfo.preMultiplied));
    }
  }
}

TextureSet TextureManager::GetTextureSet(const TextureManager::TextureId& textureId)
{
  TextureSet                textureSet;
  TextureManager::LoadState loadState = mTextureCacheManager.GetTextureStateInternal(textureId);
  if(loadState == TextureManager::LoadState::UPLOADED)
  {
    // LoadComplete has already been called - keep the same texture set
    TextureCacheIndex textureCacheIndex = mTextureCacheManager.GetCacheIndexFromId(textureId);
    if(textureCacheIndex != INVALID_CACHE_INDEX)
    {
      TextureInfo& textureInfo(mTextureCacheManager[textureCacheIndex]);
      textureSet = GetTextureSet(textureInfo);
    }
  }
  else
  {
    DALI_LOG_ERROR("GetTextureSet is failed. texture is not uploaded \n");
  }
  return textureSet;
}

TextureSet TextureManager::GetTextureSet(const TextureManager::TextureInfo& textureInfo)
{
  TextureSet textureSet;

  // LoadComplete has already been called - keep the same texture set
  textureSet = TextureSet::New();
  if(!textureInfo.textures.empty())
  {
    if(textureInfo.textures.size() > 1) // For YUV case
    {
      uint32_t index = 0u;
      for(auto&& texture : textureInfo.textures)
      {
        textureSet.SetTexture(index++, texture);
      }
    }
    else
    {
      textureSet.SetTexture(TEXTURE_INDEX, textureInfo.textures[0]);
      TextureCacheIndex maskCacheIndex = mTextureCacheManager.GetCacheIndexFromId(textureInfo.maskTextureId);
      if(maskCacheIndex != INVALID_CACHE_INDEX)
      {
        TextureInfo& maskTextureInfo(mTextureCacheManager[maskCacheIndex]);
        if(maskTextureInfo.storageType == StorageType::UPLOAD_TO_TEXTURE || maskTextureInfo.storageType == StorageType::KEEP_TEXTURE)
        {
          if(!maskTextureInfo.textures.empty())
          {
            textureSet.SetTexture(MASK_TEXTURE_INDEX, maskTextureInfo.textures[0]);
          }
        }
      }
    }
  }
  return textureSet;
}

void TextureManager::RemoveTextureObserver(TextureManager::TextureInfo& textureInfo, TextureUploadObserver* observer)
{
  // Remove its observer
  if(observer)
  {
    const auto iterEnd = textureInfo.observerList.End();
    const auto iter    = std::find(textureInfo.observerList.Begin(), iterEnd, observer);
    if(iter != iterEnd)
    {
      // Disconnect and remove the observer.
      observer->DestructionSignal().Disconnect(this, &TextureManager::ObserverDestroyed);
      textureInfo.observerList.Erase(iter);
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
