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
#include <toolkit-async-task-manager.h>

// EXTERNAL INCLUDE
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/public-api/adaptor-framework/async-task-manager.h>
#include <dali/public-api/adaptor-framework/round-robin-container-view.h>
#include <dali/public-api/common/list-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <memory>
#include <unordered_map>

// INTERNAL INCLUDE
#include <toolkit-application.h>
#include <toolkit-environment-variable.h>
#include <toolkit-event-thread-callback.h>
#include "dali-test-suite-utils.h"

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
namespace
{
std::atomic_uint32_t gThreadId = 0u;
}

class AsyncTaskThread;

class AsyncTaskManager : public Dali::BaseObject
{
public:
  static Dali::AsyncTaskManager Get();

  AsyncTaskManager();
  ~AsyncTaskManager();

public:
  void AddTask(AsyncTaskPtr task);
  void RemoveTask(AsyncTaskPtr task);
  void NotifyToTaskReady(AsyncTaskPtr task);

  Dali::AsyncTaskManager::TasksCompletedId SetCompletedCallback(CallbackBase* callback, Dali::AsyncTaskManager::CompletedCallbackTraceMask mask);
  bool                                     RemoveCompletedCallback(Dali::AsyncTaskManager::TasksCompletedId tasksCompletedId);

public:
  AsyncTaskPtr PopNextCompletedTask();
  void         TaskCompleted();
  void         TaskAllCompleted();

public: // Worker thread called method
  AsyncTaskPtr PopNextTaskToProcess();
  void         CompleteTask(AsyncTaskPtr&& task);

private:
  /**
   * @brief Helper class to keep the relation between AsyncTaskThread and corresponding container
   */
  class TaskHelper
  {
  public:
    /**
     * @brief Create an TaskHelper.
     *
     * @param[in] asyncTaskManager Reference to the AsyncTaskManager
     */
    TaskHelper(AsyncTaskManager& asyncTaskManager);

    /**
     * @brief Request the thread to process the task.
     * @return True if the request succeeds, otherwise false.
     */
    bool Request();

  public:
    TaskHelper(const TaskHelper&)            = delete;
    TaskHelper& operator=(const TaskHelper&) = delete;

    TaskHelper(TaskHelper&& rhs);
    TaskHelper& operator=(TaskHelper&& rhs) = delete;

  private:
    /**
     * @brief Main constructor that used by all other constructors
     */
    TaskHelper(std::unique_ptr<AsyncTaskThread> processor, AsyncTaskManager& asyncTaskManager);

  private:
    std::unique_ptr<AsyncTaskThread> mProcessor;
    AsyncTaskManager&                mAsyncTaskManager;
  };

  /**
   * @brief State of running task
   */
  enum class RunningTaskState
  {
    RUNNING  = 0, ///< Running task
    CANCELED = 1, ///< Canceled by user
  };

  /**
   * @brief State of complete task
   */
  enum class CompletedTaskState
  {
    REQUIRE_CALLBACK = 0, ///< Need to execute callback when completed task process.
    SKIP_CALLBACK    = 1, ///< Do not execute callback
  };

private:
  // Undefined
  AsyncTaskManager(const AsyncTaskManager& manager);

  // Undefined
  AsyncTaskManager& operator=(const AsyncTaskManager& manager);

private:
  // Keep Task as list since we take tasks by FIFO as default.
  using AsyncTaskContainer = std::list<AsyncTaskPtr>;

  using AsyncRunningTaskPair      = std::pair<AsyncTaskPtr, RunningTaskState>;
  using AsyncRunningTaskContainer = std::list<AsyncRunningTaskPair>;

  using AsyncCompletedTaskPair      = std::pair<AsyncTaskPtr, CompletedTaskState>;
  using AsyncCompletedTaskContainer = std::list<AsyncCompletedTaskPair>;

  AsyncTaskContainer          mWaitingTasks;   ///< The queue of the tasks waiting to async process. Must be locked under mWaitingTasksMutex.
  AsyncTaskContainer          mNotReadyTasks;  ///< The queue of the tasks waiting for ready to async process. Must be locked under mWaitingTasksMutex.
  AsyncRunningTaskContainer   mRunningTasks;   ///< The queue of the running tasks. Must be locked under mRunningTasksMutex.
  AsyncCompletedTaskContainer mCompletedTasks; ///< The queue of the tasks with the async process. Must be locked under mCompletedTasksMutex.

  RoundRobinContainerView<TaskHelper> mTasks;

  Dali::Mutex mWaitingTasksMutex;   ///< Mutex for mWaitingTasks. We can lock mRunningTasksMutex and mCompletedTasksMutex under this scope.
  Dali::Mutex mRunningTasksMutex;   ///< Mutex for mRunningTasks. We can lock mCompletedTasksMutex under this scope.
  Dali::Mutex mCompletedTasksMutex; ///< Mutex for mCompletedTasks. We cannot lock any mutex under this scope.
  Dali::Mutex mTasksMutex;          ///< Mutex for mTasks.        We cannot lock any mutex under this scope.

