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
#if defined(DEBUG_ENABLED)
Debug::Filter* gVectorAnimationLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VECTOR_ANIMATION");
#endif

} // unnamed namespace

VectorAnimationThread::VectorAnimationThread()
: mAnimationTasks(),
  mCompletedTasks(),
  mWorkingTasks(),
  mSleepThread(MakeCallback(this, &VectorAnimationThread::OnAwakeFromSleep)),
  mConditionalWait(),
  mEventTriggerMutex(),
  mNeedToSleep(false),
  mDestroyThread(false),
  mLogFactory(Dali::Adaptor::Get().GetLogFactory()),
  mTraceFactory(Dali::Adaptor::Get().GetTraceFactory())
{
  mAsyncTaskManager = Dali::AsyncTaskManager::Get();
  mSleepThread.Start();

  mEventTrigger = std::unique_ptr<EventThreadCallback>(new EventThreadCallback(MakeCallback(this, &VectorAnimationThread::OnEventCallbackTriggered)));
}

VectorAnimationThread::~VectorAnimationThread()
{
  // Stop the thread
  {
    ConditionalWait::ScopedLock lock(mConditionalWait);
    // Wait until some event thread trigger relative job finished.
    {
      Mutex::ScopedLock lock(mEventTriggerMutex);
      mDestroyThread = true;
    }
    mNeedToSleep = false;
    mConditionalWait.Notify(lock);
  }

  // Stop event trigger
  mEventTrigger.reset();

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationThread::~VectorAnimationThread: Join [%p]\n", this);

  Join();
}

void VectorAnimationThread::AddTask(VectorAnimationTaskPtr task)
{
  ConditionalWait::ScopedLock lock(mConditionalWait);

  // Find if the task is already in the list except loading task
  auto iter = std::find_if(mAnimationTasks.begin(), mAnimationTasks.end(), [task](VectorAnimationTaskPtr& element) { return (element == task && !element->IsLoadRequested()); });
  if(iter == mAnimationTasks.end())
  {
    auto currentTime = task->CalculateNextFrameTime(true); // Rasterize as soon as possible

    bool inserted = false;
    for(auto iter = mAnimationTasks.begin(); iter != mAnimationTasks.end(); ++iter)
    {
      auto nextTime = (*iter)->GetNextFrameTime();
      if(nextTime > currentTime)
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

    mNeedToSleep = false;
    // wake up the animation thread
    mConditionalWait.Notify(lock);
  }
}

void VectorAnimationThread::OnTaskCompleted(VectorAnimationTaskPtr task, bool success, bool keepAnimation)
{
  if(!mDestroyThread)
  {
    ConditionalWait::ScopedLock lock(mConditionalWait);
    bool                        needRasterize = false;

    auto workingTask = std::find(mWorkingTasks.begin(), mWorkingTasks.end(), task);
    if(workingTask != mWorkingTasks.end())
    {
      mWorkingTasks.erase(workingTask);
    }

    // Check pending task
    if(mAnimationTasks.end() != std::find(mAnimationTasks.begin(), mAnimationTasks.end(), task))
    {
      needRasterize = true;
    }

    if(keepAnimation && success)
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
      // wake up the animation thread
      mConditionalWait.Notify(lock);
    }
  }
}

void VectorAnimationThread::OnAwakeFromSleep()
{
  if(!mDestroyThread)
  {
    mNeedToSleep = false;
    // wake up the animation thread
    mConditionalWait.Notify();
  }
}

void VectorAnimationThread::AddEventTriggerCallback(CallbackBase* callback)
{
  Mutex::ScopedLock lock(mEventTriggerMutex);
  if(!mDestroyThread)
  {
    mTriggerEventCallbacks.push_back(callback);

    if(!mEventTriggered)
    {
      mEventTrigger->Trigger();
      mEventTriggered = true;
    }
  }
}

