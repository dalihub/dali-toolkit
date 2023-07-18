#ifndef DALI_TOOLKIT_VECTOR_ANIMATION_THREAD_H
#define DALI_TOOLKIT_VECTOR_ANIMATION_THREAD_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/integration-api/adaptor-framework/log-factory-interface.h>
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
   * @note Ownership of the callback is NOT passed onto this class.
   * @note The callback will be excuted in the main thread.
   */
  void AddEventTriggerCallback(CallbackBase* callback);

  /**
   * @brief Remove an event trigger callback.
   *
   * @param callback The callback to remove
   */
  void RemoveEventTriggerCallback(CallbackBase* callback);

protected:
  /**
   * @brief The entry function of the animation thread.
   */
  void Run() override;

private:
  /**
   * Rasterizes the tasks.
   */
  void Rasterize();

  /**
   * @brief Called when the event callback is triggered.
   */
  void OnEventCallbackTriggered();

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

  protected:
    /**
     * @brief The entry function of the animation thread.
     */
    void Run() override;

  private:
    SleepThread(const SleepThread& thread) = delete;
    SleepThread& operator=(const SleepThread& thread) = delete;

  private:
    ConditionalWait                                    mConditionalWait;
    std::unique_ptr<CallbackBase>                      mAwakeCallback;
    std::chrono::time_point<std::chrono::steady_clock> mSleepTimePoint;
    const Dali::LogFactoryInterface&                   mLogFactory;
    bool                                               mNeedToSleep;
    bool                                               mDestroyThread;
  };

private:
  // Undefined
  VectorAnimationThread(const VectorAnimationThread& thread) = delete;

  // Undefined
  VectorAnimationThread& operator=(const VectorAnimationThread& thread) = delete;

private:
  std::vector<VectorAnimationTaskPtr>  mAnimationTasks;
  std::vector<VectorAnimationTaskPtr>  mCompletedTasks;
  std::vector<VectorAnimationTaskPtr>  mWorkingTasks;
  std::vector<CallbackBase*>           mTriggerEventCallbacks{}; // Callbacks are not owned
  SleepThread                          mSleepThread;
  ConditionalWait                      mConditionalWait;
  std::unique_ptr<EventThreadCallback> mEventTrigger{};
  bool                                 mNeedToSleep;
  bool                                 mDestroyThread;
  bool                                 mEventTriggered{false};
  const Dali::LogFactoryInterface&     mLogFactory;
  Dali::AsyncTaskManager               mAsyncTaskManager;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // #endif // DALI_TOOLKIT_VECTOR_ANIMATION_THREAD_H