  std::unique_ptr<EventThreadCallback> mTrigger;

  struct TasksCompletedImpl;
  std::unique_ptr<TasksCompletedImpl> mTasksCompletedImpl; ///< TaskS completed signal interface for AsyncTaskManager.
};

inline Internal::Adaptor::AsyncTaskManager& GetImplementation(Dali::AsyncTaskManager& obj)
{
  DALI_ASSERT_ALWAYS(obj && "AsyncTaskManager is empty");

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::Adaptor::AsyncTaskManager&>(handle);
}

inline const Internal::Adaptor::AsyncTaskManager& GetImplementation(const Dali::AsyncTaskManager& obj)
{
  DALI_ASSERT_ALWAYS(obj && "AsyncTaskManager is empty");

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::Adaptor::AsyncTaskManager&>(handle);
}

/********************************************************************************/
/*********************************  INTERNAL CLASS  *****************************/
/********************************************************************************/

namespace
{
constexpr auto FORCE_TRIGGER_THRESHOLD = 128u; ///< Trigger TasksCompleted() forcely if the number of completed task contain too much.

constexpr auto DEFAULT_NUMBER_OF_ASYNC_THREADS = size_t{8u};
constexpr auto NUMBER_OF_ASYNC_THREADS_ENV     = "DALI_ASYNC_MANAGER_THREAD_POOL_SIZE";

size_t GetNumberOfThreads(const char* environmentVariable, size_t defaultValue)
{
  auto           numberString          = EnvironmentVariable::GetEnvironmentVariable(environmentVariable);
  auto           numberOfThreads       = numberString ? std::strtoul(numberString, nullptr, 10) : 0;
  constexpr auto MAX_NUMBER_OF_THREADS = 16u;
  DALI_ASSERT_DEBUG(numberOfThreads <= MAX_NUMBER_OF_THREADS);
  return (numberOfThreads > 0 && numberOfThreads <= MAX_NUMBER_OF_THREADS) ? numberOfThreads : defaultValue;
}

static Dali::AsyncTaskManager gAsyncTaskManager;
} // namespace

/**
 * The worker thread for async process
 */
class AsyncTaskThread : public Thread
{
public:
  /**
   * Constructor.
   */
  AsyncTaskThread(AsyncTaskManager& asyncTaskManager)
  : mConditionalWait(),
    mAsyncTaskManager(asyncTaskManager),
    mThreadId(++gThreadId),
    mDestroyThread(false),
    mIsThreadStarted(false),
    mIsThreadIdle(true)
  {
  }

  /**
   * Destructor.
   */
  ~AsyncTaskThread() override
  {
    // Stop the thread
    {
      ConditionalWait::ScopedLock lock(mConditionalWait);
      mDestroyThread = true;
      mConditionalWait.Notify(lock);
    }

    Join();
  }

  /**
   * @brief Request the thread to process the task.
   * @return True if the request is successed, otherwise false.
   */
  bool Request()
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

protected:
  /**
   * The entry function of the worker thread.
   */
  void Run() override
  {
    {
      char temp[100];
      snprintf(temp, 100, "AsyncTaskThread[%u]", mThreadId);
      SetThreadName(temp);
    }

    while(!mDestroyThread)
    {
      AsyncTaskPtr task = mAsyncTaskManager.PopNextTaskToProcess();
      if(!task)
      {
        ConditionalWait::ScopedLock lock(mConditionalWait);
        if(!mDestroyThread)
        {
          mIsThreadIdle = true;
          mConditionalWait.Wait(lock);
        }
      }
      else
      {
        tet_printf("BEGIN: AsyncTask[%s] Process\n", task->GetTaskName().data());
        task->Process();
        tet_printf("END: AsyncTask[%s] Process\n", task->GetTaskName().data());
        if(!mDestroyThread)
        {
          mAsyncTaskManager.CompleteTask(std::move(task));
        }
      }
    }
  }

private:
  // Undefined
  AsyncTaskThread(const AsyncTaskThread& thread) = delete;

  // Undefined
  AsyncTaskThread& operator=(const AsyncTaskThread& thread) = delete;

private:
  ConditionalWait   mConditionalWait;
  AsyncTaskManager& mAsyncTaskManager;
  uint32_t          mThreadId;
  bool              mDestroyThread;
  bool              mIsThreadStarted;
  bool              mIsThreadIdle;
};

// AsyncTaskManager::TasksCompletedImpl

struct AsyncTaskManager::TasksCompletedImpl
{
  TasksCompletedImpl(AsyncTaskManager& manager, EventThreadCallback* trigger)
  : mManager(manager),
    mTrigger(trigger),
    mEmitCompletedTaskTriggered(false)
  {
  }

public:
  /**
   * @brief Create new tasks completed id and.
   * @post AppendTaskTrace or CheckTasksCompletedCallbackCompleted should be called.
   * @param[in] callback The callback that want to be executed when we notify that all tasks completed.
   */
  Dali::AsyncTaskManager::TasksCompletedId GenerateTasksCompletedId(CallbackBase* callback)
  {
    // Lock while adding tasks completed callback list to the queue
    Mutex::ScopedLock lock(mTasksCompletedCallbacksMutex);

    auto id = mTasksCompletedCount++;
    DALI_ASSERT_ALWAYS(mTasksCompletedCallbackList.find(id) == mTasksCompletedCallbackList.end());

    mTasksCompletedCallbackList.insert({id, CallbackData(callback)});
    return id;
  }

