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
#include <dali/devel-api/adaptor-framework/environment-variable.h>
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
namespace
{
constexpr auto DEFAULT_NUMBER_OF_SVG_RASTERIZE_THREADS = size_t{4u};
constexpr auto NUMBER_OF_SVG_RASTERIZE_THREADS_ENV     = "DALI_SVG_RASTERIZE_THREADS";

size_t GetNumberOfThreads(const char* environmentVariable, size_t defaultValue)
{
  auto           numberString          = EnvironmentVariable::GetEnvironmentVariable(environmentVariable);
  auto           numberOfThreads       = numberString ? std::strtoul(numberString, nullptr, 10) : 0;
  constexpr auto MAX_NUMBER_OF_THREADS = 10u;
  DALI_ASSERT_DEBUG(numberOfThreads < MAX_NUMBER_OF_THREADS);
  return (numberOfThreads > 0 && numberOfThreads < MAX_NUMBER_OF_THREADS) ? numberOfThreads : defaultValue;
}

#if defined(DEBUG_ENABLED)
Debug::Filter* gVectorImageLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VECTOR_IMAGE");
#endif

} // unnamed namespace

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

  DALI_LOG_INFO(gVectorImageLogFilter, Debug::Verbose, "Rasterize: (%d x %d) [%p]\n", mWidth, mHeight, this);

  Devel::PixelBuffer pixelBuffer = mVectorRenderer.Rasterize(mWidth, mHeight);
  if(!pixelBuffer)
  {
    DALI_LOG_ERROR("Rasterize is failed!\n");
    return;
  }

  mPixelData    = Devel::PixelBuffer::Convert(pixelBuffer);
  mHasSucceeded = true;
}

bool SvgRasterizingTask::IsReady()
{
  return mVectorRenderer.IsLoaded();
}

PixelData SvgRasterizingTask::GetPixelData() const
{
  return mPixelData;
}

SvgRasterizeThread::SvgRasterizeThread(SvgRasterizeManager& svgRasterizeManager)
: mConditionalWait(),
  mLogFactory(Dali::Adaptor::Get().GetLogFactory()),
  mSvgRasterizeManager(svgRasterizeManager),
  mDestroyThread(false),
  mIsThreadStarted(false),
  mIsThreadIdle(true)
{
}

SvgRasterizeThread::~SvgRasterizeThread()
{
  // Stop the thread
  {
    ConditionalWait::ScopedLock lock(mConditionalWait);
    mDestroyThread = true;
    mConditionalWait.Notify(lock);
  }

  Join();
}

bool SvgRasterizeThread::RequestRasterize()
{
  if(!mIsThreadStarted)
  {
    Start();
    mIsThreadStarted = true;
  }

  {
    // Lock while adding task to the queue
    ConditionalWait::ScopedLock lock(mConditionalWait);

    if(mIsThreadIdle)
    {
      mIsThreadIdle = false;

      // wake up the thread
      mConditionalWait.Notify(lock);
      return true;
    }
  }

  return false;
}

void SvgRasterizeThread::Run()
{
  SetThreadName("SvgRasterizeThread");
  mLogFactory.InstallLogFunction();

  while(!mDestroyThread)
  {
    SvgTaskPtr task = mSvgRasterizeManager.NextTaskToProcess();
    if(!task)
    {
      ConditionalWait::ScopedLock lock(mConditionalWait);
      mIsThreadIdle = true;
      mConditionalWait.Wait(lock);
    }
    else
    {
      task->Process();

      mSvgRasterizeManager.AddCompletedTask(task);
    }
  }
}

SvgRasterizeManager::SvgRasterizeManager()
: mRasterizers(GetNumberOfThreads(NUMBER_OF_SVG_RASTERIZE_THREADS_ENV, DEFAULT_NUMBER_OF_SVG_RASTERIZE_THREADS), [&]() { return RasterizeHelper(*this); }),
  mTrigger(new EventThreadCallback(MakeCallback(this, &SvgRasterizeManager::ApplyRasterizedSVGToSampler))),
  mProcessorRegistered(false)
{
}

SvgRasterizeManager::~SvgRasterizeManager()
{
  if(mProcessorRegistered)
  {
    Adaptor::Get().UnregisterProcessor(*this);
  }

  mRasterizers.Clear();
}

