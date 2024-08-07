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
#include <dali-toolkit/internal/texture-manager/texture-manager-impl.h>

// EXTERNAL HEADERS
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/public-api/rendering/geometry.h>

// INTERNAL HEADERS
#include <dali-toolkit/internal/texture-manager/texture-async-loading-helper.h>
#include <dali-toolkit/internal/texture-manager/texture-cache-manager.h>
#include <dali-toolkit/internal/visuals/image/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/rendering-addon.h>

namespace
{
constexpr auto INITIAL_HASH_NUMBER = size_t{0u};

constexpr auto TEXTURE_INDEX      = 0u; ///< The Index for texture
constexpr auto MASK_TEXTURE_INDEX = 1u; ///< The Index for mask texture

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_IMAGE_PERFORMANCE_MARKER, false);
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
const Vector4 FULL_ATLAS_RECT(0.0f, 0.0f, 1.0f, 1.0f); ///< UV Rectangle that covers the full Texture

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

TextureManager::TextureManager(bool loadYuvPlanes)
: mTextureCacheManager(),
  mAsyncLoader(std::unique_ptr<TextureAsyncLoadingHelper>(new TextureAsyncLoadingHelper(*this))),
  mLoadQueue(),
  mLoadingQueueTextureId(INVALID_TEXTURE_ID),
  mRemoveQueue(),
  mLoadYuvPlanes(loadYuvPlanes),
  mRemoveProcessorRegistered(false)
{
  // Initialize the AddOn
  RenderingAddOn::Get();
}

TextureManager::~TextureManager()
{
  if(mRemoveProcessorRegistered && Adaptor::IsAvailable())
  {
    Adaptor::Get().UnregisterProcessorOnce(*this, true);
    mRemoveProcessorRegistered = false;
  }
}

TextureSet TextureManager::LoadAnimatedImageTexture(
  const VisualUrl&                url,
  Dali::AnimatedImageLoading      animatedImageLoading,
  const uint32_t                  frameIndex,
  TextureManager::TextureId&      textureId,
  MaskingDataPointer&             maskInfo,
  const Dali::ImageDimensions&    desiredSize,
  const Dali::FittingMode::Type   fittingMode,
  const Dali::SamplingMode::Type  samplingMode,
  const bool                      synchronousLoading,
  TextureUploadObserver*          textureObserver,
  TextureManager::MultiplyOnLoad& preMultiplyOnLoad)
{
  TextureSet textureSet;

  if(synchronousLoading)
  {
    Devel::PixelBuffer pixelBuffer;
    if(animatedImageLoading)
    {
      pixelBuffer = animatedImageLoading.LoadFrame(frameIndex, desiredSize, fittingMode, samplingMode);
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
        Devel::PixelBuffer maskPixelBuffer = LoadImageFromFile(maskInfo->mAlphaMaskUrl.GetUrl(), desiredSize, fittingMode, samplingMode, true);
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
      maskInfo->mAlphaMaskId = RequestMaskLoad(maskInfo->mAlphaMaskUrl, maskInfo->mPreappliedMasking ? TextureManager::StorageType::KEEP_PIXEL_BUFFER : TextureManager::StorageType::KEEP_TEXTURE);
      alphaMaskId            = maskInfo->mAlphaMaskId;
      if(maskInfo->mPreappliedMasking)
      {
        contentScaleFactor = maskInfo->mContentScaleFactor;
        cropToMask         = maskInfo->mCropToMask;
      }
    }

    textureId = RequestLoadInternal(url, alphaMaskId, textureId, contentScaleFactor, desiredSize, fittingMode, samplingMode, cropToMask, TextureManager::StorageType::UPLOAD_TO_TEXTURE, textureObserver, true, TextureManager::ReloadPolicy::CACHED, preMultiplyOnLoad, animatedImageLoading, frameIndex, false);

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
  const Dali::FittingMode::Type   fittingMode,
  const Dali::SamplingMode::Type  samplingMode,
  const bool                      synchronousLoading,
  TextureUploadObserver*          textureObserver,
  const bool                      orientationCorrection,
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
    RequestLoadInternal(url, INVALID_TEXTURE_ID, INVALID_TEXTURE_ID, 1.0f, desiredSize, fittingMode, samplingMode, false, TextureManager::StorageType::RETURN_PIXEL_BUFFER, textureObserver, orientationCorrection, TextureManager::ReloadPolicy::FORCED, preMultiplyOnLoad, Dali::AnimatedImageLoading(), 0u, false);
  }

  return pixelBuffer;
}

