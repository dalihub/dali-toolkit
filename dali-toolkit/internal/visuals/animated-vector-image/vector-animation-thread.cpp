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
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-thread.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/environment-variable.h>
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <thread>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
constexpr auto DEFAULT_NUMBER_OF_RASTERIZE_THREADS = size_t{4u};
constexpr auto NUMBER_OF_RASTERIZE_THREADS_ENV     = "DALI_VECTOR_RASTERIZE_THREADS";

size_t GetNumberOfThreads(const char* environmentVariable, size_t defaultValue)
{
  using Dali::EnvironmentVariable::GetEnvironmentVariable;
  auto           numberString          = GetEnvironmentVariable(environmentVariable);
  auto           numberOfThreads       = numberString ? std::strtoul(numberString, nullptr, 10) : 0;
  constexpr auto MAX_NUMBER_OF_THREADS = 100u;
  DALI_ASSERT_DEBUG(numberOfThreads < MAX_NUMBER_OF_THREADS);
  return (numberOfThreads > 0 && numberOfThreads < MAX_NUMBER_OF_THREADS) ? numberOfThreads : defaultValue;
}

#if defined(DEBUG_ENABLED)
Debug::Filter* gVectorAnimationLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VECTOR_ANIMATION");
#endif

} // unnamed namespace

VectorAnimationThread::VectorAnimationThread()
: mAnimationTasks(),
  mCompletedTasks(),
  mWorkingTasks(),
  mRasterizers(GetNumberOfThreads(NUMBER_OF_RASTERIZE_THREADS_ENV, DEFAULT_NUMBER_OF_RASTERIZE_THREADS), [&]() { return RasterizeHelper(*this); }),
  mSleepThread(MakeCallback(this, &VectorAnimationThread::OnAwakeFromSleep)),
  mConditionalWait(),
  mEventTriggerMutex(),
  mAnimationTasksMutex(),
  mTaskCompletedMutex(),
  mLogFactory(Dali::Adaptor::Get().GetLogFactory()),
  mNeedToSleep(false),
  mDestroyThread(false),
  mEventTriggered(false),
  mForceRenderOnce(false)
{
  mSleepThread.Start();

  mEventTrigger = std::unique_ptr<EventThreadCallback>(new EventThreadCallback(MakeCallback(this, &VectorAnimationThread::OnEventCallbackTriggered)));
}

/// Event thread called
VectorAnimationThread::~VectorAnimationThread()
{
  // Stop the thread
  {
    ConditionalWait::ScopedLock lock(mConditionalWait);
    // Wait until some event thread trigger relative job finished.
    {
      Mutex::ScopedLock eventTriggerLock(mEventTriggerMutex);
      Mutex::ScopedLock taskCompletedLock(mTaskCompletedMutex);
      Mutex::ScopedLock animationTasksLock(mAnimationTasksMutex);
      DALI_LOG_DEBUG_INFO("Mark VectorAnimationThread destroyed\n");
      mDestroyThread = true;
    }
    mNeedToSleep = false;
    mConditionalWait.Notify(lock);
  }

  // Stop event trigger
  mEventTrigger.reset();

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationThread::~VectorAnimationThread: Join [%p]\n", this);

  DALI_LOG_DEBUG_INFO("VectorAnimationThread Join request\n");

  Join();
}

/// Event thread called
void VectorAnimationThread::AddTask(VectorAnimationTaskPtr task)
{
  ConditionalWait::ScopedLock lock(mConditionalWait);

  // Rasterize as soon as possible
  if(MoveTasksToAnimation(task, true))
  {
    mNeedToSleep = false;
    // wake up the animation thread
    mConditionalWait.Notify(lock);
  }
}

