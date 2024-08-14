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
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/integration-api/adaptor-framework/log-factory-interface.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/helpers/round-robin-container-view.h>
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-task.h>
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-rasterize-thread.h>

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

  /**
   * @brief Move given tasks to mCompletedTasks if required.
   */
  void MoveTasksToCompleted(VectorAnimationTaskPtr task, bool keepAnimation);

private:
  /**
   * @brief Rasterizes the tasks.
   */
  void Rasterize();

private:
  /**
   * @brief Helper class to keep the relation between VectorRasterizeThread and corresponding container
   */
  class RasterizeHelper : public ConnectionTracker
  {
  public:
    /**
     * @brief Create an RasterizeHelper.
     *
     * @param[in] animationThread Reference to the VectorAnimationThread
     */
    RasterizeHelper(VectorAnimationThread& animationThread);

    /**
     * @brief Rasterizes the task.
     *
     * @param[in] task The task to rasterize.
     */
    void Rasterize(VectorAnimationTaskPtr task);

  public:
    RasterizeHelper(const RasterizeHelper&) = delete;
    RasterizeHelper& operator=(const RasterizeHelper&) = delete;

    RasterizeHelper(RasterizeHelper&& rhs);
    RasterizeHelper& operator=(RasterizeHelper&& rhs) = delete;

  private:
    /**
     * @brief Main constructor that used by all other constructors
     */
    RasterizeHelper(std::unique_ptr<VectorRasterizeThread> rasterizer, VectorAnimationThread& animationThread);

  private:
    std::unique_ptr<VectorRasterizeThread> mRasterizer;
    VectorAnimationThread&                 mAnimationThread;
  };

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
    bool                                               mNeedToSleep : 1;
    bool                                               mDestroyThread : 1;
  };

private:
  // Undefined
  VectorAnimationThread(const VectorAnimationThread& thread) = delete;

  // Undefined
  VectorAnimationThread& operator=(const VectorAnimationThread& thread) = delete;

private:
  std::vector<VectorAnimationTaskPtr> mAnimationTasks; ///< Animation processing tasks, ordered by next frame time.
  std::vector<VectorAnimationTaskPtr> mCompletedTasks; ///< Temperal storage for completed tasks.
  std::vector<VectorAnimationTaskPtr> mWorkingTasks;

  std::vector<std::pair<VectorAnimationTaskPtr, bool>> mCompletedTasksQueue; ///< Queue of completed tasks from worker thread. pair of task, and rasterize required.
                                                                             ///< It will be moved at begin of Rasterize().

  RoundRobinContainerView<RasterizeHelper>        mRasterizers;
  std::vector<std::pair<CallbackBase*, uint32_t>> mTriggerEventCallbacks{}; // Callbacks are not owned
  SleepThread                                     mSleepThread;
  ConditionalWait                                 mConditionalWait;
  Mutex                                           mEventTriggerMutex;
  Mutex                                           mAnimationTasksMutex; ///< Mutex to change + get mAnimationTasks from event thread
  Mutex                                           mTaskCompletedMutex;  ///< Mutex to collect completed tasks to mCompletedTasksQueue from worker threads
  std::unique_ptr<EventThreadCallback>            mEventTrigger{};
  const Dali::LogFactoryInterface&                mLogFactory;

  bool mNeedToSleep : 1;
  bool mDestroyThread : 1;
  bool mEventTriggered : 1;
  bool mForceRenderOnce : 1;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // #endif // DALI_TOOLKIT_VECTOR_ANIMATION_THREAD_H