TextureSet TextureManager::LoadTexture(
  const VisualUrl&                   url,
  const Dali::ImageDimensions&       desiredSize,
  const Dali::FittingMode::Type      fittingMode,
  const Dali::SamplingMode::Type     samplingMode,
  MaskingDataPointer&                maskInfo,
  const bool                         synchronousLoading,
  TextureManager::TextureId&         textureId,
  Vector4&                           textureRect,
  Dali::ImageDimensions&             textureRectSize,
  bool&                              atlasingStatus,
  bool&                              loadingStatus,
  TextureUploadObserver*             textureObserver,
  AtlasUploadObserver*               atlasObserver,
  ImageAtlasManagerPtr               imageAtlasManager,
  const bool                         orientationCorrection,
  const TextureManager::ReloadPolicy reloadPolicy,
  TextureManager::MultiplyOnLoad&    preMultiplyOnLoad)
{
  TextureSet textureSet;

  loadingStatus = false;
  textureRect   = FULL_ATLAS_RECT;

  if(VisualUrl::TEXTURE == url.GetProtocolType())
  {
    std::string location = url.GetLocation();
    if(location.size() > 0u)
    {
      TextureId id                  = std::stoi(location);
      auto      externalTextureInfo = mTextureCacheManager.GetExternalTextureInfo(id);
      if(externalTextureInfo.textureSet)
      {
        if(preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD)
        {
          // Change preMultiplyOnLoad value so make caller determine to preMultiplyAlpha or not.
          // TODO : Should we seperate input and output value?
          preMultiplyOnLoad = externalTextureInfo.preMultiplied ? TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD : TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
        }

        TextureId alphaMaskId = INVALID_TEXTURE_ID;
        if(maskInfo && maskInfo->mAlphaMaskUrl.IsValid())
        {
          maskInfo->mAlphaMaskId = RequestMaskLoad(maskInfo->mAlphaMaskUrl, TextureManager::StorageType::KEEP_TEXTURE, synchronousLoading);
          alphaMaskId            = maskInfo->mAlphaMaskId;

          // Create new textureId. this textureId is not same as location
          textureId = RequestLoad(url, alphaMaskId, textureId, 1.0f, desiredSize, fittingMode, samplingMode, false, textureObserver, orientationCorrection, reloadPolicy, preMultiplyOnLoad, synchronousLoading);

          TextureManager::LoadState loadState = mTextureCacheManager.GetTextureStateInternal(textureId);
          if(loadState == TextureManager::LoadState::UPLOADED)
          {
            textureSet = GetTextureSet(textureId);
          }
        }
        else
        {
          // TextureId is same as location
          textureId = id;

          textureSet = TextureSet::New();
          textureSet.SetTexture(TEXTURE_INDEX, externalTextureInfo.textureSet.GetTexture(TEXTURE_INDEX));
        }
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
          maskInfo->mAlphaMaskId = RequestMaskLoad(maskInfo->mAlphaMaskUrl, maskInfo->mPreappliedMasking ? TextureManager::StorageType::KEEP_PIXEL_BUFFER : TextureManager::StorageType::KEEP_TEXTURE, synchronousLoading);
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
          textureId,
          contentScaleFactor,
          desiredSize,
          fittingMode,
          samplingMode,
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
  const VisualUrl&                   url,
  const ImageDimensions&             desiredSize,
  const Dali::FittingMode::Type      fittingMode,
  const Dali::SamplingMode::Type     samplingMode,
  TextureUploadObserver*             observer,
  const bool                         orientationCorrection,
  const TextureManager::ReloadPolicy reloadPolicy,
  TextureManager::MultiplyOnLoad&    preMultiplyOnLoad,
  const bool                         synchronousLoading)
{
  return RequestLoadInternal(url, INVALID_TEXTURE_ID, INVALID_TEXTURE_ID, 1.0f, desiredSize, fittingMode, samplingMode, false, TextureManager::StorageType::UPLOAD_TO_TEXTURE, observer, orientationCorrection, reloadPolicy, preMultiplyOnLoad, Dali::AnimatedImageLoading(), 0u, synchronousLoading);
}

TextureManager::TextureId TextureManager::RequestLoad(
  const VisualUrl&                   url,
  const TextureManager::TextureId    maskTextureId,
  const TextureManager::TextureId    previousTextureId,
  const float                        contentScale,
  const Dali::ImageDimensions&       desiredSize,
  const Dali::FittingMode::Type      fittingMode,
  const Dali::SamplingMode::Type     samplingMode,
  const bool                         cropToMask,
  TextureUploadObserver*             observer,
  const bool                         orientationCorrection,
  const TextureManager::ReloadPolicy reloadPolicy,
  TextureManager::MultiplyOnLoad&    preMultiplyOnLoad,
  const bool                         synchronousLoading)
{
  return RequestLoadInternal(url, maskTextureId, previousTextureId, contentScale, desiredSize, fittingMode, samplingMode, cropToMask, TextureManager::StorageType::UPLOAD_TO_TEXTURE, observer, orientationCorrection, reloadPolicy, preMultiplyOnLoad, Dali::AnimatedImageLoading(), 0u, synchronousLoading);
}

TextureManager::TextureId TextureManager::RequestMaskLoad(
  const VisualUrl&                  maskUrl,
  const TextureManager::StorageType storageType,
  const bool                        synchronousLoading)
{
  // Use the normal load procedure to get the alpha mask.
  auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  return RequestLoadInternal(maskUrl, INVALID_TEXTURE_ID, INVALID_TEXTURE_ID, 1.0f, ImageDimensions(), Dali::FittingMode::SCALE_TO_FILL, Dali::SamplingMode::NO_FILTER, false, storageType, NULL, true, TextureManager::ReloadPolicy::CACHED, preMultiply, Dali::AnimatedImageLoading(), 0u, synchronousLoading);
}

TextureManager::TextureId TextureManager::RequestLoadInternal(
  const VisualUrl&                   url,
  const TextureManager::TextureId    maskTextureId,
  const TextureManager::TextureId    previousTextureId,
  const float                        contentScale,
  const Dali::ImageDimensions&       desiredSize,
  const Dali::FittingMode::Type      fittingMode,
  const Dali::SamplingMode::Type     samplingMode,
  const bool                         cropToMask,
  const TextureManager::StorageType  storageType,
  TextureUploadObserver*             observer,
  const bool                         orientationCorrection,
  const TextureManager::ReloadPolicy reloadPolicy,
  TextureManager::MultiplyOnLoad&    preMultiplyOnLoad,
  Dali::AnimatedImageLoading         animatedImageLoading,
  const uint32_t                     frameIndex,
  const bool                         synchronousLoading)
{
  TextureHash       textureHash   = INITIAL_HASH_NUMBER;
  TextureCacheIndex cacheIndex    = INVALID_CACHE_INDEX;
  bool              loadYuvPlanes = (mLoadYuvPlanes && maskTextureId == INVALID_TEXTURE_ID && storageType == TextureManager::StorageType::UPLOAD_TO_TEXTURE);

  if(storageType != TextureManager::StorageType::RETURN_PIXEL_BUFFER)
  {
    textureHash = mTextureCacheManager.GenerateHash(url, desiredSize, fittingMode, samplingMode, maskTextureId, cropToMask, orientationCorrection, frameIndex);

    // Look up the texture by hash. Note: The extra parameters are used in case of a hash collision.
    cacheIndex = mTextureCacheManager.FindCachedTexture(textureHash, url, desiredSize, fittingMode, samplingMode, storageType, maskTextureId, cropToMask, orientationCorrection, preMultiplyOnLoad, (animatedImageLoading) ? true : false, frameIndex);
  }

  TextureManager::TextureId textureId = INVALID_TEXTURE_ID;
  // Check if the requested Texture exists in the cache.
  if(cacheIndex != INVALID_CACHE_INDEX)
  {
    if(TextureManager::ReloadPolicy::CACHED == reloadPolicy || TextureManager::INVALID_TEXTURE_ID == previousTextureId)
    {
      // Mark this texture being used by another client resource, or Reload forced without request load before.
      // Forced reload which have current texture before, would replace the current texture.
      // without the need for incrementing the reference count.
      ++(mTextureCacheManager[cacheIndex].referenceCount);
    }
    textureId = mTextureCacheManager[cacheIndex].textureId;

    // Update preMultiplyOnLoad value. It should be changed according to preMultiplied value of the cached info.
    preMultiplyOnLoad = mTextureCacheManager[cacheIndex].preMultiplied ? TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD : TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::General, "TextureManager::RequestLoad( url=%s size=%hux%hu observer=%p ) Using cached texture id@%d, textureId=%d, maskTextureId=%d, prevTextureId=%d, frameindex=%d, premultiplied=%d, refCount=%d\n", url.GetUrl().c_str(), desiredSize.GetWidth(), desiredSize.GetHeight(), observer, cacheIndex.GetIndex(), textureId, maskTextureId, previousTextureId, frameIndex, mTextureCacheManager[cacheIndex].preMultiplied ? 1 : 0, static_cast<int>(mTextureCacheManager[cacheIndex].referenceCount));
  }

  if(textureId == INVALID_TEXTURE_ID) // There was no caching, or caching not required
  {
    textureId = mTextureCacheManager.GenerateTextureId();

    bool preMultiply = (preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD);

    // Cache new texutre, and get cacheIndex.
    cacheIndex = mTextureCacheManager.AppendCache(TextureInfo(textureId, maskTextureId, url, desiredSize, contentScale, fittingMode, samplingMode, false, cropToMask, textureHash, orientationCorrection, preMultiply, animatedImageLoading, frameIndex, loadYuvPlanes));
    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::General, "TextureManager::RequestLoad( url=%s size=%hux%hu observer=%p ) New texture, cacheIndex:%d, textureId=%d, maskTextureId=%d, frameindex=%d premultiply=%d\n", url.GetUrl().c_str(), desiredSize.GetWidth(), desiredSize.GetHeight(), observer, cacheIndex.GetIndex(), textureId, maskTextureId, frameIndex, preMultiply);
  }

  // The below code path is common whether we are using the cache or not.
  // The textureInfoIndex now refers to either a pre-existing cached TextureInfo,
  // or a new TextureInfo just created.
  TextureInfo& textureInfo(mTextureCacheManager[cacheIndex]);
  textureInfo.maskTextureId         = maskTextureId;
  textureInfo.storageType           = storageType;
  textureInfo.orientationCorrection = orientationCorrection;

  // the case using external texture has already been loaded texture, so change its status to WAITING_FOR_MASK.
  if(url.GetProtocolType() == VisualUrl::TEXTURE)
  {
    if(textureInfo.loadState != TextureManager::LoadState::UPLOADED)
    {
      textureInfo.preMultiplied = (preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD);
      textureInfo.loadState     = TextureManager::LoadState::WAITING_FOR_MASK;
    }
  }

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
    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Verbose, "TextureManager::RequestLoad( url=%s size=%hux%hu observer=%p ) ForcedReload cacheIndex:%d, textureId=%d, maskTextureId=%d, prevTextureId=%d\n", url.GetUrl().c_str(), desiredSize.GetWidth(), desiredSize.GetHeight(), observer, cacheIndex.GetIndex(), textureId, maskTextureId, previousTextureId);
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
        // Do not observe even we reload forced when texture is already loading state.
        if(TextureManager::ReloadPolicy::CACHED == reloadPolicy || TextureManager::INVALID_TEXTURE_ID == previousTextureId)
        {
          ObserveTexture(textureInfo, observer);
        }
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
        if(observer && textureInfo.storageType == TextureManager::StorageType::RETURN_PIXEL_BUFFER)
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
      if(url.GetProtocolType() == VisualUrl::TEXTURE)
      {
        // Get external textureSet from cacheManager.
        std::string location = textureInfo.url.GetLocation();
        if(!location.empty())
        {
          TextureId id                  = std::stoi(location);
          auto      externalTextureInfo = mTextureCacheManager.GetExternalTextureInfo(id);
          textureInfo.textures.push_back(externalTextureInfo.textureSet.GetTexture(0));
          textureInfo.loadState = TextureManager::LoadState::UPLOADED;
        }
      }
      else
      {
        std::vector<Devel::PixelBuffer> pixelBuffers;
        LoadImageSynchronously(url, desiredSize, fittingMode, samplingMode, orientationCorrection, loadYuvPlanes, pixelBuffers);

        if(pixelBuffers.empty())
        {
          // If pixelBuffer loading is failed in synchronously, call RequestRemove() method.
          RequestRemove(textureId, nullptr);
          return INVALID_TEXTURE_ID;
        }

        if(storageType == TextureManager::StorageType::KEEP_PIXEL_BUFFER) // For the mask image loading.
        {
          textureInfo.pixelBuffer = pixelBuffers[0]; // Store the pixel data
          textureInfo.loadState   = TextureManager::LoadState::LOAD_FINISHED;
        }
        else // For the image loading.
        {
          Texture maskTexture;
          if(maskTextureId != INVALID_TEXTURE_ID)
          {
            TextureCacheIndex maskCacheIndex = mTextureCacheManager.GetCacheIndexFromId(maskTextureId);
            if(maskCacheIndex != INVALID_CACHE_INDEX)
            {
              if(mTextureCacheManager[maskCacheIndex].storageType == TextureManager::StorageType::KEEP_PIXEL_BUFFER)
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
  }

  return textureId;
}

void TextureManager::RequestRemove(const TextureManager::TextureId textureId, TextureUploadObserver* observer)
{
  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::General, "TextureManager::RequestRemove( textureId=%d observer=%p )\n", textureId, observer);

  // Queue to remove.
  if(textureId != INVALID_TEXTURE_ID)
  {
    TextureCacheIndex textureCacheIndex = mTextureCacheManager.GetCacheIndexFromId(textureId);
    if(textureCacheIndex != INVALID_CACHE_INDEX)
    {
      if(observer)
      {
        // Remove observer from cached texture info
        TextureInfo& textureInfo(mTextureCacheManager[textureCacheIndex]);
        RemoveTextureObserver(textureInfo, observer);
      }

      mRemoveQueue.PushBack(textureId);

      if(!mRemoveProcessorRegistered && Adaptor::IsAvailable())
      {
        mRemoveProcessorRegistered = true;
        Adaptor::Get().RegisterProcessorOnce(*this, true);
      }
    }
  }
}

void TextureManager::Remove(const TextureManager::TextureId textureId)
{
  if(textureId != INVALID_TEXTURE_ID)
  {
    TextureCacheIndex textureCacheIndex = mTextureCacheManager.GetCacheIndexFromId(textureId);
    if(textureCacheIndex != INVALID_CACHE_INDEX)
    {
      TextureManager::TextureId maskTextureId = INVALID_TEXTURE_ID;
      TextureInfo&              textureInfo(mTextureCacheManager[textureCacheIndex]);
      // We only need to consider maskTextureId when texture's loadState is not cancelled. Because it is already deleted.
      if(textureInfo.loadState != TextureManager::LoadState::CANCELLED && textureInfo.loadState != TextureManager::LoadState::MASK_CANCELLED)
      {
        if(textureInfo.maskTextureId != INVALID_TEXTURE_ID)
        {
          maskTextureId = textureInfo.maskTextureId;
        }
      }

      DALI_LOG_INFO(gTextureManagerLogFilter, Debug::General, "TextureManager::Remove( textureId=%d ) cacheIndex:%d removal maskTextureId=%d, loadState=%s\n", textureId, textureCacheIndex.GetIndex(), maskTextureId, GET_LOAD_STATE_STRING(textureInfo.loadState));

      // Remove textureId in CacheManager. Now, textureInfo is invalidate.
      mTextureCacheManager.RemoveCache(textureInfo);

      // Remove maskTextureId in CacheManager
      if(maskTextureId != INVALID_TEXTURE_ID)
      {
        TextureCacheIndex maskCacheIndex = mTextureCacheManager.GetCacheIndexFromId(maskTextureId);
        if(maskCacheIndex != INVALID_CACHE_INDEX)
        {
          TextureInfo& maskTextureInfo(mTextureCacheManager[maskCacheIndex]);

          DALI_LOG_INFO(gTextureManagerLogFilter, Debug::General, "TextureManager::Remove mask texture( maskTextureId=%d ) cacheIndex:%d, loadState=%s\n", maskTextureId, maskCacheIndex.GetIndex(), GET_LOAD_STATE_STRING(maskTextureInfo.loadState));

          mTextureCacheManager.RemoveCache(maskTextureInfo);
        }
      }
    }
  }
}

void TextureManager::ProcessRemoveQueue()
{
  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_TEXTURE_MANAGER_PROCESS_REMOVE_QUEUE", [&](std::ostringstream& oss) {
    oss << "[" << mRemoveQueue.Count() << "]";
  });

  // Note that RemoveQueue is not be changed during Remove().
  for(auto&& textureId : mRemoveQueue)
  {
    if(textureId != INVALID_TEXTURE_ID)
    {
      Remove(textureId);
    }
  }

  mRemoveQueue.Clear();

  DALI_TRACE_END(gTraceFilter, "DALI_TEXTURE_MANAGER_PROCESS_REMOVE_QUEUE");
}

