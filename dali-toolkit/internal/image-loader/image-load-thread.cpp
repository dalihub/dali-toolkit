/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include "image-load-thread.h"

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
LoadingTask::LoadingTask(uint32_t id, Dali::AnimatedImageLoading animatedImageLoading, uint32_t frameIndex, DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
: pixelBuffer(),
  url(),
  encodedImageBuffer(),
  id(id),
  dimensions(),
  fittingMode(),
  samplingMode(),
  orientationCorrection(),
  preMultiplyOnLoad(preMultiplyOnLoad),
  isMaskTask(false),
  maskPixelBuffer(),
  contentScale(1.0f),
  cropToMask(false),
  animatedImageLoading(animatedImageLoading),
  frameIndex(frameIndex)
{
}

LoadingTask::LoadingTask(uint32_t id, const VisualUrl& url, ImageDimensions dimensions, FittingMode::Type fittingMode, SamplingMode::Type samplingMode, bool orientationCorrection, DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
: pixelBuffer(),
  url(url),
  encodedImageBuffer(),
  id(id),
  dimensions(dimensions),
  fittingMode(fittingMode),
  samplingMode(samplingMode),
  orientationCorrection(orientationCorrection),
  preMultiplyOnLoad(preMultiplyOnLoad),
  isMaskTask(false),
  maskPixelBuffer(),
  contentScale(1.0f),
  cropToMask(false),
  animatedImageLoading(),
  frameIndex(0u)
{
}

LoadingTask::LoadingTask(uint32_t id, const EncodedImageBuffer& encodedImageBuffer, ImageDimensions dimensions, FittingMode::Type fittingMode, SamplingMode::Type samplingMode, bool orientationCorrection, DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
: pixelBuffer(),
  url(),
  encodedImageBuffer(encodedImageBuffer),
  id(id),
  dimensions(dimensions),
  fittingMode(fittingMode),
  samplingMode(samplingMode),
  orientationCorrection(orientationCorrection),
  preMultiplyOnLoad(preMultiplyOnLoad),
  isMaskTask(false),
  maskPixelBuffer(),
  contentScale(1.0f),
  cropToMask(false),
  animatedImageLoading(),
  frameIndex(0u)
{
}

LoadingTask::LoadingTask(uint32_t id, Devel::PixelBuffer pixelBuffer, Devel::PixelBuffer maskPixelBuffer, float contentScale, bool cropToMask, DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
: pixelBuffer(pixelBuffer),
  url(""),
  encodedImageBuffer(),
  id(id),
  dimensions(),
  fittingMode(),
  samplingMode(),
  orientationCorrection(),
  preMultiplyOnLoad(preMultiplyOnLoad),
  isMaskTask(true),
  maskPixelBuffer(maskPixelBuffer),
  contentScale(contentScale),
  cropToMask(cropToMask),
  animatedImageLoading(),
  frameIndex(0u)
{
}

void LoadingTask::Load()
{
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
    pixelBuffer = Dali::LoadImageFromFile(url.GetUrl(), dimensions, fittingMode, samplingMode, orientationCorrection);
  }
  else if(url.IsValid())
  {
    pixelBuffer = Dali::DownloadImageSynchronously(url.GetUrl(), dimensions, fittingMode, samplingMode, orientationCorrection);
  }

  if(!pixelBuffer)
  {
    DALI_LOG_ERROR("LoadingTask::Load: Loading is failed: %s\n", url.GetUrl().c_str());
  }
}

void LoadingTask::ApplyMask()
{
  pixelBuffer.ApplyMask(maskPixelBuffer, contentScale, cropToMask);
}

void LoadingTask::MultiplyAlpha()
{
  if(pixelBuffer && Pixel::HasAlpha(pixelBuffer.GetPixelFormat()))
  {
    if(preMultiplyOnLoad == DevelAsyncImageLoader::PreMultiplyOnLoad::ON)
    {
      pixelBuffer.MultiplyColorByAlpha();
    }
  }
}

ImageLoadThread::ImageLoadThread(EventThreadCallback* trigger)
: mTrigger(trigger),
  mLogFactory(Dali::Adaptor::Get().GetLogFactory())
{
}

ImageLoadThread::~ImageLoadThread()
{
  // add an empty task would stop the thread from conditional wait.
  AddTask(NULL);
  // stop the thread
  Join();

  delete mTrigger;

  for(auto&& iter : mLoadQueue)
  {
    delete iter;
  }
  mLoadQueue.Clear();

  for(auto&& iter : mCompleteQueue)
  {
    delete iter;
  }
  mCompleteQueue.Clear();
}

void ImageLoadThread::Run()
{
  SetThreadName("ImageLoadThread");
  mLogFactory.InstallLogFunction();

  while(LoadingTask* task = NextTaskToProcess())
  {
    if(!task->isMaskTask)
    {
      task->Load();
    }
    else
    {
      task->ApplyMask();
    }
    task->MultiplyAlpha();

    AddCompletedTask(task);
  }
}

void ImageLoadThread::AddTask(LoadingTask* task)
{
  bool wasEmpty = false;
  {
    // Lock while adding task to the queue
    ConditionalWait::ScopedLock lock(mConditionalWait);
    wasEmpty = mLoadQueue.Empty();
    mLoadQueue.PushBack(task);
  }

  if(wasEmpty)
  {
    // wake up the image loading thread
    mConditionalWait.Notify();
  }
}

LoadingTask* ImageLoadThread::NextCompletedTask()
{
  // Lock while popping task out from the queue
  Mutex::ScopedLock lock(mMutex);

  if(mCompleteQueue.Empty())
  {
    return NULL;
  }

  Vector<LoadingTask*>::Iterator next     = mCompleteQueue.Begin();
  LoadingTask*                   nextTask = *next;
  mCompleteQueue.Erase(next);

  return nextTask;
}

bool ImageLoadThread::CancelTask(uint32_t loadingTaskId)
{
  // Lock while remove task from the queue
  ConditionalWait::ScopedLock lock(mConditionalWait);

  for(Vector<LoadingTask*>::Iterator iter = mLoadQueue.Begin(); iter != mLoadQueue.End(); ++iter)
  {
    if((*iter)->id == loadingTaskId)
    {
      delete(*iter);
      mLoadQueue.Erase(iter);
      return true;
    }
  }

  return false;
}

void ImageLoadThread::CancelAll()
{
  // Lock while remove task from the queue
  ConditionalWait::ScopedLock lock(mConditionalWait);

  for(Vector<LoadingTask*>::Iterator iter = mLoadQueue.Begin(); iter != mLoadQueue.End(); ++iter)
  {
    delete(*iter);
  }
  mLoadQueue.Clear();
}

LoadingTask* ImageLoadThread::NextTaskToProcess()
{
  // Lock while popping task out from the queue
  ConditionalWait::ScopedLock lock(mConditionalWait);

  while(mLoadQueue.Empty())
  {
    mConditionalWait.Wait(lock);
  }

  Vector<LoadingTask*>::Iterator next     = mLoadQueue.Begin();
  LoadingTask*                   nextTask = *next;
  mLoadQueue.Erase(next);

  return nextTask;
}

void ImageLoadThread::AddCompletedTask(LoadingTask* task)
{
  // Lock while adding task to the queue
  Mutex::ScopedLock lock(mMutex);
  mCompleteQueue.PushBack(task);

  // wake up the main thread
  mTrigger->Trigger();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
