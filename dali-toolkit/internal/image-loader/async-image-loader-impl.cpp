/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/adaptor-framework/async-task-manager.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
AsyncImageLoader::AsyncImageLoader()
: mLoadedSignal(),
  mLoadTaskId(0u)
{
}

AsyncImageLoader::~AsyncImageLoader()
{
  CancelAll();
}

IntrusivePtr<AsyncImageLoader> AsyncImageLoader::New()
{
  IntrusivePtr<AsyncImageLoader> internal = new AsyncImageLoader();
  return internal;
}

uint32_t AsyncImageLoader::LoadAnimatedImage(Dali::AnimatedImageLoading               animatedImageLoading,
                                             uint32_t                                 frameIndex,
                                             DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
{
  LoadingTaskPtr loadingTask = new LoadingTask(++mLoadTaskId, animatedImageLoading, frameIndex, preMultiplyOnLoad, MakeCallback(this, &AsyncImageLoader::ProcessLoadedImage));
  Dali::AsyncTaskManager::Get().AddTask(loadingTask);
  return mLoadTaskId;
}

uint32_t AsyncImageLoader::LoadAnimatedImage(Dali::AnimatedImageLoading               animatedImageLoading,
                                             uint32_t                                 frameIndex,
                                             Dali::ImageDimensions                    desiredSize,
                                             Dali::FittingMode::Type                  fittingMode,
                                             Dali::SamplingMode::Type                 samplingMode,
                                             DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
{
  LoadingTaskPtr loadingTask = new LoadingTask(++mLoadTaskId, animatedImageLoading, frameIndex, desiredSize, fittingMode, samplingMode, preMultiplyOnLoad, MakeCallback(this, &AsyncImageLoader::ProcessLoadedImage));
  Dali::AsyncTaskManager::Get().AddTask(loadingTask);
  return mLoadTaskId;
}

uint32_t AsyncImageLoader::Load(const VisualUrl&                         url,
                                ImageDimensions                          dimensions,
                                FittingMode::Type                        fittingMode,
                                SamplingMode::Type                       samplingMode,
                                bool                                     orientationCorrection,
                                DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad,
                                bool                                     loadPlanes)
{
  LoadingTaskPtr loadingTask = new LoadingTask(++mLoadTaskId, url, dimensions, fittingMode, samplingMode, orientationCorrection, preMultiplyOnLoad, loadPlanes, MakeCallback(this, &AsyncImageLoader::ProcessLoadedImage));
  AsyncTaskManager::Get().AddTask(loadingTask);
  mLoadingTasks.push_back(AsyncImageLoadingInfo(loadingTask, mLoadTaskId));
  return mLoadTaskId;
}

uint32_t AsyncImageLoader::LoadEncodedImageBuffer(const EncodedImageBuffer&                encodedImageBuffer,
                                                  ImageDimensions                          dimensions,
                                                  FittingMode::Type                        fittingMode,
                                                  SamplingMode::Type                       samplingMode,
                                                  bool                                     orientationCorrection,
                                                  DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
{
  LoadingTaskPtr loadingTask = new LoadingTask(++mLoadTaskId, encodedImageBuffer, dimensions, fittingMode, samplingMode, orientationCorrection, preMultiplyOnLoad, MakeCallback(this, &AsyncImageLoader::ProcessLoadedImage));
  Dali::AsyncTaskManager::Get().AddTask(loadingTask);
  mLoadingTasks.push_back(AsyncImageLoadingInfo(loadingTask, mLoadTaskId));
  return mLoadTaskId;
}

uint32_t AsyncImageLoader::ApplyMask(Devel::PixelBuffer                       pixelBuffer,
                                     Devel::PixelBuffer                       maskPixelBuffer,
                                     float                                    contentScale,
                                     bool                                     cropToMask,
                                     DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
{
  LoadingTaskPtr loadingTask = new LoadingTask(++mLoadTaskId, pixelBuffer, maskPixelBuffer, contentScale, cropToMask, preMultiplyOnLoad, MakeCallback(this, &AsyncImageLoader::ProcessLoadedImage));
  Dali::AsyncTaskManager::Get().AddTask(loadingTask);
  mLoadingTasks.push_back(AsyncImageLoadingInfo(loadingTask, mLoadTaskId));
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
  auto end = mLoadingTasks.end();
  for(std::vector<AsyncImageLoadingInfo>::iterator iter = mLoadingTasks.begin(); iter != end; ++iter)
  {
    if((*iter).loadId == loadingTaskId)
    {
      Dali::AsyncTaskManager::Get().RemoveTask((*iter).loadingTask);
      mLoadingTasks.erase(iter);
      return true;
    }
  }

  return false;
}

void AsyncImageLoader::CancelAll()
{
  auto end = mLoadingTasks.end();
  for(std::vector<AsyncImageLoadingInfo>::iterator iter = mLoadingTasks.begin(); iter != end; ++iter)
  {
    if((*iter).loadingTask && Dali::AsyncTaskManager::Get())
    {
      Dali::AsyncTaskManager::Get().RemoveTask(((*iter).loadingTask));
    }
  }
  mLoadingTasks.clear();
}

void AsyncImageLoader::ProcessLoadedImage(LoadingTaskPtr task)
{
  // Remove tasks before signal emit
  auto end = mLoadingTasks.end();
  for(auto iter = mLoadingTasks.begin(); iter != end; ++iter)
  {
    if((*iter).loadId == task->id)
    {
      mLoadingTasks.erase(iter);
      break;
    }
  }

  if(mPixelBufferLoadedSignal.GetConnectionCount() > 0)
  {
    mPixelBufferLoadedSignal.Emit(task->id, task->pixelBuffers);
  }
  else if(mLoadedSignal.GetConnectionCount() > 0)
  {
    PixelData pixelData;
    if(!task->pixelBuffers.empty())
    {
      pixelData = Devel::PixelBuffer::Convert(task->pixelBuffers[0]);
    }
    mLoadedSignal.Emit(task->id, pixelData);
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