void TextureManager::Process(bool postProcessor)
{
  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::General, "TextureManager::Process()\n");

  mRemoveProcessorRegistered = false;
  ProcessRemoveQueue();
}

void TextureManager::LoadImageSynchronously(
  const VisualUrl&                 url,
  const Dali::ImageDimensions&     desiredSize,
  const Dali::FittingMode::Type    fittingMode,
  const Dali::SamplingMode::Type   samplingMode,
  const bool                       orientationCorrection,
  const bool                       loadYuvPlanes,
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

void TextureManager::LoadOrQueueTexture(TextureManager::TextureInfo& textureInfo, TextureUploadObserver* observer)
{
  switch(textureInfo.loadState)
  {
    case TextureManager::LoadState::NOT_STARTED:
    case TextureManager::LoadState::LOAD_FAILED:
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
    case TextureManager::LoadState::UPLOADED:
    {
      if(mLoadingQueueTextureId != INVALID_TEXTURE_ID)
      {
        QueueLoadTexture(textureInfo, observer);
      }
      else
      {
        // The Texture has already loaded. The other observers have already been notified.
        // We need to send a "late" loaded notification for this observer.
        if(observer)
        {
          EmitLoadComplete(observer, textureInfo, true);
        }
      }
      break;
    }
    case TextureManager::LoadState::LOADING:
    case TextureManager::LoadState::CANCELLED:
    case TextureManager::LoadState::MASK_CANCELLED:
    case TextureManager::LoadState::LOAD_FINISHED:
    case TextureManager::LoadState::WAITING_FOR_MASK:
    case TextureManager::LoadState::MASK_APPLYING:
    case TextureManager::LoadState::MASK_APPLIED:
    {
      break;
    }
  }
}

void TextureManager::QueueLoadTexture(const TextureManager::TextureInfo& textureInfo, TextureUploadObserver* observer)
{
  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Verbose, "Add observer to observer queue (textureId:%d, observer:%p)\n", textureInfo.textureId, observer);

  const auto& textureId = textureInfo.textureId;
  mLoadQueue.PushBack(QueueElement(textureId, observer));

  if(observer)
  {
    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Verbose, "  Connect DestructionSignal to observer:%p\n", observer);
    observer->DestructionSignal().Connect(this, &TextureManager::ObserverDestroyed);
  }
}

