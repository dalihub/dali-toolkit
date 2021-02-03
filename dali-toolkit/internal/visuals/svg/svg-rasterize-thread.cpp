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
#include "svg-rasterize-thread.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/file-loader.h>
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/svg/svg-visual.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
RasterizingTask::RasterizingTask(SvgVisual* svgRenderer, VectorImageRenderer vectorRenderer, const VisualUrl& url, float dpi, unsigned int width, unsigned int height)
: mSvgVisual(svgRenderer),
  mVectorRenderer(vectorRenderer),
  mUrl(url),
  mDpi(dpi),
  mWidth(width),
  mHeight(height),
  mLoadSuccess(false)
{
}

RasterizingTask::~RasterizingTask()
{
}

void RasterizingTask::Load()
{
  if(!mUrl.IsLocalResource())
  {
    Dali::Vector<uint8_t> remoteBuffer;
    if(!Dali::FileLoader::DownloadFileSynchronously(mUrl.GetUrl(), remoteBuffer))
    {
      DALI_LOG_ERROR("RasterizingTask::Load: Failed to download file! [%s]\n", mUrl.GetUrl().c_str());
      return;
    }

    remoteBuffer.PushBack('\0');

    if(!mVectorRenderer.Load(remoteBuffer, mDpi))
    {
      DALI_LOG_ERROR("RasterizingTask::Load:Failed to load data! [%s]\n", mUrl.GetUrl().c_str());
      return;
    }

    mLoadSuccess = true;
  }
  else
  {
    mLoadSuccess = true;
  }
}

void RasterizingTask::Rasterize()
{
  if(mWidth <= 0u || mHeight <= 0u)
  {
    DALI_LOG_ERROR("RasterizingTask::Rasterize: Size is zero!\n");
    return;
  }

  Devel::PixelBuffer pixelBuffer = Devel::PixelBuffer::New(mWidth, mHeight, Dali::Pixel::RGBA8888);

  uint32_t defaultWidth, defaultHeight;
  mVectorRenderer.GetDefaultSize(defaultWidth, defaultHeight);

  float scaleX = static_cast<float>(mWidth) / static_cast<float>(defaultWidth);
  float scaleY = static_cast<float>(mHeight) / static_cast<float>(defaultHeight);
  float scale  = scaleX < scaleY ? scaleX : scaleY;

  if(!mVectorRenderer.Rasterize(pixelBuffer, scale))
  {
    DALI_LOG_ERROR("RasterizingTask::Rasterize: Rasterize is failed! [%s]\n", mUrl.GetUrl().c_str());
    return;
  }

  mPixelData = Devel::PixelBuffer::Convert(pixelBuffer);
}

VectorImageRenderer RasterizingTask::GetVectorRenderer() const
{
  return mVectorRenderer;
}

bool RasterizingTask::IsLoaded() const
{
  return mLoadSuccess;
}

SvgVisual* RasterizingTask::GetSvgVisual() const
{
  return mSvgVisual.Get();
}

PixelData RasterizingTask::GetPixelData() const
{
  return mPixelData;
}

SvgRasterizeThread::SvgRasterizeThread()
: mTrigger(new EventThreadCallback(MakeCallback(this, &SvgRasterizeThread::ApplyRasterizedSVGToSampler))),
  mLogFactory(Dali::Adaptor::Get().GetLogFactory()),
  mIsThreadWaiting(false),
  mProcessorRegistered(false)
{
}

SvgRasterizeThread::~SvgRasterizeThread()
{
  if(mProcessorRegistered)
  {
    Adaptor::Get().UnregisterProcessor(*this);
  }
}

void SvgRasterizeThread::TerminateThread(SvgRasterizeThread*& thread)
{
  if(thread)
  {
    // add an empty task would stop the thread from conditional wait.
    thread->AddTask(RasterizingTaskPtr());
    // stop the thread
    thread->Join();
    // delete the thread
    delete thread;
    thread = NULL;
  }
}

