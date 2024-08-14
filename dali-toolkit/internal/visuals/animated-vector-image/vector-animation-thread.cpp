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
#include <dali/integration-api/trace.h>
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

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_IMAGE_PERFORMANCE_MARKER, false);

} // unnamed namespace

VectorAnimationThread::VectorAnimationThread()
: mAnimationTasks(),
  mCompletedTasks(),
  mWorkingTasks(),
  mSleepThread(MakeCallback(this, &VectorAnimationThread::OnAwakeFromSleep)),
  mConditionalWait(),
  mEventTriggerMutex(),
  mAnimationTasksMutex(),
  mTaskCompletedMutex(),
  mLogFactory(Dali::Adaptor::Get().GetLogFactory()),
  mTraceFactory(Dali::Adaptor::Get().GetTraceFactory()),
  mNeedToSleep(false),
  mDestroyThread(false),
  mEventTriggered(false),
  mForceRenderOnce(false)
{
  mAsyncTaskManager = Dali::AsyncTaskManager::Get();
  mSleepThread.Start();

  mEventTrigger = std::unique_ptr<EventThreadCallback>(new EventThreadCallback(MakeCallback(this, &VectorAnimationThread::OnEventCallbackTriggered)));
}

/// Event thread called
VectorAnimationThread::~VectorAnimationThread()
{
  DALI_TRACE_SCOPE(gTraceFilter, "VECTOR_ANIMATION_THREAD_DESTROY");

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

  // Mark as sleep thread destroyed now.
  mSleepThread.Finalize();

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

/// VectorAnimationThread::SleepThread called, Mutex SleepThread::mAwakeCallbackMutex is locked
void VectorAnimationThread::OnAwakeFromSleep()
{
  if(DALI_LIKELY(!mDestroyThread))
  {
    ConditionalWait::ScopedLock lock(mConditionalWait);

    mNeedToSleep = false;
    // wake up the animation thread
    mConditionalWait.Notify(lock);
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
  mTraceFactory.InstallTraceFunction();

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
    DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "VECTOR_ANIMATION_THREAD_ANIMATION_TASK", [&](std::ostringstream& oss) {
      oss << "[" << mAnimationTasks.size() << "," << useCurrentTime << "]";
    });

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

      DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "VECTOR_ANIMATION_THREAD_ANIMATION_TASK", [&](std::ostringstream& oss) {
        oss << "[" << mAnimationTasks.size() << "]";
      });

      return true;
    }
    DALI_TRACE_END(gTraceFilter, "VECTOR_ANIMATION_THREAD_ANIMATION_TASK");
  }
  return false;
}