void TextureManager::LoadTexture(TextureManager::TextureInfo& textureInfo, TextureUploadObserver* observer)
{
  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::LoadTexture(): size:%hux%hu url:%s sync:%s\n", textureInfo.desiredSize.GetWidth(), textureInfo.desiredSize.GetHeight(), textureInfo.url.GetUrl().c_str(), textureInfo.loadSynchronously ? "T" : "F");
  textureInfo.loadState = TextureManager::LoadState::LOADING;
  if(!textureInfo.loadSynchronously)
  {
    auto premultiplyOnLoad = (textureInfo.preMultiplyOnLoad && textureInfo.maskTextureId == INVALID_TEXTURE_ID) ? DevelAsyncImageLoader::PreMultiplyOnLoad::ON : DevelAsyncImageLoader::PreMultiplyOnLoad::OFF;
    if(textureInfo.animatedImageLoading)
    {
      mAsyncLoader->LoadAnimatedImage(textureInfo.textureId, textureInfo.animatedImageLoading, textureInfo.frameIndex, textureInfo.desiredSize, textureInfo.fittingMode, textureInfo.samplingMode, premultiplyOnLoad);
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

      DALI_LOG_INFO(gTextureManagerLogFilter, Debug::General, "TextureManager::ProcessLoadQueue() textureId=%d, observer=%p, cacheIndex=@%d, loadState:%s\n", element.mTextureId, element.mObserver, cacheIndex.GetIndex(), GET_LOAD_STATE_STRING(textureInfo.loadState));

      if((textureInfo.loadState == TextureManager::LoadState::UPLOADED) ||
         (textureInfo.loadState == TextureManager::LoadState::LOAD_FINISHED &&
          textureInfo.storageType == TextureManager::StorageType::RETURN_PIXEL_BUFFER))
      {
        if(element.mObserver)
        {
          DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Verbose, "  Disconnect DestructionSignal to observer:%p\n", element.mObserver);
          element.mObserver->DestructionSignal().Disconnect(this, &TextureManager::ObserverDestroyed);

          EmitLoadComplete(element.mObserver, textureInfo, true);
        }
      }
      else if(textureInfo.loadState == TextureManager::LoadState::LOADING)
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

void TextureManager::ObserveTexture(TextureManager::TextureInfo& textureInfo,
                                    TextureUploadObserver*       observer)
{
  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::ObserveTexture(): size:%hux%hu url:%s observer:%p\n", textureInfo.desiredSize.GetWidth(), textureInfo.desiredSize.GetHeight(), textureInfo.url.GetUrl().c_str(), observer);

  if(observer)
  {
    textureInfo.observerList.PushBack(observer);

    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Verbose, "  Connect DestructionSignal to observer:%p\n", observer);
    observer->DestructionSignal().Connect(this, &TextureManager::ObserverDestroyed);
  }
}

void TextureManager::AsyncLoadComplete(const TextureManager::TextureId textureId, std::vector<Devel::PixelBuffer>& pixelBuffers)
{
  TextureCacheIndex cacheIndex = mTextureCacheManager.GetCacheIndexFromId(textureId);
  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::AsyncLoadComplete( textureId:%d CacheIndex:%d )\n", textureId, cacheIndex.GetIndex());
  if(cacheIndex != INVALID_CACHE_INDEX)
  {
    TextureInfo& textureInfo(mTextureCacheManager[cacheIndex]);

    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "  textureId:%d size:%hux%hu Url:%s CacheIndex:%d LoadState: %s\n", textureInfo.textureId, textureInfo.desiredSize.GetWidth(), textureInfo.desiredSize.GetHeight(), textureInfo.url.GetUrl().c_str(), cacheIndex.GetIndex(), GET_LOAD_STATE_STRING(textureInfo.loadState));
    if(textureInfo.loadState != TextureManager::LoadState::CANCELLED && textureInfo.loadState != TextureManager::LoadState::MASK_CANCELLED)
    {
      // textureInfo can be invalidated after this call (as the mTextureInfoContainer may be modified)
      PostLoad(textureInfo, pixelBuffers);
    }
    else
    {
      Remove(textureInfo.textureId);
    }
  }
}

