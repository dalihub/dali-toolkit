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
#include "async-image-loader-impl.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/adaptor.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
AsyncImageLoader::AsyncImageLoader()
: mLoadedSignal(),
  mLoadThread(new EventThreadCallback(MakeCallback(this, &AsyncImageLoader::ProcessLoadedImage))),
  mLoadTaskId(0u),
  mIsLoadThreadStarted(false)
{
}

AsyncImageLoader::~AsyncImageLoader()
{
  mLoadThread.CancelAll();
}

IntrusivePtr<AsyncImageLoader> AsyncImageLoader::New()
{
  IntrusivePtr<AsyncImageLoader> internal = new AsyncImageLoader();
  return internal;
}

uint32_t AsyncImageLoader::LoadAnimatedImage(Dali::AnimatedImageLoading animatedImageLoading,
                                             uint32_t                   frameIndex)
{
  if(!mIsLoadThreadStarted)
  {
    mLoadThread.Start();
    mIsLoadThreadStarted = true;
  }
  mLoadThread.AddTask(new LoadingTask(++mLoadTaskId, animatedImageLoading, frameIndex));

  return mLoadTaskId;
}

uint32_t AsyncImageLoader::Load(const VisualUrl&                         url,
                                ImageDimensions                          dimensions,
                                FittingMode::Type                        fittingMode,
                                SamplingMode::Type                       samplingMode,
                                bool                                     orientationCorrection,
                                DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
{
  if(!mIsLoadThreadStarted)
  {
    mLoadThread.Start();
    mIsLoadThreadStarted = true;
  }
  mLoadThread.AddTask(new LoadingTask(++mLoadTaskId, url, dimensions, fittingMode, samplingMode, orientationCorrection, preMultiplyOnLoad));

  return mLoadTaskId;
}

uint32_t AsyncImageLoader::LoadEncodedImageBuffer(const EncodedImageBuffer&                encodedImageBuffer,
                                                  ImageDimensions                          dimensions,
                                                  FittingMode::Type                        fittingMode,
                                                  SamplingMode::Type                       samplingMode,
                                                  bool                                     orientationCorrection,
                                                  DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
{
  if(!mIsLoadThreadStarted)
  {
    mLoadThread.Start();
    mIsLoadThreadStarted = true;
  }
  mLoadThread.AddTask(new LoadingTask(++mLoadTaskId, encodedImageBuffer, dimensions, fittingMode, samplingMode, orientationCorrection, preMultiplyOnLoad));

  return mLoadTaskId;
}

uint32_t AsyncImageLoader::ApplyMask(Devel::PixelBuffer                       pixelBuffer,
                                     Devel::PixelBuffer                       maskPixelBuffer,
                                     float                                    contentScale,
                                     bool                                     cropToMask,
                                     DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
{
  if(!mIsLoadThreadStarted)
  {
    mLoadThread.Start();
    mIsLoadThreadStarted = true;
  }
  mLoadThread.AddTask(new LoadingTask(++mLoadTaskId, pixelBuffer, maskPixelBuffer, contentScale, cropToMask, preMultiplyOnLoad));

  return mLoadTaskId;
}

Toolkit::AsyncImageLoader::ImageLoadedSignalType& AsyncImageLoader::ImageLoadedSignal()
{
  return mLoadedSignal;
}

Toolkit::DevelAsyncImageLoader::PixelBufferLoadedSignalType& AsyncImageLoader::PixelBufferLoadedSignal()
{
  return mPixelBufferLoadedSignal;
}

bool AsyncImageLoader::Cancel(uint32_t loadingTaskId)
{
  return mLoadThread.CancelTask(loadingTaskId);
}

void AsyncImageLoader::CancelAll()
{
  mLoadThread.CancelAll();
}

void AsyncImageLoader::ProcessLoadedImage()
{
  while(LoadingTask* next = mLoadThread.NextCompletedTask())
  {
    if(mPixelBufferLoadedSignal.GetConnectionCount() > 0)
    {
      std::vector<Devel::PixelBuffer> pixelBuffers{next->pixelBuffer};
      mPixelBufferLoadedSignal.Emit(next->id, pixelBuffers);
    }
    else if(mLoadedSignal.GetConnectionCount() > 0)
    {
      PixelData pixelData;
      if(next->pixelBuffer)
      {
        pixelData = Devel::PixelBuffer::Convert(next->pixelBuffer);
      }
      mLoadedSignal.Emit(next->id, pixelData);
    }

    delete next;
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
