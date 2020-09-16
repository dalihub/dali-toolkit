#ifndef DALI_TOOLKIT_VECTOR_ANIMATION_THREAD_H
#define DALI_TOOLKIT_VECTOR_ANIMATION_THREAD_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <memory>
#include <dali/public-api/signals/connection-tracker.h>
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/integration-api/adaptor-framework/log-factory-interface.h>

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
   * Add a animation task into the vector animation thread, called by main thread.
   *
   * @param[in] task The task added to the thread.
   */
  void AddTask( VectorAnimationTaskPtr task );

  /**
   * @brief Called when the rasterization is completed from the rasterize thread.
   * @param task The completed task
   */
  void OnTaskCompleted( VectorAnimationTaskPtr task, bool stopped );

  /**
   * @brief Called when the sleep thread is awaken.
   */
  void OnAwakeFromSleep();

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
    RasterizeHelper( VectorAnimationThread& animationThread );

    /**
     * @brief Rasterizes the task.
     *
     * @param[in] task The task to rasterize.
     */
    void Rasterize( VectorAnimationTaskPtr task );

  public:
    RasterizeHelper( const RasterizeHelper& ) = delete;
    RasterizeHelper& operator=( const RasterizeHelper& ) = delete;

    RasterizeHelper( RasterizeHelper&& rhs );
    RasterizeHelper& operator=( RasterizeHelper&& rhs ) = delete;

  private:

    /**
     * @brief Main constructor that used by all other constructors
     */
    RasterizeHelper( std::unique_ptr< VectorRasterizeThread > rasterizer, VectorAnimationThread& animationThread );

  private:
    std::unique_ptr< VectorRasterizeThread > mRasterizer;
    VectorAnimationThread&                   mAnimationThread;
  };

  /**
   * @brief The thread to sleep until the next frame time.
   */
  class SleepThread : public Thread
  {
  public:

    /**
     * @brief Constructor.
     */
    SleepThread( CallbackBase* callback );

    /**
     * @brief Destructor.
     */
    ~SleepThread() override;

    /**
     * @brief Sleeps untile the specified time point.
     */
    void SleepUntil( std::chrono::time_point< std::chrono::system_clock > timeToSleepUntil );

  protected:

    /**
     * @brief The entry function of the animation thread.
     */
    void Run() override;

  private:

    SleepThread( const SleepThread& thread ) = delete;
    SleepThread& operator=( const SleepThread& thread ) = delete;

  private:
    ConditionalWait                  mConditionalWait;
    std::unique_ptr< CallbackBase >  mAwakeCallback;
    std::chrono::time_point< std::chrono::system_clock > mSleepTimePoint;
    const Dali::LogFactoryInterface& mLogFactory;
    bool                             mNeedToSleep;
    bool                             mDestroyThread;
  };

private:

  // Undefined
  VectorAnimationThread( const VectorAnimationThread& thread ) = delete;

  // Undefined
  VectorAnimationThread& operator=( const VectorAnimationThread& thread ) = delete;

private:

  std::vector< VectorAnimationTaskPtr >      mAnimationTasks;
  std::vector< VectorAnimationTaskPtr >      mCompletedTasks;
  std::vector< VectorAnimationTaskPtr >      mWorkingTasks;
  RoundRobinContainerView< RasterizeHelper > mRasterizers;
  SleepThread                                mSleepThread;
  ConditionalWait                            mConditionalWait;
  bool                                       mNeedToSleep;
  bool                                       mDestroyThread;
  const Dali::LogFactoryInterface&           mLogFactory;

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // #endif // DALI_TOOLKIT_VECTOR_ANIMATION_THREAD_H
