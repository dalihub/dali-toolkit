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
#include <dali-toolkit/internal/image-loader/fast-track-loading-task.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/texture-upload-manager.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/texture-integ.h>
#include <dali/integration-api/trace.h>
#include <dali/public-api/common/vector-wrapper.h>

#ifdef TRACE_ENABLED
#include <sstream>
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

constexpr uint32_t CHROMINANCE_U_INDEX = 1u;
constexpr uint32_t CHROMINANCE_V_INDEX = 2u;

Dali::PixelData GetDummyChrominanceUPixelData()
{
  static Dali::PixelData pixelDataU = PixelData::New(new uint8_t[2]{0x00, 0x00}, 2, 1, 2, Pixel::L8, PixelData::DELETE_ARRAY);
  return pixelDataU;
}

Dali::PixelData GetDummyChrominanceVPixelData()
{
  static Dali::PixelData pixelDataV = PixelData::New(new uint8_t[2]{0x00, 0x00}, 2, 2, 1, Pixel::L8, PixelData::DELETE_ARRAY);
  return pixelDataV;
}

} // namespace

FastTrackLoadingTask::FastTrackLoadingTask(const VisualUrl& url, ImageDimensions dimensions, FittingMode::Type fittingMode, SamplingMode::Type samplingMode, bool orientationCorrection, DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad, bool loadPlanes, CallbackBase* callback)
: AsyncTask(MakeCallback(this, &FastTrackLoadingTask::OnComplete), url.GetProtocolType() == VisualUrl::ProtocolType::REMOTE ? AsyncTask::PriorityType::LOW : AsyncTask::PriorityType::HIGH),
  mUrl(url),
  mTextures(),
  mDimensions(dimensions),
  mFittingMode(fittingMode),
  mSamplingMode(samplingMode),
  mPreMultiplyOnLoad(preMultiplyOnLoad),
  mCallback(),
  mTextureUploadManager(Dali::Devel::TextureUploadManager::Get()),
  mImageInformations(),
  mPixelData(),
  mOrientationCorrection(orientationCorrection),
  mLoadSuccess(false),
  mLoadPlanesAvaliable(loadPlanes),
  mPremultiplied(false),
  mPlanesLoaded(false)
{
  mCallback = std::unique_ptr<CallbackBase>(callback);
  PrepareTexture();
}

FastTrackLoadingTask::~FastTrackLoadingTask()
{
}

void FastTrackLoadingTask::PrepareTexture()
{
  const uint32_t requiredTexturesCount = mLoadPlanesAvaliable ? 3u : 1u;

  mTextures.resize(requiredTexturesCount);
  mImageInformations.resize(requiredTexturesCount);
  for(uint32_t index = 0u; index < requiredTexturesCount; ++index)
  {
    mTextures[index] = mTextureUploadManager.GenerateTexture2D();

    mImageInformations[index].resourceId = Integration::GetTextureResourceId(mTextures[index]);
  }

  if(mLoadPlanesAvaliable)
  {
    // Create static dummy chrominance pixel data now, for thread safety.
    [[maybe_unused]] auto pixelDataU = GetDummyChrominanceUPixelData();
    [[maybe_unused]] auto pixelDataV = GetDummyChrominanceVPixelData();
  }
}

void FastTrackLoadingTask::OnComplete(AsyncTaskPtr task)
{
  if(mLoadSuccess)
  {
    for(uint32_t index = 0u; index < mImageInformations.size(); ++index)
    {
      Dali::Integration::SetTextureSize(mTextures[index], Dali::ImageDimensions(mImageInformations[index].width, mImageInformations[index].height));
      Dali::Integration::SetTexturePixelFormat(mTextures[index], mImageInformations[index].format);
    }
    if(mLoadPlanesAvaliable && !mPlanesLoaded)
    {
      // We will not use ChrominanceU and ChrominanceV texture anymore.
      mTextures.resize(1u);
    }
  }
  else
  {
    mTextures.clear();
  }

  if(mCallback)
  {
    CallbackBase::Execute(*mCallback, FastTrackLoadingTaskPtr(reinterpret_cast<FastTrackLoadingTask*>(task.Get())));
  }
}

// Called by worker thread

void FastTrackLoadingTask::Process()
{
  Load();
  UploadToTexture();
}

bool FastTrackLoadingTask::IsReady()
{
  return true;
}

void FastTrackLoadingTask::Load()
{
#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    std::ostringstream oss;
    oss << "[url:" << mUrl.GetUrl() << "]";
    DALI_TRACE_BEGIN(gTraceFilter, "DALI_IMAGE_FAST_TRACK_UPLOADING_TASK");
    DALI_LOG_RELEASE_INFO("BEGIN: DALI_IMAGE_FAST_TRACK_UPLOADING_TASK %s", oss.str().c_str());
  }