void SvgRasterizeThread::AddTask(RasterizingTaskPtr task)
{
  bool wasEmpty = false;

  {
    // Lock while adding task to the queue
    ConditionalWait::ScopedLock lock(mConditionalWait);
    wasEmpty = mRasterizeTasks.empty();
    if(!wasEmpty && task != NULL)
    {
      // Remove the tasks with the same renderer.
      // Older task which waiting to rasterize and apply the svg to the same renderer is expired.
      for(std::vector<RasterizingTaskPtr>::iterator it = mRasterizeTasks.begin(), endIt = mRasterizeTasks.end(); it != endIt; ++it)
      {
        if((*it) && (*it)->GetSvgVisual() == task->GetSvgVisual())
        {
          mRasterizeTasks.erase(it);
          break;
        }
      }
    }
    mRasterizeTasks.push_back(task);

    if(!mProcessorRegistered)
    {
      Adaptor::Get().RegisterProcessor(*this);
      mProcessorRegistered = true;
    }
  }

  if(wasEmpty)
  {
    // wake up the image loading thread
    mConditionalWait.Notify();
  }
}

RasterizingTaskPtr SvgRasterizeThread::NextCompletedTask()
{
  // Lock while popping task out from the queue
  Mutex::ScopedLock lock(mMutex);

  if(mCompletedTasks.empty())
  {
    return RasterizingTaskPtr();
  }

  std::vector<RasterizingTaskPtr>::iterator next     = mCompletedTasks.begin();
  RasterizingTaskPtr                        nextTask = *next;
  mCompletedTasks.erase(next);

  return nextTask;
}

void SvgRasterizeThread::RemoveTask(SvgVisual* visual)
{
  // Lock while remove task from the queue
  ConditionalWait::ScopedLock lock(mConditionalWait);
  if(!mRasterizeTasks.empty())
  {
    for(std::vector<RasterizingTaskPtr>::iterator it = mRasterizeTasks.begin(), endIt = mRasterizeTasks.end(); it != endIt; ++it)
    {
      if((*it) && (*it)->GetSvgVisual() == visual)
      {
        mRasterizeTasks.erase(it);
        break;
      }
    }
  }

  UnregisterProcessor();
}

void SvgRasterizeThread::DeleteImage(VectorImageRenderer vectorRenderer)
{
  // Lock while adding image to the delete queue
  ConditionalWait::ScopedLock lock(mConditionalWait);

  if(mIsThreadWaiting) // no rasterization is ongoing, save to delete
  {
    // TODO: what?
  }
  else // wait to delete until current rasterization completed.
  {
    mDeleteSvg.PushBack(&vectorRenderer);
  }
}

RasterizingTaskPtr SvgRasterizeThread::NextTaskToProcess()
{
  // Lock while popping task out from the queue
  ConditionalWait::ScopedLock lock(mConditionalWait);

  // Delete the image here to make sure that it is not used in the nsvgRasterize()
  if(!mDeleteSvg.Empty())
  {
    mDeleteSvg.Clear();
  }

  // conditional wait
  while(mRasterizeTasks.empty())
  {
    mIsThreadWaiting = true;
    mConditionalWait.Wait(lock);
  }
  mIsThreadWaiting = false;

  // pop out the next task from the queue
  std::vector<RasterizingTaskPtr>::iterator next     = mRasterizeTasks.begin();
  RasterizingTaskPtr                        nextTask = *next;
  mRasterizeTasks.erase(next);

  return nextTask;
}

void SvgRasterizeThread::AddCompletedTask(RasterizingTaskPtr task)
{
  // Lock while adding task to the queue
  Mutex::ScopedLock lock(mMutex);
  mCompletedTasks.push_back(task);

  // wake up the main thread
  mTrigger->Trigger();
}

void SvgRasterizeThread::Run()
{
  SetThreadName("SVGThread");
  mLogFactory.InstallLogFunction();

  while(RasterizingTaskPtr task = NextTaskToProcess())
  {
    task->Load();
    task->Rasterize();
    AddCompletedTask(task);
  }
}

void SvgRasterizeThread::ApplyRasterizedSVGToSampler()
{
  while(RasterizingTaskPtr task = NextCompletedTask())
  {
    task->GetSvgVisual()->ApplyRasterizedImage(task->GetVectorRenderer(), task->GetPixelData(), task->IsLoaded());
  }

  UnregisterProcessor();
}

void SvgRasterizeThread::Process()
{
  ApplyRasterizedSVGToSampler();
}

void SvgRasterizeThread::UnregisterProcessor()
{
  if(mProcessorRegistered)
  {
    if(mRasterizeTasks.empty() && mCompletedTasks.empty())
    {
      Adaptor::Get().UnregisterProcessor(*this);
      mProcessorRegistered = false;
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
