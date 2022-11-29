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
#include "loading-task.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
LoadingTask::LoadingTask(uint32_t id, Dali::AnimatedImageLoading animatedImageLoading, uint32_t frameIndex, DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad, CallbackBase* callback)
: AsyncTask(callback),
  url(),
  encodedImageBuffer(),
  id(id),
  textureId(TextureManagerType::INVALID_TEXTURE_ID),
  dimensions(),
  fittingMode(),
  samplingMode(),
  preMultiplyOnLoad(preMultiplyOnLoad),
  maskPixelBuffer(),
  contentScale(1.0f),
  animatedImageLoading(animatedImageLoading),
  frameIndex(frameIndex),
  orientationCorrection(),
  isMaskTask(false),
  cropToMask(false),
  loadPlanes(false),
  isReady(true)
{
}

LoadingTask::LoadingTask(uint32_t id, const VisualUrl& url, ImageDimensions dimensions, FittingMode::Type fittingMode, SamplingMode::Type samplingMode, bool orientationCorrection, DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad, bool loadPlanes, CallbackBase* callback)
: AsyncTask(callback),
  url(url),
  encodedImageBuffer(),
  id(id),
  textureId(TextureManagerType::INVALID_TEXTURE_ID),
  dimensions(dimensions),
  fittingMode(fittingMode),
  samplingMode(samplingMode),
  preMultiplyOnLoad(preMultiplyOnLoad),
  maskPixelBuffer(),
  contentScale(1.0f),
  animatedImageLoading(),
  frameIndex(0u),
  orientationCorrection(orientationCorrection),
  isMaskTask(false),
  cropToMask(false),
  loadPlanes(loadPlanes),
  isReady(true)
{
}

LoadingTask::LoadingTask(uint32_t id, const EncodedImageBuffer& encodedImageBuffer, ImageDimensions dimensions, FittingMode::Type fittingMode, SamplingMode::Type samplingMode, bool orientationCorrection, DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad, CallbackBase* callback)
: AsyncTask(callback),
  url(),
  encodedImageBuffer(encodedImageBuffer),
  id(id),
  textureId(TextureManagerType::INVALID_TEXTURE_ID),
  dimensions(dimensions),
  fittingMode(fittingMode),
  samplingMode(samplingMode),
  preMultiplyOnLoad(preMultiplyOnLoad),
  maskPixelBuffer(),
  contentScale(1.0f),
  animatedImageLoading(),
  frameIndex(0u),
  orientationCorrection(orientationCorrection),
  isMaskTask(false),
  cropToMask(false),
  loadPlanes(false),
  isReady(true)
{
}

LoadingTask::LoadingTask(uint32_t id, Devel::PixelBuffer pixelBuffer, Devel::PixelBuffer maskPixelBuffer, float contentScale, bool cropToMask, DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad, CallbackBase* callback)
: AsyncTask(callback),
  url(""),
  encodedImageBuffer(),
  id(id),
  textureId(TextureManagerType::INVALID_TEXTURE_ID),
  dimensions(),
  fittingMode(),
  samplingMode(),
  preMultiplyOnLoad(preMultiplyOnLoad),
  maskPixelBuffer(maskPixelBuffer),
  contentScale(contentScale),
  animatedImageLoading(),
  frameIndex(0u),
  orientationCorrection(),
  isMaskTask(true),
  cropToMask(cropToMask),
  loadPlanes(false),
  isReady(true)
{
  pixelBuffers.push_back(pixelBuffer);
}

LoadingTask::~LoadingTask()
{
}

void LoadingTask::Process()
{
  isReady = false;
  if(!isMaskTask)
  {
    Load();
  }
  else
  {
    ApplyMask();
  }
  MultiplyAlpha();
  isReady = true;
}

bool LoadingTask::IsReady()
{
  return isReady;
}

void LoadingTask::Load()
{
  Devel::PixelBuffer pixelBuffer;
  if(animatedImageLoading)
  {
    pixelBuffer = animatedImageLoading.LoadFrame(frameIndex);
  }
  else if(encodedImageBuffer)
  {
    pixelBuffer = Dali::LoadImageFromBuffer(encodedImageBuffer.GetRawBuffer(), dimensions, fittingMode, samplingMode, orientationCorrection);
  }
  else if(url.IsValid() && url.IsLocalResource())
  {
    if(loadPlanes)
    {
      Dali::LoadImagePlanesFromFile(url.GetUrl(), pixelBuffers, dimensions, fittingMode, samplingMode, orientationCorrection);
    }
    else
    {
      pixelBuffer = Dali::LoadImageFromFile(url.GetUrl(), dimensions, fittingMode, samplingMode, orientationCorrection);
    }
  }
  else if(url.IsValid())
  {
    pixelBuffer = Dali::DownloadImageSynchronously(url.GetUrl(), dimensions, fittingMode, samplingMode, orientationCorrection);
  }

  if(pixelBuffer)
  {
    pixelBuffers.push_back(pixelBuffer);
  }

  if(pixelBuffers.empty())
  {
    DALI_LOG_ERROR("LoadingTask::Load: Loading is failed: %s\n", url.GetUrl().c_str());
  }
}

void LoadingTask::ApplyMask()
{
  if(!pixelBuffers.empty())
  {
    pixelBuffers[0].ApplyMask(maskPixelBuffer, contentScale, cropToMask);
  }
}

void LoadingTask::MultiplyAlpha()
{
  if(!pixelBuffers.empty() && Pixel::HasAlpha(pixelBuffers[0].GetPixelFormat()))
  {
    if(preMultiplyOnLoad == DevelAsyncImageLoader::PreMultiplyOnLoad::ON)
    {
      pixelBuffers[0].MultiplyColorByAlpha();
    }
  }
}

void LoadingTask::SetTextureId(TextureManagerType::TextureId id)
{
  textureId = id;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