/// Worker thread called
void VectorAnimationThread::OnTaskCompleted(VectorAnimationTaskPtr task, bool success, bool keepAnimation)
{
  ConditionalWait::ScopedLock lock(mConditionalWait);

  Mutex::ScopedLock taskCompletedLock(mTaskCompletedMutex);

  if(DALI_LIKELY(!mDestroyThread))
  {
    mCompletedTasksQueue.emplace_back(task, success && keepAnimation);

    // wake up the animation thread.
    // Note that we should not make mNeedToSleep as false now.
    mConditionalWait.Notify(lock);
  }
}

/// VectorAnimationThread::SleepThread called
void VectorAnimationThread::OnAwakeFromSleep()
{
  if(DALI_LIKELY(!mDestroyThread))
  {
    mNeedToSleep = false;
    // wake up the animation thread
    mConditionalWait.Notify();
  }
}

/// Worker thread called
void VectorAnimationThread::AddEventTriggerCallback(CallbackBase* callback, uint32_t argument)
{
  Mutex::ScopedLock lock(mEventTriggerMutex);
  if(DALI_LIKELY(!mDestroyThread))
  {
    mTriggerEventCallbacks.emplace_back(callback, argument);

    if(!mEventTriggered)
    {
      mEventTrigger->Trigger();
      mEventTriggered = true;
    }
  }
}

/// Event thread called
void VectorAnimationThread::RemoveEventTriggerCallbacks(CallbackBase* callback)
{
  Mutex::ScopedLock lock(mEventTriggerMutex);
  if(DALI_LIKELY(!mDestroyThread))
  {
    auto iter = std::remove_if(mTriggerEventCallbacks.begin(), mTriggerEventCallbacks.end(), [&callback](std::pair<CallbackBase*, uint32_t>& item) { return item.first == callback; });
    mTriggerEventCallbacks.erase(iter, mTriggerEventCallbacks.end());
  }
}

/// Worker thread called
void VectorAnimationThread::RequestForceRenderOnce()
{
  Mutex::ScopedLock lock(mEventTriggerMutex);
  if(DALI_LIKELY(!mDestroyThread))
  {
    mForceRenderOnce = true;

    if(!mEventTriggered)
    {
      mEventTrigger->Trigger();
      mEventTriggered = true;
    }
  }
}

/// VectorAnimationThread called
void VectorAnimationThread::Run()
{
  SetThreadName("VectorAnimationThread");
  mLogFactory.InstallLogFunction();

  while(!mDestroyThread)
  {
    Rasterize();
  }
}

/// Event & VectorAnimationThread called
bool VectorAnimationThread::MoveTasksToAnimation(VectorAnimationTaskPtr task, bool useCurrentTime)
{
  Mutex::ScopedLock animationTasksLock(mAnimationTasksMutex);

  if(DALI_LIKELY(!mDestroyThread))
  {
    // Find if the task is already in the list except loading task
    auto iter = std::find_if(mAnimationTasks.begin(), mAnimationTasks.end(), [task, useCurrentTime](VectorAnimationTaskPtr& element) {
      return (element == task) &&
             (!useCurrentTime ||            // If we don't need to use current time (i.e. CompletedTasks)
              !element->IsLoadRequested()); // Or we need to use current time And loading completed.
    });

    if(iter == mAnimationTasks.end())
    {
      // Use the frame rate of the animation file, or use current time.
      auto nextFrameTime = task->CalculateNextFrameTime(useCurrentTime);

      bool inserted = false;
      for(auto iter = mAnimationTasks.begin(); iter != mAnimationTasks.end(); ++iter)
      {
        auto nextTime = (*iter)->GetNextFrameTime();
        if(nextTime > nextFrameTime)
        {
          mAnimationTasks.insert(iter, task);
          inserted = true;
          break;
        }
      }

      if(!inserted)
      {
        mAnimationTasks.push_back(task);
      }

      return true;
    }
  }
  return false;
}