void TextureManager::PostLoad(TextureManager::TextureInfo& textureInfo, std::vector<Devel::PixelBuffer>& pixelBuffers)
{
  if(!pixelBuffers.empty()) ///< Load success
  {
    if(pixelBuffers.size() == 1)
    {
      Devel::PixelBuffer pixelBuffer = pixelBuffers[0];
      if(pixelBuffer && (pixelBuffer.GetWidth() != 0) && (pixelBuffer.GetHeight() != 0))
      {
        textureInfo.preMultiplied = pixelBuffer.IsAlphaPreMultiplied();

        if(textureInfo.storageType == TextureManager::StorageType::UPLOAD_TO_TEXTURE)
        {
          // If there is a mask texture ID associated with this texture, then apply the mask
          // if it's already loaded. If it hasn't, and the mask is still loading,
          // wait for the mask to finish loading.
          // note, If the texture is already uploaded synchronously during loading,
          // we don't need to apply mask.
          if(textureInfo.loadState != TextureManager::LoadState::UPLOADED &&
             textureInfo.maskTextureId != INVALID_TEXTURE_ID)
          {
            if(textureInfo.loadState == TextureManager::LoadState::MASK_APPLYING)
            {
              textureInfo.loadState = TextureManager::LoadState::MASK_APPLIED;
              UploadTextures(pixelBuffers, textureInfo);
              NotifyObservers(textureInfo, true);
            }
            else
            {
              LoadState maskLoadState = mTextureCacheManager.GetTextureStateInternal(textureInfo.maskTextureId);
              textureInfo.pixelBuffer = pixelBuffer; // Store the pixel buffer temporarily
              if(maskLoadState == TextureManager::LoadState::LOADING)
              {
                textureInfo.loadState = TextureManager::LoadState::WAITING_FOR_MASK;
              }
              else if(maskLoadState == TextureManager::LoadState::LOAD_FINISHED || maskLoadState == TextureManager::LoadState::UPLOADED)
              {
                TextureCacheIndex maskCacheIndex = mTextureCacheManager.GetCacheIndexFromId(textureInfo.maskTextureId);
                if(maskCacheIndex != INVALID_CACHE_INDEX)
                {
                  TextureInfo& maskTextureInfo(mTextureCacheManager[maskCacheIndex]);
                  if(maskTextureInfo.storageType == TextureManager::StorageType::KEEP_PIXEL_BUFFER)
                  {
                    ApplyMask(textureInfo, textureInfo.maskTextureId);
                  }
                  else if(maskTextureInfo.storageType == TextureManager::StorageType::KEEP_TEXTURE)
                  {
                    // Upload image texture. textureInfo.loadState will be UPLOADED.
                    UploadTextures(pixelBuffers, textureInfo);

                    // notify mask texture set.
                    NotifyObservers(textureInfo, true);
                  }
                }
              }
              else // maskLoadState == TextureManager::LoadState::LOAD_FAILED
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
          textureInfo.loadState   = TextureManager::LoadState::LOAD_FINISHED;

          if(textureInfo.storageType == TextureManager::StorageType::RETURN_PIXEL_BUFFER)
          {
            NotifyObservers(textureInfo, true);
          }
          else // for the TextureManager::StorageType::KEEP_PIXEL_BUFFER and TextureManager::StorageType::KEEP_TEXTURE
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
      textureInfo.preMultiplied = false;

      UploadTextures(pixelBuffers, textureInfo);
      NotifyObservers(textureInfo, true);
    }
  }
  else ///< Load fail
  {
    textureInfo.loadState = TextureManager::LoadState::LOAD_FAILED;
    if(textureInfo.storageType == TextureManager::StorageType::KEEP_PIXEL_BUFFER || textureInfo.storageType == TextureManager::StorageType::KEEP_TEXTURE)
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
  if(maskTextureInfo.loadState == TextureManager::LoadState::LOAD_FINISHED &&
     maskTextureInfo.storageType == TextureManager::StorageType::KEEP_TEXTURE)
  {
    // Upload mask texture. textureInfo.loadState will be UPLOADED.
    std::vector<Devel::PixelBuffer> pixelBuffers;
    pixelBuffers.push_back(maskTextureInfo.pixelBuffer);
    UploadTextures(pixelBuffers, maskTextureInfo);
  }

  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::CheckForWaitingTexture(): maskTextureId=%d, maskTextureUrl=%s\n", maskTextureInfo.textureId, maskTextureInfo.url.GetUrl().c_str());

  // Search the cache, checking if any texture has this texture id as a maskTextureId
  const size_t size = mTextureCacheManager.size();

  // Keep notify observer required textureIds.
  // Note : NotifyObservers can change mTextureCacheManager cache struct. We should check id's validation before notify.
  std::vector<TextureId> notifyRequiredTextureIds;

  // TODO : Refactorize here to not iterate whole cached image.
  for(TextureCacheIndex cacheIndex = TextureCacheIndex(TextureManagerType::TEXTURE_CACHE_INDEX_TYPE_LOCAL, 0u); cacheIndex.GetIndex() < size; ++cacheIndex.detailValue.index)
  {
    if(mTextureCacheManager[cacheIndex].maskTextureId == maskTextureInfo.textureId &&
       mTextureCacheManager[cacheIndex].loadState == TextureManager::LoadState::WAITING_FOR_MASK)
    {
      TextureInfo& textureInfo(mTextureCacheManager[cacheIndex]);

      if(maskTextureInfo.loadState == TextureManager::LoadState::LOAD_FINISHED)
      {
        if(maskTextureInfo.storageType == TextureManager::StorageType::KEEP_PIXEL_BUFFER)
        {
          ApplyMask(textureInfo, maskTextureInfo.textureId);
        }
      }
      else if(maskTextureInfo.loadState == TextureManager::LoadState::UPLOADED)
      {
        if(maskTextureInfo.storageType == TextureManager::StorageType::KEEP_TEXTURE)
        {
          if(textureInfo.url.GetProtocolType() == VisualUrl::TEXTURE)
          {
            // Get external textureSet from cacheManager.
            std::string location = textureInfo.url.GetLocation();
            if(!location.empty())
            {
              TextureId id                  = std::stoi(location);
              auto      externalTextureInfo = mTextureCacheManager.GetExternalTextureInfo(id);
              textureInfo.textures.push_back(externalTextureInfo.textureSet.GetTexture(0));
              textureInfo.loadState = TextureManager::LoadState::UPLOADED;
            }
          }
          else
          {
            // Upload image texture. textureInfo.loadState will be UPLOADED.
            std::vector<Devel::PixelBuffer> pixelBuffers;
            pixelBuffers.push_back(textureInfo.pixelBuffer);
            UploadTextures(pixelBuffers, textureInfo);
          }

          // Increase reference counts for notify required textureId.
          // Now we can assume that we don't remove & re-assign this textureId
          // during NotifyObserver signal emit.
          maskTextureInfo.referenceCount++;
          textureInfo.referenceCount++;

          DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::CheckForWaitingTexture(): Ready to notify textureId=%d\n", textureInfo.textureId);

          notifyRequiredTextureIds.push_back(textureInfo.textureId);
        }
      }
      else // maskTextureInfo.loadState == TextureManager::LoadState::LOAD_FAILED
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
    RequestRemove(textureId, nullptr);
  }
}

void TextureManager::ApplyMask(TextureManager::TextureInfo& textureInfo, const TextureManager::TextureId maskTextureId)
{
  TextureCacheIndex maskCacheIndex = mTextureCacheManager.GetCacheIndexFromId(maskTextureId);
  if(maskCacheIndex != INVALID_CACHE_INDEX)
  {
    Devel::PixelBuffer maskPixelBuffer = mTextureCacheManager[maskCacheIndex].pixelBuffer;
    Devel::PixelBuffer pixelBuffer     = textureInfo.pixelBuffer;
    textureInfo.pixelBuffer.Reset();

    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::ApplyMask(): size:%hux%hu url:%s sync:%s\n", textureInfo.desiredSize.GetWidth(), textureInfo.desiredSize.GetHeight(), textureInfo.url.GetUrl().c_str(), textureInfo.loadSynchronously ? "T" : "F");

    textureInfo.loadState  = TextureManager::LoadState::MASK_APPLYING;
    auto premultiplyOnLoad = textureInfo.preMultiplyOnLoad ? DevelAsyncImageLoader::PreMultiplyOnLoad::ON : DevelAsyncImageLoader::PreMultiplyOnLoad::OFF;
    mAsyncLoader->ApplyMask(textureInfo.textureId, pixelBuffer, maskPixelBuffer, textureInfo.scaleFactor, textureInfo.cropToMask, premultiplyOnLoad);
  }
}

void TextureManager::UploadTextures(std::vector<Devel::PixelBuffer>& pixelBuffers, TextureManager::TextureInfo& textureInfo)
{
  if(!pixelBuffers.empty() && textureInfo.loadState != TextureManager::LoadState::UPLOADED)
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
  textureInfo.loadState = TextureManager::LoadState::UPLOADED;
}

void TextureManager::NotifyObservers(TextureManager::TextureInfo& textureInfo, const bool success)
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
    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureManager::NotifyObservers() observer:%p textureId:%d size:%hux%hu url:%s loadState:%s\n", observer, textureId, info->desiredSize.GetWidth(), info->desiredSize.GetHeight(), info->url.GetUrl().c_str(), GET_LOAD_STATE_STRING(info->loadState));
    // It is possible for the observer to be deleted.
    // Disconnect and remove the observer first.
    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Verbose, "  Disconnect DestructionSignal to observer:%p\n", observer);
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

  if(info->storageType == TextureManager::StorageType::RETURN_PIXEL_BUFFER && info->observerList.Count() == 0)
  {
    RequestRemove(info->textureId, nullptr);
  }
}

void TextureManager::ObserverDestroyed(TextureUploadObserver* observer)
{
  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Verbose, "TextureManager::ObserverDestroyed() observer:%p\n", observer);

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
      DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Verbose, "Remove observer from observer queue (textureId:%d, observer:%p)\n", element.mTextureId, element.mObserver);
      element.mTextureId = INVALID_TEXTURE_ID;
      element.mObserver  = nullptr;
    }
  }
}

