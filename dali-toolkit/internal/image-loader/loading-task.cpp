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
#include <dali-toolkit/internal/image-loader/loading-task.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>

#ifdef TRACE_ENABLED
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>
#endif

namespace Dali
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

LoadingTask::LoadingTask(uint32_t id, Dali::AnimatedImageLoading animatedImageLoading, uint32_t frameIndex, DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad, CallbackBase* callback)
: AsyncTask(callback),
  url(),
  encodedImageBuffer(),
  id(id),
  textureId(TextureManagerType::INVALID_TEXTURE_ID),
  dimensions(),
  fittingMode(FittingMode::SCALE_TO_FILL),
  samplingMode(SamplingMode::BOX_THEN_LINEAR),
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

LoadingTask::LoadingTask(uint32_t id, Dali::AnimatedImageLoading animatedImageLoading, uint32_t frameIndex, ImageDimensions dimensions, FittingMode::Type fittingMode, SamplingMode::Type samplingMode, DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad, CallbackBase* callback)
: AsyncTask(callback),
  url(),
  encodedImageBuffer(),
  id(id),
  textureId(TextureManagerType::INVALID_TEXTURE_ID),
  dimensions(dimensions),
  fittingMode(fittingMode),
  samplingMode(samplingMode),
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
: AsyncTask(callback, url.GetProtocolType() == VisualUrl::ProtocolType::REMOTE ? AsyncTask::PriorityType::LOW : AsyncTask::PriorityType::HIGH),
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
#ifdef TRACE_ENABLED
  uint64_t mStartTimeNanoSceonds = 0;
  uint64_t mEndTimeNanoSceonds   = 0;
#endif

  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_IMAGE_LOADING_TASK", [&](std::ostringstream& oss) {
    mStartTimeNanoSceonds = GetNanoseconds();
    oss << "[";
    if(dimensions.GetWidth() > 0 || dimensions.GetHeight() > 0)
    {
      oss << "d:" << dimensions.GetWidth() << "x" << dimensions.GetHeight() << " ";
    }
    oss << "u:" << (!!(animatedImageLoading) ? animatedImageLoading.GetUrl() : url.GetEllipsedUrl()) << "]";
  });

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

  DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_IMAGE_LOADING_TASK", [&](std::ostringstream& oss) {
    mEndTimeNanoSceonds = GetNanoseconds();
    oss << std::fixed << std::setprecision(3);
    oss << "[";
    oss << "d:" << static_cast<float>(mEndTimeNanoSceonds - mStartTimeNanoSceonds) / 1000000.0f << "ms ";
    oss << "m:" << isMaskTask << " ";
    oss << "i:" << frameIndex << " ";
    oss << "b:" << pixelBuffers.size() << " ";
    if(!pixelBuffers.empty())
    {
      oss << "s:" << pixelBuffers[0].GetWidth() << "x" << pixelBuffers[0].GetHeight() << " ";
      oss << "p:" << pixelBuffers[0].IsAlphaPreMultiplied() << " ";
    }
    if(dimensions.GetWidth() > 0 || dimensions.GetHeight() > 0)
    {
      oss << "d:" << dimensions.GetWidth() << "x" << dimensions.GetHeight() << " ";
    }
    oss << "u:" << (!!(animatedImageLoading) ? animatedImageLoading.GetUrl() : url.GetEllipsedUrl()) << "]";
  });
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
    pixelBuffer = animatedImageLoading.LoadFrame(frameIndex, dimensions, fittingMode, samplingMode);
  }
  else if(encodedImageBuffer)
  {
    pixelBuffer = Dali::LoadImageFromBuffer(encodedImageBuffer.GetRawBuffer(), dimensions, fittingMode, samplingMode, orientationCorrection);

    // We don't need to hold image buffer anymore.
    encodedImageBuffer.Reset();
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
  if(!pixelBuffers.empty())
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