  /**
   * @brief Append task that will be trace.
   * @post RemoveTaskTrace should be called.
   * @param[in] id The id of tasks completed.
   * @param[in] task The task want to trace.
   */
  void AppendTaskTrace(Dali::AsyncTaskManager::TasksCompletedId id, AsyncTaskPtr task)
  {
    // Lock while adding tasks completed callback list to the queue
    Mutex::ScopedLock lock(mTasksCompletedCallbacksMutex);

    auto iter = mTasksCompletedCallbackList.find(id);
    if(iter == mTasksCompletedCallbackList.end())
    {
      // This task is already erased. Ignore.
      return;
    }

    auto& callbackData = iter->second;

    auto jter = callbackData.mTasks.find(task.Get());

    if(jter != callbackData.mTasks.end())
    {
      // Increase reference count.
      ++(jter->second);
    }
    else
    {
      callbackData.mTasks.insert({task.Get(), 1u});
    }
  }

  /**
   * @brief Remove all task that were traced.
   * @param[in] task The task want to remove trace.
   * @param[in] taskCount The number of tasks that will be removed.
   */
  void RemoveTaskTrace(AsyncTaskPtr task, uint32_t count = 1u)
  {
    if(count == 0u)
    {
      return;
    }

    // Lock while removing tasks completed callback list to the queue
    Mutex::ScopedLock lock(mTasksCompletedCallbacksMutex);

    for(auto iter = mTasksCompletedCallbackList.begin(); iter != mTasksCompletedCallbackList.end();)
    {
      auto& callbackData      = iter->second;
      bool  eraseCallbackData = false;

      auto jter = callbackData.mTasks.find(task.Get());

      if(jter != callbackData.mTasks.end())
      {
        if(jter->second <= count)
        {
          callbackData.mTasks.erase(jter);

          if(callbackData.mTasks.empty())
          {
            eraseCallbackData = true;

            // Move callback base into list.
            // (To avoid task container changed during callback emit)
            RegisterTasksCompletedCallback(std::move(callbackData.mCallback), iter->first);

            iter = mTasksCompletedCallbackList.erase(iter);
          }
        }
        else
        {
          jter->second -= count;
        }
      }

      if(!eraseCallbackData)
      {
        ++iter;
      }
    }
  }

  /**
   * @brief Check whether current TasksCompletedId completed or not.
   * @param[in] id The id of tasks completed.
   * @return True if all tasks are completed so we need to execute callback soon. False otherwise.
   */
  bool CheckTasksCompletedCallbackCompleted(Dali::AsyncTaskManager::TasksCompletedId id)
  {
    // Lock while removing tasks completed callback list to the queue
    Mutex::ScopedLock lock(mTasksCompletedCallbacksMutex);

    auto iter = mTasksCompletedCallbackList.find(id);
    if(iter != mTasksCompletedCallbackList.end())
    {
      auto& callbackData = iter->second;
      if(callbackData.mTasks.empty())
      {
        // Move callback base into list.
        // (To avoid task container changed during callback emit)
        RegisterTasksCompletedCallback(std::move(callbackData.mCallback), iter->first);

        iter = mTasksCompletedCallbackList.erase(iter);

        return true;
      }
    }

    return false;
  }

  /**
   * @brief Remove taskS completed callbacks by id.
   * @param[in] id The id of taskS completed.
   * @return True if taskS completed id removed. False otherwise.
   */
  bool RemoveTasksCompleted(Dali::AsyncTaskManager::TasksCompletedId id)
  {
    // Lock while removing taskS completed callback list to the queue
    Mutex::ScopedLock lock(mTasksCompletedCallbacksMutex);

    auto iter = mTasksCompletedCallbackList.find(id);
    if(iter == mTasksCompletedCallbackList.end())
    {
      // This task is already erased, or completed.
      // Erase from completed excute callback list.

      // Lock while removing excute callback list to the queue
      Mutex::ScopedLock lock(mExcuteCallbacksMutex);

      for(auto iter = mExcuteCallbackList.begin(); iter != mExcuteCallbackList.end();)
      {
        if(iter->second == id)
        {
          iter = mExcuteCallbackList.erase(iter);

          return true;
        }
        else
        {
          ++iter;
        }
      }

      // This task is alread erased and completed. Ignore.
      return false;
    }

    mTasksCompletedCallbackList.erase(iter);

    return true;
  }