Dali::Geometry TextureManager::GetRenderGeometry(const TextureManager::TextureId textureId, uint32_t& frontElements, uint32_t& backElements)
{
  return RenderingAddOn::Get().IsValid() ? RenderingAddOn::Get().GetGeometry(textureId, frontElements, backElements) : Geometry();
}

void TextureManager::EmitLoadComplete(TextureUploadObserver* observer, TextureManager::TextureInfo& textureInfo, const bool success)
{
  if(textureInfo.storageType == TextureManager::StorageType::RETURN_PIXEL_BUFFER)
  {
    observer->LoadComplete(success, TextureUploadObserver::TextureInformation(TextureUploadObserver::ReturnType::PIXEL_BUFFER, textureInfo.pixelBuffer, textureInfo.url.GetUrl(), textureInfo.preMultiplied));
  }
  else
  {
    TextureSet textureSet = GetTextureSet(textureInfo);
    if(textureInfo.isAnimatedImageFormat)
    {
      observer->LoadComplete(success, TextureUploadObserver::TextureInformation(TextureUploadObserver::ReturnType::ANIMATED_IMAGE_TEXTURE, textureInfo.textureId, textureSet, textureInfo.frameCount, textureInfo.frameInterval, textureInfo.preMultiplied));
    }
    else
    {
      observer->LoadComplete(success, TextureUploadObserver::TextureInformation(TextureUploadObserver::ReturnType::TEXTURE, textureInfo.textureId, textureSet, textureInfo.preMultiplied));
    }
  }
}