/// VectorAnimationThread called
void VectorAnimationThread::MoveTasksToCompleted(VectorAnimationTaskPtr task, bool keepAnimation)
{
  if(DALI_LIKELY(!mDestroyThread))
  {
    bool needRasterize = false;

    auto workingTask = std::find(mWorkingTasks.begin(), mWorkingTasks.end(), task);
    if(workingTask != mWorkingTasks.end())
    {
      mWorkingTasks.erase(workingTask);
    }

    // Check pending task
    {
      Mutex::ScopedLock animationTasksLock(mAnimationTasksMutex);
      if(mAnimationTasks.end() != std::find(mAnimationTasks.begin(), mAnimationTasks.end(), task))
      {
        needRasterize = true;
      }
    }

    if(keepAnimation)
    {
      if(mCompletedTasks.end() == std::find(mCompletedTasks.begin(), mCompletedTasks.end(), task))
      {
        mCompletedTasks.push_back(task);
        needRasterize = true;
      }
    }

    if(needRasterize)
    {
      mNeedToSleep = false;
    }
  }
}

/// VectorAnimationThread called
void VectorAnimationThread::Rasterize()
{
  // Lock while popping task out from the queue
  ConditionalWait::ScopedLock lock(mConditionalWait);

  // conditional wait
  while(DALI_LIKELY(!mDestroyThread) && mNeedToSleep)
  {
    // ConditionalWait notifyed when sleep done, or task complete.
    // If task complete, then we need to re-validate the tasks container, and then sleep again.
    decltype(mCompletedTasksQueue) completedTasksQueue;
    {
      Mutex::ScopedLock taskCompletedLock(mTaskCompletedMutex);
      completedTasksQueue.swap(mCompletedTasksQueue);
    }
    if(completedTasksQueue.empty())
    {
      mConditionalWait.Wait(lock);
      // Task completed may have been added to the queue while we were waiting.
      {
        Mutex::ScopedLock taskCompletedLock(mTaskCompletedMutex);
        completedTasksQueue.swap(mCompletedTasksQueue);
      }
    }

    for(auto&& taskPair : completedTasksQueue)
    {
      auto& task          = taskPair.first;
      bool  keepAnimation = taskPair.second;
      MoveTasksToCompleted(task, keepAnimation);
    }
  }

  mNeedToSleep = true;

  // Process completed tasks
  for(auto&& task : mCompletedTasks)
  {
    // Should use the frame rate of the animation file
    MoveTasksToAnimation(task, false);
  }
  mCompletedTasks.clear();

  {
    Mutex::ScopedLock animationTasksLock(mAnimationTasksMutex);
    if(DALI_LIKELY(!mDestroyThread))
    {
      if(mAnimationTasks.size() > 0u)
      {
        // pop out the next task from the queue
        for(auto it = mAnimationTasks.begin(); it != mAnimationTasks.end();)
        {
          VectorAnimationTaskPtr nextTask = *it;

          auto currentTime   = std::chrono::steady_clock::now();
          auto nextFrameTime = nextTask->GetNextFrameTime();

          if(nextFrameTime <= currentTime)
          {
            // If the task is not in the working list
            if(std::find(mWorkingTasks.begin(), mWorkingTasks.end(), nextTask) == mWorkingTasks.end())
            {
              it = mAnimationTasks.erase(it);

              // Add it to the working list
              mWorkingTasks.push_back(nextTask);

              auto rasterizerHelperIt = mRasterizers.GetNext();
              DALI_ASSERT_ALWAYS(rasterizerHelperIt != mRasterizers.End());
              rasterizerHelperIt->Rasterize(nextTask);
            }
            else
            {
              it++;
            }
          }
          else
          {
            mSleepThread.SleepUntil(nextFrameTime);
            break;
          }
        }
      }
    }
  }
}

/// Event thread called (Due to mTrigger triggered)
void VectorAnimationThread::OnEventCallbackTriggered()
{
  while(true)
  {
    auto callbackPair = GetNextEventCallback();
    if(callbackPair.first == nullptr)
    {
      break;
    }
    CallbackBase::Execute(*callbackPair.first, callbackPair.second);
  }
  // Request update once if we need.
  {
    Mutex::ScopedLock lock(mEventTriggerMutex);
    if(!mDestroyThread && mForceRenderOnce)
    {
      mForceRenderOnce = false;
      if(Dali::Adaptor::IsAvailable())
      {
        Dali::Adaptor::Get().UpdateOnce();
      }
    }
  }
}