/// VectorAnimationThread called
void VectorAnimationThread::MoveTasksToCompleted(VectorAnimationTaskPtr task, bool keepAnimation)
{
  if(DALI_LIKELY(!mDestroyThread))
  {
    DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "VECTOR_ANIMATION_THREAD_COMPLETED_TASK", [&](std::ostringstream& oss) {
      oss << "[w:" << mWorkingTasks.size() << ",c:" << mCompletedTasks.size() << "]";
    });
    bool needRasterize = false;

    VectorAnimationTaskSet::const_iterator workingIter = mWorkingTasks.find(task);
    if(workingIter != mWorkingTasks.cend())
    {
      mWorkingTasks.erase(workingIter);
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
      VectorAnimationTaskSet::const_iterator completedIter = mCompletedTasks.lower_bound(task);
      if(completedIter == mCompletedTasks.cend() || task < *completedIter)
      {
        mCompletedTasks.insert(completedIter, task);
        needRasterize = true;
      }
    }

    if(needRasterize)
    {
      mNeedToSleep = false;
    }

    DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "VECTOR_ANIMATION_THREAD_COMPLETED_TASK", [&](std::ostringstream& oss) {
      oss << "[w:" << mWorkingTasks.size() << ",c:" << mCompletedTasks.size() << ",r?" << needRasterize << ",s?" << mNeedToSleep << "]";
    });
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
        DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "VECTOR_ANIMATION_THREAD_ANIMATION_TASK2", [&](std::ostringstream& oss) {
          oss << "[" << mAnimationTasks.size() << "]";
        });

        // pop out the next task from the queue
        for(auto it = mAnimationTasks.begin(); it != mAnimationTasks.end();)
        {
          VectorAnimationTaskPtr nextTask = *it;

          auto currentTime   = std::chrono::steady_clock::now();
          auto nextFrameTime = nextTask->GetNextFrameTime();

          if(nextFrameTime <= currentTime)
          {
            // If the task is not in the working list
            VectorAnimationTaskSet::const_iterator workingIter = mWorkingTasks.lower_bound(nextTask);
            if(workingIter == mWorkingTasks.cend() || nextTask < *workingIter)
            {
              it = mAnimationTasks.erase(it);

              // Add it to the working list
              mWorkingTasks.insert(workingIter, nextTask);
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
        DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "VECTOR_ANIMATION_THREAD_ANIMATION_TASK2", [&](std::ostringstream& oss) {
          oss << "[" << mAnimationTasks.size() << "]";
        });
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
  DALI_TRACE_SCOPE(gTraceFilter, "VECTOR_ANIMATION_SLEEP_THREAD_DESTROY");

  // Stop the thread
  {
    ConditionalWait::ScopedLock lock(mConditionalWait);
    Finalize();

    mConditionalWait.Notify(lock);
  }

  DALI_LOG_DEBUG_INFO("VectorAnimationThread::SleepThread Join request\n");

  Join();
}

/// VectorAnimationThread called
void VectorAnimationThread::SleepThread::SleepUntil(std::chrono::time_point<std::chrono::steady_clock> timeToSleepUntil)
{
  ConditionalWait::ScopedLock lock(mConditionalWait);

  Mutex::ScopedLock sleepLock(mSleepRequestMutex);

  if(DALI_LIKELY(!mDestroyThread))
  {
    mSleepTimePoint = timeToSleepUntil;
    mNeedToSleep    = true;
    mConditionalWait.Notify(lock);
  }
}

void VectorAnimationThread::SleepThread::Finalize()
{
  Mutex::ScopedLock awakeLock(mAwakeCallbackMutex);
  Mutex::ScopedLock sleepLock(mSleepRequestMutex);
  if(DALI_LIKELY(!mDestroyThread))
  {
    DALI_LOG_DEBUG_INFO("Mark VectorAnimationThread::SleepThread destroyed\n");
    mDestroyThread = true;
  }
  mAwakeCallback.reset();
}

void VectorAnimationThread::SleepThread::Run()
{
  SetThreadName("VectorSleepThread");
  mLogFactory.InstallLogFunction();
  mTraceFactory.InstallTraceFunction();

  while(!mDestroyThread)
  {
    bool needToSleep = false;

    std::chrono::time_point<std::chrono::steady_clock> sleepTimePoint;

    {
      ConditionalWait::ScopedLock lock(mConditionalWait);
      Mutex::ScopedLock           sleepLock(mSleepRequestMutex);

      if(DALI_LIKELY(!mDestroyThread))
      {
        needToSleep    = mNeedToSleep;
        sleepTimePoint = mSleepTimePoint;

        mNeedToSleep = false;
      }
    }

    if(needToSleep)
    {
      DALI_TRACE_SCOPE(gTraceFilter, "VECTOR_ANIMATION_SLEEP_THREAD");

      std::this_thread::sleep_until(sleepTimePoint);

      {
        Mutex::ScopedLock awakeLock(mAwakeCallbackMutex);
        if(DALI_LIKELY(mAwakeCallback))
        {
          CallbackBase::Execute(*mAwakeCallback);
        }
      }
    }

    {
      ConditionalWait::ScopedLock lock(mConditionalWait);
      if(DALI_LIKELY(!mDestroyThread) && !mNeedToSleep)
      {
        DALI_TRACE_SCOPE(gTraceFilter, "VECTOR_ANIMATION_SLEEP_THREAD_WAIT");
        mConditionalWait.Wait(lock);
      }
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