#endif

  Devel::PixelBuffer              pixelBuffer;
  std::vector<Devel::PixelBuffer> pixelBuffers;

  if(mUrl.IsValid() && mUrl.IsLocalResource())
  {
    if(mLoadPlanesAvaliable)
    {
      Dali::LoadImagePlanesFromFile(mUrl.GetUrl(), pixelBuffers, mDimensions, mFittingMode, mSamplingMode, mOrientationCorrection);
    }
    else
    {
      pixelBuffer = Dali::LoadImageFromFile(mUrl.GetUrl(), mDimensions, mFittingMode, mSamplingMode, mOrientationCorrection);
    }
  }
  else if(mUrl.IsValid())
  {
    pixelBuffer = Dali::DownloadImageSynchronously(mUrl.GetUrl(), mDimensions, mFittingMode, mSamplingMode, mOrientationCorrection);
  }

  if(pixelBuffer)
  {
    pixelBuffers.emplace_back(std::move(pixelBuffer));
  }

  if(pixelBuffers.empty())
  {
    mLoadSuccess = false;
    DALI_LOG_ERROR("FastTrackLoadingTask::Load: Loading is failed: ResourceId : %d url : [%s]\n", mImageInformations[0u].resourceId, mUrl.GetUrl().c_str());
  }
  else
  {
    mPixelData.resize(pixelBuffers.size());

    mLoadSuccess = true;
    MultiplyAlpha(pixelBuffers[0]);
    uint32_t index = 0u;
    for(auto&& pixelBuffer : pixelBuffers)
    {
      mPixelData[index++] = Dali::Devel::PixelBuffer::Convert(pixelBuffer);
    }

    if(pixelBuffers.size() > 1u)
    {
      mPlanesLoaded = true;
    }
    else if(mLoadPlanesAvaliable && pixelBuffers.size() == 1u && mTextures.size() == 3u) ///< Case when we prepare three textures to render YUV, but loaded image is not YUV.
    {
      // Dummy pixel data for fake shader that we don't use actual YUV format.
      // To fake shader, let we use indivisual sizes of texture for U and V.
      mPixelData.resize(3u);
      mPixelData[CHROMINANCE_U_INDEX] = GetDummyChrominanceUPixelData();
      mPixelData[CHROMINANCE_V_INDEX] = GetDummyChrominanceVPixelData();
    }

    if(DALI_UNLIKELY(mPixelData.size() != mImageInformations.size()))
    {
      DALI_LOG_ERROR("FastTrackLoadingTask::Load: Undefined case. pixelBuffers.size() : %zu, image size : %zu, ResourceId : %d, url : [%s]\n", pixelBuffers.size(), mImageInformations.size(), mImageInformations[0u].resourceId, mUrl.GetUrl().c_str());
      mLoadSuccess = false;
    }
  }

#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    std::ostringstream oss;
    oss << "[";
    oss << "pixelBuffers:" << pixelBuffers.size() << " ";
    if(!pixelBuffers.empty())
    {
      oss << "size:" << pixelBuffers[0].GetWidth() << "x" << pixelBuffers[0].GetHeight() << " ";
      oss << "premult:" << mPremultiplied << " ";
    }
    oss << "url:" << mUrl.GetUrl() << "]";
    DALI_TRACE_END(gTraceFilter, "DALI_IMAGE_FAST_TRACK_UPLOADING_TASK");
    DALI_LOG_RELEASE_INFO("END: DALI_IMAGE_FAST_TRACK_UPLOADING_TASK %s", oss.str().c_str());
  }
#endif
}

void FastTrackLoadingTask::MultiplyAlpha(Dali::Devel::PixelBuffer pixelBuffer)
{
  if(mPreMultiplyOnLoad == DevelAsyncImageLoader::PreMultiplyOnLoad::ON && Pixel::HasAlpha(pixelBuffer.GetPixelFormat()))
  {
    pixelBuffer.MultiplyColorByAlpha();
    mPremultiplied = pixelBuffer.IsAlphaPreMultiplied();
  }
}

void FastTrackLoadingTask::UploadToTexture()
{
  if(mLoadSuccess)
  {
    DALI_ASSERT_DEBUG(mPixelData.size() == mImageInformations.size());

    uint32_t index = 0u;
    for(auto&& pixelData : mPixelData)
    {
      mImageInformations[index].width  = pixelData.GetWidth();
      mImageInformations[index].height = pixelData.GetHeight();
      mImageInformations[index].format = pixelData.GetPixelFormat();

      mTextureUploadManager.RequestUpload(mImageInformations[index].resourceId, pixelData);

      pixelData.Reset();

      ++index;
    }
  }

  mPixelData.clear();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