TextureSet TextureManager::GetTextureSet(const TextureManager::TextureId textureId)
{
  TextureSet                textureSet;
  TextureManager::LoadState loadState = mTextureCacheManager.GetTextureStateInternal(textureId);
  if(loadState == TextureManager::LoadState::UPLOADED)
  {
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

  // Always create new TextureSet here, so we don't share same TextureSets for multiple visuals.
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
        if(maskTextureInfo.storageType == TextureManager::StorageType::UPLOAD_TO_TEXTURE || maskTextureInfo.storageType == TextureManager::StorageType::KEEP_TEXTURE)
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
  if(observer)
  {
    const auto iterEnd = textureInfo.observerList.End();
    const auto iter    = std::find(textureInfo.observerList.Begin(), iterEnd, observer);
    if(iter != iterEnd)
    {
      // Disconnect and remove the observer.
      DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Verbose, "  Disconnect DestructionSignal to observer:%p\n", observer);
      observer->DestructionSignal().Disconnect(this, &TextureManager::ObserverDestroyed);
      textureInfo.observerList.Erase(iter);
    }
    else
    {
      // Given textureId might exist at load queue.
      // Remove observer from the LoadQueue
      for(auto&& element : mLoadQueue)
      {
        if(element.mTextureId == textureInfo.textureId && element.mObserver == observer)
        {
          DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Verbose, "Remove observer from observer queue (textureId:%d, observer:%p)\n", element.mTextureId, element.mObserver);
          DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Verbose, "  Disconnect DestructionSignal to observer:%p\n", observer);
          observer->DestructionSignal().Disconnect(this, &TextureManager::ObserverDestroyed);
          element.mObserver = nullptr;
          break;
        }
      }
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
