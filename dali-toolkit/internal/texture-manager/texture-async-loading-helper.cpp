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
#include "texture-async-loading-helper.h"

// EXTERNAL HEADERS
#include <dali/integration-api/debug.h>

// INTERNAL HEADERS
#include <dali-toolkit/internal/image-loader/async-image-loader-impl.h>
#include <dali-toolkit/public-api/image-loader/sync-image-loader.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
#ifdef DEBUG_ENABLED
extern Debug::Filter* gTextureManagerLogFilter; ///< Define at texture-manager-impl.cpp
#endif

TextureAsyncLoadingHelper::TextureAsyncLoadingHelper(TextureManager& textureManager)
: TextureAsyncLoadingHelper(Toolkit::AsyncImageLoader::New(), textureManager, AsyncLoadingInfoContainerType())
{
}

void TextureAsyncLoadingHelper::LoadAnimatedImage(const TextureManager::TextureId&                textureId,
                                                  Dali::AnimatedImageLoading                      animatedImageLoading,
                                                  const std::uint32_t&                            frameIndex,
                                                  const DevelAsyncImageLoader::PreMultiplyOnLoad& preMultiplyOnLoad)
{
  mLoadingInfoContainer.push_back(AsyncLoadingInfo(textureId));
  auto id                             = GetImplementation(mLoader).LoadAnimatedImage(animatedImageLoading, frameIndex, preMultiplyOnLoad);
  mLoadingInfoContainer.back().loadId = id;
}

void TextureAsyncLoadingHelper::Load(const TextureManager::TextureId&                textureId,
                                     const VisualUrl&                                url,
                                     const Dali::ImageDimensions&                    desiredSize,
                                     const Dali::FittingMode::Type&                  fittingMode,
                                     const Dali::SamplingMode::Type&                 samplingMode,
                                     const bool&                                     orientationCorrection,
                                     const DevelAsyncImageLoader::PreMultiplyOnLoad& preMultiplyOnLoad,
                                     const bool&                                     loadYuvPlanes)
{
  mLoadingInfoContainer.push_back(AsyncLoadingInfo(textureId));
  if(DALI_UNLIKELY(url.IsBufferResource()))
  {
    auto id                             = GetImplementation(mLoader).LoadEncodedImageBuffer(mTextureManager.GetEncodedImageBuffer(url.GetUrl()), desiredSize, fittingMode, samplingMode, orientationCorrection, preMultiplyOnLoad);
    mLoadingInfoContainer.back().loadId = id;
  }
  else
  {
    auto id                             = GetImplementation(mLoader).Load(url, desiredSize, fittingMode, samplingMode, orientationCorrection, preMultiplyOnLoad, loadYuvPlanes);
    mLoadingInfoContainer.back().loadId = id;
  }
}

void TextureAsyncLoadingHelper::ApplyMask(const TextureManager::TextureId&                textureId,
                                          Devel::PixelBuffer                              pixelBuffer,
                                          Devel::PixelBuffer                              maskPixelBuffer,
                                          const float&                                    contentScale,
                                          const bool&                                     cropToMask,
                                          const DevelAsyncImageLoader::PreMultiplyOnLoad& preMultiplyOnLoad)
{
  mLoadingInfoContainer.push_back(AsyncLoadingInfo(textureId));
  auto id                             = GetImplementation(mLoader).ApplyMask(pixelBuffer, maskPixelBuffer, contentScale, cropToMask, preMultiplyOnLoad);
  mLoadingInfoContainer.back().loadId = id;
}

TextureAsyncLoadingHelper::TextureAsyncLoadingHelper(TextureAsyncLoadingHelper&& rhs)
: TextureAsyncLoadingHelper(rhs.mLoader, rhs.mTextureManager, std::move(rhs.mLoadingInfoContainer))
{
}

TextureAsyncLoadingHelper::TextureAsyncLoadingHelper(
  Toolkit::AsyncImageLoader       loader,
  TextureManager&                 textureManager,
  AsyncLoadingInfoContainerType&& loadingInfoContainer)
: mLoader(loader),
  mTextureManager(textureManager),
  mLoadingInfoContainer(std::move(loadingInfoContainer))
{
  DevelAsyncImageLoader::PixelBufferLoadedSignal(mLoader).Connect(
    this, &TextureAsyncLoadingHelper::AsyncLoadComplete);
}

void TextureAsyncLoadingHelper::AsyncLoadComplete(uint32_t                         id,
                                                  std::vector<Devel::PixelBuffer>& pixelBuffers)
{
  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureAsyncLoadingHelper::AsyncLoadComplete( loadId :%d )\n", id);
  if(mLoadingInfoContainer.size() >= 1u)
  {
    AsyncLoadingInfo loadingInfo = mLoadingInfoContainer.front();

    // We can assume that First Loading task comes First.
    if(loadingInfo.loadId == id)
    {
      // Call TextureManager::AsyncLoadComplete
      mTextureManager.AsyncLoadComplete(loadingInfo.textureId, pixelBuffers);
    }

    mLoadingInfoContainer.pop_front();
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