  /**
   * @brief Emit all completed callbacks.
   * @note This API should be called at event thread.
   */
  void EmitCompletedTasks()
  {
    ExecuteCallbackContainer executeCallbackList;
    {
      // Lock while removing excute callback list to the queue
      Mutex::ScopedLock lock(mExcuteCallbacksMutex);

      mEmitCompletedTaskTriggered = false;

      // Copy callback lists, for let we execute callbacks out of mutex
      executeCallbackList = std::move(mExcuteCallbackList);
      mExcuteCallbackList.clear();
    }

    if(!executeCallbackList.empty())
    {
      // Execute all callbacks
      for(auto&& callbackPair : executeCallbackList)
      {
        auto& callback = callbackPair.first;
        auto  id       = callbackPair.second;

        Dali::CallbackBase::Execute(*callback, id);
      }
    }
  }

  /**
   * @brief Check whether there is some completed signal what we need to trace, or not.
   * @return True if mTasksCompletedCallbackList is not empty. False otherwise.
   */
  bool IsTasksCompletedCallbackExist()
  {
    Mutex::ScopedLock lock(mTasksCompletedCallbacksMutex);
    return !mTasksCompletedCallbackList.empty();
  }

  /**
   * @brief Check whether there is some completed signal what we need to execute, or not.
   * @return True if mExcuteCallbackList is not empty. False otherwise.
   */
  bool IsExecuteCallbackExist()
  {
    Mutex::ScopedLock lock(mExcuteCallbacksMutex);
    return !mExcuteCallbackList.empty();
  }

private:
  void RegisterTasksCompletedCallback(std::unique_ptr<CallbackBase> callback, Dali::AsyncTaskManager::TasksCompletedId id)
  {
    // Lock while adding excute callback list to the queue
    Mutex::ScopedLock lock(mExcuteCallbacksMutex);

    mExcuteCallbackList.emplace_back(std::move(callback), id);

    if(!mEmitCompletedTaskTriggered)
    {
      mEmitCompletedTaskTriggered = true;
      mTrigger->Trigger();
    }
  }

private:
  struct CallbackData
  {
  public:
    CallbackData(CallbackBase* callback)
    : mCallback(callback),
      mTasks()
    {
    }

    CallbackData(CallbackData&& rhs) noexcept
    : mCallback(std::move(rhs.mCallback)),
      mTasks(std::move(rhs.mTasks))
    {
    }

    CallbackData& operator=(CallbackData&& rhs) noexcept
    {
      if(this != &rhs)
      {
        mCallback = std::move(rhs.mCallback);
        mTasks    = std::move(rhs.mTasks);
      }

      return *this;
    }

  private:
    // Delete copy operator.
    CallbackData(const CallbackData& rhs)            = delete;
    CallbackData& operator=(const CallbackData& rhs) = delete;

  public:
    std::unique_ptr<CallbackBase>                  mCallback;
    std::unordered_map<const AsyncTask*, uint32_t> mTasks;
  };

private:
  AsyncTaskManager&    mManager; ///< Owner of this CacheImpl.
  EventThreadCallback* mTrigger; ///< EventThread callback trigger. (Not owned.)

  Dali::AsyncTaskManager::TasksCompletedId mTasksCompletedCount{0u};

  using TasksCompletedContainer = std::unordered_map<Dali::AsyncTaskManager::TasksCompletedId, CallbackData>;
  TasksCompletedContainer mTasksCompletedCallbackList;

  using ExecuteCallbackContainer = std::vector<std::pair<std::unique_ptr<CallbackBase>, Dali::AsyncTaskManager::TasksCompletedId>>;
  ExecuteCallbackContainer mExcuteCallbackList;

  Dali::Mutex mTasksCompletedCallbacksMutex; ///< Mutex for mTasksCompletedCallbackList. We can lock mExcuteCallbacksMutex under this scope.
  Dali::Mutex mExcuteCallbacksMutex;         ///< Mutex for mExcuteCallbackList.

  bool mEmitCompletedTaskTriggered : 1;
};

// AsyncTaskManager

Dali::AsyncTaskManager AsyncTaskManager::Get()
{
  if(!gAsyncTaskManager)
  {
    gAsyncTaskManager = Dali::AsyncTaskManager(new AsyncTaskManager());
  }
  return gAsyncTaskManager;
}

AsyncTaskManager::AsyncTaskManager()
: mTasks(GetNumberOfThreads(NUMBER_OF_ASYNC_THREADS_ENV, DEFAULT_NUMBER_OF_ASYNC_THREADS), [&]()
         { return TaskHelper(*this); }),
  mTrigger(new EventThreadCallback(MakeCallback(this, &AsyncTaskManager::TaskCompleted))),
  mTasksCompletedImpl(new TasksCompletedImpl(*this, mTrigger.get()))
{
}

AsyncTaskManager::~AsyncTaskManager()
{
  // Join all threads.
  mTasks.Clear();

  // Remove task completed impl after all threads are join.
  mTasksCompletedImpl.reset();

  // Remove tasks
  mWaitingTasks.clear();
  mRunningTasks.clear();
  mCompletedTasks.clear();
}

