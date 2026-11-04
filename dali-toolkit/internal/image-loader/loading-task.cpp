/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/image-loader/loading-task.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/image-loading-devel.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer-operations-devel.h>
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/devel-api/images/pixel-buffer-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <dali/integration-api/trace.h>
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>

#ifdef TRACE_ENABLED
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>
#endif

namespace DALI_NAMESPACE
{
namespace Toolkit
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_IMAGE_PERFORMANCE_MARKER, false);

#ifdef TRACE_ENABLED
uint64_t GetNanoseconds()
{
  // Get the time of a monotonic clock since its epoch.
  auto epoch    = std::chrono::steady_clock::now().time_since_epoch();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);
  return static_cast<uint64_t>(duration.count());
}
#endif
} // namespace

LoadingTask::LoadingTask(uint32_t initId, Dali::AnimatedImageLoading initAnimatedImageLoading, uint32_t initFrameIndex, DevelAsyncImageLoader::PreMultiplyOnLoad initPreMultiplyOnLoad, CallbackBase* callback)
: AsyncTask(callback),
  url(),
  encodedImageBuffer(),
  id(initId),
  textureId(TextureManagerType::INVALID_TEXTURE_ID),
  dimensions(),
  samplingMode(SamplingMode::BOX_THEN_LINEAR),
  preMultiplyOnLoad(initPreMultiplyOnLoad),
  maskPixelBuffer(),
  contentScale(1.0f),
  animatedImageLoading(initAnimatedImageLoading),
  frameIndex(initFrameIndex),
  orientationCorrection(),
  isMaskTask(false),
  cropToMask(false),
  loadPlanes(false)
{
}

LoadingTask::LoadingTask(uint32_t initId, Dali::AnimatedImageLoading initAnimatedImageLoading, uint32_t initFrameIndex, ImageDimensions initDimensions, SamplingMode::Type initSamplingMode, DevelAsyncImageLoader::PreMultiplyOnLoad initPreMultiplyOnLoad, bool initLoadPlanes, CallbackBase* callback)
: AsyncTask(callback),
  url(),
  encodedImageBuffer(),
  id(initId),
  textureId(TextureManagerType::INVALID_TEXTURE_ID),
  dimensions(initDimensions),
  samplingMode(initSamplingMode),
  preMultiplyOnLoad(initPreMultiplyOnLoad),
  maskPixelBuffer(),
  contentScale(1.0f),
  animatedImageLoading(initAnimatedImageLoading),
  frameIndex(initFrameIndex),
  orientationCorrection(),
  isMaskTask(false),
  cropToMask(false),
  loadPlanes(initLoadPlanes)
{
}

LoadingTask::LoadingTask(uint32_t initId, const VisualUrl& initUrl, ImageDimensions initDimensions, SamplingMode::Type initSamplingMode, bool initOrientationCorrection, DevelAsyncImageLoader::PreMultiplyOnLoad initPreMultiplyOnLoad, bool initLoadPlanes, CallbackBase* callback)
: AsyncTask(callback, initUrl.GetProtocolType() == VisualUrl::ProtocolType::REMOTE ? AsyncTask::PriorityType::LOW : AsyncTask::PriorityType::HIGH),
  url(initUrl),
  encodedImageBuffer(),
  id(initId),
  textureId(TextureManagerType::INVALID_TEXTURE_ID),
  dimensions(initDimensions),
  samplingMode(initSamplingMode),
  preMultiplyOnLoad(initPreMultiplyOnLoad),
  maskPixelBuffer(),
  contentScale(1.0f),
  animatedImageLoading(),
  frameIndex(0u),
  orientationCorrection(initOrientationCorrection),
  isMaskTask(false),
  cropToMask(false),
  loadPlanes(initLoadPlanes)
{
}

LoadingTask::LoadingTask(uint32_t initId, const EncodedImageBuffer& initEncodedImageBuffer, ImageDimensions initDimensions, SamplingMode::Type initSamplingMode, bool initOrientationCorrection, DevelAsyncImageLoader::PreMultiplyOnLoad initPreMultiplyOnLoad, CallbackBase* callback)
: AsyncTask(callback),
  url(),
  encodedImageBuffer(initEncodedImageBuffer),
  id(initId),
  textureId(TextureManagerType::INVALID_TEXTURE_ID),
  dimensions(initDimensions),
  samplingMode(initSamplingMode),
  preMultiplyOnLoad(initPreMultiplyOnLoad),
  maskPixelBuffer(),
  contentScale(1.0f),
  animatedImageLoading(),
  frameIndex(0u),
  orientationCorrection(initOrientationCorrection),
  isMaskTask(false),
  cropToMask(false),
  loadPlanes(false)
{
}

