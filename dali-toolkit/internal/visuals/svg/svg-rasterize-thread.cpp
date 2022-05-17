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
SvgTask::SvgTask(SvgVisual* svgVisual, VectorImageRenderer vectorRenderer)
: mSvgVisual(svgVisual),
  mVectorRenderer(vectorRenderer),
  mHasSucceeded(false)
{
}

SvgVisual* SvgTask::GetSvgVisual() const
{
  return mSvgVisual.Get();
}

PixelData SvgTask::GetPixelData() const
{
  return PixelData();
}

bool SvgTask::HasSucceeded() const
{
  return mHasSucceeded;
}

SvgLoadingTask::SvgLoadingTask(SvgVisual* svgVisual, VectorImageRenderer vectorRenderer, const VisualUrl& url, float dpi)
: SvgTask(svgVisual, vectorRenderer),
  mUrl(url),
  mDpi(dpi)
{
}

SvgLoadingTask::~SvgLoadingTask()
{
}

void SvgLoadingTask::Process()
{
  if(mVectorRenderer.IsLoaded())
  {
    // Already loaded
    mHasSucceeded = true;
    return;
  }

  Dali::Vector<uint8_t> buffer;

  if(!mUrl.IsLocalResource())
  {
    if(!Dali::FileLoader::DownloadFileSynchronously(mUrl.GetUrl(), buffer))
    {
      DALI_LOG_ERROR("Failed to download file! [%s]\n", mUrl.GetUrl().c_str());
      return;
    }
  }
  else
  {
    if(!Dali::FileLoader::ReadFile(mUrl.GetUrl(), buffer))
    {
      DALI_LOG_ERROR("Failed to read file! [%s]\n", mUrl.GetUrl().c_str());
      return;
    }
  }

  buffer.PushBack('\0');

  if(!mVectorRenderer.Load(buffer, mDpi))
  {
    DALI_LOG_ERROR("Failed to load data! [%s]\n", mUrl.GetUrl().c_str());
    return;
  }

  mHasSucceeded = true;
}

SvgRasterizingTask::SvgRasterizingTask(SvgVisual* svgVisual, VectorImageRenderer vectorRenderer, unsigned int width, unsigned int height)
: SvgTask(svgVisual, vectorRenderer),
  mWidth(width),
  mHeight(height)
{
}

SvgRasterizingTask::~SvgRasterizingTask()
{
}

void SvgRasterizingTask::Process()
{
  if(!mVectorRenderer.IsLoaded())
  {
    DALI_LOG_ERROR("File is not loaded!\n");
    return;
  }

  Devel::PixelBuffer pixelBuffer = mVectorRenderer.Rasterize(mWidth, mHeight);
  if(!pixelBuffer)
  {
    DALI_LOG_ERROR("Rasterize is failed!\n");
    return;
  }

  mPixelData    = Devel::PixelBuffer::Convert(pixelBuffer);
  mHasSucceeded = true;
}

PixelData SvgRasterizingTask::GetPixelData() const
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
    thread->AddTask(SvgTaskPtr());
    // stop the thread
    thread->Join();
    // delete the thread
    delete thread;
    thread = NULL;
  }
}

void SvgRasterizeThread::AddTask(SvgTaskPtr task)
{
  bool wasEmpty = false;

  {
    // Lock while adding task to the queue
    ConditionalWait::ScopedLock lock(mConditionalWait);
    wasEmpty = mRasterizeTasks.empty();
    if(!wasEmpty && task)
    {
      // Remove the tasks with the same renderer.
      // Older task which waiting to rasterize and apply the svg to the same renderer is expired.
      // Rasterizing task only, loading task is not duplicated.
      for(std::vector<SvgTaskPtr>::iterator it = mRasterizeTasks.begin(), endIt = mRasterizeTasks.end(); it != endIt; ++it)
      {
        if((*it) && (*it)->GetSvgVisual() == task->GetSvgVisual())
        {
          SvgRasterizingTask* oldTask = dynamic_cast<SvgRasterizingTask*>(it->Get());
          SvgRasterizingTask* newTask = dynamic_cast<SvgRasterizingTask*>(task.Get());
          if(oldTask && newTask)
          {
            mRasterizeTasks.erase(it);
            break;
          }
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

SvgTaskPtr SvgRasterizeThread::NextCompletedTask()
{
  // Lock while popping task out from the queue
  Mutex::ScopedLock lock(mMutex);

  if(mCompletedTasks.empty())
  {
    return SvgTaskPtr();
  }

  std::vector<SvgTaskPtr>::iterator next     = mCompletedTasks.begin();
  SvgTaskPtr                        nextTask = *next;
  mCompletedTasks.erase(next);

  return nextTask;
}

void SvgRasterizeThread::RemoveTask(SvgVisual* visual)
{
  // Lock while remove task from the queue
  ConditionalWait::ScopedLock lock(mConditionalWait);
  if(!mRasterizeTasks.empty())
  {
    for(std::vector<SvgTaskPtr>::iterator it = mRasterizeTasks.begin(), endIt = mRasterizeTasks.end(); it != endIt; ++it)
    {
      if((*it) && (*it)->GetSvgVisual() == visual)
      {
        mRasterizeTasks.erase(it);
      }
    }
  }

  UnregisterProcessor();
}

SvgTaskPtr SvgRasterizeThread::NextTaskToProcess()
{
  // Lock while popping task out from the queue
  ConditionalWait::ScopedLock lock(mConditionalWait);

  // conditional wait
  while(mRasterizeTasks.empty())
  {
    mIsThreadWaiting = true;
    mConditionalWait.Wait(lock);
  }
  mIsThreadWaiting = false;

  // pop out the next task from the queue
  std::vector<SvgTaskPtr>::iterator next     = mRasterizeTasks.begin();
  SvgTaskPtr                        nextTask = *next;
  mRasterizeTasks.erase(next);

  return nextTask;
}

void SvgRasterizeThread::AddCompletedTask(SvgTaskPtr task)
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

  while(SvgTaskPtr task = NextTaskToProcess())
  {
    task->Process();
    AddCompletedTask(task);
  }
}

void SvgRasterizeThread::ApplyRasterizedSVGToSampler()
{
  while(SvgTaskPtr task = NextCompletedTask())
  {
    task->GetSvgVisual()->ApplyRasterizedImage(task->GetPixelData(), task->HasSucceeded());
  }

  UnregisterProcessor();
}

void SvgRasterizeThread::Process(bool postProcessor)
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