/// Main + Worker thread called
void AsyncTaskManager::AddTask(AsyncTaskPtr task)
{
  if(task)
  {
    // Lock while adding task to the queue
    Mutex::ScopedLock lock(mWaitingTasksMutex);

    // Keep this value as stack memory, for thread safety
    const bool isReady = task->IsReady();

    if(DALI_LIKELY(isReady))
    {
      // push back into waiting queue.
      mWaitingTasks.insert(mWaitingTasks.end(), task);

      {
        // For thread safety
        Mutex::ScopedLock lock(mRunningTasksMutex); // We can lock this mutex under mWaitingTasksMutex.

        // Finish all Running threads are working
        if(mRunningTasks.size() >= mTasks.GetElementCount())
        {
          return;
        }
      }
    }
    else
    {
      // push back into waiting queue.
      mNotReadyTasks.insert(mNotReadyTasks.end(), task);
      return;
    }
  }

  {
    Mutex::ScopedLock lock(mTasksMutex);
    size_t            count = mTasks.GetElementCount();
    size_t            index = 0;
    while(index++ < count)
    {
      auto processHelperIt = mTasks.GetNext();
      DALI_ASSERT_ALWAYS(processHelperIt != mTasks.End());
      if(processHelperIt->Request())
      {
        break;
      }
      // If all threads are busy, then it's ok just to push the task because they will try to get the next job.
    }
  }

  // Note : We will not use Processor for toolkit utc
}

void AsyncTaskManager::RemoveTask(AsyncTaskPtr task)
{
  if(task)
  {
    uint32_t removedCount = 0u;

    {
      // Lock while remove task from the queue
      Mutex::ScopedLock lock(mWaitingTasksMutex);

      for(auto iterator = mWaitingTasks.begin(); iterator != mWaitingTasks.end();)
      {
        if((*iterator) == task)
        {
          iterator = mWaitingTasks.erase(iterator);
          ++removedCount;
        }
        else
        {
          ++iterator;
        }
      }

      for(auto iterator = mNotReadyTasks.begin(); iterator != mNotReadyTasks.end();)
      {
        if((*iterator) == task)
        {
          iterator = mNotReadyTasks.erase(iterator);
          ++removedCount;
        }
        else
        {
          ++iterator;
        }
      }
    }

    {
      // Lock while remove task from the queue
      Mutex::ScopedLock lock(mRunningTasksMutex);

      for(auto iterator = mRunningTasks.begin(); iterator != mRunningTasks.end();)
      {
        if((*iterator).first == task)
        {
          // We cannot erase container. Just mark as canceled.
          // Note : mAvaliableLowPriorityTaskCounts will be increased after process finished.
          if((*iterator).second == RunningTaskState::RUNNING)
          {
            (*iterator).second = RunningTaskState::CANCELED;
            ++removedCount;
          }
        }
        ++iterator;
      }
    }

    {
      // Lock while remove task from the queue
      Mutex::ScopedLock lock(mCompletedTasksMutex);

      for(auto iterator = mCompletedTasks.begin(); iterator != mCompletedTasks.end();)
      {
        if((*iterator).first == task)
        {
          if((*iterator).second == CompletedTaskState::REQUIRE_CALLBACK)
          {
            ++removedCount;
          }
          iterator = mCompletedTasks.erase(iterator);
        }
        else
        {
          ++iterator;
        }
      }
    }

    // Remove TasksCompleted callback trace
    if(removedCount > 0u && mTasksCompletedImpl->IsTasksCompletedCallbackExist())
    {
      mTasksCompletedImpl->RemoveTaskTrace(task, removedCount);
    }
  }
}

/// Main + Worker thread called
void AsyncTaskManager::NotifyToTaskReady(AsyncTaskPtr task)
{
  if(task)
  {
    // Lock while adding task to the queue
    Mutex::ScopedLock lock(mWaitingTasksMutex);

    uint32_t removedCount = 0u;
    for(auto iterator = mNotReadyTasks.begin(); iterator != mNotReadyTasks.end();)
    {
      if((*iterator) == task)
      {
        iterator = mNotReadyTasks.erase(iterator);
        ++removedCount;
      }
      else
      {
        ++iterator;
      }
    }
    // push back into waiting queue.
    while(removedCount > 0u)
    {
      --removedCount;
      mWaitingTasks.insert(mWaitingTasks.end(), task);
    }
  }

  {
    Mutex::ScopedLock lock(mTasksMutex);
    size_t            count = mTasks.GetElementCount();
    size_t            index = 0;
    while(index++ < count)
    {
      auto processHelperIt = mTasks.GetNext();
      DALI_ASSERT_ALWAYS(processHelperIt != mTasks.End());
      if(processHelperIt->Request())
      {
        break;
      }
      // If all threads are busy, then it's ok just to push the task because they will try to get the next job.
    }
  }
}