void VectorAnimationThread::RemoveEventTriggerCallbacks(CallbackBase* callback)
{
  Mutex::ScopedLock lock(mEventTriggerMutex);
  if(!mDestroyThread)
  {
    auto iter = std::remove(mTriggerEventCallbacks.begin(), mTriggerEventCallbacks.end(), callback);
    mTriggerEventCallbacks.erase(iter, mTriggerEventCallbacks.end());
  }
}

void VectorAnimationThread::Run()
{
  SetThreadName("VectorAnimationThread");
  mLogFactory.InstallLogFunction();
  mTraceFactory.InstallTraceFunction();

  while(!mDestroyThread)
  {
    Rasterize();
  }
}

void VectorAnimationThread::Rasterize()
{
  // Lock while popping task out from the queue
  ConditionalWait::ScopedLock lock(mConditionalWait);

  // conditional wait
  if(mNeedToSleep)
  {
    mConditionalWait.Wait(lock);
  }

  mNeedToSleep = true;

  // Process completed tasks
  for(auto&& task : mCompletedTasks)
  {
    if(mAnimationTasks.end() == std::find(mAnimationTasks.begin(), mAnimationTasks.end(), task))
    {
      // Should use the frame rate of the animation file
      auto nextFrameTime = task->CalculateNextFrameTime(false);

      bool inserted = false;
      for(auto iter = mAnimationTasks.begin(); iter != mAnimationTasks.end(); ++iter)
      {
        auto time = (*iter)->GetNextFrameTime();
        if(time > nextFrameTime)
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
    }
  }
  mCompletedTasks.clear();

  // pop out the next task from the queue
  for(auto it = mAnimationTasks.begin(); it != mAnimationTasks.end();)
  {
    VectorAnimationTaskPtr nextTask = *it;

    auto currentTime   = std::chrono::steady_clock::now();
    auto nextFrameTime = nextTask->GetNextFrameTime();

#if defined(DEBUG_ENABLED)
//    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nextFrameTime - currentTime);

//    DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationThread::Rasterize: [next time = %lld]\n", duration.count());
#endif
    if(nextFrameTime <= currentTime)
    {
      // If the task is not in the working list
      if(std::find(mWorkingTasks.begin(), mWorkingTasks.end(), nextTask) == mWorkingTasks.end())
      {
        it = mAnimationTasks.erase(it);

        // Add it to the working list
        mWorkingTasks.push_back(nextTask);
        mAsyncTaskManager.AddTask(nextTask);
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

void VectorAnimationThread::OnEventCallbackTriggered()
{
  while(CallbackBase* callback = GetNextEventCallback())
  {
    CallbackBase::Execute(*callback);
  }
}

CallbackBase* VectorAnimationThread::GetNextEventCallback()
{
  Mutex::ScopedLock lock(mEventTriggerMutex);
  if(!mDestroyThread)
  {
    if(!mTriggerEventCallbacks.empty())
    {
      auto          iter     = mTriggerEventCallbacks.begin();
      CallbackBase* callback = *iter;
      mTriggerEventCallbacks.erase(iter);
      return callback;
    }
    mEventTriggered = false;
  }
  return nullptr;
}

VectorAnimationThread::SleepThread::SleepThread(CallbackBase* callback)
: mConditionalWait(),
  mAwakeCallback(std::unique_ptr<CallbackBase>(callback)),
  mSleepTimePoint(),
  mLogFactory(Dali::Adaptor::Get().GetLogFactory()),
  mTraceFactory(Dali::Adaptor::Get().GetTraceFactory()),
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
    mConditionalWait.Notify(lock);
  }

  Join();
}

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
  mTraceFactory.InstallTraceFunction();

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
#if defined(DEBUG_ENABLED)
//      auto sleepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(mSleepTimePoint - std::chrono::steady_clock::now());

//      DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationThread::SleepThread::Run: [sleep duration = %lld]\n", sleepDuration.count());
#endif

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
