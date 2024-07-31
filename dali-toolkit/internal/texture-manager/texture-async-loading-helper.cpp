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
#include <dali-toolkit/internal/texture-manager/texture-async-loading-helper.h>

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
: mTextureManager(textureManager),
  mLoadTaskId(0u)
{
}

void TextureAsyncLoadingHelper::LoadAnimatedImage(const TextureManager::TextureId                textureId,
                                                  Dali::AnimatedImageLoading                     animatedImageLoading,
                                                  const uint32_t                                 frameIndex,
                                                  const Dali::ImageDimensions&                   desiredSize,
                                                  const Dali::FittingMode::Type                  fittingMode,
                                                  const Dali::SamplingMode::Type                 samplingMode,
                                                  const DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
{
  LoadingTaskPtr loadingTask = new LoadingTask(++mLoadTaskId, animatedImageLoading, frameIndex, desiredSize, fittingMode, samplingMode, preMultiplyOnLoad, MakeCallback(this, &TextureAsyncLoadingHelper::AsyncLoadComplete));
  loadingTask->SetTextureId(textureId);
  Dali::AsyncTaskManager::Get().AddTask(loadingTask);
}

void TextureAsyncLoadingHelper::Load(const TextureManager::TextureId                textureId,
                                     const VisualUrl&                               url,
                                     const Dali::ImageDimensions&                   desiredSize,
                                     const Dali::FittingMode::Type                  fittingMode,
                                     const Dali::SamplingMode::Type                 samplingMode,
                                     const bool                                     orientationCorrection,
                                     const DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad,
                                     const bool                                     loadYuvPlanes)
{
  LoadingTaskPtr loadingTask;
  if(DALI_UNLIKELY(url.IsBufferResource()))
  {
    loadingTask = new LoadingTask(++mLoadTaskId, mTextureManager.GetEncodedImageBuffer(url), desiredSize, fittingMode, samplingMode, orientationCorrection, preMultiplyOnLoad, MakeCallback(this, &TextureAsyncLoadingHelper::AsyncLoadComplete));
  }
  else
  {
    loadingTask = new LoadingTask(++mLoadTaskId, url, desiredSize, fittingMode, samplingMode, orientationCorrection, preMultiplyOnLoad, loadYuvPlanes, MakeCallback(this, &TextureAsyncLoadingHelper::AsyncLoadComplete));
  }

  loadingTask->SetTextureId(textureId);
  Dali::AsyncTaskManager::Get().AddTask(loadingTask);
}

void TextureAsyncLoadingHelper::ApplyMask(const TextureManager::TextureId                textureId,
                                          Devel::PixelBuffer                             pixelBuffer,
                                          Devel::PixelBuffer                             maskPixelBuffer,
                                          const float                                    contentScale,
                                          const bool                                     cropToMask,
                                          const DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
{
  LoadingTaskPtr loadingTask = new LoadingTask(++mLoadTaskId, pixelBuffer, maskPixelBuffer, contentScale, cropToMask, preMultiplyOnLoad, MakeCallback(this, &TextureAsyncLoadingHelper::AsyncLoadComplete));
  loadingTask->SetTextureId(textureId);
  Dali::AsyncTaskManager::Get().AddTask(loadingTask);
}

void TextureAsyncLoadingHelper::AsyncLoadComplete(LoadingTaskPtr task)
{
  // Call TextureManager::AsyncLoadComplete
  if(task->textureId != TextureManager::INVALID_TEXTURE_ID)
  {
    mTextureManager.AsyncLoadComplete(task->textureId, task->pixelBuffers);
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