Dali::AsyncTaskManager::TasksCompletedId AsyncTaskManager::SetCompletedCallback(CallbackBase* callback, Dali::AsyncTaskManager::CompletedCallbackTraceMask mask)
{
  // mTasksCompletedImpl will take ownership of callback.
  Dali::AsyncTaskManager::TasksCompletedId tasksCompletedId = mTasksCompletedImpl->GenerateTasksCompletedId(callback);

  bool taskAdded = false; ///< Flag whether at least one task tracing now.

  // Please be careful the order of mutex, to avoid dead lock.
  {
    Mutex::ScopedLock lockWait(mWaitingTasksMutex);
    {
      Mutex::ScopedLock lockRunning(mRunningTasksMutex); // We can lock this mutex under mWaitingTasksMutex.
      {
        Mutex::ScopedLock lockComplete(mCompletedTasksMutex); // We can lock this mutex under mWaitingTasksMutex and mRunningTasksMutex.

        // Collect all tasks from waiting tasks
        for(auto& task : mWaitingTasks)
        {
          auto checkMask = (task->GetCallbackInvocationThread() == Dali::AsyncTask::ThreadType::MAIN_THREAD ? Dali::AsyncTaskManager::CompletedCallbackTraceMask::THREAD_MASK_MAIN : Dali::AsyncTaskManager::CompletedCallbackTraceMask::THREAD_MASK_WORKER) |
                           (task->GetPriorityType() == Dali::AsyncTask::PriorityType::HIGH ? Dali::AsyncTaskManager::CompletedCallbackTraceMask::PRIORITY_MASK_HIGH : Dali::AsyncTaskManager::CompletedCallbackTraceMask::PRIORITY_MASK_LOW);

          if((checkMask & mask) == checkMask)
          {
            taskAdded = true;
            mTasksCompletedImpl->AppendTaskTrace(tasksCompletedId, task);
          }
        }

        // Collect all tasks from not ready waiting tasks
        for(auto& task : mNotReadyTasks)
        {
          auto checkMask = (task->GetCallbackInvocationThread() == Dali::AsyncTask::ThreadType::MAIN_THREAD ? Dali::AsyncTaskManager::CompletedCallbackTraceMask::THREAD_MASK_MAIN : Dali::AsyncTaskManager::CompletedCallbackTraceMask::THREAD_MASK_WORKER) |
                           (task->GetPriorityType() == Dali::AsyncTask::PriorityType::HIGH ? Dali::AsyncTaskManager::CompletedCallbackTraceMask::PRIORITY_MASK_HIGH : Dali::AsyncTaskManager::CompletedCallbackTraceMask::PRIORITY_MASK_LOW);

          if((checkMask & mask) == checkMask)
          {
            taskAdded = true;
            mTasksCompletedImpl->AppendTaskTrace(tasksCompletedId, task);
          }
        }

        // Collect all tasks from running tasks
        for(auto& taskPair : mRunningTasks)
        {
          // Trace only if it is running now.
          if(taskPair.second == RunningTaskState::RUNNING)
          {
            auto& task      = taskPair.first;
            auto  checkMask = (task->GetCallbackInvocationThread() == Dali::AsyncTask::ThreadType::MAIN_THREAD ? Dali::AsyncTaskManager::CompletedCallbackTraceMask::THREAD_MASK_MAIN : Dali::AsyncTaskManager::CompletedCallbackTraceMask::THREAD_MASK_WORKER) |
                             (task->GetPriorityType() == Dali::AsyncTask::PriorityType::HIGH ? Dali::AsyncTaskManager::CompletedCallbackTraceMask::PRIORITY_MASK_HIGH : Dali::AsyncTaskManager::CompletedCallbackTraceMask::PRIORITY_MASK_LOW);

            if((checkMask & mask) == checkMask)
            {
              taskAdded = true;
              mTasksCompletedImpl->AppendTaskTrace(tasksCompletedId, task);
            }
          }
        }

        // Collect all tasks from complete tasks
        for(auto& taskPair : mCompletedTasks)
        {
          // Trace only if it is need callback.
          // Note : There are two CompletedTaskState::SKIP_CALLBACK cases, worker thread invocation and canceled cases.
          //        If worker thread invocation, than it already remove trace at completed timing.
          //        If canceled cases, we don't append trace at running tasks already.
          //        So, we don't need to trace for SKIP_CALLBACK cases.
          if(taskPair.second == CompletedTaskState::REQUIRE_CALLBACK)
          {
            auto& task      = taskPair.first;
            auto  checkMask = (task->GetCallbackInvocationThread() == Dali::AsyncTask::ThreadType::MAIN_THREAD ? Dali::AsyncTaskManager::CompletedCallbackTraceMask::THREAD_MASK_MAIN : Dali::AsyncTaskManager::CompletedCallbackTraceMask::THREAD_MASK_WORKER) |
                             (task->GetPriorityType() == Dali::AsyncTask::PriorityType::HIGH ? Dali::AsyncTaskManager::CompletedCallbackTraceMask::PRIORITY_MASK_HIGH : Dali::AsyncTaskManager::CompletedCallbackTraceMask::PRIORITY_MASK_LOW);

            if((checkMask & mask) == checkMask)
            {
              taskAdded = true;
              mTasksCompletedImpl->AppendTaskTrace(tasksCompletedId, task);
            }
          }
        }
      }
    }
  }

  // If there is nothing to check task, just excute callback right now.
  if(!taskAdded)
  {
    mTasksCompletedImpl->CheckTasksCompletedCallbackCompleted(tasksCompletedId);
  }
  return tasksCompletedId;
}