/// Event thread called
std::pair<CallbackBase*, uint32_t> VectorAnimationThread::GetNextEventCallback()
{
  Mutex::ScopedLock lock(mEventTriggerMutex);
  if(!mDestroyThread)
  {
    if(!mTriggerEventCallbacks.empty())
    {
      auto iter           = mTriggerEventCallbacks.begin();
      auto callbackIdPair = *iter;
      mTriggerEventCallbacks.erase(iter);
      return callbackIdPair;
    }
    mEventTriggered = false;
  }
  return std::make_pair(nullptr, 0u);
}

VectorAnimationThread::RasterizeHelper::RasterizeHelper(VectorAnimationThread& animationThread)
: RasterizeHelper(std::unique_ptr<VectorRasterizeThread>(new VectorRasterizeThread()), animationThread)
{
}

VectorAnimationThread::RasterizeHelper::RasterizeHelper(RasterizeHelper&& rhs)
: RasterizeHelper(std::move(rhs.mRasterizer), rhs.mAnimationThread)
{
}

VectorAnimationThread::RasterizeHelper::RasterizeHelper(std::unique_ptr<VectorRasterizeThread> rasterizer, VectorAnimationThread& animationThread)
: mRasterizer(std::move(rasterizer)),
  mAnimationThread(animationThread)
{
  mRasterizer->SetCompletedCallback(MakeCallback(&mAnimationThread, &VectorAnimationThread::OnTaskCompleted));
}

void VectorAnimationThread::RasterizeHelper::Rasterize(VectorAnimationTaskPtr task)
{
  if(task)
  {
    mRasterizer->AddTask(task);
  }
}

VectorAnimationThread::SleepThread::SleepThread(CallbackBase* callback)
: mConditionalWait(),
  mAwakeCallback(std::unique_ptr<CallbackBase>(callback)),
  mSleepTimePoint(),
  mLogFactory(Dali::Adaptor::Get().GetLogFactory()),
  mNeedToSleep(false),
  mDestroyThread(false)
{
}

VectorAnimationThread::SleepThread::~SleepThread()
{
  // Stop the thread
  {
    ConditionalWait::ScopedLock lock(mConditionalWait);
    mDestroyThread = true;
    mAwakeCallback.reset();
    mConditionalWait.Notify(lock);
  }

  Join();
}

/// VectorAnimationThread called
void VectorAnimationThread::SleepThread::SleepUntil(std::chrono::time_point<std::chrono::steady_clock> timeToSleepUntil)
{
  ConditionalWait::ScopedLock lock(mConditionalWait);
  mSleepTimePoint = timeToSleepUntil;
  mNeedToSleep    = true;
  mConditionalWait.Notify(lock);
}

void VectorAnimationThread::SleepThread::Run()
{
  SetThreadName("VectorSleepThread");
  mLogFactory.InstallLogFunction();

  while(!mDestroyThread)
  {
    bool                                               needToSleep;
    std::chrono::time_point<std::chrono::steady_clock> sleepTimePoint;

    {
      ConditionalWait::ScopedLock lock(mConditionalWait);

      needToSleep    = mNeedToSleep;
      sleepTimePoint = mSleepTimePoint;

      mNeedToSleep = false;
    }

    if(needToSleep)
    {
      std::this_thread::sleep_until(sleepTimePoint);

      if(mAwakeCallback)
      {
        CallbackBase::Execute(*mAwakeCallback);
      }
    }

    {
      ConditionalWait::ScopedLock lock(mConditionalWait);
      if(!mDestroyThread && !mNeedToSleep)
      {
        mConditionalWait.Wait(lock);
      }
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