void SvgRasterizeManager::AddTask(SvgTaskPtr task)
{
  {
    // Lock while adding task to the queue
    Mutex::ScopedLock lock(mMutex);

    // There are other tasks waiting for the rasterization
    if(!mRasterizeTasks.empty())
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
  }

  size_t count = mRasterizers.GetElementCount();
  size_t index = 0;
  while(index++ < count)
  {
    auto rasterizerHelperIt = mRasterizers.GetNext();
    DALI_ASSERT_ALWAYS(rasterizerHelperIt != mRasterizers.End());

    if(rasterizerHelperIt->RequestRasterize())
    {
      break;
    }
    // If all threads are busy, then it's ok just to push the task because they will try to get the next job.
  }

  if(!mProcessorRegistered)
  {
    Adaptor::Get().RegisterProcessor(*this);
    mProcessorRegistered = true;
  }

  return;
}

SvgTaskPtr SvgRasterizeManager::NextCompletedTask()
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

void SvgRasterizeManager::RemoveTask(SvgVisual* visual)
{
  {
    // Lock while remove task from the queue
    Mutex::ScopedLock lock(mMutex);
    if(!mRasterizeTasks.empty())
    {
      for(std::vector<SvgTaskPtr>::iterator it = mRasterizeTasks.begin(); it != mRasterizeTasks.end();)
      {
        // We should not remove SvgLoadingTask now.
        if((*it) && (*it)->GetSvgVisual() == visual && dynamic_cast<SvgRasterizingTask*>(it->Get()))
        {
          it = mRasterizeTasks.erase(it);
        }
        else
        {
          it++;
        }
      }
    }
  }

  UnregisterProcessor();
}

SvgTaskPtr SvgRasterizeManager::NextTaskToProcess()
{
  // Lock while popping task out from the queue
  Mutex::ScopedLock lock(mMutex);

  // pop out the next task from the queue
  SvgTaskPtr nextTask = nullptr;

  for(auto iter = mRasterizeTasks.begin(), endIter = mRasterizeTasks.end(); iter != endIter; ++iter)
  {
    if((*iter)->IsReady())
    {
      nextTask = *iter;
      mRasterizeTasks.erase(iter);
      break;
    }
  }

  return nextTask;
}

void SvgRasterizeManager::AddCompletedTask(SvgTaskPtr task)
{
  // Lock while adding task to the queue
  Mutex::ScopedLock lock(mMutex);
  mCompletedTasks.push_back(task);

  // wake up the main thread
  mTrigger->Trigger();
}

void SvgRasterizeManager::ApplyRasterizedSVGToSampler()
{
  while(SvgTaskPtr task = NextCompletedTask())
  {
    DALI_LOG_INFO(gVectorImageLogFilter, Debug::Verbose, "task = %p\n", task.Get());

    task->GetSvgVisual()->ApplyRasterizedImage(task->GetPixelData(), task->HasSucceeded());
  }

  UnregisterProcessor();
}

void SvgRasterizeManager::Process(bool postProcessor)
{
  ApplyRasterizedSVGToSampler();
}

void SvgRasterizeManager::UnregisterProcessor()
{
  Mutex::ScopedLock lock(mMutex);

  if(mProcessorRegistered)
  {
    if(mRasterizeTasks.empty() && mCompletedTasks.empty())
    {
      Adaptor::Get().UnregisterProcessor(*this);
      mProcessorRegistered = false;
    }
  }
}

SvgRasterizeManager::RasterizeHelper::RasterizeHelper(SvgRasterizeManager& svgRasterizeManager)
: RasterizeHelper(std::unique_ptr<SvgRasterizeThread>(new SvgRasterizeThread(svgRasterizeManager)), svgRasterizeManager)
{
}

SvgRasterizeManager::RasterizeHelper::RasterizeHelper(RasterizeHelper&& rhs)
: RasterizeHelper(std::move(rhs.mRasterizer), rhs.mSvgRasterizeManager)
{
}

SvgRasterizeManager::RasterizeHelper::RasterizeHelper(std::unique_ptr<SvgRasterizeThread> rasterizer, SvgRasterizeManager& svgRasterizeManager)
: mRasterizer(std::move(rasterizer)),
  mSvgRasterizeManager(svgRasterizeManager)
{
}

bool SvgRasterizeManager::RasterizeHelper::RequestRasterize()
{
  return mRasterizer->RequestRasterize();
}
} // namespace Internal

} // namespace Toolkit

} // namespace Dali