bool AsyncTaskManager::RemoveCompletedCallback(Dali::AsyncTaskManager::TasksCompletedId tasksCompletedId)
{
  return mTasksCompletedImpl->RemoveTasksCompleted(tasksCompletedId);
}

AsyncTaskPtr AsyncTaskManager::PopNextCompletedTask()
{
  std::vector<AsyncTaskPtr> ignoredTaskList; ///< To keep asyncTask reference so we can ensure that destructor called out of mutex.

  AsyncTaskPtr nextCompletedTask = nullptr;
  {
    // Lock while popping task out from the queue
    Mutex::ScopedLock lock(mCompletedTasksMutex);

    while(!mCompletedTasks.empty())
    {
      auto               next      = mCompletedTasks.begin();
      AsyncTaskPtr       nextTask  = next->first;
      CompletedTaskState taskState = next->second;
      mCompletedTasks.erase(next);

      if(taskState == CompletedTaskState::REQUIRE_CALLBACK)
      {
        nextCompletedTask = nextTask;
        break;
      }

      ignoredTaskList.push_back(nextTask);
    }
  }

  return nextCompletedTask;
}

void AsyncTaskManager::TaskCompleted()
{
  // For UTC, let we complete only 1 task here.
  if(AsyncTaskPtr task = PopNextCompletedTask())
  {
    CallbackBase::Execute(*(task->GetCompletedCallback()), task);

    // Remove TasksCompleted callback trace
    if(mTasksCompletedImpl->IsTasksCompletedCallbackExist())
    {
      mTasksCompletedImpl->RemoveTaskTrace(task);
    }
  }

  mTasksCompletedImpl->EmitCompletedTasks();
}

void AsyncTaskManager::TaskAllCompleted()
{
  while(AsyncTaskPtr task = PopNextCompletedTask())
  {
    CallbackBase::Execute(*(task->GetCompletedCallback()), task);

    // Remove TasksCompleted callback trace
    if(mTasksCompletedImpl->IsTasksCompletedCallbackExist())
    {
      mTasksCompletedImpl->RemoveTaskTrace(task);
    }
  }

  mTasksCompletedImpl->EmitCompletedTasks();
}

/// Worker thread called
AsyncTaskPtr AsyncTaskManager::PopNextTaskToProcess()
{
  // Lock while popping task out from the queue
  Mutex::ScopedLock lock(mWaitingTasksMutex);

  // pop out the next task from the queue
  AsyncTaskPtr nextTask = nullptr;

  for(auto iter = mWaitingTasks.begin(), endIter = mWaitingTasks.end(); iter != endIter; ++iter)
  {
    if((*iter)->IsReady())
    {
      bool taskAvaliable = true;

      // Check whether we try to running same task at multiple threads.
      {
        Mutex::ScopedLock lock(mRunningTasksMutex); // We can lock this mutex under mWaitingTasksMutex.
        auto              task = *iter;
        auto              jter = std::find_if(mRunningTasks.begin(), mRunningTasks.end(), [task](const AsyncRunningTaskPair& element)
                                 { return element.first == task; });
        if(jter != mRunningTasks.end())
        {
          taskAvaliable = false;
        }
      }

      if(taskAvaliable)
      {
        nextTask = *iter;

        // Add Running queue
        {
          // Lock while popping task out from the queue
          Mutex::ScopedLock lock(mRunningTasksMutex); // We can lock this mutex under mWaitingTasksMutex.

          mRunningTasks.insert(mRunningTasks.end(), std::make_pair(nextTask, RunningTaskState::RUNNING));

          mWaitingTasks.erase(iter);
        }
      }
      break;
    }
    else
    {
      DALI_LOG_ERROR("Not ready task is in wating queue! Something wrong!\n");
    }
  }

  return nextTask;
}

