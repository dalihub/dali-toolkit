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

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
FastTrackLoadingTask::FastTrackLoadingTask(const VisualUrl& url, ImageDimensions dimensions, FittingMode::Type fittingMode, SamplingMode::Type samplingMode, bool orientationCorrection, DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad, CallbackBase* callback)
: AsyncTask(MakeCallback(this, &FastTrackLoadingTask::OnComplete), url.GetProtocolType() == VisualUrl::ProtocolType::REMOTE ? AsyncTask::PriorityType::LOW : AsyncTask::PriorityType::HIGH),
  mUrl(url),
  mTexture(),
  mDimensions(dimensions),
  mFittingMode(fittingMode),
  mSamplingMode(samplingMode),
  mPreMultiplyOnLoad(preMultiplyOnLoad),
  mCallback(),
  mTextureUploadManager(Dali::Devel::TextureUploadManager::Get()),
  mImageWidth(0u),
  mImageHeight(0u),
  mImageFormat(Pixel::INVALID),
  mPixelData(),
  mResourceId(0u),
  mOrientationCorrection(orientationCorrection),
  mLoadSuccess(false),
  mLoadYuvImage(false),
  mPremultiplied(false)
{
  mCallback = std::unique_ptr<CallbackBase>(callback);
  PrepareTexture();
}

FastTrackLoadingTask::~FastTrackLoadingTask()
{
}

void FastTrackLoadingTask::PrepareTexture()
{
  mTexture    = mTextureUploadManager.GenerateTexture2D();
  mResourceId = Integration::GetTextureResourceId(mTexture);
}

void FastTrackLoadingTask::OnComplete(AsyncTaskPtr task)
{
  if(mLoadSuccess)
  {
    Dali::Integration::SetTextureSize(mTexture, Dali::ImageDimensions(mImageWidth, mImageHeight));
    Dali::Integration::SetTexturePixelFormat(mTexture, mImageFormat);
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
  Devel::PixelBuffer              pixelBuffer;
  std::vector<Devel::PixelBuffer> pixelBuffers;

  if(mUrl.IsValid() && mUrl.IsLocalResource())
  {
    // TODO : We need to consider YUV case in future.
    //Dali::LoadImagePlanesFromFile(mUrl.GetUrl(), pixelBuffers, mDimensions, mFittingMode, mSamplingMode, mOrientationCorrection);

    pixelBuffer = Dali::LoadImageFromFile(mUrl.GetUrl(), mDimensions, mFittingMode, mSamplingMode, mOrientationCorrection);
  }
  else if(mUrl.IsValid())
  {
    pixelBuffer = Dali::DownloadImageSynchronously(mUrl.GetUrl(), mDimensions, mFittingMode, mSamplingMode, mOrientationCorrection);
  }

  if(pixelBuffer)
  {
    pixelBuffers.push_back(pixelBuffer);
  }

  if(pixelBuffers.empty())
  {
    DALI_LOG_ERROR("FastTrackLoadingTask::Load: Loading is failed: ResourceId : %d, url : [%s]\n", mResourceId, mUrl.GetUrl().c_str());
  }
  else
  {
    if(pixelBuffers.size() == 1u)
    {
      mLoadSuccess = true;
      MultiplyAlpha(pixelBuffers[0]);
      mPixelData = Dali::Devel::PixelBuffer::Convert(pixelBuffers[0]);
    }
    else
    {
      DALI_LOG_ERROR("FastTrackLoadingTask::Load: ??? Undefined case. PixelBuffers.size() : %zu : ResourceId : %d, url : [%s]\n", pixelBuffers.size(), mResourceId, mUrl.GetUrl().c_str());
    }
  }
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
    mImageWidth  = mPixelData.GetWidth();
    mImageHeight = mPixelData.GetHeight();
    mImageFormat = mPixelData.GetPixelFormat();

    mTextureUploadManager.RequestUpload(mResourceId, mPixelData);
  }

  mPixelData.Reset();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