LoadingTask::LoadingTask(uint32_t initId, PixelBuffer pixelBuffer, PixelBuffer initMaskPixelBuffer, float initContentScale, bool initCropToMask, DevelAsyncImageLoader::PreMultiplyOnLoad initPreMultiplyOnLoad, CallbackBase* callback)
: AsyncTask(callback),
  url(""),
  encodedImageBuffer(),
  id(initId),
  textureId(TextureManagerType::INVALID_TEXTURE_ID),
  dimensions(),
  samplingMode(),
  preMultiplyOnLoad(initPreMultiplyOnLoad),
  maskPixelBuffer(initMaskPixelBuffer),
  contentScale(initContentScale),
  animatedImageLoading(),
  frameIndex(0u),
  orientationCorrection(),
  isMaskTask(true),
  cropToMask(initCropToMask),
  loadPlanes(false)
{
  pixelBuffers.push_back(pixelBuffer);
}

LoadingTask::~LoadingTask()
{
}

void LoadingTask::Process()
{
#ifdef TRACE_ENABLED
  uint64_t startTimeNanoSeconds = 0;
  uint64_t endTimeNanoSeconds   = 0;
#endif

  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_IMAGE_LOADING_TASK", [&](std::ostringstream& oss)
  {
    startTimeNanoSeconds = GetNanoseconds();
    oss << "[";
    if(dimensions.GetWidth() > 0 || dimensions.GetHeight() > 0)
    {
      oss << "d:" << dimensions.GetWidth() << "x" << dimensions.GetHeight() << " ";
    }
    oss << "u:" << (!!(animatedImageLoading) ? animatedImageLoading.GetUrl() : url.GetEllipsedUrl()) << "]"; });

  if(!isMaskTask)
  {
    Load();
  }
  else
  {
    ApplyMask();
  }
  MultiplyAlpha();

  DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_IMAGE_LOADING_TASK", [&](std::ostringstream& oss)
  {
    endTimeNanoSeconds = GetNanoseconds();
    oss << std::fixed << std::setprecision(3);
    oss << "[";
    oss << "d:" << static_cast<float>(endTimeNanoSeconds - startTimeNanoSeconds) / 1000000.0f << "ms ";
    oss << "m:" << isMaskTask << " ";
    oss << "i:" << frameIndex << " ";
    oss << "b:" << pixelBuffers.size() << " ";
    if(!pixelBuffers.empty())
    {
      oss << "s:" << pixelBuffers[0].GetWidth() << "x" << pixelBuffers[0].GetHeight() << " ";
      oss << "p:" << DevelPixelBuffer::IsAlphaPreMultiplied(pixelBuffers[0]) << " ";
    }
    if(dimensions.GetWidth() > 0 || dimensions.GetHeight() > 0)
    {
      oss << "d:" << dimensions.GetWidth() << "x" << dimensions.GetHeight() << " ";
    }
    oss << "u:" << (!!(animatedImageLoading) ? animatedImageLoading.GetUrl() : url.GetEllipsedUrl()) << "]"; });
}

void LoadingTask::Load()
{
  PixelBuffer pixelBuffer;
  if(animatedImageLoading)
  {
    bool planeLoaded = false;
    if(loadPlanes)
    {
      planeLoaded = animatedImageLoading.LoadFramePlanes(frameIndex, pixelBuffers, dimensions);
    }
    if(!planeLoaded)
    {
      pixelBuffer = animatedImageLoading.LoadFrame(frameIndex, dimensions, samplingMode);
    }
  }
  else if(encodedImageBuffer)
  {
    pixelBuffer = Dali::LoadImageFromBuffer(encodedImageBuffer.GetRawBuffer(), dimensions, samplingMode, orientationCorrection);

    // We don't need to hold image buffer anymore.
    encodedImageBuffer.Reset();
  }
  else if(url.IsValid() && url.IsLocalResource())
  {
    if(loadPlanes)
    {
      Dali::LoadImagePlanesFromFile(url.GetUrl(), pixelBuffers, dimensions, samplingMode, orientationCorrection);
    }
    else
    {
      pixelBuffer = Dali::LoadImageFromFile(Dali::Integration::ToDaliStringView(url.GetUrl()), dimensions, samplingMode, orientationCorrection);
    }
  }
  else if(url.IsValid())
  {
    pixelBuffer = Dali::DownloadImageSynchronously(url.GetUrl(), dimensions, samplingMode, orientationCorrection);
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
    DevelPixelBufferOperations::ApplyMask(pixelBuffers[0], maskPixelBuffer, contentScale, cropToMask);
  }
}

void LoadingTask::MultiplyAlpha()
{
  if(!pixelBuffers.empty())
  {
    if(preMultiplyOnLoad == DevelAsyncImageLoader::PreMultiplyOnLoad::ON)
    {
      DevelPixelBufferOperations::MultiplyColorByAlpha(pixelBuffers[0]);
    }
  }
}

void LoadingTask::SetTextureId(TextureManagerType::TextureId newTextureId)
{
  textureId = newTextureId;
}

} // namespace Internal

} // namespace Toolkit

} //namespace DALI_NAMESPACE