/// Worker thread called
void AsyncTaskManager::CompleteTask(AsyncTaskPtr&& task)
{
  if(task)
  {
    bool needTrigger = false;

    // Check now whether we need to execute callback or not, for worker thread cases.
    if(task->GetCallbackInvocationThread() == AsyncTask::ThreadType::WORKER_THREAD)
    {
      bool notify = false;

      // Lock while check validation of task.
      {
        Mutex::ScopedLock lock(mRunningTasksMutex);

        auto iter = std::find_if(mRunningTasks.begin(), mRunningTasks.end(), [task](const AsyncRunningTaskPair& element)
                                 { return element.first == task; });
        if(iter != mRunningTasks.end())
        {
          if(iter->second == RunningTaskState::RUNNING)
          {
            // This task is valid.
            notify = true;
          }
        }
      }

      // We should execute this tasks complete callback out of mutex
      if(notify)
      {
        CallbackBase::Execute(*(task->GetCompletedCallback()), task);

        // We need to remove task trace now.
        if(mTasksCompletedImpl->IsTasksCompletedCallbackExist())
        {
          mTasksCompletedImpl->RemoveTaskTrace(task);

          if(mTasksCompletedImpl->IsExecuteCallbackExist())
          {
            // We need to call EmitCompletedTasks(). Trigger main thread.
            needTrigger = true;
          }
        }
      }
    }

    // Lock while adding task to the queue
    {
      bool notify = false;

      Mutex::ScopedLock lock(mRunningTasksMutex);

      auto iter = std::find_if(mRunningTasks.begin(), mRunningTasks.end(), [task](const AsyncRunningTaskPair& element)
                               { return element.first == task; });
      if(iter != mRunningTasks.end())
      {
        if(iter->second == RunningTaskState::RUNNING)
        {
          // This task is valid.
          notify = true;
        }

        // Move task into completed, for ensure that AsyncTask destroy at main thread.
        {
          Mutex::ScopedLock lock(mCompletedTasksMutex); // We can lock this mutex under mRunningTasksMutex.

          const bool callbackRequired = notify && (task->GetCallbackInvocationThread() == AsyncTask::ThreadType::MAIN_THREAD);

          needTrigger |= callbackRequired;

          mCompletedTasks.insert(mCompletedTasks.end(), std::make_pair(task, callbackRequired ? CompletedTaskState::REQUIRE_CALLBACK : CompletedTaskState::SKIP_CALLBACK));

          mRunningTasks.erase(iter);

          if(!needTrigger)
          {
            needTrigger |= (mCompletedTasks.size() >= FORCE_TRIGGER_THRESHOLD);
          }

          // Now, task is invalidate.
          task.Reset();
        }
      }
    }

    // Wake up the main thread
    if(needTrigger)
    {
      mTrigger->Trigger();
    }
  }
}

// AsyncTaskManager::TaskHelper

AsyncTaskManager::TaskHelper::TaskHelper(AsyncTaskManager& asyncTaskManager)
: TaskHelper(std::unique_ptr<AsyncTaskThread>(new AsyncTaskThread(asyncTaskManager)), asyncTaskManager)
{
}

AsyncTaskManager::TaskHelper::TaskHelper(TaskHelper&& rhs)
: TaskHelper(std::move(rhs.mProcessor), rhs.mAsyncTaskManager)
{
}

AsyncTaskManager::TaskHelper::TaskHelper(std::unique_ptr<AsyncTaskThread> processor, AsyncTaskManager& asyncTaskManager)
: mProcessor(std::move(processor)),
  mAsyncTaskManager(asyncTaskManager)
{
}

bool AsyncTaskManager::TaskHelper::Request()
{
  return mProcessor->Request();
}

} // namespace Adaptor

} // namespace Internal

/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

void AsyncTask::NotifyToReady()
{
  tet_printf("NotifyToReady[%s]\n", GetTaskName().data());
  Dali::Internal::Adaptor::gAsyncTaskManager.NotifyToTaskReady(AsyncTaskPtr(this));
}

AsyncTaskManager::AsyncTaskManager() = default;

AsyncTaskManager::~AsyncTaskManager() = default;

AsyncTaskManager AsyncTaskManager::Get()
{
  return Internal::Adaptor::AsyncTaskManager::Get();
}

void AsyncTaskManager::AddTask(AsyncTaskPtr task)
{
  Internal::Adaptor::GetImplementation(*this).AddTask(task);
}

void AsyncTaskManager::RemoveTask(AsyncTaskPtr task)
{
  Internal::Adaptor::GetImplementation(*this).RemoveTask(task);
}

void AsyncTaskManager::NotifyToTaskReady(AsyncTaskPtr task)
{
  Internal::Adaptor::GetImplementation(*this).NotifyToTaskReady(task);
}

AsyncTaskManager::TasksCompletedId AsyncTaskManager::SetCompletedCallback(CallbackBase* callback, AsyncTaskManager::CompletedCallbackTraceMask mask)
{
  return Internal::Adaptor::GetImplementation(*this).SetCompletedCallback(callback, mask);
}

bool AsyncTaskManager::RemoveCompletedCallback(AsyncTaskManager::TasksCompletedId tasksCompletedId)
{
  return Internal::Adaptor::GetImplementation(*this).RemoveCompletedCallback(tasksCompletedId);
}

AsyncTaskManager::AsyncTaskManager(Internal::Adaptor::AsyncTaskManager* impl)
: BaseHandle(impl)
{
}

} // namespace Dali

namespace Test
{
namespace AsyncTaskManager
{
void DestroyAsyncTaskManager()
{
  Dali::Internal::Adaptor::gAsyncTaskManager.Reset();
}

void ProcessSingleCompletedTasks()
{
  auto asyncTaskManager = Dali::AsyncTaskManager::Get();
  Dali::Internal::Adaptor::GetImplementation(asyncTaskManager).TaskCompleted();
}

void ProcessAllCompletedTasks()
{
  auto asyncTaskManager = Dali::AsyncTaskManager::Get();
  Dali::Internal::Adaptor::GetImplementation(asyncTaskManager).TaskAllCompleted();
}
} // namespace AsyncTaskManager
} // namespace Test