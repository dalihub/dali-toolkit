#ifndef DALI_TOOLKIT_VECTOR_ANIMATION_THREAD_H
#define DALI_TOOLKIT_VECTOR_ANIMATION_THREAD_H

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
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/common/set-wrapper.h>
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/integration-api/adaptor-framework/log-factory-interface.h>
#include <dali/integration-api/adaptor-framework/trace-factory-interface.h>
#include <dali/public-api/adaptor-framework/round-robin-container-view.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-task.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * The main animation thread for vector animations
 */
class VectorAnimationThread : public Thread
{
public:
  /**
   * @brief Constructor.
   */
  VectorAnimationThread();

  /**
   * @brief Destructor.
   */
  ~VectorAnimationThread() override;

  /**
   * @brief Add a animation task into the vector animation thread, called by main thread.
   *
   * @param[in] task The task added to the thread.
   */
  void AddTask(VectorAnimationTaskPtr task);

  /**
   * @brief Called when the rasterization is completed from the rasterize thread.
   *
   * @param[in] task The completed task
   * @param[in] success true if the task succeeded, false otherwise.
   * @param[in] keepAnimation true if the animation is running, false otherwise.
   */
  void OnTaskCompleted(VectorAnimationTaskPtr task, bool success, bool keepAnimation);

  /**
   * @brief Called when the sleep thread is awaken.
   */
  void OnAwakeFromSleep();

  /**
   * @brief Add an event trigger callback.
   *
   * @param callback The callback to add
   * @param argument The argument to pass to the callback
   * @note Ownership of the callback is NOT passed onto this class.
   * @note The callback will be excuted in the main thread.
   */
  void AddEventTriggerCallback(CallbackBase* callback, uint32_t argument);

  /**
   * @brief Remove event trigger callbacks what we added before.
   *
   * @param callback The callback to remove
   */
  void RemoveEventTriggerCallbacks(CallbackBase* callback);

  /**
   * @brief Request to event callback from rasterize thread. This is called when we want to ensure rendering next frame.
   */
  void RequestForceRenderOnce();

  /**
   * @brief Finalize the thread.
   */
  void Finalize();

protected:
  /**
   * @brief The entry function of the animation thread.
   */
  void Run() override;

private:
  /**
   * @brief Move given tasks to mAnimationTasks if required.
   * @return True if task added. False if not.
   */
  bool MoveTasksToAnimation(VectorAnimationTaskPtr task, bool useCurrentTime);

  using CompletedTasksContainer = std::vector<std::pair<VectorAnimationTaskPtr, bool>>; ///< Pair of completed task, and rasterize required.

  /**
   * @brief Move given tasks to mCompletedTasks if required.
   */
  void MoveTasksToCompleted(CompletedTasksContainer&& completedTasksQueue);

private:
  /**
   * @brief Rasterizes the tasks.
   */
  void Rasterize();

  /**
   * @brief Called when the event callback is triggered.
   */
  void OnEventCallbackTriggered();

  /**
   * @brief Gets next event callback to process.
   */
  std::pair<CallbackBase*, uint32_t> GetNextEventCallback();

  /**
   * @brief The thread to sleep until the next frame time.
   */
  class SleepThread : public Thread
  {
  public:
    /**
     * @brief Constructor.
     */
    SleepThread(CallbackBase* callback);

    /**
     * @brief Destructor.
     */
    ~SleepThread() override;

    /**
     * @brief Sleeps untile the specified time point.
     */
    void SleepUntil(std::chrono::time_point<std::chrono::steady_clock> timeToSleepUntil);

    /**
     * @brief Finalizes the sleep thread. This will make ensure that we don't touch VectorAnimationThread.
     */
    void Finalize();

  protected:
    /**
     * @brief The entry function of the animation thread.
     */
    void Run() override;

  private:
    SleepThread(const SleepThread& thread) = delete;
    SleepThread& operator=(const SleepThread& thread) = delete;

  private:
    ConditionalWait mConditionalWait;
    Mutex           mAwakeCallbackMutex; ///< Mutex to check validatoin of mAwakeCallback
    Mutex           mSleepRequestMutex;  ///< Mutex to change sleep time point.

    std::unique_ptr<CallbackBase>                      mAwakeCallback;
    std::chrono::time_point<std::chrono::steady_clock> mSleepTimePoint;
    const Dali::LogFactoryInterface&                   mLogFactory;
    const Dali::TraceFactoryInterface&                 mTraceFactory;

    bool mNeedToSleep : 1;
    bool mDestroyThread : 1;
  };

private:
  // Undefined
  VectorAnimationThread(const VectorAnimationThread& thread) = delete;

  // Undefined
  VectorAnimationThread& operator=(const VectorAnimationThread& thread) = delete;

private:
  std::vector<VectorAnimationTaskPtr> mAnimationTasks; ///< Animation processing tasks, ordered by next frame time.

  using VectorAnimationTaskSet = std::set<VectorAnimationTaskPtr>;
  VectorAnimationTaskSet mCompletedTasks; ///< Temperal storage for completed tasks. Thread warning : This should be touched only at VectorAnimationThread.
  VectorAnimationTaskSet mWorkingTasks;   ///< Tasks which are currently being processed. Thread warning : This should be touched only at VectorAnimationThread.

  std::vector<std::pair<VectorAnimationTaskPtr, bool>> mCompletedTasksQueue; ///< Queue of completed tasks from worker thread. pair of task, and rasterize required.
                                                                             ///< It will be moved at begin of Rasterize().

  std::vector<std::pair<CallbackBase*, uint32_t>> mTriggerEventCallbacks{}; // Callbacks are not owned
  SleepThread                                     mSleepThread;
  ConditionalWait                                 mConditionalWait;
  Mutex                                           mEventTriggerMutex;
  Mutex                                           mAnimationTasksMutex; ///< Mutex to change + get mAnimationTasks from event thread
  Mutex                                           mTaskCompletedMutex;  ///< Mutex to collect completed tasks to mCompletedTasksQueue from worker threads
  std::unique_ptr<EventThreadCallback>            mEventTrigger{};
  const Dali::LogFactoryInterface&                mLogFactory;
  const Dali::TraceFactoryInterface&              mTraceFactory;
  Dali::AsyncTaskManager                          mAsyncTaskManager;

  bool mNeedToSleep : 1;
  bool mDestroyThread : 1;
  bool mForceRenderOnce : 1;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // #endif // DALI_TOOLKIT_VECTOR_ANIMATION_THREAD_H
